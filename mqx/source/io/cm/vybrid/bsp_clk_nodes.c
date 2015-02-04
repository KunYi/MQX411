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
*   This file implements each clock nodes and their manipulation
*   entry functions
*
*
*END************************************************************************/

#include "bsp_clk_nodes_impl.h"
#include "string.h"
#include "mqx.h"
#include "bsp.h"

static Anadig_MemMapPtr s_reg_anadig = Anadig_BASE_PTR;
static CCM_MemMapPtr s_reg_ccm = CCM_BASE_PTR;
static SCSC_MemMapPtr s_reg_scsc = SCSC_BASE_PTR;
static CAN_MemMapPtr s_reg_can0 = CAN0_BASE_PTR;
static CAN_MemMapPtr s_reg_can1 = CAN1_BASE_PTR;
static SPDIF_MemMapPtr s_reg_spdif = SPDIF_BASE_PTR;

#define EXTRACT_BITFIELD(reg, bitfield_name) \
    ((reg & bitfield_name##_MASK) >> bitfield_name##_SHIFT)

#define INS_BITFIELD(reg, bitfield_name, val) \
    (reg = (reg & (~bitfield_name##_MASK)) | ((val) << bitfield_name##_SHIFT))

#define TIMEOUT 0x10000

/*
#define WAIT(exp, timeout) \
({ \
    uint8_t result = 0; \
    while (!exp); \
    result = 1; \
    result; \
})

#define WAIT(exp, timeout) do { \
    uint8_t val; \
    do { \
        val = exp; \
    } while (!val); \
} while (0)
*/

#define WAIT(exp, timeout) do { \
    uint8_t val; \
    do { \
        val = exp; \
    } while (!val); \
} while (0)

#define CHECK_PARENT_IDENTICAL  2
#define CHECK_PARENT_INVALID    1
#define CHECK_PARENT_PASS       0

#define CG_OFF_ALL              0U
#define CG_ON_RUN               1U
#define CG_ON_ALL               2U
#define CG_ON_ALL_EXCEPT_STOP   3U

#define FREQ_FXOSC              24000000
#define FREQ_FIRC               24000000
#define FREQ_SXOSC              32000
#define FREQ_SIRC               128000

#define FREQ_ENET_50MHz         50000000

/*
 * Clock nodes implementation
 * TODO : The pseudo code don't include the register manipulation in read
 *        world, these should be added in the function series
 *      - open_clk_XXX
 *          physically open the clock node
 *      - close_clk_XXX
 *          physically close the clock node
 *      - check_parent_XXX
 *          check if the parent valid, if it is valid, check
 *          the old parent and new parent's gating status
 *      - get_parent_XXX
 *          check current physical parent
 *      - set_parent_XXX
 *          physical set altenative clock path
 */

uint8_t calc_freq_general(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    p_self->freq = parent_freq;
    return 1;
}

uint8_t set_freq_na(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    printf("There is no freq adjust register for node [%s]\n", clk_name_str(p_self->name));
    return 0;
}

/*
 * clock_names matchs CLOCK_NAME enumeration
 */
char* clock_names[] = {
    "FXOSC",
    "FIRC",
    "SXOSC",
    "SIRC",
    "FXOSC_DIV",
    "SIRC_DIV",
    "FAST",
    "SLOW",
    "CLK_24M_IRC",
    "LPRC",
    "LFSR",
    "PLL1",
    "PLL1_PFD1",
    "PLL1_PFD2",
    "PLL1_PFD3",
    "PLL1_PFD4",
    "PLL2",
    "PLL2_PFD1",
    "PLL2_PFD2",
    "PLL2_PFD3",
    "PLL2_PFD4",
    "PLL3",
    "PLL3_PFD1",
    "PLL3_PFD2",
    "PLL3_PFD3",
    "PLL3_PFD4",
    "PLL3_DIV",
    "PLL4",
    "PLL4_DIV",
    "PLL5",
    "PLL5_DIV",
    "PLL6",
    "PLL6_DIV",
    "PLL7",
    "PLL1_PFD",
    "PLL2_PFD",
    "SYS",
    "CA5_CORE",
    "PLATFORM_BUS",
    "FLEXBUS",
    "IPS_IPG",
    "DRAMC",
    "AUDIO_EXT",
    "FLEXCAN0",
    "FLEXCAN1",
    "FTM0_EXT",
    "FTM1_EXT",
    "FTM2_EXT",
    "FTM3_EXT",
    "FTM0_FIX",
    "FTM1_FIX",
    "FTM2_FIX",
    "FTM3_FIX",
    "NFC",
    "QSPI0",
    "QSPI1",
    "ENET_EXT",
    "ENET_TS_EXT",
    "ENET_RMII",
    "USBC0",
    "USBC1",
    "ENET_TS",
    "ESDHC0",
    "ESDHC1",
    "DCU_PIXEL0",
    "DCU_PIXEL1",
    "DCU0",
    "DCU1",
    "SPDIF_EXT",
    "MLB_EXT",
    "ESAI_HCKT_EXT",
    "SAI0TX_BCLK_EXT",
    "SAI3_BCLK_EXT",
    "ESAI",
    "SAI0",
    "SAI1",
    "SAI2",
    "SAI3",
    "VIDEO_ADC",
    "GPU",
    "SWO",
    "TRACE",
    "USB0",
    "USB1",
    "SPDIF",
    "DAP",
    "DMA_MUX0",
    "DMA_MUX1",
    "DMA_MUX2",
    "DMA_MUX3",
    "DMA0_TCD",
    "DMA1_TCD",
    "DMA0",
    "DMA1",
    "UART0",
    "UART1",
    "UART2",
    "UART3",
    "UART4",
    "UART5",
    "SPI0",
    "SPI1",
    "SPI2",
    "SPI3",
    "CRC",
    "PDB",
    "PIT",
    "FTM0",
    "FTM1",
    "FTM2",
    "FTM3",
    "ADC0",
    "ADC1",
    "TCON0",
    "TCON1",
    "WDOG_A5",
    "WDOG_M4",
    "LPTMR",
    "RLE",
    "IOMUXC",
    "PORTA_MULT",
    "PORTB_MULT",
    "PORTC_MULT",
    "PORTD_MULT",
    "PORTE_MULT",
    "ANADIG",
    "SCSC",
    "ASRC",
    "EWM",
    "I2C0",
    "I2C1",
    "I2C2",
    "I2C3",
    "WKUP",
    "CCM",
    "GPC",
    "VREG_DIG",
    "CMU",
    "OTP",
    "SLCD",
    "VIDEO_DECODER",
    "VIU",
    "DAC0",
    "DAC1",
    "ENET_MAC0",
    "ENET_MAC1",
    "ENET_L2_SW",
    "MSCM",
    "CA5_SCU_GIC",
    "CA5_INTD",
    "CA5_L2C",
    "NIC0",
    "NIC1",
    "NIC2",
    "NIC3",
    "NIC4",
    "NIC5",
    "NIC6",
    "NIC7",
    "SEMA4",
    "CA5_DBG",
    "CA5_PMU",
    "CA5_ETM",
    "CA5_ROMTABLE",
    "CA5_CTI",
    "CA5_ITM",
    "CA5_ETB",
    "CA5_FUNNEL",
    "PLTF_TCTL",
    "PLTF_TCTU",
    "PLTF_FUNNEL",
    "PLTF_SWO",
    "MAX"
};

char* clk_name_str(CLOCK_NAME clk)
{
    if (clk >= CLK_MAX) {
        return "BAD_NODE";
    } else {
        return clock_names[clk];
    }
}

/*
 * Clock Node FXOSC
 */
static CLK_NODE_T clk_fxosc = {
    .name = CLK_FXOSC,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_fxosc,
    .get_parent = get_parent_fxosc,
    .physically_open_clk = open_clk_fxosc,
    .physically_close_clk = close_clk_fxosc,
    .calc_freq = calc_freq_fxosc,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_fxosc()
{
    return 1;
}

P_CLK_NODE_T get_parent_fxosc()
{
    return NULL;
}

uint8_t open_clk_fxosc()
{
    return 1;
}

uint8_t close_clk_fxosc()
{
    return 1;
}

uint8_t calc_freq_fxosc(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    p_self->freq = FREQ_FXOSC;
    return 1;
}

/*
 * Clock Node FIRC
 */
static CLK_NODE_T clk_firc = {
    .name = CLK_FIRC,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_firc,
    .get_parent = get_parent_firc,
    .physically_open_clk = open_clk_firc,
    .physically_close_clk = close_clk_firc,
    .calc_freq = calc_freq_firc,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_firc()
{
    return 1;
}

P_CLK_NODE_T get_parent_firc()
{
    return NULL;
}

uint8_t open_clk_firc()
{
    return 1;
}

uint8_t close_clk_firc()
{
    return 1;
}

uint8_t calc_freq_firc(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    p_self->freq = FREQ_FIRC;
    return 1;
}

/*
 * Clock Node SXOSC
 */
static CLK_NODE_T clk_sxosc = {
    .name = CLK_SXOSC,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_sxosc,
    .get_parent = get_parent_sxosc,
    .physically_open_clk = open_clk_sxosc,
    .physically_close_clk = close_clk_sxosc,
    .calc_freq = calc_freq_sxosc,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_sxosc()
{
    uint8_t sosc_en;
    sosc_en = EXTRACT_BITFIELD(s_reg_scsc->SOSC_CTR, SCSC_SOSC_CTR_SOSC_EN);
    // writelocalbuf("sosc en is %d, SOSC_CTR is 0x%x\n", sosc_en, s_reg_scsc->SOSC_CTR);
    return sosc_en;
}

P_CLK_NODE_T get_parent_sxosc()
{
    return NULL;
}

uint8_t open_clk_sxosc()
{
    INS_BITFIELD(s_reg_scsc->SOSC_CTR, SCSC_SOSC_CTR_SOSC_EN, 1);
    return 1;
}

uint8_t close_clk_sxosc()
{
    INS_BITFIELD(s_reg_scsc->SOSC_CTR, SCSC_SOSC_CTR_SOSC_EN, 0);
    return 1;
}

uint8_t calc_freq_sxosc(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    p_self->freq = FREQ_SXOSC;
    return 1;
}

/*
 * Clock Node SIRC
 */
static CLK_NODE_T clk_sirc = {
    .name = CLK_SIRC,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_sirc,
    .get_parent = get_parent_sirc,
    .physically_open_clk = open_clk_sirc,
    .physically_close_clk = close_clk_sirc,
    .calc_freq = calc_freq_sirc,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_sirc()
{
    uint8_t     sirc_en, sirc_en_on_fail = 0;
    (void)      sirc_en_on_fail; /* suppress 'unused variable' warning */
    sirc_en = EXTRACT_BITFIELD(s_reg_scsc->SIRC_CTR, SCSC_SIRC_CTR_SIRC_EN);
    sirc_en_on_fail = EXTRACT_BITFIELD(s_reg_scsc->SIRC_CTR, SCSC_SIRC_CTR_SIRC_EN_ON_FAIL);
    // writelocalbuf("sirc en %d, on fail %d\n", sirc_en, sirc_en_on_fail);
    return sirc_en;
}

P_CLK_NODE_T get_parent_sirc()
{
    return NULL;
}

uint8_t open_clk_sirc()
{
    INS_BITFIELD(s_reg_scsc->SIRC_CTR, SCSC_SIRC_CTR_SIRC_EN, 1);
    return 1;
}

uint8_t close_clk_sirc()
{
    INS_BITFIELD(s_reg_scsc->SIRC_CTR, SCSC_SIRC_CTR_SIRC_EN, 0);
    return 1;
}

uint8_t calc_freq_sirc(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    p_self->freq = FREQ_SIRC;
    return 1;
}

/*
 * Clock Node FXOSC_DIV
 */
static CLK_NODE_T clk_fxosc_div = {
    .name = CLK_FXOSC_DIV,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_fxosc_div,
    .get_parent = get_parent_fxosc_div,
    .physically_open_clk = open_clk_fxosc_div,
    .physically_close_clk = close_clk_fxosc_div,
    .calc_freq = calc_freq_fxosc_div,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_fxosc_div()
{
    return get_parent_fxosc_div()->is_physically_open();
}

P_CLK_NODE_T get_parent_fxosc_div()
{
    return &clk_fxosc;
}

uint8_t open_clk_fxosc_div()
{
    return 1;
}

uint8_t close_clk_fxosc_div()
{
    return 1;
}

uint8_t calc_freq_fxosc_div(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    p_self->freq = parent_freq / 2;
    return 1;
}

/*
 * Clock Node SIRC_DIV
 */
static CLK_NODE_T clk_sirc_div = {
    .name = CLK_SIRC_DIV,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_sirc_div,
    .get_parent = get_parent_sirc_div,
    .physically_open_clk = open_clk_sirc_div,
    .physically_close_clk = close_clk_sirc_div,
    .calc_freq = calc_freq_sirc_div,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_sirc_div()
{
    return get_parent_sirc_div()->is_physically_open();
}

P_CLK_NODE_T get_parent_sirc_div()
{
    return &clk_sirc;
}

uint8_t open_clk_sirc_div()
{
    return 1;
}

uint8_t close_clk_sirc_div()
{
    return 1;
}

uint8_t calc_freq_sirc_div(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    p_self->freq = parent_freq / 4;
    return 1;
}

/*
 * Clock Node FAST_CLK
 */
static CLK_NODE_T clk_fast = {
    .name = CLK_FAST,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_fast,
    .get_parent = get_parent_fast,
    .check_parent = check_parent_fast,
    .dump_possible_parent = dump_possible_parent_fast,
    .physically_set_parent = set_parent_fast,
    .physically_open_clk = open_clk_fast,
    .physically_close_clk = close_clk_fast,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_fast()
{
    /*
     * For non-top level clock node's which don't have
     * clock gating, we can't assume the physical open
     * status as 1, doing so will auto enable the up
     * level parent nodes. For such kind of nodes, we
     * use its parent's physical open status instead, this
     * may involve iteration
     */
    return get_parent_fast()->is_physically_open();
}

P_CLK_NODE_T get_parent_fast()
{
    uint8_t fast_clk_sel = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_FAST_CLK_SEL);
    return fast_clk_sel?&clk_fxosc:&clk_firc;
}

uint8_t check_parent_fast(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_fast();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_fxosc) || (parent == &clk_firc)) {
        *old_open = old_parent->is_physically_open();
        *new_open = parent->is_physically_open();
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_fast()
{
    printf("%s / %s\n", clk_name_str(clk_fxosc.name), clk_name_str(clk_firc.name));
    return 1;
}

uint8_t set_parent_fast(P_CLK_NODE_T parent)
{
    if (parent == &clk_fxosc) {
        INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_FAST_CLK_SEL, 1);
    } else if (parent == &clk_firc) {
        INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_FAST_CLK_SEL, 0);
    }

    return 1;
}

uint8_t open_clk_fast()
{
    return 1;
}

uint8_t close_clk_fast()
{
    return 1;
}

/*
 * Clock Node SLOW_CLK
 */
static CLK_NODE_T clk_slow = {
    .name = CLK_SLOW,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_slow,
    .get_parent = get_parent_slow,
    .check_parent = check_parent_slow,
    .dump_possible_parent = dump_possible_parent_slow,
    .physically_set_parent = set_parent_slow,
    .physically_open_clk = open_clk_slow,
    .physically_close_clk = close_clk_slow,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_slow()
{
    return get_parent_slow()->is_physically_open();
}

P_CLK_NODE_T get_parent_slow()
{
    uint8_t slow_clk_sel = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_SLOW_CLK_SEL);
    return slow_clk_sel?&clk_sxosc:&clk_sirc_div;
}

uint8_t check_parent_slow(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_slow();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_sxosc) || (parent == &clk_sirc_div)) {
        *old_open = old_parent->is_physically_open();
        *new_open = parent->is_physically_open();
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_slow()
{
    printf("%s / %s\n", clk_name_str(clk_sxosc.name), clk_name_str(clk_sirc_div.name));
    return 1;
}

uint8_t set_parent_slow(P_CLK_NODE_T parent)
{
    if (parent == &clk_sxosc) {
        INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_SLOW_CLK_SEL, 1);
    } else if (parent == &clk_sirc_div) {
        INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_SLOW_CLK_SEL, 0);
    }

    return 1;
}

uint8_t open_clk_slow()
{
    return 1;
}

uint8_t close_clk_slow()
{
    return 1;
}

/*
 * Clock Node CLK_24M_IRC
 */
static CLK_NODE_T clk_clk_24m_irc = {
    .name = CLK_CLK_24M_IRC,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_clk_24m_irc,
    .get_parent = get_parent_clk_24m_irc,
    .check_parent = check_parent_clk_24m_irc,
    .dump_possible_parent = dump_possible_parent_clk_24m_irc,
    .physically_set_parent = set_parent_clk_24m_irc,
    .physically_open_clk = open_clk_clk_24m_irc,
    .physically_close_clk = close_clk_clk_24m_irc,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_clk_24m_irc()
{
    return get_parent_clk_24m_irc()->is_physically_open();
}

P_CLK_NODE_T get_parent_clk_24m_irc()
{
    uint8_t clk_24m_sel = EXTRACT_BITFIELD(s_reg_anadig->ANA_MISC0,
            Anadig_ANA_MISC0_CLK_24M_IRC_XTAL_SEL);
    return clk_24m_sel?&clk_fxosc:&clk_firc;
}

uint8_t check_parent_clk_24m_irc(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_clk_24m_irc();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_fxosc) || (parent == &clk_firc)) {
        *old_open = old_parent->is_physically_open();
        *new_open = parent->is_physically_open();
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_clk_24m_irc()
{
    printf("%s / %s\n", clk_name_str(clk_fxosc.name), clk_name_str(clk_firc.name));
    return 1;
}

uint8_t set_parent_clk_24m_irc(P_CLK_NODE_T parent)
{
    if (parent == &clk_fxosc) {
        INS_BITFIELD(s_reg_anadig->ANA_MISC0,
                Anadig_ANA_MISC0_CLK_24M_IRC_XTAL_SEL, 1);
    } else if (parent == &clk_firc) {
        INS_BITFIELD(s_reg_anadig->ANA_MISC0,
                Anadig_ANA_MISC0_CLK_24M_IRC_XTAL_SEL, 0);
    }

    return 1;
}

uint8_t open_clk_clk_24m_irc()
{
    return 1;
}

uint8_t close_clk_clk_24m_irc()
{
    return 1;
}

/*
 * Clock Node LPRC_CLK
 */
static CLK_NODE_T clk_lprc = {
    .name = CLK_LPRC,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_lprc,
    .get_parent = get_parent_lprc,
    .physically_open_clk = open_clk_lprc,
    .physically_close_clk = close_clk_lprc,
    .calc_freq = calc_freq_lprc,
    .set_freq = set_freq_lprc,
};

uint8_t is_physically_open_lprc()
{
    return get_parent_lprc()->is_physically_open();
}

P_CLK_NODE_T get_parent_lprc()
{
    return &clk_sirc;
}

uint8_t open_clk_lprc()
{
    return 1;
}

uint8_t close_clk_lprc()
{
    return 1;
}

uint8_t calc_freq_lprc(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_scsc->SIRC_CTR, SCSC_SIRC_CTR_SIRC_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_lprc(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_scsc->SIRC_CTR, SCSC_SIRC_CTR_SIRC_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. SCSC_SIRC_CTR.SIRC_DIV (0~127) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (old_div == new_div) {
        printf("Same Div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 127) {
        printf("Invalid div value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_scsc->SIRC_CTR, SCSC_SIRC_CTR_SIRC_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node LFSR_CLK
 */
static CLK_NODE_T clk_lfsr = {
    .name = CLK_LFSR,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_lfsr,
    .get_parent = get_parent_lfsr,
    .physically_open_clk = open_clk_lfsr,
    .physically_close_clk = close_clk_lfsr,
    .calc_freq = calc_freq_lfsr,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_lfsr()
{
    return get_parent_lfsr()->is_physically_open();
}

P_CLK_NODE_T get_parent_lfsr()
{
    P_CLK_NODE_T pclk;
    if (clk_sxosc.is_physically_open())
        pclk = &clk_sxosc;
    else
        pclk = &clk_sirc_div;
    return pclk;
}

uint8_t open_clk_lfsr()
{
    return 1;
}

uint8_t close_clk_lfsr()
{
    return 1;
}

uint8_t calc_freq_lfsr(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    if (clk_sxosc.active)
        p_self->freq = clk_sxosc.freq;
    else
        p_self->freq = clk_sirc_div.freq;
    return 1;
}

/*
 * Clock Node PLL1
 */
static CLK_NODE_T clk_pll1 = {
    .name = CLK_PLL1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll1,
    .get_parent = get_parent_pll1,
    .physically_open_clk = open_clk_pll1,
    .physically_close_clk = close_clk_pll1,
    .calc_freq = calc_freq_pll1,
    .set_freq = set_freq_pll1,
};

uint8_t is_physically_open_pll1()
{
    uint8_t pll_enable, pll_powerdown, pll_lock;
    pll_enable = EXTRACT_BITFIELD(s_reg_anadig->PLL1_CTRL,
            Anadig_PLL1_CTRL_ENABLE);
    pll_powerdown = EXTRACT_BITFIELD(s_reg_anadig->PLL1_CTRL,
            Anadig_PLL1_CTRL_POWERDOWN);
    pll_lock = EXTRACT_BITFIELD(s_reg_anadig->PLL1_CTRL,
            Anadig_PLL1_CTRL_LOCK);

    if (!pll_powerdown && pll_lock && pll_enable)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll1()
{
    return &clk_clk_24m_irc;
}

uint8_t open_clk_pll1()
{
    // We assume that when PLL is close
    //     - EN is 0
    //     - POWER is down
    // 1. power up and wait lock
    INS_BITFIELD(s_reg_anadig->PLL1_CTRL, Anadig_PLL1_CTRL_POWERDOWN, 0);
    WAIT(EXTRACT_BITFIELD(s_reg_anadig->PLL1_CTRL, Anadig_PLL1_CTRL_LOCK), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PLL1_CTRL, Anadig_PLL1_CTRL_LOCK)) {
        // lock failure
        return 0;
    } else {
        // 2. enable
        INS_BITFIELD(s_reg_anadig->PLL1_CTRL, Anadig_PLL1_CTRL_ENABLE, 1);
    }
    return 1;
}

uint8_t close_clk_pll1()
{
    // We assume that when PLL is close
    //     - POWER is on
    //     - EN is 1
    INS_BITFIELD(s_reg_anadig->PLL1_CTRL, Anadig_PLL1_CTRL_ENABLE, 0);
    INS_BITFIELD(s_reg_anadig->PLL1_CTRL, Anadig_PLL1_CTRL_POWERDOWN, 1);
    return 1;
}

uint8_t calc_freq_pll1(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_anadig->PLL1_CTRL, Anadig_PLL1_CTRL_DIV_SELECT);
    if (div == 0)
        p_self->freq = parent_freq * 20;
    else if (div == 1)
        p_self->freq = parent_freq * 22;
    return 1;
}

uint8_t set_freq_pll1(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_mult, new_mult;
    // Args Check
    numArgs = va_arg(args, int);
    old_mult = EXTRACT_BITFIELD(s_reg_anadig->PLL1_CTRL, Anadig_PLL1_CTRL_DIV_SELECT);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. ANADIG_PLL1_CTRL.DIV_SELECT (0~1) (Current %d)\n", old_mult);
        printf("        - 0 : PLL = F_ref * 20\n");
        printf("        - 1 : PLL = F_ref * 22\n");
        return 0;
    }
    new_mult = va_arg(args, int);
    if (old_mult == new_mult) {
        printf("Same Mult value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_mult > 1) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PLL1_CTRL, Anadig_PLL1_CTRL_DIV_SELECT, new_mult);

    // update the freq
    *old_val = old_mult ? 22 : 20;
    *new_val = new_mult ? 22 : 20;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL1_PFD1
 */
static CLK_NODE_T clk_pll1_pfd1 = {
    .name = CLK_PLL1_PFD1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll1_pfd1,
    .get_parent = get_parent_pll1_pfd1,
    .physically_open_clk = open_clk_pll1_pfd1,
    .physically_close_clk = close_clk_pll1_pfd1,
    .calc_freq = calc_freq_pll1_pfd1,
    .set_freq = set_freq_pll1_pfd1,
};

uint8_t is_physically_open_pll1_pfd1()
{
    uint8_t pfd_en, pfd_stable, pfd_cg;
    pfd_en = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL1_PFD1_EN);
    pfd_stable = EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS,
            Anadig_PFD_528_SYS_PFD1_STABLE);
    pfd_cg = EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS,
            Anadig_PFD_528_SYS_PFD1_CLKGATE);

    if (pfd_en && pfd_stable && !pfd_cg)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll1_pfd1()
{
    return &clk_pll1;
}

uint8_t open_clk_pll1_pfd1()
{
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL1_PFD1_EN, 1);
    INS_BITFIELD(s_reg_anadig->PFD_528_SYS,
            Anadig_PFD_528_SYS_PFD1_CLKGATE, 0);
    // WAIT(EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS,
    //             Anadig_PFD_528_SYS_PFD1_STABLE), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS,
                Anadig_PFD_528_SYS_PFD1_STABLE)) {
        return 0;
    }
    else {
        INS_BITFIELD(s_reg_anadig->PFD_528_SYS,
                Anadig_PFD_528_SYS_PFD1_CLKGATE, 0);
    }
    return 1;
}

uint8_t close_clk_pll1_pfd1()
{
    INS_BITFIELD(s_reg_anadig->PFD_528_SYS,
            Anadig_PFD_528_SYS_PFD1_CLKGATE, 1);
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL1_PFD1_EN, 0);
    return 1;
}

uint8_t calc_freq_pll1_pfd1(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t frac = EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS, Anadig_PFD_528_SYS_PFD1_FRAC);
    p_self->freq = (uint64_t)parent_freq * 18 / frac;
    return 1;
}

uint8_t set_freq_pll1_pfd1(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_frac, new_frac;
    // Args Check
    numArgs = va_arg(args, int);
    old_frac = EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS, Anadig_PFD_528_SYS_PFD1_FRAC);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. ANADIG_PLL1_PFD.PFD1_FRAC (12~35) (Current %d)\n", old_frac);
        printf("        - PFD = PLL * 18 / frac\n");
        return 0;
    }
    new_frac = va_arg(args, int);
    if (new_frac == old_frac) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if ((new_frac < 12) || (new_frac > 35)) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PFD_528_SYS, Anadig_PFD_528_SYS_PFD1_FRAC, new_frac);

    // update the freq
    *old_val = new_frac;
    *new_val = old_frac;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL1_PFD2
 */
static CLK_NODE_T clk_pll1_pfd2 = {
    .name = CLK_PLL1_PFD2,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll1_pfd2,
    .get_parent = get_parent_pll1_pfd2,
    .physically_open_clk = open_clk_pll1_pfd2,
    .physically_close_clk = close_clk_pll1_pfd2,
    .calc_freq = calc_freq_pll1_pfd2,
    .set_freq = set_freq_pll1_pfd2,
};

uint8_t is_physically_open_pll1_pfd2()
{
    uint8_t pfd_en, pfd_stable, pfd_cg;
    pfd_en = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL1_PFD2_EN);
    pfd_stable = EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS,
            Anadig_PFD_528_SYS_PFD2_STABLE);
    pfd_cg = EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS,
            Anadig_PFD_528_SYS_PFD2_CLKGATE);

    if (pfd_en && pfd_stable && !pfd_cg)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll1_pfd2()
{
    return &clk_pll1;
}

uint8_t open_clk_pll1_pfd2()
{
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL1_PFD2_EN, 1);
    INS_BITFIELD(s_reg_anadig->PFD_528_SYS,
            Anadig_PFD_528_SYS_PFD2_CLKGATE, 0);
    // WAIT(EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS,
    //             Anadig_PFD_528_SYS_PFD2_STABLE), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS,
                Anadig_PFD_528_SYS_PFD2_STABLE)) {
        return 0;
    }
    else {
        INS_BITFIELD(s_reg_anadig->PFD_528_SYS,
                Anadig_PFD_528_SYS_PFD2_CLKGATE, 0);
    }
    return 1;
}

uint8_t close_clk_pll1_pfd2()
{
    INS_BITFIELD(s_reg_anadig->PFD_528_SYS,
            Anadig_PFD_528_SYS_PFD2_CLKGATE, 1);
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL1_PFD2_EN, 0);
    return 1;
}

uint8_t calc_freq_pll1_pfd2(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t frac = EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS, Anadig_PFD_528_SYS_PFD2_FRAC);
    p_self->freq = (uint64_t)parent_freq * 18 / frac;
    return 1;
}

uint8_t set_freq_pll1_pfd2(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_frac, new_frac;
    // Args Check
    numArgs = va_arg(args, int);
    old_frac = EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS, Anadig_PFD_528_SYS_PFD2_FRAC);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. ANADIG_PLL1_PFD.PFD2_FRAC (12~35) (Current %d)\n", old_frac);
        printf("        - PFD = PLL * 18 / frac\n");
        return 0;
    }
    new_frac = va_arg(args, int);
    if (new_frac == old_frac) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if ((new_frac < 12) || (new_frac > 35)) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PFD_528_SYS, Anadig_PFD_528_SYS_PFD2_FRAC, new_frac);

    // update the freq
    *old_val = new_frac;
    *new_val = old_frac;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL1_PFD3
 */
static CLK_NODE_T clk_pll1_pfd3 = {
    .name = CLK_PLL1_PFD3,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll1_pfd3,
    .get_parent = get_parent_pll1_pfd3,
    .physically_open_clk = open_clk_pll1_pfd3,
    .physically_close_clk = close_clk_pll1_pfd3,
    .calc_freq = calc_freq_pll1_pfd3,
    .set_freq = set_freq_pll1_pfd3,
};

uint8_t is_physically_open_pll1_pfd3()
{
    uint8_t pfd_en, pfd_stable, pfd_cg;
    pfd_en = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL1_PFD3_EN);
    pfd_stable = EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS,
            Anadig_PFD_528_SYS_PFD3_STABLE);
    pfd_cg = EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS,
            Anadig_PFD_528_SYS_PFD3_CLKGATE);

    if (pfd_en && pfd_stable && !pfd_cg)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll1_pfd3()
{
    return &clk_pll1;
}

uint8_t open_clk_pll1_pfd3()
{
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL1_PFD3_EN, 1);
    INS_BITFIELD(s_reg_anadig->PFD_528_SYS,
            Anadig_PFD_528_SYS_PFD3_CLKGATE, 0);
    // WAIT(EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS,
    //             Anadig_PFD_528_SYS_PFD3_STABLE), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS,
                Anadig_PFD_528_SYS_PFD3_STABLE)) {
        return 0;
    }
    else {
        INS_BITFIELD(s_reg_anadig->PFD_528_SYS,
                Anadig_PFD_528_SYS_PFD3_CLKGATE, 0);
    }
    return 1;
}

uint8_t close_clk_pll1_pfd3()
{
    INS_BITFIELD(s_reg_anadig->PFD_528_SYS,
            Anadig_PFD_528_SYS_PFD3_CLKGATE, 1);
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL1_PFD3_EN, 0);
    return 1;
}

uint8_t calc_freq_pll1_pfd3(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t frac = EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS, Anadig_PFD_528_SYS_PFD3_FRAC);
    p_self->freq = (uint64_t)parent_freq * 18 / frac;
    return 1;
}

uint8_t set_freq_pll1_pfd3(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_frac, new_frac;
    // Args Check
    numArgs = va_arg(args, int);
    old_frac = EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS, Anadig_PFD_528_SYS_PFD3_FRAC);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. ANADIG_PLL1_PFD.PFD3_FRAC (12~35) (Current %d)\n", old_frac);
        printf("        - PFD = PLL * 18 / frac\n");
        return 0;
    }
    new_frac = va_arg(args, int);
    if (new_frac == old_frac) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if ((new_frac < 12) || (new_frac > 35)) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PFD_528_SYS, Anadig_PFD_528_SYS_PFD3_FRAC, new_frac);

    // update the freq
    *old_val = new_frac;
    *new_val = old_frac;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}
/*
 * Clock Node PLL1_PFD4
 */
static CLK_NODE_T clk_pll1_pfd4 = {
    .name = CLK_PLL1_PFD4,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll1_pfd4,
    .get_parent = get_parent_pll1_pfd4,
    .physically_open_clk = open_clk_pll1_pfd4,
    .physically_close_clk = close_clk_pll1_pfd4,
    .calc_freq = calc_freq_pll1_pfd4,
    .set_freq = set_freq_pll1_pfd4,
};

uint8_t is_physically_open_pll1_pfd4()
{
    uint8_t pfd_en, pfd_stable, pfd_cg;
    pfd_en = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL1_PFD4_EN);
    pfd_stable = EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS,
            Anadig_PFD_528_SYS_PFD4_STABLE);
    pfd_cg = EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS,
            Anadig_PFD_528_SYS_PFD4_CLKGATE);

    if (pfd_en && pfd_stable && !pfd_cg)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll1_pfd4()
{
    return &clk_pll1;
}

uint8_t open_clk_pll1_pfd4()
{
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL1_PFD4_EN, 1);
    INS_BITFIELD(s_reg_anadig->PFD_528_SYS,
            Anadig_PFD_528_SYS_PFD4_CLKGATE, 0);
    // WAIT(EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS,
    //             Anadig_PFD_528_SYS_PFD4_STABLE), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS,
                Anadig_PFD_528_SYS_PFD4_STABLE)) {
        return 0;
    }
    else {
        INS_BITFIELD(s_reg_anadig->PFD_528_SYS,
                Anadig_PFD_528_SYS_PFD4_CLKGATE, 0);
    }
    return 1;
}

uint8_t close_clk_pll1_pfd4()
{
    // printf("++++++\t close pll1 pfd4!\n");
    INS_BITFIELD(s_reg_anadig->PFD_528_SYS,
            Anadig_PFD_528_SYS_PFD4_CLKGATE, 1U);
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL1_PFD4_EN, 0);
    return 1;
}

uint8_t calc_freq_pll1_pfd4(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t frac = EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS, Anadig_PFD_528_SYS_PFD4_FRAC);
    p_self->freq = (uint64_t)parent_freq * 18 / frac;
    return 1;
}

uint8_t set_freq_pll1_pfd4(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_frac, new_frac;
    // Args Check
    numArgs = va_arg(args, int);
    old_frac = EXTRACT_BITFIELD(s_reg_anadig->PFD_528_SYS, Anadig_PFD_528_SYS_PFD4_FRAC);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. ANADIG_PLL1_PFD.PFD4_FRAC (12~35) (Current %d)\n", old_frac);
        printf("        - PFD = PLL * 18 / frac\n");
        return 0;
    }
    new_frac = va_arg(args, int);
    if (new_frac == old_frac) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if ((new_frac < 12) || (new_frac > 35)) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PFD_528_SYS, Anadig_PFD_528_SYS_PFD4_FRAC, new_frac);

    // update the freq
    *old_val = new_frac;
    *new_val = old_frac;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}
/*
 * Clock Node PLL2
 */
static CLK_NODE_T clk_pll2 = {
    .name = CLK_PLL2,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll2,
    .get_parent = get_parent_pll2,
    .physically_open_clk = open_clk_pll2,
    .physically_close_clk = close_clk_pll2,
    .calc_freq = calc_freq_pll2,
    .set_freq = set_freq_pll2,
};

uint8_t is_physically_open_pll2()
{
    uint8_t pll_enable, pll_powerdown, pll_lock;
    pll_enable = EXTRACT_BITFIELD(s_reg_anadig->PLL2_CTRL,
            Anadig_PLL2_CTRL_ENABLE);
    pll_powerdown = EXTRACT_BITFIELD(s_reg_anadig->PLL2_CTRL,
            Anadig_PLL1_CTRL_POWERDOWN);
    pll_lock = EXTRACT_BITFIELD(s_reg_anadig->PLL2_CTRL,
            Anadig_PLL1_CTRL_LOCK);

    if (!pll_powerdown && pll_lock && pll_enable)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll2()
{
    return &clk_clk_24m_irc;
}

uint8_t open_clk_pll2()
{
    // We assume that when PLL is close
    //     - EN is 0
    //     - POWER is down
    // 1. power up and wait lock
    INS_BITFIELD(s_reg_anadig->PLL2_CTRL, Anadig_PLL2_CTRL_POWERDOWN, 0);
    WAIT(EXTRACT_BITFIELD(s_reg_anadig->PLL2_CTRL, Anadig_PLL2_CTRL_LOCK), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PLL2_CTRL, Anadig_PLL2_CTRL_LOCK)) {
        // lock failure
        return 0;
    } else {
        // 2. enable
        INS_BITFIELD(s_reg_anadig->PLL2_CTRL, Anadig_PLL2_CTRL_ENABLE, 1);
    }
    return 1;
}

uint8_t close_clk_pll2()
{
    // We assume that when PLL is close
    //     - POWER is on
    //     - EN is 1
    INS_BITFIELD(s_reg_anadig->PLL2_CTRL, Anadig_PLL2_CTRL_ENABLE, 0);
    INS_BITFIELD(s_reg_anadig->PLL2_CTRL, Anadig_PLL2_CTRL_POWERDOWN, 1);
    return 1;
}

uint8_t calc_freq_pll2(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_anadig->PLL2_CTRL, Anadig_PLL2_CTRL_DIV_SELECT);
    if (div == 0)
        p_self->freq = parent_freq * 20;
    else if (div == 1)
        p_self->freq = parent_freq * 22;
    return 1;
}

uint8_t set_freq_pll2(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_anadig->PLL2_CTRL, Anadig_PLL2_CTRL_DIV_SELECT);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - ANADIG_PLL2_CTRL.DIV_SELECT (0~1) (Current %d)\n", old_div);
        printf("        - 0 : PLL = F_ref * 20\n");
        printf("        - 1 : PLL = F_ref * 22\n");
        return 0;
    }
    new_div = va_arg(args, int);

    if (old_div == new_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }

    if (new_div > 1) {
        printf("Invalid div value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PLL2_CTRL, Anadig_PLL2_CTRL_DIV_SELECT, new_div);

    // update the freq
    *old_val = old_div ? 22 : 20;
    *new_val = new_div ? 22 : 20;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL2_PFD1
 */
static CLK_NODE_T clk_pll2_pfd1 = {
    .name = CLK_PLL2_PFD1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll2_pfd1,
    .get_parent = get_parent_pll2_pfd1,
    .physically_open_clk = open_clk_pll2_pfd1,
    .physically_close_clk = close_clk_pll2_pfd1,
    .calc_freq = calc_freq_pll2_pfd1,
    .set_freq = set_freq_pll2_pfd1,
};

uint8_t is_physically_open_pll2_pfd1()
{
    uint8_t pfd_en, pfd_stable, pfd_cg;
    pfd_en = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL2_PFD1_EN);
    pfd_stable = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528,
            Anadig_PLL_PFD_528_PFD1_STABLE);
    pfd_cg = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528,
            Anadig_PLL_PFD_528_PFD1_CLKGATE);

    if (pfd_en && pfd_stable && !pfd_cg)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll2_pfd1()
{
    return &clk_pll2;
}

uint8_t open_clk_pll2_pfd1()
{
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL2_PFD1_EN, 1);
    INS_BITFIELD(s_reg_anadig->PLL_PFD_528,
            Anadig_PLL_PFD_528_PFD1_CLKGATE, 0);
    // WAIT(EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528,
    //             Anadig_PLL_PFD_528_PFD1_STABLE), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528,
                Anadig_PLL_PFD_528_PFD1_STABLE)) {
        return 0;
    }
    else {
        INS_BITFIELD(s_reg_anadig->PLL_PFD_528,
                Anadig_PLL_PFD_528_PFD1_CLKGATE, 0);
    }
    return 1;
}

uint8_t close_clk_pll2_pfd1()
{
    INS_BITFIELD(s_reg_anadig->PLL_PFD_528,
            Anadig_PLL_PFD_528_PFD1_CLKGATE, 1);
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL2_PFD1_EN, 0);
    return 1;
}

uint8_t calc_freq_pll2_pfd1(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t frac = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528, Anadig_PLL_PFD_528_PFD1_FRAC);
    p_self->freq = (uint64_t)parent_freq * 18 / frac;
    return 1;
}

uint8_t set_freq_pll2_pfd1(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_frac, new_frac;
    // Args Check
    numArgs = va_arg(args, int);
    old_frac = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528, Anadig_PLL_PFD_528_PFD1_FRAC);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. ANADIG_PLL2_PFD.PFD1_FRAC (12~35) (Current %d)\n", old_frac);
        printf("        - PFD = PLL * 18 / frac\n");
        return 0;
    }
    new_frac = va_arg(args, int);
    if (new_frac == old_frac) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if ((new_frac < 12) || (new_frac > 35)) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PLL_PFD_528, Anadig_PLL_PFD_528_PFD1_FRAC, new_frac);

    // update the freq
    *old_val = new_frac;
    *new_val = old_frac;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL2_PFD2
 */
static CLK_NODE_T clk_pll2_pfd2 = {
    .name = CLK_PLL2_PFD2,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll2_pfd2,
    .get_parent = get_parent_pll2_pfd2,
    .physically_open_clk = open_clk_pll2_pfd2,
    .physically_close_clk = close_clk_pll2_pfd2,
    .calc_freq = calc_freq_pll2_pfd2,
    .set_freq = set_freq_pll2_pfd2,
};

uint8_t is_physically_open_pll2_pfd2()
{
    uint8_t pfd_en, pfd_stable, pfd_cg;
    pfd_en = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL2_PFD2_EN);
    pfd_stable = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528,
            Anadig_PLL_PFD_528_PFD2_STABLE);
    pfd_cg = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528,
            Anadig_PLL_PFD_528_PFD2_CLKGATE);

    if (pfd_en && pfd_stable && !pfd_cg)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll2_pfd2()
{
    return &clk_pll2;
}

uint8_t open_clk_pll2_pfd2()
{
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL2_PFD2_EN, 1);
    INS_BITFIELD(s_reg_anadig->PLL_PFD_528,
            Anadig_PLL_PFD_528_PFD2_CLKGATE, 0);
    // WAIT(EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528,
    //             Anadig_PLL_PFD_528_PFD2_STABLE), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528,
                Anadig_PLL_PFD_528_PFD2_STABLE)) {
        return 0;
    }
    else {
        INS_BITFIELD(s_reg_anadig->PLL_PFD_528,
                Anadig_PLL_PFD_528_PFD2_CLKGATE, 0);
    }
    return 1;
}

uint8_t close_clk_pll2_pfd2()
{
    INS_BITFIELD(s_reg_anadig->PLL_PFD_528,
            Anadig_PLL_PFD_528_PFD2_CLKGATE, 1);
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL2_PFD2_EN, 0);
    return 1;
}

uint8_t calc_freq_pll2_pfd2(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t frac = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528, Anadig_PLL_PFD_528_PFD2_FRAC);
    p_self->freq = (uint64_t)parent_freq * 18 / frac;
    return 1;
}

uint8_t set_freq_pll2_pfd2(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_frac, new_frac;
    // Args Check
    numArgs = va_arg(args, int);
    old_frac = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528, Anadig_PLL_PFD_528_PFD2_FRAC);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. ANADIG_PLL2_PFD.PFD2_FRAC (12~35) (Current %d)\n", old_frac);
        printf("        - PFD = PLL * 18 / frac\n");
        return 0;
    }
    new_frac = va_arg(args, int);
    if (new_frac == old_frac) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if ((new_frac < 12) || (new_frac > 35)) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PLL_PFD_528, Anadig_PLL_PFD_528_PFD2_FRAC, new_frac);

    // update the freq
    *old_val = new_frac;
    *new_val = old_frac;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL2_PFD3
 */
static CLK_NODE_T clk_pll2_pfd3 = {
    .name = CLK_PLL2_PFD3,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll2_pfd3,
    .get_parent = get_parent_pll2_pfd3,
    .physically_open_clk = open_clk_pll2_pfd3,
    .physically_close_clk = close_clk_pll2_pfd3,
    .calc_freq = calc_freq_pll2_pfd3,
    .set_freq = set_freq_pll2_pfd3,
};

uint8_t is_physically_open_pll2_pfd3()
{
    uint8_t pfd_en, pfd_stable, pfd_cg;
    pfd_en = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL2_PFD3_EN);
    pfd_stable = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528,
            Anadig_PLL_PFD_528_PFD3_STABLE);
    pfd_cg = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528,
            Anadig_PLL_PFD_528_PFD3_CLKGATE);

    if (pfd_en && pfd_stable && !pfd_cg)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll2_pfd3()
{
    return &clk_pll2;
}

uint8_t open_clk_pll2_pfd3()
{
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL2_PFD3_EN, 1);
    INS_BITFIELD(s_reg_anadig->PLL_PFD_528,
            Anadig_PLL_PFD_528_PFD3_CLKGATE, 0);
    // WAIT(EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528,
    //             Anadig_PLL_PFD_528_PFD3_STABLE), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528,
                Anadig_PLL_PFD_528_PFD3_STABLE)) {
        return 0;
    }
    else {
        INS_BITFIELD(s_reg_anadig->PLL_PFD_528,
                Anadig_PLL_PFD_528_PFD3_CLKGATE, 0);
    }
    return 1;
}

uint8_t close_clk_pll2_pfd3()
{
    INS_BITFIELD(s_reg_anadig->PLL_PFD_528,
            Anadig_PLL_PFD_528_PFD3_CLKGATE, 1);
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL2_PFD3_EN, 0);
    return 1;
}

uint8_t calc_freq_pll2_pfd3(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t frac = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528, Anadig_PLL_PFD_528_PFD3_FRAC);
    p_self->freq = (uint64_t)parent_freq * 18 / frac;
    return 1;
}

uint8_t set_freq_pll2_pfd3(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_frac, new_frac;
    // Args Check
    numArgs = va_arg(args, int);
    old_frac = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528, Anadig_PLL_PFD_528_PFD3_FRAC);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. ANADIG_PLL2_PFD.PFD3_FRAC (12~35) (Current %d)\n", old_frac);
        printf("        - PFD = PLL * 18 / frac\n");
        return 0;
    }
    new_frac = va_arg(args, int);
    if (new_frac == old_frac) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if ((new_frac < 12) || (new_frac > 35)) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PLL_PFD_528, Anadig_PLL_PFD_528_PFD3_FRAC, new_frac);

    // update the freq
    *old_val = new_frac;
    *new_val = old_frac;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL2_PFD4
 */
static CLK_NODE_T clk_pll2_pfd4 = {
    .name = CLK_PLL2_PFD4,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll2_pfd4,
    .get_parent = get_parent_pll2_pfd4,
    .physically_open_clk = open_clk_pll2_pfd4,
    .physically_close_clk = close_clk_pll2_pfd4,
    .calc_freq = calc_freq_pll2_pfd4,
    .set_freq = set_freq_pll2_pfd4,
};

uint8_t is_physically_open_pll2_pfd4()
{
    uint8_t pfd_en, pfd_stable, pfd_cg;
    pfd_en = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL2_PFD4_EN);
    pfd_stable = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528,
            Anadig_PLL_PFD_528_PFD4_STABLE);
    pfd_cg = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528,
            Anadig_PLL_PFD_528_PFD4_CLKGATE);

    if (pfd_en && pfd_stable && !pfd_cg)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll2_pfd4()
{
    return &clk_pll2;
}

uint8_t open_clk_pll2_pfd4()
{
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL2_PFD4_EN, 1);
    INS_BITFIELD(s_reg_anadig->PLL_PFD_528,
            Anadig_PLL_PFD_528_PFD4_CLKGATE, 0);
    // WAIT(EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528,
    //             Anadig_PLL_PFD_528_PFD4_STABLE), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528,
                Anadig_PLL_PFD_528_PFD4_STABLE)) {
        return 0;
    }
    else {
        INS_BITFIELD(s_reg_anadig->PLL_PFD_528,
                Anadig_PLL_PFD_528_PFD4_CLKGATE, 0);
    }
    return 1;
}

uint8_t close_clk_pll2_pfd4()
{
    INS_BITFIELD(s_reg_anadig->PLL_PFD_528,
            Anadig_PLL_PFD_528_PFD4_CLKGATE, 1U);
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL2_PFD4_EN, 0);
    return 1;
}

uint8_t calc_freq_pll2_pfd4(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t frac = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528, Anadig_PLL_PFD_528_PFD4_FRAC);
    p_self->freq = (uint64_t)parent_freq * 18 / frac;
    return 1;
}

uint8_t set_freq_pll2_pfd4(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_frac, new_frac;
    // Args Check
    numArgs = va_arg(args, int);
    old_frac = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_528, Anadig_PLL_PFD_528_PFD4_FRAC);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. ANADIG_PLL2_PFD.PFD4_FRAC (12~35) (Current %d)\n", old_frac);
        printf("        - PFD = PLL * 18 / frac\n");
        return 0;
    }
    new_frac = va_arg(args, int);
    if (new_frac == old_frac) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if ((new_frac < 12) || (new_frac > 35)) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PLL_PFD_528, Anadig_PLL_PFD_528_PFD4_FRAC, new_frac);

    // update the freq
    *old_val = new_frac;
    *new_val = old_frac;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL3
 */
static CLK_NODE_T clk_pll3 = {
    .name = CLK_PLL3,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll3,
    .get_parent = get_parent_pll3,
    .physically_open_clk = open_clk_pll3,
    .physically_close_clk = close_clk_pll3,
    .calc_freq = calc_freq_pll3,
    .set_freq = set_freq_pll3,
};

uint8_t is_physically_open_pll3()
{
    uint8_t pll_enable, pll_power, pll_lock;
    pll_enable = EXTRACT_BITFIELD(s_reg_anadig->PLL3_CTRL,
            Anadig_PLL3_CTRL_ENABLE);
    pll_power = EXTRACT_BITFIELD(s_reg_anadig->PLL3_CTRL,
            Anadig_PLL3_CTRL_POWER);
    pll_lock = EXTRACT_BITFIELD(s_reg_anadig->PLL3_CTRL,
            Anadig_PLL3_CTRL_LOCK);

    if (pll_power && pll_lock && pll_enable)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll3()
{
    return &clk_clk_24m_irc;
}

uint8_t open_clk_pll3()
{
    // We assume that when PLL is close
    //     - EN is 0
    //     - POWER is down
    // 1. power up and wait lock
    INS_BITFIELD(s_reg_anadig->PLL3_CTRL, Anadig_PLL3_CTRL_POWER, 1);
    WAIT(EXTRACT_BITFIELD(s_reg_anadig->PLL3_CTRL, Anadig_PLL3_CTRL_LOCK), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PLL3_CTRL, Anadig_PLL3_CTRL_LOCK)) {
        // lock failure
        return 0;
    } else {
        // 2. enable
        INS_BITFIELD(s_reg_anadig->PLL3_CTRL, Anadig_PLL3_CTRL_ENABLE, 1);
    }
    return 1;
}

uint8_t close_clk_pll3()
{
    // We assume that when PLL is close
    //     - POWER is on
    //     - EN is 1
    INS_BITFIELD(s_reg_anadig->PLL3_CTRL, Anadig_PLL3_CTRL_ENABLE, 0);
    INS_BITFIELD(s_reg_anadig->PLL3_CTRL, Anadig_PLL3_CTRL_POWER, 0);
    return 1;
}

uint8_t calc_freq_pll3(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_anadig->PLL3_CTRL, Anadig_PLL3_CTRL_DIV_SELECT);
    if (div == 0)
        p_self->freq = parent_freq * 20;
    else if (div == 1)
        p_self->freq = parent_freq * 22;
    return 1;
}

uint8_t set_freq_pll3(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_anadig->PLL3_CTRL, Anadig_PLL3_CTRL_DIV_SELECT);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - ANADIG_PLL3_CTRL.DIV_SELECT (0~1) (Current %d)\n", old_div);
        printf("        - 0 : PLL = F_ref * 20\n");
        printf("        - 1 : PLL = F_ref * 22\n");
        return 0;
    }
    new_div = va_arg(args, int);

    if (old_div == new_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }

    if (new_div > 1) {
        printf("Invalid div value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PLL3_CTRL, Anadig_PLL3_CTRL_DIV_SELECT, new_div);

    // update the freq
    *old_val = old_div ? 22 : 20;
    *new_val = new_div ? 22 : 20;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL3_PFD1
 */
static CLK_NODE_T clk_pll3_pfd1 = {
    .name = CLK_PLL3_PFD1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll3_pfd1,
    .get_parent = get_parent_pll3_pfd1,
    .physically_open_clk = open_clk_pll3_pfd1,
    .physically_close_clk = close_clk_pll3_pfd1,
    .calc_freq = calc_freq_pll3_pfd1,
    .set_freq = set_freq_pll3_pfd1,
};

uint8_t is_physically_open_pll3_pfd1()
{
    uint8_t pfd_en, pfd_stable, pfd_cg;
    pfd_en = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL3_PFD1_EN);
    pfd_stable = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
            Anadig_PLL_PFD_480_USB0_PFD1_STABLE);
    pfd_cg = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
            Anadig_PLL_PFD_480_USB0_PFD1_CLKGATE);

    if (pfd_en && pfd_stable && !pfd_cg)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll3_pfd1()
{
    return &clk_pll3;
}

uint8_t open_clk_pll3_pfd1()
{
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL3_PFD1_EN, 1);
    INS_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
            Anadig_PLL_PFD_480_USB0_PFD1_CLKGATE, 0);
    // WAIT(EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
    //             Anadig_PLL_PFD_480_USB0_PFD1_STABLE), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
                Anadig_PLL_PFD_480_USB0_PFD1_STABLE)) {
        return 0;
    }
    else {
        INS_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
                Anadig_PLL_PFD_480_USB0_PFD1_CLKGATE, 0);
    }
    return 1;
}

uint8_t close_clk_pll3_pfd1()
{
    INS_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
            Anadig_PLL_PFD_480_USB0_PFD1_CLKGATE, 1);
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL3_PFD1_EN, 0);
    return 1;
}

uint8_t calc_freq_pll3_pfd1(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t frac = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0, Anadig_PLL_PFD_480_USB0_PFD1_FRAC);
    p_self->freq = (uint64_t)parent_freq * 18 / frac;
    return 1;
}

uint8_t set_freq_pll3_pfd1(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_frac, new_frac;
    // Args Check
    numArgs = va_arg(args, int);
    old_frac = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0, Anadig_PLL_PFD_480_USB0_PFD1_FRAC);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. ANADIG_PLL3_PFD.PFD1_FRAC (12~35) (Current %d)\n", old_frac);
        printf("        - PFD = PLL * 18 / frac\n");
        return 0;
    }
    new_frac = va_arg(args, int);
    if (new_frac == old_frac) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if ((new_frac < 12) || (new_frac > 35)) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0, Anadig_PLL_PFD_480_USB0_PFD1_FRAC, new_frac);

    // update the freq
    *old_val = new_frac;
    *new_val = old_frac;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL3_PFD2
 */
static CLK_NODE_T clk_pll3_pfd2 = {
    .name = CLK_PLL3_PFD2,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll3_pfd2,
    .get_parent = get_parent_pll3_pfd2,
    .physically_open_clk = open_clk_pll3_pfd2,
    .physically_close_clk = close_clk_pll3_pfd2,
    .calc_freq = calc_freq_pll3_pfd2,
    .set_freq = set_freq_pll3_pfd2,
};

uint8_t is_physically_open_pll3_pfd2()
{
    uint8_t pfd_en, pfd_stable, pfd_cg;
    pfd_en = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL3_PFD2_EN);
    pfd_stable = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
            Anadig_PLL_PFD_480_USB0_PFD2_STABLE);
    pfd_cg = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
            Anadig_PLL_PFD_480_USB0_PFD2_CLKGATE);

    if (pfd_en && pfd_stable && !pfd_cg)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll3_pfd2()
{
    return &clk_pll3;
}

uint8_t open_clk_pll3_pfd2()
{
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL3_PFD2_EN, 1);
    INS_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
            Anadig_PLL_PFD_480_USB0_PFD2_CLKGATE, 0);
    // WAIT(EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
    //             Anadig_PLL_PFD_480_USB0_PFD2_STABLE), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
                Anadig_PLL_PFD_480_USB0_PFD2_STABLE)) {
        return 0;
    }
    else {
        INS_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
                Anadig_PLL_PFD_480_USB0_PFD2_CLKGATE, 0);
    }
    return 1;
}

uint8_t close_clk_pll3_pfd2()
{
    INS_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
            Anadig_PLL_PFD_480_USB0_PFD2_CLKGATE, 1);
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL3_PFD2_EN, 0);
    return 1;
}

uint8_t calc_freq_pll3_pfd2(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t frac = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0, Anadig_PLL_PFD_480_USB0_PFD2_FRAC);
    p_self->freq = (uint64_t)parent_freq * 18 / frac;
    return 1;
}

uint8_t set_freq_pll3_pfd2(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_frac, new_frac;
    // Args Check
    numArgs = va_arg(args, int);
    old_frac = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0, Anadig_PLL_PFD_480_USB0_PFD2_FRAC);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. ANADIG_PLL3_PFD.PFD2_FRAC (12~35) (Current %d)\n", old_frac);
        printf("        - PFD = PLL * 18 / frac\n");
        return 0;
    }
    new_frac = va_arg(args, int);
    if (new_frac == old_frac) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if ((new_frac < 12) || (new_frac > 35)) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0, Anadig_PLL_PFD_480_USB0_PFD2_FRAC, new_frac);

    // update the freq
    *old_val = new_frac;
    *new_val = old_frac;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL3_PFD3
 */
static CLK_NODE_T clk_pll3_pfd3 = {
    .name = CLK_PLL3_PFD3,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll3_pfd3,
    .get_parent = get_parent_pll3_pfd3,
    .physically_open_clk = open_clk_pll3_pfd3,
    .physically_close_clk = close_clk_pll3_pfd3,
    .calc_freq = calc_freq_pll3_pfd3,
    .set_freq = set_freq_pll3_pfd3,
};

uint8_t is_physically_open_pll3_pfd3()
{
    uint8_t pfd_en, pfd_stable, pfd_cg;
    pfd_en = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL3_PFD3_EN);
    pfd_stable = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
            Anadig_PLL_PFD_480_USB0_PFD3_STABLE);
    pfd_cg = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
            Anadig_PLL_PFD_480_USB0_PFD3_CLKGATE);

    if (pfd_en && pfd_stable && !pfd_cg)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll3_pfd3()
{
    return &clk_pll3;
}

uint8_t open_clk_pll3_pfd3()
{
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL3_PFD3_EN, 1);
    INS_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
            Anadig_PLL_PFD_480_USB0_PFD3_CLKGATE, 0);
    // WAIT(EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
    //             Anadig_PLL_PFD_480_USB0_PFD3_STABLE), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
                Anadig_PLL_PFD_480_USB0_PFD3_STABLE)) {
        return 0;
    }
    else {
        INS_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
                Anadig_PLL_PFD_480_USB0_PFD3_CLKGATE, 0);
    }
    return 1;
}

uint8_t close_clk_pll3_pfd3()
{
    INS_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
            Anadig_PLL_PFD_480_USB0_PFD3_CLKGATE, 1);
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL3_PFD3_EN, 0);
    return 1;
}

uint8_t calc_freq_pll3_pfd3(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t frac = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0, Anadig_PLL_PFD_480_USB0_PFD3_FRAC);
    p_self->freq = (uint64_t)parent_freq * 18 / frac;
    return 1;
}

uint8_t set_freq_pll3_pfd3(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_frac, new_frac;
    // Args Check
    numArgs = va_arg(args, int);
    old_frac = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0, Anadig_PLL_PFD_480_USB0_PFD3_FRAC);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. ANADIG_PLL3_PFD.PFD3_FRAC (12~35) (Current %d)\n", old_frac);
        printf("        - PFD = PLL * 18 / frac\n");
        return 0;
    }
    new_frac = va_arg(args, int);
    if (new_frac == old_frac) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if ((new_frac < 12) || (new_frac > 35)) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0, Anadig_PLL_PFD_480_USB0_PFD3_FRAC, new_frac);

    // update the freq
    *old_val = new_frac;
    *new_val = old_frac;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL3_PFD4
 */
static CLK_NODE_T clk_pll3_pfd4 = {
    .name = CLK_PLL3_PFD4,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll3_pfd4,
    .get_parent = get_parent_pll3_pfd4,
    .physically_open_clk = open_clk_pll3_pfd4,
    .physically_close_clk = close_clk_pll3_pfd4,
    .calc_freq = calc_freq_pll3_pfd4,
    .set_freq = set_freq_pll3_pfd4,
};

uint8_t is_physically_open_pll3_pfd4()
{
    uint8_t pfd_en, pfd_stable, pfd_cg;
    pfd_en = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL3_PFD4_EN);
    pfd_stable = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
            Anadig_PLL_PFD_480_USB0_PFD4_STABLE);
    pfd_cg = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
            Anadig_PLL_PFD_480_USB0_PFD4_CLKGATE);

    if (pfd_en && pfd_stable && !pfd_cg)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll3_pfd4()
{
    return &clk_pll3;
}

uint8_t open_clk_pll3_pfd4()
{
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL3_PFD4_EN, 1U);
    INS_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
            Anadig_PLL_PFD_480_USB0_PFD4_CLKGATE, 0);
    // WAIT(EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
    //             Anadig_PLL_PFD_480_USB0_PFD4_STABLE), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
                Anadig_PLL_PFD_480_USB0_PFD4_STABLE)) {
        return 0;
    }
    else {
        INS_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
                Anadig_PLL_PFD_480_USB0_PFD4_CLKGATE, 0);
    }
    return 1;
}

uint8_t close_clk_pll3_pfd4()
{
    INS_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0,
            Anadig_PLL_PFD_480_USB0_PFD4_CLKGATE, 1U);
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL3_PFD4_EN, 0);
    return 1;
}

uint8_t calc_freq_pll3_pfd4(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t frac = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0, Anadig_PLL_PFD_480_USB0_PFD4_FRAC);
    p_self->freq = (uint64_t)parent_freq * 18 / frac;
    return 1;
}

uint8_t set_freq_pll3_pfd4(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_frac, new_frac;
    // Args Check
    numArgs = va_arg(args, int);
    old_frac = EXTRACT_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0, Anadig_PLL_PFD_480_USB0_PFD4_FRAC);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. ANADIG_PLL3_PFD.PFD4_FRAC (12~35) (Current %d)\n", old_frac);
        printf("        - PFD = PLL * 18 / frac\n");
        return 0;
    }
    new_frac = va_arg(args, int);
    if (new_frac == old_frac) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if ((new_frac < 12) || (new_frac > 35)) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PLL_PFD_480_USB0, Anadig_PLL_PFD_480_USB0_PFD4_FRAC, new_frac);

    // update the freq
    *old_val = new_frac;
    *new_val = old_frac;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL3_DIV
 */
static CLK_NODE_T clk_pll3_div = {
    .name = CLK_PLL3_DIV,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll3_div,
    .get_parent = get_parent_pll3_div,
    .physically_open_clk = open_clk_pll3_div,
    .physically_close_clk = close_clk_pll3_div,
    .calc_freq = calc_freq_pll3_div,
    .set_freq = set_freq_pll3_div,
};

uint8_t is_physically_open_pll3_div()
{
    return get_parent_pll3_div()->is_physically_open();
}

P_CLK_NODE_T get_parent_pll3_div()
{
    return &clk_pll3;
}

uint8_t open_clk_pll3_div()
{
    return 1;
}

uint8_t close_clk_pll3_div()
{
    return 1;
}

uint8_t calc_freq_pll3_div(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_PLL3_CLK_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_pll3_div(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_PLL3_CLK_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - CCM_CACRR.PLL3_CLK_DIV (0~1) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);

    if (new_div == old_div) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }

    if (new_div > 1) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_PLL3_CLK_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL4
 */
static CLK_NODE_T clk_pll4 = {
    .name = CLK_PLL4,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll4,
    .get_parent = get_parent_pll4,
    .physically_open_clk = open_clk_pll4,
    .physically_close_clk = close_clk_pll4,
    .calc_freq = calc_freq_pll4,
    .set_freq = set_freq_pll4,
};

uint8_t is_physically_open_pll4()
{
    uint8_t pll_enable, pll_powerdown, pll_lock;
    pll_enable = EXTRACT_BITFIELD(s_reg_anadig->PLL4_CTRL,
            Anadig_PLL4_CTRL_ENABLE);
    pll_powerdown = EXTRACT_BITFIELD(s_reg_anadig->PLL4_CTRL,
            Anadig_PLL4_CTRL_POWERDOWN);
    pll_lock = EXTRACT_BITFIELD(s_reg_anadig->PLL4_CTRL,
            Anadig_PLL4_CTRL_LOCK);

    if (!pll_powerdown && pll_lock && pll_enable)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll4()
{
    return &clk_clk_24m_irc;
}

uint8_t open_clk_pll4()
{
    // We assume that when PLL is close
    //     - EN is 0
    //     - POWER is down
    // 1. power up and wait lock
    INS_BITFIELD(s_reg_anadig->PLL4_CTRL, Anadig_PLL4_CTRL_POWERDOWN, 0);
    WAIT(EXTRACT_BITFIELD(s_reg_anadig->PLL4_CTRL, Anadig_PLL4_CTRL_LOCK), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PLL4_CTRL, Anadig_PLL4_CTRL_LOCK)) {
        // lock failure
        return 0;
    } else {
        // 2. enable
        INS_BITFIELD(s_reg_anadig->PLL4_CTRL, Anadig_PLL4_CTRL_ENABLE, 1);
    }
    return 1;
}

uint8_t close_clk_pll4()
{
    // We assume that when PLL is close
    //     - POWER is on
    //     - EN is 1
    INS_BITFIELD(s_reg_anadig->PLL4_CTRL, Anadig_PLL4_CTRL_ENABLE, 0);
    INS_BITFIELD(s_reg_anadig->PLL4_CTRL, Anadig_PLL4_CTRL_POWERDOWN, 1);
    return 1;
}

uint8_t calc_freq_pll4(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_anadig->PLL4_CTRL, Anadig_PLL4_CTRL_DIV_SELECT);
    p_self->freq = parent_freq * div;
    return 1;
}

uint8_t set_freq_pll4(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_mult, new_mult;
    // Args Check
    numArgs = va_arg(args, int);
    old_mult = EXTRACT_BITFIELD(s_reg_anadig->PLL4_CTRL, Anadig_PLL4_CTRL_DIV_SELECT);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - ANADIG_PLL4_CTRL.DIV_SELECT (1~127) (Current %d)\n", old_mult);
        return 0;
    }
    new_mult = va_arg(args, int);

    if (new_mult == old_mult) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }

    if ((new_mult == 0) || (new_mult > 127)) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PLL4_CTRL, Anadig_PLL4_CTRL_DIV_SELECT, new_mult);

    // update the freq
    *old_val = old_mult;
    *new_val = new_mult;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);
    return 1;
}

/*
 * Clock Node PLL4_DIV
 */
static CLK_NODE_T clk_pll4_div = {
    .name = CLK_PLL4_DIV,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll4_div,
    .get_parent = get_parent_pll4_div,
    .physically_open_clk = open_clk_pll4_div,
    .physically_close_clk = close_clk_pll4_div,
    .calc_freq = calc_freq_pll4_div,
    .set_freq = set_freq_pll4_div,
};

uint8_t is_physically_open_pll4_div()
{
    return get_parent_pll4_div()->is_physically_open();
}

P_CLK_NODE_T get_parent_pll4_div()
{
    return &clk_pll4;
}

uint8_t open_clk_pll4_div()
{
    return 1;
}

uint8_t close_clk_pll4_div()
{
    return 1;
}

uint8_t calc_freq_pll4_div(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_PLL4_CLK_DIV);
    if (div == 0)
        div = 1;
    else
        div = 2 * (div + 1);

    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_pll4_div(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_PLL4_CLK_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - CCM_CACRR.PLL4_CLK_DIV (0~7) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);

    if (new_div == old_div) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }

    if (new_div > 7) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_PLL4_CLK_DIV, new_div);

    // update the freq
    *old_val = (new_div == 0) ? 1 : 2 * (new_div + 1);
    *new_val = (old_div == 0) ? 1 : 2 * (old_div + 1);
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL5
 */
static CLK_NODE_T clk_pll5 = {
    .name = CLK_PLL5,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll5,
    .get_parent = get_parent_pll5,
    .physically_open_clk = open_clk_pll5,
    .physically_close_clk = close_clk_pll5,
    .calc_freq = calc_freq_pll5,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_pll5()
{
    uint8_t pll_enable, pll_powerdown, pll_lock;
    pll_enable = EXTRACT_BITFIELD(s_reg_anadig->PLL5_CTRL,
            Anadig_PLL5_CTRL_ENABLE);
    pll_powerdown = EXTRACT_BITFIELD(s_reg_anadig->PLL5_CTRL,
            Anadig_PLL5_CTRL_POWERDOWN);
    pll_lock = EXTRACT_BITFIELD(s_reg_anadig->PLL5_CTRL,
            Anadig_PLL5_CTRL_LOCK);

    if (!pll_powerdown && pll_lock && pll_enable)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll5()
{
    return &clk_clk_24m_irc;
}

uint8_t open_clk_pll5()
{
    // We assume that when PLL is close
    //     - EN is 0
    //     - POWER is down
    // 1. power up and wait lock
    INS_BITFIELD(s_reg_anadig->PLL5_CTRL, Anadig_PLL5_CTRL_POWERDOWN, 0);
    WAIT(EXTRACT_BITFIELD(s_reg_anadig->PLL5_CTRL, Anadig_PLL5_CTRL_LOCK), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PLL5_CTRL, Anadig_PLL5_CTRL_LOCK)) {
        // lock failure
        return 0;
    } else {
        // 2. enable
        INS_BITFIELD(s_reg_anadig->PLL5_CTRL, Anadig_PLL5_CTRL_ENABLE, 1);
    }
    return 1;
}

uint8_t close_clk_pll5()
{
    // We assume that when PLL is close
    //     - POWER is on
    //     - EN is 1
    INS_BITFIELD(s_reg_anadig->PLL5_CTRL, Anadig_PLL5_CTRL_ENABLE, 0);
    INS_BITFIELD(s_reg_anadig->PLL5_CTRL, Anadig_PLL5_CTRL_POWERDOWN, 1);
    return 1;
}

uint8_t calc_freq_pll5(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_anadig->PLL5_CTRL, Anadig_PLL5_CTRL_DIV_SELECT);
    if (div == 1)
        p_self->freq = FREQ_ENET_50MHz;
    else
        printf("something is wrong, please check\n");
    return 1;
}

/*
 * Clock Node PLL5_DIV
 *      clock node for enet_rmii, half the freq of pll5
 */
static CLK_NODE_T clk_pll5_div = {
    .name = CLK_PLL5_DIV,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll5_div,
    .get_parent = get_parent_pll5_div,
    .physically_open_clk = open_clk_pll5_div,
    .physically_close_clk = close_clk_pll5_div,
    .calc_freq = calc_freq_pll5_div,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_pll5_div()
{
    return get_parent_pll5_div()->is_physically_open();
}

P_CLK_NODE_T get_parent_pll5_div()
{
    return &clk_pll5;
}

uint8_t open_clk_pll5_div()
{
    return 1;
}

uint8_t close_clk_pll5_div()
{
    return 1;
}

uint8_t calc_freq_pll5_div(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    p_self->freq = parent_freq / 2;
    return 1;
}

/*
 * Clock Node PLL6
 */
static CLK_NODE_T clk_pll6 = {
    .name = CLK_PLL6,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll6,
    .get_parent = get_parent_pll6,
    .physically_open_clk = open_clk_pll6,
    .physically_close_clk = close_clk_pll6,
    .calc_freq = calc_freq_pll6,
    .set_freq = set_freq_pll6,
};

uint8_t is_physically_open_pll6()
{
    uint8_t pll_enable, pll_powerdown, pll_lock;
    pll_enable = EXTRACT_BITFIELD(s_reg_anadig->PLL6_CTRL,
            Anadig_PLL6_CTRL_ENABLE);
    pll_powerdown = EXTRACT_BITFIELD(s_reg_anadig->PLL6_CTRL,
            Anadig_PLL6_CTRL_POWERDOWN);
    pll_lock = EXTRACT_BITFIELD(s_reg_anadig->PLL6_CTRL,
            Anadig_PLL6_CTRL_LOCK);

    if (!pll_powerdown && pll_lock && pll_enable)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll6()
{
    return &clk_clk_24m_irc;
}

uint8_t open_clk_pll6()
{
    // We assume that when PLL is close
    //     - EN is 0
    //     - POWER is down
    // 1. power up and wait lock
    INS_BITFIELD(s_reg_anadig->PLL6_CTRL, Anadig_PLL6_CTRL_POWERDOWN, 0);
    WAIT(EXTRACT_BITFIELD(s_reg_anadig->PLL6_CTRL, Anadig_PLL6_CTRL_LOCK), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PLL6_CTRL, Anadig_PLL6_CTRL_LOCK)) {
        // lock failure
        return 0;
    } else {
        // 2. enable
        INS_BITFIELD(s_reg_anadig->PLL6_CTRL, Anadig_PLL6_CTRL_ENABLE, 1);
    }
    return 1;
}

uint8_t close_clk_pll6()
{
    // We assume that when PLL is close
    //     - POWER is on
    //     - EN is 1
    INS_BITFIELD(s_reg_anadig->PLL6_CTRL, Anadig_PLL6_CTRL_ENABLE, 0);
    INS_BITFIELD(s_reg_anadig->PLL6_CTRL, Anadig_PLL6_CTRL_POWERDOWN, 1);
    return 1;
}

uint8_t calc_freq_pll6(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_anadig->PLL6_CTRL, Anadig_PLL6_CTRL_DIV_SELECT);
    p_self->freq = parent_freq * div;
    return 1;
}

uint8_t set_freq_pll6(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_mult, new_mult;
    // Args Check
    numArgs = va_arg(args, int);
    old_mult = EXTRACT_BITFIELD(s_reg_anadig->PLL6_CTRL, Anadig_PLL6_CTRL_DIV_SELECT);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - ANADIG_PLL6_CTRL.DIV_SELECT (1~127) (Current %d)\n", old_mult);
        return 0;
    }
    new_mult = va_arg(args, int);

    if (new_mult == old_mult) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }

    if ((new_mult == 0) || (new_mult > 127)) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PLL6_CTRL, Anadig_PLL6_CTRL_DIV_SELECT, new_mult);

    // update the freq
    *old_val = old_mult;
    *new_val = new_mult;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL6_DIV
 */
static CLK_NODE_T clk_pll6_div = {
    .name = CLK_PLL6_DIV,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll6_div,
    .get_parent = get_parent_pll6_div,
    .physically_open_clk = open_clk_pll6_div,
    .physically_close_clk = close_clk_pll6_div,
    .calc_freq = calc_freq_pll6_div,
    .set_freq = set_freq_pll6_div,
};

uint8_t is_physically_open_pll6_div()
{
    return get_parent_pll6_div()->is_physically_open();
}

P_CLK_NODE_T get_parent_pll6_div()
{
    return &clk_pll6;
}

uint8_t open_clk_pll6_div()
{
    return 1;
}

uint8_t close_clk_pll6_div()
{
    return 1;
}

uint8_t calc_freq_pll6_div(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_PLL6_CLK_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_pll6_div(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_PLL6_CLK_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - CCM_CACRR.PLL6_CLK_DIV (0~1) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);

    if (new_div == old_div) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }

    if (new_div > 1) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_PLL6_CLK_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL7
 */
static CLK_NODE_T clk_pll7 = {
    .name = CLK_PLL7,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll7,
    .get_parent = get_parent_pll7,
    .physically_open_clk = open_clk_pll7,
    .physically_close_clk = close_clk_pll7,
    .calc_freq = calc_freq_pll7,
    .set_freq = set_freq_pll7,
};

uint8_t is_physically_open_pll7()
{
    uint8_t pll_enable, pll_power, pll_lock;
    pll_enable = EXTRACT_BITFIELD(s_reg_anadig->PLL7_CTRL,
            Anadig_PLL7_CTRL_ENABLE);
    pll_power = EXTRACT_BITFIELD(s_reg_anadig->PLL7_CTRL,
            Anadig_PLL7_CTRL_POWER);
    pll_lock = EXTRACT_BITFIELD(s_reg_anadig->PLL7_CTRL,
            Anadig_PLL7_CTRL_LOCK);

    if (pll_power && pll_lock && pll_enable)
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_pll7()
{
    return &clk_clk_24m_irc;
}

uint8_t open_clk_pll7()
{
    // We assume that when PLL is close
    //     - EN is 0
    //     - POWER is down
    // 1. power up and wait lock
    INS_BITFIELD(s_reg_anadig->PLL7_CTRL, Anadig_PLL7_CTRL_POWER, 1);
    WAIT(EXTRACT_BITFIELD(s_reg_anadig->PLL7_CTRL, Anadig_PLL7_CTRL_LOCK), TIMEOUT);
    if (!EXTRACT_BITFIELD(s_reg_anadig->PLL7_CTRL, Anadig_PLL7_CTRL_LOCK)) {
        // lock failure
        return 0;
    } else {
        // 2. enable
        INS_BITFIELD(s_reg_anadig->PLL7_CTRL, Anadig_PLL7_CTRL_ENABLE, 1);
    }
    return 1;
}

uint8_t close_clk_pll7()
{
    // We assume that when PLL is close
    //     - POWER is on
    //     - EN is 1
    INS_BITFIELD(s_reg_anadig->PLL7_CTRL, Anadig_PLL7_CTRL_ENABLE, 0);
    INS_BITFIELD(s_reg_anadig->PLL7_CTRL, Anadig_PLL7_CTRL_POWER, 0);
    return 1;
}

uint8_t calc_freq_pll7(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_anadig->PLL7_CTRL, Anadig_PLL7_CTRL_DIV_SELECT);
    if (div == 0)
        p_self->freq = parent_freq * 20;
    else if (div == 1)
        p_self->freq = parent_freq * 22;
    return 1;
}

uint8_t set_freq_pll7(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_anadig->PLL7_CTRL, Anadig_PLL7_CTRL_DIV_SELECT);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - ANADIG_PLL7_CTRL.DIV_SELECT (0~1) (Current %d)\n", old_div);
        printf("        - 0 : PLL = F_ref * 20\n");
        printf("        - 1 : PLL = F_ref * 22\n");
        return 0;
    }
    new_div = va_arg(args, int);

    if (old_div == new_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }

    if (new_div > 1) {
        printf("Invalid div value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_anadig->PLL7_CTRL, Anadig_PLL7_CTRL_DIV_SELECT, new_div);

    // update the freq
    *old_val = old_div ? 22 : 20;
    *new_val = new_div ? 22 : 20;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLL1_PFD
 */
static CLK_NODE_T clk_pll1_pfd = {
    .name = CLK_PLL1_PFD,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll1_pfd,
    .get_parent = get_parent_pll1_pfd,
    .check_parent = check_parent_pll1_pfd,
    .dump_possible_parent = dump_possible_parent_pll1_pfd,
    .physically_set_parent = set_parent_pll1_pfd,
    .physically_open_clk = open_clk_pll1_pfd,
    .physically_close_clk = close_clk_pll1_pfd,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_pll1_pfd()
{
    return get_parent_pll1_pfd()->is_physically_open();
}

P_CLK_NODE_T get_parent_pll1_pfd()
{
    uint8_t pll_pfd_sel = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL1_PFD_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (pll_pfd_sel) {
        case 0:
            pclk = &clk_pll1;
            break;
        case 1:
            pclk = &clk_pll1_pfd1;
            break;
        case 2:
            pclk = &clk_pll1_pfd2;
            break;
        case 3:
            pclk = &clk_pll1_pfd3;
            break;
        case 4:
            pclk = &clk_pll1_pfd4;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for PLL1_PFD\n");
            break;
    }
    return pclk;
}

uint8_t check_parent_pll1_pfd(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_pll1_pfd();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_pll1) || (parent == &clk_pll1_pfd1) ||
            (parent == &clk_pll1_pfd2) || (parent == &clk_pll1_pfd3) ||
            (parent == &clk_pll1_pfd4)) {
        *old_open = clk_pll1_pfd.active;
        *new_open = clk_pll1_pfd.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_pll1_pfd()
{
    printf("%s / %s / %s / %s / %s\n",
            clk_name_str(clk_pll1.name), clk_name_str(clk_pll1_pfd1.name), clk_name_str(clk_pll1_pfd2.name),
            clk_name_str(clk_pll1_pfd3.name), clk_name_str(clk_pll1_pfd4.name));
    return 1;
}

uint8_t set_parent_pll1_pfd(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_pll1)
        sel = 0;
    else if (parent == &clk_pll1_pfd1)
        sel = 1;
    else if (parent == &clk_pll1_pfd2)
        sel = 2;
    else if (parent == &clk_pll1_pfd3)
        sel = 3;
    else if (parent == &clk_pll1_pfd4)
        sel = 4;
    else {
            printf("set wrong parent for PLL1_PFD\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL1_PFD_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_pll1_pfd()
{
    return 1;
}

uint8_t close_clk_pll1_pfd()
{
    return 1;
}

/*
 * Clock Node PLL2_PFD
 */
static CLK_NODE_T clk_pll2_pfd = {
    .name = CLK_PLL2_PFD,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pll2_pfd,
    .get_parent = get_parent_pll2_pfd,
    .check_parent = check_parent_pll2_pfd,
    .dump_possible_parent = dump_possible_parent_pll2_pfd,
    .physically_set_parent = set_parent_pll2_pfd,
    .physically_open_clk = open_clk_pll2_pfd,
    .physically_close_clk = close_clk_pll2_pfd,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_pll2_pfd()
{
    return get_parent_pll2_pfd()->is_physically_open();
}

P_CLK_NODE_T get_parent_pll2_pfd()
{
    uint8_t pll_pfd_sel = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL2_PFD_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (pll_pfd_sel) {
        case 0:
            pclk = &clk_pll2;
            break;
        case 1:
            pclk = &clk_pll2_pfd1;
            break;
        case 2:
            pclk = &clk_pll2_pfd2;
            break;
        case 3:
            pclk = &clk_pll2_pfd3;
            break;
        case 4:
            pclk = &clk_pll2_pfd4;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for PLL2_PFD\n");
            break;
    }
    return pclk;
}

uint8_t check_parent_pll2_pfd(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_pll2_pfd();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_pll2) || (parent == &clk_pll2_pfd1) ||
            (parent == &clk_pll2_pfd2) || (parent == &clk_pll2_pfd3) ||
            (parent == &clk_pll2_pfd4)) {
        *old_open = clk_pll2_pfd.active;
        *new_open = clk_pll2_pfd.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_pll2_pfd()
{
    printf("%s / %s / %s / %s / %s\n",
            clk_name_str(clk_pll2.name), clk_name_str(clk_pll2_pfd1.name), clk_name_str(clk_pll2_pfd2.name),
            clk_name_str(clk_pll2_pfd3.name), clk_name_str(clk_pll2_pfd4.name));
    return 1;
}

uint8_t set_parent_pll2_pfd(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;

    if (parent == &clk_pll2)
        sel = 0;
    else if (parent == &clk_pll2_pfd1)
        sel = 1;
    else if (parent == &clk_pll2_pfd2)
        sel = 2;
    else if (parent == &clk_pll2_pfd3)
        sel = 3;
    else if (parent == &clk_pll2_pfd4)
        sel = 4;
    else {
            printf("set wrong parent for PLL2_PFD\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_PLL2_PFD_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_pll2_pfd()
{
    return 1;
}

uint8_t close_clk_pll2_pfd()
{
    return 1;
}

/*
 * Clock Node SYS_CLK
 */
static CLK_NODE_T clk_sys = {
    .name = CLK_SYS,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_sys,
    .get_parent = get_parent_sys,
    .check_parent = check_parent_sys,
    .dump_possible_parent = dump_possible_parent_sys,
    .physically_set_parent = set_parent_sys,
    .physically_open_clk = open_clk_sys,
    .physically_close_clk = close_clk_sys,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_sys()
{
    return get_parent_sys()->is_physically_open();
}

P_CLK_NODE_T get_parent_sys()
{
    uint8_t sys_clk_sel = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_SYS_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sys_clk_sel) {
        case 0:
            pclk = &clk_fast;
            break;
        case 1:
            pclk = &clk_slow;
            break;
        case 2:
            pclk = &clk_pll2_pfd;
            break;
        case 3:
            pclk = &clk_pll2;
            break;
        case 4:
            pclk = &clk_pll1_pfd;
            break;
        case 5:
            pclk = &clk_pll3;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for SYS CLK\n");
            break;
    }
    return pclk;
}

uint8_t check_parent_sys(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_sys();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_fast) || (parent == &clk_slow) ||
            (parent == &clk_pll2_pfd) || (parent == &clk_pll2) ||
            (parent == &clk_pll1_pfd) || (parent == &clk_pll3)) {
        *old_open = old_parent->is_physically_open();
        *new_open = parent->is_physically_open();
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_sys()
{
    printf("%s / %s / %s / %s / %s / %s\n",
            clk_name_str(clk_fast.name), clk_name_str(clk_slow.name), clk_name_str(clk_pll2_pfd.name), clk_name_str(clk_pll2.name),
            clk_name_str(clk_pll1_pfd.name), clk_name_str(clk_pll3.name));
    return 1;
}

uint8_t set_parent_sys(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_fast)
        sel = 0;
    else if (parent == &clk_slow)
        sel = 1;
    else if (parent == &clk_pll2_pfd)
        sel = 2;
    else if (parent == &clk_pll2)
        sel = 3;
    else if (parent == &clk_pll1_pfd)
        sel = 4;
    else if (parent == &clk_pll3)
        sel = 5;
    else {
        printf("set wrong parent for SYS CLK\n");
        return 0;
    }

    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_SYS_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_sys()
{
    return 1;
}

uint8_t close_clk_sys()
{
    return 1;
}

/*
 * Clock Node CA5_CORE_CLK
 */
static CLK_NODE_T clk_ca5_core = {
    .name = CLK_CA5_CORE,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ca5_core,
    .get_parent = get_parent_ca5_core,
    .physically_open_clk = open_clk_ca5_core,
    .physically_close_clk = close_clk_ca5_core,
    .calc_freq = calc_freq_ca5_core,
    .set_freq = set_freq_ca5_core,
};

uint8_t is_physically_open_ca5_core()
{
    return get_parent_ca5_core()->is_physically_open();
}

P_CLK_NODE_T get_parent_ca5_core()
{
    return &clk_sys;
}

uint8_t open_clk_ca5_core()
{
    return 1;
}

uint8_t close_clk_ca5_core()
{
    return 1;
}

uint8_t calc_freq_ca5_core(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_ARM_CLK_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_ca5_core(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_ARM_CLK_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CACRR.ARM_CLK_DIV (0~7) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (new_div == old_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 7) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_ARM_CLK_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node PLATFORM_BUS_CM4_CORE_CLK
 */
static CLK_NODE_T clk_platform_bus_cm4_core = {
    .name = CLK_PLATFORM_BUS,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_platform_bus_cm4_core,
    .get_parent = get_parent_platform_bus_cm4_core,
    .physically_open_clk = open_clk_platform_bus_cm4_core,
    .physically_close_clk = close_clk_platform_bus_cm4_core,
    .calc_freq = calc_freq_platform_bus_cm4_core,
    .set_freq = set_freq_platform_bus_cm4_core,
};

uint8_t is_physically_open_platform_bus_cm4_core()
{
    return get_parent_platform_bus_cm4_core()->is_physically_open();
}

P_CLK_NODE_T get_parent_platform_bus_cm4_core()
{
    return &clk_ca5_core;
}

uint8_t open_clk_platform_bus_cm4_core()
{
    return 1;
}

uint8_t close_clk_platform_bus_cm4_core()
{
    return 1;
}

uint8_t calc_freq_platform_bus_cm4_core(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_BUS_CLK_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_platform_bus_cm4_core(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_BUS_CLK_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CACRR.BUS_CLK_DIV (0~7) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (new_div == old_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 7) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_BUS_CLK_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node FLEXBUS_CLK
 */
static CLK_NODE_T clk_flexbus = {
    .name = CLK_FLEXBUS,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_flexbus,
    .get_parent = get_parent_flexbus,
    .physically_open_clk = open_clk_flexbus,
    .physically_close_clk = close_clk_flexbus,
    .calc_freq = calc_freq_flexbus,
    .set_freq = set_freq_flexbus,
};

uint8_t is_physically_open_flexbus()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR1, CCM_CCPGR1_PPCG14);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_flexbus()
{
    return &clk_platform_bus_cm4_core;
}

uint8_t open_clk_flexbus()
{
    INS_BITFIELD(s_reg_ccm->CCPGR1, CCM_CCPGR1_PPCG14, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_flexbus()
{
    INS_BITFIELD(s_reg_ccm->CCPGR1, CCM_CCPGR1_PPCG14, CG_OFF_ALL);
    return 1;
}

uint8_t calc_freq_flexbus(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_FLEX_CLK_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_flexbus(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_FLEX_CLK_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CACRR.FLEX_CLK_DIV (0~7) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (new_div == old_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 7) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_FLEX_CLK_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node IPS_IPG
 */
static CLK_NODE_T clk_ips_ipg = {
    .name = CLK_IPS_IPG,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ips_ipg,
    .get_parent = get_parent_ips_ipg,
    .physically_open_clk = open_clk_ips_ipg,
    .physically_close_clk = close_clk_ips_ipg,
    .calc_freq = calc_freq_ips_ipg,
    .set_freq = set_freq_ips_ipg,
};

uint8_t is_physically_open_ips_ipg()
{
    return get_parent_ips_ipg()->is_physically_open();
}

P_CLK_NODE_T get_parent_ips_ipg()
{
    return &clk_platform_bus_cm4_core;
}

uint8_t open_clk_ips_ipg()
{
    return 1;
}

uint8_t close_clk_ips_ipg()
{
    return 1;
}

uint8_t calc_freq_ips_ipg(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_IPG_CLK_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_ips_ipg(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_IPG_CLK_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CACRR.IPG_CLK_DIV (0~3) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (new_div == old_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 3) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CACRR, CCM_CACRR_IPG_CLK_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node DRAMC_CLK
 */
static CLK_NODE_T clk_dramc = {
    .name = CLK_DRAMC,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_dramc,
    .get_parent = get_parent_dramc,
    .check_parent = check_parent_dramc,
    .dump_possible_parent = dump_possible_parent_dramc,
    .physically_set_parent = set_parent_dramc,
    .physically_open_clk = open_clk_dramc,
    .physically_close_clk = close_clk_dramc,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_dramc()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG14);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_dramc()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_DDRC_CLK_SEL);
    return sel ? &clk_sys : &clk_pll2_pfd2;
}

uint8_t check_parent_dramc(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_dramc();

    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_sys) || (parent == &clk_pll2_pfd2)) {
        *old_open = clk_dramc.active;
        *new_open = clk_dramc.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_dramc()
{
    printf("%s / %s\n", clk_name_str(clk_sys.name), clk_name_str(clk_pll2_pfd2.name));
    return 1;
}

uint8_t set_parent_dramc(P_CLK_NODE_T parent)
{
    if (parent == &clk_sys) {
        INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_DDRC_CLK_SEL, 1);
    } else if (parent == &clk_pll2_pfd2) {
        INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_DDRC_CLK_SEL, 0);
    }

    return 1;
}

uint8_t open_clk_dramc()
{
    INS_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG14, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_dramc()
{
    INS_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG14, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node AUDIO_EXT
 */
static CLK_NODE_T clk_audio_ext = {
    .name = CLK_AUDIO_EXT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_audio_ext,
    .get_parent = get_parent_audio_ext,
    .physically_open_clk = open_clk_audio_ext,
    .physically_close_clk = close_clk_audio_ext,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_audio_ext()
{
    return 0;
}

P_CLK_NODE_T get_parent_audio_ext()
{
    return NULL;
}

uint8_t open_clk_audio_ext()
{
    return 1;
}

uint8_t close_clk_audio_ext()
{
    return 1;
}

/*
 * Clock Node FlexCAN0
 */
static CLK_NODE_T clk_flexcan0 = {
    .name = CLK_FLEXCAN0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_flexcan0,
    .get_parent = get_parent_flexcan0,
    .check_parent = check_parent_flexcan0,
    .dump_possible_parent = dump_possible_parent_flexcan,
    .physically_set_parent = set_parent_flexcan0,
    .physically_open_clk = open_clk_flexcan0,
    .physically_close_clk = close_clk_flexcan0,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_flexcan0()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_CAN0_EN);
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG0);

    if ((en == 1) && (cg != CG_OFF_ALL))
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_flexcan0()
{
    uint8_t sel = 0;
    /* When CG is not open, access to CAN registers will cause system hang*/
    if (is_physically_open_flexcan0())
        sel = EXTRACT_BITFIELD(s_reg_can0->CTRL1, CAN_CTRL1_CLKSRC);
    return sel ? &clk_ips_ipg : & clk_fxosc;
}

uint8_t check_parent_flexcan0(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_flexcan0();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_ips_ipg) || (parent == &clk_fxosc)) {
        *old_open = clk_flexcan0.active;
        *new_open = clk_flexcan0.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_flexcan()
{
    printf("%s / %s\n", clk_name_str(clk_ips_ipg.name), clk_name_str(clk_fxosc.name));
    return 1;
}

uint8_t set_parent_flexcan0(P_CLK_NODE_T parent)
{
    if (parent == &clk_ips_ipg)
        INS_BITFIELD(s_reg_can0->CTRL1, CAN_CTRL1_CLKSRC, 1);
    else if (parent == &clk_fxosc)
        INS_BITFIELD(s_reg_can0->CTRL1, CAN_CTRL1_CLKSRC, 0);

    return 1;
}

uint8_t open_clk_flexcan0()
{
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_CAN0_EN, 1);
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG0, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_flexcan0()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG0, CG_OFF_ALL);
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_CAN0_EN, 0);
    return 1;
}

/*
 * Clock Node FlexCAN1
 */
static CLK_NODE_T clk_flexcan1 = {
    .name = CLK_FLEXCAN1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_flexcan1,
    .get_parent = get_parent_flexcan1,
    .check_parent = check_parent_flexcan1,
    .dump_possible_parent = dump_possible_parent_flexcan,
    .physically_set_parent = set_parent_flexcan1,
    .physically_open_clk = open_clk_flexcan1,
    .physically_close_clk = close_clk_flexcan1,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_flexcan1()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_CAN1_EN);
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR9, CCM_CCGR9_CG4);

    if ((en == 1) && (cg != CG_OFF_ALL))
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_flexcan1()
{
    uint8_t sel = 0;
    /* When CG is not open, access to CAN registers will cause system hang*/
    if (is_physically_open_flexcan1())
        sel = EXTRACT_BITFIELD(s_reg_can1->CTRL1, CAN_CTRL1_CLKSRC);
    return sel ? &clk_ips_ipg : & clk_fxosc;
}

uint8_t check_parent_flexcan1(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_flexcan1();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_ips_ipg) || (parent == &clk_fxosc)) {
        *old_open = clk_flexcan1.active;
        *new_open = clk_flexcan1.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t set_parent_flexcan1(P_CLK_NODE_T parent)
{
    if (parent == &clk_ips_ipg)
        INS_BITFIELD(s_reg_can1->CTRL1, CAN_CTRL1_CLKSRC, 1);
    else if (parent == &clk_fxosc)
        INS_BITFIELD(s_reg_can1->CTRL1, CAN_CTRL1_CLKSRC, 0);

    return 1;
}

uint8_t open_clk_flexcan1()
{
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_CAN1_EN, 1);
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR9_CG4, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_flexcan1()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR9_CG4, CG_OFF_ALL);
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_CAN1_EN, 0);
    return 1;
}

/*
 * Clock Node FTM0_EXT
 */
static CLK_NODE_T clk_ftm0_ext = {
    .name = CLK_FTM0_EXT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ftm0_ext,
    .get_parent = get_parent_ftm0_ext,
    .check_parent = check_parent_ftm0_ext,
    .dump_possible_parent = dump_possible_parent_ftm_ext,
    .physically_set_parent = set_parent_ftm0_ext,
    .physically_open_clk = open_clk_ftm0_ext,
    .physically_close_clk = close_clk_ftm0_ext,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ftm0_ext()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM0_CLK_EN);
    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_ftm0_ext()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_FTM0_EXT_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_sirc;
            break;
        case 1:
            pclk = &clk_sxosc;
            break;
        case 2:
            pclk = &clk_fxosc_div;
            break;
        case 3:
            pclk = &clk_audio_ext;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for FTM0_EXT\n");
            break;
    }

    return pclk;
}

uint8_t check_parent_ftm0_ext(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_ftm0_ext();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_sirc) || (parent == &clk_sxosc) ||
            (parent == &clk_fxosc_div) || (parent == &clk_audio_ext)) {
        *old_open = clk_ftm0_ext.active;
        *new_open = clk_ftm0_ext.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_ftm_ext()
{
    printf("%s / %s / %s / %s\n", clk_name_str(clk_sirc.name), clk_name_str(clk_sxosc.name),
            clk_name_str(clk_fxosc_div.name), clk_name_str(clk_audio_ext.name));
    return 1;
}

uint8_t set_parent_ftm0_ext(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_sirc)
        sel = 0;
    else if (parent == &clk_sxosc)
        sel = 1;
    else if (parent == &clk_fxosc_div)
        sel = 2;
    else if (parent == &clk_audio_ext)
        sel = 3;
    else {
            printf("set wrong parent for FTM0_EXT\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_FTM0_EXT_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_ftm0_ext()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM0_CLK_EN, 1);
    return 1;
}

uint8_t close_clk_ftm0_ext()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM0_CLK_EN, 0);
    return 1;
}

/*
 * Clock Node FTM1_EXT
 */
static CLK_NODE_T clk_ftm1_ext = {
    .name = CLK_FTM1_EXT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ftm1_ext,
    .get_parent = get_parent_ftm1_ext,
    .check_parent = check_parent_ftm1_ext,
    .dump_possible_parent = dump_possible_parent_ftm_ext,
    .physically_set_parent = set_parent_ftm1_ext,
    .physically_open_clk = open_clk_ftm1_ext,
    .physically_close_clk = close_clk_ftm1_ext,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ftm1_ext()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM1_CLK_EN);
    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_ftm1_ext()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_FTM1_EXT_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_sirc;
            break;
        case 1:
            pclk = &clk_sxosc;
            break;
        case 2:
            pclk = &clk_fxosc_div;
            break;
        case 3:
            pclk = &clk_audio_ext;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for FTM1_EXT\n");
            break;
    }

    return pclk;
}

uint8_t check_parent_ftm1_ext(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_ftm1_ext();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_sirc) || (parent == &clk_sxosc) ||
            (parent == &clk_fxosc_div) || (parent == &clk_audio_ext)) {
        *old_open = clk_ftm1_ext.active;
        *new_open = clk_ftm1_ext.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t set_parent_ftm1_ext(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_sirc)
        sel = 0;
    else if (parent == &clk_sxosc)
        sel = 1;
    else if (parent == &clk_fxosc_div)
        sel = 2;
    else if (parent == &clk_audio_ext)
        sel = 3;
    else {
            printf("set wrong parent for FTM1_EXT\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_FTM1_EXT_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_ftm1_ext()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM1_CLK_EN, 1);
    return 1;
}

uint8_t close_clk_ftm1_ext()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM1_CLK_EN, 0);
    return 1;
}

/*
 * Clock Node FTM2_EXT
 */
static CLK_NODE_T clk_ftm2_ext = {
    .name = CLK_FTM2_EXT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ftm2_ext,
    .get_parent = get_parent_ftm2_ext,
    .check_parent = check_parent_ftm2_ext,
    .dump_possible_parent = dump_possible_parent_ftm_ext,
    .physically_set_parent = set_parent_ftm2_ext,
    .physically_open_clk = open_clk_ftm2_ext,
    .physically_close_clk = close_clk_ftm2_ext,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ftm2_ext()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM2_CLK_EN);
    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_ftm2_ext()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_FTM2_EXT_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_sirc;
            break;
        case 1:
            pclk = &clk_sxosc;
            break;
        case 2:
            pclk = &clk_fxosc_div;
            break;
        case 3:
            pclk = &clk_audio_ext;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for FTM2_EXT\n");
            break;
    }

    return pclk;
}

uint8_t check_parent_ftm2_ext(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_ftm2_ext();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_sirc) || (parent == &clk_sxosc) ||
            (parent == &clk_fxosc_div) || (parent == &clk_audio_ext)) {
        *old_open = clk_ftm2_ext.active;
        *new_open = clk_ftm2_ext.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t set_parent_ftm2_ext(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_sirc)
        sel = 0;
    else if (parent == &clk_sxosc)
        sel = 1;
    else if (parent == &clk_fxosc_div)
        sel = 2;
    else if (parent == &clk_audio_ext)
        sel = 3;
    else {
            printf("set wrong parent for FTM2_EXT\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_FTM2_EXT_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_ftm2_ext()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM2_CLK_EN, 1);
    return 1;
}

uint8_t close_clk_ftm2_ext()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM2_CLK_EN, 0);
    return 1;
}

/*
 * Clock Node FTM3_EXT
 */
static CLK_NODE_T clk_ftm3_ext = {
    .name = CLK_FTM3_EXT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ftm3_ext,
    .get_parent = get_parent_ftm3_ext,
    .check_parent = check_parent_ftm3_ext,
    .dump_possible_parent = dump_possible_parent_ftm_ext,
    .physically_set_parent = set_parent_ftm3_ext,
    .physically_open_clk = open_clk_ftm3_ext,
    .physically_close_clk = close_clk_ftm3_ext,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ftm3_ext()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM3_CLK_EN);
    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_ftm3_ext()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_FTM3_EXT_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_sirc;
            break;
        case 1:
            pclk = &clk_sxosc;
            break;
        case 2:
            pclk = &clk_fxosc_div;
            break;
        case 3:
            pclk = &clk_audio_ext;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for FTM3_EXT\n");
            break;
    }

    return pclk;
}

uint8_t check_parent_ftm3_ext(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_ftm3_ext();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_sirc) || (parent == &clk_sxosc) ||
            (parent == &clk_fxosc_div) || (parent == &clk_audio_ext)) {
        *old_open = clk_ftm3_ext.active;
        *new_open = clk_ftm3_ext.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t set_parent_ftm3_ext(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_sirc)
        sel = 0;
    else if (parent == &clk_sxosc)
        sel = 1;
    else if (parent == &clk_fxosc_div)
        sel = 2;
    else if (parent == &clk_audio_ext)
        sel = 3;
    else {
            printf("set wrong parent for FTM3_EXT\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_FTM3_EXT_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_ftm3_ext()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM3_CLK_EN, 1);
    return 1;
}

uint8_t close_clk_ftm3_ext()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM3_CLK_EN, 0);
    return 1;
}

/*
 * Clock Node FTM0_FIX
 */
static CLK_NODE_T clk_ftm0_fix = {
    .name = CLK_FTM0_FIX,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ftm0_fix,
    .get_parent = get_parent_ftm0_fix,
    .check_parent = check_parent_ftm0_fix,
    .dump_possible_parent = dump_possible_parent_ftm_fix,
    .physically_set_parent = set_parent_ftm0_fix,
    .physically_open_clk = open_clk_ftm0_fix,
    .physically_close_clk = close_clk_ftm0_fix,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ftm0_fix()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM0_CLK_EN);
    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_ftm0_fix()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_FTM0_FIX_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_sxosc;
            break;
        case 1:
            pclk = &clk_sirc;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for FTM0_FIX\n");
            break;
    }

    return pclk;
}

uint8_t check_parent_ftm0_fix(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_ftm0_fix();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_sxosc) || (parent == &clk_sirc)) {
        *old_open = clk_ftm0_fix.active;
        *new_open = clk_ftm0_fix.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_ftm_fix()
{
    printf("%s / %s\n", clk_name_str(clk_sxosc.name), clk_name_str(clk_sirc.name));
    return 1;
}

uint8_t set_parent_ftm0_fix(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_sxosc)
        sel = 0;
    else if (parent == &clk_sirc)
        sel = 1;
    else {
            printf("set wrong parent for FTM0_FIX\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_FTM0_FIX_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_ftm0_fix()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM0_CLK_EN, 1);
    return 1;
}

uint8_t close_clk_ftm0_fix()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM0_CLK_EN, 0);
    return 1;
}

/*
 * Clock Node FTM1_FIX
 */
static CLK_NODE_T clk_ftm1_fix = {
    .name = CLK_FTM1_FIX,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ftm1_fix,
    .get_parent = get_parent_ftm1_fix,
    .check_parent = check_parent_ftm1_fix,
    .dump_possible_parent = dump_possible_parent_ftm_fix,
    .physically_set_parent = set_parent_ftm1_fix,
    .physically_open_clk = open_clk_ftm1_fix,
    .physically_close_clk = close_clk_ftm1_fix,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ftm1_fix()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM1_CLK_EN);
    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_ftm1_fix()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_FTM1_FIX_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_sxosc;
            break;
        case 1:
            pclk = &clk_sirc;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for FTM1_FIX\n");
            break;
    }

    return pclk;
}

uint8_t check_parent_ftm1_fix(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_ftm1_fix();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_sxosc) || (parent == &clk_sirc)) {
        *old_open = clk_ftm1_fix.active;
        *new_open = clk_ftm1_fix.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t set_parent_ftm1_fix(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_sxosc)
        sel = 0;
    else if (parent == &clk_sirc)
        sel = 1;
    else {
            printf("set wrong parent for FTM1_FIX\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_FTM1_FIX_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_ftm1_fix()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM1_CLK_EN, 1);
    return 1;
}

uint8_t close_clk_ftm1_fix()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM1_CLK_EN, 0);
    return 1;
}

/*
 * Clock Node FTM2_FIX
 */
static CLK_NODE_T clk_ftm2_fix = {
    .name = CLK_FTM2_FIX,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ftm2_fix,
    .get_parent = get_parent_ftm2_fix,
    .check_parent = check_parent_ftm2_fix,
    .dump_possible_parent = dump_possible_parent_ftm_fix,
    .physically_set_parent = set_parent_ftm2_fix,
    .physically_open_clk = open_clk_ftm2_fix,
    .physically_close_clk = close_clk_ftm2_fix,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ftm2_fix()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM2_CLK_EN);
    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_ftm2_fix()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_FTM2_FIX_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_sxosc;
            break;
        case 1:
            pclk = &clk_sirc;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for FTM2_FIX\n");
            break;
    }

    return pclk;
}

uint8_t check_parent_ftm2_fix(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_ftm2_fix();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_sxosc) || (parent == &clk_sirc)) {
        *old_open = clk_ftm2_fix.active;
        *new_open = clk_ftm2_fix.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t set_parent_ftm2_fix(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_sxosc)
        sel = 0;
    else if (parent == &clk_sirc)
        sel = 1;
    else {
            printf("set wrong parent for FTM2_FIX\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_FTM2_FIX_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_ftm2_fix()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM2_CLK_EN, 1);
    return 1;
}

uint8_t close_clk_ftm2_fix()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM2_CLK_EN, 0);
    return 1;
}

/*
 * Clock Node FTM3_FIX
 */
static CLK_NODE_T clk_ftm3_fix = {
    .name = CLK_FTM3_FIX,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ftm3_fix,
    .get_parent = get_parent_ftm3_fix,
    .check_parent = check_parent_ftm3_fix,
    .dump_possible_parent = dump_possible_parent_ftm_fix,
    .physically_set_parent = set_parent_ftm3_fix,
    .physically_open_clk = open_clk_ftm3_fix,
    .physically_close_clk = close_clk_ftm3_fix,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ftm3_fix()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM3_CLK_EN);
    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_ftm3_fix()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_FTM3_FIX_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_sxosc;
            break;
        case 1:
            pclk = &clk_sirc;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for FTM3_FIX\n");
            break;
    }

    return pclk;
}

uint8_t check_parent_ftm3_fix(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_ftm3_fix();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_sxosc) || (parent == &clk_sirc)) {
        *old_open = clk_ftm3_fix.active;
        *new_open = clk_ftm3_fix.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t set_parent_ftm3_fix(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_sxosc)
        sel = 0;
    else if (parent == &clk_sirc)
        sel = 1;
    else {
            printf("set wrong parent for FTM3_FIX\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_FTM3_FIX_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_ftm3_fix()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM3_CLK_EN, 1);
    return 1;
}

uint8_t close_clk_ftm3_fix()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_FTM3_CLK_EN, 0);
    return 1;
}

/*
 * Clock Node NFC_CLK
 */
static CLK_NODE_T clk_nfc = {
    .name = CLK_NFC,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_nfc,
    .get_parent = get_parent_nfc,
    .check_parent = check_parent_nfc,
    .dump_possible_parent = dump_possible_parent_nfc,
    .physically_set_parent = set_parent_nfc,
    .physically_open_clk = open_clk_nfc,
    .physically_close_clk = close_clk_nfc,
    .calc_freq = calc_freq_nfc,
    .set_freq = set_freq_nfc,
};

uint8_t is_physically_open_nfc()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_NFC_EN);
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR10, CCM_CCGR10_CG0);

    if ((en == 1) && (cg != CG_OFF_ALL))
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_nfc()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_NFC_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_platform_bus_cm4_core;
            break;
        case 1:
            pclk = &clk_pll1_pfd1;
            break;
        case 2:
            pclk = &clk_pll3_pfd1;
            break;
        case 3:
            pclk = &clk_pll3_pfd3;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for NFC\n");
            break;
    }

    return pclk;
}

uint8_t check_parent_nfc(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_nfc();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_platform_bus_cm4_core) || (parent == &clk_pll1_pfd1) ||
            (parent == &clk_pll3_pfd1) || (parent == &clk_pll3_pfd3)) {
        *old_open = clk_nfc.active;
        *new_open = clk_nfc.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_nfc()
{
    printf("%s / %s / %s / %s\n", clk_name_str(clk_platform_bus_cm4_core.name), clk_name_str(clk_pll1_pfd1.name),
            clk_name_str(clk_pll3_pfd1.name), clk_name_str(clk_pll3_pfd3.name));
    return 1;
}

uint8_t set_parent_nfc(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_platform_bus_cm4_core)
        sel = 0;
    else if (parent == &clk_pll1_pfd1)
        sel = 1;
    else if (parent == &clk_pll3_pfd1)
        sel = 2;
    else if (parent == &clk_pll3_pfd3)
        sel = 3;
    else {
            printf("set wrong parent for NFC\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_NFC_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_nfc()
{
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_NFC_EN, 1);
    INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_NFC_CLK_SEL, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_nfc()
{
    INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_NFC_CLK_SEL, CG_OFF_ALL);
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_NFC_EN, 0);
    return 1;
}

uint8_t calc_freq_nfc(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t pre_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_NFC_PRE_DIV) + 1;
    uint8_t frac_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_NFC_FRAC_DIV) + 1;
    uint8_t frac_div_en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_NFC_FRAC_DIV_EN);

    if (frac_div_en)
        p_self->freq = 2 * parent_freq / (2 * pre_div * (frac_div + 1));
    else
        p_self->freq = parent_freq / (pre_div * frac_div);

    return 1;
}

uint8_t set_freq_nfc(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_preDiv, old_fracDiv, old_fracDivEn, new_preDiv, new_fracDiv, new_fracDivEn;
    numArgs = va_arg(args, int);
    old_preDiv = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_NFC_PRE_DIV);
    old_fracDiv = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_NFC_FRAC_DIV);
    old_fracDivEn = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_NFC_FRAC_DIV_EN);
    // Args Check
    if (numArgs != 3) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CSCDR3.NFC_PRE_DIV (0~7) (Current %d)\n", old_preDiv);
        printf("    - 2. CCM_CSCDR2.NFC_FRAC_DIV (0~15) (Current %d)\n", old_fracDiv);
        printf("    - 3. CCM_CSCDR2.NFC_FRAC_DIV_EN (0~1) (Current %d)\n", old_fracDivEn);
        printf("        - (frac_div_en = 1) add 0.5 to frac_div\n");
        return 0;
    }
    new_preDiv = va_arg(args, int);
    new_fracDiv = va_arg(args, int);
    new_fracDivEn = va_arg(args, int);

    if ((new_preDiv == old_preDiv) && (new_fracDiv == old_fracDiv) && (new_fracDivEn == old_fracDivEn)) {
        printf("Same frac value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }

    if ((new_preDiv > 7) || (new_fracDiv > 15) || (new_fracDiv > 1)) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_NFC_PRE_DIV, new_preDiv);
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_NFC_FRAC_DIV, new_fracDiv);
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_NFC_FRAC_DIV_EN, new_fracDivEn);

    // update the freq
    *old_val = new_preDiv * (new_fracDivEn ? (2 * new_fracDiv + 1) : (2 * new_fracDiv));
    *new_val = old_preDiv * (old_fracDivEn ? (2 * old_fracDiv + 1) : (2 * old_fracDiv));
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node QSPI0_CLK
 */
static CLK_NODE_T clk_qspi0 = {
    .name = CLK_QSPI0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_qspi0,
    .get_parent = get_parent_qspi0,
    .check_parent = check_parent_qspi0,
    .dump_possible_parent = dump_possible_parent_qspi,
    .physically_set_parent = set_parent_qspi0,
    .physically_open_clk = open_clk_qspi0,
    .physically_close_clk = close_clk_qspi0,
    .calc_freq = calc_freq_qspi0,
    .set_freq = set_freq_qspi0,
};

uint8_t is_physically_open_qspi0()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI0_EN);
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG4);

    if ((en == 1) && (cg != CG_OFF_ALL))
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_qspi0()
{
    uint8_t sel1 = EXTRACT_BITFIELD(s_reg_ccm->CGPR, CCM_CGPR_QSPIn_ACCZ);
    uint8_t sel2 = EXTRACT_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_QSPI0_CLK_SEL);
    P_CLK_NODE_T pclk;

    if (sel1 == 1)
        pclk = &clk_platform_bus_cm4_core;
    else
        switch (sel2) {
            case 0:
                pclk = &clk_pll3;
                break;
            case 1:
                pclk = &clk_pll3_pfd4;
                break;
            case 2:
                pclk = &clk_pll2_pfd4;
                break;
            case 3:
                pclk = &clk_pll1_pfd4;
                break;
            default:
                printf("Wrong bitfield Value when decide parent for QSPI0\n");
                break;
        }

    return pclk;
}

uint8_t check_parent_qspi0(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_qspi0();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_pll3) || (parent == &clk_pll3_pfd4) ||
            (parent == &clk_pll2_pfd4) || (parent == &clk_pll1_pfd4) ||
            (parent == &clk_platform_bus_cm4_core)) {
        *old_open = clk_qspi0.active;
        *new_open = clk_qspi0.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_qspi()
{
    printf("%s / %s / %s / %s / %s\n", clk_name_str(clk_pll3.name), clk_name_str(clk_pll3_pfd4.name),
            clk_name_str(clk_pll2_pfd4.name), clk_name_str(clk_pll1_pfd4.name), clk_name_str(clk_platform_bus_cm4_core.name));
    return 1;
}

uint8_t set_parent_qspi0(P_CLK_NODE_T parent)
{
    uint8_t sel;
    if (parent == &clk_platform_bus_cm4_core) {
        printf("WARNING, THIS WILL ALSO CHANGE THE PARENT OF QSPI1, please manually set parent for that too\n");
        INS_BITFIELD(s_reg_ccm->CGPR, CCM_CGPR_QSPIn_ACCZ, 1);
    }
    else {
        if (parent == &clk_pll3)
            sel = 0;
        else if (parent == &clk_pll3_pfd4)
            sel = 1;
        else if (parent == &clk_pll2_pfd4)
            sel = 2;
        else if (parent == &clk_pll1_pfd4)
            sel = 3;
        else {
            printf("set wrong parent for QSPI0\n");
            return 0;
        }
        INS_BITFIELD(s_reg_ccm->CGPR, CCM_CGPR_QSPIn_ACCZ, 0);
        INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_QSPI0_CLK_SEL, sel);
    }

    return 1;
}

uint8_t open_clk_qspi0()
{
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI0_EN, 1);
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG4, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_qspi0()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG4, CG_OFF_ALL);
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI0_EN, 0);
    return 1;
}

uint8_t calc_freq_qspi0(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t x4_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI0_X4_DIV) + 1;
    uint8_t x2_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI0_X2_DIV) + 1;
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI0_DIV) + 1;
    p_self->freq = parent_freq / (x4_div * x2_div * div);
    return 1;
}

uint8_t set_freq_qspi0(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs = va_arg(args, int);
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI0_DIV);
    uint8_t div_x2 = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI0_X2_DIV);
    uint8_t div_x4 = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI0_X4_DIV);
    uint8_t new_div, new_div_x2, new_div_x4;
    if ((numArgs == 0) || (numArgs != 3)) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CSCDR3.QSPI0_DIV (0~1) (Current %d)\n", div);
        printf("    - 2. CCM_CSCDR3.QSPI0_X2_DIV (0~1) (Current %d)\n", div_x2);
        printf("    - 3. CCM_CSCDR3.QSPI0_X4_DIV (0~3) (Current %d)\n", div_x4);
        return 0;
    }

    new_div = va_arg(args, int);
    new_div_x2 = va_arg(args, int);
    new_div_x4 = va_arg(args, int);

    // check parameter
    if ((new_div == div) && (new_div_x2 == div_x2) && (new_div_x4 == div_x4)) {
        printf("Same Div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }

    if ((new_div > 1) || (new_div_x2 > 1) || (new_div_x4 > 3)) {
        printf("Invalid div value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    *old_val = (new_div + 1) * (new_div_x2 + 1) * (new_div_x4 + 1);
    *new_val = (div + 1) * (div_x2 + 1) * (div_x4 + 1);

    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI0_DIV, new_div);
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI0_X2_DIV, new_div_x2);
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI0_X4_DIV, new_div_x4);

    p_self->freq = p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node QSPI1_CLK
 */
static CLK_NODE_T clk_qspi1 = {
    .name = CLK_QSPI1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_qspi1,
    .get_parent = get_parent_qspi1,
    .check_parent = check_parent_qspi1,
    .dump_possible_parent = dump_possible_parent_qspi,
    .physically_set_parent = set_parent_qspi1,
    .physically_open_clk = open_clk_qspi1,
    .physically_close_clk = close_clk_qspi1,
    .calc_freq = calc_freq_qspi1,
    .set_freq = set_freq_qspi1,
};

uint8_t is_physically_open_qspi1()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI1_EN);
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR8_CG4);

    if ((en == 1) && (cg != CG_OFF_ALL))
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_qspi1()
{
    uint8_t sel1 = EXTRACT_BITFIELD(s_reg_ccm->CGPR, CCM_CGPR_QSPIn_ACCZ);
    uint8_t sel2 = EXTRACT_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_QSPI1_CLK_SEL);
    P_CLK_NODE_T pclk;

    if (sel1 == 1)
        pclk = &clk_platform_bus_cm4_core;
    else
        switch (sel2) {
            case 0:
                pclk = &clk_pll3;
                break;
            case 1:
                pclk = &clk_pll3_pfd4;
                break;
            case 2:
                pclk = &clk_pll2_pfd4;
                break;
            case 3:
                pclk = &clk_pll1_pfd4;
                break;
            default:
                printf("Wrong bitfield Value when decide parent for QSPI1\n");
                break;
        }

    return pclk;
}

uint8_t check_parent_qspi1(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_qspi1();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_pll3) || (parent == &clk_pll3_pfd4) ||
            (parent == &clk_pll2_pfd4) || (parent == &clk_pll1_pfd4) ||
            (parent == &clk_platform_bus_cm4_core)) {
        *old_open = clk_qspi1.active;
        *new_open = clk_qspi1.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t set_parent_qspi1(P_CLK_NODE_T parent)
{
    uint8_t sel;
    if (parent == &clk_platform_bus_cm4_core) {
        printf("WARNING, THIS WILL ALSO CHANGE THE PARENT OF QSPI0, please manually set parent for that too\n");
        INS_BITFIELD(s_reg_ccm->CGPR, CCM_CGPR_QSPIn_ACCZ, 1);
    }
    else {
        if (parent == &clk_pll3)
            sel = 0;
        else if (parent == &clk_pll3_pfd4)
            sel = 1;
        else if (parent == &clk_pll2_pfd4)
            sel = 2;
        else if (parent == &clk_pll1_pfd4)
            sel = 3;
        else {
            printf("set wrong parent for QSPI1\n");
            return 0;
        }
        INS_BITFIELD(s_reg_ccm->CGPR, CCM_CGPR_QSPIn_ACCZ, 0);
        INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_QSPI1_CLK_SEL, sel);
    }

    return 1;
}

uint8_t open_clk_qspi1()
{
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI1_EN, 1);
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR8_CG4, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_qspi1()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR8_CG4, CG_OFF_ALL);
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI1_EN, 0);
    return 1;
}

uint8_t calc_freq_qspi1(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t x4_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI1_X4_DIV) + 1;
    uint8_t x2_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI1_X2_DIV) + 1;
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI1_DIV) + 1;
    p_self->freq = parent_freq / (x4_div * x2_div * div);
    return 1;
}

uint8_t set_freq_qspi1(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs = va_arg(args, int);
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI1_DIV);
    uint8_t div_x2 = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI1_X2_DIV);
    uint8_t div_x4 = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI1_X4_DIV);
    uint8_t new_div, new_div_x2, new_div_x4;
    if ((numArgs == 0) || (numArgs != 3)) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CSCDR3.QSPI1_DIV (0~1) (Current %d)\n", div);
        printf("    - 2. CCM_CSCDR3.QSPI1_X2_DIV (0~1) (Current %d)\n", div_x2);
        printf("    - 3. CCM_CSCDR3.QSPI1_X4_DIV (0~3) (Current %d)\n", div_x4);
        return 0;
    }

    new_div = va_arg(args, int);
    new_div_x2 = va_arg(args, int);
    new_div_x4 = va_arg(args, int);

    // check parameter
    if ((new_div == div) && (new_div_x2 == div_x2) && (new_div_x4 == div_x4)) {
        printf("Same Div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }

    if ((new_div > 1) || (new_div_x2 > 1) || (new_div_x4 > 3)) {
        printf("Invalid div value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    *old_val = (new_div + 1) * (new_div_x2 + 1) * (new_div_x4 + 1);
    *new_val = (div + 1) * (div_x2 + 1) * (div_x4 + 1);

    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI1_DIV, new_div);
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI1_X2_DIV, new_div_x2);
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_QSPI1_X4_DIV, new_div_x4);

    p_self->freq = p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node ENET_EXT
 */
static CLK_NODE_T clk_enet_ext = {
    .name = CLK_ENET_EXT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_enet_ext,
    .get_parent = get_parent_enet_ext,
    .physically_open_clk = open_clk_enet_ext,
    .physically_close_clk = close_clk_enet_ext,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_enet_ext()
{
    return 0;
}

P_CLK_NODE_T get_parent_enet_ext()
{
    return NULL;
}

uint8_t open_clk_enet_ext()
{
    return 1;
}

uint8_t close_clk_enet_ext()
{
    return 1;
}

/*
 * Clock Node ENET_TS_EXT
 */
static CLK_NODE_T clk_enet_ts_ext = {
    .name = CLK_ENET_TS_EXT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_enet_ts_ext,
    .get_parent = get_parent_enet_ts_ext,
    .physically_open_clk = open_clk_enet_ts_ext,
    .physically_close_clk = close_clk_enet_ts_ext,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_enet_ts_ext()
{
    return 0;
}

P_CLK_NODE_T get_parent_enet_ts_ext()
{
    return NULL;
}

uint8_t open_clk_enet_ts_ext()
{
    return 1;
}

uint8_t close_clk_enet_ts_ext()
{
    return 1;
}

/*
 * Clock Node ENET_RMII
 */
static CLK_NODE_T clk_enet_rmii = {
    .name = CLK_ENET_RMII,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_enet_rmii,
    .get_parent = get_parent_enet_rmii,
    .check_parent = check_parent_enet_rmii,
    .dump_possible_parent = dump_possible_parent_enet_rmii,
    .physically_set_parent = set_parent_enet_rmii,
    .physically_open_clk = open_clk_enet_rmii,
    .physically_close_clk = close_clk_enet_rmii,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_enet_rmii()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_RMII_CLK_EN);
    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_enet_rmii()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_RMII_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_enet_ext;
            break;
        case 1:
            pclk = &clk_audio_ext;
            break;
        case 2:
            pclk = &clk_pll5;
            break;
        case 3:
            pclk = &clk_pll5_div;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for ENET_RMII\n");
            break;
    }
    return pclk;
}

uint8_t check_parent_enet_rmii(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_enet_rmii();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_enet_ext) || (parent == &clk_audio_ext) ||
            (parent == &clk_pll5) || (parent == &clk_pll5_div)) {
        *old_open = clk_enet_rmii.active;
        *new_open = clk_enet_rmii.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_enet_rmii()
{
    printf("%s / %s / %s / %s\n", clk_name_str(clk_enet_ext.name), clk_name_str(clk_audio_ext.name), clk_name_str(clk_pll5.name),
            clk_name_str(clk_pll5_div.name));
    return 1;
}

uint8_t set_parent_enet_rmii(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_enet_ext)
        sel = 0;
    else if (parent == &clk_audio_ext)
        sel = 1;
    else if (parent == &clk_pll5)
        sel = 2;
    else if (parent == &clk_pll5_div)
        sel = 3;
    else {
        printf("set wrong parent for ENET_RMII\n");
        return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_RMII_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_enet_rmii()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_RMII_CLK_EN, 1);
    return 1;
}

uint8_t close_clk_enet_rmii()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_RMII_CLK_EN, 0);
    return 1;
}

/*
 * Clock Node USBC0
 */
static CLK_NODE_T clk_usbc0 = {
    .name = CLK_USBC0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_usbc0,
    .get_parent = get_parent_usbc0,
    .physically_open_clk = open_clk_usbc0,
    .physically_close_clk = close_clk_usbc0,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_usbc0()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG4);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_usbc0()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_usbc0()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG4, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_usbc0()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG4, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node USBC1
 */
static CLK_NODE_T clk_usbc1 = {
    .name = CLK_USBC1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_usbc1,
    .get_parent = get_parent_usbc1,
    .physically_open_clk = open_clk_usbc1,
    .physically_close_clk = close_clk_usbc1,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_usbc1()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG4);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_usbc1()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_usbc1()
{
    INS_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG4, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_usbc1()
{
    INS_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG4, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node ENET_TS
 */
static CLK_NODE_T clk_enet_ts = {
    .name = CLK_ENET_TS,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_enet_ts,
    .get_parent = get_parent_enet_ts,
    .check_parent = check_parent_enet_ts,
    .dump_possible_parent = dump_possible_parent_enet_ts,
    .physically_set_parent = set_parent_enet_ts,
    .physically_open_clk = open_clk_enet_ts,
    .physically_close_clk = close_clk_enet_ts,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_enet_ts()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_ENET_TS_EN);
    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_enet_ts()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_ENET_TS_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_enet_ext;
            break;
        case 1:
            pclk = &clk_fxosc;
            break;
        case 2:
            pclk = &clk_audio_ext;
            break;
        case 3:
            pclk = &clk_usbc0;
            break;
        case 4:
            pclk = &clk_enet_ts_ext;
            break;
        case 5:
            pclk = &clk_pll5_div;
            break;
        case 6:
            pclk = &clk_pll5;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for ENET_TS\n");
            break;
    }
    return pclk;
}

uint8_t check_parent_enet_ts(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_enet_ts();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_enet_ext) || (parent == &clk_fxosc) ||
            (parent == &clk_audio_ext) || (parent == &clk_usbc0) ||
            (parent == &clk_enet_ts_ext) || (parent == &clk_pll5_div) ||
            (parent == &clk_pll5)) {
        *old_open = clk_enet_ts.active;
        *new_open = clk_enet_ts.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_enet_ts()
{
    printf("%s / %s / %s / %s / %s\n", clk_name_str(clk_enet_ext.name), clk_name_str(clk_fxosc.name),
            clk_name_str(clk_audio_ext.name), clk_name_str(clk_usbc0.name), clk_name_str(clk_enet_ts_ext.name),
            clk_name_str(clk_pll5_div.name), clk_name_str(clk_pll5.name));
    return 1;
}

uint8_t set_parent_enet_ts(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_enet_ext)
        sel = 0;
    else if (parent == &clk_fxosc)
        sel = 1;
    else if (parent == &clk_audio_ext)
        sel = 2;
    else if (parent == &clk_usbc0)
        sel = 3;
    else if (parent == &clk_enet_ts_ext)
        sel = 4;
    else if (parent == &clk_pll5_div)
        sel = 5;
    else if (parent == &clk_pll5)
        sel = 6;
    else {
        printf("set wrong parent for ENET_TS\n");
        return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_ENET_TS_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_enet_ts()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_ENET_TS_EN, 1);
    return 1;
}

uint8_t close_clk_enet_ts()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_ENET_TS_EN, 0);
    return 1;
}

/*
 * Clock Node ESDHC0
 */
static CLK_NODE_T clk_esdhc0 = {
    .name = CLK_ESDHC0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_esdhc0,
    .get_parent = get_parent_esdhc0,
    .check_parent = check_parent_esdhc0,
    .dump_possible_parent = dump_possible_parent_esdhc,
    .physically_set_parent = set_parent_esdhc0,
    .physically_open_clk = open_clk_esdhc0,
    .physically_close_clk = close_clk_esdhc0,
    .calc_freq = calc_freq_esdhc0,
    .set_freq = set_freq_esdhc0,
};

uint8_t is_physically_open_esdhc0()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESDHC0_EN);
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG1);

    if ((en == 1) && (cg != CG_OFF_ALL))
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_esdhc0()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_ESDHC0_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_pll3;
            break;
        case 1:
            pclk = &clk_pll3_pfd3;
            break;
        case 2:
            pclk = &clk_pll1_pfd3;
            break;
        case 3:
            pclk = &clk_platform_bus_cm4_core;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for NFC\n");
            break;
    }

    return pclk;
}

uint8_t check_parent_esdhc0(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_esdhc0();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_pll3) || (parent == &clk_pll3_pfd3) ||
            (parent == &clk_pll1_pfd3) || (parent == &clk_platform_bus_cm4_core)) {
        *old_open = clk_esdhc0.active;
        *new_open = clk_esdhc0.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_esdhc()
{
    printf("%s / %s / %s / %s\n", clk_name_str(clk_pll3.name), clk_name_str(clk_pll3_pfd3.name), clk_name_str(clk_pll1_pfd3.name),
            clk_name_str(clk_platform_bus_cm4_core.name));
    return 1;
}

uint8_t set_parent_esdhc0(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_pll3)
        sel = 0;
    else if (parent == &clk_pll3_pfd3)
        sel = 1;
    else if (parent == &clk_pll1_pfd3)
        sel = 2;
    else if (parent == &clk_platform_bus_cm4_core)
        sel = 3;
    else {
            printf("set wrong parent for ESDHC0\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_ESDHC0_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_esdhc0()
{
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESDHC0_EN, 1);
    INS_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG1, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_esdhc0()
{
    INS_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG1, CG_OFF_ALL);
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESDHC0_EN, 0);
    return 1;
}

uint8_t calc_freq_esdhc0(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESDHC0_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_esdhc0(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESDHC0_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CSCDR2.ESDHC0_DIV (0~15) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (new_div == old_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 15) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESDHC0_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node ESDHC1
 */
static CLK_NODE_T clk_esdhc1 = {
    .name = CLK_ESDHC1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_esdhc1,
    .get_parent = get_parent_esdhc1,
    .check_parent = check_parent_esdhc1,
    .dump_possible_parent = dump_possible_parent_esdhc,
    .physically_set_parent = set_parent_esdhc1,
    .physically_open_clk = open_clk_esdhc1,
    .physically_close_clk = close_clk_esdhc1,
    .calc_freq = calc_freq_esdhc1,
    .set_freq = set_freq_esdhc1,
};

uint8_t is_physically_open_esdhc1()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESDHC1_EN);
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG2);

    if ((en == 1) && (cg != CG_OFF_ALL))
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_esdhc1()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_ESDHC1_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_pll3;
            break;
        case 1:
            pclk = &clk_pll3_pfd3;
            break;
        case 2:
            pclk = &clk_pll1_pfd3;
            break;
        case 3:
            pclk = &clk_platform_bus_cm4_core;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for NFC\n");
            break;
    }

    return pclk;
}

uint8_t check_parent_esdhc1(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_esdhc1();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_pll3) || (parent == &clk_pll3_pfd3) ||
            (parent == &clk_pll1_pfd3) || (parent == &clk_platform_bus_cm4_core)) {
        *old_open = clk_esdhc1.active;
        *new_open = clk_esdhc1.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t set_parent_esdhc1(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_pll3)
        sel = 0;
    else if (parent == &clk_pll3_pfd3)
        sel = 1;
    else if (parent == &clk_pll1_pfd3)
        sel = 2;
    else if (parent == &clk_platform_bus_cm4_core)
        sel = 3;
    else {
            printf("set wrong parent for ESDHC1\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_ESDHC1_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_esdhc1()
{
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESDHC1_EN, 1);
    INS_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG2, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_esdhc1()
{
    INS_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG1, CG_OFF_ALL);
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESDHC1_EN, 0);
    return 1;
}

uint8_t calc_freq_esdhc1(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESDHC1_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_esdhc1(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESDHC1_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CSCDR2.ESDHC1_DIV (0~15) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (new_div == old_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 15) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESDHC1_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node DCU_PIXEL0
 */
static CLK_NODE_T clk_dcu_pixel0 = {
    .name = CLK_DCU_PIXEL0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_dcu_pixel0,
    .get_parent = get_parent_dcu_pixel0,
    .check_parent = check_parent_dcu_pixel0,
    .dump_possible_parent = dump_possible_parent_dcu_pixel,
    .physically_set_parent = set_parent_dcu_pixel0,
    .physically_open_clk = open_clk_dcu_pixel0,
    .physically_close_clk = close_clk_dcu_pixel0,
    .calc_freq = calc_freq_dcu_pixel0,
    .set_freq = set_freq_dcu_pixel0,
};

uint8_t is_physically_open_dcu_pixel0()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_DCU0_EN);
    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_dcu_pixel0()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_DCU0_CLK_SEL);
    return sel ? &clk_pll3 : &clk_pll1_pfd2;
}

uint8_t check_parent_dcu_pixel0(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_dcu_pixel0();

    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_pll3) || (parent == &clk_pll1_pfd2)) {
        *old_open = clk_dcu_pixel0.active;
        *new_open = clk_dcu_pixel0.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_dcu_pixel()
{
    printf("%s / %s\n", clk_name_str(clk_pll3.name), clk_name_str(clk_pll1_pfd2.name));
    return 1;
}

uint8_t set_parent_dcu_pixel0(P_CLK_NODE_T parent)
{
    if (parent == &clk_pll3) {
        INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_DCU0_CLK_SEL, 1);
    } else if (parent == &clk_pll1_pfd2) {
        INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_DCU0_CLK_SEL, 0);
    }
    return 1;
}

uint8_t open_clk_dcu_pixel0()
{
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_DCU0_EN, 1);
    return 1;
}

uint8_t close_clk_dcu_pixel0()
{
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_DCU0_EN, 0);
    return 1;
}

uint8_t calc_freq_dcu_pixel0(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_DCU0_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_dcu_pixel0(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_DCU0_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CSCDR3.DCU0_DIV (0~7) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (new_div == old_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 7) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_DCU0_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node DCU_PIXEL1
 */
static CLK_NODE_T clk_dcu_pixel1 = {
    .name = CLK_DCU_PIXEL1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_dcu_pixel1,
    .get_parent = get_parent_dcu_pixel1,
    .check_parent = check_parent_dcu_pixel1,
    .dump_possible_parent = dump_possible_parent_dcu_pixel,
    .physically_set_parent = set_parent_dcu_pixel1,
    .physically_open_clk = open_clk_dcu_pixel1,
    .physically_close_clk = close_clk_dcu_pixel1,
    .calc_freq = calc_freq_dcu_pixel1,
    .set_freq = set_freq_dcu_pixel1,
};

uint8_t is_physically_open_dcu_pixel1()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_DCU1_EN);
    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_dcu_pixel1()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_DCU1_CLK_SEL);
    return sel ? &clk_pll3 : &clk_pll1_pfd2;
}

uint8_t check_parent_dcu_pixel1(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_dcu_pixel1();

    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_pll3) || (parent == &clk_pll1_pfd2)) {
        *old_open = clk_dcu_pixel1.active;
        *new_open = clk_dcu_pixel1.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t set_parent_dcu_pixel1(P_CLK_NODE_T parent)
{
    if (parent == &clk_pll3) {
        INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_DCU1_CLK_SEL, 1);
    } else if (parent == &clk_pll1_pfd2) {
        INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_DCU1_CLK_SEL, 0);
    }
    return 1;
}

uint8_t open_clk_dcu_pixel1()
{
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_DCU1_EN, 1);
    return 1;
}

uint8_t close_clk_dcu_pixel1()
{
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_DCU1_EN, 0);
    return 1;
}

uint8_t calc_freq_dcu_pixel1(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_DCU1_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_dcu_pixel1(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_DCU1_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CSCDR3.DCU1_DIV (0~7) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (new_div == old_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 7) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_DCU1_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}
/*
 * Clock Node DCU0
 */
static CLK_NODE_T clk_dcu0 = {
    .name = CLK_DCU0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_dcu0,
    .get_parent = get_parent_dcu0,
    .physically_open_clk = open_clk_dcu0,
    .physically_close_clk = close_clk_dcu0,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_dcu0()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR3, CCM_CCGR3_CG8);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_dcu0()
{
    return &clk_platform_bus_cm4_core;
}

uint8_t open_clk_dcu0()
{
    INS_BITFIELD(s_reg_ccm->CCGR3, CCM_CCGR3_CG8, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_dcu0()
{
    INS_BITFIELD(s_reg_ccm->CCGR3, CCM_CCGR3_CG8, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node DCU0
 */
static CLK_NODE_T clk_dcu1 = {
    .name = CLK_DCU1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_dcu1,
    .get_parent = get_parent_dcu1,
    .physically_open_clk = open_clk_dcu1,
    .physically_close_clk = close_clk_dcu1,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_dcu1()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR3, CCM_CCGR9_CG8);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_dcu1()
{
    return &clk_platform_bus_cm4_core;
}

uint8_t open_clk_dcu1()
{
    INS_BITFIELD(s_reg_ccm->CCGR3, CCM_CCGR9_CG8, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_dcu1()
{
    INS_BITFIELD(s_reg_ccm->CCGR3, CCM_CCGR9_CG8, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node SPDIF_EXT
 */
static CLK_NODE_T clk_spdif_ext = {
    .name = CLK_SPDIF_EXT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_spdif_ext,
    .get_parent = get_parent_spdif_ext,
    .physically_open_clk = open_clk_spdif_ext,
    .physically_close_clk = close_clk_spdif_ext,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_spdif_ext()
{
    return 0;
}

P_CLK_NODE_T get_parent_spdif_ext()
{
    return NULL;
}

uint8_t open_clk_spdif_ext()
{
    return 1;
}

uint8_t close_clk_spdif_ext()
{
    return 1;
}


/*
 * Clock Node MLB_EXT
 */
static CLK_NODE_T clk_mlb_ext = {
    .name = CLK_MLB_EXT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_mlb_ext,
    .get_parent = get_parent_mlb_ext,
    .physically_open_clk = open_clk_mlb_ext,
    .physically_close_clk = close_clk_mlb_ext,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_mlb_ext()
{
    return 0;
}

P_CLK_NODE_T get_parent_mlb_ext()
{
    return NULL;
}

uint8_t open_clk_mlb_ext()
{
    return 1;
}

uint8_t close_clk_mlb_ext()
{
    return 1;
}

/*
 * Clock Node ESAI_HCKT_EXT
 */
static CLK_NODE_T clk_esai_ext = {
    .name = CLK_ESAI_HCKT_EXT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_esai_ext,
    .get_parent = get_parent_esai_ext,
    .physically_open_clk = open_clk_esai_ext,
    .physically_close_clk = close_clk_esai_ext,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_esai_ext()
{
    return 0;
}

P_CLK_NODE_T get_parent_esai_ext()
{
    return NULL;
}

uint8_t open_clk_esai_ext()
{
    return 1;
}

uint8_t close_clk_esai_ext()
{
    return 1;
}

/*
 * Clock Node SAI0_EXT
 */
static CLK_NODE_T clk_sai0_ext = {
    .name = CLK_SAI0TX_BCLK_EXT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_sai0_ext,
    .get_parent = get_parent_sai0_ext,
    .physically_open_clk = open_clk_sai0_ext,
    .physically_close_clk = close_clk_sai0_ext,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_sai0_ext()
{
    return 0;
}

P_CLK_NODE_T get_parent_sai0_ext()
{
    return NULL;
}

uint8_t open_clk_sai0_ext()
{
    return 1;
}

uint8_t close_clk_sai0_ext()
{
    return 1;
}

/*
 * Clock Node SAI3_EXT
 */
static CLK_NODE_T clk_sai3_ext = {
    .name = CLK_SAI3_BCLK_EXT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_sai3_ext,
    .get_parent = get_parent_sai3_ext,
    .physically_open_clk = open_clk_sai3_ext,
    .physically_close_clk = close_clk_sai3_ext,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_sai3_ext()
{
    return 0;
}

P_CLK_NODE_T get_parent_sai3_ext()
{
    return NULL;
}

uint8_t open_clk_sai3_ext()
{
    return 1;
}

uint8_t close_clk_sai3_ext()
{
    return 1;
}


/*
 * Clock Node ESAI
 */
static CLK_NODE_T clk_esai = {
    .name = CLK_ESAI,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_esai,
    .get_parent = get_parent_esai,
    .check_parent = check_parent_esai,
    .dump_possible_parent = dump_possible_parent_esai,
    .physically_set_parent = set_parent_esai,
    .physically_open_clk = open_clk_esai,
    .physically_close_clk = close_clk_esai,
    .calc_freq = calc_freq_esai,
    .set_freq = set_freq_esai,
};

uint8_t is_physically_open_esai()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESAI_EN);
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG2);

    if ((en == 1) && (cg != CG_OFF_ALL))
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_esai()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_ESAI_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_audio_ext;
            break;
        case 2:
            pclk = &clk_spdif_ext;
            break;
        case 3:
            pclk = &clk_pll4_div;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for ESAI\n");
            break;
    }
    return pclk;
}

uint8_t check_parent_esai(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_esai();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_audio_ext) || (parent == &clk_spdif_ext) ||
            (parent == &clk_pll4_div)) {
        *old_open = clk_esai.active;
        *new_open = clk_esai.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_esai()
{
    printf("%s / %s / %s\n", clk_name_str(clk_audio_ext.name), clk_name_str(clk_spdif_ext.name), clk_name_str(clk_pll4_div.name));
    return 1;
}

uint8_t set_parent_esai(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_audio_ext)
        sel = 0;
    else if (parent == &clk_spdif_ext)
        sel = 2;
    else if (parent == &clk_pll4_div)
        sel = 3;
    else {
            printf("set wrong parent for ESAI\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_ESAI_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_esai()
{
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESAI_EN, 1);
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG2, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_esai()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG2, CG_OFF_ALL);
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESAI_EN, 0);
    return 1;
}

uint8_t calc_freq_esai(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESAI_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_esai(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESAI_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CSCDR2.ESAI_DIV (0~15) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (new_div == old_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 15) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_ESAI_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node SAI0
 */
static CLK_NODE_T clk_sai0 = {
    .name = CLK_SAI0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_sai0,
    .get_parent = get_parent_sai0,
    .check_parent = check_parent_sai0,
    .dump_possible_parent = dump_possible_parent_sai,
    .physically_set_parent = set_parent_sai0,
    .physically_open_clk = open_clk_sai0,
    .physically_close_clk = close_clk_sai0,
    .calc_freq = calc_freq_sai0,
    .set_freq = set_freq_sai0,
};

uint8_t is_physically_open_sai0()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI0_EN);
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG15);

    if ((en == 1) && (cg != CG_OFF_ALL))
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_sai0()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_SAI0_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_audio_ext;
            break;
        case 2:
            pclk = &clk_spdif_ext;
            break;
        case 3:
            pclk = &clk_pll4_div;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for SAI0\n");
            break;
    }
    return pclk;
}

uint8_t check_parent_sai0(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_sai0();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_audio_ext) || (parent == &clk_spdif_ext) ||
            (parent == &clk_pll4_div)) {
        *old_open = clk_sai0.active;
        *new_open = clk_sai0.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_sai()
{
    printf("%s / %s / %s\n", clk_name_str(clk_audio_ext.name), clk_name_str(clk_spdif_ext.name), clk_name_str(clk_pll4_div.name));
    return 1;
}

uint8_t set_parent_sai0(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_audio_ext)
        sel = 0;
    else if (parent == &clk_spdif_ext)
        sel = 2;
    else if (parent == &clk_pll4_div)
        sel = 3;
    else {
            printf("set wrong parent for SAI0\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_SAI0_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_sai0()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI0_EN, 1);
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG15, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_sai0()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG15, CG_OFF_ALL);
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI0_EN, 0);
    return 1;
}

uint8_t calc_freq_sai0(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI0_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_sai0(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI0_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CSCDR1.SAI0_DIV (0~15) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (new_div == old_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 15) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI0_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node SAI1
 */
static CLK_NODE_T clk_sai1 = {
    .name = CLK_SAI1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_sai1,
    .get_parent = get_parent_sai1,
    .check_parent = check_parent_sai1,
    .dump_possible_parent = dump_possible_parent_sai,
    .physically_set_parent = set_parent_sai1,
    .physically_open_clk = open_clk_sai1,
    .physically_close_clk = close_clk_sai1,
    .calc_freq = calc_freq_sai1,
    .set_freq = set_freq_sai1,
};

uint8_t is_physically_open_sai1()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI1_EN);
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG0);

    if ((en == 1) && (cg != CG_OFF_ALL))
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_sai1()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_SAI1_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_audio_ext;
            break;
        case 2:
            pclk = &clk_spdif_ext;
            break;
        case 3:
            pclk = &clk_pll4_div;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for SAI1\n");
            break;
    }
    return pclk;
}

uint8_t check_parent_sai1(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_sai1();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_audio_ext) || (parent == &clk_spdif_ext) ||
            (parent == &clk_pll4_div)) {
        *old_open = clk_sai1.active;
        *new_open = clk_sai1.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t set_parent_sai1(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_audio_ext)
        sel = 0;
    else if (parent == &clk_spdif_ext)
        sel = 2;
    else if (parent == &clk_pll4_div)
        sel = 3;
    else {
            printf("set wrong parent for SAI1\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_SAI1_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_sai1()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI1_EN, 1);
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR1_CG0, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_sai1()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR1_CG0, CG_OFF_ALL);
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI1_EN, 0);
    return 1;
}

uint8_t calc_freq_sai1(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI1_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_sai1(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI1_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CSCDR1.SAI1_DIV (0~15) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (new_div == old_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 15) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI1_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node SAI2
 */
static CLK_NODE_T clk_sai2 = {
    .name = CLK_SAI2,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_sai2,
    .get_parent = get_parent_sai2,
    .check_parent = check_parent_sai2,
    .dump_possible_parent = dump_possible_parent_sai,
    .physically_set_parent = set_parent_sai2,
    .physically_open_clk = open_clk_sai2,
    .physically_close_clk = close_clk_sai2,
    .calc_freq = calc_freq_sai2,
    .set_freq = set_freq_sai2,
};

uint8_t is_physically_open_sai2()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI2_EN);
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG1);

    if ((en == 1) && (cg != CG_OFF_ALL))
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_sai2()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_SAI2_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_audio_ext;
            break;
        case 2:
            pclk = &clk_spdif_ext;
            break;
        case 3:
            pclk = &clk_pll4_div;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for SAI2\n");
            break;
    }
    return pclk;
}

uint8_t check_parent_sai2(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_sai2();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_audio_ext) || (parent == &clk_spdif_ext) ||
            (parent == &clk_pll4_div)) {
        *old_open = clk_sai2.active;
        *new_open = clk_sai2.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t set_parent_sai2(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_audio_ext)
        sel = 0;
    else if (parent == &clk_spdif_ext)
        sel = 2;
    else if (parent == &clk_pll4_div)
        sel = 3;
    else {
            printf("set wrong parent for SAI2\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_SAI2_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_sai2()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI2_EN, 1);
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR1_CG1, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_sai2()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR1_CG1, CG_OFF_ALL);
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI2_EN, 0);
    return 1;
}

uint8_t calc_freq_sai2(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI2_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_sai2(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI2_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CSCDR1.SAI2_DIV (0~15) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (new_div == old_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 15) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI2_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node SAI3
 */
static CLK_NODE_T clk_sai3 = {
    .name = CLK_SAI3,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_sai3,
    .get_parent = get_parent_sai3,
    .check_parent = check_parent_sai3,
    .dump_possible_parent = dump_possible_parent_sai,
    .physically_set_parent = set_parent_sai3,
    .physically_open_clk = open_clk_sai3,
    .physically_close_clk = close_clk_sai3,
    .calc_freq = calc_freq_sai3,
    .set_freq = set_freq_sai3,
};

uint8_t is_physically_open_sai3()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI3_EN);
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG2);

    if ((en == 1) && (cg != CG_OFF_ALL))
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_sai3()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_SAI3_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_audio_ext;
            break;
        case 2:
            pclk = &clk_spdif_ext;
            break;
        case 3:
            pclk = &clk_pll4_div;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for SAI3\n");
            break;
    }
    return pclk;
}

uint8_t check_parent_sai3(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_sai3();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_audio_ext) || (parent == &clk_spdif_ext) ||
            (parent == &clk_pll4_div)) {
        *old_open = clk_sai3.active;
        *new_open = clk_sai3.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t set_parent_sai3(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_audio_ext)
        sel = 0;
    else if (parent == &clk_spdif_ext)
        sel = 2;
    else if (parent == &clk_pll4_div)
        sel = 3;
    else {
            printf("set wrong parent for SAI3\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_SAI3_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_sai3()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI3_EN, 1);
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR1_CG2, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_sai3()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR1_CG2, CG_OFF_ALL);
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI3_EN, 0);
    return 1;
}

uint8_t calc_freq_sai3(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI3_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_sai3(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI3_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CSCDR1.SAI3_DIV (0~15) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (new_div == old_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 15) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_SAI3_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node VIDEO_ADC
 */
static CLK_NODE_T clk_video_adc = {
    .name = CLK_VIDEO_ADC,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_video_adc,
    .get_parent = get_parent_video_adc,
    .check_parent = check_parent_video_adc,
    .dump_possible_parent = dump_possible_parent_video_adc,
    .physically_set_parent = set_parent_video_adc,
    .physically_open_clk = open_clk_video_adc,
    .physically_close_clk = close_clk_video_adc,
    .calc_freq = calc_freq_video_adc,
    .set_freq = set_freq_video_adc,
};

uint8_t is_physically_open_video_adc()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_VADC_EN);
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG7);

    if ((en == 1) && (cg != CG_OFF_ALL))
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_video_adc()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_VADC_CLK_SEL);
    P_CLK_NODE_T pclk = NULL;

    switch (sel) {
        case 0:
            pclk = &clk_pll6_div;
            break;
        case 1:
            pclk = &clk_pll3_div;
            break;
        case 2:
            pclk = &clk_pll3;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for VADC\n");
            break;
    }

    return pclk;
}

uint8_t check_parent_video_adc(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_video_adc();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_pll6_div) || (parent == &clk_pll3_div) ||
            (parent == &clk_pll3)) {
        *old_open = clk_video_adc.active;
        *new_open = clk_video_adc.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_video_adc()
{
    printf("%s / %s / %s\n", clk_name_str(clk_pll6_div.name), clk_name_str(clk_pll3_div.name), clk_name_str(clk_pll3.name));
    return 1;
}

uint8_t set_parent_video_adc(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_pll6_div)
        sel = 0;
    else if (parent == &clk_pll3_div)
        sel = 1;
    else if (parent == &clk_pll3)
        sel = 2;
    else {
            printf("set wrong parent for ESAI\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_VADC_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_video_adc()
{
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_VADC_EN, 1);
    INS_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG7, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_video_adc()
{
    INS_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG7, CG_OFF_ALL);
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_VADC_EN, 0);
    return 1;
}

uint8_t calc_freq_video_adc(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_VADC_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_video_adc(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_VADC_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CSCDR1.VADC_DIV (0~3) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (new_div == old_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 3) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CSCDR1, CCM_CSCDR1_VADC_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node GPU
 */
static CLK_NODE_T clk_gpu = {
    .name = CLK_GPU,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_gpu,
    .get_parent = get_parent_gpu,
    .check_parent = check_parent_gpu,
    .dump_possible_parent = dump_possible_parent_gpu,
    .physically_set_parent = set_parent_gpu,
    .physically_open_clk = open_clk_gpu,
    .physically_close_clk = close_clk_gpu,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_gpu()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_GPU_EN);
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG13);

    if ((en == 1) && (cg != CG_OFF_ALL))
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_gpu()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_GPU_CLK_SEL);
    return sel ? &clk_pll3_pfd2: &clk_pll2_pfd2;
}

uint8_t check_parent_gpu(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_gpu();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_pll3_pfd2) || (parent == &clk_pll2_pfd2)) {
        *old_open = clk_gpu.active;
        *new_open = clk_gpu.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_gpu()
{
    printf("%s / %s\n", clk_name_str(clk_pll3_pfd2.name), clk_name_str(clk_pll2_pfd2.name));
    return 1;
}

uint8_t set_parent_gpu(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;

    if (parent == &clk_pll3_pfd2)
        sel = 1;
    else if (parent == &clk_pll2_pfd2)
        sel = 0;
    else {
            printf("set wrong parent for GPU\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR1, CCM_CSCMR1_GPU_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_gpu()
{
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_GPU_EN, 1);
    INS_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG13, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_gpu()
{
    INS_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG13, CG_OFF_ALL);
    INS_BITFIELD(s_reg_ccm->CSCDR2, CCM_CSCDR2_GPU_EN, 0);
    return 1;
}

/*
 * Clock Node SWO
 */
static CLK_NODE_T clk_swo = {
    .name = CLK_SWO,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_swo,
    .get_parent = get_parent_swo,
    .check_parent = check_parent_swo,
    .dump_possible_parent = dump_possible_parent_swo,
    .physically_set_parent = set_parent_swo,
    .physically_open_clk = open_clk_swo,
    .physically_close_clk = close_clk_swo,
    .calc_freq = calc_freq_swo,
    .set_freq = set_freq_swo,
};

uint8_t is_physically_open_swo()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_SWO_EN);
    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_swo()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_SWO_CLK_SEL);
    return sel ? &clk_ips_ipg : &clk_firc;
}

uint8_t check_parent_swo(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_swo();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_ips_ipg) || (parent == &clk_firc)) {
        *old_open = clk_swo.active;
        *new_open = clk_swo.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_swo()
{
    printf("%s / %s\n", clk_name_str(clk_ips_ipg.name), clk_name_str(clk_firc.name));
    return 1;
}

uint8_t set_parent_swo(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;

    if (parent == &clk_ips_ipg)
        sel = 1;
    else if (parent == &clk_firc)
        sel = 0;
    else {
            printf("set wrong parent for SWO\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_SWO_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_swo()
{
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_SWO_EN, 1);
    return 1;
}

uint8_t close_clk_swo()
{
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_SWO_EN, 0);
    return 1;
}

uint8_t calc_freq_swo(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_SWO_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_swo(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_SWO_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CSCDR3.SWO_DIV (0~1) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (new_div == old_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 1) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_SWO_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node Trace
 */
static CLK_NODE_T clk_trace = {
    .name = CLK_TRACE,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_trace,
    .get_parent = get_parent_trace,
    .check_parent = check_parent_trace,
    .dump_possible_parent = dump_possible_parent_trace,
    .physically_set_parent = set_parent_trace,
    .physically_open_clk = open_clk_trace,
    .physically_close_clk = close_clk_trace,
    .calc_freq = calc_freq_trace,
    .set_freq = set_freq_trace,
};

uint8_t is_physically_open_trace()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_TRACE_EN);
    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_trace()
{
    uint8_t sel = EXTRACT_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_TRACE_CLK_SEL);
    return sel ? &clk_pll3 : &clk_platform_bus_cm4_core;
}

uint8_t check_parent_trace(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_trace();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_pll3) || (parent == &clk_platform_bus_cm4_core)) {
        *old_open = clk_trace.active;
        *new_open = clk_trace.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_trace()
{
    printf("%s / %s\n", clk_name_str(clk_pll3.name), clk_name_str(clk_platform_bus_cm4_core.name));
    return 1;
}

uint8_t set_parent_trace(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;

    if (parent == &clk_pll3)
        sel = 1;
    else if (parent == &clk_platform_bus_cm4_core)
        sel = 0;
    else {
            printf("set wrong parent for TRACE\n");
            return 0;
    }

    INS_BITFIELD(s_reg_ccm->CSCMR2, CCM_CSCMR2_TRACE_CLK_SEL, sel);

    return 1;
}

uint8_t open_clk_trace()
{
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_TRACE_EN, 1);
    return 1;
}

uint8_t close_clk_trace()
{
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_TRACE_EN, 0);
    return 1;
}

uint8_t calc_freq_trace(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint8_t div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_TRACE_DIV) + 1;
    p_self->freq = parent_freq / div;
    return 1;
}

uint8_t set_freq_trace(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_div, new_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_div = EXTRACT_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_TRACE_DIV);
    if (numArgs != 1) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. CCM_CSCDR3.TRACE_DIV (0~3) (Current %d)\n", old_div);
        return 0;
    }
    new_div = va_arg(args, int);
    if (new_div == old_div) {
        printf("Same div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }
    if (new_div > 3) {
        printf("Invalid mult value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_ccm->CSCDR3, CCM_CSCDR3_TRACE_DIV, new_div);

    // update the freq
    *old_val = new_div + 1;
    *new_val = old_div + 1;
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node USB0
 */
static CLK_NODE_T clk_usb0 = {
    .name = CLK_USB0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_usb0,
    .get_parent = get_parent_usb0,
    .physically_open_clk = open_clk_usb0,
    .physically_close_clk = close_clk_usb0,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_usb0()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_anadig->PLL3_CTRL, Anadig_PLL3_CTRL_EN_USB_CLKS);
    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_usb0()
{
    return &clk_pll3;
}

uint8_t open_clk_usb0()
{
    INS_BITFIELD(s_reg_anadig->PLL3_CTRL, Anadig_PLL3_CTRL_EN_USB_CLKS, 1);
    return 1;
}

uint8_t close_clk_usb0()
{
    INS_BITFIELD(s_reg_anadig->PLL3_CTRL, Anadig_PLL3_CTRL_EN_USB_CLKS, 0);
    return 1;
}

/*
 * Clock Node USB1
 */
static CLK_NODE_T clk_usb1 = {
    .name = CLK_USB1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_usb1,
    .get_parent = get_parent_usb1,
    .physically_open_clk = open_clk_usb1,
    .physically_close_clk = close_clk_usb1,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_usb1()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_anadig->PLL7_CTRL, Anadig_PLL7_CTRL_EN_USB_CLKS);
    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_usb1()
{
    return &clk_pll7;
}

uint8_t open_clk_usb1()
{
    INS_BITFIELD(s_reg_anadig->PLL7_CTRL, Anadig_PLL7_CTRL_EN_USB_CLKS, 1);
    return 1;
}

uint8_t close_clk_usb1()
{
    INS_BITFIELD(s_reg_anadig->PLL7_CTRL, Anadig_PLL7_CTRL_EN_USB_CLKS, 0);
    return 1;
}

/*
 * Clock Node SPDIF
 */
static CLK_NODE_T clk_spdif = {
    .name = CLK_SPDIF,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_spdif,
    .get_parent = get_parent_spdif,
    .check_parent = check_parent_spdif,
    .dump_possible_parent = dump_possible_parent_spdif,
    .physically_set_parent = set_parent_spdif,
    .physically_open_clk = open_clk_spdif,
    .physically_close_clk = close_clk_spdif,
    .calc_freq = calc_freq_spdif,
    .set_freq = set_freq_spdif,
};

uint8_t is_physically_open_spdif()
{
    uint8_t en = 0, cg = 0;

    /* When CG is not open, access to SPDIF registers will cause system hang*/
    cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG1);
    if (cg != CG_OFF_ALL)
        en = EXTRACT_BITFIELD(s_reg_spdif->STC, SPDIF_STC_tx_all_clk_en);

    return en ? 1 : 0;
}

P_CLK_NODE_T get_parent_spdif()
{
    uint8_t sel = 0;
    P_CLK_NODE_T pclk = NULL;

    /* When CG is not open, access to SPDIF registers will cause system hang*/
    if (is_physically_open_spdif())
        sel = EXTRACT_BITFIELD(s_reg_spdif->STC, SPDIF_STC_TxClk_Source);

    switch (sel) {
        case 0:
            pclk = &clk_spdif_ext;
            break;
        case 1:
            pclk = &clk_pll4_div;
            break;
        case 2:
            pclk = &clk_audio_ext;
            break;
        case 3:
            pclk = &clk_mlb_ext;
            break;
        case 4:
            pclk = &clk_esai_ext;
            break;
        case 5:
            pclk = &clk_sys;  // Need further examination
            break;
        case 6:
            pclk = &clk_sai0_ext;
            break;
        case 7:
            pclk = &clk_sai3_ext;
            break;
        default:
            printf("Wrong bitfield Value when decide parent for SPDIF\n");
            break;
    }
    return pclk;
}

uint8_t check_parent_spdif(P_CLK_NODE_T parent, uint8_t* old_open, uint8_t* new_open)
{
    P_CLK_NODE_T old_parent = get_parent_spdif();
    if (old_parent == parent) {
        return CHECK_PARENT_IDENTICAL;
    }
    else if ((parent == &clk_spdif_ext) || (parent == &clk_pll4_div) ||
            (parent == &clk_audio_ext) || (parent == &clk_mlb_ext) ||
            (parent == &clk_esai_ext) || (parent == &clk_sys) ||
            (parent == &clk_sai0_ext) || (parent == &clk_sai3_ext)) {
        *old_open = clk_spdif.active;
        *new_open = clk_spdif.active;
        return CHECK_PARENT_PASS;
    }
    else
        return CHECK_PARENT_INVALID;
}

uint8_t dump_possible_parent_spdif()
{
    printf("%s / %s / %s / %s / %s / %s / %s / %s\n",
            clk_name_str(clk_spdif_ext.name), clk_name_str(clk_pll4_div.name), clk_name_str(clk_audio_ext.name),
            clk_name_str(clk_mlb_ext.name), clk_name_str(clk_esai_ext.name), clk_name_str(clk_sys.name),
            clk_name_str(clk_sai0.name), clk_name_str(clk_sai3_ext.name));
    return 1;
}

uint8_t set_parent_spdif(P_CLK_NODE_T parent)
{
    uint8_t sel = 0;
    if (parent == &clk_spdif_ext)
        sel = 0;
    else if (parent == &clk_pll4_div)
        sel = 1;
    else if (parent == &clk_audio_ext)
        sel = 2;
    else if (parent == &clk_mlb_ext)
        sel = 3;
    else if (parent == &clk_esai_ext)
        sel = 4;
    else if (parent == &clk_sys)
        sel = 5;
    else if (parent == &clk_sai0_ext)
        sel = 6;
    else if (parent == &clk_sai3_ext)
        sel = 7;
    else {
        printf("set wrong parent for SPDIF\n");
        return 0;
    }

    INS_BITFIELD(s_reg_spdif->STC, SPDIF_STC_TxClk_Source, sel);

    return 1;
}

uint8_t open_clk_spdif()
{
    INS_BITFIELD(s_reg_spdif->STC, SPDIF_STC_tx_all_clk_en, 1);
    return 1;
}

uint8_t close_clk_spdif()
{
    INS_BITFIELD(s_reg_spdif->STC, SPDIF_STC_tx_all_clk_en, 0);
    return 1;
}

uint8_t calc_freq_spdif(P_CLK_NODE_T p_self, uint32_t parent_freq)
{
    uint16_t sys_div = 0;
    uint8_t tx_div = 0;

    /* When CG is not open, access to SPDIF registers will cause system hang*/
    if (is_physically_open_spdif()) {
        sys_div = EXTRACT_BITFIELD(s_reg_spdif->STC, SPDIF_STC_SYSCLK_DF);
        tx_div = EXTRACT_BITFIELD(s_reg_spdif->STC, SPDIF_STC_TxClk_DF);
        if (p_self->parent == &clk_sys) {
            p_self->freq = parent_freq / ((sys_div + 1) * (tx_div + 1));
        } else {
            p_self->freq = parent_freq / (tx_div + 1);
        }
    }
    else 
        p_self->freq = 0;

    return 1;
}

uint8_t set_freq_spdif(P_CLK_NODE_T p_self, va_list args, uint8_t* old_val, uint8_t* new_val)
{
    uint8_t numArgs, old_tx_div, new_tx_div;
    uint16_t old_sys_div, new_sys_div;
    // Args Check
    numArgs = va_arg(args, int);
    old_sys_div = EXTRACT_BITFIELD(s_reg_spdif->STC, SPDIF_STC_SYSCLK_DF);
    old_tx_div = EXTRACT_BITFIELD(s_reg_spdif->STC, SPDIF_STC_TxClk_DF);
    if (numArgs != 2) {
        printf("Freq Configuration Register for [%s] are:\n", clk_name_str(p_self->name));
        printf("    - 1. SPDIF_STC.SYSCLK_DF (1~511) (Current %d)\n", old_sys_div);
        printf("    - 2. SPDIF_STC.TXCLK_DF (0~127) (Current %d)\n", old_tx_div);
        return 0;
    }
    new_sys_div = va_arg(args, int);
    new_tx_div = va_arg(args, int);

    if ((new_sys_div == old_sys_div) && (new_tx_div == old_tx_div)) {
        printf("Same Div value for [%s], freq config reg will not be modified\n", clk_name_str(p_self->name));
        return 0;
    }

    if ((p_self->parent == &clk_sys) && (new_sys_div != old_sys_div)) {
        printf("Try to set sys_clk div for [%s] when parent is not sys_clk, please check\n", clk_name_str(p_self->name));
        return 0;
    }

    if ((new_sys_div == 0) || (new_sys_div > 511) || (new_tx_div > 127)) {
        printf("Invalid div value for [%s], please check\n", clk_name_str(p_self->name));
        return 0;
    }

    // Fill the register
    INS_BITFIELD(s_reg_spdif->STC, SPDIF_STC_SYSCLK_DF, new_sys_div);
    INS_BITFIELD(s_reg_spdif->STC, SPDIF_STC_TxClk_DF, new_tx_div);

    // update the freq
    if (p_self->parent == &clk_sys) {
        *old_val = (new_sys_div + 1) * (new_tx_div + 1);
        *new_val = (old_sys_div + 1) * (old_tx_div + 1);
    } else {
        *old_val = new_tx_div + 1;
        *new_val = old_tx_div + 1;
    }
    p_self->freq = (uint64_t)p_self->freq * (*new_val) / (*old_val);

    return 1;
}

/*
 * Clock Node DAP
 */
static CLK_NODE_T clk_dap = {
    .name = CLK_DAP,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_dap,
    .get_parent = get_parent_dap,
    .physically_open_clk = open_clk_dap,
    .physically_close_clk = close_clk_dap,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_dap()
{
    uint8_t en = EXTRACT_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_DAP_EN);
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG7);

    if ((en == 1) && (cg != CG_OFF_ALL))
        return 1;
    else
        return 0;
}

P_CLK_NODE_T get_parent_dap()
{
    return &clk_platform_bus_cm4_core;
}

uint8_t open_clk_dap()
{
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_DAP_EN, 1);
    INS_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG7, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_dap()
{
    INS_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG7, CG_OFF_ALL);
    INS_BITFIELD(s_reg_ccm->CCSR, CCM_CCSR_DAP_EN, 0);
    return 1;
}

/*
 * Clock Node DMA_MUX0
 */
static CLK_NODE_T clk_dma_mux0 = {
    .name = CLK_DMA_MUX0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_dma_mux0,
    .get_parent = get_parent_dma_mux0,
    .physically_open_clk = open_clk_dma_mux0,
    .physically_close_clk = close_clk_dma_mux0,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_dma_mux0()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG4);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_dma_mux0()
{
    return &clk_platform_bus_cm4_core;
}

uint8_t open_clk_dma_mux0()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG4, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_dma_mux0()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG4, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node DMA_MUX1
 */
static CLK_NODE_T clk_dma_mux1 = {
    .name = CLK_DMA_MUX1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_dma_mux1,
    .get_parent = get_parent_dma_mux1,
    .physically_open_clk = open_clk_dma_mux1,
    .physically_close_clk = close_clk_dma_mux1,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_dma_mux1()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG5);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_dma_mux1()
{
    return &clk_platform_bus_cm4_core;
}

uint8_t open_clk_dma_mux1()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG5, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_dma_mux1()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG5, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node DMA_MUX2
 */
static CLK_NODE_T clk_dma_mux2 = {
    .name = CLK_DMA_MUX2,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_dma_mux2,
    .get_parent = get_parent_dma_mux2,
    .physically_open_clk = open_clk_dma_mux2,
    .physically_close_clk = close_clk_dma_mux2,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_dma_mux2()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG1);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_dma_mux2()
{
    return &clk_platform_bus_cm4_core;
}

uint8_t open_clk_dma_mux2()
{
    INS_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG1, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_dma_mux2()
{
    INS_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG1, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node DMA_MUX3
 */
static CLK_NODE_T clk_dma_mux3 = {
    .name = CLK_DMA_MUX3,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_dma_mux3,
    .get_parent = get_parent_dma_mux3,
    .physically_open_clk = open_clk_dma_mux3,
    .physically_close_clk = close_clk_dma_mux3,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_dma_mux3()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG2);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_dma_mux3()
{
    return &clk_platform_bus_cm4_core;
}

uint8_t open_clk_dma_mux3()
{
    INS_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG2, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_dma_mux3()
{
    INS_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG2, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node DMA0_TCD
 */
static CLK_NODE_T clk_dma0_tcd = {
    .name = CLK_DMA0_TCD,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_dma0_tcd,
    .get_parent = get_parent_dma0_tcd,
    .physically_open_clk = open_clk_dma0_tcd,
    .physically_close_clk = close_clk_dma0_tcd,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_dma0_tcd()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR1, CCM_CCPGR1_PPCG9);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_dma0_tcd()
{
    return &clk_platform_bus_cm4_core;
}

uint8_t open_clk_dma0_tcd()
{
    INS_BITFIELD(s_reg_ccm->CCPGR1, CCM_CCPGR1_PPCG9, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_dma0_tcd()
{
    INS_BITFIELD(s_reg_ccm->CCPGR1, CCM_CCPGR1_PPCG9, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node DMA1_TCD
 */
static CLK_NODE_T clk_dma1_tcd = {
    .name = CLK_DMA1_TCD,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_dma1_tcd,
    .get_parent = get_parent_dma1_tcd,
    .physically_open_clk = open_clk_dma1_tcd,
    .physically_close_clk = close_clk_dma1_tcd,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_dma1_tcd()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG9);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_dma1_tcd()
{
    return &clk_platform_bus_cm4_core;
}

uint8_t open_clk_dma1_tcd()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG9, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_dma1_tcd()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG9, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node DMA0
 */
static CLK_NODE_T clk_dma0 = {
    .name = CLK_DMA0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_dma0,
    .get_parent = get_parent_dma0,
    .physically_open_clk = open_clk_dma0,
    .physically_close_clk = close_clk_dma0,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_dma0()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR1, CCM_CCPGR1_PPCG8);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_dma0()
{
    return &clk_platform_bus_cm4_core;
}

uint8_t open_clk_dma0()
{
    INS_BITFIELD(s_reg_ccm->CCPGR1, CCM_CCPGR1_PPCG8, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_dma0()
{
    INS_BITFIELD(s_reg_ccm->CCPGR1, CCM_CCPGR1_PPCG8, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node DMA1
 */
static CLK_NODE_T clk_dma1 = {
    .name = CLK_DMA1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_dma1,
    .get_parent = get_parent_dma1,
    .physically_open_clk = open_clk_dma1,
    .physically_close_clk = close_clk_dma1,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_dma1()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG8);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_dma1()
{
    return &clk_platform_bus_cm4_core;
}

uint8_t open_clk_dma1()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG8, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_dma1()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG8, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node UART0
 */
static CLK_NODE_T clk_uart0 = {
    .name = CLK_UART0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_uart0,
    .get_parent = get_parent_uart0,
    .physically_open_clk = open_clk_uart0,
    .physically_close_clk = close_clk_uart0,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_uart0()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG7);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_uart0()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_uart0()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG7, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_uart0()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG7, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node UART1
 */
static CLK_NODE_T clk_uart1 = {
    .name = CLK_UART1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_uart1,
    .get_parent = get_parent_uart1,
    .physically_open_clk = open_clk_uart1,
    .physically_close_clk = close_clk_uart1,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_uart1()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG8);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_uart1()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_uart1()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG8, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_uart1()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG8, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node UART2
 */
static CLK_NODE_T clk_uart2 = {
    .name = CLK_UART2,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_uart2,
    .get_parent = get_parent_uart2,
    .physically_open_clk = open_clk_uart2,
    .physically_close_clk = close_clk_uart2,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_uart2()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG9);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_uart2()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_uart2()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG9, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_uart2()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG9, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node UART3
 */
static CLK_NODE_T clk_uart3 = {
    .name = CLK_UART3,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_uart3,
    .get_parent = get_parent_uart3,
    .physically_open_clk = open_clk_uart3,
    .physically_close_clk = close_clk_uart3,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_uart3()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG10);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_uart3()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_uart3()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG10, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_uart3()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG10, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node UART4
 */
static CLK_NODE_T clk_uart4 = {
    .name = CLK_UART4,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_uart4,
    .get_parent = get_parent_uart4,
    .physically_open_clk = open_clk_uart4,
    .physically_close_clk = close_clk_uart4,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_uart4()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG9);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_uart4()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_uart4()
{
    INS_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG9, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_uart4()
{
    INS_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG9, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node UART5
 */
static CLK_NODE_T clk_uart5 = {
    .name = CLK_UART5,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_uart5,
    .get_parent = get_parent_uart5,
    .physically_open_clk = open_clk_uart5,
    .physically_close_clk = close_clk_uart5,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_uart5()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG10);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_uart5()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_uart5()
{
    INS_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG10, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_uart5()
{
    INS_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG10, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node SPI0
 */
static CLK_NODE_T clk_spi0 = {
    .name = CLK_SPI0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_spi0,
    .get_parent = get_parent_spi0,
    .physically_open_clk = open_clk_spi0,
    .physically_close_clk = close_clk_spi0,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_spi0()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG12);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_spi0()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_spi0()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG12, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_spi0()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG12, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node SPI1
 */
static CLK_NODE_T clk_spi1 = {
    .name = CLK_SPI1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_spi1,
    .get_parent = get_parent_spi1,
    .physically_open_clk = open_clk_spi1,
    .physically_close_clk = close_clk_spi1,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_spi1()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG13);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_spi1()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_spi1()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG13, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_spi1()
{
    INS_BITFIELD(s_reg_ccm->CCGR0, CCM_CCGR0_CG13, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node SPI2
 */
static CLK_NODE_T clk_spi2 = {
    .name = CLK_SPI2,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_spi2,
    .get_parent = get_parent_spi2,
    .physically_open_clk = open_clk_spi2,
    .physically_close_clk = close_clk_spi2,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_spi2()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG12);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_spi2()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_spi2()
{
    INS_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG12, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_spi2()
{
    INS_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG12, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node SPI3
 */
static CLK_NODE_T clk_spi3 = {
    .name = CLK_SPI3,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_spi3,
    .get_parent = get_parent_spi3,
    .physically_open_clk = open_clk_spi3,
    .physically_close_clk = close_clk_spi3,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_spi3()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG13);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_spi3()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_spi3()
{
    INS_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG13, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_spi3()
{
    INS_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG13, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node CRC
 */
static CLK_NODE_T clk_crc = {
    .name = CLK_CRC,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_crc,
    .get_parent = get_parent_crc,
    .physically_open_clk = open_clk_crc,
    .physically_close_clk = close_clk_crc,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_crc()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG3);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_crc()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_crc()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG3, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_crc()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG3, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node PDB
 */
static CLK_NODE_T clk_pdb = {
    .name = CLK_PDB,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pdb,
    .get_parent = get_parent_pdb,
    .physically_open_clk = open_clk_pdb,
    .physically_close_clk = close_clk_pdb,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_pdb()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG3);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_pdb()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_pdb()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG3, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_pdb()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG3, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node PIT
 */
static CLK_NODE_T clk_pit = {
    .name = CLK_PIT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pit,
    .get_parent = get_parent_pit,
    .physically_open_clk = open_clk_pit,
    .physically_close_clk = close_clk_pit,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_pit()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG7);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_pit()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_pit()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG7, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_pit()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG7, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node FTM0
 */
static CLK_NODE_T clk_ftm0 = {
    .name = CLK_FTM0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ftm0,
    .get_parent = get_parent_ftm0,
    .physically_open_clk = open_clk_ftm0,
    .physically_close_clk = close_clk_ftm0,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ftm0()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG8);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ftm0()
{
    return &clk_platform_bus_cm4_core;  // need examination
}

uint8_t open_clk_ftm0()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG8, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ftm0()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG8, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node FTM1
 */
static CLK_NODE_T clk_ftm1 = {
    .name = CLK_FTM1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ftm1,
    .get_parent = get_parent_ftm1,
    .physically_open_clk = open_clk_ftm1,
    .physically_close_clk = close_clk_ftm1,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ftm1()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG9);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ftm1()
{
    return &clk_platform_bus_cm4_core;  // need examination
}

uint8_t open_clk_ftm1()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG9, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ftm1()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG9, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node FTM2
 */
static CLK_NODE_T clk_ftm2 = {
    .name = CLK_FTM2,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ftm2,
    .get_parent = get_parent_ftm2,
    .physically_open_clk = open_clk_ftm2,
    .physically_close_clk = close_clk_ftm2,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ftm2()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG8);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ftm2()
{
    return &clk_platform_bus_cm4_core;  // need examination
}

uint8_t open_clk_ftm2()
{
    INS_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG8, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ftm2()
{
    INS_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG8, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node FTM3
 */
static CLK_NODE_T clk_ftm3 = {
    .name = CLK_FTM3,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ftm3,
    .get_parent = get_parent_ftm3,
    .physically_open_clk = open_clk_ftm3,
    .physically_close_clk = close_clk_ftm3,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ftm3()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG9);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ftm3()
{
    return &clk_platform_bus_cm4_core;  // need examination
}

uint8_t open_clk_ftm3()
{
    INS_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG9, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ftm3()
{
    INS_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG9, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node ADC0
 */
static CLK_NODE_T clk_adc0 = {
    .name = CLK_ADC0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_adc0,
    .get_parent = get_parent_adc0,
    .physically_open_clk = open_clk_adc0,
    .physically_close_clk = close_clk_adc0,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_adc0()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG11);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_adc0()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_adc0()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG11, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_adc0()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG11, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node ADC1
 */
static CLK_NODE_T clk_adc1 = {
    .name = CLK_ADC1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_adc1,
    .get_parent = get_parent_adc1,
    .physically_open_clk = open_clk_adc1,
    .physically_close_clk = close_clk_adc1,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_adc1()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG11);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_adc1()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_adc1()
{
    INS_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG11, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_adc1()
{
    INS_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG11, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node TCON0
 */
static CLK_NODE_T clk_tcon0 = {
    .name = CLK_TCON0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_tcon0,
    .get_parent = get_parent_tcon0,
    .physically_open_clk = open_clk_tcon0,
    .physically_close_clk = close_clk_tcon0,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_tcon0()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG13);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_tcon0()
{
    return &clk_platform_bus_cm4_core;
}

uint8_t open_clk_tcon0()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG13, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_tcon0()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG13, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node TCON1
 */
static CLK_NODE_T clk_tcon1 = {
    .name = CLK_TCON1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_tcon1,
    .get_parent = get_parent_tcon1,
    .physically_open_clk = open_clk_tcon1,
    .physically_close_clk = close_clk_tcon1,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_tcon1()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG13);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_tcon1()
{
    return &clk_platform_bus_cm4_core;
}

uint8_t open_clk_tcon1()
{
    INS_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG13, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_tcon1()
{
    INS_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG13, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node WDOG_A5
 */
static CLK_NODE_T clk_wdog_a5 = {
    .name = CLK_WDOG_A5,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_wdog_a5,
    .get_parent = get_parent_wdog_a5,
    .physically_open_clk = open_clk_wdog_a5,
    .physically_close_clk = close_clk_wdog_a5,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_wdog_a5()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG14);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_wdog_a5()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_wdog_a5()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG14, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_wdog_a5()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG14, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node WDOG_M4
 */
static CLK_NODE_T clk_wdog_m4 = {
    .name = CLK_WDOG_M4,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_wdog_m4,
    .get_parent = get_parent_wdog_m4,
    .physically_open_clk = open_clk_wdog_m4,
    .physically_close_clk = close_clk_wdog_m4,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_wdog_m4()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG15);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_wdog_m4()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_wdog_m4()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG15, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_wdog_m4()
{
    INS_BITFIELD(s_reg_ccm->CCGR1, CCM_CCGR1_CG15, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node LPTMR
 */
static CLK_NODE_T clk_lptmr = {
    .name = CLK_LPTMR,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_lptmr,
    .get_parent = get_parent_lptmr,
    .physically_open_clk = open_clk_lptmr,
    .physically_close_clk = close_clk_lptmr,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_lptmr()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG0);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_lptmr()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_lptmr()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG0, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_lptmr()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG0, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node RLE
 */
static CLK_NODE_T clk_rle = {
    .name = CLK_RLE,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_rle,
    .get_parent = get_parent_rle,
    .physically_open_clk = open_clk_rle,
    .physically_close_clk = close_clk_rle,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_rle()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG2);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_rle()
{
    return &clk_platform_bus_cm4_core;
}

uint8_t open_clk_rle()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG2, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_rle()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG2, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node IOMUXC
 */
static CLK_NODE_T clk_iomuxc = {
    .name = CLK_IOMUXC,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_iomuxc,
    .get_parent = get_parent_iomuxc,
    .physically_open_clk = open_clk_iomuxc,
    .physically_close_clk = close_clk_iomuxc,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_iomuxc()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG8);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_iomuxc()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_iomuxc()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG8, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_iomuxc()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG8, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node PORTA_MULT
 */
static CLK_NODE_T clk_porta_mult = {
    .name = CLK_PORTA_MULT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_porta_mult,
    .get_parent = get_parent_porta_mult,
    .physically_open_clk = open_clk_porta_mult,
    .physically_close_clk = close_clk_porta_mult,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_porta_mult()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG9);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_porta_mult()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_porta_mult()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG9, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_porta_mult()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG9, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node PORTB_MULT
 */
static CLK_NODE_T clk_portb_mult = {
    .name = CLK_PORTB_MULT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_portb_mult,
    .get_parent = get_parent_portb_mult,
    .physically_open_clk = open_clk_portb_mult,
    .physically_close_clk = close_clk_portb_mult,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_portb_mult()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG10);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_portb_mult()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_portb_mult()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG10, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_portb_mult()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG10, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node PORTC_MULT
 */
static CLK_NODE_T clk_portc_mult = {
    .name = CLK_PORTC_MULT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_portc_mult,
    .get_parent = get_parent_portc_mult,
    .physically_open_clk = open_clk_portc_mult,
    .physically_close_clk = close_clk_portc_mult,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_portc_mult()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG11);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_portc_mult()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_portc_mult()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG11, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_portc_mult()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG11, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node PORTD_MULT
 */
static CLK_NODE_T clk_portd_mult = {
    .name = CLK_PORTD_MULT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_portd_mult,
    .get_parent = get_parent_portd_mult,
    .physically_open_clk = open_clk_portd_mult,
    .physically_close_clk = close_clk_portd_mult,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_portd_mult()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG12);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_portd_mult()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_portd_mult()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG12, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_portd_mult()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG12, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node PORTE_MULT
 */
static CLK_NODE_T clk_porte_mult = {
    .name = CLK_PORTE_MULT,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_porte_mult,
    .get_parent = get_parent_porte_mult,
    .physically_open_clk = open_clk_porte_mult,
    .physically_close_clk = close_clk_porte_mult,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_porte_mult()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG13);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_porte_mult()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_porte_mult()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG13, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_porte_mult()
{
    INS_BITFIELD(s_reg_ccm->CCGR2, CCM_CCGR2_CG13, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node ANADIG
 */
static CLK_NODE_T clk_anadig = {
    .name = CLK_ANADIG,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_anadig,
    .get_parent = get_parent_anadig,
    .physically_open_clk = open_clk_anadig,
    .physically_close_clk = close_clk_anadig,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_anadig()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR3, CCM_CCGR3_CG0);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_anadig()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_anadig()
{
    INS_BITFIELD(s_reg_ccm->CCGR3, CCM_CCGR3_CG0, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_anadig()
{
    INS_BITFIELD(s_reg_ccm->CCGR3, CCM_CCGR3_CG0, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node SCSC
 */
static CLK_NODE_T clk_scsc = {
    .name = CLK_SCSC,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_scsc,
    .get_parent = get_parent_scsc,
    .physically_open_clk = open_clk_scsc,
    .physically_close_clk = close_clk_scsc,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_scsc()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR3, CCM_CCGR3_CG2);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_scsc()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_scsc()
{
    INS_BITFIELD(s_reg_ccm->CCGR3, CCM_CCGR3_CG2, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_scsc()
{
    INS_BITFIELD(s_reg_ccm->CCGR3, CCM_CCGR3_CG2, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node ASRC
 */
static CLK_NODE_T clk_asrc = {
    .name = CLK_ASRC,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_asrc,
    .get_parent = get_parent_asrc,
    .physically_open_clk = open_clk_asrc,
    .physically_close_clk = close_clk_asrc,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_asrc()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG0);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_asrc()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_asrc()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG0, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_asrc()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG0, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node EWM
 */
static CLK_NODE_T clk_ewm = {
    .name = CLK_EWM,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ewm,
    .get_parent = get_parent_ewm,
    .physically_open_clk = open_clk_ewm,
    .physically_close_clk = close_clk_ewm,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ewm()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG5);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ewm()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_ewm()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG5, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ewm()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG5, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node I2C0
 */
static CLK_NODE_T clk_i2c0 = {
    .name = CLK_I2C0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_i2c0,
    .get_parent = get_parent_i2c0,
    .physically_open_clk = open_clk_i2c0,
    .physically_close_clk = close_clk_i2c0,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_i2c0()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG6);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_i2c0()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_i2c0()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG6, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_i2c0()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG6, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node I2C1
 */
static CLK_NODE_T clk_i2c1 = {
    .name = CLK_I2C1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_i2c1,
    .get_parent = get_parent_i2c1,
    .physically_open_clk = open_clk_i2c1,
    .physically_close_clk = close_clk_i2c1,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_i2c1()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG7);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_i2c1()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_i2c1()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG7, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_i2c1()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG7, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node I2C2
 */
static CLK_NODE_T clk_i2c2 = {
    .name = CLK_I2C2,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_i2c2,
    .get_parent = get_parent_i2c2,
    .physically_open_clk = open_clk_i2c2,
    .physically_close_clk = close_clk_i2c2,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_i2c2()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR10, CCM_CCGR10_CG6);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_i2c2()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_i2c2()
{
    INS_BITFIELD(s_reg_ccm->CCGR10, CCM_CCGR10_CG6, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_i2c2()
{
    INS_BITFIELD(s_reg_ccm->CCGR10, CCM_CCGR10_CG6, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node I2C3
 */
static CLK_NODE_T clk_i2c3 = {
    .name = CLK_I2C3,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_i2c3,
    .get_parent = get_parent_i2c3,
    .physically_open_clk = open_clk_i2c3,
    .physically_close_clk = close_clk_i2c3,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_i2c3()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR10, CCM_CCGR10_CG7);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_i2c3()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_i2c3()
{
    INS_BITFIELD(s_reg_ccm->CCGR10, CCM_CCGR10_CG7, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_i2c3()
{
    INS_BITFIELD(s_reg_ccm->CCGR10, CCM_CCGR10_CG7, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node WKUP
 */
static CLK_NODE_T clk_wkup = {
    .name = CLK_WKUP,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_wkup,
    .get_parent = get_parent_wkup,
    .physically_open_clk = open_clk_wkup,
    .physically_close_clk = close_clk_wkup,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_wkup()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG10);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_wkup()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_wkup()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG10, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_wkup()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG10, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node CCM
 */
static CLK_NODE_T clk_ccm = {
    .name = CLK_CCM,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ccm,
    .get_parent = get_parent_ccm,
    .physically_open_clk = open_clk_ccm,
    .physically_close_clk = close_clk_ccm,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ccm()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG11);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ccm()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_ccm()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG11, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ccm()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG11, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node GPC
 */
static CLK_NODE_T clk_gpc = {
    .name = CLK_GPC,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_gpc,
    .get_parent = get_parent_gpc,
    .physically_open_clk = open_clk_gpc,
    .physically_close_clk = close_clk_gpc,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_gpc()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG12);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_gpc()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_gpc()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG12, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_gpc()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG12, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node VREG_DIG
 */
static CLK_NODE_T clk_vreg_dig = {
    .name = CLK_VREG_DIG,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_vreg_dig,
    .get_parent = get_parent_vreg_dig,
    .physically_open_clk = open_clk_vreg_dig,
    .physically_close_clk = close_clk_vreg_dig,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_vreg_dig()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG13);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_vreg_dig()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_vreg_dig()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG13, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_vreg_dig()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG13, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node CMU
 */
static CLK_NODE_T clk_cmu = {
    .name = CLK_CMU,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_cmu,
    .get_parent = get_parent_cmu,
    .physically_open_clk = open_clk_cmu,
    .physically_close_clk = close_clk_cmu,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_cmu()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG15);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_cmu()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_cmu()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG15, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_cmu()
{
    INS_BITFIELD(s_reg_ccm->CCGR4, CCM_CCGR4_CG15, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node OTP
 */
static CLK_NODE_T clk_otp = {
    .name = CLK_OTP,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_otp,
    .get_parent = get_parent_otp,
    .physically_open_clk = open_clk_otp,
    .physically_close_clk = close_clk_otp,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_otp()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG5);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_otp()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_otp()
{
    INS_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG5, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_otp()
{
    INS_BITFIELD(s_reg_ccm->CCGR6, CCM_CCGR6_CG5, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node SLCD
 */
static CLK_NODE_T clk_slcd = {
    .name = CLK_SLCD,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_slcd,
    .get_parent = get_parent_slcd,
    .physically_open_clk = open_clk_slcd,
    .physically_close_clk = close_clk_slcd,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_slcd()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG14);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_slcd()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_slcd()
{
    INS_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG14, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_slcd()
{
    INS_BITFIELD(s_reg_ccm->CCGR7, CCM_CCGR7_CG14, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node Video Decoder
 */
static CLK_NODE_T clk_video_decoder = {
    .name = CLK_VIDEO_DECODER,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_video_decoder,
    .get_parent = get_parent_video_decoder,
    .physically_open_clk = open_clk_video_decoder,
    .physically_close_clk = close_clk_video_decoder,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_video_decoder()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG8);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_video_decoder()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_video_decoder()
{
    INS_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG8, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_video_decoder()
{
    INS_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG8, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node VIU
 */
static CLK_NODE_T clk_viu = {
    .name = CLK_VIU,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_viu,
    .get_parent = get_parent_viu,
    .physically_open_clk = open_clk_viu,
    .physically_close_clk = close_clk_viu,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_viu()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG9);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_viu()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_viu()
{
    INS_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG9, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_viu()
{
    INS_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG9, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node DAC0
 */
static CLK_NODE_T clk_dac0 = {
    .name = CLK_DAC0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_dac0,
    .get_parent = get_parent_dac0,
    .physically_open_clk = open_clk_dac0,
    .physically_close_clk = close_clk_dac0,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_dac0()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG12);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_dac0()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_dac0()
{
    INS_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG12, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_dac0()
{
    INS_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG12, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node DAC0
 */
static CLK_NODE_T clk_dac1 = {
    .name = CLK_DAC1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_dac1,
    .get_parent = get_parent_dac1,
    .physically_open_clk = open_clk_dac1,
    .physically_close_clk = close_clk_dac1,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_dac1()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG13);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_dac1()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_dac1()
{
    INS_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG13, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_dac1()
{
    INS_BITFIELD(s_reg_ccm->CCGR8, CCM_CCGR8_CG13, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node ENET_MAC_IEEE1588_Timer0
 */
static CLK_NODE_T clk_enet_mac_ieee1588_timer0 = {
    .name = CLK_ENET_MAC0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_enet_mac_ieee1588_timer0,
    .get_parent = get_parent_enet_mac_ieee1588_timer0,
    .physically_open_clk = open_clk_enet_mac_ieee1588_timer0,
    .physically_close_clk = close_clk_enet_mac_ieee1588_timer0,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_enet_mac_ieee1588_timer0()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR9, CCM_CCGR9_CG0);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_enet_mac_ieee1588_timer0()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_enet_mac_ieee1588_timer0()
{
    INS_BITFIELD(s_reg_ccm->CCGR9, CCM_CCGR9_CG0, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_enet_mac_ieee1588_timer0()
{
    INS_BITFIELD(s_reg_ccm->CCGR9, CCM_CCGR9_CG0, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node ENET_MAC_IEEE1588_Timer1
 */
static CLK_NODE_T clk_enet_mac_ieee1588_timer1 = {
    .name = CLK_ENET_MAC1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_enet_mac_ieee1588_timer1,
    .get_parent = get_parent_enet_mac_ieee1588_timer1,
    .physically_open_clk = open_clk_enet_mac_ieee1588_timer1,
    .physically_close_clk = close_clk_enet_mac_ieee1588_timer1,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_enet_mac_ieee1588_timer1()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR9, CCM_CCGR9_CG1);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_enet_mac_ieee1588_timer1()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_enet_mac_ieee1588_timer1()
{
    INS_BITFIELD(s_reg_ccm->CCGR9, CCM_CCGR9_CG1, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_enet_mac_ieee1588_timer1()
{
    INS_BITFIELD(s_reg_ccm->CCGR9, CCM_CCGR9_CG1, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node ENET_L2_SW
 */
static CLK_NODE_T clk_enet_l2_sw = {
    .name = CLK_ENET_L2_SW,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_enet_l2_sw,
    .get_parent = get_parent_enet_l2_sw,
    .physically_open_clk = open_clk_enet_l2_sw,
    .physically_close_clk = close_clk_enet_l2_sw,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_enet_l2_sw()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCGR10, CCM_CCGR10_CG8);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_enet_l2_sw()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_enet_l2_sw()
{
    INS_BITFIELD(s_reg_ccm->CCGR10, CCM_CCGR10_CG8, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_enet_l2_sw()
{
    INS_BITFIELD(s_reg_ccm->CCGR10, CCM_CCGR10_CG8, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node MSCM
 */
static CLK_NODE_T clk_mscm = {
    .name = CLK_MSCM,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_mscm,
    .get_parent = get_parent_mscm,
    .physically_open_clk = open_clk_mscm,
    .physically_close_clk = close_clk_mscm,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_mscm()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG1);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_mscm()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_mscm()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG1, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_mscm()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG1, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node CA5_SCU_GIC
 */
static CLK_NODE_T clk_ca5_scu_gic = {
    .name = CLK_CA5_SCU_GIC,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ca5_scu_gic,
    .get_parent = get_parent_ca5_scu_gic,
    .physically_open_clk = open_clk_ca5_scu_gic,
    .physically_close_clk = close_clk_ca5_scu_gic,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ca5_scu_gic()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG2);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ca5_scu_gic()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_ca5_scu_gic()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG2, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ca5_scu_gic()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG2, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node CA5_INTD
 */
static CLK_NODE_T clk_ca5_intd = {
    .name = CLK_CA5_INTD,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ca5_intd,
    .get_parent = get_parent_ca5_intd,
    .physically_open_clk = open_clk_ca5_intd,
    .physically_close_clk = close_clk_ca5_intd,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ca5_intd()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG3);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ca5_intd()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_ca5_intd()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG3, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ca5_intd()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG3, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node CA5_L2C
 */
static CLK_NODE_T clk_ca5_l2c = {
    .name = CLK_CA5_L2C,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ca5_l2c,
    .get_parent = get_parent_ca5_l2c,
    .physically_open_clk = open_clk_ca5_l2c,
    .physically_close_clk = close_clk_ca5_l2c,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ca5_l2c()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG6);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ca5_l2c()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_ca5_l2c()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG6, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ca5_l2c()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG6, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node NIC0
 */
static CLK_NODE_T clk_nic0 = {
    .name = CLK_NIC0,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_nic0,
    .get_parent = get_parent_nic0,
    .physically_open_clk = open_clk_nic0,
    .physically_close_clk = close_clk_nic0,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_nic0()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG8);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_nic0()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_nic0()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG8, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_nic0()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG8, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node NIC1
 */
static CLK_NODE_T clk_nic1 = {
    .name = CLK_NIC1,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_nic1,
    .get_parent = get_parent_nic1,
    .physically_open_clk = open_clk_nic1,
    .physically_close_clk = close_clk_nic1,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_nic1()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG9);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_nic1()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_nic1()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG9, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_nic1()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG9, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node NIC2
 */
static CLK_NODE_T clk_nic2 = {
    .name = CLK_NIC2,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_nic2,
    .get_parent = get_parent_nic2,
    .physically_open_clk = open_clk_nic2,
    .physically_close_clk = close_clk_nic2,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_nic2()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG10);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_nic2()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_nic2()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG10, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_nic2()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG10, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node NIC3
 */
static CLK_NODE_T clk_nic3 = {
    .name = CLK_NIC3,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_nic3,
    .get_parent = get_parent_nic3,
    .physically_open_clk = open_clk_nic3,
    .physically_close_clk = close_clk_nic3,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_nic3()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG11);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_nic3()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_nic3()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG11, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_nic3()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG11, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node NIC4
 */
static CLK_NODE_T clk_nic4 = {
    .name = CLK_NIC4,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_nic4,
    .get_parent = get_parent_nic4,
    .physically_open_clk = open_clk_nic4,
    .physically_close_clk = close_clk_nic4,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_nic4()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG12);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_nic4()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_nic4()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG12, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_nic4()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG12, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node NIC5
 */
static CLK_NODE_T clk_nic5 = {
    .name = CLK_NIC5,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_nic5,
    .get_parent = get_parent_nic5,
    .physically_open_clk = open_clk_nic5,
    .physically_close_clk = close_clk_nic5,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_nic5()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG13);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_nic5()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_nic5()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG13, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_nic5()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG13, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node NIC6
 */
static CLK_NODE_T clk_nic6 = {
    .name = CLK_NIC6,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_nic6,
    .get_parent = get_parent_nic6,
    .physically_open_clk = open_clk_nic6,
    .physically_close_clk = close_clk_nic6,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_nic6()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG14);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_nic6()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_nic6()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG14, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_nic6()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG14, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node NIC7
 */
static CLK_NODE_T clk_nic7 = {
    .name = CLK_NIC7,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_nic7,
    .get_parent = get_parent_nic7,
    .physically_open_clk = open_clk_nic7,
    .physically_close_clk = close_clk_nic7,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_nic7()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG15);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_nic7()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_nic7()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG15, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_nic7()
{
    INS_BITFIELD(s_reg_ccm->CCPGR0, CCM_CCPGR0_PPCG15, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node SEMA4
 */
static CLK_NODE_T clk_sema4 = {
    .name = CLK_SEMA4,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_sema4,
    .get_parent = get_parent_sema4,
    .physically_open_clk = open_clk_sema4,
    .physically_close_clk = close_clk_sema4,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_sema4()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR1, CCM_CCPGR1_PPCG13);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_sema4()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_sema4()
{
    INS_BITFIELD(s_reg_ccm->CCPGR1, CCM_CCPGR1_PPCG13, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_sema4()
{
    INS_BITFIELD(s_reg_ccm->CCPGR1, CCM_CCPGR1_PPCG13, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node CA5_DBG
 */
static CLK_NODE_T clk_ca5_dbg = {
    .name = CLK_CA5_DBG,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ca5_dbg,
    .get_parent = get_parent_ca5_dbg,
    .physically_open_clk = open_clk_ca5_dbg,
    .physically_close_clk = close_clk_ca5_dbg,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ca5_dbg()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG8);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ca5_dbg()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_ca5_dbg()
{
    INS_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG8, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ca5_dbg()
{
    INS_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG8, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node CA5_PMU
 */
static CLK_NODE_T clk_ca5_pmu = {
    .name = CLK_CA5_PMU,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ca5_pmu,
    .get_parent = get_parent_ca5_pmu,
    .physically_open_clk = open_clk_ca5_pmu,
    .physically_close_clk = close_clk_ca5_pmu,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ca5_pmu()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG9);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ca5_pmu()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_ca5_pmu()
{
    INS_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG9, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ca5_pmu()
{
    INS_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG9, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node CA5_ETM
 */
static CLK_NODE_T clk_ca5_etm = {
    .name = CLK_CA5_ETM,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ca5_etm,
    .get_parent = get_parent_ca5_etm,
    .physically_open_clk = open_clk_ca5_etm,
    .physically_close_clk = close_clk_ca5_etm,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ca5_etm()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG10);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ca5_etm()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_ca5_etm()
{
    INS_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG10, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ca5_etm()
{
    INS_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG10, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node CA5_ROMTABLE
 */
static CLK_NODE_T clk_ca5_romtable = {
    .name = CLK_CA5_ROMTABLE,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ca5_romtable,
    .get_parent = get_parent_ca5_romtable,
    .physically_open_clk = open_clk_ca5_romtable,
    .physically_close_clk = close_clk_ca5_romtable,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ca5_romtable()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG12);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ca5_romtable()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_ca5_romtable()
{
    INS_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG12, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ca5_romtable()
{
    INS_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG12, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node CA5_CTI
 */
static CLK_NODE_T clk_ca5_cti = {
    .name = CLK_CA5_CTI,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ca5_cti,
    .get_parent = get_parent_ca5_cti,
    .physically_open_clk = open_clk_ca5_cti,
    .physically_close_clk = close_clk_ca5_cti,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ca5_cti()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG14);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ca5_cti()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_ca5_cti()
{
    INS_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG14, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ca5_cti()
{
    INS_BITFIELD(s_reg_ccm->CCPGR2, CCM_CCPGR2_PPCG14, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node CA5_ITM
 */
static CLK_NODE_T clk_ca5_itm = {
    .name = CLK_CA5_ITM,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ca5_itm,
    .get_parent = get_parent_ca5_itm,
    .physically_open_clk = open_clk_ca5_itm,
    .physically_close_clk = close_clk_ca5_itm,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ca5_itm()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG0);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ca5_itm()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_ca5_itm()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG0, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ca5_itm()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG0, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node CA5_ETB
 */
static CLK_NODE_T clk_ca5_etb = {
    .name = CLK_CA5_ETB,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ca5_etb,
    .get_parent = get_parent_ca5_etb,
    .physically_open_clk = open_clk_ca5_etb,
    .physically_close_clk = close_clk_ca5_etb,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ca5_etb()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG1);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ca5_etb()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_ca5_etb()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG1, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ca5_etb()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG1, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node CA5_Funnel
 */
static CLK_NODE_T clk_ca5_funnel = {
    .name = CLK_CA5_FUNNEL,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_ca5_funnel,
    .get_parent = get_parent_ca5_funnel,
    .physically_open_clk = open_clk_ca5_funnel,
    .physically_close_clk = close_clk_ca5_funnel,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_ca5_funnel()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG2);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_ca5_funnel()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_ca5_funnel()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG2, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_ca5_funnel()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG2, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node Pltf_TCTL
 */
static CLK_NODE_T clk_pltf_tctl = {
    .name = CLK_PLTF_TCTL,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pltf_tctl,
    .get_parent = get_parent_pltf_tctl,
    .physically_open_clk = open_clk_pltf_tctl,
    .physically_close_clk = close_clk_pltf_tctl,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_pltf_tctl()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG3);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_pltf_tctl()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_pltf_tctl()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG3, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_pltf_tctl()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG3, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node Pltf_TCTU
 */
static CLK_NODE_T clk_pltf_tctu = {
    .name = CLK_PLTF_TCTU,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pltf_tctu,
    .get_parent = get_parent_pltf_tctu,
    .physically_open_clk = open_clk_pltf_tctu,
    .physically_close_clk = close_clk_pltf_tctu,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_pltf_tctu()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG4);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_pltf_tctu()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_pltf_tctu()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG4, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_pltf_tctu()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG4, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node Pltf_Funnel
 */
static CLK_NODE_T clk_pltf_funnel = {
    .name = CLK_PLTF_FUNNEL,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pltf_funnel,
    .get_parent = get_parent_pltf_funnel,
    .physically_open_clk = open_clk_pltf_funnel,
    .physically_close_clk = close_clk_pltf_funnel,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_pltf_funnel()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG5);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_pltf_funnel()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_pltf_funnel()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG5, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_pltf_funnel()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG5, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Node Pltf_SWO
 */
static CLK_NODE_T clk_pltf_swo = {
    .name = CLK_PLTF_SWO,
    .parent = NULL,
    .child = NULL,
    .sibling = NULL,
    .active = 0,
    .is_always_open = 0,
    .freq = 0,
    .freq_valid = 0,
    .is_physically_open = is_physically_open_pltf_swo,
    .get_parent = get_parent_pltf_swo,
    .physically_open_clk = open_clk_pltf_swo,
    .physically_close_clk = close_clk_pltf_swo,
    .calc_freq = calc_freq_general,
    .set_freq = set_freq_na,
};

uint8_t is_physically_open_pltf_swo()
{
    uint8_t cg = EXTRACT_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG6);
    return (cg == CG_OFF_ALL) ? 0 : 1;
}

P_CLK_NODE_T get_parent_pltf_swo()
{
    return &clk_ips_ipg;
}

uint8_t open_clk_pltf_swo()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG6, CG_ON_ALL_EXCEPT_STOP);
    return 1;
}

uint8_t close_clk_pltf_swo()
{
    INS_BITFIELD(s_reg_ccm->CCPGR3, CCM_CCPGR3_PPCG6, CG_OFF_ALL);
    return 1;
}

/*
 * Clock Array
 */
const P_CLK_NODE_T clk_table[] = {
    &clk_fxosc,
    &clk_firc,
    &clk_sxosc,
    &clk_sirc,
    &clk_fxosc_div,
    &clk_sirc_div,
    &clk_fast,
    &clk_slow,
    &clk_clk_24m_irc,
    &clk_lprc,
    &clk_lfsr,
    &clk_pll1,
    &clk_pll1_pfd1,
    &clk_pll1_pfd2,
    &clk_pll1_pfd3,
    &clk_pll1_pfd4,
    &clk_pll1_pfd,
    &clk_pll2,
    &clk_pll2_pfd1,
    &clk_pll2_pfd2,
    &clk_pll2_pfd3,
    &clk_pll2_pfd4,
    &clk_pll2_pfd,
    &clk_pll3,
    &clk_pll3_pfd1,
    &clk_pll3_pfd2,
    &clk_pll3_pfd3,
    &clk_pll3_pfd4,
    &clk_pll3_div,
    &clk_pll4,
    &clk_pll4_div,
    &clk_pll5,
    &clk_pll5_div,
    &clk_pll6,
    &clk_pll6_div,
    &clk_pll7,
    &clk_sys,
    &clk_ca5_core,
    &clk_platform_bus_cm4_core,
    &clk_flexbus,
    &clk_ips_ipg,
    &clk_dramc,
    &clk_audio_ext,
    &clk_flexcan0,
    &clk_flexcan1,
    &clk_ftm0_ext,
    &clk_ftm1_ext,
    &clk_ftm2_ext,
    &clk_ftm3_ext,
    &clk_ftm0_fix,
    &clk_ftm1_fix,
    &clk_ftm2_fix,
    &clk_ftm3_fix,
    &clk_nfc,
    &clk_qspi0,
    &clk_qspi1,
    &clk_enet_ext,
    &clk_enet_ts_ext,
    &clk_enet_rmii,
    &clk_enet_ts,
    &clk_esdhc0,
    &clk_esdhc1,
    &clk_dcu_pixel0,
    &clk_dcu_pixel1,
    &clk_dcu0,
    &clk_dcu1,
    &clk_spdif_ext,
    &clk_mlb_ext,
    &clk_esai_ext,
    &clk_sai0_ext,
    &clk_sai3_ext,
    &clk_esai,
    &clk_sai0,
    &clk_sai1,
    &clk_sai2,
    &clk_sai3,
    &clk_video_adc,
    &clk_gpu,
    &clk_swo,
    &clk_trace,
    &clk_usb0,
    &clk_usb1,
    &clk_spdif,
    &clk_dap,
    &clk_dma_mux0,
    &clk_dma_mux1,
    &clk_dma_mux2,
    &clk_dma_mux3,
    &clk_dma0_tcd,
    &clk_dma1_tcd,
    &clk_dma0,
    &clk_dma1,
    &clk_uart0,
    &clk_uart1,
    &clk_uart2,
    &clk_uart3,
    &clk_uart4,
    &clk_uart5,
    &clk_spi0,
    &clk_spi1,
    &clk_spi2,
    &clk_spi3,
    &clk_crc,
    &clk_usbc0,
    &clk_usbc1,
    &clk_pdb,
    &clk_pit,
    &clk_ftm0,
    &clk_ftm1,
    &clk_ftm2,
    &clk_ftm3,
    &clk_adc0,
    &clk_adc1,
    &clk_tcon0,
    &clk_tcon1,
    &clk_wdog_a5,
    &clk_wdog_m4,
    &clk_lptmr,
    &clk_rle,
    &clk_iomuxc,
    &clk_porta_mult,
    &clk_portb_mult,
    &clk_portc_mult,
    &clk_portd_mult,
    &clk_porte_mult,
    &clk_anadig,
    &clk_scsc,
    &clk_asrc,
    &clk_ewm,
    &clk_i2c0,
    &clk_i2c1,
    &clk_i2c2,
    &clk_i2c3,
    &clk_wkup,
    &clk_ccm,
    &clk_gpc,
    &clk_vreg_dig,
    &clk_cmu,
    &clk_otp,
    &clk_slcd,
    &clk_video_decoder,
    &clk_viu,
    &clk_dac0,
    &clk_dac1,
    &clk_enet_mac_ieee1588_timer0,
    &clk_enet_mac_ieee1588_timer1,
    &clk_enet_l2_sw,
    &clk_mscm,
    &clk_ca5_scu_gic,
    &clk_ca5_intd,
    &clk_ca5_l2c,
    &clk_nic0,
    &clk_nic1,
    &clk_nic2,
    &clk_nic3,
    &clk_nic4,
    &clk_nic5,
    &clk_nic6,
    &clk_nic7,
    &clk_sema4,
    &clk_ca5_dbg,
    &clk_ca5_pmu,
    &clk_ca5_etm,
    &clk_ca5_romtable,
    &clk_ca5_cti,
    &clk_ca5_itm,
    &clk_ca5_etb,
    &clk_ca5_funnel,
    &clk_pltf_tctl,
    &clk_pltf_tctu,
    &clk_pltf_funnel,
    &clk_pltf_swo,
};

int get_nodes_nr()
{
    return sizeof(clk_table) / sizeof(P_CLK_NODE_T);
}

/*
 * common function
 */
uint8_t open_clk(P_CLK_NODE_T p_clk)
{
    p_clk->physically_open_clk();
    open_clk_postproc(p_clk);
    return 1;
}

uint8_t close_clk(P_CLK_NODE_T p_clk)
{
    close_clk_preproc(p_clk);
    p_clk->physically_close_clk();
    return 1;
}


void open_clk_postproc(P_CLK_NODE_T p_clk)
{
    p_clk->active = 1;
    inc_descendant(p_clk->parent);
}

void close_clk_preproc(P_CLK_NODE_T p_clk)
{
    // dec_parent(p_clk, 1);
    dec_descendant(p_clk->parent);
    p_clk->active = 0;
}

P_CLK_NODE_T set_clk_parent(P_CLK_NODE_T p_clk, P_CLK_NODE_T p_parent)
{
    uint8_t original_need_dec = 0;
    uint8_t new_need_inc = 0;
    uint8_t result;
    uint32_t old_freq, new_freq;
    P_CLK_NODE_T old_parent = NULL;

    if (p_clk->check_parent == NULL) {
        printf("Node <%s> can't change parent\n", clk_name_str(p_clk->name));
        return NULL;
    }
    result = p_clk->check_parent(p_parent, &original_need_dec, &new_need_inc);
    if (result == CHECK_PARENT_INVALID) {
        printf("unable to set <%s>'s parent to <%s>\n", clk_name_str(p_clk->name), clk_name_str(p_parent->name));
        return NULL;
    } else if (result == CHECK_PARENT_IDENTICAL) {
        printf("same parent <%s> for <%s>\n", clk_name_str(p_parent->name), clk_name_str(p_clk->name));
        return NULL;
    } else if (result != CHECK_PARENT_PASS) {
        printf("something wrong in %s, please check\n", __FUNCTION__);
        return NULL;
    }

    printf("set <%s>'s parent to <%s>\n", clk_name_str(p_clk->name), clk_name_str(p_parent->name));
    old_parent = p_clk->parent;

    /*
     * Need to update the old and new parent's child list
     */
    clk_remove_child(old_parent, p_clk);
    clk_add_child(p_parent, p_clk);

    /*
     * For glitch-free switch, we must follow the following procedure
     *  1. Prepare the new parent
     *  2. Switch the path
     *  3. Process the old parent
     */
    // 1. prepare new parent
    if (new_need_inc)
        inc_descendant(p_parent);

    // 2. Switch clock path
    p_clk->physically_set_parent(p_parent);

    p_clk->parent = p_parent;
    p_clk->active = p_clk->is_physically_open();
    // We need to pay special attention to the "active" change
    // Case 1, there is a common cg
    // Case 2, there is no cg at p_clk level
    // Case 3, there is different cg on different path (not met yet)

    // 3. process old parent
    if (original_need_dec)
        dec_descendant(old_parent);

    // 4. Freq change
    old_freq = old_parent->freq;
    new_freq = p_parent->freq;
    p_clk->freq = ((uint64_t)p_clk->freq * new_freq + old_freq / 2) / old_freq;
    update_child_freq(p_clk, old_freq, new_freq);

    // 5. Return old_parent
    return old_parent;
}

void update_child_freq(P_CLK_NODE_T p_clk, uint8_t old_val, uint8_t new_val)
{
    P_CLK_NODE_T child = p_clk->child;
    P_CLK_NODE_T p = NULL;

    if (child != NULL) {
        for (p = child; p != NULL; p = p->sibling) {
            p->freq = ((uint64_t)p->freq * new_val + old_val / 2) / old_val;
            update_child_freq(p, old_val, new_val);
        }
    }
}


/*
 * Tool functions
 */
// uint8_t is_clk_physically_open(P_CLK_NODE_T p_clk)
// {
//  // here we do a vicious circle
//  // in real world, we should check register interface to see
//  // if a clk is really open, if that is true, this function will
//  // return true, and p_clk->active will be set to 1
//  // but in this simulation, we initialize p_clk->is open to indicate
//  // hardware on/off
//  //
//  // Real World
//  //     Register value ---> p_clk->active;
//  // Simulation
//  //     p-clk->active ---> p_clk->active;
//  // printf("check status of %s\n", p_clk->name);
//  return p_clk->is_physically_open ? 1 : 0;
// }

void clk_add_child(P_CLK_NODE_T parent, P_CLK_NODE_T child)
{
    P_CLK_NODE_T sibling, prev_node;
    if (parent != NULL) {
        if (parent->child == NULL)
            parent->child = child;
        else {
            prev_node = parent->child;
            sibling = prev_node->sibling;
            while (sibling != NULL) {
                prev_node = sibling;
                sibling = sibling->sibling;
            }
            prev_node->sibling = child;
        }
    }
}

void clk_remove_child(P_CLK_NODE_T parent, P_CLK_NODE_T child)
{
    P_CLK_NODE_T sibling, prev_node;
    if (parent != NULL) {
        if (parent->child == child) {
            parent->child = child->sibling;
            child->sibling = NULL;
        } else {
            prev_node = parent->child;
            sibling = parent->child->sibling;
            while (sibling != NULL) {
                if (sibling == child)
                    break;
                prev_node = sibling;
                sibling = prev_node->sibling;
            }
            if (sibling != child)
                printf("something wrong when doing remove clk node\n");
            prev_node->sibling = child->sibling;
            child->sibling = NULL;
        }
    }
}

void inc_descendant(P_CLK_NODE_T p_clk)
{
    if (p_clk != NULL) {
        p_clk->descendant_cnt++;
        if (!p_clk->active) {
            printf("%s : open clock passively\n", clk_name_str(p_clk->name));
            open_clk(p_clk);
        }
    }
}

void dec_descendant(P_CLK_NODE_T p_clk)
{
    if (p_clk != NULL) {
        p_clk->descendant_cnt--;
        if ((p_clk->enable_cnt == 0) && (p_clk->descendant_cnt == 0) && (!p_clk->is_always_open)) {
            printf("%s : close clock passively\n", clk_name_str(p_clk->name));
            close_clk(p_clk);
        }
    }
}

P_CLK_NODE_T find_clk_node(CLOCK_NAME clk_name)
{
    P_CLK_NODE_T entry;
    int i;
    int node_nr = get_nodes_nr();

    for (i=0; i!=node_nr; i++) {
        entry = clk_table[i];
        if (clk_name == entry->name) {
            return entry;
        }
    }

    return NULL;
}

P_CLK_NODE_T get_clk_entry(int i)
{
    if ((i < 0) || (i > get_nodes_nr())) {
        printf("error : can't get clock entry %d\n", i);
        return NULL;
    }

    return clk_table[i];
}

void init_node_freq(P_CLK_NODE_T p_clk)
{
    P_CLK_NODE_T parent = NULL;
    uint32_t parent_freq = 0;
    if (p_clk->freq_valid)
        return;
    else {
        parent = p_clk->parent;
        if (parent != NULL) {
            // get freq of parent node
            if (!parent->freq_valid) {
                init_node_freq(parent);
            }
            parent_freq = parent->freq;
        }
        p_clk->calc_freq(p_clk, parent_freq);
        p_clk->freq_valid = 1;
    }
}
