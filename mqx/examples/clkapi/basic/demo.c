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
*   This file contains the source to demo the clock api usage
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#if ! BSPCFG_ENABLE_CLKTREE_MGMT
#error This application requires BSPCFG_ENABLE_CLKTREE_MGMT defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


/* Task IDs */
#define CLK_API_DEMO_TASK 5

extern void clkapi_demo_task(uint32_t);


const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index,   Function,   Stack,  Priority, Name,     Attributes,          Param, Time Slice */
    { CLK_API_DEMO_TASK,   clkapi_demo_task, 1500,   8,        "hello",  MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};

/*TASK*-----------------------------------------------------
*
* Task Name    : clkapi_demo_task
* Comments     :
*    This task demos the basic usage of clock management APIs.
*    For clock_set_freq API, a call to the specified node with
*    only one second paramter 0 will dump the frequency node
*    configuration informatino including
*      - how many config bitfields are there
*      - vaild value range for each of the bitfield
*    A following call with correct number of paramters will
*    successfully set the new frequency. Otherwise the frequency
*    will not change. The dump information will prompt user
*    about this
*
*END*-----------------------------------------------------*/
void clkapi_demo_task
    (
        uint32_t initial_data
    )
{
    void* pclk = clock_get(CLK_USB1);
    void* pclk1 = clock_get(CLK_LPRC);
    void* qspiclk = clock_get(CLK_QSPI0);
    void* pll1clk = clock_get(CLK_PLL1);

    // Experiment with QSPI0
    printf("-------- QSPI Freq Change Experiment --------\n");
    clock_dump(qspiclk);
    clock_set_freq(qspiclk, 0);
    clock_set_freq(qspiclk, 2, 0, 1);
    clock_set_freq(qspiclk, 5, 0, 1, 2, 3, 3);
    clock_set_freq(qspiclk, 3, 0, 1, 4);
    clock_set_freq(qspiclk, 3, 0, 1, 3);
    clock_dump(qspiclk);

    // Experiment with LPRC node
    printf("-------- LPRC Freq Change Experiment --------\n");
    clock_dump(pclk1);
    clock_set_freq(pclk1, 0);
    clock_set_freq(pclk1, 1, 8);
    clock_dump(pclk1);
    clock_set_freq(pclk1, 1, 0, 9);
    clock_dump(pclk1);
    clock_set_freq(pclk1, 2, 0);
    clock_set_freq(pclk1, 1, 1);
    clock_dump(pclk1);

    // Experiment on USB1 node
    printf("-------- USB1 Clock Enable Disable Experiment --------\n");
    clock_dump(pclk);
    clock_enable(pclk);
    clock_dump(pclk);
    clock_disable(pclk);
    clock_dump(pclk);
    clock_set_freq(pclk, 3, 6, 2, 8);

    // Dump all information
    printf("-------- Dump all clock nodes --------\n");
    clock_dump_all();
    _task_block();
}

/* EOF */
