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
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


#if !MQX_ENABLE_LOW_POWER
#error This application requires MQX_ENABLE_LOW_POWER to be defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

/* Task IDs */
enum task_ids {
    MAIN_TASK = 1,
    BUTTON_TASK,
    LED_TASK,
#ifdef BSP_LED2
    FOR_LOOP_TASK
#endif /* BSP_LED2 */
};


/* Task prototypes */
void main_task(uint32_t);
void button_task(uint32_t);
void led_task(uint32_t);
#ifdef BSP_LED2
void for_loop_led_task(uint32_t);
#endif /* BSP_LED2 */

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    {
        /* Task Index       */  MAIN_TASK,
        /* Function         */  main_task,
        /* Stack Size       */  2500,
        /* Priority Level   */  10,
        /* Name             */  "Main Task",
        /* Attributes       */  MQX_AUTO_START_TASK,
        /* Creation Params  */  0,
        /* Time Slice       */  0,
    },
    {
        /* Task Index       */  BUTTON_TASK,
        /* Function         */  button_task,
        /* Stack Size       */  256,
        /* Priority Level   */  11,
        /* Name             */  "Button Task",
        /* Attributes       */  MQX_AUTO_START_TASK,
        /* Creation Params  */  0,
        /* Time Slice       */  0,
    },
    {
        /* Task Index       */  LED_TASK,
        /* Function         */  led_task,
        /* Stack Size       */  256,
        /* Priority Level   */  12,
        /* Name             */  "LED Task",
        /* Attributes       */  MQX_AUTO_START_TASK,
        /* Creation Params  */  0,
        /* Time Slice       */  0,
    },
#ifdef BSP_LED2
    {
        /* Task Index       */  FOR_LOOP_TASK,
        /* Function         */  for_loop_led_task,
        /* Stack Size       */  192,
        /* Priority Level   */  13,
        /* Name             */  "For Loop LED Task",
        /* Attributes       */  MQX_AUTO_START_TASK,
        /* Creation Params  */  0,
        /* Time Slice       */  0,
    },
#endif /* BSP_LED2 */
    { 0 }
};


/* local function prototypes */
void wait_for_event (void);

/* LWEVENT masks for app_event */
#define SW_EVENT_MASK          (1 << 0)

LWEVENT_STRUCT                  app_event;


/*FUNCTION*-----------------------------------------------------
*
* Task Name    : display_clock_values
* Comments     : Prints settings for given clock configuration.
*
*END*-----------------------------------------------------------*/

static void display_clock_values
    (
        BSP_CLOCK_CONFIGURATION clock_configuration
    )
{
    printf("CLOCK_SOURCE_CORE     = %d Hz\n",_cm_get_clock(clock_configuration, CM_CLOCK_SOURCE_CORE));
    printf("CLOCK_SOURCE_BUS      = %d Hz\n",_cm_get_clock(clock_configuration, CM_CLOCK_SOURCE_BUS));
    printf("CLOCK_SOURCE_FLEXBUS  = %d Hz\n",_cm_get_clock(clock_configuration, CM_CLOCK_SOURCE_FLEXBUS));
    printf("CLOCK_SOURCE_FLASH    = %d Hz\n",_cm_get_clock(clock_configuration, CM_CLOCK_SOURCE_FLASH));
    printf("CLOCK_SOURCE_USB      = %d Hz\n",_cm_get_clock(clock_configuration, CM_CLOCK_SOURCE_USB));
    printf("CLOCK_SOURCE_PLLFLL   = %d Hz\n",_cm_get_clock(clock_configuration, CM_CLOCK_SOURCE_PLLFLL));
    printf("CLOCK_SOURCE_MCGIR    = %d Hz\n",_cm_get_clock(clock_configuration, CM_CLOCK_SOURCE_MCGIR));
    printf("CLOCK_SOURCE_OSCER    = %d Hz\n",_cm_get_clock(clock_configuration, CM_CLOCK_SOURCE_OSCER));
    printf("CLOCK_SOURCE_ERCLK32K = %d Hz\n",_cm_get_clock(clock_configuration, CM_CLOCK_SOURCE_ERCLK32K));
    printf("CLOCK_SOURCE_MCGFF    = %d Hz\n",_cm_get_clock(clock_configuration, CM_CLOCK_SOURCE_MCGFF));
}


/*FUNCTION*-----------------------------------------------------
*
* Task Name    : wait_for_event
* Comments     : Wait for button press, lwevent is set in button task
*
*END*-----------------------------------------------------------*/

void wait_for_event (void)
{
    _mqx_uint   app_event_value;

    /* Wait for button press */
    _lwevent_wait_ticks (&app_event, (SW_EVENT_MASK), FALSE, 0);

    /* Copy event masks into temporary variable */
    app_event_value = _lwevent_get_signalled();

    /* If button pressed or switch expires, moving to next clock configuration */
    if (app_event_value & SW_EVENT_MASK)
    {
        printf("\nButton pressed. Moving to next clock configuration.\n");
    }
}

/*TASK*-----------------------------------------------------
*
* Task Name    : main_task
* Comments     : Low power modes switching.
*
*END*-----------------------------------------------------*/

void main_task
    (
        uint32_t initial_data
    )
{
    BSP_CLOCK_CONFIGURATION clock_configuration;

    printf("\n***********************************************************\n");
    printf("   MQX frequency change demo\n");
    printf("***********************************************************\n");

    /* Create global event */
    if (_lwevent_create(&app_event, LWEVENT_AUTO_CLEAR) != MQX_OK)
    {
        printf("\nCreating app_event failed.\n");
        _task_block();
    }

    printf("\n***********  DEFAULT CLOCK CONFIGURATION  **********\n");

    /* Get and print active clock configuration */
    clock_configuration =  _lpm_get_clock_configuration();
    display_clock_values(clock_configuration);

    printf("\nPress button on the board to loop over frequency modes.\n");

    while (1)
    {
        /* wait for button press */
        wait_for_event();
        printf("\n***************  BSP_CLOCK_CONFIGURATION_0  **************\n");

        /* Switch clock configuration */
        if (CM_ERR_OK != _lpm_set_clock_configuration(BSP_CLOCK_CONFIGURATION_0))
        {
          printf("Cannot change clock configuration");
          _task_block();
        }

        /* Get active clock configuration */
        clock_configuration =  _lpm_get_clock_configuration();
        /* Display new clock settings */
        display_clock_values(clock_configuration);

         /* wait for button press */
        wait_for_event();
        printf("\n***************  BSP_CLOCK_CONFIGURATION_1  **************\n");

        /* Switch clock configuration */
        if (CM_ERR_OK != _lpm_set_clock_configuration(BSP_CLOCK_CONFIGURATION_1))
        {
          printf("Cannot change clock configuration");
          _task_block();
        }

        /* Get active clock configuration */
        clock_configuration =  _lpm_get_clock_configuration();
        /* Display new clock settings */
        display_clock_values(clock_configuration);

         /* wait for button press */
        wait_for_event();
        printf("\n***************  BSP_CLOCK_CONFIGURATION_2  **************\n");

        /* Switch clock configuration */
        if (CM_ERR_OK != _lpm_set_clock_configuration(BSP_CLOCK_CONFIGURATION_2))
        {
          printf("Cannot change clock configuration");
          _task_block();
        }

        /* Get active clock configuration */
        clock_configuration =  _lpm_get_clock_configuration();
        /* Display new clock settings */
        display_clock_values(clock_configuration);
    }
}


/*TASK*-----------------------------------------------------
*
* Task Name    : button_task
* Comments     :
*
*END*-----------------------------------------------------*/

#define BUTTON_DEBOUNCE_PERIOD_MS        150
#define BUTTON_SAMPLE_PERIOD_MS          50
#define BUTTON_INTEGRATOR_MAXIMUM        (BUTTON_DEBOUNCE_PERIOD_MS / BUTTON_SAMPLE_PERIOD_MS)

void button_task
    (
        uint32_t initial_data
    )
{

    LWGPIO_STRUCT   button;
    LWGPIO_VALUE    button_state      = LWGPIO_VALUE_HIGH;
    uint32_t        button_integrator = BUTTON_INTEGRATOR_MAXIMUM;

    /* set the pin to input */
    if (!lwgpio_init(&button, BSP_BUTTON2, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))    {
        /* Button initialization failed. */
        _task_block();
    }

    /* Set multiplexer to GPIO functionality */
    lwgpio_set_functionality(&button, BSP_SW2_MUX_GPIO);
    /* Enable pull up */
    lwgpio_set_attribute(&button, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);

    while(1)
    {

         /* Update the integrator based on the input signal. */
        if (LWGPIO_VALUE_LOW == lwgpio_get_value(&button))        {
            if (button_integrator > 0)  {
                button_integrator--;
            }
        }
        else if (button_integrator < BUTTON_INTEGRATOR_MAXIMUM)    {
            button_integrator++;
        }

        /* Defensive code if integrator got corrupted */
        if (button_integrator > BUTTON_INTEGRATOR_MAXIMUM)
            button_integrator = BUTTON_INTEGRATOR_MAXIMUM;


        /* Post event if integrator reaches limit */
        if ((button_integrator == 0) )   {
            if (button_state == LWGPIO_VALUE_LOW) {
                button_state = LWGPIO_VALUE_HIGH;
                _lwevent_set(&app_event, SW_EVENT_MASK);
            }
        }
        else {
            button_state = LWGPIO_VALUE_LOW;
        }

        _time_delay(BUTTON_SAMPLE_PERIOD_MS);
    }
}

/*TASK*-----------------------------------------------------
*
* Task Name    : led_task
* Comments     : LEDs setup and blinking. Blinking frequency is constant,
*                MQX tick is core speed independent
*
*END*-----------------------------------------------------*/

void led_task
    (
        uint32_t initial_data
    )
{
    LWGPIO_STRUCT led1;

    /* Initialize LED pin for output */
    if (!lwgpio_init(&led1, BSP_LED1, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))    {
        /* LED1 initialization failed. */
        _task_block();
    }

    /* Set LED pin to GPIO functionality */
    lwgpio_set_functionality(&led1, BSP_LED1_MUX_GPIO);

    while(1)
    {
        _time_delay (500);
        /* toggle led 1 */
        lwgpio_toggle_value(&led1);
    }

}


#ifdef BSP_LED2
/*TASK*-----------------------------------------------------
*
* Task Name    : for_loop_led_task
* Comments     : LEDs setup and blinking, blinking frequency
*                  depends on core speed
*
*END*-----------------------------------------------------*/

void for_loop_led_task
    (
        uint32_t initial_data
    )
{
    LWGPIO_STRUCT     led2;
    volatile uint32_t i = 0;

    /* Initialize LED pin for output */
    if (!lwgpio_init(&led2, BSP_LED2, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
    {
        /* LED2 initialization failed.*/
        _task_block();
    }
    /* Set LED pin to GPIO functionality */
    lwgpio_set_functionality(&led2, BSP_LED2_MUX_GPIO);

    while(1)
    {
        for (i = 0; i < 700000; i++) {};
        /* toggle led 2 */
        lwgpio_toggle_value(&led2);
    }
}
#endif /* BSP_LED2 */
