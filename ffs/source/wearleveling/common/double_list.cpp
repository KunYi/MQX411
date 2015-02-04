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
*   This file contains implementation of the doulbly linked list class.
*
*
*END************************************************************************/

#include "wl_common.h"
#include "double_list.h"

#if defined(__ghs__)
#pragma ghs section text=".init.text"
#endif /* defined(__ghs__) */

#if defined(__CODEWARRIOR__)
// TODO: #pragma ghs section text=".init.text"
#endif /* defined(__CODEWARRIOR__) */

DoubleList::DoubleList()
:   m_head(0),
    m_tail(0),
    m_size(0)
{
}

#if defined(__ghs__)
#pragma ghs section text=default
#endif /* defined(__ghs__) */

#if defined(__CODEWARRIOR__)
// TODO: pragma ghs section text="default"
#endif /* defined(__CODEWARRIOR__) */

DoubleList::Node::Node()
:   m_prev(NULL),
    m_next(NULL)
{
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : insertAfter
* Returned Value   : void
* Comments         :
*   This function insert a node into a list after another node
*
*END*--------------------------------------------------------------------*/
void DoubleList::insertAfter
(
    /* [IN] A new node */
    Node * node, 
    
    /* [IN] Another node */
    Node * insertPos
)
{ /* Body */
    assert(node);

    if (!insertPos)
    {
        /* Inserting at the head/LRU of the list. */
        node->m_prev = NULL;
        node->m_next = m_head;
        
        /* Update list head. */
        if (m_head)
        {
            m_head->m_prev = node;
        } /* Endif */
        m_head = node;
        
        /* Update tail. Special case for single item list. */
        if (!m_tail)
        {
            m_tail = node;
        } /* Endif */
    }
    else
    {   
        /* Insert after insertPos. */
        node->m_next = insertPos->m_next;
        if (node->m_next)
        {
            node->m_next->m_prev = node;
        } /* Endif */
        insertPos->m_next = node;
        node->m_prev = insertPos;
        
        /* Update list tail. */
        if (insertPos == m_tail)
        {
            m_tail = node;
        } /* Endif */
    } /* Endif */
    
    ++m_size;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : insertBefore
* Returned Value   : void
* Comments         :
*   This function insert a node into a list before another node
*
*END*--------------------------------------------------------------------*/
void DoubleList::insertBefore
(
    /* [IN] A new node */
    Node * node, 
    
    /* [IN] Another node */
    Node * insertPos
)
{ /* Body */
    if (insertPos)
    {
        /* Insert after the node previous to the given position. */
        insertAfter(node, insertPos->getPrevious());
    }
    else
    {
        /* The insert position was NULL, so put at the end of the list. */
        insertBack(node);
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : remove
* Returned Value   : void
* Comments         :
*   This function removes a node from list
*
*END*--------------------------------------------------------------------*/
void DoubleList::remove
(
    /* [IN] Removed node */
    Node * node
)
{
    /* Disconnect from list. */
    if (node->m_prev)
    {
        node->m_prev->m_next = node->m_next;
    } /* Endif */
    if (node->m_next)
    {
        node->m_next->m_prev = node->m_prev;
    } /* Endif */
    
    if (m_head == node)
    {
        m_head = node->m_next;
    } /* Endif */
    
    if (m_tail == node)
    {
        m_tail = node->m_prev;
    } /* Endif */
    
    /* Clear node links. */
    node->m_next = NULL;
    node->m_prev = NULL;
    
    --m_size;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : clear
* Returned Value   : void
* Comments         :
*   This function deletes all node from list
*
*END*--------------------------------------------------------------------*/
void DoubleList::clear()
{
    m_head = NULL;
    m_tail = NULL;
    m_size = 0;
}

/* EOF */
