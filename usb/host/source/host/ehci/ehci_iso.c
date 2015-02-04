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
*   chip for isochronous transfer.
*
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "hostapi.h"
#include "host_cnfg.h"

#include "ehci_prv.h"

#include "ehci_iso.h"

#ifdef __USB_OTG__
#include "otgapi.h"
   extern USB_OTG_STATE_STRUCT_PTR usb_otg_state_struct_ptr;
#endif

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_link_structure_in_periodic_list
*  Returned Value : None
*  Comments       :
*        This routine adds the given list of ITD or SITD structures in
*  periodic list starting at the earliest available slot in the slots (micro frames)
*  allocated for the given pipe.
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_ehci_link_structure_in_periodic_list (
      /* [IN] the USB_dev_initialize state structure */
      _usb_host_handle                 handle,
     
      /* The pipe descriptor to queue */            
      EHCI_PIPE_STRUCT_PTR             pipe_descr_ptr,
      
      uint32_t                      *struct_to_link_list,
      

      /* [IN] this is one more than actual length */
      uint32_t                          no_of_structs

)
{
   USB_EHCI_HOST_STATE_STRUCT_PTR     usb_host_ptr;
   EHCI_ITD_STRUCT_PTR                ITD_ptr;
   EHCI_SITD_STRUCT_PTR               SITD_ptr;
   uint32_t                            i;
   uint32_t                            earliest_frame_slot;
   uint32_t                            current_frame_number;
   uint32_t                        *transfer_data_struct_ptr = NULL;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;

   /* we can not have interrupts when adding data structures to the 
   hardware list */
   USB_lock();

   /**********************************************************************
   What frame number controller is executing at the moment. We start
   from the slot allocated to this pipe after this frame number.
   **********************************************************************/
   current_frame_number = _usb_ehci_get_frame_number(handle) % usb_host_ptr->FRAME_LIST_SIZE;
      
   /**********************************************************************
   Loop the periodic list and find the earliest frame higher than this
   frame number.We take  a margin of 3 frames for safety.
   **********************************************************************/
   if(pipe_descr_ptr->G.SPEED  == USB_SPEED_HIGH)
   {
            i =  pipe_descr_ptr->START_UFRAME; /* note that i is microframe number */
   
            while(i/8 < (current_frame_number + 3) % usb_host_ptr->FRAME_LIST_SIZE)
            {
               i += pipe_descr_ptr->G.INTERVAL;
            }

            /**********************************************************************
            If eariest microframe where we could schedule is more than
            the size of periodic list, we schedule at the start slot
            **********************************************************************/
            earliest_frame_slot = (i/8) % usb_host_ptr->FRAME_LIST_SIZE;

            /**********************************************************************
            Link all structures in periodic list. Note that ITds are added
            in the beginning in the list. Interrupts QH are linked to multiple
            lists so they are always added at the end.
            **********************************************************************/

            /*****************************************************************
            transfer_data_struct_ptr is the void *to earliest frame slot.
            *****************************************************************/
            transfer_data_struct_ptr = (uint32_t *)(usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR);
            transfer_data_struct_ptr += earliest_frame_slot;

            for(i = 0; i < no_of_structs; i++)
            {
                  ITD_ptr = (EHCI_ITD_STRUCT_PTR) (struct_to_link_list[i]);

                  /* store the frame list pointer */      
                  ITD_ptr->frame_list_ptr = transfer_data_struct_ptr;
                     
                  /* save the next one */
                  /*next_data_struct  =  (*transfer_data_struct_ptr);*/

                                          
                  /*restore the previous link back */
                  EHCI_MEM_WRITE(ITD_ptr->NEXT_LINK_PTR, EHCI_MEM_READ(*transfer_data_struct_ptr));

                  #ifdef  __USB_OTG__ 

                     #ifdef HOST_TESTING
                     
                     USB_mem_copy((unsigned char *)ITD_ptr,
                                &usb_otg_state_struct_ptr->ITD_QUEUE_LOG[usb_otg_state_struct_ptr->LOG_FRAME_COUNT]
                                  ,80);

                     usb_otg_state_struct_ptr->LOG_FRAME_COUNT++;
                     if(usb_otg_state_struct_ptr->LOG_FRAME_COUNT > HOST_LOG_MOD)
                     usb_otg_state_struct_ptr->LOG_FRAME_COUNT = 0;
                     #endif

                     #endif

                  /* add this one to the start of the frame list pointer */
                  EHCI_MEM_WRITE(*transfer_data_struct_ptr,
                     (uint32_t) ITD_ptr | (EHCI_FRAME_LIST_ELEMENT_TYPE_ITD << EHCI_FRAME_LIST_ELEMENT_TYPE_BIT_POS));
                  
                  earliest_frame_slot +=  (pipe_descr_ptr->G.INTERVAL / 8 > 1) ?
                                               pipe_descr_ptr->G.INTERVAL / 8 : 1;

         
                  /* if we did not reach the end of the list we move on*/
                  if(earliest_frame_slot <  usb_host_ptr->FRAME_LIST_SIZE)
                  {  
                        /* move to next frame interval */
                        transfer_data_struct_ptr += (pipe_descr_ptr->G.INTERVAL / 8 > 1) ?
                                               (pipe_descr_ptr->G.INTERVAL / 8) : 1;

                  }
                  else
                  {
                     /* start from the first frame allocated to this pipe */
                     earliest_frame_slot = pipe_descr_ptr->START_UFRAME/8;
                     transfer_data_struct_ptr = (uint32_t *)(usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR);
                     transfer_data_struct_ptr += earliest_frame_slot;
                  }
         
            } /* end for loop */

   }
   /******************************************************************************
   For full speed we connect SITDs in periodic list   
   ******************************************************************************/
   else
   {
         i =  pipe_descr_ptr->START_FRAME; /* note that i is frame nubmer */
   
         /* we take a safety margin of 3 frames (3 milliseconds) to find the right slot */
         while(i < (current_frame_number + 3) % usb_host_ptr->FRAME_LIST_SIZE)
         {
            i += pipe_descr_ptr->G.INTERVAL;
         }

         /**********************************************************************
         If eariest microframe where we could schedule is more than
         the size of periodic list, we schedule at the start slot
         **********************************************************************/
         earliest_frame_slot = i % usb_host_ptr->FRAME_LIST_SIZE;

         /**********************************************************************
         Link all structures in periodic list. Note that SITds are added
         in the beginning in the list. Interrupts QH are linked to multiple
         lists so they are always added at the end.
         **********************************************************************/

         /*****************************************************************
         transfer_data_struct_ptr is the void *to earliest frame slot.
         *****************************************************************/
         transfer_data_struct_ptr = (uint32_t *)(usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR);
         transfer_data_struct_ptr += earliest_frame_slot;

         for(i = 0; i < no_of_structs; i++)
         {

               SITD_ptr = (EHCI_SITD_STRUCT_PTR) (struct_to_link_list[i]);

               /* store the frame list pointer */      
               SITD_ptr->frame_list_ptr = transfer_data_struct_ptr;
               
               /*restore the previous link back */
               EHCI_MEM_WRITE(SITD_ptr->NEXT_LINK_PTR, EHCI_MEM_READ(*transfer_data_struct_ptr)); 

               #ifdef  __USB_OTG__ 

               #ifdef HOST_TESTING
               
               USB_mem_copy((unsigned char *)SITD_ptr,
                          &usb_otg_state_struct_ptr->SITD_QUEUE_LOG[usb_otg_state_struct_ptr->LOG_FRAME_COUNT]
                            ,44);

               usb_otg_state_struct_ptr->LOG_FRAME_COUNT++;
               if(usb_otg_state_struct_ptr->LOG_FRAME_COUNT > HOST_LOG_MOD)
               usb_otg_state_struct_ptr->LOG_FRAME_COUNT = 0;
               #endif

               #endif
               
               /* add this one to the start of the frame list pointer */
               EHCI_MEM_WRITE((*transfer_data_struct_ptr),
                        (uint32_t) SITD_ptr | (EHCI_FRAME_LIST_ELEMENT_TYPE_SITD << EHCI_FRAME_LIST_ELEMENT_TYPE_BIT_POS));

               
               earliest_frame_slot += pipe_descr_ptr->G.INTERVAL;
         
               /* if we did not reach the end of the list we move on*/
               if(earliest_frame_slot <  usb_host_ptr->FRAME_LIST_SIZE)
               {  
                     /* move to next frame interval */
                     transfer_data_struct_ptr += pipe_descr_ptr->G.INTERVAL;

               }
               else
               {
                     /* start from the first frame allocated to this pipe */
                     earliest_frame_slot = (pipe_descr_ptr->START_FRAME + 3) % usb_host_ptr->FRAME_LIST_SIZE;
			
                     
                      transfer_data_struct_ptr = (uint32_t *)(usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR);
                      transfer_data_struct_ptr += earliest_frame_slot;
               }
         
         } /* end for loop */

   }
   
   USB_unlock();
   
   return USB_OK;   
}

#if USBCFG_EHCI_MAX_ITD_DESCRS
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_add_ITD
*  Returned Value : None
*  Comments       :
*        Adds Isochronous transfer desriptors to the Periodic list for
   the given pipe.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_ehci_add_ITD
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_host_handle                 handle,
     
      /* The pipe descriptor to queue */            
      EHCI_PIPE_STRUCT_PTR             pipe_descr_ptr,
      
      /* [IN] the transfer parameters struct */
      EHCI_TR_STRUCT_PTR               pipe_tr_ptr
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR                  usb_host_ptr;
   EHCI_ITD_STRUCT_PTR                             ITD_ptr;
   uint32_t                                         total_length = 0, pg_select,remaining_length;
   uint32_t                                         length_scheduled;
   unsigned char                                    *buff_ptr,*curr_page_ptr=NULL;
   uint8_t                                          itd_direction_bit;
   uint32_t                                         no_of_itds=0,no_of_uframes=0;
   uint32_t                                         i,j,offset,next;
   uint8_t                                          max_slots_per_frame=0;
   uint32_t                                         struct_to_link_list[USBCFG_EHCI_MAX_ITD_DESCRS];
   USB_STATUS                                      status;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;   

   /*assert max packet size, interval, buffers, length etc.*/

   /******************************************************************************
   This list will contain all ITDs that will be linked in periodic list.
   We ensure that all fields inside it are NULL before we try to link it.
   ******************************************************************************/
   next = 0;
   USB_mem_zero(struct_to_link_list, USBCFG_EHCI_MAX_ITD_DESCRS * sizeof(uint32_t));    
    
   /******************************************************************************
   A client buffer request to an isochronous endpoint may span 1 to N microframes.
   When N is larger than one, system software may have to use multiple iTDs to
   read or write data with the buffer (if N is larger than eight, it must use
   more than one iTD).
   
   Each iTD can be initialized to service up to 24 transactions, organized into
   eight groups of up to three transactions each. Each group maps to one 
   micro-frame's worth of transactions. The EHCI controller does not provide
   per-transaction results within a micro-frame. It treats the per-micro-frame
   transactions as a single logical transfer.
   *******************************************************************************/

   /*******************************************************************************
   find out how big is the transaction
   *******************************************************************************/

   if (pipe_descr_ptr->G.DIRECTION == USB_SEND) 
   {
         total_length = pipe_tr_ptr->G.TX_LENGTH;
         buff_ptr = pipe_tr_ptr->G.TX_BUFFER;
         /*see EHCI specs. direction is 0 for an ISO OUT */
         itd_direction_bit = 0;
   } 
   else 
   {
         total_length = pipe_tr_ptr->G.RX_LENGTH;
         buff_ptr = pipe_tr_ptr->G.RX_BUFFER;
         itd_direction_bit = 1;
   }

   /*******************************************************************************
    How many micro frame this transaction will it take? If some bytes are remaining
    we increase it by 1.
   *******************************************************************************/
   no_of_uframes = ((total_length%(pipe_descr_ptr->G.MAX_PACKET_SIZE*pipe_descr_ptr->G.TRS_PER_UFRAME)) > 0) ?
                (total_length/(pipe_descr_ptr->G.MAX_PACKET_SIZE*pipe_descr_ptr->G.TRS_PER_UFRAME) + 1):
                (total_length/(pipe_descr_ptr->G.MAX_PACKET_SIZE*pipe_descr_ptr->G.TRS_PER_UFRAME));
                

   /**********************************************************************************
    How many micro frame slots (per frame) this transaction will take?. We cannot use all 8 
    microframe slots of a ITD because this ITD should execute only in the micro frames
    allocated to it.  
    **********************************************************************************/
   for(j = 0; j < 8; j++)
   {
      if(pipe_descr_ptr->BWIDTH_SLOTS[j])
      {
          max_slots_per_frame++;
      }
   }

   no_of_itds = ((no_of_uframes%max_slots_per_frame) > 0)?
                ((no_of_uframes/max_slots_per_frame) + 1) :
                (no_of_uframes/max_slots_per_frame);

   /* error check */             
   if (no_of_itds >= usb_host_ptr->ITD_ENTRIES)
   {
      return USB_log_error(__FILE__,__LINE__,USBERR_TRANSFER_IN_PROGRESS);
   }
   /*******************************************************************************
    Allocate as many ITDs and schedule it in periodic list   
   *******************************************************************************/
   remaining_length = total_length;
   length_scheduled = 0;
   //curr_page_ptr = buff_ptr;

   /* save on how mant ITDS are required for this transfer */   
   pipe_tr_ptr->no_of_itds_sitds = no_of_itds; 
   
   for(i = 0; i < no_of_itds; i++)
   {
         /*********************************************************************
         Get an ITD from the queue   
         **********************************************************************/
         EHCI_ITD_QGET(usb_host_ptr->ITD_HEAD, usb_host_ptr->ITD_TAIL, ITD_ptr);
   
         if (!ITD_ptr) {
            return USB_STATUS_TRANSFER_IN_PROGRESS;
         } /* Endif */

         usb_host_ptr->ITD_ENTRIES--;

         /*********************************************************************
         Add the ITD to the list of active ITDS (note that it is assumed that
         space is available in the queue because ITD_ptr was allocated and
         number of nodes available always match the number of ITD_ENTRIES
         **********************************************************************/
         EHCI_ACTIVE_QUEUE_ADD_NODE(usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_TAIL_PTR,ITD_ptr);
         
         /*********************************************************************
         Zero the ITD. Leave everything else expect first 16 int bytes (which are
         defined by EHCI specs). Fill the necessary fields.
         **********************************************************************/
         USB_mem_zero((unsigned char *)ITD_ptr,16*sizeof(uint32_t));

         /* Initialize the ITD private fields*/
         ITD_ptr->PIPE_DESCR_FOR_THIS_ITD = pipe_descr_ptr;
         ITD_ptr->PIPE_TR_DESCR_FOR_THIS_ITD = pipe_tr_ptr;
         ITD_ptr->SCRATCH_PTR = handle;
   
         /* Set the Terminate bit */
         EHCI_MEM_WRITE(ITD_ptr->NEXT_LINK_PTR,EHCI_ITD_T_BIT);
         
         /*store endpoint number and device address */
         EHCI_MEM_WRITE(ITD_ptr->BUFFER_PAGE_PTR_LIST[0],(uint32_t)((pipe_descr_ptr->G.ENDPOINT_NUMBER << EHCI_ITD_EP_BIT_POS) | pipe_descr_ptr->G.DEVICE_ADDRESS));

         /*store max packet size and direction */
         EHCI_MEM_WRITE(ITD_ptr->BUFFER_PAGE_PTR_LIST[1],(uint32_t)(pipe_descr_ptr->G.MAX_PACKET_SIZE | (itd_direction_bit << EHCI_ITD_DIRECTION_BIT_POS)));

         /*A High-Bandwidth transaction is allowed only if the length of bytes to schedule are same
         or more than mutiple bandwidth factor times the Max packet size */
         
         if(remaining_length >=  
            pipe_descr_ptr->G.TRS_PER_UFRAME * pipe_descr_ptr->G.MAX_PACKET_SIZE)
         {
            EHCI_MEM_WRITE(ITD_ptr->BUFFER_PAGE_PTR_LIST[2],pipe_descr_ptr->G.TRS_PER_UFRAME);                                                    
         }
         else
         {
            EHCI_MEM_WRITE(ITD_ptr->BUFFER_PAGE_PTR_LIST[2],1);
         }

         /*initialize the number of transactions on this ITD */
          ITD_ptr->number_of_transactions = 0;
         
         /*********************************************************************
         One ITD can address 7 4K pages. Find how many pages this transaction
         will take and schedule each of them in this ITD. It is assumed that
         buffer is large enough for the given transaction and we don't check
         for its validlity here.
         ***********************************************************************/
         pg_select = 0;
        
         /**************************************************************
         Prepare the ITD for the slots that are scheduled for it.
         **************************************************************/
         for(j = 0; j < 8; j++)
         {
           
            /*only if this micro frame is allocated to this pipe*/
            if(pipe_descr_ptr->BWIDTH_SLOTS[j])
            {
                  /*********************************************************************
                     Set the location of Page 0 (4K aligned)
                  *********************************************************************/

                  EHCI_MEM_WRITE(ITD_ptr->BUFFER_PAGE_PTR_LIST[pg_select],(EHCI_MEM_READ(ITD_ptr->BUFFER_PAGE_PTR_LIST[pg_select] ) | (uint32_t)buff_ptr) & 0xFFFFF000);
                                                      
                  offset = (uint32_t)buff_ptr & 0xFFF;
         

                  /**************************************************************
                  For IN transaction, we always use Max packet size but for an
                  OUT we can use the length remained in the buffer if it is less
                  than a maxk packet size.
                  **************************************************************/
                  if (pipe_descr_ptr->G.DIRECTION == USB_SEND) 
                  {
                     length_scheduled = 
                       (pipe_descr_ptr->G.TRS_PER_UFRAME * pipe_descr_ptr->G.MAX_PACKET_SIZE > remaining_length) ?
                       remaining_length : pipe_descr_ptr->G.TRS_PER_UFRAME * pipe_descr_ptr->G.MAX_PACKET_SIZE;
                       
                     remaining_length -=  length_scheduled;
  
                  }
                  else
                  {
                     /* on a ISO IN, we still schedule Max packet size but makes sure that remaining
                     length is set to 0 so no more slots are scheduled */
                     
                     length_scheduled =  (uint32_t)(pipe_descr_ptr->G.TRS_PER_UFRAME * pipe_descr_ptr->G.MAX_PACKET_SIZE);
                     remaining_length =  (length_scheduled > remaining_length) ?
                       0 : (remaining_length - length_scheduled);
                  }

                  /**************************************************************
                  Fill the fields inside ITD
                  **************************************************************/

                  EHCI_MEM_WRITE(ITD_ptr->TR_STATUS_CTL_LIST[j], 
                      ((length_scheduled << EHCI_ITD_LENGTH_BIT_POS) | (pg_select << EHCI_ITD_PG_SELECT_BIT_POS) | EHCI_ITD_IOC_BIT | offset));

                  EHCI_MEM_WRITE(ITD_ptr->TR_STATUS_CTL_LIST[j], EHCI_MEM_READ(ITD_ptr->TR_STATUS_CTL_LIST[j]) | EHCI_ITD_ACTIVE_BIT);

                  /*update the transaction number queued*/
                  ITD_ptr->number_of_transactions++;
                  
                  /* if remaining length is 0 we break from the loop because we
                  don't need to schedule any more slots*/
                  if(remaining_length == 0) 
                  {
                     break;
                  }
      
                  /* move buffer pointer */   
                  buff_ptr += length_scheduled;

                  /**************************************************************
                  if a page boundry has been crossed we move to next page
                  **************************************************************/
                  if(((uint32_t)buff_ptr & 0xFFFFF000) != ((uint32_t)curr_page_ptr & 0xFFFFF000))
                  {
                     pg_select++; /* move the  page count*/
               
                     curr_page_ptr = buff_ptr; /* start with this page now*/
               
                  }

            }
            
         } /*end for loop */

         EHCI_MEM_WRITE(ITD_ptr->RESERVED[MEM_SIZE_INDEX], length_scheduled);
         #if PSP_HAS_DATA_CACHE
             /* After writing to USB accessed structure, flush cache lines. Not needed if ITD_ptr is in uncached memory area. */
             //USB_dcache_flush_mlines((void *)ITD_ptr, sizeof(*ITD_ptr));
         #endif
         /**********************************************************************
         ITD ready. We store it in out list of ITDs
         **********************************************************************/
         #ifdef  __USB_OTG__ 

         #ifdef HOST_TESTING
               usb_otg_state_struct_ptr->NEXT_LINK[usb_otg_state_struct_ptr->NEXT_LINK_COUNT] =
                  EHCI_MEM_READ(ITD_ptr->NEXT_LINK_PTR);

               usb_otg_state_struct_ptr->NEXT_LINK_COUNT++;
               if(usb_otg_state_struct_ptr->NEXT_LINK_COUNT > HOST_LOG_MOD)
               usb_otg_state_struct_ptr->NEXT_LINK_COUNT = 0;

         #endif
         #endif
         
         struct_to_link_list[next] = (uint32_t)ITD_ptr;
         next++;
   }  /*end for loop*/

   /**********************************************************************
   List ready. We link it into periodic list now.
   **********************************************************************/
   status = _usb_ehci_link_structure_in_periodic_list(handle,pipe_descr_ptr,struct_to_link_list,next);
          
   if(status == USB_OK)
   {
      usb_host_ptr->HIGH_SPEED_ISO_QUEUE_ACTIVE = TRUE;
   }

   return status;
} /* EndBody */
#endif //USBCFG_EHCI_MAX_ITD_DESCRS

#if USBCFG_EHCI_MAX_SITD_DESCRS
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_add_SITD
*  Returned Value : None
*  Comments       :
*        Adds Isochronous transfer desriptors to the Periodic list for
   the given full speed pipe.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_ehci_add_SITD
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_host_handle                 handle,
     
      /* The pipe descriptor to queue */            
      EHCI_PIPE_STRUCT_PTR             pipe_descr_ptr,
      
      /* [IN] the transfer parameters struct */
      EHCI_TR_STRUCT_PTR               pipe_tr_ptr
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR                  usb_host_ptr;
   EHCI_SITD_STRUCT_PTR                            SITD_ptr;
   uint32_t                                        total_length = 0, pg_select,remaining_length;
   uint32_t                                        length_scheduled;
   unsigned char                                   *buff_ptr;
   uint8_t                                         sitd_direction_bit, tp_bits=0;
   uint32_t                                        no_of_sitds=0;
   uint32_t                                        i,next;
   uint32_t                                        struct_to_link_list[USBCFG_EHCI_MAX_SITD_DESCRS];
   USB_STATUS                                      status;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;   

   /*assert max packet size, interval, buffers, length etc.*/

   /******************************************************************************
   This list will contain all SITDs that will be linked in periodic list.
   We ensure that all fields inside it are NULL before we try to link it.
   Notice that the following could be a time consuming line if number of
   SITDS is large. Reconsider removing this line after proper testing.
   ******************************************************************************/
   next = 0;
   USB_mem_zero(struct_to_link_list, sizeof(struct_to_link_list));
    

   /*******************************************************************************
   find out how big is the transaction
   *******************************************************************************/

   if (pipe_descr_ptr->G.DIRECTION == USB_SEND) 
   {
         total_length = pipe_tr_ptr->G.TX_LENGTH;
         buff_ptr = pipe_tr_ptr->G.TX_BUFFER;
         /*see EHCI specs. direction is 0 for an ISO OUT */
         sitd_direction_bit = 0;
   } 
   else 
   {
         total_length = pipe_tr_ptr->G.RX_LENGTH;
         buff_ptr = pipe_tr_ptr->G.RX_BUFFER;
         sitd_direction_bit = 1;
   }

   /*******************************************************************************
   Full speed devices have a minimum frequency of 1 mili second so 1 SITD can 
   completely describe 1 Max_packet_size transaction. Thus number of SITD is same
   as number of max packet size transactions required for the given transfer.
   *******************************************************************************/
  
   no_of_sitds = ((total_length%pipe_descr_ptr->G.MAX_PACKET_SIZE) > 0) ?
                (total_length/pipe_descr_ptr->G.MAX_PACKET_SIZE + 1):
                (total_length/pipe_descr_ptr->G.MAX_PACKET_SIZE);
  
   /* error check , if we are going to run out of SITDS, we reject transfer here itself*/             
   if (no_of_sitds >= usb_host_ptr->SITD_ENTRIES)
   {
      return USB_log_error(__FILE__,__LINE__,USBERR_TRANSFER_IN_PROGRESS);
   }
   
   /*******************************************************************************
    Allocate as many SITDs and schedule it in periodic list   
   *******************************************************************************/
   remaining_length = total_length;
   length_scheduled = 0;

   /* save on how many SITDS are required for this transfer */   
   pipe_tr_ptr->no_of_itds_sitds = no_of_sitds;

   for(i = 0; i < no_of_sitds; i++)
   {
         /*********************************************************************
         Get an SITD from the queue   
         **********************************************************************/
         EHCI_SITD_QGET(usb_host_ptr->SITD_HEAD, usb_host_ptr->SITD_TAIL, SITD_ptr);
   
         if (!SITD_ptr) {
            return USB_STATUS_TRANSFER_IN_PROGRESS;
         } /* Endif */
   
         usb_host_ptr->SITD_ENTRIES--;

         /*********************************************************************
         Zero the SITD. Leave everything else expect first 7 int bytes (which are
         defined by EHCI specs). Fill the necessary fields.
         **********************************************************************/
         USB_mem_zero((void *)SITD_ptr, 7 * sizeof(uint32_t));

         /* Initialize the scratch pointer inside SITD */
         SITD_ptr->PIPE_DESCR_FOR_THIS_SITD = pipe_descr_ptr;
         SITD_ptr->PIPE_TR_DESCR_FOR_THIS_SITD = pipe_tr_ptr;
         SITD_ptr->SCRATCH_PTR = handle;

         /*********************************************************************
         Add the SITD to the list of active SITDS (note that it is assumed that
         space is available in the queue because SITD_ptr was allocated and
         number of nodes available always match the number of SITD_ENTRIES
         **********************************************************************/
         EHCI_ACTIVE_QUEUE_ADD_NODE(usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR,SITD_ptr);

         /* Set the Terminate bit */
         EHCI_MEM_WRITE(SITD_ptr->NEXT_LINK_PTR,EHCI_SITD_T_BIT);

         /* Set the Back pointer */
         EHCI_MEM_WRITE(SITD_ptr->BACK_LINK_PTR,EHCI_SITD_T_BIT);
         
         /*store direction, port number, hub, endpoint number device address etc.*/
         EHCI_MEM_WRITE(SITD_ptr->EP_CAPAB_CHARAC,
                     (uint32_t)((sitd_direction_bit              << EHCI_SITD_DIRECTION_BIT_POS) |
                               (pipe_descr_ptr->G.HUB_PORT_NUM    << EHCI_SITD_PORT_NUMBER_BIT_POS) |
                               (pipe_descr_ptr->G.HUB_DEVICE_ADDR << EHCI_SITD_HUB_ADDR_BIT_POS) |
                               (pipe_descr_ptr->G.ENDPOINT_NUMBER << EHCI_SITD_EP_ADDR_BIT_POS) |
                               pipe_descr_ptr->G.DEVICE_ADDRESS));

         /* store the split transaction schedule */                              
         EHCI_MEM_WRITE(SITD_ptr->UFRAME_SCHED_CTL, \
                     (uint32_t)((pipe_descr_ptr->COMPLETE_SPLIT << EHCI_SITD_COMPLETE_SPLIT_MASK_BIT_POS) | pipe_descr_ptr->START_SPLIT));

                                       
         /*store the buffer pointer 1 and offset*/
         /*EHCI_MEM_WRITE(SITD_ptr->BUFFER_PTR_0,(uint32_t)buff_ptr);*/

         /*********************************************************************
         One SITD can only carry a max of 1023 bytes so two page pointers are
         enough to describe it. We always set the page select to 0 in SITD and
         controller will move to next page and update the pg_select bit.
         ***********************************************************************/
         pg_select = 0;
        
         /**************************************************************
         For IN transaction, we always use Max packet size but for an
         OUT we can use the length remained in the buffer if it is less
         than a maxk packet size.
         **************************************************************/
         if (pipe_descr_ptr->G.DIRECTION == USB_SEND) 
         {
             length_scheduled = 
                      (pipe_descr_ptr->G.MAX_PACKET_SIZE > remaining_length) ?
                       remaining_length : pipe_descr_ptr->G.MAX_PACKET_SIZE;
                       
             remaining_length -=  length_scheduled;
             
         }
         else
         {
               /* on a ISO IN, we still schedule Max packet size but makes sure that remaining
               length is set to 0 so no more slots are scheduled */
                     
               length_scheduled =  pipe_descr_ptr->G.MAX_PACKET_SIZE;
               
               remaining_length =  (length_scheduled > remaining_length) ?
                       0 : (remaining_length - length_scheduled);
         }
         

         /**************************************************************
         Fill the fields inside SITD
         **************************************************************/
         /*store total bytes to transfer and status. Note that a total of
         1023 bytes max can be transferred on a SITD*/
         EHCI_MEM_WRITE(SITD_ptr->TRANSFER_STATE,EHCI_SITD_IOC_BIT_SET | \
                     (pg_select << EHCI_SITD_PAGE_SELECT_BIT_POS) | \
                     (length_scheduled << EHCI_SITD_TRANSFER_LENGTH_BIT_POS) |  \
                     EHCI_SITD_STATUS_ACTIVE);

         /* Depending upon if this SITD will carry less than 188 bytes, we have to code the TP bits*/
         tp_bits = (uint8_t)((length_scheduled < EHCI_START_SPLIT_MAX_BUDGET) ? EHCI_SITD_TP_ALL : EHCI_SITD_TP_BEGIN);
        
         /* next buffer pointer is at 4096 bytes ahead */
         EHCI_MEM_WRITE(SITD_ptr->RESERVED[MEM_SIZE_INDEX], length_scheduled);
         EHCI_MEM_WRITE(SITD_ptr->BUFFER_PTR_0, (uint32_t)buff_ptr);
         EHCI_MEM_WRITE(SITD_ptr->BUFFER_PTR_1, (((uint32_t)buff_ptr +  4096) & 0xFFFFF000) |
                      (tp_bits << EHCI_SITD_TP_BIT_POS) | pipe_descr_ptr->NO_OF_START_SPLIT);
         #if PSP_HAS_DATA_CACHE
             /* After writing to USB accessed structure, flush cache lines. Not needed if SITD_ptr is in uncached memory area. */
             //USB_dcache_flush_mlines((void *)SITD_ptr, sizeof(*SITD_ptr));
         #endif
            
         /* move buffer pointer */   
         buff_ptr += length_scheduled;


         /**********************************************************************
         SITD ready. We store it in out list of SITDs
         **********************************************************************/
         struct_to_link_list[next] = (uint32_t)SITD_ptr;
         next++;
         
   }  /*end for loop*/

   /**********************************************************************
   List ready. We keep the SITD pointers in our active list of SITds
   **********************************************************************/

   /**********************************************************************
   List ready. We link it into periodic list now.
   **********************************************************************/
   status = _usb_ehci_link_structure_in_periodic_list(handle,pipe_descr_ptr,struct_to_link_list,next);
          
   if(status == USB_OK)
   {
      usb_host_ptr->FULL_SPEED_ISO_QUEUE_ACTIVE = TRUE;
   }

   return status;

} /* EndBody */
#endif //USBCFG_EHCI_MAX_SITD_DESCRS

#if USBCFG_EHCI_MAX_ITD_DESCRS || USBCFG_EHCI_MAX_SITD_DESCRS
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_add_xfer_to_periodic_schedule_list
*  Returned Value : USB_OK or error
*  Comments       :
*        Queue the packet in the EHCI hardware Periodic schedule list
*END*-----------------------------------------------------------------*/

uint32_t _usb_ehci_add_isochronous_xfer_to_periodic_schedule_list
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      /* The pipe descriptor to queue */            
      EHCI_PIPE_STRUCT_PTR             pipe_descr_ptr,
      
      /* [IN] the transfer parameters struct */
      EHCI_TR_STRUCT_PTR               pipe_tr_ptr
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR      usb_host_ptr;
   uint32_t                             cmd_val,sts_val;
   USB_STATUS                          status;

   VUSB20_REG_STRUCT_PTR    dev_ptr;
   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;
   dev_ptr      = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->G.DEV_PTR;


   /****************************************************************************
   If it is a high-speed device we use ITds for transfer and if it is a
   full speed device, we use SITds for transfers.
   ****************************************************************************/

   if(pipe_descr_ptr->G.SPEED == USB_SPEED_HIGH)
   {
#if USBCFG_EHCI_MAX_ITD_DESCRS
      status = _usb_ehci_add_ITD(handle, pipe_descr_ptr, pipe_tr_ptr);
#endif //USBCFG_EHCI_MAX_ITD_DESCRS
   }
   else
   {
#if USBCFG_EHCI_MAX_SITD_DESCRS
      status = _usb_ehci_add_SITD(handle, pipe_descr_ptr, pipe_tr_ptr);
#endif //USBCFG_EHCI_MAX_SITD_DESCRS
   }
   
   
   if(status != USB_OK)
   {
      return status;
   }

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
      EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PERIODIC_LIST_BASE_ADDR,(uint32_t) usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR);

      /****************************************************************************
      wait until we can enable  the periodic schedule.
      ****************************************************************************/
      while((cmd_val & EHCI_USBCMD_PERIODIC_SCHED_ENABLE) !=
             (EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_STS) & EHCI_STS_PERIODIC_SCHEDULE)) {
      }
         

      /****************************************************************************
      enable the schedule now.
      ****************************************************************************/
            
      EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD,(cmd_val | EHCI_USBCMD_PERIODIC_SCHED_ENABLE)); 
   }

   return USB_OK;

} /* EndBody */
#endif //USBCFG_EHCI_MAX_ITD_DESCRS || USBCFG_EHCI_MAX_SITD_DESCRS



#if USBCFG_EHCI_MAX_ITD_DESCRS
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_process_itd_tr_complete
*  Returned Value : None
*  Comments       :
*     Search the ITD list to see which ITD had finished and 
*     Process the interrupt.
*
*END*-----------------------------------------------------------------*/

void _usb_ehci_process_itd_tr_complete
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR               usb_host_ptr;
   EHCI_ITD_STRUCT_PTR                          ITD_ptr;
   EHCI_PIPE_STRUCT_PTR                         pipe_descr_ptr = NULL;
   EHCI_TR_STRUCT_PTR                           pipe_tr_struct_ptr = NULL;
   uint32_t                                     no_of_scheduled_slots;
   uint32_t                                     status = 0;
   unsigned char                                *buffer_ptr = NULL;
   LIST_NODE_STRUCT_PTR                         node_ptr,prev_node_ptr,next_node_ptr;
   uint8_t                                      transaction_number;
   bool                                         pending_transaction;
   uint32_t                                     length_transmitted;
   uint32_t                                     *prev_link_ptr = NULL, *next_link_ptr = NULL;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;
   length_transmitted = 0;
   UNUSED_ARGUMENT(prev_link_ptr)

   /******************************************************************
   Search the ITD list starting from head till we find inactive nodes.
   Note that for Head there is no previous node so we can disntiguish
   it from rest of the list.
   ******************************************************************/
   prev_node_ptr = node_ptr =  usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR; 

   /* loop till current node is active or node is a head node*/
   while ((prev_node_ptr->next_active && (prev_node_ptr->next != NULL))
          || ((node_ptr->prev == NULL) && (node_ptr->member != NULL)))
   {
      
      ITD_ptr =  (EHCI_ITD_STRUCT_PTR) node_ptr->member;
       #ifdef  __USB_OTG__
       
         #ifdef HOST_TESTING
         /*
         usb_otg_state_struct_ptr->STATUS_STARTS[usb_otg_state_struct_ptr->LOG_ITD_COUNT]
                      = EHCI_MEM_READ(ITD_ptr->status);
         */            
         #endif
      #endif
           
      if (!ITD_ptr) {
         continue;
      }
      #if PSP_HAS_DATA_CACHE
      /* Before reading from USB accessed structure, invalidate cache lines. Not needed if ITD_ptr is in uncached memory area */
      //USB_dcache_invalidate_mlines((pointer)ITD_ptr, sizeof(*ITD_ptr));
      #endif
         
      pipe_tr_struct_ptr =  (EHCI_TR_STRUCT_PTR) ITD_ptr->PIPE_TR_DESCR_FOR_THIS_ITD;
      
      pipe_descr_ptr = (EHCI_PIPE_STRUCT_PTR) ITD_ptr->PIPE_DESCR_FOR_THIS_ITD;
      
      /* assume that no transactions are pending on this ITD */
      pending_transaction = FALSE;
      
      no_of_scheduled_slots = 0;
      
      /**************************************************************
      Check the status of every transaction inside the ITD.
      **************************************************************/
      for(transaction_number = 0; transaction_number < 8; transaction_number++)
      {

         /**************************************************************
         Note that each iteration of this loop checks the micro frame 
         number on which transaction is scheduled. If a micro frame was
         not allocated for this pipe, we don't need to check it. But
         caution is that, there could be a transaction that was too
         small and even though many bandwidth slots are available 
         but this transaction was finished only in 1 of the slots. Thus
         we also keep a check of how many transactions were allocated for
         this ITD.
         **************************************************************/
      
         if ((pipe_descr_ptr->BWIDTH_SLOTS[transaction_number]) &&
            (no_of_scheduled_slots < ITD_ptr->number_of_transactions))
         {

               
              no_of_scheduled_slots++;
               
              status = EHCI_MEM_READ(ITD_ptr->TR_STATUS_CTL_LIST[transaction_number]) & EHCI_ITD_STATUS;

            
               /* if transaction is not active and IOC was set we look in to it else we move on */
               if ((!(status & EHCI_ITD_STATUS_ACTIVE)) &&
                  (EHCI_MEM_READ(ITD_ptr->TR_STATUS_CTL_LIST[transaction_number]) & EHCI_ITD_IOC_BIT)) {

                  #if 0
                  /* send callback to app with the status*/
                  if (pipe_tr_struct_ptr->G.CALLBACK != NULL) 
                  {
                       uint_32 temp = EHCI_MEM_READ(ITD_ptr->TR_STATUS_CTL_LIST[transaction_number]);

                       offset =  temp & EHCI_ITD_BUFFER_OFFSET;
                                  
                       page_number =  (uint_8)((temp & EHCI_ITD_PAGE_NUMBER) >> EHCI_ITD_PG_SELECT_BIT_POS);

                       length_transmitted = (temp & EHCI_ITD_LENGTH_TRANSMITTED) >> EHCI_ITD_LENGTH_BIT_POS;
                                     
                        buffer_ptr = (uchar_ptr) ((EHCI_MEM_READ(ITD_ptr->BUFFER_PAGE_PTR_LIST[page_number]) &
                                                 EHCI_ITD_BUFFER_POINTER) >> EHCI_ITD_PG_SELECT_BIT_POS);
                                    
                                               
                        pipe_tr_struct_ptr->G.CALLBACK(
                                       (pointer)pipe_descr_ptr, 
                                       pipe_tr_struct_ptr->G.CALLBACK_PARAM,
                                       (buffer_ptr+offset),
                                       length_transmitted,
                                       status);
                  }
                  #endif
                  length_transmitted += (ITD_ptr->TR_STATUS_CTL_LIST[transaction_number] &
                                     EHCI_ITD_LENGTH_TRANSMITTED) >> EHCI_ITD_LENGTH_BIT_POS;
                                 
               }
               /* if IOC is set and status is active, we have a pending transaction */
               else if ((status & EHCI_ITD_STATUS_ACTIVE) &&
                     (EHCI_MEM_READ(ITD_ptr->TR_STATUS_CTL_LIST[transaction_number]) & EHCI_ITD_IOC_BIT)) {
                  /* This means that this ITD has a pending transaction */
                  pending_transaction = TRUE;
                  break;
               }
         
         }

      }

       /* If this ITD is done with all transactions, time to free it */
      if(!pending_transaction)
      {

            /* if we are freeing a head node, we move node_ptr to next head 
            or else we move normally*/
      
            if(node_ptr ==  usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR)
            {
               /*free this node */
               EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR,
                                    usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_TAIL_PTR,
                                    node_ptr);

               prev_node_ptr = node_ptr = usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR;
            }
            else
            {
                 /*save next node */
                 next_node_ptr = node_ptr->next;
                 
                 /*free current node */
                 EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR,
                                    usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_TAIL_PTR,
                                    node_ptr);

                 /*move to next node now */
                 node_ptr = next_node_ptr;                   
                 prev_node_ptr = node_ptr->prev;

            }

         /* EHCI_MEM_WRITE((*EHCI_MEM_READ(ITD_ptr->prev_data_struct_ptr)),EHCI_MEM_READ(ITD_ptr->next_data_struct_value));*/
           #ifdef  __USB_OTG__ 
              #ifdef HOST_TESTING
                        usb_otg_state_struct_ptr->STATUS[usb_otg_state_struct_ptr->LOG_ITD_COUNT]
                         = status;
                         
                        USB_mem_copy((uchar_ptr)ITD_ptr,
                                   &usb_otg_state_struct_ptr->LOG_INTERRUPT_ITDS[usb_otg_state_struct_ptr->LOG_ITD_COUNT]
                                     ,8);

                        usb_otg_state_struct_ptr->LOG_ITD_COUNT++;
                        if(usb_otg_state_struct_ptr->LOG_ITD_COUNT > HOST_LOG_MOD)
                        usb_otg_state_struct_ptr->LOG_ITD_COUNT = 0;
                     
               #endif
          #endif
            
            /*remove the ITD from periodic list */
            prev_link_ptr = next_link_ptr =  ITD_ptr->frame_list_ptr;
            /*iterate the list while we find valid pointers (1 means invalid pointer) */

            while(!((uint32_t)next_link_ptr & EHCI_FRAME_LIST_ELEMENT_POINTER_T_BIT) && !(EHCI_MEM_READ(*next_link_ptr) & EHCI_FRAME_LIST_ELEMENT_POINTER_T_BIT))
            {
               /*if a pointer matches our ITD we remove it from list*/
               if((EHCI_MEM_READ(*next_link_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK) == (uint32_t) ITD_ptr)
               {
                  EHCI_MEM_WRITE(*next_link_ptr, EHCI_MEM_READ((uint32_t)ITD_ptr->NEXT_LINK_PTR));
                  break;
               }
               
               prev_link_ptr = next_link_ptr;
               next_link_ptr = (uint32_t *) (EHCI_MEM_READ(*next_link_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK);
            }



            /* subtract on how many ITDs are pending from this transfer */
            pipe_tr_struct_ptr->no_of_itds_sitds -= 1;
            
            /* if all ITDS are served free the TR INDEX */
            if(pipe_tr_struct_ptr->no_of_itds_sitds == 0)
            {
                 /* Mark TR as unused so that next request can use it */
                 pipe_tr_struct_ptr->G.TR_INDEX = 0;
                 
            }
         
          /* Get buffer ftom ITD, aligned */
          buffer_ptr = (unsigned char *) (EHCI_MEM_READ(ITD_ptr->BUFFER_PAGE_PTR_LIST[0]) & EHCI_ITD_BUFFER_POINTER);
          /* Add offset of unaligned buffer */
          buffer_ptr += EHCI_MEM_READ(ITD_ptr->TR_STATUS_CTL_LIST[0]) & EHCI_ITD_BUFFER_OFFSET;

          /* free the ITD used */
          _usb_ehci_free_ITD((_usb_host_handle)usb_host_ptr, (void *)ITD_ptr);

          if (pipe_tr_struct_ptr->G.CALLBACK != NULL) 
          {
              pipe_tr_struct_ptr->G.CALLBACK((void *)pipe_descr_ptr, 
                                                (TR_STRUCT_PTR)pipe_tr_struct_ptr,
                                                buffer_ptr,
                                                length_transmitted,
                                                status);
          }
         
      }
      else
      {
         /* move to next ITD in the list */      
         prev_node_ptr = node_ptr;
         node_ptr = node_ptr->next;

      }
      

     
   }/* end while loop */
   
   
} /* Endbody */
#endif //USBCFG_EHCI_MAX_ITD_DESCRS

#if USBCFG_EHCI_MAX_SITD_DESCRS   
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_process_sitd_tr_complete
*  Returned Value : None
*  Comments       :
*     Search the SITD list to see which SITD had finished and 
*     Process the interrupt.
*
*END*-----------------------------------------------------------------*/
void _usb_ehci_process_sitd_tr_complete
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR               usb_host_ptr;
   EHCI_SITD_STRUCT_PTR                         SITD_ptr;
   EHCI_PIPE_STRUCT_PTR                         pipe_descr_ptr = NULL;
   EHCI_TR_STRUCT_PTR                           pipe_tr_struct_ptr = NULL;
   uint32_t                                     status = 0;
   unsigned char                                *buffer_ptr = NULL;
   LIST_NODE_STRUCT_PTR                         node_ptr,prev_node_ptr,next_node_ptr;
   uint32_t                                     length_scheduled, length_remaining;
   uint32_t                                     *prev_link_ptr = NULL, *next_link_ptr = NULL;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;
   UNUSED_ARGUMENT(prev_link_ptr)
   
   /******************************************************************
   Search the SITD list starting from head till we find inactive nodes.
   Note that for Head there is no previous node so we can disntiguish
   it from rest of the list.
   ******************************************************************/
   prev_node_ptr = node_ptr =  usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR; 
   
   /* loop till current node is active or node is a head node*/
   while ((prev_node_ptr->next_active && (prev_node_ptr->next != NULL))
          || ((node_ptr->prev == NULL) && (node_ptr->member != NULL)))
   {
      
      SITD_ptr =  (EHCI_SITD_STRUCT_PTR) node_ptr->member;
      #if PSP_HAS_DATA_CACHE
      /* Before reading from USB accessed structure, invalidate cache lines. Not needed if SITD_ptr is in uncached memory area */
      //USB_dcache_invalidate_mlines((pointer)SITD_ptr, sizeof(*SITD_ptr));
      #endif
      
      pipe_tr_struct_ptr = (EHCI_TR_STRUCT_PTR) SITD_ptr->PIPE_TR_DESCR_FOR_THIS_SITD;
      
      pipe_descr_ptr = (EHCI_PIPE_STRUCT_PTR) SITD_ptr->PIPE_DESCR_FOR_THIS_SITD;

      /*grab the status and check it */
      status = EHCI_MEM_READ(SITD_ptr->TRANSFER_STATE) & EHCI_SITD_STATUS;
               
      /* if transaction is not active we look in to it else we move on */
      if(!(status & EHCI_SITD_STATUS_ACTIVE))
      {
         
            /* send callback to app with the status*/
            if (pipe_tr_struct_ptr->G.CALLBACK != NULL) 
            {
                  length_scheduled = EHCI_MEM_READ(SITD_ptr->RESERVED[MEM_SIZE_INDEX]);
                  length_remaining =((EHCI_MEM_READ(SITD_ptr->TRANSFER_STATE) & EHCI_SITD_LENGTH_TRANSMITTED) >> 16);
                  buffer_ptr = (unsigned char *) EHCI_MEM_READ(SITD_ptr->BUFFER_PTR_0);
                  buffer_ptr = buffer_ptr - (length_scheduled - length_remaining);

                  pipe_tr_struct_ptr->G.CALLBACK(
                                 (void *)pipe_descr_ptr, 
                                 pipe_tr_struct_ptr->G.CALLBACK_PARAM,
                                 buffer_ptr,
                                 length_scheduled - length_remaining,
                                 status);
            }
            
            /*********************************************************************
             Since status is Non active for this SITD, time to delete it.           
            *********************************************************************/
            
            /* if we are freeing a head node, we move node_ptr to next head 
            or else we move normally */
      
            if(node_ptr ==  usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR)
            {
               /*free this node */
               EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR,
                                    usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR,
                                    node_ptr);

               prev_node_ptr = node_ptr = usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR;
            }
            else
            {
                 /*save next node */
                 next_node_ptr = node_ptr->next;
                 
                 /*free current node */
                 EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR,
                                    usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR,
                                    node_ptr);

                 /*move to next node now */
                 node_ptr = next_node_ptr;                   
                 prev_node_ptr = node_ptr->prev;

            }

           #ifdef  __USB_OTG__ 
 
              #ifdef HOST_TESTING
                        usb_otg_state_struct_ptr->STATUS[usb_otg_state_struct_ptr->LOG_SITD_COUNT]
                         = status;
                         
                        USB_mem_copy((uchar_ptr)SITD_ptr,
                                   &usb_otg_state_struct_ptr->LOG_INTERRUPT_SITDS[usb_otg_state_struct_ptr->LOG_SITD_COUNT]
                                     ,44);

                        usb_otg_state_struct_ptr->LOG_SITD_COUNT++;
                        if(usb_otg_state_struct_ptr->LOG_SITD_COUNT > HOST_LOG_MOD)
                        usb_otg_state_struct_ptr->LOG_SITD_COUNT = 0;
                     
               #endif
            #endif
                        
            /*remove the SITD from periodic list, TODO: remove prev_link_ptr variable */
            prev_link_ptr = next_link_ptr =  SITD_ptr->frame_list_ptr;
            /*iterate the list while we find valid pointers (1 means invalid pointer) */
            while(!((uint32_t)next_link_ptr & EHCI_FRAME_LIST_ELEMENT_POINTER_T_BIT) && !(EHCI_MEM_READ(*next_link_ptr) & EHCI_FRAME_LIST_ELEMENT_POINTER_T_BIT))
            {
               /*if a pointer matches our SITD we remove it from list*/
               if((EHCI_MEM_READ(*next_link_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK) == (uint32_t) SITD_ptr)
               {
                  EHCI_MEM_WRITE(*next_link_ptr, EHCI_MEM_READ((uint32_t)SITD_ptr->NEXT_LINK_PTR));
                  break;
               }
               
               prev_link_ptr = next_link_ptr;
               next_link_ptr = (uint32_t *) (EHCI_MEM_READ(*next_link_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK);
            }



            /* free the SITD used */
            _usb_ehci_free_SITD((_usb_host_handle)usb_host_ptr, (void *)SITD_ptr);

            /* subtract on how many SITDs are pending from this transfer */
            pipe_tr_struct_ptr->no_of_itds_sitds -= 1;
            
            /* if all SITDS are served free the TR INDEX */
            if(pipe_tr_struct_ptr->no_of_itds_sitds == 0)
            {
                 /* Mark TR as unused so that next request can use it */
                 pipe_tr_struct_ptr->G.TR_INDEX = 0;
                 
            }
         

                           
      }
      /* else move on to the next node in the queue */
      else
      {
      
         prev_node_ptr = node_ptr;
         node_ptr = node_ptr->next;

      }


   #if 0            
      /* save the next node */
      next_node_ptr =  
   
      /*free this node */
      EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR,
                           usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR,
                           node_ptr);
      
      /* subtract on how many ITDs are pending from this transfer */
      pipe_tr_struct_ptr->no_of_itds_sitds -= 1;
      
      /* if all ITDS are served free the TR INDEX */
      if(pipe_tr_struct_ptr->no_of_itds_sitds == 0)
      {
           /* Mark TR as unused so that next request can use it */
           pipe_tr_struct_ptr->TR_INDEX = 0;

      }
   
      /* if we just freed a head node, we must update our list head */
      if(node_ptr->prev == NULL)
      {
         prev_node_ptr = node_ptr = next_node_ptr;
      }
      else /* we freed some non head node so we move on normally*/
      {
          /* move to next ITD in the list */      
          node_ptr = next_node_ptr;
          prev_node_ptr = node_ptr->prev;

      }
               
   #endif
     
   }/* end while loop */
   
   
} /* Endbody */
#endif //USBCFG_EHCI_MAX_SITD_DESCRS

#if USBCFG_EHCI_MAX_ITD_DESCRS
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_free_ITD
*  Returned Value : void
*  Comments       :
*        Enqueues an ITD onto the free ITD ring.
*
*END*-----------------------------------------------------------------*/

void _usb_ehci_free_ITD
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle        handle,
      
      /* [IN] the ITD to enqueue */
      void                    *ITD_ptr
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR                usb_host_ptr;
   
   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself.
   */
   USB_lock();

   /*
   ** Add the ITD to the free ITD queue (linked via PRIVATE) and
   ** increment the tail to the next descriptor
   */
   EHCI_ITD_QADD(usb_host_ptr->ITD_HEAD, usb_host_ptr->ITD_TAIL, (EHCI_ITD_STRUCT_PTR)ITD_ptr);
   usb_host_ptr->ITD_ENTRIES++;

   USB_unlock();

} /* Endbody */
#endif //USBCFG_EHCI_MAX_ITD_DESCRS

#if USBCFG_EHCI_MAX_SITD_DESCRS
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_free_SITD
*  Returned Value : void
*  Comments       :
*        Enqueues an SITD onto the free SITD ring.
*
*END*-----------------------------------------------------------------*/

void _usb_ehci_free_SITD
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle        handle,
      
      /* [IN] the SITD to enqueue */
      void                    *SITD_ptr
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR                usb_host_ptr;
   
   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself.
   */
   USB_lock();

   /*
   ** Add the SITD to the free SITD queue (linked via PRIVATE) and
   ** increment the tail to the next descriptor
   */
   EHCI_SITD_QADD(usb_host_ptr->SITD_HEAD, usb_host_ptr->SITD_TAIL, 
      (EHCI_SITD_STRUCT_PTR)SITD_ptr);
   usb_host_ptr->SITD_ENTRIES++;

   USB_unlock();

} /* Endbody */
#endif //USBCFG_EHCI_MAX_SITD_DESCRS

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_close_isochronous_pipe
*  Returned Value : None
*  Comments       :
*        Close the Iso pipe and update the bandwidth list.
Here are the notes. In EHCI, closing an ISO pipe involves removing
the ITD or SITD from the periodic list to make sure that none of the
frames refer to this  any more.
*END*-----------------------------------------------------------------*/

void _usb_ehci_close_isochronous_pipe (

      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      /* The pipe descriptor to queue */            
      EHCI_PIPE_STRUCT_PTR             pipe_descr_ptr

)
{
   USB_EHCI_HOST_STATE_STRUCT_PTR            usb_host_ptr;
   uint8_t                                   *frame_list_bw_ptr;
   EHCI_ITD_STRUCT_PTR                       ITD_ptr;
   EHCI_SITD_STRUCT_PTR                      SITD_ptr;
   uint32_t                                  i,j,frame,uframe,start;
   uint32_t                                  interval = pipe_descr_ptr->G.INTERVAL;
   LIST_NODE_STRUCT_PTR                      node_ptr,next_node_ptr,prev_node_ptr = NULL;
   EHCI_TR_STRUCT_PTR                        pipe_tr_struct_ptr = NULL;
   uint32_t                                  *prev_link_ptr=NULL, *next_link_ptr=NULL;

   UNUSED_ARGUMENT(prev_node_ptr)
   
   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;

   /****************************************************************************
   if this is a HS pipe, search ITDs or else search SITD list to free them.
   ****************************************************************************/
   if(pipe_descr_ptr->G.SPEED  == USB_SPEED_HIGH)
   {
#if USBCFG_EHCI_MAX_ITD_DESCRS
        prev_node_ptr = node_ptr =  usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR; 

        /* loop until we find an invalid node or if this is a head node*/ 
        while (node_ptr->member != NULL)
        {
       
            ITD_ptr =  (EHCI_ITD_STRUCT_PTR) node_ptr->member;

            pipe_tr_struct_ptr =  ITD_ptr->PIPE_TR_DESCR_FOR_THIS_ITD;
       
            if(ITD_ptr->PIPE_DESCR_FOR_THIS_ITD == pipe_descr_ptr)
            {
                     /* if we are freeing a head node, we move node_ptr to next head 
                     or else we move normally*/

                     if(node_ptr ==  usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR)
                     {
                        /*free this node */
                        EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR,
                                             usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_TAIL_PTR,
                                             node_ptr);

                        prev_node_ptr = node_ptr = usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR;
                     }
                     else
                     {
                          /*save next node */
                          next_node_ptr = node_ptr->next;
                          
                          /*free current node */
                          EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR,
                                             usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_TAIL_PTR,
                                             node_ptr);

                          /*move to next node now */
                          node_ptr = next_node_ptr;                   
                          prev_node_ptr = node_ptr->prev;

                     }

                     /*remove the ITD from periodic list */
                     prev_link_ptr = next_link_ptr =  ITD_ptr->frame_list_ptr;
                     /*iterate the list while we find valid pointers (1 means invalid pointer) */
                     while(!(EHCI_MEM_READ(*next_link_ptr) & EHCI_FRAME_LIST_ELEMENT_POINTER_T_BIT))
                     {
                        /*if a pointer matches our ITD we remove it from list*/
                        if((EHCI_MEM_READ(*next_link_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK) ==  (uint32_t) ITD_ptr)
                        {
                           EHCI_MEM_WRITE(*prev_link_ptr,EHCI_MEM_READ((uint32_t)ITD_ptr->NEXT_LINK_PTR));
                           break;
                        }
         
                        prev_link_ptr = next_link_ptr;
                        next_link_ptr = (uint32_t *) (EHCI_MEM_READ(*next_link_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK);
                     }

                     /* free the ITD used */
                     _usb_ehci_free_ITD((_usb_host_handle)usb_host_ptr, (void *)ITD_ptr);

                     /* subtract on how many ITDs are pending from this transfer */
                     pipe_tr_struct_ptr->no_of_itds_sitds -= 1;
      
                     /* if all ITDS are served free the TR INDEX */
                     if(pipe_tr_struct_ptr->no_of_itds_sitds == 0)
                     {
                          /* Mark TR as unused so that next request can use it */
                          pipe_tr_struct_ptr->G.TR_INDEX = 0;
                          
                     }
         
            }
            else
            {
               /* move to next ITD in the list */      
               prev_node_ptr = node_ptr;
               node_ptr = node_ptr->next;

            }
   

       } /* while */
       

#endif //USBCFG_EHCI_MAX_ITD_DESCRS
   } /* end if */
   else
   {
#if USBCFG_EHCI_MAX_SITD_DESCRS
        prev_node_ptr = node_ptr =  usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR; 

        /* loop until we find an invalid node or if this is a head node*/ 
        while (node_ptr->member != NULL)
        {
       
            SITD_ptr =  (EHCI_SITD_STRUCT_PTR) node_ptr->member;

            pipe_tr_struct_ptr = SITD_ptr->PIPE_TR_DESCR_FOR_THIS_SITD;
       
            if(SITD_ptr->PIPE_DESCR_FOR_THIS_SITD == pipe_descr_ptr)
            {
                     /* if we are freeing a head node, we move node_ptr to next head 
                     or else we move normally*/

                     if(node_ptr ==  usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR)
                     {
                        /*free this node */
                        EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR,
                                             usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR,
                                             node_ptr);

                        prev_node_ptr = node_ptr = usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR;
                     }
                     else
                     {
                          /*save next node */
                          next_node_ptr = node_ptr->next;
                          
                          /*free current node */
                          EHCI_QUEUE_FREE_NODE(usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR,
                                             usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR,
                                             node_ptr);

                          /*move to next node now */
                          node_ptr = next_node_ptr;                   
                          prev_node_ptr = node_ptr->prev;

                     }

                     /*remove the SITD from periodic list */
                     prev_link_ptr = next_link_ptr =  SITD_ptr->frame_list_ptr;
                     /*iterate the list while we find valid pointers (1 means invalid pointer) */
                     while(!(EHCI_MEM_READ(*next_link_ptr) & EHCI_FRAME_LIST_ELEMENT_POINTER_T_BIT))
                     {
                        /*if a pointer matches our SITD we remove it from list*/
                        if((EHCI_MEM_READ(*next_link_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK) ==  (uint32_t) SITD_ptr)
                        {
                           EHCI_MEM_WRITE(*prev_link_ptr,EHCI_MEM_READ((uint32_t)SITD_ptr->NEXT_LINK_PTR));
                           break;
                        }
         
                        prev_link_ptr = next_link_ptr;
                        next_link_ptr = (uint32_t *) (EHCI_MEM_READ(*next_link_ptr) & EHCI_HORIZ_PHY_ADDRESS_MASK);
                     }

                     /* free the ITD used */
                     _usb_ehci_free_SITD((_usb_host_handle)usb_host_ptr, (void *)SITD_ptr);

                     /* subtract on how many ITDs are pending from this transfer */
                     pipe_tr_struct_ptr->no_of_itds_sitds -= 1;
      
                     /* if all ITDS are served free the TR INDEX */
                     if(pipe_tr_struct_ptr->no_of_itds_sitds == 0)
                     {
                          /* Mark TR as unused so that next request can use it */
                          pipe_tr_struct_ptr->G.TR_INDEX = 0;
                          
                     }
         
            }
            else
            {
               /* move to next ITD in the list */      
               prev_node_ptr = node_ptr;
               node_ptr = node_ptr->next;

            }
   

       } /* while */
       
#endif //USBCFG_EHCI_MAX_SITD_DESCRS
   }


   /********************************************************************
   if status is fine we should free the slots now by updating the 
   bandwidth list.
   ********************************************************************/
   frame_list_bw_ptr = usb_host_ptr->PERIODIC_FRAME_LIST_BW_PTR;

   if (pipe_descr_ptr->G.SPEED == USB_SPEED_HIGH) 
   {
      start = pipe_descr_ptr->START_UFRAME;

       for(i = start; i < usb_host_ptr->FRAME_LIST_SIZE * 8; i+= interval)
      {
         /********************************************************************
         We are allowed to use only 80% of a micro frame for 
         periodic transfers. This is to provide space for Bulk and Control
         transfers. This means that if a micro frame slot exceeds .8 * 125 = 100
         micro seconds, it is out of space.
         ********************************************************************/
         frame  = i/8;
         uframe = i%8;
         
         frame_list_bw_ptr[frame+uframe] -= pipe_descr_ptr->BWIDTH;

      }
   
   }
   else
   {
   
      start = pipe_descr_ptr->START_FRAME;
      
       for(i = start; i < usb_host_ptr->FRAME_LIST_SIZE; i+= interval)
      {
            /********************************************************************
            Update the bandwidth in all frames in which transaction is scheduled.
            ********************************************************************/

            for(j = 0; j < 8; j++)
            {

               if(pipe_descr_ptr->BWIDTH_SLOTS[j])
               {
                  frame_list_bw_ptr[i+j] -= pipe_descr_ptr->BWIDTH;
               }
                  
            }

       }
       
   }


   
#if 0   
   QH_ptr = (EHCI_QH_STRUCT_PTR) pipe_descr_ptr->QH_FOR_THIS_PIPE;

   
   
   if(QH_ptr == NULL) return;
   
      
   /****************************************************************************
   First Search the periodic list and unlink this QH from the list.
   ****************************************************************************/
   
   frame_list_bw_ptr       = usb_host_ptr->PERIODIC_FRAME_LIST_BW_PTR;   
   periodic_list_base_ptr  = (uint32_t *)(usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR);
   
   /*******************************************************************
   Start from fram 0 till end of the list and unlink the QH if found. Note
   that we should not unlink when the QH is active but current code does
   not take this in account.
   ********************************************************************/

   for(i=0;i< usb_host_ptr->FRAME_LIST_SIZE;i++)
   {

      transfer_data_struct_ptr = periodic_list_base_ptr + i;
      prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
      unlinked = FALSE;

      /*******************************************************************
      Traverse the frame i and unlink the QH if found in it.
      ********************************************************************/
      while ((!(EHCI_MEM_READ(*((uint32_t *)transfer_data_struct_ptr)) & EHCI_FRAME_LIST_ELEMENT_POINTER_T_BIT)) 
             && (unlinked == FALSE)) {

          switch (EHCI_GET_TYPE(transfer_data_struct_ptr)) 
         {
            case EHCI_ELEMENT_TYPE_ITD:
               ITD_ptr = (EHCI_ITD_STRUCT_PTR)((EHCI_MEM_READ*transfer_data_struct_ptr)) & EHCI_HORIZ_PHY_ADDRESS_MASK);
               prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
               transfer_data_struct_ptr = (void *)EHCI_MEM_READ(ITD_ptr->NEXT_LINK_PTR);
               break;
            case EHCI_ELEMENT_TYPE_QH:

               temp_QH_ptr = (EHCI_QH_STRUCT_PTR)((EHCI_MEM_READ*transfer_data_struct_ptr)) & EHCI_HORIZ_PHY_ADDRESS_MASK);

                                                   
               if(temp_QH_ptr ==  QH_ptr) 
               {
                  /*************************************************************
                  Unlink this QH now.                  
                  *************************************************************/
                  USB_lock();
                  reclaim_band_width(handle,
                                    i,
                                    transfer_data_struct_ptr,
                                    pipe_descr_ptr);
                  unlink_periodic_data_structure_from_frame (
                                 prev_transfer_data_struct_ptr,
                                 transfer_data_struct_ptr);
                  USB_unlock();
                  /*exit out of loop since there can be only 1 QH of same pipe in this frame */
                  unlinked = TRUE;
               }
               prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
               transfer_data_struct_ptr = (void *)EHCI_MEM_READ(temp_QH_ptr->HORIZ_LINK_PTR);
               break;
            case EHCI_ELEMENT_TYPE_SITD:
               SITD_ptr = (EHCI_SITD_STRUCT_PTR)((EHCI_MEM_READ(*transfer_data_struct_ptr)) & EHCI_HORIZ_PHY_ADDRESS_MASK);
               prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
               transfer_data_struct_ptr = (void *)EHCI_MEM_READ(SITD_ptr->NEXT_LINK_PTR);
               break;
            case EHCI_ELEMENT_TYPE_FSTN:
               FSTN_ptr = (EHCI_FSTN_STRUCT_PTR)((EHCI_MEM_READ(*transfer_data_struct_ptr)) & EHCI_HORIZ_PHY_ADDRESS_MASK);
               prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
               transfer_data_struct_ptr = (void *)EHCI_MEM_READ(FSTN_ptr->NORMAL_PATH_LINK_PTR);
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
      
   active_list_member_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR) \
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
         while ((!(((uint32_t)QTD_ptr) & EHCI_QTD_T_BIT)) &&  (QTD_ptr != NULL))
         {
            temp_QTD_ptr = QTD_ptr;
            QTD_ptr = (EHCI_QTD_STRUCT_PTR)EHCI_MEM_READ(QTD_ptr->NEXT_QTD_PTR);
           _usb_ehci_free_QTD(handle, (void *)temp_QTD_ptr);
         }

         /*free QH */
         _usb_ehci_free_QH(handle, QH_ptr);
         
         /*ensure that all pointers are NULL in this structure */
         active_list_member_ptr->QH_PTR = NULL;
         active_list_member_ptr->FIRST_QTD_PTR = NULL;
         
         /* store the pointer temporarily before freeing it */
         temp_active_list_member_ptr = active_list_member_ptr;
         
         /* move on to the next member of the list */
         active_list_member_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR) \
                                   active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;


         /* we should free this ACTIVE Q MGMT structure */
         if(prev_active_list_member_ptr != prev_active_list_member_ptr)
         {
            /*connect previous to next */
             ((ACTIVE_QH_MGMT_STRUCT_PTR)prev_active_list_member_ptr)->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR = 
                 temp_active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
                 
             USB_mem_free(temp_active_list_member_ptr);                   
         }
         else   /* means first element of the list */
         {
              USB_mem_free(temp_active_list_member_ptr);
         }
         
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

#endif
   
   return;

}

/* EOF */
