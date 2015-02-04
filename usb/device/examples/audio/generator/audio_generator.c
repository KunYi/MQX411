/*HEADER**********************************************************************
*
* Copyright 2010 Freescale Semiconductor, Inc.
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
* @brief  The file emulates a generator.
*
*END************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <mqx.h>
#include <lwevent.h>

#include "audio_generator.h"

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
 #define MAIN_TASK       10
 
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void TestApp_Init(void);
void USB_Prepare_Data(void);
extern void Main_Task(uint32_t param);

/****************************************************************************
 * Global Variables
 ****************************************************************************/              
LWEVENT_STRUCT     app_event;
extern USB_ENDPOINTS usb_desc_ep;
extern USB_AUDIO_UNITS usb_audio_unit;
extern DESC_CALLBACK_FUNCTIONS_STRUCT  desc_callback;

extern const unsigned char wav_data[];
extern const uint16_t wav_size;

uint8_t wav_buff[8];
uint32_t audio_position = 0;

AUDIO_HANDLE   g_app_handle;
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK, Main_Task, 2*3000L, 7L, "Main", MQX_AUTO_START_TASK, 0, 0},
   { 0L, 0L, 0L, 0L, 0L, 0L, 0, 0}
};

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val,void *arg);
void USB_Notif_Callback(uint8_t event_type,void* val,void *arg);
uint8_t USB_App_Param_Callback(uint8_t request, uint16_t value, uint8_t **data, 
    uint32_t* size,void   *arg); 

/*****************************************************************************
 * Local Variables 
 *****************************************************************************/
volatile static bool start_app = FALSE;
/*****************************************************************************
 * Local Functions
 *****************************************************************************/

 /******************************************************************************
 *
 *   @name        USB_Prepare_Data
 *
 *   @brief       This function prepares data to send
 *
 *   @param       None
 *
 *   @return      None
 *****************************************************************************
 * This function prepare data before sending
 *****************************************************************************/
void USB_Prepare_Data(void)
{
   _mqx_uint k;
   /* copy data to buffer */
   for(k = 0; k < sizeof(wav_buff); k++, audio_position++)
   {
      if (audio_position > wav_size) {
          audio_position = 0;
      }
      wav_buff[k] = wav_data[audio_position];
   }
}

 /*****************************************************************************
 *  
 *   @name        TestApp_Init
 * 
 *   @brief       This function is the entry for Audio generator
 * 
 *   @param       None
 * 
 *   @return      None
 **                
 *****************************************************************************/
void TestApp_Init(void)
{
   AUDIO_CONFIG_STRUCT audio_config;
   USB_CLASS_AUDIO_ENDPOINT * endPoint_ptr;

   endPoint_ptr = USB_mem_alloc_zero(sizeof(USB_CLASS_AUDIO_ENDPOINT)*AUDIO_DESC_ENDPOINT_COUNT);
   audio_config.usb_ep_data = &usb_desc_ep;
   audio_config.usb_ut_data = &usb_audio_unit;
   audio_config.desc_endpoint_cnt = AUDIO_DESC_ENDPOINT_COUNT;
   audio_config.audio_class_callback.callback = USB_App_Callback;
   audio_config.audio_class_callback.arg = &g_app_handle;
   audio_config.vendor_req_callback.callback = NULL;
   audio_config.vendor_req_callback.arg = NULL;
   audio_config.param_callback.callback = USB_Notif_Callback;
   audio_config.mem_param_callback.callback = USB_App_Param_Callback;
   audio_config.mem_param_callback.arg = &g_app_handle;
   audio_config.param_callback.arg = &g_app_handle;
   audio_config.desc_callback_ptr =  &desc_callback;
   audio_config.ep = endPoint_ptr;

    if (MQX_OK != _usb_device_driver_install(USBCFG_DEFAULT_DEVICE_CONTROLLER)) {
        printf("Driver could not be installed\n");
        return;
    }

   /* Initialize the USB interface */
   g_app_handle = USB_Class_Audio_Init(&audio_config);

	if (MQX_OK != _lwevent_create(&app_event, LWEVENT_AUTO_CLEAR)) {
		printf("\n_lwevent_create app_event failed.\n");
		_task_block();
	}

   if (MQX_OK != _lwevent_wait_ticks(&app_event, USB_APP_ENUM_COMPLETE_EVENT_MASK, FALSE, 0)) {
		printf("\n_lwevent_wait_ticks app_event failed.\n");
		_task_block();
   }

   USB_Prepare_Data();
   USB_Class_Audio_Send_Data(g_app_handle, AUDIO_ENDPOINT, wav_buff, sizeof(wav_buff));
   
   _task_block();
} 

/******************************************************************************
 * 
 *    @name        USB_App_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       handle : handle to Identify the controller
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val,void *arg) 
{
   UNUSED_ARGUMENT (arg)
   UNUSED_ARGUMENT (val)

   if ((event_type == USB_APP_BUS_RESET) || (event_type == USB_APP_CONFIG_CHANGED))
   {
      start_app=FALSE;
   }
   else if(event_type == USB_APP_ENUM_COMPLETE) 
   {
      start_app=TRUE; 
      printf("Audio generator is working ... \r\n");       
	 if (MQX_OK !=_lwevent_set(&app_event, USB_APP_ENUM_COMPLETE_EVENT_MASK)) {
		printf("_lwevent_set app_event failed.\n");
	}
   }
   else if(event_type == USB_APP_ERROR)
   {
      /* add user code for error handling */
   }
   return;
}

/******************************************************************************
 * 
 *    @name        USB_App_Param_Callback
 *    
 *    @brief       This function handles the callback for Get/Set report req  
 *                  
 *    @param       request  :  request type
 *    @param       value    :  give report type and id
 *    @param       data     :  pointer to the data 
 *    @param       size     :  size of the transfer
 *
 *    @return      status
 *                  USB_OK  :  if successful
 *                              else return error
 *
 *****************************************************************************/
uint8_t endpoint_memory[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
uint8_t USB_App_Param_Callback
(
   uint8_t request, 
   uint16_t value, 
   uint8_t **data, 
   uint32_t* size,
   void   *arg
) 
{
   uint8_t error = USB_OK;
   uint8_t index;

   UNUSED_ARGUMENT(arg)
   /* handle the class request */
   switch(request) 
   {
      case GET_MEM :
         *data = &endpoint_memory[value]; /* point to the report to send */                    
         break;
      case SET_MEM :
         for(index = 0; index < *size ; index++) 
         {   /* copy the report sent by the host */          
            endpoint_memory[value + index] = *(*data + index);
         }
         *size = 0;
         break;
      default:
         break;
   }
   return error;
}
/******************************************************************************
 * 
 *    @name        USB_Notif_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       handle:  handle to Identify the controller
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
void USB_Notif_Callback(uint8_t event_type,void* val,void *arg) 
{
   if(start_app == TRUE) 
   {
      if(event_type == USB_APP_SEND_COMPLETE)
      {
         USB_Prepare_Data();
         USB_Class_Audio_Send_Data(g_app_handle, AUDIO_ENDPOINT, wav_buff, sizeof(wav_buff));
      }
   }
   return;
}
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : Main_Task
* Returned Value : None
* Comments       :
*     First function called.  Calls the Test_App
*     callback functions.
* 
*END*--------------------------------------------------------------------*/
void Main_Task
   (
      uint32_t param
   )
{   
    UNUSED_ARGUMENT (param)
    TestApp_Init();  

}
/* EOF */
