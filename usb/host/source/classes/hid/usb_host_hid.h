#ifndef __usb_host_hid_h__
#define __usb_host_hid_h__
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
*   This file defines a template structure for Class Drivers.
*
*
*END************************************************************************/
#include <mqx.h>
#include <lwevent.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_ch9.h"

#define LONG_ITEM                      0xFE
#define SHORT_ITEM                     0x03

#define MAIN_INPUT                     0x80
#define MAIN_OUTPUT                    0x90
#define MAIN_FEATURE                   0xB0
#define MAIN_COLLECTION                0xA0
#define MAIN_END_COLLECTION            0xC0

#define GLOBAL_USAGE_PAGE              0x04
#define GLOBAL_LOGICAL_MIN             0x14
#define GLOBAL_LOGICAL_MAX             0x24
#define GLOBAL_PHYSICAL_MIN            0x34
#define GLOBAL_PHYSICAL_MAX            0x44
#define GLOBAL_UNIT_COMPONENT          0x54
#define GLOBAL_UNIT                    0x64
#define GLOBAL_REPORT_SIZE             0x74
#define GLOBAL_REPORT_ID               0x84
#define GLOBAL_REPORT_COUNT            0x94
#define GLOBAL_PUSH                    0xA4
#define GLOBAL_POP                     0xB4

#define LOCAL_USAGE                    0x08
#define LOCAL_USAGE_MINIMUM            0x18
#define LOCAL_USAGE_MAXIMUM            0x28
#define LOCAL_DESIGNATOR_INDEX         0x38
#define LOCAL_DESIGNATOR_MIN           0x48
#define LOCAL_DESIGNATOR_MAX           0x58
#define LOCAL_STRING_INDEX             0x78
#define LOCAL_STRING_MIN               0x88
#define LOCAL_STRING_MAX               0x98
#define LOCAL_SET_DELIMITER            0xA8


#define USAGE_PAGE_UNDEFINED           0x00
#define USAGE_PAGE_GENERIC_DESKTOP     0x01
#define USAGE_PAGE_KEYBOARD            0x07
#define USAGE_PAGE_LEDS                0x08
#define USAGE_PAGE_BUTTONS             0x09

#define DESKTOP_USAGE_POINTER          0x01
#define DESKTOP_USAGE_MOUSE            0x02
#define DESKTOP_USAGE_KEYBOARD         0x06
#define DESKTOP_USAGE_X                0x30
#define DESKTOP_USAGE_Y                0x31
#define DESKTOP_USAGE_Z                0x32
#define DESKTOP_USAGE_WHEEL            0x38

#define INPUT_DATA_CONSTBIT            0x01
#define INPUT_DATA_VAR_BIT             0x02
#define INPUT_ABS_RELATIVE_BIT         0x04
#define INPUT_NO_WRAP_BIT              0x08
#define INPUT_NON_LINEAR_BIT           0x10
#define INPUT_PREF_STATE_NO_PREF_BIT   0x20
#define INPUT_NO_NULL_STATE_BIT        0x40
#define INPUT_BIT_FIELD_BUF_BYTES_BIT  0x100

#define GET_REPORT                     0x01
#define GET_IDLE                       0x02
#define GET_PROTOCOL                   0x03
#define SET_REPORT                     0x09
#define SET_IDLE                       0x0A
#define SET_PROTOCOL                   0x0B

/* structure for HID class descriptor */
typedef struct _HID_DESCRIPTOR_STRUCT {
    uint8_t     BLENGTH;
    uint8_t     BDESCRIPTORTYPE;
    uint8_t     BCDHID[2];
    uint8_t     BCOUNTRYCODE;
    uint8_t     BNUMDESCRIPTORS;
    uint8_t     BCLASSDESCRIPTORTYPE;
    uint8_t     BDESCRIPTORLENGTH[2];
} HID_DESCRIPTOR_STRUCT, * HID_DESCRIPTOR_STRUCT_PTR;


/*
** HID Class Interface structure. This structure will be passed to
** all commands to this class driver.
*/

typedef struct {
   /* Each class must start with a GENERAL_CLASS struct */
   GENERAL_CLASS                             G;

   /* Only 1 command can be issued at one time */
   bool                                   IN_SETUP;

   /* Here we store callback and parameter from higher level */
   tr_callback                               USER_CALLBACK;
   void                                     *USER_PARAM;

} USB_HID_CLASS_INTF_STRUCT, * USB_HID_CLASS_INTF_STRUCT_PTR;


typedef struct {
   CLASS_CALL_STRUCT_PTR   CLASS_PTR;
   tr_callback             CALLBACK_FN;
   void                   *CALLBACK_PARAM;
} HID_COMMAND, * HID_COMMAND_PTR;


/* Class specific functions exported by HID class driver */
#ifdef __cplusplus
extern "C" {
#endif

extern void usb_class_hid_init(PIPE_BUNDLE_STRUCT_PTR, CLASS_CALL_STRUCT_PTR);
extern USB_STATUS usb_class_hid_get_idle(HID_COMMAND_PTR, uint8_t, uint8_t *);
extern USB_STATUS usb_class_hid_get_protocol(HID_COMMAND_PTR, unsigned char *);
extern USB_STATUS usb_class_hid_set_protocol(HID_COMMAND_PTR, uint8_t);
extern USB_STATUS usb_class_hid_get_report(HID_COMMAND_PTR, uint8_t, uint8_t, void *, uint16_t);
extern USB_STATUS usb_class_hid_set_idle(HID_COMMAND_PTR, uint8_t, uint8_t);
extern USB_STATUS usb_class_hid_set_report(HID_COMMAND_PTR, uint8_t, uint8_t,
   void *, uint16_t);

#ifdef __cplusplus
}
#endif

#endif
