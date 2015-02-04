/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains board-specific pin initialization functions.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <vybrid.h>

#if 0
static const uint16_t adc_pin_map[ADC_NUM_DEVICES][ADC_HW_CHANNELS] = {
    {
        LWADC0_AN0,
        LWADC0_AN1,
        LWADC0_AN2,
        LWADC0_AN3,
        LWADC0_AN4,
        LWADC0_AN5,
        LWADC0_AN6,
        LWADC0_AN7,
        LWADC_SIG_NC,
        LWADC_SIG_NC,
        LWADC_SIG_NC,
        LWADC_SIG_NC,
        LWADC_SIG_NC,
        LWADC_SIG_NC,
        LWADC_SIG_NC,
        LWADC_SIG_NC,
    },
    {
        LWADC1_AN0,
        LWADC1_AN1,
        LWADC1_AN2,
        LWADC1_AN3,
        LWADC1_AN4,
        LWADC1_AN5,
        LWADC1_AN6,
        LWADC1_AN7,
        LWADC_SIG_NC,
        LWADC_SIG_NC,
        LWADC_SIG_NC,
        LWADC_SIG_NC,
        LWADC_SIG_NC,
        LWADC_SIG_NC,
        LWADC_SIG_NC,
        LWADC_SIG_NC,
    }
};
#endif


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_adc_io_init
* Returned Value   : 0 for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to ADC
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_adc_io_init
(
     /* [IN] number of ADC device on which to perform hardware initialization */
    _mqx_uint adc_num
)
{
    int32_t status = IO_ERROR;

    if (adc_num == 0)
    {
        // enable ADC0 clock gate
        CCM_CCGR1 |= CCM_CCGR1_CG11(0x3);
        status = MQX_OK;
    }
    else if (adc_num == 1)
    {
        // enable ADC1 clock gate
        CCM_CCGR7 |= CCM_CCGR7_CG11(0x3);
        status = MQX_OK;
    }

    return status;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_adc_channel_io_init
* Returned Value   : 0 for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to ADC
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_adc_channel_io_init
(
     /* [IN] number of ADC device on which to perform hardware initialization */
    _mqx_uint input
)
{
    uint32_t device, channel;
    int32_t status = IO_ERROR;

    // decode device & channel
    device  = ADC_DEVICE(input);
    channel = ADC_CHANNEL(input);

    // supported ADC0SE8, ADC0SE9
    // - gpio mux is not required for these channels
    if (device == 0 && (channel == 8 || channel == 9))
    {
        status = MQX_OK;
    }

    // supported ADC1SE8, ADC1SE9
    // - gpio mux is not required for these channels
    if (device == 1 && (channel == 8 || channel == 9))
    {
        status = MQX_OK;
    }

    return status;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_rtc_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to RTC
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_rtc_io_init
(
    void
)
{

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_dspi_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to DSPI
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_dspi_io_init
(
    _mqx_uint dev_num
)
{
    // no usefull dspi routing present
    return IO_ERROR;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_quadspi_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to QuadSPI
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_quadspi_io_init
(
    _mqx_uint dev_num
)
{
    switch (dev_num)
    {
        case 0:
            /* IOMUX settings for QSPI0 */
            /* QSPI0_A_SCK */
            IOMUXC_RGPIO(79) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1); // (Y17) PTD0.QSCK_A

            /* QSPI0_A_CS0 */
            IOMUXC_RGPIO(80) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1); // (Y18) PTD1.QPCS0_A

            /* QSPI0_A_DATA[3] */
            IOMUXC_RGPIO(81) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1); // (V18) PTD2.QSPI_IO3_A

            /* QSPI0_A_DATA[2] */
            IOMUXC_RGPIO(82) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1); // (Y19) PTD3.QSPI_IO2_A

            /* QSPI0_A_DATA[1] */
            IOMUXC_RGPIO(83) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1); // (W19) PTD4.QSPI_IO1_A

            /* QSPI0_A_DATA[0] */
            IOMUXC_RGPIO(84) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1); // (W20) PTD5.QSPI_IO0_A

            /* QSPI0_B_SCK */
            IOMUXC_RGPIO(86) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1); // (V19) PTD7.QSCK_B

            /* QSPI0_B_CS0 */
            IOMUXC_RGPIO(87) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1); // (U17) PTD8.QPCS0_B

            /* QSPI0_B_DATA[3] */
            IOMUXC_RGPIO(88) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1); // (U18) PTD9.QSPI_IO3_B

            /* QSPI0_B_DATA[2] */
            IOMUXC_RGPIO(89) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1); // (U20) PTD10.QSPI_IO2_B

            /* QSPI0_B_DATA[1] */
            IOMUXC_RGPIO(90) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1); // (T20) PTD11.QSPI_IO1_B

            /* QSPI0_B_DATA[0] */
            IOMUXC_RGPIO(91) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1); // (T19) PTD12.QSPI_IO0_B

            break;
        case 1:
            /* IOMUX settings for QSPI1 */
            /* QSPI1_A_SCK */
            IOMUXC_RGPIO(9) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(7); // (U6) PTA19.QSCK_A

            /* QSPI1_A_CS0 */
            IOMUXC_RGPIO(22) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(7); // (T6) PTB0.QPCS0_A

            /* QSPI1_A_DATA[3] */
            IOMUXC_RGPIO(23) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(7); // (T7) PTB1.QSPI_IO3_A

            /* QSPI1_A_DATA[2] */
            IOMUXC_RGPIO(24) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(7); // (V7) PTB2.QSPI_IO2_A

            /* QSPI1_A_DATA[1] */
            IOMUXC_RGPIO(25) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(7); // (W7) PTB3.QSPI_IO1_A

            /* QSPI1_A_DATA[0] */
            IOMUXC_RGPIO(26) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(7); // (Y7) PTB4.QSPI_IO0_A
            break;
        default:
            return -1;
    }
    return MQX_OK;
}



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_enet_io_init
* Returned Value   : MQX_OK or IO_ERROR
* Comments         :
*    This function performs BSP-specific initialization related to ENET
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_enet_io_init
(
    _mqx_uint device
)
{
    int32_t status = IO_ERROR;

    // NOTE: sw12[3,4,5] must be ON
    switch (device)
    {
        case 0:
            // enable enet0 clock gate
            CCM_CCGR9 |= CCM_CCGR9_CG0(0x3);

            // ENET RMII clkin
            IOMUXC_RGPIO(0) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

            // RMII0_MDC
            IOMUXC_RGPIO(45) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7);

            // RMII0_MDIO
            IOMUXC_RGPIO(46) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

            // RMII0_CRS_DV
            IOMUXC_RGPIO(47) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

            // RMII0_RXD1
            IOMUXC_RGPIO(48) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

            // RMII0_RXD0
            IOMUXC_RGPIO(49) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

            // RMII0_RXER
            IOMUXC_RGPIO(50) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

            // RMII0_TXD1
            IOMUXC_RGPIO(51) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

            // RMII0_TXD0
            IOMUXC_RGPIO(52) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

            // RMII0_TXEN
            IOMUXC_RGPIO(53) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

            // select and enable enet clkin source
            CCM_CSCMR2 &= ~CCM_CSCMR2_RMII_CLK_SEL_MASK;
            CCM_CSCMR2 |= 6 << CCM_CSCMR2_RMII_CLK_SEL_SHIFT;
            CCM_CSCDR1 |= CCM_CSCDR1_RMII_CLK_EN_MASK;

            // disable NRST
            IOMUXC_RGPIO(34) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(0) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;
            // by setting NRST to "1"
            GPIO1_PSOR |= 1 << 2;

            status = MQX_OK;
        break;
    }

    return status;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_dcu4_io_init
* Returned Value   : MQX_OK or IO_ERROR
* Comments         :
*    This function performs BSP-specific initialization related to DCU4
*
*END*----------------------------------------------------------------------*/

#define DCU4_IOMUX_VALUE    \
    IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |    \
    IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(4) |    \
    IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(2) |  \
    IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1)

_mqx_int _bsp_dcu4_io_init
(
    _mqx_uint dev_num
)
{
    int32_t status = IO_ERROR;
    int32_t i;

    switch (dev_num)
    {
        case 0:
            // set clock source - PLL1 PFD2 clock
            CCM_CSCMR1 &= ~((uint32_t)CCM_CSCMR1_DCU0_CLK_SEL_MASK);
            // divide by 1
            CCM_CSCDR3  = (CCM_CSCDR3 & ~((uint32_t)CCM_CSCDR3_DCU0_DIV_MASK)) | CCM_CSCDR3_DCU0_DIV(0);
            CCM_CSCDR3 |= CCM_CSCDR3_DCU0_EN_MASK;

            // enable DCU0 clock gate
            CCM_CCGR3 |= CCM_CCGR3_CG8(0x3);

            // bypass TCON for DVI
            TCON0_CTRL1 |= TCON_CTRL1_TCON_BYPASS_MASK;

            // DCU4 HSYNC
            IOMUXC_RGPIO(105) = DCU4_IOMUX_VALUE;

            // DCU4 VSYNC
            IOMUXC_RGPIO(106) = DCU4_IOMUX_VALUE;

            // DCU4 PCLK
            IOMUXC_RGPIO(107) = DCU4_IOMUX_VALUE;

            // DCU4 TAG
            IOMUXC_RGPIO(108) = DCU4_IOMUX_VALUE;

            // DCU4 DE
            IOMUXC_RGPIO(109) = DCU4_IOMUX_VALUE;

            for (i = 0; i < 24; i++)
            {
                // R0-R7, G0-G7, B0-B7
                IOMUXC_RGPIO(110 + i) = DCU4_IOMUX_VALUE;
            }

            status = MQX_OK;
        break;
    }

    return status;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_esdhc_io_init
* Returned Value   : MQX_OK or IO_ERROR
* Comments         :
*    This function performs BSP-specific initialization related to ESDHC
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_esdhc_io_init
(
    _mqx_uint dev_num,
    _mqx_uint value
)
{
    int32_t status = IO_ERROR;

    switch (dev_num)
    {
        case 1:
            // set clock source - platform bus clock
            CCM_CSCMR1 |= CCM_CSCMR1_ESDHC1_CLK_SEL(3);
            CCM_CSCDR2  = (CCM_CSCDR2 & ~((uint32_t)CCM_CSCDR2_ESDHC1_DIV_MASK)) | CCM_CSCDR2_ESDHC1_DIV(2);
            CCM_CSCDR2 |= CCM_CSCDR2_ESDHC1_EN_MASK;

            // enable SDHC1 clock gate
            CCM_CCGR7 |= CCM_CCGR7_CG2(0x3);

            // SDHC1 CLK
            IOMUXC_RGPIO(14) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(2) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(5);

            // SDHC1 CMD
            IOMUXC_RGPIO(15) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(2) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(5);

            // SDHC1 DAT0
            IOMUXC_RGPIO(16) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(2) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(5);

            // SDHC1 DAT1
            IOMUXC_RGPIO(17) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(2) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(5);

            // SDHC1 DAT2
            IOMUXC_RGPIO(18) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(2) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(5);

            // SDHC1 DAT3
            IOMUXC_RGPIO(19) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(2) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(5);

            status = MQX_OK;
        break;
    }

    return status;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_gpio_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to GPIO
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_gpio_io_init
(
    void
)
{
    // enable clock gate for all ports
    CCM_CCGR2 |=
        CCM_CCGR2_CG9(0x3)  |  // port a
        CCM_CCGR2_CG10(0x3) |  // port b
        CCM_CCGR2_CG11(0x3) |  // port c
        CCM_CCGR2_CG12(0x3) |  // port d
        CCM_CCGR2_CG13(0x3);   // port e

    return MQX_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_i2c_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to I2C
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_i2c_io_init
(
    _mqx_uint dev_num
 )
{
    int32_t status = IO_ERROR;

    switch (dev_num)
    {
        case 1:
            // enable I2C1 clock gate
            CCM_CCGR4 |= CCM_CCGR4_CG7(0x3);

            // I2C1_SCL
            IOMUXC_RGPIO(38) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7)  |
                IOMUXC_SW_MUX_CTL_PAD_ODE(1)      |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3)|
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(2);

            // I2C1_SDA
            IOMUXC_RGPIO(39) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7)  |
                IOMUXC_SW_MUX_CTL_PAD_ODE(1)      |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3)|
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(2);

            IOMUXC_I2C1_IPP_SCL_IND_SELECT_INPUT = IOMUXC_I2C1_IPP_SCL_IND_SELECT_INPUT_DAISY(1);
            IOMUXC_I2C1_IPP_SDA_IND_SELECT_INPUT = IOMUXC_I2C1_IPP_SDA_IND_SELECT_INPUT_DAISY(1);
            status = MQX_OK;
        break;
        case 2:
            // enable I2C2 clock gate
            CCM_CCGR10 |= CCM_CCGR10_CG6(0x3);

            // I2C2_SCL
            IOMUXC_RGPIO(66) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7)  |
                IOMUXC_SW_MUX_CTL_PAD_ODE(1)      |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3)|
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(3);

            // I2C2_SDA
            IOMUXC_RGPIO(67) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7)  |
                IOMUXC_SW_MUX_CTL_PAD_ODE(1)      |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3)|
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(3);

            IOMUXC_I2C2_IPP_SCL_IND_SELECT_INPUT = IOMUXC_I2C2_IPP_SCL_IND_SELECT_INPUT_DAISY(1);   // ALT3 - PAD_12
            IOMUXC_I2C2_IPP_SDA_IND_SELECT_INPUT = IOMUXC_I2C2_IPP_SDA_IND_SELECT_INPUT_DAISY(1);   // ALT3 - PAD_13
            status = MQX_OK;
        break;
        case 3:
            // enable I2C3 clock gate
            CCM_CCGR10 |= CCM_CCGR10_CG7(0x3);

            // I2C3_SCL
            IOMUXC_RGPIO(20) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7)  |
                IOMUXC_SW_MUX_CTL_PAD_ODE(1)      |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3)|
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(5);

            // I2C3_SDA
            IOMUXC_RGPIO(21) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1)  |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7)  |
                IOMUXC_SW_MUX_CTL_PAD_ODE(1)      |
                IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(3)|
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(5);
            status = MQX_OK;
        break;
    }
    return status;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_sai_io_init
* Returned Value   : 0 for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to SAI
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_sai_io_init
(
    _mqx_uint dev_num
)
{
    int32_t status = IO_ERROR;

    switch(dev_num)
    {
        case 0:
            // select clock source
            CCM_CSCMR1 = (CCM_CSCMR1 & ~CCM_CSCMR1_SAI0_CLK_SEL_MASK) | CCM_CSCMR1_SAI0_CLK_SEL(3);
            CCM_CSCDR1 = (CCM_CSCMR1 & ~CCM_CSCDR1_SAI0_DIV_MASK) | CCM_CSCDR1_SAI0_DIV(0x0f);
            CCM_CSCDR1 |= CCM_CSCDR1_SAI0_EN_MASK;

            // enable SAI0 clock gate
            CCM_CCGR0 |= CCM_CCGR0_CG15(0x3);

            // RX_BCLK
            IOMUXC_RGPIO(94) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

            // RX_DATA
            IOMUXC_RGPIO(95) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

            // RX_SYNC
            IOMUXC_RGPIO(97) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

            // TX_BCLK
            IOMUXC_RGPIO(93) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

            // TX_DATA
            IOMUXC_RGPIO(96) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

            // TX_SYNC
            IOMUXC_RGPIO(98) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

            status = MQX_OK;
        break;
        case 1:
            // select clock source
            CCM_CSCMR1 = (CCM_CSCMR1 & ~CCM_CSCMR1_SAI1_CLK_SEL_MASK) | CCM_CSCMR1_SAI1_CLK_SEL(3);
            CCM_CSCDR1 = (CCM_CSCMR1 & ~CCM_CSCDR1_SAI1_DIV_MASK) | CCM_CSCDR1_SAI1_DIV(0x0f);
            CCM_CSCDR1 |= CCM_CSCDR1_SAI1_EN_MASK;

            // enable SAI1 clock
            CCM_CCGR1 |= CCM_CCGR1_CG0(0x3);

            // RX_BCLK
            IOMUXC_RGPIO(100) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

            // RX_DATA
            IOMUXC_RGPIO(101) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

            // RX_SYNC
            IOMUXC_RGPIO(103) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

            // TX_BCLK
            // IOMUXC_RGPIO(99) =
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1) |
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

            // TX_DATA
            // IOMUXC_RGPIO(102) =
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1) |
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

            // TX_SYNC
            // IOMUXC_RGPIO(104) =
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(1) |
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1) |
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
            //     IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

            status = MQX_OK;
        break;
        case 2:
            // select clock source
            CCM_CSCMR1 = (CCM_CSCMR1 & ~CCM_CSCMR1_SAI2_CLK_SEL_MASK) | CCM_CSCMR1_SAI2_CLK_SEL(3);
            CCM_CSCDR1 = (CCM_CSCMR1 & ~CCM_CSCDR1_SAI2_DIV_MASK) | CCM_CSCDR1_SAI2_DIV(0x0f);
            CCM_CSCDR1 |= CCM_CSCDR1_SAI2_EN_MASK;

            // enable SAI2 clock
            CCM_CCGR1 |= CCM_CCGR1_CG1(0x3);

            // RX_BCLK
            IOMUXC_RGPIO(11) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(5) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

            // RX_DATA
            IOMUXC_RGPIO(12) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(5) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

            // RX_SYNC
            IOMUXC_RGPIO(13) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(5) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(7) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(1) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

            status = MQX_OK;
        break;
    }

    return status;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_serial_io_init
* Returned Value   : MQX_OK for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to serial
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_serial_io_init
(
    /* [IN] Serial device number */
    _mqx_uint dev_num,

    /* [IN] Required functionality */
    _mqx_uint flags
)
{
    int32_t status = IO_ERROR;

    // TODO: need to implement flags
    switch (dev_num)
    {
        case 0:
            // enable UART0 clock
            CCM_CCGR0 |= CCM_CCGR0_CG7(0x3);

            // setup mux
            IOMUXC_RGPIO(32) = 0x001011A2;
            IOMUXC_RGPIO(33) = 0x001011A1;

            status = MQX_OK;
        break;
        case 2:
            // enable UART2 clock
            CCM_CCGR0 |= CCM_CCGR0_CG9(0x3);

            // setup mux
            IOMUXC_SCI_FLX2_IPP_IND_SCI_RX_SELECT_INPUT = 0x00000001;
            IOMUXC_SCI_FLX2_IPP_IND_SCI_TX_SELECT_INPUT = 0x00000001;
            IOMUXC_RGPIO(71) = 0x006021A2;
            IOMUXC_RGPIO(72) = 0x006021A1;

            status = MQX_OK;
        break;
    }
    return status;
}


_mqx_int _bsp_serial_rts_init
(
    _mqx_uint device_index
)
{
    return IO_ERROR;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_host_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to USB in host mode
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_usb_host_io_init
(
    struct usb_host_if_struct *usb_if
)
{
    /* Setup clocking and PLL */
    CCM_CLPCR &= ~CCM_CLPCR_SBYOS_MASK; //on-chip oscillator on (by default after reset)
    //Setup 3V0 regulator
    Anadig_REG_3P0 |= Anadig_REG_3P0_ENABLE_BO_MASK;
    Anadig_REG_3P0 |= Anadig_REG_3P0_ENABLE_LINREG_MASK;
    while(!( Anadig_REG_3P0 & Anadig_REG_3P0_OK_VDD3P0_MASK))
    { /* wait till regulator output is in steady state */ }


    if (usb_if->HOST_INIT_PARAM == &_ehci0_host_init_param) {
        /* For USB0 */
        // enable USB0 clock gate
        CCM_CCGR1 |= CCM_CCGR1_CG4(0x3);

        Anadig_PLL3_CTRL &= ~Anadig_PLL3_CTRL_BYPASS_CLK_SRC_MASK; //the source of clock is 24MHz XTAL CLK
        Anadig_PLL3_CTRL &= ~Anadig_PLL3_CTRL_BYPASS_MASK; //do not bypass the PLL
        Anadig_PLL3_CTRL |= Anadig_PLL3_CTRL_POWER_MASK; //power the USB0 PLL
        Anadig_PLL3_CTRL |= Anadig_PLL3_CTRL_EN_USB_CLKS_MASK; //start clocking PHY UTMI peripeheral

        /* Setup PHY0 peripheral with UTMI interface */
        USBPHY0_CTRL_CLR = USBPHY_CTRL_SFTRST_MASK | USBPHY_CTRL_CLKGATE_MASK; //leave the reset state on PHY and run clocks
        USBPHY0_DEBUG_CLR = USBPHY_DEBUG_CLKGATE_MASK;
        USBPHY0_CTRL_SET = USBPHY_CTRL_ENUTMILEVEL3_MASK | USBPHY_CTRL_ENUTMILEVEL2_MASK;

        USBPHY0_PWD = 0; //switch PHY from standby to normal operation

        /* Setup PTA16 (control of USB0 VBUS) to power the bus */
        GPIO0_PSOR = 1 << 6; //set output level of PTA16 to "1"
        IOMUXC_RGPIO(6) = IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(0) | IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(0) | IOMUXC_SW_MUX_CTL_PAD_PAD_SRE_MASK | IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(3) | IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

        USBC0_CTRL |= USBC_CTRL_PM_MASK;
    }
    else if (usb_if->HOST_INIT_PARAM == &_ehci1_host_init_param) {
        /* For USB1 */
        // enable USB1 clock gate
        CCM_CCGR7 |= CCM_CCGR7_CG4(0x3);

        Anadig_PLL7_CTRL &= ~Anadig_PLL7_CTRL_BYPASS_CLK_SRC_MASK; //the source of clock is 24MHz XTAL CLK
        Anadig_PLL7_CTRL &= ~Anadig_PLL7_CTRL_BYPASS_MASK; //do not bypass the PLL
        Anadig_PLL7_CTRL |= Anadig_PLL7_CTRL_POWER_MASK; //power the USB1 PLL
        Anadig_PLL7_CTRL |= Anadig_PLL7_CTRL_EN_USB_CLKS_MASK; //power the USB1 PHY

        /* Setup PHY1 peripheral with UTMI interface */
        USBPHY1_CTRL_CLR = USBPHY_CTRL_SFTRST_MASK | USBPHY_CTRL_CLKGATE_MASK; //leave the reset state on PHY and run clocks
        USBPHY1_DEBUG_CLR = USBPHY_DEBUG_CLKGATE_MASK;
        USBPHY1_CTRL_SET = USBPHY_CTRL_ENUTMILEVEL3_MASK | USBPHY_CTRL_ENUTMILEVEL2_MASK;

        USBPHY1_PWD = 0; //switch PHY from standby to normal operation

        /* Setup PTA24 (control of USB1 VBUS) to power the bus */
        GPIO0_PSOR = 1 << 14; //set output level of PTA24 to "1"
        IOMUXC_RGPIO(14) = IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(0) | IOMUXC_SW_MUX_CTL_PAD_PAD_SPEED(0) | IOMUXC_SW_MUX_CTL_PAD_PAD_SRE_MASK | IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(3) | IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

        USBC1_CTRL |= USBC_CTRL_PM_MASK;
    }
    else {
        /* Return unknown interface */
        return IO_ERROR;
    }

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_dev_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to USB in device mode
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_usb_dev_io_init
(
    struct usb_dev_if_struct *usb_if
)
{
    /* Setup clocking and PLL */
    CCM_CLPCR &= ~CCM_CLPCR_SBYOS_MASK; //on-chip oscillator on (by default after reset)
    //Setup 3V0 regulator
    Anadig_REG_3P0 |= Anadig_REG_3P0_ENABLE_BO_MASK;
    Anadig_REG_3P0 |= Anadig_REG_3P0_ENABLE_LINREG_MASK;
    while(!( Anadig_REG_3P0 & Anadig_REG_3P0_OK_VDD3P0_MASK))
    { /* wait till regulator output is in steady state */ }


    if (usb_if->DEV_INIT_PARAM == &_ehci0_dev_init_param) {
        /* For USB0 */
        // enable USB0 clock gate
        CCM_CCGR1 |= CCM_CCGR1_CG4(0x3);

        Anadig_PLL3_CTRL &= ~Anadig_PLL3_CTRL_BYPASS_CLK_SRC_MASK; //the source of clock is 24MHz XTAL CLK
        Anadig_PLL3_CTRL &= ~Anadig_PLL3_CTRL_BYPASS_MASK; //do not bypass the PLL
        Anadig_PLL3_CTRL |= Anadig_PLL3_CTRL_POWER_MASK; //power the USB0 PLL
        Anadig_PLL3_CTRL |= Anadig_PLL3_CTRL_EN_USB_CLKS_MASK; //start clocking PHY UTMI peripeheral

        /* Setup PHY0 peripheral with UTMI interface */
        USBPHY0_CTRL_CLR = USBPHY_CTRL_SFTRST_MASK | USBPHY_CTRL_CLKGATE_MASK; //leave the reset state on PHY and run clocks
        USBPHY0_DEBUG_CLR = USBPHY_DEBUG_CLKGATE_MASK;
        USBPHY0_CTRL_SET = USBPHY_CTRL_ENUTMILEVEL3_MASK | USBPHY_CTRL_ENUTMILEVEL2_MASK;

        USBPHY0_PWD = 0; //switch PHY from standby to normal operation

        USBC0_CTRL |= USBC_CTRL_PM_MASK;
    }
    else if (usb_if->DEV_INIT_PARAM == &_ehci1_dev_init_param) {
        /* For USB1 */
        // enable USB1 clock gate
        CCM_CCGR7 |= CCM_CCGR7_CG4(0x3);

        Anadig_PLL7_CTRL &= ~Anadig_PLL7_CTRL_BYPASS_CLK_SRC_MASK; //the source of clock is 24MHz XTAL CLK
        Anadig_PLL7_CTRL &= ~Anadig_PLL7_CTRL_BYPASS_MASK; //do not bypass the PLL
        Anadig_PLL7_CTRL |= Anadig_PLL7_CTRL_POWER_MASK; //power the USB1 PLL
        Anadig_PLL7_CTRL |= Anadig_PLL7_CTRL_EN_USB_CLKS_MASK; //power the USB1 PHY

        /* Setup PHY1 peripheral with UTMI interface */
        USBPHY1_CTRL_CLR = USBPHY_CTRL_SFTRST_MASK | USBPHY_CTRL_CLKGATE_MASK; //leave the reset state on PHY and run clocks
        USBPHY1_DEBUG_CLR = USBPHY_DEBUG_CLKGATE_MASK;
        USBPHY1_CTRL_SET = USBPHY_CTRL_ENUTMILEVEL3_MASK | USBPHY_CTRL_ENUTMILEVEL2_MASK;

        USBPHY1_PWD = 0; //switch PHY from standby to normal operation

        USBC1_CTRL |= USBC_CTRL_PM_MASK;
    }
    else {
        /* Return unknown interface */
        return IO_ERROR;
    }

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_ioexp_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to MAX7310
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_ioexp_io_init
(
    void
)
{
    IOMUXC_RGPIO(68) =
        IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(0) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(1) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(2) |
        IOMUXC_SW_MUX_CTL_PAD_PAD_OBE_MASK;

    /* MAX7310 Reset pin high */
    GPIO2_PDOR = 0x10;

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_ftm_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to FTM
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_ftm_io_init
(
    _mqx_uint dev_num
)
{
    int32_t status = IO_ERROR;

    switch (dev_num)
    {
        case 0:
            // enable FlexTimer0 clock
            CCM_CCGR1 |= CCM_CCGR1_CG8(0x3);

            status = MQX_OK;
        break;
        case 1:
            // enable FlexTimer1 clock
            CCM_CCGR1 |= CCM_CCGR1_CG9(0x3);

            // setup mux
            IOMUXC_RGPIO(30) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(4) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

            IOMUXC_RGPIO(31) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(4) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

            IOMUXC_FLEXTIMER1_IPP_IND_FTM_PHA_SELECT_INPUT =
                IOMUXC_FLEXTIMER1_IPP_IND_FTM_PHA_SELECT_INPUT_DAISY_MASK;
            IOMUXC_FLEXTIMER1_IPP_IND_FTM_PHB_SELECT_INPUT =
                IOMUXC_FLEXTIMER1_IPP_IND_FTM_PHB_SELECT_INPUT_DAISY_MASK;

            status = MQX_OK;
        break;
        case 2:
            // enable FlexTimer2 clock
            CCM_CCGR7 |= CCM_CCGR7_CG8(0x3);

            // setup mux
            IOMUXC_RGPIO(75) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(5) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(4) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

            IOMUXC_RGPIO(76) =
                IOMUXC_SW_MUX_CTL_PAD_PAD_MUX_MODE(5) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_DSE(4) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUS(3) |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PKE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_PUE_MASK |
                IOMUXC_SW_MUX_CTL_PAD_PAD_IBE_MASK;

            status = MQX_OK;
        break;
        case 3:
            // enable FlexTimer3 clock
            CCM_CCGR7 |= CCM_CCGR7_CG9(0x3);

            status = MQX_OK;
        break;
    }
    return status;

}

