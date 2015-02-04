#ifndef _esai_fifo_config_h_
#define _esai_fifo_config_h_ 1
/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
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
*   This include file is used to provide information needed by
*   applications using the ESAI FIFO I/O device driver functions.
*
*
*END************************************************************************/

#include <ioctl.h>
#include <esai_config.h>

/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** ESAI FIFO Polling ID enumeration
** Enumerate Polling ID using by Poll function
*/
typedef enum
{
    /*fifo counter equals zero*/
    ESAI_FIFO_POLL_TX_FIFO_EMPTY,
    ESAI_FIFO_POLL_RX_FIFO_EMPTY,

    /*fifo empty slots exceeds or meet the water mark*/
    ESAI_FIFO_POLL_TX_FIFO_MEET,
    ESAI_FIFO_POLL_RX_FIFO_MEET,

    /*The dma packets valid in current buffer*/
    ESAI_FIFO_POLL_TX_BUFFER_LEVEL,
    ESAI_FIFO_POLL_RX_BUFFER_LEVEL

} ESAI_FIFO_POLL_ID;


/*
** ESAI FIFO PCM format enumeration
** Enumerate the PCM format can In/Out ESAI FIFO.
*/
typedef enum
{
    /*signed 8bit, little endian*/
    ESAI_FIFO_PCM_S8_LE,

    /*unsigned 8bit, little endian*/
    ESAI_FIFO_PCM_U8_LE,

    /*signed 16bit, little endian*/
    ESAI_FIFO_PCM_S16_LE,

    /*unsigned 16bit, little endian*/
    ESAI_FIFO_PCM_U16_LE,

    /*signed 24bit, little endian*/
    ESAI_FIFO_PCM_S24_LE,

    /*unsigned 24bit, little endian*/
    ESAI_FIFO_PCM_U24_LE,

    /*packed signed 24bit to 32bits, little endian*/
    ESAI_FIFO_PCM_S24_PACK_LE,

    /*packed unsigned 24bit to 32bits, little endian*/
    ESAI_FIFO_PCM_U24_PACK_LE,

    /*invalid pcm format*/
    ESAI_FIFO_PCM_FORMAT_INVALID
} ESAI_FIFO_PCM_FORMAT;

/*
** AUD_IO_FW directions enumeration
** Enumerate the directions
*/
typedef enum
{
    /*TX*/
    AUD_IO_FW_DIR_TX = 0x1,

    /*RX*/
    AUD_IO_FW_DIR_RX = 0x2,

    /*Full duplex*/
    AUD_IO_FW_DIR_FULL_DUPLEX = 0x4,

    /*end of direction enumeration*/
    AUD_IO_FW_DIR_END

} AUD_IO_FW_DIRECTION;

/*
** ESAI FIFO TR direction setting structure
** Holds the parameters for ESAI FIFO TR direction setting
*/
typedef struct esai_fifo_tr_dir_config
{
    /*the index of transceiver*/
    uint32_t tr_index;

    /*the direction setting for one tranceiver, TX or RX*/
    AUD_IO_FW_DIRECTION dir;

} ESAI_FIFO_TR_DIR_STRUCT, * ESAI_FIFO_TR_DIR_STRUCT_PTR;


/*
** ESAI FIFO configuration structure
** Holds the parameters for ESAI FIFO configuration
*/
typedef struct esai_fifo_config
{
    /*the clock setting*/
    ESAI_MODULE_CLK_CONFIG_STRUCT clock;

    /*the audio interfaces format setting*/
    ESAI_MODULE_INF_CONFIG_STRUCT inf_fmt;

    /*the pcm data format setting*/
    ESAI_FIFO_PCM_FORMAT  pcm_fmt;

    bool asrc_8bit_covert;

} ESAI_FIFO_CONFIG_STRUCT, * ESAI_FIFO_CONFIG_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif

#endif
/* EOF */
