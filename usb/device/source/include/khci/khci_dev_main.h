/*HEADER**********************************************************************
*
* Copyright 2009 Freescale Semiconductor, Inc.
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
*   This file contains the macros, function prototypes and data structure
*   definitions required by the Full Speed USB Device Controller driver.
*
*
*END************************************************************************/

#ifndef __khci_dev_main_h__
#define __khci_dev_main_h__

#include "khci_usbprv.h"

#define EP_DISABLE              (0)
#define ASSERT_RESUME_DELAY_COUNT       (8000)/* Delay for assert resume */

#define BYTES_512                 (512)
#define BYTES_1024                (1024)
#define ENDPOINT_NUMBER_SHIFT     (4)   /* endpoint shift & mask to */
#define ENDPOINT_NUMBER_MASK      (0xf0)/* use in setting and getting status */
#define MAX_EP_BUFFER_SIZE        (64)
#define ZERO_LENGTH               (0)
#define USB_SETUP_TOKEN           (0x0d)/* Setup Token PID */
#define FRAME_HIGH_BYTE_SHIFT     (8)

/***************************************
**
** Prototypes
**/
#ifdef __cplusplus
extern "C" {
#endif

USB_STATUS _usb_dci_usbfs_preinit(_usb_device_handle *);
USB_STATUS _usb_dci_usbfs_init(_usb_device_handle);
USB_STATUS _usb_dci_usbfs_send(_usb_device_handle, XD_STRUCT_PTR);
USB_STATUS _usb_dci_usbfs_recv(_usb_device_handle, XD_STRUCT_PTR);
USB_STATUS _usb_dci_usbfs_cancel(_usb_device_handle, uint8_t, uint8_t);
USB_STATUS _usb_dci_usbfs_set_addr(_usb_device_handle, uint8_t);
USB_STATUS _usb_dci_usbfs_shutdown(_usb_device_handle);
USB_STATUS _usb_dci_usbfs_get_setup_data(_usb_device_handle, uint8_t, unsigned char *);
USB_STATUS _usb_dci_usbfs_assert_resume(_usb_device_handle);
USB_STATUS _usb_dci_usbfs_init_endpoint(_usb_device_handle, XD_STRUCT_PTR);
USB_STATUS _usb_dci_usbfs_stall_endpoint(_usb_device_handle, uint8_t, uint8_t);
USB_STATUS _usb_dci_usbfs_unstall_endpoint(_usb_device_handle, uint8_t, uint8_t);
USB_STATUS _usb_dci_usbfs_deinit_endpoint(_usb_device_handle, uint8_t, uint8_t);
USB_STATUS _usb_dci_usbfs_get_endpoint_status(_usb_device_handle, uint8_t, uint16_t *);
USB_STATUS _usb_dci_usbfs_set_endpoint_status(_usb_device_handle, uint8_t, uint16_t);
USB_STATUS _usb_dci_usbfs_get_transfer_status(_usb_device_handle,uint8_t,uint8_t);

#ifdef __cplusplus
}
#endif

#endif
