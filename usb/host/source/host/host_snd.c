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
*   This file contains the USB Host API specific functions to send data.
*
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_main.h"
#include "host_snd.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_get_frame_number
*  Returned Value : frame number
*  Comments       :
* This function can be used to inquire the current frame number.
*
*END*-----------------------------------------------------------------*/
uint32_t _usb_host_get_frame_number
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle  handle
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr;
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_get_frame_number");
   #endif

      /* Call the low-level send routine */
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_get_frame_number OS mode SUCCESSFUL");
   #endif

    return((USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->HOST_GET_FRAME_NUM(handle);
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_get_micro_frame_number
*  Returned Value : frame number
*  Comments       :
* This function can be used to inquire the current micro frame number.
*
*END*-----------------------------------------------------------------*/
uint32_t _usb_host_get_micro_frame_number
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle  handle
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr;
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_get_micro_frame_number");
   #endif

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_get_micro_frame_number OS mode SUCCESSFUL");
   #endif

      /* Call the low-level send routine */
    return((USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->HOST_GET_MICRO_FRAME_NUM(handle);
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_send_data
*  Returned Value : error or status of the transfer
*  Comments       :
* The Send Data routine is non-blocking routine that causes a block of data
* to be made available for transmission to the USB host.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_send_data
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle           handle,

      /* [IN] the pipe handle */
      _usb_pipe_handle           pipe_handle,

      /* [IN] transfer parameters */
      TR_INIT_PARAM_STRUCT_PTR   tr_params_ptr
   )
{ /* Body */
   PIPE_STRUCT_PTR            pipe_ptr;
   TR_STRUCT_PTR              tr_ptr;
   uint32_t  return_code;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_send_data");
   #endif

   USB_lock();
   return_code = _usb_host_set_up_tr(pipe_handle, tr_params_ptr, &tr_ptr);
   if (return_code != USB_STATUS_TRANSFER_QUEUED) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_send_data failed to queue transfer");
      #endif

      return USB_log_error(__FILE__,__LINE__,return_code);
   } /* Endif */

   pipe_ptr = (PIPE_STRUCT_PTR)pipe_handle;

#if PSP_HAS_DATA_CACHE      // TODO check - maybe uncached allocation used/can be used
  /*Must Flush and Invalidate the buffer before sending
   /receiving the data in it */
   if (tr_ptr->TX_BUFFER < (uint8_t *)__UNCACHED_DATA_START || ((tr_ptr->TX_BUFFER + tr_ptr->TX_LENGTH) > (uint8_t *)__UNCACHED_DATA_END))
       USB_dcache_flush_mlines((void *)tr_ptr->TX_BUFFER, tr_ptr->TX_LENGTH);
#endif // PSP_HAS_DATA_CACHE

   /* We have obtained the current TR on the Pipe's TR list
   ** from _usb_host_set_up_tr
   */

   /* Call the low-level send routine */
   return_code = _usb_host_send_call_interface (handle, pipe_ptr, tr_ptr);

   if (return_code != USB_OK) {
      /* mark the transaction as unused */
      tr_ptr->TR_INDEX = 0;
   }

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_send_data SUCCESSFUL");
   #endif

   if(return_code == USB_OK)
   {
      return USB_STATUS_TRANSFER_QUEUED;
   }
   else
   {
      return USB_log_error(__FILE__,__LINE__,return_code);
   }

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_send_setup
*  Returned Value : None
*  Comments       :
*  Sends a Setup packet. Internally, following the SendSetup call
*  this takes care of the whole transaction (including receiving or sending
*  any data to or from the device.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_send_setup
   (
      /* [IN] the USB Host state structure   */
      _usb_host_handle           handle,

      /* [IN] the pipe handle */
      _usb_pipe_handle           pipe_handle,

      /* [IN] transfer parameters */
      TR_INIT_PARAM_STRUCT_PTR   tr_params_ptr
   )
{ /* Body */
   TR_STRUCT_PTR              tr_ptr;
   PIPE_STRUCT_PTR            pipe_ptr;
   uint32_t                    return_code;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_send_setup");
   #endif

   USB_lock();

   /* Initialize a TR and link it into the pipe's TR list */
   return_code = _usb_host_set_up_tr(pipe_handle, tr_params_ptr, &tr_ptr);

   if (return_code != USB_STATUS_TRANSFER_QUEUED) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_send_setup error status");
      #endif

      return return_code;
   } /* Endif */

   pipe_ptr = (PIPE_STRUCT_PTR)pipe_handle;

   pipe_ptr->NEXTDATA01 = 0;       /* reset DATA0/1 */

   /* We have obtained the current TR on the Pipe's TR list
   ** from _usb_host_set_up_tr
   */
   /* true if this setup packet will have a send data phase */
   tr_ptr->SEND_PHASE = (bool)!(tr_ptr->setup_packet.BMREQUESTTYPE & USB_SETUP_DATA_XFER_DIRECTION);

   /***************************************************************
   For data caching it is important that we update the memory
   with the intended contents.
   ***************************************************************/
#if PSP_HAS_DATA_CACHE
    USB_dcache_flush_mlines((void *)&tr_ptr->setup_packet, 1);

    if (tr_ptr->TX_LENGTH > 0)
    {
        /* We do flush the buffer first before used by DMA */
        USB_dcache_flush_mlines((void *)tr_ptr->TX_BUFFER, tr_ptr->TX_LENGTH);
    }
    else if (tr_ptr->RX_LENGTH > 0)
    {
        /* We will do soon cache invalidation of buffer. This can have an impact to the
        ** data that sit in the same cache line, but are not part of the buffer. In order
        ** not to invalidate their cached values, we do first flushing those data.
        */
        USB_dcache_flush_mlines((void *)tr_ptr->RX_BUFFER, tr_ptr->RX_LENGTH);
        /* To ensure that the USB DMA transfer will work on a buffer that is not cached,
        ** we invalidate buffer cache lines.
        */
        USB_dcache_invalidate_mlines((void *)tr_ptr->RX_BUFFER, tr_ptr->RX_LENGTH);
    }

#endif // PSP_HAS_DATA_CACHE


   /* Call the low-level routine to send a setup packet */
   return_code = _usb_host_send_setup_call_interface (handle, pipe_ptr, tr_ptr);

   USB_unlock();

   if (return_code == USB_OK)
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_send_setup SUCCESSFUL");
      #endif

      return USB_STATUS_TRANSFER_QUEUED;
   }
   else
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_send_setup FAILED");
      #endif

      return USB_log_error(__FILE__,__LINE__,return_code);
   }

} /* Endbody */

/* EOF */
