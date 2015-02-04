#ifndef __types_h__
#define __types_h__
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
*   This file contains appropriate processor specific header file include.
*
*
*END************************************************************************/
#include "psptypes.h"

typedef uint32_t USB_STATUS;

/* forward declarations */
struct dev_instance;
struct usb_interface_descriptor;
struct pipe_struct;
struct tr_struct;

typedef void *_usb_device_handle;                /* device state struct */

typedef void *_usb_host_handle;                  /* host state struct   */
typedef void *_usb_device_instance_handle;       /* item on host's list */
typedef void *_usb_interface_descriptor_handle;  /* Chapter 9 interface descriptor */
typedef void *_usb_pipe_handle;                  /* pipe or pipe-bundle */
typedef void   *_usb_class_intf_handle;            /* interface state struct (general data + class driver specific data) */
typedef uint32_t descriptor_type;

#endif
