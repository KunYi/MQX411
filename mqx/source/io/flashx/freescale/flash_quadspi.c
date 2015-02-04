
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   The file contains functions for internal flash read, write, erase
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "flashx.h"
#include "flashxprv.h"
#include "flash_quadspi_prv.h"
#include "flash_quadspi.h"

static _mqx_int _quadspi_init_setup (IO_FLASHX_STRUCT_PTR dev_ptr);
static _mqx_int _quadspi_set_ahb_read (IO_FLASHX_STRUCT_PTR, QuadSPI_CLK_MODE, QuadSPI_IO_MODE);

const FLASHX_DEVICE_IF_STRUCT _flashx_quadspi_if = {
    quadspi_flash_erase_sector,
    quadspi_flash_write_sector,
    quadspi_flash_chip_erase,
    NULL,
    quadspi_flash_init,
    quadspi_flash_deinit,
    NULL,
    quadspi_flash_ioctl
};


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfe_flash_init
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Initialize flash specific information.
*
*END*----------------------------------------------------------------------*/
bool quadspi_flash_init
    (
        /* [IN] File pointer */
        IO_FLASHX_STRUCT_PTR dev_ptr
    )
{
    FLASHX_QUADSPI_INIT_STRUCT_PTR dev_spec_init = dev_ptr->DEVICE_SPECIFIC_INIT;
    FLASHX_QUADSPI_INTERNAL_STRUCT_PTR dev_spec_ptr;
    FLASHX_BLOCK_INFO_STRUCT_PTR b;
    VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr;
    uint32_t i, sector_size;
    uint32_t actual_serial_clk;

    /* Check QSPI init data */
    if (dev_spec_init->CLK_MODE != QuadSPI_CLK_SDR_MODE &&
        dev_spec_init->CLK_MODE != QuadSPI_CLK_DDR_MODE) {
        printf("ERROR: invalid clock mode!\n");
        return FLASHXERR_QUADSPI_INVALID_PARM;
    }

    if (dev_spec_init->IO_MODE != QuadSPI_SINGLE_MODE &&
        dev_spec_init->IO_MODE != QuadSPI_DUAL_MODE &&
        dev_spec_init->IO_MODE != QuadSPI_QUAD_MODE) {
        printf("ERROR: invalid IO mode!\n");
        return FLASHXERR_QUADSPI_INVALID_PARM;
    }

    if (dev_ptr->HW_BLOCK[0].SPECIAL_TAG != FLASHX_QUADSPI_A1_TAG) {
        printf("ERROR: the first flash block must be marked with FLASHX_QUADSPI_A1_TAG!\n");
        return FLASHXERR_QUADSPI_INVALID_PARM;
    }
      /* allocate internal data structure */
    dev_spec_ptr = (FLASHX_QUADSPI_INTERNAL_STRUCT_PTR)_mem_alloc(sizeof(FLASHX_QUADSPI_INTERNAL_STRUCT));
    if (dev_spec_ptr == NULL) {
        return FALSE;
    }

    /* Get the QuadSPI register base address */
    quadspi_reg_ptr = _bsp_get_quadspi_reg_address (dev_spec_init->MODULE_ID);
    if (quadspi_reg_ptr == NULL) {
        _mem_free(dev_spec_ptr);
        return SPI_ERROR_CHANNEL_INVALID;
    }

    /* Set internal information */
    dev_ptr->DEVICE_SPECIFIC_DATA = dev_spec_ptr;
    dev_spec_ptr->QuadSPI_REG_PTR = quadspi_reg_ptr;
    dev_spec_ptr->MODULE_ID = dev_spec_init->MODULE_ID;
    dev_spec_ptr->CLK_MODE = dev_spec_init->CLK_MODE;
    dev_spec_ptr->IO_MODE = dev_spec_init->IO_MODE;
    dev_spec_ptr->SERIAL_CLK= dev_spec_init->SERIAL_CLK;
    dev_spec_ptr->PARALLEL_ENABLED = dev_spec_init->PARALLEL_ENABLED;

    for (i = 0 ; i < 4; i++)
        dev_spec_ptr->FLASH_TOP_ADDR[i] = dev_ptr->BASE_ADDR;

    /* The HW Block only record the valid block */
    for (b = dev_ptr->HW_BLOCK; b->NUM_SECTORS != 0; b++) {
        /* Update top address */
        dev_spec_ptr->FLASH_TOP_ADDR[b->SPECIAL_TAG] += b->START_ADDR +
                         b->SECTOR_SIZE * b->NUM_SECTORS;
    }

    /* If A2/B1/B2 is empty, set the top address the same as previous block */
    for (i = 1; i < 4; i++) {
        if (dev_spec_ptr->FLASH_TOP_ADDR[i] < dev_spec_ptr->FLASH_TOP_ADDR[i-1])
            dev_spec_ptr->FLASH_TOP_ADDR[i] = dev_spec_ptr->FLASH_TOP_ADDR[i-1];
    }

    /* Configure IOMUX for QuadSPI */
    if (_bsp_quadspi_io_init (dev_spec_init->MODULE_ID) == -1) {
        /* Cannot initialize QuadSPI module */
        _mem_free(dev_spec_ptr);
        return FALSE;
    }

    /* Enable and set QuadSPI serial clock output to external flash */
    actual_serial_clk = _quadspi_flash_set_serial_clk(quadspi_reg_ptr, dev_spec_ptr->MODULE_ID, dev_spec_ptr->SERIAL_CLK);
    if (actual_serial_clk == 0) {
        _mem_free(dev_spec_ptr);
        return FALSE;
    }
    dev_spec_ptr->SERIAL_CLK = actual_serial_clk;

    /* initialize quadspi driver */
    if (_quadspi_init_setup(dev_ptr) < 0) {
        _mem_free(dev_spec_ptr);
        return FALSE;
    }

    /* The HW Block only record the valid block */
    for (b = dev_ptr->HW_BLOCK; b->NUM_SECTORS != 0; b++) {
        /* Update sector size */
        sector_size = _quadspi_memory_get_sector_size(quadspi_reg_ptr, (char *)(dev_ptr->BASE_ADDR + b->START_ADDR));
        /* 0 is a flag for not to update sector size:
          * 1. get sector size failed.
          * 2. flash don't provide register to get sector size */
        if (sector_size != 0) {
            b->NUM_SECTORS = b->SECTOR_SIZE * b->NUM_SECTORS / sector_size;
            b->SECTOR_SIZE = sector_size;
        }
    }

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : quadspi_flash_deinit
* Returned Value   : none
* Comments         :
*   Release flash specific information.
*
*END*----------------------------------------------------------------------*/
void quadspi_flash_deinit
    (
        /* [IN] File pointer */
        IO_FLASHX_STRUCT_PTR dev_ptr
    )
{
    FLASHX_QUADSPI_INTERNAL_STRUCT_PTR dev_spec_ptr = (FLASHX_QUADSPI_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA;

    /* de-allocate the device specific structure */
    _mem_free(dev_spec_ptr);
    dev_ptr->DEVICE_SPECIFIC_DATA = NULL;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : quadspi_flash_erase_sector
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Erase a flash memory block.
*
*END*----------------------------------------------------------------------*/
bool quadspi_flash_erase_sector
    (
        /* [IN] File pointer */
        IO_FLASHX_STRUCT_PTR dev_ptr,

        /* [IN] Erased sector address */
        char *dest_ptr,

        /* [IN] Erased sector size */
        _mem_size size
    )
{
    FLASHX_QUADSPI_INTERNAL_STRUCT_PTR dev_spec_ptr = dev_ptr->DEVICE_SPECIFIC_DATA;
    VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr;
    _mqx_uint i;
    bool bfind = FALSE;

    if (dev_spec_ptr == NULL) {
        printf("ERROR: empty device specific data\n");
        return FALSE;
    }

    for (i = 0; dev_ptr->HW_BLOCK[i].NUM_SECTORS != 0; i++)
    {
        if (((uint32_t)dest_ptr >= dev_ptr->BASE_ADDR + dev_ptr->HW_BLOCK[i].START_ADDR) &&
            ((uint32_t)dest_ptr <= dev_ptr->BASE_ADDR + dev_ptr->HW_BLOCK[i].START_ADDR +
            dev_ptr->HW_BLOCK[i].SECTOR_SIZE * dev_ptr->HW_BLOCK[i].NUM_SECTORS)) {
            /* check if the from_ptr is sector aligned or not */
            if ((uint32_t)dest_ptr % dev_ptr->HW_BLOCK[i].SECTOR_SIZE)
                return FALSE;
            /* check if the size is sector aligned or not */
            if ((uint32_t)size % dev_ptr->HW_BLOCK[i].SECTOR_SIZE)
                return FALSE;

            bfind = TRUE;
            break;
        }
    }

    /* The sector address which is expected to be erased is not in valid flash address */
    if (!bfind)
        return FALSE;


    /* Get QuadSPI register structure */
    quadspi_reg_ptr = dev_spec_ptr->QuadSPI_REG_PTR;

    /* Send out erase sector command to external flash */
    if (_quadspi_memory_erase_sector(quadspi_reg_ptr, dest_ptr) != MQX_OK)
        return FALSE;

    /* Invalid data in AHB buffer. Erase operation might change the data in AHB buffer.
     * Invalidate it to make sure the correction of following read operation */
    _quadspi_invalid_buffer(quadspi_reg_ptr, dest_ptr, size);

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : quadspi_flash_write_sector
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Performs a write into flash memory.
*
*END*----------------------------------------------------------------------*/
bool quadspi_flash_write_sector
    (
        /* [IN] File pointer */
        IO_FLASHX_STRUCT_PTR dev_ptr,

        /* [IN] Source address: data to be programed */
        char *src_ptr,

        /* [IN] Destination address: flash address to be programmed */
        char *dest_ptr,

        /* [IN] Number of bytes to write */
        _mem_size size
    )
{
    FLASHX_QUADSPI_INTERNAL_STRUCT_PTR dev_spec_ptr = dev_ptr->DEVICE_SPECIFIC_DATA;
    VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr;

    if (dev_spec_ptr == NULL) {
        printf("ERROR: empty device specific data\n");
        return FALSE;
    }

    /* Get QuadSPI register structure */
    quadspi_reg_ptr = dev_spec_ptr->QuadSPI_REG_PTR;

    if (_quadspi_memory_program (quadspi_reg_ptr, src_ptr, dest_ptr, size) != MQX_OK)
        return FALSE;

    /* Invalid data in AHB buffer. Write operation might change the data in AHB buffer.
     * Invalidate it to make sure the correction of following read operation */
    _quadspi_invalid_buffer(quadspi_reg_ptr, dest_ptr, size);

    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : quadspi_flash_chip_erase
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Erase the whole flash chip.
*
*END*----------------------------------------------------------------------*/
bool quadspi_flash_chip_erase
    (
        /* [IN] File pointer */
        IO_FLASHX_STRUCT_PTR dev_ptr
    )
{
    FLASHX_QUADSPI_INTERNAL_STRUCT_PTR dev_spec_ptr = dev_ptr->DEVICE_SPECIFIC_DATA;
    VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr;
    FLASHX_BLOCK_INFO_STRUCT_PTR b;
    _mem_size size = 0, block_size;
    _mqx_uint ret;

    if (dev_spec_ptr == NULL) {
        printf("ERROR: empty device specific data\n");
        return FALSE;
    }

    /* Get QuadSPI register structure */
    quadspi_reg_ptr = dev_spec_ptr->QuadSPI_REG_PTR;

    /* Delete HW block one by one. One HW block is one external flash chip. */
    for (b = dev_ptr->HW_BLOCK; b->NUM_SECTORS != 0; b++) {
        /* Update sector size */
        ret= _quadspi_memory_chip_erase(quadspi_reg_ptr, (char *)(dev_ptr->BASE_ADDR + b->START_ADDR));

        block_size = b->NUM_SECTORS * b->SECTOR_SIZE;
        size += block_size;
        if (ret != MQX_OK)
            return FALSE;
    }

    /* Invalid data in AHB buffer. Erase operation might change the data in AHB buffer.
      * Invalidate it to make sure the correction of following read operation
      *
      * quadspi_flash_chip_erase will erase all the flash content. Invalid all the flash memories */
    _quadspi_invalid_buffer(quadspi_reg_ptr, (void *) (dev_ptr->BASE_ADDR), size);

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : quadspi_flash_ioctl
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Provide QuadSPI flash IO command to user.
*
*END*----------------------------------------------------------------------*/
_mqx_int quadspi_flash_ioctl
    (
        /* [IN] File pointer */
        IO_FLASHX_STRUCT_PTR dev_ptr,

        /* [IN] the ioctl command */
        _mqx_uint cmd,

        /* [IN] the ioctl command parameter */
        void   *param_ptr
    )
{
    FLASHX_QUADSPI_INTERNAL_STRUCT_PTR dev_spec_ptr = dev_ptr->DEVICE_SPECIFIC_DATA;
    VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr;
    uint32_t serial_clk, actual_serial_clk;
    QuadSPI_IO_MODE io_mode;
    QuadSPI_CLK_MODE clk_mode;
    _mqx_int retval = IO_OK;

    if (dev_spec_ptr == NULL) {
        printf("ERROR: empty device specific data\n");
        return FALSE;
    }

    /* Get QuadSPI register structure */
    quadspi_reg_ptr = dev_spec_ptr->QuadSPI_REG_PTR;

    switch (cmd) {
        case FLASH_QUADSPI_IOCTL_SET_CLOCK:
            serial_clk = *((uint32_t *) param_ptr);

            actual_serial_clk = _quadspi_flash_set_serial_clk(quadspi_reg_ptr, dev_spec_ptr->MODULE_ID, serial_clk);
            if (actual_serial_clk == 0) {
                printf("ERROR: failed to change QuadSPI read clock\n");
                return FLASHXERR_QUADSPI_INVALID_PARM;
            }

            dev_spec_ptr->SERIAL_CLK = actual_serial_clk;
            break;
        case FLASH_QUADSPI_IOCTL_GET_CLOCK:
            *((uint32_t *) param_ptr) = dev_spec_ptr->SERIAL_CLK;
            break;
        case FLASH_QUADSPI_IOCTL_SET_IO_MODE:
            io_mode = *((QuadSPI_IO_MODE *) param_ptr);\

            if (io_mode != dev_spec_ptr->IO_MODE) {
                retval = _quadspi_set_ahb_read(dev_ptr, dev_spec_ptr->CLK_MODE, io_mode);
                if (retval < 0) {
                    printf("ERROR: failed to set QuadSPI IO mode\n");
                    return retval;
                }
            }
            break;
        case FLASH_QUADSPI_IOCTL_GET_IO_MODE:
            *((QuadSPI_IO_MODE *) param_ptr) =  dev_spec_ptr->IO_MODE;
            break;
        case FLASH_QUADSPI_IOCTL_SET_CLK_MODE:
            clk_mode = *((QuadSPI_CLK_MODE *) param_ptr);

            if (clk_mode != dev_spec_ptr->CLK_MODE) {
                retval = _quadspi_set_ahb_read(dev_ptr, clk_mode, dev_spec_ptr->IO_MODE);
                if (retval < 0) {
                    printf("ERROR: failed to set QuadSPI IO mode\n");
                    return retval;
                }
            }
            break;
        case FLASH_QUADSPI_IOCTL_GET_CLK_MODE:
            *((QuadSPI_CLK_MODE *) param_ptr) =  dev_spec_ptr->CLK_MODE;
            break;
        default:
            retval = IO_ERROR_INVALID_IOCTL_CMD;
            break;
    }

    return retval;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_send_command
* Returned Value   : none
* Comments         :
*    Send out quadspi command in specified lookup table.
*
*END*----------------------------------------------------------------------*/
_mqx_int _quadspi_send_command
    (
         /* [IN] Pointer to QSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] Destination address */
        char *dest_ptr,

        /* [IN] Sequence in Look-up-table*/
        uint8_t seqid,

        /* [IN] IP data transfer size */
        _mem_size data_size
    )
{
    uint32_t timeout = QUADSPI_REGS_POLLING_MAX;

    /* wait util current operation end */
    while (quadspi_reg_ptr->SR & (QuadSPI_SR_BUSY_MASK | QuadSPI_SR_IP_ACC_MASK
        | QuadSPI_SR_AHB_ACC_MASK | QuadSPI_SR_AHBTRN_MASK)) {
        if (!timeout--)
            return MQX_ETIMEDOUT;
    }

    quadspi_reg_ptr->SFAR = (uint32_t) dest_ptr;
    quadspi_reg_ptr->IPCR = (data_size + (seqid << QuadSPI_IPCR_SEQID_SHIFT));
    while (quadspi_reg_ptr->SR & QuadSPI_SR_BUSY_MASK) {
        if (!timeout--)
            return MQX_ETIMEDOUT;
    }

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : quadspi_set_ahb_read_function
* Returned Value   : NULL
* Comments         :
*   The ram based execution version for ahb reading configure.
*
*END*-----------------------------------------------------------------------*/
static _mqx_int _quadspi_set_ahb_read
    (
        /* [IN] File pointer */
        IO_FLASHX_STRUCT_PTR dev_ptr,

        /* [IN] QuadSPI clock mode: DDR/SDR */
        QuadSPI_CLK_MODE clk_mode,

        /*[IN] QuadSPI IO mode: single/dual/quad */
        QuadSPI_IO_MODE read_io_mode
    )
{
    FLASHX_QUADSPI_INTERNAL_STRUCT_PTR dev_spec_ptr = dev_ptr->DEVICE_SPECIFIC_DATA;
    VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr;
    FLASHX_BLOCK_INFO_STRUCT_PTR b;
    uint32_t reg_val = 0, smpr_val;
    uint32_t seq_id;
    bool enable_quad;
    uint32_t timeout = QUADSPI_REGS_POLLING_MAX;
    _mqx_int retval = MQX_OK;

    if (dev_spec_ptr == NULL) {
        printf("ERROR: empty device specific data\n");
        return FALSE;
    }

    quadspi_reg_ptr = dev_spec_ptr->QuadSPI_REG_PTR;

    /* enable quad mode for external flash if necessary */
    enable_quad = (read_io_mode == QuadSPI_QUAD_MODE);

    /* Delete HW block one by one. One HW block is one external flash chip. */
    for (b = dev_ptr->HW_BLOCK; b->NUM_SECTORS != 0; b++) {
        /* Update sector size */
        retval= _quadspi_memory_enable_quad_mode(quadspi_reg_ptr, (char *)(dev_ptr->BASE_ADDR + b->START_ADDR), enable_quad);
        if (retval != MQX_OK) {
            return retval;
        }
    }

    /*wait util current operation end*/
    while (quadspi_reg_ptr->SR & (QuadSPI_SR_BUSY_MASK | QuadSPI_SR_IP_ACC_MASK
        | QuadSPI_SR_AHB_ACC_MASK | QuadSPI_SR_AHBTRN_MASK)) {
        if (!timeout--)
            return MQX_ETIMEDOUT;
    }

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
        seq_id = (uint32_t) FLASHX_QUADSPI_READ + (uint32_t) read_io_mode;
    } else {
        /* Set DDR sampling point for DDR mode */
        smpr_val = quadspi_reg_ptr->SMPR;
        smpr_val &= ~QuadSPI_SMPR_DDRSMP_MASK;
        smpr_val |= (1 << QuadSPI_SMPR_DDRSMP_SHIFT);
        quadspi_reg_ptr->SMPR = smpr_val;

        /* Enable 2x and 4x clock for SDR mode */
        quadspi_reg_ptr->MCR |= QuadSPI_MCR_DDR_EN_MASK;;

        /* Set DDR read prefetch command sequence in LUT */
        seq_id = (uint32_t) FLASHX_QUADSPI_DDR_READ + (uint32_t) read_io_mode;
    }

    /* Enable QuadSPI clock */
    quadspi_reg_ptr->MCR &= ~(QuadSPI_MCR_MDIS_MASK);

    /* Set AHB read prefetch command */
    reg_val = quadspi_reg_ptr->BFGENCR;
    reg_val &= ~QuadSPI_BFGENCR_SEQID_MASK;
    reg_val |= (seq_id << QuadSPI_BFGENCR_SEQID_SHIFT);

    if (!dev_spec_ptr->PARALLEL_ENABLED)
        reg_val &= ~QuadSPI_BFGENCR_PAR_EN_MASK;
    else
        reg_val |= QuadSPI_BFGENCR_PAR_EN_MASK;

    quadspi_reg_ptr->BFGENCR = reg_val;

    /* update internal IO mode */
    dev_spec_ptr->CLK_MODE = clk_mode;
    dev_spec_ptr->IO_MODE = read_io_mode;

    return MQX_OK;
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
        /* [IN] File pointer */
        IO_FLASHX_STRUCT_PTR dev_ptr
    )
{
    FLASHX_QUADSPI_INTERNAL_STRUCT_PTR dev_spec_ptr = dev_ptr->DEVICE_SPECIFIC_DATA;
    VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr = dev_spec_ptr->QuadSPI_REG_PTR;
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
    quadspi_reg_ptr->SFA1AD = dev_spec_ptr->FLASH_TOP_ADDR[0]; // top address of FA1
    quadspi_reg_ptr->SFA2AD = dev_spec_ptr->FLASH_TOP_ADDR[1]; // top address of FA2
    quadspi_reg_ptr->SFB1AD = dev_spec_ptr->FLASH_TOP_ADDR[2]; // top address of FB1
    quadspi_reg_ptr->SFB2AD = dev_spec_ptr->FLASH_TOP_ADDR[3]; // top address of FB2

    /* Setup Lookup table for specific flash device */
    retval = _quadspi_flashx_setup_LUT(quadspi_reg_ptr);
    if (retval < 0) {
        return retval;
    }

    /* set quad ddr as xbar read instruction */
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
    retval = _quadspi_set_ahb_read(dev_ptr, dev_spec_ptr->CLK_MODE, dev_spec_ptr->IO_MODE);

    return retval;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_program_64bytes
* Returned Value   : number of bytes programmed. >=0 if successful, <0 if failed.
* Comments:
*       This function writes data buffer to external memory by send program command.
*       In order to avoid TX buffer underrun which might happened, the maximum program
* size is 64bytes.
*
*
*END*-----------------------------------------------------------------------*/
int32_t _quadspi_program_64bytes
    (
        /* [IN] Pointer to QuadSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] Source address: the data to be programed */
        char *src_ptr,

        /* [IN] Destination address: flash address where to program */
        char *dest_ptr,

        /* [IN] Number of bytes to write */
        _mem_size size
    )
{
    uint32_t *src_ptr_32 = (uint32_t *) src_ptr;
    int32_t i;
    uint32_t timeout = QUADSPI_REGS_POLLING_MAX;

    if (size > 64) {
        printf("ERROR: QuadSPI don't accept page size greater than %d\n", 64);
        return -1;
    }

     /* Clear TX fifo */
    quadspi_reg_ptr->MCR |= QuadSPI_MCR_CLR_TXF_MASK;

    /* Set the CS according to flash address */
    quadspi_reg_ptr->SFAR = (uint32_t) dest_ptr;

    /* The Tx buffer is 64 bytes (16 * 32bit) circle buffer. Fill it before send write command */
    for (i = 0; i < size; i += 4) {
        quadspi_reg_ptr->TBDR = byte_swap32(*src_ptr_32);
        src_ptr_32++;
    }

    /* Send write command */
    if (_quadspi_send_command(quadspi_reg_ptr, dest_ptr, FLASHX_QUADSPI_PAGE_PROM, size) != MQX_OK)
        return -1;

    while (quadspi_reg_ptr->SR & QuadSPI_SR_BUSY_MASK) {
        if (!timeout--)
            return -1;
    }

    return size;
}

