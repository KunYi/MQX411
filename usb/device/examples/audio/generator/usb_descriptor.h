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
 * @brief The file is a header file for USB Descriptors required for Mouse
 *        Application
*
*END************************************************************************/

#ifndef _USB_DESCRIPTOR_H
#define _USB_DESCRIPTOR_H

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_class.h"
#include "usb_audio.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define HIGH_SPEED_DEVICE                (0) 
#define BCD_USB_VERSION                  (0x0200)
 
#define REMOTE_WAKEUP_SHIFT              (5)
#define REMOTE_WAKEUP_SUPPORT            (TRUE)

/* Various descriptor sizes */
#define DEVICE_DESCRIPTOR_SIZE            (18)
#define CONFIG_DESC_SIZE                  (0x6D) 
#define DEVICE_QUALIFIER_DESCRIPTOR_SIZE  (10)
#define REPORT_DESC_SIZE                  (50)
#define CONFIG_ONLY_DESC_SIZE             (9)
#define IFACE_ONLY_DESC_SIZE              (9)
#define AUDIO_ONLY_DESC_SIZE              (9)
#define ENDP_ONLY_DESC_SIZE               (9)
#define HEADER_ONLY_DESC_SIZE             (9)
#define INPUT_TERMINAL_ONLY_DESC_SIZE     (12)
#define OUTPUT_TERMINAL_ONLY_DESC_SIZE    (9)
#define FEATURE_UNIT_ONLY_DESC_SIZE       (9)
#define AUDIO_STREAMING_IFACE_DESC_SIZE   (7)
#define AUDIO_STREAMING_ENDP_DESC_SIZE    (7)
#define AUDIO_STREAMING_TYPE_I_DESC_SIZE  (11)

#define USB_DEVICE_CLASS_AUDIO            (0x01)
#define USB_SUBCLASS_AUDIOCONTROL         (0x01)
#define USB_SUBCLASS_AUDIOSTREAM          (0x02)

#define AUDIO_INTERFACE_DESCRIPTOR_TYPE   (0x24)

#define AUDIO_CONTROL_HEADER              (0x01)
#define AUDIO_CONTROL_INPUT_TERMINAL      (0x02)
#define AUDIO_CONTROL_OUTPUT_TERMINAL     (0x03)
#define AUDIO_CONTROL_FEATURE_UNIT        (0x06)
#define AUDIO_STREAMING_GENERAL           (0x01)
#define AUDIO_STREAMING_FORMAT_TYPE       (0x02)
#define AUDIO_FORMAT_TYPE_I               (0x01)
#define AUDIO_ENDPOINT_GENERAL            (0x01)

/* Max descriptors provided by the Application */
#define USB_MAX_STD_DESCRIPTORS               (8)
#define USB_MAX_CLASS_SPECIFIC_DESCRIPTORS    (2)
/* Max configuration supported by the Application */
#define USB_MAX_CONFIG_SUPPORTED          (1)

/* Max string descriptors supported by the Application */
#define USB_MAX_STRING_DESCRIPTORS        (4)

/* Max language codes supported by the USB */
#define USB_MAX_LANGUAGES_SUPPORTED       (1)


#define AUDIO_UNIT_COUNT                  (3)
#define AUDIO_DESC_ENDPOINT_COUNT         (1)
#define AUDIO_ENDPOINT                    (1)


#define AUDIO_ENDPOINT_PACKET_SIZE (64)

/* string descriptors sizes */
#define USB_STR_DESC_SIZE (2)
#define USB_STR_0_SIZE  (2)
#define USB_STR_1_SIZE  (56)
#define USB_STR_2_SIZE  (28)
#define USB_STR_n_SIZE  (32)

/* descriptors codes */
#define USB_DEVICE_DESCRIPTOR     (1)
#define USB_CONFIG_DESCRIPTOR     (2)
#define USB_STRING_DESCRIPTOR     (3)
#define USB_IFACE_DESCRIPTOR      (4)
#define USB_ENDPOINT_DESCRIPTOR   (5)
#define USB_DEVQUAL_DESCRIPTOR    (6)
#define USB_AUDIO_DESCRIPTOR        (0x25)
#define USB_REPORT_DESCRIPTOR     (0x22)

#define USB_MAX_SUPPORTED_INTERFACES     (1)


/******************************************************************************
 * Types
 *****************************************************************************/


/******************************************************************************
 * Global Functions
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

uint8_t USB_Desc_Get_Descriptor(uint32_t handle, uint8_t type, uint8_t str_num, uint16_t index, uint8_t **descriptor, uint32_t *size);
uint8_t USB_Desc_Get_Interface(uint32_t handle, uint8_t interface, uint8_t *alt_interface);

uint8_t USB_Desc_Set_Interface(uint32_t handle, uint8_t interface, uint8_t alt_interface);
bool USB_Desc_Valid_Configation(uint32_t handle, uint16_t config_val);
bool USB_Desc_Valid_Interface(uint32_t handle, uint8_t interface);
bool USB_Desc_Remote_Wakeup(uint32_t handle);

USB_ENDPOINTS* USB_Desc_Get_Endpoints(uint32_t handle);
USB_AUDIO_UNITS* USB_Desc_Get_Entities(uint32_t handle);
uint8_t USB_Desc_Set_Copy_Protect(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Copy_Protect(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Cur_Mute(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Cur_Volume(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Min_Volume(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Max_Volume(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Res_Volume(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Cur_Mute(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Max_Volume(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Min_Volume(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Cur_Volume(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Res_Volume(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Min_Bass(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Min_Bass(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Max_Bass(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Max_Bass(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Cur_Bass(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Cur_Bass(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Res_Bass(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Res_Bass(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Cur_Mid(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Min_Mid(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Max_Mid(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Res_Mid(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Cur_Mid(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Min_Mid(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Max_Mid(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Res_Mid(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Cur_Treble(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Res_Treble(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Min_Treble(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Max_Treble(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Cur_Treble(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Res_Treble(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Min_Treble(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Max_Treble(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Res_Graphic_Equalizer(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Cur_Graphic_Equalizer(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Min_Graphic_Equalizer(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Max_Graphic_Equalizer(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Res_Graphic_Equalizer(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Cur_Graphic_Equalizer(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Min_Graphic_Equalizer(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Max_Graphic_Equalizer(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Cur_Automatic_Gain(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Cur_Automatic_Gain(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Cur_Delay(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Cur_Delay(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Min_Delay(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Max_Delay(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Res_Delay(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Cur_Delay(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Min_Delay(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Max_Delay(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Res_Delay(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Cur_Bass_Boost(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Cur_Bass_Boost(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Cur_Loudness(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Cur_Loudness(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Cur_Sampling_Frequency(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Min_Sampling_Frequency(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Max_Sampling_Frequency(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Res_Sampling_Frequency(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Cur_Sampling_Frequency(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Min_Sampling_Frequency(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Max_Sampling_Frequency(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Res_Sampling_Frequency(uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Set_Cur_Pitch (uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_Cur_Pitch (uint32_t handle,uint8_t interface,uint8_t **coding_data);
uint8_t USB_Desc_Get_feature(uint32_t handle,int32_t cmd, uint8_t in_data,uint8_t ** in_buf);
uint8_t USB_Desc_Set_feature(uint32_t handle,int32_t cmd, uint8_t in_data,uint8_t ** in_buf);

#ifdef __cplusplus
}
#endif

#endif
