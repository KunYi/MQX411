/*HEADER**********************************************************************
*
* Copyright 2009 Freescale Semiconductor, Inc.
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
*   This file contains the private defines, externs and data structure
*   definitions required by the Full Speed USB Device driver.
*
*
*END************************************************************************/

#ifndef __khci_dev_usbprv_h__
#define __khci_dev_usbprv_h__ 1

#include "dev_cnfg.h"
#include "devapi.h"
#include "dev_main.h"

#if PSP_MQX_CPU_IS_MCF51JM
   #define  USB_MAX_ENDPOINTS                (4)
   #define  MAX_XDS_FOR_TR_CALLS             (8) 
#else
   #define  USB_MAX_ENDPOINTS                (16) 
   #define  MAX_XDS_FOR_TR_CALLS             (32) 
#endif

/* Allow workaround for bug in the peripheral when unaligned buffer @4B address is used */
#define KHCICFG_4BYTE_ALIGN_FIX

/* Alignement of buffer for DMA transfer, needed in some cases,
** USB DMA bus could not possibly be intializes properly and 
** first data transfered is the one aligned at 4-byte boundary
*/
#define USB_DMA_ALIGN(n)                     USB_MEM4_ALIGN(n)
#define USB_DMA_ALIGN_MASK                   (0x03)

#define MAX_USB_DEVICES                      (1)
#define ODD_BUFF                             (1)
#define EVEN_BUFF                            (0)

#define BDT_BASE                ((uint32_t*)(bdt))
#define BD_PTR(ep, rxtx, odd)   ((((uint32_t)BDT_BASE) & (0xfffffe00)) | ((ep & 0x0f) << 5) | ((rxtx & 1) << 4) | ((odd & 1) << 3))

#define BD_CTRL(ep,rxtx,odd)    (*((uint32_t*)BD_PTR(ep, rxtx, odd)))
#define BD_CTRL_RX(ep, odd)     (*((uint32_t*)BD_PTR(ep, 0, odd)))
#define BD_CTRL_TX(ep, odd)     (*((uint32_t*)BD_PTR(ep, 1, odd)))

#define BD_ADDR(ep,rxtx,odd)    (*((uint32_t*)BD_PTR(ep, rxtx, odd) + 1))
#define BD_ADDR_RX(ep, odd)     (*((uint32_t*)BD_PTR(ep, 0, odd) + 1))
#define BD_ADDR_TX(ep, odd)     (*((uint32_t*)BD_PTR(ep, 1, odd) + 1))

/* Define USB buffer descriptor definitions in case of their lack */
#ifndef USB_BD_BC
#   define USB_BD_BC(n)                 ((n & 0x3ff) << 16)
#   define USB_BD_OWN                   0x80
#   define USB_BD_DATA01(n)             ((n & 1) << 6)
#   define USB_BD_DATA0                 USB_BD_DATA01(0)
#   define USB_BD_DATA1                 USB_BD_DATA01(1)
#   define USB_BD_KEEP                  0x20
#   define USB_BD_NINC                  0x10
#   define USB_BD_DTS                   0x08
#   define USB_BD_STALL                 0x04
#   define USB_BD_PID(n)                ((n & 0x0f) << 2)
#endif

#ifndef USB_TOKEN_TOKENPID_SETUP
#   define USB_TOKEN_TOKENPID_OUT                   USB_TOKEN_TOKENPID(0x1)
#   define USB_TOKEN_TOKENPID_IN                    USB_TOKEN_TOKENPID(0x9)
#   define USB_TOKEN_TOKENPID_SETUP                 USB_TOKEN_TOKENPID(0xD)
#endif

typedef struct khci_xd_struct 
{
   struct xd_struct G;

   bool       ZERO_PACKET;
   uint32_t      WPOSITION;       /* current position of the buffer */
   uint32_t      FIFO_SIZE;       /* To keep track of FIFO length for this ep */
   uint32_t      IN_FIFO_ADDR;    /* To keep track of FIFO start address for this ep */
   uint32_t      OUT_FIFO_ADDR;   /* To keep track of FIFO start address for this ep */   
   uint8_t       VECTOR;          /* Vector of this endpoint */
   unsigned char SETUP[8];        /* Just maintain SETUP packet */

   void          *MY_XDPTR;        /* This will save the xd_ptr send by API during send/ recv request */
   unsigned char *OUT_TEMP_BUFFER; /* This is required during OUT interrupt to store OUT data */
   bool       IS_SETUP_PACKET; /* To identify SETUP packet. this is required when 
                                 ** there is setup packet and upper driver does not want 
                                 ** to send the data in the  data phase, we still need to 
                                 ** set the CMD_OVER to indicate that the processing of 
                                 ** class/vendor specific request is completed.
                                 */
   uint16_t      PENDING_XD; /* counter */
   SCRATCH_STRUCT_PTR SCRATCH_PTR;

#if defined(KHCICFG_4BYTE_ALIGN_FIX)
    bool     INTERNAL_DMA_ALIGN;
#endif   

} KHCI_XD_STRUCT, * KHCI_XD_STRUCT_PTR;

typedef struct _usb_xd_queue
{
   struct xd_struct      *xd_head_ptr;
   struct xd_struct      *xd_tail_ptr;
} USB_XD_QUEUE, * USB_XD_QUEUE_PTR;

typedef struct _usb_ep_info_struct
{
   USB_XD_QUEUE xd_queue_send;  /* FIFO queue for all XDs on this endpoint */   
   USB_XD_QUEUE xd_queue_recv;
   uint8_t type;
   uint8_t direction; /* for _usb_device_call_service */
   uint16_t max_packet_size;
   uint16_t fifo_length;
   
   uint8_t tx_data0;
   uint8_t rx_data0;
   
   uint8_t tx_buf_odd;
   uint8_t rx_buf_odd;   /* next buffer is odd */
   uint8_t *data_buffer;
   uint16_t endpoint_status;
   bool stall_flag;
   /* Endpoint initialization flag for both direction */
   bool ep_init_flag[2]; 
   uint32_t save_recv_addr;
} USB_EP_INFO_STRUCT, * USB_EP_INFO_STRUCT_PTR;

/* The USB Device State Structure */
typedef struct _usb_device_state_struct 
{
   USB_DEV_STATE_STRUCT             G;
   USB_EP_INFO_STRUCT               EP_INFO[USB_MAX_ENDPOINTS];
} USB_KHCI_DEV_STATE_STRUCT, * USB_KHCI_DEV_STATE_STRUCT_PTR;

#define USB_XD_QADD(head,tail,XD)            \
   if ((head) == NULL) {                     \
      (head) = (XD);                         \
   } else {                                  \
      (tail)->SCRATCH_PTR->PRIVATE = (XD);   \
   } /* Endif */                             \
   (tail) = (XD);                            \
   (XD)->SCRATCH_PTR->PRIVATE = NULL
   
#define USB_XD_QGET(head,tail,XD)                              \
   (XD) = (head);                                              \
   if (head) {                                                 \
      (head) = (XD_STRUCT_PTR)((head)->SCRATCH_PTR->PRIVATE);  \
      if ((head) == NULL) {                                    \
         (tail) = NULL;                                        \
      } /* Endif */                                            \
   } /* Endif */
  
#define USB_XD_QUEUE_INIT(xd_queue_ptr)                        \
   (xd_queue_ptr)->xd_head_ptr = 0x0;                          \
   (xd_queue_ptr)->xd_tail_ptr = 0x0

#define USB_XD_QUEUE_ENQUEUE(xd_queue_ptr, xd_ptr)  \
   USB_XD_QADD ((xd_queue_ptr)->xd_head_ptr, (xd_queue_ptr)->xd_tail_ptr, (xd_ptr))
   
#define USB_XD_QUEUE_DEQUEUE(xd_queue_ptr, xd_ptr_ptr)  \
   USB_XD_QGET ((xd_queue_ptr)->xd_head_ptr, (xd_queue_ptr)->xd_tail_ptr, *(xd_ptr_ptr))
   
#define USB_XD_QUEUE_ENQUEUE_SAFE(xd_queue_ptr, xd_ptr)  \
   _int_disable ();  \
   USB_XD_QUEUE_ENQUEUE (xd_queue_ptr, xd_ptr); \
   _int_enable ();

#define USB_XD_QUEUE_DEQUEUE_SAFE(xd_queue_ptr, xd_ptr)  \
   _int_disable ();  \
   USB_XD_QUEUE_DEQUEUE (xd_queue_ptr, xd_ptr); \
   _int_enable ();

#define USB_XD_QUEUE_GET_HEAD(xd_queue_ptr, xd_ptr_ptr)  \
   *(xd_ptr_ptr) = (xd_queue_ptr)->xd_head_ptr

#define USB_XD_QUEUE_GET_TAIL(xd_queue_ptr, xd_ptr_ptr)  \
   *(xd_ptr_ptr) = (xd_queue_ptr)->xd_tail_ptr

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
