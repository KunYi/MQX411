/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
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
*   The file contains low level QSPI driver functions for QSPI module
*
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include <io_prv.h>

#include "qspi.h"
#include "qspi_prv.h"
#include "qspi_quadspi_prv.h"

#if BSPCFG_ENABLE_CLKTREE_MGMT
CLOCK_NAME quadspi_src_clk_name[] = {
    CLK_PLL3,
    CLK_PLL3_PFD4,
    CLK_PLL2_PFD4,
    CLK_PLL1_PFD4,
    CLK_PLATFORM_BUS,
};

CLOCK_NAME quadspi_clk_name[] = {
    CLK_QSPI0,
    CLK_QSPI1
};
#else
CM_CLOCK_SOURCE quadspi_src_clocks[] = {
    CM_CLOCK_SOURCE_PLL_USB0,
    CM_CLOCK_SOURCE_PLL3_PFD4,
    CM_CLOCK_SOURCE_PLL2_PFD4,
    CM_CLOCK_SOURCE_PLL1_PFD4,
};
#endif

#define QuadSPI_MAX_CMD_SET     8
#define QuadSPI_PROM_LUT        15
#define QuadSPI_MIN_FREQ        33000000

#ifdef QuadSPI_ENABLE_DEBUG
#define QuadSPI_DEBUG  printf
#else
#define QuadSPI_DEBUG(...)
#endif

/* QuadSPI low level driver interface functions */
static _mqx_int _quadspi_init(const void  *init_data_ptr,
                              void **io_info_ptr_ptr);
static _mqx_int _quadspi_deinit(void *io_info_ptr);
static _mqx_int _quadspi_tx_rx(void *io_info_ptr, uint8_t *txbuf,
                               uint8_t *rxbuf, uint32_t len);
static _mqx_int _quadspi_ioctl(void *io_info_ptr,
                               uint32_t cmd,
                               uint32_t *param_ptr);

const QSPI_DEVIF_STRUCT _qspi_quadspi_devif = {
    _quadspi_init,
    _quadspi_deinit,
    _quadspi_tx_rx,
    _quadspi_ioctl
};

/* Forward declarations */
static _mqx_int _quadspi_init_setup(VQuadSPI_REG_STRUCT_PTR qspi_reg_ptr,
                                    QuadSPI_INIT_STRUCT_PTR quadspi_init_ptr,
                                    uint32_t flashA1_top_addr,
                                    uint32_t flashA2_top_addr,
                                    uint32_t flashB1_top_addr,
                                    uint32_t flashB2_top_addr);
static void _quadspi_set_ahb_read(VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,
                                    QuadSPI_CLK_MODE clk_mode,
                                    QuadSPI_IO_MODE read_io_mode,
                                    bool enable_parallel_mode);
static int32_t _quadspi_read(QuadSPI_INFO_STRUCT_PTR qspi_info_ptr,
                                    uint8_t *src_ptr,
                                    uint8_t *dest_ptr,
                                    _mem_size size);

static int32_t _quadspi_write(QuadSPI_INFO_STRUCT_PTR qspi_info_ptr,
                                    uint8_t *src_ptr,
                                    uint8_t *dest_ptr,
                                    _mem_size size);

static void _quadspi_disable_clock(uint32_t quadspi_id);


/*FUNCTION****************************************************************
*
* Function Name    : _qspi_init
* Returned Value   : MQX error code
* Comments         :
*    This function initializes the QSPI driver
*
*END*********************************************************************/
static _mqx_int _quadspi_init
    (
        /* [IN] The initialization information for the device being opened */
        const void                *init_data_ptr,

        /* [OUT] The address to store device specific information */
        void                          **io_info_ptr_ptr
    )
{
    QuadSPI_INIT_STRUCT_PTR quadspi_init_ptr = (QuadSPI_INIT_STRUCT_PTR)init_data_ptr;
    QuadSPI_INFO_STRUCT_PTR quadspi_info_ptr;
    VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr;
    uint32_t quadspi_mem;
    uint32_t flash_top_addr[4];
    QuadSPI_FLASH_INFO_STRUCT_PTR flash_dev;

    uint32_t i, actual_serial_clk;
    _mqx_int retval;

    /* Check QSPI init data */
    if (quadspi_init_ptr->CLK_MODE != QuadSPI_CLK_SDR_MODE &&
        quadspi_init_ptr->CLK_MODE != QuadSPI_CLK_DDR_MODE) {
        return QuadSPI_INVAID_PARAMETER;
    }

    if (quadspi_init_ptr->IO_MODE != QuadSPI_SINGLE_MODE &&
        quadspi_init_ptr->IO_MODE != QuadSPI_DUAL_MODE &&
        quadspi_init_ptr->IO_MODE != QuadSPI_QUAD_MODE) {
        return QuadSPI_INVAID_PARAMETER;
    }

    if (quadspi_init_ptr->PAGE_SIZE <  QuadSPI_PAGE_128 ||
        quadspi_init_ptr->PAGE_SIZE > QuadSPI_PAGE_512) {
        return QuadSPI_INVAID_PARAMETER;
    }


    /* Get the QuadSPI register base address */
    quadspi_reg_ptr = _bsp_get_quadspi_reg_address (quadspi_init_ptr->MODULE_ID);
    if (quadspi_reg_ptr == NULL) {
        return SPI_ERROR_CHANNEL_INVALID;
    }

    /* Get QuadSPI memory base address */
    quadspi_mem = _bsp_get_quadspi_base_address (quadspi_init_ptr->MODULE_ID);
    if (quadspi_mem == (uint32_t) NULL) {
        return SPI_ERROR_CHANNEL_INVALID;
    }
    flash_dev = quadspi_init_ptr->FLASH_DEVICE_INFO;
    for (i = 0; i < 4; i++) {
        flash_top_addr[i] = quadspi_mem + flash_dev->START_ADDR +
                        flash_dev->SECTOR_SIZE * flash_dev->NUM_SECTOR;
        flash_dev++;
    }

    /* Initialize the QuadSPI io information pointer */
    quadspi_info_ptr =(QuadSPI_INFO_STRUCT_PTR)_mem_alloc_system_zero((uint32_t)sizeof(QuadSPI_INFO_STRUCT));
    if (quadspi_info_ptr == NULL) {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type (quadspi_info_ptr, MEM_TYPE_IO_SPI_INFO_STRUCT);
    *io_info_ptr_ptr = (void *) quadspi_info_ptr;
    quadspi_info_ptr->QuadSPI_REG_PTR = quadspi_reg_ptr;
    quadspi_info_ptr->QuadSPI_MEM_ADDR = quadspi_mem;
    quadspi_info_ptr->FLASH_CUR_ADDR = quadspi_mem;
    quadspi_info_ptr->MODULE_ID = quadspi_init_ptr->MODULE_ID;
    quadspi_info_ptr->READ_CLK = quadspi_init_ptr->READ_CLK;
    quadspi_info_ptr->WRITE_CLK = quadspi_init_ptr->WRITE_CLK;
    quadspi_info_ptr->CLK_MODE = quadspi_init_ptr->CLK_MODE;
    quadspi_info_ptr->IO_MODE = quadspi_init_ptr->IO_MODE;
    quadspi_info_ptr->PAGE_SIZE = quadspi_init_ptr->PAGE_SIZE;

    quadspi_info_ptr->FLASH_TOP_ADDR[0] = flash_top_addr[0];
    quadspi_info_ptr->FLASH_TOP_ADDR[1] = flash_top_addr[1];
    quadspi_info_ptr->FLASH_TOP_ADDR[2] = flash_top_addr[2];
    quadspi_info_ptr->FLASH_TOP_ADDR[3] = flash_top_addr[3];
    quadspi_info_ptr->HW_BLOCK = quadspi_init_ptr->FLASH_DEVICE_INFO;
    quadspi_info_ptr->FLASH_READ_FLAG = QuadSPI_FLASH_READ;
    quadspi_info_ptr->PARALLEL_ENABLED = quadspi_init_ptr->PARALLEL_ENABLED;
    quadspi_info_ptr->IP_READ_IN_PARALLEL = FALSE;

    /* Configure IOMUX for QuadSPI */
    if (_bsp_quadspi_io_init (quadspi_init_ptr->MODULE_ID) == -1) {
        _mem_free(quadspi_info_ptr);
        return SPI_ERROR_CHANNEL_INVALID;
    }

    /* Enable clocks to default value(33Mhz), the lowest read speed */
    actual_serial_clk = _quadspi_flash_set_serial_clk(quadspi_reg_ptr,
                                                      quadspi_init_ptr->MODULE_ID,
                                                      QuadSPI_MIN_FREQ);
    if (actual_serial_clk == 0) {
        _mem_free(quadspi_info_ptr);
        return QuadSPI_INVAID_PARAMETER;
    }

#if BSPCFG_ENABLE_CLKTREE_MGMT
    _quadspi_flash_enable_serial_clk(quadspi_init_ptr->MODULE_ID);
#endif

    /* initialize quadspi driver */
    retval = _quadspi_init_setup(quadspi_reg_ptr, quadspi_init_ptr,
                                 flash_top_addr[0], flash_top_addr[1],
                                 flash_top_addr[2], flash_top_addr[3]);
    if (retval < 0) {
        _mem_free(quadspi_info_ptr);
        return retval;
    }

    /*change to specific read speed*/
    if (quadspi_init_ptr->READ_CLK != QuadSPI_MIN_FREQ) {
        actual_serial_clk = _quadspi_flash_set_serial_clk(quadspi_reg_ptr,
                                                          quadspi_init_ptr->MODULE_ID,
                                                          quadspi_init_ptr->READ_CLK );
        if (actual_serial_clk == 0) {
            _mem_free(quadspi_info_ptr);
            return QuadSPI_INVAID_PARAMETER;
        } else {
           quadspi_init_ptr->READ_CLK = actual_serial_clk;
        }
    }

    /* TBD: Disable interrrupt and clear flags */

    /* TBD: Install ISRs */

    return QSPI_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _quadspi_deinit
* Returned Value   : MQX error code
* Comments         :
*    This function de-initializes the QuadSPI module
*
*END*********************************************************************/
static _mqx_int _quadspi_deinit
    (
        /* [IN] the address of the device specific information */
        void                          *io_info_ptr
    )
{
    QuadSPI_INFO_STRUCT_PTR quadspi_info_ptr = (QuadSPI_INFO_STRUCT_PTR)io_info_ptr;

    if (quadspi_info_ptr)
    {
        _quadspi_disable_clock(quadspi_info_ptr->MODULE_ID);
        _mem_free(quadspi_info_ptr);
    }

#if BSPCFG_ENABLE_CLKTREE_MGMT
    _quadspi_flash_disable_serial_clk(quadspi_info_ptr->MODULE_ID);
#endif

    return QSPI_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _quadspi_tx_rx
* Returned Value   : number of bytes transferred
* Comments         :
*   Actual transmit and receive function.
*   Overrun prevention used, no need to update statistics in this function
*
*END*********************************************************************/
static _mqx_int _quadspi_tx_rx
    (
        /* [IN] Device specific context structure */
        void                          *io_info_ptr,

        /* [IN] Data to transmit */
        uint8_t                     *txbuf,

        /* [OUT] Received data */
        uint8_t                     *rxbuf,

        /* [IN] Length of transfer in bytes */
        uint32_t                        len
    )
{
    QuadSPI_INFO_STRUCT_PTR               quadspi_info_ptr = (QuadSPI_INFO_STRUCT_PTR)io_info_ptr;
    uint8_t *flash_src_ptr = (uint8_t *) quadspi_info_ptr->FLASH_CUR_ADDR;
    _mqx_int retlen = -1;

    /* check parameter: invalid current flash address */
    if (quadspi_info_ptr->FLASH_CUR_ADDR < quadspi_info_ptr->QuadSPI_MEM_ADDR ||
        quadspi_info_ptr->FLASH_CUR_ADDR >= quadspi_info_ptr->FLASH_TOP_ADDR[3] ) {
        printf("\nERROR: invalid flash address 0x%08x! The valid flash addres is [0x%08x - 0x%08x).\n",
                            quadspi_info_ptr->FLASH_CUR_ADDR,
                            quadspi_info_ptr->QuadSPI_MEM_ADDR, quadspi_info_ptr->FLASH_TOP_ADDR[3]);
        return QuadSPI_INVAID_PARAMETER;
    }

    if (txbuf != NULL) {
        /* write operation */
        retlen = _quadspi_write(quadspi_info_ptr, txbuf, flash_src_ptr, len);
    } else if (rxbuf != NULL) {
        retlen = _quadspi_read(quadspi_info_ptr, flash_src_ptr, rxbuf, len);
    }

    return retlen;
}


/*FUNCTION****************************************************************
*
* Function Name    : _qspi_ioctl
* Returned Value   : MQX error code
* Comments         :
*    This function performs miscellaneous services for
*    the QSPI I/O device.
*
*END*********************************************************************/
static _mqx_int _quadspi_ioctl
    (
        /* [IN] The address of the device specific information */
        void                          *io_info_ptr,

        /* [IN] The command to perform */
        uint32_t cmd,

        /* [IN] Parameters for the command */
        uint32_t *param_ptr
    )
{
    QuadSPI_INFO_STRUCT_PTR quadspi_info_ptr = (QuadSPI_INFO_STRUCT_PTR)io_info_ptr;
    _mqx_int retval = QSPI_OK;

    switch (cmd) {
        case QuadSPI_IOCTL_SET_SDR:
            {
                quadspi_info_ptr->CLK_MODE = QuadSPI_CLK_SDR_MODE;
                _quadspi_set_ahb_read(quadspi_info_ptr->QuadSPI_REG_PTR,
                                      quadspi_info_ptr->CLK_MODE,
                                      quadspi_info_ptr->IO_MODE,
                                      quadspi_info_ptr->PARALLEL_ENABLED);
                break;
            }
        case QuadSPI_IOCTL_SET_DDR:
            {
                quadspi_info_ptr->CLK_MODE = QuadSPI_CLK_DDR_MODE;
                _quadspi_set_ahb_read(quadspi_info_ptr->QuadSPI_REG_PTR,
                                      quadspi_info_ptr->CLK_MODE,
                                      quadspi_info_ptr->IO_MODE,
                                      quadspi_info_ptr->PARALLEL_ENABLED);
                break;
            }
        case QuadSPI_IOCTL_SET_SINGLE_IO:
            {
                quadspi_info_ptr->IO_MODE = QuadSPI_SINGLE_MODE;
                _quadspi_set_ahb_read(quadspi_info_ptr->QuadSPI_REG_PTR,
                                      quadspi_info_ptr->CLK_MODE,
                                      quadspi_info_ptr->IO_MODE,
                                      quadspi_info_ptr->PARALLEL_ENABLED);
                break;
            }
        case QuadSPI_IOCTL_SET_DUAL_IO:
            {
                quadspi_info_ptr->IO_MODE = QuadSPI_DUAL_MODE;
                _quadspi_set_ahb_read(quadspi_info_ptr->QuadSPI_REG_PTR,
                                      quadspi_info_ptr->CLK_MODE,
                                      quadspi_info_ptr->IO_MODE,
                                      quadspi_info_ptr->PARALLEL_ENABLED);
                break;
            }
        case QuadSPI_IOCTL_SET_QUAD_IO:
            {
                quadspi_info_ptr->IO_MODE = QuadSPI_QUAD_MODE;
                _quadspi_set_ahb_read(quadspi_info_ptr->QuadSPI_REG_PTR,
                                      quadspi_info_ptr->CLK_MODE,
                                      quadspi_info_ptr->IO_MODE,
                                      quadspi_info_ptr->PARALLEL_ENABLED);
                break;
            }
        case QuadSPI_IOCTL_SET_READ_SPEED:
            {
                uint32_t clk, actual_clk;

                if (!param_ptr)
                    return QuadSPI_INVAID_PARAMETER;

                clk = *((uint32_t *) param_ptr);

                actual_clk = _quadspi_flash_set_serial_clk(quadspi_info_ptr->QuadSPI_REG_PTR,
                                                           quadspi_info_ptr->MODULE_ID, clk);
                if (actual_clk == 0) {
                    printf("ERROR: failed to change QuadSPI read clock\n");
                    return QuadSPI_INVAID_PARAMETER;
                }

                quadspi_info_ptr->READ_CLK = actual_clk;

                break;
            }
        case QuadSPI_IOCTL_SET_WRITE_SPEED:
            {
                uint32_t clk, actual_clk;

                if (!param_ptr)
                    return QuadSPI_INVAID_PARAMETER;

                clk = *((uint32_t *) param_ptr);

                actual_clk = _quadspi_flash_set_serial_clk(quadspi_info_ptr->QuadSPI_REG_PTR,
                                                           quadspi_info_ptr->MODULE_ID, clk);
                if (actual_clk == 0) {
                    printf("ERROR: failed to change QuadSPI read clock\n");
                    return QuadSPI_INVAID_PARAMETER;
                }

                quadspi_info_ptr->WRITE_CLK = actual_clk;
                break;
            }
        case QuadSPI_IOCTL_SET_FLASH_ADDR:
            {
                uint32_t flash_addr;
                if (!param_ptr)
                    return QuadSPI_INVAID_PARAMETER;

                flash_addr = *((uint32_t *) param_ptr);

                if (flash_addr < quadspi_info_ptr->QuadSPI_MEM_ADDR
                        || flash_addr >= quadspi_info_ptr->FLASH_TOP_ADDR[3] ) {
                    printf("\nERROR: invalid flash address 0x%08x! The valid flash addres is [0x%08x - 0x%08x).\n",
                                    flash_addr,
                                    quadspi_info_ptr->QuadSPI_MEM_ADDR,
                                    quadspi_info_ptr->FLASH_TOP_ADDR[3]);
                    retval = QuadSPI_INVAID_PARAMETER;
                } else {
                    quadspi_info_ptr->FLASH_CUR_ADDR = flash_addr;
                }

                break;
            }
        case QuadSPI_IOCTL_GET_MEM_BASE_ADDR:
            {
                if (!param_ptr)
                    return QuadSPI_INVAID_PARAMETER;
                *((uint32_t *) param_ptr) = quadspi_info_ptr->QuadSPI_MEM_ADDR;
                break;
            }
        case QuadSPI_IOCTL_GET_MEM_TOTAL_LEN:
            {
                if (!param_ptr)
                    return QuadSPI_INVAID_PARAMETER;
                *((uint32_t *) param_ptr) = quadspi_info_ptr->FLASH_TOP_ADDR[3] - quadspi_info_ptr->QuadSPI_MEM_ADDR;
                break;
            }
        case QuadSPI_IOCTL_GET_IO_MODE:
            {
                if (!param_ptr)
                    return QuadSPI_INVAID_PARAMETER;
                *((uint32_t *) param_ptr) = quadspi_info_ptr->IO_MODE;
                break;
                }
        case QuadSPI_IOCTL_GET_CLK_MODE:
            {
                if (!param_ptr)
                    return QuadSPI_INVAID_PARAMETER;
                *((uint32_t *) param_ptr) = quadspi_info_ptr->CLK_MODE;
                break;
            }
        case QuadSPI_IOCTL_GET_FLASH_INFO:
            {
                if (!param_ptr)
                    return QuadSPI_INVAID_PARAMETER;
                *((uint32_t *) param_ptr) = (uint32_t)quadspi_info_ptr->HW_BLOCK;
                break;
            }
        case QuadSPI_IOCTL_INVALID_BUFFER:
            {
                QuadSPI_MEM_BLOCK_STRUCT *mem_block;
                if (!param_ptr)
                    return QuadSPI_INVAID_PARAMETER;

                mem_block = (QuadSPI_MEM_BLOCK_STRUCT *) param_ptr;
                _quadspi_invalid_buffer(quadspi_info_ptr->QuadSPI_REG_PTR, mem_block->ADDR, mem_block->SIZE);
                break;
            }
        case QuadSPI_IOCTL_GET_PARA_MODE:
            {
                if (!param_ptr)
                    return QuadSPI_INVAID_PARAMETER;
                *((bool *)param_ptr) = quadspi_info_ptr->PARALLEL_ENABLED;
                break;
            }
        case QuadSPI_IOCTL_GET_IP_PARA_MODE:
            {
                if (!param_ptr)
                    return QuadSPI_INVAID_PARAMETER;
                *((bool *)param_ptr) = quadspi_info_ptr->IP_READ_IN_PARALLEL;
                break;
            }
        case QuadSPI_IOCTL_SET_IP_PARA_MODE:
            {
                if (!param_ptr)
                    return QuadSPI_INVAID_PARAMETER;
                if (!quadspi_info_ptr->PARALLEL_ENABLED)
                    return QuadSPI_ERROR_NOT_SUPPORT;
                quadspi_info_ptr->IP_READ_IN_PARALLEL = *((bool *)param_ptr);
                break;
            }
        default:
            {
                retval = IO_ERROR_INVALID_IOCTL_CMD;
                break;
            }
    }

    return retval;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : quadspi_flash_set_serial_clk
* Returned Value   : The actual serial clock the function set. =0 if failed.
* Comments         :
*   choose the best clock source and set the serial clocks for qspi flash
*
*END*-----------------------------------------------------------------------*/
uint32_t _quadspi_flash_set_serial_clk
    (
        /* [IN] Pointer to QuadSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_regs_ptr,

        /* QuadSPI module id*/
        uint32_t quadspi_id,

        /* Clock rate */
        uint32_t serial_clk
)

{
    uint32_t src_clock = 0xFF, tmp_src_freq;
    uint32_t quadspi_div = 0;
    uint32_t min_remainder, tmp_remainder, tmp_div, i;
    uint32_t actual_serial_clk = 0;

#if BSPCFG_ENABLE_CLKTREE_MGMT
    void* qspi_clk_parent;
    void* qspi_clock;

    // printf("QSPI set clock %d\n", serial_clk);
    printf("Search for best parent...");
    min_remainder = serial_clk;
    for (i = 0; i < 5; i++) {
        /* div_x1 = 2, div_x2 = 2
          * quadspi_div(div_x4) = source_clock / div_x2 / div_x1 = source_clock / 4;
          */
        qspi_clk_parent = clock_get(quadspi_src_clk_name[i]);
        // printf("\t--- clk src [%s], freq %d\n", clock_get_name(qspi_clk_parent), clock_get_freq(qspi_clk_parent));
        if (qspi_clk_parent == NULL) {
            printf("ERROR: invalid parent clock name of qspi%d\n", quadspi_id);
            return QuadSPI_INVAID_PARAMETER;
        }

        tmp_src_freq = clock_get_freq(qspi_clk_parent) / 4;
        if (tmp_src_freq == 0)
            continue;

        /* find the most close clock which is smaller than expected clock */
        tmp_div = (tmp_src_freq + serial_clk - 1) / serial_clk;
        if (tmp_div > 4)
            tmp_div = 4;
        tmp_remainder = serial_clk - tmp_src_freq / tmp_div;

        if (tmp_remainder < min_remainder) {
            min_remainder = tmp_remainder;
            src_clock = i;
            quadspi_div = tmp_div;
            actual_serial_clk = tmp_src_freq / quadspi_div;
            if (min_remainder == 0)
                break;
        }
    }

    /* Select the best clock source*/
    qspi_clk_parent = clock_get(quadspi_src_clk_name[src_clock]);

    /* couldn't find appropriate clock  */
    if (min_remainder == serial_clk) {
        printf("ERROR: the read or write frequency is not supported by QuadSPI driver\n");
        return QuadSPI_INVAID_PARAMETER;
    }

    qspi_clock = clock_get(quadspi_clk_name[quadspi_id]);

    if (qspi_clock == NULL) {
        printf("ERROR: invalid clock name of qspi%d!\n", quadspi_id);
        return QuadSPI_INVAID_PARAMETER;
    }
    printf("select %s, freqency %d, qspi_div %d, QSPI actural frequency is %d\n", clock_get_name(qspi_clk_parent), clock_get_freq(qspi_clk_parent), 4 * quadspi_div, serial_clk - min_remainder);

    /* set qspi clock parent */
    clock_set_parent(qspi_clock, qspi_clk_parent);

    /* set qspi clock frequency */
    clock_set_freq(qspi_clock, 3, 1, 1, quadspi_div-1);

    return actual_serial_clk;
#else
    min_remainder = serial_clk;
    for (i = 0; i < 4; i++) {
        /* div_x1 = 2, div_x2 = 2
          * quadspi_div(div_x4) = source_clock / div_x2 / div_x1 = source_clock / 4;
          */
        tmp_src_freq = _bsp_get_clock(_bsp_get_clock_configuration(), quadspi_src_clocks[i]) / 4;
        if (tmp_src_freq == 0)
            continue;
        /* find the most close clock which is smaller than expected clock */
        tmp_div = (tmp_src_freq + serial_clk - 1) / serial_clk;
        if (serial_clk >= tmp_src_freq / tmp_div)
            tmp_remainder = serial_clk - tmp_src_freq / tmp_div;
        else
            tmp_remainder = serial_clk; /* clock is bigger than expected serial clock*/

        if (tmp_remainder < min_remainder && (tmp_div > 0 && tmp_div <= 4)) {
            min_remainder = tmp_remainder;
            src_clock = i;
            quadspi_div = tmp_div;
            actual_serial_clk = tmp_src_freq / quadspi_div;
            if (min_remainder == 0)
                break;
        }
    }

    /* couldn't find appropriate clock  */
    if (min_remainder == serial_clk) {
        printf("ERROR: the read or write frequency is not supported by QuadSPI driver\n");
        return 0;
    }

    if (quadspi_src_clocks[src_clock] == CM_CLOCK_SOURCE_PLL1_PFD4)
        CCM_CCSR |= CCM_CCSR_PLL1_PFD4_EN(0x1);      // Enable PLL1 PFD4
    else if (quadspi_src_clocks[src_clock] == CM_CLOCK_SOURCE_PLL2_PFD4)
        CCM_CCSR |= CCM_CCSR_PLL2_PFD4_EN(0x1);      // Enable PLL2 PFD4
    else if (quadspi_src_clocks[src_clock] == CM_CLOCK_SOURCE_PLL3_PFD4)
        CCM_CCSR |= CCM_CCSR_PLL3_PFD4_EN(0x1);      // Enable PLL3 PFD4
    else if (quadspi_src_clocks[src_clock] != CM_CLOCK_SOURCE_PLL_USB0){
        printf("ERROR: invalid clock srouce for QuadSPI!\n");
        return 0;
    }

    if (quadspi_id == 0) {
        /* QuadSPI0 ungate */
        CCM_CCGR2 |= CCM_CCGR2_CG4(0x3);
        CCM_CSCMR1 &= ~CCM_CSCMR1_QSPI0_CLK_SEL_MASK;
        CCM_CSCMR1 |= CCM_CSCMR1_QSPI0_CLK_SEL(src_clock);

        CCM_CSCDR3 &= ~0x1F;
        CCM_CSCDR3 |= CCM_CSCDR3_QSPI0_X4_DIV(quadspi_div - 1) |
                      CCM_CSCDR3_QSPI0_X2_DIV(0x1) |
                      CCM_CSCDR3_QSPI0_DIV(0x1) |
                      CCM_CSCDR3_QSPI0_EN(0x1);
    } else {
        /* QuadSPI1 ungate */
        CCM_CCGR8 |= CCM_CCGR8_CG4(0x3);
        CCM_CSCMR1 &= ~CCM_CSCMR1_QSPI1_CLK_SEL_MASK;
        CCM_CSCMR1 |= CCM_CSCMR1_QSPI1_CLK_SEL(src_clock);

        CCM_CSCDR3 &= ~0x1F00;
        CCM_CSCDR3 |= CCM_CSCDR3_QSPI1_X4_DIV(quadspi_div - 1) |
                      CCM_CSCDR3_QSPI1_X2_DIV(0x1) |
                      CCM_CSCDR3_QSPI1_DIV(0x1) |
                      CCM_CSCDR3_QSPI1_EN(0x1);
    }

    return actual_serial_clk;
#endif
}


#if BSPCFG_ENABLE_CLKTREE_MGMT
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : quadspi_flash_set_serial_clk
* Returned Value   : MQX error code
* Comments         :
*   choose the best clock source and set the serial clocks for qspi flash
*
*END*-----------------------------------------------------------------------*/
_mqx_int _quadspi_flash_enable_serial_clk
    (
        /* QuadSPI module id*/
        uint32_t quadspi_id
)
{
    void* qspi_clock;

    qspi_clock = clock_get(quadspi_clk_name[quadspi_id]);
    if (qspi_clock == NULL) {
        printf("ERROR: invalid clock name of qspi%d!\n", quadspi_id);
        return QuadSPI_INVAID_PARAMETER;
    }
    clock_enable(qspi_clock);

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : quadspi_flash_set_serial_clk
* Returned Value   : MQX error code
* Comments         :
*   choose the best clock source and set the serial clocks for qspi flash
*
*END*-----------------------------------------------------------------------*/
_mqx_int _quadspi_flash_disable_serial_clk
    (
        /* QuadSPI module id*/
        uint32_t quadspi_id
)
{
    void* qspi_clock;

    qspi_clock = clock_get(quadspi_clk_name[quadspi_id]);
    if (qspi_clock == NULL) {
        printf("ERROR: invalid clock name of qspi%d!\n", quadspi_id);
        return QuadSPI_INVAID_PARAMETER;
    }
    clock_disable(qspi_clock);

    return MQX_OK;
}
#endif

void _quadspi_unlockLookupTable
    (
        /* [IN] Pointer to QSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr
    )
{
    quadspi_reg_ptr->LUTKEY = 0x5AF05AF0;
    quadspi_reg_ptr->LCKCR = 0x2;

    /* wait and check if the unlock mask is for the */
    while(!(quadspi_reg_ptr->LCKCR & QuadSPI_LCKCR_UNLOCK_MASK));
}

void _quadspi_lockLookupTable
    (
        /* [IN] Pointer to QSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr
    )
{
    quadspi_reg_ptr->LUTKEY = 0x5AF05AF0;
    quadspi_reg_ptr->LCKCR = 0x1;

    /* wait and check if the unlock mask is for the */
    while((quadspi_reg_ptr->LCKCR & QuadSPI_LCKCR_LOCK_MASK)==0);
}

static _mqx_int _quadspi_setupLookupTable
    (
        /* [IN] Pointer to QSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] The initialization information for the device being opened */
        QuadSPI_INIT_STRUCT_PTR quadspi_init_ptr
    )
{
    QuadSPI_FLASH_CMD_STRUCT *cmd = quadspi_init_ptr->FLASH_CMD;
    uint32_t i, temp, lut_seq;
    _quadspi_unlockLookupTable(quadspi_reg_ptr);

    /* SEQ0: SDR I/O read */
    lut_seq = QUADSPI_READ * 4;
    for (i = 0; i < QuadSPI_CMD_SEQ_LEN; i += 2) {
        temp = cmd->READ_CMD[i] | cmd->READ_CMD[i+1] << 16;
        quadspi_reg_ptr->LUT[lut_seq++] = temp;
    }

    /* SEQ1: SDR dual I/O read */
    lut_seq = QUADSPI_DUAL_READ * 4;
    for (i = 0; i < QuadSPI_CMD_SEQ_LEN; i += 2) {
        temp = cmd->DUAL_READ_CMD[i] | cmd->DUAL_READ_CMD[i+1] << 16;
        quadspi_reg_ptr->LUT[lut_seq++] = temp;
    }

    /* SEQ2: SDR quad I/O read */
    lut_seq = QUADSPI_QUAD_READ * 4;
    for (i = 0; i < QuadSPI_CMD_SEQ_LEN; i += 2) {
        temp = cmd->QUAD_READ_CMD[i] | cmd->QUAD_READ_CMD[i+1] << 16;
        quadspi_reg_ptr->LUT[lut_seq++] = temp;
    }

    /* SEQ3: DDR I/O read */
    lut_seq = QUADSPI_DDR_READ * 4;
    for (i = 0; i < QuadSPI_CMD_SEQ_LEN; i += 2) {
        temp = cmd->DDR_READ_CMD[i] | cmd->DDR_READ_CMD[i+1] << 16;
        quadspi_reg_ptr->LUT[lut_seq++] = temp;
    }

    /* SEQ4: DDR dual I/O read */
    lut_seq = QUADSPI_DDR_DUAL_READ * 4;
    for (i = 0; i < QuadSPI_CMD_SEQ_LEN; i += 2) {
        temp = cmd->DDR_DUAL_READ_CMD[i] | cmd->DDR_DUAL_READ_CMD[i+1] << 16;
        quadspi_reg_ptr->LUT[lut_seq++] = temp;
    }

    /* SEQ5: DDR quad I/O read */
    lut_seq = QUADSPI_DDR_QUAD_READ * 4;
    for (i = 0; i < QuadSPI_CMD_SEQ_LEN; i += 2) {
        temp = cmd->DDR_QUAD_READ_CMD[i] | cmd->DDR_QUAD_READ_CMD[i+1] << 16;
        quadspi_reg_ptr->LUT[lut_seq++] = temp;
    }

    _quadspi_lockLookupTable(quadspi_reg_ptr);

    return QSPI_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _quadspi_init_setup
* Returned Value   : MQX error code
* Comments         :
*    This function initializes the QuadSPI controller
*
*END*********************************************************************/
static _mqx_int _quadspi_init_setup
    (
        /* [IN] Pointer to QuadSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] The initialization information for the device being opened */
        QuadSPI_INIT_STRUCT_PTR quadspi_init_ptr,

        /* [IN] A1 top address */
        uint32_t flashA1_top_addr,

        /* [IN] A2 top address */
        uint32_t flashA2_top_addr,

        /* [IN] B1 top address */
        uint32_t flashB1_top_addr,

        /* [IN] B2 top address */
        uint32_t flashB2_top_addr
    )
{
    _mqx_int retval;

    /* set mdis bit  */
    quadspi_reg_ptr->MCR = (0xF0000 | QuadSPI_MCR_MDIS_MASK);
    /* clear mdis bit  */
    quadspi_reg_ptr->SMPR &= ~(QuadSPI_SMPR_FSDLY_MASK |
                        QuadSPI_SMPR_FSPHS_MASK | QuadSPI_SMPR_HSENA_MASK);
    // for 33MHz clock
    /* Enable QuadSPI clocks. */
    quadspi_reg_ptr->MCR = 0xF0000;

    /* setup flash mapped memory address */
    quadspi_reg_ptr->SFA1AD = flashA1_top_addr; // top address of FA1
    quadspi_reg_ptr->SFA2AD = flashA2_top_addr; // top address of FA2
    quadspi_reg_ptr->SFB1AD = flashB1_top_addr; // top address of FB1
    quadspi_reg_ptr->SFB2AD = flashB2_top_addr; // top address of FB2

    /* Setup Lookup table for specific flash device */
    retval = _quadspi_setupLookupTable(quadspi_reg_ptr, quadspi_init_ptr);
    if (retval < 0) {
        return retval;
    }

    // set quad ddr as xbar read instruction
    quadspi_reg_ptr->BFGENCR = 0x0000;

    /* Clear top index of buffer0/1/2
      * Vybrid A5 AHB master id is 2, M4 AHB master id is 0.
      * set dummy masterid (0xe) for buffer 0~2, so buffer 0/1/2 is not used */
    quadspi_reg_ptr->BUF0CR = QuadSPI_BUF0CR_MSTRID(0xe);   /* dummy id */
    quadspi_reg_ptr->BUF1CR = QuadSPI_BUF1CR_MSTRID(0xe);   /* dummy id */
    quadspi_reg_ptr->BUF2CR = QuadSPI_BUF2CR_MSTRID(0xe);   /* dummy id */

    /* Use buffer 3 for both A5 and M4, so enable buffer 3 all master bit. */
    quadspi_reg_ptr->BUF3CR |= QuadSPI_BUF3CR_ALLMST_MASK;    /* enable all master */
    quadspi_reg_ptr->BUF3CR |= QuadSPI_BUF3CR_ADATSZ(128);  /* AHB prefetch size: 128*8byte */

    /* Set top index of buffer0/1/2
      * Buffer 0/1/2 are 0 bytes.
      * Buffer 3 is 1024byte (128*8byte).
      */
    quadspi_reg_ptr->BUF0IND = QuadSPI_BUF0IND_TPINDX0(0x0);
    quadspi_reg_ptr->BUF1IND = QuadSPI_BUF1IND_TPINDX1(0x0);
    quadspi_reg_ptr->BUF2IND = QuadSPI_BUF2IND_TPINDX2(0x0);

    /* clear Rx and Tx FIFO */
    quadspi_reg_ptr->MCR |= QuadSPI_MCR_CLR_TXF_MASK;
    quadspi_reg_ptr->MCR |= QuadSPI_MCR_CLR_RXF_MASK;

    /* setup AHB read */
    _quadspi_set_ahb_read(quadspi_reg_ptr, quadspi_init_ptr->CLK_MODE,
                          quadspi_init_ptr->IO_MODE, quadspi_init_ptr->PARALLEL_ENABLED);

    return QSPI_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : quadspi_set_ahb_read_function
* Returned Value   : NULL
* Comments         :
*   The ram based execution version for ahb reading configure.
*
*END*-----------------------------------------------------------------------*/
static void _quadspi_set_ahb_read
    (
        /* [IN] Pointer to QuadSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] QuadSPI clock mode: DDR/SDR */
        QuadSPI_CLK_MODE clk_mode,

        /*[IN] QuadSPI IO mode: single/dual/quad */
        QuadSPI_IO_MODE read_io_mode,

        /*[IN] QuadSPI Parallel mode */
        bool enable_parallel_mode
    )
{
    uint32_t reg_val = 0, smpr_val;
    uint32_t seq_id;

    /*wait util current operation end*/
    while (quadspi_reg_ptr->SR & (QuadSPI_SR_BUSY_MASK | QuadSPI_SR_IP_ACC_MASK
        | QuadSPI_SR_AHB_ACC_MASK | QuadSPI_SR_AHBTRN_MASK));

    /* Disable QuadSPI clock before configure MCR */
    quadspi_reg_ptr->MCR |= (QuadSPI_MCR_MDIS_MASK);

    /*configure the ISD3FB ISD2FB ISD3FA ISD2FA*/
    if (clk_mode == QuadSPI_CLK_SDR_MODE) {
        /* Clear DDR sampling point for SDR mode */
        smpr_val = quadspi_reg_ptr->SMPR;
        smpr_val &= ~QuadSPI_SMPR_DDRSMP_MASK;
        quadspi_reg_ptr->SMPR = smpr_val;

        /* Disable 2x and 4x clock for SDR mode */
        quadspi_reg_ptr->MCR &= ~QuadSPI_MCR_DDR_EN_MASK;

        /* Set SDR read prefetch command sequence in LUT */
        seq_id = (uint32_t) QUADSPI_READ + (uint32_t) read_io_mode;
    } else {
        /* Set DDR sampling point for DDR mode */
        smpr_val = quadspi_reg_ptr->SMPR;
        smpr_val &= ~QuadSPI_SMPR_DDRSMP_MASK;
        smpr_val |= (1 << QuadSPI_SMPR_DDRSMP_SHIFT);
        quadspi_reg_ptr->SMPR = smpr_val;

        /* Enable 2x and 4x clock for SDR mode */
        quadspi_reg_ptr->MCR |= QuadSPI_MCR_DDR_EN_MASK;;

        /* Set DDR read prefetch command sequence in LUT */
        seq_id = (uint32_t) QUADSPI_DDR_READ + (uint32_t) read_io_mode;
    }

    /* Enable QuadSPI clock */
    quadspi_reg_ptr->MCR &= ~(QuadSPI_MCR_MDIS_MASK);

    /* Set AHB read prefetch command */
    reg_val = quadspi_reg_ptr->BFGENCR;
    reg_val &= ~QuadSPI_BFGENCR_SEQID_MASK;
    reg_val |= (seq_id << QuadSPI_BFGENCR_SEQID_SHIFT);

    if (enable_parallel_mode)
        reg_val |= QuadSPI_BFGENCR_PAR_EN_MASK;
    else
        reg_val &= ~QuadSPI_BFGENCR_PAR_EN_MASK;

    quadspi_reg_ptr->BFGENCR = reg_val;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_reset_buffer
* Returned Value   : NULL
* Comments         :
*   Invalidate the data in AHB buffer by reset the AHB domain.
*   QuadSPI controller recommend to reset AHB buffer and IP buffer together.
*
*END*-----------------------------------------------------------------------*/
void _quadspi_reset_buffer
    (
        /* [IN] Pointer to QuadSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr
    )
{
    while (quadspi_reg_ptr->SR & (QuadSPI_SR_BUSY_MASK | QuadSPI_SR_IP_ACC_MASK
        | QuadSPI_SR_AHB_ACC_MASK | QuadSPI_SR_AHBTRN_MASK));

    /* 1 invalid the data in AHB buffer */
    /* reset AHB and IP domain */
    quadspi_reg_ptr->MCR |= (QuadSPI_MCR_SWRSTHD_MASK | QuadSPI_MCR_SWRSTSD_MASK);

    /* wait for reset ready: 1AHB+2SFCK clocks */
    _time_delay(1);

    /* clear reset AHB domain bit after reset */
    quadspi_reg_ptr->MCR &= ~(QuadSPI_MCR_SWRSTHD_MASK | QuadSPI_MCR_SWRSTSD_MASK);
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_invalid_buffer
* Returned Value   : NULL
* Comments         :
*   Invalidate the data in AHB buffer and system data cache.
*   When data in external flash is changed, invalidate both AHB buffer and system data cache.
*
*END*-----------------------------------------------------------------------*/
void _quadspi_invalid_buffer
    (
        /* [IN] Pointer to QuadSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] Base address of QuadSPI memory which needs to be invalid in system data cache*/
        void *addr,

        /* [IN] Length of QuadSPI memory which needs to be invalid in system data cache*/
        _mem_size size
    )
{
    /* Invalid data in AHB or IP buffer. Erase operation might change the data in AHB buffer.
     * Invalidate it to make sure the correction of following read operation */
    _quadspi_reset_buffer(quadspi_reg_ptr);

    /* Data in AHB buffer is invalid, we should also invalid data in system data cache.
      * Other data might be in the same cache line with QuadSPI data. So before invalid data cache, flush these data.
      */
    _DCACHE_FLUSH_MLINES(addr, size);
    _DCACHE_INVALIDATE_MLINES(addr, size);
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : quadspi_read_IP
* Returned Value   : number of bytes read. >=0 if successful, <0 if failed.
* Comments         :
*   Read data from flash device to user buffer through QuadSPI IP bus.
*
*END*-----------------------------------------------------------------------*/
static int32_t _quadspi_read_IP
    (
        /* [IN] Pointer to QuadSPI port information */
        QuadSPI_INFO_STRUCT_PTR quadspi_info_ptr,

        /* [IN] Flash address to be read from */
        uint8_t *src_ptr,

        /* [OUT] Rx Buffer address */
        uint8_t *dest_ptr,

        /* [IN] Number of bytes to write */
        _mem_size size

    )
{
    VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr = quadspi_info_ptr->QuadSPI_REG_PTR;
    uint32_t temp_val, read_size, i, j;
    uint32_t timeout = QUADSPI_REGS_POLLING_MAX;
    uint32_t reg_val;
    uint32_t src_addr;
    bool trim = FALSE;

    if (size > QuadSPI_READ_BUF_SIZE) {
        printf("ERROR: quadspi IP read can maximum %d bytes at one time\n", QuadSPI_READ_BUF_SIZE);
        return -1;
    }

    reg_val = quadspi_reg_ptr->RBSR;
    while (!(reg_val & QuadSPI_RBSR_RDBFL_MASK)) {
        if (!timeout--) {
            printf("Error: wait for RX buffer data is filled\n!");
            return -1;
        }
    }

    read_size = size;
    src_addr = quadspi_reg_ptr->SFAR;

    if (quadspi_info_ptr->IP_READ_IN_PARALLEL && (src_addr & 0x1))
    {
        read_size = size + 1;
        trim = TRUE;
    }

    i = 0;
    while (read_size > 0) {
        temp_val = quadspi_reg_ptr->RBDR[i];

        j = 4;
        while (j > 0 && read_size > 0) {
            if (!trim || (read_size != 2))
            {
                *(dest_ptr) = (char)((temp_val >> ((j-1)<<3)) & 0xff);
                dest_ptr++;
            }
            read_size--;
            j--;
        }
        i++;
    }

    /*clear RX buffer*/
    quadspi_reg_ptr->MCR |= QuadSPI_MCR_CLR_RXF_MASK;

    /* RX Buffer is reading using AHB Bus Registers by default */
    quadspi_reg_ptr->RBCT &= ~QuadSPI_RBCT_RXBRD_MASK;

    return size;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : quadspi_read_AHB
* Returned Value   : number of bytes read. >=0 if successful, <0 if failed.
* Comments         :
*   Read data from flash device to user buffer through AHB bus.
*   This is a memory mapped address. Read directly.
*
*END*-----------------------------------------------------------------------*/
static int32_t _quadspi_read_AHB
    (
        /* [IN] Pointer to QuadSPI port information */
        QuadSPI_INFO_STRUCT_PTR quadspi_info_ptr,

        /* [IN] Flash address to be read from */
        uint8_t *src_ptr,

        /* [OUT] Rx Buffer address */
        uint8_t *dest_ptr,

        /* [IN] Number of bytes to write */
        _mem_size size

    )
{
    VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr = quadspi_info_ptr->QuadSPI_REG_PTR;

    /* Wait while if the QuadSPI is busy read through IP or AHB command */
    while (quadspi_reg_ptr->SR & (QuadSPI_SR_BUSY_MASK | QuadSPI_SR_IP_ACC_MASK
        | QuadSPI_SR_AHB_ACC_MASK | QuadSPI_SR_AHBTRN_MASK));

    /* RX Buffer is reading using AHB Bus Registers */
    quadspi_reg_ptr->RBCT &= ~QuadSPI_RBCT_RXBRD_MASK;

    _mem_copy(src_ptr, dest_ptr, size);

    return size;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : quadspi_read
* Returned Value   : number of bytes read. >=0 if successful, <0 if failed.
* Comments         :
*   Read data from flash device to user buffer.
*
*END*-----------------------------------------------------------------------*/
static int32_t _quadspi_read
    (
        /* [IN] Pointer to QuadSPI port information */
        QuadSPI_INFO_STRUCT_PTR quadspi_info_ptr,

        /* [IN] Flash address to be read from */
        uint8_t *src_ptr,

        /* [OUT] Rx Buffer address */
        uint8_t *dest_ptr,

        /* [IN] Number of bytes to write */
        _mem_size size

    )
{
    uint32_t ret_size;

     /* 1 if read status registers from QuadSPI flash, read from IP Bus
      * 2 other cases is read data from QuadSPI flash, read from AXI Bus is preferred, which is much faster.
      */

    if (quadspi_info_ptr->FLASH_READ_FLAG == QuadSPI_FLASH_REG_READ) {
        ret_size = _quadspi_read_IP(quadspi_info_ptr, src_ptr, dest_ptr, size);
    } else
        ret_size = _quadspi_read_AHB(quadspi_info_ptr, src_ptr, dest_ptr, size);

    /* clear flag after read operation: default status normal read flash content */
    quadspi_info_ptr->FLASH_READ_FLAG = QuadSPI_FLASH_READ;

    return ret_size;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_ram_program
* Returned Value   : number of bytes programmed. >=0 if successful, <0 if failed.
* Comments:
*       This function writes data buffer to external memory by send program command.
*       In order to avoid TX buffer underrun which might happened, the maximum program
* size is 64bytes.
*
*END*-----------------------------------------------------------------------*/
static int32_t _quadspi_ram_program
    (
        /* [IN] Pointer to QuadSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] Source address */
        uint8_t *src_ptr,

        /* [IN] Destination address */
        uint8_t *dest_ptr,

        /* [IN] Number of bytes to write */
        _mem_size size
    )
{
    int32_t j, i;
    uint32_t temp;

    if (size > QuadSPI_WR_BUF_SIZE) {
        printf("ERROR: QuadSPI don't accept page size greater than %d\n", QuadSPI_WR_BUF_SIZE);
        return -1;
    }

    /* clear TX fifo */
    quadspi_reg_ptr->MCR |= QuadSPI_MCR_CLR_TXF_MASK;

    /* designate the CS for flash A*/
    quadspi_reg_ptr->SFAR = (uint32_t) dest_ptr;

    /* tx fifo depth is 64bytes, fill tx fifo first before send write command */
    for (j = 0; j < size; j += 4) {
        temp = 0;
        for (i = 0; i < 4; i++)
        {
            temp |= *(src_ptr) << ((3-i) * 8);
            src_ptr++;
        }
        quadspi_reg_ptr->TBDR = temp;
    }

    /* write command */
    quadspi_reg_ptr->IPCR = (size + (QuadSPI_PROM_LUT<<QuadSPI_IPCR_SEQID_SHIFT));
    while (quadspi_reg_ptr->SR & QuadSPI_SR_BUSY_MASK);

    return size;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_write
* Returned Value   : number of bytes write. >=0 if successful, <0 if failed.
* Comments         :
*   Build command and send it external flash memory.
*
*END*-----------------------------------------------------------------------*/
static int32_t _quadspi_write
    (
        /* [IN] Pointer to QuadSPI port information */
        QuadSPI_INFO_STRUCT_PTR quadspi_info_ptr,

        /* [IN] Source address */
        uint8_t *src_ptr,

        /* [IN] Destination address */
        uint8_t *dest_ptr,

        /* [IN] Number of bytes to write */
        _mem_size size
    )
{
    VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr = quadspi_info_ptr->QuadSPI_REG_PTR;
    uint16_t quadspi_cmd[QuadSPI_MAX_CMD_SET] = {0};
    uint32_t src_addr;
    uint8_t cmd_opr = 0xFF;
    int cmd_num, rw_size, i;

    for (cmd_num = 0; cmd_num < QuadSPI_MAX_CMD_SET; cmd_num++) {
        quadspi_cmd[cmd_num] = (src_ptr[2*cmd_num+1] << 8) +  src_ptr[2*cmd_num];
        if (QuadSPI_GET_INST(quadspi_cmd[cmd_num]) == QuadSPI_INST_CMD) {
            if (cmd_opr != 0xFF) {
                printf("ERROR: QuadSPI driver could send on two cmds at the same time\n");
                return -1;
            }
            cmd_opr = QuadSPI_GET_OPR(quadspi_cmd[cmd_num]);
        }

        if (quadspi_cmd[cmd_num] == QuadSPI_LOOKUP_STOP)
            break;
    }

    /* cmd_num count from 0, the actual number need to +1 */
    cmd_num++;

    if (cmd_num > QuadSPI_MAX_CMD_SET || quadspi_cmd[cmd_num - 1] != QuadSPI_LOOKUP_STOP) {
        printf("ERROR: QuadSPI write cmd must be end with QuadSPI_LOOKUP_STOP (0x00)\n");
        return -1;
    }

    /* write cmd format is: operation + address + size */
    if (size < cmd_num * 2 + 4) {
        printf("ERROR: QuadSPI driver don't support the fwrite format!\n");
        return -1;
    }

    _quadspi_unlockLookupTable(quadspi_reg_ptr);
    /* Use SEQ 14 to send write command */
    for (i = 0; i < QuadSPI_MAX_CMD_SET; i += 2) {
        quadspi_reg_ptr->LUT[QuadSPI_PROM_LUT * 4+i/2] = (quadspi_cmd[i+1] << 16) + quadspi_cmd[i];
    }
    _quadspi_lockLookupTable(quadspi_reg_ptr);

    rw_size = size - cmd_num * 2 - 4;
    src_ptr = &src_ptr[cmd_num*2];
    src_addr = 0;
    for (i = 0; i < 4; i++)
            src_addr |= (*src_ptr++) << (i*8);

    /* Wait while if the QuadSPI is busy read through IP or AHB command */
    while (quadspi_reg_ptr->SR & (QuadSPI_SR_BUSY_MASK | QuadSPI_SR_IP_ACC_MASK
        | QuadSPI_SR_AHB_ACC_MASK | QuadSPI_SR_AHBTRN_MASK));

    if (src_addr == (uint32_t) NULL) {
        /* command or read operation */
        if (rw_size > 0) {
            /* For IP read command: Rx Buffer is reading using IP Bus Registers */
            quadspi_reg_ptr->RBCT |= QuadSPI_RBCT_RXBRD_MASK;
        }

        quadspi_reg_ptr->MCR |= QuadSPI_MCR_CLR_RXF_MASK;
        quadspi_reg_ptr->SFAR = (uint32_t)dest_ptr;
        quadspi_reg_ptr->SR &= ~(QuadSPI_SR_IP_ACC_MASK);
        if (quadspi_info_ptr->IP_READ_IN_PARALLEL)
            quadspi_reg_ptr->IPCR = (rw_size + (QuadSPI_PROM_LUT << QuadSPI_IPCR_SEQID_SHIFT)) | QuadSPI_IPCR_PAR_EN_MASK;
        else
            quadspi_reg_ptr->IPCR = (rw_size + (QuadSPI_PROM_LUT << QuadSPI_IPCR_SEQID_SHIFT));
        while (quadspi_reg_ptr->SR & QuadSPI_SR_BUSY_MASK);

        /* This is IP read command */
        if (rw_size > 0)
            quadspi_info_ptr->FLASH_READ_FLAG = QuadSPI_FLASH_REG_READ;
    } else {
        /* write operation */
        if (rw_size > quadspi_info_ptr->PAGE_SIZE ) {
            return -1;
        }

        size = _quadspi_ram_program(quadspi_reg_ptr, (uint8_t*)src_addr, dest_ptr, rw_size);
    }

    return size;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_disable_clock
* Returned Value   : None
* Comments         :
*   Disable the clock
*
*END*-----------------------------------------------------------------------*/
static void _quadspi_disable_clock(uint32_t quadspi_id)
{
    if (quadspi_id == 0)
        CCM_CCGR2 |= CCM_CCGR2_CG4(0);
    else
        CCM_CCGR8 |= CCM_CCGR8_CG4(0);
}

