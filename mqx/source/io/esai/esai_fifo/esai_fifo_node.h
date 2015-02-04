#ifndef _esai_fifo_node_h_
#define _esai_fifo_node_h_ 1
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
*   This include file is used to provide structures and definitions for ESAI FIFO level driver
*
*
*END************************************************************************/

//#define __ESAI_USE_EDMA_DRIVER

//#ifdef __ESAI_USE_EDMA_DRIVER
//#include "edma_channel.h"
//#else
//#include <dma.h>
//#endif

#include <dma.h>

#include <mutex.h>

#define __ESAI_24BIT_DMA_UNSUPPORT

#define ESAI_MAX_TX_DMA_PACKET_NUM 8
#define ESAI_MAX_RX_DMA_PACKET_NUM 8

#define ESAI_TX_DMA_ZERO_PACKET_NUM 3
#define ESAI_RX_DMA_ZERO_PACKET_NUM 3

#define ESAI_ZERO_PADDING_UNIT_SIZE 32 /*suggest to align with FIFO threshold*/

#define ESAI_MAX_TR_NUM 6

#define ESAI_TX_BUFLIST_MAX_ENTRIES 4

#define ESAI_TX_BUFLIST_MAX_BYTES 2097152 /*2Mbytes*/

#define ESAI_TX_DMA_CHANNEL (16)
#define ESAI_TX_ASRC_DMA_CHANNEL (17)
#define ESAI_RX_DMA_CHANNEL (18)
#define ESAI_TX_RX_DMA_NUMBER (2)

#define DMA_REQ_ESAI_TX             (34)
#define DMA_REQ_ESAI_RX             (35)


#ifdef __ESAI_24BIT_DMA_UNSUPPORT
//#define __ESAI_24_TO_32_PERF_MEASURE
//#define __ESAI_32_TO_24_PERF_MEASURE
#endif

//#define ASRC_HARDCODE_VERSION
#undef ASRC_HARDCODE_VERSION


/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/

/*
** AUD_IO_NODE types enumeration
*/

/*
** Error code means the return OK.
*/
#define ESAI_NODE_OK                                      (0x00)

/*
** Error code means the ESAI NODE TX state check fail
*/
#define ESAI_NODE_TX_STATE_ERROR                          (0x01)

/*
** Error code means the ESAI NODE RX state check fail
*/
#define ESAI_NODE_RX_STATE_ERROR                          (0x02)

/*
** Error code means memory allocation fail
*/
#define ESAI_NODE_ALLOC_MEMORY_ERROR                      (0x03)

/*
** Error code means requested resource out of limitation
*/
#define ESAI_NODE_OUT_OF_RESOURCE_LIMIT                   (0x04)

/*
** Error code means parameters value is invalid
*/
#define ESAI_NODE_INVALID_PARAMETER                       (0x05)

/*
** Error code means install ISR failure
*/
#define ESAI_NODE_INSTALL_ISR_FAIL                        (0x06)

/*
** Error code means ESAI node TX format related configuration  error
*/
#define ESAI_NODE_TX_CONFIG_FMT_PARA_ERROR                (0x07)

/*
** Error code means ESAI node TX clock related configuration  error
*/
#define ESAI_NODE_TX_CONFIG_CLK_PARA_ERROR                (0x08)

/*
** Error code means ESAI node TX protocol related configuration  error
*/
#define ESAI_NODE_TX_CONFIG_PROTOCOL_PARA_ERROR           (0x09)

/*
** Error code means ESAI node TX fails to add packet to DMA
*/
#define ESAI_NODE_TX_DMA_ADD_PACKET_FAIL                  (0x0a)

/*
** Error code means ESAI node TX fails to config DMA
*/
#define ESAI_NODE_TX_DMA_CONFIG_FAIL                      (0x0b)

/*
** Error code means ESAI node TX fails to start DMA
*/
#define ESAI_NODE_TX_DMA_START_FAIL                       (0x0c)

/*
** Error code means ESAI node TX fails to request a DMA channel
*/
#define ESAI_NODE_TX_DMA_REQUEST_FAIL                     (0x0d)

/*
** Error code means ESAI node TX fails to set callback to DMA
*/
#define ESAI_NODE_TX_DMA_CB_SET_FAIL                      (0x0e)

/*
** Error code means ESAI node TX fails to send data to DMA
*/
#define ESAI_NODE_TX_DMA_SEND_FAIL                        (0x0f)

/*
** Error code means ESAI node RX fails to request a DMA channel
*/
#define ESAI_NODE_RX_DMA_REQUEST_FAIL                     (0x10)

/*
** Error code means ESAI node RX fails to set callback to DMA
*/
#define ESAI_NODE_RX_DMA_CB_SET_FAIL                      (0x11)

/*
** Error code means ESAI node RX format related configuration  error
*/
#define ESAI_NODE_RX_CONFIG_FMT_PARA_ERROR                (0x12)

/*
** Error code means ESAI node RX fails to config DMA
*/
#define ESAI_NODE_RX_DMA_CONFIG_FAIL                      (0x13)

/*
** Error code means ESAI node RX fails to start DMA
*/
#define ESAI_NODE_RX_DMA_START_FAIL                       (0x14)

/*
** Error code means ESAI node RX protocol related configuration  error
*/
#define ESAI_NODE_RX_CONFIG_PROTOCOL_PARA_ERROR           (0x15)


/*
** Error code means ESAI node TX write fails, due to invalid parameter
*/
#define ESAI_NODE_TX_WR_INVALID_PARAMETER           (-1)

/*
** Error code means ESAI node TX write fails, due to invalid TX state
*/
#define ESAI_NODE_TX_WR_INVALID_STATE               (-2)

/*
** Error code means ESAI node TX write fails, due to no memory avaliable
*/
#define ESAI_NODE_TX_WR_MEMORY_OUT                  (-3)


/*
** ESAI_NODE_STATE
** This enumerates the ESAI FIFO states
*/
typedef enum
{
    ESAI_STATE_RESET,
    ESAI_STATE_STOP,
    ESAI_STATE_RUN
} ESAI_NODE_STATE;

/*
** ESAI_NODE_TX_PLAY_STATE
** This enumerates the ESAI FIFO TX playing states
*/
typedef enum
{
    ESAI_TX_PLAY_REQ,
    ESAI_TX_PLAY_NORMAL,
    ESAI_TX_PLAY_ZERO_FILLED
} ESAI_NODE_TX_PLAY_STATE;

/*
** ESAI_NODE_FIFO_FMT
** This enumerates the ESAI FIFO data format length
*/
typedef enum
{
    ESAI_FIFO_DATA_8BIT = 1,
    ESAI_FIFO_DATA_16BIT,
    ESAI_FIFO_DATA_24BIT,
    ESAI_FIFO_DATA_32BIT
} ESAI_NODE_FIFO_FMT;

/*
** ESAI_FIFO_NODE_TX_BUFFER internal structure
** The structure is a head that links to a list
*/
typedef struct esai_fifo_node_tx_buffer
{
    void * tx_buffer_ptr;
    uint32_t tx_buffer_length;
    uint32_t tx_buffer_status;
    struct esai_fifo_node_tx_buffer * next_tx_buffer;
}ESAI_FIFO_NODE_TX_BUFFER, * ESAI_FIFO_NODE_TX_BUFFER_PTR;


/*
** ESAI_FIFO_AUD_IO_NODE internal structure
** The structure contains the internal variables and function pointers
*/
typedef struct esai_fifo_aud_io_node
{
    /*esai register memory*/
    ESAI_MemMapPtr esai_regs;
    uint32_t esai_irq;

    uint32_t fsys_clk;
    uint32_t extal_clk;

    /*ESAI FIFO internal state*/
    ESAI_NODE_STATE tx_state;
    ESAI_NODE_STATE rx_state;

    /*ESAI FIFO internal playing states, for packet padding*/
    ESAI_NODE_TX_PLAY_STATE tx_play_state;
    ESAI_NODE_TX_PLAY_STATE rx_play_state;

    /*DMA packets*/
//#ifdef __ESAI_USE_EDMA_DRIVER
    DMA_TCD tx_dma_packets[ESAI_MAX_TX_DMA_PACKET_NUM];
    DMA_TCD rx_dma_packets[ESAI_MAX_RX_DMA_PACKET_NUM];

    DMA_TCD tx_dma_zero_packets[ESAI_TX_DMA_ZERO_PACKET_NUM];
    DMA_TCD rx_dma_zero_packets[ESAI_TX_DMA_ZERO_PACKET_NUM];

    int32_t tx_size_per_request;
    int32_t rx_size_per_request;
//#else
//    dma_buffer_t tx_dma_packets[ESAI_MAX_TX_DMA_PACKET_NUM];
//    dma_buffer_t rx_dma_packets[ESAI_MAX_RX_DMA_PACKET_NUM];

//    dma_buffer_t tx_dma_zero_packets[ESAI_TX_DMA_ZERO_PACKET_NUM];
//    dma_buffer_t rx_dma_zero_packets[ESAI_TX_DMA_ZERO_PACKET_NUM];
//#endif

    /*DMA channels*/
//    int32_t tx_dma_channel;
//    int32_t rx_dma_channel;

    DMA_CHANNEL_HANDLE tx_dma_channel;
    DMA_CHANNEL_HANDLE rx_dma_channel;

    /*DMA packet size*/
    uint32_t tx_packet_size;
    uint32_t rx_packet_size;

#ifdef __ESAI_24BIT_DMA_UNSUPPORT
    uint32_t rx_padding_size;
#endif

    /*DMA packets total number*/
    uint8_t  tx_packet_num;
    uint8_t  rx_packet_num;

    /*DMA committed packets count*/
    uint8_t  tx_packet_count;
    uint8_t  rx_packet_count;

    uint8_t  tx_buffer_window;

    uint8_t  tx_co_data_count;
    uint8_t  rx_co_data_count;

    /*Committed count for DMA zero padding packets */
    uint8_t  tx_zero_packet_count;
    uint8_t  rx_zero_packet_count;

    uint8_t  tx_zero_packet_index;
    uint8_t  rx_zero_packet_index;

    /*pointers to internal data buffer used by DMA transfer*/
    void * tx_buffer;
    void * rx_buffer;

    /*pointers to internal zero padding buffer*/
    void * tx_zero_buffer;
    void * rx_zero_buffer;

    /*variables to manage the TX and RX DMA transfer*/
    uint8_t  tx_in;
    uint8_t  tx_out;

    uint8_t  rx_in;
    uint8_t  rx_out;
    uint8_t  rx_ready;
    uint8_t  rx_buf_overrun_count;

    LWSEM_STRUCT   tx_sem;
    LWSEM_STRUCT   rx_sem;
    MUTEX_STRUCT   control_mutex;

    /*bitmaps for recording TX and RX transceiver setting*/
    uint8_t  tr_enable_save;
    uint8_t  re_enable_save;
    uint8_t  tr_tx_dir_save;
    uint8_t  tr_rx_dir_save;

    uint8_t  tx_packet_demand_threshold;

    /*water mark for ESAI FIFO setting*/
    uint8_t  tx_fifo_wm;
    uint8_t  rx_fifo_wm;

    uint32_t tx_fifo_wa;
    uint32_t rx_fifo_wa;

    /*The data format in ESAI FIFO*/
    ESAI_NODE_FIFO_FMT  tx_fifo_data_fmt;
    ESAI_NODE_FIFO_FMT  rx_fifo_data_fmt;

    /*Statistics in ESAI FIFO*/
    uint32_t tx_underrun;
    uint32_t tx_zero_pad_count;
    uint32_t tx_buffer_empty;
    uint32_t write_times;
    uint32_t read_times;

    /*Call back functions for higher level using*/
    void (*callback_func_ptr)(void *, uint32_t, void *);
    void * callback_param;

#ifdef __ESAI_24BIT_DMA_UNSUPPORT
    void * padding_buffer;
    uint32_t padding_buf_length;
    uint32_t padding_buf_data_length;
#endif

    /*ASRC related variables to control the ASRC coworking*/
    bool asrc_enable;
//#ifdef __ESAI_USE_EDMA_DRIVER
    DMA_TCD tx_asrc_dma_packet;
//#else
//    dma_buffer_t tx_asrc_dma_packet;
//#endif
    DMA_CHANNEL_HANDLE tx_asrc_esai_channel;

#ifdef ASRC_HARDCODE_VERSION
    int32_t tx_asrc_src_sample_rate;
#endif

    bool tx_asrc_8bit_data;

    /*Variables to manage buffers for DIM*/
    ESAI_FIFO_NODE_TX_BUFFER_PTR tx_buflist;
    uint32_t                  tx_buflist_bytes;
    uint32_t                  tx_buflist_entries;

    /* syn operating mode or not */
    bool syn_mode;

    /* User's timeout in write and read*/
    uint32_t rx_timeout;
    uint32_t tx_timeout;

    /*Error codes in write and read*/
    uint32_t tx_error_code;
    uint32_t rx_error_code;
} ESAI_FIFO_AUD_IO_NODE_STRUCT, * ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR;


typedef struct
{
   /* The ESAI controller to initialize */
   uint8_t                 MODULE_ID;

   /* The default water mark for the TX FIFO*/
   uint8_t                 TX_FIFO_WM;

   /* The default water mark for the RX FIFO*/
   uint8_t                 RX_FIFO_WM;

   /* The system irq number for this controller*/
   uint8_t                 IRQ_NUM;

   /* The base address for this ESAI controller */
   uint32_t                BASE_ADDRESS;

   /* The clock freq of Fsys*/
   uint32_t                FSYS_CLK;

   /* The clock freq of EXTAL*/
   uint32_t                EXTAL_CLK;

   /* Tx DMA packet size  */
   uint32_t                TX_DMA_PACKET_SIZE;

   /* Tx DMA packet max number  */
   uint32_t                TX_DMA_PACKET_MAX_NUM;

   /* Rx DMA packet size  */
   uint32_t                RX_DMA_PACKET_SIZE;

   /* Rx DMA packet max number  */
   uint32_t                RX_DMA_PACKET_MAX_NUM;

   /* Syn operating mode or not */
   bool SYN_MODE;

} ESAI_FIFO_NODE_INIT_STRUCT, * ESAI_FIFO_NODE_INIT_STRUCT_PTR;



/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern const ESAI_FIFO_NODE_INIT_STRUCT esai_fifo_default_param;

extern uint32_t esai_fifo_node_init(
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node,
    ESAI_FIFO_NODE_INIT_STRUCT_PTR init_param);

extern int32_t esai_fifo_node_read(
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node,
    void* buffer, uint32_t length);
extern int32_t esai_fifo_node_write(
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node,
    void* buffer, uint32_t length);
extern uint32_t esai_fifo_node_start(
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node, void* parameters);
extern uint32_t esai_fifo_node_stop(
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node, void* parameters);

extern uint32_t esai_fifo_node_poll(
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node, uint32_t poll_id, void* parameter);

extern ESAI_NODE_STATE esai_fifo_node_get_tx_state(
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node);

extern ESAI_NODE_STATE esai_fifo_node_get_rx_state(
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node);


#ifdef __cplusplus
}
#endif

#endif
/* EOF */
