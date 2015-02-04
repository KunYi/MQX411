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
*   This file contains the write task code.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "read.h"

/*TASK*--------------------------------------------------------
*
* Task Name : write_task
* Comments  : This task waits for the write semaphore, 
*             then writes a character to "data" and posts a
*             read semaphore. 
*END*--------------------------------------------------------*/

void write_task 
   (
      uint32_t initial_data
   )
{

   //printf("\nWrite task created: 0x%lX", initial_data);
   while (TRUE) {
      if (_lwsem_wait(&fifo.WRITE_SEM) != MQX_OK) {
         //printf("\n_lwsem_wait failed");
         _task_block();
      }
      fifo_data = (unsigned char)initial_data;
      _lwsem_post(&fifo.READ_SEM);
   }

}

/* EOF */
