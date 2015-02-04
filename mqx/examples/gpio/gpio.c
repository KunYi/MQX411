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
*   This file contains the source for the gpio example program.
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"

/* Function prototypes */
void main_task(uint32_t);
void int_service_routine(void *);

typedef enum {
    BUTTON_NOT_INITIALIZED = -1,
    BUTTON_RELEASED,
    BUTTON_PRESSED
} button_state_t;

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index, Function,  Stack, Priority, Name,   Attributes,          Param, Time Slice */
    { 1,           main_task, 1500,  8,        "main", MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};
/* Global variables */
LWSEM_STRUCT lwsem;


/******************************************************************************
*
* Functio Name      : int_service_routine
* Comments          : The interrupt service routine triggered by gpio
*
******************************************************************************/
void int_service_routine(void *pin)
{
    lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) pin);
    _lwsem_post(&lwsem);
}

/******************************************************************************
*
* Task Name    : main_task
* Comments     : The main task executes 3 steps
*
*   1) Configures BSP_BUTTON1 to trigger interrupt on falling edge if supported
*      by selected platform.
*   2) Drives BSP_LED1 based on BSP_BUTTON1 state or
*      drives BSP_LED1 automatically if BSP_BUTTON1 is not available.
*   3) Togles BSP_LED1 if BSP_BUTTON1 is not available
*
******************************************************************************/
void main_task
    (
        uint32_t initial_data
    )
{
    /* Structures holding information about specific pin */
    LWGPIO_STRUCT led1, btn1;
    button_state_t  button_state, button_last_state;
    int button_press_count;
    _mqx_uint result;

    /* Create the lightweight semaphore */
    result = _lwsem_create(&lwsem, 0);
    if (result != MQX_OK) {
        printf("\nCreating sem failed: 0x%X", result);
        _task_block();
    }
#ifdef BSP_LED1
/******************************************************************************
    Open the BSP_LED1) pin as output and drive the output level high.
******************************************************************************/
    /* initialize lwgpio handle (led1) for BSP_LED1 pin
     * (defined in mqx/source/bsp/<bsp_name>/<bsp_name>.h file)
     */
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

#if defined BSP_BUTTON1
/******************************************************************************
    Open the pin (BSP_BTN1) for input, initialize interrupt
    and set interrupt handler.
******************************************************************************/
    /* opening pins for input */
    if (!lwgpio_init(&btn1, BSP_BUTTON1, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing button GPIO as input failed.\n");
        _task_block();
    }

#ifdef BSP_BUTTON1_MUX_IRQ
    /* Some platforms require to setup MUX to IRQ functionality,
    for the rest just set MUXto GPIO functionality */
    lwgpio_set_functionality(&btn1, BSP_BUTTON1_MUX_IRQ);
#if defined(BSP_BUTTONS_ACTIVE_HIGH)
    lwgpio_set_attribute(&btn1, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
#else
    lwgpio_set_attribute(&btn1, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

    /* enable gpio functionality for given pin, react on falling edge */
    if (!lwgpio_int_init(&btn1, LWGPIO_INT_MODE_FALLING))
    {
        printf("Initializing button GPIO for interrupt failed.\n");
        _task_block();
    }

    /* install gpio interrupt service routine */
    _int_install_isr(lwgpio_int_get_vector(&btn1), int_service_routine, (void *) &btn1);
    /* set the interrupt level, and unmask the interrupt in interrupt controller */
    _bsp_int_init(lwgpio_int_get_vector(&btn1), 3, 0, TRUE);
    /* enable interrupt on GPIO peripheral module */
    lwgpio_int_enable(&btn1, TRUE);

    printf("\n====================== GPIO Example ======================\n");
    printf("The (SW1) button is configured to trigger GPIO interrupt.\n");
    printf("Press the (SW1) button 3x to continue.\n\n");
    button_press_count = 1;

    while(button_press_count < 4)       {
        /* wait for button press, lwsem is set in button isr */
        _lwsem_wait(&lwsem);
        printf("Button pressed %dx\r", button_press_count++);
    }
    /* disable interrupt on GPIO peripheral module */
    lwgpio_int_enable(&btn1, FALSE);

    printf("\n\n");
#endif /* BSP_BUTTON1_MUX_IRQ */

/******************************************************************************
    Read value from input pin (BSP_BTN1)  Note that in previous phase,
    the pin was configured as an interrupt and it have to be reconfigured
    to standard GPIO.
******************************************************************************/
    /* set pin functionality (MUX) to GPIO*/
    lwgpio_set_functionality(&btn1, BSP_BUTTON1_MUX_GPIO);
#if defined(BSP_BUTTONS_ACTIVE_HIGH)
    lwgpio_set_attribute(&btn1, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
#else
    lwgpio_set_attribute(&btn1, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

    printf("The (SW1) button state is now polled.\n");
    printf("Press the (SW1) button to switch LED on or off\n\n");

    button_last_state = BUTTON_NOT_INITIALIZED;

    while (TRUE)
    {
         /* read pin/signal status */
#if defined(BSP_BUTTONS_ACTIVE_HIGH)
        if (LWGPIO_VALUE_HIGH == lwgpio_get_value(&btn1))
#else
        if (LWGPIO_VALUE_LOW == lwgpio_get_value(&btn1))
#endif
        {
            button_state = BUTTON_PRESSED;
        }
        else
        {
            button_state = BUTTON_RELEASED;
        }

        if (button_state != button_last_state)  {
            printf("Button %s\r", button_state == BUTTON_PRESSED ? "pressed " : "released");
            button_last_state = button_state;
#ifdef BSP_LED1
            lwgpio_set_value(&led1, button_state == BUTTON_PRESSED ? LWGPIO_VALUE_LOW : LWGPIO_VALUE_HIGH);
#endif /* BSP_LED1 */
        }
        /* Check button state every 100 ms */
        _time_delay(100);
   }
#elif defined BSP_LED1

/******************************************************************************
    In case board does not have any buttons. Only blink with LED.
******************************************************************************/
    printf("Going to blink with LED\n");
    /* toggle value */
    while (TRUE)  {
        lwgpio_toggle_value(&led1);
        _time_delay(250);
    }
#else /* BSP_LED1, BSP_BUTTON1 */
    printf("Cannot run demo, no BSP_LED1 nor BSP_BUTTON1 defined for board.");
#endif /* BSP_LED1, BSP_BUTTON1 */
}
/* EOF */
