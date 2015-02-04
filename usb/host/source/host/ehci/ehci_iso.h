#ifndef __ehci_iso_h__
#define __ehci_iso_h__
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

extern uint32_t _usb_ehci_link_structure_in_periodic_list (_usb_host_handle, EHCI_PIPE_STRUCT_PTR, uint32_t *, uint32_t);
extern uint32_t _usb_ehci_add_ITD (_usb_host_handle, EHCI_PIPE_STRUCT_PTR, EHCI_TR_STRUCT_PTR);
extern USB_STATUS _usb_ehci_add_SITD (_usb_host_handle, EHCI_PIPE_STRUCT_PTR, EHCI_TR_STRUCT_PTR);
extern uint32_t _usb_ehci_add_isochronous_xfer_to_periodic_schedule_list (_usb_host_handle, EHCI_PIPE_STRUCT_PTR, EHCI_TR_STRUCT_PTR);
extern void _usb_ehci_free_ITD (_usb_host_handle, void *);
extern void _usb_ehci_free_SITD (_usb_host_handle, void *);
extern void _usb_ehci_close_isochronous_pipe (_usb_host_handle, EHCI_PIPE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
