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
*   Different write speed is specified to test the clock managment APIs, which are
*   called in QSPI driver
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

#if ! BSPCFG_ENABLE_CLKTREE_MGMT
#error This application requires BSPCFG_ENABLE_CLKTREE_MGMT defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#ifndef BSP_QUADSPI_MEMORY_CHANNEL
#error This application requires BSP_QUADSPI_MEMORY_CHANNEL or BSP_QUADSPI_MEMORY_CHANNEL to be defined. Please set it to appropriate QSPI channel number in user_config.h and recompile BSP with this option.
#endif

#if BSPCFG_ENABLE_FLASHX
#error This application requires BSPCFG_ENABLE_FLASHX to be defined zero in user_config.h
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

extern void main_task (uint32_t);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index,   Function,   Stack,  Priority,   Name,   Attributes,          Param, Time Slice */
    { 10L,          main_task,  1500L,  8L,         "Main", MQX_AUTO_START_TASK, 0,     0  },
    { 0 }
};

int32_t cmd_help(int32_t argc, char *argv[]);

void quadspi_valid_AHB_read(MQX_FILE_PTR qspifd, uint32_t clock, uint8_t * data);
void program_data(MQX_FILE_PTR qspifd, uint8_t * data);

//#define CHECK_DATA

/*TASK*-------------------------------------------------------------------
*
* Task Name : main_task
* Comments  : 
*     33MHz / 40MHz / 66MHz / 80MHz speed will specified for QSPI read. The
*     speed is set by "_quadspi_flash_set_serial_clk" call, this function
*     has a different version if "BSPCFG_ENABLE_CLKTREE_MGMT" is defined,
*     witch will use the clock management APIs, including "clock_get"
*     "clock_get_freq" "clock_set_parent" "clock_set_freq", to search for
*     a best matching parent, set QSPI's parent to it and then set proper
*     divider to achieve the most close read frequency. It is a real-world
*     example of how CLKTREE APIs are used.
*
*END*----------------------------------------------------------------------*/
void main_task
   (
      uint32_t dummy
   )
{
    MQX_FILE_PTR           qspifd;
    int32_t ret = 0;
    uint8_t *data;

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

    // printf("Chip Erase\n");
    // memory_chip_erase(qspifd);


    printf("\n-------- Read at 33MHz --------\n");
    quadspi_valid_AHB_read(qspifd, 33000000, data);

    printf("\n-------- Read at 40MHz --------\n");
    quadspi_valid_AHB_read(qspifd, 40000000, data);

    printf("\n-------- Read at 66MHz --------\n");
    quadspi_valid_AHB_read(qspifd, 66000000, data);

    printf("\n-------- Read at 80MHz --------\n");
    quadspi_valid_AHB_read(qspifd, 80000000, data);

    /* Close the SPI */
    _mem_free(data);
    ret = (uint32_t)fclose (qspifd);
    if (ret) {
        printf ("Error closing QSPI, returned: 0x%08x\n", ret);
    }

    printf ("\n-------------- End of example --------------\n\n");

}


bool validata_data(uint8_t * data, int num)
{
    int i;

    for (i = 0; i < TEST_BUF_SIZE1; i++) {
        if ((num < 128*2*16 && data[i] != (num +i) % 256) ||
            (num >= 128*2*16 && data[i] != ((uint8_t)(128 * 2*16+255-num+i) % 256))) {
#if (MQX_CPU == PSP_CPU_VF65GS10_A5)
            printf("\nA5: ");
#else
            printf("\nM4: ");
#endif
            printf("Failed at 0x%08x read value %d, expected %d \n", i+FLASH_BASE_ADR + num * TEST_BUF_SIZE1,
                    data[i], (num < 128*2*16) ? (num +i) % 256 : ((uint8_t)(128 * 2*16+255-num+i) % 256));
            return FALSE;
        }
    }

    return TRUE;
}

void quadspi_valid_AHB_read(MQX_FILE_PTR qspifd, uint32_t clock, uint8_t * data)
{
//    int read_sectors = 128 * 4;
    int read_sectors = 128 * 2 * 16, cur_sector;

#ifdef CHECK_DATA
    int repete_num = 2;
#else
    int repete_num = 20;
#endif
    int i, j;
    TIME_STRUCT start_time, end_time, diff_time;

    /* set read clock */
    if (ioctl(qspifd, QuadSPI_IOCTL_SET_READ_SPEED, &clock) != MQX_OK) {
        printf("ERROR: failed to set read clock!\n");
        return;
    }

    printf("Read Data...\n");
    for (i = 0; i < TEST_BUF_SIZE1; i++) {
        data[i] = 255;
    }

    _time_get(&start_time);

    for (j = 0; j < repete_num; j++) {
        for (i = 0; i < read_sectors; i++) {
#if (MQX_CPU == PSP_CPU_VF65GS10_A5)
            cur_sector = i;
#else
            cur_sector = i+read_sectors;
#endif
            memory_read_data (qspifd, FLASH_BASE_ADR + cur_sector * TEST_BUF_SIZE1, TEST_BUF_SIZE1, data);
#ifdef CHECK_DATA
            if (validata_data(data, cur_sector) == FALSE) {
#if (MQX_CPU == PSP_CPU_VF65GS10_A5)
                printf("A5 Validation Failed at %d MHz: read at 0x%08x !\n", clock/1000000, FLASH_BASE_ADR + cur_sector * 256);
#else
                printf("M4 Validation Failed at %d MHz: read at 0x%08x !\n", clock/1000000, FLASH_BASE_ADR + cur_sector * 256);
#endif
                return;
            }
#endif
        }
    }

    _time_get(&end_time);
    _time_diff(&start_time, &end_time, &diff_time);
#ifndef CHECK_DATA
#if (MQX_CPU == PSP_CPU_VF65GS10_A5)
    printf("\nA5: ");
#else
    printf("\nM4: ");
#endif
    printf("clock %d MHz, data read rate = %ld kbps\n", clock/1000000,
        (TEST_BUF_SIZE1 * read_sectors *repete_num)/(diff_time.SECONDS * 1000 + diff_time.MILLISECONDS));
#endif
}

void program_data(MQX_FILE_PTR qspifd, uint8_t * data)
{
    TIME_STRUCT start_time, end_time, diff_time;
    int i, byte_write, j;

    /* erase all */
    printf("\n\n*****************************************\n");
    printf("Erase all flash, this might take 30s....\n");
    printf("*****************************************\n");
    _time_get(&start_time);

    memory_chip_erase(qspifd);

    _time_get(&end_time);
    _time_diff(&start_time, &end_time, &diff_time);
    printf("\nErase whole flash %ld sec, %ld millisec\n", diff_time.SECONDS, diff_time.MILLISECONDS);
    printf("Finish erase all flash\n");

    printf("\n\n*****************************************\n");
    printf("*** Function Test <memory_write_data> ***\n");
    printf("*****************************************\n");
    _time_get(&start_time);

    for (i = 0; i < 128 * 4 * 16; i++) {
        if (i < 128 * 2 * 16) {
            for (j = 0; j < TEST_BUF_SIZE1; j++) {
                data[j] = (i + j) % 256;
            }
        } else {
            for (j = 0; j < TEST_BUF_SIZE1; j++) {
                data[j] = (128 * 2 * 16 + 255 - i + j ) % 256;
            }
        }
        byte_write = memory_write_data (qspifd, FLASH_BASE_ADR+TEST_BUF_SIZE1*i, TEST_BUF_SIZE1, data);
    }

    _time_get(&end_time);
    _time_diff(&start_time, &end_time, &diff_time);
    printf("\ndata = %d, Time spends on Flash write is %ld sec, %ld millisec, rate = %ld kbps\n",
        byte_write, diff_time.SECONDS, diff_time.MILLISECONDS,
        (byte_write) / (diff_time.SECONDS * 1000 + diff_time.MILLISECONDS));
}
