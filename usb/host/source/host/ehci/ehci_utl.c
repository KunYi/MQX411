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
*   chip.
*
*
*END************************************************************************/
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "hostapi.h"
#include "ehci_prv.h"

#include "ehci_utl.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_bus_control
*  Returned Value : None
*  Comments       :
*        Bus control
*END*-----------------------------------------------------------------*/

USB_STATUS _usb_ehci_bus_control
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle        handle,

      /* The operation to be performed on the bus */
      uint8_t                  bControl
   )
{ /* Body */
   
   switch(bControl) {
      case USB_ASSERT_BUS_RESET:
         break;
      case USB_ASSERT_RESUME:
         break;
      case USB_SUSPEND_SOF:
         _usb_ehci_bus_suspend(handle);
         break;
      case USB_DEASSERT_BUS_RESET:
      case USB_RESUME_SOF:
      case USB_DEASSERT_RESUME:
      default:
         break;
   } /* EndSwitch */
   
   return USB_OK;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_bus_suspend
*  Returned Value : None
*  Comments       :
*        Suspend the bus
*END*-----------------------------------------------------------------*/

void _usb_ehci_bus_suspend
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle              handle
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR               usb_host_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   VUSB20_REG_STRUCT_PTR                        cap_dev_ptr;
   uint8_t                                       i, total_port_numbers;
   uint32_t                                      port_control;
   USB_EHCI_HOST_INIT_STRUCT_PTR                param;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;
   param = (USB_EHCI_HOST_INIT_STRUCT_PTR) usb_host_ptr->G.INIT_PARAM;

   cap_dev_ptr = (VUSB20_REG_STRUCT_PTR) param->CAP_BASE_PTR;
   dev_ptr = (VUSB20_REG_STRUCT_PTR) usb_host_ptr->G.DEV_PTR;
      
   total_port_numbers = 
   (uint8_t)((EHCI_REG_READ(cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCS_PARAMS) & 
         EHCI_HCS_PARAMS_N_PORTS));
         
   USB_lock();
      

   /* Suspend all ports */
   for (i=0;i<total_port_numbers;i++) {
      port_control = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i]);
      
      if (port_control & EHCI_PORTSCX_PORT_ENABLE) {
      
         port_control = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0]);
         port_control &= ~EHCI_PORTSCX_W1C_BITS;
         
         EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i],(port_control | EHCI_PORTSCX_PORT_SUSPEND));
      } /* Endif */
      
   } /* Endfor */
   
   /* Stop the controller 
   SGarg: This should not be done. If the controller is stopped, we will
   get no attach or detach interrupts and this will stop all operatings
   including the OTG state machine which is still running assuming that
   Host is alive.
   
   EHCI_REG_CLEAR_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD,EHCI_CMD_RUN_STOP);
   */   
      
   USB_unlock();
   
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_ehci_bus_resume
*  Returned Value : None
*  Comments       :
*        Resume the bus
*END*-----------------------------------------------------------------*/

void _usb_ehci_bus_resume
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle              handle,

      /* The pipe descriptor to queue */            
      EHCI_PIPE_STRUCT_PTR          pipe_descr_ptr
   )
{ /* Body */
   USB_EHCI_HOST_STATE_STRUCT_PTR               usb_host_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   VUSB20_REG_STRUCT_PTR                        cap_dev_ptr;
   uint8_t                                       i, total_port_numbers;
   uint32_t                                      port_control;
   USB_EHCI_HOST_INIT_STRUCT_PTR                param;

   usb_host_ptr = (USB_EHCI_HOST_STATE_STRUCT_PTR)handle;
   param = (USB_EHCI_HOST_INIT_STRUCT_PTR) usb_host_ptr->G.INIT_PARAM;

   cap_dev_ptr = (VUSB20_REG_STRUCT_PTR) param->CAP_BASE_PTR;
   dev_ptr = (VUSB20_REG_STRUCT_PTR) usb_host_ptr->G.DEV_PTR;
   
   total_port_numbers = (uint8_t)(EHCI_REG_READ(cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCS_PARAMS) & EHCI_HCS_PARAMS_N_PORTS);

   USB_lock();
      

   /* Resume all ports */
   for (i=0;i<total_port_numbers;i++) {
      port_control = EHCI_REG_READ(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i]);
      
      if (port_control & EHCI_PORTSCX_PORT_ENABLE) {
         port_control &= ~EHCI_PORTSCX_W1C_BITS;
         EHCI_REG_WRITE(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.PORTSCX[i],(port_control | EHCI_PORTSCX_PORT_FORCE_RESUME));
      } /* Endif */
      
   } /* Endfor */
   
   /* 
    S Garg: This should not be done. See comments in suspend.
      Restart the controller   
      EHCI_REG_SET_BITS(dev_ptr->REGISTERS.OPERATIONAL_HOST_REGISTERS.USB_CMD,EHCI_CMD_RUN_STOP);
     */

   USB_unlock();
   
} /* EndBody */

/* EOF */
