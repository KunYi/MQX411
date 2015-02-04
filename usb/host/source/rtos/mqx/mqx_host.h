#ifndef __mqx_host_h__
#define __mqx_host_h__
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
*   This file contains generic and OHCI-specific implementations of USB
*   interfaces
*
*
*END************************************************************************/

#include "host_common.h"
#include "usb_mqx.h"
#include "usb_bsp.h"

/*
**
** Low-level function list structure for USB
**
** This is the structure used to store chip specific functions to be called 
** by generic layer
*/
typedef struct usb_host_callback_functions_struct
{
   /* The Host/Device pre-init function */
   USB_STATUS (_CODE_PTR_ HOST_PREINIT)(_usb_host_handle *);

   /* The Host/Device init function */
   USB_STATUS (_CODE_PTR_ HOST_INIT)(_usb_host_handle);

   /* The function to shutdown the host/device */
   USB_STATUS (_CODE_PTR_ HOST_SHUTDOWN)(_usb_host_handle);

   /* The function to send data */
   USB_STATUS (_CODE_PTR_ HOST_SEND)(_usb_host_handle, struct pipe_struct *, struct tr_struct *);

   /* The function to send setup data */
   USB_STATUS (_CODE_PTR_ HOST_SEND_SETUP)(_usb_host_handle, struct pipe_struct *, struct tr_struct *);

   /* The function to receive data */
   USB_STATUS (_CODE_PTR_ HOST_RECV)(_usb_host_handle, struct pipe_struct *, struct tr_struct *);
   
   /* The function to cancel the transfer */
   USB_STATUS (_CODE_PTR_ HOST_CANCEL)(_usb_host_handle, struct pipe_struct *, struct tr_struct *);
   
   /* The function for USB bus control */
   USB_STATUS (_CODE_PTR_ HOST_BUS_CONTROL)(_usb_host_handle, uint8_t);

   USB_STATUS (_CODE_PTR_ HOST_ALLOC_BANDWIDTH)(_usb_host_handle, struct pipe_struct *);

   USB_STATUS (_CODE_PTR_ HOST_FREE_CONTROLLER_RESOURCE)(_usb_host_handle, struct pipe_struct *);
  
   uint32_t (_CODE_PTR_ HOST_GET_FRAME_NUM)(_usb_host_handle);   
   
   uint32_t (_CODE_PTR_ HOST_GET_MICRO_FRAME_NUM)(_usb_host_handle);   
   
   USB_STATUS (_CODE_PTR_ HOST_OPEN_PIPE)(_usb_host_handle, struct pipe_struct *);
   
   USB_STATUS (_CODE_PTR_ HOST_UPDATE_MAX_PACKET_SIZE)(_usb_host_handle, struct pipe_struct *);
   
   USB_STATUS (_CODE_PTR_ HOST_UPDATE_DEVICE_ADDRESS)(_usb_host_handle, struct pipe_struct *);

} USB_HOST_CALLBACK_FUNCTIONS_STRUCT, * USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
