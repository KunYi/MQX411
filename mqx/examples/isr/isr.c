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
*   This file contains the source for the ISR example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#if (PSP_MQX_CPU_IS_KINETIS | PSP_MQX_CPU_IS_VYBRID_M4)  && !BSP_KWIKSTIK_K40X256
#define DEMO_ENABLE_KERNEL_ISR
#endif

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifdef DEMO_ENABLE_KERNEL_ISR
#if MQX_ROM_VECTORS
#error This application requires MQX_ROM_VECTORS defined zero in user_config.h. Please recompile BSP, PSP with this option.
#endif /* MQX_ROM_VECTORS */
#endif /* DEMO_ENABLE_KERNEL_ISR */

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


#define MAIN_TASK 9
#ifdef DEMO_ENABLE_KERNEL_ISR
#define BLINK_LED_TASK 10
#endif /* DEMO_ENABLE_KERNEL_ISR */

void main_task( uint32_t );
#ifdef DEMO_ENABLE_KERNEL_ISR
void blink_led_task( uint32_t );
void btn_kernel_isr( void );
void init_interrupt_btn(void *);
void init_led(void *);
#endif /* DEMO_ENABLE_KERNEL_ISR */
void new_tick_isr( void * );


const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   /* Task Index    ,       Function, Stack, Priority,        Name,          Attributes, Param, Time Slice */
    { MAIN_TASK     ,      main_task,  1000,        8,      "Main", MQX_AUTO_START_TASK,     0,     0 },
#ifdef DEMO_ENABLE_KERNEL_ISR
    { BLINK_LED_TASK, blink_led_task,  1000,        9, "blink led", MQX_AUTO_START_TASK,     0,     0 },
#endif /* DEMO_ENABLE_KERNEL_ISR */
    { 0 }
};


typedef struct my_isr_struct
{
   void                     *OLD_ISR_DATA;
   void      (_CODE_PTR_ OLD_ISR)(void *);
   _mqx_uint                   TICK_COUNT;
} MY_ISR_STRUCT, * MY_ISR_STRUCT_PTR;

/* Global variables */
LWSEM_STRUCT    lwsem;
#ifdef DEMO_ENABLE_KERNEL_ISR
LWGPIO_STRUCT   btn1, led1;
_mqx_uint       num_tick        = 0;
volatile bool   btn_pressed     = FALSE;
bool            prv_btn_pressed = TRUE;
#endif /* DEMO_ENABLE_KERNEL_ISR */



/*ISR*-----------------------------------------------------------
*
* ISR Name : new_tick_isr
* Comments :
*   This ISR replaces the existing timer ISR, then calls the
*   old timer ISR.
*END*-----------------------------------------------------------*/
void new_tick_isr
   (
      void   *user_isr_ptr
   )
{
   MY_ISR_STRUCT_PTR  isr_ptr;

   _lwsem_post(&lwsem);
   isr_ptr = (MY_ISR_STRUCT_PTR)user_isr_ptr;
   isr_ptr->TICK_COUNT++;

   /* Chain to the previous notifier */
   (*isr_ptr->OLD_ISR)(isr_ptr->OLD_ISR_DATA);
}


#ifdef DEMO_ENABLE_KERNEL_ISR

/*ISR*------------------------------------------------------------------------
*
* ISR Name : btn_kernel_isr
* Comments :
*   The interrupt service routine triggered by pusshing button.
*   The function pointer is assigned into the interrupt vector
*   table of the processor instead of the kernel interrupt vector table of MQX.
*   This interrupt handler should be used with good care about following points.
*
*    1)For native MQX interrupt handling sequence, the _int_kernel_isr() is
*       invoked as interrupt occurs. The _int_kernel_isr() updates the interrupt
*       context in the MQX kernel and saves the context of the current program
*       (which could be the lower priority interrupt). When interrupt handler is
*       installed directly into the interrupt vector table of processor, the
*       interrupt fields in the kernel is not updated for that interrupt as it
*       happens. If user calls any function: _lwevent_set(), _lwsem_post(),
*       _lwmsgq_send(), _time_delay() the functions _CHECK_RUN_SCHEDULER() is
*       invoked which then initiate the PendSV exception handler with priority
*       level higher than the current running task. This results in the Usage
*       fault exception in the processor and the application crashes.
*       
*       The user therefore must not call any function from MQX kernel API
*       inside this interrupt handler.

*   2)The interrupt flag should be cleared at the beginning of the interrupt
*       handler to overcome the problem of unintentionally invoking the ISR
*       twice. In detail the deasserting of interrupt signal to NVIC module
*       in the processor does not happen right after the interrupt flag is
*       cleared but only after a certain amount of time. This delay is because
*       of the write_buffer feature integrated into the processor.
*
*       However for the native method of handling interrupt in MQX kernel 
*       in which function _int_kernel_isr() is invoked as interrupt occurs
*       the interrupt flag can be cleared at any time in the user's ISR without
*       worrying about that phenomenon. This is because after executing user's
*       ISR the _int_kernel_isr() function does a number of operations before
*       exiting and that process always takes longer time than the delay
*       described previously. This makes the interrupt signal to NVIC module
*       to be deasserted before processor finishes servicing the interrupt.
*END*------------------------------------------------------------------------*/
void btn_kernel_isr
    (
       void
    )
{
    lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) (&btn1));
    btn_pressed = !btn_pressed;
}


/****************************************************************
*
* Function Name : init_interrupt_btn
* Comments      :
*   Open the pin BSP_BTN1 for input, initialize interrupt and set
*   interrupt handler.
*
*****************************************************************/
void init_interrupt_btn(void * button)
{

    LWGPIO_STRUCT_PTR btn_ptr = (LWGPIO_STRUCT_PTR) button;
    /* opening pins for input */
    if (!lwgpio_init(btn_ptr, BSP_BUTTON1, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing button GPIO as input failed.\n");
        _task_block();
    }
#ifdef BSP_BUTTON1_MUX_IRQ
    lwgpio_set_functionality(btn_ptr, BSP_BUTTON1_MUX_IRQ);
#if defined(BSP_BUTTONS_ACTIVE_HIGH)
    lwgpio_set_attribute(btn_ptr, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
#else
    lwgpio_set_attribute(btn_ptr, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif
    /* enable gpio functionality for given pin, react on falling edge */
    if (! lwgpio_int_init(btn_ptr, LWGPIO_INT_MODE_FALLING))
    {
        printf("Initializing button GPIO for interrupt failed.\n");
        _task_block();
    }

    /* Install the interrupt directly to the vector table in the processor. */
    if(! _int_install_kernel_isr(lwgpio_int_get_vector(btn_ptr), btn_kernel_isr))
    {
        printf("Install interrupt handler to hardware vector table failed.\n");
        _task_block();
    }

    /* set the interrupt level, and unmask the interrupt in interrupt controller */
    if(MQX_OK != _bsp_int_init(lwgpio_int_get_vector(btn_ptr), 3, 0, TRUE))
    {
        printf("Initialize interrupt failed.\n");
        _task_block();
    }

    /* enable interrupt on GPIO peripheral module */
    lwgpio_int_enable(btn_ptr, TRUE);
#else
    printf("This platform does not support pin mux interrupt function\n");
    _task_block();
#endif /* BSP_BUTTON1_MUX_IRQ */
}


/*******************************************************************
*
* Function Name : init_led.
* Comments      :
*   Open the BSP_LED1 pin as output and drive the output level high.
*
********************************************************************/
void init_led(void * led)
{
    LWGPIO_STRUCT_PTR led_ptr = (LWGPIO_STRUCT_PTR)led;
    if (!lwgpio_init(led_ptr, BSP_LED1, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing LED1 GPIO as output failed.\n");
        _task_block();
    }
    /* swich pin functionality (MUX) to GPIO mode */
    lwgpio_set_functionality(led_ptr, BSP_LED1_MUX_GPIO);

    /* write logical 1 to the pin */
    lwgpio_set_value(led_ptr, LWGPIO_VALUE_HIGH);
}

/*TASK*----------------------------------------------------------
*
* Task Name : blink_led_task
* Comments  :
*   Set up LED and button.
*   When user presses a button this task blinks the LED and prints
*   out the number of times the system timer interrupt occurred.
*   If the button is pressed again, the LED is turned off and
*   the number of times the system timer interrupt occurred
*   is also printed out.
*END*-----------------------------------------------------------*/
void blink_led_task
    (
        uint32_t initial_data
    )
{
    /* Initialize led */
    init_led((void *)(&led1));
    /* Initialize button 1 */
    init_interrupt_btn((void *)(&btn1));
    while(1)
    {
        if(TRUE == btn_pressed)
        {
            if (TRUE == prv_btn_pressed)
            {
                prv_btn_pressed = FALSE;
                printf("\nLed starts blinking at tick No. = %d\n", num_tick);
            }
            lwgpio_toggle_value(&led1);
            _time_delay(200);
        }
        else if (FALSE == prv_btn_pressed)
        {
            prv_btn_pressed = TRUE;
            printf("\nLed is off at tick No. = %d\n", num_tick);
            lwgpio_set_value(&led1, LWGPIO_VALUE_HIGH);
        }
    }
}
#endif /* DEMO_ENABLE_KERNEL_ISR */

/*TASK*----------------------------------------------------------
*
* Task Name : main_task
* Comments  :
*   This task installs a new ISR to replace the timer ISR.
*   Update the number of ticks.
*
*END*-----------------------------------------------------------*/

void main_task
    (
        uint32_t initial_data
    )
{
    MY_ISR_STRUCT_PTR  isr_ptr;

    uint32_t result;

    /* Create the lightweight semaphore */
    result = _lwsem_create(&lwsem, 0);
    if (result != MQX_OK) {
        printf("\nCreating sem failed: 0x%X", result);
        _task_block();
    }

    isr_ptr               =  _mem_alloc_zero((_mem_size)sizeof(MY_ISR_STRUCT));
    isr_ptr->TICK_COUNT   =  0;
    isr_ptr->OLD_ISR_DATA =  _int_get_isr_data(BSP_TIMER_INTERRUPT_VECTOR);
    isr_ptr->OLD_ISR      =  _int_get_isr(BSP_TIMER_INTERRUPT_VECTOR);

    /* Native MQX interrupt handling method, ISR is installed into the interrupt vector table in kernel */
    if(! _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, new_tick_isr, isr_ptr))
    {
        printf("Install interrupt handler to interrupt vector table of MQX kernel failed.\n");
        _task_block();
    }
#ifndef DEMO_ENABLE_KERNEL_ISR
    _time_delay_ticks(200);
    printf("\nTick count = %d\n", isr_ptr->TICK_COUNT);
    _task_block();
#else
    printf("\n====================== ISR Example =======================\n");
    printf("Press the SW1 to blink LED1, press it again to turn it off\n");
    while(1)
    {
      _lwsem_wait(&lwsem);
      num_tick = isr_ptr->TICK_COUNT;
    }
#endif /* DEMO_ENABLE_KERNEL_ISR */
}
/* EOF */
