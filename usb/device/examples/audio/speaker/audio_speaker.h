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
* @brief The file contains Macro's and functions needed by the audio speaker 
*        application
*
*END************************************************************************/

#ifndef _audio_speaker_h
#define _audio_speaker_h 1

#include "usb_descriptor.h"

#if defined(_i2s_h_)
    #if ! BSPCFG_ENABLE_II2S0
    #error This application requires BSPCFG_ENABLE_II2S0 defined non-zero in user_config.h. Please recompile libraries with this option.
    #endif
    #define AUDIO_DEVICE      "ii2s0:"    
    #define CLK_MULT          (256)
#elif defined(__SAI_H__)
    #if ! BSPCFG_ENABLE_SAI
    #error This application requires BSPCFG_ENABLE_SAI defined non-zero in user_config.h. Please recompile libraries with this option.
    #endif
    #define AUDIO_DEVICE      "sai:"    
    #define CLK_MULT          (384)
#else
    #error This application requires SAI or I2S audio device.
#endif

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define DATA_BUFF_SIZE                     ((AUDIO_ENDPOINT_PACKET_SIZE) * 8)
#define AUDIO_FORMAT_SAMPLE_RATE           (16000)
#define AUDIO_I2S_FS_FREQ_DIV              (256)

#define USB_APP_ENUM_COMPLETE_EVENT_MASK   (0x01 << 0)
#define USB_APP_BUFFER0_FULL_EVENT_MASK    (0x01 << 1)
#define USB_APP_BUFFER1_FULL_EVENT_MASK    (0x01 << 2)

/*****************************************************************************
 * Global variables
 *****************************************************************************/

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
extern void TestApp_Init(void);

#endif 

/* EOF */
