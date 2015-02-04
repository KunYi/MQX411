/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
* Copyright 2010 ARC International
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
*   This file contains audio types and definitions.
*
*
*END************************************************************************/

#ifndef __audio_speaker_h__
#define __audio_speaker_h__

#include <mqx.h>
#include <bsp.h>

#ifdef __USB_OTG__
#include "otgapi.h"
#include "devapi.h"
#else
#include "hostapi.h"
#endif
#include "usb_host_audio.h"

#define AUDIO_SPEAKER_FREQUENCY (1000) /* Frequency in Hz*/

#define AUDIO_IDLE       (0x00)
#define AUDIO_PLAYING    (0x01)
#define AUDIO_PAUSE      (0x02)
/***************************************
**
** Application-specific definitions
**
****************************************/

#define HOST_CONTROLLER_NUMBER      USBCFG_DEFAULT_HOST_CONTROLLER

#define NUMBER_OF_IT_TYPE 7
#define NUMBER_OF_OT_TYPE 8

#define USB_EVENT_CTRL              0x01
#define USB_EVENT_RECEIVED_DATA     0x02
#define USB_EVENT_SEND_DATA         0x04
#define USB_EVENT_AUDIO_STREAM_INF  0x05
#define USB_EVENT_MUTE              0x08
#define USB_EVENT_PLAY              0x10

#define MAX_ISO_PACKET_SIZE         512
#define MAX_SD_READ                 1024

#define USB_AUDIO_DEVICE_DIRECTION_IN                0x00
#define USB_AUDIO_DEVICE_DIRECTION_OUT               0x01
#define USB_AUDIO_DEVICE_DIRECTION_UNDEFINE          0xFF

#define USB_TERMINAL_TYPE           0x01
#define INPUT_TERMINAL_TYPE         0x02
#define OUTPUT_TERMINAL_TYPE        0x03

#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACED             (4)
#define  USB_DEVICE_SETTING_PROTOCOL       (5)
#define  USB_DEVICE_INUSE                  (6)
#define  USB_DEVICE_DETACHED               (7)
#define  USB_DEVICE_OTHER                  (8)

/* Audio deformat defaults */
#define AUDIO_DEFAULT_ENDIAN AUDIO_BIG_ENDIAN
#define AUDIO_DEFAULT_ALIGNMENT AUDIO_ALIGNMENT_RIGHT
#define AUDIO_DEFAULT_BITS AUDIO_BIT_SIZE_MAX

/* Audio data aligment */
#define AUDIO_ALIGNMENT_RIGHT 0x00
#define AUDIO_ALIGNMENT_LEFT 0x01

/* Audio data prepare*/
#define AUDIO_BUFF_EMPTY 0x01

/* Audio data endianity */
//#define AUDIO_BIG_ENDIAN 0x00
//#define AUDIO_LITTLE_ENDIAN 0x01

/* Data bit size limits */
#define AUDIO_BIT_SIZE_MIN 8
#define AUDIO_BIT_SIZE_MAX 32

/* Audio error codes */
#define AUDIO_ERROR_INVALID_IO_FORMAT (AUDIO_ERROR_BASE | 0x01)

#define HOST_VOLUME_STEP      1
#define HOST_MIN_VOLUME       0
#define HOST_MAX_VOLUME      10
/* Following structs contain all states and pointers
used by the application to control/operate devices. */

typedef struct audio_device_struct {
    uint32_t                          DEV_STATE;
    _usb_device_instance_handle      DEV_HANDLE;
    _usb_interface_descriptor_handle INTF_HANDLE;
    CLASS_CALL_STRUCT                CLASS_INTF;
} AUDIO_CONTROL_DEVICE_STRUCT, * AUDIO_CONTROL_DEVICE_STRUCT_PTR;

typedef struct data_device_struct {
    uint32_t                          DEV_STATE;
    _usb_device_instance_handle      DEV_HANDLE;
    _usb_interface_descriptor_handle INTF_HANDLE;
    CLASS_CALL_STRUCT                CLASS_INTF;
} AUDIO_STREAM_DEVICE_STRUCT, * AUDIO_STREAM_DEVICE_STRUCT_PTR;

typedef struct feature_control_struct {
    uint32_t                          FU;
} FEATURE_CONTROL_STRUCT, * FEATURE_CONTROL_STRUCT_PTR;

/* Alphabetical list of Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    void usb_host_audio_control_event(_usb_device_instance_handle,_usb_interface_descriptor_handle, uint32_t);
    void usb_host_audio_stream_event(_usb_device_instance_handle,_usb_interface_descriptor_handle, uint32_t);
    void usb_host_audio_tr_callback(_usb_pipe_handle ,void *,unsigned char *,uint32_t ,uint32_t );
    void usb_host_audio_request_ctrl_callback(_usb_pipe_handle,void *,unsigned char *,uint32_t,uint32_t);
    void usb_host_audio_ctrl_callback(_usb_pipe_handle,void *,unsigned char *,uint32_t,uint32_t);
    void usb_host_audio_interrupt_callback(_usb_pipe_handle,void *,unsigned char *,uint32_t,uint32_t);
    void config_channel(USB_AUDIO_CTRL_DESC_FU_PTR,uint8_t);
    void audio_mute_command(void);
    void audio_increase_volume_command(void);
    void audio_decrease_volume_command(void);
    uint32_t USB_Audio_Get_Packet_Size(USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR,uint8_t);
#ifdef __cplusplus
}
#endif
#endif

/* EOF */
