/*HEADER**********************************************************************
*
* Copyright 2012 Freescale Semiconductor, Inc.
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
*   application that writes and reads the SPI memory using the SPI driver.
*   It's already configured for onboard SPI flash where available.
*
*
*END************************************************************************/


#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <spi.h>
#include "spi_memory.h"


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


#ifndef BSP_SPI_MEMORY_CHANNEL
#error This application requires BSP_SPI_MEMORY_CHANNEL to be defined. Please set it to appropriate SPI channel number in user_config.h and recompile BSP with this option.
#endif


#if BSP_SPI_MEMORY_CHANNEL == 0

    #if ! BSPCFG_ENABLE_SPI0
        #error This application requires BSPCFG_ENABLE_SPI0 defined non-zero in user_config.h. Please recompile kernel with this option.
    #else
        #define TEST_CHANNEL "spi0:"
    #endif

#elif BSP_SPI_MEMORY_CHANNEL == 1

    #if ! BSPCFG_ENABLE_SPI1
        #error This application requires BSPCFG_ENABLE_SPI1 defined non-zero in user_config.h. Please recompile kernel with this option.
    #else
        #define TEST_CHANNEL "spi1:"
    #endif

#elif BSP_SPI_MEMORY_CHANNEL == 2

    #if ! BSPCFG_ENABLE_SPI2
        #error This application requires BSPCFG_ENABLE_SPI2 defined non-zero in user_config.h. Please recompile kernel with this option.
    #else
        #define TEST_CHANNEL  "spi2:"
    #endif

#else

     #error Unsupported SPI channel number. Please check settings of BSP_SPI_MEMORY_CHANNEL in BSP.

#endif


/* The SPI serial memory test addresses */
#define SPI_MEMORY_ADDR1               0x0000F0 /* test address 1 */
#define SPI_MEMORY_ADDR2               0x0001F0 /* test address 2 */
#define SPI_MEMORY_ADDR3               0x0002F0 /* test address 3 */

/* Test strings */
#define TEST_BYTE_1       0xBA
#define TEST_BYTE_2       0xA5
#define TEST_STRING_SHORT "Hello,World!"
#define TEST_STRING_LONG  "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz1234567890"


unsigned char send_buffer[SPI_MEMORY_PAGE_SIZE];
unsigned char recv_buffer[sizeof(TEST_STRING_LONG)];


const char *device_mode[] =
{
    "SPI_DEVICE_MASTER_MODE",
    "SPI_DEVICE_SLAVE_MODE",
};

const char *clock_mode[] =
{
    "SPI_CLK_POL_PHA_MODE0",
    "SPI_CLK_POL_PHA_MODE1",
    "SPI_CLK_POL_PHA_MODE2",
    "SPI_CLK_POL_PHA_MODE3"
};


extern void main_task (uint32_t);


const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index,   Function,   Stack,  Priority,   Name,   Attributes,          Param, Time Slice */
    { 10L,          main_task,  1500L,  8L,         "Main", MQX_AUTO_START_TASK, 0,     0  },
    { 0 }
};


/* DEBUG */
#define SPI_DEBUG_STATUS            1
#define SPI_DEBUG_BYTE              1
#define SPI_DEBUG_WRITE_DATA_SHORT  1
#define SPI_DEBUG_WRITE_DATA_LONG   1


#ifdef BSP_SPI_MEMORY_GPIO_CS

/*FUNCTION*---------------------------------------------------------------
*
* Function Name : set_CS
* Comments  : This function sets chip select signal to enable/disable memory.
*             It's used only on platforms with manual CS handling.
*END*----------------------------------------------------------------------*/


_mqx_int set_CS (uint32_t cs_mask, void *user_data)
{
    LWGPIO_STRUCT_PTR spigpio = (LWGPIO_STRUCT_PTR)user_data;

    if (cs_mask & BSP_SPI_MEMORY_SPI_CS)
    {
        lwgpio_set_value(spigpio, LWGPIO_VALUE_LOW);
    }
    else
    {
        lwgpio_set_value(spigpio, LWGPIO_VALUE_HIGH);
    }
    return MQX_OK;
}
#endif /* BSP_SPI_MEMORY_GPIO_CS */

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
    MQX_FILE_PTR           spifd;
    uint32_t                param, result, i = 0;
    SPI_STATISTICS_STRUCT  stats;
    SPI_READ_WRITE_STRUCT  rw;

#ifdef BSP_SPI_MEMORY_GPIO_CS
    LWGPIO_STRUCT          spigpio;
    SPI_CS_CALLBACK_STRUCT callback;
#endif

#ifdef BSP_M51EMDEMO
/*
**  The M51DEMOEM board has PTA6 & PTA7 pins dedicated for
**  controling ~WP and ~HOLD signals on MRAM memory.
**  For the purpose of the demo this pins are set to LOG1.
*/
    LWGPIO_STRUCT          lwgpio_wphold;

    /* Open GPIO file containing SPI pin SS == chip select for memory */
    if (!lwgpio_init(&lwgpio_wphold, (LWGPIO_PORT_A | LWGPIO_PIN6), LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing GPIO with associated pins failed.\n");
        _time_delay (200L);
        _task_block();
    }
    lwgpio_set_functionality(&lwgpio_wphold, BSP_SPI_MUX_GPIO);/*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
    lwgpio_set_value(&lwgpio_wphold, LWGPIO_VALUE_HIGH);

    if (!lwgpio_init(&lwgpio_wphold, (LWGPIO_PORT_A | LWGPIO_PIN7), LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing GPIO with associated pins failed.\n");
        _time_delay (200L);
        _task_block();
    }
    lwgpio_set_functionality(&lwgpio_wphold, BSP_SPI_MUX_GPIO);/*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/
    lwgpio_set_value(&lwgpio_wphold, LWGPIO_VALUE_HIGH);
#endif


    printf ("\n-------------- SPI driver example --------------\n\n");

    printf ("This example application demonstrates usage of SPI driver.\n");
    printf ("It transfers data to/from external memory over SPI bus.\n");
    printf ("The default settings in memory.h apply to TWR-MEM flash memory.\n");

    /* Open the SPI driver */
    spifd = fopen (TEST_CHANNEL, NULL);

    if (NULL == spifd)
    {
        printf ("Error opening SPI driver!\n");
        _time_delay (200L);
        _task_block ();
    }

#if BSP_SPI_MEMORY_GPIO_CS
    /* Open GPIO file containing SPI pin SS == chip select for memory */

    if (!lwgpio_init(&spigpio, BSP_SPI_MEMORY_GPIO_CS, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing GPIO with associated pins failed.\n");
        _time_delay (200L);
        _task_block();
    }
    lwgpio_set_functionality(&spigpio,BSP_SPI_MUX_GPIO);/*BSP_SPI_MUX_GPIO need define in BSP for function mux as GPIO*/

    /* Set CS callback */

    callback.CALLBACK = set_CS;
    callback.USERDATA = &spigpio;
    printf ("Setting CS callback ... ");
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback))
    {
        printf ("OK\n");
    }
    else
    {
        printf ("ERROR\n");
    }

#endif

    /* Display baud rate */
    printf ("Current baud rate ... ");
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_GET_BAUD, &param))
    {
        printf ("%d Hz\n", param);
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Set a different rate */
    param = 500000;
    printf ("Changing the baud rate to %d Hz ... ", param);
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_SET_BAUD, &param))
    {
        printf ("OK\n");
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Display baud rate */
    printf ("Current baud rate ... ");
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_GET_BAUD, &param))
    {
        printf ("%d Hz\n", param);
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Set clock mode */
    param = SPI_CLK_POL_PHA_MODE0;
    printf ("Setting clock mode to %s ... ", clock_mode[param]);
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_SET_MODE, &param))
    {
        printf ("OK\n");
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Get clock mode */
    printf ("Getting clock mode ... ");
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_GET_MODE, &param))
    {
        printf ("%s\n", clock_mode[param]);
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Set big endian */
    param = SPI_DEVICE_BIG_ENDIAN;
    printf ("Setting endian to %s ... ", param == SPI_DEVICE_BIG_ENDIAN ? "SPI_DEVICE_BIG_ENDIAN" : "SPI_DEVICE_LITTLE_ENDIAN");
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_SET_ENDIAN, &param))
    {
        printf ("OK\n");
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Get endian */
    printf ("Getting endian ... ");
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_GET_ENDIAN, &param))
    {
        printf ("%s\n", param == SPI_DEVICE_BIG_ENDIAN ? "SPI_DEVICE_BIG_ENDIAN" : "SPI_DEVICE_LITTLE_ENDIAN");
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Set transfer mode */
    param = SPI_DEVICE_MASTER_MODE;
    printf ("Setting transfer mode to %s ... ", device_mode[param]);
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_SET_TRANSFER_MODE, &param))
    {
        printf ("OK\n");
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Get transfer mode */
    printf ("Getting transfer mode ... ");
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_GET_TRANSFER_MODE, &param))
    {
        printf ("%s\n", device_mode[param]);
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Clear statistics */
    printf ("Clearing statistics ... ");
    result = ioctl (spifd, IO_IOCTL_SPI_CLEAR_STATS, NULL);
    if (SPI_OK == result)
    {
        printf ("OK\n");
    }
    else if (MQX_IO_OPERATION_NOT_AVAILABLE == result)
    {
        printf ("not available, define BSPCFG_ENABLE_SPI_STATS\n");
    }
    else
    {
        printf ("ERROR\n");
    }

    /* Get statistics */
    printf ("Getting statistics: ");
    result = ioctl (spifd, IO_IOCTL_SPI_GET_STATS, &stats);
    if (SPI_OK == result)
    {
        printf ("\n");
        printf ("Rx packets:   %d\n", stats.RX_PACKETS);
        printf ("Tx packets:   %d\n", stats.TX_PACKETS);
    }
    else if (MQX_IO_OPERATION_NOT_AVAILABLE == result)
    {
        printf ("not available, define BSPCFG_ENABLE_SPI_STATS\n");
    }
    else
    {
        printf ("ERROR\n");
    }
    printf ("\n");

#if SPI_DEBUG_STATUS

    /* Read status */
    memory_read_status (spifd);

    /* Disable protection */
    memory_set_protection (spifd, FALSE);

    /* Erase memory before tests */
    memory_chip_erase (spifd);

    printf ("\n");

#endif

#if SPI_DEBUG_BYTE

    /* Write a data byte to memory */
    memory_write_byte (spifd, SPI_MEMORY_ADDR1, TEST_BYTE_1);

    /* Read status */
    memory_read_status (spifd);

    /* Read a data byte from memory */
    if (TEST_BYTE_1 == memory_read_byte (spifd, SPI_MEMORY_ADDR1))
    {
        printf ("Byte test ... OK\n");
    }
    else
    {
        printf ("Byte test ... ERROR\n");
    }

    /* Get statistics */
    printf ("Getting statistics: ");
    result = ioctl (spifd, IO_IOCTL_SPI_GET_STATS, &stats);
    if (SPI_OK == result)
    {
        printf ("\n");
        printf ("Rx packets:   %d\n", stats.RX_PACKETS);
        printf ("Tx packets:   %d\n", stats.TX_PACKETS);
    }
    else if (MQX_IO_OPERATION_NOT_AVAILABLE == result)
    {
        printf ("not available, define BSPCFG_ENABLE_SPI_STATS\n");
    }
    else
    {
        printf ("ERROR\n");
    }
    printf ("\n");

#endif

#if SPI_DEBUG_WRITE_DATA_SHORT

    /* Write short data to memory */
    memory_write_data (spifd, SPI_MEMORY_ADDR2, sizeof(TEST_STRING_SHORT) - 1, (unsigned char *)TEST_STRING_SHORT);

    /* Read status */
    memory_read_status (spifd);

    /* Read data */
    result = memory_read_data (spifd, SPI_MEMORY_ADDR2, sizeof(TEST_STRING_SHORT) - 1, recv_buffer);

    /* Test result */
    if ((result != sizeof(TEST_STRING_SHORT) - 1) || (0 != memcmp (TEST_STRING_SHORT, recv_buffer, result)))
    {
        printf ("Write short data test ... ERROR\n");
    }
    else
    {
        printf ("Write short data test ... OK\n");
    }
    printf ("\n");

#endif

#if SPI_DEBUG_WRITE_DATA_LONG

    /* Page write to memory */
    memory_write_data (spifd, SPI_MEMORY_ADDR3, sizeof(TEST_STRING_LONG) - 1, (unsigned char *)TEST_STRING_LONG);

    /* Read status */
    memory_read_status (spifd);

    /* Read data */
    result = memory_read_data (spifd, SPI_MEMORY_ADDR3, sizeof(TEST_STRING_LONG) - 1, recv_buffer);

    /* Test result */
    if ((result != sizeof(TEST_STRING_LONG) - 1) || (0 != memcmp (TEST_STRING_LONG, recv_buffer, result)))
    {
        printf ("Write long data test ... ERROR\n");
    }
    else
    {
        printf ("Write long data test ... OK\n");
    }
    printf ("\n");

#endif

    /* Test simultaneous write and read */
    memset (send_buffer, 0, sizeof (send_buffer));
    memset (recv_buffer, 0, sizeof (recv_buffer));
    send_buffer[0] = SPI_MEMORY_READ_DATA;
    for (i = SPI_MEMORY_ADDRESS_BYTES; i != 0; i--)
    {
        send_buffer[i] = (SPI_MEMORY_ADDR1 >> ((SPI_MEMORY_ADDRESS_BYTES - i) << 3)) & 0xFF;
    }
    rw.BUFFER_LENGTH = 10;
    rw.WRITE_BUFFER = (char *)send_buffer;
    rw.READ_BUFFER = (char *)recv_buffer;
    printf ("IO_IOCTL_SPI_READ_WRITE ... ");
    if (SPI_OK == ioctl (spifd, IO_IOCTL_SPI_READ_WRITE, &rw))
    {
        printf ("OK\n");
    }
    else
    {
        printf ("ERROR\n");
    }
    fflush (spifd);
    printf ("Simultaneous write and read - memory read from 0x%08x (%d):\n", SPI_MEMORY_ADDR1, rw.BUFFER_LENGTH);
    printf ("Write: ");
    for (i = 0; i < rw.BUFFER_LENGTH; i++)
    {
        printf ("0x%02x ", (unsigned char)rw.WRITE_BUFFER[i]);
    }
    printf ("\nRead : ");
    for (i = 0; i < rw.BUFFER_LENGTH; i++)
    {
        printf ("0x%02x ", (unsigned char)rw.READ_BUFFER[i]);
    }
    if (TEST_BYTE_1 == (unsigned char)rw.READ_BUFFER[1 + SPI_MEMORY_ADDRESS_BYTES])
    {
        printf ("\nSimultaneous read/write (data == 0x%02x) ... OK\n\n", (unsigned char)rw.READ_BUFFER[1 + SPI_MEMORY_ADDRESS_BYTES]);
    }
    else
    {
        printf ("\nSimultaneous read/write (data == 0x%02x) ... ERROR\n\n", (unsigned char)rw.READ_BUFFER[1 + SPI_MEMORY_ADDRESS_BYTES]);
    }

    /* Close the SPI */
    result = (uint32_t)fclose (spifd);
    if (result)
    {
        printf ("Error closing SPI, returned: 0x%08x\n", result);
    }

    printf ("\n-------------- End of example --------------\n\n");
}
