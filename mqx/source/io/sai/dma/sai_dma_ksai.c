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
*   This file contains low level functions for the SAI interrupt device driver
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "sai_int_prv.h"
#include "sai_ksai_prv.h"
#include "edma.h"
#include "dma.h"

#define DBGPRT

//for 96K, stereo, a uint32_t value will rewind after about 6 hours
//so rather than a>=b, use below macro.
#define NOLESS(a, b) 		((int32_t)(a) - (int32_t)(b) >= 0)
#define LESS(a, b) 			((int32_t)(a)- (int32_t)(b) < 0)
#define GREATER(a, b) 		((int32_t)(a) - (int32_t)(b) > 0)


#if PSP_MQX_CPU_IS_VYBRID // BEGIN - Platform specific macros + functions for Vybrid
#define PLL_DIV_MAX 16
#define SAI_DIV_MAX 16
#define I2S_RX_FIFO_FULL_MASK 0x20
/*
** Values of Vybrid watermarks are set to 
** experimentally tested default values.
*/
#define I2S_TX_FIFO_DEFAULT_WATERMARK 16
#define I2S_RX_FIFO_DEFAULT_WATERMARK 16

#define I2S_RX_WRITE_PTR_MASK 0x1F
#define I2S_RX_READ_PTR_MASK  0x1F
#define I2S_TX_WRITE_PTR_MASK 0x1F
#define I2S_TX_READ_PTR_MASK  0x1F

static uint32_t _vi2s_get_mclk_value(KSAI_INFO_STRUCT * io_info_ptr);
static void _vi2s_set_mclk_freq(KSAI_INFO_STRUCT * io_info_ptr, uint32_t freq);
static void _vi2s_set_clk_src_int(uint8_t hw_channel);
static void _vi2s_set_clk_src_ext(uint8_t hw_channel);
static void _vi2s_set_sai_div(uint8_t hw_channel, uint8_t value);
#else //  Platform specific macros + functions for Kinetis


#define I2S_RX_FIFO_FULL_MASK 0x10
/*
** Values of Kinetis watermarks are set to 
** experimentally tested default values.
*/
#define I2S_TX_FIFO_DEFAULT_WATERMARK 4
#define I2S_RX_FIFO_DEFAULT_WATERMARK 4

#define I2S_RX_WRITE_PTR_MASK 0x7
#define I2S_RX_READ_PTR_MASK  0x7
#define I2S_TX_WRITE_PTR_MASK 0x7
#define I2S_TX_READ_PTR_MASK  0x7

static uint32_t _ki2s_get_mclk_value(KSAI_INFO_STRUCT * io_info_ptr);
#endif // END - Platform specific macros


#define MYCHANNELS 2

void _ksai_tx_fifoerr_irq_handler(void *);
void _ksai_rx_fifoerr_irq_handler(void *);


void _ksai_tx_dma_irq_handler(void *, int, uint32_t);
void _ksai_rx_dma_irq_handler(void *, int, uint32_t);


void _ksai_hw_init(KSAI_INIT_STRUCT *, I2S_MemMapPtr, _mqx_int);
uint32_t _ki2s_find_mclk_freq_div(CM_CLOCK_SOURCE src, uint16_t * frct, uint16_t * div, uint32_t frequency);
uint32_t _ki2s_set_master_mode(KSAI_INFO_STRUCT * io_info_ptr, uint8_t mode);
uint32_t _ki2s_set_slave_mode(KSAI_INFO_STRUCT * io_info_ptr, uint8_t mode);
uint32_t _ki2s_set_bclk_mode(I2S_MemMapPtr i2s_ptr, uint8_t bclk_mode);


/*FUNCTION****************************************************************
*
* Function Name    : _ksai_dma_init
* Returned Value   : MQX error code, -1 if the init structure is null.
* Comments         :
*    This function initializes an SAI device.
*
*END*********************************************************************/

_mqx_int _ksai_dma_init
(
    /* [IN] Initialization information for the device being opened */
    void * io_dev_ptr
)
{   /* Body */
    IO_SAI_DEVICE_STRUCT *    io_sai_dev_ptr = (IO_SAI_DEVICE_STRUCT *)io_dev_ptr;
    KSAI_INIT_STRUCT *        ksai_init_ptr = (KSAI_INIT_STRUCT *)io_sai_dev_ptr->DEV_INIT_DATA_PTR;
    KSAI_INFO_STRUCT *        ksai_info_ptr;
    //Initialize the two semaphore.
    I2S_STATISTICS_STRUCT *stats;
    I2S_MemMapPtr               i2s_ptr = NULL;
    _mqx_uint                   tx_vector, rx_vector, buffer_size;
    _mqx_uint                   result = 0;
    uint8_t                      channel, mode, data_bits, clk_source, endianity, aligment, bits, size, channels, bclk_mode;
    uint16_t       dma_channel = 0;
    uint8_t         dma_channel_mask = 0;
    uint16_t       dma_channel_start, dma_channel_end;

    /* Check init structure */
    if (ksai_init_ptr == NULL)
    {
        I2S_LOG("\n  _i2s_init: Error - ksai_init_ptr is NULL");
        return I2S_ERROR_INVALID_PARAMETER;
    }
    else
    {
        channel     = ksai_init_ptr->HW_CHANNEL;
        mode        = ksai_init_ptr->MODE;
        data_bits   = ksai_init_ptr->DATA_BITS;
        clk_source  = ksai_init_ptr->CLOCK_SOURCE;
        buffer_size = ksai_init_ptr->BUFFER_SIZE * sizeof(uint32_t);/*The buffer should aligned by char*/
        endianity   = ksai_init_ptr->IO_FORMAT->ENDIAN;
        aligment    = ksai_init_ptr->IO_FORMAT->ALIGNMENT;
        bits        = ksai_init_ptr->IO_FORMAT->BITS;
        size        = ksai_init_ptr->IO_FORMAT->SIZE;
        channels    = ksai_init_ptr->IO_FORMAT->CHANNELS;
        bclk_mode   = ksai_init_ptr->CLOCK_MODE;
    }
    /* Check if valid data bit length is selected */
    if ((data_bits < AUDIO_BIT_SIZE_MIN) || (data_bits > AUDIO_BIT_SIZE_MAX))
    {
        I2S_LOG("\n  _i2s_init: Error - Unsupported data word length.");
        return (I2S_ERROR_WORD_LENGTH_UNSUPPORTED);
    }   
    /* Check mode of operation */
    if (mode > (I2S_TX_SLAVE | I2S_RX_SLAVE))
    {
        I2S_LOG("\n  _i2s_init: Error - Invalid I2S mode.");
        return(I2S_ERROR_MODE_INVALID);
    }
    /* Check clock source */
    if (clk_source > I2S_CLK_EXT)
    {
        I2S_LOG("\n  _i2s_init: Error - Invalid I2S clock source.");
        return(I2S_ERROR_CLK_INVALID);
    }
    /* Check buffer size */
    if (buffer_size < 2)
    {
        I2S_LOG("\n  _i2s_init: Error - I2S transmit buffer size too small.");
        return(I2S_ERROR_BUFFER_SMALL);
    }

    /* Check bit clock setting */
    if ((bclk_mode & I2S_TX_SYNCHRONOUS) && (bclk_mode & I2S_RX_SYNCHRONOUS))
    {
        I2S_LOG("\n  _i2s_init: Error - Invalid bit clock configuration selected.");
        return(I2S_ERROR_INVALID_BCLK);
    }
    /* Check audio data format */
    if (
        (aligment > AUDIO_ALIGNMENT_LEFT) || (bits < AUDIO_BIT_SIZE_MIN) ||
        (bits > AUDIO_BIT_SIZE_MAX) || (endianity > AUDIO_LITTLE_ENDIAN) ||
        (size * I2S_CHAR_BIT < bits) || (channels > 2) || (channels == 0)
       )
    {
        I2S_LOG("\n  _i2s_init: Error - Invalid I/O data format.");
        return(AUDIO_ERROR_INVALID_IO_FORMAT);
    }
    /* Check selected data channels */
    if (ksai_init_ptr->TX_CHANNEL >= SAI_DATA_CHANNELS)
    {
        I2S_LOG("\n  _i2s_init: Error - Invalid transmitter data channel selected.");
        return(I2S_ERROR_INVALID_TX_CHANNEL);
    }
    if (ksai_init_ptr->RX_CHANNEL >= SAI_DATA_CHANNELS)
    {
        I2S_LOG("\n  _i2s_init: Error - Invalid receiver data channel selected.");
        return(I2S_ERROR_INVALID_RX_CHANNEL);
    }
    /* Check if valid peripheral is selected */
    i2s_ptr = _bsp_get_sai_base_address(channel);
    if (i2s_ptr == NULL)
    {
        I2S_LOG("\n  _i2s_init: Error - Invalid hardware channel selected.");
        return(I2S_ERROR_CHANNEL_INVALID);
    }
    ksai_info_ptr = (KSAI_INFO_STRUCT *) _mem_alloc_system_zero_uncached((uint32_t)sizeof(KSAI_INFO_STRUCT));
    if (NULL == ksai_info_ptr)
    {
        I2S_LOG("\n  _i2s_init: Error - Allocation of KI2S_INFO_STRUCT failed.");
        return(MQX_OUT_OF_MEMORY);
    }
    _mem_set_type (ksai_info_ptr, MEM_TYPE_IO_I2S_INFO_STRUCT);


    /* If the device is not alreadz open do I/O initialization */
    if ((io_sai_dev_ptr->READ_COUNT == 0) && (io_sai_dev_ptr->WRITE_COUNT == 0))
    {
        /* SAI board specific IO pins initialization */
        _bsp_sai_io_init(channel); 
    }
    /* Copy clock source to context structure */
    ksai_info_ptr->MCLK_SRC = ksai_init_ptr->MCLK_SRC;
    /* SAI board specific registers initialization */
    _ksai_hw_init(ksai_init_ptr, i2s_ptr, io_sai_dev_ptr->FLAGS);

    /*Info struct setup*/
    ksai_info_ptr->INIT           = *ksai_init_ptr;
    ksai_info_ptr->SAI_PTR        = i2s_ptr;
    ksai_info_ptr->MODE           = ksai_init_ptr->MODE;
    ksai_info_ptr->DATA_BITS      = ksai_init_ptr->DATA_BITS;
    ksai_info_ptr->CLOCK_SOURCE   = ksai_init_ptr->CLOCK_SOURCE;
    ksai_info_ptr->CLOCK_MODE     = ksai_init_ptr->CLOCK_MODE;
    ksai_info_ptr->TX_CHANNEL     = ksai_init_ptr->TX_CHANNEL;
    ksai_info_ptr->RX_CHANNEL     = ksai_init_ptr->RX_CHANNEL;
    ksai_info_ptr->IO_MODE        = io_sai_dev_ptr->FLAGS;
    ksai_info_ptr->HW_CHANNEL     = channel;

    ksai_info_ptr->IO_FORMAT.ENDIAN       = endianity;
    ksai_info_ptr->IO_FORMAT.BITS         = bits;
    ksai_info_ptr->IO_FORMAT.ALIGNMENT    = aligment;
    ksai_info_ptr->IO_FORMAT.SIZE         = size;
    ksai_info_ptr->IO_FORMAT.CHANNELS     = channels;

    if (ksai_info_ptr->CLOCK_SOURCE == I2S_CLK_INT)
    {
#if PSP_MQX_CPU_IS_VYBRID
    ksai_info_ptr->MCLK_FREQ  = _vi2s_get_mclk_value(ksai_info_ptr);
#else
    ksai_info_ptr->MCLK_FREQ  = _ki2s_get_mclk_value(ksai_info_ptr);
#endif
    ksai_info_ptr->BCLK_FREQ  = ksai_info_ptr->MCLK_FREQ / DEFAULT_BCLK_DIV;
    ksai_info_ptr->FS_FREQ    = ksai_info_ptr->BCLK_FREQ / (ksai_info_ptr->DATA_BITS * ksai_info_ptr->IO_FORMAT.CHANNELS);
    }
    else
    {
        ksai_info_ptr->MCLK_FREQ  = 0;
        ksai_info_ptr->BCLK_FREQ  = 0;
        ksai_info_ptr->FS_FREQ    = 0;
    }
    ksai_info_ptr->BCLK_DIV = DEFAULT_BCLK_DIV;
    ksai_info_ptr->TX_DUMMY = ksai_init_ptr->TX_DUMMY;
    ksai_info_ptr->ONTHEWAY = FALSE;
    ksai_info_ptr->DATA = NULL;
    ksai_info_ptr->DUMMY.DATA     = NULL;
    ksai_info_ptr->DUMMY.LENGTH   = 0;
    ksai_info_ptr->DUMMY.INDEX    = 0;
    ksai_info_ptr->CLK_ALWAYS_ON  = TRUE;
    ksai_info_ptr->FIRST_IO       = TRUE;
    ksai_info_ptr->DMA_KICKOFF    = FALSE;

    /* Buffers allocation, 4 DMA buffers in default, if the user condigure the 
     * buffer, free the default buffer */
    ksai_info_ptr->BUFFER.DMA_DATA = (uint8_t *)_mem_alloc_system_zero_uncached(buffer_size * 4);
    if (ksai_info_ptr->BUFFER.DMA_DATA == NULL)
    {
        return MQX_OUT_OF_MEMORY;
    }
    ksai_info_ptr->BUFFER.SPACE = buffer_size;
    ksai_info_ptr->BUFFER.SIZE  = buffer_size;
    ksai_info_ptr->BUFFER.PERIODS = 4;
    _mem_set_type (ksai_info_ptr->BUFFER.DMA_DATA, MEM_TYPE_IO_I2S_LEFT_BUFFER);

    ksai_info_ptr->BUFFER.START_PERIOD = ksai_info_ptr->BUFFER.DMA_DATA;
    ksai_info_ptr->BUFFER.END_PERIOD= ksai_info_ptr->BUFFER.DMA_DATA + 3 * buffer_size;

   //Initialize the stats structure.
   stats = &(ksai_info_ptr->STATS);
   if(_lwsem_create(&(stats->BUFFER_EMPTY), 4) != MQX_OK)
   {
        I2S_LOG("\n  _i2s_init: Error - create lwsem BUFFER_EMPTY \n");
        return I2S_ERROR_INVALID_PARAMETER;
   }
   
   if(_lwsem_create(&(stats->BUFFER_FULL), 0) != MQX_OK)
   {
        I2S_LOG("\n  _i2s_init: Error - create lwsem BUFFER_FULL \n");
        return I2S_ERROR_INVALID_PARAMETER;
   }
   stats->IN_BUFFER = ksai_info_ptr->BUFFER.START_PERIOD;
   stats->OUT_BUFFER = ksai_info_ptr->BUFFER.START_PERIOD;
   stats->SIZE = buffer_size;

   io_sai_dev_ptr->DEV_INFO_PTR = ksai_info_ptr;
   //used for flush
   ksai_info_ptr->IO_SAI_DEV_PTR = io_sai_dev_ptr;
   uint8_t shift_number = 0;
   //dma initialize
   dma_channel_mask = _bsp_get_sai_dma_channel_mask(ksai_info_ptr->HW_CHANNEL);
   while(dma_channel_mask)
   {
       if(dma_channel_mask & 1)
       {
           dma_channel_start = 16 * shift_number;
           dma_channel_end = 16 + dma_channel_start;
           dma_channel = dma_channel_start;
           result = dma_channel_claim(&ksai_info_ptr->DCH, dma_channel);
           while((dma_channel < dma_channel_end) && (result == MQX_NOT_RESOURCE_OWNER))
           {
               dma_channel ++;
               result = dma_channel_claim(&ksai_info_ptr->DCH, dma_channel);
           }
           if(result == MQX_OK)
           {
               break;
           }
       }
       dma_channel_mask >>= 1;
       shift_number += 1;
   }
   if(result != MQX_OK)
   {
       I2S_LOG("\n  _i2s_init: Error - dma_request_claim 0x%x\n", result);
       return I2S_ERROR_INVALID_PARAMETER;
   }
   if (io_sai_dev_ptr->FLAGS & I2S_IO_WRITE)
   {
       result = dma_callback_reg(ksai_info_ptr->DCH, _ksai_tx_dma_irq_handler, (void *)ksai_info_ptr);
       if (result != MQX_OK)
       {
           I2S_LOG("\n  _i2s_init: Error - dma_callback_reg 0x%x\n", result);
           return I2S_ERROR_INVALID_PARAMETER;
       }
       /* Interrupt vectors setup */
       tx_vector = _bsp_get_sai_tx_vector(channel);
       _int_install_isr(tx_vector, _ksai_tx_fifoerr_irq_handler, ksai_info_ptr);
       _bsp_int_init(tx_vector, ksai_info_ptr->INIT.LEVEL, 0, TRUE);
   }
   else
   {
       result = dma_callback_reg(ksai_info_ptr->DCH, _ksai_rx_dma_irq_handler,(void *)ksai_info_ptr);
       if (result != MQX_OK)
       {
           I2S_LOG("\n  _i2s_init: Error - dma_callback_reg RX_DMA_CHN 0x%x\n", result);
           return I2S_ERROR_INVALID_PARAMETER;
       }
       //interrupt initialize
       rx_vector = _bsp_get_sai_rx_vector(channel);
       _int_install_isr(rx_vector, _ksai_rx_fifoerr_irq_handler, ksai_info_ptr);
       _bsp_int_init(rx_vector, ksai_info_ptr->INIT.LEVEL, 0, TRUE);
   }
   return MQX_OK;
} /* End body */

/*FUNCTION****************************************************************
*
* Function Name    : _ksai_dma_deinit
* Returned Value   : MQX error code
* Comments         :
*    This function de-initializes an SAI device.
*
*END*********************************************************************/

_mqx_int _ksai_dma_deinit
(
    /* [IN] Initialization information for the device being closed */
    void * io_dev_ptr,
    
    void * io_info_ptr
)
{
    IO_SAI_DEVICE_STRUCT *    io_sai_dev_ptr = (IO_SAI_DEVICE_STRUCT *)io_dev_ptr;
    KSAI_INFO_STRUCT *        ksai_info_ptr = (KSAI_INFO_STRUCT *)io_info_ptr;
    I2S_MemMapPtr               i2s_ptr;
    _mqx_uint                   errcode;
    _mqx_uint                   result = 0;
    
    if ((NULL == ksai_info_ptr) || (NULL == io_sai_dev_ptr))
    {
        I2S_LOG("\n  _i2s_deinit: Error - Null void *.");
        return I2S_ERROR_INVALID_PARAMETER;
    }
    
    i2s_ptr = ksai_info_ptr->SAI_PTR;

    if (io_sai_dev_ptr->FLAGS & I2S_IO_WRITE)
    {
        dma_request_disable(ksai_info_ptr->DCH);
        dma_channel_release(ksai_info_ptr->DCH);
        
        i2s_ptr->TCSR &= ~I2S_TCSR_TE_MASK;
        /* Disable DMA request. */
        i2s_ptr->TCSR &= ~(I2S_TCSR_FWDE_MASK | I2S_TCSR_FRDE_MASK);

        i2s_ptr->TCSR &= ~I2S_TCSR_FEIE_MASK;
    }
    else
    {
        dma_request_disable(ksai_info_ptr->DCH);
        dma_channel_release(ksai_info_ptr->DCH);
        i2s_ptr->RCSR &= ~I2S_RCSR_RE_MASK;
        /* Disable DMA request. */
        i2s_ptr->RCSR &= ~(I2S_RCSR_FWDE_MASK | I2S_RCSR_FRDE_MASK);

        i2s_ptr->RCSR &= ~I2S_RCSR_FEIE_MASK;
    }

#if !PSP_MQX_CPU_IS_VYBRID
    i2s_ptr->MCR &= ~(I2S_MCR_MOE_MASK);
#endif
    /* Destroy event */
    result = _lwsem_destroy(&(ksai_info_ptr->STATS.BUFFER_EMPTY));
    if (result != MQX_OK)
    {
        I2S_LOG("\n  _i2s_deinit: Error - unable to destroy event_int_done.\n");
        _task_set_error(result);
    }
    
    result = _lwsem_destroy(&(ksai_info_ptr->STATS.BUFFER_FULL));
    if (result != MQX_OK)
    {
        I2S_LOG("\n  _i2s_deinit: Error - unable to destroy event_int_done.\n");
        _task_set_error(result);
    }

    ksai_info_ptr->ONTHEWAY = FALSE;
    
    /* Free buffers and info struct */
    errcode = _mem_free(ksai_info_ptr->BUFFER.DMA_DATA);
    if (errcode != MQX_OK)
    {
        I2S_LOG("\n  _i2s_deinit: Error - Free data buffer failed.");
        return errcode;
    }
    _mem_zero(&(ksai_info_ptr->BUFFER), sizeof(ksai_info_ptr->BUFFER));

    if (ksai_info_ptr->TX_DUMMY)
    {
        errcode = _mem_free (ksai_info_ptr->DUMMY.DATA);
        if (errcode != MQX_OK)
        {
            I2S_LOG("\n  _i2s_deinit: Error - Free dummy data failed.");
            return errcode;
        }
        ksai_info_ptr->DUMMY.DATA = NULL;
        _mem_zero(&(ksai_info_ptr->DUMMY), sizeof(ksai_info_ptr->DUMMY));
    }
    
    errcode = _mem_free(io_sai_dev_ptr->DEV_INFO_PTR);
    if (errcode != MQX_OK)
    {
        I2S_LOG("\n  _i2s_deinit: Error - Free device information void * failed.");
        return errcode;
    }
    io_sai_dev_ptr->DEV_INFO_PTR = NULL;

    return(MQX_OK);
}

/*FUNCTION****************************************************************
*
* Function Name    : _ksai_tx_fifoerr_irq_handler
* Returned Value   : void
* Comments         : Tx FIFO error interrupt
*
*END*********************************************************************/
void _ksai_tx_fifoerr_irq_handler(void * param)
{
    KSAI_INFO_STRUCT *       ksai_info_ptr = (KSAI_INFO_STRUCT *) param;
    I2S_STATISTICS_STRUCT *  stats = &(ksai_info_ptr->STATS);
    I2S_MemMapPtr              i2s_ptr = ksai_info_ptr->SAI_PTR;
    /* Reset FIFO */
    i2s_ptr->TCSR |= I2S_TCSR_FR_MASK;
    i2s_ptr->TCSR |= I2S_TCSR_FEF_MASK;
    
    stats->FIFO_ERROR++;

    /*It seems that we should close the dma?*/
}

/*FUNCTION****************************************************************
*
* Function Name    : _ksai_tx_dma_irq_handler
* Returned Value   : void
* Comments         : Tx DMA callback function.
*
*END*********************************************************************/
void _ksai_tx_dma_irq_handler
(
    /* [IN] Parameter void * */
    void * param,
    int  tcd_done,
    uint32_t tcd_seq
)
{
    KSAI_INFO_STRUCT *       ksai_info_ptr = (KSAI_INFO_STRUCT *) param;
    I2S_STATISTICS_STRUCT *  stats = &(ksai_info_ptr->STATS);
    KSAI_BUFFER *	buffer = &(ksai_info_ptr->BUFFER);
    I2S_MemMapPtr              i2s_ptr = ksai_info_ptr->SAI_PTR;
    
    stats->INTERRUPTS++;
    stats->PACKETS_PROCESSED += ksai_info_ptr->BUFFER.SIZE;
    /* Update the output buffer */
    if(stats->OUT_BUFFER == buffer->END_PERIOD)
    {
        stats->OUT_BUFFER = buffer->START_PERIOD;
    }
    else
    {
        stats->OUT_BUFFER += buffer->SIZE;
    }
    /* Check FIFO error flag */
    if (i2s_ptr->TCSR & I2S_TCSR_FEF_MASK)
    {
        i2s_ptr->TCSR |= I2S_TCSR_FR_MASK;
        stats->FIFO_ERROR++;
    }
    //See if the data transferring is dirty or the transfer is complete.
    //That means the data from buffer is slow, and the dma is transferring the old data.
    //Use the input index and output index.
    //As the dma is still transferring during the callback, so there are still some dirty data.
    if (stats->IN_BUFFER == stats->OUT_BUFFER)
    {
        ksai_info_ptr->FIRST_IO = TRUE;
        /* Disable DMA request. */
        i2s_ptr->TCSR &= ~(I2S_TCSR_FWDE_MASK | I2S_TCSR_FRDE_MASK);
        i2s_ptr->TCSR &= ~I2S_TCSR_FEIE_MASK;
        i2s_ptr->TCSR &= ~I2S_TCSR_TE_MASK;
        //That means the data is slow.
        if(stats->PACKETS_QUEUED <= stats->PACKETS_REQUESTED)
        {
            stats->BUFFER_ERROR ++;
        }
    }
   //Clear the word start flag.
    i2s_ptr->TCSR |= I2S_TCSR_WSF_MASK;

    /* Data are ready in FIFO - clear error flag */
    i2s_ptr->TCSR |= I2S_TCSR_FEF_MASK;
     //Tells the application that a buffer is empty
    if (_lwsem_post(&(stats->BUFFER_EMPTY)) != MQX_OK)
    {
        I2S_LOG("\n  i2s_tx_irq_handler: Error - Unable to set BUFFER EMPTY.");
    }
}

/*FUNCTION****************************************************************
*
* Function Name    : _ksai_rx_fifoerr_irq_handler
* Returned Value   : void
* Comments         : Rx FIFO error interrupt
*
*END*********************************************************************/
void _ksai_rx_fifoerr_irq_handler(void * param)
{
    KSAI_INFO_STRUCT *       ksai_info_ptr = (KSAI_INFO_STRUCT *) param;
    I2S_STATISTICS_STRUCT *  stats = &(ksai_info_ptr->STATS);
    I2S_MemMapPtr              i2s_ptr = ksai_info_ptr->SAI_PTR;
    /* Reset FIFO */
    i2s_ptr->RCSR |= I2S_RCSR_FR_MASK;
    i2s_ptr->RCSR |= I2S_RCSR_FEF_MASK;
    stats->FIFO_ERROR++;
}


/*FUNCTION****************************************************************
*
* Function Name    : _ksai_rx_dma_irq_handler
* Returned Value   : void
* Comment          :
*   RX DMA callback function of SAI driver
*
*END*********************************************************************/

void _ksai_rx_dma_irq_handler
(
    /* [IN] Parameter void * */
    void * param,
    int  tcd_done,
    uint32_t tcd_seq
) 
{
    KSAI_INFO_STRUCT *       ksai_info_ptr = (KSAI_INFO_STRUCT *)param;
    I2S_MemMapPtr              i2s_ptr = ksai_info_ptr->SAI_PTR;
    KSAI_BUFFER *            buffer = &(ksai_info_ptr->BUFFER);
    I2S_STATISTICS_STRUCT *  stats = &(ksai_info_ptr->STATS);

    stats->INTERRUPTS++;
    stats->PACKETS_QUEUED += buffer->SIZE;
    if(stats->IN_BUFFER == buffer->END_PERIOD)
    {
        stats->IN_BUFFER = buffer->START_PERIOD;
    }
    else
    {
        stats->IN_BUFFER += buffer->SIZE;
    }
    /* Check FIFO error flag */
    if (i2s_ptr->RCSR & I2S_RCSR_FEF_MASK)
    {
        /* Reset FIFO */
        i2s_ptr->RCSR |= I2S_RCSR_FEF_MASK;
        i2s_ptr->RCSR |= I2S_RCSR_FR_MASK;
        stats->FIFO_ERROR++;
    }
    //Check if the transfer is complete or buffer error.	
    if (stats->IN_BUFFER == stats->OUT_BUFFER)
    {
        ksai_info_ptr->FIRST_IO = TRUE;
        
        /* Disable RX. */
        i2s_ptr->RCSR &= ~I2S_RCSR_RE_MASK;

        /* Disable DMA request. */
        i2s_ptr->RCSR &= ~(I2S_RCSR_FWDE_MASK | I2S_RCSR_FRDE_MASK);
        
        i2s_ptr->RCSR &= ~I2S_RCSR_FEIE_MASK;
        //Check if is buffer error
        if(stats->PACKETS_QUEUED < stats->PACKETS_REQUESTED)
        {
            stats->BUFFER_ERROR ++;
        }
    }
    /*Tell the application there is a block is empty.*/
    if (_lwsem_post(&(stats->BUFFER_FULL)) != MQX_OK)
    {
        I2S_LOG("\n  i2s_rx_irq_handler: Error - Unable to set BUFFER EMPTY.");
    }
}


/*FUNCTION****************************************************************
*
* Function Name    : _ksai_dma_ioctl
* Returned Value   : MQX error code.
* Comments         :
*    This function performs miscellaneous services for
*    the SAI I/O device.
*
*END*********************************************************************/

_mqx_int _ksai_dma_ioctl
(
    /* [IN] the address of the device specific information */
    void *     io_sai_dev_ptr, 
    
    /* [IN] The command to perform */
    _mqx_int    cmd, 
    
    /* [IN] Parameters for the command */
    void *     param
)
{
    KSAI_INFO_STRUCT * io_info_ptr = (KSAI_INFO_STRUCT *) io_sai_dev_ptr;
    I2S_MemMapPtr i2s_ptr = io_info_ptr->SAI_PTR;
    uint32_t result = MQX_OK;
    uint32_t sample_rate = 0;
    uint8_t  bclk_mode = 0;
    KSAI_BUFFER *buffer = &(io_info_ptr->BUFFER);
    I2S_STATISTICS_STRUCT *stats = &(io_info_ptr->STATS);
    AUDIO_DATA_FORMAT *io_format = &(io_info_ptr->IO_FORMAT);
    uint32_t hwReq;
    uint32_t length;
    uint32_t i  = 0;
    SAI_BUFFER_CONFIG *buffer_config;
    switch (cmd)
    {
        case IO_IOCTL_I2S_SET_MODE_MASTER:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break; 
            }
            if (io_info_ptr->ONTHEWAY)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Device is busy.");
                result = I2S_ERROR_DEVICE_BUSY;
                break;
            }
            result = _ki2s_set_master_mode(io_info_ptr, *((uint8_t  *) param));
            if (result != MQX_OK)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid RX/TX master mode combination selected.");
                break;
            }
            break;
            
        case IO_IOCTL_I2S_SET_MODE_SLAVE:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break; 
            }
            if (io_info_ptr->ONTHEWAY)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Device is busy.");
                result = I2S_ERROR_DEVICE_BUSY;
                break;
            }
            result = _ki2s_set_slave_mode(io_info_ptr, *((uint8_t  *) param));
            if (result != MQX_OK)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid RX/TX slave mode combination selected.");
                break;
            }
            break;
            
        case IO_IOCTL_I2S_SET_CLOCK_SOURCE_INT:
            if (io_info_ptr->ONTHEWAY)
            {
               I2S_LOG("\n  _i2s_ioctl: Error - Cannot change I2S clock source while I/O actions are in progress.");
               result = I2S_ERROR_DEVICE_BUSY;
               break;
            }
            /* Clock source should not change, when in slave mode */
            if ((io_info_ptr->MODE & I2S_TX_SLAVE) || (io_info_ptr->MODE & I2S_RX_SLAVE))
            {
               I2S_LOG("\n  _i2s_ioctl: Error - Cannot chnage I2S clock source while in slave mode.");
               result = IO_ERROR_INVALID_IOCTL_CMD;
               break;
            }
            
            io_info_ptr->CLOCK_SOURCE = I2S_CLK_INT;
            /* Set master clock to be generated internally */
#if PSP_MQX_CPU_IS_VYBRID
            _vi2s_set_clk_src_int(io_info_ptr->HW_CHANNEL);
            io_info_ptr->MCLK_FREQ = _vi2s_get_mclk_value(io_info_ptr);
#else
            i2s_ptr->MCR |= (I2S_MCR_MOE_MASK);
            io_info_ptr->MCLK_FREQ = _ki2s_get_mclk_value(io_info_ptr);
#endif
            io_info_ptr->BCLK_FREQ = io_info_ptr->MCLK_FREQ / io_info_ptr->BCLK_DIV;
            io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / (io_info_ptr->DATA_BITS * MYCHANNELS);
            break;
            
        case IO_IOCTL_I2S_SET_CLOCK_SOURCE_EXT:
            if (io_info_ptr->ONTHEWAY)
            {
               I2S_LOG("\n  _i2s_ioctl: Error - Cannot chnage I2S clock source while I/O actions are in progress.");
               result = I2S_ERROR_DEVICE_BUSY;
               break;
            }
            /* You should not change clock source, when in slave mode */
            if ((io_info_ptr->MODE & I2S_TX_SLAVE) || (io_info_ptr->MODE & I2S_RX_SLAVE))
            {
               I2S_LOG("\n  _i2s_ioctl: Error - Cannot chnage I2S clock source while in slave mode.");
               result = IO_ERROR_INVALID_IOCTL_CMD;
               break;
            }
            
            io_info_ptr->CLOCK_SOURCE = I2S_CLK_EXT;
            /* Set master clock to be generated externally */
#if PSP_MQX_CPU_IS_VYBRID
            _vi2s_set_clk_src_ext(io_info_ptr->HW_CHANNEL);
#else
            i2s_ptr->MCR &= ~(I2S_MCR_MOE_MASK);
#endif
            io_info_ptr->MCLK_FREQ = 0;
            io_info_ptr->BCLK_FREQ = 0;
            io_info_ptr->FS_FREQ = 0;
            break;
            
        case IO_IOCTL_I2S_SET_DATA_BITS:
            if(io_info_ptr->ONTHEWAY)
            {
               I2S_LOG("\n  _i2s_ioctl: Error - Cannot chnage I2S data bit length while I/O actions are in progress.");
               result = I2S_ERROR_DEVICE_BUSY;
               break;
            }
            if (param == NULL)
            {
                 result = I2S_ERROR_INVALID_PARAMETER;
            }
            else
            {
               uint8_t  data_bits =  *((uint8_t  *) param);
               if ((data_bits < 8) || (data_bits > 32))
               {
                  I2S_LOG("\n  _i2s_ioctl: Error - Unsupported data word length.\n");
                  result = I2S_ERROR_WORD_LENGTH_UNSUPPORTED;
                  break;
               }
               io_info_ptr->DATA_BITS = data_bits;
               /* Setup transmitter */
               if ((io_info_ptr->IO_MODE & I2S_IO_WRITE) || (io_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS))
               {
                   /* Set 0th word length */
                   i2s_ptr->TCR5 &= ~(I2S_TCR5_W0W_MASK);
                   i2s_ptr->TCR5 |= I2S_TCR5_W0W(data_bits-1);
                   /* Set Nth word length */
                   i2s_ptr->TCR5 &= ~(I2S_TCR5_WNW_MASK);
                   i2s_ptr->TCR5 |= I2S_TCR5_WNW(data_bits-1);
                   /* Set first bit shifted to highest index in register */
                   i2s_ptr->TCR5 &= ~(I2S_TCR5_FBT_MASK);
                   i2s_ptr->TCR5 |= I2S_TCR5_FBT(data_bits - 1);
                   /* Set sync width to match word length */
                   i2s_ptr->TCR4 &= ~(I2S_TCR4_SYWD_MASK);
                   i2s_ptr->TCR4 |= I2S_TCR4_SYWD(data_bits-1); 
               }
               /* Setup receiver */
               if ((io_info_ptr->IO_MODE & I2S_IO_READ) || (io_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS))
               {
                   /* Set 0th word length */
                   i2s_ptr->RCR5 &= ~(I2S_RCR5_W0W_MASK);
                   i2s_ptr->RCR5 |= I2S_RCR5_W0W(data_bits-1);
                   /* Set Nth word length */
                   i2s_ptr->RCR5 &= ~(I2S_RCR5_WNW_MASK);
                   i2s_ptr->RCR5 |= I2S_RCR5_WNW(data_bits-1);
                   /* Set first bit shifted to highest index in register */
                   i2s_ptr->RCR5 &= ~(I2S_RCR5_FBT_MASK);
                   i2s_ptr->RCR5 |= I2S_RCR5_FBT(data_bits);
                   /* Set sync width to match word length */
                   i2s_ptr->RCR4 &= ~(I2S_RCR4_SYWD_MASK);
                   i2s_ptr->RCR4 |= I2S_RCR4_SYWD(data_bits-1);  
               }
               /* Recalculate frame sync frequency to match settings */
               io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / (io_info_ptr->DATA_BITS * MYCHANNELS);
            }
            break;
            
        case IO_IOCTL_I2S_DISABLE_DEVICE:
            if ((io_info_ptr->IO_MODE & I2S_IO_WRITE) && 
                (io_info_ptr->IO_MODE & I2S_IO_READ))
            {
                if (io_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS)
                {
                    i2s_ptr->TCSR &= ~( 
                        I2S_TCSR_TE_MASK   | /* Disable transmitter            */
                        I2S_TCSR_BCE_MASK  | /* Disable bit clock              */
                        I2S_TCSR_FEIE_MASK | /* Disable FIFO error interrupt   */
                        I2S_TCSR_FRIE_MASK   /* Disable FIFO request interrupt */
                        );
                    i2s_ptr->RCSR &= ~(
                        I2S_RCSR_RE_MASK   | /* Disable receiver               */
                        I2S_RCSR_BCE_MASK  | /* Disable bit clock              */
                        I2S_RCSR_FEIE_MASK | /* Disable FIFO error interrupt   */
                        I2S_RCSR_FRIE_MASK   /* Disable FIFO request interrupt */
                        );
                }
                else if (io_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS)
                {
                    i2s_ptr->RCSR &= ~(
                        I2S_RCSR_RE_MASK   | /* Disable receiver               */
                        I2S_RCSR_BCE_MASK  | /* Disable bit clock              */
                        I2S_RCSR_FEIE_MASK | /* Disable FIFO error interrupt   */
                        I2S_RCSR_FRIE_MASK   /* Disable FIFO request interrupt */
                        );
                    i2s_ptr->TCSR &= ~( 
                        I2S_TCSR_TE_MASK   | /* Disable transmitter            */
                        I2S_TCSR_BCE_MASK  | /* Disable bit clock              */
                        I2S_TCSR_FEIE_MASK | /* Disable FIFO error interrupt   */
                        I2S_TCSR_FRIE_MASK   /* Disable FIFO request interrupt */
                        );
                }
            }
            else if (io_info_ptr->IO_MODE & I2S_IO_WRITE) /* Disable transmitter */
            {
                i2s_ptr->TCSR &= ~( 
                    I2S_TCSR_TE_MASK   | /* Disable transmitter            */
                    I2S_TCSR_BCE_MASK  | /* Disable bit clock              */
                    I2S_TCSR_FEIE_MASK | /* Disable FIFO error interrupt   */
                    I2S_TCSR_FRIE_MASK   /* Disable FIFO request interrupt */
                    );
            }
            else if (io_info_ptr->IO_MODE & I2S_IO_READ) /* Disable receiver */
            {
                i2s_ptr->RCSR &= ~(
                    I2S_RCSR_RE_MASK   | /* Disable receiver               */
                    I2S_RCSR_BCE_MASK  | /* Disable bit clock              */
                    I2S_RCSR_FEIE_MASK | /* Disable FIFO error interrupt   */
                    I2S_RCSR_FRIE_MASK   /* Disable FIFO request interrupt */
                    );
            }
            break;
            
        case IO_IOCTL_I2S_ENABLE_DEVICE:
            if ((io_info_ptr->IO_MODE & I2S_IO_WRITE) && 
                (io_info_ptr->IO_MODE & I2S_IO_READ))
            {
                if (io_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS) /* Receiver must be enabled first */
                {
                    i2s_ptr->RCSR |= (
                        I2S_RCSR_RE_MASK   | /* Enable receiver  */
                        I2S_RCSR_BCE_MASK    /* Enable bit clock */
                        );
                    i2s_ptr->TCSR |= ( 
                        I2S_TCSR_TE_MASK   | /* Enable transmitter */
                        I2S_TCSR_BCE_MASK    /* Enable bit clock   */
                        );
                }
                else if (io_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS) /* Transmitter must be enabled first */
                {
                    i2s_ptr->TCSR |= ( 
                        I2S_TCSR_TE_MASK   | /* Enable transmitter */
                        I2S_TCSR_BCE_MASK    /* Enable bit clock   */
                        );
                     i2s_ptr->RCSR |= (
                        I2S_RCSR_RE_MASK   | /* Enable receiver  */
                        I2S_RCSR_BCE_MASK    /* Enable bit clock */
                        );
                }
            } 
            else if (io_info_ptr->IO_MODE & I2S_IO_WRITE) /* Enable transmitter */
            {
                i2s_ptr->TCSR |= ( 
                    I2S_TCSR_TE_MASK   | /* Enable transmitter */
                    I2S_TCSR_BCE_MASK    /* Enable bit clock   */
                    );
            }
            else if (io_info_ptr->IO_MODE & I2S_IO_READ)  /* Enable receiver */
            {
                i2s_ptr->RCSR &= ~(
                    I2S_RCSR_RE_MASK   | /* Enable transmitter */
                    I2S_RCSR_BCE_MASK    /* Enable bit clock   */
                    );
            }
            break;
            
        case IO_IOCTL_I2S_SET_MCLK_FREQ:
            if (io_info_ptr->ONTHEWAY)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Device is busy.");
                result = I2S_ERROR_DEVICE_BUSY;
                break;
            }
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break; 
            }
            else
            {
                uint16_t    fract = 0;
                (void)      fract; /* suppress 'unused variable' warning */
                uint16_t    div = 0;
                (void)      div; /* suppress 'unused variable' warning */
                uint32_t    frequency;
#if PSP_MQX_CPU_IS_VYBRID     
                /* Find values for SAI clock divider */
                _vi2s_set_mclk_freq(io_info_ptr, *((uint32_t *) param));
                frequency = _vi2s_get_mclk_value(io_info_ptr);
#else
                frequency = _ki2s_find_mclk_freq_div(io_info_ptr->MCLK_SRC, &fract, &div, *((uint32_t *) param));            
                i2s_ptr->MDR = (I2S_MDR_DIVIDE(div-1) | I2S_MDR_FRACT(fract-1));
                while(i2s_ptr->MCR & I2S_MCR_DUF_MASK) {}; // wait until new value is set
#endif
                /* Recalculate clock values according to new setting */
                io_info_ptr->MCLK_FREQ = frequency;
                io_info_ptr->BCLK_FREQ = io_info_ptr->MCLK_FREQ / io_info_ptr->BCLK_DIV;
                io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / (io_info_ptr->DATA_BITS * MYCHANNELS);
            }
            break;
            
        case IO_IOCTL_I2S_SET_FS_FREQ:
            if (io_info_ptr->ONTHEWAY)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Device is busy.");
                result = I2S_ERROR_DEVICE_BUSY;
                break;
            }
            
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            else
            {
                sample_rate = *((uint32_t *) param);
                if (sample_rate == 0)
                {
                    I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                    result = I2S_ERROR_INVALID_PARAMETER;
                    break;
                }
            }
            if (io_info_ptr->CLOCK_SOURCE == I2S_CLK_INT)
            {
                /* Get bit clock value for selected sample rate*/
                io_info_ptr->BCLK_FREQ = sample_rate * io_info_ptr->DATA_BITS * MYCHANNELS;
                /* Get divider value for calculated bit clock frequency */
                io_info_ptr->BCLK_DIV = io_info_ptr->MCLK_FREQ / io_info_ptr->BCLK_FREQ;
                if (io_info_ptr->BCLK_DIV == 0)
                {
                    I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                    result = I2S_ERROR_INVALID_PARAMETER;
                    break;
                }
                /* Recalculate bit clock to match divider value */
                io_info_ptr->BCLK_FREQ = io_info_ptr->MCLK_FREQ / io_info_ptr->BCLK_DIV;
                /* Get sampling frequency from new bit clock value */
                io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / (io_info_ptr->DATA_BITS * MYCHANNELS);     
                if ((io_info_ptr->IO_MODE & I2S_IO_WRITE) || (io_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS))
                {
                    /* Set bit clock divider in TCR2 register */
                    i2s_ptr->TCR2 &= ~I2S_TCR2_DIV_MASK;
                    i2s_ptr->TCR2 |= I2S_TCR2_DIV((io_info_ptr->BCLK_DIV / 2) - 1);   
                }
                if ((io_info_ptr->IO_MODE & I2S_IO_READ) || (io_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS))
                {
                    /* Set bit clock divider in RCR2 register */
                    i2s_ptr->RCR2 &= ~I2S_RCR2_DIV_MASK;
                    i2s_ptr->RCR2 |= I2S_RCR2_DIV((io_info_ptr->BCLK_DIV / 2) - 1);   
                }
                
            }
            else
            {
                /* If the clock source is external, set up frame sync frequency and recalculate bit clock frequency */
                io_info_ptr->FS_FREQ = sample_rate;
                io_info_ptr->BCLK_FREQ = sample_rate * io_info_ptr->DATA_BITS * MYCHANNELS;
            }
            
            result = MQX_OK;
            break;
#if I2S_USE_FLOAT    
        case IO_IOCTL_I2S_TX_DUMMY_ON:
            if (!(io_info_ptr->IO_MODE & I2S_IO_WRITE))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid IOCTL command.");
                result = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            }
            if (io_info_ptr->TX_DUMMY == FALSE)
            {
                io_info_ptr->TX_DUMMY = TRUE;
                dummy->LENGTH = _ksai_sine_wave(io_info_ptr->FS_FREQ, 440, &(dummy->DATA));
                dummy->INDEX = 0;
            }
            break;
            
        case IO_IOCTL_I2S_TX_DUMMY_OFF:
            if (!(io_info_ptr->IO_MODE & I2S_IO_WRITE))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid IOCTL command.");
                result = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            }
            if (io_info_ptr->TX_DUMMY == TRUE)
            {
                io_info_ptr->TX_DUMMY = FALSE;
                result = _mem_free(dummy->DATA);
                if (result == MQX_OK)
                {
                    dummy->LENGTH = 0;
                    dummy->DATA = NULL;
                }
                dummy->INDEX = 0;
            }
            break;
#endif
        case IO_IOCTL_I2S_GET_MODE:
            if (param == NULL)
            {
                 result = I2S_ERROR_INVALID_PARAMETER;
                 break;
            }
            *((uint8_t  *) param) = io_info_ptr->MODE;
            break;
        
        case IO_IOCTL_I2S_GET_CLOCK_SOURCE:
            if (param == NULL)
            {
                 result = I2S_ERROR_INVALID_PARAMETER;
                 break;
            }
            *((uint8_t  *) param) = io_info_ptr->CLOCK_SOURCE;
            break;
        
        case IO_IOCTL_I2S_GET_DATA_BITS:
             if (param == NULL)
             {
                  I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter."); 
                  result = I2S_ERROR_INVALID_PARAMETER;
                  break;
             }
             *((uint8_t  *) param) = io_info_ptr->DATA_BITS;
             break;
            
        case IO_IOCTL_I2S_GET_MCLK_FREQ:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break; 
            }
            else
            {
                *((uint32_t *)param) = io_info_ptr->MCLK_FREQ;
            }
            result = MQX_OK;
            break;
                
        case IO_IOCTL_I2S_GET_BCLK_FREQ:
            if (param == NULL)
            {
                 I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                 result = I2S_ERROR_INVALID_PARAMETER;
                 break;
            }
            *((uint32_t *) param) = io_info_ptr->BCLK_FREQ;
            break;
            
        case IO_IOCTL_I2S_GET_TX_DUMMY:
            if (!(io_info_ptr->IO_MODE & I2S_IO_WRITE))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid IOCTL command.");
                result = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            }
            if (param == NULL)
            {
                 I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                 result = I2S_ERROR_INVALID_PARAMETER;
                 break;
            }
            *((bool*) param) = io_info_ptr->TX_DUMMY;
            break;
            
        case IO_IOCTL_I2S_GET_FS_FREQ:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            else
            {
                *((uint32_t *)param) = io_info_ptr->FS_FREQ;
            }
            result = MQX_OK;
            break;

        /*Change the GET_STATISTICS function to return the status pointer*/
        case IO_IOCTL_I2S_GET_STATISTICS:
            * ((I2S_STATISTICS_STRUCT *)param) = io_info_ptr->STATS;
            break;
            
        case IO_IOCTL_I2S_SET_TXFIFO_WATERMARK:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            else if (*((uint8_t  *) param) > (SIZE_OF_FIFO - 1))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Parameter out of range.");
                result = I2S_ERROR_PARAM_OUT_OF_RANGE;
                break;
            }
            if (!(io_info_ptr->IO_MODE & I2S_IO_WRITE) && (io_info_ptr->IO_MODE & I2S_IO_READ))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid IOCTL command.");
                result = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            }
            i2s_ptr->TCR1 = I2S_TCR1_TFW(*((uint8_t  *) param));
            break;
            
        case IO_IOCTL_I2S_SET_RXFIFO_WATERMARK:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            else if (*((uint8_t  *) param) > (SIZE_OF_FIFO - 1))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Parameter out of range.");
                result = I2S_ERROR_PARAM_OUT_OF_RANGE;
                break;
            }
            if (!(io_info_ptr->IO_MODE & I2S_IO_READ) && (io_info_ptr->IO_MODE & I2S_IO_WRITE))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid IOCTL command.");
                result = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            }
            i2s_ptr->RCR1 = I2S_RCR1_RFW(*((uint8_t  *) param));
            break;
            
        case IO_IOCTL_I2S_GET_TXFIFO_WATERMARK:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            if (!(io_info_ptr->IO_MODE & I2S_IO_WRITE) && (io_info_ptr->IO_MODE & I2S_IO_READ))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid IOCTL command.");
                result = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            }
            *((uint8_t  *) param) = i2s_ptr->TCR1;
            break;

        case IO_IOCTL_I2S_GET_RXFIFO_WATERMARK:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            if (!(io_info_ptr->IO_MODE & I2S_IO_READ) && (io_info_ptr->IO_MODE & I2S_IO_WRITE))
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid IOCTL command.");
                result = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            }
            *((uint8_t  *) param) = i2s_ptr->RCR1;
            break;
            
        case IO_IOCTL_I2S_CLEAR_STATISTICS:
            _mem_zero(&(io_info_ptr->STATS), sizeof(io_info_ptr->STATS));
            break;
            
        case IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT:
            if(io_info_ptr->ONTHEWAY)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Cannot change I/O data format while I/O actions are in progress.\n");
                result = I2S_ERROR_DEVICE_BUSY;
                break;
            }
            
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            else
            {
                uint8_t  aligment, endianity, bits, channels;
                
                endianity = ((AUDIO_DATA_FORMAT *)param)->ENDIAN;
                bits = ((AUDIO_DATA_FORMAT *)param)->BITS;
                aligment = ((AUDIO_DATA_FORMAT *)param)->ALIGNMENT;
                channels = ((AUDIO_DATA_FORMAT *)param)->CHANNELS;
                
                if ((aligment > AUDIO_ALIGNMENT_LEFT) || (bits < AUDIO_BIT_SIZE_MIN) ||
                    (bits > AUDIO_BIT_SIZE_MAX) || (endianity > AUDIO_LITTLE_ENDIAN)  ||
                    (channels > 2) || (channels == 0))
                {
                    I2S_LOG("\n  _i2s_ioctl: Error - Invalid I/O data format.");
                    result = AUDIO_ERROR_INVALID_IO_FORMAT;
                    break;
                }
                _mem_copy(param, &(io_info_ptr->IO_FORMAT), sizeof(AUDIO_DATA_FORMAT));
                /* Setup frame sync for transmitter */
                if ((io_info_ptr->IO_MODE & I2S_IO_WRITE) || (io_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS))
                {
                    i2s_ptr->TCR4 &= ~(I2S_TCR4_FRSZ_MASK);
                    i2s_ptr->TCR4 |= (I2S_TCR4_FRSZ(1));
                    /* Mask second transmitter channel if there is only one data channel */
                    if (channels == 1)
                    {
                        i2s_ptr->TMR = 0x02;
                    }
                    else
                    {
                        i2s_ptr->TMR = 0;
                    }
                }
                /* Setup frame sync for receiver */
                if ((io_info_ptr->IO_MODE & I2S_IO_READ) || (io_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS))
                {
                    i2s_ptr->RCR4 &= ~(I2S_RCR4_FRSZ_MASK);
                    i2s_ptr->RCR4 |= (I2S_RCR4_FRSZ(1));
                    /* Mask second receiver channel if there is only one data channel */
                    if (channels == 1)
                    {
                        i2s_ptr->RMR = 0x02;
                    }
                    else
                    {
                        i2s_ptr->RMR = 0;
                    }
                }
            }
            break;
            
        case IO_IOCTL_AUDIO_GET_IO_DATA_FORMAT:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            _mem_copy(&(io_info_ptr->IO_FORMAT), param, sizeof(io_info_ptr->IO_FORMAT));
            break;
            
        case IO_IOCTL_I2S_SET_CLOCK_MODE:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            bclk_mode = *((uint8_t  *) param);
            if ((io_info_ptr->IO_MODE & I2S_IO_WRITE) && !(io_info_ptr->IO_MODE & I2S_IO_READ))
            {
                // Set receiver bit clock settings to zero if transmitter is being set.
                bclk_mode &= 0xF0; 
            }
            if ((io_info_ptr->IO_MODE & I2S_IO_READ) && !(io_info_ptr->IO_MODE & I2S_IO_WRITE))
            {
                // Set transmitter bit clock settings to zero if receiver is being set.
                bclk_mode &= 0x0F;
            }
            result = _ki2s_set_bclk_mode(i2s_ptr, bclk_mode);
            if (result != I2S_OK)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid BCLK setting selected.");
            }
            break;
            
        case IO_IOCTL_I2S_GET_CLOCK_MODE:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            *((uint8_t  *) param) = io_info_ptr->CLOCK_MODE;
            break;

        case IO_IOCTL_I2S_UPDATE_TX_STATUS:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            length = *((uint32_t *)param);

            /* Initialize statistics and buffers */
            stats->PACKETS_REQUESTED += length;
            stats->PACKETS_QUEUED += length;

            //Now to switch the buffer and clear the status flag.
            if(stats->IN_BUFFER == buffer->END_PERIOD)
            {
                stats->IN_BUFFER = buffer->START_PERIOD;
            }
            else
            {
                stats->IN_BUFFER += buffer->SIZE;
            }

            /* In dummy mode, data are already in memory, so we don't have to buffer them */
            if (io_info_ptr->TX_DUMMY)
            {
                stats->PACKETS_QUEUED = stats->PACKETS_REQUESTED;
            }
            /* Set device into busy state */
            io_info_ptr->ONTHEWAY = TRUE;

            if(io_info_ptr->FIRST_IO)
            {
                //If the dma is not start, strat the dma
                if(FALSE == io_info_ptr->DMA_KICKOFF)
                {
                    hwReq = _bsp_get_sai_tx_dma_source(io_info_ptr->HW_CHANNEL);
                    result = dma_channel_setup(io_info_ptr->DCH, buffer->PERIODS, DMA_CHANNEL_FLAG_LOOP_MODE);
                    if (result != MQX_OK)
                    {
                        I2S_LOG("\n	_i2s_tx: Error - dma_channel_setup. 0x%x\n", result);
                        return I2S_ERROR_INVALID_PARAMETER;
                    }
                    result = dma_request_source(io_info_ptr->DCH, hwReq);
                    if (result != MQX_OK)
                    {
                        I2S_LOG("\n	_i2s_tx: Error - dma_request_source. 0x%x\n", result);
                        return I2S_ERROR_INVALID_PARAMETER;
                    }

                    //Divide the dma transfer into four TCDs.
                    for(i = 0; i < buffer->PERIODS; i ++)
                    {
                        dma_tcd_mem2reg(&io_info_ptr->TCD, &i2s_ptr->TDR[io_info_ptr->TX_CHANNEL],
                                io_format->SIZE, buffer->START_PERIOD + i *buffer->SIZE, buffer->SIZE);
                        dma_transfer_submit(io_info_ptr->DCH, &io_info_ptr->TCD, &io_info_ptr->TCD_SEQ);
                    }
                    //Enable the dma transfer
                    dma_request_enable(io_info_ptr->DCH);
                    io_info_ptr->DMA_KICKOFF = TRUE;
                }

                //Enable the Tx.
                io_info_ptr->FIRST_IO = FALSE; 
                /* Invert transmiter frame sync in TX mode */
                i2s_ptr->TCR4 |= (I2S_TCR4_FSP_MASK);

                /* enable transmitter */
                i2s_ptr->TCSR |= I2S_TCSR_FEIE_MASK;
                i2s_ptr->TCSR |= (I2S_TCSR_FWDE_MASK | I2S_TCSR_FRDE_MASK);
                i2s_ptr->TCSR |= I2S_TCSR_TE_MASK;
                /* 
                 ** We have to enable receiver in order to get clock signals (BCLK and FS)
                 ** if transmitter is synchronous.
                 */
                if (io_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS)
                {
                    i2s_ptr->RCSR |= (I2S_RCSR_RE_MASK);
                }
            }
            break;

        case IO_IOCTL_I2S_UPDATE_RX_STATUS:
            if (param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            length = *((uint32_t *)param);
            /* Initialize statistics and buffers to default values */
            stats->PACKETS_REQUESTED += length;
            stats->PACKETS_PROCESSED += length;

            if(stats->PACKETS_PROCESSED % stats->SIZE == 0)
            {
                /*Switch the buffer block*/
                if(stats->OUT_BUFFER == buffer->END_PERIOD)
                {
                    stats->OUT_BUFFER = buffer->START_PERIOD;
                }
                else
                {
                    stats->OUT_BUFFER += buffer->SIZE;
                }
            }
            if(io_info_ptr->FIRST_IO)
            {
                //kick off a DMA
                if(FALSE == io_info_ptr->DMA_KICKOFF)
                {
                    hwReq = _bsp_get_sai_rx_dma_source(io_info_ptr->HW_CHANNEL);
                    result = dma_channel_setup(io_info_ptr->DCH, buffer->PERIODS, DMA_CHANNEL_FLAG_LOOP_MODE);
                    if (result != MQX_OK)
                    {
                        I2S_LOG("\n	_i2s_rx: Error - dma_channel_setup. 0x%x\n", result);
                        return I2S_ERROR_INVALID_PARAMETER;
                    }
                    result = dma_request_source(io_info_ptr->DCH, hwReq);
                    if (result != MQX_OK)
                    {
                        I2S_LOG("\n	_i2s_rx: Error - dma_request_source. 0x%x\n", result);
                        return I2S_ERROR_INVALID_PARAMETER;
                    }
                    /* Configure the tcds */
                    for(i = 0; i < buffer->PERIODS; i ++)
                    {
                        dma_tcd_reg2mem(&io_info_ptr->TCD, &i2s_ptr->RDR[io_info_ptr->RX_CHANNEL], io_format->SIZE,
                                buffer->START_PERIOD + i * buffer->SIZE, buffer->SIZE );
                        dma_transfer_submit(io_info_ptr->DCH, &io_info_ptr->TCD, &io_info_ptr->TCD_SEQ);
                    }
                    //Enable the dma transfer
                    dma_request_enable(io_info_ptr->DCH);
                    io_info_ptr->DMA_KICKOFF = TRUE;
                }

                /* Enable the Rx */
                io_info_ptr->FIRST_IO = FALSE; 

                /* First receive operation - enable receiver */
                i2s_ptr->RCSR |= I2S_RCSR_FEIE_MASK;
                i2s_ptr->RCSR |= (I2S_RCSR_FRDE_MASK | I2S_RCSR_FWDE_MASK);
                i2s_ptr->RCSR |= (I2S_RCSR_RE_MASK);
                /* 
                 ** We have to enable transmitter in order to get clock signals 
                 ** (BCLK and FS) if the receiver is synchronous.
                 */
                if (io_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS)
                {
                    i2s_ptr->TCSR |= (I2S_TCSR_TE_MASK);
                    i2s_ptr->TCR4 &= ~(I2S_TCR4_FSP_MASK); 
                }
            }
            break;
            
        case IO_IOCTL_I2S_WAIT_TX_EVENT:
            _lwsem_wait(&stats->BUFFER_EMPTY);
            break;
            
        case IO_IOCTL_I2S_WAIT_RX_EVENT:
            _lwsem_wait(&stats->BUFFER_FULL);           
            break;
            
        case IO_IOCTL_I2S_CONFIG_SAI_BUFFER:
            if(param == NULL)
            {
                I2S_LOG("\n  _i2s_ioctl: Error - Invalid parameter.");
                result = I2S_ERROR_INVALID_PARAMETER;
                break;
            }
            if(io_info_ptr->ONTHEWAY)
            {
                I2S_LOG("\n  _i2s_buffer_config: System busy, cannnot re-config buffer now!\n");
                result = I2S_ERROR_DEVICE_BUSY;
                break;
            }
            buffer_config = (SAI_BUFFER_CONFIG *)param;
            /* Judge the buffer size and periods */
            if((buffer_config->PERIOD_SIZE < 128) || (buffer_config->PERIOD_NUMBER < 1))
            {
                I2S_LOG("\n  _i2s_buffer_config: Period size or period number is out of range\n");
                result = I2S_ERROR_PARAM_OUT_OF_RANGE;
                break;
            }
            if((buffer_config->PERIOD_SIZE * buffer_config->PERIOD_NUMBER) > SAI_MAX_BUFFER_SIZE)
            {
                I2S_LOG("\n  _i2s_buffer_config: Buffer size is out of range!\n");
                result = I2S_ERROR_PARAM_OUT_OF_RANGE;
                break;
            }
            /* Free the buffer used before */
            if(io_info_ptr->BUFFER.DMA_DATA)
            {
                result = _mem_free(io_info_ptr->BUFFER.DMA_DATA);
                if(result != MQX_OK)
                {
                    I2S_LOG("\n  _i2s_buffer_config: Error - Free buffer failed! \n");
                    break;
                }
            }
            /* Re-config the info structure and status */
            buffer->DMA_DATA = (uint8_t *)_mem_alloc_system_zero_uncached(buffer_config->PERIOD_NUMBER
                    * buffer_config->PERIOD_SIZE);
            if(buffer->DMA_DATA == NULL)
            {
                I2S_LOG("\n  _i2s_buffer_config: Error - Memory allocate failed!\n");
                return MQX_OUT_OF_MEMORY;
            }
            buffer->PERIODS = buffer_config->PERIOD_NUMBER;
            buffer->SIZE = buffer_config->PERIOD_SIZE;
            buffer->START_PERIOD = buffer->DMA_DATA;
            buffer->END_PERIOD = buffer->DMA_DATA + (buffer->PERIODS - 1) * buffer->SIZE;
            /* Update status */
            stats->SIZE = buffer->SIZE;
            stats->IN_BUFFER = buffer->START_PERIOD;
            stats->OUT_BUFFER = buffer->START_PERIOD;
            stats->BUFFER_EMPTY.VALUE = buffer->PERIODS;
            stats->BUFFER_FULL.VALUE = 0;
            break;
        case IO_IOCTL_I2S_START_TX:
             io_info_ptr->ONTHEWAY = TRUE;
            //If the dma is not start, strat the dma
            if(FALSE == io_info_ptr->DMA_KICKOFF)
            {
                hwReq = _bsp_get_sai_tx_dma_source(io_info_ptr->HW_CHANNEL);
                result = dma_channel_setup(io_info_ptr->DCH, buffer->PERIODS, DMA_CHANNEL_FLAG_LOOP_MODE);
                if (result != MQX_OK)
                {
                    I2S_LOG("\n	_i2s_tx: Error - dma_channel_setup. 0x%x\n", result);
                    return I2S_ERROR_INVALID_PARAMETER;
                }
                result = dma_request_source(io_info_ptr->DCH, hwReq);
                if (result != MQX_OK)
                {
                    I2S_LOG("\n	_i2s_tx: Error - dma_request_source. 0x%x\n", result);
                    return I2S_ERROR_INVALID_PARAMETER;
                }
                //Divide the dma transfer into four TCDs.
                for(i = 0; i < buffer->PERIODS; i ++)
                {
                    dma_tcd_mem2reg(&io_info_ptr->TCD, &i2s_ptr->TDR[io_info_ptr->TX_CHANNEL],
                                io_format->SIZE, buffer->START_PERIOD + i *buffer->SIZE, buffer->SIZE);
                    dma_transfer_submit(io_info_ptr->DCH, &io_info_ptr->TCD, &io_info_ptr->TCD_SEQ);
                }
                //Enable the dma transfer
                dma_request_enable(io_info_ptr->DCH);
                io_info_ptr->DMA_KICKOFF = TRUE;
            }
            //Enable the Tx.
            io_info_ptr->FIRST_IO = FALSE; 
            /* Invert transmiter frame sync in TX mode */
            i2s_ptr->TCR4 |= (I2S_TCR4_FSP_MASK);
            /* enable transmitter */
            i2s_ptr->TCSR |= I2S_TCSR_FEIE_MASK;
            i2s_ptr->TCSR |= (I2S_TCSR_FWDE_MASK | I2S_TCSR_FRDE_MASK);
            i2s_ptr->TCSR |= I2S_TCSR_TE_MASK;
            /* 
             ** We have to enable receiver in order to get clock signals (BCLK and FS)
             ** if transmitter is synchronous.
             */
            if (io_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS)
            {
                i2s_ptr->RCSR |= (I2S_RCSR_RE_MASK);
            }
            break;

        case IO_IOCTL_I2S_START_RX:
            io_info_ptr->ONTHEWAY = TRUE;
            if(FALSE == io_info_ptr->DMA_KICKOFF)
            {
                hwReq = _bsp_get_sai_rx_dma_source(io_info_ptr->HW_CHANNEL);
                result = dma_channel_setup(io_info_ptr->DCH, buffer->PERIODS, DMA_CHANNEL_FLAG_LOOP_MODE);
                if (result != MQX_OK)
                {
                    I2S_LOG("\n	_i2s_rx: Error - dma_channel_setup. 0x%x\n", result);
                    return I2S_ERROR_INVALID_PARAMETER;
                }
                result = dma_request_source(io_info_ptr->DCH, hwReq);
                if (result != MQX_OK)
                {
                    I2S_LOG("\n	_i2s_rx: Error - dma_request_source. 0x%x\n", result);
                    return I2S_ERROR_INVALID_PARAMETER;
                }
                /* Configure the tcds */
                for(i = 0; i < buffer->PERIODS; i ++)
                {
                    dma_tcd_reg2mem(&io_info_ptr->TCD, &i2s_ptr->RDR[io_info_ptr->RX_CHANNEL], io_format->SIZE,
                                buffer->START_PERIOD + i * buffer->SIZE, buffer->SIZE );
                    dma_transfer_submit(io_info_ptr->DCH, &io_info_ptr->TCD, &io_info_ptr->TCD_SEQ);
                }
                //Enable the dma transfer
                dma_request_enable(io_info_ptr->DCH);
                io_info_ptr->DMA_KICKOFF = TRUE;
            }

            /* Enable the Rx */
            io_info_ptr->FIRST_IO = FALSE; 

            /* First receive operation - enable receiver */
            i2s_ptr->RCSR |= I2S_RCSR_FEIE_MASK;
            i2s_ptr->RCSR |= (I2S_RCSR_FRDE_MASK | I2S_RCSR_FWDE_MASK);
            i2s_ptr->RCSR |= (I2S_RCSR_RE_MASK);
            /* 
            ** We have to enable transmitter in order to get clock signals 
            ** (BCLK and FS) if the receiver is synchronous.
            */
            if (io_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS)
            {
                i2s_ptr->TCSR |= (I2S_TCSR_TE_MASK);
                i2s_ptr->TCR4 &= ~(I2S_TCR4_FSP_MASK); 
            }
            break;

        case IO_IOCTL_I2S_STOP_TX:
            io_info_ptr->FIRST_IO = TRUE;
            /* Disable DMA request. */
            i2s_ptr->TCSR &= ~(I2S_TCSR_FWDE_MASK | I2S_TCSR_FRDE_MASK);
            i2s_ptr->TCSR &= ~I2S_TCSR_FEIE_MASK;
            i2s_ptr->TCSR &= ~I2S_TCSR_TE_MASK;
            break;

        case IO_IOCTL_I2S_STOP_RX:
            io_info_ptr->FIRST_IO = TRUE;
            /* Disable RX. */
            i2s_ptr->RCSR &= ~I2S_RCSR_RE_MASK;
            /* Disable DMA request. */
            i2s_ptr->RCSR &= ~(I2S_RCSR_FWDE_MASK | I2S_RCSR_FRDE_MASK);
            i2s_ptr->RCSR &= ~I2S_RCSR_FEIE_MASK;
            break;

        case IO_IOCTL_I2S_RESUME_TX:
            io_info_ptr->FIRST_IO = FALSE; 
            /* enable transmitter */
            i2s_ptr->TCSR |= I2S_TCSR_FEIE_MASK;
            i2s_ptr->TCSR |= (I2S_TCSR_FWDE_MASK | I2S_TCSR_FRDE_MASK);
            i2s_ptr->TCSR |= I2S_TCSR_TE_MASK;

        case IO_IOCTL_I2S_RESUME_RX:
            io_info_ptr->FIRST_IO = FALSE; 
            /* First receive operation - enable receiver */
            i2s_ptr->RCSR |= I2S_RCSR_FEIE_MASK;
            i2s_ptr->RCSR |= (I2S_RCSR_FRDE_MASK | I2S_RCSR_FWDE_MASK);
            i2s_ptr->RCSR |= (I2S_RCSR_RE_MASK);

        default:
            I2S_LOG("\n  _i2s_ioctl: Error - Invalid IOCTL command.");
            result = IO_ERROR_INVALID_IOCTL_CMD;
            break;
    }
    return result;
}
/*FUNCTION****************************************************************
*
* Function Name    : _ksai_hw_init
* Returned Value   : MQX error code
* Comments         :
*    This function initializes an SAI hardware control registers.
*
*END*********************************************************************/

void _ksai_hw_init
(
    /* [IN] SAI init parameter void * */
    KSAI_INIT_STRUCT * ksai_init_ptr,
    
    /* [IN] SAI register void * */
    I2S_MemMapPtr i2s_ptr,
    
    /* [IN] I/O mode (read/write) */
    _mqx_int io_mode
)
{    
    uint8_t     data_bits  = ksai_init_ptr->DATA_BITS;
    uint8_t     i2s_mode   = ksai_init_ptr->MODE;
    uint8_t     clk_source = ksai_init_ptr->CLOCK_SOURCE;
    uint8_t     bclk_mode  = ksai_init_ptr->CLOCK_MODE;
    uint16_t    fract = 0;
    (void)      fract;  /* suppress 'unused variable' warning */
    uint16_t    div = 0;
    (void)      div;  /* suppress 'unused variable' warning */
    bool        set_tx = io_mode & I2S_IO_WRITE;
    bool        set_rx = io_mode & I2S_IO_READ;

    /* 
    ** Set the divider input clock to system clock
    */
#if !PSP_MQX_CPU_IS_VYBRID
    i2s_ptr->MCR &= ~I2S_MCR_MICS_MASK;
#endif


    /* ================ General transmitter configuration =================== */ 
    if (set_tx)
    {
        uint8_t tx_channel = ksai_init_ptr->TX_CHANNEL;
        /* 
        ** Software reset -> reset internal transmitter logic including the FIFO writer
        */
        i2s_ptr->TCSR = 0;
        i2s_ptr->TCSR |= I2S_TCSR_SR_MASK;
        i2s_ptr->TCSR &= ~(I2S_TCSR_SR_MASK);
        /*
        ** Enable transmitter in debug mode - this must be done else synchronous mode 
        ** will not work while debugging.
        */
        i2s_ptr->TCSR |= I2S_TCSR_DBGE_MASK; 
        /* 
        ** Set FIFO watermark
        */
        i2s_ptr->TCR1 = I2S_TCR1_TFW(I2S_TX_FIFO_DEFAULT_WATERMARK);      
        /* 
        ** Set the synch mode, clock polarity, master clock source and bit clock 
        */
        i2s_ptr->TCR2 = 0;
        i2s_ptr->TCR2 |= (
            I2S_TCR2_BCP_MASK | /* Bit clock active low           */
            I2S_TCR2_MSEL(1)  | /* MCLK from divider (CLKGEN)     */
            I2S_TCR2_DIV(3)     /* Bit clock frequency = MCLK / 8 */
            );
        /*
        ** First word in frame sets start of word flag
        */
        i2s_ptr->TCR3 = 0;
        i2s_ptr->TCR3 |= (I2S_TCR3_WDFL(0));
        /* 
        ** Enable selected transmitter channel
        */
        switch (tx_channel)
        {
            case 0:
                i2s_ptr->TCR3 |= (I2S_TCR3_TCE(1));
                break;
            case 1:
                i2s_ptr->TCR3 |= (I2S_TCR3_TCE(2));
                break;
            default:
                break;
        }
        /*
        ** Configure frame size, sync width, MSB and frame sync
        */
        i2s_ptr->TCR4 = 0;
        i2s_ptr->TCR4 = (
            I2S_TCR4_FRSZ(1)           | /* Two words in each frame          */
            I2S_TCR4_SYWD(data_bits-1) | /* Synch width as long as data word */
            I2S_TCR4_MF_MASK           | /* MSB shifted out first            */
           	I2S_TCR4_FSE_MASK            /* Frame sync one bit early         */
            );
        /* 
        ** Fist bit shifted is always MSB (bit with highest index in register)
        ** and word length is set according to user configuration.
        */
        i2s_ptr->TCR5 = 0;
        i2s_ptr->TCR5 = (
            I2S_TCR5_WNW(data_bits-1) | /* Word length for Nth word is data_bits   */
            I2S_TCR5_W0W(data_bits-1) | /* Word length for 0th word is data_bits   */
            I2S_TCR5_FBT(data_bits - 1)     /* ??? First bit shifted is on data_bits index */
            );
        /*
        ** Reset word mask 
        */
        i2s_ptr->TMR = 0;
     
    }
    
    /* ================== General receiver configuration ==================== */  
    if (set_rx)
    {
        uint8_t rx_channel = ksai_init_ptr->RX_CHANNEL;
        i2s_ptr->RCSR = 0;
        /* 
        ** Software reset -> reset internal receiver logic including the FIFO writer
        */
        i2s_ptr->RCSR |= I2S_RCSR_SR_MASK;
        i2s_ptr->RCSR &= ~(I2S_RCSR_SR_MASK);
        /*
        ** Enable receiver in debug mode - this must be done else synchronous mode 
        ** will not work while debugging.
        */
        i2s_ptr->RCSR |= I2S_RCSR_DBGE_MASK;
        /* 
        ** Set FIFO watermark
        */
        i2s_ptr->RCR1 = I2S_RCR1_RFW(I2S_RX_FIFO_DEFAULT_WATERMARK);   
        /* 
        ** Set the clock polarity and master clock source 
        */
        i2s_ptr->RCR2 = 0;
        i2s_ptr->RCR2 |= (
            I2S_RCR2_BCP_MASK |                          /* Bit clock active low           */
            I2S_RCR2_MSEL(1)  |                          /* MCLK from divider (CLKGEN)     */
            I2S_RCR2_DIV((DEFAULT_BCLK_DIV / 2) - 1)     /* Bit clock frequency = MCLK / 8 ???*/
            );
        /*
        ** First word in frame sets start of word flag
        */
        i2s_ptr->RCR3 |= (I2S_RCR3_WDFL(0));
        /* 
        ** Enable selected receiver channel
        */
        switch (rx_channel)
        {
            case 0:
                i2s_ptr->RCR3 |= (I2S_RCR3_RCE(1));
                break;
            case 1:
                i2s_ptr->RCR3 |= (I2S_RCR3_RCE(2));
                break;
            default:
                break;
        }
        /*
        ** Configure frame size, sync width, MSB and frame sync
        */
        i2s_ptr->RCR4 = 0;
        i2s_ptr->RCR4 = (
            I2S_RCR4_FRSZ(1)           | /* Two words in each frame          */
            I2S_RCR4_SYWD(data_bits-1) | /* Synch width as long as data word */
            I2S_RCR4_MF_MASK           | /* MSB shifted out first            */
            I2S_RCR4_FSE_MASK            /* Frame sync one bit early         */
            );
        /* 
        ** Fist bit shifted is always MSB (bit with highest index in register)
        ** and word length is set according to user configuration.
        */
        i2s_ptr->RCR5 = 0;
        i2s_ptr->RCR5 = (
            I2S_RCR5_WNW(data_bits-1) | /* Word length for Nth word is data_bits   */
            I2S_RCR5_W0W(data_bits-1) | /* Word length for 0th word is data_bits   */
            I2S_RCR5_FBT(data_bits-1)     /* First bit shifted is on data_bits index */
            );
        /*
        ** Reset word mask 
        */
        i2s_ptr->RMR = 0;
    }
    
    /*================= I2S clock source specific code =======================*/
    switch (clk_source)
    {
        case I2S_CLK_INT:
            /* 
            ** Set divider to get the 12.2880 MHz master clock
            */
#if PSP_MQX_CPU_IS_VYBRID
            _vi2s_set_clk_src_int(ksai_init_ptr->HW_CHANNEL);
            _vi2s_set_sai_div(ksai_init_ptr->HW_CHANNEL, 6);
#else
            _ki2s_find_mclk_freq_div(ksai_init_ptr->MCLK_SRC, &fract, &div, 12288000);
            i2s_ptr->MDR = (I2S_MDR_DIVIDE(div-1) | I2S_MDR_FRACT(fract-1));
            while(i2s_ptr->MCR & I2S_MCR_DUF_MASK) {}; // wait until new value is set
            /* 
            ** MCLK signal pin is configured as an output from the MCLK divider
            ** and the MCLK divider is enabled.
            */
            i2s_ptr->MCR |= (I2S_MCR_MOE_MASK);
            while (i2s_ptr->MCR & I2S_MCR_DUF_MASK) {};
#endif
            break;
        case I2S_CLK_EXT:
            /* 
            ** MCLK signal pin is configured as an input that bypasses
            ** the MCLK divider.
            */
#if PSP_MQX_CPU_IS_VYBRID
            _vi2s_set_clk_src_ext(ksai_init_ptr->HW_CHANNEL);
#else
            i2s_ptr->MCR &= ~(I2S_MCR_MOE_MASK);
            while (i2s_ptr->MCR & I2S_MCR_DUF_MASK) {};
#endif
            break;
        default:
            break;
    }
    /*================= I2S mode of operation specific code ===================*/
    if (i2s_mode & I2S_TX_MASTER)
    {
        /* 
        ** Transmit bit clock generated internally
        */
        i2s_ptr->TCR2 |= (I2S_TCR2_BCD_MASK);
        /*
        ** Transmit frame sync generated internally
        */
        i2s_ptr->TCR4 |= (I2S_TCR4_FSD_MASK);
    }
    if (i2s_mode & I2S_RX_MASTER)
    {
        /*
        ** Receive bit clock generated internally
        */
        i2s_ptr->RCR2 |= (I2S_RCR2_BCD_MASK);
        /* 
        ** Receive frame sync generated internally
        */
        i2s_ptr->RCR4 |= (I2S_RCR4_FSD_MASK);
    }
    if (i2s_mode & I2S_TX_SLAVE)
    {
        /*
        ** Transmit bit clock generated externally
        */
        i2s_ptr->TCR2 &= ~(I2S_TCR2_BCD_MASK);
        /* 
        ** Transmit frame sync generated externally
        */
        i2s_ptr->TCR4 &= ~(I2S_TCR4_FSD_MASK);
    }
    if (i2s_mode & I2S_RX_SLAVE)
    {
        /* 
        ** Receive bit clock generated externally
        */
        i2s_ptr->RCR2 &= ~(I2S_RCR2_BCD_MASK);
        /*
        ** Receive frame sync generated externally
        */
        i2s_ptr->RCR4 &= ~(I2S_RCR4_FSD_MASK);
    }
    /*========================== I2S clock setup =============================*/
    _ki2s_set_bclk_mode(i2s_ptr, bclk_mode);
}


#if PSP_MQX_CPU_IS_VYBRID // PSP_MQX_CPU_IS_VYBRID - Functions specific for Vybrid
/*FUNCTION****************************************************************
*
* Function Name    : _vi2s_get_mclk_value
* Returned Value   : Master clock frequency
* Comments         :
*    This function calculates master clock value from FRACT and DIV.
*
*END*********************************************************************/

static uint32_t _vi2s_get_mclk_value
(
   KSAI_INFO_STRUCT * io_info_ptr /* Device internal info struct */
)
{
    CCM_MemMapPtr ccm = CCM_BASE_PTR;

    CM_CLOCK_SOURCE src = io_info_ptr->MCLK_SRC;
    uint32_t source_freq = 0;
    uint32_t mclk_freq = 0;
    source_freq = _bsp_get_clock(_bsp_get_clock_configuration(), src);
    
    mclk_freq = source_freq / (((ccm->CACRR & CCM_CACRR_PLL4_CLK_DIV_MASK) >> CCM_CACRR_PLL4_CLK_DIV_SHIFT)*2 + 2);
    switch (io_info_ptr->HW_CHANNEL)
    {
        case 0:
            mclk_freq /= ((ccm->CSCDR1 & CCM_CSCDR1_SAI0_DIV_MASK) >> CCM_CSCDR1_SAI0_DIV_SHIFT) + 1;
            break;
        case 1:
            mclk_freq /= ((ccm->CSCDR1 & CCM_CSCDR1_SAI1_DIV_MASK) >> CCM_CSCDR1_SAI1_DIV_SHIFT) + 1;
            break;
        case 2:
            mclk_freq /= ((ccm->CSCDR1 & CCM_CSCDR1_SAI2_DIV_MASK) >> CCM_CSCDR1_SAI2_DIV_SHIFT) + 1;
            break;
        case 3:
            mclk_freq /= ((ccm->CSCDR1 & CCM_CSCDR1_SAI3_DIV_MASK) >> CCM_CSCDR1_SAI3_DIV_SHIFT) + 1;
            break;
        default:
            break;
    }
    return (mclk_freq);
}

/*FUNCTION****************************************************************
*
* Function Name    : _vi2s_set_mclk_value
* Returned Value   : None
* Comments         :
*    This function sets master clock frequency to closest to selected value.
*
*END*********************************************************************/
static void _vi2s_set_mclk_freq(KSAI_INFO_STRUCT * io_info_ptr, uint32_t freq)
{
    uint32_t audioclks[CPU_PLL_AUDIO_CLK_NUM] = {
                            CPU_PLL_AUDIO_CLK_HZ_0,
                            CPU_PLL_AUDIO_CLK_HZ_1,
                            CPU_PLL_AUDIO_CLK_HZ_2,
                            CPU_PLL_AUDIO_CLK_HZ_3,
                            CPU_PLL_AUDIO_CLK_HZ_4,
                            };

    uint32_t remainders[CPU_PLL_AUDIO_CLK_NUM];
    
    uint32_t result = 0;
    uint32_t source_freq = 0;
    CCM_MemMapPtr ccm = CCM_BASE_PTR;
    uint8_t pll_div = 4;
    uint8_t sai_div = 1;
    uint8_t temp_pll_div = 0;
    uint8_t temp_sai_div = 0;
    bool found = FALSE;
    int clkIdx = 0;
    uint32_t min_reminder_index = 0;
    uint32_t min_reminder = 0xFFFFFFFF;

    /* Calculate remainders */
    for(clkIdx = 0; clkIdx < CPU_PLL_AUDIO_CLK_NUM; clkIdx++)
    {
        remainders[clkIdx] = audioclks[clkIdx] % freq;
        
        if(remainders[clkIdx] < min_reminder)
        {
            min_reminder = remainders[clkIdx];
            min_reminder_index = clkIdx;
        }
    }

    //Maybe have above two min reminder clocks.
    //The first choice is the one which freq = audioclks[clkIdx]/(pll_div * sai_div)
    for(clkIdx = min_reminder_index; clkIdx < CPU_PLL_AUDIO_CLK_NUM; clkIdx++)
    {
        if(remainders[clkIdx] > min_reminder)
        {
            continue;
        }
        _bsp_set_clock(audioclks[clkIdx], io_info_ptr->MCLK_SRC);
        /* Read PLL4 freqency */
        source_freq =  _bsp_get_clock(_bsp_get_clock_configuration(), io_info_ptr->MCLK_SRC);

        /* Find dividers values for selecter master clock frequency */
        for (pll_div = 4; pll_div < PLL_DIV_MAX; pll_div += 2)
        {
           for(sai_div = 1; sai_div < SAI_DIV_MAX; sai_div++)
           {
               uint32_t iter_mclk = source_freq / (pll_div * sai_div);  
               if (iter_mclk < freq) break; // Value too low, try next setting
               if ((iter_mclk - freq) == 0) // Found value that matches exactly 
               {
                   found = TRUE;
                   break;
               }
               /* 
               ** If a new iteration of mclk is better than last that we have adapt 
               ** it as the new best value.
               */
               //??? should use diff to compare
               //else if ((result == 0) || ((iter_mclk % freq) < (result % freq)))
               else if ((result == 0) || ((iter_mclk - freq) < (result - freq)))
               {
                   temp_pll_div = pll_div;
                   temp_sai_div = sai_div;
                   result = iter_mclk;
               }
           }
           if (found) break;
        }

        if(found) break;

        pll_div = temp_pll_div;
        sai_div = temp_sai_div;
        
    }

    /* Set PLL4 divider */
    ccm->CACRR = (ccm->CACRR & ~CCM_CACRR_PLL4_CLK_DIV_MASK) | CCM_CACRR_PLL4_CLK_DIV(pll_div/2 - 1);
    
    /* Set SAI divider */
    _vi2s_set_sai_div(io_info_ptr->HW_CHANNEL, sai_div);
}

/*FUNCTION****************************************************************
*
* Function Name    : _vi2s_set_clk_src_int
* Returned Value   : None
* Comments         :
*    This function selects internal clock source for appropriate SAI peripheral
*
*END*********************************************************************/
static void _vi2s_set_clk_src_int(uint8_t  hw_channel)
{
    CCM_MemMapPtr ccm = CCM_BASE_PTR;
    
    switch (hw_channel)
    {
        case 0: //SAI0_CLK_SEL
            /* Select internal clock source (PLL4) */
            ccm->CSCMR1 = (ccm->CSCMR1 & ~CCM_CSCMR1_SAI0_CLK_SEL_MASK) | CCM_CSCMR1_SAI0_CLK_SEL(3);
            /* Enable SAI0 master clock */
            ccm->CSCDR1 |= CCM_CSCDR1_SAI0_EN_MASK;
            break;
        case 1: //SAI1_CLK_SEL
            ccm->CSCMR1 = (ccm->CSCMR1 & ~CCM_CSCMR1_SAI1_CLK_SEL_MASK) | CCM_CSCMR1_SAI1_CLK_SEL(3);
            /* Enable SAI1 master clock */
            ccm->CSCDR1 |= CCM_CSCDR1_SAI1_EN_MASK;
            break;
        case 2: //SAI2_CLK_SEL
            ccm->CSCMR1 = (ccm->CSCMR1 & ~CCM_CSCMR1_SAI2_CLK_SEL_MASK) | CCM_CSCMR1_SAI2_CLK_SEL(3);
            /* Enable SAI2 master clock */
            ccm->CSCDR1 |= CCM_CSCDR1_SAI2_EN_MASK;
            break;
        case 3: //SAI3_CLK_SEL
            ccm->CSCMR1 = (ccm->CSCMR1 & ~CCM_CSCMR1_SAI3_CLK_SEL_MASK) | CCM_CSCMR1_SAI3_CLK_SEL(3);
            /* Enable SAI3 master clock */
            ccm->CSCDR1 |= CCM_CSCDR1_SAI3_EN_MASK;
            break;
        default:
            break;
    }
}

/*FUNCTION****************************************************************
*
* Function Name    : _vi2s_set_clk_src_ext
* Returned Value   : None
* Comments         :
*    This function selects external clock source for appropriate SAI peripheral
*
*END*********************************************************************/
static void _vi2s_set_clk_src_ext(uint8_t hw_channel)
{
    CCM_MemMapPtr ccm = CCM_BASE_PTR;
    
    switch (hw_channel)
    {
        case 0: //SAI0_CLK_SEL
            /* Clock source for SAI0 is Audio External Clock */
            ccm->CSCMR1 &= ~(CCM_CSCMR1_SAI0_CLK_SEL_MASK);
            break;
        case 1: //SAI1_CLK_SEL
            ccm->CSCMR1 &= ~(CCM_CSCMR1_SAI1_CLK_SEL_MASK);
            break;
        case 2: //SAI2_CLK_SEL
            ccm->CSCMR1 &= ~(CCM_CSCMR1_SAI2_CLK_SEL_MASK);
            break;
        case 3: //SAI3_CLK_SEL
            ccm->CSCMR1 &= ~(CCM_CSCMR1_SAI3_CLK_SEL_MASK);
            break;
        default:
            break;
    }
    /* Set SAI clock divider to 1 (do not divide external clock) */
    _vi2s_set_sai_div(hw_channel, 1);
}

/*FUNCTION****************************************************************
*
* Function Name    : _vi2s_set_sai_div
* Returned Value   : None
* Comments         :
*    This function sets SAI clock divider for selected SAI peripheral
*
*END*********************************************************************/
static void _vi2s_set_sai_div(uint8_t hw_channel, uint8_t value)
{
    CCM_MemMapPtr ccm = CCM_BASE_PTR;
    
    switch (hw_channel)
    {
        case 0: //SAI0
            ccm->CSCDR1 = ((ccm->CSCDR1 & (~CCM_CSCDR1_SAI0_DIV_MASK)) | CCM_CSCDR1_SAI0_DIV(value-1));
            break;
        case 1: //SAI1
            ccm->CSCDR1 = ((ccm->CSCDR1 & (~CCM_CSCDR1_SAI1_DIV_MASK)) | CCM_CSCDR1_SAI1_DIV(value-1));
            break;
        case 2: //SAI2
            ccm->CSCDR1 = ((ccm->CSCDR1 & (~CCM_CSCDR1_SAI2_DIV_MASK)) | CCM_CSCDR1_SAI2_DIV(value-1));
            break;  
        case 3: //SAI3
            ccm->CSCDR1 = ((ccm->CSCDR1 & (~CCM_CSCDR1_SAI3_DIV_MASK)) | CCM_CSCDR1_SAI3_DIV(value-1));
            break;
        default:
            break;
    }
}
#else // !PSP_MQX_CPU_IS_VYBRID - Functions specific for Kinetis
/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_get_mclk_value
* Returned Value   : Master clock frequency
* Comments         :
*    This function calculates master clock value from FRACT and DIV.
*
*END*********************************************************************/
static uint32_t _ki2s_get_mclk_value
(
   KSAI_INFO_STRUCT * io_info_ptr
)
{
    I2S_MemMapPtr i2s_ptr = io_info_ptr->SAI_PTR;
    uint32_t freq = _bsp_get_clock(_bsp_get_clock_configuration(), io_info_ptr->MCLK_SRC);
    uint32_t fract = 0;
    uint32_t div = 0;
    fract = (i2s_ptr->MDR & I2S_MDR_FRACT_MASK) >> I2S_MDR_FRACT_SHIFT;
    div = (i2s_ptr->MDR & I2S_MDR_DIVIDE_MASK) >> I2S_MDR_DIVIDE_SHIFT;
    return ((freq / (div + 1)) * (fract + 1));
}

#endif // END OF PSP_MQX_CPU_IS_VYBRID - Functions specific for platform

/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_find_mclk_freq_div
* Returned Value   : Master clock frequency  
* Comments         :
*    This function tries to find master clock divider value required for 
*    selected master clock frequency.
*
*END*********************************************************************/
uint32_t _ki2s_find_mclk_freq_div
(
    CM_CLOCK_SOURCE src,      /* Master clock source               */
    uint16_t *     frct,     /* Calculated FRACT value            */
    uint16_t *     div,      /* Calculated DIVIDER value          */
    uint32_t         frequency /* Calculated master clock frequency */
)
{
    uint32_t closest_mclk = 0;
    uint32_t temp_fract = FRACT_MAX;
    uint32_t temp_div = DIV_MAX;
    uint32_t iter_mclk;
    uint16_t fract = 0;
    uint16_t divide = 0;
    bool found = FALSE;
    uint32_t source_freq = 0;

    source_freq = _bsp_get_clock(_bsp_get_clock_configuration(), src);
    /*
    ** Find best divider value for selected mclk frequency.
    */
    for (fract = 1; fract < FRACT_MAX; fract++)
    {
       for(divide = 1; divide < DIV_MAX; divide++)
       {
           iter_mclk = (source_freq / divide) * fract;
           if (iter_mclk < frequency) break; // Value too low, try next setting
           if ((iter_mclk - frequency) == 0) // Found value that matches exactly 
           {
               found = TRUE;
               break;
           }
           /* 
           ** If a new iteration of mclk is better than last that we have adapt 
           ** it as the new best value.
           */
           else if ((closest_mclk == 0) || ((iter_mclk % frequency) < (closest_mclk % frequency)))
           {
               temp_fract = fract;
               temp_div = divide;
               closest_mclk = iter_mclk;
           }
       }
       if (found) break;
    }
    /* 
    ** If we failed to find exact match use closest value we have.
    */
    if (found == FALSE)
    {
       fract = temp_fract;
       divide = temp_div;
       frequency = closest_mclk;
    }
    *frct = fract;
    *div = divide;
    return(frequency);
}
/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_set_master_mode
* Returned Value   : I2S_OK if everything is OK, error code otherwise.  
* Comments         :
*    This function sets up master mode for TX and RX (called from ioctl)
*
*END*********************************************************************/
uint32_t _ki2s_set_master_mode
(
    KSAI_INFO_STRUCT * io_info_ptr, /* Info struct                   */
    uint8_t  mode                       /* Required master mode settings */
)
{
    I2S_MemMapPtr i2s_ptr = io_info_ptr->SAI_PTR;
    bool set_tx_as_master = ((mode & I2S_TX_MASTER) >> 4) && (io_info_ptr->IO_MODE & I2S_IO_WRITE);
    bool set_rx_as_master = (mode & I2S_RX_MASTER) && (io_info_ptr->IO_MODE & I2S_IO_READ);
    /* 
    ** If the master clock source is internal, enable it and recalculate
    ** values of frequencies in the internal info structure.
    */
    if (io_info_ptr->CLOCK_SOURCE == I2S_CLK_INT)
    {
#if PSP_MQX_CPU_IS_VYBRID
        _vi2s_set_clk_src_int(io_info_ptr->HW_CHANNEL);
        io_info_ptr->MCLK_FREQ = _vi2s_get_mclk_value(io_info_ptr);
#else
        i2s_ptr->MCR |= (I2S_MCR_MOE_MASK);
        io_info_ptr->MCLK_FREQ = _ki2s_get_mclk_value(io_info_ptr); 
#endif   
        io_info_ptr->BCLK_FREQ = io_info_ptr->MCLK_FREQ / io_info_ptr->BCLK_DIV;
        io_info_ptr->FS_FREQ = io_info_ptr->BCLK_FREQ / (io_info_ptr->DATA_BITS * MYCHANNELS);
    }
    /*
    ** Else disable master clock signal and set pin as input. Also reset values
    ** of frequencies in the info structure.
    */
    else if (io_info_ptr->CLOCK_SOURCE == I2S_CLK_EXT)
    {
#if PSP_MQX_CPU_IS_VYBRID
        _vi2s_set_clk_src_ext(io_info_ptr->HW_CHANNEL);
#else
        i2s_ptr->MCR &= ~(I2S_MCR_MOE_MASK);
#endif
        io_info_ptr->MCLK_FREQ = 0;
        io_info_ptr->BCLK_FREQ = 0;
        io_info_ptr->FS_FREQ = 0;
    }
    /*
    ** In case we want to set the transmitter and it is enabled, device is busy.
    */
    if ((i2s_ptr->TCSR & I2S_TCSR_TE_MASK) && (set_tx_as_master))
    {
        return(I2S_ERROR_DEVICE_BUSY);
    }
    if ((io_info_ptr->CLOCK_MODE & I2S_TX_ASYNCHRONOUS) && (set_tx_as_master))
    {
        /* Reset Tx mode setting */
        io_info_ptr->MODE &= 0x0F;
        /* Set Tx into master mode */
        io_info_ptr->MODE |= I2S_TX_MASTER;
        i2s_ptr->TCR2 |= (I2S_TCR2_BCD_MASK);
        i2s_ptr->TCR4 |= (I2S_TCR4_FSD_MASK);
        i2s_ptr->TCR4 |= (I2S_TCR4_FSP_MASK);
        /* 
        ** If the the receiver is synchronous settings must same as the
        ** transmitter settings.
        */
        if (io_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS)
        {
            io_info_ptr->MODE &= 0xF0;
            io_info_ptr->MODE |= I2S_RX_MASTER;
            i2s_ptr->RCR2 |= (I2S_RCR2_BCD_MASK);
            i2s_ptr->RCR4 |= (I2S_RCR4_FSD_MASK);
        }
        /* 
        ** If transmitter have swapped bit clock. set receiver bit clock to be
        ** generated internally
        */
        if (io_info_ptr->CLOCK_MODE & I2S_TX_BCLK_SWAPPED)
        {
            i2s_ptr->RCR2 |= (I2S_RCR2_BCD_MASK);
        }
    }
    
    /*
    ** In case we want to the set receiver and it is enabled, device is busy.
    */
    if ((i2s_ptr->RCSR & I2S_RCSR_RE_MASK) && (set_rx_as_master))
    {
        return(I2S_ERROR_DEVICE_BUSY);
    }
    if ((io_info_ptr->CLOCK_MODE & I2S_RX_ASYNCHRONOUS) && (set_rx_as_master))
    {
        /* Reset Rx mode setting */
        io_info_ptr->MODE &= 0xF0;
        /* Set Rx in master mode */
        io_info_ptr->MODE |= I2S_RX_MASTER;
        i2s_ptr->RCR2 |= (I2S_RCR2_BCD_MASK);
        i2s_ptr->RCR4 |= (I2S_RCR4_FSD_MASK);
        /* 
        ** If the the transmitter is synchronous settings must same as the
        ** receiver settings.
        */
        if (io_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS)
        {
            
            io_info_ptr->MODE &= 0x0F;
            io_info_ptr->MODE |= I2S_TX_MASTER;
            i2s_ptr->TCR2 |= (I2S_TCR2_BCD_MASK);
            i2s_ptr->TCR4 |= (I2S_TCR4_FSD_MASK);
        }
        /* 
        ** If receiver have swapped bit clock. Set transmitter bit clock to be
        ** generated internally.
        */
        if (io_info_ptr->CLOCK_MODE & I2S_RX_BCLK_SWAPPED)
        {
            i2s_ptr->TCR2 |= (I2S_TCR2_BCD_MASK);
        }
    }
    return(I2S_OK);
}
/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_set_slave_mode
* Returned Value   : I2S_OK if everything is OK, error code otherwise.  
* Comments         :
*    This function sets up slave mode for TX and RX (called from ioctl)
*
*END*********************************************************************/
uint32_t _ki2s_set_slave_mode
(
    KSAI_INFO_STRUCT * io_info_ptr, /* Info struct                  */
    uint8_t  mode                       /* Required slave mode settings */
)
{
    I2S_MemMapPtr i2s_ptr = io_info_ptr->SAI_PTR;
    bool set_tx_as_slave = ((mode & I2S_TX_SLAVE) >> 4) && (io_info_ptr->IO_MODE & I2S_IO_WRITE);
    bool set_rx_as_slave = (mode & I2S_RX_SLAVE) && (io_info_ptr->IO_MODE & I2S_IO_READ);
    /*
    ** Disable the master clock signal and set pin as input. Also reset values
    ** of frequencies in the info structure.
    */
#if PSP_MQX_CPU_IS_VYBRID
    _vi2s_set_clk_src_ext(io_info_ptr->HW_CHANNEL);
#else
    i2s_ptr->MCR &= ~(I2S_MCR_MOE_MASK);
#endif
    io_info_ptr->MCLK_FREQ = 0;
    io_info_ptr->BCLK_FREQ = 0;
    io_info_ptr->FS_FREQ = 0;
    /*
    ** In case we want to setup the transmitter and it is enabled, device is busy.
    */
    if ((i2s_ptr->TCSR & I2S_TCSR_TE_MASK) && (set_tx_as_slave))
    {
        return(I2S_ERROR_DEVICE_BUSY);
    }
    if ((io_info_ptr->CLOCK_MODE & I2S_TX_ASYNCHRONOUS) && (set_tx_as_slave))
    {
        /* Reset Tx mode setting */
        io_info_ptr->MODE &= 0x0F;
        /* Set Tx into slave mode */
        io_info_ptr->MODE |= I2S_TX_SLAVE;
        i2s_ptr->TCR2 &= ~(I2S_TCR2_BCD_MASK);
        i2s_ptr->TCR4 &= ~(I2S_TCR4_FSD_MASK);
        /* 
        ** If the the receiver is synchronous settings must same as the
        ** transmitter settings.
        */
        if (io_info_ptr->CLOCK_MODE & I2S_RX_SYNCHRONOUS)
        {
            io_info_ptr->MODE &= 0xF0;
            io_info_ptr->MODE |= I2S_RX_SLAVE;
            i2s_ptr->RCR2 &= ~(I2S_RCR2_BCD_MASK);
            i2s_ptr->RCR4 &= ~(I2S_RCR4_FSD_MASK);
        }
        /* 
        ** If transmitter have swapped bit clock set receiver bit clock to be
        ** generated externally
        */
        if (io_info_ptr->CLOCK_MODE & I2S_TX_BCLK_SWAPPED)
        {
            i2s_ptr->RCR2 &= ~(I2S_RCR2_BCD_MASK);
        }
    }
    
    /*
    ** In case we want to the setup receiver and it is enabled, device is busy.
    */
    if ((i2s_ptr->RCSR & I2S_RCSR_RE_MASK) && (set_rx_as_slave))
    {
        return(I2S_ERROR_DEVICE_BUSY);
    }
    if ((io_info_ptr->CLOCK_MODE & I2S_RX_ASYNCHRONOUS) && (set_rx_as_slave))
    {
        /* Reset Rx mode setting */
        io_info_ptr->MODE &= 0xF0;
        /* Set Rx in master mode */
        io_info_ptr->MODE |= I2S_RX_SLAVE;
        i2s_ptr->RCR2 &= ~(I2S_RCR2_BCD_MASK);
        i2s_ptr->RCR4 &= ~(I2S_RCR4_FSD_MASK);
        /* 
        ** If the the transmitter is synchronous settings must same as the
        ** receiver settings.
        */
        if (io_info_ptr->CLOCK_MODE & I2S_TX_SYNCHRONOUS)
        {
            
            io_info_ptr->MODE &= 0x0F;
            io_info_ptr->MODE |= I2S_TX_SLAVE;
            i2s_ptr->TCR2 &= ~(I2S_TCR2_BCD_MASK);
            i2s_ptr->TCR4 &= ~(I2S_TCR4_FSD_MASK);
        }
        /* 
        ** If receiver have swapped bit clock. Set transmitter bit clock to be
        ** generated externally.
        */
        if (io_info_ptr->CLOCK_MODE & I2S_RX_BCLK_SWAPPED)
        {
            i2s_ptr->TCR2 &= ~(I2S_TCR2_BCD_MASK);
        }
    }
    return(I2S_OK);
}

/*FUNCTION****************************************************************
*
* Function Name    : _ki2s_set_bclk_mode
* Returned Value   : I2S_OK if everything is OK, error code otherwise.  
* Comments         :
*       This function modifies SAI registers to set up synchronous mode and
*       clock swapping.
*
*END*********************************************************************/
uint32_t _ki2s_set_bclk_mode
(
    I2S_MemMapPtr i2s_ptr, /* void * to registers */
    uint8_t  bclk_mode       /* Bit clock mode       */
)
{
    if ((bclk_mode & I2S_TX_SYNCHRONOUS) && (bclk_mode & I2S_RX_SYNCHRONOUS))
    {
        /* Invalid combination of bclk modes */
        return(I2S_ERROR_INVALID_PARAMETER);
    }
    /*
    ** Transmitter clock:
    */
    if (bclk_mode & I2S_TX_ASYNCHRONOUS) // Transmitter is asynchronous
    {
       i2s_ptr->TCR2 &= ~(I2S_TCR2_SYNC_MASK);
    }
    else if (bclk_mode & I2S_TX_SYNCHRONOUS) // Transmitter is synchronous
    {
       /*
       ** If transmitter is synchronous, receiver must be asynchronous
       */
       if (i2s_ptr->RCR2 & I2S_RCR2_SYNC_MASK)
       {
          /* Invalid combination of bclk modes */
          return(I2S_ERROR_INVALID_PARAMETER); 
       }
       i2s_ptr->TCR2 |= (I2S_TCR2_SYNC(1));
    }
   
    if (bclk_mode & I2S_TX_BCLK_NORMAL) // Transmitter BCLK not swapped
    {
        i2s_ptr->TCR2 &= ~(I2S_TCR2_BCS_MASK);
    }
    else if (bclk_mode & I2S_TX_BCLK_SWAPPED) // Transmitter BCLK swapped
    {
       /*
       ** TX_BCLK = SAI_RX_BCLK
       ** TX_FS = SAI_TX_SYNC
       */
       i2s_ptr->TCR2 |= (I2S_TCR2_BCS_MASK);
       /*
       ** When Tx is synch. BCS bit must be set also for Rx.
       */
       if (bclk_mode & I2S_TX_SYNCHRONOUS) 
       {
           /*
           ** TX_BCLK = SAI_TX_BCLK
           ** TX_FS = SAI_RX_SYNC
           */
           i2s_ptr->RCR2 |= (I2S_RCR2_BCS_MASK);
       }
    }
   
    /*
    ** Receiver clock:
    */
    if (bclk_mode & I2S_RX_ASYNCHRONOUS) // Receiver is asynchronous
    {
        i2s_ptr->RCR2 &= ~(I2S_RCR2_SYNC_MASK);
    }
    else if (bclk_mode & I2S_RX_SYNCHRONOUS) // Receiver is synchronous
    {
        /*
        ** If receiver is synchronous, transmitter must be asynchronous
        */
        if (i2s_ptr->TCR2 & I2S_TCR2_SYNC_MASK)
        {
            /* Invalid combination of bclk modes */
            return(I2S_ERROR_INVALID_PARAMETER); 
        }
        i2s_ptr->RCR2 |= (I2S_RCR2_SYNC(1));
    }
   
    if (bclk_mode & I2S_RX_BCLK_NORMAL) // Receiver BCLK not swapped
    {
        i2s_ptr->RCR2 &= ~(I2S_RCR2_BCS_MASK);
    }
    else if (bclk_mode & I2S_RX_BCLK_SWAPPED) // Receiver BCLK swapped
    {
        /*
        ** RX_BCLK = SAI_TX_BCLK
        ** RX_FS = SAI_RX_SYNC
        */
        i2s_ptr->RCR2 |= (I2S_RCR2_BCS_MASK);
        /*
        ** When Rx is synch. BCS bit must be set also for Tx.
        */
        if (bclk_mode & I2S_RX_SYNCHRONOUS)
        {
            /*
            ** RX_BCLK = SAI_RX_BCLK
            ** RX_FS = SAI_TX_SYNC
            */
            i2s_ptr->TCR2 |= (I2S_TCR2_BCS_MASK);
        }
    }
    return(I2S_OK);
}
/* EOF */
