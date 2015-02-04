#ifndef __hostapi_h__
#define __hostapi_h__ 1
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
*   This file contains the USB Host API specific data structures and
*   function definitions.
*
*
*END************************************************************************/
#include "usb.h"
#include "host_common.h"
#include "host_main.h"
#include "host_dev_list.h"
#include "host_ch9.h"
#include "host_rcv.h"
#include "host_snd.h"
#include "host_cnl.h"
#include "host_close.h"
#include "host_shut.h"

/* Available service types */
#define  USB_SERVICE_HOST_RESUME             (0x01)
#define  USB_SERVICE_SYSTEM_ERROR            (0x02)
#define  USB_SERVICE_SPEED                   (0x03)
#define  USB_SERVICE_ATTACH                  (0x04)
#define  USB_SERVICE_DETACH                  (0x05)
#define  USB_SERVICE_STALL_PACKET            (0x06)
#define  USB_SERVICE_ENUMERATION_DONE        (0x07)
#define  USB_SERVICE_1MS_TIMER               (0x08)

/* Used to manage services and service callbacks */
typedef struct host_service_struct
{
   struct host_service_struct      *NEXT;
   uint32_t                          TYPE;
   void                 (_CODE_PTR_ SERVICE)(void *, uint32_t);
} USB_SERVICE_STRUCT, * USB_SERVICE_STRUCT_PTR;

/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
