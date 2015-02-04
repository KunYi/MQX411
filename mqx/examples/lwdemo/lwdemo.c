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
*   This file contains source for the Lightweight MQX demo test.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
//#include <message.h>
#include <errno.h>
#include <lwevent.h>
#include <lwmsgq.h>
#include "lwdemo.h"

/*
** Global variable definitions
*/
#if 0 // removed in LWDEMO
/* Message Queue Ids */
_queue_id      Sender_Queue_qid;

/* Message Queue Ids */
_queue_id      Responder_Queue_qid;

/* Message Queue Ids */
_queue_id      Main_Queue_qid;

/* Message Pool Ids */
_pool_id       MsgPool_pool_id;
#endif 
/* Use light weight events */
LWEVENT_STRUCT lwevent;

/* Use light weight semaphores */
LWSEM_STRUCT lwsem;

/* Use light weight message queues */
uint32_t main_queue[sizeof(LWMSGQ_STRUCT)/sizeof(uint32_t) +
   NUM_MESSAGES * MSG_SIZE];
uint32_t sender_queue[sizeof(LWMSGQ_STRUCT)/sizeof(uint32_t) +
   NUM_MESSAGES * MSG_SIZE];
uint32_t responder_queue[sizeof(LWMSGQ_STRUCT)/sizeof(uint32_t) +
   NUM_MESSAGES * MSG_SIZE];

/*   Function Code       */


/* End of File */
