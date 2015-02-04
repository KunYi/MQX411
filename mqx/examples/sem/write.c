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
*   This file contains the code for the write task.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <sem.h>
#include "main.h"

/*TASK*----------------------------------------------------------
*
* Task Name : write_task
* Comments  : 
*     This task opens a connection to all three semaphores then 
*     waits for sem.write and sem.index. It writes one entry
*     in the data array and posts sem.index and sem.read.
*END*-----------------------------------------------------------*/
     
void write_task 
   (
      uint32_t initial_data
   )
{
   void   *write_sem;
   void   *read_sem;
   void   *index_sem;

   /* open connections to all the semaphores */
   if (_sem_open("sem.write", &write_sem) != MQX_OK) {
      printf("\nOpening write semaphore failed.");
      _task_block();
   }                  
   if (_sem_open("sem.index", &index_sem) != MQX_OK) {
      printf("\nOpening index semaphore failed.");
      _task_block();
   }
   if (_sem_open("sem.read", &read_sem) != MQX_OK) {
      printf("\nOpening read semaphore failed.");
      _task_block();
   }

   while (TRUE) {
      /* wait for the semphores */
      if (_sem_wait(write_sem, 0) != MQX_OK) {
         printf("\nWwaiting for Write semaphore failed");
         _task_block();
      }                  
      if (_sem_wait(index_sem, 0) != MQX_OK) {
         printf("\nWaiting for index semaphore failed");
         _task_block();
      }
      
      fifo.DATA[fifo.WRITE_INDEX++] = _task_get_id();
      if (fifo.WRITE_INDEX >= ARRAY_SIZE) {
         fifo.WRITE_INDEX = 0;
      }
      /* Post the semaphores */
      _sem_post(index_sem);
      _sem_post(read_sem);
   }

}

/* EOF */
