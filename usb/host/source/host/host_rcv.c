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
*   This file contains the USB Host API specific functions to receive data.
*
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_main.h"
#include "host_rcv.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_recv_data
*  Returned Value : error or status of the transfer
*  Comments       :
* The Receive Data routine is non-blocking routine that causes a buffer 
* to be made available for data recieved from the 
* USB host. It takes the buffer and passes it down to lower
level hardware driver. 
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_recv_data
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle           handle, 

      /* [IN] the pipe handle */
      _usb_pipe_handle           pipe_handle,

      /* [IN] transfer parameters */
      TR_INIT_PARAM_STRUCT_PTR   tr_params_ptr
   )
{ /* Body */
   PIPE_STRUCT_PTR           pipe_ptr;
   USB_STATUS    status;
   TR_STRUCT_PTR tr_ptr;
   USB_STATUS    return_code;   

#ifdef _HOST_DEBUG_
   DEBUG_LOG_TRACE("_usb_host_recv_data");
#endif

   USB_lock();

   return_code = _usb_host_set_up_tr(pipe_handle, tr_params_ptr, &tr_ptr);

   if (return_code != USB_STATUS_TRANSFER_QUEUED) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_recv_data transfer queue failed");
      #endif
      return USB_log_error(__FILE__,__LINE__,return_code);
   } /* Endif */

   pipe_ptr = (PIPE_STRUCT_PTR)pipe_handle;
#if PSP_HAS_DATA_CACHE
    /* We will do soon cache invalidation of buffer. This can have an impact to the
    ** data that sit in the same cache line, but are not part of the buffer. In order
    ** not to invalidate their cached values, we do first flushing those data.
    */
    if (tr_ptr->RX_BUFFER < (uint8_t *)__UNCACHED_DATA_START || ((tr_ptr->RX_BUFFER + tr_ptr->RX_LENGTH) > (uint8_t *)__UNCACHED_DATA_END)) {
        if (( (uint32_t)tr_ptr->RX_BUFFER & (PSP_CACHE_LINE_SIZE - 1)) != 0)
            USB_dcache_flush_mlines((void *)tr_ptr->RX_BUFFER, PSP_CACHE_LINE_SIZE);
        if (((uint32_t)((uint8_t *)tr_ptr->RX_BUFFER + tr_ptr->RX_LENGTH) & (PSP_CACHE_LINE_SIZE - 1)) != 0)
            USB_dcache_flush_mlines((uint8_t *)tr_ptr->RX_BUFFER + tr_ptr->RX_LENGTH, PSP_CACHE_LINE_SIZE);
        USB_dcache_invalidate_mlines((void *)tr_ptr->RX_BUFFER, tr_ptr->RX_LENGTH);
    }
#endif
   /* We have obtained the current TR on the Pipe's TR list 
   ** from _usb_host_set_up_tr
   */

   /* Call the low-level routine to receive data */
   status = _usb_host_recv_data_call_interface (handle, pipe_ptr, tr_ptr);
         
   USB_unlock();
	   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_recv_data SUCCESSFUL");
   #endif

   if(status == USB_OK) 
   {
      return USB_STATUS_TRANSFER_QUEUED;
   }
   else
   {
      return USB_log_error(__FILE__,__LINE__,status);
   }

   
} /* Endbody */

/* EOF */
