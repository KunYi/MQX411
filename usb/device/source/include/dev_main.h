#ifndef __dev_main_h__
#define __dev_main_h__ 1
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
*  This file contains the declarations specific to the USB Device API
*
*
*END************************************************************************/
#include "mqx_dev.h"

typedef struct xd_struct 
{
   unsigned char          EP_NUM;           /* Endpoint number */
   unsigned char          BDIRECTION;       /* Direction : Send/Receive */
   uint8_t         EP_TYPE;          /* Type of the endpoint: Ctrl, Isoch, Bulk, Int */
   unsigned char          BSTATUS;          /* Current transfer status */
   uint8_t     *WSTARTADDRESS;    /* Address of first byte */
   uint32_t        WTOTALLENGTH;     /* Number of bytes to send/recv */
   uint32_t        WSOFAR;           /* Number of bytes recv'd so far */
   uint16_t        WMAXPACKETSIZE;   /* Max Packet size */
   bool        DONT_ZERO_TERMINATE;
   SCRATCH_STRUCT_PTR SCRATCH_PTR;
} XD_STRUCT, * XD_STRUCT_PTR;

typedef struct usb_dev_state_struct {
   void                            *INIT_PARAM;          /* Device controller init parameters */
   void                            *DEV_PTR;             /* Device Controller Register base address */
   const USB_DEV_CALLBACK_FUNCTIONS_STRUCT * CALLBACK_STRUCT_PTR;
   SCRATCH_STRUCT_PTR               XD_SCRATCH_STRUCT_BASE;
   SCRATCH_STRUCT_PTR               SCRATCH_STRUCT_BASE;
   
   SERVICE_STRUCT_PTR               SERVICE_HEAD_PTR;    /* Head struct address of registered services */
   struct xd_struct               * TEMP_XD_PTR;         /* Temp xd for ep init */
   struct xd_struct               * XD_BASE;
   struct xd_struct               * XD_HEAD;             /* Head Transaction descriptors */
   struct xd_struct               * XD_TAIL;             /* Tail Transaction descriptors */
   struct xd_struct               * PENDING_XD_PTR;      /* pending transfer */
   uint32_t                          XD_ENTRIES;

   /* These fields are kept only for USB_shutdown() */
   uint16_t                          USB_STATE;
   uint16_t                          USB_DEVICE_STATE;
   uint16_t                          USB_SOF_COUNT;
   uint16_t                          ERRORS;
   uint16_t                          USB_DEV_STATE_B4_SUSPEND;
   uint8_t                           DEV_NUM;             /* Deprecated, avoid usage; USB device number on the board */
   uint8_t                           DEV_VEC;             /* Interrupt vector number for USB OTG */
   uint8_t                           SPEED;               /* Low Speed, High Speed, Full Speed */
   uint8_t                           MAX_ENDPOINTS;       /* Max endpoints supported by this device */
                                                         
   uint16_t                          USB_CURR_CONFIG;                                                         
   uint8_t                           DEVICE_ADDRESS;
} USB_DEV_STATE_STRUCT, * USB_DEV_STATE_STRUCT_PTR;

#define USB_XD_QADD(head,tail,XD)      \
   if ((head) == NULL) {         \
      (head) = (XD);            \
   } else {                      \
      (tail)->SCRATCH_PTR->PRIVATE = (XD);   \
   } /* Endif */                 \
   (tail) = (XD);               \
   (XD)->SCRATCH_PTR->PRIVATE = NULL
   
#define USB_XD_QGET(head,tail,XD)      \
   (XD) = (head);               \
   if (head) {                   \
      (head) = (XD_STRUCT_PTR)((head)->SCRATCH_PTR->PRIVATE);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

#ifdef __cplusplus
{
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
