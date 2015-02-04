#if !defined(__deferred_task_h__)
#define __deferred_task_h__
/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 1989-2008 ARC International
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
*   This file contains definition of the nand::DeferredTask class.
*
*
*END************************************************************************/

#include "wl_common.h"
#include "double_list.h"

/* 
** Class definations 
*/

namespace nand
{

    class DeferredTaskQueue;

    /*
    ** \brief Deferred task abstract base class.
    **
    ** Subclasses must implement the task() and getTaskTypeID() methods. They can optionally
    ** override the getShouldExamine(), examineOne(), and examine() methods to modify how the task
    ** looks at a queue prior to being inserted, to determine whether it should be inserted at all
    ** or perhaps perform some other operation.
    **
    ** Task priorities are inverted, in the sense that the highest priority is 0 and they go
    ** down in priority as the priority value increases. The priority is passed to the constructor
    ** and must not change over the lifetime of the task object.
    **
    ** To actually perform the task, call the run method(). It will internally invoke the pure
    ** abstract task() method that subclasses must provide. If you need more complex behaviour, then
    ** you may override run().
    **
    ** A completion callback is supported. When set, the default implementation of the run() method
    ** will call the completion callback after task() returns. If you override run(), then be sure
    ** to invoke the callback before returning.
    */
    class DeferredTask : public DoubleList::Node
    {
    public:

        /* Type for a completion callback function. */
        typedef void (*CompletionCallback_t)(DeferredTask * completedTask, void * data);
        
        /* Constructor. */
        DeferredTask(int priority);
        
        /* Destructor. */
        virtual ~DeferredTask();
        
        /* Properties */
        /* Return a unique ID for this task type. */
        virtual uint32_t getTaskTypeID() const = 0;
        
        virtual bool getShouldExamine() const;
        
        /* Return the task's priority. */
        int getPriority() const { return m_priority; }
        
        /* Operations */
        /* Execute the task. */
        virtual void run();
        
        virtual bool examine(DeferredTaskQueue & queue);
        
        virtual bool examineOne(DeferredTask * task);
        
        /* Completion callbacks */
        /* Set the completion callback. */
        void setCompletion(CompletionCallback_t callback, void * data);

    protected:
        /* The priority level for this task. */
        int m_priority;
        
        /* An optional completion callback function. */
        CompletionCallback_t m_callback;
        
        /* Arbitrary data passed to the callback. */
        void * m_callbackData;
        
        /* The task entry point provided by a concrete subclass. */
        virtual void task() = 0;
        
    };

} /* namespace nand */

#endif /* __deferred_task_h__ */

/* EOF */
