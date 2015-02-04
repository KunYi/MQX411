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
*   This file contains Low Power Manager functionality specific to Kinetis.
*
*
*END************************************************************************/


#include "mqx.h"
#include "bsp.h"


#if MQX_ENABLE_LOW_POWER


#ifndef PE_LDD_VERSION


static const LPM_CPU_POWER_MODE LPM_CPU_POWER_MODES_KINETIS[LPM_CPU_POWER_MODES] =
{
    // Kinetis RUN
    {
        MC_PMCTRL_LPWUI_MASK,                       // Mode PMCTRL register == voltage regulator ON after wakeup
        0,                                          // Mode flags == clear settings
    },
    // Kinetis WAIT
    {
        MC_PMCTRL_LPWUI_MASK,                       // Mode PMCTRL register == voltage regulator ON after wakeup
        LPM_CPU_POWER_MODE_FLAG_USE_WFI,            // Mode flags == execute WFI
    },
    // Kinetis STOP
    {
        MC_PMCTRL_LPWUI_MASK,                       // Mode PMCTRL register == voltage regulator ON after wakeup
        LPM_CPU_POWER_MODE_FLAG_DEEP_SLEEP | LPM_CPU_POWER_MODE_FLAG_USE_WFI,   // Mode flags == deepsleep, execute WFI
    },
    // Kinetis VLPR
    {
        MC_PMCTRL_RUNM(2),                          // Mode PMCTRL register == VLPR
        0,                                          // Mode flags == clear settings
    },
    // Kinetis VLPW
    {
        MC_PMCTRL_RUNM(2),                          // Mode PMCTRL register == VLPW
        LPM_CPU_POWER_MODE_FLAG_USE_WFI,            // Mode flags == execute WFI
    },
    // Kinetis VLPS
    {
        MC_PMCTRL_LPLLSM(2),                        // Mode PMCTRL register == VLPS
        LPM_CPU_POWER_MODE_FLAG_DEEP_SLEEP | LPM_CPU_POWER_MODE_FLAG_USE_WFI,   // Mode flags == deepsleep, execute WFI
    },
    // Kinetis LLS
    {
        MC_PMCTRL_LPWUI_MASK | MC_PMCTRL_LPLLSM(3), // Mode PMCTRL register == voltage regulator ON after wakeup, LLS
        LPM_CPU_POWER_MODE_FLAG_DEEP_SLEEP | LPM_CPU_POWER_MODE_FLAG_USE_WFI,   // Mode flags == deepsleep, execute WFI
    },
    // Kinetis VLLS3
    {
        MC_PMCTRL_LPWUI_MASK | MC_PMCTRL_LPLLSM(5),
        LPM_CPU_POWER_MODE_FLAG_DEEP_SLEEP | LPM_CPU_POWER_MODE_FLAG_USE_WFI
    },
    // Kinetis VLLS2
    {
        MC_PMCTRL_LPWUI_MASK | MC_PMCTRL_LPLLSM(6),
        LPM_CPU_POWER_MODE_FLAG_DEEP_SLEEP | LPM_CPU_POWER_MODE_FLAG_USE_WFI
    },
    // Kinetis VLLS1
    {
        MC_PMCTRL_LPWUI_MASK | MC_PMCTRL_LPLLSM(7),
        LPM_CPU_POWER_MODE_FLAG_DEEP_SLEEP | LPM_CPU_POWER_MODE_FLAG_USE_WFI
    }
};


#else


static const LDD_TDriverOperationMode LPM_PE_OPERATION_MODE_MAP[LPM_OPERATION_MODES] =
{
    DOM_RUN,
    DOM_WAIT,
    DOM_SLEEP,
    DOM_STOP
};


#endif


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

#ifndef PE_LDD_VERSION

    const LPM_CPU_POWER_MODE              *mode_ptr;
    _mqx_uint                              scr, flags, mcg, index, cme;

    /* Check parameters */
    if ((NULL == operation_modes) || (LPM_OPERATION_MODES <= (_mqx_uint)target_mode))
    {
        return MQX_INVALID_PARAMETER;
    }
    index = operation_modes[target_mode].MODE_INDEX;

    if (LPM_CPU_POWER_MODES <= index)
    {
        return MQX_INVALID_CONFIGURATION;
    }
    mode_ptr = &(LPM_CPU_POWER_MODES_KINETIS[index]);
    flags = mode_ptr->FLAGS | (operation_modes[target_mode].FLAGS & LPM_CPU_POWER_MODE_FLAG_USER_MASK);

    /* Go through Kinetis Run */
    scr = (SCB_SCR & (~ (SCB_SCR_SLEEPDEEP_MASK | SCB_SCR_SLEEPONEXIT_MASK)));
    SCB_SCR = scr;
    MC_PMCTRL = LPM_CPU_POWER_MODES_KINETIS[LPM_CPU_POWER_MODE_RUN].PMCTRL;
    while (0 == (PMC_REGSC & PMC_REGSC_REGONS_MASK))
        { };

    /* Go to VLPW through VLPR */
    if (LPM_CPU_POWER_MODE_VLPW == index)
    {
        MC_PMCTRL = LPM_CPU_POWER_MODES_KINETIS[LPM_CPU_POWER_MODE_VLPR].PMCTRL;
        while (0 == (PMC_REGSC & PMC_REGSC_VLPRS_MASK))
            { };
    }

    /* Setup ARM System control register */
    if (flags & LPM_CPU_POWER_MODE_FLAG_DEEP_SLEEP)
    {
        scr |= SCB_SCR_SLEEPDEEP_MASK;
    }
    if (flags & LPM_CPU_POWER_MODE_FLAG_SLEEP_ON_EXIT)
    {
        scr |= SCB_SCR_SLEEPONEXIT_MASK;
    }
    SCB_SCR = scr;

    /* Setup wake up unit for LLS and VLLSx mode */
    if ((LPM_CPU_POWER_MODE_LLS == index)
        ||(LPM_CPU_POWER_MODE_VLLS3  == index)
        || (LPM_CPU_POWER_MODE_VLLS2 == index)
        || (LPM_CPU_POWER_MODE_VLLS1 == index))
    {
        LLWU_PE1 = operation_modes[target_mode].PE1;
        LLWU_PE2 = operation_modes[target_mode].PE2;
        LLWU_PE3 = operation_modes[target_mode].PE3;
        LLWU_PE4 = operation_modes[target_mode].PE4;
        LLWU_ME = operation_modes[target_mode].ME;
        LLWU_F1 = 0xFF;
        LLWU_F2 = 0xFF;
        LLWU_F3 = 0xFF;
    }

    /* Setup wake up unit for VLLSx mode */
    if ((LPM_CPU_POWER_MODE_VLLS3 == index)
        || (LPM_CPU_POWER_MODE_VLLS2 == index)
        || (LPM_CPU_POWER_MODE_VLLS1 == index))
    {
        LLWU_PE1 = operation_modes[target_mode].PE1;
        LLWU_PE2 = operation_modes[target_mode].PE2;
        LLWU_PE3 = operation_modes[target_mode].PE3;
        LLWU_PE4 = operation_modes[target_mode].PE4;
        LLWU_ME = operation_modes[target_mode].ME;
        LLWU_F1 = 0xFF;
        LLWU_F2 = 0xFF;
        LLWU_F3 = 0xFF;
    }

    /* Keep status of MCG before mode change */
    mcg = MCG_S & MCG_S_CLKST_MASK;

    /* Disable CME if enabled before entering changing Power mode */
    cme = MCG_C6 & MCG_C6_CME_MASK;            /* Save CME state */
    MCG_C6 &= ~(MCG_C6_CME_MASK);              /* Clear CME */

    /* Operation mode setup */
    MC_PMCTRL = mode_ptr->PMCTRL;

    /* Wait for proper setup of VLPR */
    if (LPM_CPU_POWER_MODE_VLPR == index)
    {
        /* K40 P2.2, K60 P2.2 and K20 P1 use different macro from previous chips */
       while (0 == (PMC_REGSC & PMC_REGSC_VLPRS_MASK))
            { };
    }

    /* Go to sleep if required */
    if (flags & LPM_CPU_POWER_MODE_FLAG_USE_WFI)
    {
        _ASM_SLEEP(NULL);
    }

    /* After stop modes, reconfigure MCG if needed */
    if ( (LPM_CPU_POWER_MODE_STOP == index)
      || (LPM_CPU_POWER_MODE_VLPS == index)
      || (LPM_CPU_POWER_MODE_LLS == index)
      || (LPM_CPU_POWER_MODE_VLLS3 == index)
      || (LPM_CPU_POWER_MODE_VLLS2 == index)
      || (LPM_CPU_POWER_MODE_VLLS1 == index) )
    {
        if ((MCG_S_CLKST(3) == mcg) && (MCG_S_CLKST(2) == (MCG_S & MCG_S_CLKST_MASK)))
        {
            MCG_C1 &= (~ (MCG_C1_CLKS_MASK | MCG_C1_IREFS_MASK));
            while (0 == (MCG_S & MCG_S_LOCK_MASK))
                { };
        }
    }

    /* Restore CME */
    MCG_C6 |= cme;

    return MQX_OK;

#else

#ifdef Cpu_SetOperationMode_METHOD_ENABLED
    if (LPM_OPERATION_MODES <= (_mqx_uint)target_mode)
    {
        return MQX_INVALID_PARAMETER;
    }
    return Cpu_SetOperationMode (LPM_PE_OPERATION_MODE_MAP[target_mode], NULL, NULL);
#else
    #error Undefined method Cpu_SetOperationMode() in PE CPU component!
#endif

#endif

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
    SCB_SCR &= (~ (SCB_SCR_SLEEPONEXIT_MASK));
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
    _mqx_uint pmctrl, stop;

    pmctrl = MC_PMCTRL;
    stop = SCB_SCR & SCB_SCR_SLEEPDEEP_MASK;

    /* Idle sleep is available only in normal RUN/WAIT and VLPR/VLPW with LPWUI disabled */
    if ((0 == stop) && (0 == (pmctrl & MC_PMCTRL_LPLLSM_MASK)) && (! ((MC_PMCTRL_LPWUI_MASK | MC_PMCTRL_RUNM(2)) == (pmctrl & (MC_PMCTRL_LPWUI_MASK | MC_PMCTRL_RUNM_MASK)))))
    {
        return TRUE;
    }

    return FALSE;
}

/*FUNCTION*------------------------------------------------------------------
*
* Function Name    : _lpm_llwu_clear_flag
* Returned Value   : Void
* Comments         :
*    This function is used to clear LLWU flags and pass them to caller
*
*END*----------------------------------------------------------------------*/

void _lpm_llwu_clear_flag
    (
        /* [OUT] Pointer stores value of LLWU_Fx flags */
        uint32_t *llwu_fx_ptr
    )
{
    volatile uint8_t LLWU_F1_TMP;
    volatile uint8_t LLWU_F2_TMP;
    volatile uint8_t LLWU_F3_TMP;

    /* Read LLWU_Fx into temporary LLWU_Fx_TMP variables */
    LLWU_F1_TMP = LLWU_F1;
    LLWU_F2_TMP = LLWU_F2;
    LLWU_F3_TMP = LLWU_F3;

    /* clean wakeup flags */
    LLWU_F1 = LLWU_F1_TMP;
    LLWU_F2 = LLWU_F2_TMP;

    *llwu_fx_ptr = (uint32_t)(LLWU_F3_TMP << 16) | (LLWU_F2_TMP << 8) | (LLWU_F1_TMP);
}


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _lpm_get_reset_source
* Returned Value   : void
* Comments         :
*   Return reset source of Kinetis.
*
*END*-------------------------------------------------------------------------*/

uint32_t _lpm_get_reset_source()
{
    if (MC_SRSL & MC_SRSL_WAKEUP_MASK)
        return MQX_RESET_SOURCE_LLWU;
    else if (MC_SRSL & MC_SRSL_LVD_MASK)
        return MQX_RESET_SOURCE_LOW_VOLTAGE_DETECT;
    else if (MC_SRSL & MC_SRSL_LOC_MASK)
        return MQX_RESET_SOURCE_LOSS_OF_CLOCK;
    else if (MC_SRSL & MC_SRSL_COP_MASK)
        return MQX_RESET_SOURCE_WATCHDOG;
    else if (MC_SRSL & MC_SRSL_PIN_MASK)
        return MQX_RESET_SOURCE_EXTERNAL_PIN;
    else if (MC_SRSL & MC_SRSL_POR_MASK)
        return MQX_RESET_SOURCE_POWER_ON;
    else if (MC_SRSH & MC_SRSH_JTAG_MASK)
        return MQX_RESET_SOURCE_JTAG;
    else if (MC_SRSH & MC_SRSH_LOCKUP_MASK)
        return MQX_RESET_SOURCE_CORE_LOCKUP;
    else if (MC_SRSH & MC_SRSH_SW_MASK)
        return MQX_RESET_SOURCE_SOFTWARE;
    else
        return MQX_RESET_SOURCE_INVALID;
}


/*FUNCTION*------------------------------------------------------------------
*
* Function Name    : _lpm_write_rfvbat
* Returned Value   : MQX error code
* Comments         :
*    This function write data to a specific RFVBAT register file.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _lpm_write_rfvbat(uint8_t channel, uint32_t data)
{
    if (channel > 7)
        return MQX_INVALID_PARAMETER;

    RFVBAT_REG(channel) = data;

    return MQX_OK;
}


/*FUNCTION*------------------------------------------------------------------
*
* Function Name    : _lpm_read_rfvbat
* Returned Value   : MQX error code
* Comments         :
*    This function read data from a specific RFVBAT register file.
*
*END*----------------------------------------------------------------------*/

uint32_t _lpm_read_rfvbat(uint8_t channel, uint32_t *data_ptr)
{
    if (channel > 7)
        return MQX_INVALID_PARAMETER;

    *data_ptr = RFVBAT_REG(channel);

    return MQX_OK;
}


/*FUNCTION*------------------------------------------------------------------
*
* Function Name    : _lpm_write_rfsys
* Returned Value   : MQX error code
* Comments         :
*    This function write data to a specific SYSTEM register file.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _lpm_write_rfsys(uint8_t channel, uint32_t data)
{
    if (channel > 7)
        return MQX_INVALID_PARAMETER;

    RFSYS_REG(channel) = data;

    return MQX_OK;
}

/*FUNCTION*------------------------------------------------------------------
*
* Function Name    : _lpm_read_rfsys
* Returned Value   : MQX error code
* Comments         :
*    This function read data from a specific SYSTEM register file.
*
*END*----------------------------------------------------------------------*/

uint32_t _lpm_read_rfsys(uint8_t channel, uint32_t *data_ptr)
{
    if (channel > 7)
        return MQX_INVALID_PARAMETER;

    *data_ptr = RFSYS_REG(channel);

    return MQX_OK;
}


#endif /* MQX_ENABLE_LOW_POWER */

/* EOF */
