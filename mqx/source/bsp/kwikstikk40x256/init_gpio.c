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
    uint8_t dev_num,

    /* [IN] Required functionality */
    uint8_t flags
)
{
    SIM_MemMapPtr   sim = SIM_BASE_PTR;
    PORT_MemMapPtr  pctl;

    /* Setup GPIO for SCIx */
    switch (dev_num)
    {
        case 0:
            pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;
            if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE)
            {
                /* PTD6 as RX function (Alt.3) + drive strength */
                pctl->PCR[6] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
                /* PTD7 as TX function (Alt.3) + drive strength */
                pctl->PCR[7] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
            }
            if (flags & IO_PERIPHERAL_PIN_MUX_DISABLE)
            {
                /* PTD6 default */
                pctl->PCR[6] = 0;
                /* PTD7 default */
                pctl->PCR[7] = 0;
            }
            if (flags & IO_PERIPHERAL_CLOCK_ENABLE)
            {
                /* start SGI clock */
                sim->SCGC4 |= SIM_SCGC4_UART0_MASK;
            }
            if (flags & IO_PERIPHERAL_CLOCK_DISABLE)
            {
                /* stop SGI clock */
                sim->SCGC4 &= (~ SIM_SCGC4_UART0_MASK);
            }
            break;
        case 1:
            pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;
            if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE)
            {
                /* PTC3 as RX function (Alt.3) + drive strength */
                pctl->PCR[3] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
                /* PTC4 as TX function (Alt.3) + drive strength */
                pctl->PCR[4] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
            }
            if (flags & IO_PERIPHERAL_PIN_MUX_DISABLE)
            {
                /* PTC3 default */
                pctl->PCR[3] = 0;
                /* PTC4 default */
                pctl->PCR[4] = 0;
            }
            if (flags & IO_PERIPHERAL_CLOCK_ENABLE)
            {
                /* start SGI clock */
                sim->SCGC4 |= SIM_SCGC4_UART1_MASK;
            }
            if (flags & IO_PERIPHERAL_CLOCK_DISABLE)
            {
                /* start SGI clock */
                sim->SCGC4 &= (~ SIM_SCGC4_UART1_MASK);
            }
            break;
        case 2:
            pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;
            if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE)
            {
                /* PTD2 as RX function (Alt.3) + drive strength */
                pctl->PCR[2] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
                /* PTD3 as TX function (Alt.3) + drive strength */
                pctl->PCR[3] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
            }
            if (flags & IO_PERIPHERAL_PIN_MUX_DISABLE)
            {
                /* PTD2 default */
                pctl->PCR[2] = 0;
                /* PTD3 default */
                pctl->PCR[3] = 0;
            }
            if (flags & IO_PERIPHERAL_CLOCK_ENABLE)
            {
                /* start SGI clock */
                sim->SCGC4 |= SIM_SCGC4_UART2_MASK;
            }
            if (flags & IO_PERIPHERAL_CLOCK_DISABLE)
            {
                /* stop SGI clock */
                sim->SCGC4 &= (~ SIM_SCGC4_UART2_MASK);
            }
            break;

        case 3:
            pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;
            if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE)
            {
                /* PTE5 as RX function (Alt.3) + drive strength */
                pctl->PCR[5] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
                /* PTE4 as TX function (Alt.3) + drive strength */
                pctl->PCR[4] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
            }
            if (flags & IO_PERIPHERAL_PIN_MUX_DISABLE)
            {
                /* PTC16 default */
                pctl->PCR[16] = 0;
                /* PTC17 default */
                pctl->PCR[17] = 0;
            }
            if (flags & IO_PERIPHERAL_CLOCK_ENABLE)
            {
                /* start SGI clock */
                sim->SCGC4 |= SIM_SCGC4_UART3_MASK;
            }
            if (flags & IO_PERIPHERAL_CLOCK_DISABLE)
            {
                /* stop SGI clock */
                sim->SCGC4 &= (~ SIM_SCGC4_UART3_MASK);
            }
            break;

        case 4:
            pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;
            if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE)
            {
                /* PTC14 as RX function (Alt.3) + drive strength */
                pctl->PCR[14] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
                /* PTC15 as TX function (Alt.3) + drive strength */
                pctl->PCR[15] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
            }
            if (flags & IO_PERIPHERAL_PIN_MUX_DISABLE)
            {
                /* PTC14 default */
                pctl->PCR[14] = 0;
                /* PTC15 default */
                pctl->PCR[15] = 0;
            }
            if (flags & IO_PERIPHERAL_CLOCK_ENABLE)
            {
                /* starting SGI clock */
                sim->SCGC1 |= SIM_SCGC1_UART4_MASK;
            }
            if (flags & IO_PERIPHERAL_CLOCK_DISABLE)
            {
                /* starting SGI clock */
                sim->SCGC1 &= (~ SIM_SCGC1_UART4_MASK);
            }
            break;

        case 5:
            pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;
            if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE)
            {
                /* PTE8 as RX function (Alt.3) + drive strength */
                pctl->PCR[8] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
                /* PTE9 as RX function (Alt.3) + drive strength */
                pctl->PCR[9] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
            }
            if (flags & IO_PERIPHERAL_PIN_MUX_DISABLE)
            {
                /* PTE8 default */
                pctl->PCR[8] = 0;
                /* PTE9 default */
                pctl->PCR[9] = 0;
            }
            if (flags & IO_PERIPHERAL_CLOCK_ENABLE)
            {
                /* starting SGI clock */
                sim->SCGC1 |= SIM_SCGC1_UART5_MASK;
            }
            if (flags & IO_PERIPHERAL_CLOCK_DISABLE)
            {
                /* starting SGI clock */
                sim->SCGC1 &= (~ SIM_SCGC1_UART5_MASK);
            }
            break;

         default:
            return -1;
  }

  return 0;
}

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
    SIM_MemMapPtr sim_ptr = SIM_BASE_PTR;

    /* Enable ADC clocks */
    if (adc_num == 0)
        sim_ptr->SCGC6 |= SIM_SCGC6_ADC0_MASK;
    else if (adc_num == 1)
        sim_ptr->SCGC3 |= SIM_SCGC3_ADC1_MASK;
    else
        return IO_ERROR;

    return IO_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_adc_channel_io_init
* Returned Value   : 0 for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to ADC channel
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_adc_channel_io_init
(
     /* [IN] number of channel on which to perform hardware initialization */
    uint16_t   source
)
{
    uint8_t ch = ADC_GET_CHANNEL(source);
    uint8_t gpio_port;
    PORT_MemMapPtr pctl;

    #define ADC_SIG_PORTA   (0x01 << 5)
    #define ADC_SIG_PORTB   (0x02 << 5)
    #define ADC_SIG_PORTC   (0x03 << 5)
    #define ADC_SIG_PORTD   (0x04 << 5)
    #define ADC_SIG_PORTE   (0x05 << 5)
    #define ADC_SIG_NA      (0x00) /* signal not available */
    #define ADC_SIG_NC      (0x01) /* signal not configurable */

    /* Conversion table for ADC0x inputs, where x is 0 to 23, mux is defaultly "B" */
    const static uint8_t adc0_conv_table[] = {
        ADC_SIG_NC, /*0 leave as default*/
        ADC_SIG_NC, /*1 leave as default*/
        ADC_SIG_NA, /*2 not implemented, leave as default*/
        ADC_SIG_NC, /*3 leave as default*/
        ADC_SIG_PORTC | 2, /*4b */
        ADC_SIG_PORTD | 1, /*5b*/
        ADC_SIG_PORTD | 5, /*6b */
        ADC_SIG_PORTD | 6, /*7b */
        ADC_SIG_PORTB | 0, /*8  */
        ADC_SIG_PORTB | 1, /*9  */
        ADC_SIG_PORTA | 7, /*10 */
        ADC_SIG_PORTA | 8, /*11 */
        ADC_SIG_PORTB | 2, /*12 */
        ADC_SIG_PORTB | 3, /*13 */
        ADC_SIG_PORTC | 0, /*14 */
        ADC_SIG_PORTC | 1, /*15 */
        ADC_SIG_NC,         /*16 leave as default  */
        ADC_SIG_PORTE | 24, /*17 */
        ADC_SIG_PORTE | 25, /*18 */
        ADC_SIG_NC, /*19 not implemented, leave as default  */
        ADC_SIG_NC, /*20 not implemented, leave as default  */
        ADC_SIG_NA, /*21 not implemented, leave as default  */
        ADC_SIG_NA, /*22 not implemented, leave as default  */
        ADC_SIG_NA, /*23 not implemented, leave as default  */
    };

    /* Conversion table for ADC1x, where x is 0 to 23, mux is defaultly "B" (or nothing) */
    const static uint8_t adc1_conv_tableB[] = {
        ADC_SIG_NC, /* 0 leave as default */
        ADC_SIG_NC, /* 1 leave as default */
        ADC_SIG_NA, /* 2 not implemented, leave as default*/
        ADC_SIG_NC, /* 3 leave as default */
        ADC_SIG_PORTC | 7, /*4b */
        ADC_SIG_PORTC | 9, /*5b */
        ADC_SIG_PORTC | 10,/*6b */
        ADC_SIG_PORTC | 11,/*7b */
        ADC_SIG_PORTB | 0, /*8  */
        ADC_SIG_PORTB | 1, /*9  */
        ADC_SIG_PORTB | 1, /*10 */
        ADC_SIG_PORTB | 4, /*11 */
        ADC_SIG_PORTB | 5, /*12 */
        ADC_SIG_PORTB | 6, /*13 */
        ADC_SIG_PORTB | 10,/*14 */
        ADC_SIG_PORTB | 11,/*15 */
        ADC_SIG_NC, /*16 leave as default*/
        ADC_SIG_PORTA | 17, /*17*/
        ADC_SIG_NA, /*18 not implemented, leave as default  */
        ADC_SIG_NC, /*19 not implemented, leave as default  */
        ADC_SIG_NC, /*20 not implemented, leave as default  */
        ADC_SIG_NA, /*21 not implemented, leave as default  */
        ADC_SIG_NA, /*22 not implemented, leave as default  */
        ADC_SIG_NA, /*23 not implemented, leave as default  */
    };

    /* Conversion table for ADC1x, where x is 4 to 7, mux is "A" */
    const static uint8_t adc1_conv_tableA[] = {
        ADC_SIG_PORTE | 0, /*4a  */
        ADC_SIG_PORTE | 1, /*5a  */
        ADC_SIG_PORTE | 2, /*6a  */
        ADC_SIG_PORTE | 3, /*7a  */
    };

    if (ADC_GET_DIFF(source) && ch > 3)
        return IO_ERROR; /* signal not available */

    if (ch < 24) {
        if (ADC_GET_MODULE(source) == ADC_SOURCE_MODULE(1)) {
            /* Get result for module 0 */
           gpio_port = adc0_conv_table[ch];
        }
        else {
            if ((ADC_GET_MUXSEL(source) == ADC_SOURCE_MUXSEL_B) || (ADC_GET_MUXSEL(source) == ADC_SOURCE_MUXSEL_X))
            /* Get result for module 1, if user wants "B" channel or any channel */
                gpio_port = adc1_conv_tableB[ch];
            else {
                /* Get result for module 1, if user wants "A" channel or any other */
                if (ch < 4 || ch > 7)
                    gpio_port = ADC_SIG_NA;
                else
                    gpio_port = adc1_conv_tableA[ch - 4];
            }
        }
    }
    else
        gpio_port = ADC_SIG_NC;

    if (gpio_port == ADC_SIG_NA)
        return IO_ERROR; /* signal not available */
    if (gpio_port == ADC_SIG_NC)
        return IO_OK; /* no need to configure signal */
    switch (gpio_port >> 5) {
        case 1: /* PORTA */
            pctl = (PORT_MemMapPtr) PORTA_BASE_PTR;
            break;
        case 2: /* PORTB */
            pctl = (PORT_MemMapPtr) PORTB_BASE_PTR;
            break;
        case 3: /* PORTC */
            pctl = (PORT_MemMapPtr) PORTC_BASE_PTR;
            break;
        case 4: /* PORTD */
            pctl = (PORT_MemMapPtr) PORTD_BASE_PTR;
            break;
        case 5: /* PORTE */
            pctl = (PORT_MemMapPtr) PORTE_BASE_PTR;
            break;
        /* There is no possibility to get other port from table */
    }
    pctl->PCR[gpio_port & 0x1F] &= ~PORT_PCR_MUX_MASK; /* set pin's multiplexer to analog */

    return IO_OK;
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

#if PE_LDD_VERSION
    /* Check if peripheral is not used by Processor Expert RTC_LDD component */
    if (PE_PeripheralUsed((uint32_t)RTC_BASE_PTR) == TRUE)    {
        /* IO Device used by PE Component*/
        return IO_ERROR;
    }
#endif

    /* Enable the clock gate to the RTC module. */
    SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;

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
    uint32_t dev_num
)
{
    SIM_MemMapPtr   sim = SIM_BASE_PTR;
    PORT_MemMapPtr  pctl;

    switch (dev_num)
    {
        case 0:
            /* Configure GPIOD for SPI0 peripheral function */
            pctl = (PORT_MemMapPtr)PORTA_BASE_PTR;

            pctl->PCR[14] = PORT_PCR_MUX(2);     /* SPI0.PCS0   */
            pctl->PCR[15] = PORT_PCR_MUX(2);     /* SPI0.SCK    */
            pctl->PCR[16] = PORT_PCR_MUX(2);     /* SPI0.SOUT   */
            pctl->PCR[17] = PORT_PCR_MUX(2);     /* SPI0.SIN    */

            /* Enable clock gate to SPI0 module */
            sim->SCGC6 |= SIM_SCGC6_SPI0_MASK;
            break;

        case 1:
            /* Configure GPIOE for SPI1 peripheral function     */
            pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;

            pctl->PCR[1] = PORT_PCR_MUX(2);     /* SPI1.SOUT   */
            pctl->PCR[2] = PORT_PCR_MUX(2);     /* SPI1.SCK    */
            pctl->PCR[3] = PORT_PCR_MUX(2);     /* SPI1.SIN    */
            pctl->PCR[4] = PORT_PCR_MUX(2);     /* SPI1.PCS0   */

            /* Enable clock gate to SPI1 module */
            sim->SCGC6 |= SIM_SCGC6_SPI1_MASK;
            break;

        case 2:
            /* Configure GPIOD for SPI2 peripheral function     */
            pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;

            pctl->PCR[11] = PORT_PCR_MUX(2);    /* SPI2.PCS0   */
            pctl->PCR[12] = PORT_PCR_MUX(2);    /* SPI2.SCK    */
            pctl->PCR[13] = PORT_PCR_MUX(2);    /* SPI2.SOUT   */
            pctl->PCR[14] = PORT_PCR_MUX(2);    /* SPI2.SIN    */
            pctl->PCR[15] = PORT_PCR_MUX(2);    /* SPI2.PCS1   */

            /* Enable clock gate to SPI2 module */
            sim->SCGC3 |= SIM_SCGC3_SPI2_MASK;
            break;

        default:
            /* do nothing if bad dev_num was selected */
            return -1;
    }

    return MQX_OK;

}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_esdhc_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to ESDHC
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_esdhc_io_init
(
    uint8_t  dev_num,
    uint16_t value
)
{
    SIM_MemMapPtr   sim  = SIM_BASE_PTR;
    PORT_MemMapPtr  pctl;


    switch (dev_num)
    {
        case 0:
            /* Configure GPIO for SDHC peripheral function */
            pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;
            pctl->PCR[0] = value & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* ESDHC.D1 */
            pctl->PCR[1] = value & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* ESDHC.D0 */
            pctl->PCR[2] = value & (PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK);                                          /* ESDHC.CLK */
            pctl->PCR[3] = value & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* ESDHC.CMD  */
            pctl->PCR[4] = value & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* ESDHC.D3 */
            pctl->PCR[5] = value & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* ESDHC.D2 */

            /* Enable clock gate to SDHC module */
            sim->SCGC3 |= SIM_SCGC3_SDHC_MASK;
            break;

        default:
            /* Do nothing if bad dev_num was selected */
            return -1;
    }

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_flexcan_io_init
* Returned Value   : 0 or 1 for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to FLEXCAN
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_flexcan_io_init
(
    uint8_t dev_num
)
{
    SIM_MemMapPtr   sim = SIM_BASE_PTR;
    PORT_MemMapPtr  pctl;

    OSC_CR |= OSC_CR_ERCLKEN_MASK;


    switch (dev_num)
    {
        case 0:
            /* Configure GPIO for FlexCAN 0 peripheral function */
            pctl = (PORT_MemMapPtr)PORTA_BASE_PTR;
            pctl->PCR[12] = (PORT_PCR_MUX(2)|PORT_PCR_DSE_MASK|PORT_PCR_PS_MASK | PORT_PCR_PE_MASK );    /* CAN0_TX.A12  */
            pctl->PCR[13] = (PORT_PCR_MUX(2)|PORT_PCR_DSE_MASK|PORT_PCR_PS_MASK | PORT_PCR_PE_MASK );    /* CAN0_RX.A13  */


            /* Enable clock gate to FlexCAN 0 module */
            sim->SCGC6 |= SIM_SCGC6_FLEXCAN0_MASK;

            break;
        case 1:
            /* Configure GPIO for FlexCAN 1 peripheral function */
            pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;
            pctl->PCR[24] = (PORT_PCR_MUX(2)|PORT_PCR_DSE_MASK);    /* CAN1_TX.E24  */
            pctl->PCR[25] = (PORT_PCR_MUX(2)|PORT_PCR_DSE_MASK);    /* CAN1_RX.E25  */


            /* Enable clock gate to FlexCAN 1 module */
            sim->SCGC3 |= SIM_SCGC3_FLEXCAN1_MASK;
            break;

        default:
            return -1;
    }
    return 0;
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
    uint32_t dev_num
)
{
    #define ALT2    0x2
    #define ALT6    0x6

    PORT_MemMapPtr  pctl;
    SIM_MemMapPtr   sim = SIM_BASE_PTR;

    switch (dev_num)
    {
        case 0:
            /* configure GPIO for I2C0 peripheral function */
            pctl = (PORT_MemMapPtr)PORTB_BASE_PTR;

            pctl->PCR[2] = PORT_PCR_MUX(ALT2) | PORT_PCR_ODE_MASK;
            pctl->PCR[3] = PORT_PCR_MUX(ALT2) | PORT_PCR_ODE_MASK;
            /* Enable SDA rising edge detection */
#if BSPCFG_ENABLE_LEGACY_II2C_SLAVE
            pctl->PCR[3] |= PORT_PCR_IRQC(0x09);
            pctl->PCR[3] |= PORT_PCR_ISF_MASK;
#endif
             /* Enable clock gate to I2C0 module */
            sim->SCGC4 |= SIM_SCGC4_I2C0_MASK;
            break;

        case 1:
            /* Configure GPIO for I2C1 peripheral function */
            pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;

            pctl->PCR[0] = PORT_PCR_MUX(ALT6) | PORT_PCR_ODE_MASK;
            pctl->PCR[1] = PORT_PCR_MUX(ALT6) | PORT_PCR_ODE_MASK;
            /* Enable SDA rising edge detection */
#if BSPCFG_ENABLE_LEGACY_II2C_SLAVE
            pctl->PCR[0] |= PORT_PCR_IRQC(0x09);
            pctl->PCR[0] |= PORT_PCR_ISF_MASK;
#endif
             /* Enable clock gate to I2C1 module */
            sim->SCGC4 |= SIM_SCGC4_I2C1_MASK;
            break;

        default:
            /* Do nothing if bad dev_num was selected */
            return -1;
    }

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific I/O initialization related to USB
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_usb_io_init
(
    void
)
{
#if PE_LDD_VERSION
    /* USB clock is configured using CPU component */

    /* Check if peripheral is not used by Processor Expert USB_LDD component */
     if (PE_PeripheralUsed((uint32_t)USB0_BASE_PTR) == TRUE) {
         /* IO Device used by PE Component*/
         return IO_ERROR;
     }

    /**
     * Workaround for Processor Expert as USB clock divider settings has been removed
     * from __pe_initialize_hardware() and Cpu_SetClockConfiguration() functions
     * Needs to be replaced by dynamic calculation of dividers.
     * SIM_CLKDIV2: USBDIV=1,USBFRAC=0
     */
    SIM_CLKDIV2 = (uint32_t)((SIM_CLKDIV2 & (uint32_t)~0x0DUL) | (uint32_t)0x02UL); /* Update USB clock prescalers */
#endif

    /* Configure USB to be clocked from PLL */
    SIM_SOPT2_REG(SIM_BASE_PTR) |= SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL_MASK;

    /* Enable USB-OTG IP clocking */
    SIM_SCGC4_REG(SIM_BASE_PTR) |= SIM_SCGC4_USBOTG_MASK;

    /* USB D+ and USB D- are standalone not multiplexed one-purpose pins */
    /* VREFIN for device is standalone not multiplexed one-purpose pin */
    return MQX_OK;
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
    /* Enable clock gating to all ports */
    SIM_SCGC5 |=   SIM_SCGC5_PORTA_MASK \
                 | SIM_SCGC5_PORTB_MASK \
                 | SIM_SCGC5_PORTC_MASK \
                 | SIM_SCGC5_PORTD_MASK \
                 | SIM_SCGC5_PORTE_MASK;

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_serial_rts_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to GPIO
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_serial_rts_init
(
    uint32_t device_index
)
{
   PORT_MemMapPtr           pctl;

   /* set pin to RTS functionality */
   switch( device_index )
   {
      case 3:
         pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;
         pctl->PCR[18] = 0 | PORT_PCR_MUX(3);
         break;
      default:
         /* not used on this board */
         break;
   }
   return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_ftfx_io_init
* Returned Value   : 0 for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to LCD
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_ftfx_io_init
(
    _mqx_uint device_index
)
{
    SIM_MemMapPtr sim = SIM_BASE_PTR;

    if (device_index > 0)
        return IO_ERROR;

    /* Clock the controller */
    sim->SCGC6 |= SIM_SCGC6_FTFL_MASK;

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_tss_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to I2S
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_tss_io_init
(
    void
)
{
    SIM_MemMapPtr sim = SIM_BASE_PTR;

    sim->SCGC5 |= SIM_SCGC5_TSI_MASK;   /* TSI clock enablement */
    sim->SCGC6 |= SIM_SCGC6_FTM1_MASK;

    /* Set Electrodes for TSI function */
    PORTB_PCR0 = PORT_PCR_MUX(0);
    PORTA_PCR4 = PORT_PCR_MUX(0);

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _bsp_serial_irda_tx_init
 * Returned Value   : MQX_OK or -1
 * Comments         :
 *    This function performs BSP-specific initialization related to IrDA
 *
 *END*----------------------------------------------------------------------*/

_mqx_int _bsp_serial_irda_tx_init(uint32_t device_index, bool enable)
{
    /* Kwikstik REV5 board does not support this function */
    return -1;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _bsp_serial_irda_rx_init
 * Returned Value   : MQX_OK or -1
 * Comments         :
 *    This function performs BSP-specific initialization related to IrDA
 *
 *END*----------------------------------------------------------------------*/

_mqx_int _bsp_serial_irda_rx_init(uint32_t device_index, bool enable)
{
    /* Kwikstik REV5 board does not support this function */
    return -1;
}

/* EOF */
