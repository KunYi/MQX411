/*HEADER**********************************************************************
*
* Copyright 2014 Freescale Semiconductor, Inc.
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
            pctl = (PORT_MemMapPtr)PORTB_BASE_PTR;
            if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE)
            {
                /* PTB16 as RX function (Alt.3) + drive strength */
                pctl->PCR[16] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
                /* PTB17 as TX function (Alt.3) + drive strength */
                pctl->PCR[17] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
            }
            if (flags & IO_PERIPHERAL_PIN_MUX_DISABLE)
            {
                /* PTB16 default */
                pctl->PCR[16] = 0;
                /* PTB17 default */
                pctl->PCR[17] = 0;
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
            pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;
            if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE)
            {
                /* PTC16 as RX function (Alt.3) + drive strength */
                pctl->PCR[16] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
                /* PTC17 as TX function (Alt.3) + drive strength */
                pctl->PCR[17] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
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
            pctl = (PORT_MemMapPtr) PORTC_BASE_PTR;
            if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE) {

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
#if BSPCFG_ENABLE_TTYF_HW_SIGNALS
                /* PTE11 as RTS function (Alt.3) + drive strength */
                pctl->PCR[11] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
                /* PTE10 as CTS function (Alt.3) + drive strength */
                pctl->PCR[10] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
#endif
            }
            if (flags & IO_PERIPHERAL_PIN_MUX_DISABLE)
            {
                /* PTE8 default */
                pctl->PCR[8] = 0;
                /* PTE9 default */
                pctl->PCR[9] = 0;
#if BSPCFG_ENABLE_TTYF_HW_SIGNALS
                /* PTE11 default */
                pctl->PCR[11] = 0;
                /* PTE10 default */
                pctl->PCR[10] = 0;
#endif
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
    switch (adc_num)
    {
        case 0:
            sim_ptr->SCGC6 |= SIM_SCGC6_ADC0_MASK;
            break;
        case 1:
            sim_ptr->SCGC3 |= SIM_SCGC3_ADC1_MASK;
            break;
        default:
            return IO_ERROR;
    }

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
    
    #define ADC_SIG_PORT_SHIFT (5)
    #define ADC_SIG_PORTA   (0x01 << ADC_SIG_PORT_SHIFT)
    #define ADC_SIG_PORTB   (0x02 << ADC_SIG_PORT_SHIFT)
    #define ADC_SIG_PORTC   (0x03 << ADC_SIG_PORT_SHIFT)
    #define ADC_SIG_PORTD   (0x04 << ADC_SIG_PORT_SHIFT)
    #define ADC_SIG_PORTE   (0x05 << ADC_SIG_PORT_SHIFT)
    #define ADC_SIG_PORTF   (0x06 << ADC_SIG_PORT_SHIFT)
    #define ADC_SIG_NA      (0x00) /* signal not available */
    #define ADC_SIG_NC      (0x01) /* signal not configurable */



/* Conversion table for ADC0x inputs, where x is 0 to 23, mux is defaultly "B" OR "X" */
    const static uint8_t adc0_conv_table_bx[] = {
        ADC_SIG_NC,         /* 0 leave as default */ 
        ADC_SIG_NC,         /* 1 leave as default */
        ADC_SIG_NC,         /* 2 leave as default */
        ADC_SIG_NC,         /* 3 leave as default */
        ADC_SIG_PORTC | 2,  /* ADC0_SE4b */
        ADC_SIG_PORTD | 1,  /* ADC0_SE5b */
        ADC_SIG_PORTD | 5,  /* ADC0_SE6b */
        ADC_SIG_PORTD | 6,  /* ADC0_SE7b */
        ADC_SIG_PORTB | 0,  /* ADC0_SE8 */
        ADC_SIG_PORTB | 1,  /* ADC0_SE9 */
        ADC_SIG_PORTA | 7,  /* ADC0_SE10 */
        ADC_SIG_PORTA | 8,  /* ADC0_SE11 */
        ADC_SIG_PORTB | 2,  /* ADC0_SE12 */
        ADC_SIG_PORTB | 3,  /* ADC0_SE13 */
        ADC_SIG_PORTC | 0,  /* ADC0_SE14 */
        ADC_SIG_PORTC | 1,  /* ADC0_SE15 */
        ADC_SIG_NC,         /* ADC0_SE16 */
        ADC_SIG_PORTE | 24, /* ADC0_SE17 */
        ADC_SIG_PORTE | 25, /* ADC0_SE18 */
        ADC_SIG_NC,         /* 19 leave as default */
        ADC_SIG_NC,         /* 20 leave as default */
        ADC_SIG_NC,         /* ADC0_SE21 */
        ADC_SIG_NC,         /* ADC0_SE22 */
        ADC_SIG_NC,         /* DAC0_OUT/CMP1_IN3/ADC0_SE23 */
        ADC_SIG_NA,         /* 24 not implemented */
        ADC_SIG_NA,         /* 25 not implemented */       
    };
    
    /* Conversion table for ADC1x inputs, where x is 0 to 23, mux is defaultly "B" OR "X"*/
    const static uint8_t adc1_conv_table_bx[] = {
        ADC_SIG_NC,         /* 0 leave as default */
        ADC_SIG_NC,         /* 1 leave as default */
        ADC_SIG_NC,         /* 2 leave as default */
        ADC_SIG_NC,         /* 3 leave as default */
        ADC_SIG_PORTC | 8,  /* ADC1_SE4b */
        ADC_SIG_PORTC | 9,  /* ADC1_SE5b */
        ADC_SIG_PORTC | 10, /* ADC1_SE6b */
        ADC_SIG_PORTC | 11, /* ADC1_SE7b */
        ADC_SIG_PORTB | 0,  /* ADC1_SE8 */
        ADC_SIG_PORTB | 1,  /* ADC1_SE9 */
        ADC_SIG_PORTB | 4,  /* ADC1_SE10 */
        ADC_SIG_PORTB | 5,  /* ADC1_SE11 */
        ADC_SIG_PORTB | 6,  /* ADC1_SE12 */
        ADC_SIG_PORTB | 7,  /* ADC1_SE13 */
        ADC_SIG_PORTB | 10, /* ADC1_SE14 */
        ADC_SIG_PORTB | 11, /* ADC1_SE15 */
        ADC_SIG_NC,         /* ADC1_SE16 */
        ADC_SIG_PORTA | 17, /* ADC1_SE17 */
        ADC_SIG_NC,         /* VREF_OUT/CMP1_IN5/CMP0_IN5/ADC1_SE18 */
        ADC_SIG_NC,         /* 19 leave as default */
        ADC_SIG_NC,         /* 20 leave as default */
        ADC_SIG_NA,         /* 21 not implemented */
        ADC_SIG_NA,         /* 22 not implemented */
        ADC_SIG_NC,         /* DAC1_OUT/CMP0_IN4/CMP2_IN3/ADC1_SE23 */
        ADC_SIG_NA,         /* 24 not implemented */
        ADC_SIG_NA,         /* 25 not implemented */  
    };
    /* Conversion table for ADC1x inputs, where x is 4 to 7, mux is defaultly "A" */
    const static uint8_t adc1_conv_table_a[] = {
        ADC_SIG_PORTE | 0, /* ADC1_SE4a */
        ADC_SIG_PORTE | 1, /* ADC1_SE5a */
        ADC_SIG_PORTE | 2, /* ADC1_SE6a */
        ADC_SIG_PORTE | 3  /* ADC1_SE7a */
    };


    if (ADC_GET_DIFF(source) && (ch > 3))
    {
        return IO_ERROR; /* signal not available */
    }

    switch(ADC_GET_MODULE(source))
    {
        case ADC_SOURCE_MODULE(1): /* ADC0 */
            if(ch < 26)
            {
                gpio_port = adc0_conv_table_bx[ch];
            }
            else
            {
                gpio_port = ADC_SIG_NC;
            }
            break;
        case ADC_SOURCE_MODULE(2): /* ADC1 */
            if (ADC_GET_MUXSEL(source) == ADC_SOURCE_MUXSEL_A)
            {
                if(ch < 4 || ch > 7)
                {
                    gpio_port = ADC_SIG_NA; /* channel does not exist */
                }
                else
                {
                    gpio_port = adc1_conv_table_a[ch - 4];
                }
            }
            else /*ADC_SOURCE_MUXSEL_B OR ADC_SOURCE_MUXSEL_X*/
            {
                if(ch < 26)
                {
                    gpio_port = adc1_conv_table_bx[ch];
                }
                else
                {
                    gpio_port = ADC_SIG_NC;
                }
            }
            break;
        default :
            return IO_ERROR; /*ADC0 - ADC3 only*/
    }

    if (gpio_port == ADC_SIG_NA)
    {
        return IO_ERROR; /* signal not available */
    }
    
    if (gpio_port == ADC_SIG_NC)
    {
        return IO_OK; /* no need to configure signal */
    }
    
    switch (gpio_port >> ADC_SIG_PORT_SHIFT)
    {
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

        case 1:
            /* Configure GPIOD for DSPI1 peripheral function    */
            pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;

            pctl->PCR[4] = PORT_PCR_MUX(7);     /* DSPI1.PCS0   */
            pctl->PCR[5] = PORT_PCR_MUX(7);     /* DSPI1.SCK    */
            pctl->PCR[6] = PORT_PCR_MUX(7);     /* DSPI1.SOUT   */
            pctl->PCR[7] = PORT_PCR_MUX(7);     /* DSPI1.SIN    */

            /* Enable clock gate to DSPI1 module */
            sim->SCGC6 |= SIM_SCGC6_SPI1_MASK;
            break;

        case 2:
            /* Configure GPIOB for DSPI2 peripheral function     */
            pctl = (PORT_MemMapPtr)PORTB_BASE_PTR;

            pctl->PCR[20] = PORT_PCR_MUX(1);    /* DSPI2.PCS0   */
            pctl->PCR[21] = PORT_PCR_MUX(1);    /* DSPI2.SCK    */
            pctl->PCR[22] = PORT_PCR_MUX(1);    /* DSPI2.SOUT   */
            pctl->PCR[23] = PORT_PCR_MUX(1);    /* DSPI2.SIN    */

            /* Enable clock gate to DSPI2 module */
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

            pctl->PCR[0] = value & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* ESDHC.D1  */
            pctl->PCR[1] = value & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* ESDHC.D0  */
            pctl->PCR[2] = value & (PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK);                                          /* ESDHC.CLK */
            pctl->PCR[3] = value & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* ESDHC.CMD */
            pctl->PCR[4] = value & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* ESDHC.D3  */
            pctl->PCR[5] = value & (PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK);    /* ESDHC.D2  */

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
    volatile CAN_MemMapPtr  can_reg_ptr;
    SIM_MemMapPtr   sim = SIM_BASE_PTR;
    PORT_MemMapPtr  pctl;

    OSC_CR |= OSC_CR_ERCLKEN_MASK;


    switch (dev_num)
    {
        case 0:
            /* Configure GPIO for FlexCAN 0 peripheral function */
            pctl = (PORT_MemMapPtr)PORTB_BASE_PTR;
            pctl->PCR[18] = (PORT_PCR_MUX(2)|PORT_PCR_DSE_MASK);    /* CAN0_TX.A12  */
            pctl->PCR[19] = (PORT_PCR_MUX(2)|PORT_PCR_DSE_MASK);    /* CAN0_RX.A13  */

            /* Enable clock gate to FlexCAN 0 module */
            sim->SCGC6 |= SIM_SCGC6_FLEXCAN0_MASK;
            
            /* Select the bus clock as CAN engine clock source */
            can_reg_ptr = _bsp_get_flexcan_base_address(0);
            can_reg_ptr->CTRL1 |= CAN_CTRL1_CLKSRC_MASK;

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

_mqx_int _bsp_i2c_io_init(uint32_t dev_num) {

   PORT_MemMapPtr pctl;
   SIM_MemMapPtr sim = SIM_BASE_PTR;

   switch (dev_num) {
   case 0:
      pctl = (PORT_MemMapPtr) PORTE_BASE_PTR;

      pctl->PCR[24] = PORT_PCR_MUX(5) | PORT_PCR_ODE_MASK;
      pctl->PCR[25] = PORT_PCR_MUX(5) | PORT_PCR_ODE_MASK;

      sim->SCGC4 |= SIM_SCGC4_I2C0_MASK;

      break;
   case 1:
      pctl = (PORT_MemMapPtr) PORTC_BASE_PTR;

      pctl->PCR[10] = PORT_PCR_MUX(2) | PORT_PCR_ODE_MASK;
      pctl->PCR[11] = PORT_PCR_MUX(2) | PORT_PCR_ODE_MASK;

      sim->SCGC4 |= SIM_SCGC4_I2C1_MASK;

      break;
   case 2:
      pctl = (PORT_MemMapPtr) PORTA_BASE_PTR;

      pctl->PCR[11] = PORT_PCR_MUX(5) | PORT_PCR_ODE_MASK;
      pctl->PCR[12] = PORT_PCR_MUX(5) | PORT_PCR_ODE_MASK;

      sim->SCGC4 |= SIM_SCGC1_I2C2_MASK;

      break;
   default:
      /* Do nothing if bad dev_num was selected */
      return -1;
   }
   return MQX_OK;

}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_enet_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific initialization related to ENET
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_enet_io_init
(
    uint32_t device
)
{
    PORT_MemMapPtr pctl;
    SIM_MemMapPtr  sim  = (SIM_MemMapPtr)SIM_BASE_PTR;

    pctl = (PORT_MemMapPtr)PORTA_BASE_PTR;
    pctl->PCR[12] = PORT_PCR_MUX(4);     /* PTA12, RMII0_RXD1/MII0_RXD1      */
    pctl->PCR[13] = PORT_PCR_MUX(4);     /* PTA13, RMII0_RXD0/MII0_RXD0      */
    pctl->PCR[14] = PORT_PCR_MUX(4);     /* PTA14, RMII0_CRS_DV/MII0_RXDV    */
    pctl->PCR[15] = PORT_PCR_MUX(4);     /* PTA15, RMII0_TXEN/MII0_TXEN      */
    pctl->PCR[16] = PORT_PCR_MUX(4);     /* PTA16, RMII0_TXD0/MII0_TXD0      */
    pctl->PCR[17] = PORT_PCR_MUX(4);     /* PTA17, RMII0_TXD1/MII0_TXD1      */


    pctl = (PORT_MemMapPtr)PORTB_BASE_PTR;
    /* PTB0, RMII0_MDIO/MII0_MDIO, internal pull-up enabled (workaround for RevA hw issue) */
    pctl->PCR[0] = PORT_PCR_MUX(4) | PORT_PCR_ODE_MASK | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK;
    pctl->PCR[1] = PORT_PCR_MUX(4);                     /* PTB1, RMII0_MDC/MII0_MDC     */

#if ENETCFG_SUPPORT_PTP
    pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;
    pctl->PCR[16+MACNET_PTP_TIMER] = PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK; /* PTC16, ENET0_1588_TMR0   */
#endif

    /* Enable clock for ENET module */
    sim->SCGC2 |= SIM_SCGC2_ENET_MASK;

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
#if BSPCFG_USB_USE_IRC48M

    /*
    * Configure SIM_CLKDIV2: USBDIV = 0, USBFRAC = 0
    */
    SIM_CLKDIV2 = (uint32_t)0x0UL; /* Update USB clock prescalers */

    /* Configure USB to be clocked from IRC 48MHz */
    SIM_SOPT2_REG(SIM_BASE_PTR) |= SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL(3);

    /* Enable USB-OTG IP clocking */
    SIM_SCGC4_REG(SIM_BASE_PTR) |= SIM_SCGC4_USBOTG_MASK;
    /* Enable IRC 48MHz for USB module */
    USB0_CLK_RECOVER_IRC_EN = 0x03;
#else
    /* Configure USB to be clocked from PLL */
    SIM_SOPT2_REG(SIM_BASE_PTR) |= SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL(1);

    /* Enable USB-OTG IP clocking */
    SIM_SCGC4_REG(SIM_BASE_PTR) |= SIM_SCGC4_USBOTG_MASK;

        /* USB D+ and USB D- are standalone not multiplexed one-purpose pins */
        /* VREFIN for device is standalone not multiplexed one-purpose pin */
#endif

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_dev_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific I/O initialization related to USB
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_usb_dev_io_init
(
    struct usb_dev_if_struct *dev_if
)
{
    if (dev_if->DEV_INIT_PARAM == &_khci0_dev_init_param) {
        _bsp_usb_io_init();
    }
    else {
        return IO_ERROR; //unknown controller
    }

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_host_io_init
* Returned Value   : MQX_OK or -1
* Comments         :
*    This function performs BSP-specific I/O initialization related to USB
*
*END*----------------------------------------------------------------------*/

_mqx_int _bsp_usb_host_io_init
(
    struct usb_host_if_struct *dev_if
)
{
    if (dev_if->HOST_INIT_PARAM == &_khci0_host_init_param) {
        _bsp_usb_io_init();
    }
    else {
        return IO_ERROR; /* unknown controller */
    }

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

    /* Avoid NMI functionality for the PTA4 pin routed to SW3 button */
    PORTA_PCR4 = (uint32_t)((PORTA_PCR4 & (uint32_t)~(uint32_t)(PORT_PCR_MUX_MASK)) | (uint32_t)(PORT_PCR_MUX(0x01)));

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
      case 0:
         pctl = (PORT_MemMapPtr)PORTA_BASE_PTR;
         pctl->PCR[17] = 0 | PORT_PCR_MUX(3);
         break;
      case 1:
         pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;
         pctl->PCR[3] = 0 | PORT_PCR_MUX(3);
         break;
      case 2:
         pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;
         pctl->PCR[0] = 0 | PORT_PCR_MUX(3);
         break;
      case 3:
         pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;
         pctl->PCR[18] = 0 | PORT_PCR_MUX(3);
         break;
      case 4:
         pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;
         pctl->PCR[27] = 0 | PORT_PCR_MUX(3);
         break;
      case 5:
         pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;
         pctl->PCR[10] = 0 | PORT_PCR_MUX(3);
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
    sim->SCGC6 |= SIM_SCGC6_FTF_MASK;

    return MQX_OK;
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
    uint32_t dev_num
)
{
    switch(dev_num)
    {
        case 0:
            /* Enable system clock to the I2S module */
            SIM_SCGC6 |= SIM_SCGC6_I2S_MASK;
            
            /* Enable I2S pins */
            PORTC_PCR8 |= PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK;   /* Configure port for MCLK output */
            
            /* GPIO for I2S0_RX_BCLK */
            PORTC_PCR9 |= PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK;  /* Configure Port for RX Bit Clock */
            
            /* GPIO for I2S0_TX_BCLK */
            PORTB_PCR18 |= PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK;  /* Configure Port for TX Bit Clock */
            
            /* GPIO for I2S0_TX_FS */
            PORTB_PCR19 |= PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK;  /* Configure port for TX Frame Sync */
            
            /* GPIO for SSI0_XD */
            PORTC_PCR7 |= PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK;   /* Configure port for TX Data */
            PORTC_PCR5 |= PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK;   /* Configure port for RX Data */
            
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
    /* This board does not support this function */
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
    /* This board does not support this function */
    return -1;
}

/* EOF */
