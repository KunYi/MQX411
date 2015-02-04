#ifndef __usb_host_hub_h__
#define __usb_host_hub_h__
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
*   This file defines a structure(s) for hub class driver.
*
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"

#include "hostapi.h"

#define  USB_CLASS_HUB              9
#define  USB_SUBCLASS_HUB_NONE      0
#define  USB_PROTOCOL_HUB_LS        0
#define  USB_PROTOCOL_HUB_FS        0
#define  USB_PROTOCOL_HUB_HS_SINGLE 1
#define  USB_PROTOCOL_HUB_HS_MULTI  2
#define  USB_PROTOCOL_HUB_ALL       0xFF

/*
** HUB Class Interface structure. This structure will be passed to
** all commands to this class driver.
*/
typedef struct {
   /* Each class must start with a GENERAL_CLASS struct */
   GENERAL_CLASS                             G;
   /* application handle */
   CLASS_CALL_STRUCT_PTR                     APP;

   /* Pipes */
   _usb_pipe_handle                          P_CONTROL;
   _usb_pipe_handle                          P_INT_IN;
   /* Is command used? (only 1 command can be issued at one time) */
   bool                                   IN_SETUP;
   /* Only 1 interrupt pipe can be opened at one time */
   bool                                   INT_PIPE;

   /* Higher level callback and parameter */
   tr_callback                               CTRL_CALLBACK;
   void                                     *CTRL_PARAM;

   /* Higher level callback and parameter */
   tr_callback                               INT_CALLBACK;
   void                                     *INT_PARAM;

   uint32_t                                   C_BUF_SIZE;
   uint32_t                                   I_BUF_SIZE;
   /* control pipe buffer pointer, allocated in memlist */
   uint8_t                                *C_BUF;

   /* interrupt pipe buffer pointer, allocated in memlist */
   uint8_t                                *I_BUF;

} USB_HUB_CLASS_INTF_STRUCT, * USB_HUB_CLASS_INTF_STRUCT_PTR;

/* Class specific functions exported by hub class driver */
#ifdef __cplusplus
extern "C" {
#endif

extern void usb_class_hub_init(PIPE_BUNDLE_STRUCT_PTR, CLASS_CALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
