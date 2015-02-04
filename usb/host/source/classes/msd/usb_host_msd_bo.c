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
*   Implementation of mass storage class specification.
*
*
*END************************************************************************/
#include "usb_host_msd_bo.h"

/* Linked list of instances of mass storage interfaces */
static USB_MASS_CLASS_INTF_STRUCT_PTR mass_anchor = NULL;

/* Private functions */
static void usb_class_mass_call_back_cbw(void *, void *, unsigned char *, uint32_t, USB_STATUS);
static void usb_class_mass_call_back_dphase(void *, void *, unsigned char *, uint32_t, USB_STATUS);
static void usb_class_mass_call_back_csw(void *, void *, unsigned char *, uint32_t, USB_STATUS);
static void usb_class_mass_reset_callback(_usb_pipe_handle, void *, unsigned char *, uint32_t, USB_STATUS);
static USB_STATUS usb_class_mass_pass_on_usb(USB_MASS_CLASS_INTF_STRUCT_PTR);
static USB_STATUS usb_class_mass_clear_bulk_pipe_on_usb(USB_MASS_CLASS_INTF_STRUCT_PTR);


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_init
* Returned Value : None
* Comments       :
*     This function initializes the mass storage class
*
*END*--------------------------------------------------------------------*/

void usb_class_mass_init
   (
      /* [IN] structure with USB pipe information on the interface */
      PIPE_BUNDLE_STRUCT_PTR      pbs_ptr,

      /* [IN] printer call struct pointer */
      CLASS_CALL_STRUCT_PTR       ccs_ptr
   )
{ /* Body */
   USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr = ccs_ptr->class_intf_handle;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_init");
   #endif

   USB_lock(); //not needed as init is called from "already locked" function

   /* Pointer validity-checking, clear memory, init header of intf_ptr */
   if (USB_OK != usb_host_class_intf_init(pbs_ptr, intf_ptr, &mass_anchor, NULL)) 
   {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_init");
      #endif
      return;
   }

   ccs_ptr->code_key = 0;
   ccs_ptr->code_key = usb_host_class_intf_validate( ccs_ptr );

   /* Start filling up the members of interface handle (general class already filled) */
   intf_ptr->CONTROL_PIPE = usb_hostdev_get_pipe_handle(pbs_ptr,
      USB_CONTROL_PIPE, 0);
   intf_ptr->BULK_IN_PIPE = usb_hostdev_get_pipe_handle(pbs_ptr,
      USB_BULK_PIPE, USB_RECV);
   intf_ptr->BULK_OUT_PIPE = usb_hostdev_get_pipe_handle(pbs_ptr,
      USB_BULK_PIPE, USB_SEND);

   if (intf_ptr->CONTROL_PIPE    &&
      intf_ptr->BULK_IN_PIPE     &&
      intf_ptr->BULK_OUT_PIPE)  
   {
      /* Initialize the queue for storing the local copy of interface handles */
      usb_class_mass_q_init(intf_ptr);

      /* Store application handle */
      intf_ptr->APP = ccs_ptr;

      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_init, SUCCESSFULL");
      #endif
      
      return;
   } /* Endif */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_init, bad exit");
   #endif
   USB_mem_zero(intf_ptr, sizeof(USB_MASS_CLASS_INTF_STRUCT_PTR));
   ccs_ptr->class_intf_handle = NULL;
   ccs_ptr->code_key = 0;
   USB_unlock();
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_get_app
* Returned Value : CLASS_CALL_STRUCT_PTR if class found
* Comments       :
*     This function returns stored application handle as it was passed as
*     a param in select_interface.
*
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_mass_get_app
   (
      /* [IN] handle of device */
      _usb_device_instance_handle dev_ptr,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_ptr,

      /* [OUT] pointer to CLASS_CALL_STRUCT to be filled in */
      CLASS_CALL_STRUCT_PTR    *ccs_ptr
   )
{
    USB_STATUS                    error;
    GENERAL_CLASS_PTR             parser;

    USB_lock();
 
    error = usb_hostdev_validate (dev_ptr);
    if (error != USB_OK) {
        USB_unlock();
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_class_hub_get_app, FAILED");
        #endif
        return USB_log_error(__FILE__,__LINE__,error);
    } /* EndIf */
    
    for (parser = (GENERAL_CLASS_PTR) mass_anchor; parser != NULL; parser = parser->next) {
        if (parser->dev_handle == dev_ptr && parser->intf_handle == intf_ptr)
            break;
    }
    
    if (parser != NULL) {
        USB_MASS_CLASS_INTF_STRUCT_PTR msd = (USB_MASS_CLASS_INTF_STRUCT_PTR) parser;
        *ccs_ptr = msd->APP;
    }
    else {
        USB_unlock();
        #ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_class_mass_get_app, not found");
        #endif
        return USB_log_error(__FILE__,__LINE__,USBERR_NOT_FOUND);
    }
    
    USB_unlock();
    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_class_mass_get_app, SUCCESSFUL");
    #endif
    return USB_OK;
    
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_mass_storage_device_command
* Returned Value : ERROR STATUS error code
*                  USB_OK - means that command has been successfully queued in class
*                  driver queue (or has been passed to USB, if there is not other
*                  command pending)
* Comments       :
*     This routine is called by the protocol layer to execute the command
*     defined in protocol API. It can also be directly called by a user
*     application if they wish to make their own commands (vendor specific) for
*     sending to a mass storage device.
END*--------------------------------------------------------------------*/

USB_STATUS usb_class_mass_storage_device_command
   (
      /* Handle to class struct with the key */
      CLASS_CALL_STRUCT_PTR      ccs_ptr,

      /*Command */
      COMMAND_OBJECT_PTR         cmd_ptr
   )
{ /* Body */
   USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr;
   int                              temp;
   USB_STATUS                       error = USBERR_NO_INTERFACE;
   CBW_STRUCT_PTR                   pCbw = (CBW_STRUCT_PTR) cmd_ptr->CBW_PTR;
   bool                          empty;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_storage_device_command");
   #endif
   
   /* Pointer validity-checking, clear memory, init header */
   USB_lock();
   if (usb_host_class_intf_validate(ccs_ptr)) {
      intf_ptr = (USB_MASS_CLASS_INTF_STRUCT_PTR) ccs_ptr->class_intf_handle;
      error = usb_hostdev_validate (intf_ptr->G.dev_handle);
   } /* Endif */

   if ((error == USB_OK) || (error == USB_STATUS_TRANSFER_QUEUED)) {
      /* Fill in the remaining CBW fields as per the USB Mass Storage specs */
      *(uint32_t *)pCbw->DCBWSIGNATURE = HOST_TO_LE_LONG_CONST(CBW_SIGNATURE);

      /* CBW is ready so queue it and update status */
      empty = USB_CLASS_MASS_IS_Q_EMPTY(intf_ptr);
      temp = usb_class_mass_q_insert(intf_ptr, cmd_ptr);
      if (temp >= 0) {
         cmd_ptr->STATUS = STATUS_QUEUED_IN_DRIVER;
         
         /* The tag for the command packet is its queue number. */
         *(uint32_t *)pCbw->DCBWTAG = HOST_TO_LE_LONG(temp);

         /*
         ** If queue was empty send CBW to USB immediately, otherwise it will be
         ** taken from queue later by a callback function
         */
         if (empty) {
            error = usb_class_mass_pass_on_usb(intf_ptr);
         } /* Endif */
      } else {
         error = (USB_STATUS)USB_MASS_QUEUE_FULL;
      } /* Endif */
   } /* Endif */
   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_storage_device_command, SUCCESSFUL");
   #endif
   
   return USB_log_error(__FILE__,__LINE__,error);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_storage_device_command_cancel
* Returned Value : ERROR STATUS error code
*                  USB_OK - means that command has been successfully de queued in class
*                  driver queue 

* Comments       :
END*--------------------------------------------------------------------*/

bool usb_class_mass_storage_device_command_cancel
   (
      /* Handle to class struct with the key */
      CLASS_CALL_STRUCT_PTR      ccs_ptr,

      /*Command */
      COMMAND_OBJECT_PTR         cmd_ptr
   )
{ /* Body */
   USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr;
   USB_STATUS                       error = USBERR_NO_INTERFACE;
   bool                          result = FALSE;
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_storage_device_command_cancel");
   #endif
   
   /* Pointer validity-checking, clear memory, init header */
   USB_lock();
   if (usb_host_class_intf_validate(ccs_ptr)) {
      intf_ptr = (USB_MASS_CLASS_INTF_STRUCT_PTR) ccs_ptr->class_intf_handle;
      error = usb_hostdev_validate (intf_ptr->G.dev_handle);
   } /* Endif */

   if ((error == USB_OK) || (error == USB_STATUS_TRANSFER_QUEUED)) {
      result = usb_class_mass_cancelq(intf_ptr, cmd_ptr);
   }
   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_storage_device_command_cancel, SUCCESSFUL");
   #endif
   
   return USB_log_error(__FILE__,__LINE__,result);
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_call_back_cbw
* Returned Value : None
* Comments       :
*     This routine is called when the CBW phase of the currently pending command
*     has finished.
*
*END*--------------------------------------------------------------------*/

void usb_class_mass_call_back_cbw
   (
      /* [IN] Pipe on which CBW call was made */
      _usb_pipe_handle pipe_handle,

      /* [IN] Pointer to the class instance */
      void            *user_parm,

      /* [IN] Pointer to data buffer */
      unsigned char        *buffer,

      /* [IN] Length of data transferred */
      uint32_t          length_data_transfered,

      /* [IN] Status of the transfer */
      USB_STATUS       status
   )
{ /* Body */
   USB_MASS_CLASS_INTF_STRUCT_PTR   plocal_intf = NULL;
   COMMAND_OBJECT_PTR               cmd_ptr = NULL;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_call_back_cbw");
   #endif

   if (user_parm) {
      /* Get the pointer to the pending request */
      plocal_intf = (USB_MASS_CLASS_INTF_STRUCT_PTR)user_parm;
      usb_class_mass_get_pending_request(plocal_intf, &cmd_ptr);
   } /* Endif */

   /* If there is no user_parm or cmd_ptr we return */
   if (!cmd_ptr) 
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_call_back_cbw, bad parameter");
      #endif
      return;
   } /* Endif */

   /* Check status of CBW phase and go to the next phase (Data or Status) */
   if (status == USB_OK || status == USB_STATUS_TRANSFER_QUEUED) {
      /* Verify if the length of the data transferred was correct */
      if (length_data_transfered != CBW_SIZE) {
         if (cmd_ptr->RETRY_COUNT < MAX_RETRIAL_ATTEMPTS) {
            cmd_ptr->RETRY_COUNT++;
            cmd_ptr->STATUS = STATUS_QUEUED_IN_DRIVER;
            cmd_ptr->PREV_STATUS =  cmd_ptr->STATUS; /* preserve the status */
            status = usb_class_mass_reset_recovery_on_usb(plocal_intf);
         } else {
            status = STATUS_CANCELLED; /* cannot keep repeating */
         } /* Endif */
      } else {
         /* Everything is normal, go to next phase of pending request */
         cmd_ptr->STATUS = STATUS_FINISHED_CBW_ON_USB;
         status = usb_class_mass_pass_on_usb(plocal_intf);
      } /* Endif */
   } /* Endif */

   if ((status != USB_OK) && (status != USBERR_TRANSFER_IN_PROGRESS) && (status != USB_STATUS_TRANSFER_QUEUED)) {
      /*
      ** Host must do a reset recovery for the device as per the spec. We also
      ** notify the application of the failure, remove the command from the
      ** queue and move on the the next command queued
      */
      cmd_ptr->STATUS = STATUS_CANCELLED;
      cmd_ptr->CALLBACK((USB_STATUS)USB_MASS_FAILED_IN_COMMAND, plocal_intf, cmd_ptr,
         cmd_ptr->TR_BUF_LEN);
      usb_class_mass_deleteq(plocal_intf);
      status = usb_class_mass_pass_on_usb(plocal_intf);
   } /* Endif */
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_call_back_cbw, SUCCESSFUL");
   #endif
   
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_call_back_dphase
* Returned Value : None
* Comments       :
*     This routine is called by the bus driver when the data phase completes.
*     If status is USB_OK, this routine will pass on the next routine called
*     usb_class_mass_pass_on_usb() which will queue the status phase. If failure
*     the routine will either try to dequeue the packet, or send a Reset device
*     command or will try to requeue the packet for next phase.
*
*END*--------------------------------------------------------------------*/

void usb_class_mass_call_back_dphase
   (
      /* [IN] Pipe on which CBW call was made */
      _usb_pipe_handle pipe_handle,

      /* [IN] Pointer to the class instance */
      void            *user_parm,

      /* [IN] Pointer to data buffer */
      unsigned char        *buffer,

      /* [IN] Length of data transferred */
      uint32_t          length_data_transfered,

      /* [IN] Status of the transfer */
      USB_STATUS       status
   )
{ /* Body */
   USB_MASS_CLASS_INTF_STRUCT_PTR   plocal_intf = NULL;
   COMMAND_OBJECT_PTR               cmd_ptr = NULL;
   USB_STATUS                       return_code;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_call_back_dphase");
   #endif

   if (user_parm) {
      /* Get the pointer to the pending request */
      plocal_intf = (USB_MASS_CLASS_INTF_STRUCT_PTR) user_parm;
      usb_class_mass_get_pending_request(plocal_intf, &cmd_ptr);
   } /* Endif */

   if (!cmd_ptr) 
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_call_back_dphase, bad parameter");
      #endif
      return;
   } /* Endif */

   /* Save length transferred during data phase */
   cmd_ptr->TR_BUF_LEN = length_data_transfered;

   /* Test if full or partial data received */
   if (status == USB_OK || status == USB_STATUS_TRANSFER_QUEUED ||
      ((status == USBERR_TR_FAILED) && (buffer != NULL)))
   {
      /* Everything is OK, update status and go to next phase */
      cmd_ptr->STATUS = STATUS_FINISHED_DPHASE_ON_USB;
      status = usb_class_mass_pass_on_usb(plocal_intf);

   } else if (status == USBERR_ENDPOINT_STALLED) {

      if (cmd_ptr->RETRY_COUNT < MAX_RETRIAL_ATTEMPTS) {
         cmd_ptr->RETRY_COUNT++;
         //cmd_ptr->STATUS = STATUS_QUEUED_IN_DRIVER; /* this is redundant as clearing pipe will change the status */
         cmd_ptr->PREV_STATUS = STATUS_QUEUED_IN_DRIVER; /* preserve the status */

         return_code = usb_class_mass_clear_bulk_pipe_on_usb(plocal_intf);
         
         if (return_code != USB_OK && return_code != USB_STATUS_TRANSFER_QUEUED) {
            status = STATUS_CANCELLED;
         }
         else {
            cmd_ptr->PREV_STATUS = STATUS_FINISHED_DPHASE_ON_USB; /* Force the returning status from pipe clear to continue after data phase */
            status = usb_class_mass_pass_on_usb(plocal_intf);
         }
      } else {
         //cmd_ptr->STATUS = STATUS_FINISHED_DPHASE_ON_USB; /* this is redundant as clearing pipe will change the status */
         cmd_ptr->PREV_STATUS =  STATUS_FINISHED_DPHASE_ON_USB; /* preserve the status */

         return_code = usb_class_mass_clear_bulk_pipe_on_usb(plocal_intf);
         
         if (return_code != USB_OK && return_code != USB_STATUS_TRANSFER_QUEUED)
         {
            status = STATUS_CANCELLED;
         }
      } /* Endif */
   } /* Endif */

   if ((status != USB_OK) && (status != USBERR_ENDPOINT_STALLED) &&
      (status != USBERR_TRANSFER_IN_PROGRESS) && (status != USB_STATUS_TRANSFER_QUEUED))
   {
      /* Command didn't succeed. Notify application and cleanup */
      cmd_ptr->STATUS =STATUS_CANCELLED;
      cmd_ptr->CALLBACK((USB_STATUS)USB_MASS_FAILED_IN_DATA, plocal_intf, cmd_ptr,
         cmd_ptr->TR_BUF_LEN);
      usb_class_mass_deleteq(plocal_intf);

      /* Go to the next command, if any */
      status = usb_class_mass_pass_on_usb(plocal_intf);
   } /* Endif */
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_call_back_dphase, SUCCESSFUL");
   #endif
   
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_call_back_csw
* Returned Value : None
* Comments       : This routine is called by the bus driver when the
*      status phase completes on mass storage device. Please see the specs of
*      USB storage device to check what actions should be taken depending upon
*      the status returned by the device.
*
*END*--------------------------------------------------------------------*/

void usb_class_mass_call_back_csw
   (
      /* [IN] Pipe on which CBW call was made */
      _usb_pipe_handle pipe_handle,

      /* [IN] Pointer to the class instance */
      void            *user_parm,

      /* [IN] Pointer to data buffer */
      unsigned char        *buffer,

      /* [IN] Length of data transferred */
      uint32_t          length_data_transfered,

      /* [IN] Status of the transfer */
      USB_STATUS       status
   )
{ /* Body */
   USB_MASS_CLASS_INTF_STRUCT_PTR   plocal_intf = NULL;
   COMMAND_OBJECT_PTR               cmd_ptr = NULL;
   CSW_STRUCT_PTR                   pCsw = NULL;
   uint32_t                          tmp1, tmp2, tmp3;
   bool                          proc_next = FALSE;
   USB_STATUS                       return_code;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_call_back_csw");
   #endif

   if (user_parm) {
      /* Get the pointer to the pending request */
      plocal_intf = (USB_MASS_CLASS_INTF_STRUCT_PTR) user_parm;
      usb_class_mass_get_pending_request(plocal_intf, &cmd_ptr);
   } /* Endif */

   if (cmd_ptr == NULL) 
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_call_back_csw, bad param");
      #endif
      return;
   } /* Endif */

   /* Finish transaction and queue next command */
   if (status == USB_OK) {
      pCsw = (CSW_STRUCT_PTR) cmd_ptr->CSW_PTR;
      tmp1 = LONG_UNALIGNED_LE_TO_HOST(pCsw->DCSWTAG);
      tmp2 = LONG_UNALIGNED_LE_TO_HOST(cmd_ptr->CBW_PTR->DCBWTAG);
      tmp3 = LONG_UNALIGNED_LE_TO_HOST(pCsw->DCSWSIGNATURE);

      /* Size must be verified, as well as the signature and the tags */
      if ((length_data_transfered != sizeof(CSW_STRUCT)) ||
         (tmp3 != CSW_SIGNATURE) || (tmp1 != tmp2))
      {
         if (cmd_ptr->RETRY_COUNT < MAX_RETRIAL_ATTEMPTS) {
            cmd_ptr->RETRY_COUNT++;
            //cmd_ptr->STATUS = STATUS_FINISHED_DPHASE_ON_USB; /* this is redundant as reseting will change the status */
            cmd_ptr->PREV_STATUS = STATUS_FINISHED_DPHASE_ON_USB; /* preserve the status */

            status = usb_class_mass_reset_recovery_on_usb(plocal_intf);
         } else {
            status = STATUS_CANCELLED; /* cannot keep repeating */
         } /* Endif */

         if ((status != USB_OK)  && (status != USBERR_TRANSFER_IN_PROGRESS) &&
             (status != USB_STATUS_TRANSFER_QUEUED))
         {
            proc_next = TRUE;
         } /* Endif */
      } else {
         /* Check value of status field in CSW */
         switch (pCsw->BCSWSTATUS) {
            case CSW_STATUS_GOOD:
               /* Command succeed. Notify application and cleanup */
               cmd_ptr->STATUS = STATUS_COMPLETED;
               proc_next = TRUE;
               break;

            case CSW_STATUS_FAILED:
               /* Command failed. Notify application and cleanup */
               cmd_ptr->STATUS = STATUS_FAILED_IN_CSW;
               proc_next = TRUE;
               break;

            default:
               break;
         } /* Endswitch */
      } /* Endif */
   }
   else if (status == USBERR_ENDPOINT_STALLED) {
      if (cmd_ptr->RETRY_COUNT < MAX_RETRIAL_ATTEMPTS) {
         cmd_ptr->RETRY_COUNT++;
         //cmd_ptr->STATUS = STATUS_FINISHED_DPHASE_ON_USB;  /* this is redundant as clearing pipe will change the status */
         cmd_ptr->PREV_STATUS = STATUS_FINISHED_DPHASE_ON_USB; /* preserve the status */

         return_code = usb_class_mass_clear_bulk_pipe_on_usb(plocal_intf);
         
         if (return_code != USB_OK && return_code != USB_STATUS_TRANSFER_QUEUED)
         {
            status = USBERR_TRANSFER_IN_PROGRESS;
         }
      } else {
         status = STATUS_CANCELLED; /* cannot keep repeating */
      } /* Endif */

      if ((status != USB_OK) && (status != USBERR_TRANSFER_IN_PROGRESS)) {
         /* Command didn't succeed. Notify application and cleanup */
         cmd_ptr->STATUS = STATUS_CANCELLED;
         proc_next = TRUE;
      } /* Endbody */
   }
   else if (status == USBERR_TR_FAILED) {
      /* after getting NAKs, we should retry the CSW (status) phase */
      status = usb_class_mass_pass_on_usb(plocal_intf);
      proc_next = FALSE;
   }
   else {
      /* Command didn't succeed. Notify application and cleanup */
      cmd_ptr->STATUS = STATUS_CANCELLED;
      proc_next = TRUE;
   } /* Endif */

   /* Should we cleanup and process the next command? */
   if (proc_next) {
      cmd_ptr->CALLBACK(status, plocal_intf, cmd_ptr, cmd_ptr->TR_BUF_LEN);
      usb_class_mass_deleteq(plocal_intf);

      /* Send next command, if any */
      if (!USB_CLASS_MASS_IS_Q_EMPTY(plocal_intf)) {
         status = usb_class_mass_pass_on_usb(plocal_intf);
      } /* Endbody */
   } /* Endif */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_call_back_csw, SUCCESSFUL");
   #endif
   
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_pass_on_usb
* Returned Value : ERROR STATUS of the command
* Comments       :
*     This routine gets the pending request from class driver queue and finds
*     what needs to be done by looking at the status (cmd_ptr->STATUS). If a
*     phase has been completed, it will queue the next phase. If there is no
*     pending request in the queue, it will just return.
*     NOTE: This functions should only be called by a callback or withing a
*     USB_lock() USB_unlock() block!
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_mass_pass_on_usb
   (
      /* [IN] Interface handle */
      USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr
   )
{ /* Body */
   COMMAND_OBJECT_PTR      cmd_ptr = NULL;
   TR_INIT_PARAM_STRUCT    tr_init;
   USB_STATUS              status = USB_OK;
   uint8_t                  tmp;
   uint32_t                 data_len;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_pass_on_usb");
   #endif
   
   /* Nothing can be done if there is nothing pending */
   usb_class_mass_get_pending_request(intf_ptr, &cmd_ptr);
   if (cmd_ptr == NULL) {
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_pass_on_usb, no matching request");
      #endif
      return (USB_STATUS)USB_MASS_NO_MATCHING_REQUEST;
   } /* Endif */

   /* Determine the appropriate action based on the phase */
   switch (cmd_ptr->STATUS) {
      case STATUS_QUEUED_IN_DRIVER:
         /* means that CBW needs to be sent.*/
         usb_hostdev_tr_init(&tr_init, usb_class_mass_call_back_cbw, (void *)intf_ptr);
         tr_init.G.TX_BUFFER = (unsigned char *)cmd_ptr->CBW_PTR;
         tr_init.G.TX_LENGTH = sizeof(CBW_STRUCT);

         status = _usb_host_send_data(intf_ptr->G.host_handle,
            intf_ptr->BULK_OUT_PIPE, &tr_init);
         break;

      case STATUS_FINISHED_CBW_ON_USB:
         /* Determine next phase (DATA or STATUS), length and direction */
         data_len = LONG_UNALIGNED_LE_TO_HOST(cmd_ptr->CBW_PTR->DCBWDATATRANSFERLENGTH);

         if (data_len > 0) {
            /* Commen TR setup for IN or OUT direction */
            usb_hostdev_tr_init(&tr_init,usb_class_mass_call_back_dphase, (void *)intf_ptr);

            tmp = (uint8_t)((cmd_ptr->CBW_PTR->BMCBWFLAGS) & MASK_NON_DIRECTION_BITS);
            switch(tmp) {
               case DIRECTION_OUT:
                  /* Make a TR with DATA Phase call back.*/
                  tr_init.G.TX_BUFFER = (unsigned char *)cmd_ptr->DATA_BUFFER;
                  tr_init.G.TX_LENGTH = cmd_ptr->BUFFER_LEN;

                  status = _usb_host_send_data(intf_ptr->G.host_handle, intf_ptr->BULK_OUT_PIPE, &tr_init);
                  break;

               case DIRECTION_IN:
                  /* Make a TR with DATA call back.*/
                  tr_init.G.RX_BUFFER = (unsigned char *)cmd_ptr->DATA_BUFFER;
                  tr_init.G.RX_LENGTH = cmd_ptr->BUFFER_LEN;

                  status = _usb_host_recv_data(intf_ptr->G.host_handle, intf_ptr->BULK_IN_PIPE, &tr_init);
                  break;

               default:
                  break;
            } /* Endswitch */
            break;
         } /* Endif */

         /*
         ** else case:
         ** No data transfer is expected of the request. Fall through to
         ** STATUS phase
         */

      case STATUS_FINISHED_DPHASE_ON_USB:
         /* Make a TR and send it with STATUS call back */
         usb_hostdev_tr_init(&tr_init,usb_class_mass_call_back_csw, (void *)intf_ptr);
         tr_init.G.RX_BUFFER = (unsigned char *)cmd_ptr->CSW_PTR;
         tr_init.G.RX_LENGTH = sizeof(CSW_STRUCT);

         status = _usb_host_recv_data(intf_ptr->G.host_handle, intf_ptr->BULK_IN_PIPE, &tr_init);
            
         break;

      case STATUS_FINISHED_CSW_ON_USB: /* No action */
      case STATUS_FAILED_IN_CSW:       /* Should never happen */
      default:
         break;
   } /* Endswitch */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_pass_on_usb, SUCCESSFUL");
   #endif
   
   return USB_log_error(__FILE__,__LINE__,status);
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_getmaxlun_bulkonly
* Returned Value : ERROR STATUS of the command
* Comments       :
*     This is a class specific command. See the documentation of the USB
*     mass storage specification to learn how this command works. This command
*     is used the get the Number of Logical Units on the device. Caller will
*     use the LUN number to direct the commands (as a part of CBW)
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_mass_getmaxlun_bulkonly
   (
      CLASS_CALL_STRUCT_PTR      ccs_ptr,
      uint8_t                 *pLUN,
      tr_callback                callback,
      void                      *callback_param
   )
{ /* Body */
   USB_STATUS                       status = USBERR_NO_INTERFACE;
   USB_SETUP                        request;
   USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_getmaxlun_bulkonly");
   #endif
   
   /* Pointer validity-checking, clear memory, init header */
   USB_lock();
   if (usb_host_class_intf_validate(ccs_ptr)) {
      intf_ptr = (USB_MASS_CLASS_INTF_STRUCT_PTR) ccs_ptr->class_intf_handle;
      status = usb_hostdev_validate(intf_ptr->G.dev_handle);
   } /* Endif */

   if (status) {
      USB_unlock();

      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_getmaxlun_bulkonly, error status");
      #endif
   return USB_log_error(__FILE__,__LINE__,status);
   } /* Endif */

   /* Get the number of logical units on the device */
   request.BMREQUESTTYPE = REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_IN;
   request.BREQUEST = GET_MAX_LUN;
   *(uint16_t*)request.WVALUE = HOST_TO_LE_SHORT_CONST(0);
   *(uint16_t*)request.WINDEX = HOST_TO_LE_SHORT(((INTERFACE_DESCRIPTOR_PTR)intf_ptr->G.intf_handle)->bInterfaceNumber);
   *(uint16_t*)request.WLENGTH = HOST_TO_LE_SHORT_CONST(1);

   /* Send request */
   status = _usb_hostdev_cntrl_request (intf_ptr->G.dev_handle,
      &request, (unsigned char *)pLUN, callback, callback_param);
   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_getmaxlun_bulkonly, SUCCESSFUL");
   #endif
   
   return USB_log_error(__FILE__,__LINE__,status);
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_clear_bulk_pipe_on_usb
* Returned Value : ERROR STATUS of the command
* Comments       :
*     This routine gets the pending request from class driver queue and sends
      the CLEAR_FEATURE command on control pipe. This routine is called when
      status phase of the pending command fails and class driver decides to
      clear the pipes. If there is no pending request in the queue, it will
      just return.
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_mass_clear_bulk_pipe_on_usb
   (
      USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr
   )
{ /* Body */
   USB_STATUS                    status = USB_OK;
   PIPE_INIT_PARAM_STRUCT_PTR    pBulk_pipe = NULL;
   COMMAND_OBJECT_PTR            cmd_ptr = NULL;
   uint8_t                        direction;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_clear_bulk_pipe_on_usb");
   #endif
   
   /* Nothing can be done if there is nothing pending */
   usb_class_mass_get_pending_request(intf_ptr, &cmd_ptr);
   if (cmd_ptr == NULL) 
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_clear_bulk_pipe_on_usb, no matching request");
      #endif
      return USB_log_error(__FILE__,__LINE__,(USB_STATUS)USB_MASS_NO_MATCHING_REQUEST);
   } /* Endif */

   /* Determine pipe (Bulk IN or Bulk OUT) to clear */
   if ((cmd_ptr->CBW_PTR->BMCBWFLAGS & MASK_NON_DIRECTION_BITS) == DIRECTION_IN) {
      pBulk_pipe = (PIPE_INIT_PARAM_STRUCT_PTR)intf_ptr->BULK_IN_PIPE;
      direction = REQ_TYPE_IN;
   } else {
      pBulk_pipe = (PIPE_INIT_PARAM_STRUCT_PTR)intf_ptr->BULK_OUT_PIPE;
      direction = REQ_TYPE_OUT;
   } /* Endif */

   status = _usb_host_register_ch9_callback(intf_ptr->G.dev_handle,
      usb_class_mass_reset_callback, (void *)intf_ptr);

   if (status == USB_OK || status == USB_STATUS_TRANSFER_QUEUED) {
      /* Send a CLEAR PIPE command */
      cmd_ptr->STATUS = STATUS_CLEAR_BULK_PIPE;
      status = _usb_host_ch9_clear_feature(intf_ptr->G.dev_handle,
         REQ_TYPE_ENDPOINT, (uint8_t)(pBulk_pipe->G.ENDPOINT_NUMBER | direction), ENDPOINT_HALT);
   } /* Endif */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_clear_bulk_pipe_on_usb, SUCCESSFUL");
   #endif
   
   return USB_log_error(__FILE__,__LINE__,status);
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_reset_recovery_on_usb
* Returned Value : ERROR STATUS of the command
* Comments       :
*     This routine gets the pending request from class driver queue and sends
*     the RESET command on control pipe. This routine is called when
*     a phase of the pending command fails and class driver decides to
*     reset the device. If there is no pending request in the queue, it will
*     just return. This routine registers a call back for control pipe commands
*     to ensure that pending command is queued again.
*     NOTE: This functions should only be called by a callback or withing a
*     USB_lock() USB_unlock() block!
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_mass_reset_recovery_on_usb
   (
      USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr
   )
{ /* Body */
   COMMAND_OBJECT_PTR         cmd_ptr = NULL;
   USB_STATUS                 status = USB_OK;
   USB_SETUP                  request;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_reset_recovery_on_usb");
   #endif
   
   /* Nothing can be done if there is nothing pending*/
   usb_class_mass_get_pending_request(intf_ptr,&cmd_ptr);
   if(cmd_ptr == NULL) 
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_reset_recovery_on_usb,no matching request");
      #endif
      return (USB_STATUS) USB_MASS_NO_MATCHING_REQUEST;
   } /* Endif */

   /* BULK device mass storage reset */
   request.BMREQUESTTYPE   = REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_OUT;
   request.BREQUEST        = MASS_STORAGE_RESET;
   *(uint16_t*)request.WVALUE = HOST_TO_LE_SHORT_CONST(0);
   *(uint16_t*)request.WINDEX = HOST_TO_LE_SHORT(((INTERFACE_DESCRIPTOR_PTR)intf_ptr->G.intf_handle)->bInterfaceNumber);
   *(uint16_t*)request.WLENGTH = HOST_TO_LE_SHORT_CONST(0);

   cmd_ptr->STATUS = STATUS_RESET_DEVICE;
   status = _usb_hostdev_cntrl_request (intf_ptr->G.dev_handle, &request, NULL,
      usb_class_mass_reset_callback, (void *)intf_ptr);

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_reset_recovery_on_usb,SUCCESSFUL");
   #endif
      
   return USB_log_error(__FILE__,__LINE__,status);
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_reset_callback
* Returned Value : None
* Comments       : 
*     Callback used when resetting and clearing endpoints. It is also
*     state machine for reset recovery.
*
*END*--------------------------------------------------------------------*/

static void usb_class_mass_reset_callback
   (
      /* [IN] pointer to pipe */
      _usb_pipe_handle  pipe_handle,

      /* [IN] user-defined parameter */
      void             *user_parm,

      /* [IN] buffer address */
      unsigned char         *buffer,

      /* [IN] length of data transferred */
      uint32_t           length_data_transfered,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      USB_STATUS        status
   )
{ /* Body */

   USB_MASS_CLASS_INTF_STRUCT_PTR   plocal_intf = NULL;
   PIPE_STRUCT_PTR                  pPipe;
   COMMAND_OBJECT_PTR               cmd_ptr = NULL;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_reset_callback");
   #endif

   if (user_parm) {
      /* Get the pointer to the pending request */
      plocal_intf = (USB_MASS_CLASS_INTF_STRUCT_PTR)user_parm;
      usb_class_mass_get_pending_request(plocal_intf, &cmd_ptr);
   } /* Endif */

   if (!cmd_ptr) 
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_reset_callback, bad param");
      #endif
      return;
   } /* Endif */

   /* Check the status of the reset and act accordingly */
   if (status == USB_OK || status == USB_STATUS_TRANSFER_QUEUED) {
      switch (cmd_ptr->STATUS) {
         case STATUS_RESET_DEVICE:
            /* Reset the Bulk IN pipe */
            pPipe = (PIPE_STRUCT_PTR)plocal_intf->BULK_IN_PIPE;
            if (!_usb_host_register_ch9_callback(plocal_intf->G.dev_handle,
               usb_class_mass_reset_callback, plocal_intf))
            {
               status = _usb_host_ch9_clear_feature(plocal_intf->G.dev_handle,
                  REQ_TYPE_ENDPOINT, (uint8_t)(pPipe->ENDPOINT_NUMBER | REQ_TYPE_IN), ENDPOINT_HALT);
               cmd_ptr->STATUS = STATUS_RESET_BULK_IN;
            } /* Endif */
            break;

         case STATUS_RESET_BULK_IN:
            /* Reset the Bulk OUT pipe*/
            pPipe = (PIPE_STRUCT_PTR)plocal_intf->BULK_OUT_PIPE;
            if (!_usb_host_register_ch9_callback(plocal_intf->G.dev_handle,
               usb_class_mass_reset_callback, plocal_intf))
            {
               status = _usb_host_ch9_clear_feature(plocal_intf->G.dev_handle,
                  REQ_TYPE_ENDPOINT, (uint8_t)(pPipe->ENDPOINT_NUMBER | REQ_TYPE_OUT), ENDPOINT_HALT);
               cmd_ptr->STATUS   =   STATUS_RESET_BULK_OUT;
            } /* Endif */
            break;

         case STATUS_RESET_BULK_OUT:
         case STATUS_CLEAR_BULK_PIPE:
            pPipe = (PIPE_STRUCT_PTR)plocal_intf->BULK_IN_PIPE;
            pPipe->NEXTDATA01 = 0; /* reset the NEXTDATA toggle bit */
            cmd_ptr->STATUS = cmd_ptr->PREV_STATUS; /* restore status set prior to call reset function */
            status = usb_class_mass_pass_on_usb(plocal_intf);
            break;

         default:
            /* we should never appear here, however this case is handled by resetting the device */
            status = usb_class_mass_reset_recovery_on_usb(plocal_intf);
            break;
      } /* Endswitch */

   } else if (status == USBERR_ENDPOINT_STALLED) {

      cmd_ptr->STATUS = STATUS_CANCELLED;
      cmd_ptr->CALLBACK(status, plocal_intf, cmd_ptr, cmd_ptr->TR_BUF_LEN);
      usb_class_mass_deleteq(plocal_intf);

      /* go to the next packet if any */
      usb_class_mass_pass_on_usb(plocal_intf);
   } /* Endif */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_reset_callback, SUCCESSFUL");
   #endif
   
} /* Endbody */

/* EOF */

