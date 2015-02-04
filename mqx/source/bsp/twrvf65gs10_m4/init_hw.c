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
*   This file contains flash boot code to initialize chip selects,
*   disable the watchdog timer and initialize the PLL.
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "bsp_prv.h"

/* clock initialization */
static void clocks_init(void)
{
    // NOTE: default value of CCM_CCGRx registers are different between vybrid v1.1 and v1.0
    // enable default clocks - after reset
    CCM_CCGR4 |= CCM_CCGR4_CG11(0x3);   // enable clock to CCM
    CCM_CCGR3 |= CCM_CCGR3_CG0(0x3);    // enable clock to ANADIG
    CCM_CCGR2 |= CCM_CCGR2_CG8(0x3);    // IOMUX
    CCM_CCGR4 |= CCM_CCGR4_CG12(0x3);   // GPC
    CCM_CCGR4 |= CCM_CCGR4_CG13(0x3);   // VREG
    CCM_CCGR6 |= CCM_CCGR6_CG5(0x3);    // OTP
    CCM_CCGR3 |= CCM_CCGR3_CG2(0x3);    // SCSCM

    /* we use PLL2_PFD2 for DDR clock, check it and prevent double initialization of clock
     * protection for DDR targets - prevent data lost from DDR, when we change clock settings
     */
    if (CCM_CCSR & CCM_CCSR_PLL2_PFD2_EN_MASK)
    {
        return;
    }

    // set default value
    // enable FIRC, enable FXOSC_EN, OSCNT=57
    CCM_CCR = 0x00011057; 

    // power up FXOSC - set FXOSC_PWRDWN to 0, 
    // SBYOS=1, ARM_CLK_LPM=1 - disable clocks on wait mode
    // write default value to reserved area
    CCM_CLPCR = 0x00000078;

    // wait for fxosc ready
    while(!(CCM_CSR & 0x20));

    // select fxosc as source of FAST_CLK_SEL
    // select FAST_CLK_SEL as source of SYS_CLK_SEL
    CCM_CCSR = 0x00000020;

    // enable PLLs in ANADIG
    // expect default value of CLK_24M_IRC_XTAL_SEL=0 
    Anadig_PLL1_CTRL = 0x00002001;  // f * 22 = 528MHz
    Anadig_PLL2_CTRL = 0x00002001;  // f * 22 = 528MHz
    Anadig_PLL4_CTRL = 0x00002031;  // PLL4 -> 1179.648MHz
    Anadig_PLL5_CTRL = 0x00002001;  // PLL5
    Anadig_PLL6_CTRL = 0x00002028;  // PLL6
    Anadig_PLL3_CTRL = 0x00003040;  // PLL3
    Anadig_PLL7_CTRL = 0x00003040;  // PLL7

    #define PLL_LOCKS (\
        Anadig_PLL_LOCK_PLL_528_SYS_LOCK_MASK | \
        Anadig_PLL_LOCK_PLL_528_LOCK_MASK     | \
        Anadig_PLL_LOCK_PLL_AUDIO_LOCK_MASK   | \
        Anadig_PLL_LOCK_PLL_ENET_LOCK_MASK    | \
        Anadig_PLL_LOCK_PLL_VIDEO_LOCK_MASK   | \
        Anadig_PLL_LOCK_PLL_USB1_LOCK_MASK    | \
        Anadig_PLL_LOCK_PLL_USB0_LOCK_MASK)

    // wait to lock pll
    while ((Anadig_PLL_LOCK & PLL_LOCKS) != PLL_LOCKS);


    // ARM_CLK: SYS_CLK/1 = 396Mhz
    // BUS_CLK: ARM_CLK/3 = 132Mhz
    // IGP_CLK: BUS_CLK/2 = 66Mhz
    // AUDIO_DIV:  73Mhz ??
    CCM_CACRR = CCM_CACRR_ARM_CLK_DIV(0) | CCM_CACRR_BUS_CLK_DIV(2) | CCM_CACRR_IPG_CLK_DIV(1) | CCM_CACRR_PLL4_CLK_DIV(7);

    // disable all PLL3_PFD
    // disable DAP ??
    // select PLL2 main clk as source of PLL2_PFD_CLK_SEL
    // select PLL1_PFD3 as source of PLL1_PDF_CLK_SEL
    // enable all PLL1_PFD
    // enable all PLL2_PFD2
    // select PLL2_PFD2 as source of DDR DDR_CLK_SEL
    // select fxosc as source of FAST_CLK_SEL
    // TODO: check SLOW_CLK_SEL
    // select PLL1_PFD_CLK_SEL as source of SYS_CLK_SEL
    CCM_CCSR = 0x0003FF24;

    // enable all clock from CCM
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

static void ddr_iomuxc(void)
{
    IOMUXC_MemMapPtr iomux = IOMUXC_BASE_PTR;
    uint32_t volatile *p;

    for (p = &(iomux->DDR_RESETB); &(iomux->DUMMY_DDRBYTE2) >= p; p++) {
        if (p == &(iomux->DDR_CLK_0) || p == &(iomux->DDR_DQS_1) || p == &(iomux->DDR_DQS_0)) {
            *p = IOMUXC_SW_MUX_CTL_PAD_DDR_INPUT(1) | IOMUXC_SW_MUX_CTL_PAD_DDR_TRIM(0) | IOMUXC_SW_MUX_CTL_PAD_DDR_DSE(5);
        }
        else {
            *p = IOMUXC_SW_MUX_CTL_PAD_DDR_INPUT(0) | IOMUXC_SW_MUX_CTL_PAD_DDR_TRIM(0) | IOMUXC_SW_MUX_CTL_PAD_DDR_DSE(5);
        }
    }
}

#define PHY_DQ_TIMING       0x00002613
#define PHY_DQS_TIMING      0x00002615
#define PHY_CTRL            0x01210080
#define PHY_MASTER_CTRL     0x0001012a
#define PHY_SLAVE_CTRL      0x00012020

static void ddr_phy_init2(void)
{
    DDRMC_MemMapPtr ddrmc = DDRMC_BASE_PTR;

    // phy_dq_timing_reg freq set 0
    ddrmc->PHY_DQ_TR0 = PHY_DQ_TIMING;
    ddrmc->PHY_DQ_TR1 = PHY_DQ_TIMING;
    ddrmc->PHY_DQ_TR2 = PHY_DQ_TIMING;
    ddrmc->PHY48 = PHY_DQ_TIMING;

    // phy_dqs_timing_reg freq set 0
    ddrmc->PHY_DQS_TR0 = PHY_DQS_TIMING;
    ddrmc->PHY_DQS_TR1 = PHY_DQS_TIMING;
    ddrmc->PHY_DQS_TR2 = PHY_DQS_TIMING;
    ddrmc->PHY_DLL_SLAVE_CTL_LPDDR2_REG = PHY_DQS_TIMING;

    // phy_gate_lpbk_ctrl_reg freq set 0
    ddrmc->PHY_GATE_CR0 = PHY_CTRL;	// read delay bit21:19
    ddrmc->PHY_GATE_CR1 = PHY_CTRL;	// phase_detect_sel bit18:16
    ddrmc->PHY_GATE_CR2 = PHY_CTRL;	// bit lpbk_ctrl bit12
    ddrmc->PHY_GB_CR = PHY_CTRL;

    // phy_dll_master_ctrl_reg freq set 0
    ddrmc->PHY_DLL_MASTER_CR0 = PHY_MASTER_CTRL;
    ddrmc->PHY_DLL_MASTER_CR1 = PHY_MASTER_CTRL;
    ddrmc->PHY_DLL_MASTER_CR2 = PHY_MASTER_CTRL;
    ddrmc->PHY51 = PHY_MASTER_CTRL;

    // phy_dll_slave_ctrl_reg freq set 0
    ddrmc->PHY_DLL_SLAVE_CR0 = PHY_SLAVE_CTRL;
    ddrmc->PHY_DLL_SLAVE_CR1 = PHY_SLAVE_CTRL;
    ddrmc->PHY_DLL_SLAVE_CR2 = PHY_SLAVE_CTRL;
    ddrmc->PHY52 = PHY_SLAVE_CTRL;

    ddrmc->PHY_GB_CR = 0x00001100;
}

// DRAM device Mode registers
#define MR_BL_FIXED8    0x0000 // Fixed 8
#define MR_BL_4_8_A12   0x0001 // 4 or 8 on A12
#define MR_BL_FIXED4    0x0002 // Fixed 4
#define MR_CAS_5        0x0010 // CAS 5
#define MR_CAS_6        0x0020 // CAS 6
#define MR_CAS_7        0x0030 // CAS 7
#define MR_CAS_8        0x0040 // CAS 8
#define MR_CAS_9        0x0050 // CAS 9
#define MR_CAS_10       0x0060 // CAS 10
#define MR_CAS_11       0x0070 // CAS 11
#define MR_CAS_12       0x0004 // CAS 12
#define MR_CAS_13       0x0014 // CAS 13
#define MR_WR_5         0x0200 // Write Recovery 5
#define MR_WR_6         0x0400 // Write Recovery 6
#define MR_WR_7         0x0600 // Write Recovery 7
#define MR_WR_8         0x0800 // Write Recovery 8
#define MR_WR_10        0x0A00 // Write Recovery 10
#define MR_WR_12        0x0C00 // Write Recovery 12
#define MR_WR_14        0x0E00 // Write Recovery 14
#define MR              (MR_BL_FIXED8 | MR_CAS_6 | MR_WR_6)

#define MR1_ODS_RZQ_6   0x0000 // drvstrength 40 ohm
#define MR1_ODS_RZQ_7   0x0002 // drvstrength 34 ohm
#define MR1_AL_DIS      0x0000
#define MR1_AL_1        0x0008 // CL - 1
#define MR1_AL_2        0x0010 // CL - 2
#define MR1_ODT_DIS     0x0000
#define MR1_ODT_RZQ_4   0x0004 // RZQ/4 60 ohm
#define MR1_ODT_RZQ_2   0x0040 // RZQ/2 120 ohm
#define MR1_ODT_RZQ_6   0x0044 // RZQ/6 40 ohm
#define MR1_ODT_RZQ_12  0x0200 // RZQ/12 20 ohm
#define MR1_ODT_RZQ_8   0x0204 // RZQ/8 30 ohm
#define MR1_WL          0x0080 // Write Leveling
#define MR1_QOFF        0x0400 // Q off
#define MR1             (MR1_ODS_RZQ_7 | MR1_AL_DIS | MR1_ODT_RZQ_6)

#define MR2_CWL_5CK     0x0000 // 5ck (tCK >= 2.5 ns)
#define MR2_CWL_6CK     0x0008 // 6ck (2.5 ns > tCK >= 1.875 ns)
#define MR2_CWL_7CK     0x0010 // 7ck (1.875 ns > tCK >= 1.5 ns)
#define MR2_CWL_8CK     0x0018 // 8ck (1.5 ns > tCK >= 1.25 ns)
#define MR2_CWL_9CK     0x0020 // 9ck (1.25 ns > tCK >= 1.07 ns)
#define MR2_DODT_DIS    0x0000
#define MR2_DODT_RZQ_4  0x0200 // RZQ / 4
#define MR2_DODT_RZQ_2  0x0400 // RZQ / 2
#define MR2             (MR2_CWL_5CK | MR2_DODT_DIS)

#define WRADJ           0x00000500
#define RDADJ           0x00000006
#define ADJLAT          (WRADJ | RDADJ)

static void ddr_init(void)
{
    DDRMC_MemMapPtr ddrmc = DDRMC_BASE_PTR;

    // enable clock gate to DDRCM
    CCM_CCGR6 |= CCM_CCGR6_CG14(0x3); // DDRM

    if (ddrmc->CR00 & DDRMC_CR00_START_MASK)
    {
        // DDRC was initialized
        return;
    }

    // io pad init
    ddr_iomuxc();

    // Dram Device Parameters
    ddrmc->CR00 = 0x00000600;   // LPDDR2 or DDR3
    ddrmc->CR02 = 0x00000020;   // TINIT F0
                                // cold boot - 1ms??? 0x61a80
    ddrmc->CR10 = 0x0000007c;   // reset during power on
                                // warm boot - 200ns
    ddrmc->CR11 = 0x00013880;   // 500us - 10ns
    ddrmc->CR12 = 0x0000050c;   // CASLAT_LIN, WRLAT
    ddrmc->CR13 = 0x15040404;   // trc, trrd, tccd, tbst_int_interval
    ddrmc->CR14 = 0x1406040F;   // tfaw, trp, twtr, tras_min
    ddrmc->CR16 = 0x04040000;   // tmrd, trtp
    ddrmc->CR17 = 0x006db00c;   // tras_max, tmod
    ddrmc->CR18 = 0x00000403;   // tckesr, tcke

    ddrmc->CR20 = 0x01000403;   // ap, writeinterp, tckesr_f1, tcke_f1
    ddrmc->CR21 = 0x06060101;   // twr_int, trcd_int, tras_lockout, ccAP
    ddrmc->CR22 = 0x000b0000;   // tdal
    ddrmc->CR23 = 0x03000200;   // bstlen, tmrr - lpddr2, tdll
    ddrmc->CR24 = 0x00000006;   // addr_mirror, reg_dimm, trp_ab
    ddrmc->CR25 = 0x00010000;   // tref_enable, auto_refresh, arefresh
    ddrmc->CR26 = 0x0c28002c;   // tref, trfc
    ddrmc->CR28 = 0x00000005;   // tref_interval fixed at 5
    ddrmc->CR29 = 0x00000003;   // tpdex_f0

    ddrmc->CR30 = 0x0000000a;   // txpdll
    ddrmc->CR31 = 0x00440200;   // txsnr, txsr
    ddrmc->CR33 = 0x00010000;   // cke_dly, en_quick_srefresh,
                                // srefresh_exit_no_refresh,
                                // pwr, srefresh_exit
    ddrmc->CR34 = 0x00050500;   // cksrx_f0,

    // frequency change
    ddrmc->CR38 = 0x00000100;   // freq change...
    ddrmc->CR39 = 0x04001002;   // PHY_INI: com, sta, freq_ch_dll_off
    ddrmc->CR41 = 0x00000001;   // 15.02 - allow dfi_init_start
    ddrmc->CR45 = 0x00000000;   // wrmd
    ddrmc->CR46 = 0x00000000;   // rmd
    ddrmc->CR47 = 0x00000000;   // REF_PER_AUTO_TEMPCHK: LPDDR2 set to 2, else 0

    // DRAM device Mode registers
    ddrmc->CR48 = ((MR1 << 16) | MR);
                                // mr0, ddr3 burst of 8 only
                                // mr1, if freq < 125, dll_dis = 1, rtt = 0
                                // if freq > 125, dll_dis = 0, rtt = 3
    ddrmc->CR49 = MR2;          // mr0_f1_0 & mr2_f0_0

    ddrmc->CR51 = 0x00000000;   // mr3 & mrsingle_data
    ddrmc->CR52 = 0x00000000;   // mr17, mr16

    // ECC
    ddrmc->CR57 = 0x00000000;   // ctrl_raw
                                // ctrl_raw, if DDR3, set 3, else 0
    ddrmc->CR58 = 0x00000000;

    // ZQ stuff
    ddrmc->CR66 = 0x01000200;   // zqcl, zqinit
    ddrmc->CR67 = 0x02000040;   // zqcs
    ddrmc->CR69 = 0x00000200;   // zq_on_sref_exit, qz_req

    ddrmc->CR70 = 0x00000040;   // ref_per_zq
    ddrmc->CR71 = 0x00000000;   // zqreset, ddr3 set to 0
    ddrmc->CR72 = 0x01000000;   // zqcs_rotate, no_zq_init, zqreset_f1

    // DRAM controller misc
    ddrmc->CR73 = 0x0a010300;   // arebit, col_diff, row_diff, bank_diff
    ddrmc->CR74 = 0x0101ffff;   // bank_split, addr_cmp_en, cmd/age cnt
    ddrmc->CR75 = 0x01010101;   // rw same pg, rw same en, pri en, plen
    ddrmc->CR76 = 0x03030101;   // #q_entries_act_dis, (#cmdqueues
                                // dis_rw_grp_w_bnk_conflict
                                // w2r_split_en, cs_same_en
    ddrmc->CR77 = 0x01000101;   // cs_map, inhibit_dram_cmd, dis_interleave, swen //by devendra
    ddrmc->CR78 = 0x0000000c;   // qfull, lpddr2_s4, reduc, burst_on_fly
    ddrmc->CR79 = 0x01000000;   // ctrlupd_req_per aref en, ctrlupd_req
                                // ctrller busy, in_ord_accept

    // ODT
    ddrmc->CR87 = 0x01010000;   // odt: wr_map_cs, rd_map_cs, port_data_err_id
    ddrmc->CR88 = 0x00040000;   // todtl_2cmd
                                // todtl_2cmd = odtl_off = CWL + AL - 2ck
    ddrmc->CR89 = 0x00000002;   // add_odt stuff
    ddrmc->CR91 = 0x00020000;
    ddrmc->CR92 = 0x00000000;   // tdqsck_min, _max, w2w_smcsdl

    ddrmc->CR96 = 0x00002819;   // wlmrd, wldqsen

    ddrmc->CR105 = 0x00202000;
    ddrmc->CR106 = 0x20200000;
    ddrmc->CR110 = 0x00002020;
    ddrmc->CR114 = 0x00202000;
    ddrmc->CR115 = 0x20200000;

    // AXI ports
    ddrmc->CR117 = 0x00000101;  // FIFO type (0-async, 1-2:1, 2-1:2, 3- sync, w_pri, r_pri
    ddrmc->CR118 = 0x01010000;  // w_pri, rpri, en
    ddrmc->CR119 = 0x00000000;  // fifo_type

    ddrmc->CR120 = 0x02020000;
    ddrmc->CR121 = 0x00000202;  // round robin port ordering
    ddrmc->CR122 = 0x01010064;
    ddrmc->CR123 = 0x00010101;
    ddrmc->CR124 = 0x00000064;

    // TDFI
    ddrmc->CR125 = 0x00000000;  // dll_rst_adj_dly, dll_rst_delay
    ddrmc->CR126 = 0x00000b00;  // phy_rdlat
    ddrmc->CR127 = 0x00000000;  // dram_ck_dis
    ddrmc->CR131 = 0x00000000;  // tdfi_ctrlupd_interval_f0

    ddrmc->CR132 = ADJLAT;      // wrlat, rdlat 15.02
    ddrmc->CR137 = 0x00020000;  // Phyctl_dl
    ddrmc->CR139 = 0x04070303;

    ddrmc->CR136 = 0x00000000;  // tdfi_ctrlupd_interval_f1

    ddrmc->CR154 = 0x68200000;  // pad_zq: _early_cmp_en_timer, _mode, _hw_for, _cmp_out_smp
    ddrmc->CR155 = 0x00000202;  // pad: _ibe1, ibe0, pad_ibe: _sel1, _sel0,
                                // axi_awcache, axi_cobuf, pad_odt: bate0, byte1
    ddrmc->CR158 = 0x00000006;  // twr
    ddrmc->CR159 = 0x00000006;  // todth

    ddr_phy_init2();

    ddrmc->CR82 = 0x1fffffff;
    ddrmc->CR00 = 0x00000601;   // LPDDR2 or DDR3, start

    {
        int i = 5000;
        while (i--);
    }
}

void _bsp_aux_core_clock(_mqx_uint enable)
{
    if (enable)
    {
        CCM_CCOWR = CCM_CCOWR_AUX_CORE_WKUP_MASK | 0x5a5a;
    }
    else
    {
        CCM_CCOWR = 0x5a5a;
    }
}

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : init_hardware
* Returned Value   : void
* Comments         :
*   Initialize device.
*
*END*-------------------------------------------------------------------------*/
void init_hardware(void)
{
    clocks_init();
    ddr_init();
    _bsp_gpio_io_init();
}

