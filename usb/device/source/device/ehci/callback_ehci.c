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
*   This file contains the default callback function for the ehci 
*   and device.
*                                                               
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "dev_cnfg.h"
#include "devapi.h"
#include "dev_main.h"
#include "mqx_dev.h"

#include "ehci_dev_main.h"
#include "ehci_usbprv.h"

const USB_DEV_CALLBACK_FUNCTIONS_STRUCT _usb_ehci_dev_callback_table =
{
   _usb_dci_usbhs_preinit,
   _usb_dci_usbhs_init,
   _usb_dci_usbhs_send_data,
   _usb_dci_usbhs_recv_data,
   _usb_dci_usbhs_cancel_transfer,
   _usb_dci_usbhs_init_endpoint,
   _usb_dci_usbhs_deinit_endpoint,
   _usb_dci_usbhs_unstall_endpoint,
   _usb_dci_usbhs_get_endpoint_status,
   _usb_dci_usbhs_set_endpoint_status,
   _usb_dci_usbhs_get_transfer_status,
   _usb_dci_usbhs_set_address,
   _usb_dci_usbhs_shutdown,
   _usb_dci_usbhs_get_setup_data,
   _usb_dci_usbhs_assert_resume,
   _usb_dci_usbhs_stall_endpoint,
};
