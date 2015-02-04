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
*   This file contains the source for the kernel log example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <log.h>
#include <klog.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


extern void main_task(uint32_t initial_data);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   /* Task Index, Function,   Stack, Priority,  Name,     Attributes,          Param, Time Slice */
    { 10,         main_task,  1500,  8,         "Main",   MQX_AUTO_START_TASK, 0,     0},
    { 0 }
};

/*TASK*----------------------------------------------------------
*
* Task Name : main_task
* Comments  :
*   This task logs timer interrupts to the kernel log,
*   then prints out the log.
*END*-----------------------------------------------------------*/

void main_task
   (
      uint32_t initial_data
   )
{
   _mqx_uint result;
   _mqx_uint i;

   /* Create the kernel log */
   result = _klog_create(2*1024, 0);
   if (result != MQX_OK) {
      printf("Main task: _klog_create failed %08x", result);
      _task_block();
   }

   /* Enable kernel logging */
   _klog_control(KLOG_ENABLED | KLOG_CONTEXT_ENABLED |
      KLOG_INTERRUPTS_ENABLED| KLOG_SYSTEM_CLOCK_INT_ENABLED |
      KLOG_FUNCTIONS_ENABLED | KLOG_TIME_FUNCTIONS |
      KLOG_INTERRUPT_FUNCTIONS, TRUE);

   /* Write data into kernel log */   
   for (i = 0; i < 10; i++) {
      _time_delay_ticks(5 * i);
   }

   /* Disable kernel logging */
   _klog_control(0xFFFFFFFF, FALSE);

   /* Read data from the kernel log */
   printf("\nKernel log contains:\n");
   while (_klog_display()){
   }

   _task_block();

}

/* EOF */
