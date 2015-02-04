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
* @brief The file contains USB stack HID class layer api header function.
* 
*END************************************************************************/
#ifndef _USB_HID_H
#define _USB_HID_H 1

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
#define HID_MAX_QUEUE_ELEMS  (4) 
 
 /* class specific requests */
#define USB_HID_GET_REPORT_REQUEST     (0x01)
#define USB_HID_GET_IDLE_REQUEST       (0x02)
#define USB_HID_GET_PROTOCOL_REQUEST   (0x03)
#define USB_HID_SET_REPORT_REQUEST     (0x09)
#define USB_HID_SET_IDLE_REQUEST       (0x0A)
#define USB_HID_SET_PROTOCOL_REQUEST   (0x0B)

/* for class specific requests */
#define HIGH_BYTE_SHIFT                   (8) 
#define MSB_MASK                          (0xFF00)
#define USB_HID_REQUEST_DIR_MASK          (0x08)
#define USB_HID_REQUEST_TYPE_MASK         (0x01)
#define REPORT_SIZE                       (4)
#define CLASS_REQ_DATA_SIZE               (0x01)
#define MAX_HID_DEVICE                    (0x05)

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
void USB_Service_Hid(PTR_USB_EVENT_STRUCT event,void *arg);
void USB_Class_Hid_Event(uint8_t event, void* val,void *arg) ;

 /******************************************************************************
 * Types
 *****************************************************************************/
 typedef uint32_t HID_HANDLE; 
 
 
 /* structure to hold a request in the endpoint queue */
typedef struct _usb_class_hid_queue 
{
    _usb_device_handle handle;
    uint8_t channel;       
    uint8_t *app_buff; /* buffer to send */
    uint32_t size; /* size of the transfer */  
}USB_CLASS_HID_QUEUE, *PTR_USB_CLASS_HID_QUEUE;
 
/* USB class hid endpoint data */
  
typedef struct _usb_class_hid_endpoint 
{
    uint8_t endpoint; /* endpoint num */                    
    uint8_t type;     /* type of endpoint (interrupt, bulk or isochronous) */   
    uint8_t bin_consumer;/* the num of queued elements */
    uint8_t bin_producer;/* the num of de-queued elements */
    USB_CLASS_HID_QUEUE queue[HID_MAX_QUEUE_ELEMS]; /* queue data */  
}USB_CLASS_HID_ENDPOINT;

/* contains the endpoint data for non control endpoints */
typedef struct _usb_class_hid_endpoint_data 
{    
    uint8_t count;  /* num of non control endpoints */  
    /* contains the endpoint info */
    /*Allocate Memory In App Layer*/    
    USB_CLASS_HID_ENDPOINT *ep;       
}USB_CLASS_HID_ENDPOINT_DATA, *PTR_USB_CLASS_HID_ENDPOINT_DATA;
 
 /* Strucutre holding HID class state information*/
 typedef struct hid_device_struct
 {
   _usb_device_handle handle;
   uint32_t user_handle;
   USB_CLASS_HANDLE class_handle;
   USB_ENDPOINTS *ep_desc_data;
   USB_CLASS_CALLBACK_STRUCT hid_class_callback;
   USB_REQ_CALLBACK_STRUCT       vendor_req_callback;
   USB_CLASS_SPECIFIC_HANDLER_CALLBACK_STRUCT param_callback;
   USB_CLASS_HID_ENDPOINT_DATA hid_endpoint_data;
   /* for get/set idle and protocol requests*/
   uint8_t class_request_params[2]; 
 }HID_DEVICE_STRUCT, * HID_DEVICE_STRUCT_PTR;
 
 /* Structure used to configure HID class by APP*/
 typedef struct hid_config_struct
 {
    uint32_t desc_endpoint_cnt;
    USB_ENDPOINTS *ep_desc_data;
    USB_CLASS_HID_ENDPOINT *ep; 
    USB_CLASS_CALLBACK_STRUCT hid_class_callback;
    USB_REQ_CALLBACK_STRUCT       vendor_req_callback;
    USB_CLASS_SPECIFIC_HANDLER_CALLBACK_STRUCT param_callback;
    DESC_CALLBACK_FUNCTIONS_STRUCT_PTR  desc_callback_ptr; 
 }HID_CONFIG_STRUCT, * HID_CONFIG_STRUCT_PTR;
/******************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Class_HID_Init
 *
 * @brief The funtion initializes the Device and Controller layer 
 *
 * @param *handle: handle pointer to Identify the controller
 * @param hid_class_callback:   event callback      
 * @param vendor_req_callback:  vendor specific class request callback      
 * @param param_callback:       application params callback      
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 *
 *This function initializes the HID Class layer and layers it is dependent on 
 *
 *****************************************************************************/
extern HID_HANDLE USB_Class_HID_Init (HID_CONFIG_STRUCT_PTR hid_config_ptr /*[IN]*/);

/**************************************************************************//*!
 *
 * @name  USB_Class_HID_Send_Data
 *
 * @brief 
 *
 * @param handle          :   handle returned by USB_Class_HID_Init
 * @param ep_num          :   endpoint num 
 * @param app_buff        :   buffer to send
 * @param size            :   length of the transfer   
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *****************************************************************************/
extern uint8_t USB_Class_HID_Send_Data
(
    HID_HANDLE         handle,        /* [IN]*/
    uint8_t             ep_num,        /* [IN]*/
    uint8_t         *buff_ptr,      /* [IN] buffer to send */      
    uint32_t            size           /* [IN] length of the transfer */
);

#define USB_HID_Periodic_Task USB_Class_Periodic_Task

#endif

/* EOF */
