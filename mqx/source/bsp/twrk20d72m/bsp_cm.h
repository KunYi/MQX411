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

#define CPU_BUS_CLK_HZ                  0x02255100UL /* Initial value of the bus clock frequency in Hz */
#define CPU_CORE_CLK_HZ                 0x044AA200UL /* Initial value of the core/system clock frequency in Hz.  */

#define CPU_CLOCK_CONFIG_NUMBER         0x03U       /* Specifies number of defined clock configurations. */

#define CPU_BUS_CLK_HZ_CLOCK_CONFIG0    0x02255100UL /* Value of the bus clock frequency in the clock configuration 0 in Hz. */
#define CPU_CORE_CLK_HZ_CLOCK_CONFIG0   0x044AA200UL /* Value of the core/system clock frequency in the clock configuration 0 in Hz. */

#define CPU_BUS_CLK_HZ_CLOCK_CONFIG1    0x00B71B00U /* Value of the bus clock frequency in the clock configuration 1 in Hz. */
#define CPU_CORE_CLK_HZ_CLOCK_CONFIG1   0x00B71B00U /* Value of the core/system clock frequency in the clock configuration 1 in Hz. */

#define CPU_BUS_CLK_HZ_CLOCK_CONFIG2    0x001E8480U /* Value of the bus clock frequency in the clock configuration 2 in Hz. */
#define CPU_CORE_CLK_HZ_CLOCK_CONFIG2   0x001E8480U /* Value of the core/system clock frequency in the clock configuration 2 in Hz. */


#define CPU_XTAL_CLK_HZ                 0x007A1200U /* Value of the external crystal or oscillator clock frequency in Hz */
#define CPU_XTAL32k_CLK_HZ              0x8000U     /* Value of the external 32k crystal or oscillator clock frequency in Hz */
#define CPU_INT_SLOW_CLK_HZ             0x8000U     /* Value of the slow internal oscillator clock frequency in Hz  */
#define CPU_INT_FAST_CLK_HZ             0x003D0900U /* Value of the fast internal oscillator clock frequency in Hz  */

#define CPU_FAMILY_Kinetis             /* Specification of the core type of the selected cpu */
#define CPU_DERIVATIVE_MK20DX256LL7   /* Name of the selected cpu derivative */
#define CPU_PARTNUM_MK20DX256VLL7    /* Part number of the selected cpu */

/* CPU frequencies in clock configuration 0 */

#define CPU_CLOCK_CONFIG_0              0x00U           /* Clock configuration 0 identifier 72M */
#define CPU_CORE_CLK_HZ_CONFIG_0        0x044AA200UL    /* Core clock frequency in clock configuration 0 72M*/
#define CPU_BUS_CLK_HZ_CONFIG_0         0x02255100UL    /* Bus clock frequency in clock configuration 0 36M*/
#define CPU_FLEXBUS_CLK_HZ_CONFIG_0     0x02255100UL    /* Flexbus clock frequency in clock configuration 0 36M */
#define CPU_FLASH_CLK_HZ_CONFIG_0       0x0112A880UL    /* FLASH clock frequency in clock configuration 0 18M*/
#define CPU_USB_CLK_HZ_CONFIG_0         0x02DC6C00UL    /* USB clock frequency in clock configuration 0 48M*/
#define CPU_PLL_FLL_CLK_HZ_CONFIG_0     0x044AA200UL    /* PLL/FLL clock frequency in clock configuration 0 */
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
#define CPU_PLL_FLL_CLK_HZ_CONFIG_1     0x044AA200UL    /* PLL/FLL clock frequency in clock configuration 1 */
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
    BSP_CLOCK_CONFIGURATION_0 = 0,      /* 72 MHZ PEE   */
    BSP_CLOCK_CONFIGURATION_1,          /* 12 MHz PEE   */
    BSP_CLOCK_CONFIGURATION_2,          /*  2 MHz BLPI  */
    BSP_CLOCK_CONFIGURATIONS,           /* Number of available clock configurations */
    BSP_CLOCK_CONFIGURATION_AUTOTRIM    = BSP_CLOCK_CONFIGURATION_1,
    BSP_CLOCK_CONFIGURATION_DEFAULT     = BSP_CLOCK_CONFIGURATION_0,
    BSP_CLOCK_CONFIGURATION_72MHZ       = BSP_CLOCK_CONFIGURATION_0,
    BSP_CLOCK_CONFIGURATION_12MHZ       = BSP_CLOCK_CONFIGURATION_1,
    BSP_CLOCK_CONFIGURATION_2MHZ        = BSP_CLOCK_CONFIGURATION_2
} BSP_CLOCK_CONFIGURATION;

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

