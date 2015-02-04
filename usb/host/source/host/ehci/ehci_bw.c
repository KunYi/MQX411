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
*   This file contains the low-level Host API functions for bandwidth
*   allocation.
*
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "hostapi.h"
#include "host_cnfg.h"

#include "ehci_main.h"
#include "ehci_prv.h"

/*******************************************************************
Routines not exported by this file
*******************************************************************/

static USB_STATUS _usb_ehci_commit_bandwidth_slots(_usb_host_handle, EHCI_PIPE_STRUCT_PTR, uint32_t, uint32_t, uint8_t *, uint32_t);

static void link_interrupt_qh_to_periodiclist(_usb_host_handle, EHCI_QH_STRUCT_PTR, EHCI_PIPE_STRUCT_PTR, uint32_t);

static USB_STATUS allocate_ss_cs_bandwidth(_usb_host_handle, EHCI_PIPE_STRUCT_PTR, uint32_t, uint32_t, uint32_t);

static USB_STATUS split_combinations(_usb_host_handle, EHCI_PIPE_STRUCT_PTR, uint8_t [], int, int, int);


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_calculate_uframe_tr_time
*  Returned Value : bus time in nanoseconds or -1 if error
*  Comments       :
*        Calculate the high speed bus transaction time (USB2.0 Spec 5.11.3)
* in micro seconds for given number of bytes and type of pipe. Handshake
time is included in this and worst case bt stuffing time is take in to
account.
*END*-----------------------------------------------------------------*/

USB_STATUS _usb_ehci_calculate_uframe_tr_time (

      uint32_t     bytes,

      uint8_t      pipetype

   )
{ /* Body */
   uint32_t  uframe_tr_time=0;

   /******************************************************************
   The following formulae taken from USB specification are used
   Non-Isochronous Transfer (Handshake Included)
   = (55 * 8 * 2.083) + (2.083 * Floor(3.167 + BitStuffTime(Data_bc))) +
   Host_Delay
   Isochronous Transfer (No Handshake)
   = (38 * 8 * 2.083) + (2.083 * Floor(3.167 + BitStuffTime(Data_bc))) +
   Host_Delay
   ******************************************************************/

   if (pipetype == USB_ISOCHRONOUS_PIPE)
   {

      /******************************************************************
      Host delay has been taken as 3 nano seconds (3000 pico seconds to guess).
      Linux code takes a rough guess of 5 nano seconds.
      ******************************************************************/

      uframe_tr_time = 38 * 8 * 2083 +
                       2083 * ((3167 + BitStuffTime(1000*bytes))/1000) +
                       VUSB_HS_DELAY;

      uframe_tr_time = uframe_tr_time/1000; /*back to nano seconds */

   }
   else
   {

      /******************************************************************
      Host delay has been taken as 3 nano seconds (3000 pico seconds to guess).
      Linux code takes a rough guess of 5 nano seconds.
      ******************************************************************/

      uframe_tr_time = 55 * 8 * 2083 +
                       2083 * ((3167 + BitStuffTime(1000*bytes))/1000) +
                       VUSB_HS_DELAY;

      uframe_tr_time = uframe_tr_time/1000; /*back to nano seconds */


   }

   /******************************************************************
   convert the times back to micro seconds
   ******************************************************************/
   uframe_tr_time = uframe_tr_time/1000;

   /******************************************************************
   if time is less than 1 micro seconds we take an assumption of 1 Micro sec.
   This is true for transfers that are few bytes esp. Interrupt transfers.
   Actually if the bytes are less than 36 bytes, it will always be less than
   1 Micro seconds.
   ******************************************************************/


   if(uframe_tr_time < 1)  uframe_tr_time = 1;

   return uframe_tr_time;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_get_frame_number
*  Returned Value : uint32_t
*  Comments       :
*        Get the frame number
*END*-----------------------------------------------------------------*/

uint32_t _usb_ehci_get_frame_number
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle              handle
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR              usb_host_ptr;
   VUSB20_REG_STRUCT_PTR                  dev_ptr;

   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->DEV_PTR;

   /* Get the frame number (not the uframe number */
   return((EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_FRINDEX) & 0x1FFF)>> 3);

} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_get_micro_frame_number
*  Returned Value : uint32_t
*  Comments       :
*        Get the micro frame number
*END*-----------------------------------------------------------------*/

uint32_t _usb_ehci_get_micro_frame_number
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle              handle
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR              usb_host_ptr;
   VUSB20_REG_STRUCT_PTR                  dev_ptr;

   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->DEV_PTR;

   /* Get the uframe number */
   return(EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_FRINDEX) & 0x07);
} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_allocate_bandwidth
*  Returned Value : USB_OK or error
*  Comments       :
*        Allocates the slots for scheduling transactions on this pipe.
   This routine looks at the type of transfer (Iso or Periodic) and
   allocatest the slots in periodic list. Once all slots are found,
   it updates the bandwidth list to reflect the change.
*END*-----------------------------------------------------------------*/

USB_STATUS _usb_ehci_allocate_bandwidth
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      /* The pipe descriptor to queue */
      PIPE_STRUCT_PTR                  pipe_ptr
   )
{ /* Body */

   uint32_t                             i;
   uint32_t                             max_ss_slots;
   USB_STATUS                           status = USB_OK;
   USB_EHCI_HOST_STATE_STRUCT_PTR       usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;
   uint32_t                             *temp_periodic_list_ptr = NULL;
   EHCI_ITD_STRUCT_PTR                  ITD_ptr;
   void                                 *prev_ptr;
   LIST_NODE_STRUCT_PTR                 temp_itd_node_ptr,temp_sitd_node_ptr;
   EHCI_SITD_STRUCT_PTR                 SITD_ptr;
   EHCI_PIPE_STRUCT_PTR                 pipe_descr_ptr = (EHCI_PIPE_STRUCT_PTR) pipe_ptr;


   /*******************************************************************************
   Initialize the periodic list if it is not initialized already. Note that
   this code could have been put under Host_init routine but since host must
   initialze witthin 1 mili second under OTG timing restrictions, this code
   has been moved here.
   *******************************************************************************/
    if(!usb_host_ptr->PERIODIC_LIST_INITIALIZED)
    {
        /* Set T-Bits of all elements in periodic frame list to 1 */
        temp_periodic_list_ptr = (uint32_t *)usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR;

        for (i=0;i<usb_host_ptr->FRAME_LIST_SIZE; i++)
        {

            EHCI_MEM_WRITE(*temp_periodic_list_ptr,EHCI_FRAME_LIST_ELEMENT_POINTER_T_BIT);
            temp_periodic_list_ptr++;

        }

        /************************************************
        Cache must be flushed to update the RAM
        ************************************************/
        USB_dcache_flush_mlines((void *)usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR, (usb_host_ptr->FRAME_LIST_SIZE * sizeof(EHCI_FRAME_LIST_ELEMENT_POINTER)));

        usb_host_ptr->PERIODIC_LIST_INITIALIZED = TRUE;
    }

   /*******************************************************************************
   Initialize the ITDS list if this is High speed pipe.
   *******************************************************************************/
   if ((!usb_host_ptr->ITD_LIST_INITIALIZED) &&
      (pipe_descr_ptr->G.SPEED == USB_SPEED_HIGH))
   {
         /* Enqueue all the ITDs */
         ITD_ptr = usb_host_ptr->ITD_ALIGNED_BASE_PTR;

#if USBCFG_EHCI_MAX_ITD_DESCRS
         /* Enqueue all the ITDs */
         for (i=0;i<USBCFG_EHCI_MAX_ITD_DESCRS;i++)
         {
            /* Set the dTD to be invalid */
            EHCI_MEM_WRITE(ITD_ptr->NEXT_LINK_PTR,EHCI_ITD_T_BIT);
            /* Set the Reserved fields to 0 */
            ITD_ptr->SCRATCH_PTR = (void *)usb_host_ptr;
            _usb_ehci_free_ITD((_usb_host_handle)usb_host_ptr, (void *)ITD_ptr);
            ITD_ptr++;
         }
#endif //USBCFG_EHCI_MAX_ITD_DESCRS

#if USBCFG_EHCI_MAX_SITD_DESCRS
         /* initialize all nodes and link them */
         temp_itd_node_ptr =  (LIST_NODE_STRUCT_PTR) usb_host_ptr->ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR;
         prev_ptr = NULL;
         for(i=0;i<USBCFG_EHCI_MAX_ITD_DESCRS;i++)
         {
             /* next node is not an active node */
             temp_itd_node_ptr->next_active = FALSE;

             /* previous node connetcion */
             temp_itd_node_ptr->prev = prev_ptr;

             /* move pointer */
             prev_ptr =  temp_itd_node_ptr;

             /* move next */
             temp_itd_node_ptr++;

             /* store the next pointer in previous node */
             ((LIST_NODE_STRUCT_PTR) prev_ptr)->next = temp_itd_node_ptr;
         }
#endif //USBCFG_EHCI_MAX_SITD_DESCRS

      usb_host_ptr->ITD_LIST_INITIALIZED = TRUE;
   }
   /*******************************************************************************
   Initialize the SITDS list if this is full speed or low speed pipe.
   *******************************************************************************/
   if ((!usb_host_ptr->SITD_LIST_INITIALIZED) &&
      (pipe_descr_ptr->G.SPEED != USB_SPEED_HIGH))
   {
         SITD_ptr = usb_host_ptr->SITD_ALIGNED_BASE_PTR;

         /* Enqueue all the SITDs */
         for (i = 0; i < USBCFG_EHCI_MAX_SITD_DESCRS; i++)
         {
            /* Set the dTD to be invalid */
            EHCI_MEM_WRITE(SITD_ptr->NEXT_LINK_PTR,EHCI_SITD_T_BIT);
            /* Set the Reserved fields to 0 */
            SITD_ptr->SCRATCH_PTR = (void *)usb_host_ptr;
            _usb_ehci_free_SITD((_usb_host_handle)usb_host_ptr, (void *)SITD_ptr);
            SITD_ptr++;
         }


         /* initialize all nodes and link them */
         temp_sitd_node_ptr =
            (LIST_NODE_STRUCT_PTR) usb_host_ptr->ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR;
         prev_ptr = NULL;
         for(i = 0; i < USBCFG_EHCI_MAX_SITD_DESCRS; i++)
         {
             /* next node is not an active node */
             temp_sitd_node_ptr->next_active = FALSE;

             /* previous node connection */
             temp_sitd_node_ptr->prev = prev_ptr;

             /* move pointer */
             prev_ptr =  temp_sitd_node_ptr;

             /* move next */
             temp_sitd_node_ptr++;

             /* store the next pointer in previous node */
             ((LIST_NODE_STRUCT_PTR) prev_ptr)->next = temp_sitd_node_ptr;
         }

         usb_host_ptr->SITD_LIST_INITIALIZED = TRUE;
   }

   /*******************************************************************************
   Go Through the  PERIODIC_FRAME_LIST_BW record to find the available slots. Here
   starts the complexity of the process. See the following from USB specifications.

   An isochronous endpoint must specify its required bus access period. Full-/high-speed
   endpoints must specify a desired period as (2^(bInterval-1)) x F, where bInterval is
   in the range one to (and including) 16 and F is 125 µs for high-speed and 1ms for
   full-speed. This allows full-/high-speed isochronous transfers to have rates slower
   than one transaction per (micro)frame.

   An endpoint for an interrupt pipe specifies its desired bus access period. A
   full-speed endpoint can specify a desired period from 1 ms to 255 ms. Low-speed
   endpoints are limited to specifying only 10 ms to 255 ms. High-speed endpoints can
   specify a desired period (2^(bInterval-1))x125 µs, where bInterval is in the range 1 to
   (including) 16.


   For high speed interrupts, Since we must have slots available at uframe_interval
   periods, if we can not find any slot starting from anywhere in Micro frame number
   1 to Micro frame number equal to uframe_interval....till the end of
   periodic list we are out of bandwidth. For low and full speed device we do the same
   starting from frame 1 to frame frame_interval.

   ********************************************************************************/

   /*************************************************************************
   If it is a high speed interrupt, allocate the slots at micro frame interval.
   Or if low or full speed, allocate slots at frame intervals. Note that
   the possible values of the interval for high speed are 1,2,4,8,16.
   **************************************************************************/

   if (pipe_descr_ptr->G.SPEED == USB_SPEED_HIGH)
   {

      for (i = 0; (i < pipe_descr_ptr->G.INTERVAL) && (i < usb_host_ptr->FRAME_LIST_SIZE * 8); i++)
      {
         status = _usb_ehci_commit_bandwidth_slots(handle, pipe_descr_ptr, i, pipe_descr_ptr->G.INTERVAL, NULL, 0);

         if (status != USBERR_BANDWIDTH_ALLOC_FAILED)
         {
            /* Slot allocation was either successful or there was some other error. */
            break;
         }

      }

   }
   else
   {

      /***************************************************************************
      Find the possible SS and CS that this endpoint could have. Check  the
      periodic list for each such slot. If it fails, restart searching the periodic
      list for new slot.
      ****************************************************************************/
      /********************************************************************
      It is no simple game as it sounds in the code here. We should consider
      the time taken by the transaction and set the start split and complete
      split transactions for low and full speed devices. Read the following
      rules carefully to understand the code simplifying assumptions.
      ********************************************************************/

      /********************************************************************
      We are allowed to use only 80% of a frame for full speed and low speed
      periodic transfers. This is to provide space for Bulk and Control
      transfers. This means that if a frame slot exceeds .8x1000 micro seconds
      ,it is out of space. Since this transaction is a low or full speed
      transaction we need to find which start split and complete split could
      be scheduled. The following rules are followed in this code. See section
      11.18 in USB specs for details. Y0-Y7 refers to micro frames on B bus
      which is 1 Microframe behind H Bus.

      1) Host never schedules a start split in Y6.

       If We can not schedule the SS in H0-H6 for OUT, we will move to next frame.
       For IN transactions we must schedule SS early enough to ensure that
       all CS are scheduled within H7. Simplification is that we don't want to
       cross the microframe boundry in our implementation of CS.

      ISO OUT
      ========
      2) for Iso OUTs we must determine how many 188 bytes chunks it will
      take and schedule each of them in a Microframe. Each such microframe
      (also called budgeted microframe) should have a SS in previous
      micro frame. We always try to do a SS in H0-H6.
      If this is not possible we just move over to next frame.

      3) There is no complete split scheduled for Iso OUT


      ISO IN
      =======

      4) For ISO IN, we must schedule a complete split in each micro frame
      following the micro frame in which transaction is budgeted. Also
      if the last micro frame in which complete-split is scheduled is
      less that Y6, we schedule two more in Y6 and Y7.

      5)We take a simplyfying assumption that we budget the transaction
      between H0-H5 and schedule a complete split in rest of the
      micro frames till H7.


      Interrupt IN/OUT
      ================
      6)For interrupt IN/OUT host must schedule a complete split in the
      next two micro frames after transaction is budgeted. An additional
      complete split must be scheduled in the 3rd micro frame unless the
      transaction was budgeted to start in Y6.

      7)We make a simplyfying assumption that if the transaction is not
      within H0-H4, we will go to the next frame (we will never schedule
      interrupt transactions to the H5-H7). This will allow us to
      schedule 3 CS in H5-H7 and 1 SS in the H0-H3.

      8)Max data size for low speed interrupt is  8 bytes
       Max data size for full speed interrupt is 64 bytes

       This means that we will never need more than a Microframe for
       an Interrupt OUT or IN.


      9) Host never schedules for than 16 SS (start splits) in a Micro
      frame for a given TT.

      ********************************************************************/

      switch(pipe_descr_ptr->G.PIPETYPE)
      {
         case USB_ISOCHRONOUS_PIPE:

            /* Get max. allocated start split slots, at least 1 */
            max_ss_slots =
               (pipe_descr_ptr->G.MAX_PACKET_SIZE + EHCI_START_SPLIT_MAX_BUDGET - 1) / EHCI_START_SPLIT_MAX_BUDGET;

            switch(pipe_descr_ptr->G.DIRECTION)
            {

               case USB_SEND: /*ISO OUT*/
                  /***********************************************************
                  For ISO OUT we want to schedule the SS in H0-H6.
                  See the rules above. This means that we can do
                  188*7 = 1316 Max packetsize in one frame, return fail if this
                  is not the case.
                  ***********************************************************/
                  if(max_ss_slots > 7)
                  {
                     return USB_log_error(__FILE__,__LINE__,USBERR_BANDWIDTH_ALLOC_FAILED);
                  }

                  /***********************************************************************
                  If the packet size is within budget, try to allocate bandwidth for
                  all possible start split combinations starting from microframe 0 to
                  7.
                  ************************************************************************/
                  status = allocate_ss_cs_bandwidth(handle,pipe_descr_ptr,max_ss_slots,0,7);
                  break;

               case USB_RECV:

                  /***********************************************************************
                  For ISO IN, we want to schedule SS between H0-H5 so that complete splits
                  could be scheduled latest by H6. In best case we will have 7 slots
                  to complete split transaction.
                  ************************************************************************/
                  if(max_ss_slots > 7)
                  {
                     return USB_log_error(__FILE__,__LINE__,USBERR_BANDWIDTH_ALLOC_FAILED);
                  }

                  /* Note that ideally we should start SS as late as possible in this
                  case because we want to minimize the number of slots when a complete
                  transaction is scheduled but in this case we start from 0 always
                  for simplicity. This should be addressed if we run in bandwidth
                  problems because of more ISO IN pipes.*/

                  /* schedule a SS from 0 to as late as 7-max_ss_slots */
                  status = allocate_ss_cs_bandwidth(handle,pipe_descr_ptr,1,0,(7-max_ss_slots));
                  break;

               default:
                  break;
            }
            break;
         case USB_INTERRUPT_PIPE:
            /*****************************************************************************
            For interrupt pipes, life is cool because we need only 1 microframe max to
            send all data (see the data limits on interrupt pipes in comments above).
            We still make a simplifying assumption to schedule SS in Y0-Y3.
            *****************************************************************************/

            switch(pipe_descr_ptr->G.DIRECTION)
            {
               case USB_SEND:
                  /* 1 slot required between microframes 0-3 to send data and we
                  can schedule CS in next 3 of them*/
                  status = allocate_ss_cs_bandwidth(handle,pipe_descr_ptr,1,0,3);
                  break;

               case USB_RECV:
                  /* 1 slot required between microframes 0-3 to so SS and we
                  can schedule CS in next 3 of them*/
                  status = allocate_ss_cs_bandwidth(handle,pipe_descr_ptr,1,0,3);
                  break;

               default:
                  break;
            }
            break;
         default:
            //DEBUG_PRINT("_usb_ehci_commit_bandwidth_slots: Unknown Pipe type\n");
            break;

      }/*end switch (pipe_descr_ptr->PIPETYPE) */

    }

   return status;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_commit_bandwidth_slots
*  Returned Value : USB_OK or error
*  Comments       :
*        Allocates the slots for scheduling transactions on this pipe.
   This routine looks at the BW list and finds regular slots at the
   asked interval starting from 'start' slot number.
*END*-----------------------------------------------------------------*/

static USB_STATUS _usb_ehci_commit_bandwidth_slots
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      /* The pipe descriptor to queue */
      EHCI_PIPE_STRUCT_PTR             pipe_descr_ptr,

      /* The starting slot from the periodic list*/
      uint32_t                          start,

      /* The interval for the pipe              */
      uint32_t                          interval,

      /* start split slots asked for this pipe, NULL for high speed pipe*/
      uint8_t                           *ss,

      /* Max number of SS slots required*/
      uint32_t                          max_ss_slots


)
{
      USB_EHCI_HOST_STATE_STRUCT_PTR      usb_host_ptr;
      uint8_t                             *frame_list_bw_ptr;
      EHCI_QH_STRUCT_PTR                  QH_ptr = NULL;
      uint32_t                            i,j,count,uframe;
      uint32_t                            status = USB_OK, time_for_action;
      bool                             bandwidth_slots[8] = {FALSE,FALSE,FALSE,FALSE, \
                                                                FALSE,FALSE,FALSE,FALSE};

      usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;

      //ASSERT("_usb_ehci_commit_bandwidth_slots",(pipe_descr_ptr == NULL))
      //ASSERT("_usb_ehci_commit_bandwidth_slots",(handle == NULL))

      /****************************************************************************
      Here is the important note. We want to keep a record of how much bandwidth
      has been allocated in the periodic list. We use this two dimension array
      called   PERIODIC_FRAME_LIST_BW_PTR to keep a record. This array is
      a  SIZEx8 (SIZE for periodic list size and 8 for each micro frame in a frame).
      It knows how much time has been consumed in each micro frame. We look at
      that time and find if we can allocate this new transfer or not. When a
      pipe is closed we update and adjust this array to reflect the available
      slots. There could be very complex schemes to manage bandwidth and we need
      to improve the adjusting algorithms so that this array provides the best shot
      for bandwidth but current code does a simple but workable solution for
      periodic transfers.
      ****************************************************************************/

      frame_list_bw_ptr = usb_host_ptr->PERIODIC_FRAME_LIST_BW_PTR;


      /*********************************************************************************
      Start looking in the bandwidth frame list for the slots. We will
      do two iterations. In first we will find if slots are available and
      in second we will update our bandwidth list to commit slots for this pipe.
      *********************************************************************************/


      /**************************************************************************
      If the speed is high, we just look at micro frames and we don't need to
      worry about the split transations. So go through the periodic list and
      find available micro frames at regular interval. Return if not available.
      ***************************************************************************/

      if (pipe_descr_ptr->G.SPEED == USB_SPEED_HIGH)
      {

         /*************************************************************************
         Calculate the time required in microseconds for a transaction on this
         pipe. Note that we are scheduling on high speed bus and times are
         calculated as recommended for high speed by USB 2.0 specification.
         For HS endpoints, the maximum bandwidth allocated by the endpoint
         per uframe depends also on the number of transfers per 1 uframe.
         **************************************************************************/

         time_for_action = _usb_ehci_calculate_uframe_tr_time(
                                 pipe_descr_ptr->G.MAX_PACKET_SIZE * pipe_descr_ptr->G.TRS_PER_UFRAME,
                                 pipe_descr_ptr->G.PIPETYPE);


         for (i = start; i < usb_host_ptr->FRAME_LIST_SIZE * 8; i += interval)
         {
            /********************************************************************
            We are allowed to use only 80% of a micro frame and 80% of frame
            periodic transfers. This is to provide space for Bulk and Control
            transfers. This means that if a micro frame slot exceeds
            .8 * 125 = 100 micro seconds, it is out of space.
            ********************************************************************/
            //frame  = i / 8;
            uframe = i % 8;

            /*******************************************************************
            The interval in HS can have value one of 1, 2, 4, 8, 16, 32 ... uframes.
            From which implies the uframe offset: 1, 2, 4, 0, 0, 0, 0, ...
            So some optimization can be applied here, but we will not do that for now.
            ********************************************************************/
            bandwidth_slots[uframe] = TRUE;

            if((frame_list_bw_ptr[i] + time_for_action) > 100)
            {
               return  USB_log_error(__FILE__,__LINE__,USBERR_BANDWIDTH_ALLOC_FAILED);
            }
         }
      }
      else
      {
         /**************************************************************************
         If the speed is low or full,we have to find slots where we can issue
         split transactions. Read the following from USB 2.0 specs.

         11.18.2 TT Microframe Pipeline

         The TT implements a microframe pipeline of split transactions in support of
         a full-/low-speed bus. Startsplit transactions are scheduled a microframe
         before the earliest time that their corresponding full-/lowspeed transaction
         is expected to start. Complete-split transactions are scheduled in
         microframes that the full-/low-speed transaction can finish.

         This clearly says that we must check the budget in next microframe for
         Start splits and same microframe for complete splits.
         ***************************************************************************/

         /*************************************************************************
         If this pipe takes less than 188 bytes of time, we use lower number to
         save bandwidth time.
         **************************************************************************/
         if (max_ss_slots <= 1) //max_ss_slots should be always > 0
         {
            /* splitted transaction will have size less than MAX_PACKET_SIZE */
            time_for_action = _usb_ehci_calculate_uframe_tr_time(
                                 pipe_descr_ptr->G.MAX_PACKET_SIZE,
                                 pipe_descr_ptr->G.PIPETYPE);
         }
         else
         {
            /* splitted transaction will have size less than EHCI_START_SPLIT_MAX_BUDGET */
            time_for_action = _usb_ehci_calculate_uframe_tr_time(
                                                         EHCI_START_SPLIT_MAX_BUDGET,
                                                         pipe_descr_ptr->G.PIPETYPE
                                                         );
         }


         /**********************************************************
         Check bandwidth in periodic list for the asked combination
         of start split transaction.
         ***********************************************************/
         for (i = start; i < usb_host_ptr->FRAME_LIST_SIZE; i += interval)
         {

               /**********************************************************
               Interrupt OUT budget the next microframe after SS
               ***********************************************************/

               if ((pipe_descr_ptr->G.PIPETYPE == USB_INTERRUPT_PIPE) &&
                 (pipe_descr_ptr->G.DIRECTION == USB_SEND))
               {
                   for (j = 0; j < max_ss_slots; j++)
                   {
                       if ((frame_list_bw_ptr[8 * i + ss[j] + 1] + time_for_action) > 100)
                       {
                           status =  USB_log_error(__FILE__,__LINE__,USBERR_BANDWIDTH_ALLOC_FAILED);
                           return status;
                       }

                       /* we never allocate last microframe for the slot so ss[j] is always less than 7 */
                       bandwidth_slots[ss[j] + 1] = TRUE; /* at ss[j] the SS will be potentionally placed */
                   }
               }
               /* for interrupt IN check 3 micro frames (starting from second) for CS*/
               else if ((pipe_descr_ptr->G.PIPETYPE == USB_INTERRUPT_PIPE) &&
                 (pipe_descr_ptr->G.DIRECTION == USB_RECV))
               {
                   for (j = 0; j < max_ss_slots; j++)
                   {
                       /* look for the budget in the microframe*/
                       if (((frame_list_bw_ptr[i+ss[j]+2] + time_for_action) > 100) ||
                          ((frame_list_bw_ptr[i+ss[j]+3] + time_for_action) > 100) ||
                          ((frame_list_bw_ptr[i+ss[j]+4] + time_for_action) > 100))
                       {
                          status =  USB_log_error(__FILE__,__LINE__,USBERR_BANDWIDTH_ALLOC_FAILED);
                          return status;
                       }

                       bandwidth_slots[ss[j]+2] = TRUE;
                       bandwidth_slots[ss[j]+3] = TRUE;
                       bandwidth_slots[ss[j]+4] = TRUE;
                   }
               }

               /* for ISO OUT check next micro frame */
               else if ((pipe_descr_ptr->G.PIPETYPE == USB_ISOCHRONOUS_PIPE) &&
                 (pipe_descr_ptr->G.DIRECTION == USB_SEND))

               {
                   for (j = 0; j < max_ss_slots; j++)
                   {
                       if ((frame_list_bw_ptr[8 * i + ss[j] + 1] + time_for_action) > 100)
                       {
                          status =  USB_log_error(__FILE__,__LINE__,USBERR_BANDWIDTH_ALLOC_FAILED);
                          return status;
                       }

                       bandwidth_slots[ss[j] + 1] = TRUE;
                   }
               }

               /* for ISO IN check next all micro frames after first SS. We ignore next SS positions */
               else if ((pipe_descr_ptr->G.PIPETYPE == USB_ISOCHRONOUS_PIPE) &&
                     (pipe_descr_ptr->G.DIRECTION == USB_RECV))

               {
                   for (j = ss[0] + 1; j < 7; j++) //make sure that the last uframe is used for CS and not for iso
                   {
                       if ((frame_list_bw_ptr[8 * i + j] + time_for_action) > 100)
                       {
                           status =  USB_log_error(__FILE__,__LINE__,USBERR_BANDWIDTH_ALLOC_FAILED);
                           return status;
                       }

                       bandwidth_slots[j] = TRUE;
                   }
               }
         } /*for */

      } /*else*/



      /********************************************************************
      store the time for action in the pipe descriptor
      ********************************************************************/

      pipe_descr_ptr->BWIDTH = (uint16_t)time_for_action;


      /********************************************************************
      store the convenient bandwidth slots in pipe descriptor pointer.
      ********************************************************************/
      for(j = 0; j < 8; j++)
      {
            pipe_descr_ptr->BWIDTH_SLOTS[j] = (uint8_t)bandwidth_slots[j];
      }

      /***********************************************************************
      store the start frame or micro frame number in pipe descriptor structure
      ************************************************************************/
      if (pipe_descr_ptr->G.SPEED == USB_SPEED_HIGH)
      {
          pipe_descr_ptr->START_UFRAME = start;
      }
      else
      {
          pipe_descr_ptr->START_FRAME = start;
      }


      /********************************************************************
      When Bandwidth is available,
      For ISOCHRONOUS full speed TRANSACTIONS, we just need to store the
      SS and CS positions inside frames.
      ********************************************************************/
      if ((pipe_descr_ptr->G.SPEED != USB_SPEED_HIGH) &&
         (pipe_descr_ptr->G.PIPETYPE == USB_ISOCHRONOUS_PIPE))
      {

         switch(pipe_descr_ptr->G.DIRECTION)
         {
            case USB_SEND: /* ISO OUT pipe */

               count = 0; /* used to find how many start splits are needed */
               for (j = 0; j < 8; j++)
               {
                   if (bandwidth_slots[j])
                   {
                       /* bandwidth is allocated at next position to SS so we use j-1 to start a split */
                       pipe_descr_ptr->START_SPLIT |= (1 << (j - 1));
                       count++;
                   }
               }

               /* there is no complete split on a ISO OUT so we set the field to 0 */
               pipe_descr_ptr->COMPLETE_SPLIT = 0;
               pipe_descr_ptr->NO_OF_START_SPLIT = count;

               break;

            case USB_RECV: /* ISO IN pipe */

               /* we have only 1 start split on a ISO IN */
               pipe_descr_ptr->START_SPLIT = (uint8_t)(1 << ss[0]);

               /* we do complete split on all positions after first SS */
               for (j = 0; j < 8; j++)
               {
                   if (bandwidth_slots[j])
                   {
                       /* CS is scheduled in the next micro frame in which transaction is budgeted so we use j + 1 */
                       pipe_descr_ptr->COMPLETE_SPLIT |= (1 << (j + 1));
                    }
               }

               break;
         }
      }

      /********************************************************************
      Since bandwidth is available, letus make the life easy for interrupt
      transfers. We will allocate a QH here and link it to the available
      slots in periodic slots. This is the best place to do it since we
      already know the slots (micro frames) that are available. We will however
      keep the QH inactive until a transfer is really queued on it. Life is
      not easy for Iso transfers however because they have no queue heads.
      In case of Iso transfers we should remember the slots available inside
      the pipe decriptor and use them when a transfer is queued.
      ********************************************************************/
      if (pipe_descr_ptr->G.PIPETYPE == USB_INTERRUPT_PIPE)
      {

               /******************************************************************
               Allocate a new queue head
               *******************************************************************/

               EHCI_QH_QGET(usb_host_ptr->QH_HEAD, usb_host_ptr->QH_TAIL, QH_ptr);

               if (!QH_ptr) {
                  return USB_log_error(__FILE__,__LINE__,USBERR_ALLOC);
               } /* Endif */

               usb_host_ptr->QH_ENTRIES--;

               //printf("\nQH Get 0x%x, #entries=%d",QH_ptr,usb_host_ptr->QH_ENTRIES);

               /******************************************************************
               Initialize the interrupt Queue head
               *******************************************************************/
               _usb_ehci_init_Q_HEAD(handle,
                                     pipe_descr_ptr,/*pipe that uses this QH */
                                     QH_ptr,        /*QH to initialize*/
                                     NULL,          /*Previous QH to link*/
                                     NULL);         /*first QTD to link */

               /******************************************************************
               Depending upon the speed and direction of the transfer update the
               QH with the micro frame numbers that are allocated
               ******************************************************************/

               if (pipe_descr_ptr->G.SPEED == USB_SPEED_HIGH)
               {
                  /* store the micro frame in QH in which this pipe will have its tranfers done */
                  for (i = 0; i < 8; i++)
                  {
                      /*set a 1 at the position of the micro frame which interrupt transfer
                      is scheduled. */
                     if(bandwidth_slots[i])
                     {
                           pipe_descr_ptr->START_SPLIT |=  (1 << i);
                     }

                  }
               }
               else  /*full and low speed interrupt IN and OUT*/
               {

                  for(j = 1; j <= max_ss_slots; j++)
                  {
                     /*schedule a SS */
                     pipe_descr_ptr->START_SPLIT |=  (1 << ss[j]);

                     /*schedule a CS at the next three positions*/
                     pipe_descr_ptr->COMPLETE_SPLIT |=  (1 << (ss[j]+2));
                     pipe_descr_ptr->COMPLETE_SPLIT |=  (1 << (ss[j]+3));
                     pipe_descr_ptr->COMPLETE_SPLIT |=  (1 << (ss[j]+4));

                  }
               }

               /*set the interrupt mask schedule field in QH */
               EHCI_MEM_CLEAR_BITS(QH_ptr->EP_CAPAB_CHARAC2, 0x000000ff); /*zero last byte*/
               EHCI_MEM_SET_BITS(QH_ptr->EP_CAPAB_CHARAC2,pipe_descr_ptr->START_SPLIT);

               /*set the interrupt complete schedule field in QH */
               EHCI_MEM_CLEAR_BITS(QH_ptr->EP_CAPAB_CHARAC2,0x0000ff00); /*zero second byte*/
               EHCI_MEM_SET_BITS(QH_ptr->EP_CAPAB_CHARAC2,pipe_descr_ptr->COMPLETE_SPLIT << 8);

      }



      /********************************************************************
      if status is fine we should commit the slots now by updating the
      bandwidth list.
      ********************************************************************/

      if (pipe_descr_ptr->G.SPEED == USB_SPEED_HIGH)
      {
         for (i = start; i < usb_host_ptr->FRAME_LIST_SIZE * 8; i += interval)
         {
            /********************************************************************
            We are allowed to use only 80% of a micro frame for
            periodic transfers. This is to provide space for Bulk and Control
            transfers. This means that if a micro frame slot exceeds .8 * 125 = 100
            micro seconds, it is out of space.
            ********************************************************************/
            uframe = i % 8;

            frame_list_bw_ptr[i] += time_for_action;


            /******************************************************************
            if this is for an interrupt pipe, we can link the QH here itself
            *******************************************************************/
            if(pipe_descr_ptr->G.PIPETYPE == USB_INTERRUPT_PIPE)
            {
               link_interrupt_qh_to_periodiclist(handle, QH_ptr, pipe_descr_ptr, i);
            }
         }
      }
      else
      {
         for (i = start; i < usb_host_ptr->FRAME_LIST_SIZE; i+= interval)
         {
            /********************************************************************
            Update the bandwidth in all frames in which transaction is scheduled.
            ********************************************************************/
            for (j = 0; j < 8; j++)
            {
               if (bandwidth_slots[j])
               {
                  frame_list_bw_ptr[8 * i + j] += time_for_action;
               }
            }

            /******************************************************************
            if this is for an interrupt pipe, we can link the QH here itself
            *******************************************************************/
            if (pipe_descr_ptr->G.PIPETYPE == USB_INTERRUPT_PIPE)
            {
               link_interrupt_qh_to_periodiclist(handle, QH_ptr, pipe_descr_ptr, i);
            }
         }
      }

      return status;
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : link_interrupt_qh_to_periodiclist
*  Returned Value : USB_OK or error
*  Comments       :
*        Links the QH to the given slot in periodic list. If pipe is
a high speed, slot number is the micro frame number to link QH in. If
pipe is a low speed, slot number is the frame number to link QH in. In
all cases, we link in frame except that we also update the microframe
schedule mask number.
*END*-----------------------------------------------------------------*/

static void link_interrupt_qh_to_periodiclist
   (

      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      /* QH that will be scheduled. */
      EHCI_QH_STRUCT_PTR               QH_ptr,

      /* The pipe descriptor to queue */
      EHCI_PIPE_STRUCT_PTR             pipe_descr_ptr,

      /* slot in which this QH should be scheduled */
      uint32_t                          slot_number

   )
{
      USB_EHCI_HOST_STATE_STRUCT_PTR      usb_host_ptr;
      uint32_t                            frame_index;
      EHCI_ITD_STRUCT_PTR                 ITD_ptr;
      EHCI_SITD_STRUCT_PTR                SITD_ptr;
      uint32_t                            item_type = EHCI_FRAME_LIST_ELEMENT_TYPE_QH;
      volatile uint32_t                   *transfer_data_struct_ptr = NULL;
      volatile uint32_t                   *prev_transfer_data_struct_ptr = NULL;
      EHCI_QH_STRUCT_PTR                  temp_QH_ptr = NULL;
      EHCI_FSTN_STRUCT_PTR                FSTN_ptr=NULL;

      usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR) handle;

      /*****************************************************************
      Calculate the frame and micro frame index in the periodic list
      *****************************************************************/
      if (pipe_descr_ptr->G.SPEED == USB_SPEED_HIGH)
      {
         frame_index  = slot_number >> 3; /*divide by 8 */
      }
      else
      {
         frame_index  = slot_number; /* already slot is in frame*/
      }

      /*****************************************************************
      traverse the list for the choosen frame and link the QH at the end.
      *****************************************************************/
      transfer_data_struct_ptr = (volatile uint32_t *)(usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR);
      transfer_data_struct_ptr += frame_index;

      /*****************************************************************
      loop through the frame list elements till we reach the end
      *****************************************************************/
      prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
      if ((transfer_data_struct_ptr != NULL)
          && !(EHCI_MEM_READ(*transfer_data_struct_ptr) & EHCI_FRAME_LIST_ELEMENT_POINTER_T_BIT))
      {
            switch (EHCI_GET_TYPE(prev_transfer_data_struct_ptr))
            {
               case EHCI_ELEMENT_TYPE_ITD:
                  ITD_ptr = (EHCI_ITD_STRUCT_PTR)(((uint32_t) prev_transfer_data_struct_ptr));
                  prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
                  transfer_data_struct_ptr = (void *)ITD_ptr->NEXT_LINK_PTR;
                  break;
               case EHCI_ELEMENT_TYPE_QH:
                  temp_QH_ptr = (EHCI_QH_STRUCT_PTR)(((uint32_t) prev_transfer_data_struct_ptr));
                  prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
                  transfer_data_struct_ptr = (void *)(temp_QH_ptr->HORIZ_LINK_PTR & EHCI_HORIZ_PHY_ADDRESS_MASK);
                  if ((EHCI_QH_STRUCT_PTR) transfer_data_struct_ptr == QH_ptr)
                     return;
                  break;
               case EHCI_ELEMENT_TYPE_SITD:
                  SITD_ptr = (EHCI_SITD_STRUCT_PTR)(((uint32_t) prev_transfer_data_struct_ptr));
                  prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
                  transfer_data_struct_ptr = (void *)SITD_ptr->NEXT_LINK_PTR;
                  break;
               case EHCI_ELEMENT_TYPE_FSTN:
                  FSTN_ptr = (EHCI_FSTN_STRUCT_PTR)(((uint32_t) prev_transfer_data_struct_ptr));
                  prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
                  transfer_data_struct_ptr = (void *)FSTN_ptr->NORMAL_PATH_LINK_PTR;
                  break;
               default:
                  break;
            } /* EndSwitch */
      }
      prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
      while (!(EHCI_MEM_READ(*transfer_data_struct_ptr) & EHCI_QUEUE_HEAD_POINTER_T_BIT))
      {

            switch (EHCI_GET_TYPE(prev_transfer_data_struct_ptr))
            {
               case EHCI_ELEMENT_TYPE_ITD:
                  ITD_ptr = (EHCI_ITD_STRUCT_PTR)(((uint32_t) prev_transfer_data_struct_ptr) &
                                                      EHCI_HORIZ_PHY_ADDRESS_MASK);
                  prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
                  transfer_data_struct_ptr = (void *)EHCI_MEM_READ(ITD_ptr->NEXT_LINK_PTR);
               break;
               case EHCI_ELEMENT_TYPE_QH:
                  temp_QH_ptr = (EHCI_QH_STRUCT_PTR)(((uint32_t) prev_transfer_data_struct_ptr) &
                                                      EHCI_HORIZ_PHY_ADDRESS_MASK);
                  prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
                  transfer_data_struct_ptr = (void *)(EHCI_MEM_READ(temp_QH_ptr->HORIZ_LINK_PTR) & EHCI_HORIZ_PHY_ADDRESS_MASK);
                  if ((EHCI_QH_STRUCT_PTR) transfer_data_struct_ptr == QH_ptr)
                     return;
               break;
               case EHCI_ELEMENT_TYPE_SITD:
                  SITD_ptr = (EHCI_SITD_STRUCT_PTR)(((uint32_t) prev_transfer_data_struct_ptr) &
                                                      EHCI_HORIZ_PHY_ADDRESS_MASK);
                  prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
                  transfer_data_struct_ptr = (void *)EHCI_MEM_READ(SITD_ptr->NEXT_LINK_PTR);
               break;
               case EHCI_ELEMENT_TYPE_FSTN:
                  FSTN_ptr = (EHCI_FSTN_STRUCT_PTR)(((uint32_t) prev_transfer_data_struct_ptr) &
                                                      EHCI_HORIZ_PHY_ADDRESS_MASK);
                  prev_transfer_data_struct_ptr = transfer_data_struct_ptr;
                  transfer_data_struct_ptr = (void *)EHCI_MEM_READ(FSTN_ptr->NORMAL_PATH_LINK_PTR);
               break;
               default:
                  /**************************************************************
                  Should never some here ..clearly means bug or memory corruption
                  ***************************************************************/
                  break;
            } /* EndSwitch */
      } /* EndWhile */


      /*****************************************************************
      time to link with previous data structure.
      *****************************************************************/
      if (prev_transfer_data_struct_ptr != transfer_data_struct_ptr)
      {
         switch (EHCI_GET_TYPE(prev_transfer_data_struct_ptr))
         {

            case EHCI_ELEMENT_TYPE_ITD:
               ITD_ptr = (EHCI_ITD_STRUCT_PTR) transfer_data_struct_ptr;
               EHCI_MEM_WRITE(ITD_ptr->NEXT_LINK_PTR,((uint32_t)QH_ptr | (item_type << EHCI_QH_ELEMENT_TYPE_BIT_POS)));
               break;
            case EHCI_ELEMENT_TYPE_QH:
               temp_QH_ptr = (EHCI_QH_STRUCT_PTR) transfer_data_struct_ptr;
               EHCI_MEM_WRITE(temp_QH_ptr->HORIZ_LINK_PTR,((uint32_t)QH_ptr | (item_type << EHCI_QH_ELEMENT_TYPE_BIT_POS)));

               break;
            case EHCI_ELEMENT_TYPE_SITD:
               SITD_ptr = (EHCI_SITD_STRUCT_PTR) transfer_data_struct_ptr;
               EHCI_MEM_WRITE(SITD_ptr->NEXT_LINK_PTR ,((uint32_t)QH_ptr | (item_type << EHCI_QH_ELEMENT_TYPE_BIT_POS)));
               break;

            case EHCI_ELEMENT_TYPE_FSTN:
               FSTN_ptr = (EHCI_FSTN_STRUCT_PTR) transfer_data_struct_ptr;
               EHCI_MEM_WRITE(FSTN_ptr->NORMAL_PATH_LINK_PTR ,((uint32_t)QH_ptr | (item_type << EHCI_QH_ELEMENT_TYPE_BIT_POS)));
               break;
            default:
               break;
         } /* EndSwitch */

      } /* endif */

      /*******************************
      It means new list is starting out
      ********************************/
      else
      {
         EHCI_MEM_WRITE(*transfer_data_struct_ptr,((uint32_t)QH_ptr | (item_type << EHCI_QH_ELEMENT_TYPE_BIT_POS)));
      }


      return;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : unlink_periodic_data_structure_from_frame
*  Returned Value : None
*  Comments       :
*        unlinks the data structure from periodic list
*END*-----------------------------------------------------------------*/
void unlink_periodic_data_structure_from_frame(
                                          volatile uint32_t  *prev_transfer_data_struct_ptr,
                                          volatile uint32_t  *transfer_data_struct_ptr
)
{
         EHCI_ITD_STRUCT_PTR                          ITD_ptr;
         EHCI_SITD_STRUCT_PTR                         SITD_ptr;
         EHCI_FSTN_STRUCT_PTR                         FSTN_ptr;
         EHCI_QH_STRUCT_PTR                           QH_ptr;
         uint32_t                                     next=0;

         /**************************************************************
          Find the void *to the next structure to be pointed in the list
         **************************************************************/

         switch (EHCI_GET_TYPE(transfer_data_struct_ptr))
         {

            case EHCI_ELEMENT_TYPE_ITD:
               ITD_ptr = (EHCI_ITD_STRUCT_PTR)((EHCI_MEM_READ(*transfer_data_struct_ptr)) & EHCI_HORIZ_PHY_ADDRESS_MASK);
               next  = EHCI_MEM_READ(ITD_ptr->NEXT_LINK_PTR);
               break;
            case EHCI_ELEMENT_TYPE_QH:
               QH_ptr = (EHCI_QH_STRUCT_PTR)((EHCI_MEM_READ(*transfer_data_struct_ptr)) & EHCI_HORIZ_PHY_ADDRESS_MASK);
               next = EHCI_MEM_READ(QH_ptr->HORIZ_LINK_PTR);

               break;
            case EHCI_ELEMENT_TYPE_SITD:
               SITD_ptr = (EHCI_SITD_STRUCT_PTR)((EHCI_MEM_READ(*transfer_data_struct_ptr)) & EHCI_HORIZ_PHY_ADDRESS_MASK);
               next = EHCI_MEM_READ(SITD_ptr->NEXT_LINK_PTR);
               break;

            case EHCI_ELEMENT_TYPE_FSTN:
               FSTN_ptr = (EHCI_FSTN_STRUCT_PTR)((EHCI_MEM_READ(*transfer_data_struct_ptr)) & EHCI_HORIZ_PHY_ADDRESS_MASK);
               next = EHCI_MEM_READ(FSTN_ptr->NORMAL_PATH_LINK_PTR);
               break;
            default:
               break;
         }


          /**************************************************************
            Assign the previous to new one
          **************************************************************/
         if(prev_transfer_data_struct_ptr != transfer_data_struct_ptr)
         {
               switch (EHCI_GET_TYPE(prev_transfer_data_struct_ptr))
               {

                  case EHCI_ELEMENT_TYPE_ITD:
                     ITD_ptr = (EHCI_ITD_STRUCT_PTR)((EHCI_MEM_READ(*prev_transfer_data_struct_ptr)) & EHCI_HORIZ_PHY_ADDRESS_MASK);
                     EHCI_MEM_WRITE(ITD_ptr->NEXT_LINK_PTR,next);
                     break;
                  case EHCI_ELEMENT_TYPE_QH:
                     QH_ptr = (EHCI_QH_STRUCT_PTR)((EHCI_MEM_READ(*prev_transfer_data_struct_ptr)) & EHCI_HORIZ_PHY_ADDRESS_MASK);
                     EHCI_MEM_WRITE(QH_ptr->HORIZ_LINK_PTR,next);

                     break;
                  case EHCI_ELEMENT_TYPE_SITD:
                     SITD_ptr = (EHCI_SITD_STRUCT_PTR)((EHCI_MEM_READ(*prev_transfer_data_struct_ptr)) & EHCI_HORIZ_PHY_ADDRESS_MASK);
                     EHCI_MEM_WRITE(SITD_ptr->NEXT_LINK_PTR,next);
                     break;

                  case EHCI_ELEMENT_TYPE_FSTN:
                     FSTN_ptr = (EHCI_FSTN_STRUCT_PTR)((EHCI_MEM_READ(*prev_transfer_data_struct_ptr)) & EHCI_HORIZ_PHY_ADDRESS_MASK);
                     EHCI_MEM_WRITE(FSTN_ptr->NORMAL_PATH_LINK_PTR,next);
                     break;
                  default:
                     break;
               }
           }
           else
           {
                 EHCI_MEM_WRITE(*prev_transfer_data_struct_ptr,next);
           }

          return;

}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : reclaim_band_width
*  Returned Value : None
*  Comments       :
*        updates the bandwidth list with released time
*END*-----------------------------------------------------------------*/

void reclaim_band_width(
         /* [IN] the USB Host state structure */
         _usb_host_handle                 handle,

        /*Frame number that will get the bandwidth*/
        uint32_t                          frame_number,

        /*data structure that is releasing the bandwidth */
        volatile uint32_t                 *transfer_data_struct_ptr,

        /* The pipe descriptor to queue */
        EHCI_PIPE_STRUCT_PTR              pipe_descr_ptr

)
{
      USB_EHCI_HOST_STATE_STRUCT_PTR               usb_host_ptr;
      uint32_t                                     time_for_action,uframe_index;
      uint8_t                                      *frame_list_bw_ptr;


      usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;
      frame_list_bw_ptr = usb_host_ptr->PERIODIC_FRAME_LIST_BW_PTR;

      /**************************************************************
       Find the void *to the next structure to be pointed in the list
      **************************************************************/

      switch (EHCI_GET_TYPE(transfer_data_struct_ptr))
      {

         case EHCI_ELEMENT_TYPE_ITD:
            //ITD_ptr = (EHCI_ITD_STRUCT_PTR)((EHCI_MEM_READ(*transfer_data_struct_ptr)) & EHCI_HORIZ_PHY_ADDRESS_MASK);
            break;
         case EHCI_ELEMENT_TYPE_QH:
            //QH_ptr = (EHCI_QH_STRUCT_PTR)((EHCI_MEM_READ(*transfer_data_struct_ptr)) & EHCI_HORIZ_PHY_ADDRESS_MASK);
            time_for_action =  pipe_descr_ptr->BWIDTH;

            if (pipe_descr_ptr->G.SPEED == USB_SPEED_HIGH){
               uframe_index = frame_number % 8;
               if (pipe_descr_ptr->BWIDTH_SLOTS[uframe_index]) {
                  frame_list_bw_ptr[frame_number] -= time_for_action;
               }
            }
            else {
               for (uframe_index = 0; uframe_index < 8; uframe_index++) {
                  /* if bandwidth is allocated in this microframe subtract (free) it */
                  if (pipe_descr_ptr->BWIDTH_SLOTS[uframe_index]) {
                      frame_list_bw_ptr[8 * frame_number + uframe_index] -= time_for_action;
                  }
               }
            }
            break;
         case EHCI_ELEMENT_TYPE_SITD:
            //SITD_ptr = (EHCI_SITD_STRUCT_PTR)((EHCI_MEM_READ(*transfer_data_struct_ptr)) & EHCI_HORIZ_PHY_ADDRESS_MASK);
            break;
         case EHCI_ELEMENT_TYPE_FSTN:
            //FSTN_ptr = (EHCI_FSTN_STRUCT_PTR)((EHCI_MEM_READ(*transfer_data_struct_ptr)) & EHCI_HORIZ_PHY_ADDRESS_MASK);
            break;
         default:
            break;
      }


      return;
}



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : allocate_ss_cs_bandwidth
*  Returned Value : None
*  Comments       :
*  Allocates the slots for scheduling split transactions on this pipe.
*END*-----------------------------------------------------------------*/

static USB_STATUS allocate_ss_cs_bandwidth (

     /* [IN] the USB Host state structure */
        _usb_host_handle                  handle,

     /* [IN] The pipe descriptor to queue */
        EHCI_PIPE_STRUCT_PTR              pipe_descr_ptr,

        /*Number of slots required by this pipe*/
        uint32_t                           slots_asked,

        /* starting slot out of 8 */
        uint32_t                           start_slot,

        /* ending slot out of 8 */
        uint32_t                           end_slot

)
{
   /* Generate a pattern where */
   uint8_t  microframes[8] = {0,0,0,0,0,0,0,0};

   /**********************************************************
   Generate all start split combinations for this pipe and
   check the bandwidth allocation for each of them. If this
   fails return bandwidth alloctaion failed. Notice that by
   nature of the work, this is time consuming part of the code
   because of several combinations available and huge
   periodic list to iterate through to find if a combination
   is available. If it is not, we go to the next one.In general
   this code will take time when more devices are plugged in.
   *************************************************************/
   return split_combinations(handle,
                      pipe_descr_ptr,
                      microframes,
                      (int)start_slot,
                      (int)end_slot,
                      (int)slots_asked);
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : split_combinations
*  Returned Value : None
*  Comments       :
*  Finds all combinations for k elements from a set of n elements format.
*END*-----------------------------------------------------------------*/

static USB_STATUS split_combinations
(
      /* [IN] the USB Host state structure */
      _usb_host_handle                 handle,

      /* [IN] The pipe descriptor to queue */
      EHCI_PIPE_STRUCT_PTR             pipe_descr_ptr,

     /* [IN] Array to use to generate combinations */
      uint8_t   v[],

      /* generate combinations starting from */
      int start,

      /* generate combinations until */
      int end,

      /* generate combination for max. n microframes */
      int n
)
{
   int num, i, j;
   uint32_t frame_number;
   uint32_t status = USB_OK;
   USB_EHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;

   /* Prepare slots for bandwidth allocation.
   ** This algorithm tries to allocate as much as possible microframes.
   **
   **
   ** start = 3, end = 5, n = 1
   ** NORMAL             REVERSE
   ** --------------------------
   ** [3]                [5]
   ** [4]                [4]
   ** [5]                [3]
   **
   **
   ** start = 3, end = 5, n = 2
   ** NORMAL             REVERSE
   ** --------------------------
   ** [3, 4]             [4, 5]  110 6
   ** [3, 5]             [3, 5]  101 5
   ** [4, 5]             [3, 4]  011 3
   ** [3]                [5]
   ** [4]                [4]
   ** [5]                [3]
   **
   */
   for (; n > 0; n--) {
      for (j = n; j < 8; j++) { // zero vector unused fields
         v[j] = 0;
      }

      for (num = 1 << start; num < (1 << (end + 1)) - 1; num++) { //search for number having j slots in the slots interval [start, end]
         //test if number i contains j slots (bits set to one)
         j = 0;
         for (i = 0; i < 8; i++) {
            if (num & (1 << i)) {
               j++;
            }
         }
         if (j != n) {
             continue;
         }

         //Found number i having the number of slots as high as requested ('j'). Now, create vector v for slots
         j = 0;
         for (i = 0; i < 8; i++) {
            if (num & (1 << i)) {
               v[j++] = i;
            }
         }

         /* we prepared a combination so we now check if this is available in periodic list if bandwidth is not already allocated */
         if (pipe_descr_ptr->BWIDTH == 0)
         {
            for (frame_number = 0; (frame_number < pipe_descr_ptr->G.INTERVAL) && (frame_number < usb_host_ptr->FRAME_LIST_SIZE); frame_number++)
            {
               status = _usb_ehci_commit_bandwidth_slots(handle,
                                                         pipe_descr_ptr,
                                                         frame_number,
                                                         pipe_descr_ptr->G.INTERVAL,
                                                         v, /* combination to try (and commit if possible) */
                                                         n /*number of slots */
                                                         );

               if (status != USBERR_BANDWIDTH_ALLOC_FAILED)
               {
                  /***************************************************************************
                  Slot allocation was either unsuccessful or there was some other error.
                  ****************************************************************************/
                  return status;
               }
            }
         }
      }
   }

   return status;
}

/* EOF */
