#ifndef __usb_mqx_h__
#define __usb_mqx_h__
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
*   This file contains MQX-specific code used by the USB stack
*
*
*END************************************************************************/

#include "mqx.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t USB_log_error_internal(char *file, uint32_t line, uint32_t error);
extern uint32_t USB_set_error_ptr(char *file, uint32_t line, uint32_t *error_ptr, uint32_t error);

#ifdef __cplusplus
}
#endif

#endif
