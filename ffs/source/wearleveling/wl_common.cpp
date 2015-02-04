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
*   This file contains declaration of global variables of WL module
*
*
*END************************************************************************/

#include "wl_common.h"

#if  ( (WL_DEBUG))
uint32_t g_wl_log_module_attributes = 
    WL_MODULE_GENERAL 
    /*| WL_MODULE_HAL*/  
    | WL_MODULE_MAPPER 
    | WL_MODULE_MEDIA 
    | WL_MODULE_LOGICALDRIVE
    | WL_MODULE_DEFEERREDTASK 
    | WL_MODULE_MEDIABUFFER 
    | WL_MODULE_NANDWL 
    | WL_MODULE_NANDWL_TESTING
;

uint32_t g_wl_log_level = WL_LOG_DEBUG;
#else
uint32_t g_wl_log_module_attributes = WL_MODULE_GENERAL;
uint32_t g_wl_log_level = WL_LOG_DISABLE;
#endif

short countBits(short n, uint8_t bit)
{ /* Body */
    short i = 0;

    if ( n )
    {
        while ( bit == (n & 1) )
        {
            ++i;
            n >>= 1;
        } /* Endwhile */
    } /* Endif */

    return i;
} /* Endbody */

/* EOF */
