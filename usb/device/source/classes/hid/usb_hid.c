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
* @brief The file contains USB stack HID layer implimentation.
* 
*END************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <mqx.h>
#include "usb_stack_config.h"

#if !MQX_USE_IO_OLD
#include <stdio.h>
#endif

#if HID_CONFIG

#include "usb_hid.h"

/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/


/****************************************************************************
 * Global Variables
 ****************************************************************************/
 HID_DEVICE_STRUCT    *device_array[MAX_HID_DEVICE];
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
uint8_t USB_HID_Other_Requests(USB_SETUP_STRUCT * setup_packet,
                          uint8_t **data,
                          uint32_t *size,void   *arg);
static uint8_t HID_USB_Map_Ep_To_Struct_Index(HID_DEVICE_STRUCT_PTR devicePtr,
                          uint8_t ep_num
                          );

/*****************************************************************************
 * Local Variables - None
 *****************************************************************************/


/*****************************************************************************
 * Local Functions
 *****************************************************************************/
 /*************************************************************************//*!
 *
 * @name  USB_Hid_Allocate_Handle
 *
 * @brief The function reserves entry in device array and returns the index.
 *
 * @param none.
 * @return returns the reserved handle or if no entry found device busy.
 *
 *****************************************************************************/
static HID_HANDLE  USB_Hid_Allocate_Handle()
{
    uint32_t cnt = 0;
    for (;cnt< MAX_HID_DEVICE;cnt++)
    {
       if (device_array[cnt] == NULL)
        return cnt;
    }
    return USBERR_DEVICE_BUSY;
}
 /*************************************************************************//*!
 *
 * @name  USB_Hid_Free_Handle
 *
 * @brief The function releases entry in device array .
 *
 * @param handle  index in device array to be released..
 * @return returns and error code or USB_OK.
 *
 *****************************************************************************/

static int32_t USB_Hid_Free_Handle(HID_HANDLE handle)
{
    if (/*(handle < 0) || */ (handle > MAX_HID_DEVICE))
        return USBERR_ERROR;

    USB_mem_free((void *)device_array[handle]);
    device_array[handle] = NULL;
    return USB_OK;
}
 /*************************************************************************//*!
 *
 * @name  USB_Hid_Get_Device_Ptr
 *
 * @brief The function gets the device pointer from device array .
 *
 * @param handle  index in device array.
 * @return returns returns pointer to HID device structure..
 *
 *****************************************************************************/
static HID_DEVICE_STRUCT_PTR USB_Hid_Get_Device_Ptr(HID_HANDLE handle)
{
     return device_array[handle];
}
 /*************************************************************************//*!
 *
 * @name  HID_USB_Map_Ep_To_Struct_Index
 *
 * @brief The function maps the endpoint num to the index of the ep data
 *           structure
 *
 * @param handle          handle to identify the controller
 * @param ep_num          endpoint num
 *
 * @return index          mapped index
 *
 *****************************************************************************/
static uint8_t HID_USB_Map_Ep_To_Struct_Index(HID_DEVICE_STRUCT_PTR devicePtr,
uint8_t ep_num
)
{
    uint8_t count = 0;

    USB_ENDPOINTS *ep_desc_data = devicePtr->ep_desc_data;
    /* map the endpoint num to the index of the endpoint structure */
    for(count = 0; count < ep_desc_data->count; count++)
    {
        if(ep_desc_data->ep[count].ep_num == ep_num)
            break;
    }

    return count;

}

/**************************************************************************//*!
 *
 * @name  USB_Service_Hid
 *
 * @brief The function ic callback function of HID endpoint
 *
 * @param event
 *
 * @return None
 *
 *****************************************************************************/
void USB_Service_Hid(PTR_USB_EVENT_STRUCT event,void *arg)
{
    uint8_t index;
    uint8_t producer, consumer;
    HID_DEVICE_STRUCT_PTR  devicePtr;

    devicePtr = (HID_DEVICE_STRUCT_PTR)arg;

     /* map the endpoint num to the index of the endpoint structure */
    index = HID_USB_Map_Ep_To_Struct_Index(devicePtr, event->ep_num);

    producer = devicePtr->hid_endpoint_data.ep[index].bin_producer;

    /* if there are no errors de-queue the queue and decrement the no. of
     transfers left, else send the same data again */
     /* de-queue if the send is complete with no error */
   devicePtr->hid_endpoint_data.ep[index].bin_consumer++;

    consumer = devicePtr->hid_endpoint_data.ep[index].bin_consumer;

    if(consumer != producer)
    {/*if bin is not empty */

        USB_CLASS_HID_QUEUE queue;
        /* send the next packet in queue */
        queue = devicePtr->hid_endpoint_data.ep[index].
                                         queue[consumer % HID_MAX_QUEUE_ELEMS];

        (void)USB_Class_Send_Data(devicePtr->class_handle, queue.channel,
                                      queue.app_buff, queue.size);
    }

    /* notify the app of the send complete */
    devicePtr->hid_class_callback.callback(USB_APP_SEND_COMPLETE, 0,
    devicePtr->hid_class_callback.arg);
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Hid_Event
 *
 * @brief The function initializes HID endpoint
 *
 * @param handle          handle to Identify the controller
 * @param event           pointer to event structure
 * @param val             gives the configuration value
 *
 * @return None
 *
 *****************************************************************************/
void USB_Class_Hid_Event(uint8_t event, void* val,void * arg)
{
    uint8_t index;

    USB_EP_STRUCT_PTR ep_struct_ptr;

    HID_DEVICE_STRUCT_PTR  devicePtr;

    devicePtr = (HID_DEVICE_STRUCT_PTR)arg;

    if(event == USB_APP_ENUM_COMPLETE)
    {
        uint8_t count = 0;
        uint8_t component;

        /* get the endpoints from the descriptor module */
        USB_ENDPOINTS *ep_desc_data = devicePtr->ep_desc_data;

        /* intialize all non control endpoints */
        while(count < ep_desc_data->count)
        {
            ep_struct_ptr= (USB_EP_STRUCT*) &ep_desc_data->ep[count];
            component = (uint8_t)(ep_struct_ptr->ep_num |
                (ep_struct_ptr->direction << COMPONENT_PREPARE_SHIFT));
            (void)_usb_device_init_endpoint(devicePtr->handle, ep_struct_ptr, TRUE);

            /* register callback service for endpoint 1 */
            (void)_usb_device_register_service(devicePtr->handle,
                (uint8_t)(USB_SERVICE_EP0+ep_struct_ptr->ep_num),
                USB_Service_Hid, arg);
            /* set the EndPoint Status as Idle in the device layer */
            /* (no need to specify direction for this case) */
            (void)_usb_device_set_status(devicePtr->handle,
                  (uint8_t)(USB_STATUS_ENDPOINT|component),
                  (uint16_t)USB_STATUS_IDLE);
            count++;
        }
    }
    else if(event == USB_APP_BUS_RESET)
    {
        /* clear producer and consumer */
        for(index = 0; index < devicePtr->hid_endpoint_data.count; index++)
        {
            devicePtr->hid_endpoint_data.ep[index].bin_consumer = 0x00;
            devicePtr->hid_endpoint_data.ep[index].bin_producer = 0x00;
        }
    }

    if(devicePtr->hid_class_callback.callback != NULL)
    {
        devicePtr->hid_class_callback.callback(event,val,
        devicePtr->hid_class_callback.arg);
    }
}

/**************************************************************************//*!
 *
 * @name  USB_HID_Other_Requests
 *
 * @brief The function provides flexibilty to add class and vendor specific
 *        requests
 *
 * @param handle:
 * @param setup_packet:     setup packet recieved
 * @param data:             data to be send back
 * @param size:             size to be returned
 *
 * @return status:
 *                        USB_OK : When Successfull
 *                        Others : When Error
 *
 *****************************************************************************/
uint8_t USB_HID_Other_Requests(USB_SETUP_STRUCT * setup_packet,
                          uint8_t **data,
                          uint32_t *size,
                          void   *arg)
{
    uint8_t index;
    uint8_t error = USBERR_INVALID_REQ_TYPE;
    HID_DEVICE_STRUCT_PTR  devicePtr;
    /* buffer to send in case of get report req */
    uint8_t rpt_buf[REPORT_SIZE];

    *((uint32_t *)rpt_buf) = 0;

    /*Get the device pointer to which this callback corresponds.*/
    devicePtr = (HID_DEVICE_STRUCT_PTR)arg;

    if((setup_packet->request_type & USB_REQUEST_CLASS_MASK) ==
                                                      USB_REQUEST_CLASS_CLASS)
    {
       /* class request so handle it here */
       /* index == 0 for get/set idle, index == 1 for get/set protocol */
        index = (uint8_t)((setup_packet->request - 2) & (USB_HID_REQUEST_TYPE_MASK));

        error = USB_OK;

        /* point to the data which comes after the setup packet */
        *data = ((uint8_t*)setup_packet) + USB_SETUP_PKT_SIZE;
        *size = 0;
        /* handle the class request */
        switch(setup_packet->request)
        {
            case USB_HID_GET_REPORT_REQUEST :
                /*set the data pointer to the buffer to send */
                *data = &rpt_buf[0];
                if(*size > REPORT_SIZE) *size = REPORT_SIZE;
                break;

            case USB_HID_SET_REPORT_REQUEST :
                for(index = 0; index < REPORT_SIZE; index++)
                {   /* copy the data sent by host in the buffer */
                    rpt_buf[index] = *(*data + index);
                }
                break;

            case USB_HID_GET_IDLE_REQUEST :
                /* point to the current idle rate */
                *data = &devicePtr->class_request_params[index];
                *size = CLASS_REQ_DATA_SIZE;
                break;

            case USB_HID_SET_IDLE_REQUEST :
                /* set the idle rate sent by the host */
                devicePtr->class_request_params[index] =
                (uint8_t)((setup_packet->value & MSB_MASK) >> HIGH_BYTE_SHIFT);
                break;

            case USB_HID_GET_PROTOCOL_REQUEST :
                /* point to the current protocol code
                   0 = Boot Protocol
                   1 = Report Protocol*/
                *data = &devicePtr->class_request_params[index];
                *size = CLASS_REQ_DATA_SIZE;
                break;

            case USB_HID_SET_PROTOCOL_REQUEST :
                /* set the protocol sent by the host
                  0 = Boot Protocol
                  1 = Report Protocol*/
                devicePtr->class_request_params[index] =
                (uint8_t)(setup_packet->value);
                break;
        }
        if(devicePtr->param_callback.callback != NULL)
        {
          /* handle callback if the application has supplied it */
          /* set the size of the transfer from the setup packet */
            *size = setup_packet->length;

          /* notify the application of the class request.*/
          /* give control to the application */
           error = devicePtr->param_callback.callback(
                                      /* request type */
                                      setup_packet->request,
                                      setup_packet->value,
                                      /* pointer to the data */
                                      data,
                                      /* size of the transfer */
                                      size,devicePtr->param_callback.arg);
        }
    }
    else if((setup_packet->request_type & USB_REQUEST_CLASS_MASK) ==
                                                     USB_REQUEST_CLASS_VENDOR)
    {   /* vendor specific request  */
        if(devicePtr->vendor_req_callback.callback != NULL)
        {
            error = devicePtr->vendor_req_callback.callback(setup_packet,
            data,size,devicePtr->vendor_req_callback.arg);
        }
    }

    return error;
}

/*****************************************************************************
 * Global Functions
 *****************************************************************************/


/**************************************************************************//*!
 *
 * @name  USB_Class_HID_Init
 *
 * @brief The function initializes the Device and Controller layer
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
HID_HANDLE USB_Class_HID_Init(
   HID_CONFIG_STRUCT_PTR hid_config_ptr
)
{
    uint8_t index;
    uint8_t error;
    HID_HANDLE           hidHandle;
    HID_DEVICE_STRUCT_PTR  devicePtr = NULL;

    if (NULL == hid_config_ptr)
    {
        return USBERR_ERROR;
    }


    devicePtr = (HID_DEVICE_STRUCT_PTR)USB_mem_alloc_zero(sizeof(HID_DEVICE_STRUCT));
    if (NULL == devicePtr)
    {
        #if _DEBUG
            printf("USB_Class_HID_Init: Memalloc failed\n");
        #endif
        return USBERR_ALLOC;
    }

    hidHandle = USB_Hid_Allocate_Handle();
    if (USBERR_DEVICE_BUSY == hidHandle)
    {
        USB_mem_free(devicePtr);
        devicePtr = NULL;
        return USBERR_INIT_FAILED;
    }

    /* Initialize the device layer*/
    error = _usb_device_init(USBCFG_DEFAULT_DEVICE_CONTROLLER, &(devicePtr->handle),
        (uint8_t)(hid_config_ptr->ep_desc_data->count+1));
    if(error != USB_OK)
    {
      goto error1;
    }

   hid_config_ptr->desc_callback_ptr->handle = hidHandle;
   /* Initialize the generic class functions */
   devicePtr->class_handle = USB_Class_Init(devicePtr->handle,
   USB_Class_Hid_Event,USB_HID_Other_Requests,(void *)devicePtr,
                             hid_config_ptr->desc_callback_ptr);
   if(error != USB_OK)
   {
      goto error2;
   }

   devicePtr->ep_desc_data =  hid_config_ptr->ep_desc_data;
   devicePtr->hid_endpoint_data.count = hid_config_ptr->ep_desc_data->count;
   devicePtr->hid_endpoint_data.ep = hid_config_ptr->ep;

   for(index = 0; index < hid_config_ptr->ep_desc_data->count; index++)
   {
      devicePtr->hid_endpoint_data.ep[index].endpoint =
                                 hid_config_ptr->ep_desc_data->ep[index].ep_num;
      devicePtr->hid_endpoint_data.ep[index].type =
                                  hid_config_ptr->ep_desc_data->ep[index].type;
      devicePtr->hid_endpoint_data.ep[index].bin_consumer = 0x00;
      devicePtr->hid_endpoint_data.ep[index].bin_producer = 0x00;
    }

  /* save the callback pointer */
  USB_mem_copy(&hid_config_ptr->hid_class_callback,
  &devicePtr->hid_class_callback,sizeof(USB_CLASS_CALLBACK_STRUCT));

  /* save the callback pointer */
  USB_mem_copy(&hid_config_ptr->vendor_req_callback,
  &devicePtr->vendor_req_callback,sizeof(USB_REQ_CALLBACK_STRUCT));

  /* Save the callback to ask application for class specific params*/
  USB_mem_copy(&hid_config_ptr->param_callback,
  &devicePtr->param_callback.callback ,sizeof(USB_REQ_CALLBACK_STRUCT));


  devicePtr->user_handle = hidHandle;
  device_array[hidHandle] = devicePtr;

  return USB_OK;

  error2:
     /* TBD:Implement _usb_device_deinit and call here*/
  error1:
     USB_Hid_Free_Handle(hidHandle);
     USB_mem_free(devicePtr);
     devicePtr = NULL;
  return error;
}

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
uint8_t USB_Class_HID_Send_Data(
                                HID_HANDLE handle,/*[IN]*/
                                uint8_t ep_num,/*[IN]*/
                                uint8_t *app_buff,/*[IN]*/
                                uint32_t size /*[IN]*/
                               )
{
    uint8_t index;
    uint8_t producer, consumer;
    uint8_t error = USB_OK;
    HID_DEVICE_STRUCT_PTR  devicePtr;

    if (/*(handle < 0) || */ (handle > MAX_HID_DEVICE))
        return USBERR_ERROR;

    devicePtr = USB_Hid_Get_Device_Ptr(handle);
    if (NULL == devicePtr)
    {
        return USBERR_NO_DEVICE_CLASS;
    }

     /* map the endpoint num to the index of the endpoint structure */
    index = HID_USB_Map_Ep_To_Struct_Index(devicePtr, ep_num);

    producer = devicePtr->hid_endpoint_data.ep[index].bin_producer;
    consumer = devicePtr->hid_endpoint_data.ep[index].bin_consumer;

    if((uint8_t)(producer - consumer) != (uint8_t)(HID_MAX_QUEUE_ELEMS))
    {/* the bin is not full*/

        uint8_t queue_num = (uint8_t)(producer % HID_MAX_QUEUE_ELEMS);
        /* put all send request parameters in the endpoint data structure */
        devicePtr->hid_endpoint_data.ep[index].queue[queue_num].channel = ep_num;
        devicePtr->hid_endpoint_data.ep[index].queue[queue_num].app_buff = app_buff;
        devicePtr->hid_endpoint_data.ep[index].queue[queue_num].size = size;
        devicePtr->hid_endpoint_data.ep[index].queue[queue_num].handle = devicePtr->handle;

        /* increment producer bin by 1*/
        devicePtr->hid_endpoint_data.ep[index].bin_producer++;
        producer++;

        if((uint8_t)(producer - consumer) == (uint8_t)1)
        {
            /*send the IO if there is only one element in the queue */
            error = USB_Class_Send_Data(devicePtr->class_handle, ep_num, app_buff,size);
        }
    }
    else /* bin is full */
    {
        error = USBERR_DEVICE_BUSY;
    }

    return error;
}
#endif /*HID_CONFIG*/
/* EOF */
