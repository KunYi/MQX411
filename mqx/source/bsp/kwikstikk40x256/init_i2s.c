/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains the definition for the baud rate for the I2C
*   channel
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "i2s.h"
#include "i2s_ki2s.h"
#include "i2s_audio.h"

const AUDIO_DATA_FORMAT _bsp_audio_data_init = {
        AUDIO_LITTLE_ENDIAN,       /* Endian of input data */
        AUDIO_ALIGNMENT_LEFT,   /* Aligment of input data */
        8,                      /* Bit size of input data */
        1,                      /* Sample size in bytes */
        1                       /* Number of channels */
};
const KI2S_INIT_STRUCT _bsp_i2s0_init = {
   0,                      /* I2S channel      */
   BSP_I2S0_MODE,          /* I2S mode         */ 
   BSP_I2S0_DATA_BITS,     /* I2C data bits    */ 
   BSP_I2S0_CLOCK_SOURCE,  /* I2C clock source */
   FALSE,                  /* I2C data are Stereo */
   FALSE,                  /* I2C Transmit dummy data */
   BSP_I2S0_INT_LEVEL,     /* I2C interrupt level */
   BSP_I2S0_BUFFER_SIZE,   /* I2C TX buffer size */
   &_bsp_audio_data_init
};

/* EOF */
