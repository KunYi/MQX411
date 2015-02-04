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
*   This file contains the functions which write and read the QuadSPI memories
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "qspi_memory.h"

//#define QSPI_MEMORY_DEBUG

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : build_flash_command
* Comments  : This function build QuadSPI command into command buffer.
*             The command must end up with QuadSPI_LOOKUP_STOP
* Return:
*         NULL
*
*END*----------------------------------------------------------------------*/
static void qspi_build_flash_command(uint8_t *buffer, uint16_t cmd, ...)
{
    uint32_t i = 0;
    va_list ap;

    i = 0;
    va_start(ap, cmd);
    while(1) {
        if (cmd == QuadSPI_LOOKUP_STOP)
            break;
        buffer[i++] = cmd & 0xFF;
        buffer[i++] = (cmd >> 8) & 0xFF;
        cmd = va_arg(ap, uint32_t);
    }
    va_end(ap);

    /* All the command is end up with STOP command */
    buffer[i++] = QuadSPI_LOOKUP_STOP & 0xFF;
    buffer[i] = (QuadSPI_LOOKUP_STOP >> 8) & 0xFF;
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : qspi_build_read_command
* Comments  : This function build QuadSPI read command into command buffer
*             according to clock and IO mode.
* Return:
*         NULL
*
*END*----------------------------------------------------------------------*/
static void qspi_build_read_command(uint8_t *buffer, uint32_t clock_mode, uint32_t io_mode, uint32_t read_size)
{
    uint32_t mode = QSPI_READ_MODE(clock_mode, io_mode);

    switch (mode) {
        case QSPI_SDR_SINGLE_READ:
            /* SDR single I/O read */
            qspi_build_flash_command(buffer,
                    QuadSPI_SET_RD,
                    QuadSPI_SET_ADDR(io_mode),
                    QuadSPI_READ_DATA(io_mode, read_size),
                    QuadSPI_LOOKUP_STOP);
            break;
        case QSPI_SDR_DUAL_READ:
            /* SDR dual I/O read */
            qspi_build_flash_command(buffer,
                    QuadSPI_SET_DUAL_RD,
                    QuadSPI_SET_ADDR(io_mode),
                    QuadSPI_DUMMY_DATA(0x04),
                    QuadSPI_READ_DATA(io_mode, read_size),
                    QuadSPI_LOOKUP_STOP);
            break;
        case QSPI_SDR_QUAD_READ:
            /* SDR quad I/O read */
            qspi_build_flash_command(buffer,
                    QuadSPI_SET_QUAD_RD,
                    QuadSPI_SET_ADDR(io_mode),
                    QuadSPI_SET_MODE(io_mode, 0xFF),
                    QuadSPI_DUMMY_DATA(0x04),
                    QuadSPI_READ_DATA(QuadSPI_QUAD_PAD, read_size),
                    QuadSPI_LOOKUP_STOP);
            break;
        case QSPI_DDR_SINGLE_READ:
             /* DDR single I/O read */
            qspi_build_flash_command(buffer,
                    QuadSPI_SET_DDR_FAST_RD,
                    QuadSPI_SET_DDR_ADDR(io_mode),
                    QuadSPI_SET_MODE_DDR(io_mode, 0xFF),
                    QuadSPI_DUMMY_DATA(0x02),
                    QuadSPI_DDR_READ_DATA(QuadSPI_SINGLE_PAD, read_size),
                    QuadSPI_LOOKUP_STOP);
             break;
        case QSPI_DDR_DUAL_READ:
            /* DDR dual I/O read */
            qspi_build_flash_command(buffer,
                    QuadSPI_SET_DDR_DUAL_RD,
                    QuadSPI_SET_DDR_ADDR(io_mode),
                    QuadSPI_SET_MODE_DDR(io_mode, 0xFF),
                    QuadSPI_DUMMY_DATA(0x04),
                    QuadSPI_DDR_READ_DATA(QuadSPI_DUAL_PAD, read_size),
                    QuadSPI_LOOKUP_STOP);
            break;
        case QSPI_DDR_QUAD_READ:
            /* DDR quad I/O read */
            qspi_build_flash_command(buffer,
                    QuadSPI_SET_DDR_QUAD_RD,
                    QuadSPI_SET_DDR_ADDR(io_mode),
                    QuadSPI_SET_MODE_DDR(io_mode, 0xFF),
                    QuadSPI_DUMMY_DATA(0x06),
                    QuadSPI_DDR_READ_DATA(QuadSPI_QUAD_PAD, read_size),
                    QuadSPI_LOOKUP_STOP);
            break;
        default:
            printf("ERROR: QSPI I/O clock mode is not supported\n");
            break;
    }
}

void addr_to_data_buf(uint32_t data_ptr, uint8_t *buffer)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        buffer[i] = (uint8_t)(data_ptr & 0xFF);
        data_ptr >>= 8;
    }
}

void memory_wait_for_not_busy(MQX_FILE_PTR qspifd)
{
    uint8_t status = 0x01;

    while(status & 0x1) {
        if (memory_read_status1(qspifd, &status) < 0)
            status = 0x01;
    }
}

void memory_wait_for_write_en(MQX_FILE_PTR qspifd)
{
    uint8_t status = 0x0;

    while(!(status & 0x2)) {
        if (memory_read_status1(qspifd, &status) < 0)
            status = 0x0;
    }
}

bool memory_check_quad_mode(MQX_FILE_PTR qspifd)
{
    uint8_t status = 0x0;
    bool result = FALSE;

    if( memory_read_config1(qspifd, &status) < 0)
        return FALSE;

    if (status & 0x2)
        result = TRUE;

    return result;
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_set_write_en
* Comments  : This function enable/disable memory write operation
* Return  :
*            NULL.
*
*END*----------------------------------------------------------------------*/
static void memory_set_write_en(MQX_FILE_PTR qspifd, uint32_t addr, bool enable)
{
    _mqx_int result;
    uint8_t buffer[4+QuadSPI_ADDR_BYTES] = {0};

    if (qspifd == NULL) {
        printf("ERROR: Empty QuadSPI file handler! Please check! \n");
        return;
    }

    result = ioctl(qspifd, QuadSPI_IOCTL_SET_FLASH_ADDR, &addr);
    if (result != MQX_OK) {
        printf("memory_write_data: failed at ioctl set flash address!\n");
        return;
    }

    if (enable) {
        qspi_build_flash_command(buffer, QuadSPI_WRITE_EN, QuadSPI_LOOKUP_STOP);
    } else {
        qspi_build_flash_command(buffer, QuadSPI_WRITE_DISABLE, QuadSPI_LOOKUP_STOP);
    }

    /* Write instruction */
    result = fwrite (buffer, 1, 4 + QuadSPI_ADDR_BYTES, qspifd);
    if (result < 0) {
        printf ("ERROR\n");
        return;
    }

    /* Wait until external flash is write enable */
    if (enable)
       memory_wait_for_write_en(qspifd);
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_chip_erase
* Comments  : This function erases the whole memory QuadSPI chip
* Return  :
*            MQX error code. <0 if failed.
*
*END*----------------------------------------------------------------------*/
int32_t memory_chip_erase(MQX_FILE_PTR qspifd)
{
    _mqx_int result;
    uint8_t buffer[4+QuadSPI_ADDR_BYTES] = {0};
    QuadSPI_FLASH_INFO_STRUCT *flash_dev;
    uint32_t i, addr, flash_base_addr;
    int32_t size;
    QuadSPI_MEM_BLOCK_STRUCT mem_block;

    if (qspifd == NULL) {
        printf("ERROR: Empty QuadSPI file handler! Please check! \n");
        return -1;
    }

    /* Build erase command */
    qspi_build_flash_command(buffer, QuadSPI_CHIP_ERASE, QuadSPI_LOOKUP_STOP);

    result = ioctl(qspifd, QuadSPI_IOCTL_GET_FLASH_INFO, &flash_dev);
    if (result < 0) {
        printf("ERROR: failed to get flash information!\n");
        return result;
    }

    /* Get flash base address */
    result = ioctl(qspifd, QuadSPI_IOCTL_GET_MEM_BASE_ADDR, &flash_base_addr);
    if (result < 0) {
        printf("ERROR: failed to get flash memory mapped base address!\n");
        return result;
    }

    for (i = 0; i < 4; i++) {
        /* No external flash on that port */
        if ((flash_dev->NUM_SECTOR * flash_dev->SECTOR_SIZE )== 0) {
            flash_dev++;
            continue;
        }

        /* Enable flash memory write */
        addr = flash_base_addr + flash_dev->START_ADDR;
        memory_set_write_en(qspifd, addr, TRUE);

        /* Send erase command */
        result = fwrite(buffer, 1, 4 + QuadSPI_ADDR_BYTES, qspifd);
        if (result < 0) {
            printf("ERROR\n");
            return result;
        }

        /* Wait till the flash is not busy at program */
        memory_wait_for_not_busy(qspifd);

        flash_dev++;
    }

    /* Invalid data in AHB or IP buffer. Erase operation might change the data in AHB buffer.
     * Invalidate it to make sure the correction of following read operation
     *
     * memory_chip_erase will erase all the flash content. Invalid all the flash memories */
    size = memory_get_total_size(qspifd);
    if (size < 0)
        return -1;

    mem_block.ADDR = (void *) flash_base_addr;
    mem_block.SIZE = size;

    result = ioctl(qspifd, QuadSPI_IOCTL_INVALID_BUFFER, &mem_block);
    if (result < 0) {
        printf ("ERROR: failed to invalid buffer!\n");
        return result;
    }

    printf("QuadSPI Successfully Erase Flash\n");

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _quadspi_memory_get_sector_size
* Returned Value   : The actual sector size of external flash device. = 0 if failed.
* Comments         :
*    Flash actual sector size read from flash device id.
*
*END*----------------------------------------------------------------------*/
uint32_t memory_get_sector_size (MQX_FILE_PTR qspifd)
{
    uint32_t sector_size;
    uint8_t buffer[6+QuadSPI_ADDR_BYTES] = {0};
    uint8_t id_info[8];
    _mqx_int result = MQX_OK;

    if (qspifd == NULL) {
        printf("ERROR: Empty QuadSPI file handler! Please check! \n");
        return 0;
    }

    qspi_build_flash_command(buffer,
                    QuadSPI_READ_ID,
                    QuadSPI_READ_DATA(QuadSPI_SINGLE_PAD, 8),
                    QuadSPI_LOOKUP_STOP);

    /* Write instruction */
    result = fwrite(buffer, 1, 6 + QuadSPI_ADDR_BYTES + 8, qspifd);
    if(result < 0) {
      /* Stop transfer */
        printf("ERROR (tx)\n");
        return 0;
    }

    /* Read memory status: 1byte */
    result = fread (id_info, 1, 8, qspifd);
    if(result < 1) {
        printf ("memory_read_status1: ERROR (rx)\n");
        return 0;
    }

    if (id_info[DEVICE_ID_SECTOR_ADDR] == DEVICE_ID_SECTOR_256K)
        sector_size = QuadSPI_SECTOR_256K;
    else if (id_info[DEVICE_ID_SECTOR_ADDR] == DEVICE_ID_SECTOR_64K)
        sector_size = QuadSPI_SECTOR_64K;
    else {
        printf("ERROR: unknown sector size!\n");
        sector_size = 0;
    }

    return sector_size;
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_chip_erase
* Comments  : This function erases the sector where the current address is in.
* Return  :
*            The begin address of erased sector. <0 if failed.
*
*END*----------------------------------------------------------------------*/
int32_t memory_sector_erase(MQX_FILE_PTR qspifd, uint32_t addr)
{
    _mqx_int result;
    uint8_t buffer[6+QuadSPI_ADDR_BYTES] = {0};
    uint32_t flash_base_addr, sector_index, sector_base_addr;
    static uint32_t sector_size;
    QuadSPI_MEM_BLOCK_STRUCT mem_block;

    if (qspifd == NULL) {
        printf("ERROR: Empty QuadSPI file handler! Please check! \n");
        return -1;
    }

    if (sector_size == 0) {
        sector_size = memory_get_sector_size (qspifd);
        if (sector_size == 0)
            return -1;
    }

    result = ioctl(qspifd, QuadSPI_IOCTL_GET_MEM_BASE_ADDR, &flash_base_addr);
    if (result < 0) {
        printf("ERROR: failed to get flash memory mapped base address!\n");
        return result;
    }

    sector_index = (addr - flash_base_addr) / sector_size;
    sector_base_addr = flash_base_addr + sector_index * sector_size;
#ifdef QSPI_MEMORY_DEBUG
    printf("INFO: Sector %d: [0x%08x - 0x%08x] will be erased!\n", sector_index,
                    sector_base_addr, sector_base_addr+sector_size-1);
#endif

    /* Set write enable before sector erase */
    memory_set_write_en(qspifd, addr, TRUE);

    /* Send out erase command */
    qspi_build_flash_command(buffer,
                    QuadSPI_SECTOR_ERASE,
                    QuadSPI_SET_ADDR(QuadSPI_SINGLE_PAD),
                    QuadSPI_LOOKUP_STOP);

    result = fwrite(buffer, 1, 6 + QuadSPI_ADDR_BYTES, qspifd);
    if(result < 0) {
      /* Stop transfer */
        printf("ERROR (tx)\n");
        return result;
    }

    /* Wait till the flash is not busy at program */
    memory_wait_for_not_busy(qspifd);

    /* Invalid data in AHB or IP buffer. Write operation might change the data in AHB buffer.
     * Invalidate it to make sure the correction of following read operation */
    mem_block.ADDR = (void *) sector_base_addr;
    mem_block.SIZE = sector_size;

    result = ioctl(qspifd, QuadSPI_IOCTL_INVALID_BUFFER, &mem_block);
    if (result < 0) {
        printf ("ERROR: failed to invalid buffer!\n");
        return result;
    }

    return sector_base_addr;
}


/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_read_status1
* Comments  : This function reads memory status1 register. The value is set in status pointer.
* Return  :
*           MQX error code. <0 if failed.
*
*END*----------------------------------------------------------------------*/
int32_t memory_read_status1(MQX_FILE_PTR qspifd, uint8_t *status)
{
    _mqx_int result;
    uint8_t temp = *status;
    uint8_t buffer[6 + QuadSPI_ADDR_BYTES] = {0};

    if (qspifd == NULL) {
        printf("ERROR: Empty QuadSPI file handler! Please check! \n");
        return -1;
    }

    qspi_build_flash_command(buffer,
                    QuadSPI_READ_STATUS1,
                    QuadSPI_READ_DATA(QuadSPI_SINGLE_PAD, 1),
                    QuadSPI_LOOKUP_STOP);

    /* Write instruction */
    result = fwrite(buffer, 1, 6 + QuadSPI_ADDR_BYTES + 1, qspifd);
    if(result < 0) {
      /* Stop transfer */
        printf("ERROR (tx)\n");
        return result;
    }

    /* Read memory status: 1byte */
    result = fread (status, 1, 1, qspifd);
    if(result < 1) {
        printf ("memory_read_status1: ERROR (rx)\n");
        *status = temp;
        return result;
    } 

    return MQX_OK;
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_read_config1
* Comments  : This function reads memory configuration1 register. The value is set in conf pointer.
* Return  :
*           MQX error code. <0 if failed.
*
*END*----------------------------------------------------------------------*/
int32_t memory_read_config1(MQX_FILE_PTR qspifd, uint8_t *conf)
{
    _mqx_int result;
    uint8_t temp = *conf;
    uint8_t buffer[6 + QuadSPI_ADDR_BYTES] = {0};

    if (qspifd == NULL) {
        printf("ERROR: Empty QuadSPI file handler! Please check! \n");
        return -1;
    }

    qspi_build_flash_command(buffer,
                    QuadSPI_READ_CONFIG,
                    QuadSPI_READ_DATA(QuadSPI_SINGLE_PAD, 1),
                    QuadSPI_LOOKUP_STOP);

    /* Write instruction */
    result = fwrite(buffer, 1, 6 + QuadSPI_ADDR_BYTES + 1, qspifd);
    if(result < 0) {
      /* Stop transfer */
        printf("ERROR (tx)\n");
        return result;
    }

    /* Read memory status: 1byte */
    result = fread (conf, 1, 1, qspifd);
    if(result < 0) {
        printf ("memory_read_status1: ERROR (rx)\n");
        *conf = temp;
        return result;
    }

    return MQX_OK;
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_enable_quad_mode
* Comments  : This function enable the quad mode for external flash.
* Return:
*         MQX error code. < 0 if failed.
*
*END*----------------------------------------------------------------------*/
int32_t memory_enable_quad_mode(MQX_FILE_PTR qspifd, bool enable)
{
    uint8_t buffer[6+QuadSPI_ADDR_BYTES] = {0};
    uint32_t write_size = 2, addr, i;
    uint8_t write_data[2];
    QuadSPI_FLASH_INFO_STRUCT *flash_dev;
    _mqx_int result;

    if (qspifd == NULL) {
        printf("ERROR: Empty QuadSPI file handler! Please check! \n");
        return -1;
    }

    result = ioctl(qspifd, QuadSPI_IOCTL_GET_FLASH_INFO, &flash_dev);
    if (result < 0) {
        printf("ERROR: failed to get flash information!\n");
        return result;
    }

    for (i = 0; i < 4; i++) {
        result = ioctl(qspifd, QuadSPI_IOCTL_GET_MEM_BASE_ADDR, &addr);
        if (result < 0) {
            printf("ERROR: failed to get flash memory mapped base address!\n");
            return result;
        }
        /* No external flash on that port */
        if ((flash_dev->NUM_SECTOR * flash_dev->SECTOR_SIZE )== 0) {
            flash_dev++;
            continue;
        }

        /* Enable flash memory write */
        addr += flash_dev->START_ADDR;
        memory_set_write_en(qspifd, addr, TRUE);

        /* Send write register command */
        qspi_build_flash_command(buffer, QuadSPI_WRITE_REG, QuadSPI_WRITE_DATA(write_size), QuadSPI_LOOKUP_STOP);

        write_data[0] = 0000;
        if (enable)
            write_data[1] = 0002;
        else
            write_data[1] = 0000;
        addr_to_data_buf((uint32_t) write_data, &(buffer[6]));

        /* Write instruction */
        result = fwrite(buffer, 1, 6 + QuadSPI_ADDR_BYTES + write_size, qspifd);
        if (result < 0) {
            printf("ERROR\n");
            return result;
        }

        /* Wait till the flash is not busy  */
        memory_wait_for_not_busy(qspifd);

        /* Wait till the flash quad mode is set */
        if (enable ^ memory_check_quad_mode(qspifd)) {
            return result;
        }

        flash_dev++;
    }

    return MQX_OK;
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_set_io_mode
* Comments  : This function set external flash io mode according to QuadSPI status.
* Return:
*         MQX error code. <0 if failed.
*
*END*----------------------------------------------------------------------*/
int32_t memory_set_io_mode(MQX_FILE_PTR qspifd, QuadSPI_IO_MODE io_mode)
{
    int32_t result;

    if (qspifd == NULL) {
        printf("ERROR: Empty QuadSPI file handler! Please check! \n");
        return -1;
    }

    if (io_mode == QuadSPI_SINGLE_MODE) {
        result = memory_enable_quad_mode(qspifd, FALSE);
        if (result < 0) {
            printf("ERROR: failed to disable flash quad mode\n");
            return result;
        }

       ioctl(qspifd, QuadSPI_IOCTL_SET_SINGLE_IO, NULL);
    } else if (io_mode == QuadSPI_DUAL_MODE) {
        result = memory_enable_quad_mode(qspifd, FALSE);
        if (result < 0) {
            printf("ERROR: failed to disable flash quad mode\n");
            return result;
        }

        ioctl(qspifd, QuadSPI_IOCTL_SET_DUAL_IO, NULL);
    } else if (io_mode == QuadSPI_QUAD_MODE) {
        result = memory_enable_quad_mode(qspifd, TRUE);
        if (result < 0) {
            printf("ERROR: failed to enable flash quad mode\n");
            return result;
        }
        ioctl(qspifd, QuadSPI_IOCTL_SET_QUAD_IO, NULL);
    }

    return MQX_OK;
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_ip_read_byte_in_para
* Comments  : This function reads a data byte from memory via IP command
* Return:
*         The 0 if successful.
*
*END*----------------------------------------------------------------------*/
int32_t memory_ip_read_byte_in_para(MQX_FILE_PTR qspifd, uint32_t addr, uint8_t *out, bool para_enabled)
{
    uint8_t buffer[12 + QuadSPI_ADDR_BYTES] = {0};
    uint32_t clock_mode, io_mode;
    int32_t read_size = 1;
    int32_t result;

    if ((qspifd == NULL) || !out) {
        printf("ERROR: Empty QuadSPI file handler! Please check!\n");
        return -1;
    }

    /* Get as clock and IO mode */
    result = ioctl(qspifd, QuadSPI_IOCTL_GET_CLK_MODE, &clock_mode);
    if (result < 0) {
        printf ("ERROR: failed to get clock mode!\n");
        return -1;
    }

    result = ioctl(qspifd, QuadSPI_IOCTL_GET_IO_MODE, &io_mode);
    if (result < 0) {
        printf ("ERROR: failed to get IO mode!\n");
        return -1;
    }

    /* Build read byte command according to clock and IO mode */
    qspi_build_read_command(buffer, clock_mode, io_mode, read_size);

    /* Set flash current address */
    result = ioctl(qspifd, QuadSPI_IOCTL_SET_FLASH_ADDR, &addr);
    if (result != MQX_OK) {
        printf("%s: failed at ioctl set flash address!\n", __func__);
        return -1;
    }

    if (para_enabled)
    {
        if (addr & 0x1)
            read_size = 2;

        result = ioctl(qspifd, QuadSPI_IOCTL_SET_IP_PARA_MODE, &para_enabled);
        if (result != MQX_OK) {
            printf("%s: failed at ioctl set IP read in parallel mode!\n", __func__);
            return -1;
        }
    }

    /* Write instruction and address */
    result = fwrite(buffer, 1, 12 + QuadSPI_ADDR_BYTES + read_size, qspifd);
    if (result < 0) {
        /* Stop transfer */
        printf("ERROR (tx)\n");
        return -1;
    }

    /* Read data from memory */
    result = fread(out, 1, 1, qspifd);
    if (result < 0) {
        printf ("ERROR (rx)\n");
        return -1;
    }

    if (para_enabled)
    {
        para_enabled = FALSE;
        result = ioctl(qspifd, QuadSPI_IOCTL_SET_IP_PARA_MODE, &para_enabled);
        if (result != MQX_OK) {
            printf("%s: failed at ioctl set IP read in parallel mode!\n", __func__);
            return -1;
        }
    }

    return 0;
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_ip_read_byte
* Comments  : This function reads a data byte from memory via IP command
* Return:
*         The 0 if successful
*
*END*----------------------------------------------------------------------*/
int32_t memory_ip_read_byte(MQX_FILE_PTR qspifd, uint32_t addr, uint8_t *out)
{
    if (!out || memory_ip_read_byte_in_para(qspifd, addr, out, FALSE))
        return -1;
    return 0;
}


/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_read_byte
* Comments  : This function reads a data byte from memory
* Return:
*         The 0 if successful.
*
*END*----------------------------------------------------------------------*/
int32_t memory_read_byte(MQX_FILE_PTR qspifd, uint32_t addr, uint8_t *out)
{
    int32_t read_size = 1;
    int32_t result;

    if (!out || (qspifd == NULL)) {
        printf("ERROR: Empty QuadSPI file handler! Please check! \n");
        return -1;
    }


    /* Set flash current address */
    result = ioctl(qspifd, QuadSPI_IOCTL_SET_FLASH_ADDR, &addr);
    if (result != MQX_OK) {
        printf("%s: failed at ioctl set flash address!\n", __func__);
        return -1;
    }

    /* Read data from memory */
    result = fread(out, 1, read_size, qspifd);
    if (result < 0) {
        printf ("%s: ERROR (rx)\n", __func__);
        return -1;
    }

    return 0;
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_write_data
* Comments  : This function writes data buffer to memory using page write
* Return:
*         Number of bytes written. >=0 if successful, <0 if failed
*
*END*----------------------------------------------------------------------*/
int32_t memory_write_data (MQX_FILE_PTR qspifd, uint32_t addr, uint32_t size, uint8_t *data)
{
    _mqx_int result;
    uint32_t page_size = QuadSPI_MEMORY_PAGE_SIZE;
    uint32_t temp, start_addr, count = size, write_size;
    uint8_t buffer[8 + QuadSPI_ADDR_BYTES];
    uint8_t *src_ptr = (uint8_t *)data;
    QuadSPI_MEM_BLOCK_STRUCT mem_block;

    if (qspifd == NULL) {
        printf("ERROR: Empty QuadSPI file handler! Please check! \n");
        return -1;
    }

    /* save write address for invalid buffer purpose */
    start_addr = addr;
    while (count > 0) {
        temp = addr;
        write_size = (page_size - (temp % page_size));
        write_size = (write_size <= count)?write_size:count;

        memory_set_write_en(qspifd, addr, TRUE);

        /* Write instruction, address and data to buffer */
        qspi_build_flash_command(buffer,
                        QuadSPI_SET_PAGE_WR,
                        QuadSPI_SET_ADDR(QuadSPI_SINGLE_PAD),
                        QuadSPI_WRITE_DATA(page_size),
                        QuadSPI_LOOKUP_STOP);
        addr_to_data_buf((uint32_t) src_ptr, &(buffer[8]));

        result = fwrite (buffer, 1, 8 + QuadSPI_ADDR_BYTES + write_size, qspifd);
        if (result < 0) {
          printf ("ERROR\n");
          return result;
        }

        /* Wait till the flash is not busy at program */
        memory_wait_for_not_busy(qspifd);

        count -= write_size;
        addr += write_size;
        src_ptr += write_size;
    }

    /* Invalid data in AHB or IP buffer. Write operation might change the data in AHB buffer.
     * Invalidate it to make sure the correction of following read operation */
    mem_block.ADDR = (void *) start_addr;
    mem_block.SIZE = size;

    result = ioctl(qspifd, QuadSPI_IOCTL_INVALID_BUFFER, &mem_block);
    if (result < 0) {
        printf ("ERROR: failed to invalid buffer!\n");
        return result;
    }

    return size;
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_read_data
* Comments  : This function reads data from memory into given buffer
* Return:
*         Number of bytes read. >=0 if successful, <0 if failed
*
*END*----------------------------------------------------------------------*/
int32_t memory_read_data (MQX_FILE_PTR qspifd, uint32_t addr, uint32_t size, uint8_t *data)
{
    _mqx_int result;

    if (qspifd == NULL) {
        printf("ERROR: Empty QuadSPI file handler! Please check! \n");
        return -1;
    }

    /* Set flash address to read from */
    result = ioctl(qspifd, QuadSPI_IOCTL_SET_FLASH_ADDR, &addr);
    if (result != MQX_OK) {
        printf("memory_read_data: failed at ioctl set flash address!\n");
        return result;
    }

    /* Read data from memory */
    result = fread (data, 1, size, qspifd);
    if (result < 0) {
        printf ("ERROR (rx)\n");
        return result;
    }

    return size;
}


/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_get_start_addr
* Comments  : This function gets start address of memory
* Return:
*         start address of specific flash memory.
*
*END*----------------------------------------------------------------------*/
uint32_t memory_get_start_addr (MQX_FILE_PTR qspifd)
{
    uint32_t start_addr = 0;
    if (ioctl(qspifd, QuadSPI_IOCTL_GET_MEM_BASE_ADDR, &start_addr) != MQX_OK) {
        printf("%s: failed to get start address of flash!\n", __func__);
        return 0xFFFFFFFF;
    }
    return start_addr;
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : memory_get_total_size
* Comments  : This function gets total size of memory in bytes
* Return:
*         total size in bytes of specific flash memory.
*
*END*----------------------------------------------------------------------*/
int32_t memory_get_total_size (MQX_FILE_PTR qspifd)
{
    int32_t total_len = 0;
    if (ioctl(qspifd, QuadSPI_IOCTL_GET_MEM_TOTAL_LEN, &total_len) != MQX_OK) {
        printf("%s: failed to get total length of flash!\n", __func__);
        return -1;
    }
    return total_len;
}

