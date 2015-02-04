/*HEADER**********************************************************************
*
* Copyright 2008-2011 Freescale Semiconductor, Inc.
* Copyright 2004-2011 Embedded Access Inc.
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
*   This file contains the source functions for functions required to
*   control the SEMA4 module.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include <core_mutex.h>

static CORE_MUTEX_COMPONENT_PTR _core_mutex_component_ptr = NULL;

/*!
 * \brief Get pointer to core mutex component structure.
 * 
 * This function returns pointer to the core mutex component structure.
 * 
 * \return CORE_MUTEX_COMPONENT_PTR
 */ 
CORE_MUTEX_COMPONENT_PTR _core_mutext_get_component_ptr(void)
{
   return _core_mutex_component_ptr;
}

/*!
 * \brief Set pointer to core mutex component structure.
 * 
 * This function sets the core mutex component pointer.
 * 
 * \param[in] p   Pointer to the core mutex component structure.
 *
 * \return void
 */ 
void _core_mutext_set_component_ptr(CORE_MUTEX_COMPONENT_PTR p)
{
   _core_mutex_component_ptr=p;
}
