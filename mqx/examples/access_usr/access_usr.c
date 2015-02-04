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
*   This file contains the source for the hello example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h> 
#include <fio.h>

#include <string.h>

#if ! MQX_ENABLE_USER_MODE
#error This application requires MQX_ENABLE_USER_MODE defined non-zero in user_config.h. Please recompile BSP and PSP with this option.
#endif

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


/* Task IDs */
#define HELLO_TASK      5
#define USR_TASK_DEF    6
#define USR_TASK_RW     7
#define USR_TASK_RO     8
#define USR_TASK_NO_R   9
#define USR_TASK_NO_W   10


static void hello_task(uint32_t);
static void usr_task(uint32_t);
static void usr_task_no_w(uint32_t);


USER_RW_ACCESS static int g_urw;
USER_RO_ACCESS static int g_uro;
USER_NO_ACCESS static int g_uno;
static int g_udef;

USER_RW_ACCESS static char str[200];

const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{ 
   /* Task Index,   Function,   Stack,  Priority, Name,     Attributes,          Param, Time Slice */
    { HELLO_TASK,   hello_task, 1500,   9,  "hello",    MQX_AUTO_START_TASK,    0,  0 },
    { USR_TASK_DEF,     usr_task,   1500,   8,  "usr_task def", MQX_USER_TASK,          0,  0 },
    { USR_TASK_RW,     usr_task,   1500,   8,  "usr_task rw", MQX_USER_TASK,          0,  0 },
    { USR_TASK_RO,     usr_task,   1500,   8,  "usr_task ro", MQX_USER_TASK,          0,  0 },
    { USR_TASK_NO_R,     usr_task,   1500,   8,  "usr_task no_r", MQX_USER_TASK,          0,  0 },
    { USR_TASK_NO_W,     usr_task_no_w,   1500,   8,  "usr_task no_w", MQX_USER_TASK,     0,  0 },
    { 0 }
};


/*TASK*-----------------------------------------------------
* 
* Task Name    : hello_task
* Comments     :
*    This task prints " Hello World "
*
*END*-----------------------------------------------------*/
static void hello_task
   (
      uint32_t initial_data
   )
{
    printf("\n\nUSER PRIVILEGE MODE TEST\n"); 
        
    str[0] = 0;
    _task_create(0, USR_TASK_DEF, (uint32_t)&g_udef);
    printf("global user default access task: %s\n", str);

    
    str[0] = 0;
    _task_create(0, USR_TASK_RW, (uint32_t)&g_urw);
    printf("global user rw task: %s\n", str);
    
    
    str[0] = 0;
    _task_create(0, USR_TASK_RO, (uint32_t)&g_uro);
    printf("global user read only task: %s\n", str);

    
    str[0] = 0;
    _task_create(0, USR_TASK_NO_R, (uint32_t)&g_uno);
    printf("global user no access - read task: %s\n", str);

    
    str[0] = 0;
    _task_create(0, USR_TASK_NO_W, (uint32_t)&g_uno);
    printf("global user no access - write task: %s\n", str);
    
    printf("\n\nEND\n"); 
    
    _task_block();
}

static void usr_task(uint32_t initial_data) {
    uint32_t val;
    uint32_t *test = (uint32_t*)initial_data;
    
    strcat(str, "READING ");
  
    val = *test;
    
    strcat(str, "READ_DONE ");
    
    val++;
    
    strcat(str, "WRITING ");
    *test = val;

    strcat(str, "WRITE_DONE");

    _task_block();
}

static void usr_task_no_w(uint32_t initial_data) {
    uint32_t val = 0;
    
    strcat(str, "WRITING ");
    g_uno = val;
    strcat(str, "WRITE_DONE");

    _task_block();
}
