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
*   This file contains the source for a simple example of an
*   application that writes and reads the QuadSPI memory using the QuadSPI driver.
*   It's already configured for onboard QSPI flash where available.
*
*
*END************************************************************************/


#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <qspi.h>
#include "qspi_memory.h"


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#ifndef BSP_QUADSPI_MEMORY_CHANNEL
#error This application requires BSP_QUADSPI_MEMORY_CHANNEL or BSP_QUADSPI_MEMORY_CHANNEL to be defined. Please set it to appropriate QSPI channel number in user_config.h and recompile BSP with this option.
#endif

#if BSPCFG_ENABLE_FLASHX_QUADSPI0 || BSPCFG_ENABLE_FLASHX_QUADSPI1
#error This application requires BSPCFG_ENABLE_FLASHX_QUADSPI0/1 to be defined zero in user_config.h
#endif

#if BSP_QUADSPI_MEMORY_CHANNEL == 0
  #if ! BSPCFG_ENABLE_QUADSPI0
    #error This application requires BSPCFG_ENABLE_QUADSPI0 defined non-zero in user_config.h. Please recompile kernel with this option.
  #else
    #define TEST_CHANNEL "qspi0:"
  #endif

#elif BSP_QUADSPI_MEMORY_CHANNEL == 1
  #if ! BSPCFG_ENABLE_QUADSPI1
    #error This application requires BSPCFG_ENABLE_QUADSPI1 defined non-zero in user_config.h. Please recompile kernel with this option.
  #else
    #define TEST_CHANNEL "qspi1:"
  #endif
#endif

typedef struct
{
    uint32_t offset;
    uint32_t length;
} test_addr_t;

const static uint32_t g_qspi_test_pattern_a = 0xAB, g_qspi_test_pattern_b = 0xCD;
static bool g_qspi_test_para_enabled = FALSE;
static int32_t g_qspi_test_total_size;
static uint32_t g_qspi_test_sector_size;

extern void main_task (uint32_t);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index,   Function,   Stack,  Priority,   Name,   Attributes,          Param, Time Slice */
    { 10L,          main_task,  1500L,  8L,         "Main", MQX_AUTO_START_TASK, 0,     0  },
    { 0 }
};

int32_t cmd_help(int32_t argc, char *argv[]);

static uint32_t rw_compare_test(MQX_FILE_PTR qspifd, uint32_t flash_base, uint32_t offset, uint32_t test_len)
{
    uint32_t sector_base, test_addr, i, ret = 1;
    int32_t byte_write, byte_read;
    uint8_t *data = NULL, *expected_data = NULL, *data_read = NULL;

    test_addr = flash_base + offset;

    data = (uint8_t *)_mem_alloc_zero(TEST_BUF_SIZE1);
    if (data == NULL) {
        printf("fail to allocate write buffer\n");
        goto out;
    }

    data_read = (uint8_t *)_mem_alloc_zero(test_len);
    if (data_read == NULL) {
        printf("fail to allocate write buffer\n");
        goto out;
    }

    printf("\n\n*****************************************\n");
    printf("**** Compare Test <memory_read_data> ****\n");
    printf("*****************************************\n");

    //get sector base
    sector_base = memory_sector_erase(qspifd, test_addr);

    if ((test_addr + test_len - sector_base) / g_qspi_test_sector_size)
        printf("crossing sectors!\n");
    for (i = 0; i <= (test_addr + test_len - sector_base) / g_qspi_test_sector_size; i++)
    {
        memory_sector_erase(qspifd, test_addr + i * test_len);
        if (g_qspi_test_para_enabled)
            /* erase the sector in pair */
            memory_sector_erase(qspifd, test_addr + i * test_len + g_qspi_test_total_size / 2);
    }

    memset(data, g_qspi_test_pattern_a, test_len);

    printf("Write 0x%X to 0x%08X, len: 0x%X\n", g_qspi_test_pattern_a, test_addr, test_len);
    byte_write = memory_write_data (qspifd, test_addr, test_len, data);
    if (byte_write < 0) {
        printf("memory_write_data failed!\n");
        goto out;
    }

    if (g_qspi_test_para_enabled)
    {
        printf("Parallel mode is enabled, prepare the data\n");

        memset(data, g_qspi_test_pattern_b, test_len);
        test_addr = flash_base + g_qspi_test_total_size / 2 + offset;
        printf("Write 0x%X to 0x%08X, len: 0x%X\n", g_qspi_test_pattern_b, test_addr, test_len);
        byte_write = memory_write_data (qspifd, test_addr, test_len, data);
        if (byte_write < 0) {
            printf("memory_write_data failed!\n");
            goto out;
        }
    }

    if (g_qspi_test_para_enabled)
        test_addr = flash_base + offset * 2;
    else
        test_addr = test_addr;

    printf("Read from 0x%08X, len: 0x%X\n", test_addr, test_len);
    byte_read = memory_read_data(qspifd, test_addr, test_len, data_read);
    if (byte_read < 0) {
        printf("memory_read_data failed!\n");
        goto out;
    }

    if (g_qspi_test_para_enabled)
    {
        expected_data = (uint8_t *)_mem_alloc_zero(test_len);
        if (!expected_data) {
            printf("fail to allocate write buffer\n");
            goto out;
        }

        for (i = 0; i < test_len; i++)
        {
            if (i % 2)
                expected_data[i] = (g_qspi_test_pattern_a & 0x0F) << 4 | (g_qspi_test_pattern_b & 0x0F);
            else
                expected_data[i] = (g_qspi_test_pattern_a & 0xF0) | (g_qspi_test_pattern_b & 0xF0) >> 4;
        }
    }
    else
    {
        expected_data = data;
    }

    for (i = 0; i < test_len; i++)
    {
        if (data_read[i] != expected_data[i])
        {
            printf("data read @ 0x%08X: 0x%X does not match the expected: 0x%X\n", test_addr + i, data_read[i], expected_data[i]);
            goto out;
        }
    }

    if (g_qspi_test_para_enabled)
    {
        printf("\n\n*****************************************\n");
        printf("**** Compare Test <memory_read_byte> ****\n");
        printf("*****************************************\n");
        for (i = 0; i < test_len; i++) {
            if (i % 2)
                *expected_data = (g_qspi_test_pattern_a & 0x0F) << 4 | (g_qspi_test_pattern_b & 0x0F);
            else
                *expected_data = (g_qspi_test_pattern_a & 0xF0) | (g_qspi_test_pattern_b & 0xF0) >> 4;
            if (memory_read_byte(qspifd, test_addr + i, data_read))
            {
                printf("data read byte @ 0x%08X failed\n", test_addr + i);
                goto out;
            }
            if (*expected_data != *data_read)
            {
                printf("data read byte @ 0x%08X: 0x%X does not match the expected: 0x%X\n", test_addr + i, *data_read, *expected_data);
                goto out;
            }
            if (memory_ip_read_byte_in_para(qspifd, test_addr + i, data_read, g_qspi_test_pattern_b))
            {
                printf("data read byte in parallel mode @ 0x%08X failed\n", test_addr + i);
                goto out;
            }
            if (*expected_data != *data_read)
            {
                printf("data read byte in parallel mode @ 0x%08X: 0x%X does not match the expected: 0x%X\n", test_addr + i, *data_read, *expected_data);
                goto out;
            }
        }
    }
    ret = 0;
out:
    if (data)
        _mem_free(data);
    if (data_read)
        _mem_free(data_read);
    if (expected_data)
        _mem_free(expected_data);
    return ret;
}

/*TASK*-------------------------------------------------------------------
*
* Task Name : main_task
* Comments  :
*
*END*----------------------------------------------------------------------*/
void main_task
   (
      uint32_t dummy
   )
{
    MQX_FILE_PTR           qspifd;
    int32_t ret = 0, i, byte_write, byte_read;
    uint8_t *data = NULL, out;
    TIME_STRUCT start_time, end_time, diff_time;
    uint32_t sector_base, read_size, test_length;
    uint32_t flash_test_addr, flash_base_addr;
    uint32_t offset;

    // hard coded test data, for sector size 0x10000 flashes only
    test_addr_t rw_test_addrs[] = {
        {0x2000, 0x1000}, // all aligned
        {0x2001, 0x1000}, // unaligned start address
        {0x2000, 0x1001}, // unaligned end address
        {0x2001, 0x1001}, // unaligned start and end addresses
        {0x2000, 0x2},    // unaligned end address
        {0x2000, 0x3},    // unaligned end address
        {0xF000, 0x1000}, // cross two sectors
        {0xF001, 0x1000}, // cross two sectors, unaligned start address
        {0xF000, 0x1001}, // cross two sectors, unaligned end address
        {0xF001, 0x1001}, // cross two sectors, unaligned start and end address
        {0xFFFF, 0x2},    //unaligned end address
        {0xFFFF, 0x3},    // unaligned end address
    };

    printf ("\n-------------- QSPI driver example --------------\n\n");
    printf ("This example application demonstrates usage of QSPI driver.\n");

    /* Open the QSPI driver */
    qspifd = fopen (TEST_CHANNEL, NULL);
    if (qspifd == NULL) {
        printf ("Error opening QSPI driver!\n");
        _time_delay (200L);
        _task_block ();
    }

    data = (uint8_t *)_mem_alloc_zero(TEST_BUF_SIZE1);
    if (data == NULL) {
        printf("fail to allocate write buffer\n");
        fclose(qspifd);
        return;
    }

    /* Set the qspi memory mapped address */
    /* Get flash base address */
    ret = ioctl(qspifd, QuadSPI_IOCTL_GET_MEM_BASE_ADDR, &flash_base_addr);
    if (ret != MQX_OK) {
        printf("ERROR: failed to get flash memory mapped base address!\n");
        goto QSPI_EXIT;
    }

    /* Get QSPI parallel mode */
    ret = ioctl(qspifd, QuadSPI_IOCTL_GET_PARA_MODE, &g_qspi_test_para_enabled);
    if (ret != MQX_OK) {
        printf("ERROR: failed to get QSPI parallel mode!\n");
        goto QSPI_EXIT;
    }

    g_qspi_test_total_size = memory_get_total_size(qspifd);
    if (g_qspi_test_total_size < 0) {
        printf("ERROR: failed to get the total size of flash memory!\n");
        goto QSPI_EXIT;
    }

    g_qspi_test_sector_size = memory_get_sector_size(qspifd);

    /* Set some random place to show read and write functionality */
    offset = 0x8001;
    flash_test_addr = flash_base_addr + offset;

    /* erase all */
    printf("\n\n************************************************************************\n");
    printf("Erase the first flash chip, for S25FL128S/256S, it might take 30s/60s....\n");
    printf("************************************************************************\n");
    _time_get(&start_time);

    memory_chip_erase(qspifd);

    _time_get(&end_time);
    _time_diff(&start_time, &end_time, &diff_time);
    printf("\nErase whole flash %ld sec, %ld millisec\n", diff_time.SECONDS, diff_time.MILLISECONDS);
    printf("Finish erase all flash\n");

    printf("\n\n*****************************************\n");
    printf("*** Function Test <memory_read_data> ****\n");
    printf("*****************************************\n");
    printf("From Flash %08x: first 20 btyes\n", flash_test_addr);
    byte_read = memory_read_data(qspifd, flash_test_addr, 20, data);
    if (byte_read < 0) {
        printf("memory_read_data failed!\n");
        goto QSPI_EXIT;
    }

    for (i = 0; i < 20; i++) {
        printf("0x%02x ", data[i]);
    }

    printf("\n\n*****************************************\n");
    printf("*** Function Test <memory_ip_read_byte> ****\n");
    printf("*****************************************\n");
    printf("From Flash %08x: first 20 bytes\n", flash_test_addr);
    for (i = 0; i < 20; i++) {
        if (memory_ip_read_byte(qspifd, flash_test_addr + i, &out))
        {
            printf("memory_read_byte failed!\n");
            goto QSPI_EXIT;
        }
        printf("0x%02x ", out);
    }
    printf("\n");

    printf("\n\n*****************************************\n");
    printf("*** Function Test <memory_write_data> ***\n");
    printf("*****************************************\n");
    for (i = 0; i < TEST_BUF_SIZE1; i++) {
        data[i] = i % 256;
    }

    _time_get(&start_time);

    byte_write = memory_write_data (qspifd, flash_test_addr, TEST_BUF_SIZE1, data);
    if (byte_write < 0) {
        printf("memory_write_data failed!\n");
        goto QSPI_EXIT;
    }

    _time_get(&end_time);
    _time_diff(&start_time, &end_time, &diff_time);
    printf("\ndata = %d, Time spends on Flash write is %ld sec, %ld millisec, rate = %ld kbps\n",
        byte_write, diff_time.SECONDS, diff_time.MILLISECONDS,
        (byte_write)/(diff_time.SECONDS * 1000 + diff_time.MILLISECONDS));

    printf("\n\n*****************************************\n");
    printf("***** Time Test <memory_read_data> ******\n");
    printf("*****************************************\n");
    for (i = 0; i < TEST_BUF_SIZE1; i++) {
        data[i] = 255;
    }

    _time_get(&start_time);

    byte_read = memory_read_data (qspifd, flash_test_addr, TEST_BUF_SIZE1, data);
    if (byte_read < 0) {
        printf("memory_read_data failed!\n");
        goto QSPI_EXIT;;
    }

    _time_get(&end_time);
    _time_diff(&start_time, &end_time, &diff_time);
    printf("\ndata = %d, Time spends on Flash read is %ld sec, %ld millisec, rate = %ld kbps\n\n",
        byte_write, diff_time.SECONDS, diff_time.MILLISECONDS,
        (byte_write)/(diff_time.SECONDS * 1000 + diff_time.MILLISECONDS));

    printf("memory_read_data read data from %08x: first 20 bytes \n", flash_test_addr);
    for(i = 0; i < 20; i++) {
        printf ("0x%02x ", data[i]);
    }
    printf("\n");

    printf("\n\n*****************************************\n");
    printf("**** Compare Test <memory_ip_read_byte> ****\n");
    printf("*****************************************\n");
    printf("memory_ip_read_byte from %08x: first 20 bytes \n", flash_test_addr);
    for (i = 0; i < 20; i++) {
        if (memory_ip_read_byte(qspifd, flash_test_addr + i, &out))
        {
            printf("memory_ip_read_byte failed!\n");
            goto QSPI_EXIT;
        }
        printf("0x%02x ", out);
    }
    printf("\n");

    printf("\n\n*****************************************\n");
    printf("***** Function Test <memory_sector_erase> ******\n");
    printf("*****************************************\n");
    sector_base = memory_sector_erase(qspifd, flash_test_addr);
    test_length = g_qspi_test_sector_size;

    printf("Sector erase 0x%08x. Sector [0x%08x - 0x%08x] erase is ", flash_test_addr, sector_base, sector_base + test_length - 1);

    /* check if the sector is erased */
    while (test_length > 0) {
        read_size = (test_length > TEST_BUF_SIZE2) ? TEST_BUF_SIZE2: test_length;

        for (i = 0; i < read_size; i++) {
            data[i] = 0;
        }

        byte_read = memory_read_data (qspifd, flash_test_addr, read_size, data);
        if (byte_read < 0) {
            printf("failed!\n");
            goto QSPI_EXIT;
        }

        for (i = 0; i < read_size; i++) {
            if (data[i] != 0xFF) {
                printf("failed!\n");
                goto QSPI_EXIT;
            }
        }

        test_length -= read_size;
    }
    printf("successful!\n");

    for (i = 0; i < sizeof(rw_test_addrs) / sizeof(test_addr_t); i++)
    {
        if (rw_compare_test(qspifd, flash_base_addr, rw_test_addrs[i].offset, rw_test_addrs[i].length))
        {
            printf("Test RW comparison failed at 0x%08X, 0x%X!\n", flash_base_addr + rw_test_addrs[i].offset, rw_test_addrs[i].length);
            goto QSPI_EXIT;
        }
    }

    if (g_qspi_test_para_enabled)
        offset = g_qspi_test_total_size / 2 - 0x2;
    else
        offset = g_qspi_test_total_size - 0x2;

    if (rw_compare_test(qspifd, flash_base_addr, offset, 1))
    {
        printf("Test RW comparison failed at 0x%08X, 0x%X!\n", flash_base_addr + offset, 1);
        goto QSPI_EXIT;
    }

    printf("All tests are passed!\n");

QSPI_EXIT:
    /* Close the SPI */
    if (data)
        _mem_free(data);

    ret = (uint32_t)fclose (qspifd);
    if (ret) {
        printf ("Error closing QSPI, returned: 0x%08x\n", ret);
    }

    printf ("\n-------------- End of example --------------\n\n");

}
