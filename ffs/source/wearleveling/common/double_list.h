#if !defined(__double_list_h__)
#define __double_list_h__
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
*   This file contains declaration of a doubly linked list class.
*
*
*END************************************************************************/

/* 
** Type definations 
*/

/*
** Doubly linked list.
**
** This list class maintains a double linked list of Node objects. It is intended to work
** similarly to the STL std::list class, but is generally smaller in size and does not
** support all of the STL features such as reverse iterators.
**
** All nodes of the list must be a subclass of DoubleList::Node. This class itself is not a
** template in order to keep the code simple and to reduce duplicate code. Most users will
** actually want to use the DoubleListT<T> template subclass that lets you specify your
** Node subclass in the template argument, and thus works more like std::list.
**
** The user of this class must manage memory for nodes; this class assumes no responsibility
** for creating or deleting node objects. In particular, when the list object is deleted it
** will not automatically delete any nodes in the list for you. Thus, you must take special
** care to walk the list and delete nodes before deleting the list. This is done to allow
** users greater control over the memory lifetimes of node objects. If you need a certain
** behaviour, you may want to subclass DoubleList to add your own memory management.
**
** To walk a list, use an iterator object very much like you would for std::list. Get the
** iterator with getBegin(), and compare against the iterator returned by getEnd() to tell
** when there are no more nodes to return.
**
** Example list walking code:
**  DoubleList mylist;
**  DoubleList::Iterator myit = mylist.getBegin();
**  
**  for (; myit != mylist.getEnd(); ++myit)
**  {
**      DoubleList::Node ** n = *mylist;
**      // use the node
**  }
**
** It is also possible to use a simpler form of iteration, like this:
**  DoubleList::Iterator myit = mylist.getBegin();
**  DoubleList::Node ** n;
*
**  while ((n = *myit++))
**  {
**      // use the node
**  }
**
** Notice that this second form of iteration uses the postfix increment operator supported
** by the Iterator class. Both styles of iteration are equally efficient, so you can use
** whichever one you prefer.
*/
class DoubleList
{
public:

    /*!
    ** List node base class.
    **
    ** Subclass this node class to add your own data. You may also use this class as a
    ** mix-in with other classes. The next and previous link data members provided by this
    ** class are what allow it to be inserted in a DoubleList.
    */
    class Node
    {
    public:
        /* Default constructor. */
        Node();
        
        /* Sibling access */
        Node * getNext() { return m_next; }
        const Node * getNext() const { return m_next; }
        
        Node * getPrevious() { return m_prev; }
        const Node * getPrevious() const { return m_prev; }
        
    protected:
        /* Pointer to previous node in the list. */
        Node * m_prev;
        
        /* Pointer to next node in the list. */
        Node * m_next;
        
        friend class DoubleList;
    };
    
    /*
    ** List iterator class.
    **
    ** An iterator with a NULL node pointer represents the item after the end of the list.
    **
    ** Both prefix and postfix increment and decrement operators are supported. This allows
    ** you to use an alternate iteration style that uses a while loop instead of the for
    ** loop and comparison against the end iterator that simulates the STL style.
    */
    class Iterator
    {
    public:
        /* Constructor. */
        Iterator(Node * theNode) : m_current(theNode) {}
        
        /* Copy constructor. */
        Iterator(const Iterator & other) : m_current(other.m_current) {}
        
        /* Assignment operator. */
        Iterator & operator = (const Iterator & other)
        {
            m_current = other.m_current;
            return *this;
        }
        
        /* Prefix increment/decrement */
        void operator ++ () { if (m_current) m_current = m_current->getNext(); }
        void operator -- () { if (m_current) m_current = m_current->getPrevious(); }

        /* Postfix increment/decrement */
        void operator ++ (int) { ++(*this); }
        void operator -- (int) { --(*this); }
        
        /* Conversion operators */
        Node * operator * () { return m_current; }
        Node * operator -> () { return m_current; }
        operator Node * () { return m_current; }
        
        /* Comparison operators */
        bool operator == (const Iterator & other) { return m_current == other.m_current; }
        bool operator != (const Iterator & other) { return !(*this == other); }
        
    protected:
        /* 
        ** The current node pointed to by this iterator.
        ** This pointer will be NULL when the iterator represents the end of the list. 
        */
        Node * m_current;
    };

    /* Default constructor. */
    DoubleList();
    
    /* List operations */
    /* Insert a node into the list at the start of the list. */
    void insertFront(Node * node) { insertAfter(node, NULL); }

    /* Append the node onto the end of the list. */
    void insertBack(Node * node) { insertAfter(node, getTail()); }

    /* 
    ** Insert a node into the list after node insertPos.
    ** If (NULL == insertPos), then node is inserted at the head of the list instead. 
    */
    void insertAfter(Node * node, Node * insertPos);

    /* 
    ** Insert a node into the list before node insertPos.
    ** If (NULL == insertPos), then node is inserted at the tail of the list instead. 
    */
    void insertBefore(Node * node, Node * insertPos);

    /* Remove node from its place in the list. */
    void remove(Node * node);
    
    /* Remove all items from the list. */
    void clear();
    
    /* Access */
    /* Returns the first item in the list. */
    Node * getHead() { return m_head; }

    /* Returns the first item in the list. */
    const Node * getHead() const { return m_head; }

    /* Returns the first item in the list. */
    Node * getTail() { return m_tail; }

    /* Returns the first item in the list. */
    const Node * getTail() const { return m_tail; }
    
    /* Iterator creation */
    Iterator getBegin() { return Iterator(m_head); }
    Iterator getEnd() { return Iterator(NULL); }
    
    /* List info */
    /* Returns true if the list has no items in it. */
    bool isEmpty() const { return m_head == NULL; }
    
    /* Returns the number of items currently in the list. */
    int getSize() const { return m_size; }

protected:
    /* The head of the list. */
    Node * m_head; 
    
    /* The tail of the list. */
    Node * m_tail;
    
    /* Number of items in the list. */
    int m_size;
};

/*
** Template subclass of DoubleList to simplify its usage.
**
** This template class extends its DoubleList superclass so that the type of all node objects
** is the type specified in the template argument. This allows you to use the class without
** having to cast to your custom Node subclass all over the place. The same concept applies
** to the DoubleList<T>::Iterator class defined herein.
**
** A major reason for having a non-template base class is so that the code can be easily placed
** in the correct linker section. It also reduces duplication of code dramatically. If the
** entire list class were a template, then all of its code may be duplicated by the compiler
** for each template variant.
*/
template <class T>
class DoubleListT : public DoubleList
{
public:

    /* List iterator that works with the template node class. */
    class Iterator : public DoubleList::Iterator
    {
    public:
        /* Constructor. */
        Iterator(T * theNode) :
        DoubleList::Iterator(theNode) {
        }

        /* Copy constructor. */
        Iterator(const Iterator & other) :
        DoubleList::Iterator(other.m_current) {
        }

        /* Assignment operator. */
        Iterator & operator =(const Iterator & other) {
            m_current = other.m_current;
            return *this;
        }        
        
        /* Conversion operators */
        T * operator * () { return static_cast<T*>(m_current); }
        T * operator -> () { return static_cast<T*>(m_current); }
        operator T * () { return static_cast<T*>(m_current); }
    };

#if defined(__ghs__)
#pragma ghs section text=".init.text"
#endif /* defined(__ghs__) */

#if defined(__CODEWARRIOR__)
#endif /* defined(__CODEWARRIOR__) */
    /* Default constructor. */
    DoubleListT() : DoubleList() {}

#if defined(__ghs__)
#pragma ghs section text=default
#endif /* defined(__ghs__) */

#if defined (__CODEWARRIOR__)
#endif /* defined(__CODEWARRIOR__) */

    /* Access */
    /* Returns the first item in the list. */
    T * getHead() { return static_cast<T *>(m_head); }

    /* Returns the first item in the list. */
    const T * getHead() const { return static_cast<T *>(m_head); }

    /* Returns the first item in the list. */
    T * getTail() { return static_cast<T *>(m_tail); }

    /* Returns the first item in the list. */
    const T * getTail() const { return static_cast<T *>(m_tail); }
    
    /* Iterator creation */
    Iterator getBegin() { return Iterator(static_cast<T*>(m_head)); }
    Iterator getEnd() { return Iterator(static_cast<T*>(m_head)); }    
};

#endif /* __double_list_h__ */

/* EOF */
