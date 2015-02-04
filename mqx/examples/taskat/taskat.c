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
*   This file contains a demo for creating a task AT a specified
*   address (using the specified memory for the task stack and TD
*   structure).
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


#define MAIN_TASK               10
#define TEST_TASK1              11
#define TEST_TASK1_PARAM        0x12345678

extern void main_task(uint32_t);
extern void test_task(uint32_t);


const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   /* Task Index, Function,   Stack, Priority,  Name,   Attributes,          Param, Time Slice */
    { MAIN_TASK,  main_task,  4000,  9,         "Main", MQX_AUTO_START_TASK, 0,     0 },
    { TEST_TASK1, test_task,  0,     8,         "test", 0,                   0,     0 },
    { 0 }
};

#define TEST_STACK_SIZE 2000
// Should be ABI aligned ... cannot do so portably....
double test_task_stack[TEST_STACK_SIZE/sizeof(double)];

volatile uint32_t test_task_val = 0;

/*TASK*-------------------------------------------------------------------
*
* Task Name    : test_task
* Comments     :
*   created task
*
*END*----------------------------------------------------------------------*/

void test_task
   (
      uint32_t param
   )
{/* Body */
   char buf[8];

   test_task_val = 1;
   printf("test_task: Local var at 0x%08x and param 0x%08x\n", buf, param);
}/* Endbody */

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   starts up the task at given memory area.
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t param
   )
{/* Body */
   _task_id tid;

   printf("main_task: Task create at 0x%08x..0x%08x\n",
      test_task_stack, (unsigned char *)test_task_stack + TEST_STACK_SIZE);

   tid = _task_create_at(0, TEST_TASK1, TEST_TASK1_PARAM, test_task_stack,
      TEST_STACK_SIZE);

   if (tid == MQX_NULL_TASK_ID) {
      printf("task create at failed");
      _mqx_fatal_error(MQX_INVALID_TASK_ID);
   } /* Endif */

   if (test_task_val != 1) {
      printf("ERROR: test_task_val != 1\n");
   } else {
      printf("PASSED\n");
   }/* Endif */

   _task_block();

} /* Endbody */

/* EOF */
