/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 1989-2008 ARC International
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
*   This file contains the source for the FlashX example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <string.h>

#if (BSP_M51EMDEMO || BSP_TWRMCF51MM || BSP_TWRMCF51JE)
    #if !BSPCFG_ENABLE_FLASHX2
    #error This application requires BSPCFG_ENABLE_FLASHX1 defined non-zero in user_config.h. Please recompile BSP with this option.
    #endif
    #define         FLASH_NAME "flashx2:bank1"
#elif BSP_M54455EVB
    #if !BSPCFG_ENABLE_FLASHX0
    #error This application requires BSPCFG_ENABLE_FLASHX0 defined non-zero in user_config.h. Please recompile BSP with this option.
    #endif
    #define         FLASH_NAME "flashx0:bank0"
#elif PSP_MQX_CPU_IS_VYBRID
    #if BSPCFG_ENABLE_FLASHX_QUADSPI0
        #define         FLASH_NAME "flashx_qspi0:bank0"
    #elif BSPCFG_ENABLE_FLASHX_QUADSPI1
        #define         FLASH_NAME "flashx_qspi1:bank0"
    #else
    #error This application requires BSPCFG_ENABLE_FLASHX_QUADSPI0/1 defined non-zero in user_config.h. Please recompile BSP with this option.
    #endif
#else
    #if !BSPCFG_ENABLE_FLASHX
    #error This application requires BSPCFG_ENABLE_FLASHX defined non-zero in user_config.h. Please recompile BSP with this option.
    #endif
    #define         FLASH_NAME "flashx:bank0"
#endif

#define STRING_SIZE     (32)
#define BUFFER_SIZE     (80)

/* function prototypes */
void flash_task(uint32_t);

static char *buffer;

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   /* Task Index,   Function,          Stack,  Priority,   Name,           Attributes,          Param, Time Slice */
    {          1,   flash_task,   2000,   7,          "flash_test",   MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};

/*TASK*-----------------------------------------------------
*
* Task Name    : flash_task
* Comments     :
*    This task finds a string at 'the end' of flash memory
*    then it tries to rewrite it. The successfull rewrite
*    can be proved by re-executing this example application.
*
*END*-----------------------------------------------------*/
void flash_task
   (
      uint32_t initial_data
   )
{
    (void)          initial_data; /* suppress 'unused variable' warning */
    MQX_FILE_PTR    flash_file;
    _mqx_int        i;
    _mqx_int        len = 0;
    uint32_t        ioctl_param;

    _int_install_unexpected_isr();

    printf("\n\nMQX Flash Example");

    buffer = (char *)_mem_alloc_zero(BUFFER_SIZE);
    if (buffer == NULL) {
        printf("\nCan't allocate buffer");
        _task_block();
    }

    /* Open the flash device */
    flash_file = fopen(FLASH_NAME, NULL);
    if (flash_file == NULL) {
        printf("\nUnable to open file %s", FLASH_NAME);
        _task_block();
    } else {
        printf("\nFlash file %s opened", FLASH_NAME);
    }

    /* Get the size of the flash file */
    fseek(flash_file, 0, IO_SEEK_END);
    printf("\nSize of the flash file: 0x%x Bytes", ftell(flash_file));

    /* Disable sector cache */
    ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
    printf("\nFlash sector cache enabled.");

    /* Move STRING_SIZE Bytes back */
    printf("\nReading last %d Bytes.", STRING_SIZE);
    fseek(flash_file, -STRING_SIZE, IO_SEEK_END);

    len = read(flash_file, buffer, STRING_SIZE);
    if (STRING_SIZE != len) {
        printf("\nERROR! Could not read from flash. Exiting...");
        goto example_end;
    }
    for (i = 0; i < STRING_SIZE; i++) {
        if (buffer[i] != (char)0xFF)
        break;
    }
    if (i == STRING_SIZE) {
        printf("\nBytes are blank.");
    }
    else {
        while ((buffer[i] != 0) && (i < STRING_SIZE)) {
            i++;
        }

        if (i == STRING_SIZE) {
            printf("\nFound non-blank data, but not zero-ended string.");
        }
        else {
            printf("\nString found: %s", buffer);
        }
    }

    do {
        printf("\nType a string to be written to the end of file (%d chars max.):", STRING_SIZE-1);
        fgets(buffer, BUFFER_SIZE, stdin);
        len = (_mqx_int)strlen(buffer);
    } while (!len || len > STRING_SIZE-1);

    /* Move STRING_SIZE Bytes back */
    fseek(flash_file, -STRING_SIZE, IO_SEEK_END);

    /* Unprotecting the the FLASH might be required */
    ioctl_param = 0;
    ioctl(flash_file, FLASH_IOCTL_WRITE_PROTECT, &ioctl_param);

    len = write(flash_file, buffer, STRING_SIZE);
    if (len != STRING_SIZE) {
        printf("\nError writing to the file. Error code: %d", _io_ferror(flash_file));
    }
    else {
        printf("\nData written to the flash.\nNow you can power down and power up your device");
        printf("\nand then retry the test to see if the string was written correctly.");
    }

    example_end:
    fclose(flash_file);
    printf("\nFlash example finished.");

    _task_block();
}

/* EOF */
