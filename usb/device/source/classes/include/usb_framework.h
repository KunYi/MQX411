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
* @brief The file contains USB Framework module api header function.
* 
*END************************************************************************/
#ifndef _USB_FRAMEWORK_H
#define _USB_FRAMEWORK_H 1

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_class.h"
/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define MAX_STRD_REQ            (13)  /* Max value of stadard request */
/* size of data to be returned for various Get Desc calls */
#define DEVICE_STATUS_SIZE      (2)
#define INTERFACE_STATUS_SIZE   (1)
#define CONFIG_SIZE             (1)
#define FRAME_SIZE              (2)
#define ENDP_STATUS_SIZE        (2)

#define DEVICE_REQUEST_MASK     (0x0003)
#define REPORT_DESCRIPTOR_TYPE  (0x22)
#define STRING_DESCRIPTOR_TYPE  (0x03)

/* masks and values for provides of Get Desc information */ 
#define USB_REQUEST_SRC_MASK      (0x03) 
#define USB_REQUEST_SRC_DEVICE    (0x00) 
#define USB_REQUEST_SRC_INTERFACE (0x01) 
#define USB_REQUEST_SRC_ENDPOINT  (0x02) 

#define  USB_SET_REQUEST_MASK     (0x02) 

/* for transfer direction check */
#define USB_DATA_TO_HOST          (0x80)
#define USB_DATA_TO_DEVICE        (0x00)
#define USB_DATA_DIREC_MASK       (0x80)
#define USB_MAX_CLASS_FW_OBJECT   (0x01)
 
/******************************************************************************
 * Types
 *****************************************************************************/
 typedef uint32_t USB_CLASS_FW_HANDLE;
 
/* Strucutre holding USB state information and handles.*/
typedef struct _usb_class_fw_object
{
   uint16_t std_framework_data;
   uint16_t assigned_address;
   _usb_device_handle controller_handle;
   USB_SETUP_STRUCT setup_packet;
   uint8_t* ext_req_to_host;
   USB_REQ_CALLBACK_STRUCT  other_req_callback;
   USB_CLASS_CALLBACK_STRUCT class_callback; 
   DESC_CALLBACK_FUNCTIONS_STRUCT desc;
   USB_CLASS_HANDLE  class_handle;  
}USB_CLASS_FW_OBJECT_STRUCT, * USB_CLASS_FW_OBJECT_STRUCT_PTR;

 /* callback function pointer structure to handle USB framework request */
typedef uint8_t (_CODE_PTR_ USB_FW_REQ_FUNC)(USB_CLASS_FW_OBJECT_STRUCT_PTR usb_fw_ptr,
                                            USB_SETUP_STRUCT * setup_packet, 
                                            uint8_t **data, 
                                            uint32_t *size); 

/******************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Framework_Init
 *
 * @brief The funtion initializes the Class Module 
 *
 * @param handle :              handle to Identify the controller
 * @param class_callback:       event callback 
 * @param other_req_callback:   callback for vendor or usb framework class req    
 *
 * @return status       
 *         Framework Handle           : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
USB_CLASS_FW_HANDLE USB_Framework_Init
(
    _usb_device_handle handle, /*[IN]*/
    USB_CLASS_CALLBACK class_callback,/*[IN]*/
    USB_REQ_FUNC other_req_callback,/*[IN]*/
    void   *callback_data,/*[IN]*/
    int32_t data,/*[IN]*/
    DESC_CALLBACK_FUNCTIONS_STRUCT_PTR  desc_callback_ptr /*[IN]*/
        
) ;
/**************************************************************************//*!
 *
 * @name  USB_Framework_Deinit
 *
 * @brief The funtion initializes the Class Module 
 *
 * @param handle :              handle to Identify the controller
 * @param fw_handle:       		framework handle
 * @return status       
 *         Framework Handle : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
uint8_t USB_Framework_Deinit
(
    _usb_device_handle handle, /*[IN]*/
    USB_CLASS_FW_HANDLE fw_handle /*[IN]*/
        
);
/**************************************************************************//*!
 *
 * @name  USB_Framework_Periodic_Task
 *
 * @brief The funtion is called to respond to any control request
 *
 * @param None
 *
 * @return None       
 *
 *****************************************************************************/
#ifdef DELAYED_PROCESSING
extern void USB_Framework_Periodic_Task(void);
#endif
/**************************************************************************//*!
 *
 * @name  USB_Framework_Reset
 *
 * @brief The funtion resets the framework 
 *
 * @param handle: handle to Identify the controller
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
extern uint8_t USB_Framework_Reset
(
    _usb_device_handle handle /*[IN] the USB device controller to initialize*/ 
);

/**************************************************************************//*!
 *
 * @name  USB_Framework_GetDesc
 *
 * @brief  This function is called in to get the discriptor as specified in cmd.
 *
 * @param handle:           USB framework handle. Received from
 *                          USB_Framework_Init      
 * @param cmd:              command for USB discriptor to get.
 * @param in_data:          input to the Application functions.
 * @param in_buff           buffer which will contian the discriptors.
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint8_t USB_Framework_GetDesc(USB_CLASS_FW_HANDLE handle, /*[IN]*/
int32_t cmd,/*[IN]*/
uint8_t in_data,/*[IN]*/
uint8_t ** in_buff/*[OUT]*/
);

/**************************************************************************//*!
 *
 * @name  USB_Framework_SetDesc
 *
 * @brief  This function is called in to get the discriptor as specified in cmd.
 *
 * @param handle:           USB framework handle. Received from
 *                          USB_Framework_Init      
 * @param cmd:              command for USB discriptor to get.
 * @param in_data:          input to the Application functions.
 * @param outBuf           buffer which will contian the discriptors.
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint8_t USB_Framework_SetDesc(USB_CLASS_FW_HANDLE handle,/*[IN]*/
int32_t cmd,/*[IN]*/
uint8_t input_data,/*[IN]*/
uint8_t ** outBuf/*[IN]*/
);
/**************************************************************************//*!
 *
 * @name  USB_Framework_Remote_wakeup
 *
 * @brief  This function is called in to get the discriptor as specified in cmd.
 *
 * @param handle:           USB framework handle. Received from
 *                          USB_Framework_Init      
 * @param cmd:              command for USB discriptor to get.
 * @param in_data:          input to the Application functions.
 * @param in_buff           buffer which will contian the discriptors.
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
bool USB_Framework_Remote_wakeup(USB_CLASS_FW_HANDLE handle/*[IN]*/);

#endif

/* EOF */
