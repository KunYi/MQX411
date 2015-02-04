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
#include <lwsem.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

/*hwtimer example defines*/
#define HWTIMER1_FREQUENCY  10          //frequency set to hwtimer 1
#define HWTIMER2_PERIOD     1000000     //period set to 1s to hwtimer 2
/* Task IDs */
#define HWTIMER_TASK 5

/* local function prototypes */
void hwtimer_task(uint32_t );
static void hwtimer1_callback(void *p);
static void hwtimer2_callback(void *p);

HWTIMER hwtimer1;                               //hwtimer handle
HWTIMER hwtimer2;                               //hwtimer handle
LWSEM_STRUCT job_done;                          //semaphore for end of job
LWGPIO_STRUCT led1, led2;                       //Structures holding information about specific pin
const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index,   Function,   Stack,  Priority, Name,     Attributes,          Param, Time Slice */
    { HWTIMER_TASK,  hwtimer_task, 1500,   8,   "hwtimer",  MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};

/*FUNCTION*----------------------------------------------------------------*
* Function Name  : hwtimer1_callback
* Returned Value : void
* Comments :
*       Callback for hwtimer1
*END*--------------------------------------------------------------------*/
static void hwtimer1_callback(void *p)
{
#ifdef BSP_LED1
    lwgpio_toggle_value(&led1);
#endif
}

/*FUNCTION*----------------------------------------------------------------*
* Function Name  : hwtimer2_callback
* Returned Value : void
* Comments :
*   Callback for hwtimer2
*
*END*--------------------------------------------------------------------*/
static void hwtimer2_callback(void *p)
{
    int ticks = hwtimer_get_ticks(&hwtimer2);
    
    if(1 == ticks)
    {
        /* Start hwtimer1*/
    #ifdef BSP_LED2
        lwgpio_toggle_value(&led2);
    #endif
        hwtimer_start(&hwtimer1);
    }
    if(2 == ticks)
    {
        /* block hwtimer1_callback */
        hwtimer_callback_block(&hwtimer1);
    #ifdef BSP_LED2
        lwgpio_toggle_value(&led2);
    #endif
    }
    if(4 == ticks)
    {
        /* unblock hwtimer1_callback */
    #ifdef BSP_LED2
        lwgpio_toggle_value(&led2);
    #endif
        hwtimer_callback_cancel(&hwtimer1); // Clear pending callback for hwtimer1
        hwtimer_callback_unblock(&hwtimer1);
    }
    if(6 == ticks)
    {
    #ifdef BSP_LED2
        lwgpio_toggle_value(&led2);
    #endif
        hwtimer_stop(&hwtimer1);
        /* Stop hwtimer1 */
        hwtimer_stop(&hwtimer2);
        /* Stop hwtimer2 */
        _lwsem_post(&job_done);
    }
}

/*TASK*-----------------------------------------------------
*
* Task Name    : hwtimer_task
* Comments     :
*    This task prints " HWTIMER"
*
*END*-----------------------------------------------------*/
void hwtimer_task
    (
        uint32_t initial_data
    )
{
    printf("\n--------------------------START OF HWTIMER EXAMPLE--------------------------\n");

#ifdef BSP_LED1
    /* initialize lwgpio handle (led1) for BSP_LED1 pin (defined in mqx/source/bsp/<bsp_name>/<bsp_name>.h file) */
    if (!lwgpio_init(&led1, BSP_LED1, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing LED1 GPIO as output failed.\n");
        _task_block();
    }
    /* swich pin functionality (MUX) to GPIO mode */
    lwgpio_set_functionality(&led1, BSP_LED1_MUX_GPIO);

    /* write logical 1 to the pin */
    lwgpio_set_value(&led1, LWGPIO_VALUE_HIGH); /* set pin to 1 */
#endif

#ifdef BSP_LED2
    /* initialize lwgpio handle (led1) for BSP_LED1 pin (defined in mqx/source/bsp/<bsp_name>/<bsp_name>.h file) */
    if (!lwgpio_init(&led2, BSP_LED2, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing LED1 GPIO as output failed.\n");
        _task_block();
    }
    /* swich pin functionality (MUX) to GPIO mode */
    lwgpio_set_functionality(&led2, BSP_LED1_MUX_GPIO);

    /* write logical 1 to the pin */
    lwgpio_set_value(&led2, LWGPIO_VALUE_HIGH); /* set pin to 1 */
#endif

    if (MQX_OK != _lwsem_create(&job_done, 0))
    {
        printf("Failed to create semaphore");
        _task_block();
    }
    /* Initialization of hwtimer1*/
    printf("Initialization of hwtimer1   :");
    if (MQX_OK != hwtimer_init(&hwtimer1, &BSP_HWTIMER1_DEV, BSP_HWTIMER1_ID, (BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX + 1)))
    {
        printf(" FAILED!\n");
    }
    else
    {
        printf(" OK\n");
    }
    printf("Try to set frequency %d Hz to hwtimer1\n",HWTIMER1_FREQUENCY);
    hwtimer_set_freq(&hwtimer1, BSP_HWTIMER1_SOURCE_CLK, HWTIMER1_FREQUENCY);
    printf("Read frequency from hwtimer1 : %d Hz\n", hwtimer_get_freq(&hwtimer1));
    printf("Read period from hwtimer1    : %d us\n", hwtimer_get_period(&hwtimer1));
    printf("Read modulo from hwtimer1    : %d\n", hwtimer_get_modulo(&hwtimer1));
    /* Register isr for hwtimer1 */
    printf("Register callback for hwtimer1\n");
    hwtimer_callback_reg(&hwtimer1,(HWTIMER_CALLBACK_FPTR)hwtimer1_callback, NULL);

    /* Initialization of hwtimer2*/
    printf("\nInitialization of hwtimer2   :");
    if (MQX_OK != hwtimer_init(&hwtimer2, &BSP_HWTIMER2_DEV, BSP_HWTIMER2_ID, (BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX + 1)))
    {
        printf(" FAILED!\n");
    }
    else
    {
        printf(" OK\n");
    }
    printf("Try to set period %d us to hwtimer2\n", HWTIMER2_PERIOD);
    hwtimer_set_period(&hwtimer2, BSP_HWTIMER2_SOURCE_CLK, HWTIMER2_PERIOD);
    printf("Read frequency from hwtimer2 : %d Hz\n", hwtimer_get_freq(&hwtimer2));
    printf("Read period from hwtimer2    : %d us\n", hwtimer_get_period(&hwtimer2));
    printf("Read modulo from hwtimer2    : %d\n", hwtimer_get_modulo(&hwtimer2));
    /* Register isr for hwtimer2 */
    printf("Register callback for hwtimer2\n");
    hwtimer_callback_reg(&hwtimer2,(HWTIMER_CALLBACK_FPTR)hwtimer2_callback, NULL);
    /* Start hwtimer1*/
    printf("Start hwtimer2\n");
    hwtimer_start(&hwtimer2);

    /* Waiting for semaphore from callback routine */
    _lwsem_wait(&job_done);
    printf("hwtimer2 = %d ticks, hwtimer1 = %d ticks\n",hwtimer_get_ticks(&hwtimer2), hwtimer_get_ticks(&hwtimer1));
    /* Deinitialization of hwtimers */
    printf("Deinit hwtimer1\n");
    hwtimer_deinit(&hwtimer1);
    printf("Deinit hwtimer2\n");
    hwtimer_deinit(&hwtimer2);

    printf("---------------------------END OF HWTIMER EXAMPLE---------------------------\n");
    _lwsem_destroy(&job_done);
}

/* EOF */
