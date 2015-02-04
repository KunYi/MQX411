#ifndef __print_host_printer_h__
#define __print_host_printer_h__
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
*   This file contains definitions for  USB printer class.
*   Reference Specification:
*   "USB Device Class Definition for Printing Devices" version 1.1
*   January 2000, from USB Implementers Forum.
*
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_main.h"
#include "host_snd.h"
#include "host_rcv.h"
#include "host_ch9.h"

/* Printer Port-Status Bits */
#define  USB_PRINTER_PAPER_OUT     (0x20)   /* 0 = Not empty */
                                          /* 1 = Paper empty */
#define  USB_PRINTER_SELECTED      (0x10)   /* 0 = Not selected */
                                          /* 1 = Selected */
#define  USB_PRINTER_NO_ERROR      (0x08)   /* 0 = Error */
                                          /* 1 = No error!!! */

/* Bulk In/Out Endpoint-found tags */
#define  USB_PRINTER_IN_TAG   (0x01)
#define  USB_PRINTER_OUT_TAG  (0x02)
#define  USB_PRINTER_BI_TAG   (USB_PRINTER_IN_TAG | USB_PRINTER_OUT_TAG)

/* Error codes */
#define  USB_PRINTER_UNSUPPORTED_SUB_CLASS (0x80)
#define  USB_PRINTER_UNSUPPORTED_PROTOCOL  (0x81)
#define  USB_PRINTER_BAD_COMMAND           (0x82)

/* Class-specific Requests for Printers */
#define  USB_PRINTER_GET_DEVICE_ID        0
#define  USB_PRINTER_GET_PORT_STATUS      1
#define  USB_PRINTER_SOFT_RESET           2
/* Non-class Requests for Printers */
#define  USB_PRINTER_SEND_BULK            3
#define  USB_PRINTER_RECV_BULK            4

/* Class-specific interface info struct, useful for
**       an application to send/receive on in/out
**       pipes by name without internal details.
** NOTE: Since the class may have to handle several
**       printers attached simultaneously, it is
**       essential to have an array or list.
**       Here we choose a NULL-terminated list. */
typedef struct printer_interface {
   struct printer_interface        *next;    /* next printer */
   struct printer_interface        *anchor;  /* list start */
   _usb_device_instance_handle      dev_handle;
   _usb_interface_descriptor_handle intf_handle;
   _usb_host_handle                 host_handle;
   uint32_t                          key_code;
   _usb_pipe_handle                 control_pipe;
   _usb_pipe_handle                 bulk_in_pipe;
   _usb_pipe_handle                 bulk_out_pipe;
} PRINTER_INTERFACE_STRUCT, * PRINTER_INTERFACE_STRUCT_PTR;

/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

USB_STATUS usb_printer_get_device_ID(CLASS_CALL_STRUCT_PTR,
   tr_callback, void *, uint32_t, unsigned char *);
USB_STATUS usb_printer_get_port_status(CLASS_CALL_STRUCT_PTR,
   tr_callback, void *, uint8_t *);
void usb_printer_init(PIPE_BUNDLE_STRUCT_PTR, CLASS_CALL_STRUCT_PTR);
USB_STATUS usb_printer_recv_data(CLASS_CALL_STRUCT_PTR,
   tr_callback, void *, uint32_t, unsigned char *);
USB_STATUS usb_printer_send_data(CLASS_CALL_STRUCT_PTR,
   tr_callback, void *, uint32_t, unsigned char *);
USB_STATUS usb_printer_soft_reset(CLASS_CALL_STRUCT_PTR,
   tr_callback, void *);

#ifdef __cplusplus
}
#endif


#endif
