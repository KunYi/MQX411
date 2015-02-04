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
*   This file contains the code for the main task.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <sem.h>
#include "main.h"


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


SW_FIFO      fifo;

/*TASK*----------------------------------------------------------
*
* Task Name : main_task
* Comments  : 
*     This task initializes three semaphores, creates NUM_WRITERS 
*     write_task's, and creates one read_task.
*END*----------------------------------------------------------*/

void main_task 
   (
      uint32_t initial_data
   )
{
   _task_id   task_id;
   _mqx_uint  i;

   fifo.READ_INDEX  = 0;
   fifo.WRITE_INDEX = 0;

   /* Create the semaphores */
   if (_sem_create_component(3,1,6) != MQX_OK) {
      printf("\nCreate semaphore component failed");
      _task_block();
   }
   if (_sem_create("sem.write", ARRAY_SIZE, 0) != MQX_OK) {
      printf("\nCreating write semaphore failed");
      _task_block();
   }
   if (_sem_create("sem.read", 0, 0) != MQX_OK) {
      printf("\nCreating read semaphore failed");
      _task_block();
   }
   if (_sem_create("sem.index", 1, 0) != MQX_OK) {
      printf("\nCreating index semaphore failed");
      _task_block();
   }

   /* Create the tasks */
   for (i = 0; i < NUM_WRITERS; i++) {
      task_id = _task_create(0, WRITE_TASK, (uint32_t)i);
      printf("\nwrite_task created, id 0x%lx", task_id);
   }
   
   task_id = _task_create(0,READ_TASK, 0);
   printf("\nread_task created, id 0x%lX", task_id);

   _task_block();

}

/* EOF */
