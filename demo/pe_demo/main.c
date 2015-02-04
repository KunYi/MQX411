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
*   This example demonstrates how to use Processor Expert to configure MQX BSP.
*   The application demo
*   1. The PWM signal is generated using FlexTimer FTM0 Channel 0
*      It can be observed by scope on PWM0 - A40 pin
*      on TWR-ELEV FUNCTIONAL or TWR-PROTO board.
*   2. It also toggles by LED1 on board using GPIO driver
*      to signalize that application is running.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h.
/* Please recompile BSP with this option. */
#endif


#ifndef PE_LDD_VERSION
#error Demo requires Processor Expert BSP code to be generated. 
#error See "<MQX_INSTALL_DIR>\doc\tools\cw\MQX_CW10_Getting_Started.pdf" chapter "CW10.x, MQX and Processor Expert" for details 
#else

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL.
/*
 * Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h
 * and recompile BSP with this option.
 */
#endif

/* Task enumerations and prototypes */
enum {
    PWM_TASK = 1,
    LED_TASK
} etask_type;

void pwm_task(uint32_t);
void led_task(uint32_t);

/* Task template list */
const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   /* Task Index,   Function,   Stack,  Priority,   Name,        Attributes,          Param,   Time Slice */
    { PWM_TASK,     pwm_task,    800,       8,      "PWM Task",  MQX_AUTO_START_TASK,   0,         0 },
    { LED_TASK,     led_task,    400,       9,      "LED Task",  MQX_AUTO_START_TASK,   0,         0 },
    { 0 }
};


#define TERMINAL_CURSOR_POSITION_MAX    (80)

/*TASK*-------------------------------------------------------------------------
*
* Task Name    : pwm_task
* Comments     :
*
*END*-------------------------------------------------------------------------*/

static int                  pwm_task_count;
static LDD_TDeviceData     *PWM_DeviceData;
static LDD_TError           PWM_Error;
volatile PWM_TValueType     PWM_Value;
volatile PWM_TValueType     PWM_MaxValue;
volatile PWM_TValueType     PWM_Step;
volatile int                PWM_Freguency;
void pwm_task
   (
      uint32_t initial_data
   )
{
    static int terminal_cursor_position = 1;

    /* Print example functional description on terminal */
    puts("\n");
    puts("\n********************************************************************************");
    puts("\n*");
    puts("\n*   This example demonstrates how to use Processor Expert to configure MQX BSP.");
    puts("\n*   The application demo :");
    puts("\n*   1. The PWM signal is generated using FlexTimer FTM0 Channel 0");
    puts("\n*      It can be observed by scope on PWM0 - A40 pin");
    puts("\n*      on TWR-ELEV FUNCTIONAL or TWR-PROTO board.");
    puts("\n*   2. It also toggles LED1 on board using GPIO driver");
    puts("\n*      to signalize that application is running.");
    puts("\n*");
    puts("\n********************************************************************************");
    puts("\n");

    /* Initialize PWM device on FTM0 device */
     puts("\nInitializing PWM device.....");
    PWM_DeviceData = PWM_Init(NULL);
    if (PWM_DeviceData == NULL)  {
    puts("failed");
        _task_block();
    }
    else  {
        puts("done");
    }

    PWM_Value       = 0;
    PWM_Step        = PWM_PERIOD_TICKS / 32;
    PWM_MaxValue    = PWM_PERIOD_TICKS;
    PWM_Freguency   = (PWM_CNT_INP_FREQ_U_0/PWM_PERIOD_TICKS);

  printf("\n - PWM frequency              = %d Hz", PWM_Freguency);
    puts("\nThe PWM signal is generated on FTM0 Channel 0");
    puts("\n");
    /* Enable PWM device */
    PWM_Error = PWM_Enable(PWM_DeviceData);

    while(1)
    {
        pwm_task_count++;

        /* Suspend task for 250ms */
        _time_delay(250);

        /* Print dot on console to see that application is running */
        if (terminal_cursor_position++ > TERMINAL_CURSOR_POSITION_MAX) {
            terminal_cursor_position = 1;
            puts("\n");
        }
        else {
            puts(".");
        }
    }
}

/*
** ===================================================================
**     Event       :  PWM_OnCounterRestart (overloads function in module Events)
**
**     Component   :  PWM [TimerUnit_LDD]
**     Description :
**         Called if counter overflow/underflow or counter is
**         reinitialized by modulo or compare register matching.
**         OnCounterRestart event and Timer unit must be enabled. See
**         <SetEventMask> and <GetEventMask> methods. This event is
**         available only if a <Interrupt> is enabled.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * UserDataPtr     - Pointer to the user or
**                           RTOS specific data. The pointer passed as
**                           the parameter of Init method.
**     Returns     : Nothing
** ===================================================================
*/

void PWM_OnCounterRestart(LDD_TUserData *UserDataPtr)
{
    /* Increment PWM duty-cycle from 0-100% */
    PWM_Value += PWM_Step;

    if (PWM_Value > PWM_MaxValue) PWM_Value = 0;

    /* Set new PWM channel value */
    PWM_Error = PWM_SetOffsetTicks(PWM_DeviceData, 0, PWM_Value);
}


/*TASK*-------------------------------------------------------------------------
*
* Task Name    : led_task
* Comments     :
*
*END*-------------------------------------------------------------------------*/

/* LED */
LDD_TDeviceData     *LED_DeviceData;
LDD_TError           LED_Error;

static int           led_count;

void led_task
   (
      uint32_t initial_data
   )
{
    /* Initialize GPIO  */
    puts("\nInitializing LED device.....");
    LED_DeviceData = GPIO1_Init(NULL);
    if (LED_DeviceData == NULL)    {
        puts("failed");
        _task_block();
    }
    else {
        puts("done");
    }

    puts("\n");

    while(1)
    {
        led_count++;

        _time_delay(500);

        GPIO1_ToggleFieldBits(LED_DeviceData, LED1, 1);
    }
}
#endif /* PE_LDD_VERSION */ 
/* EOF */
