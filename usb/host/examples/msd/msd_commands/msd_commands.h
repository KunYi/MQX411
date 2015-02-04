#ifndef __msc_commands_h__
#define __msc_commands_h__
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
*   This file contains mass storage class application types and definitions.
*
*
*END************************************************************************/

#include "host_dev_list.h"
#include "host_common.h"

/* Application-specific definitions */
#define  MAX_PENDING_TRANSACTIONS      16
#define  MAX_FRAME_SIZE                1024
#define  HOST_CONTROLLER_NUMBER        USBCFG_DEFAULT_HOST_CONTROLLER
#define  MAX_MASS_DEVICES              (3)
#define  TEST_WRITE                    (0)

#define  BUFF_SIZE                     (0x1000)

#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACED             (4)
#define  USB_DEVICE_DETACHED               (5)
#define  USB_DEVICE_OTHER                  (6)

/*
** Following structs contain all states and pointers
** used by the application to control/operate devices.
*/

typedef struct device_struct
{
   uint32_t                          dev_state;  /* Attach/detach state */
   PIPE_BUNDLE_STRUCT_PTR           pbs;        /* Device & pipes */
   _usb_device_instance_handle      dev_handle;
   _usb_interface_descriptor_handle intf_handle;
   CLASS_CALL_STRUCT                class_intf; /* Class-specific info */
} DEVICE_STRUCT, * DEVICE_STRUCT_PTR;


/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

static void usb_host_mass_device_event(_usb_device_instance_handle, _usb_interface_descriptor_handle, uint32_t);
static void usb_host_mass_ctrl_callback(_usb_pipe_handle, void *, unsigned char *, uint32_t, uint32_t);
static void usb_host_mass_bulk_callback(USB_STATUS, void *, void *, uint32_t);
static void usb_host_mass_test_storage(_mqx_uint i);

#ifdef __cplusplus
}
#endif


#endif

/* EOF */
