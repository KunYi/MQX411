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
*   required for the SAI/I2S driver
*
*
*END************************************************************************/
#ifndef __SAI_KSAI_PRV_H__
#define __SAI_KSAI_PRV_H__

#include "sai_ksai.h"
#include "sai_audio.h"

/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

/* Limits for master clock divider */
#define FRACT_MAX 256
#define DIV_MAX 4096

/*
** SAI limits
*/

#define BCLK_DIV_MIN    1
#define BCLK_DIV_MAX    4096

/* Hardware FIFO size */
#if !PSP_MQX_CPU_IS_VYBRID
  #define SIZE_OF_FIFO    8
#else
  #define SIZE_OF_FIFO    32
#endif
/* Number of data channels on both RX and TX */
#if !PSP_MQX_CPU_IS_VYBRID
  #define SAI_DATA_CHANNELS 2
#else
  #define SAI_DATA_CHANNELS 1
#endif

#define DEFAULT_BCLK_DIV 8

/* Define the max size of the buffer */
#define SAI_MAX_BUFFER_SIZE 8192



/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** SAI software buffer structure
*/
typedef struct ksai_buffer
{
    /* Pointer to data for interrupt mode*/
    uint32_t * DATA;
    
    /* data for dma mode*/
    uint8_t *DMA_DATA;
    
    /* Buffer size */
    uint32_t SIZE;
    
    /* Data input index */
    uint32_t IN;
    
    /* Data output index */
    uint32_t OUT;
    
    /* Free space in buffer */
    uint32_t SPACE;

    /*The low block address */
    uint8_t * START_PERIOD;

    /*The high block address*/
    uint8_t * END_PERIOD; 

    /* The period number */
    uint8_t PERIODS;

} KSAI_BUFFER, *KSAI_BUFFER_PTR;

typedef struct ksai_dummy_data
{
    /* Pointer to dummy data (sine wave) */
    int16_t * DATA;

    /* Length in samples of dummy data */
    uint32_t LENGTH;

    /* Output index */
    uint32_t INDEX;

} KSAI_DUMMY_DATA, *KSAI_DUMMY_DATA_PTR;

typedef struct ksai_info_struct
{  
    /* Current initialized values */
    KSAI_INIT_STRUCT INIT;

    I2S_MemMapPtr SAI_PTR;

    /* The previous TX interrupt handler and data */
    void (_CODE_PTR_ OLD_TX_ISR)(void *);
    void *  OLD_TX_ISR_DATA;
    
    /* The previous TX interrupt handler and data */
    void (_CODE_PTR_ OLD_RX_ISR)(void *);
    void *  OLD_RX_ISR_DATA;
    
    /* Selected SAI HW module */
    uint8_t HW_CHANNEL;
    
    /* I2S TX channel */
    uint8_t TX_CHANNEL;

    /* I2S RX channel */
    uint8_t RX_CHANNEL;
    
    /* Divider of fsys to create SSI clock*/
    uint8_t MCLK_DIV;
    
    /* Master clock source*/
    CM_CLOCK_SOURCE MCLK_SRC;
    
    /* Bit clock divider */
    uint16_t BCLK_DIV;

    /* 
    ** Oversampling clock frequency in Hz - only valid 
    ** when internal clock source is selected, ignored otherwise 
    */
    uint32_t MCLK_FREQ;
    
    /* 
    ** Bit clock frequency in Hz - only valid 
    ** when internal clock source is selected, ignored otherwise 
    */
    uint32_t BCLK_FREQ;

    /* 
    ** Frame sync frequency in Hz - only valid 
    ** when internal clock source is selected, ignored otherwise 
    */
    uint32_t FS_FREQ;
    
    /* I2S module clock source (only affects SAI master mode) */
    uint8_t CLOCK_SOURCE;

    /* I2S mode of operation (master/slave)*/
    uint8_t MODE;

    /* I2S I/O mode (write/read) */
    uint8_t IO_MODE;

    /* Audio data input/output format */
    AUDIO_DATA_FORMAT IO_FORMAT;

    /* Transmit SAI clocks and dummy data when buffer is empty */
    bool TX_DUMMY;

    /* Number of valid data bits */
    uint8_t DATA_BITS;

    /* Transmission in progress */
    bool ONTHEWAY;

    /* Pointer to the buffer to use for current data */
    KSAI_BUFFER BUFFER;

    /* Transmission statistics */
    I2S_STATISTICS_STRUCT STATS;

    /* Data for dummy transmission*/
    KSAI_DUMMY_DATA DUMMY;

    /* Pointer to data that are going to be processed in channel*/
    char * DATA;
    
    /* Event signaling finished read/write operation */
    LWSEM_STRUCT EVENT_IO_FINISHED;
    
    /* Clock state when no tx/rx */
    bool CLK_ALWAYS_ON;

    /* First Input/Output operation */
    bool FIRST_IO;

    /* Is DMA kick off */
    bool DMA_KICKOFF;
    
    /* Clock setup: sync-async; bitclock: normal-swapped */
    uint8_t CLOCK_MODE;

    /* void * to  IO_SAI_DEV_PTR */
    void * IO_SAI_DEV_PTR;

    /*DMA settings*/
    DMA_TCD TCD;

    DMA_CHANNEL_HANDLE DCH;

    uint32_t TCD_SEQ;
		
} KSAI_INFO_STRUCT, *KSAI_INFO_STRUCT_PTR;

#endif /* __SAI_KSAI_PRV_H__ */

/* EOF */
