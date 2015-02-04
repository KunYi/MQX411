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
*   This file contains red black tree implementation.
*
*
*END************************************************************************/

#include "red_black_tree.h"

/*
** If the symbol CHECK_RB_TREE_ASSUMPTIONS is defined then the
** code does a lot of extra checking to make sure certain assumptions
** are satisfied.  This only needs to be done if you suspect bugs are
** present or if you make significant changes and want to make sure
** your changes didn't mess anything up.
*/
#define CHECK_RB_TREE_ASSUMPTIONS                   0

#if defined(__ghs__)
#pragma ghs section text=".init.text"
#endif /* defined(__ghs__) */

#if defined(__CODEWARRIOR__)
#endif /* defined(__CODEWARRIOR__) */

RedBlackTree::RedBlackTree()
:   m_rootStorage(),
    m_nilStorage(),
    m_root(&m_rootStorage),
    m_nil(&m_nilStorage)
{
    m_nil->m_left = m_nil->m_right = m_nil->m_parent = m_nil;
    m_nil->m_isRed = 0;
    
    m_root->m_parent = m_root->m_left = m_root->m_right = m_nil;
    m_root->m_isRed = 0;
}

#if defined(__ghs__)
#pragma ghs section text=default
#endif /* defined(__ghs__) */

#if defined(__CODEWARRIOR__)
#endif /* defined(__CODEWARRIOR__) */

RedBlackTree::~RedBlackTree()
{
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : find
* Returned Value   : A Node object
* Comments         :
*   This function find a node depending on its key
*
*END*--------------------------------------------------------------------*/
RedBlackTree::Node * RedBlackTree::find(Key_t key) const
{ /* Body */
    Node * node = m_root->m_left;
    
    while (node != m_nil)
    {
        Key_t nodeKey = node->getKey();
        
        /* Return the current node if matches the search key. */
        if (nodeKey == key)
        {
            return node;
        } /* Endif */
        
        /* Follow the tree. */
        node = nodeKey > key ? node->m_left : node->m_right;
    } /* Endwhile */
    
    /* Didn't find a matching tree node, so return NULL. */
    return NULL;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : leftRotate
* Returned Value   : void
* Comments         :
*   Rotates as described in _Introduction_To_Algorithms by
*   Cormen, Leiserson, Rivest (Chapter 14).  Basically this
*   makes the parent of x be to the left of x, x the parent of
*   its parent before the rotation and fixes other pointers
*   accordingly.
*END*--------------------------------------------------------------------*/
void RedBlackTree::leftRotate
(
    /* [IN] Node x */
    Node* x
)
{
    Node * y;
    
    /* 
    ** I originally wrote this function to use the sentinel for
    ** m_nil to avoid checking for m_nil.  However this introduces a
    ** very subtle bug because sometimes this function modifies
    ** the parent pointer of m_nil.  This can be a problem if a
    ** function which calls leftRotate also uses the m_nil sentinel
    ** and expects the m_nil sentinel's parent pointer to be unchanged
    ** after calling this function.  For example, when DeleteFixUP
    ** calls leftRotate it expects the parent pointer of m_nil to be
    ** unchanged. 
    */
    
    y = x->m_right;
    x->m_right = y->m_left;
    
    if (y->m_left != m_nil)
    {
        y->m_left->m_parent = x;
    }
    
    y->m_parent = x->m_parent;   
    
    /* 
    ** Instead of checking if x->m_parent is the m_root as in the book, 
    ** we count on the m_root sentinel to implicitly take care of this case 
    */
    if (x == x->m_parent->m_left)
    {
        x->m_parent->m_left = y;
    }
    else
    {
        x->m_parent->m_right = y;
    } /* Endif */
    y->m_left = x;
    x->m_parent = y;

#ifdef CHECK_RB_TREE_ASSUMPTIONS
    checkAssumptions();
#endif /* CHECK_RB_TREE_ASSUMPTIONS */
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : rightRotate
* Returned Value   : void
* Comments         :
*   Rotates as described in _Introduction_To_Algorithms by
*   Cormen, Leiserson, Rivest (Chapter 14).  Basically this
*   makes the parent of x be to the left of x, x the parent of
*   its parent before the rotation and fixes other pointers
*   accordingly. 
*
*END*--------------------------------------------------------------------*/
void RedBlackTree::rightRotate
(
    /* [IN] Node y */
    Node* y
)
{
    Node * x;
    
    /*
    ** I originally wrote this function to use the sentinel for
    ** m_nil to avoid checking for m_nil.  However this introduces a
    ** very subtle bug because sometimes this function modifies
    ** the parent pointer of m_nil.  This can be a problem if a
    ** function which calls leftRotate also uses the m_nil sentinel
    ** and expects the m_nil sentinel's parent pointer to be unchanged
    ** after calling this function.  For example, when DeleteFixUP
    ** calls leftRotate it expects the parent pointer of m_nil to be
    ** unchanged.
    */
    
    x = y->m_left;
    y->m_left = x->m_right;
    
    if (m_nil != x->m_right)
    {
        x->m_right->m_parent = y;
    } /* Endif */
    
    /* 
    ** Instead of checking if x->m_parent is the m_root as in the book, 
    ** we count on the m_root sentinel to implicitly take care of this case. 
    */
    x->m_parent = y->m_parent;
    if (y == y->m_parent->m_left)
    {
        y->m_parent->m_left = x;
    }
    else
    {
        y->m_parent->m_right = x;
    } /* Endif */
    x->m_right = y;
    y->m_parent = x;

#ifdef CHECK_RB_TREE_ASSUMPTIONS
    checkAssumptions();
#endif /* CHECK_RB_TREE_ASSUMPTIONS */
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : insertFixUp
* Returned Value   : void
* Comments         :
*   Inserts z into the tree as if it were a regular binary tree
*   using the algorithm described in _Introduction_To_Algorithms_
*   by Cormen et al.  This funciton is only intended to be called
*   by the Insert function and not by the user
*
*END*--------------------------------------------------------------------*/
void RedBlackTree::insertFixUp
(
    /* [IN] Node z */
    Node * z
)
{
    Node * x;
    Node * y;
    
    z->m_left = m_nil;
    z->m_right = m_nil;
    y = m_root;
    x = m_root->m_left;
    
    Key_t zKey = z->getKey();
    
    while (x != m_nil)
    {
        y = x;
        if (x->getKey() > zKey)
        { 
            x = x->m_left;
        }
        else
        {
            // x->key <= z->key
            x = x->m_right;
        } /* Endif */
    } /* Endwhile */
    z->m_parent = y;
    
    if (y == m_root || (y->getKey() > zKey))
    { 
        y->m_left = z;
    }
    else 
    {
        y->m_right = z;
    } /* Endif */
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : _taskq_test
* Returned Value   : MQX_OK or an error code
* Comments         :
*   Before calling InsertNode  the node x should have its key set
*
*   This function returns a pointer to the newly inserted node
*   which is guarunteed to be valid until this node is deleted.
*   What this means is if another data structure stores this
*   pointer then the tree does not need to be searched when this
*   is to be deleted.
*
*   Creates a node node which contains the appropriate key and
*   info pointers and inserts it into the tree.*
*END*--------------------------------------------------------------------*/
RedBlackTree::Node * RedBlackTree::insert
(
    /* [IN] New node */
    Node * newNode
)
{ /* Body */
    Node * y;
    Node * x;

    x = newNode;
    insertFixUp(x);
    x->m_isRed = 1;
    
    while (x->m_parent->m_isRed)
    {
        /* Use sentinel instead of checking for m_root. */
        if (x->m_parent == x->m_parent->m_parent->m_left)
        {
            y = x->m_parent->m_parent->m_right;
            if (y->m_isRed)
            {
                x->m_parent->m_isRed = 0;
                y->m_isRed = 0;
                x->m_parent->m_parent->m_isRed = 1;
                x = x->m_parent->m_parent;
            }
            else
            {
                if (x == x->m_parent->m_right)
                {
                    x = x->m_parent;
                    leftRotate(x);
                } /* Endif */
                x->m_parent->m_isRed = 0;
                x->m_parent->m_parent->m_isRed = 1;
                rightRotate(x->m_parent->m_parent);
            } /* Endif */
        }
        else
        {
            /* 
            ** Case for x->m_parent == x->m_parent->m_parent->m_right.
            ** This part is just like the section above with
            ** left and right interchanged. 
            */
            y = x->m_parent->m_parent->m_left;
            if (y->m_isRed)
            {
                x->m_parent->m_isRed = 0;
                y->m_isRed = 0;
                x->m_parent->m_parent->m_isRed = 1;
                x = x->m_parent->m_parent;
            }
            else
            {
                if (x == x->m_parent->m_left)
                {
                    x = x->m_parent;
                    rightRotate(x);
                } /* Endif */
                x->m_parent->m_isRed = 0;
                x->m_parent->m_parent->m_isRed = 1;
                leftRotate(x->m_parent->m_parent);
            } /* Endif */
        } /* Endif */
    } /* Endwhile */
    m_root->m_left->m_isRed = 0;

#ifdef CHECK_RB_TREE_ASSUMPTIONS
    checkAssumptions();
#endif /* CHECK_RB_TREE_ASSUMPTIONS */

    return(newNode);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getSuccessorOf
* Returned Value   : A Node object
* Comments         :
*   This function returns the successor of x or NULL if no successor exists.
*
*END*--------------------------------------------------------------------*/
RedBlackTree::Node * RedBlackTree::getSuccessorOf
(
    /* [IN] Node x */
    Node * x
) const
{ /* Body */
    Node * y;
    
    if (x == NULL)
    {
        x = m_nil;
    } /* Endif */
    
    if (m_nil != (y = x->m_right)) // Assignment to y is intentional.
    {
        while (y->m_left != m_nil)
        {
            // Returns the minium of the right subtree of x.
            y = y->m_left;
        } /* Endwhile */
        return y;
    }
    else
    {
        y = x->m_parent;
        while (x == y->m_right)
        {
            // Sentinel used instead of checking for m_nil.
            x = y;
            y = y->m_parent;
        } /* Endwhile */
        
        if (y == m_root)
        {
            return NULL;
        } /* Endif */
        
        return y;
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : _taskq_test
* Returned Value   : MQX_OK or an error code
* Comments         :
*   This function returns the predecessor of x or NULL if no predecessor exists.
*
*END*--------------------------------------------------------------------*/
RedBlackTree::Node * RedBlackTree::getPredecessorOf
(
    /* [IN] Node x */
    Node * x
) const
{ /* Body */
    Node * y;
    
    if (x == NULL)
    {
        x = m_nil;
    } /* Endif */
    
    if (m_nil != (y = x->m_left)) // Assignment to y is intentional.
    {
        while (y->m_right != m_nil)
        {
            // Returns the maximum of the left subtree of x.
            y = y->m_right;
        } /* Endwhile */
        return y;
    }
    else
    {
        y = x->m_parent;
        while (x == y->m_left)
        { 
            if (y == m_root)
            {
                return NULL;
            } /* Endif */
            
            x = y;
            y = y->m_parent;
        } /* Endwhile */
        return y;
    } /* Endif */
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : deleteFixUp
* Returned Value   : void
* Comments         :
*   Performs rotations and changes colors to restore red-black
*   properties after a node is deleted
*
*   x is the child of the spliced out node in DeleteNode. 
*
*END*--------------------------------------------------------------------*/
void RedBlackTree::deleteFixUp
(
    /* [IN] A node */
    Node* x
)
{ /* Body */
    Node * w;
    Node * rootLeft = m_root->m_left;
    
    while ((!x->m_isRed) && (rootLeft != x))
    {
        if (x == x->m_parent->m_left)
        {
            w = x->m_parent->m_right;
            if (w->m_isRed)
            {
                w->m_isRed = 0;
                x->m_parent->m_isRed = 1;
                leftRotate(x->m_parent);
                w = x->m_parent->m_right;
            } /* Endif */
            
            if ((!w->m_right->m_isRed) && (!w->m_left->m_isRed))
            { 
                w->m_isRed = 1;
                x = x->m_parent;
            }
            else
            {
                if (!w->m_right->m_isRed)
                {
                    w->m_left->m_isRed = 0;
                    w->m_isRed = 1;
                    rightRotate(w);
                    w = x->m_parent->m_right;
                } /* Endif */
                
                w->m_isRed = x->m_parent->m_isRed;
                x->m_parent->m_isRed = 0;
                w->m_right->m_isRed = 0;
                leftRotate(x->m_parent);
                
                /* This is to exit while loop. */
                x = rootLeft;
            } /* Endif */
        }
        else
        {
            /* The code below is has left and right switched from above. */
            w = x->m_parent->m_left;
            if (w->m_isRed)
            {
                w->m_isRed = 0;
                x->m_parent->m_isRed = 1;
                rightRotate(x->m_parent);
                w = x->m_parent->m_left;
            } /* Endif */
            
            if ((!w->m_right->m_isRed) && (!w->m_left->m_isRed))
            { 
                w->m_isRed = 1;
                x = x->m_parent;
            }
            else
            {
                if (!w->m_left->m_isRed)
                {
                    w->m_right->m_isRed = 0;
                    w->m_isRed = 1;
                    leftRotate(w);
                    w = x->m_parent->m_left;
                } /* Endif */
                
                w->m_isRed = x->m_parent->m_isRed;
                x->m_parent->m_isRed = 0;
                w->m_left->m_isRed = 0;
                rightRotate(x->m_parent);
                /* This is to exit while loop */
                x = rootLeft;
            } /* Endif */
        } /* Endif */
    } /* Endwhile */
    x->m_isRed = 0;

#ifdef CHECK_RB_TREE_ASSUMPTIONS
    checkAssumptions();
#endif /* CHECK_RB_TREE_ASSUMPTIONS */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : remove
* Returned Value   : void
* Comments         :
*   Deletes z from tree.
*
*END*--------------------------------------------------------------------*/
void RedBlackTree::remove
(
    /* [IN] Removed node */
    Node * z
)
{ /* Body */
    /* Check if the node is already not a node of the tree. */
    if (z->m_left == NULL || z->m_right == NULL || z->m_parent == NULL)
    {
        return;
    } /* Endif */
    
    Node * y;
    Node * x;
    
    y = ((z->m_left == m_nil) || (z->m_right == m_nil)) ? z : getSuccessorOf(z);
    x = (y->m_left == m_nil) ? y->m_right : y->m_left;
    
    /* Assignment of y->p to x->p is intentional */
    if (m_root == (x->m_parent = y->m_parent))
    {
        m_root->m_left = x;
    }
    else
    {
        if (y == y->m_parent->m_left)
        {
            y->m_parent->m_left = x;
        }
        else
        {
            y->m_parent->m_right = x;
        } /* Endif */
    } /* Endif */
    
    if (y != z)
    {
        /* y should not be m_nil in this case */
        assert(y != m_nil);
        
        /* y is the node to splice out and x is its child */
        
        y->m_left = z->m_left;
        y->m_right = z->m_right;
        y->m_parent = z->m_parent;
        z->m_left->m_parent = z->m_right->m_parent=y;
        
        if (z == z->m_parent->m_left)
        {
            z->m_parent->m_left = y; 
        }
        else
        {
            z->m_parent->m_right = y;
        } /* Endif */
        
        if (!(y->m_isRed))
        {
            y->m_isRed = z->m_isRed;
            deleteFixUp(x);
        }
        else
        {
            y->m_isRed = z->m_isRed;
        } /* Endif */
    }
    else 
    {
        /* y == z */
        if (!(y->m_isRed))
        {
            deleteFixUp(x);
        }
    }
    
    /* Clear links of the removed node. */
    z->m_parent = NULL;
    z->m_left = NULL;
    z->m_right = NULL;
    z->m_isRed = 0;

#ifdef CHECK_RB_TREE_ASSUMPTIONS
    checkAssumptions();
#endif /* CHECK_RB_TREE_ASSUMPTIONS */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : checkAssumptions
* Returned Value   : void
* Comments         :
*   This function checks the list valid or not
*
*END*--------------------------------------------------------------------*/
void RedBlackTree::checkAssumptions() const
{ /* Body */
    assert(m_nil->m_isRed == 0);
    assert(m_root->m_isRed == 0);
} /* Endbody */

/* EOF */
