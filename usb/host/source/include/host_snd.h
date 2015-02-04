#ifndef __host_snd_h__
#define __host_snd_h__
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
#include "host_common.h"

/* List of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

uint32_t _usb_host_get_frame_number (_usb_host_handle);
uint32_t _usb_host_get_micro_frame_number (_usb_host_handle);
USB_STATUS _usb_host_send_data (_usb_host_handle, _usb_pipe_handle, TR_INIT_PARAM_STRUCT_PTR);
USB_STATUS _usb_host_send_setup (_usb_host_handle, _usb_pipe_handle, TR_INIT_PARAM_STRUCT_PTR);
   
#ifdef __cplusplus
}
#endif

#endif
/* EOF */
