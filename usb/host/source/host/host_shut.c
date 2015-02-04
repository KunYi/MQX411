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
*   This file contains the USB Host API specific function to shutdown
*   the host
*
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_main.h"
#include "host_shut.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_shutdown
*  Returned Value : USB_OK or error code
*  Comments       :
*        Shutdown an initialized USB Host
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_host_shutdown
   (
      /* [IN] the USB_host handle */
      _usb_host_handle         handle
   )
{ /* Body */
   USB_STATUS                 status;
   DEV_INSTANCE_PTR           dev_instance_ptr;
   USB_HOST_IF_STRUCT_PTR     usb_c;
   USB_HOST_STATE_STRUCT_PTR  usb_host_ptr;
   _mqx_uint                  i;

   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_shutdown");
   #endif

   USB_lock();

   usb_c = _mqx_get_io_component_handle(IO_USB_COMPONENT);

   /* Check if the interface is registered */
   if (!usb_c) {
      /* No interface was registered yet */
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_shutdown no interface registered");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
   }
   else {
      /* Find out if the interface has been initialized */
      for (i = 0; i < USBCFG_MAX_DRIVERS; i++) {
         if (usb_c[i].HOST_HANDLE == handle) {
            /* Found installed device */
            break;
         }
      }
   }

   if (i == USBCFG_MAX_DRIVERS) {
      /* The interface has not been registered, so it cannot be initialized */
      USB_unlock();
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_shutdown not installed");
      #endif
      return USB_log_error(__FILE__,__LINE__,USBERR_DEVICE_NOT_FOUND);
   }

   while (usb_host_ptr->DEVICE_LIST_PTR != NULL)
   {
      dev_instance_ptr = (DEV_INSTANCE_PTR)usb_host_ptr->DEVICE_LIST_PTR;
      usb_dev_list_detach_device(usb_host_ptr, dev_instance_ptr->hub_no, dev_instance_ptr->port_no);
   }


   status = _usb_host_shutdown_call_interface(usb_host_ptr);

   //status = _bsp_usb_host_deinit(&usb_c[i]); //TODO: some physical deinitialization could be made

   usb_c[i].HOST_HANDLE = NULL;

   USB_unlock();

#if (USBCFG_HOST_NUM_ISO_PACKET_DESCRIPTORS != 0)
   _usb_host_iso_packet_desc_pool_destroy ();
#endif
   USB_mem_free(usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR);
   USB_mem_free(usb_host_ptr);

   if (status != USB_OK)
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("_usb_host_shutdown FAILED");
      #endif
      return status;
   }
   USB_mem_free((void *)usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR);
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_shutdown SUCCESSFUL");
   #endif

   return USB_OK;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_bus_control
*  Returned Value : None
*  Comments       :
* _usb_host_bus_control controls the bus operations such as asserting and
* deasserting the bus reset, asserting and deasserting resume,
* suspending and resuming the SOF generation
*
*END*-----------------------------------------------------------------*/

void _usb_host_bus_control
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle           handle,

      /* The operation to be performed on the bus */
      uint8_t                     bcontrol
   )
{ /* Body */

   USB_STATUS status;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_bus_control");
   #endif

   status = _usb_host_bus_control_data_call_interface (handle, bcontrol);

   if (status != USB_OK)
   {
      #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_bus_control FAILED");
      #endif
   }

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("_usb_host_bus_control SUCCESSFUL");
   #endif

} /* Endbody */

/* EOF */
