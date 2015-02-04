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
*   This file contains the macros, function prototypes and data structure
*   definitions required by the KHCI USB device driver.
*
*
*END************************************************************************/
#ifndef __khci_prv_h__
#define __khci_prv_h__

#include <mqx.h>
#include <lwevent.h>
#include <lwmsgq.h>

#include <bsp.h>

#include "usb.h"
#include "usb_misc.h"

#include "host_cnfg.h"
#include "hostapi.h"    /* needed for USB_SERVICE_STRUCT_PTR */
#include "host_main.h"

enum {
    TR_CTRL,
    TR_IN,
    TR_OUT,
    TR_OUT_HNDSK,
    TR_IN_HNDSK
};

typedef struct khci_tr_struct {
   struct tr_struct G;
   uint32_t  status;
} KHCI_TR_STRUCT, * KHCI_TR_STRUCT_PTR;

typedef struct khci_pipe_struct
{
   /* Fields common to both pipe descriptors and the structures used to
    * initialize new pipe instances.
    */
   PIPE_STRUCT         G;
} KHCI_PIPE_STRUCT, * KHCI_PIPE_STRUCT_PTR;

/* Transaction type */
typedef enum {
    TR_MSG_UNKNOWN,     // unknown - not used
    TR_MSG_SETUP,       // setup transaction
    TR_MSG_SEND,        // send transaction
    TR_MSG_RECV,        // receive transaction
    SHDWN_MSG           // shutdown
} MSG_TYPE;


/* Transaction message */
typedef struct {
    MSG_TYPE type;                       // transaction type
    union {
        struct TR_MSG_STRUCT {
            KHCI_PIPE_STRUCT_PTR pipe_desc;         // pointer to pipe descriptor
            KHCI_TR_STRUCT_PTR pipe_tr;             // pointer to transaction
        } tr;
        struct SHDWN_MSG_STRUCT {
            LWSEM_STRUCT_PTR end_of_task;         // KHCI task notifies about its end of life
        } shdwn;
    } msg;
} MSG_STRUCT;

/* Interrupt transaction message queue */
typedef struct tr_int_que_itm_struct {
    TIME_STRUCT   time;         /* next time to fire interrupt */
    _mqx_uint     period;       /* period in [ms] for interrupt endpoint */
    MSG_STRUCT    message;      /* atom transaction message */
} TR_INT_QUE_ITM_STRUCT, * TR_INT_QUE_ITM_STRUCT_PTR;

#define BDT_BASE                ((uint32_t*)(bdt))
#define BD_PTR(ep, rxtx, odd)   ((((uint32_t)BDT_BASE) & 0xfffffe00) | ((ep & 0x0f) << 5) | ((rxtx & 1) << 4) | ((odd & 1) << 3))

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

/* Alignement of buffer for DMA transfer, needed in some cases,
** USB DMA bus could not possibly be intializes properly and
** first data transfered is the one aligned at 4-byte boundary
*/
#define USB_DMA_ALIGN(n)                USB_MEM4_ALIGN(n)
#define USB_DMA_ALIGN_MASK              (0x03)

typedef struct usb_khci_host_state_struct
{
   USB_HOST_STATE_STRUCT               G; /* generic host state */
   /* Driver-specific fields */
   uint32_t                             vector;
   bool                             endian_swap;
   /* RX/TX buffer descriptor toggle bits */
   uint8_t                              rx_bd;
   uint8_t                              tx_bd;
   /* event from KHCI peripheral */
   LWEVENT_STRUCT                      khci_event;
   /* Pipe, that had latest NAK respond, usefull to cut bandwidth for interrupt pipes sending no data */
   KHCI_PIPE_STRUCT_PTR                last_to_pipe;
   /* Interrupt transactions */
   TR_INT_QUE_ITM_STRUCT               tr_int_que[USBCFG_KHCI_MAX_INT_TR];
   _mqx_max_type                       tr_que[(sizeof(LWMSGQ_STRUCT) + USBCFG_KHCI_TR_QUE_MSG_CNT * sizeof(MSG_STRUCT)) / sizeof(_mqx_max_type) + 1];
} USB_KHCI_HOST_STATE_STRUCT, * USB_KHCI_HOST_STATE_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
