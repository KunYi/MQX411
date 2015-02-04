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
*   This file contains implementation of the nand::DeferredTask class.
*
*
*END************************************************************************/
#include "wl_common.h"
#include "deferred_task_queue.h"
#include "deferred_task.h"
#include "simple_mutex.h"

using namespace nand;

#if !defined(__ghs__)
//#pragma mark --DeferredTask--
#endif /* (__ghs__) */

DeferredTask::DeferredTask(int priority)
:   DoubleList::Node(),
    m_priority(priority),
    m_callback(NULL),
    m_callbackData(0)
{
}

DeferredTask::~DeferredTask()
{
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getShouldExamine
* Returned Value   : TRUE or FALSE
* Comments         :
*   Returns whether the task wants to examine queue entries before insertion.
*   By default, we do want to examine queue entries. However, 
*   examineOne(DeferredTask * task) must be overridden by the subclass to modify
*   the default behaviour of always being inserted into the queue.
*
*END*--------------------------------------------------------------------*/
bool DeferredTask::getShouldExamine() const
{ /* Body */
    return true;
} /* Endbody */

void DeferredTask::setCompletion(CompletionCallback_t callback, void * data)
{ /* Body */
    m_callback = callback;
    m_callbackData = data;
} /* Endbody */

void DeferredTask::run()
{ /* Body */
    /* Do the deed. */
    task();
    
    /* Invoke the completion callback if set. */
    if (m_callback)
    {
        m_callback(this, m_callbackData);
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : examine
* Returned Value   : TRUE or FASLE
*   - TRUE Indicates that this task should not be inserted into the queue. It is 
*   up to the calling queue to delete the task.
*   - FALSE Continue with inserting the new task into the queue.
* Comments         :
*   Optionally review current queue entries and take action.
*   This method will iterate over all of the tasks currently in \a queue, from beginning
*   to end. It will call examineOne(DeferredTask * task) on each entry for detailed
*   examination. If that call returns true then iteration is stopped and true returned to
*   the caller immediately.
*
*   If getShouldExamine() returns false, then the queue will not be examined and no other
*   action will be taken. In this case, false will always be returned to indicate that the
*   task should be inserted into the queue.
*
*END*--------------------------------------------------------------------*/
bool DeferredTask::examine(DeferredTaskQueue & queue)
{
    /* 
    ** If we don't want to examine the queue, then return false to indicate that we should
    ** just be inserted into the queue as normal. 
    */
    if (!getShouldExamine())
    {
        return false;
    } /* Endif */
    
    DeferredTaskQueue::iterator_t it = queue.getBegin();
    DeferredTaskQueue::iterator_t last = queue.getEnd();
    
    /* Search for the insert position for this task. This could easily be optimized. */
    for (; it != last; ++it)
    {
        /* Let's take a look at this one queue entry. */
        if (examineOne(*it))
        {
            /* Hold it! We don't want to be placed into the queue for some reason. */
            return true;
        } /* Endif */
    } /* Endfor */

    /* Continue with insertion in the queue. */
    return false;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : examineOne
* Returned Value   : TRUE or FASLE
*   - TRUE Indicates that this task should not be inserted into the queue.
*   - FALSE Continue with inserting the new task into the queue.
* Comments         :
*   Optionally review a single current queue entry and take action.
*
*END*--------------------------------------------------------------------*/
bool DeferredTask::examineOne(DeferredTask * task)
{ /* Body */
    /* Continue with insertion in the queue. */
    return false;
} /* Endbody */

/* EOF */
