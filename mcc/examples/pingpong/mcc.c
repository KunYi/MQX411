/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
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
*   This file contains the source for one of the MCC pingpong examples.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "mcc.h"
#include "mcc_config.h"
#include "mcc_common.h"
#include "mcc_api.h"
#include "mcc_mqx.h"
#include <string.h>

#include <core_mutex.h>

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

/* Application-specific settings */
/* 1: Blocking mode (task is blocked when no new message is available)
 * 0: Non-blocking mode (task is waiting for a new message in a spin loop) */
#define MCC_APP_BLOCKING_MODE  (1)

extern void main_task(uint32_t);
extern void responder_task(uint32_t);

#if PSP_MQX_CPU_IS_VYBRID_A5
const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   /* Task Index,    Function,       Stack,  Priority, Name,        Attributes,          Param, Time Slice */
    { MAIN_TTN,      main_task,      2000,   9,        "Main",      MQX_AUTO_START_TASK, MCC_MQX_NODE_A5,   0 },
    { 0 }
};
#else
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   /* Task Index,    Function,       Stack,  Priority,  Name,        Attributes,          Param, Time Slice */
    { RESPONDER_TTN, responder_task, 2000,   9,         "Responder", MQX_AUTO_START_TASK, MCC_MQX_NODE_M4,  0 },
    { 0 }
};
#endif

MCC_ENDPOINT    mqx_endpoint_a5 = {0,MCC_MQX_NODE_A5,MCC_MQX_SENDER_PORT};
MCC_ENDPOINT    mqx_endpoint_m4 = {1,MCC_MQX_NODE_M4,MCC_MQX_RESPONDER_PORT};

/*TASK*----------------------------------------------------------
*
* Task Name : main_task
* Comments  :
*     This task creates a message pool and a message queue then
*     sends a message to a queue on the second CPU.
*     It waits for a return message, validating the message before
*     sending a new message.
*END*-----------------------------------------------------------*/

void main_task(uint32_t node_num)
{
    THE_MESSAGE     msg;
    MCC_MEM_SIZE    num_of_received_bytes;
    CORE_MUTEX_PTR  coremutex_app_ptr;
    MCC_INFO_STRUCT mcc_info;
    int             ret_value;
#if !MCC_APP_BLOCKING_MODE
    unsigned int    num_available_msgs;
#endif

    /* create core mutex used in the app. for accessing the serial console */
    coremutex_app_ptr = _core_mutex_create( 0, MCC_PRINTF_SEMAPHORE_NUMBER, MQX_TASK_QUEUE_FIFO );

    msg.DATA = 1;
    ret_value = mcc_initialize(node_num);
    ret_value = mcc_get_info(node_num, &mcc_info);
    if(MCC_SUCCESS == ret_value && (strcmp(mcc_info.version_string, MCC_VERSION_STRING) != 0)) {
        _core_mutex_lock(coremutex_app_ptr);
        printf("\n\n\nError, attempting to use different versions of the MCC library on each core! Application is stopped now.\n");
        _core_mutex_unlock(coremutex_app_ptr);
        mcc_destroy(node_num);
        _task_block();
    }
    ret_value = mcc_create_endpoint(&mqx_endpoint_a5, MCC_MQX_SENDER_PORT);

    _core_mutex_lock(coremutex_app_ptr);
    printf("\n\n\nMain task started, MCC version is %s\n", mcc_info.version_string);
    _core_mutex_unlock(coremutex_app_ptr);

    while (1) {
        /* wait until the remote endpoint is created by the other core */
        while(MCC_ERR_ENDPOINT == mcc_send(&mqx_endpoint_m4, &msg, sizeof(THE_MESSAGE), 0xffffffff)) {
            _time_delay(1);
        }
#if MCC_APP_BLOCKING_MODE
        ret_value = mcc_recv_copy(&mqx_endpoint_a5, &msg, sizeof(THE_MESSAGE), &num_of_received_bytes, 0xffffffff);
#else
        mcc_msgs_available(&mqx_endpoint_a5, &num_available_msgs);
        while(num_available_msgs==0) {
        	mcc_msgs_available(&mqx_endpoint_a5, &num_available_msgs);
        }
        ret_value = mcc_recv_copy(&mqx_endpoint_a5, &msg, sizeof(THE_MESSAGE), &num_of_received_bytes, 0);
#endif
        if(MCC_SUCCESS != ret_value) {
            _core_mutex_lock(coremutex_app_ptr);
            printf("Main task receive error: %i\n", ret_value);
            _core_mutex_unlock(coremutex_app_ptr);
        } else {
            _core_mutex_lock(coremutex_app_ptr);
            printf("Main task received a msg\n");
            printf("Message: Size=%x, DATA = %x\n", num_of_received_bytes, msg.DATA);
            _core_mutex_unlock(coremutex_app_ptr);
            msg.DATA++;
        }
    }
}

/*TASK*----------------------------------------------------------
*
* Task Name : responder_task
* Comments  :
*     This task creates a message queue then waits for a message.
*     Upon receiving the message the data is incremented before
*     the message is returned to the sender.
*END*-----------------------------------------------------------*/

void responder_task(uint32_t node_num)
{
    THE_MESSAGE     msg;
    MCC_MEM_SIZE    num_of_received_bytes;
    CORE_MUTEX_PTR  coremutex_app_ptr;
    MCC_INFO_STRUCT mcc_info;
    int             ret_value;
#if !MCC_APP_BLOCKING_MODE
    unsigned int    num_available_msgs;
#endif

    /* create core mutex used in the app. for accessing the serial console */
    coremutex_app_ptr = _core_mutex_create( 0, MCC_PRINTF_SEMAPHORE_NUMBER, MQX_TASK_QUEUE_FIFO );

    msg.DATA = 1;
    ret_value = mcc_initialize(node_num);
    ret_value = mcc_get_info(node_num, &mcc_info);
    if(MCC_SUCCESS == ret_value && (strcmp(mcc_info.version_string, MCC_VERSION_STRING) != 0)) {
        _core_mutex_lock(coremutex_app_ptr);
        printf("\n\n\nError, attempting to use different versions of the MCC library on each core! Application is stopped now.\n");
        _core_mutex_unlock(coremutex_app_ptr);
        mcc_destroy(node_num);
        _task_block();
    }
    ret_value = mcc_create_endpoint(&mqx_endpoint_m4, MCC_MQX_RESPONDER_PORT);

    _core_mutex_lock(coremutex_app_ptr);
    printf("\n\n\nResponder task started, MCC version is %s\n", mcc_info.version_string);
    _core_mutex_unlock(coremutex_app_ptr);
    while (TRUE) {
#if MCC_APP_BLOCKING_MODE
        ret_value = mcc_recv_copy(&mqx_endpoint_m4, &msg, sizeof(THE_MESSAGE), &num_of_received_bytes, 0xffffffff);
#else
        mcc_msgs_available(&mqx_endpoint_m4, &num_available_msgs);
        while(num_available_msgs==0) {
            mcc_msgs_available(&mqx_endpoint_m4, &num_available_msgs);
        }
        ret_value = mcc_recv_copy(&mqx_endpoint_m4, &msg, sizeof(THE_MESSAGE), &num_of_received_bytes, 0);
#endif
        if(MCC_SUCCESS != ret_value) {
            _core_mutex_lock(coremutex_app_ptr);
            printf("Responder task receive error: %i\n", ret_value);
            _core_mutex_unlock(coremutex_app_ptr);
        } else {
            _core_mutex_lock(coremutex_app_ptr);
            printf("Responder task received a msg\n");
            printf("Message: Size=%x, DATA = %x\n", num_of_received_bytes, msg.DATA);
            _core_mutex_unlock(coremutex_app_ptr);
            msg.DATA++;
            ret_value = mcc_send(&mqx_endpoint_a5, &msg, sizeof(THE_MESSAGE), 0xffffffff);
        }
    }
}
