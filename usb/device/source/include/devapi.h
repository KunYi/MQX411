#ifndef __devapi_h__
#define __devapi_h__ 1
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
*  This file contains the declarations specific to the USB Device API
*
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Informational Request/Set Types */
#define  USB_STATUS_DEVICE_STATE          (0x01)
#define  USB_STATUS_INTERFACE             (0x02)
#define  USB_STATUS_ADDRESS               (0x03)
#define  USB_STATUS_CURRENT_CONFIG        (0x04)
#define  USB_STATUS_SOF_COUNT             (0x05)
#define  USB_STATUS_DEVICE                (0x06)
#define  USB_STATUS_TEST_MODE             (0x07)
#define  USB_STATUS_ENDPOINT              (0x10)
#define  USB_STATUS_ENDPOINT_NUMBER_MASK  (0x0F)

#define  USB_TEST_MODE_TEST_PACKET        (0x0400)

/* Available service types */
/* Services 0 through 15 are reserved for endpoints */
#define  USB_SERVICE_EP0                  (0x00)
#define  USB_SERVICE_EP1                  (0x01)
#define  USB_SERVICE_EP2                  (0x02)
#define  USB_SERVICE_EP3                  (0x03)
#define  USB_SERVICE_BUS_RESET            (0x10)
#define  USB_SERVICE_SUSPEND              (0x11)
#define  USB_SERVICE_SOF                  (0x12)
#define  USB_SERVICE_RESUME               (0x13)
#define  USB_SERVICE_SLEEP                (0x14)
#define  USB_SERVICE_SPEED_DETECTION      (0x15)
#define  USB_SERVICE_ERROR                (0x16)
#define  USB_SERVICE_STALL                (0x17)

#define USB_CONTROL_ENDPOINT              (0)
#define USB_SETUP_PKT_SIZE                (8)/* Setup Packet Size */
#define  USB_UNINITIALIZED_VAL_32         (0xFFFFFFFF)
#define COMPONENT_PREPARE_SHIFT           (7)

/* Common Data Structures */
typedef struct _USB_SETUP_STRUCT
{
   uint8_t      request_type;
   uint8_t      request;
   uint16_t     value;
   uint16_t     index;
   uint16_t     length;
} USB_SETUP_STRUCT, * USB_SETUP_STRUCT_PTR;

typedef struct _USB_EP_STRUCT
{
  uint8_t          ep_num;      /* endpoint number         */
  uint8_t          type;        /* type of endpoint        */
  uint8_t          direction;   /* direction of endpoint   */
  uint32_t         size;        /* buffer size of endpoint */
}USB_EP_STRUCT;

typedef USB_EP_STRUCT* USB_EP_STRUCT_PTR;


typedef struct _USB_EVENT_STRUCT
{
  _usb_device_handle handle;          /* conttroler device handle*/
  uint8_t          ep_num;
  bool         setup;              /* is setup packet         */
  bool         direction;          /* direction of endpoint   */
  uint8_t      *buffer_ptr;         /* pointer to buffer       */
  uint32_t         len;                /* buffer size of endpoint */
} USB_EVENT_STRUCT, *PTR_USB_EVENT_STRUCT;

typedef struct {
   void                *PRIVATE;
   void (_CODE_PTR_     FREE)(_usb_device_handle, void *);
   void                *XD_FOR_THIS_DTD;
} SCRATCH_STRUCT, * SCRATCH_STRUCT_PTR;

/* Callback function storage structure */
typedef struct service_struct 
{
   uint8_t   TYPE;
   void     (_CODE_PTR_ SERVICE)(PTR_USB_EVENT_STRUCT,void *arg);
   void    *arg;
   struct service_struct        *NEXT;
} SERVICE_STRUCT, * SERVICE_STRUCT_PTR;

void _usb_device_free_XD(_usb_device_handle, void  *xd_ptr);
USB_STATUS _usb_device_init(struct usb_dev_if_struct *, _usb_device_handle *, _mqx_uint);
USB_STATUS _usb_device_deinit(_usb_device_handle);
USB_STATUS _usb_device_recv_data(_usb_device_handle, uint8_t, uint8_t *, uint32_t);
USB_STATUS _usb_device_send_data(_usb_device_handle, uint8_t, uint8_t *, uint32_t);
USB_STATUS _usb_device_get_transfer_status(_usb_device_handle, uint8_t, uint8_t);
USB_STATUS _usb_device_cancel_transfer(_usb_device_handle, uint8_t, uint8_t);
USB_STATUS _usb_device_get_status(_usb_device_handle, uint8_t, uint16_t *);
USB_STATUS _usb_device_set_status(_usb_device_handle, uint8_t, uint16_t);
USB_STATUS _usb_device_register_service(_usb_device_handle, uint8_t,
   void(_CODE_PTR_ service)(PTR_USB_EVENT_STRUCT, void *),void *arg);
USB_STATUS _usb_device_unregister_service(_usb_device_handle, uint8_t);
USB_STATUS _usb_device_call_service(uint8_t, PTR_USB_EVENT_STRUCT);
USB_STATUS _usb_device_shutdown(_usb_device_handle);
USB_STATUS _usb_device_set_address(_usb_device_handle, uint8_t);
USB_STATUS _usb_device_read_setup_data(_usb_device_handle, uint8_t, uint8_t *);
USB_STATUS _usb_device_assert_resume(_usb_device_handle);
USB_STATUS _usb_device_init_endpoint(_usb_device_handle, USB_EP_STRUCT_PTR, uint8_t);
USB_STATUS _usb_device_stall_endpoint(_usb_device_handle, uint8_t, uint8_t);
USB_STATUS _usb_device_unstall_endpoint(_usb_device_handle, uint8_t, uint8_t);
USB_STATUS _usb_device_deinit_endpoint(_usb_device_handle, uint8_t, uint8_t);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
