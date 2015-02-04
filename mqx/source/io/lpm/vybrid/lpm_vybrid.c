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
*   This file contains Low Power Manager functionality specific to Vybrid.
*
*
*END************************************************************************/


#include "mqx.h"
#include "bsp.h"


#if MQX_ENABLE_LOW_POWER

#define GPC_LPMR_MODE_RUN   0
#define GPC_LPMR_MODE_WAIT  1
#define GPC_LPMR_MODE_STOP  2
/*FUNCTION*------------------------------------------------------------------
*
* Function Name    : _lpm_set_cpu_operation_mode
* Returned Value   : MQX error code
* Comments         :
*    This function changes operation mode of the CPU core.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _lpm_set_cpu_operation_mode
    (
        /* [IN] Specification of CPU core low power operation modes available */
        const LPM_CPU_OPERATION_MODE  *operation_modes,

        /* [IN] Low power operation mode identifier */
        LPM_OPERATION_MODE                 target_mode
    )
{
    _mqx_uint index;

    // at this moment UART is closed, no dump
    // printf("_lpm_set_cpu_operation_mode for vybrid...\n");

    /*
     * operation_modes is assigned to LPM_CPU_OPERATION_MODES
     */

    /* Check parameters */
    if ((NULL == operation_modes) || (LPM_OPERATION_MODES <= (_mqx_uint)target_mode))
    {
        return MQX_INVALID_PARAMETER;
    }
    index = operation_modes[target_mode].MODE_INDEX;    // LPM_CPU_OPERATION_MODES

    if (LPM_CPU_POWER_MODES <= index)
    {
        return MQX_INVALID_CONFIGURATION;
    }

    if (LPM_CPU_POWER_MODE_RUN == index) {
        /*Enable all PLLs*/
        Anadig_PLL3_CTRL |= Anadig_PLL3_CTRL_ENABLE_MASK;
        Anadig_PLL7_CTRL |= Anadig_PLL7_CTRL_ENABLE_MASK;
        Anadig_PLL2_CTRL |= Anadig_PLL2_CTRL_ENABLE_MASK;
        Anadig_PLL4_CTRL |= Anadig_PLL4_CTRL_ENABLE_MASK;
        Anadig_PLL6_CTRL |= Anadig_PLL6_CTRL_ENABLE_MASK;
        Anadig_PLL5_CTRL |= Anadig_PLL5_CTRL_ENABLE_MASK;
        Anadig_PLL1_CTRL |= Anadig_PLL1_CTRL_ENABLE_MASK;
    } else {
        /*Close all PLLs*/
        Anadig_PLL3_CTRL &= ~Anadig_PLL3_CTRL_ENABLE_MASK;
        Anadig_PLL7_CTRL &= ~Anadig_PLL7_CTRL_ENABLE_MASK;
        Anadig_PLL2_CTRL &= ~Anadig_PLL2_CTRL_ENABLE_MASK;
        Anadig_PLL4_CTRL &= ~Anadig_PLL4_CTRL_ENABLE_MASK;
        Anadig_PLL6_CTRL &= ~Anadig_PLL6_CTRL_ENABLE_MASK;
        Anadig_PLL5_CTRL &= ~Anadig_PLL5_CTRL_ENABLE_MASK;
        Anadig_PLL1_CTRL &= ~Anadig_PLL1_CTRL_ENABLE_MASK;
    }

    if (LPM_CPU_POWER_MODE_WAIT == index) {
        GPC_LPMR = GPC_LPMR_MODE_WAIT;   // WAIT
        _ASM_SLEEP(NULL);
    }

    if (LPM_CPU_POWER_MODE_STOP == index) {
        // In stop mode, Power Domain 1 still on, well bias is disabled
        GPC_PGCR &= ~(GPC_PGCR_PG_PD1_MASK | GPC_PGCR_WB_STOP_MASK);

        // In stop mode, assert deep sleep to memories, HPREG is power off
        GPC_PGCR |= (GPC_PGCR_DS_STOP_MASK | GPC_PGCR_HP_OFF_MASK);
        GPC_LPMR = GPC_LPMR_MODE_STOP;  // STOP
        _ASM_SLEEP(NULL);
    }

    if (LPM_CPU_POWER_MODE_RUN == index) {
        GPC_LPMR = GPC_LPMR_MODE_RUN; // RUN
    }

    return MQX_OK;
}

/*FUNCTION*------------------------------------------------------------------
*
* Function Name    : _lpm_wakeup_core
* Returned Value   : None
* Comments         :
*    This function must be called from ISR not to let the core to return to sleep again.
*
*END*----------------------------------------------------------------------*/

void _lpm_wakeup_core
    (
        void
    )
{
}


/*FUNCTION*------------------------------------------------------------------
* 
* Function Name    : _lpm_idle_sleep_check
* Returned Value   : TRUE if idle sleep possible, FALSE otherwise
* Comments         :
*    This function checks whether cpu core can sleep during execution of idle task
*    in current power mode. 
*
*END*----------------------------------------------------------------------*/

bool _lpm_idle_sleep_check
    ( 
        void
    )
{
    return TRUE;
}


/*FUNCTION*------------------------------------------------------------------
*
* Function Name    : _lpm_llwu_clear_flag
* Returned Value   : Void
* Comments         :
*    This function is used to clear LLWU flags and pass them to caller
*    Not used in Vybrid, just add a function stub
*
*END*----------------------------------------------------------------------*/

void _lpm_llwu_clear_flag
    (
        /* [OUT] Pointer stores value of LLWU_Fx flags */
        uint32_t *llwu_fx_ptr
    )
{
}
#endif


/* EOF */
