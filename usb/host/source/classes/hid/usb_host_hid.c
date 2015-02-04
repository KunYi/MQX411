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
*   This file Contains the implementation of class driver for HID devices.
*
*
*END************************************************************************/

#include "usb_host_hid.h"


static USB_HID_CLASS_INTF_STRUCT_PTR hid_anchor = NULL;


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_hid_mass_init
* Returned Value : None
* Comments       :
*     This function is called by common class to initialize the class driver. It
*     is called in response to a select interface call by application
*
*END*--------------------------------------------------------------------*/

void usb_class_hid_init
   (
      /* [IN]  structure with USB pipe information on the interface */
      PIPE_BUNDLE_STRUCT_PTR      pbs_ptr,

      /* [IN] printer call struct pointer */
      CLASS_CALL_STRUCT_PTR       ccs_ptr
   )
{ /* Body */
   USB_HID_CLASS_INTF_STRUCT_PTR if_ptr = ccs_ptr->class_intf_handle;
   USB_STATUS                    status;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hid_init");
   #endif
   

   /* Make sure the device is still attached */
   USB_lock(); //not needed as init is called from "already locked" function

   status = usb_host_class_intf_init(pbs_ptr, if_ptr, &hid_anchor, NULL);
   if (status == USB_OK) {
      /*
      ** We generate a code_key based on the attached device. This is used to
      ** verify that the device has not been detached and replaced with another.
      */
      ccs_ptr->code_key = 0;
      ccs_ptr->code_key = usb_host_class_intf_validate(ccs_ptr);

//      if_ptr->IFNUM =
//         ((INTERFACE_DESCRIPTOR_PTR)if_ptr->G.intf_handle)->bInterfaceNumber;

//      if_ptr->P_CONTROL = usb_hostdev_get_pipe_handle(pbs_ptr, USB_CONTROL_PIPE,
//         0);
//      if_ptr->P_INT_IN = usb_hostdev_get_pipe_handle(pbs_ptr,
//         USB_INTERRUPT_PIPE, USB_RECV);
   } /* Endif */

   /* Signal that an error has occured by setting the "code_key" to 0 */
//   if (status || !if_ptr->P_INT_IN || !if_ptr->P_CONTROL) {
//      ccs_ptr->code_key = 0;
//   } /* Endif */

   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hid_init, SUCCESSFUL");
   #endif
   
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hid_cntrl_callback
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     This is the callback used when HID information is sent or received
*
*END*--------------------------------------------------------------------*/

static void usb_class_hid_cntrl_callback
   (
      /* [IN] Unused */
      void       *pipe,
      /* [IN] Pointer to the class interface instance */
      void       *param,
      /* [IN] Data buffer */
      unsigned char   *buffer,
      /* [IN] Length of buffer */
      uint32_t     len,
      /* [IN] Error code (if any) */
      USB_STATUS  status
   )
{ /* Body */
   USB_HID_CLASS_INTF_STRUCT_PTR      if_ptr;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hid_cntrl_callback");
   #endif

   /*
   ** There is need for USB_lock in the callback function, and there is also no
   ** need to check if the device is still attached (otherwise this callback
   ** would never have been called!
   */

   /* Get class interface handle, reset IN_SETUP and call callback */
   if_ptr = (USB_HID_CLASS_INTF_STRUCT_PTR)param;

   if_ptr->IN_SETUP = FALSE;

   if (if_ptr->USER_CALLBACK) {
      if_ptr->USER_CALLBACK(pipe, if_ptr->USER_PARAM, buffer, len, status);
   } /* Endif */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hid_cntrl_callback, SUCCESSFUL");
   #endif
   
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hid_cntrl_common
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     This function is used to send a control request
*
*END*--------------------------------------------------------------------*/

static USB_STATUS usb_class_hid_cntrl_common
   (
      /* [IN] The communication device common command structure */
      HID_COMMAND_PTR         com_ptr,
      /* [IN] Bitmask of the request type */
      uint8_t                  bmrequesttype,
      /* [IN] Request code */
      uint8_t                  brequest,
      /* [IN] Value to copy into WVALUE field of the REQUEST */
      uint16_t                 wvalue,
      /* [IN] Length of the data associated with REQUEST */
      uint16_t                 wlength,
      /* [IN] Pointer to data buffer used to send/recv */
      unsigned char               *data
   )
{ /* Body */
   USB_HID_CLASS_INTF_STRUCT_PTR    if_ptr;
   USB_SETUP                        req;
   USB_STATUS                       status = USBERR_NO_INTERFACE;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hid_cntrl_common");
   #endif
   
   USB_lock();
   /* Validity checking */
   if (usb_host_class_intf_validate(com_ptr->CLASS_PTR)) {
      if_ptr =
         (USB_HID_CLASS_INTF_STRUCT_PTR)com_ptr->CLASS_PTR->class_intf_handle;
      status = usb_hostdev_validate(if_ptr->G.dev_handle);
   } /* Endif */

   if (!status && if_ptr->IN_SETUP) {
      status = USBERR_TRANSFER_IN_PROGRESS;
   } /* Endif */

   if (!status) {
      /* Save the higher level callback and ID */
      if_ptr->USER_CALLBACK = com_ptr->CALLBACK_FN;
      if_ptr->USER_PARAM = com_ptr->CALLBACK_PARAM;

      /* Setup the request */
      req.BMREQUESTTYPE = bmrequesttype;
      req.BREQUEST = brequest;
      *(uint16_t*)req.WVALUE = HOST_TO_LE_SHORT(wvalue);
      *(uint16_t*)req.WINDEX = HOST_TO_LE_SHORT(((INTERFACE_DESCRIPTOR_PTR)if_ptr->G.intf_handle)->bInterfaceNumber);
      *(uint16_t*)req.WLENGTH = HOST_TO_LE_SHORT(wlength);
      status = _usb_hostdev_cntrl_request(if_ptr->G.dev_handle, &req, data,
         usb_class_hid_cntrl_callback, if_ptr);
      if (status == USB_STATUS_TRANSFER_QUEUED) {
         if_ptr->IN_SETUP = TRUE;
      } /* Endif */
   } /* Endif */
   USB_unlock();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hid_cntrl_common, SUCCESSFUL");
   #endif
   return status;
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hid_get_report
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     This function is called by the application to get a report from the HID
*     device
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hid_get_report
   (
      /* [IN] Class Interface structure pointer */
      HID_COMMAND_PTR            com_ptr,

      /* [IN] Report ID (see HID specification) */
      uint8_t                     rid,

      /* [IN] Report type (see HID specification) */
      uint8_t                     rtype,

      /* [IN] Buffer to receive report data */
      void                      *buf,

      /* [IN] length of the Buffer */
      uint16_t                    blen
   )
{ /* Body */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hid_get_report, SUCCESSFUL");
   #endif

   return usb_class_hid_cntrl_common(com_ptr,
      REQ_TYPE_IN | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE,
      GET_REPORT, (uint16_t)((rtype << 8) | rid), blen, buf);

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hid_set_report
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to send a report to the HID
*     device
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hid_set_report
   (
      /* [IN] Class Interface structure pointer */
      HID_COMMAND_PTR            com_ptr,

      /* [IN] Report ID (see HID specification) */
      uint8_t                     rid,

      /* [IN] Report type (see HID specification) */
      uint8_t                     rtype,

      /* [IN] Buffer to receive report data */
      void                      *buf,

      /* [IN] length of the Buffer */
      uint16_t                    blen
   )
{ /* Body */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hid_set_report, SUCCESSFUL");
   #endif

   return usb_class_hid_cntrl_common(com_ptr,
      REQ_TYPE_OUT | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE,
      SET_REPORT, (uint16_t)((rtype << 8) | rid), blen, buf);

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hid_get_idle
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     This function is called by the application to read the idle rate of a
*     particular HID device report
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hid_get_idle
   (
      /* [IN] Class Interface structure pointer */
      HID_COMMAND_PTR            com_ptr,

      /* [IN] Report ID (see HID specification) */
      uint8_t                     rid,

      /* [OUT] idle rate of this report */
      uint8_t                 *idle_rate
   )
{ /* Body */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hid_set_report, SUCCESSFUL");
   #endif

   return usb_class_hid_cntrl_common(com_ptr,
      REQ_TYPE_IN | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE,
      GET_IDLE, rid, 1, idle_rate);

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hid_set_idle
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     This function is called by the application to silence a particular report
*     on interrupt In pipe until a new event occurs or specified  time elapses
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hid_set_idle
   (
      /* [IN] Class Interface structure pointer */
      HID_COMMAND_PTR            com_ptr,

      /* [IN] Report ID (see HID specification) */
      uint8_t   rid,

      /*[IN] Idle rate */
      uint8_t   duration
   )
{ /* Body */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hid_set_idle, SUCCESSFUL");
   #endif

   return usb_class_hid_cntrl_common(com_ptr,
      REQ_TYPE_OUT | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE,
      SET_IDLE, (uint16_t)((duration << 8) | rid), 0, NULL);

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hid_get_protocol
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     Reads the active protocol (boot protocol or report protocol)
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hid_get_protocol
   (
      /* [IN] Class Interface structure pointer */
      HID_COMMAND_PTR            com_ptr,

      /* [IN] Protocol (1 byte, 0 = Boot Protocol or 1 = Report Protocol */
      unsigned char                  *protocol
   )
{ /* Body */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hid_get_protocol, SUCCESSFUL");
   #endif

   return usb_class_hid_cntrl_common(com_ptr,
      REQ_TYPE_IN | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE,
      GET_PROTOCOL, 0, 1, protocol);

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hid_set_protocol
* Returned Value : USB_OK if command has been passed on USB
* Comments       :
*     Switches between the boot protocol and the report protocol (or vice versa)
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hid_set_protocol
   (
      /* [IN] Class Interface structure pointer */
      HID_COMMAND_PTR            com_ptr,

      /* [IN] The protocol (0 = Boot, 1 = Report) */
      uint8_t                     protocol
   )
{ /* Body */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_hid_set_protocol, SUCCESSFUL");
   #endif

   return usb_class_hid_cntrl_common(com_ptr,
      REQ_TYPE_OUT | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE,
      SET_PROTOCOL, protocol, 0, NULL);

} /* Endbody */


/* EOF */

