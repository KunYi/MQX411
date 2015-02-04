/*HEADER**********************************************************************
*
* Copyright 2012-2013 Freescale Semiconductor, Inc.
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
*   Implementation of subset of clock manager API to provide clock frequencies
*   on platforms without full-featured clock manager
*
*       _bsp_get_clock_configuration();
*       _bsp_get_clock();
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <bsp_prv.h>
#include <bsp_clk_nodes.h>

/* Clock Manager Errors  */
#define ERR_OK          CM_ERR_OK               /* OK */
#define ERR_SPEED       CM_ERR_SPEED            /* This device does not work in the active speed mode. */
#define ERR_RANGE       CM_ERR_RANGE            /* Parameter out of range. */
#define ERR_VALUE       CM_ERR_VALUE            /* Parameter of incorrect value. */
#define ERR_FAILED      CM_ERR_FAILED           /* Requested functionality or process failed. */
#define ERR_PARAM_MODE  CM_ERR_PARAM_MODE       /* Invalid mode. */

#define CCM_CCSR_SYS_CLK_SEL_PLL1_PFD   0x4     /* select PLL1_PFD as SYS clock root*/

static const uint32_t cm_clock_freq[BSP_CLOCK_CONFIGURATIONS][CM_CLOCK_SOURCES] = {
  /* Clock configuration 0 */
  {
      BSP_CORE_CLOCK,
      BSP_BUS_CLOCK,
      BSP_IPG_CLOCK,
  },
  {
      BSP_CORE_CLOCK_FIRC,
      BSP_BUS_CLOCK_FIRC,
      BSP_IPG_CLOCK_FIRC,
  }
};

typedef uint8_t     LDD_TClockConfiguration; /* CPU clock configuration type. */
typedef uint16_t    LDD_TError;           /* Error type. */

static uint8_t ClockConfigurationID = CPU_CLOCK_CONFIG_0; /* Active clock configuration */

/*
** ===================================================================
**     Method      :  LDD_SetClockConfiguration (component MK60N512MD100)
**
**     Description :
**         This method changes the clock configuration of all LDD
**         components in the project.
** ===================================================================
*/
void LDD_SetClockConfiguration(LDD_TClockConfiguration ClockConfiguration)
{
  (void)ClockConfiguration;            /* Parameter is not used, suppress unused argument warning */
}

/*
** ===================================================================
**     Method      :  Cpu_GetClockConfiguration
**
**     Description :
**         Returns the active clock configuration identifier.
**     Parameters  : None
**     Returns     :
**         ---             - Active clock configuration identifier
** ===================================================================
*/
uint8_t Cpu_GetClockConfiguration(void)
{
  return ClockConfigurationID;         /* Return the actual clock configuration identifier */
}

/*
** ===================================================================
**     Method      :  Cpu_SetClockConfiguration (component MK60N512MD100)
**
**     Description :
**         Calling of this method will cause the clock configuration
**         change and reconfiguration of all components according to
**         the requested clock configuration setting.
**     Parameters  :
**         NAME            - DESCRIPTION
**         ModeID          - Clock configuration identifier
**     Returns     :
**         ---             - ERR_OK - OK.
**                           ERR_RANGE - Mode parameter out of range
** ===================================================================
*/
LDD_TError Cpu_SetClockConfiguration(LDD_TClockConfiguration ModeID)
{
  if (ModeID > 0x02U) {
    return ERR_RANGE;                  /* Undefined clock configuration requested requested */
  }
  switch (ModeID) {
    case CPU_CLOCK_CONFIG_0:
        CCM_CCSR |= CCM_CCSR_FAST_CLK_SEL_MASK;  // fast clock select, 24Mhz FXOSC
        CCM_CCSR |= CCM_CCSR_SYS_CLK_SEL_PLL1_PFD;  // system clock select, PLL1_PFD
        CCM_CCR &= ~CCM_CCR_FIRC_EN_MASK;    // enable FIRC
        break;
    case CPU_CLOCK_CONFIG_1:
        CCM_CCR |= CCM_CCR_FIRC_EN_MASK;    // enable FIRC
        CCM_CCSR &= ~CCM_CCSR_FAST_CLK_SEL_MASK; // fast clock select
        CCM_CCSR &= ~CCM_CCSR_SYS_CLK_SEL_MASK;  // system clock select
        break;
    case CPU_CLOCK_CONFIG_2:
        break;
    default:
        break;
  }
  LDD_SetClockConfiguration(ModeID);   /* Call all LDD components to update the clock configuration */
  ClockConfigurationID = ModeID;       /* Store clock configuration identifier */
  return ERR_OK;
}

BSP_CLOCK_CONFIGURATION _bsp_get_clock_configuration
(
    void
)
{
    return (BSP_CLOCK_CONFIGURATION)Cpu_GetClockConfiguration();
}

static uint32_t Pll4ClockInHz = 0; /* PLL4 clock frequency in Hz */
static LWSEM_STRUCT pll4sem; /* Semaphore for protection of PLL4 setup critical sections */ 

_mqx_int _bsp_set_clock_configuration(BSP_CLOCK_CONFIGURATION clock_configuration)
{
    uint16_t cpu_error = ERR_OK;
    uint32_t result;

    cpu_error = Cpu_SetClockConfiguration((uint8_t)clock_configuration);
    if (cpu_error != ERR_OK) {
        return cpu_error;
    }

    /* Change frequency for system timer */
    result = hwtimer_set_freq(&systimer, BSP_SYSTIMER_SRC_CLK, BSP_ALARM_FREQUENCY);
    if (MQX_OK != result)
    {
        return ERR_FAILED;
    }

    return ERR_OK;
}

#if BSPCFG_ENABLE_CLKTREE_MGMT
/*
** ===================================================================
**     Method      :  clock_tree_init
**
**     Description :
**         Initialize the whole clock tree
**     Parameters  : None
**     Returns     : None
** ===================================================================
*/
void clock_tree_init(void)
{
    int i, ele_cnt;

    ele_cnt = get_nodes_nr();

    for (i=0; i!=ele_cnt; i++) {
        P_CLK_NODE_T entry = get_clk_entry(i);
        entry->parent = entry->get_parent(entry);           /*get_parent_XXX will do hardware register check*/
        clk_add_child(entry->parent, entry);
        if (entry->is_physically_open() && !entry->active) {        /*active is now purely a software processed flag*/
            entry->active = 1;
        }
        if (entry->active == 1) {
            inc_descendant(entry->parent);
        }
    }

    // second round, calc freq
    for (i=0; i!=ele_cnt; i++) {
        P_CLK_NODE_T entry = get_clk_entry(i);
        init_node_freq(entry);
    }
}
#endif

/*
** ===================================================================
**     Method      :  _bsp_clock_manager_init
**
**     Description :
**         Placeholder for clock configuration setting
**     Parameters  : clock configuration
**     Returns     :
**         ---            zero
** ===================================================================
*/
void _bsp_clock_manager_init(void)
{
    /* initialize semaphore */
    _lwsem_create(&pll4sem, 1);
#if BSPCFG_ENABLE_CLKTREE_MGMT
    clock_tree_init();
#endif
}

/*
** ===================================================================
**     Method      :  _bsp_get_pll_clock
**
**     Description :
**         This function returns clock freqency in Hertz for specified pllx_pfdy clock source
**
**     Parameters  : pll_num, pfd_num
**     Returns     :
**         ---      Clock freqenecy in Hertz
** ===================================================================
*/
static uint32_t _bsp_get_pll_clock
(
    /* [IN] PLL number, the valid value is: CM_CLOCK_PLL1 ~ CM_CLOCK_PLL7 */
    CM_CLOCK_PLL pll_num,

    /* [IN] PLL PFD number. If want to get PLL main clock, use CM_CLOCK_PLL_main.
         For other PFD number, the valid value is: CM_CLOCK_PLL1 ~ CM_CLOCK_PLL4 */
    CM_CLOCK_PLL_PFD pfd_num
)
{
    Anadig_MemMapPtr anadig = Anadig_BASE_PTR;
    uint32_t mfi, mfn, mfd;
    uint32_t pll_clock, pfd_frac;

    if (pll_num >= CM_CLOCK_PLL4 && pfd_num != CM_CLOCK_PLL_main) {
        printf("Error: PLL4~PLL6 don't have PFD clocks!");
        return 0;
    }

    switch (pll_num) {
        case CM_CLOCK_PLL1:
            mfi = (anadig->PLL1_CTRL & Anadig_PLL1_CTRL_DIV_SELECT_MASK) >> Anadig_PLL1_CTRL_DIV_SELECT_SHIFT;
            mfn = anadig->PLL1_NUM;
            mfd = anadig->PLL1_DENOM;

            mfi = (mfi == 0) ? 20 : 22;
            pll_clock = (uint32_t) ((CPU_XTAL_CLK_HZ * mfi) + ((long long) CPU_XTAL_CLK_HZ * mfn) / mfd);

            if (pfd_num == CM_CLOCK_PLL_main)
                return pll_clock;
            else if (pfd_num == CM_CLOCK_PLL_PFD1)
                pfd_frac = (anadig->PFD_528_SYS & Anadig_PFD_528_SYS_PFD1_FRAC_MASK) >> Anadig_PFD_528_SYS_PFD1_FRAC_SHIFT;
            else if (pfd_num == CM_CLOCK_PLL_PFD2)
                pfd_frac = (anadig->PFD_528_SYS & Anadig_PFD_528_SYS_PFD2_FRAC_MASK) >> Anadig_PFD_528_SYS_PFD2_FRAC_SHIFT;
            else if (pfd_num == CM_CLOCK_PLL_PFD3)
                pfd_frac = (anadig->PFD_528_SYS & Anadig_PFD_528_SYS_PFD3_FRAC_MASK) >> Anadig_PFD_528_SYS_PFD3_FRAC_SHIFT;
            else if (pfd_num == CM_CLOCK_PLL_PFD4)
                pfd_frac = (anadig->PFD_528_SYS & Anadig_PFD_528_SYS_PFD4_FRAC_MASK) >> Anadig_PFD_528_SYS_PFD4_FRAC_SHIFT;
            else {
                printf("Error: PLL1 PFD%d is not supported by %s\n", pfd_num, __func__);
                return 0;
            }
            return ((long long) pll_clock * 18) / pfd_frac;

        case CM_CLOCK_PLL2:
            mfi = (anadig->PLL2_CTRL & Anadig_PLL2_CTRL_DIV_SELECT_MASK) >> Anadig_PLL2_CTRL_DIV_SELECT_SHIFT;
            mfn = anadig->PLL2_NUM;
            mfd = anadig->PLL2_DENOM;

            mfi = (mfi == 0) ? 20 : 22;
            pll_clock = (uint32_t) ((CPU_XTAL_CLK_HZ * mfi) + ((long long) CPU_XTAL_CLK_HZ * mfn) / mfd);

            if (pfd_num == CM_CLOCK_PLL_main)
                return pll_clock;
            else if (pfd_num == CM_CLOCK_PLL_PFD1)
                pfd_frac = (anadig->PLL_PFD_528 & Anadig_PLL_PFD_528_PFD1_FRAC_MASK) >> Anadig_PLL_PFD_528_PFD1_FRAC_SHIFT;
            else if (pfd_num == CM_CLOCK_PLL_PFD2)
                pfd_frac = (anadig->PLL_PFD_528 & Anadig_PLL_PFD_528_PFD2_FRAC_MASK) >> Anadig_PLL_PFD_528_PFD2_FRAC_SHIFT;
            else if (pfd_num == CM_CLOCK_PLL_PFD3)
                pfd_frac = (anadig->PLL_PFD_528 & Anadig_PLL_PFD_528_PFD3_FRAC_MASK) >> Anadig_PLL_PFD_528_PFD3_FRAC_SHIFT;
            else if (pfd_num == CM_CLOCK_PLL_PFD4)
                pfd_frac = (anadig->PLL_PFD_528 & Anadig_PLL_PFD_528_PFD4_FRAC_MASK) >> Anadig_PLL_PFD_528_PFD4_FRAC_SHIFT;
            else {
                printf("Error: PLL2 PFD%d is not supported by %s\n", pfd_num, __func__);
                return 0;
            }
            return ((long long) pll_clock * 18) / pfd_frac;

        case CM_CLOCK_PLL3:
            mfi = (anadig->PLL3_CTRL & Anadig_PLL3_CTRL_DIV_SELECT_MASK) >> Anadig_PLL3_CTRL_DIV_SELECT_SHIFT;

            mfi = (mfi == 0) ? 20: 22;
            pll_clock = CPU_XTAL_CLK_HZ * mfi;

            if (pfd_num == CM_CLOCK_PLL_main)
                return pll_clock;
            else if (pfd_num == CM_CLOCK_PLL_PFD1)
                pfd_frac = (anadig->PLL_PFD_480_USB0 & Anadig_PLL_PFD_480_USB0_PFD1_FRAC_MASK) >> Anadig_PLL_PFD_480_USB0_PFD1_FRAC_SHIFT;
            else if (pfd_num == CM_CLOCK_PLL_PFD2)
                pfd_frac = (anadig->PLL_PFD_480_USB0 & Anadig_PLL_PFD_480_USB0_PFD2_FRAC_MASK) >> Anadig_PLL_PFD_480_USB0_PFD2_FRAC_SHIFT;
            else if (pfd_num == CM_CLOCK_PLL_PFD3)
                pfd_frac = (anadig->PLL_PFD_480_USB0 & Anadig_PLL_PFD_480_USB0_PFD3_FRAC_MASK) >> Anadig_PLL_PFD_480_USB0_PFD3_FRAC_SHIFT;
            else if (pfd_num == CM_CLOCK_PLL_PFD4)
                pfd_frac = (anadig->PLL_PFD_480_USB0 & Anadig_PLL_PFD_480_USB0_PFD4_FRAC_MASK) >> Anadig_PLL_PFD_480_USB0_PFD4_FRAC_SHIFT;
            else {
                printf("Error: PLL2 PFD%d is not supported by %s\n", pfd_num, __func__);
                return 0;
            }
            return ((long long) pll_clock * 18) / pfd_frac;

        case CM_CLOCK_PLL4:
            mfi = (anadig->PLL4_CTRL & Anadig_PLL4_CTRL_DIV_SELECT_MASK) >> Anadig_PLL4_CTRL_DIV_SELECT_SHIFT;
            mfn = anadig->PLL4_NUM;
            mfd = anadig->PLL4_DENOM;

            pll_clock = (uint32_t) ((CPU_XTAL_CLK_HZ * mfi) + ((long long) CPU_XTAL_CLK_HZ * mfn) / mfd);
            return pll_clock;

        case CM_CLOCK_PLL5:
            mfi = (anadig->PLL5_CTRL & Anadig_PLL5_CTRL_DIV_SELECT_MASK) >> Anadig_PLL5_CTRL_DIV_SELECT_SHIFT;

            if (mfi == 0x01)
                pll_clock = 50000000;
            return pll_clock;

        case CM_CLOCK_PLL6:
            mfi = (anadig->PLL6_CTRL & Anadig_PLL6_CTRL_DIV_SELECT_MASK) >> Anadig_PLL6_CTRL_DIV_SELECT_SHIFT;
            mfn = anadig->PLL6_NUM;
            mfd = anadig->PLL6_DENOM;

            pll_clock = (uint32_t) ((CPU_XTAL_CLK_HZ * mfi) + ((long long) CPU_XTAL_CLK_HZ * mfn) / mfd);
            return pll_clock;

        case CM_CLOCK_PLL7:
            mfi = (anadig->PLL3_CTRL & Anadig_PLL7_CTRL_DIV_SELECT_MASK) >> Anadig_PLL7_CTRL_DIV_SELECT_SHIFT;

            mfi = (mfi ==0) ? 20: 22;
            pll_clock = CPU_XTAL_CLK_HZ * mfi;
            return pll_clock;

        default:
            printf("Error: PLL%d is not supported by %s!\n", pll_num, __func__);
            return 0;
    }
}


uint32_t _bsp_get_clock
(
    const BSP_CLOCK_CONFIGURATION   clock_configuration,
    const CM_CLOCK_SOURCE           clock_source
)
{
   /*For PLL4 audio*/
    if (clock_source == CM_CLOCK_SOURCE_PLL_AUDIO)
    {
        Anadig_MemMapPtr anadig = Anadig_BASE_PTR;
        /* integer part of divider */
        uint32_t mfi = (anadig->PLL4_CTRL & Anadig_PLL4_CTRL_DIV_SELECT_MASK) >> Anadig_PLL4_CTRL_DIV_SELECT_SHIFT;
        /* divider numerator */
        uint32_t mfn = anadig->PLL4_NUM;
        /* divider denominator */
        uint32_t mfd = anadig->PLL4_DENOM;
        
        /* If function is called after PLL4 clock change, recalculate global Pll4clockInHz */
        _lwsem_wait(&pll4sem);
        if (!Pll4ClockInHz)
        {
            /* If PLL4 is active and locked */
            if ((anadig->PLL4_CTRL & (Anadig_PLL4_CTRL_LOCK_MASK | Anadig_PLL4_CTRL_ENABLE_MASK)) &&
               !(anadig->PLL4_CTRL & (Anadig_PLL4_CTRL_BYPASS_CLK_SRC_MASK | Anadig_PLL4_CTRL_POWERDOWN_MASK)))
            {
                Pll4ClockInHz = (uint32_t) ((CPU_XTAL_CLK_HZ * mfi) + ((long long) CPU_XTAL_CLK_HZ * mfn) / mfd);
            }
            else
            {
                _lwsem_post(&pll4sem);
                return(0);
            }
        }
        _lwsem_post(&pll4sem);
        return (Pll4ClockInHz);
    }
    if ((clock_source <= CM_CLOCK_SOURCE_IPG) && (clock_configuration < BSP_CLOCK_CONFIGURATIONS))
    {
        return cm_clock_freq[clock_configuration][clock_source];
    }
    else
    {
        switch (clock_source)
        {
            case CM_CLOCK_SOURCE_PLL1_PFD4:
                return _bsp_get_pll_clock(CM_CLOCK_PLL1, CM_CLOCK_PLL_PFD4);

            case CM_CLOCK_SOURCE_PLL2_PFD4:
                return _bsp_get_pll_clock(CM_CLOCK_PLL2, CM_CLOCK_PLL_PFD4);

            case CM_CLOCK_SOURCE_PLL_USB0:
                return _bsp_get_pll_clock(CM_CLOCK_PLL3, CM_CLOCK_PLL_main);

            case CM_CLOCK_SOURCE_PLL3_PFD4:
                return _bsp_get_pll_clock(CM_CLOCK_PLL3, CM_CLOCK_PLL_PFD4);
            default:
                break;
        }

        return 0;
    }
}
uint32_t _bsp_set_clock
(
    const uint32_t        frequency,
    const CM_CLOCK_SOURCE clock_source
)
{
    if (clock_source == CM_CLOCK_SOURCE_PLL_AUDIO)
    {
        Anadig_MemMapPtr anadig = Anadig_BASE_PTR;

        /* If PLL4 is disabled, bypassed or powered down return */
        if (anadig->PLL4_CTRL & (Anadig_PLL4_CTRL_BYPASS_CLK_SRC_MASK | Anadig_PLL4_CTRL_POWERDOWN_MASK) ||
           !(anadig->PLL4_CTRL & Anadig_PLL4_CTRL_ENABLE_MASK))    
        {
            return(0);
        }
        /* Lock PLL4 */
        _lwsem_wait(&pll4sem);

        /* Disable PLL4 */
        anadig->PLL4_CTRL &= (~Anadig_PLL4_CTRL_ENABLE_MASK);
        
        anadig->PLL4_DENOM = CPU_PLL_AUDIO_DENOM;
        /* Frequency lower or same as minimum */
        if (frequency <= CPU_PLL_AUDIO_CLK_HZ_0)
        {
            anadig->PLL4_NUM = CPU_PLL_AUDIO_NUM_0;
            anadig->PLL4_CTRL = (anadig->PLL4_CTRL & ~Anadig_PLL4_CTRL_DIV_SELECT_MASK) | Anadig_PLL4_CTRL_DIV_SELECT(CPU_PLL_AUDIO_DIV_0);
        }
        /* Frequency between PLL freq. 0 and 1 */
        else if (frequency <= CPU_PLL_AUDIO_CLK_HZ_1)
        {
            /* lower half*/
            if (frequency <= (CPU_PLL_AUDIO_CLK_HZ_0 + (CPU_PLL_AUDIO_CLK_HZ_1 - CPU_PLL_AUDIO_CLK_HZ_0) / 2))
            {
                anadig->PLL4_NUM = CPU_PLL_AUDIO_NUM_0;
                anadig->PLL4_CTRL = (anadig->PLL4_CTRL & ~Anadig_PLL4_CTRL_DIV_SELECT_MASK) | Anadig_PLL4_CTRL_DIV_SELECT(CPU_PLL_AUDIO_DIV_0);
            }
            /* upper half */
            else
            {
                anadig->PLL4_NUM = CPU_PLL_AUDIO_NUM_1;
                anadig->PLL4_CTRL = (anadig->PLL4_CTRL & ~Anadig_PLL4_CTRL_DIV_SELECT_MASK) | Anadig_PLL4_CTRL_DIV_SELECT(CPU_PLL_AUDIO_DIV_1);
            }
        }
        /* Frequency between PLL freq. 1 and 2 */
        else if (frequency <= CPU_PLL_AUDIO_CLK_HZ_2)
        {
            /* lower half*/
            if (frequency <= (CPU_PLL_AUDIO_CLK_HZ_1 + (CPU_PLL_AUDIO_CLK_HZ_2 - CPU_PLL_AUDIO_CLK_HZ_1) / 2))
            {
                anadig->PLL4_NUM = CPU_PLL_AUDIO_NUM_1;
                anadig->PLL4_CTRL = (anadig->PLL4_CTRL & ~Anadig_PLL4_CTRL_DIV_SELECT_MASK) | Anadig_PLL4_CTRL_DIV_SELECT(CPU_PLL_AUDIO_DIV_1);
            }
            /* upper half */
            else
            {
                anadig->PLL4_NUM = CPU_PLL_AUDIO_NUM_2;
                anadig->PLL4_CTRL = (anadig->PLL4_CTRL & ~Anadig_PLL4_CTRL_DIV_SELECT_MASK) | Anadig_PLL4_CTRL_DIV_SELECT(CPU_PLL_AUDIO_DIV_2);
            }
        }
        /* Frequency between PLL freq. 2 and 3 */
        else if (frequency <= CPU_PLL_AUDIO_CLK_HZ_3)
        {
            /* lower half*/
            if (frequency <= (CPU_PLL_AUDIO_CLK_HZ_2 + (CPU_PLL_AUDIO_CLK_HZ_3 - CPU_PLL_AUDIO_CLK_HZ_2) / 2))
            {
                anadig->PLL4_NUM = CPU_PLL_AUDIO_NUM_2;
                anadig->PLL4_CTRL = (anadig->PLL4_CTRL & ~Anadig_PLL4_CTRL_DIV_SELECT_MASK) | Anadig_PLL4_CTRL_DIV_SELECT(CPU_PLL_AUDIO_DIV_2);
            }
            /* upper half */
            else
            {
                anadig->PLL4_NUM = CPU_PLL_AUDIO_NUM_3;
                anadig->PLL4_CTRL = (anadig->PLL4_CTRL & ~Anadig_PLL4_CTRL_DIV_SELECT_MASK) | Anadig_PLL4_CTRL_DIV_SELECT(CPU_PLL_AUDIO_DIV_3);
            }
        }
		/* Frequency between PLL freq. 3 and 4 */
        else if (frequency <= CPU_PLL_AUDIO_CLK_HZ_4)
        {
            /* lower half*/
            if (frequency <= (CPU_PLL_AUDIO_CLK_HZ_3 + (CPU_PLL_AUDIO_CLK_HZ_4 - CPU_PLL_AUDIO_CLK_HZ_3) / 2))
            {
                anadig->PLL4_NUM = CPU_PLL_AUDIO_NUM_3;
                anadig->PLL4_CTRL = (anadig->PLL4_CTRL & ~Anadig_PLL4_CTRL_DIV_SELECT_MASK) | Anadig_PLL4_CTRL_DIV_SELECT(CPU_PLL_AUDIO_DIV_3);
            }
            /* upper half */
            else
            {
                anadig->PLL4_NUM = CPU_PLL_AUDIO_NUM_4;
                anadig->PLL4_CTRL = (anadig->PLL4_CTRL & ~Anadig_PLL4_CTRL_DIV_SELECT_MASK) | Anadig_PLL4_CTRL_DIV_SELECT(CPU_PLL_AUDIO_DIV_4);
            }
        }
        /* Frequency higher or same as maximum */
        else
        {
            anadig->PLL4_NUM = CPU_PLL_AUDIO_NUM_4;
            anadig->PLL4_CTRL = (anadig->PLL4_CTRL & ~Anadig_PLL4_CTRL_DIV_SELECT_MASK) | Anadig_PLL4_CTRL_DIV_SELECT(CPU_PLL_AUDIO_DIV_4);
        }
        anadig->PLL4_CTRL |= (Anadig_PLL4_CTRL_ENABLE_MASK);

        /* Change in PLL4 dividers, so set global Pll4ClockInHz to zero -> in will be recalculated */
        Pll4ClockInHz = 0;
        _lwsem_post(&pll4sem);
        return(_bsp_get_clock(_bsp_get_clock_configuration(), clock_source));
    }
    return(0);
}
