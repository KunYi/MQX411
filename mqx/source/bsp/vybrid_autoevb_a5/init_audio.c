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
*   This file contains the bsp related codes for audio
*
*
*END************************************************************************/

#include "mqx_inc.h"
#include "bsp.h"
#include "bsp_prv.h"
#include "io_rev.h"
#include "bsp_rev.h"

#include "vybrid.h"

#include <esai_vport.h>

#define ASRC_PLL4_DIV_CLK  147456000
#define AUDIO_SYSTEM_CLK      24576000
#define ESAI_CLOCK_PLL4 0


/*#define BSP_ESAI_DEBUG*/

/*![ESAI driver FIFO layer init data]*/
const VYBRID_ESAI_FIFO_INIT_STRUCT _bsp_esai_init =
{
    0,   /*device id*/
    0x60,  /*tx fifo watermark*/
    0x40,  /*rx fifo watermark*/
    32, /*tx default slot width*/
    32, /*rx default slot width*/
    1536, /*tx dma packet size*/
    8,  /*tx dma packet num*/
    1536, /*rx dma packet size*/
    8,   /*rx dma packet num*/
    TRUE, /*tx clock master*/
    TRUE, /*rx clock master*/
    TRUE  /*Synchronous operating mode*/
};
/*![ESAI driver FIFO layer init data]*/

/*![ESAI VPORT TX0 init data]*/
const VYBRID_ESAI_VPORT_INIT_STRUCT _bsp_esai_vport0_tx_init =
{
    0,   /*transceiver id*/
    VPORT_DIR_TX,  /*transceiver direction*/
};
/*![ESAI VPORT TX0 init data]*/

const VYBRID_ESAI_VPORT_INIT_STRUCT _bsp_esai_vport1_tx_init =
{
    1,   /*transceiver id*/
    VPORT_DIR_TX,  /*transceiver direction*/
};

const VYBRID_ESAI_VPORT_INIT_STRUCT _bsp_esai_vport2_tx_init =
{
    2,   /*transceiver id*/
    VPORT_DIR_TX,  /*transceiver direction*/
};

const VYBRID_ESAI_VPORT_INIT_STRUCT _bsp_esai_vport2_rx_init =
{
    2,   /*transceiver id*/
    VPORT_DIR_RX,  /*transceiver direction*/
};


const VYBRID_ESAI_VPORT_INIT_STRUCT _bsp_esai_vport3_tx_init =
{
    3,   /*transceiver id*/
    VPORT_DIR_TX,  /*transceiver direction*/
};

const VYBRID_ESAI_VPORT_INIT_STRUCT _bsp_esai_vport3_rx_init =
{
    3,   /*transceiver id*/
    VPORT_DIR_RX,  /*transceiver direction*/
};

const VYBRID_ESAI_VPORT_INIT_STRUCT _bsp_esai_vport4_tx_init =
{
    4,   /*transceiver id*/
    VPORT_DIR_TX,  /*transceiver direction*/
};


const VYBRID_ESAI_VPORT_INIT_STRUCT _bsp_esai_vport4_rx_init =
{
    4,   /*transceiver id*/
    VPORT_DIR_RX,  /*transceiver direction*/
};

const VYBRID_ESAI_VPORT_INIT_STRUCT _bsp_esai_vport5_tx_init =
{
    5,   /*transceiver id*/
    VPORT_DIR_TX,  /*transceiver direction*/
};


const VYBRID_ESAI_VPORT_INIT_STRUCT _bsp_esai_vport5_rx_init =
{
    5,   /*transceiver id*/
    VPORT_DIR_RX,  /*transceiver direction*/
};


const VYBRID_ASRC_INIT_STRUCT _bsp_asrc_init =
{
    0,
    2,
    2,
    2,
    32,
    32
};


void _bsp_pll4_init(void)
{
    uint32_t regVal = 0;

    /*
         * enable the pll4 output, the default anadig pll audio output
         * PLL Output = 24M*49.152=1179.648M
         */

    /*set pll4 divider to 8, the clock is 147.456*/
    regVal = CCM_CACRR;
    regVal &= ~(CCM_CACRR_PLL4_CLK_DIV_MASK);
    regVal |= ((0x00000003) << CCM_CACRR_PLL4_CLK_DIV_SHIFT);
    CCM_CACRR = regVal;
}

static void _bsp_esai_clocks_init(void)
{
    uint32_t regVal = 0;

    _bsp_pll4_init();

    /*gate the ESAI clocks*/
    regVal = CCM_CCGR4;
    regVal &= ~(CCM_CCGR4_CG2_MASK | CCM_CCGR4_CG3_MASK);
    CCM_CCGR4 = regVal;

#if ESAI_CLOCK_PLL4
    /*select pll as the mux input ESAI*/
    regVal = CCM_CSCMR1;
    regVal &= ~(CCM_CSCMR1_ESAI_CLK_SEL_MASK);
    regVal |= ((0x00000003) << CCM_CSCMR1_ESAI_CLK_SEL_SHIFT);
    CCM_CSCMR1 = regVal;

    /*set the ESAI CCM divider to 6, so that the ESAI serial clock is 24.576Mhz*/
    regVal = CCM_CSCDR2;
    regVal &= ~(CCM_CSCDR2_ESAI_DIV_MASK);
    regVal |= ((0x00000005) << CCM_CSCDR2_ESAI_DIV_SHIFT);
    regVal |= CCM_CSCDR2_ESAI_EN_MASK;
    CCM_CSCDR2 = regVal;
#else
    regVal = CCM_CSCMR1;
    regVal &= ~(CCM_CSCMR1_ESAI_CLK_SEL_MASK);
    regVal |= ((0x00000000) << CCM_CSCMR1_ESAI_CLK_SEL_SHIFT);
    CCM_CSCMR1 = regVal;

    regVal = CCM_CSCDR2;
    regVal &= ~(CCM_CSCDR2_ESAI_DIV_MASK);
    regVal |= ((0x00000000) << CCM_CSCDR2_ESAI_DIV_SHIFT);
    regVal |= CCM_CSCDR2_ESAI_EN_MASK;
    CCM_CSCDR2 = regVal;
#endif
    /*ungate the ESAI clocks*/
    regVal = CCM_CCGR4;
    regVal |= (CCM_CCGR4_CG2_MASK | CCM_CCGR4_CG3_MASK);
    CCM_CCGR4 = regVal;
}

void _bsp_aud_temp_codec_hw_init(void)
{
    MQX_FILE_PTR file;
    uint32_t k = 1;

    file = fopen("ioexp0:", NULL);
    if (file != NULL) {
        ioctl(file, IO_IOCTL_IOEXP_SET_PIN_NO,
             (void *)k);
        ioctl(file, IO_IOCTL_IOEXP_SET_PIN_DIR_OUT, NULL);
        ioctl(file, IO_IOCTL_IOEXP_SET_PIN_VAL_LOW, NULL);

        _time_delay(50);
        ioctl(file, IO_IOCTL_IOEXP_SET_PIN_VAL_HIGH, NULL);
        _time_delay(50);

        fclose(file);

        _time_delay(1000);
        _bsp_dspi_io_init(0);
    }

}


void _bsp_esai_io_init(void)
{
    _bsp_esai_clocks_init();

    IOMUXC_RGPIO(54) =
        IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(3) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(1) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(2) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_HYS_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

    IOMUXC_RGPIO(55) =
        IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(3) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(1) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(2) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_HYS_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

    IOMUXC_RGPIO(56) =
        IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(3) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(1) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(2) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_HYS_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

    IOMUXC_RGPIO(57) =
        IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(3) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(1) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(2) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_HYS_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

    IOMUXC_RGPIO(58) =
        IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(3) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(1) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(2) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_HYS_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

    IOMUXC_RGPIO(59) =
        IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(3) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(1) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(2) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_HYS_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

    IOMUXC_RGPIO(60) =
        IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(3) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(1) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(2) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_HYS_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

    IOMUXC_RGPIO(61) =
        IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(3) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(1) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(2) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_HYS_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK |
        IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

    IOMUXC_ESAI_IPP_IND_FST_SELECT_INPUT =
        (1 << IOMUXC_ESAI_IPP_IND_FST_SELECT_INPUT_DAISY_SHIFT) & IOMUXC_ESAI_IPP_IND_FST_SELECT_INPUT_DAISY_MASK;

    IOMUXC_ESAI_IPP_IND_SCKT_SELECT_INPUT =
        (1 << IOMUXC_ESAI_IPP_IND_SCKT_SELECT_INPUT_DAISY_SHIFT) & IOMUXC_ESAI_IPP_IND_SCKT_SELECT_INPUT_DAISY_MASK;

    IOMUXC_ESAI_IPP_IND_SDO0_SELECT_INPUT =
        (1 << IOMUXC_ESAI_IPP_IND_SDO0_SELECT_INPUT_DAISY_SHIFT) & IOMUXC_ESAI_IPP_IND_SDO0_SELECT_INPUT_DAISY_MASK;

    IOMUXC_ESAI_IPP_IND_SDO1_SELECT_INPUT =
        (1 << IOMUXC_ESAI_IPP_IND_SDO1_SELECT_INPUT_DAISY_SHIFT) & IOMUXC_ESAI_IPP_IND_SDO1_SELECT_INPUT_DAISY_MASK;

     IOMUXC_ESAI_IPP_IND_SDO5_SDI0_SELECT_INPUT =
        (1 << IOMUXC_ESAI_IPP_IND_SDO5_SDI0_SELECT_INPUT_DAISY_SHIFT) & IOMUXC_ESAI_IPP_IND_SDO5_SDI0_SELECT_INPUT_DAISY_MASK;

    //Configure the DSPI0 to the mode of slave
    IOMUXC_RGPIO(41) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_HYS(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SRE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1);

    IOMUXC_RGPIO(42) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_HYS(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SRE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1);

    IOMUXC_RGPIO(43) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_HYS(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SRE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1);

    IOMUXC_RGPIO(44) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_HYS(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SRE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1);

    _bsp_aud_temp_codec_hw_init();

}

uint32_t _bsp_esai_clocks_get_fsys(void)
{
    return 133*1000*1000; /*133Mhz*/
}

uint32_t _bsp_esai_clocks_get_extal(void)
{
    return AUDIO_SYSTEM_CLK;
}

uint32_t _bsp_asrc_get_audio_extal(void)
{
    return AUDIO_SYSTEM_CLK;
}

uint32_t _bsp_asrc_get_custom_clock(void)
{
    return ASRC_PLL4_DIV_CLK;
}

_mqx_int _bsp_asrc_io_init(void)
{
    CCM_CCGR4 |= CCM_CCGR4_CG0(0x3);
    return MQX_OK;
}

_mqx_int _bsp_aud_mclk_io_init(void)
{
    IOMUXC_RGPIO(40) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(2) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(2) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_HYS_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

    IOMUXC_CCM_AUD_EXT_CLK_SELECT_INPUT = IOMUXC_CCM_AUD_EXT_CLK_SELECT_INPUT_DAISY(2);

    return MQX_OK;
}

/* EOF */
