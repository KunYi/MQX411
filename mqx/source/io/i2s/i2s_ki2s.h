#ifndef _i2s_ki2s_h
#define _i2s_ki2s_h 1
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
*   This file contains the definitions of constants and structures
*   required for the I2S drivers for the MCF54XX.
*
*
*END************************************************************************/

#include "i2s_audio.h"

#define BIT16_MAX 32767
/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** MCF54XX_I2S_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a serial port is initialized.
*/
typedef struct ki2s_init_struct
{
   /* The I2S channel to initialize */
   uint8_t CHANNEL;
   
   /* Default operating mode */
   uint8_t MODE;
   
   /* Number of valid data bits*/
   uint8_t DATA_BITS;
   
   /* SSI_CLOCK source*/
   uint8_t CLOCK_SOURCE;
   
   /* Data are stereo */
   bool STEREO;
   
   /* Transmit dummy data */
   bool TX_DUMMY;
   
   /* Interrupt level to use */
   _int_level LEVEL;
   
   /* Tx buffer size */
   uint32_t BUFFER_SIZE;
   
   /* I/O data format */
   AUDIO_DATA_FORMAT const * IO_FORMAT;

} KI2S_INIT_STRUCT, * KI2S_INIT_STRUCT_PTR;
typedef const KI2S_INIT_STRUCT * KI2S_INIT_STRUCT_CPTR;

/*--------------------------------------------------------------------------*/
/* 
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t _ki2s_int_install (char *, KI2S_INIT_STRUCT_CPTR);
extern void   *_bsp_get_i2s_base_address (uint8_t);
extern uint32_t _bsp_get_i2s_vector (uint8_t);
#ifdef __cplusplus
}
#endif

#endif
/* EOF */
