#ifndef __mqx_dev_h__
#define __mqx_dev_h__
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
* Description:      
*  This file contains device-specifc defines for MQX RTOS.
*                                                               
*
*END************************************************************************/

#include <mqx.h>
#include <psp.h>
#include <bsp.h>
#include "devapi.h"

/*
**
** Low-level function list structure for USB
**
** This is the structure used to store chip specific functions to be called 
** by generic layer
*/
struct xd_struct;

typedef struct usb_dev_callback_functions_struct
{
   /* The Host/Device init function */
   USB_STATUS (_CODE_PTR_ DEV_PREINIT)(_usb_device_handle *);

   /* The Host/Device init function */
   USB_STATUS (_CODE_PTR_ DEV_INIT)(_usb_device_handle);

   /* The function to send data */
   USB_STATUS (_CODE_PTR_ DEV_SEND)(_usb_device_handle, struct xd_struct *);

   /* The function to receive data */
   USB_STATUS (_CODE_PTR_ DEV_RECV)(_usb_device_handle, struct xd_struct *);
   
   /* The function to cancel the transfer */
   USB_STATUS (_CODE_PTR_ DEV_CANCEL_TRANSFER)(_usb_device_handle, uint8_t, uint8_t);
   
   USB_STATUS (_CODE_PTR_ DEV_INIT_ENDPOINT)(_usb_device_handle, struct xd_struct *);
   
   USB_STATUS (_CODE_PTR_ DEV_DEINIT_ENDPOINT)(_usb_device_handle, uint8_t, uint8_t);
   
   USB_STATUS (_CODE_PTR_ DEV_UNSTALL_ENDPOINT)(_usb_device_handle, uint8_t, uint8_t);
   
   USB_STATUS (_CODE_PTR_ DEV_GET_ENDPOINT_STATUS)(_usb_device_handle, uint8_t, uint16_t *);
   
   USB_STATUS (_CODE_PTR_ DEV_SET_ENDPOINT_STATUS)(_usb_device_handle, uint8_t, uint16_t);

   USB_STATUS (_CODE_PTR_ DEV_GET_TRANSFER_STATUS)(_usb_device_handle, uint8_t, uint8_t);
   
   USB_STATUS (_CODE_PTR_ DEV_SET_ADDRESS)(_usb_device_handle, uint8_t);
   
   USB_STATUS (_CODE_PTR_ DEV_SHUTDOWN)(_usb_device_handle);
   
   USB_STATUS (_CODE_PTR_ DEV_GET_SETUP_DATA)(_usb_device_handle, uint8_t, uint8_t *);
   
   USB_STATUS (_CODE_PTR_ DEV_ASSERT_RESUME)(_usb_device_handle);
   
   USB_STATUS (_CODE_PTR_ DEV_STALL_ENDPOINT)(_usb_device_handle, uint8_t, uint8_t);

} USB_DEV_CALLBACK_FUNCTIONS_STRUCT, * USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR;

#define USB_uint_16_low(x)          ((x) & 0xFF)
#define USB_uint_16_high(x)         (((x) >> 8) & 0xFF)

#ifdef __cplusplus
extern "C" {
#endif
extern uint8_t _usb_dev_driver_install(uint32_t, void *);
extern uint8_t _usb_dev_driver_uninstall(void);
#ifdef __cplusplus
}
#endif

#endif
