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
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
* @brief  The file emulates a speaker.
*
*
*END************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <mqx.h>
#include <lwevent.h>
#include <bsp.h>
#include <usb.h>
#include "audio_speaker.h"
#include "io_gpio.h"
#include "sgtl5000.h"

extern void Main_Task(uint32_t param);
extern void Play_Task(uint32_t param);

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { 10L, Main_Task, 2000L, 7L, "Main Task", MQX_AUTO_START_TASK, 0, 0},
   { 11L, Play_Task,  2000L, 3L, "Play Task", MQX_AUTO_START_TASK, 0, 0},
   { 0L, 0L, 0L, 0L, 0L, 0L, 0, 0}
};

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
 
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void TestApp_Init(void);

/****************************************************************************
 * Global Variables
 ****************************************************************************/              
extern USB_ENDPOINTS                   usb_desc_ep;
extern USB_AUDIO_UNITS                 usb_audio_unit;
extern DESC_CALLBACK_FUNCTIONS_STRUCT  desc_callback;
uint8_t *audio_data_buff0;
uint8_t *audio_data_buff1;
AUDIO_HANDLE   g_app_handle;
LWEVENT_STRUCT                         app_event;
static volatile uint8_t codecisready = 0;
static uint32_t datasize;
static uint8_t *audio_current_buff;
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
static void USB_App_Callback(uint8_t event_type, void* val,void *arg);
static void USB_Notif_Callback(uint8_t event_type,void* val,void *arg);

/*****************************************************************************
 * Local Variables 
 *****************************************************************************/
static bool start_app = FALSE;

/*****************************************************************************
 * Local Functions
 *****************************************************************************/

 /*****************************************************************************
 *  
 *   @name        TestApp_Init
 * 
 *   @brief       This function is the entry for speaker (or other usuage)
 * 
 *   @param       None
 * 
 *   @return      None
 **
 *****************************************************************************/

void TestApp_Init(void)
{
   AUDIO_CONFIG_STRUCT       audio_config;
   USB_CLASS_AUDIO_ENDPOINT  * endPoint_ptr;

   /* Pointer to audio endpoint entry */  
   endPoint_ptr = USB_mem_alloc_zero(sizeof(USB_CLASS_AUDIO_ENDPOINT)*AUDIO_DESC_ENDPOINT_COUNT);
   /* USB descriptor endpoint */
   audio_config.usb_ep_data = &usb_desc_ep;
   /* USB audio unit */
   audio_config.usb_ut_data = &usb_audio_unit;
   /* Endpoint count */
   audio_config.desc_endpoint_cnt = AUDIO_DESC_ENDPOINT_COUNT;
   /* Application callback */
   audio_config.audio_class_callback.callback = USB_App_Callback;
   /* Application callback argurment */
   audio_config.audio_class_callback.arg = &g_app_handle;
   /* Vendor callback */
   audio_config.vendor_req_callback.callback = NULL;
   /* Vendor callback argurment */
   audio_config.vendor_req_callback.arg = NULL;
   /* Param callback function */
   audio_config.param_callback.callback = USB_Notif_Callback;
   /* Param callback argurment */
   audio_config.param_callback.arg = &g_app_handle;
   /* Memory param callback */
   audio_config.mem_param_callback.callback = NULL;
   /* Memory param callback argurment */
   audio_config.mem_param_callback.arg = &g_app_handle;
   /* Descriptor callback pointer */
   audio_config.desc_callback_ptr =  &desc_callback;
   /* Audio enpoint pointer */
   audio_config.ep = endPoint_ptr;

   /* Initialize timer module */
//   audio_timer_init();

//   _audio_timer_init_freq(AUDIO_TIMER, AUDIO_TIMER_CHANNEL, AUDIO_SPEAKER_FREQUENCY, AUDIO_TIMER_CLOCK, TRUE);

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
   while(1)
   {
       if (MQX_OK != _lwevent_wait_ticks(&app_event, USB_APP_ENUM_COMPLETE_EVENT_MASK, FALSE, 0)) {
          printf("\n_lwevent_wait_ticks app_event failed.\n");
          _task_block();
       }

       if (MQX_OK != _lwevent_clear(&app_event, USB_APP_ENUM_COMPLETE_EVENT_MASK)) {
          printf("\n_lwevent_clear app_event failed.\n");
          _task_block();
       }

       /* Prepare buffer for first isochronous input */
        while (!codecisready)
            _time_delay(1);

        printf("Audio speaker is working ... \r\n");
        USB_Class_Audio_Recv_Data(g_app_handle,AUDIO_ISOCHRONOUS_ENDPOINT,
                  audio_data_buff0, datasize);

   }
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
static void USB_App_Callback(uint8_t event_type, void* val,void *arg) 
{
   UNUSED_ARGUMENT (arg)
   UNUSED_ARGUMENT (val)

   if(event_type == USB_APP_BUS_RESET) 
   {
      start_app=FALSE;
   }
   else if(event_type == USB_APP_ENUM_COMPLETE_EVENT_MASK) 
   {
      start_app = TRUE;
      if (MQX_OK !=_lwevent_set(&app_event, USB_APP_ENUM_COMPLETE_EVENT_MASK)){
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
static void USB_Notif_Callback(uint8_t event_type,void* val,void *arg) 
{
   uint32_t handle;
   APP_DATA_STRUCT* data_receive;
   static _mqx_uint read_data = 0;
   
   if(start_app == TRUE) 
   { 
      if(event_type == USB_APP_DATA_RECEIVED)
      {
         handle = *((uint32_t *)arg);
         data_receive = (APP_DATA_STRUCT*)val;
         read_data += data_receive->data_size;

         if (read_data >= datasize) {
            read_data = 0;
            if (MQX_OK !=_lwevent_set(&app_event, audio_current_buff == audio_data_buff0 ? USB_APP_BUFFER0_FULL_EVENT_MASK : USB_APP_BUFFER1_FULL_EVENT_MASK)) {
               //an error occured while setting the lwevent
            }
            /* switch to the other buffer */
            if (audio_current_buff == audio_data_buff0) {
               audio_current_buff = audio_data_buff1;
            }
            else {
               audio_current_buff = audio_data_buff0;
            }
         }
         /* request next data to the current buffer */
         USB_Class_Audio_Recv_Data(handle, AUDIO_ISOCHRONOUS_ENDPOINT,
            audio_current_buff + read_data, datasize - read_data);
      }
   }
}

/*Task*----------------------------------------------------------------
* 
* Task Name  : Main_Task
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
   _time_delay(200);
   _task_block();
}

/*Task*----------------------------------------------------------------
* 
* Task Name      : Play_Task
* Returned Value : None
* Comments       :
*     This task writes audio data buffer to I2S device.
* 
*END*--------------------------------------------------------------------*/
void Play_Task
(
   uint32_t param
)
{
   MQX_FILE_PTR              i2s_ptr = NULL;
   AUDIO_DATA_FORMAT         audio_format;
   _mqx_int                  errcode;
   _mqx_int                  freq;
   _mqx_int                  app_event_value;
#if BSPCFG_ENABLE_SAI
   I2S_STATISTICS_STRUCT stats;
#endif
   errcode = InitCodec();
   i2s_ptr = fopen(AUDIO_DEVICE, "w");
   if ((errcode != 0) || (i2s_ptr == NULL))
   {
      printf("Initializing audio codec...[FAIL]\n");
      printf("  Error 0x%X\n", errcode);
      fclose(i2s_ptr);
      _task_block();
   }
   
   /* Audio format in little endian */
   audio_format.ENDIAN = AUDIO_LITTLE_ENDIAN;
   /* Alignment of input audio data format */
   audio_format.ALIGNMENT = AUDIO_ALIGNMENT_LEFT;
   /* Audio format bits */
   audio_format.BITS = AUDIO_FORMAT_BITS;
   /* Audio format size */
   audio_format.SIZE = AUDIO_FORMAT_SIZE;
   /* Audio format channel number */
   audio_format.CHANNELS = AUDIO_FORMAT_CHANNELS;
   
   /* Setting audio format */
   ioctl(i2s_ptr, IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT, &audio_format);
   
   /* Setting over sampling clock frequency in Hz */
   freq = AUDIO_FORMAT_SAMPLE_RATE * AUDIO_I2S_FS_FREQ_DIV;
   ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &freq);
   
   /* Setting bit clock frequency in Hz */
   freq = AUDIO_FORMAT_SAMPLE_RATE;
   ioctl(i2s_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &freq);
   ioctl(i2s_ptr, IO_IOCTL_I2S_TX_DUMMY_OFF, NULL);
   
   /* Setting audio codec for SGTL5000 device */
   SetupCodec(i2s_ptr);
   datasize = 512;
#if BSPCFG_ENABLE_SAI
   ioctl(i2s_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats);
   datasize = stats.SIZE;
#endif
   codecisready = 1;
   if (NULL == (audio_data_buff0 = _mem_alloc(datasize)))
   {
         printf("\nMemory allocation for audio_data_buff0 failed.\n");
         _task_block();
   }
    if (NULL == (audio_data_buff1 = _mem_alloc(datasize)))
   {
        printf("\nMemory allocation for audio_data_buff1 failed.\n");
        _task_block();
   }
   audio_current_buff = audio_data_buff0;
   _time_delay(1);
#if BSPCFG_ENABLE_SAI
      ioctl(i2s_ptr, IO_IOCTL_I2S_START_TX, NULL);
#endif
   while (TRUE) 
   {
      if (MQX_OK != _lwevent_wait_ticks(&app_event, USB_APP_BUFFER0_FULL_EVENT_MASK | USB_APP_BUFFER1_FULL_EVENT_MASK, FALSE, 0)) {
         printf("\n_lwevent_wait_ticks app_event failed.\n");
         fclose(i2s_ptr);
         _task_block();
      }
      app_event_value = _lwevent_get_signalled();
#if BSPCFG_ENABLE_SAI
      ioctl(i2s_ptr, IO_IOCTL_I2S_WAIT_TX_EVENT, NULL);
#endif
      if (USB_APP_BUFFER0_FULL_EVENT_MASK & app_event_value) {
#if BSPCFG_ENABLE_SAI
          ioctl(i2s_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats);
           _mem_copy(audio_data_buff0,stats.IN_BUFFER,datasize);
           ioctl(i2s_ptr, IO_IOCTL_I2S_UPDATE_TX_STATUS, &datasize);
#else
           write(i2s_ptr, audio_data_buff0, datasize);
#endif
        }
        else if (USB_APP_BUFFER1_FULL_EVENT_MASK & app_event_value) {
#if BSPCFG_ENABLE_SAI
          ioctl(i2s_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats);
           _mem_copy(audio_data_buff1,stats.IN_BUFFER,datasize);
           ioctl(i2s_ptr, IO_IOCTL_I2S_UPDATE_TX_STATUS, &datasize);
#else
           write(i2s_ptr, audio_data_buff1, datasize);
#endif
        }
   }/* End while */
}

/* EOF */
