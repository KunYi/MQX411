#ifndef __redblacktree_h__
#define __redblacktree_h__
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
*   This file contains class declarations for a red black tree implementation.
*
*
*END************************************************************************/

#include "wl_common.h"
#include <math.h>
#include <limits.h>

/* 
** Type definations 
*/

/*
** Red black tree class.
**
** This tree class is specially optimized for the media cache and is not
** intended to be totally general purpose. For one, it assumes that all tree
** node objects, instances of RedBlackTreeNode, are pre-allocated and do not
** have to be freed when they are removed from the tree.
**
** A sentinel is used for root and for nil.  These sentinels are
** created when RedBlackTreeCreate is caled.  root->left should always
** point to the node which is the root of the tree.  nil points to a
** node which should always be black but has aribtrary children and
** parent and no key or info.  The point of using these sentinels is so
** that the root and nil nodes do not require special cases in the code
**
*/
class RedBlackTree
{
public:

    /* Type for the key values used in the red black tree. */
    typedef int64_t Key_t;

    /*
    ** Abstract base class for a node of the red black tree.
    **
    ** Subclasses of this node class must implement the getKey() virtual method
    ** to return the actual key value for that node. The key value must not
    ** change while the node is inserted in the tree, or unexpected behaviour
    ** will occur (i.e., the tree will get all screwed up).
    **
    */
    class Node
    {
    public:
        /* Constructor. */
        Node()
        :   m_isRed(0),
        m_left(0),
        m_right(0),
        m_parent(0)
        {
        }
        
        /* Copy constructor. */
        Node(const Node & other)
        :   m_isRed(other.m_isRed),
        m_left(other.m_left),
        m_right(other.m_right),
        m_parent(other.m_parent)
        {
        }
        
        /* Destructor. */
        virtual ~Node() {}
        
        /* Key value accessor. */
        virtual Key_t getKey() const = 0;
        
        /* Red status accessor. */
        inline bool isRed() const { return m_isRed; }
        
        /* Node link accessors */
        inline Node * getLeft() { return m_left; }
        inline Node * getRight() { return m_right; }
        inline Node * getParent() { return m_parent; }

    protected:
        /* If red=0 then the node is black. */
        int m_isRed; 
        Node * m_left;
        Node * m_right;
        Node * m_parent;

        /* The tree is our friend so it can directly access the node link pointers. */
        friend class RedBlackTree;
    };
    
public:
    /* Constructor. */
    RedBlackTree();
    
    /* Destructor. */
    virtual ~RedBlackTree();

    /* Tree operations */
    void remove(Node * z);
    Node * insert(Node * newNode);
    Node * getPredecessorOf(Node * x) const;
    Node * getSuccessorOf(Node * x) const;
    Node * find(Key_t key) const;
    
    /* Validate certain invariants. */
    void checkAssumptions() const;
    
protected:

    /* Internal node subclass for the root sentinel node. */
    class RootNode : public Node
    {
    public:
        virtual Key_t getKey() const { return LLONG_MAX; }
    };
    
    /* Internal subclass for the nil sentinel node. */
    class NilNode : public Node
    {
    public:
        virtual Key_t getKey() const { return LLONG_MIN; }
    };

    RootNode m_rootStorage;
    NilNode m_nilStorage;
    RootNode * m_root;
    NilNode * m_nil;
    
    void leftRotate(Node * x);
    void rightRotate(Node * y);
    void insertFixUp(Node * z);
    void deleteFixUp(Node * x);
};

#endif /* __redblacktree_h__ */

/* EOF */
