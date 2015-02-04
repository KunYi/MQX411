#ifndef __ehci_bw_h__
#define __ehci_bw_h__
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

#ifdef __cplusplus
extern "C" {
#endif

extern USB_STATUS _usb_ehci_calculate_uframe_tr_time (uint32_t, uint8_t);
extern uint32_t _usb_ehci_get_frame_number (_usb_host_handle);
extern uint32_t _usb_ehci_get_micro_frame_number (_usb_host_handle);
extern USB_STATUS _usb_ehci_allocate_bandwidth (_usb_host_handle, PIPE_STRUCT_PTR);
extern void unlink_periodic_data_structure_from_frame (volatile uint32_t *, volatile uint32_t *);
extern void reclaim_band_width (_usb_host_handle, uint32_t, volatile uint32_t *, EHCI_PIPE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
