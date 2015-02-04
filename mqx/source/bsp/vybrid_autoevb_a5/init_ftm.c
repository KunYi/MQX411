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
*   This file contains the bsp related clock setting for ftm.
*
*
*END************************************************************************/

#include "mqx_inc.h"
#include "bsp.h"
#include "bsp_prv.h"
#include "io_rev.h"
#include "bsp_rev.h"
#include "vybrid.h"

const QUADDEC_INIT_INFO_STRUCT _bsp_ftm1_quaddec_init = {
    1,
    FIXED_FREQ_CLK,
    PHASE_MODE,
    NORMAL_POL,
    NORMAL_POL,
    0xf,
    1,
    0,
    TRUE,
    TRUE,
};

const QUADDEC_INIT_INFO_STRUCT _bsp_ftm2_quaddec_init = {
    2,
    FIXED_FREQ_CLK,
    PHASE_MODE,
    NORMAL_POL,
    NORMAL_POL,
    0xf,
    1,
    0,
    TRUE,
    TRUE,
};

static void init_ftm0_clocks(void)
{
    /* Select 32kHz as  FTM0 fixed clock */
    CCM_CSCMR2 &= ~CCM_CSCMR2_FTM0_FIX_CLK_SEL_MASK;

    /* Select XOSC (12 MHz) as  FTM0 external clock */
    CCM_CSCMR2 |= 2 << CCM_CSCMR2_FTM0_EXT_CLK_SEL_SHIFT;

    /*Enable FTM0 clock*/
    CCM_CSCDR1 |= 1 << CCM_CSCDR1_FTM0_CLK_EN_SHIFT;
}

static void init_ftm1_clocks(void)
{
    /* Select 32kHz as  FTM1 fixed clock */
    CCM_CSCMR2 &= ~CCM_CSCMR2_FTM1_FIX_CLK_SEL_MASK;

    /* Select XOSC (12 MHz) as  FTM1external clock */
    CCM_CSCMR2 |= 2 << CCM_CSCMR2_FTM1_EXT_CLK_SEL_SHIFT;

    /*Enable FTM1 clock*/
    CCM_CSCDR1 |= 1 << CCM_CSCDR1_FTM1_CLK_EN_SHIFT;
}

static void init_ftm2_clocks(void)
{
    /* Select 32kHz as  FTM2 fixed clock */
    CCM_CSCMR2 &= ~CCM_CSCMR2_FTM2_FIX_CLK_SEL_MASK;

    /* Select XOSC (12 MHz) as  FTM2 external clock */
    CCM_CSCMR2 |= 2 << CCM_CSCMR2_FTM2_EXT_CLK_SEL_SHIFT;

    /*Enable FTM2 clock*/
    CCM_CSCDR1 |= 1 << CCM_CSCDR1_FTM2_CLK_EN_SHIFT;
}

static void init_ftm3_clocks(void)
{
    /* Select 32kHz as  FTM3 fixed clock */
    CCM_CSCMR2 &= ~CCM_CSCMR2_FTM3_FIX_CLK_SEL_MASK;

    /* Select XOSC (12 MHz) as  FTM3 external clock */
    CCM_CSCMR2 |= 2 << CCM_CSCMR2_FTM3_EXT_CLK_SEL_SHIFT;

    /*Enable FTM3 clock*/
    CCM_CSCDR1 |= 1 << CCM_CSCDR1_FTM3_CLK_EN_SHIFT;
}

_mqx_uint _bsp_ftm_clocks_init(uint8_t channel)
{
    switch  (channel)   {

    case    0:
            init_ftm0_clocks();
            break;

    case    1:
            init_ftm1_clocks();
            break;

    case    2:
            init_ftm2_clocks();
            break;

    case    3:
            init_ftm3_clocks();
            break;

    default:
            return MQX_INVALID_PARAMETER;
    }

    return MQX_OK;
}

/* EOF */
