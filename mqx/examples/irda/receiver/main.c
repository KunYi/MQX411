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
#define TX_TASK 5
#define RX_TASK 6
#define MESSAGE_RECEIVED_MASK  0x01

extern void tx_task(uint32_t);
extern void rx_task(uint32_t);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index,   Function,   Stack,  Priority, Name,     Attributes,          Param, Time Slice */
    { TX_TASK,     tx_task,   1500,   8,        "tx",      MQX_AUTO_START_TASK, 0,     0 },
    { RX_TASK,     rx_task,   1500,   9,        "rx",      MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};

LWEVENT_STRUCT lwevent;

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
    uint32_t         result, param;
    char             recv_buffer[100] = "NUL";

    if (0 == memcmp (BSP_SERIAL_IRDA_RX_CHANNEL, BSP_DEFAULT_IO_CHANNEL, strlen(BSP_SERIAL_IRDA_TX_CHANNEL)))
    {
        printf("Error: IrDA channel and default IO channel must be different \n");
        _task_block();
    }

    irda_rx_device = fopen (BSP_SERIAL_IRDA_RX_CHANNEL, NULL);
    if( irda_rx_device == NULL )
    {
       /* device could not be opened */
        printf("\nFatal Error: IrDA RX Device \"%s\" open fail.\n", BSP_SERIAL_IRDA_RX_CHANNEL);
        printf("Please add proper configuration to user_config.h\n");
        printf("i.e #define BSPCFG_ENABLE_TTYA 1\n ");
        _task_block();
    }

    /* Set IRDA mode */
    param = TRUE;
    result = ioctl( irda_rx_device, IO_IOCTL_SERIAL_SET_IRDA_RX, &param );
    if( MQX_OK != result )
    {
        printf("Set IRDA RX mode fail\n");
       _task_block();
    }

    param = TEST_BAUDRATE;
    result = ioctl(irda_rx_device, IO_IOCTL_SERIAL_SET_BAUD, &param);
    if (MQX_OK != result) {
        printf("Set Baudrate fail\n");
        _task_block();
    }

    printf("Receiver running...\n");
    for (;;)
    {
        /*Wait until start of frame*/
        while (TRUE){
            read(irda_rx_device, recv_buffer, 1);
            if (*recv_buffer == '*') break;
        }

        result = read(irda_rx_device, recv_buffer, strlen(TEST_STRING));
        /* check result */
        if ((result == strlen(TEST_STRING))&&(0 == memcmp (TEST_STRING, recv_buffer, strlen(TEST_STRING))))
        {
            printf("Message received\n");
            /* Start transmit task to send ACK signal */
            if (_lwevent_set(&lwevent,MESSAGE_RECEIVED_MASK) != MQX_OK) {
               printf("Set Event failed\n");
               _task_block();
            }
        }
    }

}

/*TASK*-----------------------------------------------------
*
* Task Name    : tx_task
* Comments     :
*    This task send ACK string to Transmitter via Infrared
*
*END*-----------------------------------------------------*/
void tx_task
    (
        uint32_t initial_data
    )
{
    MQX_FILE_PTR     irda_tx_device = NULL;
    uint32_t result, param;

    /* Create lwevent group */
    if (_lwevent_create(&lwevent,LWEVENT_AUTO_CLEAR) != MQX_OK) {
       printf("\nMake event failed");
       _task_block();
    }

    if (0 == memcmp (BSP_SERIAL_IRDA_TX_CHANNEL, BSP_DEFAULT_IO_CHANNEL, strlen(BSP_SERIAL_IRDA_TX_CHANNEL)))
    {
        printf("Error: IrDA channel and default IO channel must be different \n");
        _task_block();
    }

    irda_tx_device = fopen (BSP_SERIAL_IRDA_TX_CHANNEL, NULL);
    if( irda_tx_device == NULL )
    {
       /* device could not be opened */
        printf("\nFatal Error: IrDA TX Device \"%s\" open fail.\n", BSP_SERIAL_IRDA_TX_CHANNEL);
        printf("Please add proper configuration to user_config.h\n");
        printf("i.e #define BSPCFG_ENABLE_TTYA 1\n ");
        _task_block();
    }

    /* Set IRDA mode */
    param = TRUE;
    result = ioctl( irda_tx_device, IO_IOCTL_SERIAL_SET_IRDA_TX, &param );
    if( MQX_OK != result )
    {
        printf("Set IRDA TX mode fail\n");
       _task_block();
    }

    param = TEST_BAUDRATE;
    result = ioctl(irda_tx_device, IO_IOCTL_SERIAL_SET_BAUD, &param);
    if (MQX_OK != result) {
        printf("Set Baudrate fail\n");
        _task_block();
    }

    while (TRUE)
    {
        if (_lwevent_wait_ticks(&lwevent,MESSAGE_RECEIVED_MASK,TRUE,0) != MQX_OK) {
           printf("\nEvent Wait failed");
           _task_block();
        }
        /* Send ACK to transmitter */
        write( irda_tx_device, ACK, strlen(ACK) );
    }
}

/* EOF */
