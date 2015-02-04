#ifndef __cdc_serial_h__
#define __cdc_serial_h__
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
*   This file contains keyboard-application types and definitions.
*
*
*END************************************************************************/

#ifdef __USB_OTG__
#include "otgapi.h"
#include "devapi.h"
#else
#include "hostapi.h"
#endif


/***************************************
**
** Application-specific definitions
*/

/* Used to initialize USB controller */
#define HOST_CONTROLLER_NUMBER      USBCFG_DEFAULT_HOST_CONTROLLER
#define CDC_EXAMPLE_USE_HW_FLOW     1

#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACED             (4)
#define  USB_DEVICE_DETACHED               (5)
#define  USB_DEVICE_OTHER                  (6)

#define DEVICE_REGISTERED_EVENT            (0x01)
#define DEVICE_CTRL_ATTACHED               (0x02)
#define DEVICE_CTRL_INTERFACED             (0x04)
#define DEVICE_DATA_ATTACHED               (0x08)
#define DEVICE_DATA_INTERFACED             (0x10)
#define DEVICE_CTRL_DETACHED               (0x20)

#define CDC_SERIAL_DEV_ATTACHED            (0x01)
#define CDC_SERIAL_DEV_REGISTERED          (0x02)
#define CDC_SERIAL_FUSB_OPENED_UART2USB    (0x04)
#define CDC_SERIAL_FUSB_OPENED_USB2UART    (0x08)
#define CDC_SERIAL_DEV_DETACH_USB2UART     (0x10)
#define CDC_SERIAL_DEV_DETACH_UART2USB     (0x20)
#define CDC_SERIAL_UART2USB_DONE           (0x40)
#define CDC_SERIAL_USB2UART_DONE           (0x80)
/*
** Following structs contain all states and pointers
** used by the application to control/operate devices.
*/

typedef struct acm_device_struct {
   CLASS_CALL_STRUCT                CLASS_INTF; /* Class-specific info */
   LWEVENT_STRUCT                   acm_event;
} ACM_DEVICE_STRUCT, * ACM_DEVICE_STRUCT_PTR;

typedef struct data_device_struct {
   CLASS_CALL_STRUCT                CLASS_INTF; /* Class-specific info */
   LWEVENT_STRUCT                   data_event;
} DATA_DEVICE_STRUCT, * DATA_DEVICE_STRUCT_PTR;

/* To maintain usb file handle */
typedef struct f_usb_info_struct {
   MQX_FILE_PTR f_usb;
   uint32_t cnt; /* how many times f_usb has been opened */
}F_USB_INFO_T;

/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

void usb_host_cdc_acm_event(_usb_device_instance_handle, _usb_interface_descriptor_handle, uint32_t);
void usb_host_cdc_data_event(_usb_device_instance_handle, _usb_interface_descriptor_handle, uint32_t);

#ifdef __cplusplus
}
#endif


#endif

/* EOF */
