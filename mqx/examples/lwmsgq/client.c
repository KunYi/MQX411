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
#include <lwmsgq.h>
#include "server.h"


/*TASK*--------------------------------------------------------
*
* Task Name : client_task
* Comments  : This task sends a message to the server_task and
*   then waits for a reply.
*END*--------------------------------------------------------*/

void client_task
   (
      uint32_t index
   )
{
   _mqx_uint          msg[MSG_SIZE];
  
   while (TRUE) {
      msg[0] = ('A'+ index);
     
      printf("Client Task %ld\n", index);
      _lwmsgq_send((void *)server_queue, msg, LWMSGQ_SEND_BLOCK_ON_FULL);

      _time_delay_ticks(1);
      
      /* wait for a return message */
      _lwmsgq_receive((void *)client_queue, msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);
   }

}

/* EOF */
