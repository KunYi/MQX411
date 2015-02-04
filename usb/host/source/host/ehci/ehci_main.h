#ifndef __ehci_main_h__
#define __ehci_main_h__
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
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "hostapi.h"

#include "ehci_main.h"
#include "ehci_prv.h"

#ifdef __cplusplus
extern "C" {
#endif

USB_STATUS _usb_ehci_send_data (_usb_host_handle, PIPE_STRUCT_PTR, TR_STRUCT_PTR);
USB_STATUS _usb_ehci_send_setup (_usb_host_handle, PIPE_STRUCT_PTR, TR_STRUCT_PTR);
USB_STATUS _usb_ehci_recv_data (_usb_host_handle, PIPE_STRUCT_PTR, TR_STRUCT_PTR);
USB_STATUS _usb_ehci_queue_pkts ( _usb_host_handle, EHCI_PIPE_STRUCT_PTR, EHCI_TR_STRUCT_PTR);
void _usb_ehci_init_Q_HEAD (_usb_host_handle, EHCI_PIPE_STRUCT_PTR, EHCI_QH_STRUCT_PTR, EHCI_QH_STRUCT_PTR, EHCI_QTD_STRUCT_PTR);
void _usb_ehci_init_QTD (_usb_host_handle, EHCI_QTD_STRUCT_PTR, unsigned char *, uint32_t);
uint32_t _usb_ehci_init_Q_element (_usb_host_handle, EHCI_PIPE_STRUCT_PTR, EHCI_TR_STRUCT_PTR);
uint32_t _usb_ehci_add_xfer_to_asynch_schedule_list (_usb_host_handle, EHCI_PIPE_STRUCT_PTR, EHCI_TR_STRUCT_PTR);
bool _usb_ehci_process_port_change (_usb_host_handle);
void _usb_ehci_reset_and_enable_port (_usb_host_handle, uint8_t);
void _usb_host_process_reset_recovery_done (_usb_host_handle);

void _usb_ehci_process_tr_complete(_usb_host_handle);

void _usb_ehci_isr (_usb_host_handle);

USB_STATUS _usb_ehci_preinit (_usb_host_handle *handle);
USB_STATUS _usb_ehci_init (_usb_host_handle);
void _usb_ehci_free_QTD (_usb_host_handle, void *);
void _usb_ehci_free_QH (_usb_host_handle, void *);
USB_STATUS _usb_ehci_free_resources (_usb_host_handle, PIPE_STRUCT_PTR);
void _usb_host_delay (_usb_host_handle, uint32_t);

USB_STATUS _usb_ehci_open_pipe(_usb_host_handle handle, PIPE_STRUCT_PTR);

void _usb_ehci_process_timer(_usb_host_handle);
USB_STATUS _usb_ehci_update_dev_address(_usb_host_handle, PIPE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
