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
*   This file contains the USB Host API specific functions to cancel 
*   a transfer.
*
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_main.h"
#include "host_cnl.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_cancel_transfer
*  Returned Value : error or status of the transfer before cancellation
*  Comments       :
* _usb_host_cancel_transfer is a non-blocking routine that causes a transfer to 
* be terminated. 
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_cancel_transfer
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle     handle,

      /* [IN] the pipe handle */
      _usb_pipe_handle     pipe_handle,

      /* [IN] the transfer numnber for this pipe */
      uint32_t              tr_number
   )
{ /* Body */
   PIPE_STRUCT_PTR            pipe_descr_ptr = (PIPE_STRUCT_PTR)pipe_handle;
   TR_STRUCT_PTR              pipe_tr_ptr;
   USB_STATUS error;
   uint32_t  bRet;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_cancel_transfer");
   #endif

   USB_lock();

   /* Check if valid pipe id */
   if (pipe_descr_ptr->PIPE_ID > USBCFG_MAX_PIPES) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_cancel_transfer, invalid pipe");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_PIPE_HANDLE);
   } /* Endif */

   pipe_tr_ptr = pipe_descr_ptr->tr_list_ptr;
   while (pipe_tr_ptr->TR_INDEX != tr_number)  {
      /* If unused TR found, or end of list, exit with bad status */
      if ((pipe_tr_ptr->TR_INDEX == 0) ||
         (pipe_tr_ptr->NEXT== pipe_descr_ptr->tr_list_ptr))
      {
         USB_unlock();
         #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_cancel_transfer, invalid pipe");
         #endif
         return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_PIPE_HANDLE);
      } /* EndIf */
      pipe_tr_ptr = pipe_tr_ptr->NEXT;   
   } /* EndWhile */

   /* Get the current status */
   bRet = pipe_tr_ptr->STATUS;

   /* Cancel the transfer at low-level */
   error = _usb_host_cancel_call_interface (handle, pipe_descr_ptr, pipe_tr_ptr);
   
   if (error != USB_OK)
   {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_cancel_transfer, FAILED");
      #endif
      return USB_log_error(__FILE__,__LINE__,error);
   }

   /* Indicate that the pipe is idle */
   pipe_tr_ptr->STATUS = USB_STATUS_IDLE;

   if (pipe_tr_ptr->CALLBACK != NULL) {
      pipe_tr_ptr->CALLBACK((void *)pipe_descr_ptr,
         pipe_tr_ptr->CALLBACK_PARAM,
         NULL,
         0,
         USBERR_SHUTDOWN);
   } /* Endif */

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_cancel_transfer,SUCCESSFUL");
   #endif

   /* Return the status prior to the transfer cancellation */
   return bRet;

} /* Endbody */
 
/* EOF */
