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
*   
*
*
*END************************************************************************/

#if defined (BSP_TWRMCF54418) || defined(BSP_TWR_K40X256) || defined(BSP_TWR_K60N512) || defined(BSP_KWIKSTIK_K40X256) ||\
    defined(BSP_TWR_K40D100M)  || defined(BSP_TWR_K70F120M) || defined(BSP_TWRPXS20) || defined(BSP_TWRPXS30) ||\
    defined(BSP_TWR_K60F120M)  || defined(BSP_TWR_K60D100M) || defined(BSP_TWRPXD10)
#define CAN_DEVICE  1
#else
#define CAN_DEVICE  0
#endif

// Set NODE to 1 or 2 depending on which endpoint you are running. See notes in test.c
#define NODE 1

#define MY_EVENT_GROUP 123

/* Task define */
#define MAIN_TASK   1
#define TX_TASK     2
#define RX_TASK     3

/* Tasks */
void MY_FLEXCAN_ISR(void *);
extern void Main_Task(uint32_t);
extern void Tx_Task(uint32_t);
extern void Rx_Task(uint32_t);

/* Functions */
extern void print_result(uint32_t);
extern void get_string(char *,uint32_t *);

/* Global variables */
uint32_t data_len_code;
uint32_t bit_rate;
uint32_t TX_identifier;
uint32_t RX_identifier;
uint32_t TX_remote_identifier;
uint32_t RX_remote_identifier;
uint32_t format;
uint32_t interrupt;
uint32_t TX_mailbox_num;
uint32_t RX_mailbox_num;
uint32_t TX_remote_mailbox_num;
uint32_t RX_remote_mailbox_num;
uint32_t bit_timing0;
uint32_t bit_timing1;
uint32_t frequency;
uint32_t flexcan_mode;
uint32_t flexcan_error_interrupt;

/* EOF */
