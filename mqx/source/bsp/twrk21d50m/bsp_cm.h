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
*   Clock manager Kinetis BSP specific definitions and function prototypes.
*
*       _bsp_initialize_hardware();
*       _bsp_set_clock_configuration();
*       _bsp_get_clock_configuration();
*       _bsp_get_clock();
*       _bsp_osc_autotrim();
*
*
*END************************************************************************/

#ifndef __bsp_cm_h__
    #define __bsp_cm_h__


#ifndef PE_LDD_VERSION

/* Methods configuration constants - generated for all enabled component's methods */
#define Cpu_SetClockConfiguration_METHOD_ENABLED
#define Cpu_GetClockConfiguration_METHOD_ENABLED
#define Cpu_SetOperationMode_METHOD_ENABLED
#define Cpu_EnableInt_METHOD_ENABLED
#define Cpu_DisableInt_METHOD_ENABLED
#define Cpu_GetLLSWakeUpFlags_METHOD_ENABLED
#define Cpu_MCGAutoTrim_METHOD_ENABLED
#define CPU_BUS_CLK_HZ                  0x02DC6C00UL /* Initial value of the bus clock frequency in Hz */
#define CPU_CORE_CLK_HZ                 0x02DC6C00UL /* Initial value of the core/system clock frequency in Hz.  */

#define CPU_CLOCK_CONFIG_NUMBER         0x03U       /* Specifies number of defined clock configurations. */

#define CPU_BUS_CLK_HZ_CLOCK_CONFIG0    0x02DC6C00UL /* Value of the bus clock frequency in the clock configuration 0 in Hz. */
#define CPU_CORE_CLK_HZ_CLOCK_CONFIG0   0x02DC6C00UL /* Value of the core/system clock frequency in the clock configuration 0 in Hz. */

#define CPU_BUS_CLK_HZ_CLOCK_CONFIG1    0x00B71B00U /* Value of the bus clock frequency in the clock configuration 1 in Hz. */
#define CPU_CORE_CLK_HZ_CLOCK_CONFIG1   0x00B71B00U /* Value of the core/system clock frequency in the clock configuration 1 in Hz. */

#define CPU_BUS_CLK_HZ_CLOCK_CONFIG2    0x001E8480U /* Value of the bus clock frequency in the clock configuration 2 in Hz. */
#define CPU_CORE_CLK_HZ_CLOCK_CONFIG2   0x001E8480U /* Value of the core/system clock frequency in the clock configuration 2 in Hz. */


#define CPU_XTAL_CLK_HZ                 0x007A1200U /* Value of the external crystal or oscillator clock frequency in Hz */
#define CPU_XTAL32k_CLK_HZ              0x8000U     /* Value of the external 32k crystal or oscillator clock frequency in Hz */
#define CPU_INT_SLOW_CLK_HZ             0x8000U     /* Value of the slow internal oscillator clock frequency in Hz  */
#define CPU_INT_FAST_CLK_HZ             0x003D0900U /* Value of the fast internal oscillator clock frequency in Hz  */

#define CPU_FAMILY_Kinetis             /* Specification of the core type of the selected cpu */
#define CPU_DERIVATIVE_MK21DN512M5   /* Name of the selected cpu derivative */
#define CPU_PARTNUM_MK21DN512M5    /* Part number of the selected cpu */

/* CPU frequencies in clock configuration 0 */

#define CPU_CLOCK_CONFIG_0              0x00U           /* Clock configuration 0 identifier */
#define CPU_CORE_CLK_HZ_CONFIG_0        0x02DC6C00UL    /* Core clock frequency in clock configuration 0 */
#define CPU_BUS_CLK_HZ_CONFIG_0         0x02DC6C00UL    /* Bus clock frequency in clock configuration 0 */
#define CPU_FLEXBUS_CLK_HZ_CONFIG_0     0x016E3600UL    /* Flexbus clock frequency in clock configuration 0 */
#define CPU_FLASH_CLK_HZ_CONFIG_0       0x016E3600UL    /* FLASH clock frequency in clock configuration 0 */
#define CPU_USB_CLK_HZ_CONFIG_0         0x02DC6C00UL    /* USB clock frequency in clock configuration 0 */
#define CPU_PLL_FLL_CLK_HZ_CONFIG_0     0x016E3600UL    /* PLL/FLL clock frequency in clock configuration 0 */
#define CPU_MCGIR_CLK_HZ_CONFIG_0       0x00UL          /* MCG internal reference clock frequency in clock configuration 0 */
#define CPU_OSCER_CLK_HZ_CONFIG_0       0x07A1200UL     /* System OSC external reference clock frequency in clock configuration 0 */
#define CPU_ERCLK32K_CLK_HZ_CONFIG_0    0x8000UL        /* External reference clock 32k frequency in clock configuration 0 */
#define CPU_MCGFF_CLK_HZ_CONFIG_0       0x3D09UL        /* MCG fixed frequency clock */

/* CPU frequencies in clock configuration 1 */
#define CPU_CLOCK_CONFIG_1              0x01U           /* Clock configuration 1 identifier */
#define CPU_CORE_CLK_HZ_CONFIG_1        0x00B71B00UL    /* Core clock frequency in clock configuration 1 */
#define CPU_BUS_CLK_HZ_CONFIG_1         0x00B71B00UL    /* Bus clock frequency in clock configuration 1 */
#define CPU_FLEXBUS_CLK_HZ_CONFIG_1     0x00B71B00UL    /* Flexbus clock frequency in clock configuration 1 */
#define CPU_FLASH_CLK_HZ_CONFIG_1       0x00B71B00UL    /* FLASH clock frequency in clock configuration 1 */
#define CPU_USB_CLK_HZ_CONFIG_1         0x02DC6C00UL    /* USB clock frequency in clock configuration 1 */
#define CPU_PLL_FLL_CLK_HZ_CONFIG_1     0x016E3600UL    /* PLL/FLL clock frequency in clock configuration 1 */
#define CPU_MCGIR_CLK_HZ_CONFIG_1       0x00UL          /* MCG internal reference clock frequency in clock configuration 1 */
#define CPU_OSCER_CLK_HZ_CONFIG_1       0x07A1200UL     /* System OSC external reference clock frequency in clock configuration 1 */
#define CPU_ERCLK32K_CLK_HZ_CONFIG_1    0x8000UL        /* External reference clock 32k frequency in clock configuration 1 */
#define CPU_MCGFF_CLK_HZ_CONFIG_1       0x3D09UL        /* MCG fixed frequency clock */

/* CPU frequencies in clock configuration 2 */
#define CPU_CLOCK_CONFIG_2              0x02U            /* Clock configuration 2 identifier */
#define CPU_CORE_CLK_HZ_CONFIG_2        0x001E8480UL    /* Core clock frequency in clock configuration 2 */
#define CPU_BUS_CLK_HZ_CONFIG_2         0x001E8480UL    /* Bus clock frequency in clock configuration 2 */
#define CPU_FLEXBUS_CLK_HZ_CONFIG_2     0x001E8480UL    /* Flexbus clock frequency in clock configuration 2 */
#define CPU_FLASH_CLK_HZ_CONFIG_2       0x0007A120UL    /* FLASH clock frequency in clock configuration 2 */
#define CPU_USB_CLK_HZ_CONFIG_2         0x00UL          /* USB clock frequency in clock configuration 2 */
#define CPU_PLL_FLL_CLK_HZ_CONFIG_2     0x00UL          /* PLL/FLL clock frequency in clock configuration 2 */
#define CPU_MCGIR_CLK_HZ_CONFIG_2       0x003D0900UL    /* MCG internal reference clock frequency in clock configuration 2 */
#define CPU_OSCER_CLK_HZ_CONFIG_2       0x000UL         /* System OSC external reference clock frequency in clock configuration 2 */
#define CPU_ERCLK32K_CLK_HZ_CONFIG_2    0x00008000UL    /* External reference clock 32k frequency in clock configuration 2 */
#define CPU_MCGFF_CLK_HZ_CONFIG_2       0x00004000UL    /* MCG fixed frequency clock */

#endif /* PE_LDD_VERSION */


typedef enum {
    BSP_CLOCK_CONFIGURATION_0 = 0,      /* 48 MHZ PEE   */
    BSP_CLOCK_CONFIGURATION_1,          /* 12 MHz PEE   */
    BSP_CLOCK_CONFIGURATION_2,          /*  2 MHz BLPI  */
    BSP_CLOCK_CONFIGURATIONS,           /* Number of available clock configurations */
    BSP_CLOCK_CONFIGURATION_AUTOTRIM    = BSP_CLOCK_CONFIGURATION_1,
    BSP_CLOCK_CONFIGURATION_DEFAULT     = BSP_CLOCK_CONFIGURATION_0,
    BSP_CLOCK_CONFIGURATION_48MHZ       = BSP_CLOCK_CONFIGURATION_0,
    BSP_CLOCK_CONFIGURATION_12MHZ       = BSP_CLOCK_CONFIGURATION_1,
    BSP_CLOCK_CONFIGURATION_2MHZ        = BSP_CLOCK_CONFIGURATION_2
} BSP_CLOCK_CONFIGURATION;

/* Constants defined for use with GetLLSWakeUpFlags method */
#define LLWU_EXT_PIN0                  0x01U /* WakeUp caused by - External pin 0 (PTE1/LLWU_P0/UART1_RX) */
#define LLWU_EXT_PIN3                  0x08U /* WakeUp caused by - External pin 3 (TSI0_CH5/PTA4/LLWU_P3/FTM0_CH1/NMI_b/EZP_CS_b) */
#define LLWU_EXT_PIN4                  0x10U /* WakeUp caused by - External pin 4 (PTA13/LLWU_P4/FTM1_CH1/I2S0_TX_FS/FTM1_QD_PHB) */
#define LLWU_EXT_PIN5                  0x20U /* WakeUp caused by - External pin 5 (ADC0_SE8/TSI0_CH0/PTB0/LLWU_P5/I2C0_SCL/FTM1_CH0/FTM1_QD_PHA) */
#define LLWU_EXT_PIN6                  0x40U /* WakeUp caused by - External pin 6 (ADC0_SE15/TSI0_CH14/PTC1/LLWU_P6/SPI0_PCS3/UART1_RTS_b/FTM0_CH0/I2S0_TXD0) */
#define LLWU_EXT_PIN7                  0x80U /* WakeUp caused by - External pin 7 (CMP1_IN1/PTC3/LLWU_P7/SPI0_PCS1/UART1_RX/FTM0_CH2/CLKOUTa/I2S0_TX_BCLK) */
#define LLWU_EXT_PIN8                  0x0100U /* WakeUp caused by - External pin 8 (PTC4/LLWU_P8/SPI0_PCS0/UART1_TX/FTM0_CH3/CMP1_OUT) */
#define LLWU_EXT_PIN9                  0x0200U /* WakeUp caused by - External pin 9 (PTC5/LLWU_P9/SPI0_SCK/LPTMR0_ALT2/I2S0_RXD0/CMP0_OUT) */
#define LLWU_EXT_PIN10                 0x0400U /* WakeUp caused by - External pin 10 (CMP0_IN0/PTC6/LLWU_P10/SPI0_SOUT/PDB0_EXTRG/I2S0_RX_BCLK/I2S0_MCLK) */
#define LLWU_EXT_PIN11                 0x0800U /* WakeUp caused by - External pin 11 (PTC11/LLWU_P11) */
#define LLWU_EXT_PIN12                 0x1000U /* WakeUp caused by - External pin 12 (PTD0/LLWU_P12/SPI0_PCS0/UART2_RTS_b) */
#define LLWU_EXT_PIN13                 0x2000U /* WakeUp caused by - External pin 13 (PTD2/LLWU_P13/SPI0_SOUT/UART2_RX) */
#define LLWU_EXT_PIN14                 0x4000U /* WakeUp caused by - External pin 14 (PTD4/LLWU_P14/SPI0_PCS1/UART0_RTS_b/FTM0_CH4/EWM_IN) */
#define LLWU_EXT_PIN15                 0x8000U /* WakeUp caused by - External pin 15 (ADC0_SE7b/PTD6/LLWU_P15/SPI0_PCS3/UART0_RX/FTM0_CH6/FTM0_FLT0) */
#define LLWU_INT_MODULE0               0x00010000U /* WakeUp caused by - Internal module 0 (LPTMR0) */
#define LLWU_INT_MODULE1               0x00020000U /* WakeUp caused by - Internal module 1 (CMP0) */
#define LLWU_INT_MODULE2               0x00040000U /* WakeUp caused by - Internal module 2 (CMP1) */
#define LLWU_INT_MODULE4               0x00100000U /* WakeUp caused by - Internal module 4 (TSI0) */
#define LLWU_INT_MODULE5               0x00200000U /* WakeUp caused by - Internal module 5 (RTC) */
#define LLWU_INT_MODULE7               0x00800000U /* WakeUp caused by - Internal module 7 (RTC_Seconds) */
#define LLWU_FILTER1                   0x01000000U /* WakeUp caused by digital filter 1 */
#define LLWU_FILTER2                   0x02000000U /* WakeUp caused by digital filter 2 */

#ifdef __cplusplus
extern "C" {
#endif


void _bsp_initialize_hardware
(
    void
);

BSP_CLOCK_CONFIGURATION _bsp_get_clock_configuration
(
    void
);

uint16_t _bsp_set_clock_configuration
(
    /* [IN] runtime clock configuration */
    const BSP_CLOCK_CONFIGURATION clock_configuration
);


uint32_t _bsp_get_clock
(
    /* [IN] clock configuration */
    const BSP_CLOCK_CONFIGURATION   clock_configuration,
    /* [IN] clock source index */
    const CM_CLOCK_SOURCE           clock_source
);

uint16_t _bsp_osc_autotrim
(
    void
);

void _bsp_low_level_init(void);


#ifdef __cplusplus
}
#endif


#endif /* __bsp_cm_h__ */
/* EOF */

