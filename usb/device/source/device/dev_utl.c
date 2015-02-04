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
*  This file contains USB device API specific utility functions.
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
*  Function Name  : _usb_device_unstall_endpoint
*  Returned Value : USB_OK or error code
*  Comments       :
*     Unstalls the endpoint in specified direction
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_device_unstall_endpoint
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint8_t                     ep_num,

      /* [IN] direction */
      uint8_t                     direction
   )
{ /* Body */
   USB_STATUS                        error = USB_OK;
   volatile USB_DEV_STATE_STRUCT_PTR usb_dev_ptr;

   if (handle  == NULL)
   {
      #if _DEBUG
         printf("_usb_device_unstall_endpoint: handle is NULL\n");
      #endif
      return USBERR_ERROR;
   }

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   USB_lock();


       if (((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)\
          usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_UNSTALL_ENDPOINT != NULL)
       {
          error= ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)\
          usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_UNSTALL_ENDPOINT(handle, ep_num, direction);
       }
       else
       {
          return USBERR_ERROR;
       }
   USB_unlock();
   return error;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_device_get_status
* Returned Value : USB_OK or error code
* Comments       :
*     Provides API to access the USB internal state.
*
*END*--------------------------------------------------------------------*/
USB_STATUS _usb_device_get_status
   (
      /* [IN] Handle to the USB device */
      _usb_device_handle   handle,

      /* [IN] What to get the error of */
      uint8_t               component,

      /* [OUT] The requested error */
      uint16_t          *error
   )
{ /* Body */
    volatile USB_DEV_STATE_STRUCT_PTR usb_dev_ptr;

    if((handle == NULL)||(error == NULL))
    {
        #if _DEBUG
            printf("_usb_device_get_status: NULL pointer\n");
        #endif
        return USBERR_ERROR;
    }
    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    USB_lock();
    switch (component)
    {
        case USB_STATUS_DEVICE_STATE:
            *error = usb_dev_ptr->USB_STATE;
            break;

        case USB_STATUS_DEVICE:
            *error = usb_dev_ptr->USB_DEVICE_STATE;
            break;

        case USB_STATUS_INTERFACE:
            break;

        case USB_STATUS_ADDRESS:
            *error = usb_dev_ptr->DEVICE_ADDRESS;
            break;

        case USB_STATUS_CURRENT_CONFIG:
            *error = usb_dev_ptr->USB_CURR_CONFIG;
            break;

        case USB_STATUS_SOF_COUNT:
            *error = usb_dev_ptr->USB_SOF_COUNT;
            break;

        default:
            if (component & USB_STATUS_ENDPOINT)
            {
                if (((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
                   usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_GET_ENDPOINT_STATUS != NULL)
                {
                    ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
                        usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_GET_ENDPOINT_STATUS(handle,
                        (uint8_t)(component & USB_STATUS_ENDPOINT_NUMBER_MASK),error);
                }
                else
                {
                    #if _DEBUG
                        printf("_usb_device_get_status: DEV_GET_ENDPOINT_STATUS is NULL\n");
                    #endif
                    USB_unlock();
                    return USBERR_ERROR;
                }
            }
            else
            {
                USB_unlock();
                return USBERR_BAD_STATUS;
            }
         break;
   } /* Endswitch */
   USB_unlock();
   return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_device_set_status
* Returned Value : USB_OK or error code
* Comments       :
*     Provides API to set internal state
*
*END*--------------------------------------------------------------------*/
USB_STATUS _usb_device_set_status
   (
      /* [IN] Handle to the usb device */
      _usb_device_handle   handle,

      /* [IN] What to set the error of */
      uint8_t               component,

      /* [IN] What to set the error to */
      uint16_t              setting
   )
{
    volatile USB_DEV_STATE_STRUCT_PTR usb_dev_ptr;
    uint8_t error = USB_OK;

    if (handle == NULL)
    {
        #if _DEBUG
            printf("_usb_device_set_status: handle is NULL\n");
        #endif
        return USBERR_ERROR;
    }
    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    USB_lock();

    switch (component)
    {
        case USB_STATUS_DEVICE_STATE:
            usb_dev_ptr->USB_STATE = setting;
            break;
        case USB_STATUS_DEVICE:
            usb_dev_ptr->USB_DEVICE_STATE = setting;
            break;
        case USB_STATUS_INTERFACE:
            break;
        case USB_STATUS_CURRENT_CONFIG:
            usb_dev_ptr->USB_CURR_CONFIG = setting;
            break;
        case USB_STATUS_SOF_COUNT:
            usb_dev_ptr->USB_SOF_COUNT = setting;
            break;
        case USB_STATUS_ADDRESS:
            usb_dev_ptr->DEVICE_ADDRESS = (uint8_t)setting;
            if (((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
                    usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_SET_ADDRESS != NULL)
            {
                error = ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
                    usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_SET_ADDRESS(
                    (void *)usb_dev_ptr, (uint8_t)(setting&0x00FF));
            }
            else
            {
                #if _DEBUG
                    printf("_usb_device_set_status: DEV_SET_ADDRESS is NULL\n");
                #endif
                error = USBERR_ERROR;
            }
            break;
        default:
            if (component & USB_STATUS_ENDPOINT)
            {
                if (((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
                    usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_SET_ENDPOINT_STATUS != NULL)
                {
                    error = ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
                        usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_SET_ENDPOINT_STATUS(
                        handle, component, setting);
                }
                else
                {
                    #if _DEBUG
                        printf("_usb_device_set_status: DEV_SET_ENDPOINT_STATUS is NULL\n");
                    #endif
                    error = USBERR_ERROR;
                }
            }
            else
            {
                USB_unlock();
                return USBERR_BAD_STATUS;
            } /* Endif */
        break;
   }/* Endswitch */

   USB_unlock();
   return error;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_stall_endpoint
*  Returned Value : USB_OK or error code
*  Comments       :
*     Stalls the endpoint.
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_device_stall_endpoint
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint8_t                     ep_num,

      /* [IN] direction */
      uint8_t                     direction
   )
{ /* Body */
   USB_STATUS                             error = 0;
   volatile USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

   if (handle == NULL)
   {
      #if _DEBUG
        printf("_usb_device_stall_endpoint: handle is NULL\n");
      #endif
      return USBERR_ERROR;
   }
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    if (((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
    usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_STALL_ENDPOINT
        != NULL)
    {
        error = ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
            usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_STALL_ENDPOINT(handle,
            ep_num, direction);
    }
    else
    {
        #if _DEBUG
            printf("_usb_device_stall_endpoint: DEV_STALL_ENDPOINT is NULL\n");
        #endif
        error = USBERR_ERROR;
    }

    return  error;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_process_resume
*  Returned Value : USB_OK or error code
*  Comments       :
*        Process Resume event
*
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_device_assert_resume
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle
   )
{ /* Body */
   volatile USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;
   USB_STATUS                             error = USB_OK;

   if (handle == NULL)
   {
      #if _DEBUG
        printf("_usb_device_assert_resume: handle is NULL\n");
      #endif
      return USBERR_ERROR;
   }
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    if (((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)
    usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_ASSERT_RESUME
        != NULL)
    {
        error= ((USB_DEV_CALLBACK_FUNCTIONS_STRUCT_PTR)\
            usb_dev_ptr->CALLBACK_STRUCT_PTR)->DEV_ASSERT_RESUME(handle);
    }
    else
    {
        #if _DEBUG
            printf("_usb_device_assert_resume: DEV_ASSERT_RESUME is NULL\n");
        #endif
        error = USBERR_ERROR;
    }

    return error;
}

/* EOF */
