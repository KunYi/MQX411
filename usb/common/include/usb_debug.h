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
*   This file contains definitions for debugging the software stack
*
*
*END************************************************************************/
#ifndef __usb_debug_h__
#define __usb_debug_h__
#include "usb_error.h"

/************************************************************
The following array is used to make a run time trace route
inside the USB stack.
*************************************************************/
//#define _HOST_DEBUG_
//#define _DEV_DEBUG_
#define DEBUG_LOG_TRACE(x) printf("\n%s",x);

/*--------------------------------------------------------------------------*/
/*
** FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

USB_STATUS USB_log_error(char *file, unsigned int line, USB_STATUS error);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
