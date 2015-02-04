#ifndef __auto_free_h__
#define __auto_free_h__
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
*   Defines a class to ensure release of memory allocated 
*
*
*END************************************************************************/

#include <stdlib.h>

/*
** Type definitions
*/

/*
** Template helper class to automatically free memory.
** When the instance of this class falls out of scope, it will free the pointer it
** owns with the standard C library free() API.
*/

template <typename T>
class auto_free
{
    typedef T * t_ptr_t;
    typedef const T * const_t_ptr_t;
    typedef T & t_ref_t;
    typedef const T & const_t_ref_t;
    
    /* The pointer we own. */
    t_ptr_t m_ptr;
    
public:
    /* Default constructor, sets pointer to NULL. */
    inline auto_free() : m_ptr(0) {}
    
    /* Constructor taking a pointer to own. */
    inline auto_free(t_ptr_t p) : m_ptr(p) {}
    
    /* Constructor taking a void pointer to own. */
    inline auto_free(void * p) : m_ptr(reinterpret_cast<t_ptr_t>(p)) {}
    
    /* Copy constructor. */
    inline auto_free(auto_free<T> & o) : m_ptr(o.release()) {}
    
    /* Copy constructor for compatible pointer type. */
    template <typename O>
    inline auto_free(auto_free<O> & o) : m_ptr(o.release()) {}
    
    /* Destructor. */
    inline ~auto_free()
    {
        free();
    }
    
    /* Clear the pointer we own. */
    inline void reset()
    {
        m_ptr = 0;
    }
    
    /* Return the pointer and clear it. */
    inline t_ptr_t release()
    {
        t_ptr_t tmp = m_ptr;
        reset();
        return tmp;
    }
    
    /* 
    ** Free the memory occupied by the pointer.
    ** Does nothing if the pointer is NULL. 
    */
    inline void free()
    {
        if (m_ptr)
        {
            ::free(m_ptr);
            reset();
        }
    }
    
    /* 
    ** Get and set
    ** Return the owned pointer. 
    */
    inline t_ptr_t get() { return m_ptr; }
    
    /* Return the owned pointer. */
    inline const_t_ptr_t get() const { return m_ptr; }
    
    /* 
    ** Changed the owned pointer to a new value.
    ** If there was a previous pointer, it will be freed first. If p is NULL or the same
    ** as the currently owned pointer then nothing will be done. 
    */
    inline void set(t_ptr_t p)
    {
        if (p && p != m_ptr)
        {
            free();
        }
        m_ptr = p;
    }
    
    /* Variant of set() taking a void pointer. */
    inline void set(void * p) { set(reinterpret_cast<t_ptr_t>(p)); }
    
    /* Conversion Operators */
    inline operator t_ptr_t () { return m_ptr; }

    inline operator const_t_ptr_t () const { return m_ptr; }
    
    inline operator bool () const { return m_ptr != NULL; }
    
    /* Access operators */
    inline t_ref_t operator * () { return *m_ptr; }
    
    inline const_t_ref_t operator * () const { return *m_ptr; }
    
    inline t_ptr_t operator -> () { return m_ptr; }
    
    inline const_t_ptr_t operator -> () const { return m_ptr; }
    
    /* Assignment operator. */
    inline auto_free<T> & operator = (t_ptr_t p)
    {
        set(p);
        return *this;
    }
    
    /* Compatible assignment operator. */
    template <typename O>
    inline auto_free<T> & operator = (auto_free<O> & o)
    {
        set(o.release());
        return *this;
    }

};

/*
** Template helper class to automatically free memory.
** When the instance of this class falls out of scope, it will free the pointer it
** owns with the delete operator. This class is much like std::auto_ptr but adds
** some useful functionality. Plus, it resembles auto_free.
*/
template <typename T>
class auto_delete
{
    typedef T * t_ptr_t;
    typedef const T * const_t_ptr_t;
    typedef T & t_ref_t;
    typedef const T & const_t_ref_t;
    
    /* The pointer we own. */
    t_ptr_t m_ptr;
    
public:
    /* Default constructor, sets pointer to NULL. */
    inline auto_delete() : m_ptr(0) {}
    
    /* Constructor taking a pointer to own. */
    inline auto_delete(t_ptr_t p) : m_ptr(p) {}
    
    /* Constructor taking a void pointer to own. */
    inline auto_delete(void * p) : m_ptr(reinterpret_cast<t_ptr_t>(p)) {}
    
    /* Copy constructor. */
    inline auto_delete(auto_delete<T> & o) : m_ptr(o.release()) {}
    
    /* Copy constructor for compatible pointer type. */
    template <typename O>
    inline auto_delete(auto_delete<O> & o) : m_ptr(o.release()) {}
    
    /* Destructor. */
    inline ~auto_delete()
    {
        free();
    }
    
    /* 
    ** Pointer control
    ** Clear the pointer we own without deleting it. 
    */
    inline void reset()
    {
        m_ptr = 0;
    }
    
    /* Return the pointer and clear it. */
    inline t_ptr_t release()
    {
        t_ptr_t tmp = m_ptr;
        reset();
        return tmp;
    }
    
    /* 
    ** Free the memory occupied by the pointer.
    ** Does nothing if the pointer is NULL. 
    */
    inline void free()
    {
        if (m_ptr)
        {
            delete m_ptr;
            reset();
        }
    }
    
    /* Return the owned pointer. */
    inline t_ptr_t get() { return m_ptr; }
    
    /* Return the owned pointer. */
    inline const_t_ptr_t get() const { return m_ptr; }
    
    /* 
    ** Changed the owned pointer to a new value.
    ** If there was a previous pointer, it will be freed first. If \a p is NULL or the same
    ** as the currently owned pointer then nothing will be done. 
    */
    inline void set(t_ptr_t p)
    {
        if (p && p != m_ptr)
        {
            free();
        }
        m_ptr = p;
    }
    
    /* Variant of set() taking a void pointer. */
    inline void set(void * p) { set(reinterpret_cast<t_ptr_t>(p)); }
    
    /* Conversion Operators */
    inline operator t_ptr_t () { return m_ptr; }

    inline operator const_t_ptr_t () const { return m_ptr; }
    
    inline operator bool () const { return m_ptr != NULL; }
    
    /* Access operators */
    inline t_ref_t operator * () { return *m_ptr; }
    
    inline const_t_ref_t operator * () const { return *m_ptr; }
    
    inline t_ptr_t operator -> () { return m_ptr; }
    
    inline const_t_ptr_t operator -> () const { return m_ptr; }
    
    /* Assignment operator. */
    inline auto_delete<T> & operator = (t_ptr_t p)
    {
        set(p);
        return *this;
    }
    
    /* Compatible assignment operator. */
    template <typename O>
    inline auto_delete<T> & operator = (auto_free<O> & o)
    {
        set(o.release());
        return *this;
    }

};

#endif /* __auto_free_h__ */

/* EOF */
