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
*   This file contains the source for the isr example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <log.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


#define MAIN_TASK  10
#define MY_LOG     1
extern void main_task(uint32_t initial_data);
 
const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   /* Task Index, Function,  Stack, Priority,   Name,        Attributes,          Param, Time Slice */
    { MAIN_TASK,  main_task, 2000,  8,          "Main",      MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};

typedef struct entry_struct
{
   LOG_ENTRY_STRUCT   HEADER;
   _mqx_uint          C;
   _mqx_uint          I;
} ENTRY_STRUCT, * ENTRY_STRUCT_PTR;


/*TASK*----------------------------------------------------------
*
* Task Name : main_task
* Comments  :
*   This task logs 10 keystroke entries then prints out the log.
*END*-----------------------------------------------------------*/

void main_task
   (
      uint32_t initial_data
   )
{
   ENTRY_STRUCT entry;
   _mqx_uint    result;
   _mqx_uint    i;
   unsigned char        c;

   /* Create the log component */
   result = _log_create_component();
   if (result != MQX_OK) {
      printf("Main task: _log_create_component failed");
      _task_block();  
   }

   /* Create a log */
   result = _log_create(MY_LOG,
      10 * (sizeof(ENTRY_STRUCT)/sizeof(_mqx_uint)), 0);
   if (result != MQX_OK) {
      printf("Main task: _log_create failed");   
      _task_block();  
   }

   /* Write the data to the log */   
   printf("Please type in 10 characters:\n");
   for (i = 0; i < 10; i++) {
      c = getchar();
      result = _log_write(MY_LOG, 2, (_mqx_uint)c, i);
      if (result != MQX_OK) {
         printf("Main task: _log_write failed");   
      }
   }

   /* Read data from the log */
   printf("\nLog contains:\n");
   while (_log_read(MY_LOG, LOG_READ_OLDEST_AND_DELETE, 2,
      (LOG_ENTRY_STRUCT_PTR)&entry) == MQX_OK)
   {
      printf("Time: %lu.%03u%03u, c=%c, i=%u\n",
         entry.HEADER.SECONDS,
         (_mqx_uint)entry.HEADER.MILLISECONDS,
         (_mqx_uint)entry.HEADER.MICROSECONDS,
         (unsigned char)entry.C & 0xff,
         entry.I);
   }

   /* Destroy the log */
   _log_destroy(MY_LOG);

   _task_block();

}

/* EOF */
