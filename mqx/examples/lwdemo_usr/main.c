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
*   This file contains source for the Lightweight MQX demo test.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <message.h>
#include <errno.h>
#include <lwevent.h>
#include <lwmsgq.h>
#if MQX_KERNEL_LOGGING
#include <klog.h>
#endif
#if MQX_USE_LOGS
#include <log.h>
#endif
#include "lwdemo.h"

#if ! MQX_ENABLE_USER_MODE
#error This application requires MQX_ENABLE_USER_MODE defined non-zero in user_config.h. Please recompile BSP and PSP with this option.
#endif

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


/*   Task Code -  Main     */

/*TASK---------------------------------------------------------------
*   
* Task Name   :  Main
* Comments    : 
* 
*END*--------------------------------------------------------------*/

void main_task
   (
      uint32_t   parameter
   )
{
   _mqx_uint                 msg[MSG_SIZE];
   _task_id                  created_task;
#if MQX_USE_LOGS || MQX_KERNEL_LOGGING
   _mqx_uint                 log_result;
#endif
   _mqx_uint                 result;

   _int_install_unexpected_isr();
   printf("\nMQX %s\n",_mqx_version);
   printf("Hello from main_task().");

#if MQX_USE_LOGS
   /* create the log component */
   log_result = _log_create_component();
   if (log_result != MQX_OK) { 
      printf("Error: log component could not be created\n");
   } /* endif */
#endif
   /* create lwevent group */
   result = _usr_lwevent_create(&lwevent, 0);
   if (result != MQX_OK) { 
      printf("Error: event component could not be created\n");
   } /* endif */
   
   /* create a lwsem */
   result = _usr_lwsem_create(&lwsem, 10);
   if (result != MQX_OK) {
      /* semaphore component could not be created */
   } /* endif */

   _usr_lwmsgq_init((void *)main_queue, NUM_MESSAGES, MSG_SIZE);
   _usr_lwmsgq_init((void *)sender_queue, NUM_MESSAGES, MSG_SIZE);
   _usr_lwmsgq_init((void *)responder_queue, NUM_MESSAGES, MSG_SIZE);

   created_task = _task_create(0, SENDER, 0);
   if (created_task == MQX_NULL_TASK_ID) {
      printf("Error: SENDER task creation failed\n");
   }
   created_task = _task_create(0, LWSEMA, 0);
   if (created_task == MQX_NULL_TASK_ID) {
      printf("Error: LWSEMA task creation failed\n");
   }
   created_task = _task_create(0, LWSEMB, 0);
   if (created_task == MQX_NULL_TASK_ID) {
      printf("Error: LWSEMB task creation failed\n");
   }
   created_task = _task_create(0, LWEVENTA, 0);
   if (created_task == MQX_NULL_TASK_ID) {
      printf("Error: LWEVENTA task creation failed\n");
   }
   created_task = _task_create(0, LWEVENTB, 0);
   if (created_task == MQX_NULL_TASK_ID) {
      printf("Error: LWEVENTB task creation failed\n");
   }

#if MQX_KERNEL_LOGGING == 1
   /* create log number 0 */
   log_result = _klog_create(200, 0);
   if (log_result != MQX_OK) { 
      printf("Error: log 0 could not be created\n");
   } /* endif */

   /* define kernel logging */
   _klog_control(0xFFFFFFFF, FALSE);
   _klog_control(
      KLOG_ENABLED                  |
      KLOG_FUNCTIONS_ENABLED        |
      KLOG_INTERRUPTS_ENABLED       |
      KLOG_SYSTEM_CLOCK_INT_ENABLED |
      KLOG_CONTEXT_ENABLED          |
      KLOG_TASKING_FUNCTIONS        |
      KLOG_ERROR_FUNCTIONS          |
      KLOG_MESSAGE_FUNCTIONS        |
      KLOG_INTERRUPT_FUNCTIONS      |
      KLOG_MEMORY_FUNCTIONS         |
      KLOG_TIME_FUNCTIONS           |
      KLOG_EVENT_FUNCTIONS          |
      KLOG_NAME_FUNCTIONS           |
      KLOG_MUTEX_FUNCTIONS          |
      KLOG_SEMAPHORE_FUNCTIONS      |
      KLOG_WATCHDOG_FUNCTIONS, 
      TRUE
      );
#endif

   /* 
   ** LOOP - 
   */
   while ( TRUE ) {
      _lwmsgq_send((void *)sender_queue, msg, LWMSGQ_SEND_BLOCK_ON_FULL);
      _lwmsgq_receive((void *)main_queue, msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);
   } /* endwhile */ 
} /*end of task*/

/* End of File */
