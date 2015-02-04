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
*   This file contains the USB Host API specific functions to close pipe(s).
*
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_main.h"
#include "host_close.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_close_pipe
*  Returned Value : None
*  Comments       :
*        _usb_host_close_pipe routine removes the pipe from the open pipe list
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_close_pipe
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle     handle,
      
      /* [IN] the pipe (handle) to close */
      _usb_pipe_handle     pipe_handle
   )
{ /* Body */
   USB_STATUS error;
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   PIPE_STRUCT_PTR pipe_ptr = (PIPE_STRUCT_PTR)pipe_handle;
   PIPE_STRUCT_PTR pipe_bkp_ptr;
   TR_STRUCT_PTR tr_ptr, tr_ptr_next;
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_close_pipe");
   #endif

   if (pipe_ptr->PIPE_ID > USBCFG_MAX_PIPES) {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_close_pipe invalid pipe");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_INVALID_PIPE_HANDLE);
   } /* Endif */

   USB_lock();

   /* Call the low-level routine to free the controller specific resources */
   error = _usb_host_close_pipe_call_interface (handle, pipe_ptr);

   if (error != USB_OK)
   {
      USB_unlock();

      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_close_pipe FAILED");
      #endif
         
      return USB_log_error(__FILE__,__LINE__,error);
   }

   /* Before cleaning pipe_ptr struct we have to make sure that structures in the circullar list are really deallocated */
   /* Before deallocating structures in the list call the callback about failed pending transaction */
   if (pipe_ptr->tr_list_ptr != NULL) {
      tr_ptr = pipe_ptr->tr_list_ptr;
      do {
         tr_ptr_next = tr_ptr->NEXT;
         if (tr_ptr->TR_INDEX !=0 && tr_ptr->CALLBACK != NULL) {
            if (tr_ptr->RX_BUFFER != NULL) {
                tr_ptr->CALLBACK((void *)pipe_ptr, tr_ptr->CALLBACK_PARAM, tr_ptr->RX_BUFFER, 0, USBERR_PIPE_CLOSED);
            }
            else {
                tr_ptr->CALLBACK((void *)pipe_ptr, tr_ptr->CALLBACK_PARAM, tr_ptr->TX_BUFFER, 0, USBERR_PIPE_CLOSED);
            }
         }	 
         /* free transaction belonging to this pipe */
         USB_mem_free(tr_ptr);
         tr_ptr = tr_ptr_next;
      } while (tr_ptr != pipe_ptr->tr_list_ptr);  

      //pipe_ptr->tr_list_ptr = NULL; //this will be done later by zeroing pipe_ptr
   }

   /* De-initialise the pipe descriptor, but preserve the chain */
   pipe_bkp_ptr = pipe_ptr->NEXT;

   USB_mem_zero(pipe_ptr, usb_host_ptr->PIPE_SIZE);
   /* restore the chain */
   pipe_ptr->NEXT = pipe_bkp_ptr;

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_close_pipe SUCCESSFUL");
   #endif

   return USB_OK;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_close_all_pipes
*  Returned Value : None
*  Comments       :
*  _usb_host_close_all_pipes routine removes the pipe from the open pipe list
*
*END*-----------------------------------------------------------------*/
void  _usb_host_close_all_pipes
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle  handle
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr;
   PIPE_STRUCT_PTR pipe_ptr, pipe_bkp_ptr;
   TR_STRUCT_PTR tr_ptr, tr_ptr_next;
   
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_close_all_pipes");
   #endif

   USB_lock();

   for (pipe_ptr = usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR; pipe_ptr != NULL; pipe_ptr = pipe_ptr->NEXT) {
      if (pipe_ptr->OPEN) {
         /* Call the low-level routine to free the controller specific resources for this pipe */
         _usb_host_close_pipe_call_interface (handle, pipe_ptr);

         /* before cleaning pipe_ptr struct we need to deallocate structures in the circullar list */
         if (pipe_ptr->tr_list_ptr != NULL) {
            tr_ptr = pipe_ptr->tr_list_ptr;
            do {
               tr_ptr_next = tr_ptr->NEXT;
               /* free transaction belonging to this pipe */
               USB_mem_free(tr_ptr);
               tr_ptr = tr_ptr_next;
            } while (tr_ptr != pipe_ptr->tr_list_ptr);  

            //pipe_ptr->tr_list_ptr = NULL; //this will be done later by zeroing pipe_ptr
         }

         /* de-initialise the pipe descriptor, but prevent the chain */
         pipe_bkp_ptr = pipe_ptr->NEXT;

         USB_mem_zero(pipe_ptr, sizeof(PIPE_STRUCT));
         /* restore the chain */
         pipe_ptr->NEXT = pipe_bkp_ptr;
      } /* Endif */
   } /* Endfor */
   
   USB_unlock();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_close_all_pipes SUCCESSFUL");
   #endif
   
} /* Endbody */

/* EOF */
