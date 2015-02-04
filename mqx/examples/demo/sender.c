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

#include <mqx.h>
#include <bsp.h>
#include <message.h>
#include <errno.h>
#include <mutex.h>
#include <sem.h>
#include <event.h>
#include <log.h>
#include "demo.h"

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
   MESSAGE_HEADER_STRUCT_PTR   msg_ptr;
   _task_id                    created_task;

   Sender_Queue_qid = _msgq_open( MSGQ_FREE_QUEUE, SIZE_UNLIMITED);
   if (Sender_Queue_qid == (_queue_id)0){
         /* queue could not be opened */
   }
   created_task = _task_create(0, RESPONDER, 0);
   if (created_task == MQX_NULL_TASK_ID) {
      /* task creation failed */
   }

   /*
   * Service the message queue - Sender_Queue
   */
   msg_ptr = _msgq_receive_ticks(Sender_Queue_qid, NO_TIMEOUT);
   /* process message Start_msg */
   msg_ptr->SIZE = sizeof(MESSAGE_HEADER_STRUCT);
   msg_ptr->SOURCE_QID = msg_ptr->TARGET_QID;
   msg_ptr->TARGET_QID = Responder_Queue_qid;
   _msgq_send(msg_ptr);

   /* 
   ** LOOP - 
   */
   while ( TRUE ) {
      /*
      * Service the message queue - Sender_Queue
      */
      msg_ptr = _msgq_receive_ticks(Sender_Queue_qid, NO_TIMEOUT);
      /* process message Loop_msg */
      _time_delay_ticks(3);
      msg_ptr->SIZE = sizeof(MESSAGE_HEADER_STRUCT);
      msg_ptr->SOURCE_QID = msg_ptr->TARGET_QID;
      msg_ptr->TARGET_QID = Responder_Queue_qid;
      _msgq_send(msg_ptr);

   } /* endwhile */ 

} /*end of task*/

/* End of File */
