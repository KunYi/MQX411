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
*   This file contains the low-level Host API functions specific to the VUSB
*   chip for interrupt transfer
*
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "hostapi.h"
#include "ehci_prv.h"

#include "ehci_bw.h"
#include "ehci_main.h"
#include "ehci_intr.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_add_interrupt_xfer_to_periodic_list
*  Returned Value : None
*  Comments       :
*        Queue the transfer in the EHCI hardware Periodic schedule list
*END*-----------------------------------------------------------------*/
uint32_t _usb_ehci_add_interrupt_xfer_to_periodic_list
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      /* The pipe descriptor to queue */
      EHCI_PIPE_STRUCT_PTR             pipe_descr_ptr,

      /* [IN] the transfer parameters struct */
      EHCI_TR_STRUCT_PTR               pipe_tr_ptr
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR   usb_host_ptr;
   VUSB20_REG_STRUCT_PTR            dev_ptr;
   ACTIVE_QH_MGMT_STRUCT_PTR        active_list_member_ptr, temp_list_ptr;

   EHCI_QH_STRUCT_PTR               QH_ptr = NULL;
   EHCI_QTD_STRUCT_PTR              first_QTD_ptr, temp_QTD_ptr;
   uint32_t                         cmd_val,sts_val;
   bool                             found_existing_q_head = FALSE;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->G.DEV_PTR;

   /****************************************************************************
   QTD MAKING
   ****************************************************************************/

   /* Initialize the QTDs for the Queue Head */
   first_QTD_ptr = (EHCI_QTD_STRUCT_PTR)_usb_ehci_init_Q_element(
                                          handle,pipe_descr_ptr, pipe_tr_ptr
                                          );

#ifdef DEBUG_INFO
      {
         uint32_t token = EHCI_MEM_READ(first_QTD_ptr->TOKEN);

         printf("QTD queued Top QTD Token=%x\n"
            "   Status=%x,PID code=%x,error code=%x,page=%x,IOC=%x,Bytes=%x,Toggle=%x\n",
          token
          (token&0xFF),
          (token >> 8)&0x3,
          (token >> 10) &0x3,
          (token >> 12)&0x7,
          (token >> 15)&0x1,
          (token >> 16)&0x7FFF,
          (token&EHCI_QTD_DATA_TOGGLE) >>31);
      }
#endif


   /****************************************************************************
   Obtain the QH for this pipe
   ****************************************************************************/
   QH_ptr = (EHCI_QH_STRUCT_PTR) pipe_descr_ptr->QH_FOR_THIS_PIPE;


   /****************************************************************************
   Ensure that this QH is in the list of active QHs for interrupt pipe
   ****************************************************************************/

   /******************************************************************
   If active list does not exist, we make a new list and this is the
   first member of the list. Otherwise we append the list at the end.
   *******************************************************************/
   if(usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR == NULL)
   {

      active_list_member_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR)USB_mem_alloc_zero(sizeof(ACTIVE_QH_MGMT_STRUCT));

      if (!active_list_member_ptr)
      {
         return USB_log_error(__FILE__,__LINE__,USBERR_ALLOC);
      }

      _mem_set_type((void *)active_list_member_ptr, MEM_TYPE_USB_INTR_TMP_BUFF);
      usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR = active_list_member_ptr;

      /****************************************************************
      Connect the QH with the active list
      ****************************************************************/
      active_list_member_ptr->QH_PTR = (EHCI_QH_STRUCT_PTR)QH_ptr;
      active_list_member_ptr->FIRST_QTD_PTR = (EHCI_QTD_STRUCT_PTR)first_QTD_ptr;

      /****************************************************************
      Connect the QH with the QTD
      ****************************************************************/
      EHCI_MEM_WRITE(QH_ptr->ALT_NEXT_QTD_LINK_PTR,EHCI_QTD_T_BIT);
      EHCI_MEM_WRITE(QH_ptr->NEXT_QTD_LINK_PTR,(uint32_t)first_QTD_ptr);

   }
   else
   {

      /****************************************************************
      search the list to find if this QH aleady exists in the list. If
      not, allocate a new list member and add to the list or else move
      on with no action.
      ****************************************************************/

      temp_list_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR) \
                        usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR;


       while (temp_list_ptr!=NULL) {
         if(temp_list_ptr->QH_PTR == QH_ptr)
         {
            found_existing_q_head = TRUE;
            break;
         }
         if (temp_list_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR == NULL)
            break;

         temp_list_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR) temp_list_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
      }

      if (temp_list_ptr==NULL) {
         return USB_log_error(__FILE__,__LINE__,USBERR_ERROR);
      }


      /****************************************************************
      If no QH not found a new list memeber or connect QTDs to the existing one
      ****************************************************************/


      if(!found_existing_q_head)
      {

         active_list_member_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR)USB_mem_alloc_zero(sizeof(ACTIVE_QH_MGMT_STRUCT));

         if (!active_list_member_ptr)
         {
            return USB_log_error(__FILE__,__LINE__,USBERR_ALLOC);
         }

         _mem_set_type((void *)active_list_member_ptr, MEM_TYPE_USB_INTR_TMP_BUFF);
         temp_list_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR = active_list_member_ptr;

         /****************************************************************
         Connect the QH with the active list
         ****************************************************************/
         active_list_member_ptr->QH_PTR = (EHCI_QH_STRUCT_PTR)QH_ptr;
         active_list_member_ptr->FIRST_QTD_PTR = (EHCI_QTD_STRUCT_PTR)first_QTD_ptr;

         /****************************************************************
         Connect the QH with the QTD
         ****************************************************************/
         EHCI_MEM_WRITE(QH_ptr->ALT_NEXT_QTD_LINK_PTR,EHCI_QTD_T_BIT);
         EHCI_MEM_WRITE(QH_ptr->NEXT_QTD_LINK_PTR,(uint32_t)first_QTD_ptr);

      }
      else
      {
         /****************************************************************
         update the active interrupt list now.
         ****************************************************************/
         temp_QTD_ptr = (EHCI_QTD_STRUCT_PTR)temp_list_ptr->FIRST_QTD_PTR;

         if (( ((uint32_t)temp_QTD_ptr) & EHCI_QTD_T_BIT) || (temp_QTD_ptr == NULL)) {
            temp_list_ptr->FIRST_QTD_PTR = (EHCI_QTD_STRUCT_PTR)first_QTD_ptr;
         } else {
            while (!(EHCI_MEM_READ(temp_QTD_ptr->NEXT_QTD_PTR) & EHCI_QTD_T_BIT)) {
               temp_QTD_ptr = (EHCI_QTD_STRUCT_PTR)EHCI_MEM_READ(temp_QTD_ptr->NEXT_QTD_PTR);
            }

            EHCI_MEM_WRITE(temp_QTD_ptr->NEXT_QTD_PTR,(uint32_t)first_QTD_ptr);

         } /*else*/

         /****************************************************************
         This case is arrived when the QH is active and there is a
         possibility that it may also have active QTDs.
         ****************************************************************/
         if (EHCI_MEM_READ(QH_ptr->NEXT_QTD_LINK_PTR) & EHCI_QTD_T_BIT) {
            EHCI_MEM_WRITE(QH_ptr->ALT_NEXT_QTD_LINK_PTR,EHCI_QTD_T_BIT);
            EHCI_MEM_WRITE(QH_ptr->NEXT_QTD_LINK_PTR,(uint32_t)first_QTD_ptr);
          }



      }/*else*/

   } /*else */

#ifdef DEBUG_INFO
   {
      uint32_t token = EHCI_MEM_READ(first_QTD_ptr->TOKEN);
      printf("_usb_ehci_add_interrupt_xfer_to_periodic_list: QH =%x\n"
             "  Status=%x,PID code=%x,error code=%x,page=%x,IOC=%x,Bytes=%x,Toggle=%x\n",
             token,
             (token&0xFF),
             (token >> 8)&0x3,
             (token >> 10) &0x3,
             (token >> 12)&0x7,
             (token >> 15)&0x1,
             (token>> 16)&0x7FFF,
             (token)&EHCI_QTD_DATA_TOGGLE) >>31);
   }
#endif


   /****************************************************************************
   if periodic schedule is not already enabled, enable it.
   ****************************************************************************/
   sts_val = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS);

   if(!(sts_val & EHCI_STS_PERIODIC_SCHEDULE))
   {

         cmd_val = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD);

      /****************************************************************************
      write the address of the periodic list in to the periodic base register
      ****************************************************************************/
         EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PERIODIC_LIST_BASE_ADDR,
                        (uint32_t) usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR);

      /****************************************************************************
      wait until we can enable  the periodic schedule.
      ****************************************************************************/
         while ((cmd_val & EHCI_USBCMD_PERIODIC_SCHED_ENABLE) !=
                (EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS) & EHCI_STS_PERIODIC_SCHEDULE)) {
      }


      /****************************************************************************
      enable the schedule now.
      ****************************************************************************/

      EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD,(cmd_val | EHCI_USBCMD_PERIODIC_SCHED_ENABLE));
   }
   return USB_OK;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_process_qh_interrupt_tr_complete
*  Returned Value : None
*  Comments       :
*     Search the interrupt list to see which QTD had finished and
*     Process the interrupt.
*
*END*-----------------------------------------------------------------*/
void _usb_ehci_process_qh_interrupt_tr_complete
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
   uint32_t                                     total_req_bytes = 0;
   uint32_t                                     remaining_bytes = 0;
   uint32_t                                     errors = 0, status = 0;
   unsigned char                                *buffer_ptr = NULL;

   /* Check all transfer descriptors on all active queue heads */
   do {
         /* Get the queue head from the active list */
         QH_ptr = active_list_member_ptr->QH_PTR;
         /* Get the first QTD for this Queue head */
         QTD_ptr = active_list_member_ptr->FIRST_QTD_PTR;


         while ((!(((uint32_t)QTD_ptr) & EHCI_QTD_T_BIT)) && (QTD_ptr != NULL))
         {
            /* This is a valid qTD */


      #ifdef DEBUG_INFO
                     uint32_t token = EHCI_MEM_READ(QTD_ptr->TOKEN);
                     printf("_usb_ehci_process_qh_interrupt_tr_complete: QTD =%x\n"
                              "  Status=%x,PID code=%x,error code=%x,page=%x,IOC=%x,Bytes=%x,Toggle=%x\n",
                     token,
                     (token&0xFF),
                     (token >> 8)&0x3,
                     (token >> 10) &0x3,
                     (token >> 12)&0x7,
                     (token >> 15)&0x1,
                     (token >> 16)&0x7FFF,
                     (token&EHCI_QTD_DATA_TOGGLE) >>31);
      #endif

         if (!(EHCI_MEM_READ(QTD_ptr->TOKEN) & EHCI_QTD_STATUS_ACTIVE)) {

                     /* Get the pipe descriptor for this transfer */
                     pipe_descr_ptr = (EHCI_PIPE_STRUCT_PTR)QTD_ptr->PIPE_DESCR_FOR_THIS_QTD;
                     pipe_tr_struct_ptr = (EHCI_TR_STRUCT_PTR) QTD_ptr->TR_FOR_THIS_QTD;

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
                        EHCI_MEM_WRITE(QH_ptr->STATUS,0);
                     } /* Endif */

                     if (pipe_descr_ptr->G.DIRECTION) {
                        total_req_bytes = pipe_tr_struct_ptr->G.TX_LENGTH;
                        buffer_ptr = pipe_tr_struct_ptr->G.TX_BUFFER;
                     } else {
                        total_req_bytes = pipe_tr_struct_ptr->G.RX_LENGTH;
                        buffer_ptr = pipe_tr_struct_ptr->G.RX_BUFFER;
                     } /* Endif */
                     remaining_bytes = ((EHCI_MEM_READ(QTD_ptr->TOKEN) & EHCI_QTD_LENGTH_BIT_MASK) >> EHCI_QTD_LENGTH_BIT_POS);

   #ifdef DEBUG_INFO
                     printf("_usb_ehci_process_qh_interrupt_tr_complete: Requested Bytes = %d\
                     ,Remaining bytes = %d,",total_req_bytes,remaining_bytes);
   #endif

                     if (EHCI_MEM_READ(QTD_ptr->TOKEN) & EHCI_QTD_IOC) {
                        /* total number of bytes sent/received */
                        pipe_tr_struct_ptr->G.STATUS = USB_STATUS_IDLE;
                     } /* Endif */

                     temp_QTD_ptr = QTD_ptr;

                     QTD_ptr = (EHCI_QTD_STRUCT_PTR)EHCI_MEM_READ(QTD_ptr->NEXT_QTD_PTR);

                     /* Queue the transfer onto the relevant queue head */
                     EHCI_MEM_WRITE(QH_ptr->NEXT_QTD_LINK_PTR,(uint32_t)QTD_ptr);


                        active_list_member_ptr->FIRST_QTD_PTR = (EHCI_QTD_STRUCT_PTR)QTD_ptr;

                     /* Dequeue the used QTD */
                     _usb_ehci_free_QTD(handle, (void *)temp_QTD_ptr);

                     /* Now mark it as unused so that next request can use it */
                     pipe_tr_struct_ptr->G.TR_INDEX = 0;

                     if (pipe_tr_struct_ptr->G.STATUS == USB_STATUS_IDLE)
                     {
                        /* Transfer done. Call the callback function for this
                        ** transaction if there is one (usually true).
                        */
                        if (pipe_tr_struct_ptr->G.CALLBACK != NULL)
                        {
                          pipe_tr_struct_ptr->G.CALLBACK((void *)pipe_descr_ptr,
                              pipe_tr_struct_ptr->G.CALLBACK_PARAM,
                              buffer_ptr,
                              (total_req_bytes - remaining_bytes),
                              status);

                           /* If the application enqueued another request on this pipe
                           ** in this callback context then it will be at the end of the list
                           */
                        } /* Endif */
                        errors = 0;
                        status = 0;
                     } /* Endif */


            } /* Endif */
            else
            { /* Body */

               break;
            } /* Endbody */


         } /* EndWhile */

         active_list_member_ptr =
            active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;

      } while (active_list_member_ptr);   /* While there are more queue
                                       ** heads in the active list */


} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_close_interrupt_pipe
*  Returned Value : None
*  Comments       :
*        Close the Interrupt pipe and update the bandwidth list.
Here are the notes. In EHCI, closing an interrupt pipe involves removing
the queue head from the periodic list to make sure that none of the
frames refer to this queue head any more. It is also important to remember
that we must start removing the queue head link from a safe place which
is not currently being executed by EHCi controller. Apart from this we
should free all QTDs associated with it and QH Managements structure.
*END*-----------------------------------------------------------------*/

void _usb_ehci_close_interrupt_pipe (

      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      /* The pipe descriptor to queue */
      EHCI_PIPE_STRUCT_PTR             pipe_descr_ptr

)
{
   USB_EHCI_HOST_STATE_STRUCT_PTR               usb_host_ptr;
   EHCI_QH_STRUCT_PTR                           QH_ptr,temp_QH_ptr;
   ACTIVE_QH_MGMT_STRUCT_PTR                    active_list_member_ptr;
   ACTIVE_QH_MGMT_STRUCT_PTR                    temp_active_list_member_ptr,prev_active_list_member_ptr;
   EHCI_QTD_STRUCT_PTR                          QTD_ptr, temp_QTD_ptr;
   volatile uint32_t                            *periodic_list_base_ptr;
   volatile uint32_t                            *transfer_data_struct_ptr;
   volatile uint32_t                            *prev_transfer_data_struct_ptr;
   EHCI_ITD_STRUCT_PTR                          ITD_ptr;
   EHCI_SITD_STRUCT_PTR                         SITD_ptr;
   EHCI_FSTN_STRUCT_PTR                         FSTN_ptr;
   int                                          i;
   bool                                         unlinked;
   uint32_t                                     frame_list_size, interval, frame_number;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;

   /****************************************************************************
   Obtain the QH for this pipe
   ****************************************************************************/
   QH_ptr = (EHCI_QH_STRUCT_PTR) pipe_descr_ptr->QH_FOR_THIS_PIPE;

   if(QH_ptr == NULL) return;


   /****************************************************************************
   First Search the periodic list and unlink this QH from the list.
   ****************************************************************************/

   periodic_list_base_ptr  = (volatile uint32_t *)(usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR);

   /*******************************************************************
   Start from fram 0 till end of the list and unlink the QH if found. Note
   that we should not unlink when the QH is active but current code does
   not take this in account.
   ********************************************************************/

   /* Get frame list size and interval */
   if( pipe_descr_ptr->G.SPEED == USB_SPEED_HIGH) {
      frame_list_size = usb_host_ptr->FRAME_LIST_SIZE * 8;
   }
   else {
      frame_list_size = usb_host_ptr->FRAME_LIST_SIZE;
   }
   interval = pipe_descr_ptr->G.INTERVAL;

   for (i = 0; i < frame_list_size; i += interval) {
      if (pipe_descr_ptr->G.SPEED == USB_SPEED_HIGH) {
         frame_number = i / 8;
      }
      else {
         frame_number = i;
      }

      transfer_data_struct_ptr = (volatile uint32_t *) periodic_list_base_ptr + frame_number;
      prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
      unlinked = FALSE;

      /* updates the bandwidth list with released time */
      reclaim_band_width(
                        handle,
                        (uint32_t)i,
                        (volatile uint32_t *) transfer_data_struct_ptr,
                        pipe_descr_ptr);

      /*******************************************************************
      Traverse the frame i and unlink the QH if found in it.
      ********************************************************************/
      while ((transfer_data_struct_ptr)
             && (!(EHCI_MEM_READ(*transfer_data_struct_ptr) & EHCI_FRAME_LIST_ELEMENT_POINTER_T_BIT))
              && (unlinked == FALSE)) {

         switch (EHCI_GET_TYPE(transfer_data_struct_ptr))
         {
            case EHCI_ELEMENT_TYPE_ITD:
               ITD_ptr = (EHCI_ITD_STRUCT_PTR)(EHCI_MEM_READ(*transfer_data_struct_ptr)& EHCI_HORIZ_PHY_ADDRESS_MASK);
               prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
               transfer_data_struct_ptr = (void *) (EHCI_MEM_READ(ITD_ptr->NEXT_LINK_PTR) & EHCI_HORIZ_PHY_ADDRESS_MASK);
               break;
            case EHCI_ELEMENT_TYPE_QH:

               temp_QH_ptr = (EHCI_QH_STRUCT_PTR)(EHCI_MEM_READ(*transfer_data_struct_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK);


               if(temp_QH_ptr ==  QH_ptr)
               {
                  /*************************************************************
                  Unlink this QH now.
                  *************************************************************/
                  USB_lock();
                  unlink_periodic_data_structure_from_frame (
                                 (volatile uint32_t *) prev_transfer_data_struct_ptr,
                                 (volatile uint32_t *) transfer_data_struct_ptr);
                  USB_unlock();
                  /*exit out of loop since there can be only 1 QH of same pipe in this frame */
                  unlinked = TRUE;
               }
               prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
               transfer_data_struct_ptr = (void *) (EHCI_MEM_READ(temp_QH_ptr->HORIZ_LINK_PTR) & EHCI_HORIZ_PHY_ADDRESS_MASK);
               break;
            case EHCI_ELEMENT_TYPE_SITD:
               SITD_ptr = (EHCI_SITD_STRUCT_PTR)(EHCI_MEM_READ(*transfer_data_struct_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK);
               prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
               transfer_data_struct_ptr = (void *) (EHCI_MEM_READ(SITD_ptr->NEXT_LINK_PTR) & EHCI_HORIZ_PHY_ADDRESS_MASK);
               break;
            case EHCI_ELEMENT_TYPE_FSTN:
               FSTN_ptr = (EHCI_FSTN_STRUCT_PTR)(EHCI_MEM_READ(*transfer_data_struct_ptr) &  EHCI_HORIZ_PHY_ADDRESS_MASK);
               prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
               transfer_data_struct_ptr = (void *) (EHCI_MEM_READ(FSTN_ptr->NORMAL_PATH_LINK_PTR)  & EHCI_HORIZ_PHY_ADDRESS_MASK);
               break;
            default:
               break;
         } /* EndSwitch */

      } /*while */

   }  /* for */



   /****************************************************************************
   Search the list of active interrupt transfers to find if this QH aleady exists
   in the list. If it exists we should free QH and all QTD associated with it.
   **************************************************************************/

   active_list_member_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR)
                             usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR;

   /*point to itself to start with */
   prev_active_list_member_ptr = active_list_member_ptr;

   /*count the number of elements in the list, if None, set the list to NULL*/
   i=0;

   while (active_list_member_ptr)
   {
      if(active_list_member_ptr->QH_PTR == QH_ptr)
      {

         /* don't want to get interrupted when changing the QTD and QH list */
         USB_lock();
         /***********************************************
         Do the freeing stuff
         disable this QH so that there are no more QTDs executed.
         ***********************************************************/
         EHCI_MEM_WRITE(QH_ptr->NEXT_QTD_LINK_PTR,EHCI_QTD_T_BIT);

         /***********************************************
         free all the QTDs if any.
         ************************************************/
         QTD_ptr = active_list_member_ptr->FIRST_QTD_PTR;
         while (((!((uint32_t)QTD_ptr) & EHCI_QTD_T_BIT)) &&  (QTD_ptr != NULL)) {
            temp_QTD_ptr = QTD_ptr;
            QTD_ptr = (EHCI_QTD_STRUCT_PTR)EHCI_MEM_READ(QTD_ptr->NEXT_QTD_PTR);
           _usb_ehci_free_QTD(handle, (void *)temp_QTD_ptr);
         }

         /*free QH */
         _usb_ehci_free_QH(handle, (void *) QH_ptr);
         QH_ptr=NULL;
         /*ensure that all pointers are NULL in this structure */
         active_list_member_ptr->QH_PTR = NULL;
         active_list_member_ptr->FIRST_QTD_PTR = NULL;

         /* store the pointer temporarily before freeing it */
         temp_active_list_member_ptr = active_list_member_ptr;

         /* move on to the next member of the list */
         active_list_member_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR) \
                                   active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;

         /*connect previous to next */
         ((ACTIVE_QH_MGMT_STRUCT_PTR)prev_active_list_member_ptr)->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR = active_list_member_ptr;

         /* we should free this ACTIVE Q MGMT structure */
         USB_mem_free(temp_active_list_member_ptr);

         USB_unlock();
         break;
      }
      else
      {
         prev_active_list_member_ptr = active_list_member_ptr;
         active_list_member_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR) \
                                   active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
         i++;
      }

   }

   if(i == 0) /* if list is empty make sure to initialize the pointer */
   {
      usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR = NULL;
   }

   if (QH_ptr != NULL) {
      _usb_ehci_free_QH(handle, (void *) QH_ptr);
   }

   return;

}




/* EOF */
