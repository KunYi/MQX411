#ifndef _simple_mutex_h_
#define _simple_mutex_h_
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
*   Defines a mutex helper class
*
*END************************************************************************/

#include "wl_common.h"
#include "mutex.h"

typedef MUTEX_STRUCT WL_MUTEX ;
/*
** Types
*/

/*
** Mutex helper class.
**
** Automatically gets the mutex in the constructor and puts the mutex in the destructor.
** To use, allocate an instance on the stack so that it will put the mutex when it
** falls out of scope.
*/
class SimpleMutex {
public:

    static bool InitMutex(WL_MUTEX * pIncMutex);
    static bool DestroyMutex(WL_MUTEX * pIncMutex);
    /* Constructor. Gets mutex. Takes a mutex pointer. */
    SimpleMutex(WL_MUTEX * pIncMutex);

    /* Constructor. Gets mutex. Takes a mutex reference. */
    SimpleMutex(WL_MUTEX & incMutex);
    /* Destructor. Puts mutex. */
    ~SimpleMutex();

protected:
    static bool CreateMutexAttr() ;

protected:
    /* Pointer to the mutex object. */
    WL_MUTEX * m_mutex;
    static MUTEX_ATTR_STRUCT m_mutexattr;
};

#endif /* _simple_mutex_h_ */
/*  EOF */
