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
*   This file contains USB Device API defines for state and function
*   returns.
*
*
*END************************************************************************/
#ifndef __usb_error_h__
#define __usb_error_h__ 1

#include "usb_types.h"

/* Error codes */
#define USB_OK                              (0x00)
#define USBERR_ALLOC                        (0x81)
#define USBERR_BAD_STATUS                   (0x82)
#define USBERR_CLOSED_SERVICE               (0x83)
#define USBERR_OPEN_SERVICE                 (0x84)
#define USBERR_TRANSFER_IN_PROGRESS         (0x85)
#define USBERR_ENDPOINT_STALLED             (0x86)
#define USBERR_ALLOC_STATE                  (0x87)
#define USBERR_DRIVER_INSTALL_FAILED        (0x88)
#define USBERR_DRIVER_NOT_INSTALLED         (0x89)
#define USBERR_INSTALL_ISR                  (0x8A)
#define USBERR_INVALID_DEVICE_NUM           (0x8B)
#define USBERR_ALLOC_SERVICE                (0x8C)
#define USBERR_INIT_FAILED                  (0x8D)
#define USBERR_SHUTDOWN                     (0x8E)
#define USBERR_INVALID_PIPE_HANDLE          (0x8F)
#define USBERR_OPEN_PIPE_FAILED             (0x90)
#define USBERR_INIT_DATA                    (0x91)
#define USBERR_SRP_REQ_INVALID_STATE        (0x92)
#define USBERR_TX_FAILED                    (0x93)
#define USBERR_RX_FAILED                    (0x94)
#define USBERR_EP_INIT_FAILED               (0x95)
#define USBERR_EP_DEINIT_FAILED             (0x96)
#define USBERR_TR_FAILED                    (0x97)
#define USBERR_BANDWIDTH_ALLOC_FAILED       (0x98)
#define USBERR_INVALID_NUM_OF_ENDPOINTS     (0x99)
#define USBERR_ADDRESS_ALLOC_FAILED         (0x9A)
#define USBERR_PIPE_CLOSED                  (0x9B)
#define USBERR_NOT_FOUND                    (0x9C)

#define USBERR_DEVICE_NOT_FOUND             (0xC0)
#define USBERR_DEVICE_BUSY                  (0xC1)
#define USBERR_NO_DEVICE_CLASS              (0xC3)
#define USBERR_UNKNOWN_ERROR                (0xC4)
#define USBERR_INVALID_BMREQ_TYPE           (0xC5)
#define USBERR_GET_MEMORY_FAILED            (0xC6)
#define USBERR_BAD_ALIGNMENT                (0xC7)
#define USBERR_INVALID_MEM_TYPE             (0xC8)
#define USBERR_NO_DESCRIPTOR                (0xC9)
#define USBERR_NULL_CALLBACK                (0xCA)
#define USBERR_NO_INTERFACE                 (0xCB)
#define USBERR_INVALID_CFIG_NUM             (0xCC)
#define USBERR_INVALID_ANCHOR               (0xCD)
#define USBERR_INVALID_REQ_TYPE             (0xCE)
#define USBERR_ERROR                        (0xFF)

/* Error Codes for lower-layer */
#define USBERR_ALLOC_EP_QUEUE_HEAD          (0xA8)
#define USBERR_ALLOC_TR                     (0xA9)
#define USBERR_ALLOC_DTD_BASE               (0xAA)
#define USBERR_CLASS_DRIVER_INSTALL         (0xAB)

#endif
