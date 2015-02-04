#ifndef __host_main_h__
#define __host_main_h__
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
#include "hostapi.h"
#include "mqx_host.h"

typedef struct usb_host_generic_structure {
   void                               *INIT_PARAM;
   void                               *DEV_PTR;
   uint8_t                              DEV_NUM; //deprecated, should not be used
   uint32_t                             SPEED;
   const USB_HOST_CALLBACK_FUNCTIONS_STRUCT * CALLBACK_STRUCT_PTR;
   void                               *DEVICE_LIST_PTR;
   struct pipe_struct                  * PIPE_DESCRIPTOR_BASE_PTR;
   struct host_service_struct          * SERVICE_HEAD_PTR;
   struct driver_info                  * DEVICE_INFO_TABLE;
   uint32_t                             PIPE_SIZE;
   uint32_t                             TR_SIZE;
} USB_HOST_STATE_STRUCT, * USB_HOST_STATE_STRUCT_PTR;

/* List of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

USB_STATUS _usb_host_init_call_interface (_usb_host_handle);
USB_STATUS _usb_host_shutdown_call_interface (_usb_host_handle);
USB_STATUS _usb_host_send_call_interface (_usb_host_handle, struct pipe_struct *, struct tr_struct *);
USB_STATUS _usb_host_send_setup_call_interface (_usb_host_handle, struct pipe_struct *, struct tr_struct *);
USB_STATUS _usb_host_recv_data_call_interface (_usb_host_handle, struct pipe_struct  *pipe_descr_ptr, struct tr_struct *);
USB_STATUS _usb_host_cancel_call_interface (_usb_host_handle, struct pipe_struct *, struct tr_struct *);
USB_STATUS _usb_host_bus_control_data_call_interface (_usb_host_handle, uint8_t);
USB_STATUS _usb_host_alloc_bandwidth_call_interface (_usb_host_handle, struct pipe_struct *);
USB_STATUS _usb_host_close_pipe_call_interface (_usb_host_handle, struct pipe_struct *);
USB_STATUS _usb_host_open_pipe_call_interface (_usb_host_handle, struct pipe_struct *);
USB_STATUS _usb_host_update_max_packet_size_call_interface (_usb_host_handle, struct pipe_struct *);
USB_STATUS _usb_host_update_device_address_call_interface (_usb_host_handle, struct pipe_struct *);
//USB_STATUS _usb_host_iso_packet_desc_pool_create (uint32_t);
//void _usb_host_iso_packet_desc_pool_destroy (void);
//USB_STATUS _usb_host_iso_packet_desc_list_alloc (uint32_t, USB_ISO_PACKET_DESC_LIST_STRUCT_PTR);
//void _usb_host_iso_packet_desc_list_free (USB_ISO_PACKET_DESC_LIST_STRUCT_PTR);
USB_STATUS _usb_host_init (USB_HOST_IF_STRUCT_PTR, _usb_host_handle *);
USB_STATUS _usb_host_register_service (_usb_host_handle, uint8_t, void(_CODE_PTR_ service)(void *, uint32_t));
USB_STATUS _usb_host_call_service (_usb_host_handle, uint8_t, uint32_t);
USB_STATUS _usb_host_check_service (_usb_host_handle, uint8_t, uint32_t);
USB_STATUS _usb_host_unregister_service (_usb_host_handle, uint8_t);
USB_STATUS _usb_host_open_pipe (_usb_host_handle, PIPE_INIT_PARAM_STRUCT_PTR, _usb_pipe_handle *);
struct tr_struct      *_usb_host_get_tr_element (_usb_pipe_handle, uint32_t);
USB_STATUS  _usb_host_set_up_tr (_usb_pipe_handle, TR_INIT_PARAM_STRUCT_PTR, struct tr_struct  **);
uint32_t _usb_host_get_transfer_status (_usb_pipe_handle, uint32_t);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
