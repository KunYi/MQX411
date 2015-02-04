#ifndef __usb_host_hub_sm_h__
#define __usb_host_hub_sm_h__
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
*   This file defines structures used by the implementation of hub SM on host.
*
*
*END************************************************************************/
#include <mqx.h>
#include <lwevent.h>

#include "usb.h"
#include "hostapi.h"

/*
** Following structs contain all states and pointers
** used by the application to control/operate devices.
*/

typedef struct hub_device_port_struct {
/* status read from GetPortStatus transfer */
   uint16_t                          STATUS;
#define HUB_PORT_ATTACHED           (0x01) /* flag informs that application started attach process */
#define HUB_PORT_REMOVABLE          (0x02) /* flag informs that application started attach process */
   uint8_t                           APP_STATUS;
} HUB_PORT_STRUCT, * HUB_PORT_STRUCT_PTR;

typedef struct hub_device_struct {
   CLASS_CALL_STRUCT                CCS;
   /* below the hub specific data */
   enum {
      HUB_IDLE,
      HUB_GET_DESCRIPTOR_TINY_PROCESS,
      HUB_GET_DESCRIPTOR_PROCESS,
      HUB_SET_PORT_FEATURE_PROCESS,
      HUB_CLEAR_PORT_FEATURE_PROCESS,
      HUB_GET_PORT_STATUS_PROCESS,
      HUB_GET_PORT_STATUS_ASYNC,
      HUB_RESET_DEVICE_PORT_PROCESS,
      HUB_ADDRESS_DEVICE_PORT_PROCESS,
      HUB_DETACH_DEVICE_PORT_PROCESS,
      HUB_GET_STATUS_ASYNC,
      HUB_NONE
   }                                STATE;
   unsigned char                            HUB_PORT_NR;
   _mqx_int                         port_iterator;
   HUB_PORT_STRUCT_PTR              HUB_PORTS;
} HUB_DEVICE_STRUCT, * HUB_DEVICE_STRUCT_PTR;

/* List of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

void usb_host_hub_device_event(_usb_device_instance_handle, _usb_interface_descriptor_handle, uint32_t);

#ifdef __cplusplus
}
#endif

#endif
