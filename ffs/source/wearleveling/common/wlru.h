#if !defined(__wlru_h__)
#define __wlru_h__
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

#include "double_list.h"

/** Type definations */

/*
** Class to manage a weighted LRU list.
**
** This class maintains a doubly linked list of node sorted in LRU order, which is
** equivalent to FIFO order. In addition to strict LRU ordering, the class features
** support for weighted LRU ordering. That is, highly weighted objects have a
** higher "recency" than low weighted objects. This allows the user to retain objects
** with a high cost of loading or known high access frequency more than other objects.
**
** The constuctor takes a maximum window size parameter. If the computed window size
** is larger than the maximum, then weights will be scaled down to fit. Pass 0 to
** disable weighting entirely and enforce strict LRU ordering.
*/
class WeightedLRUList : public DoubleList
{
public:

    /*
    ** Abstract base class for a node in an LRU list.
    */
    class Node : public DoubleList::Node
    {
    public:

        /* Default constructor. */
        Node();
        
        /* Pure virtual method that is used to determine if the node is valid. */
        virtual bool isNodeValid() const = 0;
        
        /* Pure virtual method to return the node's weight value. */
        virtual int getWeight() const = 0;

    };

public:
    /* Constructor. */
    WeightedLRUList(int minWeight, int maxWeight, unsigned windowSize);
    
    /* 
    ** List operations 
    **
    ** Insert node into the list at or near the tail/MRU position.
    ** 
    ** The node is nominally inserted at the tail/MRU position, but can be moved
    ** further away from the tail by using the weighting scheme.
    ** If weights are being used in this LRU, then a weight of m_maxWeight on node will cause
    ** insertion at the tail, and a weight of zero on node will cause insertion
    ** at the head.
    */
    void insert(Node * node);
    
    /* Get the oldest entry (head/LRU) in the list. */
    Node * select();

    /* Put node back on the head/LRU of the list. */
    void deselect(Node * node);

protected:
    /* Maximum weight value. */
    int m_maxWeight;
    
    /* Scale multiplier. */
    int m_scaleNumerator;
    
    /* Scale divider. */
    int m_scaleDenominator;
};

#endif /** __wlru_h__ */

/* EOF */
