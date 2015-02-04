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
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <bsp_prv.h>


#ifdef PE_LDD_VERSION

extern const TCpuClockConfiguration PE_CpuClockConfigurations[CPU_CLOCK_CONFIG_NUMBER];

#else /* PE_LDD_VERSION */


/* Clock Manager Errors  */
#define ERR_OK          CM_ERR_OK               /* OK */
#define ERR_SPEED       CM_ERR_SPEED            /* This device does not work in the active speed mode. */
#define ERR_RANGE       CM_ERR_RANGE            /* Parameter out of range. */
#define ERR_VALUE       CM_ERR_VALUE            /* Parameter of incorrect value. */
#define ERR_FAILED      CM_ERR_FAILED           /* Requested functionality or process failed. */
#define ERR_PARAM_MODE  CM_ERR_PARAM_MODE       /* Invalid mode. */


/* Symbols representing MCG modes */
#define MCG_MODE_FBI                    0x00U
#define MCG_MODE_BLPI                   0x01U
#define MCG_MODE_FBE                    0x02U
#define MCG_MODE_PBE                    0x03U
#define MCG_MODE_PEE                    0x04U
static const uint8_t MCGTransitionMatrix[5][5] = {
/* This matrix defines which mode is next in the MCG Mode state diagram in transitioning from the
   current mode to a target mode*/
  {  MCG_MODE_FBI,  MCG_MODE_BLPI,  MCG_MODE_FBE,  MCG_MODE_FBE,  MCG_MODE_FBE}, /* FBI */
  {  MCG_MODE_FBI,  MCG_MODE_BLPI,  MCG_MODE_FBI,  MCG_MODE_FBI,  MCG_MODE_FBI}, /* BLPI */
  {  MCG_MODE_FBI,  MCG_MODE_FBI,  MCG_MODE_FBE,  MCG_MODE_PBE,  MCG_MODE_PBE}, /* FBE */
  {  MCG_MODE_FBE,  MCG_MODE_FBE,  MCG_MODE_FBE,  MCG_MODE_PBE,  MCG_MODE_PEE}, /* PBE */
  {  MCG_MODE_PBE,  MCG_MODE_PBE,  MCG_MODE_PBE,  MCG_MODE_PBE,  MCG_MODE_PEE}  /* PEE */
};

/*
 * lint -esym(765, PE_CpuClockConfigurations) Disable MISRA rule (8.10) checking for symbols (PE_CpuClockConfigurations).
 */

/*
** ===========================================================================
** The array of clock frequencies in configured clock configurations.
** ===========================================================================
*/
static const TCpuClockConfiguration PE_CpuClockConfigurations[CPU_CLOCK_CONFIG_NUMBER] = {
  /* Clock configuration 0 */
  {
    CPU_CORE_CLK_HZ_CONFIG_0,          /*!< Core clock frequency in clock configuration 0 */
    CPU_BUS_CLK_HZ_CONFIG_0,           /*!< Bus clock frequency in clock configuration 0 */
    CPU_FLEXBUS_CLK_HZ_CONFIG_0,       /*!< Flexbus clock frequency in clock configuration 0 */
    CPU_FLASH_CLK_HZ_CONFIG_0,         /*!< FLASH clock frequency in clock configuration 0 */
    CPU_USB_CLK_HZ_CONFIG_0,           /*!< USB clock frequency in clock configuration 0 */
    CPU_PLL_FLL_CLK_HZ_CONFIG_0,       /*!< PLL/FLL clock frequency in clock configuration 0 */
    CPU_MCGIR_CLK_HZ_CONFIG_0,         /*!< MCG internal reference clock frequency in clock configuration 0 */
    CPU_OSCER_CLK_HZ_CONFIG_0,         /*!< System OSC external reference clock frequency in clock configuration 0 */
    CPU_ERCLK32K_CLK_HZ_CONFIG_0,      /*!< External reference clock 32k frequency in clock configuration 0 */
    CPU_MCGFF_CLK_HZ_CONFIG_0          /*!< MCG fixed frequency clock */
  },
  /* Clock configuration 1 */
  {
    CPU_CORE_CLK_HZ_CONFIG_1,          /*!< Core clock frequency in clock configuration 1 */
    CPU_BUS_CLK_HZ_CONFIG_1,           /*!< Bus clock frequency in clock configuration 1 */
    CPU_FLEXBUS_CLK_HZ_CONFIG_1,       /*!< Flexbus clock frequency in clock configuration 1 */
    CPU_FLASH_CLK_HZ_CONFIG_1,         /*!< FLASH clock frequency in clock configuration 1 */
    CPU_USB_CLK_HZ_CONFIG_1,           /*!< USB clock frequency in clock configuration 1 */
    CPU_PLL_FLL_CLK_HZ_CONFIG_1,       /*!< PLL/FLL clock frequency in clock configuration 1 */
    CPU_MCGIR_CLK_HZ_CONFIG_1,         /*!< MCG internal reference clock frequency in clock configuration 1 */
    CPU_OSCER_CLK_HZ_CONFIG_1,         /*!< System OSC external reference clock frequency in clock configuration 1 */
    CPU_ERCLK32K_CLK_HZ_CONFIG_1,      /*!< External reference clock 32k frequency in clock configuration 1 */
    CPU_MCGFF_CLK_HZ_CONFIG_1          /*!< MCG fixed frequency clock */
  },
  /* Clock configuration 2 */
  {
    CPU_CORE_CLK_HZ_CONFIG_2,          /*!< Core clock frequency in clock configuration 2 */
    CPU_BUS_CLK_HZ_CONFIG_2,           /*!< Bus clock frequency in clock configuration 2 */
    CPU_FLEXBUS_CLK_HZ_CONFIG_2,       /*!< Flexbus clock frequency in clock configuration 2 */
    CPU_FLASH_CLK_HZ_CONFIG_2,         /*!< FLASH clock frequency in clock configuration 2 */
    CPU_USB_CLK_HZ_CONFIG_2,           /*!< USB clock frequency in clock configuration 2 */
    CPU_PLL_FLL_CLK_HZ_CONFIG_2,       /*!< PLL/FLL clock frequency in clock configuration 2 */
    CPU_MCGIR_CLK_HZ_CONFIG_2,         /*!< MCG internal reference clock frequency in clock configuration 2 */
    CPU_OSCER_CLK_HZ_CONFIG_2,         /*!< System OSC external reference clock frequency in clock configuration 2 */
    CPU_ERCLK32K_CLK_HZ_CONFIG_2,      /*!< External reference clock 32k frequency in clock configuration 2 */
    CPU_MCGFF_CLK_HZ_CONFIG_2          /*!< MCG fixed frequency clock */
  }
};

/* Global variables */
volatile uint8_t SR_reg;               /* Current value of the FAULTMASK register */
volatile uint8_t SR_lock = 0x00U;      /* Lock */
static uint8_t ClockConfigurationID = CPU_CLOCK_CONFIG_0; /* Active clock configuration */



/*
** ===================================================================
**     Method      :  LDD_SetClockConfiguration (component MK20DX256Z)
**
**     Description :
**         This method changes the clock configuration of all LDD
**         components in the project.
** ===================================================================
*/
void LDD_SetClockConfiguration(LDD_TClockConfiguration ClockConfiguration)
{
  (void)ClockConfiguration;            /* Parameter is not used, suppress unused argument warning */
}


static void Cpu_SetMCGModePEE(uint8_t CLKMode);
/*
** ===================================================================
**     Method      :  Cpu_SetMCGModePEE (component MK21FN1M0MC12)
**
**     Description :
**         This method sets the MCG to PEE mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

static void Cpu_SetMCGModePBE(uint8_t CLKMode);
/*
** ===================================================================
**     Method      :  Cpu_SetMCGModePBE (component MK21FN1M0MC12)
**
**     Description :
**         This method sets the MCG to PBE mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

static void Cpu_SetMCGModeFBE(uint8_t CLKMode);
/*
** ===================================================================
**     Method      :  Cpu_SetMCGModeFBE (component MK21FN1M0MC12)
**
**     Description :
**         This method sets the MCG to FBE mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

static void Cpu_SetMCGModeBLPI(uint8_t CLKMode);
/*
** ===================================================================
**     Method      :  Cpu_SetMCGModeBLPI (component MK21FN1M0MC12)
**
**     Description :
**         This method sets the MCG to BLPI mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

static void Cpu_SetMCGModeFBI(uint8_t CLKMode);
/*
** ===================================================================
**     Method      :  Cpu_SetMCGModeFBI (component MK21FN1M0MC12)
**
**     Description :
**         This method sets the MCG to FBI mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

static void Cpu_SetMCG(uint8_t CLKMode);
/*
** ===================================================================
**     Method      :  Cpu_SetMCG (component MK21FN1M0MC12)
**
**     Description :
**         This method updates the MCG according the requested clock
**         source setting.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

static uint8_t Cpu_GetCurrentMCGMode(void);
/*
** ===================================================================
**     Method      :  Cpu_GetCurrentMCGMode (component MK21FN1M0MC12)
**
**     Description :
**         This method returns the active MCG mode
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

/*
** ===================================================================
**     Method      :  Cpu_SetMCGModePEE (component MK21FN1M0MC12)
**
**     Description :
**         This method sets the MCG to PEE mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void Cpu_SetMCGModePEE(uint8_t CLKMode)
{
  switch (CLKMode) {
    case 0U:
      /* Switch to PEE Mode */
      /* OSC_CR: ERCLKEN=1,??=0,EREFSTEN=1,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
      OSC_CR = (OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK);
      /* MCG_C7: OSCSEL=0 */
      MCG_C7 &= (uint8_t)~(uint8_t)(MCG_C7_OSCSEL_MASK);
      /* MCG_C1: CLKS=0,FRDIV=3,IREFS=0,IRCLKEN=0,IREFSTEN=0 */
      MCG_C1 = (MCG_C1_CLKS(0x00) | MCG_C1_FRDIV(0x03));
      /* MCG_C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=1,LP=0,IRCS=0 */
      MCG_C2 = (MCG_C2_RANGE0(0x02) | MCG_C2_EREFS0_MASK);
      /* MCG_C5: ??=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=1 */
      MCG_C5 = MCG_C5_PRDIV0(0x01);
      /* MCG_C6: LOLIE0=0,PLLS=1,CME0=0,VDIV0=6 */
      MCG_C6 = (MCG_C6_PLLS_MASK | MCG_C6_VDIV0(0x06));
      while((MCG_S & 0x0CU) != 0x0CU) { /* Wait until output of the PLL is selected */
      }
      break;
    default:
      break;
  }
}

/*
** ===================================================================
**     Method      :  Cpu_SetMCGModePBE (component MK21FN1M0MC12)
**
**     Description :
**         This method sets the MCG to PBE mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void Cpu_SetMCGModePBE(uint8_t CLKMode)
{
  switch (CLKMode) {
    case 0U:
      /* Switch to PBE Mode */
      /* OSC_CR: ERCLKEN=1,??=0,EREFSTEN=1,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
      OSC_CR = (OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK);
      /* MCG_C7: OSCSEL=0 */
      MCG_C7 &= (uint8_t)~(uint8_t)(MCG_C7_OSCSEL_MASK);
      /* MCG_C1: CLKS=2,FRDIV=3,IREFS=0,IRCLKEN=0,IREFSTEN=0 */
      MCG_C1 = (MCG_C1_CLKS(0x02) | MCG_C1_FRDIV(0x03));
      /* MCG_C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=1,LP=0,IRCS=0 */
      MCG_C2 = (MCG_C2_RANGE0(0x02) | MCG_C2_EREFS0_MASK);
      /* MCG_C5: ??=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=1 */
      MCG_C5 = MCG_C5_PRDIV0(0x01);
      /* MCG_C6: LOLIE0=0,PLLS=1,CME0=0,VDIV0=6 */
      MCG_C6 = (MCG_C6_PLLS_MASK | MCG_C6_VDIV0(0x06));
      while((MCG_S & 0x0CU) != 0x08U) { /* Wait until external reference clock is selected as MCG output */
      }
      while((MCG_S & MCG_S_LOCK0_MASK) == 0x00U) { /* Wait until locked */
      }
      break;
    case 1U:
      /* Switch to PBE Mode */
      /* OSC_CR: ERCLKEN=1,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
      OSC_CR = OSC_CR_ERCLKEN_MASK;
      /* MCG_C7: OSCSEL=0 */
      MCG_C7 &= (uint8_t)~(uint8_t)(MCG_C7_OSCSEL_MASK);
      /* MCG_C1: CLKS=2,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
      MCG_C1 = (MCG_C1_CLKS(0x02) | MCG_C1_FRDIV(0x03) | MCG_C1_IRCLKEN_MASK);
      /* MCG_C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=1,LP=0,IRCS=1 */
      MCG_C2 = (MCG_C2_RANGE0(0x02) | MCG_C2_EREFS0_MASK | MCG_C2_IRCS_MASK);
      /* MCG_C5: ??=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=2 */
      MCG_C5 = MCG_C5_PRDIV0(0x02);
      /* MCG_C6: LOLIE0=0,PLLS=1,CME0=0,VDIV0=0 */
      MCG_C6 = (MCG_C6_PLLS_MASK | MCG_C6_VDIV0(0x00));
      while((MCG_S & 0x0CU) != 0x08U) { /* Wait until external reference clock is selected as MCG output */
      }
      while((MCG_S & MCG_S_LOCK0_MASK) == 0x00U) { /* Wait until locked */
      }
      break;
    default:
      break;
  }
}

/*
** ===================================================================
**     Method      :  Cpu_SetMCGModeFBE (component MK21FN1M0MC12)
**
**     Description :
**         This method sets the MCG to FBE mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void Cpu_SetMCGModeFBE(uint8_t CLKMode)
{
  switch (CLKMode) {
    case 0U:
      /* Switch to FBE Mode */
      /* MCG_C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=1,LP=0,IRCS=0 */
      MCG_C2 = (MCG_C2_RANGE0(0x02) | MCG_C2_EREFS0_MASK);
      /* OSC_CR: ERCLKEN=1,??=0,EREFSTEN=1,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
      OSC_CR = (OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK);

      /* MCG_C7: OSCSEL=0 */
      MCG_C7 &= (uint8_t)~(uint8_t)(MCG_C7_OSCSEL_MASK);
      /* MCG_C1: CLKS=2,FRDIV=3,IREFS=0,IRCLKEN=0,IREFSTEN=0 */
      MCG_C1 = (MCG_C1_CLKS(0x02) | MCG_C1_FRDIV(0x03));
      /* MCG_C4: DMX32=0,DRST_DRS=0 */
      MCG_C4 &= (uint8_t)~(uint8_t)((MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x03)));
      /* MCG_C5: ??=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=1 */
      MCG_C5 = MCG_C5_PRDIV0(0x01);
      /* MCG_C6: LOLIE0=0,PLLS=0,CME0=0,VDIV0=6 */
      MCG_C6 = MCG_C6_VDIV0(0x06);
      while((MCG_S & MCG_S_OSCINIT0_MASK) == 0x00U) { /* Check that the oscillator is running */
      }
      while((MCG_S & MCG_S_IREFST_MASK) != 0x00U) { /* Check that the source of the FLL reference clock is the external reference clock. */
      }
      while((MCG_S & 0x0CU) != 0x08U) { /* Wait until external reference clock is selected as MCG output */
      }
      break;
    case 1U:
      /* Switch to FBE Mode */
      /* MCG_C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=1,LP=0,IRCS=1 */
      MCG_C2 = (MCG_C2_RANGE0(0x02) | MCG_C2_EREFS0_MASK | MCG_C2_IRCS_MASK);
      /* OSC_CR: ERCLKEN=1,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
      OSC_CR = OSC_CR_ERCLKEN_MASK;

      /* MCG_C7: OSCSEL=0 */
      MCG_C7 &= (uint8_t)~(uint8_t)(MCG_C7_OSCSEL_MASK);
      /* MCG_C1: CLKS=2,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
      MCG_C1 = (MCG_C1_CLKS(0x02) | MCG_C1_FRDIV(0x03) | MCG_C1_IRCLKEN_MASK);
      /* MCG_C4: DMX32=0,DRST_DRS=0 */
      MCG_C4 &= (uint8_t)~(uint8_t)((MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x03)));
      /* MCG_C5: ??=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=0 */
      MCG_C5 = MCG_C5_PRDIV0(0x00);
      /* MCG_C6: LOLIE0=0,PLLS=0,CME0=0,VDIV0=0 */
      MCG_C6 = MCG_C6_VDIV0(0x00);
      while((MCG_S & MCG_S_OSCINIT0_MASK) == 0x00U) { /* Check that the oscillator is running */
      }
      while((MCG_S & MCG_S_IREFST_MASK) != 0x00U) { /* Check that the source of the FLL reference clock is the external reference clock. */
      }
      while((MCG_S & 0x0CU) != 0x08U) { /* Wait until external reference clock is selected as MCG output */
      }
      break;
    default:
      break;
  }
}

/*
** ===================================================================
**     Method      :  Cpu_SetMCGModeBLPI (component MK21FN1M0MC12)
**
**     Description :
**         This method sets the MCG to BLPI mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void Cpu_SetMCGModeBLPI(uint8_t CLKMode)
{
  switch (CLKMode) {
    case 1U:
      /* Switch to BLPI Mode */
      /* MCG_C1: CLKS=1,FRDIV=0,IREFS=1,IRCLKEN=1,IREFSTEN=0 */
      MCG_C1 = MCG_C1_CLKS(0x01) |
               MCG_C1_FRDIV(0x00) |
               MCG_C1_IREFS_MASK |
               MCG_C1_IRCLKEN_MASK;
      /* MCG_C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=1,LP=1,IRCS=1 */
      MCG_C2 = MCG_C2_RANGE0(0x02) |
               MCG_C2_EREFS0_MASK |
               MCG_C2_LP_MASK |
               MCG_C2_IRCS_MASK;
      /* OSC_CR: ERCLKEN=1,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
      OSC_CR = OSC_CR_ERCLKEN_MASK;
      while((MCG_S & MCG_S_IREFST_MASK) == 0x00U) { /* Check that the source of the FLL reference clock is the internal reference clock. */
      }
      while((MCG_S & MCG_S_IRCST_MASK) == 0x00U) { /* Check that the fast external reference clock is selected. */
      }
      break;
    default:
      break;
  }
}

/*
** ===================================================================
**     Method      :  Cpu_SetMCGModeFBI (component MK21FN1M0MC12)
**
**     Description :
**         This method sets the MCG to FBI mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void Cpu_SetMCGModeFBI(uint8_t CLKMode)
{
  switch (CLKMode) {
    case 0U:
      /* Switch to FBI Mode */
      /* MCG_C1: CLKS=1,FRDIV=0,IREFS=1,IRCLKEN=0,IREFSTEN=0 */
      MCG_C1 = (MCG_C1_CLKS(0x01) | MCG_C1_FRDIV(0x00) | MCG_C1_IREFS_MASK);
      /* MCG_C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=1,LP=0,IRCS=0 */
      MCG_C2 = (MCG_C2_RANGE0(0x02) | MCG_C2_EREFS0_MASK);
      /* MCG_C4: DMX32=0,DRST_DRS=0 */
      MCG_C4 &= (uint8_t)~(uint8_t)((MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x03)));
      /* OSC_CR: ERCLKEN=1,??=0,EREFSTEN=1,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
      OSC_CR = (OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK);

      /* MCG_C7: OSCSEL=0 */
      MCG_C7 &= (uint8_t)~(uint8_t)(MCG_C7_OSCSEL_MASK);
      /* MCG_C5: ??=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=1 */
      MCG_C5 = MCG_C5_PRDIV0(0x01);
      /* MCG_C6: LOLIE0=0,PLLS=0,CME0=0,VDIV0=6 */
      MCG_C6 = MCG_C6_VDIV0(0x06);
      while((MCG_S & MCG_S_IREFST_MASK) == 0x00U) { /* Check that the source of the FLL reference clock is the internal reference clock. */
      }
      while((MCG_S & 0x0CU) != 0x04U) { /* Wait until internal reference clock is selected as MCG output */
      }
      break;
    case 1U:
      /* Switch to FBI Mode */
      /* MCG_C1: CLKS=1,FRDIV=0,IREFS=1,IRCLKEN=1,IREFSTEN=0 */
      MCG_C1 = MCG_C1_CLKS(0x01) |
               MCG_C1_FRDIV(0x00) |
               MCG_C1_IREFS_MASK |
               MCG_C1_IRCLKEN_MASK;
      /* MCG_C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=1,LP=0,IRCS=1 */
      MCG_C2 = (MCG_C2_RANGE0(0x02) | MCG_C2_EREFS0_MASK | MCG_C2_IRCS_MASK);
      /* MCG_C4: DMX32=0,DRST_DRS=0 */
      MCG_C4 &= (uint8_t)~(uint8_t)((MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x03)));
      /* OSC_CR: ERCLKEN=1,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
      OSC_CR = OSC_CR_ERCLKEN_MASK;

      /* MCG_C7: OSCSEL=0 */
      MCG_C7 &= (uint8_t)~(uint8_t)(MCG_C7_OSCSEL_MASK);
      /* MCG_C5: ??=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=0 */
      MCG_C5 = MCG_C5_PRDIV0(0x00);
      /* MCG_C6: LOLIE0=0,PLLS=0,CME0=0,VDIV0=0 */
      MCG_C6 = MCG_C6_VDIV0(0x00);
      while((MCG_S & MCG_S_IREFST_MASK) == 0x00U) { /* Check that the source of the FLL reference clock is the internal reference clock. */
      }
      while((MCG_S & 0x0CU) != 0x04U) { /* Wait until internal reference clock is selected as MCG output */
      }
      break;
    default:
      break;
  }
}

/*
** ===================================================================
**     Method      :  Cpu_SetMCG (component MK21FN1M0MC12)
**
**     Description :
**         This method updates the MCG according the requested clock
**         source setting.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void Cpu_SetMCG(uint8_t CLKMode)
{
  uint8_t TargetMCGMode = 0x00U;
  uint8_t NextMCGMode;

  switch (CLKMode) {
    case 0U:
      TargetMCGMode = MCG_MODE_PEE;
      break;
    case 1U:
      TargetMCGMode = MCG_MODE_BLPI;
      break;
    default:
      break;
  }
  NextMCGMode = Cpu_GetCurrentMCGMode(); /* Identify the currently active MCG mode */
  do {
    NextMCGMode = MCGTransitionMatrix[NextMCGMode][TargetMCGMode]; /* Get the next MCG mode on the path to the target MCG mode */
    switch (NextMCGMode) {             /* Set the next MCG mode on the path to the target MCG mode */
      case MCG_MODE_FBI:
        Cpu_SetMCGModeFBI(CLKMode);
        break;
      case MCG_MODE_BLPI:
        Cpu_SetMCGModeBLPI(CLKMode);
        break;
      case MCG_MODE_FBE:
        Cpu_SetMCGModeFBE(CLKMode);
        break;
      case MCG_MODE_PBE:
        Cpu_SetMCGModePBE(CLKMode);
        break;
      case MCG_MODE_PEE:
        Cpu_SetMCGModePEE(CLKMode);
        break;
      default:
        break;
    }
  } while (TargetMCGMode != NextMCGMode); /* Loop until the target MCG mode is set */
}

/*
** ===================================================================
**     Method      :  Cpu_GetCurrentMCGMode (component MK21FN1M0MC12)
**
**     Description :
**         This method returns the active MCG mode
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
uint8_t Cpu_GetCurrentMCGMode(void)
{
  switch (MCG_C1  & MCG_C1_CLKS_MASK) {
    case  0x00U:
      return MCG_MODE_PEE;
    case 0x40U:
      /* Internal reference clock is selected */
      if ((MCG_C2 & MCG_C2_LP_MASK) == 0x00U) {
        /* Low power mode is disabled */
        return MCG_MODE_FBI;
      } else {
        /* Low power mode is enabled */
        return MCG_MODE_BLPI;
      }
    case 0x80U:
      if ((MCG_C6 & MCG_C6_PLLS_MASK) == 0x00U) {
        /* FLL is selected */
        return MCG_MODE_FBE;
      } else {
        /* PLL is selected */
        return MCG_MODE_PBE;
       }
    default:
      return 0x00U;
  }
}

/*
** ===================================================================
**     Method      :  Cpu_SetClockConfiguration (component MK21FN1M0MC12)
*/
/*!
**     @brief
**         Calling of this method will cause the clock configuration
**         change and reconfiguration of all components according to
**         the requested clock configuration setting.
**     @param
**         ModeID          - Clock configuration identifier
**     @return
**                         - ERR_OK - OK.
**                           ERR_RANGE - Mode parameter out of range
*/
/* ===================================================================*/
LDD_TError Cpu_SetClockConfiguration(LDD_TClockConfiguration ModeID)
{
  if (ModeID > 0x02U) {
    return ERR_RANGE;                  /* Undefined clock configuration requested requested */
  }
  switch (ModeID) {
    case CPU_CLOCK_CONFIG_0:
      if (ClockConfigurationID == 2U) {
        /* Clock configuration 0 and clock configuration 2 use different clock configuration */
        /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=1,OUTDIV3=4,OUTDIV4=4,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) |
                      SIM_CLKDIV1_OUTDIV2(0x01) |
                      SIM_CLKDIV1_OUTDIV3(0x04) |
                      SIM_CLKDIV1_OUTDIV4(0x04); /* Set the system prescalers to safe value */
        Cpu_SetMCG(0U);                /* Update clock source setting */
      }
      /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=1,OUTDIV3=2,OUTDIV4=4,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
      SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) |
                    SIM_CLKDIV1_OUTDIV2(0x01) |
                    SIM_CLKDIV1_OUTDIV3(0x02) |
                    SIM_CLKDIV1_OUTDIV4(0x04); /* Update system prescalers */
      /* SIM_CLKDIV2: USBDIV=0,USBFRAC=0 */
      SIM_CLKDIV2 = (uint32_t)0x09UL; /* Update USB clock prescalers */
      /* SIM_SOPT2: PLLFLLSEL=1 */
      SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; /* Select PLL as a clock source for various peripherals */
      /* SIM_SOPT1: OSC32KSEL=0 */
      SIM_SOPT1 &= (uint32_t)~(uint32_t)(SIM_SOPT1_OSC32KSEL(0x03)); /* System oscillator drives 32 kHz clock for various peripherals */
      break;
    case CPU_CLOCK_CONFIG_1:
      if (ClockConfigurationID == 2U) {
        /* Clock configuration 1 and clock configuration 2 use different clock configuration */
        /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=1,OUTDIV3=4,OUTDIV4=4,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
        SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) |
                      SIM_CLKDIV1_OUTDIV2(0x01) |
                      SIM_CLKDIV1_OUTDIV3(0x04) |
                      SIM_CLKDIV1_OUTDIV4(0x04); /* Set the system prescalers to safe value */
        Cpu_SetMCG(0U);                /* Update clock source setting */
      }
      /* SIM_CLKDIV1: OUTDIV1=9,OUTDIV2=9,OUTDIV3=9,OUTDIV4=9,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
      SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x09) |
                    SIM_CLKDIV1_OUTDIV2(0x09) |
                    SIM_CLKDIV1_OUTDIV3(0x09) |
                    SIM_CLKDIV1_OUTDIV4(0x09); /* Update system prescalers */
      /* SIM_CLKDIV2: USBDIV=0,USBFRAC=0 */
      SIM_CLKDIV2 = (uint32_t)0x09UL; /* Update USB clock prescalers */
      /* SIM_SOPT2: PLLFLLSEL=1 */
      SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; /* Select PLL as a clock source for various peripherals */
      /* SIM_SOPT1: OSC32KSEL=0 */
      SIM_SOPT1 &= (uint32_t)~(uint32_t)(SIM_SOPT1_OSC32KSEL(0x03)); /* System oscillator drives 32 kHz clock for various peripherals */
      break;
    case CPU_CLOCK_CONFIG_2:
      /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=1,OUTDIV3=4,OUTDIV4=4,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
      SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) |
                    SIM_CLKDIV1_OUTDIV2(0x01) |
                    SIM_CLKDIV1_OUTDIV3(0x04) |
                    SIM_CLKDIV1_OUTDIV4(0x04); /* Set the system prescalers to safe value */
      if ((MCG_C2 & MCG_C2_IRCS_MASK) == 0x00U) {
        /* MCG_SC: FCRDIV=1 */
        MCG_SC = (uint8_t)((MCG_SC & (uint8_t)~(uint8_t)(
                  MCG_SC_FCRDIV(0x06)
                 )) | (uint8_t)(
                  MCG_SC_FCRDIV(0x01)
                 ));
      } else {
        /* MCG_C2: IRCS=0 */
        MCG_C2 &= (uint8_t)~(uint8_t)(MCG_C2_IRCS_MASK);
        while((MCG_S & MCG_S_IRCST_MASK) != 0x00U) { /* Check that the source internal reference clock is slow clock. */
        }
        /* MCG_SC: FCRDIV=1 */
        MCG_SC = (uint8_t)((MCG_SC & (uint8_t)~(uint8_t)(
                  MCG_SC_FCRDIV(0x06)
                 )) | (uint8_t)(
                  MCG_SC_FCRDIV(0x01)
                 ));
        /* MCG_C2: IRCS=1 */
        MCG_C2 |= MCG_C2_IRCS_MASK;
        while((MCG_S & MCG_S_IRCST_MASK) == 0x00U) { /* Check that the source internal reference clock is fast clock. */
        }
      }
      Cpu_SetMCG(1U);                  /* Update clock source setting */
      /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=0,OUTDIV3=0,OUTDIV4=3,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
      SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) |
                    SIM_CLKDIV1_OUTDIV2(0x00) |
                    SIM_CLKDIV1_OUTDIV3(0x00) |
                    SIM_CLKDIV1_OUTDIV4(0x03); /* Update system prescalers */
      /* SIM_CLKDIV2: USBDIV=0,USBFRAC=0 */
      SIM_CLKDIV2 = (uint32_t)0x09UL; /* Update USB clock prescalers */
      /* SIM_SOPT2: PLLFLLSEL=1 */
      SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; /* Select PLL as a clock source for various peripherals */
      /* SIM_SOPT1: OSC32KSEL=0 */
      SIM_SOPT1 &= (uint32_t)~(uint32_t)(SIM_SOPT1_OSC32KSEL(0x03)); /* System oscillator drives 32 kHz clock for various peripherals */
      break;
    default:
      break;
  }
  LDD_SetClockConfiguration(ModeID);   /* Call all LDD components to update the clock configuration */
  ClockConfigurationID = ModeID;       /* Store clock configuration identifier */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  Cpu_GetClockConfiguration (component MK21FN1M0MC12)
*/
/*!
**     @brief
**         Returns the active clock configuration identifier. The
**         method is automatically enabled if more than one clock
**         configuration is enabled in the component.
**     @return
**                         - Active clock configuration identifier
*/
/* ===================================================================*/
uint8_t Cpu_GetClockConfiguration(void)
{
  return ClockConfigurationID;         /* Return the actual clock configuration identifier */
}

/*
** ===================================================================
**     Method      :  Cpu_SetOperationMode (component MK21FN1M0MC12)
*/
/*!
**     @brief
**         This method requests to change the component's operation
**         mode (RUN, WAIT, SLEEP, STOP). The target operation mode
**         will be entered immediately.
**         See [Operation mode settings] for further details of the
**         operation modes mapping to low power modes of the cpu.
**     @param
**         OperationMode   - Requested driver
**                           operation mode
**     @param
**         ModeChangeCallback - Callback to
**                           notify the upper layer once a mode has been
**                           changed. Parameter is ignored, only for
**                           compatibility of API with other components.
**     @param
**         ModeChangeCallbackParamPtr
**                           - Pointer to callback parameter to notify
**                           the upper layer once a mode has been
**                           changed. Parameter is ignored, only for
**                           compatibility of API with other components.
**     @return
**                         - Error code
**                           ERR_OK - OK
**                           ERR_PARAM_MODE - Invalid operation mode
*/
/* ===================================================================*/
LDD_TError Cpu_SetOperationMode(LDD_TDriverOperationMode OperationMode, LDD_TCallback ModeChangeCallback, LDD_TCallbackParam *ModeChangeCallbackParamPtr)
{
  (void) ModeChangeCallback;           /* Parameter is not used, suppress unused argument warning */
  (void) ModeChangeCallbackParamPtr;   /* Parameter is not used, suppress unused argument warning */
  switch (OperationMode) {
    case DOM_RUN:
      /* SCB_SCR: SLEEPDEEP=0,SLEEPONEXIT=0 */
      SCB_SCR &= (uint32_t)~(uint32_t)(
                  SCB_SCR_SLEEPDEEP_MASK |
                  SCB_SCR_SLEEPONEXIT_MASK
                 );
      if  (ClockConfigurationID != 2U) {
        if ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST(3)) { /* If in PBE mode, switch to PEE. PEE to PBE transition was caused by wakeup from low power mode. */
          /* MCG_C1: CLKS=0,IREFS=0 */
          MCG_C1 &= (uint8_t)~(uint8_t)((MCG_C1_CLKS(0x03) | MCG_C1_IREFS_MASK));
          while( (MCG_S & MCG_S_LOCK0_MASK) == 0x00U) { /* Wait for PLL lock */
          }
        }
      }
      break;
    case DOM_WAIT:
      /* SCB_SCR: SLEEPDEEP=0 */
      SCB_SCR &= (uint32_t)~(uint32_t)(SCB_SCR_SLEEPDEEP_MASK);
      /* SCB_SCR: SLEEPONEXIT=0 */
      SCB_SCR &= (uint32_t)~(uint32_t)(SCB_SCR_SLEEPONEXIT_MASK);
      PE_WFI();
      break;
    case DOM_SLEEP:
      /* SCB_SCR: SLEEPDEEP=1 */
      SCB_SCR |= SCB_SCR_SLEEPDEEP_MASK;
      /* SMC_PMCTRL: STOPM=0 */
      SMC_PMCTRL &= (uint8_t)~(uint8_t)(SMC_PMCTRL_STOPM(0x07));
      (void)(SMC_PMCTRL == 0U);        /* Dummy read of SMC_PMCTRL to ensure the register is written before enterring low power mode */
      /* SCB_SCR: SLEEPONEXIT=1 */
      SCB_SCR |= SCB_SCR_SLEEPONEXIT_MASK;
      PE_WFI();
      break;
    case DOM_STOP:
      break;
    default:
      return ERR_PARAM_MODE;
  }
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  __pe_initialize_hardware (component MK21FN1M0MC12)
**
**     Description :
**         Initializes the whole system like timing, external bus, etc.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

void __pe_initialize_hardware(void)
{

  /*** !!! Here you can place your own code before PE initialization using property "User code before PE initialization" on the build options tab. !!! ***/

  /*** ### MK21FN1M0VMC12 "Cpu" init code ... ***/
  /*** PE initialization code after reset ***/

  /* Disable the WDOG module */
  /* WDOG_UNLOCK: WDOGUNLOCK=0xC520 */
  WDOG_UNLOCK = WDOG_UNLOCK_WDOGUNLOCK(0xC520); /* Key 1 */
  /* WDOG_UNLOCK: WDOGUNLOCK=0xD928 */
  WDOG_UNLOCK = WDOG_UNLOCK_WDOGUNLOCK(0xD928); /* Key 2 */
  /* WDOG_STCTRLH: ??=0,DISTESTWDOG=0,BYTESEL=0,TESTSEL=0,TESTWDOG=0,??=0,??=1,WAITEN=1,STOPEN=1,DBGEN=0,ALLOWUPDATE=1,WINEN=0,IRQRSTEN=0,CLKSRC=1,WDOGEN=0 */
  WDOG_STCTRLH = WDOG_STCTRLH_BYTESEL(0x00) |
                 WDOG_STCTRLH_WAITEN_MASK |
                 WDOG_STCTRLH_STOPEN_MASK |
                 WDOG_STCTRLH_ALLOWUPDATE_MASK |
                 WDOG_STCTRLH_CLKSRC_MASK |
                 0x0100U;

#if MQX_ENABLE_LOW_POWER
    /* Reset from LLWU wake up source */
    if (_lpm_get_reset_source() == MQX_RESET_SOURCE_LLWU)
    {
        PMC_REGSC |= PMC_REGSC_ACKISO_MASK;
    }
#endif
  /* SIM_SCGC6: RTC=1 */
  SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;
  if ((RTC_CR & RTC_CR_OSCE_MASK) == 0u) { /* Only if the OSCILLATOR is not already enabled */
    /* RTC_CR: SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
    RTC_CR &= (uint32_t)~(uint32_t)(
               RTC_CR_SC2P_MASK |
               RTC_CR_SC4P_MASK |
               RTC_CR_SC8P_MASK |
               RTC_CR_SC16P_MASK
              );
    /* RTC_CR: OSCE=1 */
    RTC_CR |= RTC_CR_OSCE_MASK;
    /* RTC_CR: CLKO=0 */
    RTC_CR &= (uint32_t)~(uint32_t)(RTC_CR_CLKO_MASK);
  }

  /* System clock initialization */
  /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=1,OUTDIV3=3,OUTDIV4=3,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) |
                SIM_CLKDIV1_OUTDIV2(0x01) |
                SIM_CLKDIV1_OUTDIV3(0x03) |
                SIM_CLKDIV1_OUTDIV4(0x03); /* Set the system prescalers to safe value */
  /* SIM_SCGC5: PORTD=1,PORTC=1,PORTA=1 */
  SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK |
               SIM_SCGC5_PORTC_MASK |
               SIM_SCGC5_PORTA_MASK;   /* Enable clock gate for ports to enable pin routing */
  /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=1,OUTDIV3=2,OUTDIV4=4,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) |
                SIM_CLKDIV1_OUTDIV2(0x01) |
                SIM_CLKDIV1_OUTDIV3(0x02) |
                SIM_CLKDIV1_OUTDIV4(0x04); /* Update system prescalers */
  /* SIM_CLKDIV2: USBDIV=0,USBFRAC=0 */
  SIM_CLKDIV2 = (uint32_t)0x09UL; /* Update USB clock prescalers */
  /* SIM_SOPT2: PLLFLLSEL=1 */
  SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; /* Select PLL as a clock source for various peripherals */
  /* SIM_SOPT1: OSC32KSEL=0 */
  SIM_SOPT1 &= (uint32_t)~(uint32_t)(SIM_SOPT1_OSC32KSEL(0x03)); /* System oscillator drives 32 kHz clock for various peripherals */
  /* PORTA_PCR18: ISF=0,MUX=0 */
  PORTA_PCR18 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
  /* PORTA_PCR19: ISF=0,MUX=0 */
  PORTA_PCR19 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
  /* Switch to FBE Mode */
  /* MCG_C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=1,LP=0,IRCS=0 */
  MCG_C2 = (MCG_C2_RANGE0(0x02) | MCG_C2_EREFS0_MASK);
  /* OSC_CR: ERCLKEN=1,??=0,EREFSTEN=1,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC_CR = (OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK);

  /* MCG_C7: OSCSEL=0 */
  MCG_C7 &= (uint8_t)~(uint8_t)(MCG_C7_OSCSEL_MASK);
  /* MCG_C1: CLKS=2,FRDIV=3,IREFS=0,IRCLKEN=0,IREFSTEN=0 */
  MCG_C1 = (MCG_C1_CLKS(0x02) | MCG_C1_FRDIV(0x03));
  /* MCG_C4: DMX32=0,DRST_DRS=0 */
  MCG_C4 &= (uint8_t)~(uint8_t)((MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x03)));
  /* MCG_C5: ??=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=1 */
  MCG_C5 = MCG_C5_PRDIV0(0x01);
  /* MCG_C6: LOLIE0=0,PLLS=0,CME0=0,VDIV0=6 */
  MCG_C6 = MCG_C6_VDIV0(0x06);
  while((MCG_S & MCG_S_OSCINIT0_MASK) == 0x00U) { /* Check that the oscillator is running */
  }
  while((MCG_S & MCG_S_IREFST_MASK) != 0x00U) { /* Check that the source of the FLL reference clock is the external reference clock. */
  }
  while((MCG_S & 0x0CU) != 0x08U) {    /* Wait until external reference clock is selected as MCG output */
  }
  /* Switch to PBE Mode */
  /* MCG_C6: LOLIE0=0,PLLS=1,CME0=0,VDIV0=6 */
  MCG_C6 = (MCG_C6_PLLS_MASK | MCG_C6_VDIV0(0x06));
  while((MCG_S & 0x0CU) != 0x08U) {    /* Wait until external reference clock is selected as MCG output */
  }
  while((MCG_S & MCG_S_LOCK0_MASK) == 0x00U) { /* Wait until locked */
  }
  /* Switch to PEE Mode */
  /* MCG_C1: CLKS=0,FRDIV=3,IREFS=0,IRCLKEN=0,IREFSTEN=0 */
  MCG_C1 = (MCG_C1_CLKS(0x00) | MCG_C1_FRDIV(0x03));
  while((MCG_S & 0x0CU) != 0x0CU) {    /* Wait until output of the PLL is selected */
  }
  /*** End of PE initialization code after reset ***/

  /*** !!! Here you can place your own code after PE initialization using property "User code after PE initialization" on the build options tab. !!! ***/

}



/*
** ===================================================================
**     Method      :  PE_low_level_init (component MK21FN1M0MC12)
**
**     Description :
**         Initializes beans and provides common register initialization.
**         The method is called automatically as a part of the
**         application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void PE_low_level_init(void)
{
}


/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.08]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/


#endif /* PE_LDD_VERSION */


uint16_t _bsp_set_clock_configuration
(
    /* [IN] runtime clock configuration */
    const BSP_CLOCK_CONFIGURATION clock_configuration
)
{
    uint16_t    cpu_error = ERR_OK;
    uint32_t    result;

    cpu_error = Cpu_SetClockConfiguration((uint8_t)clock_configuration);
    if (cpu_error != ERR_OK)    {
        return cpu_error;
    }

    /* Change frequency for system timer */
    result = hwtimer_set_freq(&systimer, BSP_SYSTIMER_SRC_CLK, BSP_ALARM_FREQUENCY);
    if (MQX_OK != result)
    {
        return ERR_FAILED;
    }

    return ERR_OK;
}


BSP_CLOCK_CONFIGURATION _bsp_get_clock_configuration
(
    void
)
{
    return (BSP_CLOCK_CONFIGURATION)Cpu_GetClockConfiguration();
}


uint32_t _bsp_get_clock
(
    const BSP_CLOCK_CONFIGURATION   clock_configuration,
    const CM_CLOCK_SOURCE           clock_source
)
{
    uint32_t clock = 0;

    if (clock_configuration < BSP_CLOCK_CONFIGURATIONS)    {
        if (clock_source  < (sizeof(PE_CpuClockConfigurations[0])/sizeof(uint32_t)))  {
            clock = *(((uint32_t *)&(PE_CpuClockConfigurations[clock_configuration])) + clock_source);
        }
        /* Return low power oscillator frequency which is not in clock structure */
        else if (clock_source == CM_CLOCK_SOURCE_LPO) {
            clock = (uint32_t)1000;
        }
    }

    return clock;
}


void _bsp_initialize_hardware(void)
{
    __pe_initialize_hardware();
}

void _bsp_low_level_init(void)
{
    PE_low_level_init();
}

