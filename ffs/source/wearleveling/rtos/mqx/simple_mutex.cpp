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
*  Defines a mutex helper class
*
*END************************************************************************/

#include "wl_common.h"
#include "simple_mutex.h"

MUTEX_ATTR_STRUCT SimpleMutex::m_mutexattr =
{   0};

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name    : SimpleMutex::CreateMutexAttr
 * Returned Value   : TRUE or FALSE
 * Comments         :
 *   Initialise the Mutex Attributes
 *
 *END*--------------------------------------------------------------------*/
bool SimpleMutex::CreateMutexAttr()
{ /* Body */
    static bool bIsInit = FALSE;

    /* Init already */
    if (bIsInit)
    return TRUE;

    if (_mutatr_init(&SimpleMutex::m_mutexattr) != MQX_OK)
    {
        WL_LOG(WL_MODULE_GENERAL,WL_LOG_ERROR, "Initializing mutex attributes failed.\n");
        return FALSE;
    }/* Endif*/
    bIsInit = TRUE;
    return TRUE;
}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name    : SimpleMutex::InitMutex
 * Returned Value   : TRUE or FALSE
 * Comments         :
 *   Initialise the Mutex 
 *
 *END*--------------------------------------------------------------------*/
bool SimpleMutex::InitMutex
(
    /* [IN] pointer to Mutex  */
    WL_MUTEX * pIncMutex
)
{/* Body */
    SimpleMutex::CreateMutexAttr();

    if (_mutex_init(pIncMutex, &SimpleMutex::m_mutexattr) != MQX_OK)
    {
        WL_LOG(WL_MODULE_GENERAL, WL_LOG_ERROR,"Initializing mutex failed.\n");
        return FALSE;
    }/* Endif*/
    return TRUE;
}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name    : SimpleMutex::DestroyMutex
 * Returned Value   : TRUE or FALSE
 * Comments         :
 *   Destroy the Mutex 
 *
 *END*--------------------------------------------------------------------*/
bool SimpleMutex::DestroyMutex
(
    /* [IN] pointer to Mutex  */
    WL_MUTEX * pIncMutex
)
{/* Body */

    if (_mutex_destroy(pIncMutex) != MQX_OK)
    {
        WL_LOG(WL_MODULE_GENERAL, WL_LOG_ERROR,"Destroying mutex failed.\n");
        return FALSE;
    }/* Endif*/
    return TRUE;
}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name    : SimpleMutex::InitMutex
 * Returned Value   : TRUE or FALSE
 * Comments         :
 *   Constructor. Gets mutex. Takes a mutex pointer 
 *
 *END*--------------------------------------------------------------------*/
SimpleMutex::SimpleMutex
(
    /* [IN] pointer to Mutex  */
    WL_MUTEX * pIncMutex
)
:
m_mutex(pIncMutex)
{/* Body */
    if (_mutex_lock(m_mutex) != MQX_OK)
    {
        WL_LOG(WL_MODULE_GENERAL, WL_LOG_ERROR, "Mutex lock failed.\n");
        assert(0);
    }/* Endif*/

}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name    : SimpleMutex::InitMutex
 * Returned Value   : TRUE or FALSE
 * Comments         :
 *   Constructor. Gets mutex. Takes a mutex reference 
 *
 *END*--------------------------------------------------------------------*/
SimpleMutex::SimpleMutex
(
    /* [IN] pointer to Mutex  */
    WL_MUTEX & incMutex
)
:
m_mutex(&incMutex)
{/* Body */
    if (_mutex_lock(m_mutex) != MQX_OK)
    {
        WL_LOG(WL_MODULE_GENERAL, WL_LOG_ERROR, "Mutex lock failed.\n");
        assert(0);
    }/* Endif*/
}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name    : SimpleMutex::InitMutex
 * Returned Value   : TRUE or FALSE
 * Comments         :
 *   Destructor. Puts mutex 
 *
 *END*--------------------------------------------------------------------*/
SimpleMutex::~SimpleMutex()
{ /* Body */
    _mutex_unlock(m_mutex);
}/* Endbody */

/* EOF */
