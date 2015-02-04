#ifndef __server_h__
#define __server_h__
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
*   This file contains definitions for this application
*
*
*END************************************************************************/

#include <mqx.h>
#include <message.h>

/* Number of clients */
#define NUM_CLIENTS           (7)
#define FP_SYSTEM_QUEUE_BASE   2

/* Task IDs */
#define SERVER_TASK       5
#define CLIENT_TASK       6

/* Queue IDs */
#define SERVER_QUEUE      8
#define CLIENT_QUEUE_BASE 9

/* This structure contains a data field and a message header structure */
typedef struct server_message
{
   MESSAGE_HEADER_STRUCT   HEADER;
   unsigned char                   DATA[5];
} SERVER_MESSAGE, * SERVER_MESSAGE_PTR;

/* Function prototypes */
extern void server_task (uint32_t initial_data);
extern void client_task (uint32_t initial_data);
extern _pool_id message_pool;

#endif
/* EOF */
