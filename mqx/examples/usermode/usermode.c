/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
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
*   This file contains the source for the hello example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>

#include <lwmsgq.h>

#include <string.h>

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#if !MQX_ENABLE_USER_MODE
#error This application requires MQX_ENABLE_USER_MODE defined non-zero in user_config.h. Please recompile BSP and PSP with this option.
#endif

/* Task IDs */
#define PRIV_TASK       5
#define USR_MAIN_TASK   6
#define USR_TASK        7

#define MSG_SIZE        1
#define NUM_MESSAGES    5

#define USR_TASK_CNT    6


static void privilege_task(uint32_t);
static void usr_task(uint32_t);
static void usr_main_task(uint32_t);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{ 
   /* Task Index,       Function,       Stack,  Priority,   Name,         Attributes,          Param,  Time Slice */
    { PRIV_TASK,        privilege_task, 1500,   8,          "priv_task",  MQX_AUTO_START_TASK, 0,      0 },
    { USR_MAIN_TASK,    usr_main_task,  1500,   8,          "usr_main_task", MQX_USER_TASK,    0,      0 },
    { USR_TASK,         usr_task,       1500,   9,          "usr_task",   MQX_USER_TASK,       0,      0 },
    { 0 }
};

USER_RW_ACCESS uint32_t *que;

USER_RW_ACCESS _lwmem_pool_id usr_pool_id;


/*TASK*-----------------------------------------------------
* 
* Task Name    : privilege_task
* Comments     : the main auto-start task
*
*END*-----------------------------------------------------*/

static void privilege_task(uint32_t initial_data) 
{
    _mqx_uint msg[MSG_SIZE];
    _lwmem_pool_id mem_pool_id;
    LWMEM_POOL_STRUCT mem_pool;
    void   *mem_pool_start;
    
    LWMEM_POOL_STRUCT_PTR usr_pool_ptr;
    void   *usr_pool_start;
    
    /* memory pool: Read-Only for User tasks */
    mem_pool_start = _mem_alloc(1024);
    mem_pool_id = _lwmem_create_pool(&mem_pool, mem_pool_start, 1024);
    _mem_set_pool_access(mem_pool_id, POOL_USER_RO_ACCESS);

    /* message queue to communicate between this task and User tasks */
    que = (uint32_t*)_mem_alloc_from(mem_pool_id, sizeof(LWMSGQ_STRUCT) + NUM_MESSAGES * MSG_SIZE * sizeof(_mqx_uint));
    _usr_lwmsgq_init((void *)que, NUM_MESSAGES, MSG_SIZE);

    /* memory pool: Read-Write for user tasks */
    usr_pool_ptr = _usr_mem_alloc(sizeof(LWMEM_POOL_STRUCT));
    usr_pool_start = _mem_alloc(1024);
    usr_pool_id = _lwmem_create_pool(usr_pool_ptr, usr_pool_start, 1024);
    _mem_set_pool_access(usr_pool_id, POOL_USER_RW_ACCESS);

    /* create the user "main" task, whcih then creates the others */
    _task_create(0, USR_MAIN_TASK, USR_TASK_CNT);

    /* receive messages from user tasks and print */
    while (1) {
        _lwmsgq_receive((void *)que, msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);
        printf(" %c \n", msg[0]);
    }
}

/*TASK*-----------------------------------------------------
* 
* Task Name    : usr_main_task
* Comments     : user task which demonstrates creating 
*                other user tasks
*
*END*-----------------------------------------------------*/

static void usr_main_task(uint32_t initial_data) 
{
    int i;
    
    /* each user task receives its indes as a parameter */
    for (i = 0; i < initial_data; i++) {
        _usr_task_create(0, USR_TASK, i);
    }

    _task_block();
}

/*TASK*-----------------------------------------------------
* 
* Task Name    : usr_task
* Comments     : worker user task
*
*END*-----------------------------------------------------*/

static void usr_task(uint32_t initial_data) 
{
    _mqx_uint *msg;
    void   *ptr;
    
    /* allocate message object from default User memory pool */
    msg = (_mqx_uint*)_usr_mem_alloc(MSG_SIZE * sizeof(_mqx_uint));
  
    while (1) 
    {
        /* build the message with our signature */
        msg[0] = ('A'+ initial_data);
        
        /* demonstrate allocation from user pool created dynamically */
        ptr = _usr_mem_alloc_from(usr_pool_id, 16);

        /* send message to the privilege task and wait 'random' number of time */
        _usr_lwmsgq_send((void *)que, msg, LWMSGQ_SEND_BLOCK_ON_FULL);
        _time_delay_ticks((initial_data + 1)* 10);

        /* this memory was not really needed, just for a demo purpose */
        _usr_mem_free(ptr);
    }
}
