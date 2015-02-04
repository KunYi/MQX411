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
*   This file contains MQX-specific code to install low-level device
*   driver functions
*
*
*END************************************************************************/
#include <mqx.h>

#include <usb.h>
#include <usb_debug.h>

USB_STATUS USB_log_error(char *file, unsigned int line, USB_STATUS error)
{
   if ((error != USB_OK) && (error != USB_STATUS_TRANSFER_QUEUED)) {
   }
   return error;
}
