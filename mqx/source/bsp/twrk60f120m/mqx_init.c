/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains the source for the default MQX INITIALIZATION
*   STRUCTURE
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"

extern TASK_TEMPLATE_STRUCT MQX_template_list[];

const MQX_INITIALIZATION_STRUCT  MQX_init_struct =
{
   /* PROCESSOR_NUMBER                */  BSP_DEFAULT_PROCESSOR_NUMBER,
   /* START_OF_KERNEL_MEMORY          */  BSP_DEFAULT_START_OF_KERNEL_MEMORY,
   /* END_OF_KERNEL_MEMORY            */  BSP_DEFAULT_END_OF_KERNEL_MEMORY,
   /* INTERRUPT_STACK_SIZE            */  BSP_DEFAULT_INTERRUPT_STACK_SIZE,
   /* TASK_TEMPLATE_LIST              */  MQX_template_list,
   /* MQX_HARDWARE_INTERRUPT_LEVEL_MAX*/  BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX,
   /* MAX_MSGPOOLS                    */  BSP_DEFAULT_MAX_MSGPOOLS,
   /* MAX_MSGQS                       */  BSP_DEFAULT_MAX_MSGQS,
   /* IO_CHANNEL                      */  BSP_DEFAULT_IO_CHANNEL,
   /* IO_OPEN_MODE                    */  BSP_DEFAULT_IO_OPEN_MODE,
   0,
   0,
#if MQX_ENABLE_USER_MODE   
   BSP_DEFAULT_START_OF_KERNEL_AREA,
   BSP_DEFAULT_END_OF_KERNEL_AREA,
   
   BSP_DEFAULT_START_OF_USER_DEFAULT_MEMORY,
   BSP_DEFAULT_END_OF_USER_DEFAULT_MEMORY,
   
   BSP_DEFAULT_START_OF_USER_HEAP,
   BSP_DEFAULT_END_OF_USER_HEAP,
   
   BSP_DEFAULT_START_OF_USER_RW_MEMORY,
   BSP_DEFAULT_END_OF_USER_RW_MEMORY,
   
   BSP_DEFAULT_START_OF_USER_RO_MEMORY,
   BSP_DEFAULT_END_OF_USER_RO_MEMORY,
   
   BSP_DEFAULT_START_OF_USER_NO_MEMORY,
   BSP_DEFAULT_END_OF_USER_NO_MEMORY,

   BSP_DEFAULT_MAX_USER_TASK_PRIORITY,
   BSP_DEFAULT_MAX_USER_TASK_COUNT,
#endif
};

/* EOF */
