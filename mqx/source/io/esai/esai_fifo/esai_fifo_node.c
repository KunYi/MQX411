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
*   applications using the audio I/O framework
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "mqx_inc.h"
#include "fio.h"
#include "esai_fifo_prv.h"
#include "esai_fifo_node.h"
#include "esai_fifo_hw.h"
#include "esai_fifo_config.h"



#define ESAI_INC_BUF(x, y)\
{\
    x = (x + 1) % y;\
}

#define ESAI_DEC_BUF(x, y)\
{\
    x = (x + y - 1) % y;\
}


#define ESAI_DMA_TX_MAX_NUM_REQ 2

#define GET_EXTENSION_PTR(x) (x->private_data)

//#define ESAI_FIFO_NODE_ENABLE_DEBUG
//#define ESAI_ASRC_ENABLE_DEBUG
//#undef ESAI_FIFO_NODE_ENABLE_DEBUG

#ifdef ESAI_FIFO_NODE_ENABLE_DEBUG
#define ESAI_FIFO_DEBUG  printf
#else
#define ESAI_FIFO_DEBUG(...)
#endif

#define ESAI_FIFO_NODE_ENABLE_ERROR
#ifdef ESAI_FIFO_NODE_ENABLE_ERROR
#define ESAI_FIFO_ERROR  printf
#else
#define ESAI_FIFO_ERROR(...)
#endif


#define ESAI_RX_FUNC_RESERVED 0


static uint32_t _esai_fifo_32_to_24(void*, uint32_t, void*);


/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_tx_buffer_full
* Returned Value   : bool
* Comments         :
*    This function returns TRUE, if the tx buffer is full,
*    returns FALSE, if the tx buffer is not full
*
*END*********************************************************************/
static bool _esai_fifo_tx_buffer_full
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node
)
{
    if (esai_fifo_node->tx_packet_count >= esai_fifo_node->tx_packet_num)
        return TRUE;

    return FALSE;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_tx_buffer_data_ready
* Returned Value   : bool
* Comments         :
*    This function returns TRUE, if the tx buffer has data,
*    returns FALSE, if the tx buffer is not ready
*
*END*********************************************************************/
static bool _esai_fifo_tx_buffer_data_ready
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node
)
{
    //ESAI_FIFO_DEBUG("[ESAI] check data ready, tx_in=%u, tx_out=%u\n",
    //            esai_fifo_node->tx_in, esai_fifo_node->tx_out);
    if (esai_fifo_node->tx_buffer_window > 0) {
        return TRUE;
    }

    return FALSE;

}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_tx_dma_fill_packets
* Returned Value   : return 0 if success, else return uint32_t error code
* Comments         :
*    This function is used to fill and feed DMA packets for ESAI TX
*
*END*********************************************************************/
static uint32_t _esai_fifo_tx_dma_fill_packets
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node,

    /* [IN] fill zero packets ?*/
    bool fill_zero_packet
)
{
    uint8_t i = 0, num;
    uint8_t out_temp;
    int32_t ret;

    if (esai_fifo_node->tx_state != ESAI_STATE_RUN)
        return ESAI_NODE_TX_STATE_ERROR;

    if (!fill_zero_packet) {

        out_temp = esai_fifo_node->tx_out;

        num = ESAI_DMA_TX_MAX_NUM_REQ;

        while (i < num) {

            if (_esai_fifo_tx_buffer_data_ready(esai_fifo_node)) {
                i++;
                ESAI_INC_BUF(esai_fifo_node->tx_out,
                    esai_fifo_node->tx_packet_num);
                esai_fifo_node->tx_buffer_window--;
            } else {
                break;
            }
        }

        ESAI_FIFO_DEBUG("[ESAI] tx_dma_fill_packets i=%d, tx_in=%u"
                ", tx_out=%u, ct=%u\n",
                i, esai_fifo_node->tx_in, esai_fifo_node->tx_out,
                esai_fifo_node->tx_packet_count);

        if (i != 0)
        {
            if (esai_hw_tx_fifo_empty(esai_fifo_node->esai_regs)) {
                ESAI_FIFO_ERROR("[ESAI] TX FIFO empty!!!, tx_packet_count=%u\n",
                    esai_fifo_node->tx_packet_count);
                    esai_fifo_node->tx_underrun++;
            }

            do {
                ret = dma_transfer_submit(esai_fifo_node->tx_dma_channel,
                            &esai_fifo_node->tx_dma_packets[out_temp],
                            NULL);
                if(ESAI_NODE_OK != ret) {
                    ESAI_FIFO_ERROR("[ESAI] dma packet push failed %d\n", ret);
                    ESAI_FIFO_DEBUG("[ESAI] i=%d, out_temp=%d\n", i, out_temp);
                    ESAI_FIFO_DEBUG("[ESAI] packet info, count=%u, src=0x%x,"
                     "size_per_request=%u\n",
                     esai_fifo_node->tx_dma_packets[out_temp].LOOP_COUNT,
                     esai_fifo_node->tx_dma_packets[out_temp].SRC_ADDR,
                     esai_fifo_node->tx_dma_packets[out_temp].LOOP_BYTES);
                    return ESAI_NODE_TX_DMA_ADD_PACKET_FAIL;
                }
                ESAI_INC_BUF(out_temp, esai_fifo_node->tx_packet_num);
            } while (--i > 0);

        }
    }else {

        /*TX underrun happend, use zero packets to fill the FIFO*/
        out_temp = esai_fifo_node->tx_zero_packet_index;
        if (out_temp < ESAI_TX_DMA_ZERO_PACKET_NUM) {

            if (esai_hw_tx_fifo_empty(esai_fifo_node->esai_regs)) {
                ESAI_FIFO_DEBUG("[ESAI] TX FIFO empty!!!, tx_packet_count=%u,"
                    "tx_zero_packet_index=%u, tx_zero_packet_count=%u,\n",
                esai_fifo_node->tx_packet_count,
                esai_fifo_node->tx_zero_packet_index,
                esai_fifo_node->tx_zero_packet_count);
                esai_fifo_node->tx_underrun++;

            }

            ret = dma_transfer_submit(esai_fifo_node->tx_dma_channel,
                            &esai_fifo_node->tx_dma_zero_packets[out_temp],
                            NULL);
            if(ESAI_NODE_OK != ret) {
                ESAI_FIFO_ERROR("[ESAI] dma push failed ret :%d\n");
                ESAI_FIFO_DEBUG("[ESAI] out_temp = %d\n",out_temp);
                ESAI_FIFO_DEBUG("[ESAI] zero packet info, count=%u, src=0x%x,"
                "size_per_request=%u\n",
                esai_fifo_node->tx_dma_zero_packets[out_temp].LOOP_COUNT,
                esai_fifo_node->tx_dma_zero_packets[out_temp].SRC_ADDR,
                esai_fifo_node->tx_dma_zero_packets[out_temp].LOOP_BYTES);
                return ESAI_NODE_TX_DMA_ADD_PACKET_FAIL;
            }

            esai_fifo_node->tx_zero_packet_count++;
            esai_fifo_node->tx_zero_packet_index = (out_temp + 1)
                % ESAI_TX_DMA_ZERO_PACKET_NUM;
        }
    }

    return ESAI_NODE_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_tx_dma_req
* Returned Value   : uint32_t
* Comments         :
*    push data packets to dma and start dma to send these packets
*    when calling from ISR, the 'int_disable' should set to TRUE
*
*END*********************************************************************/
static uint32_t _esai_fifo_tx_dma_req
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node,

    /* [IN] interrupt disable ? */
    bool int_disable,

    /* [IN] input zero packets ?*/
    bool fill_zero_packet

)
{
    uint32_t ret;

    if (esai_fifo_node->tx_state != ESAI_STATE_RUN)
        return ESAI_NODE_TX_STATE_ERROR;

    if (int_disable)
        _int_disable();

    ret = _esai_fifo_tx_dma_fill_packets(esai_fifo_node, fill_zero_packet);

    ret = dma_request_enable(esai_fifo_node->tx_dma_channel);
    if (ESAI_NODE_OK != ret) {
        ESAI_FIFO_ERROR("[ESAI] edma_channel_start error %d\n", ret);
    }

    if (int_disable)
        _int_enable();

    return ret;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_tx_dma_callback
* Returned Value   : void
* Comments         :
*    TX DMA callback function to deal with DMA message
*
*END*********************************************************************/

static void _esai_fifo_tx_dma_callback
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR    esai_fifo_node
)
{
    uint32_t ret = 0;

    if (esai_fifo_node->tx_dma_channel == NULL)
        return;

    if (esai_fifo_node->tx_state != ESAI_STATE_RUN)
        return;

    /*check the dma callback messages*/
    if (ESAI_TX_PLAY_ZERO_FILLED == esai_fifo_node->tx_play_state) {

        esai_fifo_node->tx_zero_packet_count--;

        /*check if new available packets arrived*/
        if (0 != esai_fifo_node->tx_packet_count) {

            /*fill next availiable data to dma queue*/
            ret = _esai_fifo_tx_dma_fill_packets(esai_fifo_node, FALSE);

            if (0 == ret) {
                /*change state to normal*/
                esai_fifo_node->tx_play_state = ESAI_TX_PLAY_NORMAL;

                /*if lower than threshold, callback to demand more data*/
                if (esai_fifo_node->tx_packet_demand_threshold >=
                    esai_fifo_node->tx_packet_count) {

                    if (esai_fifo_node->callback_func_ptr) {
                        (*esai_fifo_node->callback_func_ptr)(
                            esai_fifo_node->callback_param,
                            ESAI_FIFO_EVENT_TX_DATA_DEMAND,
                            NULL);
                    }
                }
            }
        } else {

            /*fill zero packets*/
            _esai_fifo_tx_dma_fill_packets(esai_fifo_node, TRUE);

            /*callback to demand more data*/
            if (esai_fifo_node->callback_func_ptr) {
                (*esai_fifo_node->callback_func_ptr)(
                    esai_fifo_node->callback_param,
                    ESAI_FIFO_EVENT_TX_DATA_DEMAND,
                    NULL);
            }
        }
    } else if (ESAI_TX_PLAY_NORMAL == esai_fifo_node->tx_play_state) {

        if (0 < esai_fifo_node->tx_zero_packet_count) {
            esai_fifo_node->tx_zero_packet_count--;

        } else if (0 < esai_fifo_node->tx_packet_count) {
            /*decrease the the tx_packet_count, so that buffer can be free*/
            esai_fifo_node->tx_packet_count--;

            /*post the semaphore*/
            _lwsem_post(&esai_fifo_node->tx_sem);

            /*fill next availiable data to dma queue*/
            _esai_fifo_tx_dma_fill_packets(esai_fifo_node, FALSE);

            if (esai_fifo_node->tx_packet_demand_threshold >=
                esai_fifo_node->tx_packet_count) {
                if (esai_fifo_node->callback_func_ptr) {
                    (*esai_fifo_node->callback_func_ptr)(
                        esai_fifo_node->callback_param,
                        ESAI_FIFO_EVENT_TX_DATA_DEMAND,
                        NULL);
                }
            }

        } else if (0 < esai_fifo_node->tx_co_data_count) {
            esai_fifo_node->tx_co_data_count--;

            ESAI_INC_BUF(esai_fifo_node->tx_out,
                esai_fifo_node->tx_packet_num);

            if (esai_fifo_node->callback_func_ptr) {
                (*esai_fifo_node->callback_func_ptr)(
                    esai_fifo_node->callback_param,
                    ESAI_FIFO_EVENT_TX_DATA_TRANSFERED,
                    NULL);
            }

            if (esai_hw_tx_fifo_empty(esai_fifo_node->esai_regs)) {
                ESAI_FIFO_ERROR("[ESAI] TX FIFO empty after"
                    "transfered event!!!\n");
            }

            if (esai_fifo_node->tx_packet_demand_threshold >=
                esai_fifo_node->tx_co_data_count) {
                if (esai_fifo_node->callback_func_ptr) {
                    (*esai_fifo_node->callback_func_ptr)(
                        esai_fifo_node->callback_param,
                        ESAI_FIFO_EVENT_TX_DATA_DEMAND,
                        NULL);
                }

                if (esai_hw_tx_fifo_empty(esai_fifo_node->esai_regs)) {
                    ESAI_FIFO_ERROR("[ESAI] TX FIFO empty after "
                        "demand event!!!\n");
                    ESAI_FIFO_ERROR("tx_co_data_count = %d, "
                        "tx_packet_count = %d\n",
                        esai_fifo_node->tx_co_data_count,
                        esai_fifo_node->tx_packet_count);
                }
            }
        }
    }

}

static void _esai_fifo_tx_dma_callback_w(void *callback_data, int tcds_done, uint32_t tcd_seq)
{
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node =
            (ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR)callback_data;
    _esai_fifo_tx_dma_callback(esai_fifo_node);
}


/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_rx_buffer_empty
* Returned Value   : bool
* Comments         :
*    This function returns TRUE, if the rx buffer is empty,
*    returns FALSE, if the rx buffer is not empty
*
*END*********************************************************************/
static bool _esai_fifo_rx_buffer_empty
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node
)
{
    if (esai_fifo_node->rx_packet_count)
        return FALSE;

    return TRUE;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_rx_buffer_full
* Returned Value   : bool
* Comments         :
*    This function returns TRUE, if the rx buffer is full,
*    returns FALSE, if the rx buffer is not full
*
*END*********************************************************************/
static bool _esai_fifo_rx_buffer_full
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node
)
{
    if (esai_fifo_node->rx_packet_count >= esai_fifo_node->rx_packet_num)
        return TRUE;

    return FALSE;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_rx_dma_req
* Returned Value   : uint32_t
* Comments         :
*    push data packets to dma and start dma to send these packets
*    when calling from ISR, the 'int_disable' should set to TRUE
*
*END*********************************************************************/
static uint32_t _esai_fifo_rx_dma_req
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node,

    /* [IN] interrupt disable ? */
    bool int_disable
)
{
    uint32_t ret=0;

    if (esai_fifo_node->rx_state != ESAI_STATE_RUN)
        return ESAI_NODE_TX_STATE_ERROR;

    if (int_disable)
        _int_disable();

    esai_fifo_node->rx_dma_packets[esai_fifo_node->rx_in].LOOP_BYTES =
                                            esai_fifo_node->rx_size_per_request;
    esai_fifo_node->rx_dma_packets[esai_fifo_node->rx_in].LOOP_COUNT=
            esai_fifo_node->rx_packet_size/esai_fifo_node->rx_size_per_request;

    dma_transfer_submit(esai_fifo_node->rx_dma_channel,
                        &esai_fifo_node->rx_dma_packets[esai_fifo_node->rx_in],
                        NULL);
    ret = dma_request_enable(esai_fifo_node->rx_dma_channel);
    if (MQX_OK != ret) {
        ESAI_FIFO_ERROR("[ESAI] edma_channel_start error %d\n", ret);
    }

    if (int_disable)
        _int_enable();

    return ret;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_rx_dma_callback
* Returned Value   : void
* Comments         :
*    RX DMA callback function to deal with DMA message
*
*END*********************************************************************/
static void _esai_fifo_rx_dma_callback
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR    esai_fifo_node
)
{
    int32_t ret = 0;
    uint8_t in;

    if (esai_fifo_node->rx_dma_channel == NULL)
        return;

    if (esai_fifo_node->rx_state != ESAI_STATE_RUN)
        return;

    /*check the dma callback messages*/
    esai_fifo_node->rx_packet_count++;
    in = esai_fifo_node->rx_in;

    ESAI_INC_BUF(esai_fifo_node->rx_in,
                 esai_fifo_node->rx_packet_num);

    if (_esai_fifo_rx_buffer_full(esai_fifo_node)) {
        esai_fifo_node->rx_buf_overrun_count++;
        esai_fifo_node->rx_out = esai_fifo_node->rx_in;
        ESAI_INC_BUF(esai_fifo_node->rx_out,
                 esai_fifo_node->rx_packet_num);

        esai_fifo_node->rx_ready = esai_fifo_node->rx_in;
        ESAI_INC_BUF(esai_fifo_node->rx_ready,
                 esai_fifo_node->rx_packet_num);

        esai_fifo_node->rx_packet_count--;
    }

    /*post the semaphore*/
    _lwsem_post(&esai_fifo_node->rx_sem);

    in = esai_fifo_node->rx_in;
    esai_fifo_node->rx_dma_packets[in].LOOP_BYTES
                            = esai_fifo_node->rx_size_per_request;

    esai_fifo_node->rx_dma_packets[in].LOOP_COUNT =
        esai_fifo_node->rx_packet_size
        / esai_fifo_node->rx_size_per_request;

    ret = dma_transfer_submit(esai_fifo_node->rx_dma_channel,
            &esai_fifo_node->rx_dma_packets[in],
            NULL);
    if(ESAI_NODE_OK != ret) {
        ESAI_FIFO_ERROR("[ESAI] rx dma push failed :%d, rx_in=%d\n",
                                    ret, in);
        return;
    }

}

static void _esai_fifo_rx_dma_callback_w(void *callback_data, int tcds_done, uint32_t tcd_seq)
{
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node =
            (ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR)callback_data;
    _esai_fifo_rx_dma_callback(esai_fifo_node);
}


/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_map_slotword_value
* Returned Value   : uint32_t
* Comments         :
*    accroding with the slot width and word width to select register value
*
*END*********************************************************************/
static uint32_t _esai_fifo_map_slotword_value
(
    /* [IN] slot width */
    ESAI_MODULE_SLOT_WIDTH slot_width,
    /* [IN] word length */
    ESAI_NODE_FIFO_FMT word_width,
    /* [OUT] the value for the slot and word width */
    uint32_t *slot_word_length
)
{
    uint32_t ret = ESAI_NODE_OK;

    switch (slot_width) {
        case ESAI_MODULE_SW_12BIT:
            if (word_width == ESAI_FIFO_DATA_8BIT)
                *slot_word_length = ESAI_TCR_TSWS_STL12_WDL8;
            else
                ret = ESAI_NODE_TX_CONFIG_FMT_PARA_ERROR;

            break;
        case ESAI_MODULE_SW_16BIT:

            if (word_width == ESAI_FIFO_DATA_8BIT)
                *slot_word_length = ESAI_TCR_TSWS_STL16_WDL8;
            else if (word_width == ESAI_FIFO_DATA_16BIT)
                *slot_word_length = ESAI_TCR_TSWS_STL16_WDL16;
            else
                ret = ESAI_NODE_TX_CONFIG_FMT_PARA_ERROR;

            break;

        case ESAI_MODULE_SW_20BIT:

            if (word_width == ESAI_FIFO_DATA_8BIT)
                *slot_word_length = ESAI_TCR_TSWS_STL20_WDL8;
            else if (word_width == ESAI_FIFO_DATA_16BIT)
                *slot_word_length = ESAI_TCR_TSWS_STL20_WDL16;
            else
                ret = ESAI_NODE_TX_CONFIG_FMT_PARA_ERROR;

            break;

        case ESAI_MODULE_SW_24BIT:

            if (word_width == ESAI_FIFO_DATA_8BIT)
                *slot_word_length = ESAI_TCR_TSWS_STL24_WDL8;
            else if (word_width == ESAI_FIFO_DATA_16BIT)
                *slot_word_length = ESAI_TCR_TSWS_STL24_WDL16;
            else if (word_width == ESAI_FIFO_DATA_24BIT)
                *slot_word_length = ESAI_TCR_TSWS_STL24_WDL24;
            else
                ret = ESAI_NODE_TX_CONFIG_FMT_PARA_ERROR;

            break;

        case ESAI_MODULE_SW_32BIT:

            if (word_width == ESAI_FIFO_DATA_8BIT)
                *slot_word_length = ESAI_TCR_TSWS_STL32_WDL8;
            else if (word_width == ESAI_FIFO_DATA_16BIT)
                *slot_word_length = ESAI_TCR_TSWS_STL32_WDL16;
            else if (word_width == ESAI_FIFO_DATA_24BIT)
                *slot_word_length = ESAI_TCR_TSWS_STL32_WDL24;
            else if (word_width == ESAI_FIFO_DATA_32BIT)
                *slot_word_length = ESAI_TCR_TSWS_STL32_WDL24;
            else
                ret = ESAI_NODE_TX_CONFIG_FMT_PARA_ERROR;

            break;

        default:
            ret = ESAI_NODE_TX_CONFIG_FMT_PARA_ERROR;
            break;
    }

    return ret;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_calc_sck_value
* Returned Value   : uint32_t
* Comments         :
*    return the serial bit clock rate
*
*END*********************************************************************/
static uint32_t _esai_fifo_calc_sck_value
(
    /* [IN] frame rate*/
    uint32_t frame_rate,
    /* [IN] slot width*/
    ESAI_MODULE_SLOT_WIDTH slot_width,
    /* [IN] slots number*/
    uint32_t slot_num,
    /* [OUT] slots rate*/
    uint32_t *slot_rate
)
{
    uint32_t sw = 0;
    *slot_rate = (uint32_t)(frame_rate * slot_num);

    switch (slot_width) {
        case ESAI_MODULE_SW_12BIT:
            sw = 12;
            break;
        case ESAI_MODULE_SW_16BIT:
            sw = 16;
            break;
        case ESAI_MODULE_SW_20BIT:
            sw = 20;
            break;
        case ESAI_MODULE_SW_24BIT:
            sw = 24;
            break;
        case ESAI_MODULE_SW_32BIT:
            sw = 32;
            break;
        default:
            sw = 32;
            break;
    }

    return ((*slot_rate) * sw);
}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_isr
* Returned Value   : void
* Comments         :
*    ESAI interrupt service function, not register now
*
*END*********************************************************************/
static void _esai_fifo_isr
(
    /* [IN] the parameter in register the ISR function */
    void * parameter
)
{
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node =
        (ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR)parameter;

    bool val = FALSE;
    (void)val; /* disable 'unused variable' warning */

    if (NULL == esai_fifo_node || ESAI_STATE_RUN != esai_fifo_node->tx_state)
        return;


    if (esai_hw_tx_fifo_underrun(esai_fifo_node->esai_regs)) {
        val = esai_hw_tx_fifo_empty(esai_fifo_node->esai_regs);
        ESAI_FIFO_DEBUG("!!!!tx_fifo_underrun, tx fifo empty = %d\n", val);
        esai_fifo_node->tx_underrun++;
    } else if (esai_hw_rx_fifo_overrun(esai_fifo_node->esai_regs)) {
        val = esai_hw_rx_fifo_full(esai_fifo_node->esai_regs);
        ESAI_FIFO_DEBUG("!!!!rx_fifo_overrun, rx fifo full = %d\n", val);
    }
}

#ifdef __ESAI_24BIT_DMA_UNSUPPORT

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_24_to_32_extension
* Returned Value   : uint32_t
* Comments         :
*    Extend 24bit samples to 32bit samples
*
*END*********************************************************************/
static uint32_t _esai_fifo_24_to_32_extension
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node,

    /* [IN] 24bit sample's buffer*/
    void* buffer,

    /* [IN] 24bit sample's buffer length */
    uint32_t length
)
{
    uint32_t buffer_length = (length / 3) << 2;
    uint8_t *temp = (uint8_t*)buffer;
    uint32_t i, j, data_bytes, data2, data;
    uint32_t *destination_buf;

#ifdef __ESAI_24_TO_32_PERF_MEASURE
    uint32_t startns, endns;
#endif

    if (length % 3) {
        /*length should align with 3 bytes*/
        length -= (length %3);
        esai_fifo_node->tx_error_code = IO_ERROR_NOTALIGNED;
    }

    if (esai_fifo_node->padding_buffer != NULL &&
        buffer_length <= esai_fifo_node->padding_buf_length ) {
        esai_fifo_node->padding_buf_data_length = buffer_length;
    } else {
        if (esai_fifo_node->padding_buffer != NULL) {
            _mem_free(esai_fifo_node->padding_buffer);
        }

        esai_fifo_node->padding_buffer = _mem_alloc_system_zero(buffer_length);
        if (esai_fifo_node->padding_buffer)
            _mem_set_type(esai_fifo_node->padding_buffer,
            MEM_TYPE_IO_ESAI_PADDING_BUFFER);
        else
            return ESAI_NODE_ALLOC_MEMORY_ERROR;
        esai_fifo_node->padding_buf_length = buffer_length;
        esai_fifo_node->padding_buf_data_length = buffer_length;
    }

    data_bytes = 3;
    destination_buf = (uint32_t *)esai_fifo_node->padding_buffer;

#ifdef __ESAI_24_TO_32_PERF_MEASURE
    startns = _time_get_nanoseconds();
#endif

    for (i=0; i<length; i+=data_bytes)
    {
        data = 0;
        for (j=0;j<data_bytes;j++) {
            data2 = *(temp+i+j);
            data |= (data2 << (j<<3));
        }
        *destination_buf = data;
        destination_buf++;
    }

#ifdef __ESAI_24_TO_32_PERF_MEASURE
    endns = _time_get_nanoseconds();
    ESAI_FIFO_DEBUG("\nTime nanoseconds is endns=%ld, startns=%ld\n",
        endns, startns);
#endif

    return ESAI_NODE_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_32_to_24
* Returned Value   : uint32_t
* Comments         :
*    Convert 32bit samples to 24bit samples
*
*END*********************************************************************/
static uint32_t _esai_fifo_32_to_24
(
    /* [IN] 32bit sample's buffer*/
    void* src,

    /* [IN] 32bit sample's buffer length */
    uint32_t src_len,

    /* [IN] 24bit sample's buffer*/
    void* dst
)
{
    uint8_t *src_temp = (uint8_t*)src;
    uint8_t *dst_temp = (uint8_t*)dst;
    uint32_t i = 0, j = 0;

#ifdef __ESAI_32_TO_24_PERF_MEASURE
    uint32_t startns, endns;
#endif

#ifdef __ESAI_32_TO_24_PERF_MEASURE
    startns = _time_get_nanoseconds();
#endif

    j = 0;
    for (i=0; i<src_len; i++) {
        if ((i + 1) % 4) {
            dst_temp[j] = src_temp[i];
            j++;
        }
    }

#ifdef __ESAI_32_TO_24_PERF_MEASURE
    endns = _time_get_nanoseconds();
    ESAI_FIFO_DEBUG("\n 32 to 24 time : %d\n",(endns-startns));
#endif

    return j;
}


#endif


/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_tx_write_buffer_install
* Returned Value   : return 0 if success, else return uint32_t error code
* Comments         :
*    This function allocates and initializes the memory for tx buffer
*
*END*********************************************************************/
static uint32_t _esai_fifo_tx_write_buffer_install
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node
)
{
    uint8_t i = 0;

    if (NULL == esai_fifo_node->tx_buffer) {
        esai_fifo_node->tx_buffer   =
             (void *)_mem_alloc_system_uncached(esai_fifo_node->tx_packet_size
              * esai_fifo_node->tx_packet_num);
        if (NULL == esai_fifo_node->tx_buffer)
        {
            ESAI_FIFO_ERROR("[ESAI] out of memory for tx\n");
            return ESAI_NODE_ALLOC_MEMORY_ERROR;
        }

        _mem_set_type (esai_fifo_node->tx_buffer,
            MEM_TYPE_IO_ESAI_FIFO_NODE_TX_BUF);

        for (i=0; i<esai_fifo_node->tx_packet_num; i++) {
            esai_fifo_node->tx_dma_packets[i].SRC_ADDR =
                ((uint32_t)esai_fifo_node->tx_buffer)
                + (i*esai_fifo_node->tx_packet_size);
            ESAI_FIFO_DEBUG("[ESAI] dma packet buffer [%d] = 0x%x\n",
                i, esai_fifo_node->tx_dma_packets[i].SRC_ADDR);
        }

    }

    return ESAI_NODE_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_rx_read_buffer_install
* Returned Value   : return 0 if success, else return uint32_t error code
* Comments         :
*    This function allocates and initializes the memory for rx buffer
*
*END*********************************************************************/
static uint32_t _esai_fifo_rx_read_buffer_install
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node
)
{
    uint8_t i = 0;

    if (NULL == esai_fifo_node->rx_buffer) {
        esai_fifo_node->rx_buffer   =
            (void *)_mem_alloc_system_uncached(esai_fifo_node->rx_packet_size
             * esai_fifo_node->rx_packet_num);
        if (NULL == esai_fifo_node->rx_buffer)
        {
            ESAI_FIFO_ERROR("[ESAI] out of memory for rx\n");
            return ESAI_NODE_ALLOC_MEMORY_ERROR;
        }

        _mem_set_type (esai_fifo_node->rx_buffer,
            MEM_TYPE_IO_ESAI_FIFO_NODE_RX_BUF);


        for (i=0; i<esai_fifo_node->rx_packet_num; i++) {
            esai_fifo_node->rx_dma_packets[i].DST_ADDR =
                                          ((uint32_t)esai_fifo_node->rx_buffer)
                                          + (i*esai_fifo_node->rx_packet_size);
            ESAI_FIFO_DEBUG("[ESAI] dma rx packet buffer [%d] = 0x%x\n",
                                i, esai_fifo_node->rx_dma_packets[i].SRC_ADDR);
        }
    }

    return ESAI_NODE_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_tx_buflist_init
* Returned Value   : return 0 if success, else return uint32_t error code
* Comments         :
*    This function initializes a list to manage the buffers used in directly commiting mode
*
*END*********************************************************************/
static uint32_t _esai_fifo_tx_buflist_init
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node
)
{
    esai_fifo_node->tx_buflist = NULL;
    esai_fifo_node->tx_buflist_bytes = 0;
    esai_fifo_node->tx_buflist_entries = 0;

    return ESAI_NODE_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_tx_buflist_append
* Returned Value   : return 0 if success, else return uint32_t error code
* Comments         :
*    This function is used to link a buffer to the manage list
*
*END*********************************************************************/
uint32_t _esai_fifo_tx_buflist_append
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node,

    /* [IN] the linked buffer's address*/
    void * new_buffer,

    /* [IN] the linked buffer's size*/
    uint32_t new_buf_size
)
{
    ESAI_FIFO_NODE_TX_BUFFER_PTR temp_ptr;
    ESAI_FIFO_NODE_TX_BUFFER_PTR new_ptr;

    if (ESAI_TX_BUFLIST_MAX_ENTRIES == (esai_fifo_node->tx_buflist_entries+1)) {
        return ESAI_NODE_OUT_OF_RESOURCE_LIMIT;
    }

    if (ESAI_TX_BUFLIST_MAX_BYTES <
        (esai_fifo_node->tx_buflist_bytes + new_buf_size)) {
        return ESAI_NODE_OUT_OF_RESOURCE_LIMIT;
    }

    new_ptr = (ESAI_FIFO_NODE_TX_BUFFER_PTR)
        _mem_alloc_system_zero(sizeof(ESAI_FIFO_NODE_TX_BUFFER));

    if (NULL == new_ptr) {
        return ESAI_NODE_ALLOC_MEMORY_ERROR;
    }

    _mem_set_type (new_ptr, MEM_TYPE_IO_ESAI_FIFO_NODE_TX_BUF);

    new_ptr->tx_buffer_length = new_buf_size;
    new_ptr->next_tx_buffer = NULL;
    new_ptr->tx_buffer_ptr = new_buffer;
    new_ptr->tx_buffer_status = 0;

    if (NULL == esai_fifo_node->tx_buflist) {
        esai_fifo_node->tx_buflist = new_ptr;

    } else {
        temp_ptr = esai_fifo_node->tx_buflist;
        while (temp_ptr->next_tx_buffer != NULL) {
            temp_ptr = temp_ptr->next_tx_buffer;
        }

        temp_ptr->next_tx_buffer = new_ptr;
    }

    esai_fifo_node->tx_buflist_bytes += new_ptr->tx_buffer_length;
    esai_fifo_node->tx_buflist_entries++;

    return ESAI_NODE_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_tx_buflist_remove
* Returned Value   : return 0 if success, else return uint32_t error code
* Comments         :
*    This function is used to remove a buffer from the manage list
*
*END*********************************************************************/
uint32_t _esai_fifo_tx_buflist_remove
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node,

    /* [IN] the removed buffer's address*/
    void * buffer_addr
)
{
    ESAI_FIFO_NODE_TX_BUFFER_PTR tx_buf_ptr = NULL;
    ESAI_FIFO_NODE_TX_BUFFER_PTR prev_tx_buf_ptr = NULL;

    tx_buf_ptr = esai_fifo_node->tx_buflist;
    prev_tx_buf_ptr = NULL;

    while (NULL != tx_buf_ptr) {
        if (tx_buf_ptr->tx_buffer_ptr == buffer_addr) {

            if (NULL == prev_tx_buf_ptr) {
                esai_fifo_node->tx_buflist = tx_buf_ptr->next_tx_buffer;
            } else {
                prev_tx_buf_ptr->next_tx_buffer = tx_buf_ptr->next_tx_buffer;
            }
            esai_fifo_node->tx_buflist_bytes -= tx_buf_ptr->tx_buffer_length;
            esai_fifo_node->tx_buflist_entries -= 1;
            _mem_free(tx_buf_ptr);
            return ESAI_NODE_OK;

        }
        prev_tx_buf_ptr = tx_buf_ptr;
        tx_buf_ptr = tx_buf_ptr->next_tx_buffer;
    }

    return ESAI_NODE_INVALID_PARAMETER; /*fail to find*/
}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_tx_buflist_pop
* Returned Value   : return 0 if success, else return uint32_t error code
* Comments         :
*    This function returns the first buffer in the list. The buffer also be removed from the list
*END*********************************************************************/
static uint32_t _esai_fifo_tx_buflist_pop
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node,

    /* [OUT] The first buffer's address*/
    void * *tx_buf_pptr,

    /* [OUT] The first buffer's size*/
    uint32_t *buf_size

)
{
    ESAI_FIFO_NODE_TX_BUFFER_PTR tx_buf_ptr = NULL;

    if (NULL == esai_fifo_node->tx_buflist) {
        *tx_buf_pptr = NULL;
        *buf_size = 0;

        return ESAI_NODE_OK;
    }

    tx_buf_ptr = esai_fifo_node->tx_buflist;
    *tx_buf_pptr = tx_buf_ptr->tx_buffer_ptr;
    *buf_size = tx_buf_ptr->tx_buffer_length;

    esai_fifo_node->tx_buflist_bytes -= tx_buf_ptr->tx_buffer_length;
    esai_fifo_node->tx_buflist_entries -= 1;
    esai_fifo_node->tx_buflist = tx_buf_ptr->next_tx_buffer;

    _mem_free(tx_buf_ptr);

    return ESAI_NODE_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_tx_buflist_deinit
* Returned Value   : return 0 if success, else return uint32_t error code
* Comments         :
*    This function destroys all the buffers and the manage list
*
*END*********************************************************************/
static uint32_t _esai_fifo_tx_buflist_deinit
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node
)
{
    void * tx_buf_ptr = NULL;
    uint32_t buf_size = 0;

    if (NULL == esai_fifo_node->tx_buflist) {
        return ESAI_NODE_OK;
    }

    _esai_fifo_tx_buflist_pop(esai_fifo_node, &tx_buf_ptr, &buf_size);
    while (NULL != tx_buf_ptr) {
        _mem_free(tx_buf_ptr);

        tx_buf_ptr = NULL;
        _esai_fifo_tx_buflist_pop(esai_fifo_node, &tx_buf_ptr, &buf_size);
    }

    return ESAI_NODE_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_delay
* Returned Value   : void
* Comments         :
*    A internal delay loop without sleep
*
*END*********************************************************************/
static void _esai_delay
(
    uint32_t loop_len
)
{
    volatile uint32_t loop_nop = 0;
    while(loop_len--)
    {
        loop_nop = 1000;
        while (loop_nop--)
        {
             __asm("nop");
        }
    }
}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_clk_debug
* Returned Value   : void
* Comments         :
*    A internal debug function to print detail of clock settings
*
*END*********************************************************************/
static void _esai_fifo_clk_debug
(
    ESAI_HW_CLK_SETTING_STRUCT_PTR clk_setting
)
{
    ESAI_FIFO_DEBUG("[ESAI] clock setting\n");
    ESAI_FIFO_DEBUG("dc_val = %u\n", clk_setting->dc_val);
    ESAI_FIFO_DEBUG("fp_val = %u\n", clk_setting->fp_val);
    ESAI_FIFO_DEBUG("fst_dir = %u\n", clk_setting->fst_dir);
    ESAI_FIFO_DEBUG("hck_dir = %u\n", clk_setting->hck_dir);
    ESAI_FIFO_DEBUG("pm_val = %u\n", clk_setting->pm_val);
    ESAI_FIFO_DEBUG("psr_val = %u\n", clk_setting->psr_val);
    ESAI_FIFO_DEBUG("sck_dir = %u\n", clk_setting->sck_dir);
    ESAI_FIFO_DEBUG("sclk_polar = %u\n", clk_setting->sclk_polar);
    ESAI_FIFO_DEBUG("source_extal = %u\n", clk_setting->source_extal);
    ESAI_FIFO_DEBUG("sync_polar = %u\n", clk_setting->sync_polar);
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_start_section_rx
* Returned Value   : uint32_t
* Comments         :
*    Start the ESAI TX hardware, DMA channels and installed ASRC to run
*
*END*********************************************************************/
static uint32_t _esai_fifo_node_start_section_tx
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node
)
{
    uint8_t i = 0;
    uint8_t tr_mask = 0;
    uint8_t tr_enable_num = 0;
    uint8_t sample_width = 4;

    uint32_t dw;
    ASRC_MemMapPtr asrc_regs = ASRC_BASE_PTR;
    uint32_t ret = MQX_OK;


    /*start the TX*/
    if (esai_fifo_node->tx_state != ESAI_STATE_STOP
        && esai_fifo_node->tx_state != ESAI_STATE_RESET) {
        return ESAI_NODE_TX_STATE_ERROR;
    }

    ESAI_FIFO_DEBUG("[ESAI] TX Start req, data_fmt = %u\n",
        esai_fifo_node->tx_fifo_data_fmt);

    /*setup dma channel*/
    switch (esai_fifo_node->tx_fifo_data_fmt) {
        case ESAI_FIFO_DATA_8BIT:

            dw = 1;
            sample_width = 1;
            break;
        case ESAI_FIFO_DATA_16BIT:
            if (esai_fifo_node->tx_asrc_8bit_data &&
                esai_fifo_node->asrc_enable) {

                dw = 1;
                sample_width = 1;
            } else {

                dw = 2;
                sample_width = 2;
            }
            break;
        case ESAI_FIFO_DATA_24BIT:
#ifdef __ESAI_24BIT_DMA_UNSUPPORT
            dw = 4;
            sample_width = 4;
#else
            /*not support*/
            ESAI_FIFO_ERROR("[ESAI] EDMA not support 24bits\n");
            return ESAI_NODE_TX_CONFIG_FMT_PARA_ERROR;

#endif
            break;
        case ESAI_FIFO_DATA_32BIT:
            dw = 4;
            sample_width = 4;
            break;
        default:
            /*error*/
            return ESAI_NODE_TX_CONFIG_FMT_PARA_ERROR;
    }

    /*reset the tx_in and tx_out pointers*/
    esai_fifo_node->tx_in = 0;
    esai_fifo_node->tx_out = 0;
    esai_fifo_node->tx_packet_count = 0;
    esai_fifo_node->tx_buffer_window = 0;
    esai_fifo_node->tx_co_data_count = 0;
    esai_fifo_node->tx_zero_packet_count = 0;
    esai_fifo_node->tx_size_per_request = 0x20 * sample_width;

    for (i=0; i<esai_fifo_node->tx_packet_num; i++) {
        esai_fifo_node->tx_dma_packets[i].LOOP_COUNT = 0;
    }

    tr_mask = (esai_fifo_node->tr_enable_save
        & esai_fifo_node->tr_tx_dir_save);

    for (i=0; i<ESAI_MAX_TR_NUM; i++) {
        if ((tr_mask >> i) & 0x1)
            tr_enable_num++;
    }

    /*init the zero packets*/
    for (i=0; i<ESAI_TX_DMA_ZERO_PACKET_NUM; i++) {

        ESAI_FIFO_DEBUG("zero packet src = 0x%x\n",
                        esai_fifo_node->tx_dma_zero_packets[i].SRC_ADDR);
        esai_fifo_node->tx_dma_zero_packets[i].LOOP_COUNT = (tr_enable_num << 1) * 10;
        esai_fifo_node->tx_dma_zero_packets[i].LOOP_BYTES =
            esai_fifo_node->tx_size_per_request;

        //Need to fill the zero packets.

        if (esai_fifo_node->asrc_enable)
        {
           esai_fifo_node->tx_dma_zero_packets[i].DST_ADDR =
                (uint32_t)((uint32_t *)&(asrc_regs->ASRDI[0]));
        }
        else
        {
            esai_fifo_node->tx_dma_zero_packets[i].DST_ADDR =
                (uint32_t) &(esai_fifo_node->esai_regs->ETDR);
        }
        esai_fifo_node->tx_dma_zero_packets[i].DST_MODULO = 0;
        esai_fifo_node->tx_dma_zero_packets[i].DST_OFFSET = 0;
        esai_fifo_node->tx_dma_zero_packets[i].DST_WIDTH = dw;
        esai_fifo_node->tx_dma_zero_packets[i].SRC_MODULO = 0;
        esai_fifo_node->tx_dma_zero_packets[i].SRC_OFFSET = dw;
        esai_fifo_node->tx_dma_zero_packets[i].SRC_WIDTH = dw;


    }

    if (esai_fifo_node->asrc_enable) {

        for (i = 0; i < ESAI_MAX_TX_DMA_PACKET_NUM; i++)
        {
            esai_fifo_node->tx_dma_packets[i].DST_ADDR =
                        (uint32_t)((uint32_t *)&(asrc_regs->ASRDI[0]));
            esai_fifo_node->tx_dma_packets[i].DST_MODULO = 0;
            esai_fifo_node->tx_dma_packets[i].DST_OFFSET = 0;
            esai_fifo_node->tx_dma_packets[i].DST_WIDTH = dw;
            esai_fifo_node->tx_dma_packets[i].SRC_MODULO = 0;
            esai_fifo_node->tx_dma_packets[i].SRC_OFFSET = dw;
            esai_fifo_node->tx_dma_packets[i].SRC_WIDTH = dw;
        }

    } else {

        for (i = 0; i < ESAI_MAX_TX_DMA_PACKET_NUM; i++)
        {

            esai_fifo_node->tx_dma_packets[i].DST_ADDR =
                        (uint32_t)&(esai_fifo_node->esai_regs->ETDR);
            esai_fifo_node->tx_dma_packets[i].DST_MODULO = 0;
            esai_fifo_node->tx_dma_packets[i].DST_OFFSET = 0;
            esai_fifo_node->tx_dma_packets[i].DST_WIDTH = dw;
            esai_fifo_node->tx_dma_packets[i].SRC_MODULO = 0;
            esai_fifo_node->tx_dma_packets[i].SRC_OFFSET = dw;
            esai_fifo_node->tx_dma_packets[i].SRC_WIDTH = dw;
        }

    }

    /*config and enable transmit including tx FIFO,
            transmitters and transmit section*/
    esai_hw_tx_fifo_enable(esai_fifo_node->esai_regs,
        tr_mask,
        esai_fifo_node->tx_fifo_wm,
        esai_fifo_node->tx_fifo_wa);

    /*let the esai tx section out of reset, so esai output clocks (serial and sync) can
              start to work, the serial clock is needed by ASRC to convert sample rate*/
    esai_hw_tx_out_of_reset(esai_fifo_node->esai_regs);

    ESAI_FIFO_DEBUG("[ESAI] TX HW Started, tr_mask = 0x%x\n", tr_mask);

    esai_fifo_node->tx_state = ESAI_STATE_RUN;
    esai_fifo_node->tx_play_state = ESAI_TX_PLAY_REQ;

    ESAI_FIFO_DEBUG("[ESAI] Start the asrc P2P then, tr_mask = 0x%x\n", tr_mask);

        if (esai_fifo_node->asrc_enable) {


#ifdef ASRC_HARDCODE_VERSION
            asrc_plugin_init();

            if (ESAI_FIFO_DATA_24BIT == esai_fifo_node->tx_fifo_data_fmt) {
                asrc_plugin_config_pair(0, 24, 32,
                        esai_fifo_node->tx_asrc_src_sample_rate);
            } else if (ESAI_FIFO_DATA_16BIT == esai_fifo_node->tx_fifo_data_fmt){
                asrc_plugin_config_pair(0, 16, 32,
                        esai_fifo_node->tx_asrc_src_sample_rate);
            }

#endif

            esai_fifo_node->tx_asrc_dma_packet.SRC_ADDR =
                            (uint32_t)((uint32_t *)&(asrc_regs->OUTPUT.ASRDO[0]));
            esai_fifo_node->tx_asrc_dma_packet.DST_ADDR =
                (uint32_t)&(esai_fifo_node->esai_regs->ETDR);
                    esai_fifo_node->tx_asrc_dma_packet.SRC_WIDTH = 4;
            esai_fifo_node->tx_asrc_dma_packet.SRC_MODULO = 0;
            esai_fifo_node->tx_asrc_dma_packet.SRC_OFFSET = 0;
            esai_fifo_node->tx_asrc_dma_packet.DST_MODULO = 0;
            esai_fifo_node->tx_asrc_dma_packet.DST_OFFSET = 0;
            esai_fifo_node->tx_asrc_dma_packet.DST_WIDTH = 4;
            esai_fifo_node->tx_asrc_dma_packet.LOOP_BYTES = 4;
            esai_fifo_node->tx_asrc_dma_packet.LOOP_COUNT = 0x18;

            ret = dma_transfer_submit(
                esai_fifo_node->tx_asrc_esai_channel,
                &esai_fifo_node->tx_asrc_dma_packet, NULL);
            if (MQX_OK != ret) {
                ESAI_FIFO_ERROR("[ESAI] dma_config failed, "
                    "DMA_DEVICE_ASRCA_ESAI\n");
                return ESAI_NODE_TX_DMA_CONFIG_FAIL;
            }

            ret = dma_request_enable(esai_fifo_node->tx_asrc_esai_channel);
            if (MQX_OK != ret) {
                ESAI_FIFO_ERROR("[ESAI] dma_start failed, "
                    "DMA_DEVICE_ASRCA_ESAI\n");
                return ESAI_NODE_TX_DMA_START_FAIL;
            }

        }


    if (ESAI_TX_PLAY_REQ == esai_fifo_node->tx_play_state) {

        if (esai_fifo_node->asrc_enable) {
#ifdef ASRC_HARDCODE_VERSION
            asrc_plugin_start_pair(0);
#endif
        }

        _int_disable();
        _esai_fifo_tx_dma_req(esai_fifo_node, TRUE, TRUE);
        esai_fifo_node->tx_play_state = ESAI_TX_PLAY_ZERO_FILLED;
        _int_enable();

        /*delay 50ms to wait dma data copying completed*/
        _esai_delay(50);

#ifdef ESAI_ASRC_ENABLE_DEBUG
        asrc_plugin_dump_regs();
#endif

        tr_mask = (esai_fifo_node->tr_enable_save
            & esai_fifo_node->tr_tx_dir_save);

        ESAI_FIFO_DEBUG("[ESAI] enable tx tr_mask = 0x%x\n", tr_mask);
        esai_hw_tx_enable(esai_fifo_node->esai_regs, tr_mask);

#ifdef ESAI_FIFO_NODE_ENABLE_DEBUG
//        esai_hw_tx_enable_interrupts(esai_fifo_node->esai_regs, TRUE);
#endif
    }





    return ESAI_NODE_OK;
}



/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_start_section_rx
* Returned Value   : uint32_t
* Comments         :
*    Start the ESAI RX hardware, DMA channels and installed ASRC to run
*
*END*********************************************************************/
static uint32_t _esai_fifo_node_start_section_rx
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node
)
{

    uint8_t i = 0;
    uint8_t tr_mask = 0, temp_tr_mask;
    uint8_t tr_enable_num = 0;
    uint8_t sample_width = 4;
    uint32_t dw;

    uint32_t ret = ESAI_NODE_OK;


    /*start the RX*/
    if (esai_fifo_node->rx_state != ESAI_STATE_STOP
        && esai_fifo_node->rx_state != ESAI_STATE_RESET) {
        return ESAI_NODE_RX_STATE_ERROR;
    }

    ESAI_FIFO_DEBUG("[ESAI] RX Start req, data_fmt = %u\n",
        esai_fifo_node->rx_fifo_data_fmt);

    /*setup dma channel*/
    switch  (esai_fifo_node->rx_fifo_data_fmt) {
        case ESAI_FIFO_DATA_8BIT:
            dw = 1;
            sample_width = 1;
            break;

        case ESAI_FIFO_DATA_16BIT:
            dw = 2;
            sample_width = 2;
            break;

        case ESAI_FIFO_DATA_24BIT:
#ifdef __ESAI_24BIT_DMA_UNSUPPORT
            dw = 4;
            sample_width = 4;
#else
            /*not support*/
            ESAI_FIFO_ERROR("[ESAI] EDMA not support 24bits\n");
            return ESAI_NODE_RX_CONFIG_FMT_PARA_ERROR;
            sample_width = 3;
#endif
            break;

        case ESAI_FIFO_DATA_32BIT:
            dw = 4;
            sample_width = 4;
            break;

        default:
            /*error*/
            return ESAI_NODE_RX_CONFIG_FMT_PARA_ERROR;
    }

    /*reset the rx_in and rx_out pointers*/
    esai_fifo_node->rx_in = 0;
    esai_fifo_node->rx_out = 0;
    esai_fifo_node->rx_ready = 0;
    esai_fifo_node->rx_buf_overrun_count = 0;
    esai_fifo_node->rx_packet_count = 0;
    esai_fifo_node->rx_size_per_request = 0x20 * sample_width;

    ret = _esai_fifo_rx_read_buffer_install(esai_fifo_node);
    if (ret != ESAI_NODE_OK) {
        return ret;
    }

    for (i=0; i<esai_fifo_node->rx_packet_num; i++) {
        esai_fifo_node->rx_dma_packets[i].LOOP_COUNT = 0;
    }

    for (i = 0; i < ESAI_MAX_RX_DMA_PACKET_NUM; i++)
    {
        esai_fifo_node->rx_dma_packets[i].SRC_ADDR = (uint32_t)&(esai_fifo_node->esai_regs->ERDR);
        esai_fifo_node->rx_dma_packets[i].SRC_OFFSET = 0;
        esai_fifo_node->rx_dma_packets[i].SRC_MODULO = 0;
        esai_fifo_node->rx_dma_packets[i].SRC_WIDTH = dw;
        esai_fifo_node->rx_dma_packets[i].DST_MODULO = 0;
        esai_fifo_node->rx_dma_packets[i].DST_OFFSET = dw;
        esai_fifo_node->rx_dma_packets[i].DST_WIDTH = dw;

    }


    temp_tr_mask = (esai_fifo_node->tr_enable_save
        & esai_fifo_node->tr_rx_dir_save);

    tr_mask = 0;

    for (i=0; i<ESAI_MAX_TR_NUM; i++) {
        if ((temp_tr_mask >> i) & 0x1) {
            tr_enable_num++;
            tr_mask |= (1 << (ESAI_MAX_TR_NUM - 1 - i));
        }
    }

    /*config and enable receive including rx FIFO,
            receiverss and receive section*/
    esai_hw_rx_fifo_enable(esai_fifo_node->esai_regs,
                           tr_mask,
                           esai_fifo_node->rx_fifo_wm,
                           esai_fifo_node->rx_fifo_wa);

    /*let the esai rx section out of reset, so esai output clocks (serial and sync) can
              start to work, the serial clock is needed by ASRC to convert sample rate*/
    esai_hw_rx_out_of_reset(esai_fifo_node->esai_regs);
    esai_fifo_node->rx_state = ESAI_STATE_RUN;

    ESAI_FIFO_DEBUG("[ESAI] RX HW Started, tr_mask = 0x%x\n", tr_mask);

    _esai_fifo_rx_dma_req(esai_fifo_node, TRUE);

    ESAI_FIFO_DEBUG("[ESAI] enable rx tr_mask = 0x%x\n", tr_mask);
    esai_hw_rx_enable(esai_fifo_node->esai_regs, tr_mask);
#ifdef ESAI_FIFO_NODE_ENABLE_DEBUG
    esai_hw_rx_enable_interrupts(esai_fifo_node->esai_regs, TRUE);
#endif

    return ESAI_NODE_OK;

}

/*FUNCTION****************************************************************
*
* Function Name    : _esai_fifo_node_stop_dma
* Returned Value   : uint32_t
* Comments         :
*    A internal function to stop DMA channels for TX or RX
*
*END*********************************************************************/
static void _esai_fifo_node_stop_dma
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node,

    /* [IN] Stop the TX DMA or RX DMA */
    AUD_IO_FW_DIRECTION             dir
)
{
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node = node;
    uint32_t ret = ESAI_NODE_OK;
    int32_t i = 0;

    if (dir == AUD_IO_FW_DIR_TX) {
        if (esai_fifo_node->tx_dma_channel != NULL) {
            ret = dma_request_disable(esai_fifo_node->tx_dma_channel);
            if (MQX_OK != ret) {
                ESAI_FIFO_ERROR("[ESAI] tx edma_channel_stop error %d\n", ret);
            }
            ESAI_FIFO_DEBUG("[ESAI] Stop TX DMA\n");
        }

        /*free the zero packets*/
        for (i=0; i<ESAI_TX_DMA_ZERO_PACKET_NUM; i++) {
            esai_fifo_node->tx_dma_zero_packets[i].LOOP_COUNT = 0;
        }
    }

    if (dir == AUD_IO_FW_DIR_RX) {
        if (esai_fifo_node->rx_dma_channel != NULL) {
            ret = dma_request_disable(esai_fifo_node->rx_dma_channel);
            if (MQX_OK != ret) {
                ESAI_FIFO_ERROR("[ESAI] rx edma_channel_stop error %d\n", ret);
            }

            ESAI_FIFO_DEBUG("[ESAI] Stop RX DMA\n");
        }
    }
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_reg_callback
* Returned Value   : uint32_t
* Comments         :
*    Register a user callback function
*
*END*********************************************************************/
uint32_t esai_fifo_reg_callback
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node,

    /* [IN] user's callback function */
    void (_CODE_PTR_ cb_ptr)(void *, uint32_t, void *),

    /* [IN] args for call back */
    void * args
)
{
    if (NULL == esai_fifo_node)
        return ESAI_NODE_INVALID_PARAMETER;

    if (cb_ptr) {
        esai_fifo_node->callback_func_ptr = cb_ptr;
        esai_fifo_node->callback_param = args;
    } else {
        esai_fifo_node->callback_func_ptr = NULL;
        esai_fifo_node->callback_param = NULL;
    }

    return ESAI_NODE_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_reset
* Returned Value   : uint32_t
* Comments         :
*    call the hw init for ESAI, all settings turn to invalid
*
*END*********************************************************************/
uint32_t esai_fifo_reset
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node
)
{
    if (NULL == esai_fifo_node)
        return ESAI_NODE_INVALID_PARAMETER;

    _mutex_lock(&esai_fifo_node->control_mutex);
    if (esai_fifo_node->tx_state == ESAI_STATE_RUN
        || esai_fifo_node->rx_state == ESAI_STATE_RUN) {
        ESAI_FIFO_DEBUG("[ESAI] Must stop the TX and RX to reset\n");
        _mutex_unlock(&esai_fifo_node->control_mutex);
        return ESAI_NODE_TX_STATE_ERROR;
    }

    esai_hw_init(esai_fifo_node->esai_regs);

    /*Enable syn operating mode*/
    if (TRUE == esai_fifo_node->syn_mode)
        esai_enable_syn(esai_fifo_node->esai_regs);

    esai_fifo_node->tx_state = ESAI_STATE_RESET;
    esai_fifo_node->rx_state = ESAI_STATE_RESET;

    _mutex_unlock(&esai_fifo_node->control_mutex);

    return ESAI_NODE_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_enable_tr
* Returned Value   : uint32_t
* Comments         :
*    enable/disable one tranceiver
*
*END*********************************************************************/
uint32_t esai_fifo_enable_tr
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node,

    /* [IN] tranceiver's index*/
    uint32_t tr_index,

    /* [IN] enable/disable */
    bool enable
)
{
    if (NULL == esai_fifo_node)
        return ESAI_NODE_INVALID_PARAMETER;

    if (ESAI_MAX_TR_NUM <= tr_index)
        return ESAI_NODE_OUT_OF_RESOURCE_LIMIT;

    if (esai_fifo_node->tr_tx_dir_save & (1 << tr_index)) {
        if (esai_fifo_node->tx_state != ESAI_STATE_RESET)
            return ESAI_NODE_TX_STATE_ERROR;

    } else if (esai_fifo_node->tr_rx_dir_save & (1 << tr_index)) {
        if (esai_fifo_node->rx_state != ESAI_STATE_RESET)
            return ESAI_NODE_RX_STATE_ERROR;
    } else {
        /*nothing to do here*/
    }

    if (enable)
        esai_fifo_node->tr_enable_save |= (1 << tr_index);
    else
        esai_fifo_node->tr_enable_save &= ~(1 << tr_index);

    ESAI_FIFO_DEBUG("tr_enable_save = 0x%x\n", esai_fifo_node->tr_enable_save);

    return ESAI_NODE_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_set_dir
* Returned Value   : uint32_t
* Comments         :
*    set the direction (TX/RX) of one tranceiver
*
*END*********************************************************************/
uint32_t esai_fifo_set_dir
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node,

    /* [IN] tranceiver's index*/
    uint32_t tr_index,

    /* [IN] direction for this tranceiver*/
    AUD_IO_FW_DIRECTION dir
)
{
    if (NULL == esai_fifo_node)
        return ESAI_NODE_INVALID_PARAMETER;

    if (ESAI_MAX_TR_NUM <= tr_index)
        return ESAI_NODE_OUT_OF_RESOURCE_LIMIT;

    if (dir == AUD_IO_FW_DIR_TX) {

        if (esai_fifo_node->tx_state != ESAI_STATE_RESET)
            return ESAI_NODE_TX_STATE_ERROR;

        if ((1 << tr_index) & esai_fifo_node->tr_rx_dir_save)
        {
            /*need to change the dir, make sure the rx is stop*/
            if (esai_fifo_node->rx_state != ESAI_STATE_RESET)
                return ESAI_NODE_RX_STATE_ERROR;
        }

        esai_fifo_node->tr_tx_dir_save |= (1 << tr_index);
        esai_fifo_node->tr_rx_dir_save &= ~(1 << tr_index);

    } else if (dir == AUD_IO_FW_DIR_RX) {

        if (esai_fifo_node->rx_state != ESAI_STATE_RESET)
            return ESAI_NODE_RX_STATE_ERROR;

        if ((1 << tr_index) & esai_fifo_node->tr_tx_dir_save)
        {
            /*need to change the dir, make sure the tx is stop*/
            if (esai_fifo_node->tx_state != ESAI_STATE_RESET)
                return ESAI_NODE_TX_STATE_ERROR;
        }

        esai_fifo_node->tr_rx_dir_save |= (1 << tr_index);
        esai_fifo_node->tr_tx_dir_save &= ~(1 << tr_index);

    } else {

        return ESAI_NODE_INVALID_PARAMETER;
    }

    ESAI_FIFO_DEBUG("tr_tx_dir_save = 0x%x, tr_rx_dir_save = 0x%x\n",
        esai_fifo_node->tr_tx_dir_save, esai_fifo_node->tr_rx_dir_save);
    return ESAI_NODE_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_tx_config
* Returned Value   : uint32_t
* Comments         :
*    apply the tx configuration
*
*END*********************************************************************/
uint32_t esai_fifo_node_tx_config
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node,

    /* [IN] pointer to TX configuration*/
    ESAI_FIFO_CONFIG_STRUCT_PTR config
)
{
    uint32_t ret = ESAI_NODE_OK;

    ESAI_HW_CR_SETTING_STRUCT cr_setting;
    ESAI_HW_CLK_SETTING_STRUCT clk_setting;
    ESAI_MODULE_SLOT_WIDTH slot_width;

    ESAI_MODULE_NETWORK_CUSTOM_PARA_PTR custom_para_p = NULL;
    ESAI_MODULE_NORMAL_MODE_PARA_PTR normal_mode_para_p = NULL;

    uint32_t sck = 0, hck = 0, slot_rate, temp;


    if (NULL == esai_fifo_node)
        return ESAI_NODE_INVALID_PARAMETER;

    /*only reset or stop state can apply the configuration*/
    if (esai_fifo_node->tx_state != ESAI_STATE_STOP
        && esai_fifo_node->tx_state != ESAI_STATE_RESET)
        return ESAI_NODE_TX_STATE_ERROR;

    if (config == NULL)
        return ESAI_NODE_INVALID_PARAMETER;

    ESAI_FIFO_DEBUG("[ESAI] esai_fifo_node_tx_config\n");

    /*apply config*/
    switch (config->inf_fmt.fmt) {

        case ESAI_MODULE_FMT_I2S:

            /*slots width*/
            slot_width = config->inf_fmt.fmt_para.stereo_para.slot_width;
            cr_setting.slots_num = 2;

            /*slots mask 0x3, the low two slots enabled*/
            cr_setting.slots_mask = 0x3;

            cr_setting.fsync_slot_length = TRUE;
            cr_setting.fsync_early = TRUE; /*early*/
            cr_setting.left_align  = TRUE; /*left aligned*/
            cr_setting.zero_pad    = TRUE;  /*zero padding*/
            cr_setting.msb_shift   = TRUE; /*MSB shift*/

            cr_setting.mode = ESAI_TCR_TMOD_NETWORK;

            ESAI_FIFO_DEBUG("[ESAI] TX Set ESAI_MODULE_FMT_I2S\n");

            break;
        case ESAI_MODULE_FMT_LEFT_J:

            /*slots width*/
            slot_width = config->inf_fmt.fmt_para.stereo_para.slot_width;
            cr_setting.slots_num = 2;

            /*slots mask 0x3, the low two slots enabled*/
            cr_setting.slots_mask = 0x3;

            cr_setting.fsync_slot_length = TRUE;
            cr_setting.fsync_early = FALSE; /*not early*/
            cr_setting.left_align  = TRUE; /*left aligned*/
            cr_setting.zero_pad    = TRUE;  /*zero padding*/
            cr_setting.msb_shift   = TRUE; /*MSB shift*/

            cr_setting.mode = ESAI_TCR_TMOD_NETWORK;

            ESAI_FIFO_DEBUG("[ESAI] TX Set ESAI_MODULE_FMT_LEFT_J\n");

            break;
        case ESAI_MODULE_FMT_RIGHT_J:

            /*slots width*/
            slot_width = config->inf_fmt.fmt_para.stereo_para.slot_width;
            cr_setting.slots_num = 2;

            /*slots mask 0x3, the low two slots enabled*/
            cr_setting.slots_mask = 0x3;

            cr_setting.fsync_slot_length = TRUE;
            cr_setting.fsync_early = FALSE; /*not early*/
            cr_setting.left_align  = FALSE; /*right aligned*/
            cr_setting.zero_pad    = TRUE;  /*zero padding*/
            cr_setting.msb_shift   = TRUE; /*MSB shift*/

            cr_setting.mode = ESAI_TCR_TMOD_NETWORK;

            ESAI_FIFO_DEBUG("[ESAI] TX Set ESAI_MODULE_FMT_RIGHT_J\n");

            break;

        case ESAI_MODULE_FMT_TDM:

            cr_setting.slots_num = config->inf_fmt.fmt_para.tdm_para.slots;
            cr_setting.slots_mask = config->inf_fmt.fmt_para.tdm_para.mask;
            slot_width = ESAI_MODULE_SW_32BIT;

            cr_setting.fsync_slot_length = TRUE;
            cr_setting.fsync_early = TRUE; /*early*/
            cr_setting.left_align  = TRUE; /*left aligned*/
            cr_setting.zero_pad    = TRUE;  /*zero padding*/
            cr_setting.msb_shift   = TRUE; /*MSB shift*/
            cr_setting.mode        = ESAI_TCR_TMOD_NETWORK;

            ESAI_FIFO_DEBUG("[ESAI] TX Set ESAI_MODULE_FMT_TDM\n");

            break;

        case ESAI_MODULE_FMT_CUSTOM_NETWORK:

            custom_para_p = &(config->inf_fmt.fmt_para.custom_network_para);
            cr_setting.slots_num =
                custom_para_p->slots;
            cr_setting.slots_mask =
                custom_para_p->mask;
            slot_width =
                custom_para_p->common_para.slot_width;

            if (custom_para_p->common_para.fsync_length
                == ESAI_MODULE_ONE_SLOT)
                cr_setting.fsync_slot_length = TRUE;
            else
                cr_setting.fsync_slot_length = FALSE;

            cr_setting.fsync_early =
                custom_para_p->common_para.fsync_early;
            cr_setting.left_align =
                custom_para_p->common_para.left_align;
            cr_setting.zero_pad =
                custom_para_p->common_para.zero_pad;
            cr_setting.msb_shift =
                custom_para_p->common_para.msb_shift;
            cr_setting.mode = ESAI_TCR_TMOD_NETWORK;

            ESAI_FIFO_DEBUG("[ESAI] TX Set ESAI_MODULE_FMT_CUSTOM_NETWORK\n");

            break;

        case ESAI_MODULE_FMT_CUSTOM_NORMAL:

            normal_mode_para_p = &(config->inf_fmt.fmt_para.custom_normal_para);
            cr_setting.slots_num =
                normal_mode_para_p->sync_period;
            cr_setting.slots_mask = 0x1;
            slot_width =
                normal_mode_para_p->common_para.slot_width;

            if (normal_mode_para_p->common_para.fsync_length
                    == ESAI_MODULE_ONE_SLOT)
                cr_setting.fsync_slot_length = TRUE;
            else
                cr_setting.fsync_slot_length = FALSE;
            cr_setting.fsync_early =
                normal_mode_para_p->common_para.fsync_early;
            cr_setting.left_align =
                normal_mode_para_p->common_para.left_align;
            cr_setting.zero_pad =
                normal_mode_para_p->common_para.zero_pad;
            cr_setting.msb_shift =
                normal_mode_para_p->common_para.msb_shift;
            cr_setting.mode = ESAI_TCR_TMOD_NORMAL;

            ESAI_FIFO_DEBUG("[ESAI] TX Set ESAI_MODULE_FMT_CUSTOM_NORMAL\n");

            break;

        default:
            /*not support*/
            ESAI_FIFO_DEBUG("[ESAI] Interface Format not supported, FMT=%u\n",
                config->inf_fmt.fmt);
            return ESAI_NODE_TX_CONFIG_PROTOCOL_PARA_ERROR;
    }


    switch (config->pcm_fmt) {
        case ESAI_FIFO_PCM_S8_LE:
            esai_fifo_node->tx_fifo_data_fmt = ESAI_FIFO_DATA_8BIT;

            /*set fifo word length and alignment*/
            esai_fifo_node->tx_fifo_wa = ESAI_TFCR_TWA_WL_8;


            break;
        case ESAI_FIFO_PCM_S16_LE:
            esai_fifo_node->tx_fifo_data_fmt = ESAI_FIFO_DATA_16BIT;

            /*set fifo word length and alignment*/
            esai_fifo_node->tx_fifo_wa = ESAI_TFCR_TWA_WL_16;


            break;
        case ESAI_FIFO_PCM_S24_LE:
            esai_fifo_node->tx_fifo_data_fmt = ESAI_FIFO_DATA_24BIT;

            /*set fifo word length and alignment*/
            esai_fifo_node->tx_fifo_wa = ESAI_TFCR_TWA_WL_24;

            break;
        case ESAI_FIFO_PCM_S24_PACK_LE:
            esai_fifo_node->tx_fifo_data_fmt = ESAI_FIFO_DATA_32BIT;

            /*set fifo word length and alignment*/
            esai_fifo_node->tx_fifo_wa = ESAI_TFCR_TWA_WL_32;


            break;
        default:
            /*not support ... */
            ESAI_FIFO_ERROR("[ESAI] pcm word Format not supported, FMT=%u\n",
                config->pcm_fmt);
            return ESAI_NODE_TX_CONFIG_FMT_PARA_ERROR;
    }

    ret = _esai_fifo_map_slotword_value(slot_width,
                esai_fifo_node->tx_fifo_data_fmt, &cr_setting.slot_data_width);

    if (ret != ESAI_NODE_OK) {
        ESAI_FIFO_ERROR("[ESAI] slot and word width are not matched\n");
        return ret;
    }

    esai_hw_tx_control_setting(esai_fifo_node->esai_regs, &cr_setting);

    ESAI_FIFO_DEBUG("[ESAI] Applied control setting\n");

    /*setting clock*/
    if (config->clock.clock_direction == ESAI_CLOCK_MASTER) {

        clk_setting.dc_val = (cr_setting.slots_num - 1);
        clk_setting.sck_dir = TRUE; /*output*/
        clk_setting.fst_dir = TRUE;
        clk_setting.hck_dir = TRUE;

        switch (config->clock.clock_polarity) {
            case ESAI_CLOCK_POLAR_NB_NF:

                clk_setting.sync_polar = FALSE;
                clk_setting.sclk_polar = TRUE;

                break;
            case ESAI_CLOCK_POLAR_IB_NF:

                clk_setting.sync_polar = FALSE;
                clk_setting.sclk_polar = FALSE;

                break;
            case ESAI_CLOCK_POLAR_NB_IF:

                clk_setting.sync_polar = TRUE;
                clk_setting.sclk_polar = TRUE;

                break;
            case ESAI_CLOCK_POLAR_IB_IF:

                clk_setting.sync_polar = TRUE;
                clk_setting.sclk_polar = FALSE;

                break;
        }

        if (config->clock.output_refer_clk == ESAI_CLOCK_EXTAL) {
            /*set ETI and ETO to 1, use the EXTAL*/

            clk_setting.source_extal = TRUE;

            /*don't need divders for hck*/
            hck = esai_fifo_node->extal_clk;

            /*TFP needs for sck */
            sck = _esai_fifo_calc_sck_value(
                config->clock.timing.extal_para.frame_rate,
                slot_width, cr_setting.slots_num, &slot_rate);

            ESAI_FIFO_DEBUG("hck = %u, sck = %u, slot_rate = %u \n",
                hck, sck, slot_rate);

            temp = (hck/sck);

            if ((hck % sck) || (temp % 2) || temp > 32)
            {
                ESAI_FIFO_ERROR("[ESAI] sample rate is not supported\n");

                return ESAI_NODE_TX_CONFIG_CLK_PARA_ERROR;
            }

            clk_setting.fp_val = ((temp >> 1) - 1);
            clk_setting.pm_val = 0;
            clk_setting.psr_val = TRUE;

        } else if (config->clock.output_refer_clk == ESAI_CLOCK_FSYS) {

            //uint32_t fsys = _bsp_esai_clocks_get_fsys();

            /*clear ETI and ETO to 1, use the Fsys*/
            clk_setting.source_extal = FALSE;

            /*To use the fsys, the dividers (TPSR and TPM) need to be set for hck*/
            clk_setting.pm_val = config->clock.timing.fsys_para.pm - 1;
            clk_setting.psr_val = config->clock.timing.fsys_para.psr;

            /*TFP needs for sck */
            clk_setting.fp_val = config->clock.timing.fsys_para.fp - 1;

        }

        /*print clk setting*/
        _esai_fifo_clk_debug(&clk_setting);
        esai_hw_tx_clock_setting(esai_fifo_node->esai_regs, &clk_setting);

    } else if (config->clock.clock_direction == ESAI_CLOCK_SLAVE) {

        clk_setting.dc_val = (cr_setting.slots_num - 1);
        clk_setting.sck_dir = FALSE; /*output*/
        clk_setting.fst_dir = FALSE;
        clk_setting.hck_dir = FALSE;

        switch (config->clock.clock_polarity) {
            case ESAI_CLOCK_POLAR_NB_NF:

                clk_setting.sync_polar = FALSE;
                clk_setting.sclk_polar = TRUE;

                break;
            case ESAI_CLOCK_POLAR_IB_NF:

                clk_setting.sync_polar = FALSE;
                clk_setting.sclk_polar = FALSE;

                break;
            case ESAI_CLOCK_POLAR_NB_IF:

                clk_setting.sync_polar = TRUE;
                clk_setting.sclk_polar = TRUE;

                break;
            case ESAI_CLOCK_POLAR_IB_IF:

                clk_setting.sync_polar = TRUE;
                clk_setting.sclk_polar = FALSE;

                break;
        }

        clk_setting.fp_val = 0; //((hck/sck) %  - 1);
        clk_setting.pm_val = 0;
        clk_setting.psr_val = 0;
        clk_setting.source_extal = FALSE;

        esai_hw_tx_clock_setting(esai_fifo_node->esai_regs, &clk_setting);
    }

    esai_fifo_node->tx_asrc_8bit_data = config->asrc_8bit_covert;

    return ESAI_NODE_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_rx_config
* Returned Value   : uint32_t
* Comments         :
*    apply the rx configuration
*
*END*********************************************************************/
uint32_t esai_fifo_node_rx_config
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node,

    /* [IN] pointer to RX configuration*/
    ESAI_FIFO_CONFIG_STRUCT_PTR config
)
{
    uint32_t ret = ESAI_NODE_OK;

    ESAI_HW_CR_SETTING_STRUCT cr_setting;
    ESAI_HW_CLK_SETTING_STRUCT clk_setting;
    ESAI_MODULE_SLOT_WIDTH slot_width;

    ESAI_MODULE_NETWORK_CUSTOM_PARA_PTR custom_para_p = NULL;
    ESAI_MODULE_NORMAL_MODE_PARA_PTR normal_mode_para_p = NULL;

    uint32_t sck = 0, hck = 0, slot_rate, temp;


    if (NULL == esai_fifo_node)
        return ESAI_NODE_INVALID_PARAMETER;

    /*only reset or stop state can apply the configuration*/
    if (esai_fifo_node->rx_state != ESAI_STATE_STOP
        && esai_fifo_node->rx_state != ESAI_STATE_RESET)
        return ESAI_NODE_RX_STATE_ERROR;

    if (config == NULL)
        return ESAI_NODE_INVALID_PARAMETER;

    ESAI_FIFO_DEBUG("[ESAI] esai_fifo_node_rx_config\n");

    /*apply config*/
    switch  (config->inf_fmt.fmt)   {

    case    ESAI_MODULE_FMT_I2S:

            /*slots width*/
            slot_width = config->inf_fmt.fmt_para.stereo_para.slot_width;
            cr_setting.slots_num = 2;

            /*slots mask 0x3, the low two slots enabled*/
            cr_setting.slots_mask = 0x3;

            cr_setting.fsync_slot_length = TRUE;
            cr_setting.fsync_early = TRUE; /*early*/
            cr_setting.left_align  = TRUE; /*left aligned*/
            cr_setting.zero_pad    = TRUE;  /*zero padding*/
            cr_setting.msb_shift   = TRUE; /*MSB shift*/

            cr_setting.mode = ESAI_RCR_RMOD_NETWORK;

            ESAI_FIFO_DEBUG("[ESAI] RX Set ESAI_MODULE_FMT_I2S\n");

            break;

    case    ESAI_MODULE_FMT_LEFT_J:

            /*slots width*/
            slot_width = config->inf_fmt.fmt_para.stereo_para.slot_width;
            cr_setting.slots_num = 2;

            /*slots mask 0x3, the low two slots enabled*/
            cr_setting.slots_mask = 0x3;

            cr_setting.fsync_slot_length = TRUE;
            cr_setting.fsync_early = FALSE; /*not early*/
            cr_setting.left_align  = TRUE; /*left aligned*/
            cr_setting.zero_pad    = TRUE;  /*zero padding*/
            cr_setting.msb_shift   = TRUE; /*MSB shift*/

            cr_setting.mode = ESAI_RCR_RMOD_NETWORK;

            ESAI_FIFO_DEBUG("[ESAI] RX Set ESAI_MODULE_FMT_LEFT_J\n");

            break;

    case    ESAI_MODULE_FMT_RIGHT_J:

            /*slots width*/
            slot_width = config->inf_fmt.fmt_para.stereo_para.slot_width;
            cr_setting.slots_num = 2;

            /*slots mask 0x3, the low two slots enabled*/
            cr_setting.slots_mask = 0x3;

            cr_setting.fsync_slot_length = TRUE;
            cr_setting.fsync_early = FALSE; /*not early*/
            cr_setting.left_align  = FALSE; /*right aligned*/
            cr_setting.zero_pad    = TRUE;  /*zero padding*/
            cr_setting.msb_shift   = TRUE; /*MSB shift*/

            cr_setting.mode = ESAI_RCR_RMOD_NETWORK;

            ESAI_FIFO_DEBUG("[ESAI] RX Set ESAI_MODULE_FMT_RIGHT_J\n");

            break;

    case    ESAI_MODULE_FMT_TDM:

            cr_setting.slots_num = config->inf_fmt.fmt_para.tdm_para.slots;
            cr_setting.slots_mask = config->inf_fmt.fmt_para.tdm_para.mask;
            slot_width = ESAI_MODULE_SW_32BIT;

            cr_setting.fsync_slot_length = TRUE;
            cr_setting.fsync_early = TRUE; /*early*/
            cr_setting.left_align  = TRUE; /*left aligned*/
            cr_setting.zero_pad    = TRUE;  /*zero padding*/
            cr_setting.msb_shift   = TRUE; /*MSB shift*/

            cr_setting.mode = ESAI_RCR_RMOD_NETWORK;

            ESAI_FIFO_DEBUG("[ESAI] RX Set ESAI_MODULE_FMT_TDM\n");

            break;

    case    ESAI_MODULE_FMT_CUSTOM_NETWORK:

            custom_para_p = &(config->inf_fmt.fmt_para.custom_network_para);
            cr_setting.slots_num =
                custom_para_p->slots;
            cr_setting.slots_mask =
                custom_para_p->mask;
            slot_width =
                custom_para_p->common_para.slot_width;

            if (custom_para_p->common_para.fsync_length
                == ESAI_MODULE_ONE_SLOT)
                cr_setting.fsync_slot_length = TRUE;
            else
                cr_setting.fsync_slot_length = FALSE;

            cr_setting.fsync_early =
                custom_para_p->common_para.fsync_early;
            cr_setting.left_align =
                custom_para_p->common_para.left_align;
            cr_setting.zero_pad =
                custom_para_p->common_para.zero_pad;
            cr_setting.msb_shift =
                custom_para_p->common_para.msb_shift;

            cr_setting.mode = ESAI_RCR_RMOD_NETWORK;

            ESAI_FIFO_DEBUG("[ESAI] TX Set ESAI_MODULE_FMT_CUSTOM_NETWORK\n");

            break;

    case    ESAI_MODULE_FMT_CUSTOM_NORMAL:

            normal_mode_para_p = &(config->inf_fmt.fmt_para.custom_normal_para);
            cr_setting.slots_num =
                normal_mode_para_p->sync_period;
            cr_setting.slots_mask = 0x1;
            slot_width =
                normal_mode_para_p->common_para.slot_width;

            if (normal_mode_para_p->common_para.fsync_length
                    == ESAI_MODULE_ONE_SLOT)
                cr_setting.fsync_slot_length = TRUE;
            else
                cr_setting.fsync_slot_length = FALSE;
            cr_setting.fsync_early =
                normal_mode_para_p->common_para.fsync_early;
            cr_setting.left_align =
                normal_mode_para_p->common_para.left_align;
            cr_setting.zero_pad =
                normal_mode_para_p->common_para.zero_pad;
            cr_setting.msb_shift =
                normal_mode_para_p->common_para.msb_shift;

            cr_setting.mode = ESAI_RCR_RMOD_NORMAL;

            ESAI_FIFO_DEBUG("[ESAI] RX Set ESAI_MODULE_FMT_CUSTOM_NORMAL\n");

            break;

        default:
            /*not support*/
            ESAI_FIFO_ERROR("[ESAI] Interface Format not supported, FMT=%u\n",
                config->inf_fmt.fmt);
            return ESAI_NODE_RX_CONFIG_PROTOCOL_PARA_ERROR;
    }


    switch  (config->pcm_fmt)   {

    case    ESAI_FIFO_PCM_S8_LE:
            esai_fifo_node->rx_fifo_data_fmt = ESAI_FIFO_DATA_8BIT;

            /*set fifo word length and alignment*/
            esai_fifo_node->rx_fifo_wa = ESAI_RFCR_RWA_WL_8;

            break;

    case    ESAI_FIFO_PCM_S16_LE:
            esai_fifo_node->rx_fifo_data_fmt = ESAI_FIFO_DATA_16BIT;

            /*set fifo word length and alignment*/
            esai_fifo_node->rx_fifo_wa = ESAI_RFCR_RWA_WL_16;

            break;

    case    ESAI_FIFO_PCM_S24_LE:
            esai_fifo_node->rx_fifo_data_fmt = ESAI_FIFO_DATA_24BIT;

            /*set fifo word length and alignment*/
            esai_fifo_node->rx_fifo_wa = ESAI_RFCR_RWA_WL_24;

            break;

    case    ESAI_FIFO_PCM_S24_PACK_LE:
            esai_fifo_node->rx_fifo_data_fmt = ESAI_FIFO_DATA_32BIT;

            /*set fifo word length and alignment*/
            esai_fifo_node->rx_fifo_wa = ESAI_RFCR_RWA_WL_32;

            break;

    default:
            /*not support ... */
            ESAI_FIFO_ERROR("[ESAI] pcm word Format not supported, FMT=%u\n",
                config->pcm_fmt);
            return ESAI_NODE_RX_CONFIG_FMT_PARA_ERROR;
    }

    ret = _esai_fifo_map_slotword_value(slot_width,
                esai_fifo_node->rx_fifo_data_fmt, &cr_setting.slot_data_width);

    if (ret != ESAI_NODE_OK) {
        ESAI_FIFO_ERROR("[ESAI] slot and word width setting not matched\n");
        return ESAI_NODE_RX_CONFIG_FMT_PARA_ERROR;
    }

    esai_hw_rx_control_setting(esai_fifo_node->esai_regs, &cr_setting);

    ESAI_FIFO_DEBUG("[ESAI] Applied rx control setting\n");

    /*setting clock*/
    if (config->clock.clock_direction == ESAI_CLOCK_MASTER) {

        clk_setting.dc_val = (cr_setting.slots_num - 1);
        clk_setting.sck_dir = TRUE; /*output*/
        clk_setting.fst_dir = TRUE;
        clk_setting.hck_dir = TRUE;

        switch  (config->clock.clock_polarity)  {

        case    ESAI_CLOCK_POLAR_NB_NF:
                clk_setting.sync_polar = FALSE;
                clk_setting.sclk_polar = TRUE;
                break;

        case    ESAI_CLOCK_POLAR_IB_NF:
                clk_setting.sync_polar = FALSE;
                clk_setting.sclk_polar = FALSE;
                break;

        case    ESAI_CLOCK_POLAR_NB_IF:
                clk_setting.sync_polar = TRUE;
                clk_setting.sclk_polar = TRUE;
                break;

        case    ESAI_CLOCK_POLAR_IB_IF:
                clk_setting.sync_polar = TRUE;
                clk_setting.sclk_polar = FALSE;
                break;
        }

        if (config->clock.output_refer_clk == ESAI_CLOCK_EXTAL) {
            /*set ETI and ETO to 1, use the EXTAL*/

            clk_setting.source_extal = TRUE;

            /*don't need divders for hck*/
            hck = esai_fifo_node->extal_clk;

            /*TFP needs for sck */
            sck = _esai_fifo_calc_sck_value(
                config->clock.timing.extal_para.frame_rate,
                slot_width, cr_setting.slots_num, &slot_rate);

            ESAI_FIFO_DEBUG("hck = %u, sck = %u, slot_rate = %u \n",
                hck, sck, slot_rate);

            temp = (hck/sck);

            if ((hck % sck) || (temp % 2) || temp > 32)
            {
                ESAI_FIFO_ERROR("[ESAI] sample rate is not supported\n");

                return ESAI_NODE_TX_CONFIG_CLK_PARA_ERROR;
            }

            clk_setting.fp_val = ((temp >> 1) - 1);
            clk_setting.pm_val = 0;
            clk_setting.psr_val = TRUE;

        } else if (config->clock.output_refer_clk == ESAI_CLOCK_FSYS) {

            //uint32_t fsys = _bsp_esai_clocks_get_fsys();

            /*clear ETI and ETO to 1, use the Fsys*/
            clk_setting.source_extal = FALSE;

            /*To use the fsys, the dividers (TPSR and TPM) need to be set for hck*/
            clk_setting.pm_val = config->clock.timing.fsys_para.pm - 1;
            clk_setting.psr_val = config->clock.timing.fsys_para.psr;

            /*TFP needs for sck */
            clk_setting.fp_val = config->clock.timing.fsys_para.fp - 1;

        }

        /*print clk setting*/
        _esai_fifo_clk_debug(&clk_setting);

        esai_hw_rx_clock_setting(esai_fifo_node->esai_regs, &clk_setting);

    } else if (config->clock.clock_direction == ESAI_CLOCK_SLAVE) {

        clk_setting.dc_val = (cr_setting.slots_num - 1);
        clk_setting.sck_dir = FALSE; /*output*/
        clk_setting.fst_dir = FALSE;
        clk_setting.hck_dir = FALSE;

        switch  (config->clock.clock_polarity)  {

        case    ESAI_CLOCK_POLAR_NB_NF:
                clk_setting.sync_polar = FALSE;
                clk_setting.sclk_polar = TRUE;
                break;

        case    ESAI_CLOCK_POLAR_IB_NF:
                clk_setting.sync_polar = FALSE;
                clk_setting.sclk_polar = FALSE;
                break;

        case    ESAI_CLOCK_POLAR_NB_IF:
                clk_setting.sync_polar = TRUE;
                clk_setting.sclk_polar = TRUE;
                break;

        case    ESAI_CLOCK_POLAR_IB_IF:
                clk_setting.sync_polar = TRUE;
                clk_setting.sclk_polar = FALSE;
                break;
        }

        clk_setting.fp_val = 0; //((hck/sck) %  - 1);
        clk_setting.pm_val = 0;
        clk_setting.psr_val = 0;
        clk_setting.source_extal = FALSE;

        esai_hw_rx_clock_setting(esai_fifo_node->esai_regs, &clk_setting);
    }


    return ESAI_NODE_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_read
* Returned Value   : int32_t
* Comments         :
*    read the data from esai fifo, return the length readed.
*
*END*********************************************************************/
int32_t esai_fifo_node_read
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node,

    /* [IN] User's buffer to read the data to*/
    void* buffer,

    /* [IN] User's buffer length */
    uint32_t length
)
{
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node = node;
    uint32_t pack_len = length, remain_len = 0, temp_remian_len = 0, mp_len = 0;
    uint8_t *buf_temp = buffer, *temp_ptr = NULL;
    _mqx_uint retval;

    MQX_TICK_STRUCT start_tick, end_tick;
    int32_t tick_diff;
    bool overflow, poll;
    uint32_t timeout = esai_fifo_node->rx_timeout;

    esai_fifo_node->rx_error_code = IO_OK;
    esai_fifo_node->read_times++;

    if (ESAI_FIFO_DATA_24BIT == esai_fifo_node->rx_fifo_data_fmt) {
        pack_len = (length / 3) * 3;
        mp_len = (length % 3);
    }

    /*checking the rx fifo status, is fifo full?*/
    if (esai_hw_rx_fifo_full(esai_fifo_node->esai_regs)) {
         ESAI_FIFO_DEBUG("[ESAI] RX FIFO is FULL!!!, rx_packet_count=%u\n",
            esai_fifo_node->rx_packet_count);
    }

    while (pack_len > 0) {
        if (esai_fifo_node->rx_state != ESAI_STATE_RUN) {

            esai_fifo_node->rx_error_code = IO_ERROR_DEVICE_INVALID;
            return (length - mp_len - pack_len);
        }

        /*try to get the semaphore, may block the calling task*/
        while (_esai_fifo_rx_buffer_empty(esai_fifo_node)) {
            ESAI_FIFO_DEBUG("ESAI] RX FIFO is EMPTY!!!\n");

            if (esai_fifo_node->rx_timeout > 0) {
                if (timeout > 0) {
                    _time_get_ticks(&start_tick);
                    retval = _lwsem_wait_ticks(&(esai_fifo_node->rx_sem),
                        timeout);
                    if (MQX_LWSEM_WAIT_TIMEOUT == retval) {
                        /*set time out error code*/
                        esai_fifo_node->rx_error_code = IO_ERROR_TIMEOUT;
                        return (length - mp_len - pack_len);
                    }

                    _time_get_ticks(&end_tick);
                    tick_diff = _time_diff_ticks_int32(&end_tick, &start_tick,
                        &overflow);

                    /*check the ticks diff with timeout ticks, update timeout value*/
                    if (overflow || (uint32_t)tick_diff > timeout)
                        timeout = 0;
                    else
                        timeout = timeout - tick_diff;

                } else {
                    /*use non-blocking semaphore get when timeout becomes 0*/
                    poll = _lwsem_poll(&(esai_fifo_node->rx_sem));
                    if (!poll) {
                        esai_fifo_node->rx_error_code = IO_ERROR_TIMEOUT;
                        return (length - mp_len - pack_len);
                    }
                }
            }else {
                _lwsem_wait(&(esai_fifo_node->rx_sem));
            }
       }


        _mutex_lock(&esai_fifo_node->control_mutex);

        if (esai_fifo_node->rx_state != ESAI_STATE_RUN) {
            esai_fifo_node->rx_error_code = IO_ERROR_DEVICE_INVALID;
            _mutex_unlock(&esai_fifo_node->control_mutex);
            return (length - mp_len - pack_len);
        }

        remain_len = esai_fifo_node->rx_packet_size;

        temp_ptr = (uint8_t *)
                (esai_fifo_node->rx_dma_packets[esai_fifo_node->rx_out].DST_ADDR
                 + esai_fifo_node->rx_packet_size
                 - remain_len);

        if (ESAI_FIFO_DATA_24BIT == esai_fifo_node->rx_fifo_data_fmt) {
            temp_remian_len = remain_len;
            remain_len = (remain_len / 4) * 3;
        }

        if (pack_len < remain_len) {
            if (ESAI_FIFO_DATA_24BIT == esai_fifo_node->rx_fifo_data_fmt) {
                pack_len = (pack_len / 3) * 4;
                _esai_fifo_32_to_24(temp_ptr, pack_len, buf_temp);
            } else {
                _mem_copy(temp_ptr, buf_temp, pack_len);
            }

            pack_len = 0;
        } else {
            if (ESAI_FIFO_DATA_24BIT == esai_fifo_node->rx_fifo_data_fmt) {
                _esai_fifo_32_to_24(temp_ptr, temp_remian_len, buf_temp);
            } else {
                _mem_copy(temp_ptr, buf_temp, remain_len);
            }

            pack_len -= remain_len;
            buf_temp += remain_len;

            _int_disable();
            ESAI_INC_BUF(esai_fifo_node->rx_out, esai_fifo_node->rx_packet_num);
            ESAI_INC_BUF(esai_fifo_node->rx_ready,
                         esai_fifo_node->rx_packet_num);
            if (esai_fifo_node->rx_packet_count)
                esai_fifo_node->rx_packet_count--;
            _int_enable();
        }

        _mutex_unlock(&esai_fifo_node->control_mutex);
    }

    if (mp_len)
        esai_fifo_node->rx_error_code = IO_ERROR_NOTALIGNED;

    return (length - mp_len - pack_len);
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_write
* Returned Value   : int32_t
* Comments         :
*    write the data to esai fifo by DMA,  would be blocked if the buffer is full
*    return the length written.
*
*END*********************************************************************/
int32_t esai_fifo_node_write
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node,

    /* [IN] User's buffer to write the data from*/
    void* buffer,

    /* [IN] User's buffer length */
    uint32_t length
)
{
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node = node;

    uint32_t pack_len = length, ret;
    bool is_buf_full = FALSE;
    uint8_t *buf_temp = buffer;
    int32_t temp_count;
    _mqx_uint retval;

    MQX_TICK_STRUCT start_tick, end_tick;
    int32_t tick_diff;
    bool overflow, poll;
    uint32_t timeout = esai_fifo_node->tx_timeout;
    uint8_t in;

    esai_fifo_node->tx_error_code = IO_OK;

    if (NULL == node || NULL == buffer || 0 == length) {
        esai_fifo_node->tx_error_code = IO_ERROR_WRITE;
        return ESAI_NODE_TX_WR_INVALID_PARAMETER;
    }

    if (esai_fifo_node->tx_state != ESAI_STATE_RUN) {
        esai_fifo_node->tx_error_code = IO_ERROR_DEVICE_INVALID;
        return ESAI_NODE_TX_WR_INVALID_STATE;
    }

    retval = _esai_fifo_tx_write_buffer_install(esai_fifo_node);
    if (retval != ESAI_NODE_OK) {
        esai_fifo_node->tx_error_code = IO_ERROR_WRITE;
        return ESAI_NODE_TX_WR_MEMORY_OUT;
    }


    /*checking the fifo status, is buffer empty?*/
    esai_fifo_node->write_times++;

    if (0 == esai_fifo_node->tx_packet_count) {
        esai_fifo_node->tx_buffer_empty++;
    }

    if (esai_hw_tx_fifo_empty(esai_fifo_node->esai_regs)) {
         ESAI_FIFO_DEBUG("[ESAI] TX FIFO empty!!!, tx_packet_count=%u\n",
            esai_fifo_node->tx_packet_count);
    }

#ifdef __ESAI_24BIT_DMA_UNSUPPORT

    if (esai_fifo_node->tx_fifo_data_fmt == ESAI_FIFO_DATA_24BIT) {

        ret = _esai_fifo_24_to_32_extension(esai_fifo_node, buffer, length);
        if (ret == 0) {
            buf_temp = esai_fifo_node->padding_buffer;
            pack_len = esai_fifo_node->padding_buf_data_length;
            length = esai_fifo_node->padding_buf_data_length;
        } else {
            ESAI_FIFO_DEBUG("[ESAI] Format 24 bits to 32 failed\n",
                esai_fifo_node->tx_packet_count);

            esai_fifo_node->tx_error_code = IO_ERROR_WRITE;
            return 0;
        }
    }

#endif

    while (pack_len > 0) {

        if (esai_fifo_node->tx_state != ESAI_STATE_RUN) {
            esai_fifo_node->tx_error_code = IO_ERROR_DEVICE_INVALID;
#ifdef __ESAI_24BIT_DMA_UNSUPPORT
            if (esai_fifo_node->tx_fifo_data_fmt == ESAI_FIFO_DATA_24BIT) {
                return (((length - pack_len) >> 2)*3);
            }
#endif
            return (length - pack_len);
        }

        _int_disable();
        is_buf_full = _esai_fifo_tx_buffer_full(esai_fifo_node);
        _int_enable();

        if (is_buf_full) {
            /*left blank*/
        }


        if (esai_fifo_node->tx_timeout > 0) {
            if (timeout > 0) {

                _time_get_ticks(&start_tick);

                /*try to get the semaphore, may block the calling task*/
                retval = _lwsem_wait_ticks(&(esai_fifo_node->tx_sem), timeout);
                if (MQX_LWSEM_WAIT_TIMEOUT == retval) {
                    /*set time out error code*/
                    esai_fifo_node->tx_error_code = IO_ERROR_TIMEOUT;

#ifdef __ESAI_24BIT_DMA_UNSUPPORT
                    if (ESAI_FIFO_DATA_24BIT ==
                        esai_fifo_node->tx_fifo_data_fmt) {
                        return (((length - pack_len) >> 2)*3);
                    }
#endif
                    return (length - pack_len);
                }

                _time_get_ticks(&end_tick);
                tick_diff = _time_diff_ticks_int32(&end_tick, &start_tick,
                    &overflow);

                /*check the ticks diff with timeout ticks, update timeout value*/
                if (overflow || (uint32_t)tick_diff > timeout)
                    timeout = 0;
                else
                    timeout = timeout - tick_diff;

            } else {
                /*use non-blocking semaphore get when timeout becomes 0*/
                poll = _lwsem_poll(&(esai_fifo_node->tx_sem));
                if (!poll) {

                    /*set time out error code*/
                    esai_fifo_node->tx_error_code = IO_ERROR_TIMEOUT;

#ifdef __ESAI_24BIT_DMA_UNSUPPORT
                    if (ESAI_FIFO_DATA_24BIT ==
                        esai_fifo_node->tx_fifo_data_fmt) {
                        return (((length - pack_len) >> 2)*3);
                    }
#endif
                    return (length - pack_len);
                }
            }
        }else {
            _lwsem_wait(&(esai_fifo_node->tx_sem));
        }

        /*Have got a empy packet here*/

        _mutex_lock(&esai_fifo_node->control_mutex);
        if (esai_fifo_node->tx_state != ESAI_STATE_RUN) {
            _lwsem_post(&esai_fifo_node->tx_sem);
            esai_fifo_node->tx_error_code = IO_ERROR_DEVICE_INVALID;
            _mutex_unlock(&esai_fifo_node->control_mutex);

#ifdef __ESAI_24BIT_DMA_UNSUPPORT
            if (esai_fifo_node->tx_fifo_data_fmt == ESAI_FIFO_DATA_24BIT) {
                return (((length - pack_len) >> 2)*3);
            }
#endif
            return (length - pack_len);
        }

        in = esai_fifo_node->tx_in;
        if (pack_len <= esai_fifo_node->tx_packet_size) {
            /*allocate one packet buffer for this transfer*/
            _mem_copy(buf_temp,
             (void*)(esai_fifo_node->tx_dma_packets[in].SRC_ADDR),
              pack_len);

            temp_count = pack_len/esai_fifo_node->tx_size_per_request;
            if (temp_count) {
                esai_fifo_node->tx_dma_packets[in].LOOP_BYTES
                    = esai_fifo_node->tx_size_per_request;
                esai_fifo_node->tx_dma_packets[in].LOOP_COUNT
                    = temp_count;
                pack_len -= (temp_count * esai_fifo_node->tx_size_per_request);
            } else if (pack_len % esai_fifo_node->tx_size_per_request){
                esai_fifo_node->tx_dma_packets[in].LOOP_BYTES
                    = pack_len % esai_fifo_node->tx_size_per_request;
                esai_fifo_node->tx_dma_packets[in].LOOP_COUNT
                    = 1;
                pack_len -= esai_fifo_node->tx_dma_packets[in].LOOP_BYTES;
            }
        } else {
            _mem_copy(buf_temp,
               (void*)esai_fifo_node->tx_dma_packets[in].SRC_ADDR,
               esai_fifo_node->tx_packet_size);
            /*assume the tx_packet_size if the multiple of tx_size_per_request*/
            esai_fifo_node->tx_dma_packets[in].LOOP_BYTES
                = esai_fifo_node->tx_size_per_request;
            esai_fifo_node->tx_dma_packets[in].LOOP_COUNT
                = esai_fifo_node->tx_packet_size
                / esai_fifo_node->tx_size_per_request;

            pack_len -= esai_fifo_node->tx_packet_size;
            buf_temp += esai_fifo_node->tx_packet_size;
        }

        _int_disable();
        ESAI_INC_BUF(esai_fifo_node->tx_in, esai_fifo_node->tx_packet_num);
        esai_fifo_node->tx_packet_count++;
        esai_fifo_node->tx_buffer_window++;
        _int_enable();

        _mutex_unlock(&esai_fifo_node->control_mutex);

    }

#ifdef __ESAI_24BIT_DMA_UNSUPPORT
    if (esai_fifo_node->tx_fifo_data_fmt == ESAI_FIFO_DATA_24BIT) {
        return (((length - pack_len) >> 2)*3);
    }
#endif

    return (length - pack_len);
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_write
* Returned Value   : int32_t
* Comments         :
*    write the data to esai fifo by CPU
*    return the length written.
*
*END*********************************************************************/
int32_t esai_fifo_node_write_word
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node,

    /* [IN] User's buffer to write the data from*/
    void* buffer,

    /* [IN] User's buffer length */
    uint32_t length
)
{
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node = node;

    uint32_t data_bytes = 0;
    uint32_t i, j;
    uint32_t data, data2 = 0;
    uint8_t *temp = (uint8_t*)buffer;

    uint32_t tx_underrun = 0;

    if (NULL == node)
        return ESAI_NODE_TX_WR_INVALID_PARAMETER;

    switch (esai_fifo_node->tx_fifo_data_fmt) {
        case ESAI_FIFO_DATA_8BIT:
            data_bytes = 1;
            break;
        case ESAI_FIFO_DATA_16BIT:
            data_bytes = 2;
            break;
        case ESAI_FIFO_DATA_24BIT:
            data_bytes = 3;
            break;
        case ESAI_FIFO_DATA_32BIT:
            data_bytes = 4;
            break;
        default:
            /*error*/
            return ESAI_NODE_TX_WR_INVALID_PARAMETER;
    }

    for (i=0; i<length; i+=data_bytes)
    {
        data = 0;
        for (j=0;j<data_bytes;j++) {
            data2 = *(temp+i+j);
            data |= (data2 << (j<<3));
        }

        while (!(esai_fifo_node->esai_regs->ESR & ESAI_ESR_TFE_MASK))
        {}

        if ((esai_fifo_node->esai_regs->SAISR & ESAI_SAISR_TUE_MASK))
          tx_underrun++;

        esai_hw_tx_fifo_write(esai_fifo_node->esai_regs, data);

    }

    if (tx_underrun != 0)
      ESAI_FIFO_DEBUG("tx_underrun=%d\n", tx_underrun);

    return i;
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_poll
* Returned Value   : uint32_t
* Comments         :
*    Poll one status of ESAI, the caller will be blocked util the event got
*
*END*********************************************************************/
uint32_t esai_fifo_node_poll
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node,

    /* [IN] poll id*/
    uint32_t poll_id,

    /* [IN] poll parameter */
    void* parameter
)
{
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node = node;

    switch (poll_id) {
        case ESAI_FIFO_POLL_TX_FIFO_EMPTY:
            while (!esai_hw_tx_fifo_empty(esai_fifo_node->esai_regs))
            {
                _time_delay(10);
            }
            break;

        default:
            break;
    }

    return ESAI_NODE_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_start
* Returned Value   : uint32_t
* Comments         :
*    Start he ESAI FIFO hardware, DMA channels and ASRC to run
*
*END*********************************************************************/
uint32_t esai_fifo_node_start
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node,

    /* [IN] parameters for node start */
    void* parameters
)
{
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node = node;
    uint32_t temp = (uint32_t)parameters;
    AUD_IO_FW_DIRECTION dir = (AUD_IO_FW_DIRECTION)temp;
    uint32_t ret = ESAI_NODE_OK;

    if (NULL == node)
        return ESAI_NODE_INVALID_PARAMETER;

    _mutex_lock(&esai_fifo_node->control_mutex);

    if (dir == AUD_IO_FW_DIR_TX || dir == AUD_IO_FW_DIR_FULL_DUPLEX)
    {
        ret = _esai_fifo_node_start_section_tx(esai_fifo_node);
        if (ret != ESAI_NODE_OK) {
            _mutex_unlock(&esai_fifo_node->control_mutex);
            return ret;
        }
    }


    if (dir == AUD_IO_FW_DIR_RX || dir == AUD_IO_FW_DIR_FULL_DUPLEX) {
        ret = _esai_fifo_node_start_section_rx(esai_fifo_node);
        if (ret != ESAI_NODE_OK) {
            _mutex_unlock(&esai_fifo_node->control_mutex);
            return ret;
        }
    }

    _mutex_unlock(&esai_fifo_node->control_mutex);

    return ESAI_NODE_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_stop
* Returned Value   : uint32_t
* Comments         :
*    Stop the ESAI FIFO hardware, DMA channels and ASRC if have it.
*
*END*********************************************************************/
uint32_t esai_fifo_node_stop
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node,

    /* [IN] parameters for node stop */
    void* parameters
)
{
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node = node;
    uint32_t temp = (uint32_t)parameters;
    AUD_IO_FW_DIRECTION dir = (AUD_IO_FW_DIRECTION)temp;
    uint32_t ret = ESAI_NODE_OK;
    int32_t i = 0;

    if (NULL == node)
        return ESAI_NODE_INVALID_PARAMETER;

    _mutex_lock(&esai_fifo_node->control_mutex);

    if (dir == AUD_IO_FW_DIR_TX || dir == AUD_IO_FW_DIR_FULL_DUPLEX)
    {
        /*stop the TX*/
        if (esai_fifo_node->tx_state != ESAI_STATE_RUN) {
            _mutex_unlock(&esai_fifo_node->control_mutex);
            return ESAI_NODE_TX_STATE_ERROR;
        }
        ESAI_FIFO_DEBUG("[ESAI] TX Stop req\n");

        esai_hw_tx_enable_interrupts(esai_fifo_node->esai_regs, FALSE);

        _int_disable();
        esai_fifo_node->tx_state = ESAI_STATE_STOP;

        /*release all pending write task*/
        while (esai_fifo_node->tx_packet_count--)
        {
            _lwsem_post(&esai_fifo_node->tx_sem);
        }

        esai_fifo_node->tx_buffer_window = 0;

        /*stop old dma_channel*/
        _int_enable();

        _esai_fifo_node_stop_dma(esai_fifo_node, AUD_IO_FW_DIR_TX);

        /*free the zero packets*/
        for (i=0; i<ESAI_TX_DMA_ZERO_PACKET_NUM; i++) {
            esai_fifo_node->tx_dma_zero_packets[i].LOOP_COUNT= 0;
        }

        if (esai_fifo_node->asrc_enable) {
            dma_request_disable(esai_fifo_node->tx_asrc_esai_channel);

#ifdef ASRC_HARDCODE_VERSION
            asrc_plugin_stop_pair(0);
#endif
        }

        /*put transmit to reset*/
        esai_hw_tx_reset(esai_fifo_node->esai_regs);

        ESAI_FIFO_DEBUG("[ESAI] TX Stopped\n");

        ESAI_FIFO_DEBUG("underrun = %d, tx_zero_pad_count = %d\n",
            esai_fifo_node->tx_underrun,
            esai_fifo_node->tx_zero_pad_count);
    }

    if (dir == AUD_IO_FW_DIR_RX || dir == AUD_IO_FW_DIR_FULL_DUPLEX)
    {
        /*stop the RX*/
        if (esai_fifo_node->rx_state != ESAI_STATE_RUN) {
            _mutex_unlock(&esai_fifo_node->control_mutex);
            return ESAI_NODE_RX_STATE_ERROR;
        }
        ESAI_FIFO_DEBUG("[ESAI] RX Stop req\n");

        esai_hw_rx_enable_interrupts(esai_fifo_node->esai_regs, FALSE);

        _int_disable();
        esai_fifo_node->rx_state = ESAI_STATE_STOP;
        _lwsem_post(&esai_fifo_node->rx_sem);
        _int_enable();

        /*stop old dma_channel*/
        _esai_fifo_node_stop_dma(esai_fifo_node, AUD_IO_FW_DIR_RX);

        /*put receiver to reset*/
        esai_hw_rx_reset(esai_fifo_node->esai_regs);

        ESAI_FIFO_DEBUG("[ESAI] RX Stopped\n");

        ESAI_FIFO_DEBUG("rx buffer overrun = %d\n",
                esai_fifo_node->rx_buf_overrun_count);
    }

    _mutex_unlock(&esai_fifo_node->control_mutex);
    return ret;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_get_tx_state
* Returned Value   : ESAI_NODE_STATE
* Comments         :
*    Returns the current ESAI FIFO RX state
*
*END*********************************************************************/
ESAI_NODE_STATE esai_fifo_node_get_tx_state
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node
)
{
    return node->tx_state;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_get_rx_state
* Returned Value   : ESAI_NODE_STATE
* Comments         :
*    Returns the current ESAI FIFO RX state
*
*END*********************************************************************/
ESAI_NODE_STATE esai_fifo_node_get_rx_state
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node
)
{
    return node->rx_state;
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_init
* Returned Value   : uint32_t
* Comments         :
*    initilization of ESAI fifo node
*
*END*********************************************************************/
uint32_t esai_fifo_node_init
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node,

    /* [IN] initilaztion paramters */
    ESAI_FIFO_NODE_INIT_STRUCT_PTR init_param
)
{
    uint32_t i = 0;
    uint32_t result = 0;
    uint32_t zero_packet_tx_size = 0;
    uint32_t zero_packet_rx_size = 0;

    if (esai_fifo_node == NULL || init_param == NULL)
        return ESAI_NODE_INVALID_PARAMETER;

    ESAI_FIFO_DEBUG("[ESAI] esai_fifo_node_init\n");

    /*init internal buffers*/
    if (init_param->TX_DMA_PACKET_SIZE == 0)
        esai_fifo_node->tx_packet_size          = 512;
    else if (init_param->TX_DMA_PACKET_SIZE > 2048)
        esai_fifo_node->tx_packet_size          = 2048;
    else
        esai_fifo_node->tx_packet_size          =
                init_param->TX_DMA_PACKET_SIZE;

    init_param->RX_DMA_PACKET_SIZE &= ~(0x7F);/*align with 128 */
    if (init_param->RX_DMA_PACKET_SIZE == 0)
        esai_fifo_node->rx_packet_size          = 512;
    else if (init_param->RX_DMA_PACKET_SIZE > 2048)
        esai_fifo_node->rx_packet_size          = 2048;
    else
        esai_fifo_node->rx_packet_size          =
                init_param->RX_DMA_PACKET_SIZE;

#ifdef __ESAI_24BIT_DMA_UNSUPPORT
    esai_fifo_node->rx_padding_size = (esai_fifo_node->rx_packet_size / 4) * 3;
#endif

    if (init_param->TX_DMA_PACKET_MAX_NUM == 0)
        esai_fifo_node->tx_packet_num           =
                ESAI_MAX_TX_DMA_PACKET_NUM;
    else if (init_param->TX_DMA_PACKET_MAX_NUM > 20)
        esai_fifo_node->tx_packet_num           = 20;
    else
        esai_fifo_node->tx_packet_num           =
                init_param->TX_DMA_PACKET_MAX_NUM;

    if (init_param->RX_DMA_PACKET_MAX_NUM == 0)
        esai_fifo_node->rx_packet_num           =
                ESAI_MAX_RX_DMA_PACKET_NUM;
    else if (init_param->RX_DMA_PACKET_MAX_NUM > 20)
        esai_fifo_node->rx_packet_num           = 20;
    else
        esai_fifo_node->rx_packet_num           =
                init_param->RX_DMA_PACKET_MAX_NUM;

    esai_fifo_node->tx_packet_count             = 0;
    esai_fifo_node->tx_buffer_window            = 0;
    esai_fifo_node->rx_packet_count             = 0;

    _lwsem_create(&esai_fifo_node->tx_sem, esai_fifo_node->tx_packet_num);
    _lwsem_create(&esai_fifo_node->rx_sem, esai_fifo_node->rx_packet_num);
    _mutex_init(&esai_fifo_node->control_mutex, NULL);


    esai_fifo_node->esai_regs                   =
        (ESAI_MemMapPtr)init_param->BASE_ADDRESS;
    esai_fifo_node->esai_irq                    = init_param->IRQ_NUM;
    esai_fifo_node->tx_state                    = ESAI_STATE_RESET;
    esai_fifo_node->rx_state                    = ESAI_STATE_RESET;

    esai_fifo_node->tr_enable_save              = 0;
    esai_fifo_node->tr_tx_dir_save              = 0;
    esai_fifo_node->tr_rx_dir_save              = 0;

    if (init_param->TX_FIFO_WM >= 128 || init_param->TX_FIFO_WM == 0)
        esai_fifo_node->tx_fifo_wm              = 64;
    else
        esai_fifo_node->tx_fifo_wm              = init_param->TX_FIFO_WM;

    if (init_param->RX_FIFO_WM >= 128 || init_param->RX_FIFO_WM == 0)
        esai_fifo_node->rx_fifo_wm              = 64;
    else
        esai_fifo_node->rx_fifo_wm              = init_param->RX_FIFO_WM;


    esai_fifo_node->fsys_clk = init_param->FSYS_CLK;
    esai_fifo_node->extal_clk = init_param->EXTAL_CLK;


    esai_fifo_node->tx_buffer   = NULL;
    esai_fifo_node->rx_buffer   = NULL;

    /*Max size, 2 channel, 6 tx, 32bits sample*/
    zero_packet_tx_size = (ESAI_ZERO_PADDING_UNIT_SIZE << 3) * 6;
    esai_fifo_node->tx_zero_buffer   =
        (void *)_mem_alloc_system_zero_uncached(zero_packet_tx_size
        * ESAI_TX_DMA_ZERO_PACKET_NUM);


    if (NULL == esai_fifo_node->tx_zero_buffer)
    {
        ESAI_FIFO_ERROR("[ESAI] out of memory for tx pad buffer\n");
        return ESAI_NODE_ALLOC_MEMORY_ERROR;
    }


    /*Max size, 2 channel, 6 tx, 32bits sample*/
    zero_packet_rx_size = (ESAI_ZERO_PADDING_UNIT_SIZE << 3) * 4;
    esai_fifo_node->rx_zero_buffer   =
        (void *)_mem_alloc_system_zero_uncached(zero_packet_rx_size
        * ESAI_RX_DMA_ZERO_PACKET_NUM);
    if (NULL == esai_fifo_node->rx_zero_buffer)
    {
        ESAI_FIFO_ERROR("[ESAI] out of memory for rx pad buffer\n");
        return ESAI_NODE_ALLOC_MEMORY_ERROR;
    }


    _mem_set_type (esai_fifo_node->tx_zero_buffer,
        MEM_TYPE_IO_ESAI_FIFO_NODE_TX_BUF);
    _mem_set_type (esai_fifo_node->rx_zero_buffer,
        MEM_TYPE_IO_ESAI_FIFO_NODE_RX_BUF);

    /*init zero packets, they are used to deal with underrun case*/
    for (i=0; i<ESAI_TX_DMA_ZERO_PACKET_NUM; i++) {
        esai_fifo_node->tx_dma_zero_packets[i].SRC_ADDR =
            ((uint32_t)esai_fifo_node->tx_zero_buffer)
            + (i*zero_packet_tx_size);
        esai_fifo_node->tx_dma_zero_packets[i].DST_ADDR
                = (uint32_t)&(esai_fifo_node->esai_regs->ETDR);
        esai_fifo_node->tx_dma_zero_packets[i].LOOP_COUNT = 0;
        esai_fifo_node->tx_dma_zero_packets[i].LOOP_BYTES = 0; /*bytes*/
        ESAI_FIFO_DEBUG("[ESAI] dma zero packet buffer [%d] = 0x%x\n",
            i, esai_fifo_node->tx_dma_zero_packets[i].SRC_ADDR);
    }

    for (i=0; i<ESAI_RX_DMA_ZERO_PACKET_NUM; i++) {
        esai_fifo_node->rx_dma_zero_packets[i].DST_ADDR =
            ((uint32_t)esai_fifo_node->rx_zero_buffer)
            + (i*zero_packet_rx_size);
        esai_fifo_node->rx_dma_zero_packets[i].SRC_ADDR
                = (uint32_t)&(esai_fifo_node->esai_regs->ERDR);
        esai_fifo_node->rx_dma_zero_packets[i].LOOP_COUNT = 0;
        esai_fifo_node->rx_dma_zero_packets[i].LOOP_BYTES = 0; /*bytes*/
    }

    esai_fifo_node->tx_in = 0;
    esai_fifo_node->tx_out = 0;

    esai_fifo_node->rx_in = 0;
    esai_fifo_node->rx_out = 0;
    esai_fifo_node->rx_ready = 0;
    esai_fifo_node->rx_buf_overrun_count = 0;

    esai_fifo_node->tx_zero_packet_index = 0;
    esai_fifo_node->rx_zero_packet_index = 0;

    esai_fifo_node->tx_zero_packet_count = 0;
    esai_fifo_node->rx_zero_packet_count = 0;

    esai_fifo_node->tx_underrun = 0;
    esai_fifo_node->tx_zero_pad_count = 0;
    esai_fifo_node->tx_buffer_empty = 0;
    esai_fifo_node->write_times = 0;
    esai_fifo_node->read_times = 0;

    esai_fifo_node->callback_func_ptr = NULL;
    esai_fifo_node->callback_param = NULL;

#ifdef __ESAI_24BIT_DMA_UNSUPPORT
    esai_fifo_node->padding_buffer = NULL;
    esai_fifo_node->padding_buf_length = 0;
    esai_fifo_node->padding_buf_data_length = 0;
#endif

    esai_fifo_node->asrc_enable = FALSE;

    esai_fifo_node->tx_asrc_dma_packet.SRC_ADDR = 0;
    esai_fifo_node->tx_asrc_dma_packet.DST_ADDR =
        (uint32_t)&(esai_fifo_node->esai_regs->ETDR);
    esai_fifo_node->tx_asrc_dma_packet.LOOP_COUNT = 10;
    esai_fifo_node->tx_asrc_dma_packet.LOOP_BYTES = 4;

    esai_fifo_node->tx_asrc_8bit_data = FALSE;

    /*pcmmgr interfaces init*/
    _esai_fifo_tx_buflist_init(esai_fifo_node);

    esai_fifo_node->tx_co_data_count = 0;
    esai_fifo_node->rx_co_data_count = 0;

    esai_fifo_node->tx_packet_demand_threshold = 1;

    esai_fifo_node->syn_mode = init_param->SYN_MODE;

    esai_fifo_node->tx_timeout = 0; /*unlimit wait*/
    esai_fifo_node->rx_timeout = 0; /*unlimit wait*/
    esai_fifo_node->tx_error_code = 0; /*no error*/
    esai_fifo_node->rx_error_code = 0; /*no error*/
    ESAI_FIFO_DEBUG("[ESAI] software init finished\n");

    /*reset the ESAI and ESAI FIFO*/
    esai_hw_init(esai_fifo_node->esai_regs);

    /*Enable syn operating mode*/
    if (TRUE == esai_fifo_node->syn_mode)
        esai_enable_syn(esai_fifo_node->esai_regs);

    /*install isr*/
    _bsp_int_init(esai_fifo_node->esai_irq, BSP_ESAI_INT_LEVEL, 0, FALSE);
    if (_int_install_isr(esai_fifo_node->esai_irq, _esai_fifo_isr,
        (void *)esai_fifo_node) == NULL) {
        ESAI_FIFO_ERROR("[ESAI] install esai isr failed\n");
        return ESAI_NODE_INSTALL_ISR_FAIL;
    }
    /*disable all ESAI interrupts bits*/
    esai_hw_tx_enable_interrupts(esai_fifo_node->esai_regs, FALSE);
    esai_hw_rx_enable_interrupts(esai_fifo_node->esai_regs, FALSE);

    /*enable ARM interrupt*/
    _bsp_int_enable(esai_fifo_node->esai_irq);


    ESAI_FIFO_DEBUG("[ESAI] hardware init finished\n");

    /*config dma*/
    esai_fifo_node->tx_dma_channel = NULL;
    esai_fifo_node->rx_dma_channel = NULL;


    result = dma_channel_claim(&esai_fifo_node->tx_dma_channel,
        ESAI_TX_DMA_CHANNEL);

    result = dma_channel_setup(esai_fifo_node->tx_dma_channel,
            ESAI_MAX_TX_DMA_PACKET_NUM,
            0);

    result = dma_request_source(esai_fifo_node->tx_dma_channel,
            DMA_REQ_ESAI_TX);

    if(MQX_OK != result){
        ESAI_FIFO_ERROR("[ESAI] TX dma_request failed\n");
        return ESAI_NODE_TX_DMA_REQUEST_FAIL;
    }

    result = dma_callback_reg(esai_fifo_node->tx_dma_channel,
                (DMA_EOT_CALLBACK)_esai_fifo_tx_dma_callback_w,
                (void*)esai_fifo_node);

    if(MQX_OK != result){
        dma_channel_release(esai_fifo_node->tx_dma_channel);

        ESAI_FIFO_ERROR("[ESAI] TX dma_set_callback failed\n");
        return ESAI_NODE_TX_DMA_CB_SET_FAIL;
    }

        result = dma_channel_claim(&esai_fifo_node->rx_dma_channel,
            ESAI_RX_DMA_CHANNEL);

        result = dma_channel_setup(esai_fifo_node->rx_dma_channel,
            ESAI_MAX_TX_DMA_PACKET_NUM,
            0);

        result = dma_request_source(esai_fifo_node->rx_dma_channel,
            DMA_REQ_ESAI_RX);

        if(MQX_OK != result){
            ESAI_FIFO_ERROR("[ESAI] RX dma_request failed\n");
            return ESAI_NODE_RX_DMA_REQUEST_FAIL;
        }

        result = dma_callback_reg(esai_fifo_node->rx_dma_channel,
                (DMA_EOT_CALLBACK)_esai_fifo_rx_dma_callback_w,
                (void*)esai_fifo_node);

        if(MQX_OK != result){
            dma_channel_release(esai_fifo_node->rx_dma_channel);

            ESAI_FIFO_ERROR("[ESAI] RX dma_set_callback failed\n");
            return ESAI_NODE_RX_DMA_CB_SET_FAIL;
        }

    ESAI_FIFO_DEBUG("[ESAI] dma init finished\n");

    return ESAI_NODE_OK;

}


/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_deinit
* Returned Value   : uint32_t
* Comments         :
*    destory the ESAI fifo node
*
*END*********************************************************************/
uint32_t esai_fifo_node_deinit
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node
)
{
    if (NULL == esai_fifo_node)
        return ESAI_NODE_INVALID_PARAMETER;

    /*stop tx and rx*/
    esai_fifo_node_stop(esai_fifo_node,
        (void *)AUD_IO_FW_DIR_FULL_DUPLEX);

    /*disable ARM interrupt*/
    _bsp_int_disable(esai_fifo_node->esai_irq);

    if (esai_fifo_node->asrc_enable) {
#ifdef ASRC_HARDCODE_VERSION
        dma_channel_release(esai_fifo_node->tx_asrc_esai_channel);
#endif
    }

    if (esai_fifo_node->tx_dma_channel != NULL)
        dma_channel_release(esai_fifo_node->tx_dma_channel);

    if (esai_fifo_node->rx_dma_channel != NULL)
        dma_channel_release(esai_fifo_node->rx_dma_channel);

    _lwsem_destroy(&esai_fifo_node->tx_sem);
    _lwsem_destroy(&esai_fifo_node->rx_sem);

    _mutex_destroy(&esai_fifo_node->control_mutex);

    /*release buffer*/
    if (NULL != esai_fifo_node->tx_buffer)
        _mem_free(esai_fifo_node->tx_buffer);
    if (NULL != esai_fifo_node->rx_buffer)
        _mem_free(esai_fifo_node->rx_buffer);

    _mem_free(esai_fifo_node->tx_zero_buffer);
    _mem_free(esai_fifo_node->rx_zero_buffer);

    _esai_fifo_tx_buflist_deinit(esai_fifo_node);

#ifdef __ESAI_24BIT_DMA_UNSUPPORT
    if (esai_fifo_node->padding_buffer != NULL) {
        _mem_free(esai_fifo_node->padding_buffer);
    }
    esai_fifo_node->padding_buf_length = 0;
    esai_fifo_node->padding_buf_data_length = 0;
#endif

    ESAI_FIFO_DEBUG("[ESAI] TX_underrun = %u\n"
        "[ESAI] TX_write_times = %u\n"
        "[ESAI] TX_buffer_empty = %u\n",
        esai_fifo_node->tx_underrun,
        esai_fifo_node->write_times,
        esai_fifo_node->tx_buffer_empty);

    ESAI_FIFO_DEBUG("[ESAI] esai_fifo_node_deinit\n");

    return ESAI_NODE_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : easi_fifo_set_asrc_dma
* Returned Value   : uint32_t
* Comments         :
*    This function is used to configure ASRC channels to ESAI, so a dma based link can be set up
*
*END*********************************************************************/
uint32_t easi_fifo_set_asrc_dma
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node,

    /* [IN] asrc input dma channel id*/
    DMA_CHANNEL_HANDLE input_dma_channel,

    /* [IN] asrc output dma channel id*/
    DMA_CHANNEL_HANDLE output_dma_channel
)
{
    int32_t result = 0;

    if (NULL == esai_fifo_node)
        return ESAI_NODE_INVALID_PARAMETER;

    if (esai_fifo_node->tx_state == ESAI_STATE_RUN)
        return ESAI_NODE_TX_STATE_ERROR;

    if (!esai_fifo_node->asrc_enable && input_dma_channel != NULL
        && output_dma_channel != NULL) {
        esai_fifo_node->asrc_enable = TRUE;

        dma_channel_release(esai_fifo_node->tx_dma_channel);

        esai_fifo_node->tx_dma_channel = input_dma_channel;
        esai_fifo_node->tx_asrc_esai_channel = output_dma_channel;

        result = dma_callback_reg(esai_fifo_node->tx_dma_channel,
                    _esai_fifo_tx_dma_callback_w,
                    (void*)esai_fifo_node);

        if(MQX_OK != result){
            dma_channel_release(esai_fifo_node->tx_dma_channel);

            ESAI_FIFO_ERROR("[ESAI] dma_set_callback failed\n");
            return ESAI_NODE_TX_DMA_CB_SET_FAIL;
        }

    } else if (esai_fifo_node->asrc_enable &&
        (input_dma_channel == NULL || output_dma_channel == NULL)){

        esai_fifo_node->asrc_enable = FALSE;
        esai_fifo_node->tx_dma_channel = NULL;
        esai_fifo_node->tx_asrc_esai_channel = NULL;

        dma_channel_claim(&esai_fifo_node->tx_dma_channel,
                            ESAI_TX_DMA_CHANNEL);
        dma_channel_setup(esai_fifo_node->tx_dma_channel,
                            ESAI_MAX_TX_DMA_PACKET_NUM,
                            0);
        dma_request_source(esai_fifo_node->tx_dma_channel,
                            DMA_REQ_ESAI_TX);

        if(MQX_OK != result){
            ESAI_FIFO_ERROR("[ESAI] ESAI_TX dma_request failed %d\n", result);
            return ESAI_NODE_TX_DMA_REQUEST_FAIL;
        }
        result = dma_callback_reg(esai_fifo_node->tx_dma_channel,
                                (DMA_EOT_CALLBACK)_esai_fifo_tx_dma_callback_w,
                                (void*)esai_fifo_node);

        if(MQX_OK != result){
            dma_channel_release(esai_fifo_node->tx_dma_channel);
            ESAI_FIFO_ERROR("[ESAI] dma_set_callback failed\n");
            return ESAI_NODE_TX_DMA_CB_SET_FAIL;
        }
    } else {
        ESAI_FIFO_ERROR("[ESAI] easi_fifo_set_asrc_dma parameters error\n");
        return ESAI_NODE_INVALID_PARAMETER;
    }

    return ESAI_NODE_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_enable_asrc
* Returned Value   : uint32_t
* Comments         :
*    This function is depreciated, only work for asrc plugin codes
*
*END*********************************************************************/
uint32_t esai_fifo_node_enable_asrc
(
    /* [IN] ESAI FIFO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node,
    bool enable,
    int32_t source_sample_rate
)
{
#ifdef ASRC_HARDCODE_VERSION
    int32_t result = 0;
    uint32_t i;
    ASRC_MemMapPtr asrc_regs = ASRC_BASE_PTR;

    if (NULL == esai_fifo_node)
        return ESAI_NODE_INVALID_PARAMETER;

    if (esai_fifo_node->tx_state == ESAI_STATE_RUN)
        return ESAI_NODE_TX_STATE_ERROR;

    if (enable && !esai_fifo_node->asrc_enable) {
        esai_fifo_node->asrc_enable = TRUE;

        ESAI_FIFO_DEBUG("[ESAI] enable asrc plugin\n");

        dma_channel_release(esai_fifo_node->tx_dma_channel);

        result = dma_channel_claim(&esai_fifo_node->tx_asrc_esai_channel,
                    ESAI_TX_ASRC_DMA_CHANNEL);
        result = dma_channel_setup(esai_fifo_node->tx_asrc_esai_channel,
                       ESAI_MAX_TX_DMA_PACKET_NUM , DMA_REQ_ASRCA_TX)
        /* !! FIXME: probably missing DMA request source configuration */

        if(MQX_OK != result){
            ESAI_FIFO_ERROR("[ESAI] dma_request failed, ASRCA_TX\n");
            return ESAI_NODE_TX_DMA_REQUEST_FAIL;
        }

        //Configure the DMA tcd
        esai_fifo_node->tx_asrc_dma_packet.SRC_ADDR =
                        (uint32_t)((uint32_t *)&(asrc_regs->ASRDOA));
        esai_fifo_node->tx_asrc_dma_packet.DST_ADDR = 0;
        esai_fifo_node->tx_asrc_dma_packet.SRC_WIDTH = 4;
        esai_fifo_node->tx_asrc_dma_packet.SRC_MODULO = 0;
        esai_fifo_node->tx_asrc_dma_packet.SRC_OFFSET = 0;
        esai_fifo_node->tx_asrc_dma_packet.DST_MODULO = 0;
        esai_fifo_node->tx_asrc_dma_packet.DST_OFFSET = 0;
        esai_fifo_node->tx_asrc_dma_packet.DST_WIDTH = 4;

        if (MQX_OK != result) {
            ESAI_FIFO_DEBUG("source edma_channel_quick_config failed\n");
            return -5;
        }

        result = dma_channel_claim(&esai_fifo_node->tx_dma_channel,
                                    ESAI_TX_DMA_CHANNEL);
        result = dma_channel_setup(esai_fifo_node->tx_dma_channel,
                            ESAI_MAX_TX_DMA_PACKET_NUM,
                            0);
        result = dma_request_source(esai_fifo_node->tx_dma_channel,
                            DMA_REQ_ASRCA_RX);

        if(ESAI_NODE_OK != result){
            ESAI_FIFO_ERROR("[ESAI] dma_request failed, ASRCA_RX\n");
            return ESAI_NODE_TX_DMA_REQUEST_FAIL;
        }

        result = dma_callback_reg(esai_fifo_node->tx_dma_channel,
                    (DMA_EOT_CALLBACK)_esai_fifo_tx_dma_callback_w,
                    (void*)esai_fifo_node);

        if(ESAI_NODE_OK != result){
            dma_channel_release(esai_fifo_node->tx_dma_channel);

            ESAI_FIFO_ERROR("[ESAI] dma_set_callback failed\n");
            return ESAI_NODE_TX_DMA_CB_SET_FAIL;
        }

        for (i = 0; i < ESAI_MAX_TX_DMA_PACKET_NUM; i++)
        {
             dma_tcd_mem2reg(&esai_fifo_node->tx_dma_packets[i],
                        (volatile void *)&(asrc_regs->ASRDIA),
                        4,
                        NULL,
                        0);
        }

        esai_fifo_node->tx_asrc_src_sample_rate = source_sample_rate;

    } else if (!enable && esai_fifo_node->asrc_enable) {
        esai_fifo_node->asrc_enable = FALSE;

        dma_channel_release(esai_fifo_node->tx_dma_channel);
        dma_channel_release(esai_fifo_node->tx_asrc_esai_channel);

        result = dma_channel_claim(&esai_fifo_node->tx_dma_channel,
                                    ESAI_TX_DMA_CHANNEL);
        result = dma_channel_setup(esai_fifo_node->tx_dma_channel,
                            ESAI_MAX_TX_DMA_PACKET_NUM,
                            0);
        result = dma_request_source(esai_fifo_node->tx_dma_channel,
                            DMA_REQ_ESAI_TX);

        if(MQX_OK != result){
            ESAI_FIFO_ERROR("[ESAI] dma_request failed, DMA_DEVICE_ESAI_TX_24BIT\n");
            return ESAI_NODE_TX_DMA_REQUEST_FAIL;
        }

        result = dma_callback_reg(esai_fifo_node->tx_dma_channel,
                    (DMA_EOT_CALLBACK)_esai_fifo_tx_dma_callback_w,
                    (void*)esai_fifo_node);
        if(MQX_OK != result){
            dma_channel_release(esai_fifo_node->tx_dma_channel);

            ESAI_FIFO_ERROR("[ESAI] dma_set_callback failed\n");
            return ESAI_NODE_TX_DMA_CB_SET_FAIL;
        }

        esai_fifo_node->tx_asrc_src_sample_rate = 0;
    }

    return ESAI_NODE_OK;

#else
    return ESAI_NODE_OUT_OF_RESOURCE_LIMIT;
#endif

}

/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_clear_tx_buf
* Returned Value   : int32_t
* Comments         :
*    Clear the esai fifo node TX buffer and reset all buffer variables
*
*END*********************************************************************/
int32_t esai_fifo_node_clear_tx_buf
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node
)
{
    uint8_t num = 0;

    _int_disable();

    num = node->tx_buffer_window;
    if (num > 0) {
        node->tx_packet_count -= num;
        node->tx_buffer_window = 0;
        node->tx_in = node->tx_out;
        while (num--)
            _lwsem_post(&node->tx_sem);
    }
    _int_enable();

    return ESAI_NODE_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_clear_rx_buf
* Returned Value   : int32_t
* Comments         :
*    Clear the esai fifo node RX buffer and reset all buffer variables
*
*END*********************************************************************/
int32_t esai_fifo_node_clear_rx_buf
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node
)
{
    _int_disable();
    node->rx_out = node->rx_in;
    node->rx_packet_count = 0;
    _int_enable();

    return ESAI_NODE_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_tx_error
* Returned Value   : int32_t
* Comments         :
*    Get the esai fifo tx error code
*
*END*********************************************************************/
uint32_t esai_fifo_node_tx_error
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node
)
{
    return node->tx_error_code;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_rx_error
* Returned Value   : int32_t
* Comments         :
*    Get the esai fifo rx error code
*
*END*********************************************************************/
uint32_t esai_fifo_node_rx_error
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node
)
{
    return node->rx_error_code;
}


/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_set_tx_timeout
* Returned Value   : int32_t
* Comments         :
*    set the tx timeout to the esai fifo
*
*END*********************************************************************/
uint32_t esai_fifo_node_set_tx_timeout
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node,

    /* [IN] timeout duration */
    uint32_t timeout
)
{
    _mutex_lock(&node->control_mutex);

    if (0 == timeout) {
        node->tx_timeout = 0;
    } else {
        node->tx_timeout = (timeout * _time_get_ticks_per_sec()) / 1000 ;
        if (0 == node->tx_timeout)
            node->tx_timeout = 1;
    }

    ESAI_FIFO_DEBUG("[ESAI] SET TX TIMEOUT %u\n",
            node->tx_timeout);

    _mutex_unlock(&node->control_mutex);

    return MQX_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : esai_fifo_node_set_rx_timeout
* Returned Value   : int32_t
* Comments         :
*    set the rx timeout to the esai fifo
*
*END*********************************************************************/
uint32_t esai_fifo_node_set_rx_timeout
(
    /* [IN] ESAI FIFO AUD IO node pointer */
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR node,

    /* [IN] timeout duration */
    uint32_t timeout
)
{
    _mutex_lock(&node->control_mutex);

    if (0 == timeout) {
        node->rx_timeout = 0;
    } else {
        node->rx_timeout = (timeout * _time_get_ticks_per_sec()) / 1000 ;
        if (0 == node->rx_timeout)
            node->rx_timeout = 1;
    }

    ESAI_FIFO_DEBUG("[ESAI] SET RX TIMEOUT %u\n",
            node->rx_timeout);

    _mutex_unlock(&node->control_mutex);

    return MQX_OK;
}


/* EOF */
