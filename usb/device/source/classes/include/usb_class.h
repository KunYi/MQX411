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
* @brief The file contains USB stack class layer api header function.
* 
*END************************************************************************/
#ifndef _USB_CLASS_H
#define _USB_CLASS_H 1

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <mqx.h>
#include <bsp.h>

#include "usb.h"
#include "usb_prv.h"

#include "devapi.h"

/******************************************************************************
 * Global Variables
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
 /*#define DELAYED_PROCESSING  1 This define is used to delay the control 
                                processing and not have it executed as part
                                of the interrupt routine */

#define USB_MAX_EP_BUFFER_SIZE   (64)
#define SOF_HIGH_BYTE_SHIFT      (8)

#define GET_STATUS_DEVICE_MASK           (0x0003)  
#define REMOTE_WAKEUP_STATUS_MASK        (0x0002) 
#define BUS_POWERED                      (0x80)   
#define SELF_POWERED                     (0x40)  
#define SELF_POWER_BIT_SHIFT             (6)     


/* USB Specs define CONTROL_MAX_PACKET_SIZE for High Speed device as only 64,
   whereas for FS its allowed to be 8, 16, 32 or 64 */
#define CONTROL_MAX_PACKET_SIZE       (64)

#if (HIGH_SPEED_DEVICE && (CONTROL_MAX_PACKET_SIZE != 64))
#error "For High Speed CONTROL_MAX_PACKET_SIZE should be 64"
#endif

#define UNINITIALISED_VAL        (0xffffffff)  
/* Events to the Application */
#define USB_APP_BUS_RESET           (0)
#define USB_APP_CONFIG_CHANGED      (1)
#define USB_APP_ENUM_COMPLETE       (2)
#define USB_APP_SEND_COMPLETE       (3)
#define USB_APP_DATA_RECEIVED       (4) 
#define USB_APP_ERROR               (5)
#define USB_APP_GET_DATA_BUFF       (6)
#define USB_APP_EP_STALLED          (7)
#define USB_APP_EP_UNSTALLED        (8) 
#define USB_APP_GET_TRANSFER_SIZE   (9)

/* identification values and masks to identify request types  */
#define USB_REQUEST_CLASS_MASK   (0x60) 
#define USB_REQUEST_CLASS_STRD   (0x00) 
#define USB_REQUEST_CLASS_CLASS  (0x20) 
#define USB_REQUEST_CLASS_VENDOR (0x40) 
#define USB_MAX_CLASS_OBJECT     (0x1)
/******************************************************************************
 * Types
 *****************************************************************************/
typedef int32_t USB_CLASS_HANDLE;

/* callback function pointer structure for Application to handle events */
typedef void(_CODE_PTR_ USB_CLASS_CALLBACK)(uint8_t event, void* val, void *arg);

/* callback function pointer structure to handle USB framework request */
typedef uint8_t (_CODE_PTR_ USB_REQ_FUNC)(USB_SETUP_STRUCT *,
                                          uint8_t **,
                                          uint32_t*,void   *arg); 

/*callback function pointer structure for application to provide class params*/
typedef uint8_t (_CODE_PTR_ USB_CLASS_SPECIFIC_HANDLER_FUNC)(
                           uint8_t,
                           uint16_t, 
                           uint8_t **,
                           uint32_t*,
                           void   *arg);

/* Structure Representing class callback */
typedef struct usb_class_callback_struct
{
    USB_CLASS_CALLBACK  callback;
    void   *arg;
}USB_CLASS_CALLBACK_STRUCT, * USB_CLASS_CALLBACK_STRUCT_PTR ;

/* Structure other request class callback */
typedef struct usb_req_callback_struct
{
    USB_REQ_FUNC  callback;
    void   *arg;
}USB_REQ_CALLBACK_STRUCT, * USB_REQ_CALLBACK_STRUCT_PTR ;

typedef struct usb_class_specific_handler_callback_struct
{
    USB_CLASS_SPECIFIC_HANDLER_FUNC  callback;
    void   *arg;
}USB_CLASS_SPECIFIC_HANDLER_CALLBACK_STRUCT,
     *USB_CLASS_SPECIFIC_HANDLER_CALLBACK_STRUCT_PTR;

/* Strucutre holding USB class object state*/
typedef struct _usb_class_object
{
   uint32_t usb_fw_handle;
   _usb_device_handle controller_handle;
   void   *arg;
   USB_CLASS_CALLBACK class_callback;   
}USB_CLASS_OBJECT_STRUCT, * USB_CLASS_OBJECT_STRUCT_PTR;

 typedef struct _USB_LANGUAGE 
{
    uint16_t language_id;
    uint8_t ** lang_desc;
    uint8_t * lang_desc_size;    
} USB_LANGUAGE;
 
typedef struct _USB_ALL_LANGUAGES 
{
    uint8_t *languages_supported_string;
    uint8_t languages_supported_size;
    /*Allocate Memory In App Layer*/
    USB_LANGUAGE *usb_language;
       
}USB_ALL_LANGUAGES;

/* Strucutre Representing Endpoints and number of endpoints user want*/
typedef struct _USB_ENDPOINTS
{
    uint8_t count;
    /*Allocate Memory In App Layer*/
    USB_EP_STRUCT *ep; 
}USB_ENDPOINTS;

 /* Callback Functions to be implemented by APP.*/
 typedef struct _usb_desc_callbackFunction_struct
 {
    uint32_t handle;
    uint8_t (_CODE_PTR_ GET_DESC)(uint32_t handle,uint8_t type,uint8_t str_num,
        uint16_t index,uint8_t **descriptor,uint32_t *size);
    USB_ENDPOINTS * (_CODE_PTR_ GET_DESC_ENDPOINTS)(uint32_t handle);
    uint8_t (_CODE_PTR_ GET_DESC_INTERFACE)(uint32_t handle,uint8_t interface,
        uint8_t *alt_interface);
    uint8_t (_CODE_PTR_ SET_DESC_INTERFACE)(uint32_t handle,uint8_t interface,
        uint8_t alt_interface);
    bool (_CODE_PTR_ IS_DESC_VALID_CONFIGURATION)(uint32_t handle,
        uint16_t config_val);
    bool (_CODE_PTR_ DESC_REMOTE_WAKEUP)(uint32_t handle);
    uint8_t (_CODE_PTR_ DESC_SET_FEATURE)(uint32_t handle,int32_t cmd,
        uint8_t in_data,uint8_t **feature);
    uint8_t (_CODE_PTR_ DESC_GET_FEATURE)(uint32_t handle,int32_t cmd,
        uint8_t in_data,uint8_t ** feature);
 }DESC_CALLBACK_FUNCTIONS_STRUCT, * DESC_CALLBACK_FUNCTIONS_STRUCT_PTR;

/******************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Class_Init
 *
 * @brief The funtion initializes the Class Module 
 *
 * @param handle             :handle to Identify the controller
 * @param class_callback     :event callback      
 * @param other_req_callback :call back for class/vendor specific requests on 
 *                            CONTROL ENDPOINT
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
USB_CLASS_HANDLE USB_Class_Init
(
    _usb_device_handle handle, /* [IN] the USB device controller to initialize */                  
    USB_CLASS_CALLBACK class_callback,/*[IN]*/
    USB_REQ_FUNC       other_req_callback,/*[IN]*/
    void   *user_arg,/*[IN]*/
    DESC_CALLBACK_FUNCTIONS_STRUCT_PTR  desc_callback_ptr/*[IN]*/
); 

/**************************************************************************//*!
 *
 * @name  USB_Class_Deinit
 *
 * @brief The funtion initializes the Class Module 
 *
 * @param handle             :handle to Identify the controller
 * @param class_handle       :class handle      
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
uint8_t USB_Class_Deinit
(
    _usb_device_handle handle, /* [IN] the USB device controller to initialize */                  
    USB_CLASS_HANDLE  class_handle
 );
 /**************************************************************************//*!
 *
 * @name  USB_Class_Send_Data
 *
 * @brief The funtion calls the device to send data upon recieving an IN token 
 *
 * @param handle:               handle to Identify the controller
 * @param ep_num:               The endpoint number     
 * @param buff_ptr:             buffer to send
 * @param size:                 length of transfer
 * 
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
uint8_t USB_Class_Send_Data
(
    USB_CLASS_HANDLE handle, /*[IN]*/
    uint8_t ep_num,        /* [IN] the Endpoint number */                  
    uint8_t *buff_ptr,      /* [IN] buffer to send */      
    uint32_t size           /* [IN] length of the transfer */
); 

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Desc
 *
 * @brief  This function is called in to get the descriptor as specified in cmd.
 *
 * @param handle:           USB class handle. Received from
 *                          USB_Class_Init      
 * @param cmd:              command for USB discriptor to get.
 * @param in_data:          input to the Application functions.
 * @param out_buf           buffer which will contian the discriptors.
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint8_t USB_Class_Get_Desc(USB_CLASS_HANDLE handle,/*[IN]*/
int32_t cmd,/*[IN]*/
uint8_t input_data,/*[IN]*/
uint8_t **in_buf /*[OUT]*/
);

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Desc
 *
 * @brief  This function is called in to Set the descriptor as specified in cmd.
 *
 * @param handle:           USB class handle. Received from
 *                          USB_Class_Init      
 * @param cmd:              command for USB discriptor to get.
 * @param in_data:          input to the Application functions.
 * @param in_buf           buffer which will contian the discriptors.
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint8_t USB_Class_Set_Desc(USB_CLASS_HANDLE handle,/*[IN]*/
int32_t cmd,/*[IN]*/
uint8_t input_data,/*[IN]*/
uint8_t **out_buf /*[IN]*/
);

/**************************************************************************//*!
 *
 * @name   USB_Class_Periodic_Task
 *
 * @brief  The funtion calls for periodic tasks 
 *
 * @param  None
 *
 * @return None
 *
 *****************************************************************************/
extern void USB_Class_Periodic_Task(void);

#endif

/* EOF */
