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


#if ! MQX_ENABLE_LOW_POWER
#error This application requires MQX_ENABLE_LOW_POWER to be defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#if ! MQX_USE_IDLE_TASK
#error This application requires MQX_USE_IDLE_TASK to be defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


/* Task IDs */
enum task_ids {
    MAIN_TASK = 1,
#ifdef BSP_LED2
    FOR_LOOP_LED_TASK
#endif
};


/* Task prototypes */
void main_task(uint32_t);
void led_task(uint32_t);
#ifdef BSP_LED2
void for_loop_led_task(uint32_t);
#endif
static void timer_wakeup_isr(void *);


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
#ifdef BSP_LED2
    {
        /* Task Index       */  FOR_LOOP_LED_TASK,
        /* Function         */  for_loop_led_task,
        /* Stack Size       */  2500,
        /* Priority Level   */  11,
        /* Name             */  "FOR loop LED Task",
        /* Attributes       */  MQX_AUTO_START_TASK,
        /* Creation Params  */  0,
        /* Time Slice       */  0,
    },
#endif
    { 0 }
};


/* LWEVENT masks for app_event */
#define SW_EVENT_MASK          (1 << 0)
#define TIMER_EVENT_MASK       (1 << 1)
#define ALL_EVENTS_MASK        (SW_EVENT_MASK | TIMER_EVENT_MASK)


LWEVENT_STRUCT app_event;
LWGPIO_STRUCT  led1;

HWTIMER lpttimer;

/*FUNCTION*-----------------------------------------------------
*
* Task Name    : get_power_mode_name
* Comments     : Gets string name for given cpu power mode.
*
*END*-----------------------------------------------------------*/

static char *get_power_mode_name 
    (
        _mqx_uint power_mode_index
    )
{
    char *name = "";
    switch (power_mode_index)
    {
        case LPM_CPU_POWER_MODE_RUN:
            name = "LPM_CPU_POWER_MODE_RUN";
            break;
        case LPM_CPU_POWER_MODE_WAIT:
            name = "LPM_CPU_POWER_MODE_WAIT";
            break;
        case LPM_CPU_POWER_MODE_STOP:
            name = "LPM_CPU_POWER_MODE_STOP";
            break;
        case LPM_CPU_POWER_MODE_VLPR:
            name = "LPM_CPU_POWER_MODE_VLPR";
            break;
        case LPM_CPU_POWER_MODE_VLPW:
            name = "LPM_CPU_POWER_MODE_VLPW";
            break;
        case LPM_CPU_POWER_MODE_VLPS:
            name = "LPM_CPU_POWER_MODE_VLPS";
            break;
        case LPM_CPU_POWER_MODE_LLS:
            name = "LPM_CPU_POWER_MODE_LLS";
            break;
#if (PSP_MQX_CPU_IS_KINETIS)
        case LPM_CPU_POWER_MODE_VLLS3:
            name = "LPM_CPU_POWER_MODE_VLLS3";
            break;
        case LPM_CPU_POWER_MODE_VLLS2:
            name = "LPM_CPU_POWER_MODE_VLLS2";
            break;
        case LPM_CPU_POWER_MODE_VLLS1:
            name = "LPM_CPU_POWER_MODE_VLLS1";
            break;
#endif
        default:
            break;
    }
    return name;
}


/*FUNCTION*-----------------------------------------------------
*
* Task Name    : get_operation_mode_name
* Comments     : Gets string name for given operation mode.
*
*END*-----------------------------------------------------------*/

static char *get_operation_mode_name 
    (
        _mqx_uint operation_mode_index
    )
{
    char *name = "";
    switch (operation_mode_index)
    {
        case LPM_OPERATION_MODE_RUN:
            name = "LPM_OPERATION_MODE_RUN";
            break;
        case LPM_OPERATION_MODE_WAIT:
            name = "LPM_OPERATION_MODE_WAIT";
            break;
        case LPM_OPERATION_MODE_SLEEP:
            name = "LPM_OPERATION_MODE_SLEEP";
            break;
        case LPM_OPERATION_MODE_STOP:
            name = "LPM_OPERATION_MODE_STOP";
            break;
        default:
            break;
    }
    return name;
}


/*FUNCTION*-----------------------------------------------------
*
* Task Name    : display_operation_mode_setting
* Comments     : Prints settings for given operation mode.
*
*END*-----------------------------------------------------------*/

static void display_operation_mode_setting
    (
        LPM_OPERATION_MODE operation_mode
    )
{
    LPM_CPU_OPERATION_MODE core_bsp_setting = LPM_CPU_OPERATION_MODES[operation_mode];
    
    printf("Core Low Power Settings:\n");
    printf("  Mode:      %s \n", get_power_mode_name (core_bsp_setting.MODE_INDEX));
    printf("Wake up events:\n");
    printf("  LLWU_PE1 = 0x%02x Mode wake up events from pins 0..3\n", core_bsp_setting.PE1);
    printf("  LLWU_PE2 = 0x%02x Mode wake up events from pins 4..7\n", core_bsp_setting.PE2);
    printf("  LLWU_PE3 = 0x%02x Mode wake up events from pins 8..11\n", core_bsp_setting.PE3);
    printf("  LLWU_PE4 = 0x%02x Mode wake up events from pins 12..15\n", core_bsp_setting.PE4);
    printf("  LLWU_ME  = 0x%02x Mode wake up events from internal sources\n\n", core_bsp_setting.ME);
}


/*FUNCTION*-----------------------------------------------------
*
* Task Name    : timer_clock_frequency_change
* Comments     : This function demonstrates user LPM registration.
*                The function is called by LPM when it switches
*                between different clock configurations so the
*                timer can adapt the period to new input frequency.
*
*END*-----------------------------------------------------------*/

static LPM_NOTIFICATION_RESULT timer_clock_frequency_change 
    (
        LPM_NOTIFICATION_STRUCT_PTR notification,
        void                       *user_data
    )
{

    if (LPM_NOTIFICATION_TYPE_PRE == notification->NOTIFICATION_TYPE)
    {
        /* Stop the timer before clock configuration changes */
        hwtimer_stop(&lpttimer);
    }

    if (LPM_NOTIFICATION_TYPE_POST == notification->NOTIFICATION_TYPE)
    {
        /* Reinitialize the timer after clock configuration change */
        hwtimer_init(&lpttimer, &lpt_devif, 0, 2);
        hwtimer_set_period(&lpttimer, CM_CLOCK_SOURCE_LPO, 10*1000000);
        hwtimer_callback_reg(&lpttimer,(HWTIMER_CALLBACK_FPTR)timer_wakeup_isr, NULL);
        hwtimer_stop(&lpttimer);
    }
    
    return LPM_NOTIFICATION_RESULT_OK;
}


/*FUNCTION*-----------------------------------------------------
*
* Task Name    : timer_wakeup_isr
* Comments     : This isr is supposed to wake up the CPU core
*                and escape from "interrupts only" low power mode.
*
*END*-----------------------------------------------------------*/

static void timer_wakeup_isr
    (
        void   *parameter
    )
{
    /* Stop the timer */
    hwtimer_stop(&lpttimer);
    
    /* Do not return to sleep after isr again */
    _lpm_wakeup_core ();

    /* Signal the timer event */
    _lwevent_set (&app_event, TIMER_EVENT_MASK);
}


/*FUNCTION*-----------------------------------------------------
*
* Task Name    : install_timer_interrupt
* Comments     : Installs timer with period 10 seconds.
*
*END*-----------------------------------------------------------*/

static void install_timer_interrupt
    (
        void
    )
{
    LPM_REGISTRATION_STRUCT registration;
    uint32_t                dummy_handle;
    uint32_t                result;
    
    /* Install the timer */
    result = hwtimer_init(&lpttimer, &lpt_devif, 0, 2);
    if (MQX_OK != result) {
        printf ("\nError during installation of timer interrupt!\n");
        _task_block();
    }

    result = hwtimer_set_period(&lpttimer, CM_CLOCK_SOURCE_LPO, 10*1000000);
    if (MQX_OK != result) {
        hwtimer_deinit(&lpttimer);
        printf ("\nError during installation of timer interrupt!\n");
        _task_block();
    }

    result = hwtimer_callback_reg(&lpttimer,(HWTIMER_CALLBACK_FPTR)timer_wakeup_isr, NULL);
    if (MQX_OK != result) {
        hwtimer_deinit(&lpttimer);
        printf ("\nError during installation of timer interrupt!\n");
        _task_block();
    }

    result = hwtimer_stop(&lpttimer);
    if (MQX_OK != result) {
        hwtimer_deinit(&lpttimer);
        printf ("\nError during installation of timer interrupt!\n");
        _task_block();
    }

    /* Registration of timer at LPM for clock frequency changes handling */
    registration.CLOCK_CONFIGURATION_CALLBACK = timer_clock_frequency_change;
    registration.OPERATION_MODE_CALLBACK = NULL;
    registration.DEPENDENCY_LEVEL = 10;
    _lpm_register_driver (&registration, (void *)0, &dummy_handle);
}


/*FUNCTION*-----------------------------------------------------
*
* Task Name    : set_timer_wakeup
* Comments     : Run timer with period set to 10 seconds.
*
*END*-----------------------------------------------------------*/

static void set_timer_wakeup
    (
        void
    )
{
    hwtimer_start(&lpttimer);
    printf ("\nTimer wakeup set to +10 seconds.\n");
}


/*FUNCTION*-----------------------------------------------------
*
* Task Name    : button_isr
* Comments     : SW interrupt callback used to catch SW press
*                and trigger SW_EVENT
*
*END*-----------------------------------------------------------*/

static void button_isr
    (
         void   *pin
    )
{
    lwgpio_toggle_value (&led1);
    lwgpio_int_clear_flag (pin);
    _lwevent_set (&app_event, SW_EVENT_MASK);
}


/*FUNCTION*-----------------------------------------------------
*
* Task Name    : button_led_init
* Comments     : Setup the button to trigger interrupt on each button press.
*
*END*-----------------------------------------------------------*/

static void button_led_init
    (
        void
    )
{
    static LWGPIO_STRUCT                   sw;
    /* Set the pin to input */
    if (!lwgpio_init(&sw, BSP_SW2, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("\nSW initialization failed.\n");
        _task_block();
    }

    /* Set functionality to GPIO mode */
    lwgpio_set_functionality(&sw, BSP_SW2_MUX_GPIO);
    
    /* Enable pull up */
    lwgpio_set_attribute(&sw, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);

    /* Setup the pin interrupt mode */
    if (!lwgpio_int_init(&sw, LWGPIO_INT_MODE_FALLING))
    {
        printf("Initializing SW for interrupt failed.\n");
        _task_block();
    }

    /* Install gpio interrupt service routine */
    _int_install_isr(lwgpio_int_get_vector(&sw), button_isr, (void *) &sw);
    
    /* Set interrupt priority and enable interrupt source in the interrupt controller */
    _bsp_int_init(lwgpio_int_get_vector(&sw), 3, 0, TRUE);
    
    /* Enable interrupt for pin */
    lwgpio_int_enable(&sw, TRUE);

    /* Initialize LED pin for output */
    if (!lwgpio_init(&led1, BSP_LED1, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
    {
        printf("\nLED1 initialization failed.\n");
        _task_block();
    }
    /* Set LED pin to GPIO functionality */
    lwgpio_set_functionality(&led1, BSP_LED1_MUX_GPIO);
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
    LPM_OPERATION_MODE operation_mode;
    IDLE_LOOP_STRUCT   idle_loops;
    uint32_t            loop1;

    /* Initialize switches */
    button_led_init();
    
    _int_install_unexpected_isr();
    _lpm_register_wakeup_callback(BSP_LLWU_INTERRUPT_VECTOR, BSP_LLWU_INTERRUPT_PRIORITY, NULL);

    /* Install interrupt for timer wakeup */
    install_timer_interrupt();
    
    /* Create global event */
    if (_lwevent_create(&app_event, 0) != MQX_OK)
    {
        printf("\nCreating app_event failed.\n");
        _task_block();
    }

#if (PSP_MQX_CPU_IS_KINETIS)
    if (_lpm_get_reset_source() != MQX_RESET_SOURCE_LLWU)
        printf("\nMQX Low Power Modes Demo\n");
    else
#endif
        printf("\nWake up by reset from LLWU\n");

    while (1)
    {
        /* Find out current mode setting */
        operation_mode = _lpm_get_operation_mode();

        printf("\n******************************************************************************\n");
        printf("************** Operation mode : %s ***********************\n", get_operation_mode_name (operation_mode));
        printf("******************************************************************************\n");

        display_operation_mode_setting(operation_mode);

        printf(
        "Info: RUN operation mode is mapped on RUN power mode by default.\n"
        "      The core runs at full clock speed.\n"
        "      It continues the execution after entering the mode.\n"
        "      LED2 blinks quickly, LED1 toggles after the button press.\n");

        /* Demonstration of idle task sleep feature */
        printf("\nIdle task sleep feature disabled.\n");
        _lpm_idle_sleep_setup (FALSE);
        
        printf("Task suspended for 1 second to let run the idle task.\n");
        _mqx_get_idle_loop_count (&idle_loops);
        loop1 = idle_loops.IDLE_LOOP1;
        _time_delay (1000);
        
        _mqx_get_idle_loop_count (&idle_loops);
        printf ("Idle loops per second with idle sleep disabled: %d\n", idle_loops.IDLE_LOOP1 - loop1);
        
        printf("Idle task sleep feature enabled.\n");
        _lpm_idle_sleep_setup (TRUE);
        
        printf("Task suspended for 1 second to let run the idle task.\n");
        _mqx_get_idle_loop_count (&idle_loops);
        loop1 = idle_loops.IDLE_LOOP1;
        _time_delay (1000);
        
        _mqx_get_idle_loop_count (&idle_loops);
        printf ("Idle loops per second with idle sleep enabled:  %d\n", idle_loops.IDLE_LOOP1 - loop1);

        printf("Idle task sleep feature disabled.\n\n");
        _lpm_idle_sleep_setup (FALSE);
        
        /* Wait for button press */
        printf ("Press button to move to next operation mode.\n");
        _lwevent_wait_ticks (&app_event, SW_EVENT_MASK, FALSE, 0);
        _lwevent_clear (&app_event, ALL_EVENTS_MASK);
        printf("\nButton pressed. Moving to next operation mode.\n");

        operation_mode = LPM_OPERATION_MODE_WAIT;
        
        printf("\n******************************************************************************\n");
        printf("************** Operation mode : %s **********************\n", get_operation_mode_name (operation_mode));
        printf("******************************************************************************\n");

        display_operation_mode_setting(operation_mode);

        printf(
        "Info: WAIT operation mode is mapped on VLPR power mode by default.\n"
        "      It requires 2 MHz core clock and bypassed pll.\n"
        "      Core continues the execution after entering the mode.\n"
        "      LED2 blinks slowly, LED1 toggles after the button press.\n");

        /* The LPM_OPERATION_MODE_WAIT is mapped on LPM_CPU_POWER_MODE_VLPR by default,
        this mode requires 2 MHz, bypassed PLL clock setting. Change clocks to appropriate mode */
        printf("\nChanging frequency to 2 MHz.\n");
        if (CM_ERR_OK != _lpm_set_clock_configuration(BSP_CLOCK_CONFIGURATION_2MHZ))
        {
            printf("Cannot change clock configuration");
            _task_block();
        }
        /* Change the operation mode */
        printf ("\nSetting operation mode to %s ... ", get_operation_mode_name (operation_mode));
        printf ("%s\n", _lpm_set_operation_mode (LPM_OPERATION_MODE_WAIT) == 0 ? "OK" : "ERROR");

        /* Wait for button press */
        printf ("\nPress button to move to next operation mode.\n");
        _lwevent_wait_ticks (&app_event, SW_EVENT_MASK, FALSE, 0);
        _lwevent_clear (&app_event, ALL_EVENTS_MASK);
        printf("\nButton pressed.\n");

        /* Return to RUN mode */
        printf ("\nSetting operation mode back to %s ... ", get_operation_mode_name (LPM_OPERATION_MODE_RUN));
        printf ("%s\n", _lpm_set_operation_mode (LPM_OPERATION_MODE_RUN) == 0 ? "OK" : "ERROR");

        /* Return default clock configuration */
        printf("\nChanging frequency back to the default one.\n");
        if (CM_ERR_OK != _lpm_set_clock_configuration(BSP_CLOCK_CONFIGURATION_DEFAULT))
        {
            printf("Cannot change clock configuration");
            _task_block();
        }

        printf("\nMoving to next operation mode.\n");

        operation_mode = LPM_OPERATION_MODE_SLEEP;

        printf("\n******************************************************************************\n");
        printf("************** Operation mode : %s *********************\n", get_operation_mode_name (operation_mode));
        printf("******************************************************************************\n");

        display_operation_mode_setting(operation_mode);

        printf(
        "Info: SLEEP operation mode is mapped on WAIT power mode by default.\n"
        "      The core is inactive in this mode, reacting only to interrupts.\n"
        "      The LPM_CPU_POWER_MODE_FLAG_SLEEP_ON_EXIT is set on Kinetis, therefore\n"
        "      core goes to sleep again after any isr finishes. The core will stay awake\n"
        "      after call to _lpm_wakeup_core() from timer wakeup or serial interrupt.\n"
        "      LED2 doesn't blink, LED1 toggles after the button press.\n");

        /* Wake up in 10 seconds */
        set_timer_wakeup ();

        /* Change the operation mode */
        printf ("\nSetting operation mode to %s ... ", get_operation_mode_name (operation_mode));
        printf ("%s\n", _lpm_set_operation_mode (operation_mode) == 0 ? "OK" : "ERROR");
        
        if (LWEVENT_WAIT_TIMEOUT == _lwevent_wait_ticks (&app_event, TIMER_EVENT_MASK, FALSE, 1))
        {
            printf("\nCore woke up by interrupt. Waiting for timer wakeup ... ");
            _lwevent_wait_ticks (&app_event, TIMER_EVENT_MASK, FALSE, 0);
            printf("OK\n");
        }
        else
        {
            printf("\nCore woke up by timer wakeup.\n");
        }
        _lwevent_clear (&app_event, ALL_EVENTS_MASK);
        
        /* Wait for button press */
        printf ("\nPress button to move to next operation mode.\n");
        _lwevent_wait_ticks (&app_event, SW_EVENT_MASK, FALSE, 0);
        _lwevent_clear (&app_event, ALL_EVENTS_MASK);
        printf("\nButton pressed. Moving to next operation mode.\n");

        operation_mode = LPM_OPERATION_MODE_STOP;

        printf("\n******************************************************************************\n");
        printf("************** Operation mode : %s **********************\n", get_operation_mode_name (operation_mode));
        printf("******************************************************************************\n");

        display_operation_mode_setting(operation_mode);

        printf(
        "Info: STOP operation mode is mapped to LLS power mode by default.\n"
        "      Core and most peripherals are inactive in this mode, reacting only to\n"
        "      specified wake up events. The events can be changed in BSP (init_lpm.c).\n"
        "      Serial line is turned off in this mode. The core will wake up from\n"
        "      timer wakeup interrupt.\n"
        "      LED2 doesn't blink, LED1 toggles after the button press.\n");

        /* Wake up in 10 seconds */
        set_timer_wakeup ();

        /* Change the operation mode */
        printf ("\nSetting operation mode to %s ... \n", get_operation_mode_name (operation_mode));
        _lpm_set_operation_mode (operation_mode);

        /**************************************************************************************************/
        /* SCI HW MODULE IS DISABLED AT THIS POINT - SERIAL DRIVER MUST NOT BE USED UNTIL MODE IS CHANGED */
        /**************************************************************************************************/
        
        /* Return to RUN mode */
        _lpm_set_operation_mode (LPM_OPERATION_MODE_RUN);
        
        printf("\nCore is awake. Moved to next operation mode.\n");
    }
}


#ifdef BSP_LED2
/*TASK*-----------------------------------------------------
*
* Task Name    : for_loop_led_task
* Comments     : LEDs setup and blinking, blinking frequency depends on core speed
*
*END*-----------------------------------------------------*/

void for_loop_led_task
    (
        uint32_t initial_data
    )
{
    volatile uint32_t i = 0;
    LWGPIO_STRUCT led2;

    /* Initialize LED pin for output */
    if (!lwgpio_init(&led2, BSP_LED2, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH))
    {
        printf("\nLED2 initialization failed.\n");
        _task_block();
    }
    /* Set LED pin to GPIO functionality */
    lwgpio_set_functionality(&led2, BSP_LED2_MUX_GPIO);
    
    while(1)
    {
        /* Duration of this loop depends on actual core clock */
        for (i = 0; i < 800000; i++) {};
        
        /* To let the idle task run too */
        _time_delay (50);
        
        /* Toggle led 2 */
        lwgpio_toggle_value(&led2);
    }
}
#endif /* BSP_LED2 */
