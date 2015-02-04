
/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains MQX-specific MCC library functions
*
*
*END************************************************************************/

#include "mcc_config.h"
#include "mcc_common.h"
#include "mcc_api.h"
#include "mcc_mqx.h"
#include "mcc_vf600.h"

/* Global variables */
CORE_MUTEX_PTR cm_ptr;
LWEVENT_STRUCT lwevent_buffer_queued[MCC_MQX_LWEVENT_COMPONENTS_COUNT];
LWEVENT_STRUCT lwevent_buffer_freed;
static unsigned int cpu_to_cpu_isr_vector = MCC_VECTOR_NUMBER_INVALID; 

/*!
 * \brief This function is the CPU-to-CPU interrupt handler.
 *
 * Each core can interrupt the other. There are two logical signals:
 * \n - Receive data available for (Node,Port) - signaled when a buffer is queued to a Receive Data Queue.
 * \n - Buffer available - signaled when a buffer is queued to the Free Buffer Queue.
 * \n It is possible that several signals can occur while one interrupt is being processed.
 *  Therefore, a Receive Signal Queue of received signals is also required - one for each core.
 *  The interrupting core queues to the tail and the interrupted core pulls from the head.
 *  For a circular file, no semaphore is required since only the sender modifies the tail and only the receiver modifies the head.
 *
 * \param[in] param Pointer to data being passed to the ISR.
 */
static void mcc_cpu_to_cpu_isr(void *param)
{
    MCC_SIGNAL serviced_signal;

    /* Try to lock the core mutex. If successfully locked, perform mcc_dequeue_signal(), release the gate and 
       finally clear the interrupt flag. If trylock fails (HW semaphore already locked by another core), 
       do not clear the interrupt flag – this way the CPU-to-CPU isr is re-issued again until 
       the HW semaphore is locked. Higher priority ISRs will be serviced while issued at the time 
       we are waiting for the unlocked gate. To prevent trylog failure due to core mutex currently locked by our own core
       (a task), the cpu-to-cpu isr is temporarily disabled when mcc_get_semaphore() is called and re-enabled again
       when mcc_release_semaphore() is issued. */
    if(COREMUTEX_LOCKED == _core_mutex_trylock(cm_ptr)) {
        while(MCC_SUCCESS == mcc_dequeue_signal(MCC_CORE_NUMBER, &serviced_signal)) {
            if((serviced_signal.type == BUFFER_QUEUED) &&
               (serviced_signal.destination.core == MCC_CORE_NUMBER)) {
                /* Unblock receiver, in case of asynchronous communication */
                _lwevent_set(&lwevent_buffer_queued[serviced_signal.destination.port / MCC_MQX_LWEVENT_GROUP_SIZE], 1<<(serviced_signal.destination.port % MCC_MQX_LWEVENT_GROUP_SIZE));
            }
            else if(serviced_signal.type == BUFFER_FREED) {
                /* Unblock sender, in case of asynchronous communication */
                _lwevent_set(&lwevent_buffer_freed, 1);
            }
        }
        
        /* Unlocks the core mutex */ 
        _core_mutex_unlock(cm_ptr);
        
        /* Clear the interrupt flag */
        mcc_clear_cpu_to_cpu_interrupt(MCC_CORE_NUMBER);
    }
}

/*!
 * \brief This function initializes the hw semaphore (SEMA4).
 *
 * Calls MQX core-mutex driver to create a core mutex.
 *
 * \param[in] sem_num SEMA4 gate number.
 */
int mcc_init_semaphore(unsigned int sem_num)
{
    /* Create a core mutex */
    cm_ptr = _core_mutex_create(0, sem_num, MQX_TASK_QUEUE_FIFO);

    if(NULL == cm_ptr)
        return MCC_ERR_SEMAPHORE;
    else
        return MCC_SUCCESS;
}

/*!
 * \brief This function de-initializes the hw semaphore (SEMA4).
 *
 * Calls MQX core-mutex driver to destroy a core mutex.
 *
 * \param[in] sem_num SEMA4 gate number.
 */
int mcc_deinit_semaphore(unsigned int sem_num)
{
    /* Destroy the core mutex */
    if(COREMUTEX_OK == _core_mutex_destroy(cm_ptr))
        return MCC_SUCCESS;
    else
        return MCC_ERR_SEMAPHORE;
}

/*!
 * \brief This function locks the specified core mutex.
 *
 * Calls MQX core-mutex driver to lock the core mutex.
 *
 */
int mcc_get_semaphore(void)
{
    _bsp_int_disable(cpu_to_cpu_isr_vector);
    if(COREMUTEX_OK == _core_mutex_lock(cm_ptr)) {
        return MCC_SUCCESS;
    }
    else {
        _bsp_int_enable(cpu_to_cpu_isr_vector);
        return MCC_ERR_SEMAPHORE;
    }
}

/*!
 * \brief This function unlocks the specified core mutex.
 *
 * Calls MQX core-mutex driver to unlock the core mutex.
 *
 */
int mcc_release_semaphore(void)
{
    if(COREMUTEX_OK == _core_mutex_unlock(cm_ptr)) {
        /* Enable the cpu-to-cpu isr just in case _core_mutex_unlock function has not woke up another task waiting for the core mutex. */
        if (*cm_ptr->GATE_PTR != (MCC_CORE_NUMBER + 1))
            _bsp_int_enable(cpu_to_cpu_isr_vector);
        return MCC_SUCCESS;
    }
    else
        return MCC_ERR_SEMAPHORE;
}

/*!
 * \brief This function registers the CPU-to-CPU interrupt.
 *
 * Calls MQX interrupt component functions to install and enable the CPU-to-CPU interrupt.
 *
 */
int mcc_register_cpu_to_cpu_isr(void)
{
    unsigned int vector_number;

    vector_number = mcc_get_cpu_to_cpu_vector(MCC_CORE_NUMBER);

    if(vector_number != MCC_VECTOR_NUMBER_INVALID) {
        _int_install_isr((_mqx_uint)vector_number, mcc_cpu_to_cpu_isr, NULL);
        mcc_clear_cpu_to_cpu_interrupt(MCC_CORE_NUMBER);
        /* Priority of the cpu-to-cpu isr must be lower than the core mutex isr priority */
        _bsp_int_init(vector_number, BSPCFG_CORE_MUTEX_PRIORITY + 1, 0, TRUE);
        _bsp_int_enable(vector_number);
        cpu_to_cpu_isr_vector = vector_number;
        return MCC_SUCCESS;
    }
    else {
        return MCC_ERR_INT;
    }
}

/*!
 * \brief This function triggers an interrupt to other core(s).
 *
 */
int mcc_generate_cpu_to_cpu_interrupt(void)
{
    /* Assert directed CPU interrupts for all processors except the requesting core */
    mcc_triger_cpu_to_cpu_interrupt();

    return MCC_SUCCESS;
}

/*!
 * \brief This function copies data.
 *
 * Copies the number of single-addressable units from the source address to destination address.
 *
 * \param[in] src Source address.
 * \param[in] dest Destination address.
 * \param[in] size Number of single-addressable units to copy.
 */
void mcc_memcpy(void *src, void *dest, unsigned int size)
{
    _mem_copy((void *)src, (void *)dest, (_mem_size)size);
}

