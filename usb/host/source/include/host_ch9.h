#ifndef __host_ch9_h__
#define __host_ch9_h__
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
*   This file contains definitions for standard device requests,
*   i.e. USB specification Chapter 9 requests.
*
*
*END************************************************************************/
#include "host_common.h"

/*----------------------------------------------------------------**
** Chapter 9.4 Standard Device Requests -- all devices            **
** See Table 9-3 p. 250 of USB 2.0 spec for combinations          **
**   of request type bitfields with requests, WVALUE, WINDEX etc. **
**----------------------------------------------------------------*/
#define  REQ_TYPE_DEVICE    0x00
#define  REQ_TYPE_INTERFACE 0x01
#define  REQ_TYPE_ENDPOINT  0x02

/* Combine one of the 3 above with one of the following 2 */
#define  REQ_TYPE_IN        0x80
#define  REQ_TYPE_OUT       0x00

/* Also for class requests set the following bit */
#define  REQ_TYPE_CLASS     0x20

/* Standard USB requests, see Chapter 9 */
#define  REQ_GET_STATUS          0
#define  REQ_CLEAR_FEATURE       1
#define  REQ_SET_FEATURE         3
#define  REQ_SET_ADDRESS         5
#define  REQ_GET_DESCRIPTOR      6
#define  REQ_SET_DESCRIPTOR      7
#define  REQ_GET_CONFIGURATION   8
#define  REQ_SET_CONFIGURATION   9
#define  REQ_GET_INTERFACE       10
#define  REQ_SET_INTERFACE       11
#define  REQ_SYNCH_FRAME         12

/*
NOTE!!!
DEPRECATED: Use USB_DESC_TYPE_xxx constant from usb_desc.h !
#define  DESCTYPE_DEVICE         0x1
#define  DESCTYPE_CONFIG         0x2
#define  DESCTYPE_STRING         0x3
#define  DESCTYPE_INTERFACE      0x4
#define  DESCTYPE_ENDPOINT       0x5
#define  DESCTYPE_QUALIFIER      0x6
#define  DESCTYPE_OTHER_SPEED    0x7
#define  DESCTYPE_INTF_POWER     0x8
#define  DESCTYPE_OTG            0x9
*/ 

/*******************************************************************
**
** Values specific to CLEAR FEATURE commands (must go to common.h later)
*/

#define  ENDPOINT_HALT        0
#define  DEVICE_REMOTE_WAKEUP 1
#define  TEST_MODE            2

/* States of device instances on the device list */

/* initial device state */
#define  DEVSTATE_INITIAL        0x00
/* device descriptor [0..7]*/
#define  DEVSTATE_DEVDESC8       0x01
/* address set */
#define  DEVSTATE_ADDR_SET       0x02
/* full device descriptor */
#define  DEVSTATE_DEV_DESC       0x03
/* config descriptor [0..7] */
#define  DEVSTATE_GET_CFG9       0x04
/* config set */
#define  DEVSTATE_SET_CFG        0x05
/* full config desc. read in */
#define  DEVSTATE_CFG_READ       0x06
/* application callbacks */
#define  DEVSTATE_APP_CALL       0x07
/* Select interface done */
#define  DEVSTATE_SET_INTF       0x08
#define  DEVSTATE_ENUM_OK        0x09
#define  DEVSTATE_CHK_OTG        0x0A

/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

USB_STATUS _usb_host_register_ch9_callback(_usb_device_instance_handle, tr_callback, void *);
USB_STATUS _usb_host_ch9_clear_feature(_usb_device_instance_handle, uint8_t, uint8_t, uint16_t);
USB_STATUS _usb_host_ch9_get_configuration(_usb_device_instance_handle, unsigned char *);     
USB_STATUS _usb_host_ch9_get_descriptor(_usb_device_instance_handle, uint16_t, uint16_t, uint16_t, unsigned char *);     
USB_STATUS _usb_host_ch9_get_interface(_usb_device_instance_handle, uint8_t, unsigned char *);     
USB_STATUS _usb_host_ch9_get_status(_usb_device_instance_handle, uint8_t, uint8_t, unsigned char *);     
USB_STATUS _usb_host_ch9_set_address(_usb_device_instance_handle);     
USB_STATUS _usb_host_ch9_set_configuration(_usb_device_instance_handle, uint16_t);     
USB_STATUS _usb_host_ch9_set_descriptor(_usb_device_instance_handle, uint16_t, uint16_t, uint16_t, unsigned char *);     
USB_STATUS _usb_host_ch9_set_feature(_usb_device_instance_handle, uint8_t, uint8_t, uint16_t);
USB_STATUS _usb_host_ch9_set_interface(_usb_device_instance_handle, uint8_t, uint8_t);
USB_STATUS _usb_host_ch9_synch_frame(_usb_device_instance_handle, uint8_t, unsigned char *);
USB_STATUS _usb_hostdev_cntrl_request(_usb_device_instance_handle, USB_SETUP_PTR, unsigned char *, tr_callback, void *);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
