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

    /* Setup GPIO for UART devices */
    switch (dev_num)
    {
        case 0:
            pctl = (PORT_MemMapPtr)PORTA_BASE_PTR;
            if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE)
            {
                /* PTA1 as RX function (Alt.2) + drive strength */
                pctl->PCR[1] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
                /* PTA2 as TX function (Alt.2) + drive strength */
                pctl->PCR[2] = 0 | PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK;
            }
            if (flags & IO_PERIPHERAL_PIN_MUX_DISABLE)
            {
                /* PTA1 default */
                pctl->PCR[1] = 0;
                /* PTA2 default */
                pctl->PCR[2] = 0;
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
            pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;
            if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE)
            {
                /* PTE1 as RX function (Alt.3) + drive strength */
                pctl->PCR[1] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
                /* PTE0 as TX function (Alt.3) + drive strength */
                pctl->PCR[0] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
            }
            if (flags & IO_PERIPHERAL_PIN_MUX_DISABLE)
            {
                /* PTE0 default */
                pctl->PCR[0] = 0;
                /* PTE1 default */
                pctl->PCR[1] = 0;
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
    PORT_MemMapPtr pctl = NULL;

    #define ADC_SIG_PORTA   (0x01 << 5)
    #define ADC_SIG_PORTB   (0x02 << 5)
    #define ADC_SIG_PORTC   (0x03 << 5)
    #define ADC_SIG_PORTD   (0x04 << 5)
    #define ADC_SIG_PORTE   (0x05 << 5)
    #define ADC_SIG_NA      (0x00) /* signal not available */
    #define ADC_SIG_NC      (0x01) /* signal not configurable */

    /* Conversion table for ADC0x inputs, where x is 0 to 23, mux is defaultly "B" */
    const static uint8_t adc0_conv_table[] = {
        ADC_SIG_NC, //0 leave as default
        ADC_SIG_NA, //1 not implemented
        ADC_SIG_NA, //2 not implemented
        ADC_SIG_NC, //3 leave as default
        ADC_SIG_PORTC | 2, //4b
        ADC_SIG_PORTD | 1, //5b
        ADC_SIG_PORTD | 5, //6b
        ADC_SIG_PORTD | 6, //7b
        ADC_SIG_PORTB | 0, //8
        ADC_SIG_PORTB | 1, //9
        ADC_SIG_NA, //10 not implemented
        ADC_SIG_NA, //11 not implemented
        ADC_SIG_PORTB | 2, //12
        ADC_SIG_PORTB | 3, //13
        ADC_SIG_PORTC | 0, //14
        ADC_SIG_PORTC | 1, //15
        ADC_SIG_NA, //16 not implemented
        ADC_SIG_NA, //17 not implemented
        ADC_SIG_NA, //18 not implemented
        ADC_SIG_NC, //19 ADC0_DM0, leave as default
        ADC_SIG_NA, //20 not implemented
        ADC_SIG_NC, //21 ADC0_DM3, leave as default
        ADC_SIG_NC, //22 VREF, leave as default
        ADC_SIG_NC, //23 ADC0_SE23, leave as default
        ADC_SIG_NA, //24 not implemented
        ADC_SIG_NA, //25 not implemented
        //below: use ADC_SIG_NC (leave as default)
    };

    if (ADC_GET_DIFF(source) && ch > 3)
        return IO_ERROR; /* signal not available */

    if (ADC_GET_DIFF(source) == 0 && ch == 2)
        return IO_ERROR; /* channel 2 (PGA) can be used only as a diff pair */

    if (ch < 26) {
		/* Get result for module 0 */
	   gpio_port = adc0_conv_table[ch];
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
            /* Configure GPIOD for DSPI0 peripheral function */
            pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;

            pctl->PCR[0] = PORT_PCR_MUX(2);     /* DSPI0.PCS0   */
            pctl->PCR[1] = PORT_PCR_MUX(2);     /* DSPI0.SCK    */
            pctl->PCR[2] = PORT_PCR_MUX(2);     /* DSPI0.SOUT   */
            pctl->PCR[3] = PORT_PCR_MUX(2);     /* DSPI0.SIN    */

            /* Enable clock gate to DSPI0 module */
            sim->SCGC6 |= SIM_SCGC6_SPI0_MASK;
            break;

         default:
            /* do nothing if bad dev_num was selected */
            return -1;
    }

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
    uint32_t dev_num
)
{
    #define ALT2 0x2

    PORT_MemMapPtr  pctl;
    SIM_MemMapPtr sim = SIM_BASE_PTR;

    switch (dev_num)
    {
        case 0:
            pctl = (PORT_MemMapPtr)PORTB_BASE_PTR;

            pctl->PCR[2] = PORT_PCR_MUX(ALT2) | PORT_PCR_ODE_MASK;  /* I2C0.SCL */
            pctl->PCR[3] = PORT_PCR_MUX(ALT2) | PORT_PCR_ODE_MASK;  /* I2C0.SDA */
            /* Enable SDA rising edge detection */
#if BSPCFG_ENABLE_LEGACY_II2C_SLAVE
            pctl->PCR[3] |= PORT_PCR_IRQC(0x09);
            pctl->PCR[3] |= PORT_PCR_ISF_MASK;
#endif
            sim->SCGC4 |= SIM_SCGC4_I2C0_MASK;

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

    /** If we need the USB working as a host we have to provide
     ** power to the USB connector. To do this PTC9 has to be set
     ** as a GPIO output in high level.
     ** If USB should work as a device, the J26 jumper should be removed.
     */
    PORT_PCR_REG(PORTC_BASE_PTR, 9) = PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK;
    GPIO_PDDR_REG(PTC_BASE_PTR) |= 0x00000200; // PTC9 as output
    GPIO_PDOR_REG(PTC_BASE_PTR) |= 0x00000200; // PTC9 in high level

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
	/* not used on this board */

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
* Function Name    : _bsp_i2s_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to I2S
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_i2s_io_init
(
    uint32_t dev_num
)
{
    #define ALT4 0x04
    #define ALT6 0x06

    PORT_MemMapPtr  pctl;
    SIM_MemMapPtr sim = SIM_BASE_PTR;

    switch (dev_num)
    {
        case 0:
            pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;
            /* PTC0 - I2S0_TXD */
            /* TODO: Check this - there is no I2S0_TDX on PTC0 in K20 RM Rev. 2 */
            pctl->PCR[0] = PORT_PCR_MUX(ALT6);
            /* PTC11 - I2S0_RXD */
            pctl->PCR[11]  = PORT_PCR_MUX(ALT4);
            /* PTC8 - I2S0_MCLK */
            pctl->PCR[8]  = PORT_PCR_MUX(ALT4);

            pctl = (PORT_MemMapPtr)PORTB_BASE_PTR;
            /* PTB18 - I2S0_TX_BCLK */
            pctl->PCR[18] = PORT_PCR_MUX(ALT4);
            /* PTB19 - I2S0_TX_FS */
            pctl->PCR[19] = PORT_PCR_MUX(ALT4);

            sim->SCGC6 |= SIM_SCGC6_I2S_MASK;

            break;
        default:
            /* Do nothing if bad dev_num was selected */
            return -1;
    }

    return MQX_OK;

}

_mqx_int _bsp_sai_io_init
(
    uint32_t dev_num
)
{
    switch(dev_num)
    {
        case 0:
            /* Enable system clock to the I2S module */
            SIM_SCGC6 |= SIM_SCGC6_I2S_MASK;

            /* Enable SSI pins */
            PORTC_PCR8 |= PORT_PCR_MUX(0x04);                   /* Configure port for MCLK output */

            /* GPIO for SSI0_BLCK */
            PORTB_PCR18 |= PORT_PCR_MUX(0x04);                  /* Configure Port for TX Bit Clock */

            /* GPIO for SSI0_FS */
            PORTB_PCR19 |= PORT_PCR_MUX(0x04);                  /* Configure port for TX Frame Sync */

            /* GPIO for SSI0_XD */
            // PORTC_PCR0 |= PORT_PCR_MUX(0x06);                   /* Configure port for TX Data Channel 1 */
            PORTC_PCR1 |= PORT_PCR_MUX(0x06);                   /* Configure port for TX Data Channel 0 */
            PORTC_PCR11 |= PORT_PCR_MUX(0x04);                  /* Configure port for RX Data */

            break;
        default:
            break;
    }

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
    PORT_MemMapPtr pctl;
    SIM_MemMapPtr sim = SIM_BASE_PTR;

    /* Only UART0 support IrDA transmit */
    if (0 != device_index)
    {
        return -1;
    }
    if (TRUE == enable) 
    {
        /* Enable drive strength for IrDA TX pin (PTD7)*/
        sim->SOPT2 |= SIM_SOPT2_PTD7PAD_MASK;

        /* PTD7 as UART TX function (Alt.3) + drive strength */
        pctl = (PORT_MemMapPtr) PORTD_BASE_PTR;
        pctl->PCR[7] = PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
    } 
    return MQX_OK;
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
    PORT_MemMapPtr pctl;
    SIM_MemMapPtr sim = SIM_BASE_PTR;
    CMP_MemMapPtr cmp = CMP0_BASE_PTR;

    if (TRUE == enable) 
    {
        /* Enable comparator module */
        sim->SCGC4 |= SIM_SCGC4_CMP_MASK;

        /* Select RX source for UART */
        switch (device_index) {
        case 0:
            sim->SOPT5 &= ~SIM_SOPT5_UART0RXSRC_MASK;
            /* Select CMP0 as RX source for UART0*/
            sim->SOPT5 |= SIM_SOPT5_UART0RXSRC(1);
            break;
        case 1:
            sim->SOPT5 &= ~SIM_SOPT5_UART1RXSRC_MASK
            /* Select CMP0 as RX source for UART1*/;
            sim->SOPT5 |= SIM_SOPT5_UART1RXSRC(1);
        default:
            /* Do not support source select for other UART module*/
            return -1;
        }

        /* Setting Comparator module: Minus pin is DAC source, Plus pin is IrDA RX*/
        cmp->MUXCR = CMP_MUXCR_MSEL(7) | CMP_MUXCR_PSEL(1);
        /* Enable DAC input comparator */
        cmp->DACCR = CMP_DACCR_DACEN_MASK | CMP_DACCR_VOSEL(BSP_SERIAL_IRDA_DAC_VREF)
                | CMP_DACCR_VRSEL_MASK;

        /* PTC7 as CPM IN function (Alt.0) */
        pctl = (PORT_MemMapPtr) PORTC_BASE_PTR;
        pctl->PCR[7] = PORT_PCR_MUX(0);

        /* Enable Comparator module */
        cmp->CR1 |= CMP_CR1_EN_MASK | CMP_CR1_COS_MASK
                | CMP_CR1_OPE_MASK | CMP_CR1_PMODE_MASK;
    } 
    else 
    {
        switch (device_index) {
        case 0:
            sim->SOPT5 &= ~SIM_SOPT5_UART0RXSRC_MASK;
            /* Select UART0_RX pin as RX source for UART0*/
            sim->SOPT5 |= SIM_SOPT5_UART0RXSRC(0);
            break;
        case 1:
            sim->SOPT5 &= ~SIM_SOPT5_UART1RXSRC_MASK
            /* Select UART1_RX pin as RX source for UART1*/;
            sim->SOPT5 |= SIM_SOPT5_UART1RXSRC(0);
        default:
            /* Do not support source select for other UART module*/
            return -1;
        }
        /* Disable comparator module */
        sim->SCGC4 &= ~SIM_SCGC4_CMP_MASK;
    }
    return MQX_OK;
}
/* EOF */
