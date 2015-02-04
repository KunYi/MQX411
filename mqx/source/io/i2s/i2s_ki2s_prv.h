#ifndef _i2s_ki2s_prv_h
#define _i2s_ki2s_prv_h 
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
*   required for the I2S drivers for 54XX Coldfire family.
*
*
*END************************************************************************/
#include "i2s_ki2s.h"
#include "i2s_audio.h"

/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

/*
** I2S defaults
*/
#define I2S_DEFAULT_BUFFER_SIZE 64
#define I2S_DEFAULT_DATA_BITS   16
#define I2S_DEFAULT_CHANNEL     0
#define I2S_DEFAULT_MODE        I2S_MODE_MASTER
#define I2S_DEFAULT_CLK_SOURCE  I2S_CLK_INT
/*
** I2S Registers
*/
#define MCF54XX_CCM_MISCCR_SSI0SRC (0x10)
#define MCF54XX_CCM_MISCCR_SSI1SRC (0x20)
#define MCF54XX_CCM_MISCCR_CDRH_SSI0DIV(x) ((x) << 8)
#define MCF54XX_CCM_MISCCR_CDRH_SSI1DIV(x) (x)
/*
** I2S limits
*/
#define SSIDIV_MIN 3
#define SSIDIV_MAX 100
#define BCLK_DIV_MIN 1
#define BCLK_DIV_MAX 4096
#define FIFO_WATERMARK_MAX 15
#define SSI0_SLOT 47
#define SSI1_SLOT 50

//#define I2S_DEBUG 1

/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** I2S software buffer structure
*/
typedef struct ki2s_buffer
{
	/* Pointer to data */
	uint32_t *DATA;
	
	/* Buffer size */
	uint32_t SIZE;
	
	/* Data input index */
	uint32_t IN;
	
	/* Data output index */
	uint32_t OUT;
	
	/* Free space in buffer */
	uint32_t SPACE;
		
} KI2S_BUFFER, * KI2S_BUFFER_PTR;

/*
**
*/
typedef struct ki2s_dummy_data
{
   /* Pointer to dummy data (sine wave) */
   int16_t *DATA;
   
   /* Length in samples of dummy data */
   uint32_t LENGTH;
   
   /* Output index left */
   uint32_t INDEX_L;
   
   /* Output index right */
   uint32_t INDEX_R;
   
} KI2S_DUMMY_DATA, *  KI2S_DUMMY_DATA_PTR;

/*
** I2S_STATISTICS_STRUCT
** I2S run time statistics structure
*/
typedef struct i2s_statistics_struct_prv
{
    /* Number of I2S interrupts so far */
    uint32_t INTERRUPTS;

    /* FIFO0 underruns */
    uint32_t UNDERRUNS_L;

    /* FIFO1 underruns */
    uint32_t UNDERRUNS_R;

    /* FIFO0 overruns */
    uint32_t OVERRUNS_L;

    /* FIFO1 underruns */
    uint32_t OVERRUNS_R;

    /* Number of valid bytes received (not dummy receives) */
    uint32_t RX_PACKETS;

    /* Number of valid bytes transmitted (not dummy transmits) */
    uint32_t TX_PACKETS;

    /* Number of bytes currently read or wrote - left */
    uint32_t PACKETS_PROCESSED_L;

    /* Number of bytes currently buffered - left */
    uint32_t PACKETS_QUEUED_L;

    /* Number of bytes requested for reading or writing - left */
    uint32_t PACKETS_REQUESTED_L;

    /* Number of bytes currently read or wrote - right */
    uint32_t PACKETS_PROCESSED_R;

    /* Number of bytes currently buffered - right */
    uint32_t PACKETS_QUEUED_R;

    /* Number of bytes requested for reading or writing - right */
    uint32_t PACKETS_REQUESTED_R;

} I2S_STATISTICS_STRUCT_PRV, * I2S_STATISTICS_STRUCT_PRV_PTR;

/*
** KI2S_INFO_STRUCT
** I2S run time state information
*/
typedef struct ki2s_info_struct
{  
   /* Current initialized values */
   KI2S_INIT_STRUCT INIT;
   
   I2S_MemMapPtr I2S_PTR;
   
   /* The previous interrupt handler and data */
   void (_CODE_PTR_ OLD_ISR)(void *);
   void    *OLD_ISR_DATA;

   /* SSI channel */
   uint8_t CHANNEL;
   
   /* Divider of fsys to create SSI clock*/
   uint8_t MCLK_DIV;
   
   /* Bit clock divider */
   uint16_t BCLK_DIV;
   
   /* Oversampling clock frequency in Hz - only valid when internal clock source
	is selected, ignored otherwise */
   uint32_t MCLK_FREQ;
	
   /* Bit clock frequency in Hz - only valid when internal clock source
	is selected, ignored otherwise */
   uint32_t BCLK_FREQ;

   /* Frame sync frequency in Hz - only valid when internal clock source
	is selected, ignored otherwise */
   uint32_t FS_FREQ;
	
   /* SSI module clock source (only affects I2S master mode) */
   uint8_t CLOCK_SOURCE;
   
   /* I2S mode of operation (master/slave)*/
   uint8_t MODE;
   
   /* I2S I/O mode (write/read) */
   uint8_t IO_MODE;
   
   /* Audio data input/output format */
   AUDIO_DATA_FORMAT IO_FORMAT;
   
   /* Transmit I2S clocks and dummy data when buffer is empty */
   bool TX_DUMMY;
   
   /* Number of valid data bits */
   uint8_t DATA_BITS;
   
   /* Transmission in progress */
   bool ONTHEWAY;

   /* Pointer to the buffer to use for current data - left */
   KI2S_BUFFER BUFFER_L;
   
   /* Pointer to the buffer to use for current data - right */
   KI2S_BUFFER BUFFER_R;
   
   /* Transmission statistics */
   I2S_STATISTICS_STRUCT_PRV STATS;
   
   /* Data for dummy transmission*/
   KI2S_DUMMY_DATA DUMMY;
   
   /* Pointer to data that are going to be processed in left channel*/
   char *DATA_L;
   
   /* Pointer to data that are going to be processed in right channel*/ 
   char *DATA_R;
   
   /* Event signaling finished read/write operation */
   LWSEM_STRUCT EVENT_IO_FINISHED;
   
   /* Clock state when no tx/rx */
   bool CLK_ALWAYS_ON;
   
   /* First Input/Output operation */
   bool FIRST_IO;
   
   /* Mask for receive interrupts enabling/disabling*/
   uint32_t RX_INT_MASK;
   
   /* Mask for transmitt interrupts enabling/disabling*/
   uint32_t TX_INT_MASK;
   
} KI2S_INFO_STRUCT, * KI2S_INFO_STRUCT_PTR;

#endif
/* EOF */
