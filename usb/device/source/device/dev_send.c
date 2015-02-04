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
*  This file contains USB device API specific function to send data.
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
*  Function Name  : _usb_device_send_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Sends data on a specified endpoint.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_device_send_data
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint8_t                     ep_num,
            
      /* [IN] buffer to send */
      uint8_t                  *buff_ptr,
            
      /* [IN] length of the transfer */
      uint32_t                    size
   )
{ /* Body */
   USB_STATUS                       error;
   XD_STRUCT_PTR                    xd_ptr;
   volatile USB_DEV_STATE_STRUCT_PTR usb_dev_ptr;
   
    if (handle == NULL)
    {
        #if _DEBUG
            printf("_usb_device_send_data: handle is NULL\n");
        #endif  
        return USBERR_ERROR;
    }

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   
    #if PSP_HAS_DATA_CACHE
        /********************************************************
        If system has a data cache, it is assumed that buffer
        passed to this routine will be aligned on a cache line
        boundry. The following code will flush the
        buffer before passing it to hardware driver.   
        ********************************************************/
    if ((buff_ptr < (uint8_t *)__UNCACHED_DATA_START) || ((buff_ptr + size) > (uint8_t *)__UNCACHED_DATA_END))
        USB_dcache_flush_mlines((void *)buff_ptr, size);
    #endif
     
    USB_lock();

    if (!usb_dev_ptr->XD_ENTRIES) 
    {
        USB_unlock();
        return USB_STATUS_TRANSFER_IN_PROGRESS;
    }

    /* Get a transfer descriptor */
    USB_XD_QGET(usb_dev_ptr->XD_HEAD, usb_dev_ptr->XD_TAIL, xd_ptr);
    usb_dev_ptr->XD_ENTRIES--;

    /* Initialize the new transfer descriptor */      
    xd_ptr->EP_NUM = ep_num;
    xd_ptr->BDIRECTION = USB_SEND;
    xd_ptr->WTOTALLENGTH = size;
    xd_ptr->WSTARTADDRESS = buff_ptr;
    xd_ptr->WSOFAR = 0;
   
    xd_ptr->BSTATUS = USB_STATUS_TRANSFER_ACCEPTED;
    
    if (((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)\
        usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_SEND != NULL)
    {
        error = ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)\
            usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_SEND(handle, xd_ptr);    
    }
    else
    {
        #if _DEBUG
            printf("_usb_device_send_data: DEV_SEND is NULL\n");
        #endif  
        return USBERR_ERROR;
    }

    USB_unlock();
   
    if (error) 
    {
        #if _DEBUG
            printf("_usb_device_send_data, transfer failed\n");
        #endif  
        return USBERR_TX_FAILED;
    }
    return error;
} 

/* EOF */
