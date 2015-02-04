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
#ifndef __usb_misc_h__
#define __usb_misc_h__ 1

/* Host specific */
#define USB_DEBOUNCE_DELAY                  (101)
#define USB_RESET_RECOVERY_DELAY            (11)
#define USB_RESET_DELAY                     (10)

/* Endpoint types */
#define USB_CONTROL_PIPE                    (0x00)
#define USB_ISOCHRONOUS_PIPE                (0x01)
#define USB_BULK_PIPE                       (0x02)
#define USB_INTERRUPT_PIPE                  (0x03)

/* USB device state- see USB 2.0 Documentation */
#define USB_STATE_UNKNOWN                   (0xff)
#define USB_STATE_PENDING_ADDRESS           (0x04) 
#define USB_STATE_POWERED                   (0x03)
#define USB_STATE_DEFAULT                   (0x02)
#define USB_STATE_ADDRESS                   (0x01)
#define USB_STATE_CONFIG                    (0x00)
#define USB_STATE_SUSPEND                   (0x80)

#define USB_SELF_POWERED                    (0x01)
#define USB_REMOTE_WAKEUP                   (0x02)

/* Bus Control values */
#define USB_NO_OPERATION                    (0x00)
#define USB_ASSERT_BUS_RESET                (0x01)
#define USB_DEASSERT_BUS_RESET              (0x02)
#define USB_ASSERT_RESUME                   (0x03)
#define USB_DEASSERT_RESUME                 (0x04)
#define USB_SUSPEND_SOF                     (0x05)
#define USB_RESUME_SOF                      (0x06)

/* possible values of XD->bStatus */
#define USB_STATUS_IDLE                     (0)
#define USB_STATUS_STALLED                  (1) 
#define USB_STATUS_TRANSFER_PENDING         (2)
#define USB_STATUS_TRANSFER_IN_PROGRESS     (3)
#define USB_STATUS_ERROR                    (4)
#define USB_STATUS_DISABLED                 (5)
#define USB_STATUS_TRANSFER_ACCEPTED        (6)
#define USB_STATUS_TRANSFER_QUEUED          (7)

/* Direction constants */
#define USB_RECV                            (0)
#define USB_SEND                            (1)

#define USB_DEVICE_DONT_ZERO_TERMINATE      (0x1)

#define USB_SETUP_DATA_XFER_DIRECTION       (0x80)

/* Speed definitions */
#define USB_SPEED_FULL                      (0)
#define USB_SPEED_LOW                       (1)
#define USB_SPEED_HIGH                      (2)

#define USB_MAX_PKTS_PER_UFRAME             (0x6)

/*----------------------------------------------------------------**
** Chapter 9.4 Standard Device Requests -- all devices            **
** See Table 9-3 p. 250 of USB 2.0 spec for combinations          **
**   of request type bitfields with requests, WVALUE, WINDEX etc. **
**----------------------------------------------------------------*/
#define USB_REQ_TYPE_DEVICE    (0x00)
#define USB_REQ_TYPE_INTERFACE (0x01)
#define USB_REQ_TYPE_ENDPOINT  (0x02)
/* Combine one of the 3 above with one of the following 2 */
#define USB_REQ_TYPE_IN        (0x80)
#define USB_REQ_TYPE_OUT       (0x00)
/* Also for class requests set the following bit */
#define USB_REQ_TYPE_CLASS     (0x20)
/* Standard USB requests, see Chapter 9 */
#define USB_REQ_GET_STATUS          (0)
#define USB_REQ_CLEAR_FEATURE       (1)
#define USB_REQ_SET_FEATURE         (3)
#define USB_REQ_SET_ADDRESS         (5)
#define USB_REQ_GET_DESCRIPTOR      (6)
#define USB_REQ_SET_DESCRIPTOR      (7)
#define USB_REQ_GET_CONFIGURATION   (8)
#define USB_REQ_SET_CONFIGURATION   (9)
#define USB_REQ_GET_INTERFACE       (10)
#define USB_REQ_SET_INTERFACE       (11)
#define USB_REQ_SYNCH_FRAME         (12)

/* Event codes for attach/detach etc. callback */
#define  USB_ATTACH_EVENT           (1)   /* device attach */
#define  USB_DETACH_EVENT           (2)   /* device detach */
#define  USB_CONFIG_EVENT           (3)   /* device reconfigured */
#define  USB_INTF_EVENT             (4)   /* device interface selected */

/* Memory blocks */
#define MEM_TYPE_USB_BASE                    ((IO_USB_COMPONENT) << (MEM_TYPE_COMPONENT_SHIFT))
#define MEM_TYPE_USB_HOST_DEVICE             (MEM_TYPE_USB_BASE+1)
#define MEM_TYPE_USB_HOST_STATE_STRUCT       (MEM_TYPE_USB_BASE+2)
#define MEM_TYPE_USB_ISO_PACKET_DESC_STRUCT  (MEM_TYPE_USB_BASE+3)
#define MEM_TYPE_USB_HOST_PIPE_DESCRIPTORS   (MEM_TYPE_USB_BASE+4)
#define MEM_TYPE_USB_HOST_PIPE_TR_STRUCT     (MEM_TYPE_USB_BASE+5)
#define MEM_TYPE_USB_FILESYSTEM_STRUCT       (MEM_TYPE_USB_BASE+6)
#define MEM_TYPE_USB_CALLBACK_STRUCT         (MEM_TYPE_USB_BASE+7)
#define MEM_TYPE_USB_MFS_STRUCT              (MEM_TYPE_USB_BASE+8)
#define MEM_TYPE_USB_HOST_SERVICE_STRUCT     (MEM_TYPE_USB_BASE+9)
#define MEM_TYPE_USB_HID_STRUCT              (MEM_TYPE_USB_BASE+10)
#define MEM_TYPE_USB_AUDIO_STRUCT            (MEM_TYPE_USB_BASE+11)
#define MEM_TYPE_USB_AUDIO_RING_BUFF         (MEM_TYPE_USB_BASE+12)
#define MEM_TYPE_USB_INTR_TMP_BUFF           (MEM_TYPE_USB_BASE+13)
#define MEM_TYPE_USB_CTRL_TMP_BUFF           (MEM_TYPE_USB_BASE+14)
#define MEM_TYPE_USB_BULK_TMP_BUFF           (MEM_TYPE_USB_BASE+15)
#define MEM_TYPE_USB_ISO_TMP_BUFF            (MEM_TYPE_USB_BASE+16)
#define MEM_TYPE_USB_LIST_MEM_BUFF           (MEM_TYPE_USB_BASE+17)
#define MEM_TYPE_USB_ASYNC_TMP_BUFF          (MEM_TYPE_USB_BASE+18)
#define MEM_TYPE_USB_END                     MEM_TYPE_USB_ISO_TMP_BUFF

/* Macro for alignment to specific byte boundary, finds the nearest aligned upper address */
#define USB_MEM_ALIGN(n, a)    ((n) + (-(n) & (a - 1)))
#define USB_MEM4_ALIGN(n)      USB_MEM_ALIGN(n, 4)
#define USB_MEM32_ALIGN(n)     USB_MEM_ALIGN(n, 32)
#define USB_MEM2048_ALIGN(n)   USB_MEM_ALIGN(n, 2048)
#define USB_MEM4096_ALIGN(n)   USB_MEM_ALIGN(n, 4096)
/* RX buffer size should be 4-byte aligned. Use a swap buffer to fix it. */
#endif
