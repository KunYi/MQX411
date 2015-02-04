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
*   This file contains the source for the irda example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <main.h>

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


/* Task IDs */
#define TX_TASK             5
#define RX_TASK             6
#define NUM_TEST_MESSAGE    50

extern void tx_task(uint32_t);
extern void rx_task(uint32_t);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index,   Function,   Stack,  Priority, Name,     Attributes,          Param, Time Slice */
    { TX_TASK,     tx_task,   1500,   8,        "tx",      MQX_AUTO_START_TASK, 0,     0 },
    { RX_TASK,     rx_task,   1500,   9,        "rx",      MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};
uint32_t msg_cnt = 0;


/*TASK*-----------------------------------------------------
*
* Task Name    : tx_task
* Comments     :
*    This task send test string to Receiver via Infrared
*
*END*-----------------------------------------------------*/
void tx_task
    (
        uint32_t initial_data
    )
{
    MQX_FILE_PTR     irda_tx_device = NULL;
    uint32_t result, param;

    printf ("\n-------------- UART IrDA driver example --------------\n\n");

    printf ("This example application demonstrates usage of UART IrDA mode\n");
    printf ("It transfers data to other device over Infrared\n");

    if (0 == memcmp (BSP_SERIAL_IRDA_TX_CHANNEL, BSP_DEFAULT_IO_CHANNEL, strlen(BSP_SERIAL_IRDA_TX_CHANNEL)))
    {
        printf("Error: IrDA channel and default IO channel must be different \n");
        _task_block();
    }

    irda_tx_device = fopen (BSP_SERIAL_IRDA_TX_CHANNEL, NULL);
    if( irda_tx_device == NULL )
    {
       /* device could not be opened */
        printf("Fatal Error: IrDA TX Device \"%s\" open fail.\n", BSP_SERIAL_IRDA_TX_CHANNEL);
        printf("Please add proper configuration to user_config.h\n");
        printf("i.e #define BSPCFG_ENABLE_TTYA 1\n");
        _task_block();
    }

    param = TEST_BAUDRATE;
    result = ioctl(irda_tx_device, IO_IOCTL_SERIAL_SET_BAUD, &param);
    if (MQX_OK != result) {
        printf("Set Baudrate fail\n");
        _task_block();
    }

    /* Set IRDA TX mode */
    printf("Enable IrDA TX mode...");
    param = TRUE;
    result = ioctl( irda_tx_device, IO_IOCTL_SERIAL_SET_IRDA_TX, &param );
    if( MQX_OK != result )
    {
        printf("Error\n");
       _task_block();
    }
    else
    {
        printf("OK\n");
    }
    /* Switch to rx initialize */
    _time_delay(10);

    printf("\nSend test messages:\n");

    while (1)
    {
        write( irda_tx_device, TEST_STRING, strlen(TEST_STRING) );
        _time_delay(100);
        if (NUM_TEST_MESSAGE == msg_cnt)
        {
            break;
        }
    }
    printf("Disable IrDA TX mode...");
    param = FALSE;
    result = ioctl( irda_tx_device, IO_IOCTL_SERIAL_SET_IRDA_TX, &param );
    if( MQX_OK != result )
    {
        printf("Error\n");
       _task_block();
    }
    else
    {
        printf("OK\n");
    }
    printf("\nAll complete\n");
}


/*TASK*-----------------------------------------------------
*
* Task Name    : rx_task
* Comments     :
*    Read ACK from Receiver
*
*END*-----------------------------------------------------*/
void rx_task
    (
        uint32_t initial_data
    )
{
    MQX_FILE_PTR     irda_rx_device = NULL;
    uint32_t result, param;
    char  recv_buffer[] = "NUL";

    if (0 == memcmp (BSP_SERIAL_IRDA_RX_CHANNEL, BSP_DEFAULT_IO_CHANNEL, strlen(BSP_SERIAL_IRDA_TX_CHANNEL)))
    {
        printf("Error: IrDA channel and default IO channel must be different \n");
        _task_block();
    }

    irda_rx_device = fopen (BSP_SERIAL_IRDA_RX_CHANNEL, NULL);
    if( irda_rx_device == NULL )
    {
       /* device could not be opened */
        printf("Fatal Error: IrDA RX Device \"%s\" open fail.\n", BSP_SERIAL_IRDA_RX_CHANNEL);
        printf("Please add proper configuration to user_config.h\n");
        printf("i.e #define BSPCFG_ENABLE_TTYA 1\n ");
        _task_block();
    }

    /* Set IRDA RX mode */
    printf("Enable IrDA RX mode...");
    param = TRUE;
    result = ioctl( irda_rx_device, IO_IOCTL_SERIAL_SET_IRDA_RX, &param );
    if( MQX_OK != result )
    {
        printf("ERROR\n");
       _task_block();
    }
    else
    {
        printf("OK\n");
    }

    param = TEST_BAUDRATE;
    result = ioctl(irda_rx_device, IO_IOCTL_SERIAL_SET_BAUD, &param);
    if (MQX_OK != result) {
        printf("Set Baudrate fail\n");
        _task_block();
    }
    while (1)
    {
        /*Wait until start of frame*/
        while (1){
            read(irda_rx_device, recv_buffer, 1);
            if (*recv_buffer == '#') break;
        }
        result = fread(recv_buffer, 1, strlen(ACK) - 1,irda_rx_device);

        /* Wait for ACK result */
        if ((result == (strlen(ACK)-1)) && (0 == memcmp (ACK, recv_buffer, result)))
        {
            putchar ('+');
            msg_cnt++;
        }
        if (NUM_TEST_MESSAGE == msg_cnt)
        {
            break;
        }
    }
    printf("\nDone");
    printf("\nDisable IrDA RX mode...");
    param = FALSE;
    result = ioctl( irda_rx_device, IO_IOCTL_SERIAL_SET_IRDA_RX, &param );
    if( MQX_OK != result )
    {
        printf("Error\n");
       _task_block();
    }
    else
    {
        printf("OK\n");
    }

    _task_block();

}
/* EOF */
