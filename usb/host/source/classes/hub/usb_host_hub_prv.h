#ifndef __usb_host_hub_prv_h__
#define __usb_host_hub_prv_h__
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
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_ch9.h"
#include "host_rcv.h"
#include "host_dev_list.h"

#define  C_HUB_LOCAL_POWER          (0)
#define  C_HUB_OVER_CURRENT         (1)
#define  PORT_CONNECTION            (0)
#define  PORT_ENABLE                (1)
#define  PORT_SUSPEND               (2)
#define  PORT_OVER_CURRENT          (3)
#define  PORT_RESET                 (4)
#define  PORT_POWER                 (8)
#define  PORT_LOW_SPEED             (9)
#define  PORT_HIGH_SPEED            (10)
#define  C_PORT_CONNECTION          (16)
#define  C_PORT_ENABLE              (17)
#define  C_PORT_SUSPEND             (18)
#define  C_PORT_OVER_CURRENT        (19)
#define  C_PORT_RESET               (20)
#define  C_PORT_POWER               (24)
#define  PORT_TEST                  (21)
#define  PORT_INDICATOR             (22)

/* structure for HUB class descriptor */
typedef struct {
    uint8_t     BLENGTH;
    uint8_t     BDESCRIPTORTYPE;
    uint8_t     BNRPORTS;
    uint8_t     WHUBCHARACTERISTICS[2];
    uint8_t     BPWRON2PWRGOOD;
    uint8_t     BHUBCONTRCURRENT;
    uint8_t     DEVICEREMOVABLE[];
/* not used: */
/*    uint8_t     PORTPOWERCTRLMASK; */
} HUB_DESCRIPTOR_STRUCT, * HUB_DESCRIPTOR_STRUCT_PTR;

typedef struct {
    uint16_t    WHUBSTATUS;
    uint16_t    WHUBCHANGE;
} HUB_STATUS_STRUCT, * HUB_STATUS_STRUCT_PTR;

/* Class specific functions exported by hub class driver */
#ifdef __cplusplus
extern "C" {
#endif

USB_STATUS usb_class_hub_get_app(_usb_device_instance_handle, _usb_interface_descriptor_handle, CLASS_CALL_STRUCT_PTR *);
USB_STATUS usb_class_hub_cntrl_common(CLASS_CALL_STRUCT_PTR, tr_callback, void *, uint8_t, uint8_t, uint16_t, uint16_t, uint16_t);
void usb_class_hub_cntrl_callback(void *, void *, unsigned char *, uint32_t, USB_STATUS);
USB_STATUS usb_class_hub_get_descriptor(CLASS_CALL_STRUCT_PTR, tr_callback, void *, unsigned char);
USB_STATUS usb_class_hub_set_port_feature(CLASS_CALL_STRUCT_PTR, tr_callback, void *, uint8_t, uint8_t);
USB_STATUS usb_class_hub_clear_feature(CLASS_CALL_STRUCT_PTR, tr_callback, void *, uint8_t);
USB_STATUS usb_class_hub_clear_port_feature(CLASS_CALL_STRUCT_PTR, tr_callback, void *, uint8_t, uint8_t);
USB_STATUS usb_class_hub_get_status(CLASS_CALL_STRUCT_PTR, tr_callback, void *, unsigned char);
USB_STATUS usb_class_hub_get_port_status(CLASS_CALL_STRUCT_PTR, tr_callback, void *, uint8_t, unsigned char);

USB_STATUS usb_class_hub_wait_for_interrupt(CLASS_CALL_STRUCT_PTR, tr_callback, void *, uint8_t);
void usb_class_hub_int_callback(void *, void *, unsigned char *, uint32_t, USB_STATUS);

#ifdef __cplusplus
}
#endif

#endif
