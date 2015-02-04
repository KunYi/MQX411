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
*   This file contains the source for one of the IPC program examples.
*   This program should be started second.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <message.h>
#include <ipc.h>
#include <ipc_pcb.h>
#include <io_pcb.h>
#include <pcb_mqxa.h>
#include "ipc_ex.h"


#if ! MQX_USE_IPC
#error This application requires MQX_USE_IPC defined non-zero in user_config.h. Please recompile libraries with this option.
#endif


#if ! MQX_IS_MULTI_PROCESSOR
#error This application requires MQX_IS_MULTI_PROCESSOR defined non-zero in user_config.h. Please recompile libraries with this option.
#endif


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#warning This application requires to define which IO channel is used for inter-processor communication. 
#warning Please set corresponding BSPCFG_ENABLE_ITTYx (different from default IO channel) as non-zero in user_config.h and recompile BSP with this option.
#warning Please check that IO_PORT_NAME member of the pcb_mqxa_init structure below is set identically.

#ifndef DEFAULT_SERIAL_IPC_PORT_NAME
    #define DEFAULT_SERIAL_IPC_PORT_NAME "ittyb:"
#endif

extern void main_task(uint32_t);

IO_PCB_MQXA_INIT_STRUCT pcb_mqxa_init = 
{
   /* IO_PORT_NAME */          DEFAULT_SERIAL_IPC_PORT_NAME, 
   /* BAUD_RATE    */          0, // use default baud rate for device
   /* IS POLLED */             TRUE,
   /* INPUT MAX LENGTH */      sizeof(THE_MESSAGE),
   /* INPUT TASK PRIORITY */   7,
   /* OUPUT TASK PRIORITY */   7
};

IPC_PCB_INIT_STRUCT pcb_init =
{
   /* IO_PORT_NAME */             "pcb_mqxa_ittyx:",
   /* DEVICE_INSTALL? */          _io_pcb_mqxa_install,
   /* DEVICE_INSTALL_PARAMETER*/  (void *)&pcb_mqxa_init,
   /* IN_MESSAGES_MAX_SIZE */     sizeof(THE_MESSAGE),
   /* IN MESSAGES_TO_ALLOCATE */  8,
   /* IN MESSAGES_TO_GROW */      8,
   /* IN_MESSAGES_MAX_ALLOCATE */ 16,
   /* OUT_PCBS_INITIAL */         8,
   /* OUT_PCBS_TO_GROW */         8,
   /* OUT_PCBS_MAX */             16
};

const IPC_ROUTING_STRUCT ipc_routing_table[] =
{
   { TEST2_ID, TEST2_ID, QUEUE_TO_TEST2 },
   { 0, 0, 0 }
};

const IPC_PROTOCOL_INIT_STRUCT ipc_init_table[] =
{
   { _ipc_pcb_init, &pcb_init, "Pcb_to_test2", QUEUE_TO_TEST2 },
   { NULL, NULL, NULL, 0}
};

static const IPC_INIT_STRUCT ipc_init = {
    ipc_routing_table,
    ipc_init_table
};

const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   /* Task Index, Function,  Stack,                  Priority, Name,        Attributes,          Param, Time Slice */
    { IPC_TTN,    _ipc_task, IPC_DEFAULT_STACK_SIZE, 8,        "_ipc_task", MQX_AUTO_START_TASK, (uint32_t)&ipc_init,     0 },
    { MAIN_TTN,   main_task, 2000,                   9,        "Main",      MQX_AUTO_START_TASK, 0,                      0 },
    { 0 }
};

MQX_INITIALIZATION_STRUCT  MQX_init_struct =
{
   /* PROCESSOR_NUMBER */                 TEST_ID,
   /* START_OF_KERNEL_MEMORY */           BSP_DEFAULT_START_OF_KERNEL_MEMORY,
   /* END_OF_KERNEL_MEMORY */             BSP_DEFAULT_END_OF_KERNEL_MEMORY,
   /* INTERRUPT_STACK_SIZE */             BSP_DEFAULT_INTERRUPT_STACK_SIZE,
   /* TASK_TEMPLATE_LIST */               (void *)MQX_template_list,
   /* MQX_HARDWARE_INTERRUPT_LEVEL_MAX */ BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX,
   /* MAX_MSGPOOLS */                     8,
   /* MAX_MSGQS */                        16,
   /* IO_CHANNEL */                       BSP_DEFAULT_IO_CHANNEL,
   /* IO_OPEN_MODE */                     BSP_DEFAULT_IO_OPEN_MODE
};

unsigned char global_endian_swap_def[2] = {4, 0};

/*TASK*----------------------------------------------------------
*
* Task Name : main_task
* Comments  : 
*     This task creates a message pool and a message queue then 
*     sends a message to a queue on the second CPU.
*     It waits for a return message, validating the message before
*     sending a new message.
*END*-----------------------------------------------------------*/

void main_task
   (
      uint32_t dummy
   ) 
{
   _pool_id        msgpool;
   THE_MESSAGE_PTR msg_ptr;
   _queue_id       qid;
   _queue_id       my_qid;
   uint32_t         test_number = 0;

   my_qid  = _msgq_open(MAIN_QUEUE,0);
   qid     = _msgq_get_id(TEST2_ID,RESPONDER_QUEUE);
   msgpool = _msgpool_create(sizeof(THE_MESSAGE), 8, 8, 16);
   while (test_number < 1000) {
      msg_ptr = (THE_MESSAGE_PTR)_msg_alloc(msgpool);
      msg_ptr->HEADER.TARGET_QID = qid;
      msg_ptr->HEADER.SOURCE_QID = my_qid;
      msg_ptr->DATA = test_number++;
      putchar('-');
      _msgq_send(msg_ptr);
      msg_ptr = _msgq_receive(MSGQ_ANY_QUEUE, 1000);
      if (msg_ptr == NULL) {
         puts("Receive failed\n");
         //_task_block();
      } else {
         if (msg_ptr->HEADER.SIZE != sizeof(THE_MESSAGE)) {
            puts("Message wrong size\n");
            //_task_block();
         } else {
            if (MSG_MUST_CONVERT_DATA_ENDIAN(msg_ptr->HEADER.CONTROL)) {
               _mem_swap_endian(global_endian_swap_def, &msg_ptr->DATA);
            } /* Endif */
            if (msg_ptr->DATA != test_number) {
               puts("Message data incorrect\n");
               //_task_block();
            } else {
               puts("Message received\n");
            }
         }
         _msg_free(msg_ptr);
      }
   } 
   puts("All complete\n");
   _task_block();
} 

/* EOF */
