#if !defined(__deferred_task_queue_h__)
#define __deferred_task_queue_h__
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
*   Definition of the nand::DeferredTask class.
*
*END************************************************************************/

#include "wl_common.h"
#include "double_list.h"
#include "simple_mutex.h"
#include "deferred_task.h"
/*
** Definitions
*/

namespace nand
{


/*
**  Priority queue of deferred task objects.
**
** This class is not only a priority queue but the manager for the thread that executes the
** tasks inserted into the queue.
**
** Users of a queue must ensure that the drain() method is called prior to destructing the queue
** if they want all tasks to be executed. Otherwise, the destructor will simply delete any
** tasks remaining on the queue.
*/
class DeferredTaskQueue
{
public:

    /* Constants for the task execution thread. */
    enum _task_thread_constants
    {
        /* kTaskThreadStackSize = 2048, */
        /* kTaskThreadPriority = 12, */
        kTaskThreadTimeoutMSecs = 500
    };

    /* Queue types */
    typedef uint16_t test12;
    typedef DoubleListT<DeferredTask> queue_t;    
    typedef DoubleListT<DeferredTask>::Iterator iterator_t;

    /* Constructor. */
    DeferredTaskQueue();
    
    /* Destructor. */
    ~DeferredTaskQueue();
    
    /* Initializer. */
    RtStatus_t init();
    
    /* Wait for all current tasks to complete. */
    RtStatus_t drain();
    
    /* Add a new task to the queue. */
    void post(DeferredTask * task);
    
    /* Returns whether the queue is empty. */
    bool isEmpty() const { return m_entries.isEmpty(); }

    iterator_t getBegin() {  return m_entries.getBegin(); }
    iterator_t getEnd()  { return m_entries.getEnd(); }
    
    /* Returns the task that is currently being executed. */
    DeferredTask * getCurrentTask() { return m_currentTask; }

protected:
    
    /* Mutex protecting the queue. */
    mutex_struct m_mutex;   
    
    /* List of queue entries. */
    queue_t m_entries;  
    
    /* Thread used to execute tasks. */
    _task_id m_thread;   
    
    /* Semaphore to signal availability of tasks to the thread. */
    LWSEM_STRUCT m_taskSem; 
    
    /* Task being executed. */
    DeferredTask * volatile m_currentTask;   
    TASK_TEMPLATE_STRUCT m_wl_task_template;
    
    /* Static entry point for the task thread. */
    static void taskThreadStub(uint32_t arg);
    
    /* The main entry point for the task thread. */
    void taskThread();
    
    /* Function to dispose of the task thread. */
    static void disposeTaskThread(uint32_t param);

};


} /*  namespace nand */

#endif /*  __deferred_task_queue_h__ */
/*  EOF */
