#ifndef __ehci_prv_h__
#define __ehci_prv_h__ 1
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
*   This file contains the internal USB Host specific type definitions
*
*
*END************************************************************************/

#include "hostapi.h"
#include "host_cnfg.h"

#include "ehci_cache.h"
#include "ehci.h"


#define  HUB_LS_SETUP                        (333)

/***************************************
**
** Code macros
**
*/

#define  BitStuffTime(x)                     (7* 8 * x / 6)

typedef struct ehci_tr_struct
{
   TR_STRUCT                     G;  /* generic transfer fields */
  /* number of ITds/SITDs used for this transfer */
   uint32_t                       no_of_itds_sitds;
   uint32_t                       SOFAR; /* number of bytes sent/recv'd so far */

   /****************************************************
   Data cache requirement is that any buffer that stores
   the data should be cache aligned. This means that the
   setup packet buffer must be cache aligned too. Structure
   fields above this have occupied 28 + 16 = 44 bytes.
   We add a 20 bytes padding to makesure that it is aligned
   on a 32 byte boundry since 44 + 20 = 64; We also
   add a padding at the end to ensure that we don't corrupt
   someother memory when we flush the setup packet
   from cache.
   ****************************************************/
#if PSP_HAS_DATA_CACHE  // TODO check align
    uint8_t                       RESERVED1[USB_CACHE_ALIGN(40) - 40];
#endif // PSP_HAS_DATA_CACHE
   
#if PSP_HAS_DATA_CACHE  // TODO check align
    uint8_t               RESERVED2[USB_CACHE_ALIGN(8) - 8];
#endif
	bool										setup_first_phase;
} EHCI_TR_STRUCT, * EHCI_TR_STRUCT_PTR;
    
typedef struct ehci_pipe_struct
{
   PIPE_STRUCT          G;

   uint32_t              CURRENT_NAK_COUNT;
   void                *QH_FOR_THIS_PIPE; /* Queue head for this pipe */

   /* Special fields defined for periodic transfers */
   uint32_t              START_FRAME;    /*frame number from which this pipe is started to schedule*/
   uint32_t              START_UFRAME;   /*Micro frame number from which this pipe is started to schedule*/
   uint32_t              NO_OF_START_SPLIT; /* store the number of start splits (used for ISO OUT )*/
   uint8_t               START_SPLIT;    /* store the start split slots for full speed devices*/
   uint8_t               COMPLETE_SPLIT; /* store the complete split slots for full speed devices*/
   uint16_t              BWIDTH;         /* time take by this pipe (for periodic pipes only */
   uint8_t               BWIDTH_SLOTS[8];/*micro frame slots budgeted for transaction */
   
   /* 84 bytes so far add padding to ensure cache alignment*/
#if PSP_HAS_DATA_CACHE  // TODO check align
   uint8_t               reserved[USB_CACHE_ALIGN(74) - 74]; 
#endif  
    
} EHCI_PIPE_STRUCT, * EHCI_PIPE_STRUCT_PTR;

/* Queue head management data structure */
typedef struct active_qh_mgmt_struct {
   EHCI_QH_STRUCT_PTR                  QH_PTR;
   EHCI_QTD_STRUCT_PTR                 FIRST_QTD_PTR;
   int32_t                              TIME;
   struct active_qh_mgmt_struct       *NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
} ACTIVE_QH_MGMT_STRUCT, * ACTIVE_QH_MGMT_STRUCT_PTR;

/* ITD,SITD list management data structure (doubly link list )*/
typedef struct list_node_struct {
   struct list_node_struct       *next;        /* next member in the list */
   struct list_node_struct       *prev;        /* previous member in the list */
   bool   next_active; /* is next node a active node */
   void     *member;      /* pointer to the currently active ITD or SITD*/
} LIST_NODE_STRUCT, * LIST_NODE_STRUCT_PTR;


/* Class Callback function storage structure */
typedef struct class_service_struct {
   uint8_t               CLASS_TYPE;
   uint8_t               SUB_CLASS;
   uint8_t               PROTOCOL;
   /* index of the instance */
   uint8_t               INDEX;
   /* identification string unique to this type */
   char             *CLASS_ID_STRING;
   /* class handle will be NULL if it is not initialized */
   void                *CLASS_HANDLE;
   void   *(_CODE_PTR_  INIT)(void *, uint32_t);
   void    (_CODE_PTR_  DEINIT)(void *);
   void    (_CODE_PTR_  CALL_BACK_INIT)(void *, char *);
   void    (_CODE_PTR_  CALL_BACK_REMOVE)(void *);
   struct class_service_struct      *NEXT_INSTANCE;
   struct class_service_struct      *NEXT;
} USB_HOST_CLASS_DRIVER_STRUCT, * USB_HOST_CLASS_DRIVER_STRUCT_PTR;

typedef struct  usb_ehci_host_state_structure {
   USB_HOST_STATE_STRUCT               G; /* generic host state */
   uint32_t                             FRAME_LIST_SIZE;
   uint8_t                          *PERIODIC_FRAME_LIST_BW_PTR;
   void                               *PERIODIC_LIST_BASE_ADDR;
   void                               *ALIGNED_PERIODIC_LIST_BASE_ADDR;
   EHCI_QH_STRUCT_PTR                  ASYNC_LIST_BASE_ADDR;
   EHCI_QH_STRUCT_PTR                  ALIGNED_ASYNCLIST_ADDR;
   EHCI_QH_STRUCT_PTR                  QH_BASE_PTR; 
   EHCI_QH_STRUCT_PTR                  QH_ALIGNED_BASE_PTR; 
   uint32_t                             QH_ENTRIES;
   EHCI_QH_STRUCT_PTR                  QH_HEAD;
   EHCI_QH_STRUCT_PTR                  QH_TAIL;
   ACTIVE_QH_MGMT_STRUCT_PTR           ACTIVE_ASYNC_LIST_PTR;
   ACTIVE_QH_MGMT_STRUCT_PTR           ACTIVE_INTERRUPT_PERIODIC_LIST_PTR;
   EHCI_QTD_STRUCT_PTR                 QTD_BASE_PTR;
   EHCI_QTD_STRUCT_PTR                 QTD_HEAD;
   EHCI_QTD_STRUCT_PTR                 QTD_TAIL;
   EHCI_QTD_STRUCT_PTR                 QTD_ALIGNED_BASE_PTR;
   uint32_t                             QTD_ENTRIES;
   bool                             HIGH_SPEED_ISO_QUEUE_ACTIVE; 
   LIST_NODE_STRUCT_PTR                ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR;
   LIST_NODE_STRUCT_PTR                ACTIVE_ISO_ITD_PERIODIC_LIST_TAIL_PTR;
   EHCI_ITD_STRUCT_PTR                 ITD_BASE_PTR;
   EHCI_ITD_STRUCT_PTR                 ITD_ALIGNED_BASE_PTR;
   EHCI_ITD_STRUCT_PTR                 ITD_HEAD;
   EHCI_ITD_STRUCT_PTR                 ITD_TAIL;
   uint32_t                             ITD_ENTRIES;
   bool                             FULL_SPEED_ISO_QUEUE_ACTIVE; 
   LIST_NODE_STRUCT_PTR                ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR;
   LIST_NODE_STRUCT_PTR                ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR;
   EHCI_SITD_STRUCT_PTR                SITD_BASE_PTR;
   EHCI_SITD_STRUCT_PTR                SITD_ALIGNED_BASE_PTR;
   EHCI_SITD_STRUCT_PTR                SITD_HEAD;
   EHCI_SITD_STRUCT_PTR                SITD_TAIL;
   uint32_t                             SITD_ENTRIES;
   uint32_t                             RESET_RECOVERY_TIMER;
   uint32_t                             PORT_NUM;
   uint8_t                              UFRAME_COUNT;
   void                               *CONTROLLER_MEMORY;
   bool                             PERIODIC_LIST_INITIALIZED;
   bool                             ITD_LIST_INITIALIZED;
   bool                             SITD_LIST_INITIALIZED;
   bool                             IS_RESETTING;
} USB_EHCI_HOST_STATE_STRUCT, * USB_EHCI_HOST_STATE_STRUCT_PTR;

#define EHCI_GET_TYPE(data_struct_ptr) \
   (EHCI_MEM_READ(*((volatile uint32_t *)data_struct_ptr)) & EHCI_ELEMENT_TYPE_MASK)

#define EHCI_ITD_QADD(head, tail, ITD)      \
   if ((head) == NULL) {         \
      (head) = (ITD);            \
   } else {                      \
      (tail)->SCRATCH_PTR = (void *) (ITD);   \
   } /* Endif */                 \
   (tail) = (ITD);               \
   (ITD)->SCRATCH_PTR = NULL
   
#define EHCI_ITD_QGET(head, tail, ITD)      \
   (ITD) = (head);               \
   if (head) {                   \
      (head) = (EHCI_ITD_STRUCT_PTR)((head)->SCRATCH_PTR);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

#define EHCI_ACTIVE_QUEUE_ADD_NODE(tail,member_ptr)     \
   if (tail->prev != NULL) {                             \
       tail->prev->next_active = TRUE;                   \
   }                                                     \
   tail->member = (void *) member_ptr;                                  \
   tail->next_active = FALSE;                            \
   tail = tail->next;                   

#define EHCI_QUEUE_FREE_NODE(head,tail,node_ptr) \
   if(node_ptr->prev != NULL) {                             \
     node_ptr->prev->next = node_ptr->next;                 \
     node_ptr->prev->next_active = node_ptr->next_active;   \
     node_ptr->next->prev = node_ptr->prev;                 \
   } else  {                                                \
     head = node_ptr->next;                                 \
     head->prev = NULL;                                     \
   }                                                        \
   node_ptr->next = tail->next;                             \
   node_ptr->prev = tail;                                   \
   node_ptr->next_active = FALSE;                           \
   node_ptr->member = NULL;                                 \
   if(tail->next != NULL)   {                               \
      tail->next->prev = node_ptr;                          \
   }                                                        \
   tail->next = node_ptr;                                   \
   tail->next_active = FALSE;                             




#define EHCI_SITD_QADD(head, tail, SITD)      \
   if ((head) == NULL) {         \
      (head) = (SITD);            \
   } else {                      \
      (tail)->SCRATCH_PTR = (void *) (SITD);   \
   } /* Endif */                 \
   (tail) = (void *) (SITD);               \
   (SITD)->SCRATCH_PTR = NULL
   
#define EHCI_SITD_QGET(head, tail, SITD)      \
   (SITD) = (head);               \
   if (head) {                   \
      (head) = (EHCI_SITD_STRUCT_PTR)((head)->SCRATCH_PTR);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */
   
#define EHCI_QTD_QADD(head, tail, QTD)      \
   if ((head) == NULL) {         \
      (head) = (QTD);            \
   } else {                      \
      (tail)->SCRATCH_PTR = (void *) (QTD);   \
   } /* Endif */                 \
   (tail) = (void *) (QTD);               \
   (QTD)->SCRATCH_PTR = NULL
   
#define EHCI_QTD_QGET(head, tail, QTD)      \
   (QTD) = (head);               \
   if (head) {                   \
      (head) = (EHCI_QTD_STRUCT_PTR)((head)->SCRATCH_PTR);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

#define EHCI_QH_QADD(head, tail, QH)      \
   if ((head) == NULL) {         \
      (head) = (void *) (QH);            \
   } else {                      \
      (tail)->SCRATCH_PTR = (void *) (QH);   \
   } /* Endif */                 \
   (tail) = (QH);               \
   (QH)->SCRATCH_PTR = NULL
   
#define EHCI_QH_QGET(head, tail, QH)      \
   (QH) = (head);               \
   if (head) {                   \
      (head) = (EHCI_QH_STRUCT_PTR)((head)->SCRATCH_PTR);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

/* Additional Includes */
#include "ehci_bw.h"
#include "ehci_cncl.h"
#include "ehci_intr.h"
#include "ehci_iso.h"
#include "ehci_main.h"
#include "ehci_shut.h"
#include "ehci_utl.h"

/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

void _usb_ehci_process_qh_list_tr_complete(_usb_host_handle handle, ACTIVE_QH_MGMT_STRUCT_PTR active_list_member_ptr);
void _usb_ehci_process_qh_interrupt_tr_complete(_usb_host_handle handle, ACTIVE_QH_MGMT_STRUCT_PTR);
void _usb_ehci_process_itd_tr_complete(_usb_host_handle handle);
void _usb_ehci_process_sitd_tr_complete(_usb_host_handle handle);

#ifdef __cplusplus
}
#endif

#endif
