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
#include <errno.h>
#include <lwevent.h>
#include <lwmsgq.h>
#include "lwdemo.h"

/*   Task Code -  Sender     */


/*TASK---------------------------------------------------------------
*   
* Task Name   :  Sender
* Comments    : 
* 
*END*--------------------------------------------------------------*/

void Sender
   (
      uint32_t   parameter
   )
{
   _mqx_uint  msg[MSG_SIZE];
   _task_id   created_task;
   
   created_task = _task_create(0, RESPONDER, 0);
   if (created_task == MQX_NULL_TASK_ID) {
      /* task creation failed */
   }

   /*
   * Service the message queue - Sender_Queue
   */
   _lwmsgq_receive((void *)sender_queue, msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);
   _lwmsgq_send((void *)responder_queue, msg, LWMSGQ_SEND_BLOCK_ON_FULL);   

   /* 
   ** LOOP - 
   */
   while ( TRUE ) {
      /*
      * Service the message queue - Sender_Queue
      */
      _lwmsgq_receive((void *)sender_queue, msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);
      _time_delay_ticks(3);
      _lwmsgq_send((void *)responder_queue, msg, LWMSGQ_SEND_BLOCK_ON_FULL);
   } /* endwhile */ 

} /*end of task*/

/* End of File */
