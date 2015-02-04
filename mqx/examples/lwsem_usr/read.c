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
*   This file contains the read task code.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "read.h"

#if ! MQX_ENABLE_USER_MODE
#error This application requires MQX_ENABLE_USER_MODE defined non-zero in user_config.h. Please recompile BSP and PSP with this option.
#endif

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


USER_RO_ACCESS SW_FIFO fifo;
USER_RW_ACCESS unsigned char fifo_data;

/*TASK*--------------------------------------------------------
*
* Task Name : read_task
* Comments  : This task creates two semaphores and 
*             NUM_WRITER write_tasks. It waits
*             on the read_sem and finally outputs the 
*             "data" variable.
*END*--------------------------------------------------------*/

void read_task 
   (
      uint32_t initial_data
   )
{
   _task_id  task_id;
   _mqx_uint result;
   _mqx_uint i;

   /* Create the lightweight semaphores */
   result = _usr_lwsem_create(&fifo.READ_SEM, 0);
   if (result != MQX_OK) {
      printf("\nCreating read_sem failed: 0x%X", result);
      _task_block();
   }

   result = _usr_lwsem_create(&fifo.WRITE_SEM, 1);
   if (result != MQX_OK) {
      printf("\nCreating write_sem failed: 0x%X", result);
      _task_block();
   }

   /* Create the write tasks */
   for (i = 0; i < NUM_WRITERS; i++) {
      task_id = _task_create(0, WRITE_TASK, (uint32_t)('A' + i));
      printf("\nwrite_task created, id 0x%lX", task_id);
   }

   while(TRUE) {
      result = _lwsem_wait(&fifo.READ_SEM);
      if (result != MQX_OK) {
         printf("\n_lwsem_wait failed: 0x%X", result);
         _task_block();
      }
      putchar('\n');
      putchar(fifo_data);
      _lwsem_post(&fifo.WRITE_SEM);
   }

}

/* EOF */
