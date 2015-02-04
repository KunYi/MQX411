#ifndef __ehci_utl_h__
#define __ehci_utl_h__
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
*   
*
*
*END************************************************************************/
#include "host_common.h"

#ifdef __cplusplus
extern "C" {
#endif

extern USB_STATUS _usb_ehci_bus_control (_usb_host_handle, uint8_t);
extern void _usb_ehci_bus_suspend (_usb_host_handle);
extern void _usb_ehci_bus_resume (_usb_host_handle, EHCI_PIPE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
