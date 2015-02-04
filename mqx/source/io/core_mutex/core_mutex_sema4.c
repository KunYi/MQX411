/*HEADER**********************************************************************
*
* Copyright 2008-2011 Freescale Semiconductor, Inc.
* Copyright 2004-2011 Embedded Access Inc.
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
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the source functions for functions required to
*   control the SEMA4 module.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include <core_mutex.h>
#include <core_mutex_sema4.h>

#define _sema4_int_disable() _int_disable()
#define _sema4_int_enable()  _int_enable()

#if PSP_MQX_CPU_IS_VYBRID
static const int idx[16] = {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12};
#else
static const int idx[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
#endif

/*!
 * \brief SEMA4 interrupt service routine.
 * 
 * Interrupt notification after a failed lock write provides a mechanism to
 * indicate when the gate is unlocked.
 * 
 * \param[in] p   Pointer to core mutex structure.
 *
 * \return void
 * 
 * \see _core_mutex_install   
 * \see _core_mutex_lock 
 */ 
static void _sema4_isr(void *p)
{
    CORE_MUTEX_DEVICE_PTR   device_ptr = (CORE_MUTEX_DEVICE_PTR) p;
    uint32_t                i;
    volatile unsigned char *gate_ptr;
    unsigned char           lock = _psp_core_num()+1;
    uint16_t                mask = 0x8000;
    bool                    waiting;

    for (i=0;i<SEMA4_NUM_GATES;i++) {
        if (*device_ptr->CPNTF_PTR & mask ) {
            /* An interrupt is pending on this mutex, the only way to clear it is to lock it (either
               by this core or another) */
            gate_ptr = device_ptr->MUTEX_PTR[idx[i]]->GATE_PTR;
            *gate_ptr = lock;
            if (*gate_ptr == lock) {
                /* Now, check to see if any task is waiting for it */
                waiting = FALSE;
                if (device_ptr->MUTEX_PTR[idx[i]]) {
                    if (_taskq_get_value(device_ptr->MUTEX_PTR[idx[i]]->WAIT_Q)) {
                        _taskq_resume(device_ptr->MUTEX_PTR[idx[i]]->WAIT_Q, FALSE);
                        waiting = TRUE;
                    }
                }
                if (!waiting) {
                    /* No task was waiting, give it back - this can occur due to a failed trylock */
                    *gate_ptr = SEMA4_UNLOCK;
                }
            }
        }
        mask >>= 1;
   }
}

/*!
 * \brief Core mutex installation function.
 * 
 * The function performs initial installation of the device. This is done only 
 * once on each core, typically upon system initialization in the BSP.
 * 
 * \param[in] init_ptr   Pointer to core mutex initialization structure.
 *
 * \return MQX_COMPONENT_EXISTS (Core mutex component already initialized.)
 * \return MQX_OUT_OF_MEMORY (Not enough free memory.)
 * \return MQX_INVALID_DEVICE (Invalid device number provided.)
 * \return COREMUTEX_OK (Success.)
 * 
 */ 
uint32_t _core_mutex_install( const CORE_MUTEX_INIT_STRUCT *init_ptr )
{
   CORE_MUTEX_COMPONENT_PTR   component_ptr = _core_mutext_get_component_ptr();
   PSP_INTERRUPT_TABLE_INDEX  vector;
   uint32_t                   i;

#if MQX_CHECK_ERRORS
   if (component_ptr!=NULL) {
      return MQX_COMPONENT_EXISTS;
   }
#endif

   component_ptr = _mem_alloc_system_zero(sizeof(*component_ptr));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (component_ptr==NULL) {
       return MQX_OUT_OF_MEMORY;
   }
#endif

   for (i=0;i<SEMA4_NUM_DEVICES;i++) {
       vector = _bsp_get_sema4_vector(i);
       component_ptr->DEVICE[i].SEMA4_PTR = _bsp_get_sema4_base_address(i);
       if (_psp_core_num()==0) {
           component_ptr->DEVICE[i].CPNTF_PTR = &(component_ptr->DEVICE[i].SEMA4_PTR->CP0NTF);
       } else {
           component_ptr->DEVICE[i].CPNTF_PTR = &(component_ptr->DEVICE[i].SEMA4_PTR->CP1NTF);
       }
#if MQX_CHECK_ERRORS
       if ((component_ptr->DEVICE[i].SEMA4_PTR == NULL) || (vector==0)) {
           _mem_free(component_ptr);
           return MQX_INVALID_DEVICE;
       }
#endif
   }

   _int_disable();
#if MQX_CHECK_ERRORS
   if (_core_mutext_get_component_ptr()) {
       _int_enable();
       _mem_free(component_ptr);
       return MQX_COMPONENT_EXISTS;
   }
#endif

   _core_mutext_set_component_ptr(component_ptr);
   _int_enable();


   for (i=0;i<SEMA4_NUM_DEVICES;i++) {
       vector = _bsp_get_sema4_vector(i);
       _int_install_isr(vector, _sema4_isr, &component_ptr->DEVICE[i]);
       _bsp_int_init(vector, init_ptr->INT_PRIORITY, 0, TRUE);
   }

   return COREMUTEX_OK;
}

/*!
 * \brief Core mutex create internal function.
 * 
 * This function allocates core_mutex structure internally and returns a handle 
 * to it. The mutex to be created is identified by SEMA4 device number and mutex 
 * (gate) number. The handle is used to reference the created mutex in calls to 
 * other core_mutex API functions. This function is to be called only once for 
 * each mutex. The policy parameter determines behavior of task queue 
 * associated with the mutex.
 * 
 * \param[in] mutex_ptr   Pointer to core_mutex structure.
 * \param[in] dev_num     SEMA4 device (module) number.
 * \param[in] mutex_num   Mutex (gate) number.
 * \param[in] policy      Queuing policy, one of the following:
 * \li MQX_TASK_QUEUE_BY_PRIORITY
 * \li MQX_TASK_QUEUE_FIFO
 * \param[in] allocated   TRUE = allocated dynamically by the driver, 
 *                        FALSE = allocated by the user.
 *
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Core mutex component not installed.)
 * \return MQX_INVALID_PARAMETER (Wrong input parameter.)
 * \return MQX_TASKQ_CREATE_FAILED (Failed to create a task queue.)
 * \return MQX_COMPONENT_EXISTS (This core mutex already initialized.)
 * \return COREMUTEX_OK (Success.)
 * 
 * \see _core_mutex_create   
 * \see _core_mutex_create_at 
 * \see _core_mutex_destroy 
 */ 
uint32_t _core_mutex_create_internal(CORE_MUTEX_PTR mutex_ptr, uint32_t dev_num, uint32_t mutex_num, uint32_t policy, bool allocated )
{
    CORE_MUTEX_COMPONENT_PTR component_ptr = _core_mutext_get_component_ptr();

#if MQX_CHECK_ERRORS
    if (component_ptr == NULL) {
        return MQX_COMPONENT_DOES_NOT_EXIST;
    }

    /* range check mutex_id */
    if (mutex_num>=SEMA4_NUM_GATES) {
        return (MQX_INVALID_PARAMETER);
    }

    if (dev_num>=SEMA4_NUM_DEVICES) {
        return (MQX_INVALID_PARAMETER);
    }

    if (mutex_ptr==NULL) {
        return MQX_INVALID_PARAMETER;
    }
#endif

    mutex_ptr->VALID    = CORE_MUTEX_VALID;
    mutex_ptr->GATE_NUM = mutex_num;
    mutex_ptr->GATE_PTR = &(component_ptr->DEVICE[dev_num].SEMA4_PTR->GATE[mutex_ptr->GATE_NUM]);
    mutex_ptr->WAIT_Q   = _taskq_create(policy);
#if MQX_CHECK_ERRORS
    if (mutex_ptr->WAIT_Q==NULL) {
        return MQX_TASKQ_CREATE_FAILED;
    }
#endif

    _int_disable();
#if MQX_CHECK_ERRORS
    if (component_ptr->DEVICE[dev_num].MUTEX_PTR[mutex_num] !=NULL) {
        _int_enable();
        _taskq_destroy(mutex_ptr->WAIT_Q);
        return MQX_COMPONENT_EXISTS;
    }
#endif
    component_ptr->DEVICE[dev_num].MUTEX_PTR[mutex_num]=mutex_ptr;

    /* Need to remember if we allocated this mutex, or user did */
    if (allocated) {
       component_ptr->DEVICE[dev_num].ALLOCED |= (1<<mutex_num);
    }

    /* Enable 'unlocked' interrupt for this core */
    if (_psp_core_num()==0) {
        component_ptr->DEVICE[dev_num].SEMA4_PTR->CP0INE |= 1 << (SEMA4_NUM_GATES - 1 - idx[mutex_num]);	/* 1 << (SEMA4_NUM_GATES-1-mutex_num); */
    } else {
        component_ptr->DEVICE[dev_num].SEMA4_PTR->CP1INE |= 1 << (SEMA4_NUM_GATES - 1 - idx[mutex_num]);	/* 1 << (SEMA4_NUM_GATES-1-mutex_num); */
    }
    _int_enable();

    return COREMUTEX_OK;
}

/*!
 * \brief Core mutex create function.
 * 
 * This function allocates core_mutex structure and returns a handle 
 * to it. The mutex to be created is identified by SEMA4 device number and mutex 
 * (gate) number. The handle is used to reference the created mutex in calls to 
 * other core_mutex API functions. This function is to be called only once for 
 * each mutex. The policy parameter determines behavior of task queue 
 * associated with the mutex.
 * 
 * \param[in] dev_num     SEMA4 device (module) number.
 * \param[in] mutex_num   Mutex (gate) number.
 * \param[in] policy      Queuing policy, one of the following:
 * \li MQX_TASK_QUEUE_BY_PRIORITY
 * \li MQX_TASK_QUEUE_FIFO
 *
 * \return NULL (Failure.)
 * \return CORE_MUTEX_PTR (Success.)
 * 
 * \see _core_mutex_create_at 
 * \see _core_mutex_destroy 
 */ 
CORE_MUTEX_PTR _core_mutex_create(uint32_t dev_num, uint32_t mutex_num, uint32_t policy )
{
    CORE_MUTEX_PTR mutex_ptr;
    uint32_t       result;

    mutex_ptr = _mem_alloc_system_zero(sizeof(*mutex_ptr));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (mutex_ptr==NULL) {
        _task_set_error(MQX_OUT_OF_MEMORY);
        return NULL;
    }
#endif

    result = _core_mutex_create_internal(mutex_ptr, dev_num, mutex_num, policy, TRUE );
#if MQX_CHECK_ERRORS
    if (result != COREMUTEX_OK) {
        _mem_free(mutex_ptr);
        _task_set_error(result);
        mutex_ptr = NULL;
    }
#endif
    return mutex_ptr;
}

/*!
 * \brief Core mutex create_at function.
 * 
 * This function is similar to _core_mutex_create() function but it does not 
 * use dynamic allocation of CORE_MUTEX structure. A pointer to pre-allocated 
 * memory area is passed by the caller instead.
 * 
 * \param[in] mutex_ptr   Pointer to core_mutex structure.
 * \param[in] dev_num     SEMA4 device (module) number.
 * \param[in] mutex_num   Mutex (gate) number.
 * \param[in] policy      Queuing policy, one of the following:
 * \li MQX_TASK_QUEUE_BY_PRIORITY
 * \li MQX_TASK_QUEUE_FIFO
 *
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Core mutex component not installed.)
 * \return MQX_INVALID_PARAMETER (Wrong input parameter.)
 * \return MQX_TASKQ_CREATE_FAILED (Failed to create a task queue.)
 * \return MQX_COMPONENT_EXISTS (This core mutex already initialized.)
 * \return COREMUTEX_OK (Success.)
 * 
 * \see _core_mutex_create 
 * \see _core_mutex_destroy 
 */ 
uint32_t _core_mutex_create_at( CORE_MUTEX_PTR mutex_ptr, uint32_t dev_num, uint32_t mutex_num, uint32_t policy )
{
#if MQX_CHECK_ERRORS
    if (mutex_ptr==NULL) {
        return MQX_INVALID_PARAMETER;
    }
#endif

    return _core_mutex_create_internal(mutex_ptr, dev_num, mutex_num, policy, FALSE );
}

/*!
 * \brief Core mutex destroy function.
 * 
 * This function destroys a core mutex.
 * 
 * \param[in] mutex_ptr   Pointer to core_mutex structure.
 *
 * \return MQX_COMPONENT_DOES_NOT_EXIST (Core mutex component not installed.)
 * \return MQX_INVALID_PARAMETER (Wrong input parameter.)
 * \return MQX_TASKQ_CREATE_FAILED (Failed to create a task queue.)
 * \return COREMUTEX_OK (Success.)
 * 
 * \see _core_mutex_create 
 * \see _core_mutex_create_at 
 */ 
uint32_t _core_mutex_destroy( CORE_MUTEX_PTR mutex_ptr )
{
    CORE_MUTEX_COMPONENT_PTR   component_ptr = _core_mutext_get_component_ptr();
    SEMA4_MemMapPtr            sema4_ptr;
    void                      *tq;
    uint32_t                   dev_num, mutex_num, i;

#if MQX_CHECK_ERRORS
    if (component_ptr == NULL) {
       return MQX_COMPONENT_DOES_NOT_EXIST;
    }

    if (mutex_ptr==NULL) {
        return MQX_INVALID_POINTER;
    }

    if (mutex_ptr->VALID != CORE_MUTEX_VALID) {
        return MQX_INVALID_POINTER;
    }
#endif

    mutex_num = mutex_ptr->GATE_NUM;

    /* figure out which device this mutex is associated with */
    for (i=0;i<SEMA4_NUM_DEVICES;i++) {
        sema4_ptr = _bsp_get_sema4_base_address(i);
        if (&sema4_ptr->GATE[mutex_num] == mutex_ptr->GATE_PTR) {
            dev_num = i;
            break;
        }
    }

    _int_disable();
#if MQX_CHECK_ERRORS
    if (component_ptr->DEVICE[dev_num].MUTEX_PTR[mutex_num] == NULL) {
       _int_enable();
       return MQX_COMPONENT_DOES_NOT_EXIST;
    }
#endif

    component_ptr->DEVICE[dev_num].MUTEX_PTR[mutex_num] = NULL;

    if (_psp_core_num()==0) {
        component_ptr->DEVICE[dev_num].SEMA4_PTR->CP0INE &= ~(1 << (SEMA4_NUM_GATES - 1 - idx[mutex_num]));	/* ~(1 << (SEMA4_NUM_GATES-1-mutex_num)); */
    } else {
        component_ptr->DEVICE[dev_num].SEMA4_PTR->CP1INE &= ~(1 << (SEMA4_NUM_GATES - 1 - idx[mutex_num]));	/* ~(1 << (SEMA4_NUM_GATES-1-mutex_num)); */
    }
    tq = mutex_ptr->WAIT_Q;
    mutex_ptr->VALID=0;
    _int_enable();

    if (component_ptr->DEVICE[dev_num].ALLOCED & (1<<mutex_num)) {
        component_ptr->DEVICE[dev_num].ALLOCED &=  ~(1<<mutex_num);
    } else {
        mutex_ptr=NULL;
    }
    _taskq_destroy(tq);
    if (mutex_ptr) {
        _mem_free(mutex_ptr);
    }

    return COREMUTEX_OK;
}

/*!
 * \brief Get core mutex handle.
 * 
 * This function returns handle to an already created mutex.
 * 
 * \param[in] dev_num     SEMA4 device (module) number.
 * \param[in] mutex_num   Mutex (gate) number.
 *
 * \return NULL (Failure.)
 * \return CORE_MUTEX_PTR (Success.)
 * 
 * \see _core_mutex_install   
 * \see _core_mutex_create 
 * \see _core_mutex_create_at 
 */ 
CORE_MUTEX_PTR _core_mutex_get(uint32_t dev_num, uint32_t mutex_num )
{
   CORE_MUTEX_COMPONENT_PTR component_ptr = _core_mutext_get_component_ptr();

#if MQX_CHECK_ERRORS
   if (component_ptr == NULL) {
      _task_set_error(MQX_COMPONENT_DOES_NOT_EXIST);
      return NULL;
   }

   if (mutex_num>=SEMA4_NUM_GATES) {
      _task_set_error(MQX_INVALID_PARAMETER);
      return NULL;
   }

   if (dev_num>=SEMA4_NUM_DEVICES) {
      _task_set_error(MQX_INVALID_PARAMETER);
      return NULL;
   }
#endif
   return component_ptr->DEVICE[dev_num].MUTEX_PTR[mutex_num];
}

/*!
 * \brief Lock the core mutex.
 * 
 * This function attempts to lock a mutex. If the mutex is already locked 
 * by another task the function blocks and waits until it is possible to lock 
 * the mutex for the calling task.
 * 
 * \param[in] core_mutex_ptr   Pointer to core_mutex structure.
 *
 * \return MQX_INVALID_POINTER (Wrong pointer to the core_mutex structure provided.)
 * \return COREMUTEX_OK (Core mutex successfully locked.)
 * 
 * \see _core_mutex_unlock 
 * \see _core_mutex_trylock 
 */ 
uint32_t _core_mutex_lock( CORE_MUTEX_PTR core_mutex_ptr )
{
    unsigned char lock = _psp_core_num()+1;

#if MQX_CHECK_ERRORS
    if (core_mutex_ptr == NULL) {
        return MQX_INVALID_POINTER;
    }

    if (core_mutex_ptr->VALID != CORE_MUTEX_VALID) {
        return MQX_INVALID_POINTER;
    }
#endif

    _sema4_int_disable();
#if BSPCFG_CORE_MUTEX_STATS
    core_mutex_ptr->LOCKS++;
#endif

    /* Check to see if this core already own it */
    if (*core_mutex_ptr->GATE_PTR == lock) {
        /* Yes, then we have to wait for owning task to release it */
        #if BSPCFG_CORE_MUTEX_STATS
            core_mutex_ptr->WAITS++;
        #endif
        _taskq_suspend(core_mutex_ptr->WAIT_Q);

    } else {
        /* can only try to lock the mutex if another task is not waiting  otherwise we need to get in line */
        if (_taskq_get_value(core_mutex_ptr->WAIT_Q)==0) {
            *core_mutex_ptr->GATE_PTR = lock;
        }
        if (*core_mutex_ptr->GATE_PTR != lock) {
            #if BSPCFG_CORE_MUTEX_STATS
                core_mutex_ptr->WAITS++;
            #endif
            _taskq_suspend(core_mutex_ptr->WAIT_Q);
            /* Our turn now */
        }
    }
    _sema4_int_enable();

    return COREMUTEX_OK;
}

/*!
 * \brief Try to lock the core mutex.
 * 
 * This function attempts to lock a mutex. If the mutex is successfully locked 
 * for the calling task, COREMUTEX_LOCKED is returned. If the mutex is already 
 * locked by another task, the function does not block but rather returns 
 * COREMUTEX_UNLOCKED immediately.
 * 
 * \param[in] core_mutex_ptr   Pointer to core_mutex structure.
 *
 * \return MQX_INVALID_POINTER (Wrong pointer to the core_mutex structure provided.)
 * \return COREMUTEX_LOCKED (Core mutex successfully locked.)
 * \return COREMUTEX_UNLOCKED (Core mutex not locked.)
 * 
 * \see _core_mutex_lock 
 * \see _core_mutex_unlock 
 */ 
uint32_t _core_mutex_trylock( CORE_MUTEX_PTR core_mutex_ptr )
{
    volatile unsigned char  *gate_ptr;
    unsigned char            lock = _psp_core_num()+1;
    bool                     locked = FALSE;

#if MQX_CHECK_ERRORS
    if (core_mutex_ptr == NULL) {
        return MQX_INVALID_POINTER;
    }

    if (core_mutex_ptr->VALID != CORE_MUTEX_VALID) {
        return MQX_INVALID_POINTER;
    }
#endif

    gate_ptr = core_mutex_ptr->GATE_PTR;

    _sema4_int_disable();
    /* If any other task is pending on the semaphore, then it's already locked */
    if (_taskq_get_value(core_mutex_ptr->WAIT_Q)==0) {
        /* Or if it's not unlocked... */
        if (*gate_ptr == SEMA4_UNLOCK) {
            *gate_ptr = lock;
            /* double check to ensure another core didn't lock it before we could */
            locked = (*gate_ptr == lock);
        }
    }
    _sema4_int_enable();

    if(locked) {
        return COREMUTEX_LOCKED;
    }
    else {
        return COREMUTEX_UNLOCKED;
    }
}

/*!
 * \brief Unlock the core mutex.
 * 
 * This function unlocks the specified core mutex.
 * 
 * \param[in] core_mutex_ptr   Pointer to core_mutex structure.
 *
 * \return MQX_INVALID_POINTER (Wrong pointer to the core_mutex structure provided.)
 * \return MQX_NOT_RESOURCE_OWNER (This mutex has not been locked by this core.)
 * \return COREMUTEX_OK (Core mutex successfully unlocked.)
 * 
 * \see _core_mutex_lock 
 * \see _core_mutex_trylock 
 */ 
uint32_t _core_mutex_unlock( CORE_MUTEX_PTR core_mutex_ptr )
{
    unsigned char           lock = _psp_core_num()+1;
    volatile unsigned char *gate_ptr;

#if MQX_CHECK_ERRORS
    if (core_mutex_ptr == NULL) {
        return MQX_INVALID_POINTER;
    }

    if (core_mutex_ptr->VALID != CORE_MUTEX_VALID) {
        return MQX_INVALID_POINTER;
    }
#endif

    gate_ptr = core_mutex_ptr->GATE_PTR;

    /* Make sure it is locked by this core */
    if ( *gate_ptr != lock) {
        return MQX_NOT_RESOURCE_OWNER;
    }

#if BSPCFG_CORE_MUTEX_STATS
    core_mutex_ptr->UNLOCKS++;
#endif

    /* Unlock if */
    *gate_ptr = SEMA4_UNLOCK;


    /* See if this core has any other tasks waiting for this lock */
    if (_taskq_get_value(core_mutex_ptr->WAIT_Q)) {

        /* if so, have to queue the next request */
        _sema4_int_disable();
        *gate_ptr = lock;
        if (*gate_ptr == lock) {
            /* if we got it, wake up the next task */
            _taskq_resume(core_mutex_ptr->WAIT_Q, FALSE);
        }
        _sema4_int_enable();
    }

    return COREMUTEX_OK;
}

/*!
 * \brief Get core mutex owner.
 * 
 * This function returns the number of core currently "owning" the mutex.
 * 
 * \param[in] core_mutex_ptr   Pointer to core_mutex structure.
 *
 * \return MQX_INVALID_POINTER (Wrong pointer to the core_mutex structure provided.)
 * \return COREMUTEX_OK (Core number as int32_t value.)
 */ 
int32_t _core_mutex_owner( CORE_MUTEX_PTR core_mutex_ptr )
{
#if MQX_CHECK_ERRORS
    if (core_mutex_ptr == NULL) {
        return MQX_INVALID_POINTER;
    }

    if (core_mutex_ptr->VALID != CORE_MUTEX_VALID) {
        return MQX_INVALID_POINTER;
    }
#endif

    return *core_mutex_ptr->GATE_PTR - 1;
}
