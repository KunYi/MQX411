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
const TCpuClockConfiguration PE_CpuClockConfigurations[CPU_CLOCK_CONFIG_NUMBER] = {
  /* Clock configuration 0 */
  {
    CPU_CORE_CLK_HZ_CONFIG_0,          /* Core clock frequency in clock configuration 0 */
    CPU_BUS_CLK_HZ_CONFIG_0,           /* Bus clock frequency in clock configuration 0 */
    CPU_FLEXBUS_CLK_HZ_CONFIG_0,       /* Flexbus clock frequency in clock configuration 0 */
    CPU_FLASH_CLK_HZ_CONFIG_0,         /* FLASH clock frequency in clock configuration 0 */
    CPU_USB_CLK_HZ_CONFIG_0,           /* USB clock frequency in clock configuration 0 */
    CPU_PLL_FLL_CLK_HZ_CONFIG_0,       /* PLL/FLL clock frequency in clock configuration 0 */
    CPU_MCGIR_CLK_HZ_CONFIG_0,         /* MCG internal reference clock frequency in clock configuration 0 */
    CPU_OSCER_CLK_HZ_CONFIG_0,         /* System OSC external reference clock frequency in clock configuration 0 */
    CPU_ERCLK32K_CLK_HZ_CONFIG_0,      /* External reference clock 32k frequency in clock configuration 0 */
    CPU_MCGFF_CLK_HZ_CONFIG_0          /* MCG fixed frequency clock */
  },
  /* Clock configuration 1 */
  {
    CPU_CORE_CLK_HZ_CONFIG_1,          /* Core clock frequency in clock configuration 1 */
    CPU_BUS_CLK_HZ_CONFIG_1,           /* Bus clock frequency in clock configuration 1 */
    CPU_FLEXBUS_CLK_HZ_CONFIG_1,       /* Flexbus clock frequency in clock configuration 1 */
    CPU_FLASH_CLK_HZ_CONFIG_1,         /* FLASH clock frequency in clock configuration 1 */
    CPU_USB_CLK_HZ_CONFIG_1,           /* USB clock frequency in clock configuration 1 */
    CPU_PLL_FLL_CLK_HZ_CONFIG_1,       /* PLL/FLL clock frequency in clock configuration 1 */
    CPU_MCGIR_CLK_HZ_CONFIG_1,         /* MCG internal reference clock frequency in clock configuration 1 */
    CPU_OSCER_CLK_HZ_CONFIG_1,         /* System OSC external reference clock frequency in clock configuration 1 */
    CPU_ERCLK32K_CLK_HZ_CONFIG_1,      /* External reference clock 32k frequency in clock configuration 1 */
    CPU_MCGFF_CLK_HZ_CONFIG_1          /* MCG fixed frequency clock */
  },
  /* Clock configuration 2 */
  {
    CPU_CORE_CLK_HZ_CONFIG_2,          /* Core clock frequency in clock configuration 2 */
    CPU_BUS_CLK_HZ_CONFIG_2,           /* Bus clock frequency in clock configuration 2 */
    CPU_FLEXBUS_CLK_HZ_CONFIG_2,       /* Flexbus clock frequency in clock configuration 2 */
    CPU_FLASH_CLK_HZ_CONFIG_2,         /* FLASH clock frequency in clock configuration 2 */
    CPU_USB_CLK_HZ_CONFIG_2,           /* USB clock frequency in clock configuration 2 */
    CPU_PLL_FLL_CLK_HZ_CONFIG_2,       /* PLL/FLL clock frequency in clock configuration 2 */
    CPU_MCGIR_CLK_HZ_CONFIG_2,         /* MCG internal reference clock frequency in clock configuration 2 */
    CPU_OSCER_CLK_HZ_CONFIG_2,         /* System OSC external reference clock frequency in clock configuration 2 */
    CPU_ERCLK32K_CLK_HZ_CONFIG_2,      /* External reference clock 32k frequency in clock configuration 2 */
    CPU_MCGFF_CLK_HZ_CONFIG_2          /* MCG fixed frequency clock */
  }
};

/* Global variables */
static uint8_t ClockConfigurationID = CPU_CLOCK_CONFIG_0; /* Active clock configuration */


/* Local function prototypes */
LDD_TError Cpu_MCGAutoTrim(uint8_t ClockSelect);
LDD_TError Cpu_VLPModeDisable(void);
LDD_TError Cpu_VLPModeEnable(void);
void       Cpu_EnableInt(void);
void       Cpu_DisableInt(void);
uint32_t   Cpu_GetLLSWakeUpFlags(void);
LDD_TError Cpu_SetClockConfiguration(LDD_TClockConfiguration ModeID);
uint8_t    Cpu_GetClockConfiguration(void);
LDD_TError Cpu_SetOperationMode(LDD_TDriverOperationMode OperationMode, LDD_TCallback ModeChangeCallback, LDD_TCallbackParam *ModeChangeCallbackParamPtr);

void       __pe_initialize_hardware(void);
void       PE_low_level_init(void);


/*
** ===================================================================
**     Method      :  LDD_SetClockConfiguration (component MK53N512MD100)
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

/*
** ===================================================================
**     Method      :  Cpu_MCGAutoTrim (component MK53N512MD100)
**
**     Description :
**         This method uses MCG auto trim feature to trim internal
**         reference clock. This method can be used only in a clock
**         configuration which derives its bus clock from external
**         reference clock (<MCG mode> must be one of the following
**         modes - FEE, FBE, BLPE, PEE, PBE) and if value of <Bus clock>
**         is in the range <8; 16>MHz.
**         The slow internal reference clock is trimmed to the value
**         selected by <Slow internal reference clock [kHz]> property.
**         The fast internal reference clock will be trimmed to value
**         4MHz.
**     Parameters  :
**         NAME            - DESCRIPTION
**         ClockSelect     - Selects which internal
**                           reference clock will be trimmed.
**                           0 ... slow (32kHz) internal reference clock
**                           will be trimmed
**                           > 0 ... fast (4MHz) internal reference
**                           clock will be trimmed
**     Returns     :
**         ---             - Error code
**                           ERR_OK - OK
**                           ERR_SPEED - The method does not work in the
**                           active clock configuration.
**                           ERR_FAILED - Autotrim process failed.
** ===================================================================
*/
LDD_TError Cpu_MCGAutoTrim(uint8_t ClockSelect)
{
  switch (ClockConfigurationID) {
    case CPU_CLOCK_CONFIG_1:
      if (ClockSelect == 0x00U) {
        /* Slow internal reference clock */
        MCG_ATCVH = 0x1EU;
        MCG_ATCVL = 0x0AU;
      } else {
        /* Fast internal reference clock */
        MCG_ATCVH = 0x1FU;
        MCG_ATCVL = 0x80U;
      }
      break;
    default:
      return ERR_SPEED;
  }
  if (ClockSelect == 0x00U) {
    /* MCG_ATC: ATME=1,ATMS=0,ATMF=0,??=0,??=0,??=0,??=0,??=0 */
    MCG_ATC = (uint8_t)0x80U;          /* Start trimming of the slow internal reference clock */
  } else {
    /* MCG_ATC: ATME=1,ATMS=1,ATMF=0,??=0,??=0,??=0,??=0,??=0 */
    MCG_ATC = (uint8_t)0xC0U;          /* Start trimming of the fast internal reference clock */
  }
  while ((MCG_ATC & MCG_ATC_ATME_MASK) != 0x00U) { /* Wait until autotrim completes */
  }
  if ((MCG_ATC & MCG_ATC_ATMF_MASK) == 0x00U) {
    return ERR_OK;                     /* Trim operation completed successfully */
  } else {
    return ERR_FAILED;                 /* Trim operation failed */
  }
}

/*
** ===================================================================
**     Method      :  Cpu_GetLLSWakeUpFlags (component MK53N512MD100)
**
**     Description :
**         This method returns the current status of the LLWU wake-up
**         flags indicating which wake-up source caused the MCU to exit
**         LLS or VLLSx low power mode.
**         The following predefined constants can be used to determine
**         the wake-up source:
**         LLWU_EXT_PIN0, ... LLWU_EXT_PIN15 - external pin 0 .. 15
**         caused the wake-up
**         LLWU_INT_MODULE0 .. LLWU_INT_MODULE7 - internal module 0..15
**         caused the wake-up.
**     Parameters  : None
**     Returns     :
**         ---             - Returns the current status of the LLWU
**                           wake-up flags indicating which wake-up
**                           source caused the MCU to exit LLS or VLLSx
**                           low power mode.
** ===================================================================
*/
uint32_t Cpu_GetLLSWakeUpFlags(void)
{
  uint32_t Flags;

  Flags = LLWU_F1;
  Flags |= (uint32_t)((uint32_t)LLWU_F2 << 8U);
  Flags |= (uint32_t)((uint32_t)LLWU_F3 << 16U);
  return Flags;
}

static void Cpu_SetMCGModePEE(uint8_t CLKMode);
/*
** ===================================================================
**     Method      :  Cpu_SetMCGModePEE (component MK53N512MD100)
**
**     Description :
**         This method sets the MCG to PEE mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

static void Cpu_SetMCGModePBE(uint8_t CLKMode);
/*
** ===================================================================
**     Method      :  Cpu_SetMCGModePBE (component MK53N512MD100)
**
**     Description :
**         This method sets the MCG to PBE mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

static void Cpu_SetMCGModeFBE(uint8_t CLKMode);
/*
** ===================================================================
**     Method      :  Cpu_SetMCGModeFBE (component MK53N512MD100)
**
**     Description :
**         This method sets the MCG to FBE mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

static void Cpu_SetMCGModeBLPI(uint8_t CLKMode);
/*
** ===================================================================
**     Method      :  Cpu_SetMCGModeBLPI (component MK53N512MD100)
**
**     Description :
**         This method sets the MCG to BLPI mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

static void Cpu_SetMCGModeFBI(uint8_t CLKMode);
/*
** ===================================================================
**     Method      :  Cpu_SetMCGModeFBI (component MK53N512MD100)
**
**     Description :
**         This method sets the MCG to FBI mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

static void Cpu_SetMCG(uint8_t CLKMode);
/*
** ===================================================================
**     Method      :  Cpu_SetMCG (component MK53N512MD100)
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
**     Method      :  Cpu_GetCurrentMCGMode (component MK53N512MD100)
**
**     Description :
**         This method returns the active MCG mode
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

/*
** ===================================================================
**     Method      :  Cpu_SetMCGModePEE (component MK60N512MD100)
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
      OSC_CR = (uint8_t)0xA0U;
      /* SIM_SOPT2: MCGCLKSEL=0 */
      SIM_SOPT2 &= (uint32_t)~0x01UL;
      /* MCG_C1: CLKS=0,FRDIV=5,IREFS=0,IRCLKEN=0,IREFSTEN=0 */
      MCG_C1 = (uint8_t)0x28U;
      /* MCG_C2: ??=0,??=0,RANGE=2,HGO=0,EREFS=0,LP=0,IRCS=1 */
      MCG_C2 = (uint8_t)0x21U;
      /* MCG_C5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV=0x18 */
      MCG_C5 = (uint8_t)0x18U;
      /* MCG_C6: LOLIE=0,PLLS=1,CME=0,VDIV=0x18 */
      MCG_C6 = (uint8_t)0x58U;
      while((MCG_S & 0x0CU) != 0x0CU) { /* Wait until output of the PLL is selected */
      }
      break;
    default:
      break;
  }
}

/*
** ===================================================================
**     Method      :  Cpu_SetMCGModePBE (component MK53N512MD100)
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
      OSC_CR = (uint8_t)0xA0U;
      /* SIM_SOPT2: MCGCLKSEL=0 */
      SIM_SOPT2 &= (uint32_t)~0x01UL;
      /* MCG_C1: CLKS=2,FRDIV=5,IREFS=0,IRCLKEN=0,IREFSTEN=0 */
      MCG_C1 = (uint8_t)0xA8U;
      /* MCG_C2: ??=0,??=0,RANGE=2,HGO=0,EREFS=0,LP=0,IRCS=1 */
      MCG_C2 = (uint8_t)0x21U;
      /* MCG_C5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV=0x18 */
      MCG_C5 = (uint8_t)0x18U;
      /* MCG_C6: LOLIE=0,PLLS=1,CME=0,VDIV=0x18 */
      MCG_C6 = (uint8_t)0x58U;
      while((MCG_S & 0x0CU) != 0x08U) { /* Wait until external reference clock is selected as MCG output */
      }
      while((MCG_S & MCG_S_LOCK_MASK) == 0x00U) { /* Wait until locked */
      }
      break;
    case 1U:
      /* Switch to PBE Mode */
      /* OSC_CR: ERCLKEN=0,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
      OSC_CR = (uint8_t)0x00U;
      /* SIM_SOPT2: MCGCLKSEL=0 */
      SIM_SOPT2 &= (uint32_t)~0x01UL;
      /* MCG_C1: CLKS=2,FRDIV=5,IREFS=0,IRCLKEN=1,IREFSTEN=1 */
      MCG_C1 = (uint8_t)0xABU;
      /* MCG_C2: ??=0,??=0,RANGE=2,HGO=0,EREFS=0,LP=0,IRCS=1 */
      MCG_C2 = (uint8_t)0x21U;
      /* MCG_C5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV=0x0D */
      MCG_C5 = (uint8_t)0x0DU;
      /* MCG_C6: LOLIE=0,PLLS=1,CME=0,VDIV=0 */
      MCG_C6 = (uint8_t)0x40U;
      while((MCG_S & 0x0CU) != 0x08U) { /* Wait until external reference clock is selected as MCG output */
      }
      while((MCG_S & MCG_S_LOCK_MASK) == 0x00U) { /* Wait until locked */
      }
      break;
    default:
      break;
  }
}

/*
** ===================================================================
**     Method      :  Cpu_SetMCGModeFBE (component MK53N512MD100)
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
      /* OSC_CR: ERCLKEN=1,??=0,EREFSTEN=1,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
      OSC_CR = (uint8_t)0xA0U;
      /* SIM_SOPT2: MCGCLKSEL=0 */
      SIM_SOPT2 &= (uint32_t)~0x01UL;
      /* MCG_C2: ??=0,??=0,RANGE=2,HGO=0,EREFS=0,LP=0,IRCS=1 */
      MCG_C2 = (uint8_t)0x21U;
      /* MCG_C1: CLKS=2,FRDIV=5,IREFS=0,IRCLKEN=0,IREFSTEN=0 */
      MCG_C1 = (uint8_t)0xA8U;
      /* MCG_C4: DMX32=0,DRST_DRS=0 */
      MCG_C4 &= (uint8_t)~(uint8_t)0xE0U;
      /* MCG_C5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV=0x18 */
      MCG_C5 = (uint8_t)0x18U;
      /* MCG_C6: LOLIE=0,PLLS=0,CME=0,VDIV=0x18 */
      MCG_C6 = (uint8_t)0x18U;
      while((MCG_S & MCG_S_IREFST_MASK) != 0x00U) { /* Check that the source of the FLL reference clock is the external reference clock. */
      }
      while((MCG_S & 0x0CU) != 0x08U) { /* Wait until external reference clock is selected as MCG output */
      }
      break;
    case 1U:
      /* Switch to FBE Mode */
      /* OSC_CR: ERCLKEN=0,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
      OSC_CR = (uint8_t)0x00U;
      /* SIM_SOPT2: MCGCLKSEL=0 */
      SIM_SOPT2 &= (uint32_t)~0x01UL;
      /* MCG_C2: ??=0,??=0,RANGE=2,HGO=0,EREFS=0,LP=0,IRCS=1 */
      MCG_C2 = (uint8_t)0x21U;
      /* MCG_C1: CLKS=2,FRDIV=5,IREFS=0,IRCLKEN=1,IREFSTEN=1 */
      MCG_C1 = (uint8_t)0xABU;
      /* MCG_C4: DMX32=0,DRST_DRS=0 */
      MCG_C4 &= (uint8_t)~(uint8_t)0xE0U;
      /* MCG_C5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV=0 */
      MCG_C5 = (uint8_t)0x00U;
      /* MCG_C6: LOLIE=0,PLLS=0,CME=0,VDIV=0 */
      MCG_C6 = (uint8_t)0x00U;
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
**     Method      :  Cpu_SetMCGModeBLPI (component MK53N512MD100)
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
      /* Switch to FEI Mode */
      /* MCG_C1: CLKS=1,FRDIV=0,IREFS=1,IRCLKEN=1,IREFSTEN=1 */
      MCG_C1 = (uint8_t)0x47U;
      /* MCG_C2: ??=0,??=0,RANGE=2,HGO=0,EREFS=0,LP=1,IRCS=1 */
      MCG_C2 = (uint8_t)0x23U;
      /* OSC_CR: ERCLKEN=0,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
      OSC_CR = (uint8_t)0x00U;
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
**     Method      :  Cpu_SetMCGModeFBI (component MK53N512MD100)
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
      /* Switch to FEI Mode */
      /* MCG_C1: CLKS=1,FRDIV=0,IREFS=1,IRCLKEN=0,IREFSTEN=0 */
      MCG_C1 = (uint8_t)0x44U;
      /* MCG_C2: ??=0,??=0,RANGE=2,HGO=0,EREFS=0,LP=0,IRCS=1 */
      MCG_C2 = (uint8_t)0x21U;
      /* MCG_C4: DMX32=0,DRST_DRS=0 */
      MCG_C4 &= (uint8_t)~(uint8_t)0xE0U;
      /* OSC_CR: ERCLKEN=1,??=0,EREFSTEN=1,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
      OSC_CR = (uint8_t)0xA0U;
      /* SIM_SOPT2: MCGCLKSEL=0 */
      SIM_SOPT2 &= (uint32_t)~0x01UL;
      /* MCG_C5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV=0x18 */
      MCG_C5 = (uint8_t)0x18U;
      /* MCG_C6: LOLIE=0,PLLS=0,CME=0,VDIV=0x18 */
      MCG_C6 = (uint8_t)0x18U;
      while((MCG_S & MCG_S_IREFST_MASK) == 0x00U) { /* Check that the source of the FLL reference clock is the internal reference clock. */
      }
      while((MCG_S & 0x0CU) != 0x04U) { /* Wait until internal reference clock is selected as MCG output */
      }
      break;
    case 1U:
      /* Switch to FEI Mode */
      /* MCG_C1: CLKS=1,FRDIV=0,IREFS=1,IRCLKEN=1,IREFSTEN=1 */
      MCG_C1 = (uint8_t)0x47U;
      /* MCG_C2: ??=0,??=0,RANGE=2,HGO=0,EREFS=0,LP=0,IRCS=1 */
      MCG_C2 = (uint8_t)0x21U;
      /* MCG_C4: DMX32=0,DRST_DRS=0 */
      MCG_C4 &= (uint8_t)~(uint8_t)0xE0U;
      /* OSC_CR: ERCLKEN=0,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
      OSC_CR = (uint8_t)0x00U;
      /* SIM_SOPT2: MCGCLKSEL=0 */
      SIM_SOPT2 &= (uint32_t)~0x01UL;
      /* MCG_C5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV=0 */
      MCG_C5 = (uint8_t)0x00U;
      /* MCG_C6: LOLIE=0,PLLS=0,CME=0,VDIV=0 */
      MCG_C6 = (uint8_t)0x00U;
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
**     Method      :  Cpu_SetMCG (component MK53N512MD100)
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
**     Method      :  Cpu_GetCurrentMCGMode (component MK53N512MD100)
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
**     Method      :  Cpu_SetClockConfiguration (component MK53N512MD100)
**
**     Description :
**         Calling of this method will cause the clock configuration
**         change and reconfiguration of all components according to
**         the requested clock configuration setting.
**     Parameters  :
**         NAME            - DESCRIPTION
**         ModeID          - Clock configuration identifier
**     Returns     :
**         ---             - ERR_OK - OK.
**                           ERR_RANGE - Mode parameter out of range
** ===================================================================
*/
LDD_TError Cpu_SetClockConfiguration(LDD_TClockConfiguration ModeID)
{
  if (ModeID > 0x02U) {
    return ERR_RANGE;                  /* Undefined clock configuration requested requested */
  }
  switch (ModeID) {
    case CPU_CLOCK_CONFIG_0:
      if (ClockConfigurationID == 2U) {
        /* Clock configuration 0 and clock configuration 2 use different clock configuration */
        /* MCG_C6: CME=0 */
        MCG_C6 &= (uint8_t)~(uint8_t)0x20U; /* Disable the clock monitor */
        /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=1,OUTDIV3=3,OUTDIV4=3,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
        SIM_CLKDIV1 = (uint32_t)0x01330000UL; /* Set the system prescalers to safe value */
        Cpu_SetMCG(0U);                /* Update clock source setting */
        /* MCG_C6: CME=1 */
        MCG_C6 |= (uint8_t)0x20U;      /* Enable the clock monitor */
      }
      /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=1,OUTDIV3=1,OUTDIV4=3,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
      SIM_CLKDIV1 = (uint32_t)0x01130000UL; /* Update system prescalers */
      /* SIM_CLKDIV2: USBDIV=1,USBFRAC=0 */
      SIM_CLKDIV2 = (uint32_t)((SIM_CLKDIV2 & (uint32_t)~0x0DUL) | (uint32_t)0x02UL); /* Update USB clock prescalers */
      /* SIM_SOPT2: PLLFLLSEL=1 */
      SIM_SOPT2 |= (uint32_t)0x00010000UL; /* Select PLL as a clock source for various peripherals */
      /* SIM_SOPT1: OSC32KSEL=1 */
      SIM_SOPT1 |= (uint32_t)0x00080000UL; /* RTC oscillator drives 32 kHz clock for various peripherals */
      break;
    case CPU_CLOCK_CONFIG_1:
      if (ClockConfigurationID == 2U) {
        /* Clock configuration 1 and clock configuration 2 use different clock configuration */
        /* MCG_C6: CME=0 */
        MCG_C6 &= (uint8_t)~(uint8_t)0x20U; /* Disable the clock monitor */
        /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=1,OUTDIV3=3,OUTDIV4=3,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
        SIM_CLKDIV1 = (uint32_t)0x01330000UL; /* Set the system prescalers to safe value */
        Cpu_SetMCG(0U);                /* Update clock source setting */
        /* MCG_C6: CME=1 */
        MCG_C6 |= (uint8_t)0x20U;      /* Enable the clock monitor */
      }
      /* SIM_CLKDIV1: OUTDIV1=7,OUTDIV2=7,OUTDIV3=7,OUTDIV4=7,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
      SIM_CLKDIV1 = (uint32_t)0x77770000UL; /* Update system prescalers */
      /* SIM_CLKDIV2: USBDIV=1,USBFRAC=0 */
      SIM_CLKDIV2 = (uint32_t)((SIM_CLKDIV2 & (uint32_t)~0x0DUL) | (uint32_t)0x02UL); /* Update USB clock prescalers */
      /* SIM_SOPT2: PLLFLLSEL=1 */
      SIM_SOPT2 |= (uint32_t)0x00010000UL; /* Select PLL as a clock source for various peripherals */
      /* SIM_SOPT1: OSC32KSEL=1 */
      SIM_SOPT1 |= (uint32_t)0x00080000UL; /* RTC oscillator drives 32 kHz clock for various peripherals */
      break;
    case CPU_CLOCK_CONFIG_2:
      /* MCG_C6: CME=0 */
      MCG_C6 &= (uint8_t)~(uint8_t)0x20U; /* Disable the clock monitor */
      /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=1,OUTDIV3=3,OUTDIV4=3,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
      SIM_CLKDIV1 = (uint32_t)0x01330000UL; /* Set the system prescalers to safe value */
      Cpu_SetMCG(1U);                  /* Update clock source setting */
      /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=0,OUTDIV3=0,OUTDIV4=3,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
      SIM_CLKDIV1 = (uint32_t)0x00030000UL; /* Update system prescalers */
      /* SIM_CLKDIV2: USBDIV=0,USBFRAC=0 */
      SIM_CLKDIV2 &= (uint32_t)~0x0FUL; /* Update USB clock prescalers */
      /* SIM_SOPT2: PLLFLLSEL=1 */
      SIM_SOPT2 |= (uint32_t)0x00010000UL; /* Select PLL as a clock source for various peripherals */
      /* SIM_SOPT1: OSC32KSEL=1 */
      SIM_SOPT1 |= (uint32_t)0x00080000UL; /* RTC oscillator drives 32 kHz clock for various peripherals */
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
**     Method      :  Cpu_GetClockConfiguration (component MK53N512MD100)
**
**     Description :
**         Returns the active clock configuration identifier. The
**         method is enabled only if more than one clock configuration
**         is enabled in the component.
**     Parameters  : None
**     Returns     :
**         ---             - Active clock configuration identifier
** ===================================================================
*/
uint8_t Cpu_GetClockConfiguration(void)
{
  return ClockConfigurationID;         /* Return the actual clock configuration identifier */
}

/*
** ===================================================================
**     Method      :  Cpu_SetOperationMode (component MK53N512MD100)
**
**     Description :
**         This method requests to change the component's operation
**         mode (RUN, WAIT, SLEEP, STOP). The target operation mode
**         will be entered immediately.
**         See <Operation mode settings> for further details of the
**         operation modes mapping to low power modes of the cpu.
**     Parameters  :
**         NAME            - DESCRIPTION
**         OperationMode   - Requested driver
**                           operation mode
**         ModeChangeCallback - Callback to
**                           notify the upper layer once a mode has been
**                           changed. Parameter is ignored, only for
**                           compatibility of API with other components.
**       * ModeChangeCallbackParamPtr
**                           - Pointer to callback parameter to notify
**                           the upper layer once a mode has been
**                           changed. Parameter is ignored, only for
**                           compatibility of API with other components.
**     Returns     :
**         ---             - Error code
**                           ERR_OK - OK
**                           ERR_PARAM_MODE - Invalid operation mode
** ===================================================================
*/
LDD_TError Cpu_SetOperationMode(LDD_TDriverOperationMode OperationMode, LDD_TCallback ModeChangeCallback, LDD_TCallbackParam *ModeChangeCallbackParamPtr)
{
  (void) ModeChangeCallback;           /* Parameter is not used, suppress unused argument warning */
  (void) ModeChangeCallbackParamPtr;   /* Parameter is not used, suppress unused argument warning */
  switch (OperationMode) {
    case DOM_RUN:
      /* SCB_SCR: SLEEPDEEP=0,SLEEPONEXIT=0 */
      SCB_SCR &= (uint32_t)~0x06UL;
      if  (ClockConfigurationID != 2U) {
        if ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST(3)) { /* If in PBE mode, switch to PEE. PEE to PBE transition was caused by wakeup from low power mode. */
          /* MCG_C1: CLKS=0,IREFS=0 */
          MCG_C1 &= (uint8_t)~(uint8_t)0xC4U;
          while( (MCG_S & MCG_S_LOCK_MASK) == 0x00U) { /* Wait for PLL lock */
          }
        }
      }
      break;
    case DOM_WAIT:
      /* SCB_SCR: SLEEPDEEP=0 */
      SCB_SCR &= (uint32_t)~0x04UL;
      /* SCB_SCR: SLEEPONEXIT=0 */
      SCB_SCR &= (uint32_t)~0x02UL;
      PE_WFI();
      break;
    case DOM_SLEEP:
      /* SCB_SCR: SLEEPDEEP=1 */
      SCB_SCR |= (uint32_t)0x04UL;
      /* MC_PMCTRL: LPLLSM=0 */
      MC_PMCTRL &= (uint8_t)~(uint8_t)0x07U;
      /* SCB_SCR: SLEEPONEXIT=1 */
      SCB_SCR |= (uint32_t)0x02UL;
      PE_WFI();
      break;
    case DOM_STOP:
    /* Clear LLWU flags */
      /* LLWU_F1: WUF7=1,WUF6=1,WUF5=1,WUF4=1,WUF3=1,WUF2=1,WUF1=1,WUF0=1 */
      LLWU_F1 = (uint8_t)0xFFU;
      /* LLWU_F2: WUF15=1,WUF14=1,WUF13=1,WUF12=1,WUF11=1,WUF10=1,WUF9=1,WUF8=1 */
      LLWU_F2 = (uint8_t)0xFFU;
      /* LLWU_F3: MWUF7=1,MWUF6=1,MWUF5=1,MWUF4=1,MWUF3=1,MWUF2=1,MWUF1=1,MWUF0=1 */
      LLWU_F3 = (uint8_t)0xFFU;
      /* SCB_SCR: SLEEPONEXIT=0 */
      SCB_SCR &= (uint32_t)~0x02UL;
      /* SCB_SCR: SLEEPDEEP=1 */
      SCB_SCR |= (uint32_t)0x04UL;
      /* MC_PMCTRL: LPLLSM=3 */
      MC_PMCTRL = (uint8_t)((MC_PMCTRL & (uint8_t)~(uint8_t)0x04U) | (uint8_t)0x03U);
      PE_WFI();
      break;
    default:
      return ERR_PARAM_MODE;
  }
  return ERR_OK;
}


/*
** ===================================================================
**     Method      :  __pe_initialize_hardware (component MK53N512MD100)
**
**     Description :
**         Initializes the whole system like timing, external bus, etc.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

/*** !!! Here you can place your own code using property "User data declarations" on the build options tab. !!! ***/

void __pe_initialize_hardware(void)
{
  _bsp_watchdog_disable();

  /*** !!! Here you can place your own code before PE initialization using property "User code before PE initialization" on the build options tab. !!! ***/

  /*** ### MK40DX256ZVMD10 "Cpu" init code ... ***/
  /*** PE initialization code after reset ***/
  /* System clock initialization */
  /* SIM_SCGC5: PORTA=1 */
  SIM_SCGC5 |= (uint32_t)0x0202UL;     /* Enable clock gate for ports to enable pin routing */
  if ( *((uint8_t*) 0x03FFU) != 0xFFU) {
    MCG_C3 = *((uint8_t*) 0x03FFU);
    MCG_C4 = (MCG_C4 & 0xE0U) | ((*((uint8_t*) 0x03FEU)) & 0x1FU);
  }
  /* SIM_CLKDIV1: OUTDIV1=0,OUTDIV2=1,OUTDIV3=1,OUTDIV4=3,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM_CLKDIV1 = (uint32_t)0x01130000UL; /* Update system prescalers */
  /* SIM_CLKDIV2: USBDIV=1,USBFRAC=0 */
  SIM_CLKDIV2 = (uint32_t)((SIM_CLKDIV2 & (uint32_t)~0x0DUL) | (uint32_t)0x02UL); /* Update USB clock prescalers */
  /* SIM_SOPT2: PLLFLLSEL=1 */
  SIM_SOPT2 |= (uint32_t)0x00010000UL; /* Select PLL as a clock source for various peripherals */
  /* SIM_SOPT1: OSC32KSEL=1 */
  SIM_SOPT1 |= (uint32_t)0x00080000UL; /* RTC oscillator drives 32 kHz clock for various peripherals */
  /* PORTA_PCR18: ISF=0,MUX=0 */
  PORTA_PCR18 &= (uint32_t)~0x01000700UL;
  /* Switch to FBE Mode */
  /* OSC_CR: ERCLKEN=1,??=0,EREFSTEN=1,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC_CR = (uint8_t)0xA0U;
  /* SIM_SOPT2: MCGCLKSEL=0 */
  SIM_SOPT2 &= (uint32_t)~0x01UL;
  /* MCG_C2: ??=0,??=0,RANGE=2,HGO=0,EREFS=0,LP=0,IRCS=1 */
  MCG_C2 = (uint8_t)0x21U;
  /* MCG_C1: CLKS=2,FRDIV=5,IREFS=0,IRCLKEN=0,IREFSTEN=0 */
  MCG_C1 = (uint8_t)0xA8U;
  /* MCG_C4: DMX32=0,DRST_DRS=0 */
  MCG_C4 &= (uint8_t)~(uint8_t)0xE0U;
  /* MCG_C5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV=0x18 */
  MCG_C5 = (uint8_t)0x18U;
  /* MCG_C6: LOLIE=0,PLLS=0,CME=0,VDIV=0x18 */
  MCG_C6 = (uint8_t)0x18U;
  while((MCG_S & MCG_S_IREFST_MASK) != 0x00U) { /* Check that the source of the FLL reference clock is the external reference clock. */
  }
  while((MCG_S & 0x0CU) != 0x08U) {    /* Wait until external reference clock is selected as MCG output */
  }
  /* Switch to PBE Mode */
  /* OSC_CR: ERCLKEN=1,??=0,EREFSTEN=1,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC_CR = (uint8_t)0xA0U;
  /* SIM_SOPT2: MCGCLKSEL=0 */
  SIM_SOPT2 &= (uint32_t)~0x01UL;
  /* MCG_C1: CLKS=2,FRDIV=5,IREFS=0,IRCLKEN=0,IREFSTEN=0 */
  MCG_C1 = (uint8_t)0xA8U;
  /* MCG_C2: ??=0,??=0,RANGE=2,HGO=0,EREFS=0,LP=0,IRCS=1 */
  MCG_C2 = (uint8_t)0x21U;
  /* MCG_C5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV=0x18 */
  MCG_C5 = (uint8_t)0x18U;
  /* MCG_C6: LOLIE=0,PLLS=1,CME=0,VDIV=0x18 */
  MCG_C6 = (uint8_t)0x58U;
  while((MCG_S & 0x0CU) != 0x08U) {    /* Wait until external reference clock is selected as MCG output */
  }
  while((MCG_S & MCG_S_LOCK_MASK) == 0x00U) { /* Wait until locked */
  }
  /* Switch to PEE Mode */
  /* OSC_CR: ERCLKEN=1,??=0,EREFSTEN=1,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC_CR = (uint8_t)0xA0U;
  /* SIM_SOPT2: MCGCLKSEL=0 */
  SIM_SOPT2 &= (uint32_t)~0x01UL;
  /* MCG_C1: CLKS=0,FRDIV=5,IREFS=0,IRCLKEN=0,IREFSTEN=0 */
  MCG_C1 = (uint8_t)0x28U;
  /* MCG_C2: ??=0,??=0,RANGE=2,HGO=0,EREFS=0,LP=0,IRCS=1 */
  MCG_C2 = (uint8_t)0x21U;
  /* MCG_C5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV=0x18 */
  MCG_C5 = (uint8_t)0x18U;
  /* MCG_C6: LOLIE=0,PLLS=1,CME=0,VDIV=0x18 */
  MCG_C6 = (uint8_t)0x58U;
  while((MCG_S & 0x0CU) != 0x0CU) {    /* Wait until output of the PLL is selected */
  }
  /* MCG_C6: CME=1 */
  MCG_C6 |= (uint8_t)0x20U;            /* Enable the clock monitor */
  /*** End of PE initialization code after reset ***/

  /*** !!! Here you can place your own code after PE initialization using property "User code after PE initialization" on the build options tab. !!! ***/

}


/*
** ===================================================================
**     Method      :  PE_low_level_init (component MK53N512MD100)
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
      /* Initialization of the SIM module */
  /* SIM_SOPT6: RSTFLTEN=0,RSTFLTSEL=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM_SOPT6 = (uint32_t)0x00UL;        /* Set reset pin filter */
      /* Initialization of the PMC module */
  /* PMC_LVDSC1: LVDACK=1,LVDIE=0,LVDRE=1,LVDV=0 */
  PMC_LVDSC1 = (uint8_t)((PMC_LVDSC1 & (uint8_t)~(uint8_t)0x23U) | (uint8_t)0x50U);
  /* PMC_LVDSC2: LVWACK=1,LVWIE=0,LVWV=0 */
  PMC_LVDSC2 = (uint8_t)((PMC_LVDSC2 & (uint8_t)~(uint8_t)0x23U) | (uint8_t)0x40U);
  /* PMC_REGSC: TRAMPO=0,??=0,BGBE=0 */
  PMC_REGSC &= (uint8_t)~(uint8_t)0x13U;
  /* SIM_SCGC4: LLWU=1 */
  SIM_SCGC4 |= (uint32_t)0x10000000UL;
        /* Initialization of the LLWU module */
  /* LLWU_PE1: WUPE3=0,WUPE2=0,WUPE1=0,WUPE0=0 */
  LLWU_PE1 = (uint8_t)0x00U;
  /* LLWU_PE2: WUPE7=0,WUPE6=0,WUPE5=0,WUPE4=0 */
  LLWU_PE2 = (uint8_t)0x00U;
  /* LLWU_PE3: WUPE11=0,WUPE10=0,WUPE9=0,WUPE8=0 */
  LLWU_PE3 = (uint8_t)0x00U;
  /* LLWU_PE4: WUPE15=0,WUPE14=0,WUPE13=0,WUPE12=0 */
  LLWU_PE4 = (uint8_t)0x00U;
  /* LLWU_ME: WUME7=0,WUME5=1,WUME4=0,WUME3=0,WUME2=0,WUME1=0,WUME0=0 */
  LLWU_ME = (uint8_t)((LLWU_ME & (uint8_t)~(uint8_t)0x9FU) | (uint8_t)0x20U);
  /* LLWU_CS: ACKISO=1,??=1,FLTEP=0,FLTR=0 */
  LLWU_CS = (uint8_t)((LLWU_CS & (uint8_t)~(uint8_t)0x03U) | (uint8_t)0x84U);
}

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


uint16_t _bsp_osc_autotrim
(
    void
)
{
    uint16_t        CPU_Error = ERR_OK;
    /*
     * Its assumed that before auto trimming process
     * the MCG is switched to a clock configuration
     * which derives its bus clock from external reference clock
     * and (<MCG mode> is set to one of the following modes
     * FEE, FBE, BLPE, PEE, PBE) and if value of <Bus clock>
     * is in the range <8; 16>MHz.
     */

    /* Auto trim Slow internal reference clock */
    CPU_Error = Cpu_MCGAutoTrim(0);
    if (CPU_Error != ERR_OK) return CPU_Error;

    /* Auto trim Fast internal reference clock */
    CPU_Error = Cpu_MCGAutoTrim(1);
    if (CPU_Error != ERR_OK) return CPU_Error;

    return ERR_OK;
}

void _bsp_low_level_init(void)
{
    PE_low_level_init();
}



void _bsp_initialize_hardware(void)
{
    __pe_initialize_hardware();
}
