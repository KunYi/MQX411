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
*   This file is an example of device drivers for the Audio host class. This example
*   demonstrates the audio transfer capability of audio host class with audio devices.
*
*
*END************************************************************************/
#include "audio_speaker.h"
#include "hidkeyboard.h"
#include "sd_card.h"
#include <hwtimer.h>
/***************************************
**
** Macros
****************************************/
#define AUDIO_LWEVENT_TIMEOUT                    (1)

/***************************************
**
** Global functions
****************************************/

/***************************************
**
** Local functions
****************************************/
static USB_STATUS check_device_type (USB_AUDIO_CTRL_DESC_IT_PTR,USB_AUDIO_CTRL_DESC_OT_PTR,char **,char *);
static void USB_Prepare_Data(void);

/***************************************
**
** Global variables
****************************************/
char                                    device_direction = USB_AUDIO_DEVICE_DIRECTION_UNDEFINE;
uint8_t                                  wav_buff[MAX_ISO_PACKET_SIZE];
uint8_t                                  sd_buff_1[MAX_SD_READ]; /* sd card buffer */
uint8_t                                  sd_buff_2[MAX_SD_READ];
uint32_t                                 packet_size; /* number of bytes the host send to the device each mS */
uint8_t                                  resolution_size;
uint8_t                                  audio_state = AUDIO_IDLE;
bool                                 read_data_err = FALSE;
volatile AUDIO_CONTROL_DEVICE_STRUCT    audio_control = { 0 };
volatile AUDIO_CONTROL_DEVICE_STRUCT    audio_stream  = { 0 };
static uint8_t                           error_state = 0;
USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR   frm_type_desc = NULL;
USB_AUDIO_CTRL_DESC_FU_PTR              fu_desc = NULL;
AUDIO_COMMAND audio_com;
volatile static int16_t                  cur_volume, min_volume, max_volume, res_volume;
volatile static int16_t                  physic_volume;
volatile uint8_t                         host_cur_volume = 5;
volatile int16_t                         device_volume_step;
/* Following are buffers for USB, should be aligned at cache boundary */
volatile uint8_t *g_cur_mute;
volatile uint8_t *g_cur_vol;
volatile uint8_t *g_max_vol;
volatile uint8_t *g_min_vol;
volatile uint8_t *g_res_vol;

extern FILE_PTR                         file_ptr;
volatile uint32_t                        buffer_1_free = 1; /* buffer is free */
volatile uint32_t                        buffer_2_free = 1;
volatile uint8_t                         buffer_read = 1; /* buffer need to be read */
extern uint8_t                           file_open_count;
extern uint8_t                           sd_card_state;
extern volatile USB_KEYBOARD_DEVICE_STRUCT kbd_hid_device;
extern LWEVENT_STRUCT                   USB_Audio_FU_Request;
extern LWEVENT_STRUCT                   USB_Keyboard_Event;
extern LWEVENT_STRUCT                   SD_Card_Event;
HWTIMER                                 audio_timer;/* hwtimer handle */

LWEVENT_STRUCT                           USB_Audio_Event;
/***************************************
**
** Local variables
****************************************/
static ENDPOINT_DESCRIPTOR_PTR                 endp;
/* Transfer Types */
static char *TransferType[4] =
{
    "Control",
    "Isochronous",
    "Bulk",
    "Interrupt"
};
/* Sync Types */
static char *SyncType[4] =
{
    "No synchronization",
    "Asynchronous",
    "Adaptive",
    "Synchrounous"
};
/* Data Types */
static char *DataType[4] =
{
    "Data endpoint",
    "Feedback endpoint",
    "Implicit feedback",
    "Reserved"
};

/* Input Terminal types */
static char *it_type_string[NUMBER_OF_IT_TYPE] =
{
    "Input Undefined",
    "Microphone",
    "Desktop Microphone",
    "Personal Microphone",
    "Omni directional Microphone",
    "Microphone array",
    "Processing Microphone array",
};

/* Output Terminal types */
static char *ot_type_string[NUMBER_OF_OT_TYPE] =
{
    "Output Undefined",
    "Speaker",
    "Headphones",
    "Head Mounted Display Audio",
    "Desktop Speaker",
    "Room Speaker",
    "Communication Speaker",
    "Low Frequency Effects Speaker",
};
static char                         *device_string;

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : audio_timer_isr
* Comments  : Callback called by hwtimer every elapsed period
*
*END*----------------------------------------------------------------------*/
static void audio_timer_isr(void *p)
{
    if(device_direction == USB_AUDIO_DEVICE_DIRECTION_IN)
    {
        if (USB_DEVICE_INUSE == audio_stream.DEV_STATE)
        {
            /* Send data */
            usb_audio_send_data((CLASS_CALL_STRUCT_PTR)&audio_control.CLASS_INTF,
            (CLASS_CALL_STRUCT_PTR)&audio_stream.CLASS_INTF, usb_host_audio_tr_callback,
            NULL, packet_size, (unsigned char *)wav_buff);
        }
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : USB_Prepare_Data
* Returned Value : None
* Comments       :
*    This function prepares data to send.
*
*END*--------------------------------------------------------------------*/
static void USB_Prepare_Data(void)
{
    static uint32_t read_count=0;
    uint32_t i, prepare_data = 0;
    
    for(i=0;i<packet_size;i++)
    {
        if(1==buffer_read) /* read data from buffer 1 */
        {
            wav_buff[i]=sd_buff_1[read_count];
            read_count++;
            if (MAX_SD_READ==read_count)
            {
                read_count=0;
                buffer_1_free=1;
                buffer_read = 0;
                prepare_data = 1;
            }
        }
        else                 /* read data from buffer 2 */
        {
            wav_buff[i]=sd_buff_2[read_count];
            read_count++;
            if (MAX_SD_READ==read_count)
            {
                read_count=0;
                buffer_2_free=1;
                buffer_read = 1;
                prepare_data = 1;
            }
        }
    }

    if(1 == prepare_data)
    {
        if (_lwevent_set(&USB_Audio_Event,AUDIO_BUFF_EMPTY) != MQX_OK)
        {
            printf("_lwevent_set USB_Audio_Event failed.\n");
        }
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : check_device_type
* Returned Value : None
* Comments       :
*    This function check whether the attached device is out-device or in-device.
*
*END*--------------------------------------------------------------------*/
USB_STATUS check_device_type
(
/* [IN] Input terminal descriptor */
USB_AUDIO_CTRL_DESC_IT_PTR      it_desc,

/* [IN] Output terminal descriptor */
USB_AUDIO_CTRL_DESC_OT_PTR      ot_desc,

/* [OUT] Terminal type name */
char *                  *device_type,

/* [OUT] device direction */
char                        *direction
)
{
    unsigned char it_type_high, it_type_low, ot_type_low;

    it_type_high = it_desc->wTerminalType[0];
    it_type_low  = it_desc->wTerminalType[1];
    ot_type_low  = ot_desc->wTerminalType[1];

    /* Input terminal associates with audio streaming */
    if (USB_TERMINAL_TYPE == it_type_low)
    {
        *direction = USB_AUDIO_DEVICE_DIRECTION_IN;
    }
    /* Input terminal type */
    else if (INPUT_TERMINAL_TYPE == it_type_low)
    {
        /* get type device name */
        *device_type = it_type_string[it_type_high];
    }
    else
    {
        return USBERR_ERROR;
    }

    /* Output terminal associates with audio streaming */
    if (USB_TERMINAL_TYPE == ot_type_low)
    {
        if (USB_AUDIO_DEVICE_DIRECTION_IN == (*direction))
        {
            *direction = USB_AUDIO_DEVICE_DIRECTION_UNDEFINE;
            return USBERR_ERROR;
        }
        else
        {
            *direction = USB_AUDIO_DEVICE_DIRECTION_OUT;
        }
    }
    /* Output terminal type */
    else if (OUTPUT_TERMINAL_TYPE == ot_type_low)
    {
        /* get type device name */
        *device_type = ot_type_string[it_type_high];
    }
    else
    {
        return USBERR_ERROR;
    }
    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : USB_Audio_Get_Packet_Size
* Returned Value : None
* Comments       :
*     This function gets the packet size to send to the device each mS.
*
*END*--------------------------------------------------------------------*/
uint32_t USB_Audio_Get_Packet_Size
(
/* [IN] Point to format type descriptor */
USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR format_type_desc,
uint8_t                                bSamFreqType_index
)
{
    uint32_t packet_size_tmp;
    /* calculate packet size to send to the device each mS.*/
    /* packet_size = (sample frequency (Hz) /1000) * (bit resolution/8) * number of channels */
    packet_size_tmp = (((format_type_desc->tSamFreq[3*bSamFreqType_index + 2] << 16)|
    (format_type_desc->tSamFreq[3*bSamFreqType_index + 1] << 8) |
    (format_type_desc->tSamFreq[3*bSamFreqType_index + 0] << 0) )
    *(format_type_desc->bBitResolution / 8)
    *(format_type_desc->bNrChannels) / 1000);
    return (packet_size_tmp);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : Audio_Task
* Returned Value : none
* Comments       :
*       Execution starts here
*
*END*--------------------------------------------------------------------*/
void Audio_Task ( uint32_t param )
{ /* Body */
    FEATURE_CONTROL_STRUCT control_feature;
    uint8_t bSamFreqType_index;
    /* Initialization of hwtimer */
    if (MQX_OK != hwtimer_init(&audio_timer, &BSP_HWTIMER1_DEV, BSP_HWTIMER1_ID, (BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX + 1)))
    {
      printf("\n hwtimer initialization failed.\n");
      _task_block();
    }
    hwtimer_set_freq(&audio_timer, BSP_HWTIMER1_SOURCE_CLK, AUDIO_SPEAKER_FREQUENCY);
    hwtimer_callback_reg(&audio_timer,(HWTIMER_CALLBACK_FPTR)audio_timer_isr, NULL);
    if (NULL == (g_cur_mute = _mem_alloc(4)))
    {
      printf("\n memory allocation failed.\n");
      _task_block();
    }
    if (NULL == (g_cur_mute = _mem_alloc(4)))
    {
      printf("\n memory allocation failed.\n");
      _task_block();
    }
    if (NULL == (g_cur_vol = _mem_alloc(4)))
    {
      printf("\n memory allocation failed.\n");
      _task_block();
    }
    if (NULL == (g_max_vol = _mem_alloc(4)))
    {
      printf("\n memory allocation failed.\n");
      _task_block();
    }
    if (NULL == (g_min_vol = _mem_alloc(4)))
    {
      printf("\n memory allocation failed.\n");
      _task_block();
    }
    if (NULL == (g_res_vol = _mem_alloc(4)))
    {
      printf("\n memory allocation failed.\n");
      _task_block();
    }
    
    /* create lwevent group */
    if (_lwevent_create(&USB_Audio_Event,0) != MQX_OK)
    {
        printf("\n _lwevent_create USB_Audio_Event failed.\n");
        _task_block();
    }
   
    for(;;)
    {
    	/* Get information of audio interface */
        if (USB_DEVICE_INTERFACED == audio_stream.DEV_STATE)
        {
            audio_com.CLASS_PTR = (CLASS_CALL_STRUCT_PTR)&audio_control.CLASS_INTF;
            audio_com.CALLBACK_FN = usb_host_audio_request_ctrl_callback;
            audio_com.CALLBACK_PARAM = (void *)&control_feature;
            /* Get min volume */
            usb_class_audio_feature_command(&audio_com,1,(void *)g_min_vol,USB_AUDIO_GET_MIN_VOLUME);
            /* Get max volume */
            usb_class_audio_feature_command(&audio_com,1,(void *)g_max_vol,USB_AUDIO_GET_MAX_VOLUME);
            control_feature.FU = USB_AUDIO_CTRL_FU_VOLUME;
            /* Get res volume */
            if(USB_OK == usb_class_audio_feature_command(&audio_com,1,(void *)g_res_vol,USB_AUDIO_GET_RES_VOLUME))
                _lwevent_wait_ticks(&USB_Audio_FU_Request, FU_VOLUME_MASK, FALSE, (_time_get_ticks_per_sec() / 10) );
            else
                printf("The device does not support Get Resolution Volume command.\n");
            _time_delay(10);
            min_volume = (g_min_vol[1]<<8)|(g_min_vol[0]);
            max_volume = (g_max_vol[1]<<8)|(g_max_vol[0]);
            res_volume = (g_res_vol[1]<<8)|(g_res_vol[0]);
            _lwevent_clear(&USB_Audio_FU_Request, FU_VOLUME_MASK);
            control_feature.FU = 0;
            /* Synchronize host volume and device volume */
            device_volume_step = (int16_t)(((int16_t)(max_volume) - (int16_t)(min_volume))/(HOST_MAX_VOLUME - HOST_MIN_VOLUME));
            cur_volume = (int16_t)(min_volume + device_volume_step*host_cur_volume);
            /* Calculate physical volume(dB) */
            physic_volume = ((int16_t)(cur_volume)*39)/10000;
            g_cur_vol[0] = (uint8_t)((int16_t)(cur_volume)&0x00FF);
            g_cur_vol[1] = (uint8_t)((int16_t)(cur_volume)>>8);
            /* Unmute and set volume for audio device*/
            config_channel(fu_desc,frm_type_desc->bNrChannels);
            /* Audio device information */
            printf("Audio device information:\n");
            printf("   - Device type    : %s\n", device_string);
            for (bSamFreqType_index =0; bSamFreqType_index < frm_type_desc->bSamFreqType; bSamFreqType_index++)
            {
                printf("   - Frequency device support      : %d Hz\n", (frm_type_desc->tSamFreq[3*bSamFreqType_index + 2] << 16) |
                (frm_type_desc->tSamFreq[3*bSamFreqType_index + 1] << 8)  |
                (frm_type_desc->tSamFreq[3*bSamFreqType_index + 0] << 0));
            }
            printf("   - Bit resolution : %d bits\n", frm_type_desc->bBitResolution);
            printf("   - Number of channels : %d channels\n", frm_type_desc->bNrChannels);
            printf("   - Transfer type : %s\n", TransferType[(endp->bmAttributes)&EP_TYPE_MASK]);
            printf("   - Sync type : %s\n", SyncType[(endp->bmAttributes>>2)&EP_TYPE_MASK]);
            printf("   - Usage type : %s\n", DataType[(endp->bmAttributes>>4)&EP_TYPE_MASK]);
            if (device_direction == USB_AUDIO_DEVICE_DIRECTION_OUT)
            {
                printf("The device is unsupported!\n");
            }
            else
            {
                printf("This audio device supports play audio files with these properties:\n");
                printf("   - Sample rate    :\n");
                for (bSamFreqType_index =0; bSamFreqType_index < frm_type_desc->bSamFreqType; bSamFreqType_index++)
                {
                    printf("                    : %d Hz\n", (frm_type_desc->tSamFreq[3*bSamFreqType_index + 2] << 16) |
                    (frm_type_desc->tSamFreq[3*bSamFreqType_index + 1] << 8)  |
                    (frm_type_desc->tSamFreq[3*bSamFreqType_index] << 0));
                }
                printf("   - Sample size    : %d bits\n", frm_type_desc->bBitResolution);
                printf("   - Number of channels : %d channels\n", frm_type_desc->bNrChannels);
                printf("Type play command to play audio files:\n");
                for (bSamFreqType_index =0; bSamFreqType_index < frm_type_desc->bSamFreqType; bSamFreqType_index++)
                {
                    printf("Type play a:\\%dk_%dbit_%dch.wav to play the file\n",\
                    ((frm_type_desc->tSamFreq[3*bSamFreqType_index + 2] << 16) |
                    (frm_type_desc->tSamFreq[3*bSamFreqType_index + 1] << 8)  |
                    (frm_type_desc->tSamFreq[3*bSamFreqType_index + 0] << 0))/1000,\
                    frm_type_desc->bBitResolution,\
                    frm_type_desc->bNrChannels);
                }
                printf("\nCurrent physical volume: %d dB\n",physic_volume);
            }
            fflush(stdout);
            /* After get information of audio inteface, make HID interface is ready to use */
            if (kbd_hid_device.DEV_STATE != USB_DEVICE_INUSE)
            {
                kbd_hid_device.DEV_STATE = USB_DEVICE_INUSE;
                _lwevent_set(&USB_Keyboard_Event, USB_Keyboard_Event_CTRL);
            }
            audio_stream.DEV_STATE = USB_DEVICE_INUSE;
        }
        /* If an audio file is playing */
        if (AUDIO_PLAYING == audio_state)
        {
            /* Check if SD card disconnected */
            if ((SD_CARD_READY != sd_card_state) || read_data_err)
            {
                printf("  Error: Can't read audio file\n");
                hwtimer_stop(&audio_timer);
                audio_state = AUDIO_IDLE;
                fclose(file_ptr);
                if (MQX_OK !=_lwevent_set(&SD_Card_Event, SD_Card_Event_CLOSE)){
                    printf("_lwevent_set SD_Card_Event failed.\n");
                }
                file_open_count --;
                read_data_err = FALSE;
            }
            else if(!feof(file_ptr))
            {
            	/* Check if audio speaker disconnected */
                if (USB_DEVICE_INUSE != audio_stream.DEV_STATE)
                {
                    printf("  Error: Audio Speaker is disconnected\n");
                    hwtimer_stop(&audio_timer);
                    audio_state = AUDIO_IDLE;
                    fclose(file_ptr);
                    if (MQX_OK !=_lwevent_set(&SD_Card_Event, SD_Card_Event_CLOSE)){
                        printf("_lwevent_clear SD_Card_Event failed.\n");
                    }
                    file_open_count --;
                }
                if(1==buffer_1_free)
                {
                    /* write data to buffer 1 */
                    if (fread(sd_buff_1,1,MAX_SD_READ,file_ptr) < 0) read_data_err = TRUE;
                    buffer_1_free=0;
                }
                if(1==buffer_2_free)
                {
                    /* write data to buffer 2 */
                    if (fread(sd_buff_2,1,MAX_SD_READ,file_ptr) < 0) read_data_err = TRUE;
                    buffer_2_free=0;
                }
            }
            else
            {
                hwtimer_stop(&audio_timer);
                printf("\nFinished");
                audio_state = AUDIO_IDLE;
                fclose(file_ptr);
                file_open_count --;
            }
        }
        
        if(AUDIO_PLAYING == audio_state)
        {
            _lwevent_wait_ticks(&USB_Audio_Event,AUDIO_BUFF_EMPTY,FALSE,AUDIO_LWEVENT_TIMEOUT);
            _lwevent_clear(&USB_Audio_Event,AUDIO_BUFF_EMPTY);
        }
        else
        {
            _time_delay(1);
        }
    }
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_request_ctrl_callback
* Returned Value : None
* Comments       :
*     Called when a mute request is sent successfully.
*
*END*--------------------------------------------------------------------*/
void usb_host_audio_request_ctrl_callback
(
/* [IN] pointer to pipe */
_usb_pipe_handle  pipe_handle,

/* [IN] user-defined parameter */
void           *user_parm,

/* [IN] buffer address */
unsigned char         *buffer,

/* [IN] length of data transferred */
uint32_t           buflen,

/* [IN] status, hopefully USB_OK or USB_DONE */
uint32_t           status
)
{ /* Body */
    static FEATURE_CONTROL_STRUCT_PTR control_feature_ptr;
    control_feature_ptr = (FEATURE_CONTROL_STRUCT_PTR)user_parm;

    switch (control_feature_ptr->FU)
    {
    case USB_AUDIO_CTRL_FU_MUTE:
        {
            if (MQX_OK !=_lwevent_set(&USB_Audio_FU_Request, FU_MUTE_MASK)){
                printf("_lwevent_set USB_Audio_FU_Request failed.\n");
            }
            break;
        }
    case USB_AUDIO_CTRL_FU_VOLUME:
        {
            if (MQX_OK !=_lwevent_set(&USB_Audio_FU_Request, FU_VOLUME_MASK)){
                printf("_lwevent_set USB_Audio_FU_Request failed.\n");
            }
            break;
        }
    default:
        break;
    }
    fflush(stdout);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_tr_callback
* Returned Value : None
* Comments       :
*     Called when a ISO packet is sent/received successfully.
*
*END*--------------------------------------------------------------------*/
void usb_host_audio_tr_callback(
/* [IN] pointer to pipe */
_usb_pipe_handle pipe_handle,

/* [IN] user-defined parameter */
void *user_parm,

/* [IN] buffer address */
unsigned char *buffer,

/* [IN] length of data transferred */
uint32_t buflen,

/* [IN] status, hopefully USB_OK or USB_DONE */
uint32_t status
)
{
    if(USB_AUDIO_DEVICE_DIRECTION_IN == device_direction)
    {
        USB_Prepare_Data();
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_control_event
* Returned Value : None
* Comments       :
*     Called when control interface has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
void usb_host_audio_control_event
(
/* [IN] pointer to device instance */
_usb_device_instance_handle      dev_handle,

/* [IN] pointer to interface descriptor */
_usb_interface_descriptor_handle intf_handle,

/* [IN] code number for event causing callback */
uint32_t                          event_code
)
{
    INTERFACE_DESCRIPTOR_PTR   intf_ptr =
    (INTERFACE_DESCRIPTOR_PTR)intf_handle;

    switch (event_code) {
    case USB_CONFIG_EVENT:
        /* Drop through into attach, same processing */
    case USB_ATTACH_EVENT:
        {
            USB_AUDIO_CTRL_DESC_HEADER_PTR   header_desc = NULL;
            USB_AUDIO_CTRL_DESC_IT_PTR       it_desc  = NULL;
            USB_AUDIO_CTRL_DESC_OT_PTR       ot_desc  = NULL;

            if((audio_stream.DEV_STATE == USB_DEVICE_IDLE) || (audio_stream.DEV_STATE == USB_DEVICE_DETACHED))
            {
                audio_control.DEV_HANDLE  = dev_handle;
                audio_control.INTF_HANDLE = intf_handle;
                audio_control.DEV_STATE   = USB_DEVICE_ATTACHED;
            }
            else
            {
                printf("Audio device already attached\n");
                fflush(stdout);
            }

            /* finds all the descriptors in the configuration */
            if (USB_OK != usb_class_audio_control_get_descriptors(dev_handle,
                        intf_handle,
                        &header_desc,
                        &it_desc,
                        &ot_desc,
                        &fu_desc))
            {
                break;
            };

            /* initialize new interface members and select this interface */
            if (USB_OK != _usb_hostdev_select_interface(dev_handle,
                        intf_handle, (void *)&audio_control.CLASS_INTF))
            {
                break;
            }

            /* set all info got from descriptors to the class interface struct */
            usb_class_audio_control_set_descriptors((void *)&audio_control.CLASS_INTF,
            header_desc, it_desc, ot_desc, fu_desc);

            if(USB_OK != check_device_type(it_desc, ot_desc, &device_string, &device_direction))
            {
                error_state=1;
                break;
            }

            printf("----- Audio control interface: attach event -----\n");
            fflush(stdout);
            printf("State = attached");
            printf("  Class = %d", intf_ptr->bInterfaceClass);
            printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
            printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
            break;
        }
    case USB_INTF_EVENT:
        {
            USB_STATUS status;

            status = usb_class_audio_init_ipipe((CLASS_CALL_STRUCT_PTR)&audio_control.CLASS_INTF,
            NULL,NULL);

            if ((status != USB_OK) && (status != USBERR_OPEN_PIPE_FAILED))
            break;

            printf("----- Audio control interface: interface event -----\n");
            audio_control.DEV_STATE = USB_DEVICE_INTERFACED;
            break;
        }

    case USB_DETACH_EVENT:
        {
            AUDIO_CONTROL_INTERFACE_STRUCT_PTR if_ptr;

            if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR) audio_control.CLASS_INTF.class_intf_handle;

            _lwevent_destroy(&if_ptr->control_event);

            printf("----- Audio control interface: detach event -----\n");
            fflush(stdout);
            printf("State = detached");
            printf("  Class = %d", intf_ptr->bInterfaceClass);
            printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
            printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
            fflush(stdout);
            audio_control.DEV_HANDLE = NULL;
            audio_control.INTF_HANDLE = NULL;
            audio_control.DEV_STATE = USB_DEVICE_DETACHED;
            error_state = 0;
            device_direction = USB_AUDIO_DEVICE_DIRECTION_UNDEFINE;
            break;
        }
    default:
        printf("Audio Device: unknown control event\n");
        fflush(stdout);
        break;
    } /* EndSwitch */
    fflush(stdout);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_stream_event
* Returned Value : None
* Comments       :
*     Called when stream interface has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
void usb_host_audio_stream_event
(
/* [IN] pointer to device instance */
_usb_device_instance_handle      dev_handle,

/* [IN] pointer to interface descriptor */
_usb_interface_descriptor_handle intf_handle,

/* [IN] code number for event causing callback */
uint32_t                          event_code
)
{ /* Body */
    INTERFACE_DESCRIPTOR_PTR   intf_ptr =
    (INTERFACE_DESCRIPTOR_PTR)intf_handle;

    /* Check audio stream interface alternating 0 */
    if (intf_ptr->bAlternateSetting != 1)
    return;

    switch (event_code) {
    case USB_CONFIG_EVENT:
        /* Drop through into attach, same processing */
    case USB_ATTACH_EVENT:
        {
            USB_AUDIO_STREAM_DESC_SPEPIFIC_AS_IF_PTR     as_itf_desc = NULL;
            USB_AUDIO_STREAM_DESC_SPECIFIC_ISO_ENDP_PTR  iso_endp_spec_desc = NULL;

            if((audio_stream.DEV_STATE == USB_DEVICE_IDLE) || (audio_stream.DEV_STATE == USB_DEVICE_DETACHED))
            {
                audio_stream.DEV_HANDLE  = dev_handle;
                audio_stream.INTF_HANDLE = intf_handle;
                audio_stream.DEV_STATE   = USB_DEVICE_ATTACHED;
            }
            else
            {
                printf("Audio device already attached\n");
                fflush(stdout);
            }

            /* finds all the descriptors in the configuration */
            if (USB_OK != usb_class_audio_stream_get_descriptors(dev_handle,
                        intf_handle,
                        &as_itf_desc,
                        &frm_type_desc,
                        &iso_endp_spec_desc))
            {
                break;
            };

            /* initialize new interface members and select this interface */
            if (USB_OK != _usb_hostdev_select_interface(dev_handle,
                        intf_handle, (void *)&audio_stream.CLASS_INTF))
            {
                break;
            }

            /* set all info got from descriptors to the class interface struct */
            usb_class_audio_stream_set_descriptors((void *)&audio_stream.CLASS_INTF,
            as_itf_desc, frm_type_desc, iso_endp_spec_desc);

            printf("----- Audio stream interface: attach event -----\n");
            fflush(stdout);
            printf("State = attached");
            printf("  Class = %d", intf_ptr->bInterfaceClass);
            printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
            printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
            fflush(stdout);

            break;
        }
    case USB_INTF_EVENT:
        {
            if(0==error_state)
            {
                audio_stream.DEV_STATE = USB_DEVICE_INTERFACED;
                if (USB_OK != _usb_hostdev_get_descriptor(
                            dev_handle,
                            intf_handle,
                            USB_DESC_TYPE_EP,  /* Functional descriptor for interface */
                            1,                           /* Index of descriptor */
                            intf_ptr->bAlternateSetting, /* Index of interface alternate */
                            (void **)&endp))
                printf("Get end point descriptor error!");
                printf("----- Audio stream interface: interface event-----\n");
            }
            else
            {
                printf("The device is unsupported!\n");
                fflush(stdout);
            }
            break;
        }
    case USB_DETACH_EVENT:
        {
            audio_stream.DEV_HANDLE = NULL;
            audio_stream.INTF_HANDLE = NULL;
            audio_stream.DEV_STATE = USB_DEVICE_DETACHED;
            printf("----- Audio stream interface: detach event-----\n");
            fflush(stdout);
            hwtimer_stop(&audio_timer);
            break;
        }
    default:
        printf("Audio device: unknown data event\n");
        fflush(stdout);
        break;
    } /* EndSwitch */
} /* Endbody */

/******************************************************************************
*   @name        config_channel
*
*   @brief       Config volume for all channels of speaker
*
*   @param       None
*
*   @return      None
*
******************************************************************************
* This function is used to unmute and set volume of audio speaker
*****************************************************************************/
void config_channel(USB_AUDIO_CTRL_DESC_FU_PTR fu_ptr,uint8_t num_channel)
{
    uint8_t i;
    FEATURE_CONTROL_STRUCT control_feature;
    for (i = 0; i<=num_channel; i++)
    {
        audio_com.CLASS_PTR = (CLASS_CALL_STRUCT_PTR)&audio_control.CLASS_INTF;
        audio_com.CALLBACK_FN = usb_host_audio_request_ctrl_callback;
        control_feature.FU = USB_AUDIO_CTRL_FU_MUTE;
        audio_com.CALLBACK_PARAM = (void *)&control_feature;
        g_cur_mute[0] = FALSE;
        usb_class_audio_feature_command(&audio_com,i,(void *)g_cur_mute,USB_AUDIO_SET_CUR_MUTE);
        control_feature.FU = USB_AUDIO_CTRL_FU_VOLUME;
        audio_com.CALLBACK_PARAM = (void *)&control_feature;
        usb_class_audio_feature_command(&audio_com,i,(void *)g_cur_vol,USB_AUDIO_SET_CUR_VOLUME);
    }
}

/******************************************************************************
*   @name        audio_mute_command
*
*   @brief       Mute/unmute audio speaker
*
*   @param       None
*
*   @return      None
*
******************************************************************************
* This function is used to mute/unmute audio speaker
*****************************************************************************/
void audio_mute_command(void)
{
    FEATURE_CONTROL_STRUCT control_feature;
    if ((USB_DEVICE_INUSE != audio_stream.DEV_STATE)||(device_direction != USB_AUDIO_DEVICE_DIRECTION_IN))
    {
        printf("  Error: Audio Speaker is not connected\n");
        return;
    }
    g_cur_mute[0] = !g_cur_mute[0];
    if (g_cur_mute[0])
    {
        printf("Mute ...\n");
    }
    else
    {
        printf("UnMute ...\n");
    }
    /* Send set mute request */
    control_feature.FU = USB_AUDIO_CTRL_FU_MUTE;
    audio_com.CLASS_PTR = (CLASS_CALL_STRUCT_PTR)&audio_control.CLASS_INTF;
    audio_com.CALLBACK_FN = usb_host_audio_request_ctrl_callback;
    audio_com.CALLBACK_PARAM = &control_feature;
    usb_class_audio_feature_command(&audio_com,0,(void *)g_cur_mute,USB_AUDIO_SET_CUR_MUTE);
}

/******************************************************************************
*   @name        audio_increase_volume_command
*
*   @brief       Increase audio speaker
*
*   @param       None
*
*   @return      None
*
******************************************************************************
* This function is used to increase volume of audio speaker
*****************************************************************************/
void audio_increase_volume_command(void)
{
    FEATURE_CONTROL_STRUCT control_feature;
    static uint32_t i;
    uint8_t max_audio_channel;
    if ((USB_DEVICE_INUSE != audio_stream.DEV_STATE)||(device_direction != USB_AUDIO_DEVICE_DIRECTION_IN))
    {
        printf("  Error: Audio Speaker is not connected\n");
        return;
    }
    max_audio_channel = frm_type_desc->bNrChannels;
    /* Send set mute request */
    cur_volume = (g_cur_vol[1]<<8)|(g_cur_vol[0]);
    if((host_cur_volume + HOST_VOLUME_STEP) > HOST_MAX_VOLUME)
    {
        host_cur_volume = HOST_MAX_VOLUME;
        i = 0;
    }
    else
    {
        host_cur_volume += HOST_VOLUME_STEP;
        i = 1;
    }
    cur_volume += (int16_t)(i*HOST_VOLUME_STEP*device_volume_step);
    g_cur_vol[0] = (uint8_t)((int16_t)(cur_volume)&0x00FF);
    g_cur_vol[1] = (uint8_t)((int16_t)(cur_volume)>>8);
    audio_com.CLASS_PTR = (CLASS_CALL_STRUCT_PTR)&audio_control.CLASS_INTF;
    audio_com.CALLBACK_FN = usb_host_audio_request_ctrl_callback;
    audio_com.CALLBACK_PARAM = &control_feature;
    for (i = 0; i <= max_audio_channel; i++)
    {
        usb_class_audio_feature_command(&audio_com,i,(void *)g_cur_vol,USB_AUDIO_SET_CUR_VOLUME);
    }
    physic_volume = ((int16_t)(cur_volume)*39)/10000;
    printf("\n Current physical volume: %d dB",physic_volume);
}

/******************************************************************************
*   @name        audio_decrease_volume_command
*
*   @brief       Decrease audio speaker
*
*   @param       None
*
*   @return      None
*
******************************************************************************
* This function is used to decrease volume of audio speaker
*****************************************************************************/
void audio_decrease_volume_command(void)
{
    FEATURE_CONTROL_STRUCT control_feature;
    static uint32_t i;
    uint8_t max_audio_channel;
    if ((USB_DEVICE_INUSE != audio_stream.DEV_STATE)||(device_direction != USB_AUDIO_DEVICE_DIRECTION_IN))
    {
        printf("  Error: Audio Speaker is not connected\n");
    }
    max_audio_channel = frm_type_desc->bNrChannels;
    /* Send set mute request */
    audio_com.CLASS_PTR = (CLASS_CALL_STRUCT_PTR)&audio_control.CLASS_INTF;
    cur_volume = (g_cur_vol[1]<<8)|(g_cur_vol[0]);
    if(host_cur_volume < (HOST_VOLUME_STEP + HOST_MIN_VOLUME))
    {
        host_cur_volume = HOST_MIN_VOLUME;
        i = 0;
    }
    else
    {
        host_cur_volume -= HOST_VOLUME_STEP;
        i = 1;
    }
    cur_volume -= (int16_t)(i*HOST_VOLUME_STEP*device_volume_step);
    g_cur_vol[0] = (uint8_t)((int16_t)(cur_volume)&0x00FF);
    g_cur_vol[1] = (uint8_t)((int16_t)(cur_volume)>>8);
    audio_com.CALLBACK_FN = usb_host_audio_request_ctrl_callback;
    audio_com.CALLBACK_PARAM = &control_feature;
    for (i=0 ; i<= max_audio_channel ; i++)
    {
        usb_class_audio_feature_command(&audio_com,i,(void *)g_cur_vol,USB_AUDIO_SET_CUR_VOLUME);
    }
    physic_volume = ((int16_t)(cur_volume)*39)/10000;
    printf("\n Current physical volume: %d dB",physic_volume);
}

/* EOF */
