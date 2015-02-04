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
*   This file contains declaration of a weighted LRU class.
*
*
*END************************************************************************/

#include "wl_common.h"
#include "wlru.h"

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

#if defined(__ghs__)
#pragma ghs section text=".init.text"
#endif /* defined(__ghs__) */

#if defined(__CODEWARRIOR__)
#endif /* defined(__CODEWARRIOR__) */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : WeightedLRUList
* Returned Value   : n/a
* Comments         : n/a
*
*END*--------------------------------------------------------------------*/
WeightedLRUList::WeightedLRUList
(
    /* [IN] Min weight */
    int minWeight, 
    
    /* [IN] Max weight */
    int maxWeight, 
    
    /* [IN] Window size */
    unsigned windowSize
)
:   DoubleList(),
    m_maxWeight(maxWeight),
    m_scaleNumerator(1),
    m_scaleDenominator(1)
{
    /* Compute window size. */
    if (windowSize == 0)
    {
        m_scaleNumerator = 0;
        m_scaleDenominator = 0;
    }
    else
    {
        int k = m_maxWeight - minWeight;
        
        if (k > (int)windowSize)
        {
            /* Round up to fit within the window. */
            m_scaleDenominator = (k + windowSize - 1) / windowSize;
        } /* Endif */
    } /* Endif */
}

#if defined(__ghs__)
#pragma ghs section text=default
#endif /* defined(__ghs__) */

#if defined(__CODEWARRIOR__)
#endif /* defined(__CODEWARRIOR__) */

WeightedLRUList::Node::Node()
:   DoubleList::Node()
{
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : insert
* Returned Value   : void
* Comments         :
*   This function inserts a node to list
*
*END*--------------------------------------------------------------------*/
void WeightedLRUList::insert(Node * node)
{
    Node * insertPos;
    
    /* Find where to insert the entry. */
    if (!node->isNodeValid())
    {
        /* 
        ** Invalid cache entries get inserted at the head/LRU of the list,
        ** because we want to use them immediately. 
        */
        insertPos = NULL;
    }
    else
    {
        /* If the window size is 0 then we just insert at the tail/MRU. */
        insertPos = static_cast<Node *>(getTail());

        if (m_scaleDenominator > 0)
        {
            /* 
            ** The LRU window size is nonzero, so we need to use the weight on
            ** this node to choose an insertion point proportionally toward the head/LRU. 
            */
            int ki = (m_maxWeight - node->getWeight()) * m_scaleNumerator / m_scaleDenominator;
            int i = 0;
            
            /* 
            ** Work from tail to head to find the insert position in the window for this
            ** node's weight. 
            */
            while (insertPos && i < ki)
            {
                insertPos = static_cast<Node *>(insertPos->getPrevious());
                i++;
            } /* Endwhile */            
        } /* Endif */
    } /* Endif */
    
    /* Insert in sorted position. */
    insertAfter(node, insertPos);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : select
* Returned Value   : Node
* Comments         :
*   This function selects a node and remove it from list
*
*END*--------------------------------------------------------------------*/
WeightedLRUList::Node * WeightedLRUList::select()
{
    Node * node = static_cast<Node *>(getHead());
    if (node)
    {
        remove(node);
    }
    return node;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : deselect
* Returned Value   : void
* Comments         :
*   This function insert a node on top of list
*
*END*--------------------------------------------------------------------*/
void WeightedLRUList::deselect(Node * node)
{
    insertFront(node);
}

/* EOF */
