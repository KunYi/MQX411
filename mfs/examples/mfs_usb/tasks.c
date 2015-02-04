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
*   
*
*
*END************************************************************************/


#include "mfs_usb.h"


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


#if PSP_MQX_CPU_IS_MCF51JM
   /* small ram taget */
   #define SHELL_TASK_STACK_SIZE 1000
   #define USB_TASK_STACK_SIZE   1000
#else
   #define SHELL_TASK_STACK_SIZE 4000
   #define USB_TASK_STACK_SIZE   2500   
#endif   

/*
** MQX initialization information
*/

const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
  /* Task Index, Function,   Stack,                   Priority,  Name,      Attributes,          Param, Time Slice */
   { 1,          Shell_Task, SHELL_TASK_STACK_SIZE,   10,        "Shell",   MQX_AUTO_START_TASK, 0,     0 },
   { 2,          USB_task,   USB_TASK_STACK_SIZE,     8,         "USB",     MQX_AUTO_START_TASK, 0,     0 },
   { 0 }
};



 
/* EOF */
