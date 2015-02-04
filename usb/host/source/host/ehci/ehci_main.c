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
*   This file contains the main low-level Host API functions specific to
*   the VUSB chip.
*
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>

#include "usb.h"
#include "usb_misc.h"
#include "usb_prv.h"

#include "hostapi.h"
#include "host_dev_list.h"

#include "host_cnfg.h"
#include "ehci_main.h"
#include "ehci_prv.h"

#ifdef __USB_OTG__
#include "otgapi.h"
   extern USB_OTG_STATE_STRUCT_PTR usb_otg_state_struct_ptr;
#endif

/**************************************************************************
 Local Functions. Note intended to be called from outside this file
**************************************************************************/

const USB_HOST_CALLBACK_FUNCTIONS_STRUCT _usb_ehci_host_callback_table =
{
   /* The Host/Device init function */
   _usb_ehci_preinit,

   /* The Host/Device init function */
   _usb_ehci_init,

   /* The function to shutdown the host/device */
   _usb_ehci_shutdown,

   /* The function to send data */
   _usb_ehci_send_data,

   /* The function to send setup data */
   _usb_ehci_send_setup,

   /* The function to receive data */
   _usb_ehci_recv_data,

   /* The function to get the transfer status */
//   NULL,

   /* The function to cancel the transfer */
   _usb_ehci_cancel_transfer,

   /* The function to do USB sleep */
//   NULL,

   /* The function for USB bus control */
   _usb_ehci_bus_control,

   _usb_ehci_allocate_bandwidth,

   _usb_ehci_free_resources,

   _usb_ehci_get_frame_number,

   _usb_ehci_get_micro_frame_number,

   /* The function to open a pipe */
   _usb_ehci_open_pipe,

   /* The function to update the maximum packet size */
   NULL,

   /* The function to update the device address */
   _usb_ehci_update_dev_address
};

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_send_data
*  Returned Value : None
*  Comments       :
*        Send the data
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_ehci_send_data
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle              handle,

      /* [IN] The pipe descriptor to queue */
      PIPE_STRUCT_PTR               pipe_descr_ptr,

      /* [IN] the transfer parameters struct */
      TR_STRUCT_PTR                 current_pipe_tr_struct_ptr
   )
{ /* Body */
   USB_STATUS status;

   status = _usb_ehci_queue_pkts(handle, (EHCI_PIPE_STRUCT_PTR) pipe_descr_ptr, (EHCI_TR_STRUCT_PTR) current_pipe_tr_struct_ptr);
   return status;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_send_setup
*  Returned Value : USB_STATUS
*  Comments       :
*        Send Setup packet
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_ehci_send_setup
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle              handle,

      /* [IN] The pipe descriptor to queue */
      PIPE_STRUCT_PTR               pipe_descr_ptr,

      /* [IN] the transfer parameters struct */
      TR_STRUCT_PTR                 current_pipe_tr_struct_ptr
   )
{ /* Body */
   USB_STATUS status;
   status = _usb_ehci_queue_pkts(handle, (EHCI_PIPE_STRUCT_PTR) pipe_descr_ptr, (EHCI_TR_STRUCT_PTR) current_pipe_tr_struct_ptr);
   return status;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_recv_data
*  Returned Value : None
*  Comments       :
*        Receive data
*END*-----------------------------------------------------------------*/

USB_STATUS _usb_ehci_recv_data
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle              handle,

      /* [IN] The pipe descriptor to queue */
      PIPE_STRUCT_PTR               pipe_descr_ptr,

      /* [IN] the transfer parameters struct */
      TR_STRUCT_PTR                 current_pipe_tr_struct_ptr
   )
{ /* Body */
   USB_STATUS status;

   status = _usb_ehci_queue_pkts(handle, (EHCI_PIPE_STRUCT_PTR) pipe_descr_ptr, (EHCI_TR_STRUCT_PTR) current_pipe_tr_struct_ptr);

   return status;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_queue_pkts
*  Returned Value : status of the transfer queuing.
*  Comments       :
*        Queue the packet in the hardware
*END*-----------------------------------------------------------------*/

USB_STATUS _usb_ehci_queue_pkts
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      /* The pipe descriptor to queue */
      EHCI_PIPE_STRUCT_PTR            pipe_descr_ptr,

      /* [IN] the transfer parameters struct */
      EHCI_TR_STRUCT_PTR               current_pipe_tr_struct_ptr
   )
{ /* Body */
   USB_STATUS  status;
   if(pipe_descr_ptr->G.PIPETYPE == USB_CONTROL_PIPE )
     current_pipe_tr_struct_ptr->setup_first_phase = 0;
   /* Queue the packets */
   switch (pipe_descr_ptr->G.PIPETYPE) {
#if USBCFG_EHCI_MAX_ITD_DESCRS || USBCFG_EHCI_MAX_SITD_DESCRS
      case USB_ISOCHRONOUS_PIPE:
         status = _usb_ehci_add_isochronous_xfer_to_periodic_schedule_list(handle, pipe_descr_ptr, current_pipe_tr_struct_ptr);
         break;
#endif //USBCFG_EHCI_MAX_ITD_DESCRS || USBCFG_EHCI_MAX_SITD_DESCRS
#ifdef USBCFG_EHCI_MAX_QTD_DESCRS
      case USB_INTERRUPT_PIPE:
         status = _usb_ehci_add_interrupt_xfer_to_periodic_list(handle, pipe_descr_ptr, current_pipe_tr_struct_ptr);
         break;
      case USB_CONTROL_PIPE:
      case USB_BULK_PIPE:
         /* Queue it on the asynchronous schedule list */
         status = _usb_ehci_add_xfer_to_asynch_schedule_list(handle, pipe_descr_ptr, current_pipe_tr_struct_ptr);
         break;
#endif //USBCFG_EHCI_MAX_QTD_DESCRS
      default:
         status = USB_STATUS_ERROR;
         break;
   } /* EndSwitch */

   return status;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_init_Q_HEAD
*  Returned Value : None
*  Comments       :
*        Initialize the Queue Head. This routine initializes a queue head
*  for interrupt/Bulk/Control transfers.
*END*-----------------------------------------------------------------*/
void _usb_ehci_init_Q_HEAD
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      /* The pipe descriptor to queue */
      EHCI_PIPE_STRUCT_PTR             pipe_descr_ptr,

      /* [IN] the queue head to initialize */
      EHCI_QH_STRUCT_PTR               QH_ptr,

      /* [IN] the previous queue head to link to */
      EHCI_QH_STRUCT_PTR               prev_QH_ptr,

      /* [IN] the first QTD to queue for this Queue head */
      EHCI_QTD_STRUCT_PTR              first_QTD_ptr
   )
{ /* Body */
   uint32_t                       control_ep_flag = 0, split_completion_mask = 0;
   uint32_t                       interrupt_schedule_mask = 0;
   uint32_t                       data_toggle_control = 0, item_type = 0;
   uint32_t                       H_bit = 0, temp;
   uint8_t                        mult = 1;

/******************************************************************************
 PREPARE THE BASIC FIELDS OF A QUEUE HEAD DEPENDING UPON THE PIPE TYPE
*******************************************************************************/
#if PSP_HAS_DATA_CACHE    // TODO change to memzero and flush
   /********************************************************
   USB Memzero does not bypass the cache and hence explicit
   setting to zero is required in software
   ********************************************************/
   EHCI_MEM_WRITE(QH_ptr->HORIZ_LINK_PTR,0);
   EHCI_MEM_WRITE(QH_ptr->EP_CAPAB_CHARAC1,0);
   EHCI_MEM_WRITE(QH_ptr->EP_CAPAB_CHARAC2,0);
   EHCI_MEM_WRITE(QH_ptr->CURR_QTD_LINK_PTR,0);
   EHCI_MEM_WRITE(QH_ptr->NEXT_QTD_LINK_PTR,0);
   EHCI_MEM_WRITE(QH_ptr->ALT_NEXT_QTD_LINK_PTR,0);
   EHCI_MEM_WRITE(QH_ptr->STATUS,0);
   EHCI_MEM_WRITE(QH_ptr->BUFFER_PTR_0,0);
   EHCI_MEM_WRITE(QH_ptr->BUFFER_PTR_1,0);
   EHCI_MEM_WRITE(QH_ptr->BUFFER_PTR_2,0);
   EHCI_MEM_WRITE(QH_ptr->BUFFER_PTR_3,0);
   EHCI_MEM_WRITE(QH_ptr->BUFFER_PTR_4,0);
#else
      USB_mem_zero((void *)QH_ptr, 12*sizeof(uint32_t));
#endif // PSP_HAS_DATA_CACHE

   switch (pipe_descr_ptr->G.PIPETYPE)
   {
      case  USB_CONTROL_PIPE:
         if (pipe_descr_ptr->G.SPEED != USB_SPEED_HIGH)
         {
            control_ep_flag = 1;
         }
         item_type = EHCI_FRAME_LIST_ELEMENT_TYPE_QH;
         H_bit = (uint32_t) ((prev_QH_ptr == QH_ptr)? 1 : 0);
         data_toggle_control = 1; /* Data toggle in qTD */
         break;

      case  USB_INTERRUPT_PIPE:
         data_toggle_control = 0; /* Data toggle in QH */
         break;

      case  USB_BULK_PIPE:
         item_type = EHCI_FRAME_LIST_ELEMENT_TYPE_QH;
         H_bit = (uint32_t) ((prev_QH_ptr == QH_ptr) ? 1 : 0);
#if USBCFG_EHCI_USE_SW_TOGGLING
         data_toggle_control = 1; /* Data toggle in qTD */
#else
         data_toggle_control = 0; /* Data toggle in QH */
#endif
         break;

   }

   if (pipe_descr_ptr->G.SPEED == USB_SPEED_HIGH)
   {
      mult = pipe_descr_ptr->G.TRS_PER_UFRAME;
   }

/******************************************************************************
 PREPARE THE Fields that are same for all pipes.
*******************************************************************************/

   QH_ptr->PIPE_DESCR_FOR_THIS_QH = (void *)pipe_descr_ptr;
   pipe_descr_ptr->QH_FOR_THIS_PIPE = (void *)QH_ptr;

   EHCI_MEM_WRITE(QH_ptr->CURR_QTD_LINK_PTR,0);
   EHCI_MEM_WRITE(QH_ptr->ALT_NEXT_QTD_LINK_PTR,EHCI_QTD_T_BIT);
   EHCI_MEM_WRITE(QH_ptr->STATUS,0);
   EHCI_MEM_WRITE(QH_ptr->BUFFER_PTR_0,0);
   EHCI_MEM_WRITE(QH_ptr->BUFFER_PTR_1,0);
   EHCI_MEM_WRITE(QH_ptr->BUFFER_PTR_2,0);
   EHCI_MEM_WRITE(QH_ptr->BUFFER_PTR_3,0);
   EHCI_MEM_WRITE(QH_ptr->BUFFER_PTR_4,0);


/****************************************************************************
 copy the fields inside the QH
*****************************************************************************/


   /* Initialize the endpoint capabilities registers */
   temp = (
          ((uint32_t)pipe_descr_ptr->G.NAK_COUNT << EHCI_QH_NAK_COUNT_RL_BITS_POS) |
          (control_ep_flag << EHCI_QH_EP_CTRL_FLAG_BIT_POS) |
          (pipe_descr_ptr->G.MAX_PACKET_SIZE << EHCI_QH_MAX_PKT_SIZE_BITS_POS) |
          (H_bit << EHCI_QH_HEAD_RECLAMATION_BIT_POS) |
          (data_toggle_control << EHCI_QH_DTC_BIT_POS) |
          (pipe_descr_ptr->G.SPEED << EHCI_QH_SPEED_BITS_POS) |
          ((uint32_t)pipe_descr_ptr->G.ENDPOINT_NUMBER << EHCI_QH_EP_NUM_BITS_POS) |
          pipe_descr_ptr->G.DEVICE_ADDRESS);
   EHCI_MEM_WRITE(QH_ptr->EP_CAPAB_CHARAC1, temp);

   temp = (
          (mult << EHCI_QH_HIGH_BW_MULT_BIT_POS) |
          (pipe_descr_ptr->G.HUB_PORT_NUM << EHCI_QH_HUB_PORT_NUM_BITS_POS) |
          (pipe_descr_ptr->G.HUB_DEVICE_ADDR << EHCI_QH_HUB_ADDR_BITS_POS) |
          (split_completion_mask << EHCI_QH_SPLIT_COMPLETION_MASK_BITS_POS) |
          interrupt_schedule_mask);
   EHCI_MEM_WRITE(QH_ptr->EP_CAPAB_CHARAC2,temp);

/****************************************************************************
 Queue the transfers now by updating the pointers
*****************************************************************************/

  switch (pipe_descr_ptr->G.PIPETYPE)
  {
      case  USB_CONTROL_PIPE:
         /* Link the new Queue Head to the previous Queue Head */
         EHCI_MEM_WRITE(QH_ptr->NEXT_QTD_LINK_PTR,(uint32_t)first_QTD_ptr);
         EHCI_MEM_WRITE(QH_ptr->HORIZ_LINK_PTR,EHCI_MEM_READ((uint32_t)prev_QH_ptr->HORIZ_LINK_PTR));
         EHCI_MEM_WRITE(prev_QH_ptr->HORIZ_LINK_PTR,((uint32_t)QH_ptr | (item_type << EHCI_QH_ELEMENT_TYPE_BIT_POS)));
      break;
      case  USB_BULK_PIPE:
         /* Link the new Queue Head to the previous Queue Head */
         EHCI_MEM_WRITE(QH_ptr->NEXT_QTD_LINK_PTR,(uint32_t)first_QTD_ptr);
         EHCI_MEM_WRITE(QH_ptr->HORIZ_LINK_PTR,EHCI_MEM_READ((uint32_t)prev_QH_ptr->HORIZ_LINK_PTR));
         EHCI_MEM_WRITE(prev_QH_ptr->HORIZ_LINK_PTR,((uint32_t)QH_ptr | (item_type << EHCI_QH_ELEMENT_TYPE_BIT_POS)));
      break;
      case  USB_INTERRUPT_PIPE:
         EHCI_MEM_WRITE(QH_ptr->NEXT_QTD_LINK_PTR,EHCI_QTD_T_BIT);
         EHCI_MEM_WRITE(QH_ptr->HORIZ_LINK_PTR,EHCI_QUEUE_HEAD_POINTER_T_BIT);
      break;
  }

  return;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_init_QTD
*  Returned Value : None
*  Comments       :
*        Initialize the QTD
*END*-----------------------------------------------------------------*/
void _usb_ehci_init_QTD
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      /* [IN] the address of the QTD to initialize */
      EHCI_QTD_STRUCT_PTR              QTD_ptr,


      /* The buffer start address for the QTD */
      unsigned char                        *buffer_start_address,

      /* The token value */
      uint32_t                          token
   )
{ /* Body */

#if PSP_HAS_DATA_CACHE    // TODO change to memzero and flush
      /********************************************************
      USB Memzero does not bypass the cache and hence explicit
      setting to zero is required in software
      ********************************************************/
      EHCI_MEM_WRITE(QTD_ptr->NEXT_QTD_PTR,0);
      EHCI_MEM_WRITE(QTD_ptr->ALT_NEXT_QTD_PTR,0);
      EHCI_MEM_WRITE(QTD_ptr->TOKEN,0);
      EHCI_MEM_WRITE(QTD_ptr->BUFFER_PTR_0,0);
      EHCI_MEM_WRITE(QTD_ptr->BUFFER_PTR_1,0);
      EHCI_MEM_WRITE(QTD_ptr->BUFFER_PTR_2,0);
      EHCI_MEM_WRITE(QTD_ptr->BUFFER_PTR_3,0);
      EHCI_MEM_WRITE(QTD_ptr->BUFFER_PTR_4,0);
#else
      /* Zero the QTD. Leave the scratch pointer */
      USB_mem_zero((void *)QTD_ptr, 8*sizeof(uint32_t));
#endif // PSP_HAS_DATA_CACHE

   /* Initialize the QTD */
   QTD_ptr->SCRATCH_PTR = handle;

   /* Set the Terminate bit */
   EHCI_MEM_WRITE(QTD_ptr->NEXT_QTD_PTR,EHCI_QTD_T_BIT);

   /* Set the Terminate bit */
   EHCI_MEM_WRITE(QTD_ptr->ALT_NEXT_QTD_PTR,EHCI_QTD_T_BIT);

   EHCI_MEM_WRITE(QTD_ptr->BUFFER_PTR_0,(uint32_t)buffer_start_address);

   /* 4K apart buffer page pointers */
   EHCI_MEM_WRITE(QTD_ptr->BUFFER_PTR_1,(EHCI_MEM_READ(QTD_ptr->BUFFER_PTR_0) + 4096));
   EHCI_MEM_WRITE(QTD_ptr->BUFFER_PTR_2,(EHCI_MEM_READ(QTD_ptr->BUFFER_PTR_1) + 4096));
   EHCI_MEM_WRITE(QTD_ptr->BUFFER_PTR_3,(EHCI_MEM_READ(QTD_ptr->BUFFER_PTR_2) + 4096));
   EHCI_MEM_WRITE(QTD_ptr->BUFFER_PTR_4,(EHCI_MEM_READ(QTD_ptr->BUFFER_PTR_3) + 4096));

   EHCI_MEM_WRITE(QTD_ptr->TOKEN,token);

} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_init_Q_element
*  Returned Value : None
*  Comments       :
*        Initialize the Queue Element descriptor(s)
*END*-----------------------------------------------------------------*/
uint32_t _usb_ehci_init_Q_element
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      /* The pipe descriptor to queue */
      EHCI_PIPE_STRUCT_PTR             pipe_descr_ptr,

      /* [IN] the transfer parameters struct */
      EHCI_TR_STRUCT_PTR               current_pipe_tr_struct_ptr
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR    usb_host_ptr;
   EHCI_QTD_STRUCT_PTR               QTD_ptr, prev_QTD_ptr, first_QTD_ptr;
   uint32_t                          pid_code, data_toggle, token;
   int32_t                           total_length, qtd_length;
   unsigned char                     *buff_ptr;
#if USBCFG_EHCI_USE_SW_TOGGLING
    int32_t                           num_of_packets;
#endif

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;

   /* Get a QTD from the queue */
   EHCI_QTD_QGET(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL, QTD_ptr);

   if (!QTD_ptr) {
      return USB_STATUS_TRANSFER_IN_PROGRESS;
   } /* Endif */

   usb_host_ptr->QTD_ENTRIES--;

   /* The data 0/1 for this pipe */
   data_toggle = (uint32_t)((pipe_descr_ptr->G.NEXTDATA01 << EHCI_QTD_DATA_TOGGLE_BIT_POS));

   /* The very first QTD */
   prev_QTD_ptr = first_QTD_ptr = QTD_ptr;

   if (pipe_descr_ptr->G.PIPETYPE == USB_CONTROL_PIPE) {
      pid_code = EHCI_QTD_PID_SETUP;
      /* Setup packet is always 8 bytes */
      token = (((uint32_t)8 << EHCI_QTD_LENGTH_BIT_POS) |
         data_toggle | pid_code | EHCI_QTD_STATUS_ACTIVE);

      /* Initialize a QTD for Setup phase */
          _usb_ehci_init_QTD(handle, QTD_ptr,
             (unsigned char *)&current_pipe_tr_struct_ptr->G.setup_packet,
             token);
      QTD_ptr->RESERVED[MEM_SIZE_INDEX] = 8;
      QTD_ptr->PIPE_DESCR_FOR_THIS_QTD = (void *)pipe_descr_ptr;
      QTD_ptr->TR_FOR_THIS_QTD = (void *)current_pipe_tr_struct_ptr;

      /* Get a QTD from the queue for the data phase or a status phase */
      EHCI_QTD_QGET(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL, QTD_ptr);

      if (!QTD_ptr) {
         return USB_STATUS_TRANSFER_IN_PROGRESS;
      } /* Endif */

      usb_host_ptr->QTD_ENTRIES--;

      /* The data phase QTD chained to the Setup QTD */
      EHCI_MEM_WRITE(prev_QTD_ptr->NEXT_QTD_PTR,(uint32_t)QTD_ptr);
      data_toggle = EHCI_QTD_DATA_TOGGLE;

      if (current_pipe_tr_struct_ptr->G.SEND_PHASE) {
         /* Host to Device data phase */
         if (current_pipe_tr_struct_ptr->G.TX_LENGTH) {
            /* Anything to send ? */
            pid_code = EHCI_QTD_PID_OUT;
            token = ((current_pipe_tr_struct_ptr->G.TX_LENGTH << EHCI_QTD_LENGTH_BIT_POS) | data_toggle | pid_code | EHCI_QTD_STATUS_ACTIVE);

            /* Initialize the QTD for OUT data phase */
          #if 0 //PSP_HAS_DATA_CACHE, TODO: remove the section
              uncache_buff_ptr = USB_mem_alloc_uncached_zero(current_pipe_tr_struct_ptr->G.TX_LENGTH);
              if (!uncache_buff_ptr) {
                  printf("%s,%d USB memory allocate failed ...\n",__FUNCTION__,__LINE__);
                  return 0;
              }
              _mem_set_type(uncache_buff_ptr, MEM_TYPE_USB_HOST_DEVICE);
              USB_mem_copy(current_pipe_tr_struct_ptr->G.TX_BUFFER,
                           uncache_buff_ptr,
                           current_pipe_tr_struct_ptr->G.TX_LENGTH);
              _usb_ehci_init_QTD(handle, QTD_ptr, (unsigned char *)uncache_buff_ptr,token);
              QTD_ptr->RESERVED[MEM_UNCACHE_INDEX] = (uint32_t)uncache_buff_ptr;
              QTD_ptr->RESERVED[MEM_CACHE_INDEX] = (uint32_t)(current_pipe_tr_struct_ptr->G.TX_BUFFER);
          #else
               _usb_ehci_init_QTD(handle, QTD_ptr, current_pipe_tr_struct_ptr->G.TX_BUFFER, token);
          #endif
            QTD_ptr->RESERVED[MEM_SIZE_INDEX] = current_pipe_tr_struct_ptr->G.TX_LENGTH;
            QTD_ptr->PIPE_DESCR_FOR_THIS_QTD = (void *)pipe_descr_ptr;
            QTD_ptr->TR_FOR_THIS_QTD = (void *)current_pipe_tr_struct_ptr;

            /* Use the QTD to chain the next QTD */
            prev_QTD_ptr = QTD_ptr;

            /* Get a QTD from the queue for status phase */
            EHCI_QTD_QGET(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL, QTD_ptr);

            if (!QTD_ptr) {
               return USB_STATUS_TRANSFER_IN_PROGRESS;
            } /* Endif */

            usb_host_ptr->QTD_ENTRIES--;

            /* Chain the status phase QTD to the data phase QTD */
            EHCI_MEM_WRITE(prev_QTD_ptr->NEXT_QTD_PTR,(uint32_t)QTD_ptr);
#if PSP_HAS_DATA_CACHE
            /* Flush QTD in the case it is in cached memory area */
            if (prev_QTD_ptr < (EHCI_QTD_STRUCT_PTR)__UNCACHED_DATA_START || ((prev_QTD_ptr + sizeof(*prev_QTD_ptr)) > (EHCI_QTD_STRUCT_PTR)__UNCACHED_DATA_END))
                USB_dcache_flush_mlines((void *)prev_QTD_ptr, sizeof(*prev_QTD_ptr));
#endif
         } /* Endif */
         /* Zero length IN */
         pid_code = EHCI_QTD_PID_IN;
      } else {

         if (current_pipe_tr_struct_ptr->G.RX_LENGTH) {
            pid_code = EHCI_QTD_PID_IN;
            token = ((current_pipe_tr_struct_ptr->G.RX_LENGTH << EHCI_QTD_LENGTH_BIT_POS) |
                data_toggle | pid_code | EHCI_QTD_STATUS_ACTIVE);

            /* Initialize the QTD for the IN data phase */
            _usb_ehci_init_QTD(handle, QTD_ptr, current_pipe_tr_struct_ptr->G.RX_BUFFER, token);
            QTD_ptr->RESERVED[MEM_SIZE_INDEX] = current_pipe_tr_struct_ptr->G.RX_LENGTH;
            QTD_ptr->PIPE_DESCR_FOR_THIS_QTD = (void *)pipe_descr_ptr;
            QTD_ptr->TR_FOR_THIS_QTD = (void *)current_pipe_tr_struct_ptr;


            /* Use thie QTD to chain the next QTD */
            prev_QTD_ptr = QTD_ptr;

            /* Get a QTD from the queue for status phase */
            EHCI_QTD_QGET(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL, QTD_ptr);

            if (!QTD_ptr) {
               return USB_STATUS_TRANSFER_IN_PROGRESS;
            } /* Endif */

            usb_host_ptr->QTD_ENTRIES--;

            /* Chain the status phase QTD to the data phase QTD */
            QTD_ptr->RESERVED[MEM_SIZE_INDEX] = 0x00;
            EHCI_MEM_WRITE(prev_QTD_ptr->NEXT_QTD_PTR, (uint32_t)QTD_ptr);
#if PSP_HAS_DATA_CACHE
            /* Flush QTD in the case it is in cached memory area */
            if (prev_QTD_ptr < (EHCI_QTD_STRUCT_PTR)__UNCACHED_DATA_START ||
                    ((prev_QTD_ptr + sizeof(*prev_QTD_ptr)) > (EHCI_QTD_STRUCT_PTR)__UNCACHED_DATA_END))
                USB_dcache_flush_mlines((void *)prev_QTD_ptr, sizeof(*prev_QTD_ptr));
#endif
         } /* Endif */
         /* Zero-length OUT token */
         pid_code = EHCI_QTD_PID_OUT;
      } /* Endif */

      /* Already chained so add a qTD for status phase */
      /* Initialize the QTD for the status phase -- Zero length opposite
      ** direction packet
      */
      token = ((0 << EHCI_QTD_LENGTH_BIT_POS) |
            data_toggle | EHCI_QTD_IOC | pid_code | EHCI_QTD_STATUS_ACTIVE);
      _usb_ehci_init_QTD(handle, QTD_ptr, NULL, token);
      QTD_ptr->PIPE_DESCR_FOR_THIS_QTD = (void *)pipe_descr_ptr;
      QTD_ptr->TR_FOR_THIS_QTD = (void *)current_pipe_tr_struct_ptr;
   }
   else {

      /* Save the next data toggle based on the number of packets
      ** for this transfer
      */
      if (pipe_descr_ptr->G.DIRECTION) {
         total_length = current_pipe_tr_struct_ptr->G.TX_LENGTH;
         buff_ptr = current_pipe_tr_struct_ptr->G.TX_BUFFER;
         pid_code = EHCI_QTD_PID_OUT;
      } else {
         total_length = current_pipe_tr_struct_ptr->G.RX_LENGTH;
         buff_ptr = current_pipe_tr_struct_ptr->G.RX_BUFFER;
         pid_code = EHCI_QTD_PID_IN;
      } /* Endif */

      do {

#if USBCFG_EHCI_USE_SW_TOGGLING
         /* The data 0/1 for this pipe for next transaction */
         data_toggle = (uint32_t)((pipe_descr_ptr->NEXTDATA01 <<
            EHCI_QTD_DATA_TOGGLE_BIT_POS));
#endif
         if (total_length > VUSB_EP_MAX_LENGTH_TRANSFER) {
            /* Split OUT transaction to more shorter transactions */
            token = ((VUSB_EP_MAX_LENGTH_TRANSFER  <<
               EHCI_QTD_LENGTH_BIT_POS) | data_toggle |
               pid_code | EHCI_QTD_DEFAULT_CERR_VALUE |
               EHCI_QTD_STATUS_ACTIVE);
            qtd_length = VUSB_EP_MAX_LENGTH_TRANSFER;
            total_length -= VUSB_EP_MAX_LENGTH_TRANSFER;
         } else {
            token = ((total_length << EHCI_QTD_LENGTH_BIT_POS) |
               data_toggle | EHCI_QTD_IOC | pid_code |
               EHCI_QTD_DEFAULT_CERR_VALUE | EHCI_QTD_STATUS_ACTIVE);
            qtd_length = total_length;
            total_length = 0;
         } /* Endif */

#if USBCFG_EHCI_USE_SW_TOGGLING
         if (pipe_descr_ptr->DIRECTION) {
            /* for OUT transactions, compute DATA0 / DATA1 toggle bit */

            num_of_packets = 1 + (qtd_length - 1) / pipe_descr_ptr->MAX_PACKET_SIZE;

            pipe_descr_ptr->NEXTDATA01 ^= (num_of_packets & 0x1);
         }
         else
         {
            /* for IN transactions, the data toggle bit will be computed
               after transaction ends as we don't know how many packets
               will the device send.
            */
         }
#endif
         /* Initialize the QTD for the OUT data transaction */
         _usb_ehci_init_QTD(handle, QTD_ptr, buff_ptr, token);
         QTD_ptr->RESERVED[MEM_SIZE_INDEX] = qtd_length;
         QTD_ptr->PIPE_DESCR_FOR_THIS_QTD = (void *)pipe_descr_ptr;
         QTD_ptr->TR_FOR_THIS_QTD = (void *)current_pipe_tr_struct_ptr;
         buff_ptr += qtd_length;

         if (total_length) {

            /* Use the QTD to chain the next QTD */
            prev_QTD_ptr = QTD_ptr;

            /* Get a QTD from the queue for status phase */
            EHCI_QTD_QGET(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL, QTD_ptr);

            if (!QTD_ptr) {
               return USB_STATUS_TRANSFER_IN_PROGRESS;
            } /* Endif */

            usb_host_ptr->QTD_ENTRIES--;

            /* Chain the status phase QTD to the data phase QTD */
            EHCI_MEM_WRITE(prev_QTD_ptr->NEXT_QTD_PTR,(uint32_t) QTD_ptr);
         } /* Endif */

#if PSP_HAS_DATA_CACHE
        /* Flush QTD in the case it is in cached memory area */
        if (prev_QTD_ptr < (EHCI_QTD_STRUCT_PTR)__UNCACHED_DATA_START ||
                ((prev_QTD_ptr + sizeof(*prev_QTD_ptr)) > (EHCI_QTD_STRUCT_PTR)__UNCACHED_DATA_END))
            USB_dcache_flush_mlines((void *)prev_QTD_ptr, sizeof(*prev_QTD_ptr));
#endif
      } while (total_length); /* EndWhile */
   } /* Endif */

#if PSP_HAS_DATA_CACHE
   /* Flush QTD in the case it is in cached memory area */
   if (QTD_ptr < (EHCI_QTD_STRUCT_PTR)__UNCACHED_DATA_START ||
           ((QTD_ptr + sizeof(*QTD_ptr)) > (EHCI_QTD_STRUCT_PTR)__UNCACHED_DATA_END))
       USB_dcache_flush_mlines((void *)QTD_ptr, sizeof(*QTD_ptr));
#endif
   return ((uint32_t)first_QTD_ptr);

} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_add_xfer_to_asynch_schedule_list
*  Returned Value : USB Status
*  Comments       :
*        Queue the packet in the EHCI hardware Asynchronous schedule list
*END*-----------------------------------------------------------------*/
uint32_t _usb_ehci_add_xfer_to_asynch_schedule_list
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      /* The pipe descriptor to queue */
      EHCI_PIPE_STRUCT_PTR             pipe_descr_ptr,

      /* [IN] the transfer parameters struct */
      EHCI_TR_STRUCT_PTR               current_pipe_tr_struct_ptr
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR               usb_host_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   EHCI_QH_STRUCT_PTR                           QH_ptr=NULL;
   EHCI_QH_STRUCT_PTR                           prev_QH_ptr=NULL;
   EHCI_QH_STRUCT_PTR                           first_QH_ptr;
   EHCI_QTD_STRUCT_PTR                          first_QTD_ptr, temp_QTD_ptr;
   EHCI_PIPE_STRUCT_PTR                         pipe_for_queue = NULL;
   ACTIVE_QH_MGMT_STRUCT_PTR                    active_list_member_ptr, temp_list_ptr;
   bool                                         init_async_list = FALSE;
   bool                                         found_existing_q_head = FALSE;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->G.DEV_PTR;

   /* All Control and Bulk Transfers */
   /* First initialize all QTDs and then initialize the QHs */

   /* Initialize the QTDs for the Queue Head */
   first_QTD_ptr = (EHCI_QTD_STRUCT_PTR)_usb_ehci_init_Q_element(handle,
      pipe_descr_ptr, current_pipe_tr_struct_ptr);

   current_pipe_tr_struct_ptr->SOFAR = 0; /* nothing was transferred yet */

   /* If the Asynch Schedule is disabled then initialize a new list */
   if (((!(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD) &
          EHCI_USBCMD_ASYNC_SCHED_ENABLE)) &&
       (!(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS) &
          EHCI_STS_ASYNCH_SCHEDULE))) || (!usb_host_ptr->ACTIVE_ASYNC_LIST_PTR)) {
      init_async_list = TRUE;
   } else {
      first_QH_ptr = QH_ptr = (EHCI_QH_STRUCT_PTR)
      ((uint32_t)usb_host_ptr->ACTIVE_ASYNC_LIST_PTR->QH_PTR &
         EHCI_QH_HORIZ_PHY_ADDRESS_MASK);

      while (QH_ptr) {
         pipe_for_queue = (EHCI_PIPE_STRUCT_PTR)QH_ptr->PIPE_DESCR_FOR_THIS_QH;

         if (pipe_for_queue == pipe_descr_ptr) {
            /* found an existing QHEAD for this endpoint */
            found_existing_q_head = TRUE;
            break;
         } else {
            prev_QH_ptr = QH_ptr;
            QH_ptr = (EHCI_QH_STRUCT_PTR)(EHCI_MEM_READ((uint32_t)QH_ptr->HORIZ_LINK_PTR) &
               EHCI_QH_HORIZ_PHY_ADDRESS_MASK);
            if (((uint32_t)QH_ptr == (uint32_t)first_QH_ptr)) {
               break;
            } /* Endif */
         } /* Endif */
      } /* Endwhile */
   } /* Endif */

   if (!found_existing_q_head) {

      active_list_member_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR) USB_mem_alloc_zero(sizeof(ACTIVE_QH_MGMT_STRUCT));

      if (!active_list_member_ptr) {
         return USB_log_error(__FILE__,__LINE__,USBERR_ALLOC);
      } /* Endif */

      _mem_set_type((void *)active_list_member_ptr, MEM_TYPE_USB_HOST_DEVICE);

      active_list_member_ptr->TIME = USBCFG_EHCI_PIPE_TIMEOUT;

      active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR = NULL;

      /* Get new QH from the free QH queue */
      EHCI_QH_QGET(usb_host_ptr->QH_HEAD, usb_host_ptr->QH_TAIL, QH_ptr);

      if (!QH_ptr) {
         USB_mem_free(active_list_member_ptr);
         return USB_log_error(__FILE__,__LINE__,USBERR_ALLOC);
      } /* Endif */

      active_list_member_ptr->QH_PTR = (EHCI_QH_STRUCT_PTR)QH_ptr;

      usb_host_ptr->QH_ENTRIES--;

      /* printf("\nQH Get 0x%x, #entries=%d",QH_ptr,usb_host_ptr->QH_ENTRIES); */

      /* Not required, done by init Q head routine */
      /* pipe_descr_ptr->QH_FOR_THIS_PIPE = (void *)QH_ptr; */

      if (init_async_list) {
         prev_QH_ptr = (EHCI_QH_STRUCT_PTR)QH_ptr;
         usb_host_ptr->ACTIVE_ASYNC_LIST_PTR = active_list_member_ptr;
      } else {
         temp_list_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR;
         while (temp_list_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR) {
            temp_list_ptr = temp_list_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
         } /* EndWhile */
         temp_list_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR = active_list_member_ptr;
      } /* Endif */

      /* Initialize the Queue head */
      _usb_ehci_init_Q_HEAD(handle,pipe_descr_ptr,(EHCI_QH_STRUCT_PTR)QH_ptr,
         (EHCI_QH_STRUCT_PTR)prev_QH_ptr, first_QTD_ptr);

      active_list_member_ptr->FIRST_QTD_PTR = (EHCI_QTD_STRUCT_PTR) first_QTD_ptr;

   } else {

      /* Get the pointer to the member of the active list */
      active_list_member_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR;
      while ((active_list_member_ptr != NULL) && (active_list_member_ptr->QH_PTR != QH_ptr)) {
         active_list_member_ptr = active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
      } /* EndWhile */

      if (active_list_member_ptr == NULL) /* just for safe, should not occur */
         goto synchro_error;

      active_list_member_ptr->TIME = USBCFG_EHCI_PIPE_TIMEOUT;

      /* Disable Async Schedule. This is here because of accessing QH's overlay area.
      ** See WARNING below.
      **/
      EHCI_REG_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD,EHCI_USBCMD_ASYNC_SCHED_ENABLE);
      while (EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS) & EHCI_STS_ASYNCH_SCHEDULE)
      {
      /* Wait To stop Async Schedule */
      }

      temp_QTD_ptr = (EHCI_QTD_STRUCT_PTR) active_list_member_ptr->FIRST_QTD_PTR;
      if (!((uint32_t) temp_QTD_ptr & EHCI_QH_HORIZ_PHY_ADDRESS_MASK)) {
         active_list_member_ptr->FIRST_QTD_PTR = (EHCI_QTD_STRUCT_PTR) first_QTD_ptr;
      } else {
         /* Link the qTD to the end of queue */
         while (!(EHCI_MEM_READ(temp_QTD_ptr->NEXT_QTD_PTR) & EHCI_QTD_T_BIT)) {
            temp_QTD_ptr = (EHCI_QTD_STRUCT_PTR)(EHCI_MEM_READ(temp_QTD_ptr->NEXT_QTD_PTR) & EHCI_HORIZ_PHY_ADDRESS_MASK);
         } /* EndWhile */
         EHCI_MEM_WRITE(temp_QTD_ptr->NEXT_QTD_PTR,(uint32_t)first_QTD_ptr);
      } /* Endif */
      /* Don't forget to link active_list_member with new qTD list on QH */
      /* Queue head is already on the active list. Simply queue the transfer */
      /* Queue transfer onto relevant queue head */
      if (EHCI_MEM_READ(QH_ptr->NEXT_QTD_LINK_PTR) & EHCI_QTD_T_BIT) {
         /* No alternate descriptor */
         EHCI_MEM_SET_BITS(QH_ptr->ALT_NEXT_QTD_LINK_PTR,EHCI_QTD_T_BIT);
         /* WARNING! If this happens during overlay save, then we can lose information!
         ** SOLUTION: disable async schedule so this will not happen
         */
         EHCI_MEM_WRITE(QH_ptr->NEXT_QTD_LINK_PTR,(uint32_t)first_QTD_ptr);
      }
   } /* Endif */


   if (init_async_list) {
      /* Write the QH address to the Current Async List Address */
      EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.CURR_ASYNC_LIST_ADDR,(uint32_t)QH_ptr);
   } /* Endif */

   /* Enable the Asynchronous schedule */
   EHCI_REG_SET_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD,EHCI_USBCMD_ASYNC_SCHED_ENABLE);

   return USB_OK;

synchro_error:
   return USB_log_error(__FILE__,__LINE__,USBERR_BAD_STATUS);
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_process_port_change
*  Returned Value : None
*  Comments       :
*        Process port change
*END*-----------------------------------------------------------------*/

bool _usb_ehci_process_port_change
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle        handle
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR      usb_host_ptr;
   VUSB20_REG_STRUCT_PTR               dev_ptr;
   VUSB20_REG_STRUCT_PTR               cap_dev_ptr;
   uint8_t                              i, total_port_numbers;
   uint32_t                             port_control, status;
   USB_EHCI_HOST_INIT_STRUCT_PTR       param;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;

   param = (USB_EHCI_HOST_INIT_STRUCT_PTR) usb_host_ptr->G.INIT_PARAM;

   cap_dev_ptr = (VUSB20_REG_STRUCT_PTR) param->CAP_BASE_PTR;

   dev_ptr = usb_host_ptr->G.DEV_PTR;

   total_port_numbers =
   (uint8_t)(EHCI_REG_READ(cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCS_PARAMS) &
      EHCI_HCS_PARAMS_N_PORTS);


   for (i = 0; i < total_port_numbers; i++) {
      port_control = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i]);

      if (port_control & EHCI_PORTSCX_CONNECT_STATUS_CHANGE) {
         /* Turn on the 125 usec uframe interrupt. This effectively
         ** starts the timer to count 125 usecs
         */
         EHCI_REG_SET_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR, EHCI_INTR_SOF_UFRAME_EN);
         do {
            if (port_control & EHCI_PORTSCX_CONNECT_STATUS_CHANGE) {
               usb_host_ptr->UFRAME_COUNT = 0;
               port_control = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i]);
               port_control &= ~EHCI_PORTSCX_W1C_BITS;
               port_control |= EHCI_PORTSCX_CONNECT_STATUS_CHANGE;
               EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i],port_control);
            } /* Endif */
            status = (EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS) &
                      EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR));
            if (status & EHCI_STS_SOF_COUNT) {
               /* Increment the 125 usecs count */
               usb_host_ptr->UFRAME_COUNT++;
               EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS,status);
            } /* Endif */
            port_control = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i]);
         } while (usb_host_ptr->UFRAME_COUNT != 2);
         /* Turn off the 125 usec uframe interrupt. This effectively
         ** stops the timer to count 125 usecs
         */
         EHCI_REG_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR, EHCI_INTR_SOF_UFRAME_EN);

         port_control = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i]);

         usb_host_ptr->UFRAME_COUNT = 0;

         /* We waited to check for stable current connect status */
         if (port_control & EHCI_PORTSCX_CURRENT_CONNECT_STATUS) {
            /* Attach on port i */
            /* send change report to the hub-driver */
            /* The hub driver does GetPortStatus and identifies the new connect */
            /* usb_host_ptr->ROOT_HUB_DRIVER(handle, hub_num, GET_PORT_STATUS); */
            /* reset and enable the port */
            _usb_ehci_reset_and_enable_port(handle, i);
#if PSP_MQX_CPU_IS_VYBRID
            /*enable HS disconnect detection */
            port_control = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i]);
            if(port_control & EHCI_PORTSCX_PORT_HIGH_SPEED) {
                _bsp_usb_hs_disconnect_detection_enable(usb_host_ptr);
            }
#endif
         } else {
            /* Detach on port i */
            /* send change report to the hub-driver */
            /* usb_host_ptr->ROOT_HUB_DRIVER(handle, hub_num, GET_PORT_STATUS); */

#if PSP_MQX_CPU_IS_VYBRID
            /* disable HS disconnect detection */
            _bsp_usb_hs_disconnect_detection_disable(usb_host_ptr);
#endif
            /* clear the connect status change */
            port_control = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i]);
            port_control &= ~EHCI_PORTSCX_W1C_BITS;
            port_control |= EHCI_PORTSCX_CONNECT_STATUS_CHANGE;
            EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i],port_control);

            /* Disable the asynch and periodic schedules */
            EHCI_REG_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD,(EHCI_USBCMD_ASYNC_SCHED_ENABLE | EHCI_USBCMD_PERIODIC_SCHED_ENABLE));

            if ((_usb_host_check_service((_usb_host_handle)usb_host_ptr, USB_SERVICE_DETACH, 0)) == USB_OK)
            {
               _usb_host_call_service(usb_host_ptr, USB_SERVICE_DETACH, 0);
            } else {
               /* call device detach (host pointer, speed, hub #, port #) */
               usb_dev_list_detach_device((_usb_host_handle)usb_host_ptr, 0, (uint8_t)((i + 1)));
            } /* Endif */

            if (!i) {
               return TRUE;
            } /* Endif */
         } /* Endif */
      } /* Endif */

      if (port_control & EHCI_PORTSCX_PORT_FORCE_RESUME) {
         port_control = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i]);
         port_control &= ~EHCI_PORTSCX_W1C_BITS;
         port_control &= ~EHCI_PORTSCX_PORT_FORCE_RESUME;
         _usb_host_call_service((_usb_host_handle)usb_host_ptr, USB_SERVICE_HOST_RESUME, 0);
      } /* Endif */

      if ((usb_host_ptr->IS_RESETTING != FALSE) &&
         (port_control & EHCI_PORTSCX_PORT_ENABLE))
      {
         usb_host_ptr->IS_RESETTING = FALSE;

         /* reset process complete */
         /* Report the change to the hub driver and enumerate */
         usb_host_ptr->G.SPEED = ((port_control & VUSBHS_SPEED_MASK) >> VUSBHS_SPEED_BIT_POS);
         //usb_host_ptr->SPEED = USB_SPEED_HIGH;
         usb_host_ptr->PORT_NUM = (uint32_t)(i + 1);

         /* Now wait for reset recovery */
         usb_host_ptr->RESET_RECOVERY_TIMER = (USB_RESET_RECOVERY_DELAY*8);

         /* Turn on the 125 usec uframe interrupt. This effectively
         ** starts the timer to count 125 usecs
         */
         EHCI_REG_SET_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR, EHCI_INTR_SOF_UFRAME_EN);
      } /* Endif */

   } /* Endfor */

   return FALSE;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_reset_and_enable_port
*  Returned Value : None
*  Comments       :
*        Reset and enabled the port
*END*-----------------------------------------------------------------*/

void _usb_ehci_reset_and_enable_port
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle        handle,

      /* [IN] the port number */
      uint8_t                  port_number
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR       usb_host_ptr;
   VUSB20_REG_STRUCT_PTR               dev_ptr;
   uint32_t                             port_status_control;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;
   dev_ptr      = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->G.DEV_PTR;

   /* Check the line status bit in the PORTSC register */
   port_status_control = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[port_number]);

   port_status_control &= ~EHCI_PORTSCX_W1C_BITS;

   /* reset should wait for 100 Ms debouce period before starting*/
#ifdef __USB_OTG__
   if ((usb_otg_state_struct_ptr->STATE_STRUCT_PTR->STATE != A_SUSPEND) &&
      (usb_otg_state_struct_ptr->STATE_STRUCT_PTR->STATE != B_WAIT_ACON))
#endif

   {
      _mqx_int i;

      for (i = 0; i < USB_DEBOUNCE_DELAY; i++) {
         _usb_host_delay(handle, 1);    //wait 1 msec

         /* Check the line status bit in the PORTSC register */
         if (!(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[port_number]) & EHCI_PORTSCX_CURRENT_CONNECT_STATUS))
            break;
      } /* Endfor */
   }

   usb_host_ptr->IS_RESETTING = TRUE;

   /*
   ** Start the reset process
   */
   EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[port_number] ,(port_status_control | EHCI_PORTSCX_PORT_RESET));

   /* Wait for Reset complete */
   do
   {
       port_status_control = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[port_number]);
   } while (port_status_control & EHCI_PORTSCX_PORT_RESET);
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_process_reset_recovery_done
*  Returned Value : None
*  Comments       :
*        Reset and enabled the port
*END*-----------------------------------------------------------------*/

void _usb_host_process_reset_recovery_done
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle        handle
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR      usb_host_ptr;
   VUSB20_REG_STRUCT_PTR               dev_ptr;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->G.DEV_PTR;

   /* call device attach (host pointer, speed, hub #, port #) */
   if ((_usb_host_check_service((_usb_host_handle)usb_host_ptr, USB_SERVICE_ATTACH, usb_host_ptr->G.SPEED)) == USB_OK)
   {
      _usb_host_call_service(usb_host_ptr, USB_SERVICE_ATTACH, usb_host_ptr->G.SPEED);
   } else {
      usb_dev_list_attach_device((_usb_host_handle)usb_host_ptr, (uint8_t)(usb_host_ptr->G.SPEED), 0, (uint8_t)(usb_host_ptr->PORT_NUM));
   } /* Endif */
   /* Turn off the 125 usec uframe interrupt. This effectively
   ** stops the timer to count 125 usecs
   */
   EHCI_REG_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR, EHCI_INTR_SOF_UFRAME_EN);
} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_isr
*  Returned Value : None
*  Comments       :
*        Service all the interrupts in the VUSB1.1 hardware
*END*-----------------------------------------------------------------*/

void _usb_ehci_isr
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle        handle
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR      usb_host_ptr;
   VUSB20_REG_STRUCT_PTR               dev_ptr;
   uint32_t                            status;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;

   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->G.DEV_PTR;

   /* We use a while loop to process all the interrupts while we are in the
   ** loop so that we don't miss any interrupts
   */
   while (TRUE) {

      /* Check if any of the enabled interrupts are asserted */
      status = (EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS) &
                EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR));

      if (!status) {
         /* No more interrupts pending to be serviced */
         break;
      } /* Endif */

      /* Clear all enabled interrupts */
      EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS,status);

      if (status & EHCI_STS_SOF_COUNT) {
         /* Waiting for an interval of 10 ms for reset recovery */
         if (usb_host_ptr->RESET_RECOVERY_TIMER) {
            usb_host_ptr->RESET_RECOVERY_TIMER--;
            if (!usb_host_ptr->RESET_RECOVERY_TIMER) {
               _usb_host_process_reset_recovery_done((_usb_host_handle)usb_host_ptr);
            } /* Endif */
         } /* Endif */
      } /* Endif */

      if (status & EHCI_STS_ASYNCH_ADVANCE) {
         /* Process the asynch advance */
      } /* Endif */

      if (status & EHCI_STS_HOST_SYS_ERROR) {
         /* Host system error. Controller halted. Inform the upper layers */
         _usb_host_call_service((_usb_host_handle)usb_host_ptr, USB_SERVICE_SYSTEM_ERROR, 0);
      } /* Endif */

      if (status & EHCI_STS_FRAME_LIST_ROLLOVER) {
         /* Process frame list rollover */
      } /* Endif */

      if (status & EHCI_STS_RECLAIMATION) {
         /* Process reclaimation */
      } /* Endif */

      if (status & EHCI_STS_NAK) {
         _usb_ehci_process_tr_complete((_usb_host_handle)usb_host_ptr);
      }

      if (status & EHCI_STS_PORT_CHANGE) {
         /* Process the port change detect */
         if (_usb_ehci_process_port_change((_usb_host_handle)usb_host_ptr)) {
            /* There was a detach on port 0 so we should return */
            return;
         } /* Endif */
         /* Should return if there was a detach on OTG port */
      } /* Endif */

      if (status & (EHCI_STS_USB_INTERRUPT | EHCI_STS_USB_ERROR)) {
         /* Process the USB transaction completion and transaction error
         ** interrupt
         */
         _usb_ehci_process_tr_complete((_usb_host_handle)usb_host_ptr);
         #ifdef DEBUG_INFO
            printf("TR completed\n");
         #endif

      } /* Endif */

      if (status & EHCI_STS_TIMER0) {
         _usb_ehci_process_timer((_usb_host_handle)usb_host_ptr);
      }

   } /* EndWhile */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_preinit
*  Returned Value : error or USB_OK
*  Comments       :
*        Allocate the structures for EHCI
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_ehci_preinit(_usb_host_handle *handle) {
    USB_EHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR) USB_mem_alloc_zero(sizeof(USB_EHCI_HOST_STATE_STRUCT));
    EHCI_PIPE_STRUCT_PTR p, pp;
    int i;

    if (NULL != usb_host_ptr) {
        /* Allocate the USB Host Pipe Descriptors */
        usb_host_ptr->G.PIPE_DESCRIPTOR_BASE_PTR = (PIPE_STRUCT_PTR)USB_mem_alloc(sizeof(EHCI_PIPE_STRUCT) * USBCFG_MAX_PIPES);
        if (usb_host_ptr->G.PIPE_DESCRIPTOR_BASE_PTR == NULL) {
            USB_mem_free(usb_host_ptr);

            return USBERR_ALLOC;
        }
        USB_mem_zero(usb_host_ptr->G.PIPE_DESCRIPTOR_BASE_PTR, sizeof(EHCI_PIPE_STRUCT) * USBCFG_MAX_PIPES);

        p = (EHCI_PIPE_STRUCT_PTR) usb_host_ptr->G.PIPE_DESCRIPTOR_BASE_PTR;
        pp = NULL;
        for (i = 0; i < USBCFG_MAX_PIPES; i++) {
           if (pp != NULL) {
              pp->G.NEXT = (PIPE_STRUCT_PTR) p;
           }
           pp = p;
           p++;
        }

        usb_host_ptr->G.PIPE_SIZE = sizeof(EHCI_PIPE_STRUCT);
        usb_host_ptr->G.TR_SIZE = sizeof(EHCI_TR_STRUCT);

    } /* Endif */

    *handle = (_usb_host_handle) usb_host_ptr;
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_init
*  Returned Value : error or USB_OK
*  Comments       :
*        Initialize the VUSB_HS controller
*END*-----------------------------------------------------------------*/

USB_STATUS  _usb_ehci_init
   (
      /* the USB Host State structure */
      _usb_host_handle     handle
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR               usb_host_ptr;
   USB_EHCI_HOST_INIT_STRUCT_PTR                param;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   VUSB20_REG_STRUCT_PTR                        cap_dev_ptr;
   EHCI_QH_STRUCT_PTR                           QH_ptr;
   EHCI_QTD_STRUCT_PTR                          QTD_ptr;
   uint8_t                                       vector;
   uint32_t                                      i, frame_list_size_bits;
#ifndef __USB_OTG__
   uint32_t                                      port_control[16];
   uint8_t                                       portn = 0;
#endif
   uint32_t                                  total_non_periodic_memory = 0;
   void                                      *non_periodic_mem_ptr = NULL;
   uint32_t                                  total_periodic_memory = 0;
   void                                      *periodic_mem_ptr = NULL;
   uint32_t                                  endian;

   #if USBCFG_MEMORY_ENDIANNESS == MQX_LITTLE_ENDIAN
      endian = 0;
   #else
      endian = VUSBHS_MODE_BIG_ENDIAN;
   #endif

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;
   param = (USB_EHCI_HOST_INIT_STRUCT_PTR) usb_host_ptr->G.INIT_PARAM;

   if (param == NULL)
       return USBERR_INIT_DATA;

   cap_dev_ptr  = (VUSB20_REG_STRUCT_PTR) param->CAP_BASE_PTR;

   /* timer_dev_ptr = (VUSB20_REG_STRUCT_PTR) param->TIMER_BASE_PTR;*/

   /* Get the base address of the VUSB_HS registers */
   usb_host_ptr->G.DEV_PTR = (void *) ((uint32_t)cap_dev_ptr +
                                      (EHCI_REG_READ(cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.CAPLENGTH_HCIVER) &
                                       EHCI_CAP_LEN_MASK));

#ifndef __USB_OTG__
   switch (param->SPEED) {
      case BSP_EHCI_HS:
         usb_host_ptr->G.SPEED = USB_SPEED_HIGH;
         break;
      case BSP_EHCI_FS:
      default:
         usb_host_ptr->G.SPEED = USB_SPEED_FULL;
         break;
   }
#endif

   dev_ptr = usb_host_ptr->G.DEV_PTR;

   /* Stop the controller */
   EHCI_REG_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD,EHCI_CMD_RUN_STOP);

   /* Configure the VUSBHS has a host controller */
   EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_MODE,VUSBHS_MODE_CTRL_MODE_HOST);
   EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_MODE,VUSBHS_MODE_CTRL_MODE_HOST | endian);

   /* Stop the controller */
   EHCI_REG_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD,EHCI_CMD_RUN_STOP);

   /* Get the interrupt vector number for the VUSB_HS host */
   vector = param->VECTOR;

   portn = EHCI_REG_READ(cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCS_PARAMS) & 0x0000000f;
   for (i = 0; i < portn; i++)
   {
       port_control[i] = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i]);
   }
#ifndef __USB_OTG__

   if (!(USB_install_isr(vector, _usb_ehci_isr, (void *)usb_host_ptr))) {
      return USB_log_error(__FILE__,__LINE__,USBERR_INSTALL_ISR);
   } /* Endbody */

#endif /* __USB_OTG__ */

   while (!(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS) & EHCI_STS_HC_HALTED)) {
      /* Wait for the controller to stop */
   }

   /* Reset the controller to get default values */
   EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD,EHCI_CMD_CTRL_RESET);

   while (EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD) & EHCI_CMD_CTRL_RESET) {
      /* Wait for the controller reset to complete */
   } /* EndWhile */

   /* Configure the VUSBHS has a host controller */
   EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_MODE,VUSBHS_MODE_CTRL_MODE_HOST | endian);

   EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.CTRLDSSEGMENT,0);

   /*******************************************************************
    Set the size of frame list in CMD register
   *******************************************************************/
   if (param->FRAME_LIST_SIZE > 512)
   {
       usb_host_ptr->FRAME_LIST_SIZE = 1024;
       frame_list_size_bits = EHCI_CMD_FRAME_SIZE_1024;
   }
   else if (param->FRAME_LIST_SIZE > 256)
   {
       usb_host_ptr->FRAME_LIST_SIZE = 512;
       frame_list_size_bits = EHCI_CMD_FRAME_SIZE_512;
   }
   else if (param->FRAME_LIST_SIZE > 128)
   {
       usb_host_ptr->FRAME_LIST_SIZE = 256;
       frame_list_size_bits = EHCI_CMD_FRAME_SIZE_256;
   }
   else if (param->FRAME_LIST_SIZE > 64)
   {
       usb_host_ptr->FRAME_LIST_SIZE = 128;
       frame_list_size_bits = EHCI_CMD_FRAME_SIZE_128;
   }
   else if (param->FRAME_LIST_SIZE > 32)
   {
       usb_host_ptr->FRAME_LIST_SIZE = 64;
       frame_list_size_bits = EHCI_CMD_FRAME_SIZE_64;
   }
   else if (param->FRAME_LIST_SIZE > 16)
   {
       usb_host_ptr->FRAME_LIST_SIZE = 32;
       frame_list_size_bits = EHCI_CMD_FRAME_SIZE_32;
   }
   else if (param->FRAME_LIST_SIZE > 8)
   {
       usb_host_ptr->FRAME_LIST_SIZE = 16;
       frame_list_size_bits = EHCI_CMD_FRAME_SIZE_16;
   }
   else if (param->FRAME_LIST_SIZE > 0)
   {
       usb_host_ptr->FRAME_LIST_SIZE = 8;
       frame_list_size_bits = EHCI_CMD_FRAME_SIZE_8;
   }
   else
   {
       usb_host_ptr->FRAME_LIST_SIZE = 1024;
       frame_list_size_bits = EHCI_CMD_FRAME_SIZE_1024;
   }

   if (!(EHCI_REG_READ(cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCC_PARAMS) & EHCI_HCC_PARAMS_PGM_FRM_LIST_FLAG)
        && (frame_list_size_bits != EHCI_CMD_FRAME_SIZE_1024))
   {
      /* Cannot shrink frame list size because it is unsupported by silicon vendor */
      return USB_log_error(__FILE__,__LINE__,USBERR_INIT_FAILED);
   }


   /*
   **   ALL CONTROLLER DRIVER MEMORY NEEDS are allocated here.
   */

   total_non_periodic_memory += (USBCFG_EHCI_MAX_QH_DESCRS * sizeof(EHCI_QH_STRUCT)) + 32;
   total_non_periodic_memory += (USBCFG_EHCI_MAX_QTD_DESCRS * sizeof(EHCI_QTD_STRUCT)) + 32;

   /* allocate space for uframe list bandwidth computation */
   total_periodic_memory += usb_host_ptr->FRAME_LIST_SIZE * 8 * sizeof(uint8_t);

   /*memory required by high-speed Iso transfers */
   total_periodic_memory += (USBCFG_EHCI_MAX_ITD_DESCRS * sizeof(EHCI_ITD_STRUCT)) + 32;
   total_periodic_memory +=  sizeof(LIST_NODE_STRUCT) * USBCFG_EHCI_MAX_ITD_DESCRS;
   /*memory required by full-speed Iso transfers */
   total_periodic_memory +=  (USBCFG_EHCI_MAX_SITD_DESCRS * sizeof(EHCI_SITD_STRUCT)) + 32;
   total_periodic_memory +=  sizeof(LIST_NODE_STRUCT) * USBCFG_EHCI_MAX_SITD_DESCRS;

   non_periodic_mem_ptr = USB_mem_alloc_uncached(total_non_periodic_memory);
   if (!non_periodic_mem_ptr)
   {
      return USB_log_error(__FILE__,__LINE__,USBERR_ALLOC);
   }

   USB_mem_zero(non_periodic_mem_ptr, total_non_periodic_memory);
#if PSP_HAS_DATA_CACHE
   /* memzero the whole memory */
    if ((non_periodic_mem_ptr < (void *)__UNCACHED_DATA_START) || (((uint8_t *)non_periodic_mem_ptr + total_non_periodic_memory) > (uint8_t *)__UNCACHED_DATA_END))
        USB_dcache_flush_mlines(non_periodic_mem_ptr, total_non_periodic_memory);
#endif

   /* store the memory block pointer in device handle */
   usb_host_ptr->CONTROLLER_MEMORY = non_periodic_mem_ptr;

   periodic_mem_ptr = USB_mem_alloc_uncached(total_periodic_memory);
   if (!periodic_mem_ptr)
   {
      USB_mem_free(non_periodic_mem_ptr);
      return USB_log_error(__FILE__,__LINE__,USBERR_ALLOC);
   }

   /* memzero the whole memory */
   USB_mem_zero(periodic_mem_ptr, total_periodic_memory);
#if PSP_HAS_DATA_CACHE
    if ((periodic_mem_ptr < (void *)__UNCACHED_DATA_START) || (((uint8_t *)periodic_mem_ptr + total_periodic_memory) > (uint8_t *)__UNCACHED_DATA_END))
        USB_dcache_flush_mlines(periodic_mem_ptr, total_periodic_memory);
#endif

   /* allocate space for frame list aligned at 4kB boundary */
   usb_host_ptr->PERIODIC_LIST_BASE_ADDR = USB_mem_alloc_align_uncached(sizeof(EHCI_FRAME_LIST_ELEMENT_POINTER) * usb_host_ptr->FRAME_LIST_SIZE, 4096);
   if (!usb_host_ptr->PERIODIC_LIST_BASE_ADDR)
   {
      USB_mem_free(non_periodic_mem_ptr);
      USB_mem_free(periodic_mem_ptr);
      return USB_log_error(__FILE__,__LINE__,USBERR_ALLOC);
   }
   /* TODO: mem transfer for USB_mem_alloc_align_uncached. Now it is workarounded by additional call. */
   _mem_transfer(usb_host_ptr->PERIODIC_LIST_BASE_ADDR, _task_get_id(), _mqx_get_system_task_id());
   /* TODO: remove PERIODIC_LIST_BASE_ADDR from host handle struct since it is always aligned => redundant */
   usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR = usb_host_ptr->PERIODIC_LIST_BASE_ADDR;

   /* memzero the whole memory */
   USB_mem_zero(usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR, sizeof(EHCI_FRAME_LIST_ELEMENT_POINTER) * usb_host_ptr->FRAME_LIST_SIZE);
#if PSP_HAS_DATA_CACHE
   USB_dcache_flush_mlines(usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR, sizeof(EHCI_FRAME_LIST_ELEMENT_POINTER) * usb_host_ptr->FRAME_LIST_SIZE);
#endif

   /*
   **   NON-PERIODIC MEMORY DISTRIBUTION STUFF
   */

   usb_host_ptr->ASYNC_LIST_BASE_ADDR = usb_host_ptr->QH_BASE_PTR =
   (void *) non_periodic_mem_ptr;

   usb_host_ptr->ALIGNED_ASYNCLIST_ADDR = usb_host_ptr->QH_ALIGNED_BASE_PTR =
      (void *)USB_MEM32_ALIGN((uint32_t)usb_host_ptr->ASYNC_LIST_BASE_ADDR);

   non_periodic_mem_ptr =  (uint8_t *) non_periodic_mem_ptr +
                           (USBCFG_EHCI_MAX_QH_DESCRS * sizeof(EHCI_QH_STRUCT)) + 32;

   usb_host_ptr->QTD_BASE_PTR = (EHCI_QTD_STRUCT_PTR) non_periodic_mem_ptr;

   /* Align the QTD base to 32 byte boundary */
   usb_host_ptr->QTD_ALIGNED_BASE_PTR = (EHCI_QTD_STRUCT_PTR)
               USB_MEM32_ALIGN((uint32_t)usb_host_ptr->QTD_BASE_PTR);

   non_periodic_mem_ptr =  (uint8_t *) non_periodic_mem_ptr +
   (USBCFG_EHCI_MAX_QTD_DESCRS * sizeof(EHCI_QTD_STRUCT)) + 32;

   QH_ptr = usb_host_ptr->QH_ALIGNED_BASE_PTR;

   for (i = 0; i < USBCFG_EHCI_MAX_QH_DESCRS; i++) {
      /* Set the dTD to be invalid */
      EHCI_MEM_WRITE(QH_ptr->HORIZ_LINK_PTR,EHCI_QUEUE_HEAD_POINTER_T_BIT);
      _usb_ehci_free_QH((_usb_host_handle)usb_host_ptr, (void *)QH_ptr);
      QH_ptr++;
   } /* Endfor */

   QTD_ptr = usb_host_ptr->QTD_ALIGNED_BASE_PTR;

   /* Enqueue all the QTDs */
   for (i = 0; i < USBCFG_EHCI_MAX_QTD_DESCRS; i++) {
      /* Set the QTD to be invalid */
      EHCI_MEM_WRITE(QTD_ptr->NEXT_QTD_PTR,EHCI_QTD_T_BIT);
      _usb_ehci_free_QTD((_usb_host_handle)usb_host_ptr, (void *)QTD_ptr);
      QTD_ptr++;
   } /* Endfor */



   /*
   **   BANDWIDTH MEMORY DISTRIBUTION STUFF
   */

   /*********************************************************************
   Allocating the memory to store periodic bandwidth. A periodic BW list
   is a two dimensional array with dimension (frame list size x 8 u frames).

   Also note that the following could be a large allocation of memory
   The max value stored in a location will be 125 micro seconds and so we
   use uint8_t
   *********************************************************************/

   usb_host_ptr->PERIODIC_FRAME_LIST_BW_PTR = (uint8_t *) periodic_mem_ptr;
   periodic_mem_ptr = (uint8_t *) periodic_mem_ptr +
         usb_host_ptr->FRAME_LIST_SIZE * 8 * sizeof(uint8_t);

   USB_mem_zero(usb_host_ptr->PERIODIC_FRAME_LIST_BW_PTR,
                   usb_host_ptr->FRAME_LIST_SIZE *  \
                   8 *            \
                   sizeof(uint8_t)  \
                   );


    /*make sure that periodic list is uninitialized */
    usb_host_ptr->PERIODIC_LIST_INITIALIZED = FALSE;

    /* Initialize the list of active structures to NULL initially */
    usb_host_ptr->ACTIVE_ASYNC_LIST_PTR = NULL;
    usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR = NULL;


   /*
   **  HIGH SPEED ISO TRANSFERS MEMORY ALLOCATION STUFF
   */

   usb_host_ptr->ITD_BASE_PTR = (EHCI_ITD_STRUCT_PTR) periodic_mem_ptr;
   periodic_mem_ptr = (uint8_t *) periodic_mem_ptr +
                      (USBCFG_EHCI_MAX_ITD_DESCRS * sizeof(EHCI_ITD_STRUCT)) + 32;


   /* Align the ITD base to 32 byte boundary */
   usb_host_ptr->ITD_ALIGNED_BASE_PTR = (EHCI_ITD_STRUCT_PTR)
      USB_MEM32_ALIGN((uint32_t) usb_host_ptr->ITD_BASE_PTR);


   usb_host_ptr->ITD_LIST_INITIALIZED = FALSE;

   /*****************************************************************************
     ITD QUEUING PREPARATION
   *****************************************************************************/

   /* memory for doubly link list of nodes that keep active ITDs. Head and Tail point to
   same place when list is empty */
   usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR = periodic_mem_ptr;
   usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_TAIL_PTR = periodic_mem_ptr;
   USB_mem_zero(usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR,
                   sizeof(LIST_NODE_STRUCT) * USBCFG_EHCI_MAX_ITD_DESCRS);

   periodic_mem_ptr = (uint8_t *) periodic_mem_ptr +
                      sizeof(LIST_NODE_STRUCT) * USBCFG_EHCI_MAX_ITD_DESCRS;


   /*
   **   FULL SPEED ISO TRANSFERS MEMORY ALLOCATION STUFF
   */

   /* Allocate the Split-transactions Isochronous Transfer Descriptors:
   ** 32 bytes aligned
   */
   usb_host_ptr->SITD_BASE_PTR = (EHCI_SITD_STRUCT_PTR) periodic_mem_ptr;
   periodic_mem_ptr = (uint8_t *) periodic_mem_ptr +
                      (USBCFG_EHCI_MAX_SITD_DESCRS * sizeof(EHCI_SITD_STRUCT)) + 32;


   /* Align the SITD base to 32 byte boundary */
   usb_host_ptr->SITD_ALIGNED_BASE_PTR = (EHCI_SITD_STRUCT_PTR)
      USB_MEM32_ALIGN((uint32_t)usb_host_ptr->SITD_BASE_PTR);

   usb_host_ptr->SITD_LIST_INITIALIZED = FALSE;

   /*****************************************************************************
     SITD QUEUING PREPARATION
   *****************************************************************************/

   /* memory for doubly link list of nodes that keep active SITDs. Head and Tail point to
   same place when list is empty */
   usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR = periodic_mem_ptr;
   usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR = periodic_mem_ptr;

   USB_mem_zero(usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR,
                   sizeof(LIST_NODE_STRUCT) * USBCFG_EHCI_MAX_SITD_DESCRS);

   periodic_mem_ptr = (uint8_t *) periodic_mem_ptr +
                      sizeof(LIST_NODE_STRUCT) * USBCFG_EHCI_MAX_SITD_DESCRS;

   /*
   ** HARDWARE  REGISTERS INITIALIZATION STUFF
   */


   /* 4-Gigabyte segment where all of the  interface data structures are allocated. */
   /* If no 64-bit addressing capability then this is zero */
   if ((EHCI_REG_READ(cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCC_PARAMS) &
        EHCI_HCC_PARAMS_64_BIT_ADDR_CAP)) {
      EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.CTRLDSSEGMENT,0);
   } else {
      EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.CTRLDSSEGMENT,EHCI_DATA_STRUCTURE_BASE_ADDRESS);
   } /* Endif */

#ifndef __USB_OTG__
   if (EHCI_REG_READ(cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCS_PARAMS) &
       VUSB20_HCS_PARAMS_PORT_POWER_CONTROL_FLAG) {
       for (i = 0; i < portn; i++)
       {
           port_control[i] &= ~EHCI_PORTSCX_W1C_BITS; //Do not accidentally clear w1c bits by writing 1 back
           if (usb_host_ptr->G.SPEED == USB_SPEED_FULL) {
               EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i],port_control[i] | EHCI_PORTSCX_PORT_POWER | VUSBHS_PORTSCX_PORT_PFSC);
           }
           else { //usb_host_ptr->G.SPEED == USB_SPEED_HIGH
               EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i],port_control[i] | EHCI_PORTSCX_PORT_POWER);
           }
       }
  } /* Endif */
#endif

   EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR,VUSB20_HOST_INTR_EN_BITS);

   /* start the controller, setup the frame list size */
   EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD,(EHCI_INTR_NO_THRESHOLD_IMMEDIATE | frame_list_size_bits | EHCI_CMD_RUN_STOP));

   /* route all ports to the EHCI controller */
   EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.CONFIG_FLAG,1);

   /* The GPTIMER was disabled. The idea was to use GPTIMER to cancel some transfers that were pending for data.
   ** Since the conception is not right, it was removed, but the code still remains here commented here
   ** for developer to persist backward compatibility. NOTE: GPTimer is not included in the EHCI standard.
   if (timer_dev_ptr) {
      EHCI_REG_WRITE(timer_dev_ptr->REGISTERS.TIMER_REGISTERS.GPTIMER0LD, 10000 - 1);
      EHCI_REG_WRITE(timer_dev_ptr->REGISTERS.TIMER_REGISTERS.GPTIMER0CTL, EHCIFSL_GTCTL_GPTIMER_REPEAT | EHCIFSL_GTCTL_GPTIMER_RESET | EHCIFSL_GTCTL_GPTIMER_RUN);
   }

   EHCI_REG_SET_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR,EHCI_INTR_TIMER0);
   */

   return USB_OK;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_free_QTD
*  Returned Value : void
*  Comments       :
*        Enqueues an QTD onto the free QTD ring.
*
*END*-----------------------------------------------------------------*/

void _usb_ehci_free_QTD
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle        handle,

      /* [IN] the QTD to enqueue */
      void                   *QTD_ptr
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR              usb_host_ptr;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself.
   */
   USB_lock();

   EHCI_MEM_WRITE(((EHCI_QTD_STRUCT_PTR)QTD_ptr)->NEXT_QTD_PTR,EHCI_QTD_T_BIT);

   /*
   ** Add the QTD to the free QTD queue and increment the tail to the next descriptor
   */
   EHCI_QTD_QADD(usb_host_ptr->QTD_HEAD, usb_host_ptr->QTD_TAIL,
      (EHCI_QTD_STRUCT_PTR)QTD_ptr);
   usb_host_ptr->QTD_ENTRIES++;

   USB_unlock();

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_free_QH
*  Returned Value : void
*  Comments       :
*        Enqueues a QH onto the free QH ring.
*
*END*-----------------------------------------------------------------*/

void _usb_ehci_free_QH
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle        handle,

      /* [IN] the QH to enqueue */
      void                   *QH_ptr
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR                    usb_host_ptr;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself.
   */
   USB_lock();

   /*
   ** Add the QH to the free QH queue and increment the tail to the next descriptor
   */

   EHCI_QH_QADD(usb_host_ptr->QH_HEAD, usb_host_ptr->QH_TAIL,
      (EHCI_QH_STRUCT_PTR) QH_ptr);
   usb_host_ptr->QH_ENTRIES++;

   // printf("\nQH Add 0x%x, #entries=%d",QH_ptr,usb_host_ptr->QH_ENTRIES);

   USB_unlock();

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_free_resources
*  Returned Value : none
*  Comments       :
*        Frees the controller specific resources for a given pipe
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_ehci_free_resources
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      /* The pipe descriptor to queue */
      PIPE_STRUCT_PTR                  pipe_ptr
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR               usb_host_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   ACTIVE_QH_MGMT_STRUCT_PTR                    active_list_member_ptr;
   ACTIVE_QH_MGMT_STRUCT_PTR                    prev_ptr = NULL, next_ptr = NULL;
   EHCI_QH_STRUCT_PTR                           QH_ptr, temp_QH_ptr;
   EHCI_PIPE_STRUCT_PTR                         pipe_descr_ptr = (EHCI_PIPE_STRUCT_PTR) pipe_ptr;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->G.DEV_PTR;
   QH_ptr = pipe_descr_ptr->QH_FOR_THIS_PIPE;

   if ((pipe_descr_ptr->G.PIPETYPE == USB_CONTROL_PIPE) || (pipe_descr_ptr->G.PIPETYPE == USB_BULK_PIPE))
   {
      USB_lock();

      /************************************************************
      Close the pipe on Async transfers (Bulk and Control)
      ************************************************************/
      /* Get the head of the active queue head */
      active_list_member_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR;


      while (active_list_member_ptr) {

         temp_QH_ptr = (EHCI_QH_STRUCT_PTR)
            ((uint32_t)active_list_member_ptr->QH_PTR &
               EHCI_QH_HORIZ_PHY_ADDRESS_MASK);

         next_ptr = active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;

         if (temp_QH_ptr == QH_ptr) {
            /* Found it */

            if (prev_ptr) {
               /* If this is not the first struct in the queue */
               EHCI_MEM_WRITE(prev_ptr->QH_PTR->HORIZ_LINK_PTR, EHCI_MEM_READ(QH_ptr->HORIZ_LINK_PTR));

               USB_mem_free(active_list_member_ptr);
               prev_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR = next_ptr;
            } else {
               /* If this is the first struct in the queue */
               if (next_ptr == NULL) {
                  /* If the head is the only one in the queue, disable asynchronous queue */
                  EHCI_REG_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD,EHCI_USBCMD_ASYNC_SCHED_ENABLE);
               } else {
                  /* we dont have previous pointer, so roll down to the end of list */
                  /* get last queue head */
                  while ((EHCI_MEM_READ(temp_QH_ptr->HORIZ_LINK_PTR) & EHCI_QH_HORIZ_PHY_ADDRESS_MASK) != (uint32_t) QH_ptr)
                     temp_QH_ptr = (void *) (EHCI_MEM_READ(temp_QH_ptr->HORIZ_LINK_PTR) & EHCI_QH_HORIZ_PHY_ADDRESS_MASK);

                  /* get next queue for last queue head */
                  next_ptr = active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
                  /* link queue heads in the controller */
                  EHCI_MEM_WRITE(temp_QH_ptr->HORIZ_LINK_PTR,EHCI_MEM_READ(QH_ptr->HORIZ_LINK_PTR));
               }

               USB_mem_free(active_list_member_ptr);
               usb_host_ptr->ACTIVE_ASYNC_LIST_PTR = next_ptr;
            } /* Endif */

            /* move it to free queue heads */
            _usb_ehci_free_QH(handle, (void *) QH_ptr);

            break;
         } /* Endif */

         prev_ptr = active_list_member_ptr;
         active_list_member_ptr = next_ptr;

      } /* Endwhile */

      USB_unlock();

   }
   else if (pipe_descr_ptr->G.PIPETYPE == USB_INTERRUPT_PIPE)
   {
        _usb_ehci_close_interrupt_pipe(handle, pipe_descr_ptr);
   }
   else if(pipe_descr_ptr->G.PIPETYPE == USB_ISOCHRONOUS_PIPE)
   {
        _usb_ehci_close_isochronous_pipe(handle,pipe_descr_ptr);
   }

   return USB_OK;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_process_tr_complete
*  Returned Value : None
*  Comments       :
*     Process the Transaction Done interrupt on the EHCI hardware. Note that this
*     routine should be improved later. It is needless to search all the lists
*     since interrupt will belong to only one of them at one time.
*
*END*-----------------------------------------------------------------*/

void _usb_ehci_process_tr_complete
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle           handle
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR               usb_host_ptr;
   ACTIVE_QH_MGMT_STRUCT_PTR                    active_list_member_ptr;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;

   /***************************************************************************
    SEARCH THE NON PERIODIC LIST FIRST
   ***************************************************************************/

   /* Get the head of the active queue head */
   active_list_member_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR;
   if(active_list_member_ptr)
   {
      _usb_ehci_process_qh_list_tr_complete(handle,active_list_member_ptr);
   }

   /***************************************************************************
    SEARCH THE INTERRUPT LIST
   ***************************************************************************/

   /* Get the head of the active structures for periodic list*/
   active_list_member_ptr = usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR;
   if(active_list_member_ptr)
   {
      _usb_ehci_process_qh_interrupt_tr_complete(handle,active_list_member_ptr);
   }

   /***************************************************************************
    SEARCH THE HIGH SPEED ISOCHRONOUS LIST
   ***************************************************************************/
   if(usb_host_ptr->HIGH_SPEED_ISO_QUEUE_ACTIVE)
   {
#if USBCFG_EHCI_MAX_ITD_DESCRS
      _usb_ehci_process_itd_tr_complete(handle);
#endif //USBCFG_EHCI_MAX_ITD_DESCRS
   }

   /***************************************************************************
    SEARCH THE FULL SPEED ISOCHRONOUS LIST
   ***************************************************************************/
   if(usb_host_ptr->FULL_SPEED_ISO_QUEUE_ACTIVE)
   {
#if USBCFG_EHCI_MAX_SITD_DESCRS
      _usb_ehci_process_sitd_tr_complete(handle);
#endif //USBCFG_EHCI_MAX_SITD_DESCRS
   }


} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_process_qh_list_tr_complete
*  Returned Value : None
*  Comments       :
*     Search the asynchronous or interrupt list to see which QTD had finished and
*     Process the interrupt.
*
*END*-----------------------------------------------------------------*/

void _usb_ehci_process_qh_list_tr_complete
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      ACTIVE_QH_MGMT_STRUCT_PTR        active_list_member_ptr
   )
{ /* Body */
   EHCI_QH_STRUCT_PTR                           QH_ptr;
   EHCI_QTD_STRUCT_PTR                          QTD_ptr;
   EHCI_QTD_STRUCT_PTR                          temp_QTD_ptr;
   EHCI_PIPE_STRUCT_PTR                         pipe_descr_ptr = NULL;
   EHCI_TR_STRUCT_PTR                           pipe_tr_struct_ptr = NULL;
   uint32_t                                     req_bytes = 0;
   uint32_t                                     remaining_bytes = 0;
   uint32_t                                     errors = 0, status = 0, expired;
   unsigned char                                *buffer_ptr = NULL;

   /* Check all transfer descriptors on all active queue heads */
   do {
         /* Get the queue head from the active list */
         QH_ptr = active_list_member_ptr->QH_PTR;
         /* Get the first QTD for this Queue head */
         QTD_ptr = active_list_member_ptr->FIRST_QTD_PTR;

#if PSP_HAS_DATA_CACHE
         if ((QH_ptr < (EHCI_QH_STRUCT_PTR)__UNCACHED_DATA_START) ||
                 ((QH_ptr + sizeof(EHCI_QH_STRUCT)) > (EHCI_QH_STRUCT_PTR)__UNCACHED_DATA_END))
            USB_dcache_invalidate_mlines((void *)QH_ptr,sizeof(EHCI_QH_STRUCT));
         if (QTD_ptr < (EHCI_QTD_STRUCT_PTR)__UNCACHED_DATA_START ||
                 ((QTD_ptr + sizeof(EHCI_QTD_STRUCT)) > (EHCI_QTD_STRUCT_PTR)__UNCACHED_DATA_END))
            USB_dcache_invalidate_mlines((void *)QTD_ptr,sizeof(EHCI_QTD_STRUCT));
#endif

         if (!(active_list_member_ptr->TIME))
            expired = 1;
         else
            expired = 0;

         while (!(((uint32_t)QTD_ptr) & EHCI_QTD_T_BIT))
         {
            /* This is a valid qTD */
            if (!(EHCI_MEM_READ(QTD_ptr->TOKEN) & EHCI_QTD_STATUS_ACTIVE) || expired) {
                   /* Inactive qTD is "done" or "finished" qTD */
#ifdef DEBUG_INFO
            uint32_t token = EHCI_MEM_READ(QTD_ptr->TOKEN);
            printf("QTD done Token=%x\n"
                   "  Status=%x,PID code=%x,error code=%x,page=%x,IOC=%x,Bytes=%x,Toggle=%x\n",
                   token,
                   ((token)&0xFF),
                   ((token) >> 8)&0x3,
                   ((token) >> 10) &0x3,
                   ((token) >> 12)&0x7,
                   ((token) >> 15)&0x1,
                   ((token) >> 16)&0x7FFF,
                   ((token)&EHCI_QTD_DATA_TOGGLE) >>31);
#endif

                     /* Get the pipe descriptor for this transfer */
                     pipe_descr_ptr = (EHCI_PIPE_STRUCT_PTR)QTD_ptr->PIPE_DESCR_FOR_THIS_QTD;
                     pipe_tr_struct_ptr = (EHCI_TR_STRUCT_PTR)QTD_ptr->TR_FOR_THIS_QTD;

                     /* Check for errors */
                     if (EHCI_MEM_READ(QTD_ptr->TOKEN) & EHCI_QTD_ERROR_BITS_MASK) {
                        errors |= (EHCI_MEM_READ(QTD_ptr->TOKEN) & EHCI_QTD_ERROR_BITS_MASK);
                        EHCI_MEM_CLEAR_BITS(QTD_ptr->TOKEN,EHCI_QTD_ERROR_BITS_MASK);
                        status = USBERR_TR_FAILED;
                     } /* Error */

                     /* Check if STALL or endpoint halted because of errors */
                     if (EHCI_MEM_READ(QTD_ptr->TOKEN) & EHCI_QTD_STATUS_HALTED) {
                        errors |= EHCI_QTD_STATUS_HALTED;
                        status = USBERR_ENDPOINT_STALLED;
                        EHCI_MEM_CLEAR_BITS(QTD_ptr->TOKEN,EHCI_QTD_STATUS_HALTED);
                        QH_ptr->STATUS = 0;
                     } /* Endif */

                     if (pipe_descr_ptr->G.PIPETYPE == USB_CONTROL_PIPE)
                     {

                           if (EHCI_MEM_READ(QTD_ptr->TOKEN) & EHCI_QTD_PID_SETUP)
                           {
                              pipe_tr_struct_ptr->setup_first_phase= TRUE;
                              req_bytes = remaining_bytes = 0;
                           }
                           else if (pipe_tr_struct_ptr->setup_first_phase)
                           {
                              pipe_tr_struct_ptr->setup_first_phase = FALSE;
                              if (pipe_tr_struct_ptr->G.SEND_PHASE)
                              {
                                 buffer_ptr = pipe_tr_struct_ptr->G.TX_BUFFER;
                                 pipe_tr_struct_ptr->G.SEND_PHASE = FALSE;
                              }
                              else
                              {
                                 buffer_ptr = pipe_tr_struct_ptr->G.RX_BUFFER;
                              } /* Endif */

                              /* Increment the total number of bytes sent/received */
                              req_bytes = QTD_ptr->RESERVED[MEM_SIZE_INDEX];
                              remaining_bytes = ((EHCI_MEM_READ(QTD_ptr->TOKEN) & EHCI_QTD_LENGTH_BIT_MASK) >> EHCI_QTD_LENGTH_BIT_POS);
                           }
                           else {
                              //handshake phase, do nothing and setup that we have not received any data in this phase
                              req_bytes = remaining_bytes = 0;
                           }
                     }
                     else
                     {
                           req_bytes = QTD_ptr->RESERVED[MEM_SIZE_INDEX];
                           remaining_bytes = ((EHCI_MEM_READ(QTD_ptr->TOKEN) & EHCI_QTD_LENGTH_BIT_MASK) >> EHCI_QTD_LENGTH_BIT_POS);

                           if (pipe_descr_ptr->G.DIRECTION) {
                              buffer_ptr = pipe_tr_struct_ptr->G.TX_BUFFER;
                           } else {
#if USBCFG_EHCI_USE_SW_TOGGLING
                              int32_t received, num_of_packets;
#endif
                              buffer_ptr = pipe_tr_struct_ptr->G.RX_BUFFER;

#if USBCFG_EHCI_USE_SW_TOGGLING
                              /* For IN transfers, compute pipe_descr_ptr->NEXTDATA01 value
                                 based on number of received packets */
                              received = req_bytes - remaining_bytes;

                              num_of_packets = 1 + received / pipe_descr_ptr->MAX_PACKET_SIZE;
                              /* If we received what we asked for and it is aligned for max. packet size, then
                                 the Zero-Length Packet was not received at the end of transfer */
                              if (req_bytes && received && (received % pipe_descr_ptr->MAX_PACKET_SIZE == 0))
                                    num_of_packets--;

                              pipe_descr_ptr->NEXTDATA01 ^= (num_of_packets & 0x1);
#endif
                           } /* Endif */

                     } /* Endif */

                     pipe_tr_struct_ptr->SOFAR += (req_bytes - remaining_bytes);
                     if (EHCI_MEM_READ(QTD_ptr->TOKEN) & EHCI_QTD_IOC) {
                        /* total number of bytes sent/received */
                        pipe_tr_struct_ptr->G.STATUS = USB_STATUS_IDLE;
                     }

                     temp_QTD_ptr = QTD_ptr;
                     QTD_ptr = (EHCI_QTD_STRUCT_PTR)EHCI_MEM_READ(QTD_ptr->NEXT_QTD_PTR);

                     if (active_list_member_ptr->FIRST_QTD_PTR == temp_QTD_ptr)
                        active_list_member_ptr->FIRST_QTD_PTR = QTD_ptr;


                     /* queue the transfer only if it is a valid transfer */

                     if (EHCI_MEM_READ(QH_ptr->NEXT_QTD_LINK_PTR) & EHCI_QTD_T_BIT) {
                        /* No alternate descriptor */
                        EHCI_MEM_SET_BITS(QH_ptr->ALT_NEXT_QTD_LINK_PTR,EHCI_QTD_T_BIT);
                     } /* Endif */

                     if (pipe_tr_struct_ptr->G.STATUS == USB_STATUS_IDLE)
                     {
                        /* If there are remaining bytes, that means device returned less than expected.
                        ** We set USBERR_TR_FAILED, but still buffer_ptr is not NULL.
                        ** Application should check: if buffer_ptr is not NULL and status is USBERR_TR_FAILED,
                        ** then it means the transfer was done although not as expected (perhaps the last packet ?)
                        */
                        /* TODO: patch to have less than requested not handled as error */
                        if ((pipe_tr_struct_ptr->SOFAR < (pipe_tr_struct_ptr->G.RX_LENGTH + pipe_tr_struct_ptr->G.TX_LENGTH) && (status == USB_OK)))
                           status = USBERR_TR_FAILED;

                        /* Transfer done. Call the callback function for this
                        ** transaction if there is one (usually true).
                        */
                        if (pipe_tr_struct_ptr->G.CALLBACK != NULL)
                        {
                           #ifdef DEBUG_INFO
                           printf("_usb_ehci_process_qh_list_tr_complete: Callback\n");
                           #endif
                           pipe_tr_struct_ptr->G.CALLBACK((void *)pipe_descr_ptr,
                              pipe_tr_struct_ptr->G.CALLBACK_PARAM,
                              buffer_ptr,
                              pipe_tr_struct_ptr->SOFAR,
                              status);

                           /* If the application enqueued another request on this pipe
                           ** in this callback context then it will be at the end of the list
                           */
                        } /* Endif */
                        errors = 0;
                        status = 0;

                        /* Now mark transaction as unused so that next request can use it */
                        pipe_tr_struct_ptr->G.TR_INDEX = 0;
                     } /* Endif */

                     /* Dequeue the used QTD */
                     _usb_ehci_free_QTD(handle, (void *)temp_QTD_ptr);

                     active_list_member_ptr->TIME = USBCFG_EHCI_PIPE_TIMEOUT;
                     expired = 0;

            } /* Endif */
            else
            {
               QTD_ptr = (EHCI_QTD_STRUCT_PTR)EHCI_MEM_READ(QTD_ptr->NEXT_QTD_PTR);
            }

            USB_dcache_invalidate_mlines((void *)QTD_ptr,sizeof(EHCI_QTD_STRUCT));

         } /* EndWhile */

         #ifdef DEBUG_INFO
            printf("_usb_ehci_process_qh_list_tr_complete: next active QH\n");
         #endif

         if (expired)
            active_list_member_ptr->TIME = -1; /* it means that no QTD is in this QH */

         active_list_member_ptr =
            active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;

      } while (active_list_member_ptr);   /* While there are more queue heads in the active list */
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_delay
*  Returned Value : None
*  Comments       :
*        Delay for specified number of milliseconds.
*END*-----------------------------------------------------------------*/

void _usb_host_delay
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle        handle,

      /* [IN] time to wait in ms */
      uint32_t delay
   )
{ /* Body */
   uint32_t                               start_frame=0,curr_frame=0,diff =0,i=0,j=0;

   /* Get the frame number (not the uframe number */
   start_frame = _usb_ehci_get_frame_number(handle);

   /*wait till frame numer exceeds by delay mili seconds.*/
   do
   {
      curr_frame = _usb_ehci_get_frame_number(handle);
      i++;
      if(curr_frame != start_frame)
      {
         diff++;
         start_frame =  curr_frame;
         j++;
      }

   }while(diff < delay);

#if 0
   volatile uint32_t              delay_count;
   OTG_STRUCT_PTR                otg_reg;
   uint32_t                       otg_status;
   uint32_t                       curr_toggle_bit =0;
   uint32_t                       new_toggle_bit =0;
   volatile VUSB20_REG_STRUCT            *dev_ptr;
   uint32_t
   USB_OTG_STATE_STRUCT_PTR usb_otg_ptr = (USB_OTG_STATE_STRUCT_PTR)usb_otg_state_struct_ptr;
   int test1[101],test2[101];
   int i=0,j=0;
   otg_reg = usb_otg_ptr->OTG_REG_PTR;
   dev_ptr = usb_otg_ptr->USB_REG_PTR;
   delay_count = delay;
      j++;
      if (otg_status & VUSBHS_OTGSC_1MSIS)
      {
         i++;
         test1[i] = otg_status;
         otg_status |=  VUSBHS_OTGSC_1MSIS;
         EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.OTGSC,otg_status);
         test2[i] = otg_status;
   }
#endif
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_open_pipe
*  Returned Value : USB Status
*  Comments       :
*        When opening a pipe, this callback ensures for iso / int pipes
*        to allocate bandwidth.
*END*-----------------------------------------------------------------*/

USB_STATUS _usb_ehci_open_pipe
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      /* The pipe descriptor to queue */
      PIPE_STRUCT_PTR                  pipe_descr_ptr
   )
{ /* Body */
   USB_STATUS error;
   if ((pipe_descr_ptr->PIPETYPE == USB_INTERRUPT_PIPE) || ((pipe_descr_ptr->PIPETYPE == USB_ISOCHRONOUS_PIPE))) {
      /* Call the low-level routine to send a setup packet */
      error = _usb_host_alloc_bandwidth_call_interface (handle, pipe_descr_ptr);

      if (error != USB_OK){
         return USB_log_error(__FILE__,__LINE__,USBERR_BANDWIDTH_ALLOC_FAILED);
      } /* Endif */
   } /* Endif */
   return USB_OK;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_update_dev_address
*  Returned Value : USB Status
*  Comments       :
*        Update the queue in the EHCI hardware Asynchronous schedule list
*        to reflect new device address.
*END*-----------------------------------------------------------------*/

USB_STATUS _usb_ehci_update_dev_address
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      /* The pipe descriptor to queue */
      PIPE_STRUCT_PTR                  pipe_ptr
   )
{ /* Body */

   USB_EHCI_HOST_STATE_STRUCT_PTR               usb_host_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   EHCI_QH_STRUCT_PTR                           QH_ptr = NULL, QH_prev_ptr = NULL;
   uint32_t                                      saved_link;
   bool                                      active_async;
   EHCI_PIPE_STRUCT_PTR                         pipe_descr_ptr = (EHCI_PIPE_STRUCT_PTR) pipe_ptr;

//   return _usb_ehci_free_resources(handle, pipe_descr_ptr);

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->G.DEV_PTR;

   /* Get the pointer to the member of the active list */
   if (usb_host_ptr->ACTIVE_ASYNC_LIST_PTR != NULL)
      QH_prev_ptr = QH_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR->QH_PTR;
   else
      return USB_log_error(__FILE__,__LINE__,USBERR_BAD_STATUS);

   while (( EHCI_MEM_READ(QH_ptr->HORIZ_LINK_PTR) & EHCI_QH_HORIZ_PHY_ADDRESS_MASK) != (uint32_t) usb_host_ptr->ACTIVE_ASYNC_LIST_PTR->QH_PTR) {
      QH_prev_ptr = QH_ptr;
      QH_ptr = (void *) EHCI_MEM_READ((QH_ptr->HORIZ_LINK_PTR) & EHCI_QH_HORIZ_PHY_ADDRESS_MASK);
      if (QH_ptr == pipe_descr_ptr->QH_FOR_THIS_PIPE)
         break;
   } /* EndWhile */

   /* Queue head is already on the active list. Simply update the queue */
   active_async = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD) & EHCI_USBCMD_ASYNC_SCHED_ENABLE;
   /* stop async schedule */
   EHCI_REG_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD,EHCI_USBCMD_ASYNC_SCHED_ENABLE);
   saved_link = EHCI_MEM_READ(QH_ptr->HORIZ_LINK_PTR);
//    EHCI_MEM_WRITE(QH_prev_ptr->HORIZ_LINK_PTR,EHCI_MEM_READ(QH_ptr->HORIZ_LINK_PTR)); /* unlink QH */
   _usb_ehci_init_Q_HEAD(handle, pipe_descr_ptr, QH_ptr, QH_prev_ptr, (void *) EHCI_MEM_READ(QH_ptr->NEXT_QTD_LINK_PTR));
   EHCI_MEM_WRITE(QH_ptr->HORIZ_LINK_PTR,saved_link);

   if (active_async)
      EHCI_REG_SET_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD,EHCI_USBCMD_ASYNC_SCHED_ENABLE);

   return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_process_timer
*  Returned Value : USB Status
*  Comments       :
*        Updates Asynchronous list QTDs timer information. Removes QTDs
*        with timeout.
*END*-----------------------------------------------------------------*/
void _usb_ehci_process_timer
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle
   )
{
   USB_EHCI_HOST_STATE_STRUCT_PTR               usb_host_ptr;
   ACTIVE_QH_MGMT_STRUCT_PTR                    active_list_member;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;

   for (active_list_member = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR; active_list_member != NULL; active_list_member = active_list_member->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR)
   {
      if (active_list_member->TIME > 0)
         if (!--active_list_member->TIME)
            _usb_ehci_process_qh_list_tr_complete(handle, usb_host_ptr->ACTIVE_ASYNC_LIST_PTR);
   }
}
/* EOF */
