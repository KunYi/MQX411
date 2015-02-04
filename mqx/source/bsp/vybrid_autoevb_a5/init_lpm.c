/*HEADER**********************************************************************
*
* Copyright 2011-2013 Freescale Semiconductor, Inc.
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
*   This file contains Low Power Manager initialization information.
*
*
*END************************************************************************/


#include "mqx.h"
#include "bsp.h"

#define MASK_ALL_WAKEUP_INTERRUPT 0xFFFFFFFF

#if MQX_ENABLE_LOW_POWER
const LPM_CPU_OPERATION_MODE LPM_CPU_OPERATION_MODES[LPM_OPERATION_MODES] = 
{
    // LPM_OPERATION_MODE_RUN
    {
        LPM_CPU_POWER_MODE_RUN,                     // Index of predefined mode
    },
    // LPM_OPERATION_MODE_WAIT
    {
        LPM_CPU_POWER_MODE_LPR,                    // Index of predefined mode
    },
    // LPM_OPERATION_MODE_SLEEP
    {
        LPM_CPU_POWER_MODE_WAIT,                    // Index of predefined mode
    },
    // LPM_OPERATION_MODE_STOP
    {
        LPM_CPU_POWER_MODE_STOP,                     // Index of predefined mode
    }
};
#endif

void _bsp_wakeup_init(void)
{
    GPC_IMR1 = MASK_ALL_WAKEUP_INTERRUPT;
    GPC_IMR2 = MASK_ALL_WAKEUP_INTERRUPT;
    GPC_IMR3 = MASK_ALL_WAKEUP_INTERRUPT;
    GPC_IMR4 = MASK_ALL_WAKEUP_INTERRUPT;
}

void _bsp_open_all_clock_gates(void)
{
    /*
     * enable all clock from CCM
     */
    CCM_CCGR0 = 0xffffffff;
    CCM_CCGR1 = 0xffffffff;
    CCM_CCGR2 = 0xffffffff;
    CCM_CCGR3 = 0xffffffff;
    CCM_CCGR4 = 0xffffffff;
    CCM_CCGR5 = 0xffffffff;
    CCM_CCGR6 = 0xffffffff;
    CCM_CCGR7 = 0xffffffff;
    CCM_CCGR8 = 0x3fffffff;
    CCM_CCGR9 = 0xffffffff;
    CCM_CCGR10 = 0xffffffff;
    CCM_CCGR11 = 0xffffffff;
}


/* EOF */
