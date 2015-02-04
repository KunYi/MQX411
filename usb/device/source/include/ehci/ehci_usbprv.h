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
*  This file contains the private defines, externs and
*  data structure definitions required by the VUSB_HS Device
*  driver.
*                                                               
*
*END************************************************************************/

#ifndef __ehci_dev_usbprv_h__
#define __ehci_dev_usbprv_h__ 1

#include "dev_cnfg.h"
#include "ehci_dev_main.h"

#define  MAX_EP_TR_DESCRS                    (32) 
#define  MAX_XDS_FOR_TR_CALLS                (32) 
#define  ZERO_LENGTH                         (0) 
#define  USB_MAX_ENDPOINTS                   (6) 

#define  USB_MAX_CTRL_PAYLOAD                (64)
 
#define EHCI_DTD_QADD(head,tail,dTD)      \
   if ((head) == NULL) {         \
      (head) = (dTD);            \
   } else {                      \
      (tail)->SCRATCH_PTR->PRIVATE = (void *) (dTD);   \
   } /* Endif */                 \
   (tail) = (dTD);               \
   (dTD)->SCRATCH_PTR->PRIVATE = NULL
   
#define EHCI_DTD_QGET(head,tail,dTD)      \
   (dTD) = (head);               \
   if (head) {                   \
      (head) = (head)->SCRATCH_PTR->PRIVATE;  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

/***************************************
**
** Data structures
**
*/

typedef struct ehci_xd_struct 
{
   struct xd_struct G;

   uint8_t         MAX_PKTS_PER_UFRAME;
} EHCI_XD_STRUCT, * EHCI_XD_STRUCT_PTR;

/* The USB Device State Structure */
typedef struct _usb_ehci_device_state_struct 
{
   USB_DEV_STATE_STRUCT             G;
   void(_CODE_PTR_                  OLDISR_PTR)(void *);
   void                            *OLDISR_DATA;

   void                            *CAP_BASE_PTR;

   bool                          BUS_RESETTING;       /* Device is being reset */
   bool                          TRANSFER_PENDING;    /* Transfer pending ? */

   USBHS_EP_QUEUE_HEAD_STRUCT_PTR   EP_QUEUE_HEAD_PTR;   /* Endpoint Queue head */
   uint8_t                       *DRIVER_MEMORY;       /* pointer to driver memory*/
   uint32_t                          TOTAL_MEMORY;        /* total memory occupied by driver */
   USBHS_EP_QUEUE_HEAD_STRUCT_PTR   EP_QUEUE_HEAD_BASE;
   USBHS_EP_TR_STRUCT_PTR           DTD_BASE_PTR;         /* Device transfer descriptor pool address */
   USBHS_EP_TR_STRUCT_PTR           DTD_ALIGNED_BASE_PTR; /* Aligned transfer descriptor pool address */
   USBHS_EP_TR_STRUCT_PTR           DTD_HEAD;
   USBHS_EP_TR_STRUCT_PTR           DTD_TAIL;
   USBHS_EP_TR_STRUCT_PTR           EP_DTD_HEADS[USB_MAX_ENDPOINTS * 2];
   USBHS_EP_TR_STRUCT_PTR           EP_DTD_TAILS[USB_MAX_ENDPOINTS * 2];
   uint16_t                          DTD_ENTRIES;

} USB_EHCI_DEV_STATE_STRUCT, * USB_EHCI_DEV_STATE_STRUCT_PTR;

/***************************************
**
** Prototypes
**
*/
#ifdef __cplusplus
extern "C" {
#endif
extern void _usb_dci_usbhs_isr(void *);

#ifdef __cplusplus
}
#endif

#endif
