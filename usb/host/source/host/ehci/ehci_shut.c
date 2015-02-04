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
*   chip for shutting down the host controller.
*
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "hostapi.h"
#include "ehci_prv.h"

#include "ehci_shut.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_shutdown
*  Returned Value : None
*  Comments       :
*     Shutdown and de-initialize the VUSB1.1 hardware
*
*END*-----------------------------------------------------------------*/

USB_STATUS _usb_ehci_shutdown
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle        handle
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR    usb_host_ptr;
   VUSB20_REG_STRUCT_PTR             dev_ptr;
   ACTIVE_QH_MGMT_STRUCT_PTR         active_list_member_ptr, temp_ptr = NULL;
   
   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;
   
   dev_ptr      = (VUSB20_REG_STRUCT_PTR)usb_host_ptr->G.DEV_PTR;
   
   /* Disable interrupts */
   EHCI_REG_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_INTR,VUSB20_HOST_INTR_EN_BITS);
      
   /* Stop the controller */
   EHCI_REG_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD,EHCI_CMD_RUN_STOP);
   
   /* Reset the controller to get default values */
   EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD,EHCI_CMD_CTRL_RESET);
   
   /**********************************************************
   ensure that periodic list in uninitilized.
   **********************************************************/
   usb_host_ptr->ITD_LIST_INITIALIZED = FALSE;
   usb_host_ptr->SITD_LIST_INITIALIZED = FALSE;
   usb_host_ptr->PERIODIC_LIST_INITIALIZED = FALSE;
   usb_host_ptr->ALIGNED_PERIODIC_LIST_BASE_ADDR = NULL;
   
   /**********************************************************
   Free all memory occupied by active transfers   
   **********************************************************/
   active_list_member_ptr = usb_host_ptr->ACTIVE_ASYNC_LIST_PTR;
   while(active_list_member_ptr) 
   {
      temp_ptr =  active_list_member_ptr;
      active_list_member_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR)active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
      USB_mem_free(temp_ptr);
   }

   active_list_member_ptr = usb_host_ptr->ACTIVE_INTERRUPT_PERIODIC_LIST_PTR;
   while(active_list_member_ptr) 
   {
      temp_ptr =  active_list_member_ptr;
      active_list_member_ptr = (ACTIVE_QH_MGMT_STRUCT_PTR) \
                              active_list_member_ptr->NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
      USB_mem_free(temp_ptr);
      
   }

   /* Free all controller non-periodic specific memory */
   USB_mem_free((void *)usb_host_ptr->CONTROLLER_MEMORY);

   /* Free all controller periodic frame list bandwidth and other specific memory */
   USB_mem_free((void *)usb_host_ptr->PERIODIC_FRAME_LIST_BW_PTR);

   /* Free all controller periodic frame list entries memory */
   USB_mem_free((void *)usb_host_ptr->PERIODIC_LIST_BASE_ADDR);

#if 0      
   /* Free all controller specific memory, this is now merged into two sections */
   USB_memfree(__FILE__,__LINE__,(void *)usb_host_ptr->PERIODIC_FRAME_LIST_BW_PTR);
   USB_memfree(__FILE__,__LINE__,(void *)usb_host_ptr->ASYNC_LIST_BASE_ADDR);
   USB_memfree(__FILE__,__LINE__,(void *)usb_host_ptr->ITD_BASE_PTR);
   USB_memfree(__FILE__,__LINE__,(void *)usb_host_ptr->ITD_SCRATCH_STRUCT_BASE);
   USB_memfree(__FILE__,__LINE__,(void *)usb_host_ptr->SITD_BASE_PTR);
   USB_memfree(__FILE__,__LINE__,(void *)usb_host_ptr->QTD_BASE_PTR);
   USB_memfree(__FILE__,__LINE__,(void *)usb_host_ptr->QTD_SCRATCH_STRUCT_BASE);
   USB_memfree(__FILE__,__LINE__,(void *)usb_host_ptr->SITD_SCRATCH_STRUCT_BASE);
   USB_memfree(__FILE__,__LINE__,(void *)usb_host_ptr->PERIODIC_LIST_BASE_ADDR);
   USB_memfree(__FILE__,__LINE__,(void *)usb_host_ptr->QH_SCRATCH_STRUCT_BASE);
#endif

   return USB_OK;
   
} /* EndBody */

/* EOF */
