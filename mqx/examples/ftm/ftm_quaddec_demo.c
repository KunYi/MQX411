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
*   This file provides sample codes of FTM quaddec driver
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <string.h>
#include "ftm.h"
#include "ftm_quaddec.h"

#if ! BSPCFG_ENABLE_FTM1_QD
#error This application requires BSPCFG_ENABLE_FTM1_QD defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#if ! BSPCFG_ENABLE_FTM2_QD
#error This application requires BSPCFG_ENABLE_FTM2_QD defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#define MAIN_TASK       12

bool quadencodertask_run = FALSE;

static const char * ftm1_dev = "ftm1_quaddec:";
static const char * ftm2_dev = "ftm2_quaddec:";

static char * ftm_quaddec_dev = NULL;

void main_task(uint32_t);


const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   /* Task Index, Function,   Stack, Priority,  Name,       Attributes,          Param, Time Slice */
    { MAIN_TASK,  main_task,  4000,  8,         "Main",     MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};

static void ftm_quaddec_example_task
    (
        uint32_t param
    )
{
    uint32_t ftm_event = 0, last_event = 0;
    int32_t base_val = 50;
    MQX_FILE *drv_ptr = (MQX_FILE *)param;


    printf("Entering quad encoder monitor task\n");

    printf ("Please rotate knob to check ......\n");
    printf ("press any key to exit this application.\n");

#if 0
    printf("FTM registers dump:\n");
    ioctl(drv_ptr, IO_IOCTL_FTM_REG_DUMP, (pointer)1);
#endif

    do{
        if (ioctl(drv_ptr, IO_IOCTL_FTM_QUADDEC_GET_EVENT, &ftm_event)
            != MQX_OK){
            printf ("get FTM quadrature decoder event failed \n");
            return;
        }

        switch  (ftm_event) {
        case    QUADDEC_CNT_UP:
                base_val++;
                printf("%d\n", base_val);
                if (last_event != ftm_event) {
                    printf("reversed to increment direction\n");
                    last_event = QUADDEC_CNT_UP;
                }
                break;

        case    QUADDEC_CNT_DOWN:
                base_val--;
                printf("%d\n", base_val);
                if (last_event != ftm_event) {
                    printf("reversed to decrement direction\n");
                    last_event = QUADDEC_CNT_DOWN;
                }
                break;
        }

        _time_delay(20);
     }while (quadencodertask_run == TRUE);

     fclose(drv_ptr);
     drv_ptr = NULL;
     printf("close quaddec device\n");

     return;
}

_mqx_int ftm_quaddec_example_sync_mode
    (
        MQX_FILE *drv_ptr
    )
{
    uint32_t input = 0;
    TASK_TEMPLATE_STRUCT task_temp;
    _task_id tid = MQX_NULL_TASK_ID;

    printf ("===== Set Quaddecoder using polling  =====\n");

    printf ("start a polling task.\n");
    quadencodertask_run = TRUE;

    _mem_zero((void *)&task_temp, sizeof(task_temp));
    task_temp.TASK_ADDRESS      = ftm_quaddec_example_task;
    task_temp.TASK_STACKSIZE    = 512;
    task_temp.TASK_PRIORITY      = 3;
    task_temp.TASK_NAME         = "quad decoder loop example task";
    task_temp.CREATION_PARAMETER = (uint32_t)drv_ptr;

    tid = _task_create(0, 0, (uint32_t)&task_temp);
    if (MQX_NULL_TASK_ID == tid) {
        printf("failed to create quaddec loop example task\n");
        return MQX_INVALID_TASK_ID;
    }

    scanf("%d\n",&input);
    quadencodertask_run = FALSE;

    printf ("===== End of Quaddecoder example =====\n");

    return MQX_OK;
}

static void ftm_quaddec_callback
    (
        void *arg1,
        void* arg2
    )
{
    uint32_t ftm_event = (uint32_t)arg2;
    static int32_t base_val = 50;


     if (QUADDEC_CNT_UP == ftm_event) {
        base_val++;
        printf("%d\n", base_val);
     } else if (QUADDEC_CNT_DOWN == ftm_event) {
        base_val--;
        printf("%d\n", base_val);
     }
}

_mqx_int ftm_quaddec_example_async_mode
    (
        MQX_FILE *drv_ptr
    )
{
    _mqx_int ret = MQX_OK;
    uint32_t input = 0;
    FTM_QUADDEC_CALLBACK_STRUCT ftm_tio_cb;

    printf ("===== Set Quaddecoder using interrupt  =====\n");

    printf ("Installing interrupt callback function.\n");
    ftm_tio_cb.cb_fn = ftm_quaddec_callback;
    ftm_tio_cb.arg = drv_ptr;
    ret = ioctl(drv_ptr, IO_IOCTL_FTM_QUADDEC_SET_CB, &ftm_tio_cb);
    if (MQX_OK != ret){
        printf ("Installing interrupt callback function failed.\n");
        return ret;
    }

    printf ("Please rotate knob to check ......\n");

    printf ("press any key to exit this application.\n");
    scanf("%d\n",&input);

    fclose(drv_ptr);
    drv_ptr = NULL;
    printf("close quaddec device\n");

    return MQX_OK;
}

void main_task
    (
        uint32_t initial_data
    )
{
    _mqx_int ret = MQX_OK;
    uint32_t channel = 0;
    uint32_t inquire_mode = 0;
    int32_t choice = 0;
    MQX_FILE *drv_ptr = NULL;

    printf ("\n\n===== Quaddecoder Driver Example Application =====\n");

    while (1) {
        printf("Please select FTM device, currently, only supports FTM1 and FTM2:\n");
        printf("1. FTM1\n");
        printf("2. FTM2\n");

        scanf("%d", &choice);

        if (1 == choice) {
            ftm_quaddec_dev = (char *)ftm1_dev;
            break;
        } else if (2 == choice) {
            ftm_quaddec_dev = (char *)ftm2_dev;
            break;
        } else {
            printf("invalid choice, please try again...\n");
        }
    }

    printf ("Open quaddecoder device.\n");
    drv_ptr = fopen(ftm_quaddec_dev, "r");
    if (drv_ptr == NULL){
        printf("open quaddec device failed\n");
        _mqx_exit(1);
    }
    else
        printf ("open quaddec device[%s] success\n", ftm_quaddec_dev);

    if (ioctl(drv_ptr, IO_IOCTL_FTM_GET_CHANNEL, &channel) != MQX_OK){
        printf ("get channel failed.\n");
        _mqx_exit (1);
    }

    printf ("channel number: %d\n", channel);

    if (ioctl(drv_ptr, IO_IOCTL_FTM_QUADDEC_GET_MODE, &inquire_mode)
        != MQX_OK){
        printf ("get inquiring mode failed.\n");
        _mqx_exit (1);
    }

    if (QUADDEC_SYNC_MODE == inquire_mode)
        printf ("FTM Quadrature Decoder driver works with sync mode\n");
    else
        printf ("FTM Quadrature Decoder driver works with async mode\n");

    if (QUADDEC_ASYNC_MODE == inquire_mode) {
        ret = ftm_quaddec_example_async_mode(drv_ptr);
        if (MQX_OK != ret) {
            printf("get event failure with async inquiring mode\n");
            _mqx_exit (1);
        }
    } else if (QUADDEC_SYNC_MODE == inquire_mode) {
        ret = ftm_quaddec_example_sync_mode(drv_ptr);
        if (MQX_OK != ret) {
            printf("get event failure with sync inquiring mode\n");
            _mqx_exit (1);
        }
    } else {
        printf("invalid inquiring mode: %d\n", inquire_mode);
        _mqx_exit(1);
    }

    printf("example done\n");
} /* Endbody */

/* EOF */

