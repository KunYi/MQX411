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
*  This file contains USB device API specific function to cancel 
* transfer.
*                                                               
*
*END************************************************************************/
#include <mqx.h>

#if !MQX_USE_IO_OLD
#include <stdio.h>
#endif


#include "usb.h"
#include "usb_prv.h"

#include "dev_cnfg.h"
#include "devapi.h"
#include "dev_main.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_cancel_transfer
*  Returned Value : USB_OK or error code
*  Comments       :
*        returns the status of the transaction on the specified endpoint.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_device_cancel_transfer
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint8_t                     ep_num,
            
      /* [IN] direction */
      uint8_t                     direction
   )
{ 
    uint8_t                        error = USB_OK;
    USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;
    
    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    
    USB_lock();
    
    /* Cancel transfer on the specified endpoint for the specified 
    ** direction 
    */
    if (((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
      usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_CANCEL_TRANSFER != NULL)   
    {
        error = ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
            usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_CANCEL_TRANSFER(handle, 
            ep_num, direction);
    }
    else
    {
        #if _DEBUG
            printf("_usb_device_cancel_transfer: DEV_CANCEL_TRANSFER is NULL\n");               
        #endif  
        return USBERR_ERROR;
    }

    USB_unlock();

    return error;
}

/* EOF */
