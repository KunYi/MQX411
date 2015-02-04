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
*   This file contains the source for the power mode circle demo.
*   RUN/WAIT/SLEEP/STOP mode is circled, SW1 is used as a wakeup
*   source. Dump message and LEDs will tell user what is the
*   current mode.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h> 
#include <fio.h>
#include <lwevent.h>


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
#define MAIN_TASK 5

#define DELAY_CNT 50 

#define SW_EVENT_MASK          (1 << 0)

#define BUTTON_NOT_PRESSED      0
#define BUTTON_PRESSED          1


#if defined(BSP_TWR_VF65GS10_A5) ||  defined(BSP_TWR_VF65GS10_M4)
#define INT_BUTTON          BSP_BUTTON2     /*SW2*/
#define INT_BUTTON_FUNC     BSP_BUTTON2_MUX_IRQ   
#define WAKEUP_BUTTON       BSP_BUTTON1     /*SW1*/
#define WAKEUP_BUTTON_FUNC  BSP_BUTTON1_MUX_IRQ
#define WKPU_ID             11              /*SW1 is PTB16, which is WKPU_P11*/
#elif defined(BSP_VYBRID_AUTOEVB_A5) || defined(BSP_VYBRID_AUTOEVB_M4)
#define INT_BUTTON          BSP_BUTTON1     /*SW6*/
#define INT_BUTTON_FUNC     BSP_BUTTON1_MUX_IRQ   
#define WAKEUP_BUTTON       BSP_BUTTON4     /*SW9*/
#define WAKEUP_BUTTON_FUNC  BSP_BUTTON4_MUX_IRQ
#define WKPU_ID             14              /*SW9 is PTB27, which is WKPU_P14*/
#endif

LWEVENT_STRUCT app_event;

extern void main_task(uint32_t);


const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{ 
    /* Task Index,   Function,   Stack,  Priority, Name,     Attributes,          Param, Time Slice */
    { MAIN_TASK,   main_task, 1500,   8,        "hello",  MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};

LWGPIO_STRUCT led1, led2, led3, led4;

void init_leds(void)
{
    /*
     * The LED1 and LED4 are reverse for tower board
     */
#if defined(BSP_TWR_VF65GS10_A5) ||  defined(BSP_TWR_VF65GS10_M4)
    if (!lwgpio_init(&led1, BSP_LED4, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing LED1 GPIO as output failed.\n");
        _task_block();
    }
    lwgpio_set_functionality(&led1, BSP_LED4_MUX_GPIO);
#elif defined(BSP_VYBRID_AUTOEVB_A5) || defined(BSP_VYBRID_AUTOEVB_M4)
    if (!lwgpio_init(&led1, BSP_LED1, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing LED1 GPIO as output failed.\n");
        _task_block();
    }
    lwgpio_set_functionality(&led1, BSP_LED1_MUX_GPIO);
#endif

    if (!lwgpio_init(&led2, BSP_LED2, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing LED2 GPIO as output failed.\n");
        _task_block();
    }
    lwgpio_set_functionality(&led2, BSP_LED2_MUX_GPIO);

    if (!lwgpio_init(&led3, BSP_LED3, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing LED3 GPIO as output failed.\n");
        _task_block();
    }
    lwgpio_set_functionality(&led3, BSP_LED3_MUX_GPIO);

#if defined(BSP_TWR_VF65GS10_A5) ||  defined(BSP_TWR_VF65GS10_M4)
    if (!lwgpio_init(&led4, BSP_LED1, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing LED4 GPIO as output failed.\n");
        _task_block();
    }
    lwgpio_set_functionality(&led4, BSP_LED1_MUX_GPIO);
#elif defined(BSP_VYBRID_AUTOEVB_A5) || defined(BSP_VYBRID_AUTOEVB_M4)
    if (!lwgpio_init(&led4, BSP_LED4, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing LED4 GPIO as output failed.\n");
        _task_block();
    }
    lwgpio_set_functionality(&led4, BSP_LED4_MUX_GPIO);
#endif

    // initial set them off
    lwgpio_set_value(&led1, LWGPIO_VALUE_HIGH);
    lwgpio_set_value(&led2, LWGPIO_VALUE_HIGH);
    lwgpio_set_value(&led3, LWGPIO_VALUE_HIGH);
    lwgpio_set_value(&led4, LWGPIO_VALUE_HIGH);
}

uint32_t my_delay(void)
{
    uint32_t i, j, k;
    for (i=0; i!=DELAY_CNT; i++)
        for (j=0; j!=DELAY_CNT; j++)
            for (k=0; k!=DELAY_CNT; k++)
                _ASM_NOP();
    return k;
}

uint32_t check_button(void)
{
    uint32_t result = BUTTON_NOT_PRESSED;

    // delay for a period before checking the button press status
    my_delay();

    if (_lwevent_wait_ticks(&app_event, SW_EVENT_MASK, FALSE, 1) == MQX_OK) {
        _lwevent_clear(&app_event, SW_EVENT_MASK);
        result = BUTTON_PRESSED;
    }
    return result;
}

void blink_leds(void)
{
    printf("Press SW2 to exit LED blink loop...");

    for (;;) {
        lwgpio_toggle_value(&led1);
        if (check_button() == BUTTON_PRESSED)
            break;
        lwgpio_toggle_value(&led2);
        if (check_button() == BUTTON_PRESSED)
            break;
        lwgpio_toggle_value(&led3);
        if (check_button() == BUTTON_PRESSED)
            break;
    }

    // After button pressed, all the LEDs are put ot off
    lwgpio_set_value(&led1, LWGPIO_VALUE_HIGH);
    lwgpio_set_value(&led2, LWGPIO_VALUE_HIGH);
    lwgpio_set_value(&led3, LWGPIO_VALUE_HIGH);

    printf("OK\n");
}

void led4_on(void)
{
    lwgpio_set_value(&led4, LWGPIO_VALUE_LOW);
}


/*FUNCTION*-----------------------------------------------------
*
* Task Name    : button_isr
* Comments     : SW interrupt callback used to catch SW2 press
*                and trigger SW_EVENT
*
*END*-----------------------------------------------------------*/

static void button_isr
    (
         void   *pin
    )
{
    lwgpio_int_clear_flag(pin);
    _lwevent_set(&app_event, SW_EVENT_MASK);
}       

/*FUNCTION*-----------------------------------------------------
*
* Task Name    : wakeup_isr
* Comments     : SW interrupt callback used to catch SW1 press
*
*END*-----------------------------------------------------------*/

static void wakeup_isr
    (
         void   *pin
    )
{
    // LED4 off
    lwgpio_set_value(&led4, LWGPIO_VALUE_HIGH);

    wkpu_clear_wakeup(WKPU_ID);
}       

void init_interrupt_button()
{
    static LWGPIO_STRUCT btn;
    if (!lwgpio_init(&btn, INT_BUTTON, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing button GPIO as input failed.\n");
        _task_block();
    }

    lwgpio_set_functionality(&btn, INT_BUTTON_FUNC);

#if defined(BSP_BUTTONS_ACTIVE_HIGH)
    lwgpio_set_attribute(&btn, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
#else
    lwgpio_set_attribute(&btn, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

    /* enable gpio functionality for given pin, react on falling edge */
    if (!lwgpio_int_init(&btn, LWGPIO_INT_MODE_FALLING))
    {
        printf("Initializing button GPIO for interrupt failed.\n");
        _task_block();
    }

    /* install gpio interrupt service routine */
    _int_install_isr(lwgpio_int_get_vector(&btn), button_isr, (void *)&btn);
    /* set the interrupt level, and unmask the interrupt in interrupt controller*/
    _bsp_int_init(lwgpio_int_get_vector(&btn), 3, 0, TRUE);
    /* enable interrupt on GPIO peripheral module*/
    lwgpio_int_enable(&btn, TRUE);
}

void init_wakeup_button()
{
    static LWGPIO_STRUCT sw;
    if (!lwgpio_init(&sw, WAKEUP_BUTTON, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("\nSW initialization failed.\n");
        _task_block();
    }

    /* Set functionality to GPIO mode */
    lwgpio_set_functionality(&sw, WAKEUP_BUTTON_FUNC);

#if defined(BSP_BUTTONS_ACTIVE_HIGH)
    lwgpio_set_attribute(&sw, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
#else
    lwgpio_set_attribute(&sw, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

    /* Setup the pin wakeup mode*/
    wkpu_wakeup_enable(WKPU_ID);
    _int_install_isr(wkpu_int_get_vector(), wakeup_isr, (void *)&sw);
    _bsp_int_init(wkpu_int_get_vector(), 3, 1, TRUE);
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
    /* Configure for normal interrupt button*/
    init_interrupt_button();

    /* Configure for wakeup interrupt button*/
    init_wakeup_button();

    /* Configure for LED1 to LED4*/
    init_leds();
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
        case LPM_CPU_POWER_MODE_LPR:
            name = "LPM_CPU_POWER_MODE_LPR";
            break;
        case LPM_CPU_POWER_MODE_ULPR:
            name = "LPM_CPU_POWER_MODE_ULPR";
            break;
        case LPM_CPU_POWER_MODE_WAIT:
            name = "LPM_CPU_POWER_MODE_WAIT";
            break;
        case LPM_CPU_POWER_MODE_STOP:
            name = "LPM_CPU_POWER_MODE_STOP";
            break;
        case LPM_CPU_POWER_MODE_LPS3:
            name = "LPM_CPU_POWER_MODE_LPS3";
            break;
        case LPM_CPU_POWER_MODE_LPS2:
            name = "LPM_CPU_POWER_MODE_LPS2";
            break;
        case LPM_CPU_POWER_MODE_LPS1:
            name = "LPM_CPU_POWER_MODE_LPS1";
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
    printf("  Mode:      %s \n", get_power_mode_name(core_bsp_setting.MODE_INDEX));
}

/*TASK*-----------------------------------------------------
* 
* Task Name    : main_task
* Comments     :
*    This task demos how CPU switches between different
*    Power Modes
*
*END*-----------------------------------------------------*/
void main_task
    (
        uint32_t initial_data
    )
{
    LPM_OPERATION_MODE operation_mode;

    printf("\nMQX Power Modes Switch Demo\n");
    printf("!!!CAUSION!!! This example only support RAM version\n");
    printf("!!!CAUSION!!! Please select Int Ram version configuration to build and run\n");

    /* Create global event */
    if (_lwevent_create(&app_event, 0) != MQX_OK)
    {
        printf("\nCreating app_event failed.\n");
        _task_block();
    }

#if defined(BSP_VYBRID_AUTOEVB_A5) || defined(BSP_VYBRID_AUTOEVB_M4)
    /*
     * clocks_init in tower board opens all the CCGRs in the end, but this is
     * missing in autoevb board. to make WKPU work, this should be added
     */
    _bsp_open_all_clock_gates();
#endif

    /*disable all system wakeup at first*/
    _bsp_wakeup_init();

    /* Initialize switches */
    button_led_init();


    while (1)
    {
        /*
         * Normal Mode
         */
        operation_mode = _lpm_get_operation_mode();

        printf("\n******************************************************************************\n");
        printf("************** Operation mode : %s ***********************\n", get_operation_mode_name(operation_mode));
        printf("******************************************************************************\n");

        display_operation_mode_setting(operation_mode);

        printf(
        "Info: RUN operation mode is mapped on RUN power mode by default.\n"
        "      The core runs at full clock speed.\n"
        "      LEDs blinks quickly\n");

        blink_leds();
    
        /*
         * Wait Mode
         */
        operation_mode = LPM_OPERATION_MODE_WAIT;
        
        printf("\n******************************************************************************\n");
        printf("************** Operation mode : %s **********************\n", get_operation_mode_name (operation_mode));
        printf("******************************************************************************\n");

        display_operation_mode_setting(operation_mode);

        printf(
        "Info: WAIT operation mode is mapped on VLPR power mode by default.\n"
        "      PLL are disabled, core runs with FXOSC (32MHz).\n"
        "      LED2 blinks slowly\n");

        /*Change to use FIRC clock frequency*/
        if (CM_ERR_OK != _lpm_set_clock_configuration(BSP_CLOCK_CONFIGURATION_USE_FIRC))
        {
            printf("Cannot change clock configuration");
            _task_block();
        }

        /* Change the operation mode */
        printf ("\nSetting operation mode to %s ... ", get_operation_mode_name(operation_mode));
        printf ("%s\n", _lpm_set_operation_mode(operation_mode) == 0 ? "OK" : "ERROR");

        blink_leds();

        /*
         * Sleep Mode
         */
        operation_mode = LPM_OPERATION_MODE_SLEEP;
        
        printf("\n******************************************************************************\n");
        printf("************** Operation mode : %s **********************\n", get_operation_mode_name (operation_mode));
        printf("******************************************************************************\n");

        display_operation_mode_setting(operation_mode);

        printf(
        "Info: SLEEP operation mode is mapped on WAIT power mode by default.\n"
        "      The core is inactive in this mode, reacting only to wakeup interrupts.\n"
        "      LEDs don't blink\n");

        printf ("\nSetting operation mode to %s, Press SW1 to wakeup... ", get_operation_mode_name (operation_mode));

        // Light LED4, prompt user that SW1 should be pressed to wakeup the CPU
        led4_on();

        printf ("%s\n", _lpm_set_operation_mode(operation_mode) == 0 ? "OK" : "ERROR");

        blink_leds();

        /*
         * Stop Mode
         */
        operation_mode = LPM_OPERATION_MODE_STOP;
        
        printf("\n******************************************************************************\n");
        printf("************** Operation mode : %s **********************\n", get_operation_mode_name (operation_mode));
        printf("******************************************************************************\n");

        display_operation_mode_setting(operation_mode);

        printf(
        "Info: STOP operation mode is mapped on STOP power mode by default.\n"
        "      The core is inactive in this mode, reacting only to wakeup interrupts.\n"
        "      LEDs don't blink\n");

        printf ("\nSetting operation mode to %s, Press SW1 to wakeup... ", get_operation_mode_name (operation_mode));

        // Light LED4, prompt user that SW1 should be pressed to wakeup the CPU
        led4_on();

        printf ("%s\n", _lpm_set_operation_mode(operation_mode) == 0 ? "OK" : "ERROR");

        blink_leds();

        /*
         * Back to Run mode
         */

        /*Enable PLLs first*/
        operation_mode = LPM_OPERATION_MODE_RUN;
        printf ("\nSetting operation mode to %s ... ", get_operation_mode_name (operation_mode));
        printf ("%s\n", _lpm_set_operation_mode(operation_mode) == 0 ? "OK" : "ERROR");

        /*Change to use NORMAL clock frequency*/
        // printf("\nChanging to use NORMAL frequency\n");
        if (CM_ERR_OK != _lpm_set_clock_configuration(BSP_CLOCK_CONFIGURATION_FULL_SPEED))
        {
            printf("Cannot change clock configuration");
            _task_block();
        }
    }
}

/* EOF */
