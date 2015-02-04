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
*   This file contains the source for the mutex example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mutex.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


/* Task IDs */
#define MAIN_TASK     5
#define PRINT_TASK    6

extern void main_task(uint32_t initial_data);
extern void print_task(uint32_t initial_data);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   /* Task Index,   Function,   Stack,  Priority,   Name,       Attributes,          Param, Time Slice */
    { MAIN_TASK,    main_task,  1000,   8,          "main",     MQX_AUTO_START_TASK, 0,     0 },
    { PRINT_TASK,   print_task, 1000,   9,          "print",    0,                   0,     3 },
    { 0 }
};

MUTEX_STRUCT   print_mutex;

/*TASK*--------------------------------------------------------
*
* Task Name : main_task
* Comments  : This task creates a mutex and then two 
*             instances of the print task.
*END*--------------------------------------------------------*/

void main_task
   (
      uint32_t initial_data
   )
{
   MUTEX_ATTR_STRUCT mutexattr;
   char*            strings1[] = { "1: ", "Hello from Print task 1\n" };
   char*            strings2[] = { "2: ", "Print task 2 is alive\n" };

   /* Initialize mutex attributes */
   if (_mutatr_init(&mutexattr) != MQX_OK) {
      printf("Initialize mutex attributes failed.\n");
      _task_block();
   }
   
   /* Initialize the mutex */ 
   if (_mutex_init(&print_mutex, &mutexattr) != MQX_OK) {
      printf("Initialize print mutex failed.\n");
      _task_block();
   }
   /* Create the print tasks */
   _task_create(0, PRINT_TASK, (uint32_t)strings1);
   _task_create(0, PRINT_TASK, (uint32_t)strings2);

   _task_block();
}   

/*TASK*--------------------------------------------------------
*
* Task Name : print_task
* Comments  : This task prints a message. It uses a mutex to 
*             ensure I/O is not interleaved.
*END*--------------------------------------------------------*/

void print_task
   (
      uint32_t initial_data
   )
{
   char **strings = (char **)initial_data;

   while(TRUE) {
      if (_mutex_lock(&print_mutex) != MQX_OK) {
         printf("Mutex lock failed.\n");
         _task_block();
      }

      /*
       * The yield in between the puts calls just returnes back,
       * as the other task is blocked waiting for the mutex.
       * Both strings are always printed together on a single line.
       */
      _io_puts(strings[0]);     
      _sched_yield(); 
      _io_puts(strings[1]);

      _mutex_unlock(&print_mutex);

      /* Passes control to the other task (which then obtains the mutex) */
      _sched_yield();
   }
}

/* EOF */
