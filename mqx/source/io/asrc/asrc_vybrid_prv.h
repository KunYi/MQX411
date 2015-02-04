#ifndef _asrc_vybrid_prv_h
#define _asrc_vybrid_prv_h 1
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
*   This file is ASRC private data header file
*
*
*END************************************************************************/
#include "vybrid_asrc_regs.h"

#define ASRC_VYBRID_IO_NOT_AVAILABLE (-1)
#define ASRC_VYBRID_IO_OK (0)
#define ASRC_VYBRID_EINVAL (-2)

#define ASRCA_DMA_RX_CHANNEL (31)
#define ASRCA_DMA_TX_CHANNEL (30)
#define ASRCB_DMA_RX_CHANNEL (21)
#define ASRCB_DMA_TX_CHANNEL (22)
#define ASRCC_DMA_RX_CHANNEL (23)
#define ASRCC_DMA_TX_CHANNEL (24)

#define DMA_REQ_ASRCA_TX        (41)
#define DMA_REQ_ASRCB_TX        (43)
#define DMA_REQ_ASRCC_TX        (53)
#define DMA_REQ_ASRCA_RX        (40)
#define DMA_REQ_ASRCB_RX        (42)
#define DMA_REQ_ASRCC_RX        (52)


/*
** VYBRID_ASRC_CLK
** Enumerate the ASRC reference clock sources
*/
typedef enum vybrid_asrc_clk {
    VYBRID_CLK_NONE = 0x0F,
    VYBRID_CLK_ESAI_RX = 0x00,
    VYBRID_CLK_SAI0_RX = 0x01,
    VYBRID_CLK_SAI1_RX = 0x02,
    VYBRID_CLK_SAI2_RX = 0x03,
    VYBRID_CLK_SAI3_RX = 0x04,
    VYBRID_CLK_SPDIF_RX = 0x05,
    VYBRID_CLK_MLB_CLK = 0x06,
    VYBRID_CLK_ESAI_TX = 0x07,
    VYBRID_CLK_SAI0_TX = 0x08,
    VYBRID_CLK_SAI3_TX = 0x09,
    VYBRID_CLK_SPDIF_TX = 0x0a,
    VYBRID_CLK_PLL4_DIV = 0x0b,
    VYBRID_CLK_EXT_AUD_CLK = 0x0c,
    VYBRID_CLK_SAI1_TX = 0x0d,
    VYBRID_CLK_NA = 0xf
}VYBRID_ASRC_CLK;


/*
** ASRC_STATE
** Enumerate the internal state used in ASRC internal driver
*/
typedef enum asrc_state {
    /*init state,  once all the ports DMA are installed, change to STOP*/
    ASRC_INIT = 0x0,

    /*stop state, once any port DMA is uninstalled, change to INIT*/
    ASRC_STOP,

    /*running state, can't apply any configuration in this state*/
    ASRC_RUNNING
}ASRC_STATE;


struct vybrid_asrc_info_struct;

/*
** VYBRID_ASRC_SET_NODE_STRUCT
** The internal information structure for one ASRC pair.
** Driver uses this structure to manage the ASRC pair
*/
typedef struct vybrid_asrc_set_node
{
    /*This ASRC pair's id, from 0-2*/
    uint8_t asrc_pair_id;

    /*This ASRC pair's state, refer IMX53_ASRC_STATE*/
    uint8_t state;

    /*Record the open mode for this ASRC pair*/
    uint8_t mode;

    /* The channels number of this ASRC pair*/
    uint8_t pair_channels;

    /* Record the DMA channel ID installed on the input port of this ASRC pair*/
    DMA_CHANNEL_HANDLE input_dma_channel;

    /* Record the DMA channel ID installed on the output port of this ASRC pair*/
    DMA_CHANNEL_HANDLE output_dma_channel;

    /* Pointer to the high level ASRC information structure*/
    struct vybrid_asrc_info_struct *asrc_info_ptr;

    /* The mutex to sychronize the access this ASRC pair*/
    MUTEX_STRUCT   access_mutex;
}VYBRID_ASRC_SET_NODE_STRUCT, * VYBRID_ASRC_SET_NODE_STRUCT_PTR;


/*
** VYBRID_ASRC_PAIR_DESC_STRUCT
** Driver uses this descriptor to connect and idetify ASRC pair information
*/
typedef struct vybrid_asrc_pair_descriptor
{
    /* Uniqule ID for this ASRC pair descriptor
       ** 0xFFFFFFFF indicates this descriptor is free
       */
    uint32_t                            identity;

    /* Pointer to ASRC set node's structure*/
    VYBRID_ASRC_SET_NODE_STRUCT_PTR     asrc_set_node_ptr;

    /* Pointer to ASRC set node's configuration structure
       ** which records current configurations
       */
    ASRC_SET_CONFIG_STRUCT_PTR   asrc_set_node_config_ptr;

}VYBRID_ASRC_PAIR_DESC_STRUCT, * VYBRID_ASRC_PAIR_DESC_STRUCT_PTR;


/*
** VYBRID_ASRC_INFO_STRUCT
** Run time state information for imx53 asrc device
*/
typedef struct vybrid_asrc_info_struct
{
    /* Current initialized values */
    VYBRID_ASRC_INIT_STRUCT                  INIT;

    /* ASRC pairs' descriptors*/
    VYBRID_ASRC_PAIR_DESC_STRUCT             PAIR_DESCS[ASRC_PAIRS_NUM];

    /* Base registers address for this ASRC device */
    ASRC_MemMapPtr asrc_regs;

} VYBRID_ASRC_INFO_STRUCT, * VYBRID_ASRC_INFO_STRUCT_PTR;

#endif
