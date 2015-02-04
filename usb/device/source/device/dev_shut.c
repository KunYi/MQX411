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
*  This file contains USB device API specific function to shutdown 
*  the device.
*                                                               
*
*END************************************************************************/
#include <mqx.h>
#if !MQX_USE_IO_OLD
#include <stdio.h>
#endif

#include "usb.h"     //common public
#include "usb_prv.h" //common private

#include "dev_cnfg.h"
#include "devapi.h"  //device public
#include "dev_main.h" //device private

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_shutdown
*  Returned Value : USB_OK or error code
*  Comments       :
*        Shutdown an initialized USB device
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_device_shutdown
(
    /* [IN] the USB_USB_dev_initialize state structure */
    _usb_device_handle         handle
)
{ 
    USB_STATUS                        error;
    volatile USB_DEV_STATE_STRUCT_PTR usb_dev_ptr;
   
    if (handle == NULL)
    {
        #if _DEBUG
            printf("_usb_device_shutdown: handle is NULL\n");
        #endif  
        return USBERR_ERROR;
    }

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    if ( ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)\
        usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_SHUTDOWN != NULL)
    {
        error = ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)\
            usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_SHUTDOWN(usb_dev_ptr);
    }
    else
    {
        #if _DEBUG
            printf("_usb_device_shutdown: DEV_SHUTDOWN is NULL\n");
        #endif  
        return USBERR_ERROR;
    }
      
    return  error;   
} /* EndBody */
