#ifndef _esai_vport_config_h_
#define _esai_vport_config_h_ 1
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
*   applications using the ESAI VPORT I/O device driver functions.
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
** ESAI VPORT Channel type enumeration
** Enumerate the Channel type can be supported by ESAI VPORT.
*/
typedef enum
{
    ESAI_VPORT_CHNL_MONO = 0x1,
    ESAI_VPORT_CHNL_STEREO = 0x2
} ESAI_VPORT_CHNL_TYPE;

/*
** ESAI VPORT data width enumeration
** Enumerate the data width can in/out ESAI VPORT.
*/
typedef enum
{
    ESAI_VPORT_DW_8BITS = 0x1,
    ESAI_VPORT_DW_16BITS = 0x2,
    ESAI_VPORT_DW_24BITS = 0x3,
    ESAI_VPORT_DW_32BITS = 0x4,
    ESAI_VPORT_DW_INVALID
} ESAI_VPORT_DATA_WIDTH;

/*
** ESAI VPORT audio hw interface enumeration
** Enumerate the audio hw interface can in/out ESAI VPORT.
*/
typedef enum
{
    ESAI_VPORT_HW_INF_I2S = 0x1,
    ESAI_VPORT_HW_INF_LEFTJ = 0x2,      /*LEFTJ protocol*/
    ESAI_VPORT_HW_INF_RIGHTJ = 0x3,     /*RIGHTJ protocol*/
    ESAI_VPORT_HW_INF_TDM = 0x4
} ESAI_VPORT_HW_INTERFACE;

/*
** ESAI VPORT configuration structure
** Holds the parameters for ESAI VPORT configuration
*/
typedef struct esai_vport_config
{
    /*the channel type, mono or stereo*/
    ESAI_VPORT_CHNL_TYPE chnl_type;

    /*the pcm data format setting*/
    ESAI_VPORT_DATA_WIDTH  data_width;

    bool asrc_8bit_covert;

} ESAI_VPORT_CONFIG_STRUCT, * ESAI_VPORT_CONFIG_STRUCT_PTR;



/*
** ESAI VPORT asrc configuration structure
** Holds the parameters for ESAI VPORT asrc configuration
*/
typedef struct esai_asrc_dma
{
    int32_t input_dma_channel;

    int32_t output_dma_channel;

} ESAI_ASRC_DMA_STRUCT, * ESAI_ASRC_DMA_STRUCT_PTR;

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
