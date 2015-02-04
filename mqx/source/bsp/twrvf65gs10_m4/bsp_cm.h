/*HEADER**********************************************************************
*
* Copyright 2012-2013 Freescale Semiconductor, Inc.
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
*   Implementation of subset of clock manager API to provide clock frequencies
*   on platforms without full-featured clock manager
*
*       _bsp_get_clock_configuration();
*       _bsp_get_clock();
*
*
*END************************************************************************/

#ifndef __bsp_cm_h__
#define __bsp_cm_h__ 1
#include <cm_vybrid.h>

#define CPU_XTAL_CLK_HZ         24000000UL /* Value of the external crystal or oscillator clock frequency in Hz */

#define CPU_PLL_AUDIO_CLK_NUM 5

/* CPU frequencies in clock configuration 0 */
#define CPU_CLOCK_CONFIG_0              0x00U /* Clock configuration 0 identifier */
#define CPU_PLL_AUDIO_CLK_HZ_0          423360000UL  /* PLL4 clock frequency 3 - for 11025 */
#define CPU_PLL_AUDIO_CLK_HZ_1          516096000UL  /* PLL4 clock frequency 3 - for 8000, 12000, 16000, 24000 */
#define CPU_PLL_AUDIO_CLK_HZ_2          846720000UL   /* PLL4 clock frequency 0 - for 44100 with oversampling of 384 and 768  */
#define CPU_PLL_AUDIO_CLK_HZ_3          1128960000UL  /* PLL4 clock frequency 1 - for 44100 with oversampling from 256 to 768 */
#define CPU_PLL_AUDIO_CLK_HZ_4          1179648000UL  /* PLL4 clock frequency 2 - for 48000 with oversampling of 384, 256 and 512  */


#define CPU_PLL_AUDIO_DENOM             0x1FFFFFDB    /* Audio PLL divider denominator */
#define CPU_PLL_AUDIO_NUM_0             0x147AE130    /* Audio PLL divider numerator for frequency 3 */
#define CPU_PLL_AUDIO_NUM_1             0x1020C489    /* Audio PLL divider numerator for frequency 3 */
#define CPU_PLL_AUDIO_NUM_2             0x08F5C285    /* Audio PLL divider numerator for frequency 0 */
#define CPU_PLL_AUDIO_NUM_3             0x0147AE13    /* Audio PLL divider numerator for frequency 1 */
#define CPU_PLL_AUDIO_NUM_4             0x04DD2F15    /* Audio PLL divider numerator for frequency 2 */

#define CPU_PLL_AUDIO_DIV_0             (CPU_PLL_AUDIO_CLK_HZ_0/CPU_XTAL_CLK_HZ)    /* Audio PLL divider for frequency 3 */
#define CPU_PLL_AUDIO_DIV_1             (CPU_PLL_AUDIO_CLK_HZ_1/CPU_XTAL_CLK_HZ)    /* Audio PLL divider for frequency 3 */
#define CPU_PLL_AUDIO_DIV_2             (CPU_PLL_AUDIO_CLK_HZ_2/CPU_XTAL_CLK_HZ)    /* Audio PLL divider for frequency 0 */
#define CPU_PLL_AUDIO_DIV_3             (CPU_PLL_AUDIO_CLK_HZ_3/CPU_XTAL_CLK_HZ)    /* Audio PLL divider for frequency 1 */
#define CPU_PLL_AUDIO_DIV_4             (CPU_PLL_AUDIO_CLK_HZ_4/CPU_XTAL_CLK_HZ)    /* Audio PLL divider for frequency 2 */
typedef enum {
    BSP_CLOCK_CONFIGURATION_0 = 0,      /* Default configuration */
    BSP_CLOCK_CONFIGURATION_1,          /* Use FXOSC*/
    BSP_CLOCK_CONFIGURATIONS,           /* Number of available clock configurations */
    BSP_CLOCK_CONFIGURATION_DEFAULT     = BSP_CLOCK_CONFIGURATION_0,
    BSP_CLOCK_CONFIGURATION_FULL_SPEED  = BSP_CLOCK_CONFIGURATION_0,
    BSP_CLOCK_CONFIGURATION_USE_FIRC    = BSP_CLOCK_CONFIGURATION_1
} BSP_CLOCK_CONFIGURATION;

#define CPU_CLOCK_CONFIG_0              0x00U /* Clock configuration 0 identifier */
#define CPU_CLOCK_CONFIG_1              0x01U
#define CPU_CLOCK_CONFIG_2              0x02U

#if 0
/*already defined in cm_vybrid.h, with minor difference*/
typedef enum {
    CM_CLOCK_SOURCE_SYSTEM = 0,
    CM_CLOCK_SOURCE_CORE,
    CM_CLOCK_SOURCE_BUS,

    /* Number of clock sources available */
    CM_CLOCK_SOURCES,

    /* Clock source need to get through the calculation */
    CM_CLOCK_SOURCE_PLL1_PFD4,
    CM_CLOCK_SOURCE_PLL2_PFD4,
    CM_CLOCK_SOURCE_PLL_USB0,
    CM_CLOCK_SOURCE_PLL3_PFD4,
    CM_CLOCK_SOURCE_PLL_AUDIO,

    /* Aliases */
} CM_CLOCK_SOURCE;
#endif


#if 0
/*already defined in cm_vybrid.h*/
typedef enum  {
    CM_CLOCK_PLL1 = 1,
    CM_CLOCK_PLL2,
    CM_CLOCK_PLL3,
    CM_CLOCK_PLL4,
    CM_CLOCK_PLL5,
    CM_CLOCK_PLL6,
    CM_CLOCK_PLL7,
} CM_CLOCK_PLL;


typedef enum  {
    CM_CLOCK_PLL_main = 0,
    CM_CLOCK_PLL_PFD1 = 1,
    CM_CLOCK_PLL_PFD2,
    CM_CLOCK_PLL_PFD3,
    CM_CLOCK_PLL_PFD4,
} CM_CLOCK_PLL_PFD;
#endif


#ifdef __cplusplus
extern "C" {
#endif

void _bsp_clock_manager_init(void);

BSP_CLOCK_CONFIGURATION _bsp_get_clock_configuration(void);

_mqx_int _bsp_set_clock_configuration
(
    BSP_CLOCK_CONFIGURATION clock_configuration
);

uint32_t _bsp_get_clock(
    /* [IN] clock configuration */
    const BSP_CLOCK_CONFIGURATION   clock_configuration,
    /* [IN] clock source index */
    const CM_CLOCK_SOURCE           clock_source
);

uint32_t _bsp_set_clock
(
    const uint32_t        frequency,
    const CM_CLOCK_SOURCE clock_source
);

#ifdef __cplusplus
}
#endif


#endif /* __bsp_cm_h__ */
