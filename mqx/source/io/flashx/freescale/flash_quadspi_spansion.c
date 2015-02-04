
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
*   The file contains device specific flash functions.
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "flashx.h"
#include "flashxprv.h"
#include "flash_quadspi.h"
#include "flash_quadspi_prv.h"
#include "flash_vybrid.h"

/* write in progress bit in RDSR register */
#define RDSR1_WIP_SHIFT                 0

/* write enable bit in RDSR register */
#define RDSR1_WEL_SHIFT                 1

/* write in progress bit in RDSR register */
#define CR1_QUAD_SHIFT                  1
#define CR1_QUAD_MASK                   0x1 << CR1_QUAD_SHIFT

#define DEVICE_ID_SECTOR                4

/* maximum time to try if write enable is set */
#define QUADSPI_WAIT_WREN_TIMEOUT            100
#define QUADSPI_WAIT_EN_QUAD_TIMEOUT         1000 * 1000
#define QUADSPI_WAIT_RD_STATUS_TIMEOUT       100
/* maximum time to wait for 64bytes is programmed to external flash:
 * program rate is 1000kbytes/s, the 64byte /1000k = 64us
 * 16 serial clock to read register: 16 / 133MHz = 0.12 us
 * */
#define QUADSPI_PROGRAM_64_TIMEOUT      500000

/* maximum time to wait for chip erase in ms (10min)*/
#define QUADSPI_ERASE_CHIP_TIMEOUT      10 * 60 * 1000

/* maximum time to wait for sector erase in ms (30s)
  * 0x40000 / 512kbytes/s = 512 ms */
#define QUADSPI_SECTOR_ERASE_TIMEOUT    30 * 1000

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_setup_LUT
* Returned Value   : Setup spansion flash's lookup table for quadspi
* Comments         :
*    MQX error code.
*
*END*----------------------------------------------------------------------*/
_mqx_int _quadspi_flashx_setup_LUT
    (
        /* [IN] Pointer to QSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr
    )
{
    uint32_t lut_seq;
    _quadspi_unlockLookupTable(quadspi_reg_ptr);

    /* SEQ0: SDR I/O read */
    lut_seq = FLASHX_QUADSPI_READ * 4;
#ifdef BSP_QUADSPI_ENABLE_32BIT
    quadspi_reg_ptr->LUT[lut_seq++] = 0x08200413;
#else
    quadspi_reg_ptr->LUT[lut_seq++] = 0x08180403;
#endif
    quadspi_reg_ptr->LUT[lut_seq++] = 0x1c80;
    quadspi_reg_ptr->LUT[lut_seq++] = 0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0;

    /* SEQ1: SDR dual I/O read */
    lut_seq = FLASHX_QUADSPI_DUAL_READ * 4;
#ifdef BSP_QUADSPI_ENABLE_32BIT
    quadspi_reg_ptr->LUT[lut_seq++] = 0x092004BC;
#else
    quadspi_reg_ptr->LUT[lut_seq++] = 0x091804BB;
#endif
    quadspi_reg_ptr->LUT[lut_seq++] = 0x1D800C04;
    quadspi_reg_ptr->LUT[lut_seq++] = 0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0;

    /* SEQ2: SDR quad I/O read */
    lut_seq = FLASHX_QUADSPI_QUAD_READ * 4;
#ifdef BSP_QUADSPI_ENABLE_32BIT
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0A2004EC;
#else
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0A1804EB;
#endif
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0C0412FF;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x1E80;
    quadspi_reg_ptr->LUT[lut_seq++] = 0;

    /* SEQ3: DDR I/O read */
    lut_seq = FLASHX_QUADSPI_DDR_READ * 4;
#ifdef BSP_QUADSPI_ENABLE_32BIT
    quadspi_reg_ptr->LUT[lut_seq++] = 0x2820040E;
#else
    quadspi_reg_ptr->LUT[lut_seq++] = 0x2818040D;
#endif
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0C022CFF;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x3880;
    quadspi_reg_ptr->LUT[lut_seq++] = 0;

    /* SEQ4: DDR dual I/O read */
    lut_seq = FLASHX_QUADSPI_DDR_DUAL_READ * 4;
#ifdef BSP_QUADSPI_ENABLE_32BIT
    quadspi_reg_ptr->LUT[lut_seq++] = 0x292004BE;
#else
    quadspi_reg_ptr->LUT[lut_seq++] = 0x291804BD;
#endif
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0C042DFF;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x3980;
    quadspi_reg_ptr->LUT[lut_seq++] = 0;

    /* SEQ5: DDR quad I/O read */
    lut_seq = FLASHX_QUADSPI_DDR_QUAD_READ * 4;
#ifdef BSP_QUADSPI_ENABLE_32BIT
    quadspi_reg_ptr->LUT[lut_seq++] = 0x2A2004EE;
#else
    quadspi_reg_ptr->LUT[lut_seq++] = 0x2A1804ED;
#endif
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0C062EFF;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x3A80;
    quadspi_reg_ptr->LUT[lut_seq++] = 0;

    /* SEQ6: Set write enable */
    lut_seq = FLASHX_QUADSPI_WRITE_EN * 4;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x406;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;

    /* SEQ7: Sector erase */
    lut_seq = FLASHX_QUADSPI_SECTOR_ERASE * 4;
#ifdef BSP_QUADSPI_ENABLE_32BIT
    quadspi_reg_ptr->LUT[lut_seq++] = 0x082004DC;
#else
    quadspi_reg_ptr->LUT[lut_seq++] = 0x081804D8;
#endif
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;

    /* SEQ8: Chip erase */
    lut_seq = FLASHX_QUADSPI_CHIP_ERASE * 4;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x460;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;

    /* SEQ9: Page program */
    lut_seq = FLASHX_QUADSPI_PAGE_PROM * 4;
#ifdef BSP_QUADSPI_ENABLE_32BIT
    quadspi_reg_ptr->LUT[lut_seq++] = 0x08200412;
#else
    quadspi_reg_ptr->LUT[lut_seq++] = 0x08180402;
#endif
    quadspi_reg_ptr->LUT[lut_seq++] = 0x2040;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;

    /* SEQ10: Write config */
    lut_seq = FLASHX_QUADSPI_WRITE_CONFIG * 4;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x20020401;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;

    /* SEQ11: Read status */
    lut_seq = FLASHX_QUADSPI_READ_STATUS * 4;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x1c010405;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;

    /* SEQ12: Read config */
    lut_seq = FLASHX_QUADSPI_READ_CONFIG * 4;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x1c010435;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;

    /* SEQ13: Read ID */
    lut_seq = FLASHX_QUADSPI_READ_ID * 4;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x1c08049f;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;
    quadspi_reg_ptr->LUT[lut_seq++] = 0x0;

    _quadspi_lockLookupTable(quadspi_reg_ptr);

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_memory_read_status
* Returned Value   : none
* Comments         :
*    Flash status register value in byte.
*
*END*----------------------------------------------------------------------*/
static bool _quadspi_memory_read_status
    (
         /* [IN] Pointer to QSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] Destination address */
        char   *dest_ptr,

        /* [IN/OUT] Status register value */
        uint8_t *status
    )
{
    uint32_t tmp_val;

    /* RX Buffer is reading using IP Bus Registers */
    quadspi_reg_ptr->RBCT |= QuadSPI_RBCT_RXBRD_MASK;

    /* Clear RX buffer */
    quadspi_reg_ptr->MCR |= QuadSPI_MCR_CLR_RXF_MASK;

    /* Send out read status command */
    _quadspi_send_command(quadspi_reg_ptr, dest_ptr, FLASHX_QUADSPI_READ_STATUS, 1);

    /* Read status in IP buffer */
    if (quadspi_reg_ptr->RBSR & QuadSPI_RBSR_RDBFL_MASK)
        tmp_val = quadspi_reg_ptr->RBDR[0];
    else
        return FALSE;

    *status = byte_swap32(tmp_val) & 0xFF;

    /* Clear RX buffer */
    quadspi_reg_ptr->MCR |= QuadSPI_MCR_CLR_RXF_MASK;

    /* RX Buffer is reading using AHB bus by default*/
    quadspi_reg_ptr->RBCT &= ~QuadSPI_RBCT_RXBRD_MASK;

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_memory_read_config
* Returned Value   : none
* Comments         :
*    Flash configuration register value in byte.
*
*END*----------------------------------------------------------------------*/
static _mqx_int _quadspi_memory_read_config
    (
         /* [IN] Pointer to QSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] Destination address */
        char   *dest_ptr,

        /* [IN/OUT] Status register value */
        uint8_t *config
    )
{
    uint32_t tmp_val;
    uint32_t timeout = QUADSPI_REGS_POLLING_MAX;

    /* RX Buffer is reading using IP Bus Registers */
    quadspi_reg_ptr->RBCT |= QuadSPI_RBCT_RXBRD_MASK;

    /* Clear RX buffer */
    quadspi_reg_ptr->MCR |= QuadSPI_MCR_CLR_RXF_MASK;

    /* Send out read status command */
    _quadspi_send_command(quadspi_reg_ptr, dest_ptr, FLASHX_QUADSPI_READ_CONFIG, 1);

    /* Read status in IP buffer */
    while (!(quadspi_reg_ptr->RBSR & QuadSPI_RBSR_RDBFL_MASK)) {
        if (!timeout--) {
            printf("TIMTOUT: wait for RX buffer data is filled\n!");
            return MQX_ETIMEDOUT;
        }
    }

    tmp_val = quadspi_reg_ptr->RBDR[0];

    *config = byte_swap32(tmp_val) & 0xFF;

    /* Clear RX buffer */
    quadspi_reg_ptr->MCR |= QuadSPI_MCR_CLR_RXF_MASK;

    /* RX Buffer is reading using AHB bus by default*/
    quadspi_reg_ptr->RBCT &= ~QuadSPI_RBCT_RXBRD_MASK;

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_memory_write_config
* Returned Value   : none
* Comments         :
*    Set flash configration register.
*
*END*----------------------------------------------------------------------*/
static _mqx_int _quadspi_memory_write_config
    (
         /* [IN] Pointer to QSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] Destination address */
        char   *dest_ptr,

        /* [IN/OUT] Status register value */
        uint8_t config
    )
{
    uint32_t timeout = QUADSPI_WAIT_RD_STATUS_TIMEOUT;
    uint8_t status;
    uint16_t reg_val;

    /* check if WEL bit is set */
    while (_quadspi_memory_read_status(quadspi_reg_ptr, dest_ptr, &status) == FALSE) {
        if (!timeout--)
            return MQX_ETIMEDOUT;
    };

    /* Clear TX buffer */
    quadspi_reg_ptr->MCR |= QuadSPI_MCR_CLR_TXF_MASK;

    /* Send out write register command: status + configuration */
    reg_val = (uint16_t) status + ((uint16_t) config << 8);
    quadspi_reg_ptr->TBDR = byte_swap32(reg_val);

    _quadspi_send_command(quadspi_reg_ptr, dest_ptr, FLASHX_QUADSPI_WRITE_CONFIG, 2);

    /* Clear RX buffer */
    quadspi_reg_ptr->MCR |= QuadSPI_MCR_CLR_TXF_MASK;

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_memory_wait_for_not_busy
* Returned Value   : none
* Comments         :
*    Wait until external flash memory is not in write progress.
*
*END*----------------------------------------------------------------------*/
static  _mqx_int _quadspi_memory_wait_for_not_busy
    (
         /* [IN] Pointer to QSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] Destination address */
        char   *dest_ptr,

        /* [IN] Time out times (unit: time of read status operation) */
        uint32_t timeout
    )
{
    uint8_t status, wip_flag = 1 << RDSR1_WIP_SHIFT;

    status = wip_flag;
    while (status & wip_flag) {
        if (!timeout--)
            return MQX_ETIMEDOUT;

        if (_quadspi_memory_read_status(quadspi_reg_ptr, dest_ptr, &status) == FALSE)
            status = wip_flag;
    }

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_memory_sleep_wait_for_not_busy
* Returned Value   : none
* Comments         :
*    Wait until external flash memory is not in write/erase progress. It will sleep to release CPU for other task.
*
*END*----------------------------------------------------------------------*/
static _mqx_int _quadspi_memory_sleep_wait_for_not_busy
    (
         /* [IN] Pointer to QSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] Destination address */
        char   *dest_ptr,

        /* [IN]  Sleep time after every query in ms. */
        uint32_t sleep_time_ms,

        /* [IN] Time out times (unit: sleep_time_ms) */
        uint32_t timeout
    )
{
    uint8_t status, wip_flag = 1 << RDSR1_WIP_SHIFT;

    status = wip_flag;
    while (status & wip_flag) {
        if (!timeout--)
            return MQX_ETIMEDOUT;

        if (_quadspi_memory_read_status(quadspi_reg_ptr, dest_ptr, &status) == FALSE)
            status = wip_flag;

        _time_delay(sleep_time_ms);
    }

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_memory_set_write_en
* Returned Value   : none
* Comments         :
*    Set flash write enable (WREN).
*
*END*----------------------------------------------------------------------*/
static _mqx_int _quadspi_memory_set_write_en
    (
         /* [IN] Pointer to QSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] Destination address */
        char   *dest_ptr
    )
{
    uint8_t status = 0, wel_flag = 1 << RDSR1_WEL_SHIFT;
    uint32_t timeout = QUADSPI_WAIT_WREN_TIMEOUT;

    /* Clear RX buffer */
    quadspi_reg_ptr->MCR |= QuadSPI_MCR_CLR_RXF_MASK;

    status = 0;
    while ((status & wel_flag) != wel_flag) {
        if (!timeout--)
            return MQX_ETIMEDOUT;

        /* send out write enable command */
        _quadspi_send_command(quadspi_reg_ptr, dest_ptr, FLASHX_QUADSPI_WRITE_EN, 0);

        /* check if WEL bit is set */
        if (_quadspi_memory_read_status(quadspi_reg_ptr, dest_ptr, &status) == FALSE)
            status = 0;
    }

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_memory_erase_sector
* Returned Value   : MQX Error Code
* Comments         :
*    Erase a sector of flash. The sector is specified by flash address.
*
*END*----------------------------------------------------------------------*/
_mqx_int _quadspi_memory_erase_sector
    (
         /* [IN] Pointer to QSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] Destination address: address to be erased */
        char   *dest_ptr
    )
{
    _mqx_int retval = MQX_OK;

    /* 1 Set write enable for external flash */
    retval = _quadspi_memory_set_write_en(quadspi_reg_ptr, dest_ptr);
    if (retval != MQX_OK)
        return retval;

    /* 2 Send out sector serase command */
    retval = _quadspi_send_command(quadspi_reg_ptr, dest_ptr, FLASHX_QUADSPI_SECTOR_ERASE, 0);
    if (retval != MQX_OK)
        return retval;

    /* 3 Wait until flash is not busy */
    retval = _quadspi_memory_sleep_wait_for_not_busy(quadspi_reg_ptr, dest_ptr, 5, QUADSPI_SECTOR_ERASE_TIMEOUT/5);

    return retval;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_memory_chip_erase
* Returned Value   : MQX Error Code
* Comments         :
*    Erase a whole of flash chip.
*
*END*----------------------------------------------------------------------*/
_mqx_int _quadspi_memory_chip_erase
    (
         /* [IN] Pointer to QSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] Destination address: address to be erased */
        char   *dest_ptr
    )
{
    _mqx_int retval = MQX_OK;

    /* 1 Set write enable for external flash */
    retval = _quadspi_memory_set_write_en(quadspi_reg_ptr, dest_ptr);
    if (retval != MQX_OK)
        return retval;

    /* 2 Send out sector serase command */
    retval = _quadspi_send_command(quadspi_reg_ptr, dest_ptr, FLASHX_QUADSPI_CHIP_ERASE, 0);
    if (retval != MQX_OK)
        return retval;

    /* 3 Wait until flash is not busy */
    retval = _quadspi_memory_sleep_wait_for_not_busy(quadspi_reg_ptr, dest_ptr, 100, QUADSPI_ERASE_CHIP_TIMEOUT/100);

    return retval;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_memory_program_dynamic
* Returned Value   : MQX Error Code
* Comments         :
*    This function writes data buffer to memory using page write.
*
*END*----------------------------------------------------------------------*/
int32_t _quadspi_memory_program
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
    uint32_t write_size, count = size, temp;
    uint32_t page_size = 64;
    _mqx_int retval = MQX_OK;

    temp = (uint32_t) dest_ptr;
    while(count > 0) {
        /* For page alignment, which is needed for the first time only */
        write_size = (page_size - ((uint32_t)temp % page_size));
        write_size = (write_size <= count) ? write_size : count;

        /* Set write enable for external flash */
        retval = _quadspi_memory_set_write_en (quadspi_reg_ptr, dest_ptr);
        if (retval != MQX_OK)
            return retval;

        /* Program external flash */
        if (_quadspi_program_64bytes (quadspi_reg_ptr, src_ptr, dest_ptr, write_size) < 0)
            return FLASHXERR_QUADSPI_PROGRAM;

        /* Wait until flash is not busy */
        retval = _quadspi_memory_wait_for_not_busy(quadspi_reg_ptr, dest_ptr, QUADSPI_PROGRAM_64_TIMEOUT);
        if (retval != MQX_OK)
            return retval;

        count -= write_size;
        src_ptr += write_size;
        dest_ptr += write_size;
    }

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_memory_get_sector_size
* Returned Value   : none
* Comments         :
*    Flash actual sector size read from flash device id.
*
*END*----------------------------------------------------------------------*/
uint32_t _quadspi_memory_get_sector_size
    (
         /* [IN] Pointer to QSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] Destination address */
        char   *dest_ptr
    )
{
    uint32_t id[2], sector_size;
    uint8_t *sector_info_ptr;
    _mqx_int retval = MQX_OK;

    /* RX Buffer is reading using IP Bus Registers */
    quadspi_reg_ptr->RBCT |= QuadSPI_RBCT_RXBRD_MASK;

    /* Send out read status command */
    retval = _quadspi_send_command(quadspi_reg_ptr, dest_ptr, FLASHX_QUADSPI_READ_ID, 0);
    if (retval != MQX_OK)
        return 0;

    id[0] = byte_swap32(quadspi_reg_ptr->RBDR[0]);
    id[1] = byte_swap32(quadspi_reg_ptr->RBDR[1]);

    sector_info_ptr = (uint8_t *)((uint32_t) id + DEVICE_ID_SECTOR);

    if (*sector_info_ptr == 0x00)
        sector_size = 0x40000;
    else if (*sector_info_ptr == 0x01)
        sector_size = 0x10000;
    else {
        printf("ERROR: unknown sector size!\n");
        sector_size = 0;
    }

    return sector_size;
}

static bool _quadspi_memory_check_quad_mode
    (
        /* [IN] Pointer to QSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] Destination address: address to be erased */
        char   *dest_ptr,

        /* [IN/OUT] Pointer ot flash configuration value*/
        uint8_t *cr1_reg_val
    )
{
    bool result = FALSE;

    if(_quadspi_memory_read_config(quadspi_reg_ptr, dest_ptr, cr1_reg_val) < 0)
        return FALSE;

    if (*cr1_reg_val & (1 << CR1_QUAD_SHIFT))
        result = TRUE;

    return result;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_memory_enable_quad_mode
* Returned Value   : none
* Comments         :
*    Enable or disable external flash quad mode.
*
*END*----------------------------------------------------------------------*/
uint32_t _quadspi_memory_enable_quad_mode
    (
         /* [IN] Pointer to QSPI register base address */
        VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,

        /* [IN] Destination address: address to be erased */
        char   *dest_ptr,

        /* [IN] Destination address */
        bool enable
    )
{
    uint8_t cr1_reg_val;
    bool cur_quad_enable;
    _mqx_int retval = MQX_OK;
    // uint32_t timeout = QUADSPI_WAIT_EN_QUAD_TIMEOUT;

    /* read flash configuration register (CR1) */
    cur_quad_enable = _quadspi_memory_check_quad_mode(quadspi_reg_ptr, dest_ptr, &cr1_reg_val);
    if (cur_quad_enable == enable) {
        /* don't change quad mode, nothing to do */
        return MQX_OK;
    }

    /* Set write enable for external flash */
    retval = _quadspi_memory_set_write_en(quadspi_reg_ptr, dest_ptr);
    if (retval != MQX_OK) {
        return retval;
    }

    /* Send write register command */
    if (enable)
        cr1_reg_val |= CR1_QUAD_MASK;
    else
        cr1_reg_val &= ~CR1_QUAD_MASK;

    _quadspi_memory_write_config(quadspi_reg_ptr, dest_ptr, cr1_reg_val);

    /* Wait until flash is not busy */
    retval = _quadspi_memory_wait_for_not_busy(quadspi_reg_ptr, dest_ptr, QUADSPI_WAIT_EN_QUAD_TIMEOUT);
    if (retval != MQX_OK) {
        printf("ERROR: failed to wait for flash device not busy\n");
        return retval;
    }

    /* Check if the quad mode is set correctly */
    cur_quad_enable = _quadspi_memory_check_quad_mode(quadspi_reg_ptr, dest_ptr, &cr1_reg_val);
    if (cur_quad_enable != enable) {
        /* don't change quad mode, nothing to do */
        return FLASHXERR_QUADSPI_SET_QUAD_MODE;
    }

    return MQX_OK;
}

