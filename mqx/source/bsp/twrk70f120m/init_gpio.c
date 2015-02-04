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
#include <bsp_prv.h>

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
            pctl = (PORT_MemMapPtr)PORTF_BASE_PTR;
            if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE)
            {
                /* PTF17 as RX function (Alt.4) + drive strength */
                pctl->PCR[17] = 0 | PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK;
                /* PTF18 as TX function (Alt.4) + drive strength */
                pctl->PCR[18] = 0 | PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK;
            }
            if (flags & IO_PERIPHERAL_PIN_MUX_DISABLE)
            {
                /* PTF17 default */
                pctl->PCR[17] = 0;
                /* PTF18 default */
                pctl->PCR[18] = 0;
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
            pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;
            if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE)
            {
                /* PTE16 as TX function (Alt.3) + drive strength */
                pctl->PCR[16] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
                /* PTE17 as RX function (Alt.3) + drive strength */
                pctl->PCR[17] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
            }
            if (flags & IO_PERIPHERAL_PIN_MUX_DISABLE)
            {
                /* PTE16 default */
                pctl->PCR[16] = 0;
                /* PTE17 default */
                pctl->PCR[17] = 0;
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
            pctl = (PORT_MemMapPtr)PORTF_BASE_PTR;
            if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE)
            {
                /* PTF7 as RX function (Alt.4) + drive strength */
                pctl->PCR[7] = 0 | PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK;
                /* PTF8 as TX function (Alt.4) + drive strength */
                pctl->PCR[8] = 0 | PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK;
            }
            if (flags & IO_PERIPHERAL_PIN_MUX_DISABLE)
            {
                /* PTF7 default */
                pctl->PCR[7] = 0;
                /* PTC17 default */
                pctl->PCR[8] = 0;
                // /* PTC18 default */
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
            pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;
            if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE)
            {
                /* PTE25 as RX function (Alt.3) + drive strength */
                pctl->PCR[25] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
                /* PTE24 as TX function (Alt.3) + drive strength */
                pctl->PCR[24] = 0 | PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
            }
            if (flags & IO_PERIPHERAL_PIN_MUX_DISABLE)
            {
                /* PTE25 default */
                pctl->PCR[25] = 0;
                /* PTE24 default */
                pctl->PCR[24] = 0;
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
            pctl = (PORT_MemMapPtr)PORTF_BASE_PTR;
            if (flags & IO_PERIPHERAL_PIN_MUX_ENABLE)
            {
                /* PTF20 as TX function (Alt.4) + drive strength */
                pctl->PCR[20] = 0 | PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK;
                /* PTF19 as RX function (Alt.4) + drive strength */
                pctl->PCR[19] = 0 | PORT_PCR_MUX(4) | PORT_PCR_DSE_MASK;
            }
            if (flags & IO_PERIPHERAL_PIN_MUX_DISABLE)
            {
                /* PTF20 default */
                pctl->PCR[20] = 0;
                /* PTF19 default */
                pctl->PCR[19] = 0;
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
        case 2:
            sim_ptr->SCGC6 |= SIM_SCGC6_ADC2_MASK;
            break;
        case 3:
            sim_ptr->SCGC3 |= SIM_SCGC3_ADC3_MASK;
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
    PORT_MemMapPtr pctl;

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
        ADC_SIG_PORTC | 2,  /* ADC0_SE4b/CMP1_IN0/TSI0_CH15 */
        ADC_SIG_PORTD | 1,  /* ADC0_SE5b */
        ADC_SIG_PORTD | 5,  /* ADC0_SE6b */
        ADC_SIG_PORTD | 6,  /* ADC0_SE7b */
        ADC_SIG_PORTB | 0,  /* ADC0_SE8/ADC1_SE8/ADC2_SE8/ADC3_SE8/TSI0_CH0 */
        ADC_SIG_PORTB | 1,  /* ADC0_SE9/ADC1_SE9/ADC2_SE9/ADC3_SE9/TSI0_CH6 */
        ADC_SIG_PORTA | 7,  /* ADC0_SE10 */
        ADC_SIG_PORTA | 8,  /* ADC0_SE11 */
        ADC_SIG_PORTB | 2,  /* ADC0_SE12/TSI0_CH7 */
        ADC_SIG_PORTB | 3,  /* ADC0_SE13/TSI0_CH8 */
        ADC_SIG_PORTC | 0,  /* ADC0_SE14/TSI0_CH13 */
        ADC_SIG_PORTC | 1,  /* ADC0_SE15/TSI0_CH14 */
        ADC_SIG_NC,         /* ADC0_SE16/CMP1_IN2/ADC0_SE21 */
        ADC_SIG_PORTE | 24, /* ADC0_SE17/EXTAL1 */
        ADC_SIG_PORTE | 25, /* ADC0_SE18/XTAL1 */
        ADC_SIG_NC,         /* 19 leave as default */
        ADC_SIG_NC,         /* 20 leave as default */
        ADC_SIG_NC,         /* ADC0_SE16/CMP1_IN2/ADC0_SE21 */
        ADC_SIG_NC,         /* ADC1_SE16/CMP2_IN2/ADC0_SE22 */
        ADC_SIG_NC,         /* DAC0_OUT/CMP1_IN3/ADC0_SE23 */
        ADC_SIG_NA,         /* 24 not implemented */
        ADC_SIG_NA,         /* 25 not implemented */
    };

    /* Conversion table for ADC0x inputs, where x is 4 to 7, mux is defaultly "A" */
    const static uint8_t adc0_conv_table_a[] = {
        ADC_SIG_PORTE | 16, /* ADC0_SE4a */
        ADC_SIG_PORTE | 17, /* ADC0_SE5a */
        ADC_SIG_PORTE | 18, /* ADC0_SE6a */
        ADC_SIG_PORTE | 19  /* ADC0_SE7a */
    };

    /* Conversion table for ADC1x inputs, where x is 0 to 23, mux is defaultly "B" OR "X"*/
    const static uint8_t adc1_conv_table_bx[] = {
        ADC_SIG_NC,         /* 0 leave as default */
        ADC_SIG_NC,         /* 1 leave as default */
        ADC_SIG_NC,         /* 2 leave as default */
        ADC_SIG_NC,         /* 3 leave as default */
        ADC_SIG_PORTC | 8,  /* ADC1_SE4b/CMP0_IN2 */
        ADC_SIG_PORTC | 9,  /* ADC1_SE5b/CMP0_IN3 */
        ADC_SIG_PORTC | 10, /* ADC1_SE6b */
        ADC_SIG_PORTC | 11, /* ADC1_SE7b */
        ADC_SIG_PORTB | 0,  /* ADC0_SE8/ADC1_SE8/ADC2_SE8/ADC3_SE8/TSI0_CH0 */
        ADC_SIG_PORTB | 1,  /* ADC0_SE9/ADC1_SE9/ADC2_SE9/ADC3_SE9/TSI0_CH6 */
        ADC_SIG_PORTB | 4,  /* ADC1_SE10 */
        ADC_SIG_PORTB | 5,  /* ADC1_SE11 */
        ADC_SIG_PORTB | 6,  /* ADC1_SE12 */
        ADC_SIG_PORTB | 7,  /* ADC1_SE13 */
        ADC_SIG_PORTB | 10, /* ADC1_SE14 */
        ADC_SIG_PORTB | 11, /* ADC1_SE15 */
        ADC_SIG_NC,         /* ADC1_SE16/CMP2_IN2/ADC0_SE22 */
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
    /* Conversion table for ADC2x inputs, where x is 0 to 23, mux is defaultly "B" OR "X"*/
    const static uint8_t adc2_conv_table_bx[] = {
        ADC_SIG_NC,         /* 0 leave as default */
        ADC_SIG_NA,         /* 1 not implemented */
        ADC_SIG_NC,         /* 2 leave as default */
        ADC_SIG_NC,         /* 3 leave as default */
        ADC_SIG_PORTF | 4,  /* ADC2_SE4b */
        ADC_SIG_PORTF | 5,  /* ADC2_SE5b */
        ADC_SIG_PORTF | 6,  /* ADC2_SE6b */
        ADC_SIG_PORTF | 7,  /* ADC2_SE7b */
        ADC_SIG_PORTB | 0,  /* ADC0_SE8/ADC1_SE8/ADC2_SE8/ADC3_SE8/TSI0_CH0 */
        ADC_SIG_PORTB | 1,  /* ADC0_SE9/ADC1_SE9/ADC2_SE9/ADC3_SE9/TSI0_CH6 */
        ADC_SIG_PORTF | 1,  /* ADC2_SE10 */
        ADC_SIG_PORTF | 0,  /* ADC2_SE11 */
        ADC_SIG_PORTA | 29, /* ADC2_SE12 */
        ADC_SIG_PORTA | 28, /* ADC2_SE13 */
        ADC_SIG_PORTA | 27, /* ADC2_SE14 */
        ADC_SIG_PORTA | 26, /* ADC2_SE15 */
        ADC_SIG_PORTE | 8,  /* ADC2_SE16 */
        ADC_SIG_PORTE | 9,  /* ADC2_SE17 */
        ADC_SIG_NC,         /* 18 leave as default */
        ADC_SIG_NC,         /* 19 leave as default */
        ADC_SIG_NA,         /* 20 not implemented */
        ADC_SIG_NA,         /* 21 not implemented */
        ADC_SIG_NA,         /* 22 not implemented */
        ADC_SIG_NC,         /* 23 leave as default */
        ADC_SIG_NC,         /* 24 leave as default */
        ADC_SIG_NA,         /* 25 not implemented */
    };
    /* Conversion table for ADC2x inputs, where x is 4 to 7, mux is defaultly "A" */
    const static uint8_t adc2_conv_table_a[] = {
        ADC_SIG_NA,         /* 4 not implemented */
        ADC_SIG_PORTB | 21, /* ADC2_SE5a */
        ADC_SIG_PORTF | 2,  /* ADC2_SE6a */
        ADC_SIG_PORTF | 3   /* ADC2_SE7a */
    };
    /* Conversion table for ADC3x inputs, where x is 0 to 23, mux is defaultly "B" OR "X"*/
    const static uint8_t adc3_conv_table_bx[] = {
        ADC_SIG_NC,         /* 0 leave as default */
        ADC_SIG_NA,         /* 1 not implemented */
        ADC_SIG_NC,         /* 2 leave as default */
        ADC_SIG_NC,         /* 3 leave as default */
        ADC_SIG_PORTE | 27, /* ADC3_SE4b */
        ADC_SIG_PORTE | 26, /* ADC3_SE5b */
        ADC_SIG_PORTF | 21, /* ADC3_SE6b */
        ADC_SIG_PORTF | 22, /* ADC3_SE7b */
        ADC_SIG_PORTB | 0,  /* ADC0_SE8/ADC1_SE8/ADC2_SE8/ADC3_SE8/TSI0_CH0 */
        ADC_SIG_PORTB | 1,  /* ADC0_SE9/ADC1_SE9/ADC2_SE9/ADC3_SE9/TSI0_CH6 */
        ADC_SIG_PORTF | 23, /* ADC3_SE10 */
        ADC_SIG_PORTF | 24, /* ADC3_SE11 */
        ADC_SIG_PORTF | 25, /* ADC3_SE12 */
        ADC_SIG_PORTF | 26, /* ADC3_SE13 */
        ADC_SIG_PORTF | 27, /* ADC3_SE14 */
        ADC_SIG_PORTA | 11, /* ADC3_SE15 */
        ADC_SIG_PORTE | 11, /* ADC3_SE16 */
        ADC_SIG_PORTE | 12, /* ADC3_SE17 */
        ADC_SIG_NC,         /* 18 leave as default */
        ADC_SIG_NC,         /* 19 leave as default */
        ADC_SIG_NA,         /* 20 not implemented */
        ADC_SIG_NA,         /* 21 not implemented */
        ADC_SIG_NA,         /* 22 not implemented */
        ADC_SIG_NC,         /* 23 leave as default */
        ADC_SIG_NC,         /* 24 leave as default */
        ADC_SIG_NA,         /* 25 not implemented */
    };
        /* Conversion table for ADC0x inputs, where x is 4 to 7, mux is defaultly "A" */
    const static uint8_t adc3_conv_table_a[] = {
        ADC_SIG_PORTA | 10, /* ADC3_SE4a */
        ADC_SIG_PORTA | 9,  /* ADC3_SE5a */
        ADC_SIG_PORTA | 6,  /* ADC3_SE6a */
        ADC_SIG_PORTE | 28  /* ADC3_SE7a */
    };

    if (ADC_GET_DIFF(source) && (ch > 3))
    {
        return IO_ERROR; /* signal not available */
    }

    switch(ADC_GET_MODULE(source))
    {
        case ADC_SOURCE_MODULE(1): /* ADC0 */
            if (ADC_GET_MUXSEL(source) == ADC_SOURCE_MUXSEL_A)
            {
                if(ch < 4 || ch > 7)
                {
                    gpio_port = ADC_SIG_NA; /* channel does not exist */
                }
                else
                {
                    gpio_port = adc0_conv_table_a[ch - 4];
                }
            }
            else /*ADC_SOURCE_MUXSEL_B OR ADC_SOURCE_MUXSEL_X*/
            {
                if(ch < 26)
                {
                    gpio_port = adc0_conv_table_bx[ch];
                }
                else
                {
                    gpio_port = ADC_SIG_NC;
                }
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
        case ADC_SOURCE_MODULE(3): /* ADC2 */
            if (ADC_GET_MUXSEL(source) == ADC_SOURCE_MUXSEL_A)
            {
                if(ch < 4 || ch > 7)
                {
                    gpio_port = ADC_SIG_NA; /* channel does not exist */
                }
                else
                {
                    gpio_port = adc2_conv_table_a[ch - 4];
                }
            }
            else /*ADC_SOURCE_MUXSEL_B OR ADC_SOURCE_MUXSEL_X*/
            {
                if(ch < 26)
                {
                    gpio_port = adc2_conv_table_bx[ch];
                }
                else
                {
                    gpio_port = ADC_SIG_NC;
                }
            }
            break;
        case ADC_SOURCE_MODULE(4): /* ADC3 */
            if (ADC_GET_MUXSEL(source) == ADC_SOURCE_MUXSEL_A)
            {
                if(ch < 4 || ch > 7)
                {
                    gpio_port = ADC_SIG_NA; /* channel does not exist */
                }
                else
                {
                    gpio_port = adc3_conv_table_a[ch - 4];
                }
            }
            else /*ADC_SOURCE_MUXSEL_B OR ADC_SOURCE_MUXSEL_X*/
            {
                if(ch < 26)
                {
                    gpio_port = adc3_conv_table_bx[ch];
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
        case 6: /* PORTF */
            pctl = (PORT_MemMapPtr) PORTF_BASE_PTR;
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
            /* Configure GPIOE for DSPI0 peripheral function */
            pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;

            pctl->PCR[16] = PORT_PCR_MUX(2);     /* DSPI0.PCS0   */
            pctl->PCR[17] = PORT_PCR_MUX(2);     /* DSPI0.SCK    */
            pctl->PCR[18] = PORT_PCR_MUX(2);     /* DSPI0.SOUT   */
            pctl->PCR[19] = PORT_PCR_MUX(2);     /* DSPI0.SIN    */

            /* Enable clock gate to DSPI0 module */
            sim->SCGC6 |= SIM_SCGC6_DSPI0_MASK;
            break;

        case 1:
            /* Configure GPIOE for DSPI1 peripheral function     */
            pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;

            pctl->PCR[1] = PORT_PCR_MUX(2);     /* DSPI1.SOUT   */
            pctl->PCR[2] = PORT_PCR_MUX(2);     /* DSPI1.SCK    */
            pctl->PCR[3] = PORT_PCR_MUX(2);     /* DSPI1.SIN    */
            pctl->PCR[4] = PORT_PCR_MUX(2);     /* DSPI1.PCS0   */

            /* Enable clock gate to DSPI1 module */
            sim->SCGC6 |= SIM_SCGC6_DSPI1_MASK;
            break;

        case 2:
            /* Configure GPIOD for DSPI2 peripheral function     */
            pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;

            pctl->PCR[11] = PORT_PCR_MUX(2);    /* DSPI2.PCS0   */
            pctl->PCR[12] = PORT_PCR_MUX(2);    /* DSPI2.SCK    */
            pctl->PCR[13] = PORT_PCR_MUX(2);    /* DSPI2.SOUT   */
            pctl->PCR[14] = PORT_PCR_MUX(2);    /* DSPI2.SIN    */
            pctl->PCR[15] = PORT_PCR_MUX(2);    /* DSPI2.PCS1   */

            /* Enable clock gate to DSPI2 module */
            sim->SCGC3 |= SIM_SCGC3_DSPI2_MASK;
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
            sim->SCGC3 |= SIM_SCGC3_ESDHC_MASK;
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
    volatile CAN_MemMapPtr  can;

    OSC0_CR |= OSC_CR_ERCLKEN_MASK;


    switch (dev_num)
    {
        case 0:
            /* Configure GPIO for FlexCAN 0 peripheral function */
            pctl = (PORT_MemMapPtr)PORTA_BASE_PTR;
            pctl->PCR[12] = (PORT_PCR_MUX(2)|PORT_PCR_DSE_MASK);    /* CAN0_TX.A12  */
            pctl->PCR[13] = (PORT_PCR_MUX(2)|PORT_PCR_DSE_MASK);    /* CAN0_RX.A13  */

            /* Enable clock gate to FlexCAN 0 module */
            sim->SCGC6 |= SIM_SCGC6_FLEXCAN0_MASK;
            
            /* Select the bus clock as CAN engine clock source */
            can = (CAN_MemMapPtr)CAN0_BASE_PTR;
            can->CTRL1 |= CAN_CTRL1_CLKSRC_MASK;

            break;
        case 1:
            /* Configure GPIO for FlexCAN 1 peripheral function */
            pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;
            pctl->PCR[17] = (PORT_PCR_MUX(2)|PORT_PCR_DSE_MASK);    /* CAN1_TX.C17  */
            pctl->PCR[16] = (PORT_PCR_MUX(2)|PORT_PCR_DSE_MASK);    /* CAN1_RX.C16  */

            /* Enable clock gate to FlexCAN 1 module */
            sim->SCGC3 |= SIM_SCGC3_FLEXCAN1_MASK;

            /* Select the bus clock as CAN engine clock source */
            can = (CAN_MemMapPtr)CAN1_BASE_PTR;
            can->CTRL1 |= CAN_CTRL1_CLKSRC_MASK;

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

    PORT_MemMapPtr  pctl;
    SIM_MemMapPtr sim = SIM_BASE_PTR;

    switch (dev_num)
    {
        case 0:
            pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;

            pctl->PCR[18] = PORT_PCR_MUX(4) | PORT_PCR_ODE_MASK;
            pctl->PCR[19] = PORT_PCR_MUX(4) | PORT_PCR_ODE_MASK;
            /* Enable SDA rising edge detection */
#if BSPCFG_ENABLE_LEGACY_II2C_SLAVE
            pctl->PCR[18] |= PORT_PCR_IRQC(0x09);
            pctl->PCR[18] |= PORT_PCR_ISF_MASK;
#endif
            sim->SCGC4 |= SIM_SCGC4_IIC0_MASK ;

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
    pctl->PCR[12] = 0x00000400;     /* PTA12, RMII0_RXD1/MII0_RXD1      */
    pctl->PCR[13] = 0x00000400;     /* PTA13, RMII0_RXD0/MII0_RXD0      */
    pctl->PCR[14] = 0x00000400;     /* PTA14, RMII0_CRS_DV/MII0_RXDV    */
    pctl->PCR[15] = 0x00000400;     /* PTA15, RMII0_TXEN/MII0_TXEN      */
    pctl->PCR[16] = 0x00000400;     /* PTA16, RMII0_TXD0/MII0_TXD0      */
    pctl->PCR[17] = 0x00000400;     /* PTA17, RMII0_TXD1/MII0_TXD1      */


    pctl = (PORT_MemMapPtr)PORTB_BASE_PTR;
    pctl->PCR[0] = PORT_PCR_MUX(4) | PORT_PCR_ODE_MASK; /* PTB0, RMII0_MDIO/MII0_MDIO   */
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

static _mqx_int _bsp_usb_io_init
(
    _mqx_uint dev_num
)
{
    if (dev_num == 0) {
#if PE_LDD_VERSION
        /* USB clock is configured using CPU component */

        /* Check if peripheral is not used by Processor Expert USB_LDD component */
        if (PE_PeripheralUsed((uint32_t)USB0_BASE_PTR) == TRUE) {
            /* IO Device used by PE Component*/
            return IO_ERROR;
        }
#endif
        /* Configure USB to be clocked from PLL0 */
        SIM_SOPT2_REG(SIM_BASE_PTR) &= ~(SIM_SOPT2_USBFSRC_MASK);
        SIM_SOPT2_REG(SIM_BASE_PTR) |= SIM_SOPT2_USBFSRC(1);
        /* Configure USB to be clocked from clock divider */
        SIM_SOPT2_REG(SIM_BASE_PTR) |= SIM_SOPT2_USBF_CLKSEL_MASK;
        /* Configure USB divider to be 120MHz * 2 / 5 = 48 MHz */
        SIM_CLKDIV2_REG(SIM_BASE_PTR) &= ~(SIM_CLKDIV2_USBFSDIV_MASK | SIM_CLKDIV2_USBFSFRAC_MASK);
        SIM_CLKDIV2_REG(SIM_BASE_PTR) |= SIM_CLKDIV2_USBFSDIV(4) | SIM_CLKDIV2_USBFSFRAC_MASK;

        /* Enable USB-OTG IP clocking */
        SIM_SCGC4_REG(SIM_BASE_PTR) |= SIM_SCGC4_USBFS_MASK;

        /* USB D+ and USB D- are standalone not multiplexed one-purpose pins */
        /* VREFIN for device is standalone not multiplexed one-purpose pin */

#if BSP_USB_TWR_SER2
        /* TWR-SER2 board has 2 connectors: on channel A, there is Micro-USB connector,
        ** which is not routed to TWRK60 board. On channel B, there is standard
        ** A-type host connector routed to the USB0 peripheral on TWRK60. To enable
        ** power to this connector, GPIO PB8 must be set as GPIO output
        */
        PORT_PCR_REG(PORTB_BASE_PTR, 8) = PORT_PCR_MUX(0x01);
        GPIO_PDDR_REG(PTB_BASE_PTR) |= 1 << 8; // PB8 as output
        GPIO_PDOR_REG(PTB_BASE_PTR) |= 1 << 8; // PB8 in high level
#endif
    }
    else if (dev_num == 1) {
        //Disable MPU so the module can access RAM
        MPU_CESR &= ~MPU_CESR_VLD_MASK;

        //Enable clock to the module
        SIM_SCGC6 |= SIM_SCGC6_USBHS_MASK;

        SIM_CLKDIV2 |= SIM_CLKDIV2_USBHSFRAC_MASK | SIM_CLKDIV2_USBHSDIV_MASK; // Divide reference clock to obtain 60MHz 
        SIM_SOPT2 &= ~SIM_SOPT2_USBHSRC_MASK;  // MCGPLLCLK for the USB 60MHz CLKC source 
        SIM_SOPT2 |= SIM_SOPT2_USBHSRC(1);     // MCGPLLCLK for the USB 60MHz CLKC source 

        //Select external clock for USBH controller
        PORTA_PCR7 = PORT_PCR_MUX(2);   //ULPI DIR
        PORTA_PCR8 = PORT_PCR_MUX(2);   //ULPI NXT
        PORTA_PCR10 = PORT_PCR_MUX(2);  //ULPI DATA0
        PORTA_PCR11 = PORT_PCR_MUX(2);  //ULPI DATA1
        PORTA_PCR24 = PORT_PCR_MUX(2);  //ULPI DATA2
        PORTA_PCR25 = PORT_PCR_MUX(2);  //ULPI DATA3
        PORTA_PCR26 = PORT_PCR_MUX(2);  //ULPI DATA4
        PORTA_PCR27 = PORT_PCR_MUX(2);  //ULPI DATA5
        PORTA_PCR28 = PORT_PCR_MUX(2);  //ULPI DATA6
        PORTA_PCR29 = PORT_PCR_MUX(2);  //ULPI DATA7
        PORTA_PCR6 = PORT_PCR_MUX(2);   //ULPI CLK
        PORTA_PCR9 = PORT_PCR_MUX(2);   //ULPI STP
        
        /*
         *If we can't control when to provide the power to ULPI PHY and when to enable the
         * ULPI PHY clock a PHY RESET should be done after ULPI PHY initialization to make the
         * PHY recover from the stall state.
         */
        PORT_PCR_REG(PORTB_BASE_PTR, 8) = PORT_PCR_MUX(0x01);
        GPIO_PDDR_REG(PTB_BASE_PTR) |= 1 << 8; // PB8 as output
        GPIO_PDOR_REG(PTB_BASE_PTR) &= ~(1 << 8); // PB8 in low level
        _time_delay(1);
        GPIO_PDOR_REG(PTB_BASE_PTR) |= 1 << 8; // PB8 in high level
    }
    else {
        return IO_ERROR; //unknow controller
    }

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
        _bsp_usb_io_init(0);
    }
    else if (dev_if->DEV_INIT_PARAM == &_ehci0_dev_init_param) {
        _bsp_usb_io_init(1);
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
        _bsp_usb_io_init(0);
    }
    else if (dev_if->HOST_INIT_PARAM == &_ehci0_host_init_param) {
        _bsp_usb_io_init(1);
    }
    else {
        return IO_ERROR; //unknown controller
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
                 | SIM_SCGC5_PORTE_MASK \
                 | SIM_SCGC5_PORTF_MASK;

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
      case 4:
         pctl = (PORT_MemMapPtr)PORTE_BASE_PTR;
         pctl->PCR[27] = 0 | PORT_PCR_MUX(3);
         break;
      default:
         /* not used on this board */
         break;
   }
   return MQX_OK;
}
/*FUNCTION*-------------------------------------------------------------------
* Function Name    : _bsp_nandflash_io_init
* Returned Value   :
* Comments         :
* This function performs BSP-specific initialization related to nandflash
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_nandflash_io_init
(
   void
)

{
    SIM_MemMapPtr   sim = SIM_BASE_PTR;
    PORT_MemMapPtr  pctl;

    pctl = (PORT_MemMapPtr)PORTB_BASE_PTR;
    pctl->PCR[20] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
    pctl->PCR[21] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
    pctl->PCR[22] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
    pctl->PCR[23] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;

    pctl = (PORT_MemMapPtr)PORTC_BASE_PTR;
    pctl->PCR[0]  = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
    pctl->PCR[1]  = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
    pctl->PCR[2]  = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
    pctl->PCR[4]  = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
    pctl->PCR[5]  = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
    pctl->PCR[6]  = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
    pctl->PCR[7]  = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
    pctl->PCR[8]  = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
    pctl->PCR[9]  = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
    pctl->PCR[10] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
    pctl->PCR[11] = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
    pctl->PCR[16] = 0 | PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK;
    pctl->PCR[17] = 0 | PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK;

    pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;
    pctl->PCR[4]  = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
    pctl->PCR[5]  = 0 | PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK;
    pctl->PCR[8]  = 0 | PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK;
    pctl->PCR[9]  = 0 | PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK;
    pctl->PCR[10] = 0 | PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK;

    SIM_SOPT2_REG(SIM_BASE_PTR) |= SIM_SOPT2_NFCSRC(0x00);

    /* Enable clock gate to NFC module */
    sim->SCGC3 |= SIM_SCGC3_NFC_MASK;

    return MQX_OK ;
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
    sim->SCGC6 |= SIM_SCGC6_FTFE_MASK;

    return MQX_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_tss_io_init
* Returned Value   : MQX_OK
* Comments         :
*    This function performs BSP-specific initialization related to TSS
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_tss_io_init
(
    void
)
{
    SIM_MemMapPtr sim = SIM_BASE_PTR;
    
    sim->SCGC5 |= SIM_SCGC5_TSI_MASK;   /* TSI clock enablement */
    
    /* Set Electrodes for TSI function */
    PORTB_PCR0 = PORT_PCR_MUX(0);
    PORTB_PCR1 = PORT_PCR_MUX(0);
    PORTB_PCR2 = PORT_PCR_MUX(0);
    PORTB_PCR3 = PORT_PCR_MUX(0);
    PORTC_PCR0 = PORT_PCR_MUX(0);
    PORTC_PCR1 = PORT_PCR_MUX(0);
    PORTC_PCR2 = PORT_PCR_MUX(0);
    PORTA_PCR4 = PORT_PCR_MUX(0);
    PORTB_PCR16 = PORT_PCR_MUX(0);
    PORTB_PCR17 = PORT_PCR_MUX(0);
    PORTB_PCR18 = PORT_PCR_MUX(0);
    PORTB_PCR19 = PORT_PCR_MUX(0);

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
            SIM_SCGC6 |= SIM_SCGC6_SAI0_MASK;                   /* Enable SAI0 clock gate control & DMAMUX0 clock gate controlntrol */
            
            /* Enable SSI pins */
            PORTE_PCR6 |= PORT_PCR_MUX(0x04);                   /* Configure port for MCLK output */
            
            /* GPIO for SSI0_BLCK */
            PORTE_PCR12 |= PORT_PCR_MUX(0x04);                  /* Configure Port for TX Bit Clock */
            
            /* GPIO for SSI0_FS */
            PORTE_PCR11 |= PORT_PCR_MUX(0x04);                  /* Configure port for TX Frame Sync */
            
            /* GPIO for SSI0_XD */
            PORTE_PCR10 |= PORT_PCR_MUX(0x04);                  /* Configure port for TX Data */
            PORTE_PCR7 |= PORT_PCR_MUX(0x04);                   /* Configure port for RX Data */
            break;
            
        case 1:
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
        sim->SOPT2 |= SIM_SOPT2_CMTUARTPAD_MASK;

        /* PTD7 as UART TX function (Alt.3) + drive strength */
        pctl = (PORT_MemMapPtr) PORTD_BASE_PTR;
        pctl->PCR[7] = PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;
    } 
    else 
    {
        return MQX_OK;
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
    if (TRUE == enable) 
    {
        /* hardware does not support this feature. The IrDA receiver connected 
        ** to CMR2_IN3 but the UART does not support slecting source from CMP2 out put
        */
        return -1;
    } 
    return MQX_OK;
}
/* EOF */
