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
*   This file contains the code for the client_task
*
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include "server.h"


/*TASK*--------------------------------------------------------
*
* Task Name : client_task
* Comments  : This task opens a message queue and
*   allocates a message from the message pool.
*   The message is sent to the server_task and
*   a reply is waited for. The reply message is then freed.
*END*--------------------------------------------------------*/

void client_task 
   (
      uint32_t index
   )
{
   SERVER_MESSAGE_PTR msg_ptr;
   _queue_id          client_qid;
   bool            result;

   client_qid  = _msgq_open((_queue_number)(CLIENT_QUEUE_BASE +
      index), 0);

   if (client_qid == 0) {
      printf("\nCould not open a client message queue\n");
      _task_block();
   }
   
   while (TRUE) {
      /*allocate a message*/
      msg_ptr = (SERVER_MESSAGE_PTR)_msg_alloc(message_pool);

      if (msg_ptr == NULL) {
         printf("\nCould not allocate a message\n");
         _task_block();
      }

      msg_ptr->HEADER.SOURCE_QID = client_qid;      
      msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, SERVER_QUEUE);
      msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + 
         strlen((char *)msg_ptr->DATA) + 1;
      msg_ptr->DATA[0] = ('A'+ index);
     
      printf("Client Task %ld\n", index);  
      
      result = _msgq_send(msg_ptr);
      
      if (result != TRUE) {
         printf("\nCould not send a message\n");
         _task_block();
      }
   
      /* wait for a return message */
      msg_ptr = _msgq_receive(client_qid, 0);
      
      if (msg_ptr == NULL) {
         printf("\nCould not receive a message\n");
         _task_block();
      }
    
       /* free the message */
      _msg_free(msg_ptr);
   }

}

/* EOF */
