/*HEADER**********************************************************************
*
* Copyright 2012 Freescale Semiconductor, Inc.
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
*   Implementation of the nand::DeferredTask class.
*
*END************************************************************************/
#include "wl_common.h"

#include "ddi/common/deferred_task.h"
#include "deferred_task_queue.h"
#include "mem_prv.h"
#include "mqx_prv.h"
using namespace nand;

/********************************************************************************
* Code
********************************************************************************/

#if !defined(__ghs__)
//#pragma mark --DeferredTaskQueue--
#endif

#if defined(__ghs__)
#pragma ghs section text=".init.text"
#endif 

#if defined (__CODEWARRIOR__)
#endif 

#define WL_TASK_NAME            "NANDWL_WLTask"
#define WL_TASK_STACK_SIZE      (3000L)
#define WL_TASK_PRIORITY        (5)
#define WL_TASK_CREATION_PARAM (0x2908)

/* In case scheduler manages DeferedTask via RoundRobin algo */
#ifdef WL_TASK_USES_RR /* */
/* Scheduler slice this DeferedTask on every 5ms */
#define WL_TASK_DEFAULT_TIME_SLICE (5)
#else 
#define WL_TASK_DEFAULT_TIME_SLICE (0)
#endif

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DeferredTaskQueue::DeferredTaskQueue
* Returned Value   : N/A
* Comments         :
*   Ctor of DeferedTaskQueue
*
*END*--------------------------------------------------------------------*/
DeferredTaskQueue::DeferredTaskQueue() :    
    m_entries(), 
    m_thread(MQX_NULL_TASK_ID), 
    m_currentTask(NULL)
{ /* Body */
    m_wl_task_template.TASK_TEMPLATE_INDEX = 0;
    // TODO: change a bit to work with IAR
    m_wl_task_template.TASK_ADDRESS = (void (_CODE_PTR_)(uint32_t))taskThreadStub;
    m_wl_task_template.TASK_STACKSIZE = WL_TASK_STACK_SIZE;
    m_wl_task_template.TASK_PRIORITY = WL_TASK_PRIORITY;
    m_wl_task_template.TASK_NAME = (char *)WL_TASK_NAME;
    m_wl_task_template.CREATION_PARAMETER = (uint32_t)this;/*WL_TASK_CREATION_PARAM;*/

    /* In case scheduler manages DeferedTask via RoundRobin algo */
#ifdef WL_TASK_USES_RR 
    m_wl_task_template.TASK_ATTRIBUTES = MQX_TIME_SLICE_TASK;
#else 
    m_wl_task_template.TASK_ATTRIBUTES = MQX_AUTO_START_TASK;
#endif  

    m_wl_task_template.DEFAULT_TIME_SLICE = WL_TASK_DEFAULT_TIME_SLICE;
}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DeferredTaskQueue::init
* Returned Value   : SUCCESS or specific error
* Comments         :
*   This init function would init the Mutex and semaphore as well
*
*END*--------------------------------------------------------------------*/
RtStatus_t DeferredTaskQueue::init() 
{ /* Body */
    RtStatus_t status;

    status = SimpleMutex::InitMutex(&m_mutex);
    if (FALSE == (bool)status)
    {
        return MQX_INVALID_HANDLE;
    } /* Endif */

    status = _lwsem_create(&m_taskSem, 0);
    if (status != MQX_OK)
    {
        WL_LOG(WL_MODULE_DEFEERREDTASK, WL_LOG_ERROR, "\nCreating read_sem failed: 0x%X", status);
        return MQX_INVALID_HANDLE;

    } /* Endif */
    return SUCCESS;
} /* Endbody */

#if defined(__ghs__)
#pragma ghs section text=default
#endif 

#if defined (__CODEWARRIOR__)
#endif 

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DeferredTaskQueue::~DeferredTaskQueue
* Returned Value   : N/A
* Comments         :
*   Dtor of DeferedTaskQueue. It removes all DeferedTask 
*   and destroy the mutex and semaphore.
*
*END*--------------------------------------------------------------------*/
DeferredTaskQueue::~DeferredTaskQueue() 
{ /* Body */
    /*  Delete any tasks remaining on the queue. */
    while (!isEmpty())
    {
        DeferredTask * task = m_entries.getHead();
        m_entries.remove(task);
        delete task;
    } /* Endwhile */

    /*
    ** Dispose of OS objects. Once the semaphore is delete, the thread (if it exists) will 
    ** deallocate itself. 
    */
    SimpleMutex::DestroyMutex(&m_mutex);
    _lwsem_destroy(&m_taskSem);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DeferredTaskQueue::drain
* Returned Value   : SUCCESS or specific error
* Comments         :
*   In order to shutdown the DeferedTaskQueue gracefully, it waits for each 
*   DeferedTask be release in every 50ms.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DeferredTaskQueue::drain() 
{ /* Body */
    /*  Sleep until the queue is completely empty and there is no task being run. */
    WL_LOG(WL_MODULE_DEFEERREDTASK, WL_LOG_INFO,
    "DeferredTaskQueue::drain m_currentTask=%0x, size of queue=%d \n",
    m_currentTask, m_entries.getSize());

    while (!isEmpty() || m_currentTask)
    {
        _time_delay(50);
    } /* Endwhile */

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DeferredTaskQueue::post
* Returned Value   : SUCCESS or specific error
* Comments         :
*   Post a specific DeferedTask to DeferedTaskQueue.
*
*END*--------------------------------------------------------------------*/
void DeferredTaskQueue::post
(
    /* [IN] task which is request to post into DeferedTaskQueue */
    DeferredTask * task
) 
{ /* Body */
    TD_STRUCT_PTR task_td_struct;
    assert(task);

    {
        /*  Lock the queue protection mutex. */
        SimpleMutex protectQueue(m_mutex);

        /*  Ask the task if it should really be inserted. */

        if (task->examine(*this))
        {
            /*  The task doesn't want to be placed into the queue, so just delete it and exit. */
            delete task;
            return;
        } /* Endif */

        int newPriority = task->getPriority();

        /*  Create iterators.        */
        iterator_t it = getBegin();
        iterator_t last = getEnd();

        /*  Search for the insert position for this task. This could easily be optimized. */
        for (; it != last; ++it)
        {

            if (newPriority < it->getPriority())
            {
                break;
            }/* Endif */
        } /* Endfor */

        /*  Insert the new task before the search iterator. */
        m_entries.insertBefore(task, it);
    }

    /*  
    ** Put the semaphore to indicate a newly available task. 
    ** This semaphore should be post before check task already create  
    ** because the task thread function only wait Sem for a while   
    */
    _lwsem_post(&m_taskSem);

    /*  Create the task thread if necessary. */
    if (MQX_NULL_TASK_ID == m_thread)
    {
        m_thread = _task_create_blocked(0, 0, (uint32_t) &m_wl_task_template);
        /*_task_set_parameter_for((uint32_t)this, m_thread);*/

        assert(m_thread != MQX_NULL_TASK_ID);

        WL_LOG(WL_MODULE_DEFEERREDTASK, WL_LOG_INFO, "Nand: started deferred task thread\n");
        task_td_struct = (TD_STRUCT_PTR)_task_get_td(m_thread);
        _task_ready(task_td_struct);
    }/* Endif */
    
    
}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DeferredTaskQueue::taskThreadStub
* Returned Value   : N/A
* Comments         :
*   This static stub function simply passes control along to the member function of 
*   the object passed in as its sole argument. 
*END*--------------------------------------------------------------------*/
void DeferredTaskQueue::taskThreadStub
(
    /* [IN] Pointer of DeferredTaskQueue */
    uint32_t arg
) 

{ /* Body */
    DeferredTaskQueue * _this = reinterpret_cast<DeferredTaskQueue *> (arg);
    if (_this)
    {
        _this->taskThread();
    }/* Endif */
}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DeferredTaskQueue::taskThread
* Returned Value   : N/A
* Comments         :
*   This task function dequeues DeferedTask from queue then
*   execute it consecutively
*
*END*--------------------------------------------------------------------*/
void DeferredTaskQueue::taskThread() 
{ /* Body */
    PSP_TICK_STRUCT ticks;
    _task_id task_id_tmp;
    /*  
    ** Loop until the semaphore get times out, which means that there 
    ** have been no available tasks for some time. It may also return an error, which 
    ** is likely because the semaphore was deleted. 
    */
    PSP_MILLISECONDS_TO_TICKS(kTaskThreadTimeoutMSecs, &ticks);
    while (MQX_OK == _lwsem_wait_until(&m_taskSem, (MQX_TICK_STRUCT_PTR) &ticks) )
    {
        /* while (MQX_OK ==  _lwsem_wait(&m_taskSem) ) { */
        DeferredTask * task;

        /*  Pop the head of the queue. */
        {
            SimpleMutex protectQueue(m_mutex);
            task = m_entries.getHead();
            m_entries.remove(task);
            m_currentTask = task;
        }

        /*
        ** It's conceivable that the semaphore count is greater than the number of tasks, 
        ** if a task modified the queue in its examine() method. 
        */
        if (task)
        {
            WL_LOG(WL_MODULE_DEFEERREDTASK, WL_LOG_INFO,
            "Nand: running deferred task 0x%08x\n", (uint32_t)task);

            /* Execute this task, then dispose of it. */
            task->run();

            m_currentTask = NULL;
            delete task;
        }/* Endif */
    }/* Endwhile */

    WL_LOG(WL_MODULE_DEFEERREDTASK, WL_LOG_INFO, "Nand: exiting deferred task thread\n");

    /*
    ** Post a DPC to deallocate this thread. This thread's struct pointer is passed to 
    ** the DPC function as its argument, and we clear our member thread pointer, to prevent 
    ** any possible collisions in case we get a new task before the old thread has fully 
    ** been disposed. 
    */
    task_id_tmp = m_thread;
    m_thread = MQX_NULL_TASK_ID;
    _task_destroy(task_id_tmp); 
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DeferredTaskQueue::disposeTaskThread
* Returned Value   : N/A
* Comments         :
*   A dynamically allocated thread cannot dispose of itself. So the last thing 
*   the task thread does is to post this function as a DPC in order to clean 
*   itself up. 
*
*END*--------------------------------------------------------------------*/
void DeferredTaskQueue::disposeTaskThread
(
    /* [IN] ID of dispose task */
    uint32_t param
) 
{ /* Body */
    _task_id taskid = (_task_id) param;
    _task_destroy(taskid);
} /* Endbody */

/*  EOF */
