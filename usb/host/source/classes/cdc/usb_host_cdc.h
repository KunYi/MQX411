#ifndef __usb_host_cdc_h__
#define __usb_host_cdc_h__
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

#include <fio.h>

#include <bsp.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_rcv.h"
#include "host_ch9.h"
#include "host_snd.h"

#define USB_CDC_SET_LINE_CODING              0x20
#define USB_CDC_GET_LINE_CODING              0x21
typedef struct {
    uint32_t      baudrate;
    uint8_t       stopbits; /* 1 ~ 1bit, 2 ~ 2bits, 3 ~ 1.5bit */
    uint8_t       parity;   /* 1 ~ even, -1 ~ odd, 0 ~ no parity */
    uint8_t       databits;
} USB_CDC_UART_CODING, * USB_CDC_UART_CODING_PTR;

/* set ouput pin state */
#define USB_CDC_SET_CTRL_LINE_STATE          0x22
typedef struct {
#define USB_ACM_LINE_STATE_DTR               0x01
#define USB_ACM_LINE_STATE_RTS               0x02
    uint16_t      state;
} USB_CDC_CTRL_STATE, * USB_CDC_CTRL_STATE_PTR;

/* receive interrupt state */
typedef struct {
    uint8_t       reservedA[8];
#define USB_ACM_STATE_RX_CARRIER             0x01
#define USB_ACM_STATE_TX_CARRIER             0x02
#define USB_ACM_STATE_BREAK                  0x04
#define USB_ACM_STATE_RING_SIGNAL            0x08
#define USB_ACM_STATE_FRAMING                0x10
#define USB_ACM_STATE_PARITY                 0x20
#define USB_ACM_STATE_OVERRUN                0x40
    uint8_t       bmStates;
    uint8_t       reservedB[1];
} USB_CDC_ACM_STATE, * USB_CDC_ACM_STATE_PTR;

#define USB_DESC_SUBTYPE_CS_HEADER           0x00
typedef struct {
    uint8_t   bFunctionLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bcdCDC[2];
} USB_CDC_DESC_HEADER, * USB_CDC_DESC_HEADER_PTR;

#define USB_DESC_SUBTYPE_CS_CM               0x01
typedef struct {
    uint8_t   bFunctionLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
#define USB_ACM_CM_CAP_HANDLE_MANAGEMENT     0x01
#define USB_ACM_CM_CAP_DATA_CLASS            0x02
    uint8_t   bmCapabilities;
    uint8_t   bDataInterface;
} USB_CDC_DESC_CM, * USB_CDC_DESC_CM_PTR;

#define USB_DESC_SUBTYPE_CS_ACM              0x02
typedef struct {
    uint8_t   bFunctionLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
#define USB_ACM_CAP_COMM_FEATURE             0x01
#define USB_ACM_CAP_LINE_CODING              0x02
#define USB_ACM_CAP_SEND_BREAK               0x04
#define USB_ACM_CAP_NET_NOTIFY               0x08
    uint8_t   bmCapabilities;
} USB_CDC_DESC_ACM, * USB_CDC_DESC_ACM_PTR;

#define USB_DESC_SUBTYPE_CS_DL               0x03
typedef struct {
    uint8_t   bFunctionLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bmCapabilities;
} USB_CDC_DESC_DL, * USB_CDC_DESC_DL_PTR;

#define USB_DESC_SUBTYPE_CS_TR               0x04
typedef struct {
    uint8_t   bFunctionLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bRingerVolSteps;
    uint8_t   bNumRingerPatterns;
} USB_CDC_DESC_TR, * USB_CDC_DESC_TR_PTR;

#define USB_DESC_SUBTYPE_CS_TCS              0x05
typedef struct {
    uint8_t   bFunctionLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bmCapabilities[4];
} USB_CDC_DESC_TCS, * USB_CDC_DESC_TCS_PTR;

#define USB_DESC_SUBTYPE_CS_UNION            0x06
typedef struct {
    uint8_t   bFunctionLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bMasterInterface;
    uint8_t   bSlaveInterface[];
} USB_CDC_DESC_UNION, * USB_CDC_DESC_UNION_PTR;

#define USB_DESC_SUBTYPE_CS_TOM              0x08
typedef struct {
    uint8_t   bFunctionLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bmCapabilities;
} USB_CDC_DESC_TOM, * USB_CDC_DESC_TOM_PTR;

typedef union {
    USB_CDC_DESC_HEADER         header;
    USB_CDC_DESC_CM             cm;
    USB_CDC_DESC_ACM            acm;
    USB_CDC_DESC_DL             dl;
    USB_CDC_DESC_TR             tr;
    USB_CDC_DESC_TCS            tcs;
    USB_CDC_DESC_TOM            tom;
    USB_CDC_DESC_UNION          uni;
} USB_CDC_FUNC_DESC, * USB_CDC_FUNC_DESC_PTR;

/*
** CDC Class Interface structure. This structure will be passed to
** all commands to this class driver.
*/
typedef struct usb_cdc_general_class {
    /* Each class must start with a GENERAL_CLASS struct */
    GENERAL_CLASS                             G;

    /* Higher level callback and parameter */
    tr_callback                               USER_CALLBACK;
    void                                     *USER_PARAM;

    uint8_t                                    IFNUM;

} USB_CDC_GENERAL_CLASS, * USB_CDC_GENERAL_CLASS_PTR;

typedef struct {
    /* Each CDC subclass must start with a USB_CDC_GENERAL_CLASS struct */
    USB_CDC_GENERAL_CLASS                     CDC_G;
    USB_CDC_DESC_ACM_PTR                      acm_desc;
    USB_CDC_DESC_CM_PTR                       cm_desc;
    USB_CDC_DESC_HEADER_PTR                   header_desc;
    USB_CDC_DESC_UNION_PTR                    union_desc;
   
    USB_CDC_UART_CODING                       uart_coding;

    _usb_pipe_handle                          interrupt_pipe;
    USB_CDC_ACM_STATE                         interrupt_buffer;
    
    USB_CDC_CTRL_STATE                        ctrl_state;

#define USB_ACM_DETACH           0x01
#define USB_ACM_CTRL_PIPE_FREE   0x02
#define USB_ACM_INT_PIPE_FREE    0x04
    LWEVENT_STRUCT_PTR                        acm_event;

} USB_ACM_CLASS_INTF_STRUCT, * USB_ACM_CLASS_INTF_STRUCT_PTR;

typedef struct {
    /* Each CDC subclass must start with a USB_CDC_GENERAL_CLASS struct */
    USB_CDC_GENERAL_CLASS                 CDC_G;
    /* Address of bound control interface */
    CLASS_CALL_STRUCT_PTR                 BOUND_CONTROL_INTERFACE;

    char                                  *RX_BUFFER;
    char                                  *RX_BUFFER_DRV;
    char                                  *RX_BUFFER_APP;
    uint32_t                              RX_BUFFER_SIZE;
    uint32_t                              RX_READ;
    
    uint32_t                              TX_SENT;

    PIPE_STRUCT_PTR                       in_pipe;
    PIPE_STRUCT_PTR                       out_pipe;
   
    char                                  *device_name;

#define USB_DATA_DETACH            0x01
#define USB_DATA_READ_COMPLETE     0x02
#define USB_DATA_READ_PIPE_FREE    0x04
#define USB_DATA_SEND_COMPLETE     0x08
#define USB_DATA_SEND_PIPE_FREE    0x10
    LWEVENT_STRUCT_PTR                    data_event;

} USB_DATA_CLASS_INTF_STRUCT, * USB_DATA_CLASS_INTF_STRUCT_PTR;

typedef struct {
    CLASS_CALL_STRUCT_PTR   CLASS_PTR;
    tr_callback             CALLBACK_FN;
    void                   *CALLBACK_PARAM;
} CDC_COMMAND, * CDC_COMMAND_PTR;

typedef struct cdc_serial_init {
#define USB_UART_BLOCKING          (0x0000)
#define USB_UART_NO_BLOCKING       (0x0001)
/* NOTE!!!
** HW control flow has one weakness: we dont know exactly when DCD signal got log. 0 or
** log.1. This is because interrupts pipe is polled in time intervals in some milliseconds.
** HW flow control is here only just for compatibility purposes.
** As for output pins, we set always DTR and RTS signal for HW flow control. The user can
** still use usb_class_cdc_set_acm_ctrl_state to control those signals manually.
** Note also, that additional HW control flow is done on USB,
** which implies from USB specification (NAK responding).
*/
#define USB_UART_HW_FLOW           (0x0002)
    _mqx_uint               FLAGS;
} CDC_SERIAL_INIT, * CDC_SERIAL_INIT_PTR;

/* Class specific functions exported by CDC class driver */
#ifdef __cplusplus
extern "C" {
#endif

extern void usb_class_cdc_acm_init(PIPE_BUNDLE_STRUCT_PTR, CLASS_CALL_STRUCT_PTR);
extern USB_STATUS usb_class_cdc_acm_use_lwevent(CLASS_CALL_STRUCT_PTR, LWEVENT_STRUCT_PTR);

extern void usb_class_cdc_data_init(PIPE_BUNDLE_STRUCT_PTR, CLASS_CALL_STRUCT_PTR);
extern USB_STATUS usb_class_cdc_data_use_lwevent(CLASS_CALL_STRUCT_PTR, LWEVENT_STRUCT_PTR);

extern CLASS_CALL_STRUCT_PTR usb_class_cdc_get_ctrl_interface(void *);
extern CLASS_CALL_STRUCT_PTR usb_class_cdc_get_data_interface(void *);
extern USB_STATUS usb_class_cdc_get_acm_line_coding(CLASS_CALL_STRUCT_PTR, USB_CDC_UART_CODING_PTR);
extern USB_STATUS usb_class_cdc_set_acm_line_coding(CLASS_CALL_STRUCT_PTR, USB_CDC_UART_CODING_PTR);
extern USB_STATUS usb_class_cdc_set_acm_ctrl_state(CLASS_CALL_STRUCT_PTR, uint8_t, uint8_t);
extern USB_STATUS usb_class_cdc_get_acm_descriptors(_usb_device_instance_handle, _usb_interface_descriptor_handle, USB_CDC_DESC_ACM_PTR *, USB_CDC_DESC_CM_PTR *, USB_CDC_DESC_HEADER_PTR *, USB_CDC_DESC_UNION_PTR *);
extern USB_STATUS usb_class_cdc_set_acm_descriptors(CLASS_CALL_STRUCT_PTR, USB_CDC_DESC_ACM_PTR, USB_CDC_DESC_CM_PTR, USB_CDC_DESC_HEADER_PTR, USB_CDC_DESC_UNION_PTR);
extern USB_STATUS usb_class_cdc_get_ctrl_descriptor(_usb_device_instance_handle, _usb_interface_descriptor_handle, INTERFACE_DESCRIPTOR_PTR *);
extern USB_STATUS usb_class_cdc_bind_data_interfaces(_usb_device_instance_handle, CLASS_CALL_STRUCT_PTR);
extern USB_STATUS usb_class_cdc_unbind_data_interfaces(CLASS_CALL_STRUCT_PTR);
extern USB_STATUS usb_class_cdc_bind_acm_interface(CLASS_CALL_STRUCT_PTR, INTERFACE_DESCRIPTOR_PTR);
extern USB_STATUS usb_class_cdc_unbind_acm_interface(CLASS_CALL_STRUCT_PTR);
#if MQX_USE_IO_OLD
extern USB_STATUS usb_class_cdc_init_ipipe(CLASS_CALL_STRUCT_PTR);
extern USB_STATUS usb_class_cdc_install_driver(CLASS_CALL_STRUCT_PTR, char *);
extern USB_STATUS usb_class_cdc_uninstall_driver(CLASS_CALL_STRUCT_PTR);
#endif //MQX_USE_IO_OLD

#ifdef __cplusplus
}
#endif

#endif
