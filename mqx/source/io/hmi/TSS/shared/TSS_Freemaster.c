/*HEADER**********************************************************************
*
* Copyright 2008-2011 Freescale Semiconductor, Inc.
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
*   This file contains FreeMASTER interface for TSS variables.
*
*
*END************************************************************************/

#include "TSS_API.h"

#if TSS_USE_FREEMASTER_GUI
  #include "freemaster.h"

  typedef struct {
    uint32_t indexdata;
    uint32_t flagreg;
  } FMSTR_WRITE_STRUCT;

  FMSTR_WRITE_STRUCT fmstr_write = {0xffffffff, 0xff};

  typedef struct {
    uint8_t low_bit            :1;
    uint8_t low_bit1           :1;
    uint8_t low_bit2           :1;
    uint8_t low_bit3           :1;
    uint8_t low_bit4           :1;
    uint8_t low_bit5           :1;
    uint8_t low_bit6           :1;
    uint8_t low_bit7           :1;
  } FMSTR_BIT_STRUCT;

  typedef struct{
    uint8_t             ptr_size;
    FMSTR_BIT_STRUCT  low_bit_mask;
  } FMSTR_CONFIG_STRUCT;

  volatile FMSTR_CONFIG_STRUCT fmstr_config = {sizeof(const uint8_t *), {1,0,0,0,0,0,0,0}};

  /////////////////////////////////////////////////////////////////////////

  extern uint16_t tss_au16ElecBaseline[TSS_N_ELECTRODES];
  extern uint8_t tss_cu8NumElecs;
  extern const uint8_t tss_cu8NumCtrls;
  extern const uint16_t tss_cu16Version;
  #if TSS_N_CONTROLS > 0
    extern const uint8_t * const tss_acpsCSStructs[TSS_N_CONTROLS];
    extern uint8_t * const tss_acpsDecRAMContext[TSS_N_CONTROLS];
    extern const uint8_t * const tss_acpsDecROMContext[TSS_N_CONTROLS];
  #endif
  #if (TSS_USE_KEYDETECTOR_VERSION == 2)
    extern TSS_KeyDetectorFiltersData tss_asKeyDetectorFiltersData[TSS_N_ELECTRODES];
  #endif

  void TSS_FmstrCall(void);
  void TSS_FmstrIsrCallback(void);

  #if FMSTR_USE_TSA
    /* FreeMASTER TSA table for TSS variables */
    FMSTR_TSA_TABLE_BEGIN(TSS_Table)
      //FreeMASTER interface to write
      FMSTR_TSA_RW_VAR(fmstr_write,                         FMSTR_TSA_USERTYPE(FMSTR_WRITE_STRUCT))
      FMSTR_TSA_STRUCT(FMSTR_WRITE_STRUCT)
      FMSTR_TSA_MEMBER(FMSTR_WRITE_STRUCT, indexdata,       FMSTR_TSA_UINT32)
      FMSTR_TSA_MEMBER(FMSTR_WRITE_STRUCT, flagreg,         FMSTR_TSA_UINT32)

      FMSTR_TSA_RW_VAR(fmstr_config,                        FMSTR_TSA_USERTYPE(FMSTR_CONFIG_STRUCT))
      FMSTR_TSA_STRUCT(FMSTR_CONFIG_STRUCT)
      FMSTR_TSA_MEMBER(FMSTR_CONFIG_STRUCT, ptr_size,       FMSTR_TSA_UINT8)
      FMSTR_TSA_MEMBER(FMSTR_CONFIG_STRUCT, low_bit_mask,   FMSTR_TSA_UINT8)

      //TSS system config menu
      FMSTR_TSA_RW_VAR(tss_CSSys,                                  FMSTR_TSA_USERTYPE(TSS_CSSystem))
      FMSTR_TSA_STRUCT(TSS_CSSystem)
      FMSTR_TSA_MEMBER(TSS_CSSystem, Faults,                       FMSTR_TSA_UINT8)
      FMSTR_TSA_MEMBER(TSS_CSSystem, SystemConfig,                 FMSTR_TSA_UINT8)
      FMSTR_TSA_MEMBER(TSS_CSSystem, NSamples,                     FMSTR_TSA_UINT8)
      FMSTR_TSA_MEMBER(TSS_CSSystem, DCTrackerRate,                FMSTR_TSA_UINT8)
      FMSTR_TSA_MEMBER(TSS_CSSystem, SlowDCTrackerFactor,          FMSTR_TSA_UINT8)
      FMSTR_TSA_MEMBER(TSS_CSSystem, ResponseTime,                 FMSTR_TSA_UINT8)
      FMSTR_TSA_MEMBER(TSS_CSSystem, StuckKeyTimeout,              FMSTR_TSA_UINT8)
      FMSTR_TSA_MEMBER(TSS_CSSystem, LowPowerElectrode,            FMSTR_TSA_UINT8)
      FMSTR_TSA_MEMBER(TSS_CSSystem, LowPowerElectrodeSensitivity, FMSTR_TSA_UINT8)
      FMSTR_TSA_MEMBER(TSS_CSSystem, SystemTrigger,                FMSTR_TSA_UINT8)

      // Keydetector 2 data
      #if (TSS_USE_KEYDETECTOR_VERSION == 2)
        FMSTR_TSA_RO_VAR(tss_asKeyDetectorFiltersData,             FMSTR_TSA_USERTYPE(TSS_KeyDetectorFiltersData))
        FMSTR_TSA_STRUCT(TSS_KeyDetectorFiltersData)
        FMSTR_TSA_MEMBER(TSS_KeyDetectorFiltersData, xIIR,         FMSTR_TSA_SINT16)
        FMSTR_TSA_MEMBER(TSS_KeyDetectorFiltersData, yIIR,         FMSTR_TSA_SINT32)
      #endif

      //electrodes config menu
      #if TSS_USE_DELTA_LOG
        FMSTR_TSA_RO_VAR(tss_ai8InstantDelta,     FMSTR_TSA_UINT8)
      #endif
      #if TSS_USE_SIGNAL_LOG
        FMSTR_TSA_RO_VAR(tss_au16InstantSignal,   FMSTR_TSA_UINT16)
      #endif
      #if TSS_USE_INTEGRATION_DELTA_LOG
        FMSTR_TSA_RO_VAR(tss_ai8IntegrationDelta, FMSTR_TSA_SINT8)
      #endif
      FMSTR_TSA_RO_VAR(tss_au16ElecBaseline,    FMSTR_TSA_UINT16)
      FMSTR_TSA_RO_VAR(tss_au8ElectrodeStatus,  FMSTR_TSA_UINT8)
      FMSTR_TSA_RW_VAR(tss_au8Sensitivity,      FMSTR_TSA_UINT8)
      FMSTR_TSA_RW_VAR(tss_au8ElectrodeEnablers,FMSTR_TSA_UINT8)
      FMSTR_TSA_RW_VAR(tss_au8ElectrodeDCTrackerEnablers,FMSTR_TSA_UINT8)
      FMSTR_TSA_RO_VAR(tss_cu8NumElecs,         FMSTR_TSA_UINT8)
      #if ((TSS_USE_NOISE_MODE == 1) && (TSS_USE_NOISE_SIGNAL_LOG == 1))
        FMSTR_TSA_RO_VAR(tss_au8InstantNoise,   FMSTR_TSA_UINT8)
      #endif
      //controls menu
      FMSTR_TSA_RO_VAR(tss_cu8NumCtrls,         FMSTR_TSA_UINT8)
      #if TSS_N_CONTROLS > 0
        FMSTR_TSA_RO_VAR(tss_acpsCSStructs,     FMSTR_TSA_UINT32)
        FMSTR_TSA_RO_VAR(tss_acpsDecRAMContext, FMSTR_TSA_UINT32)
        FMSTR_TSA_RO_VAR(tss_acpsDecROMContext, FMSTR_TSA_UINT32)
      #endif
      // TSS version information
      FMSTR_TSA_RO_VAR(tss_cu16Version,         FMSTR_TSA_UINT16)
    FMSTR_TSA_TABLE_END()
  #endif

  /*
   * FreeMASTER interface to write configure TSS feature over FreeMASTER
   */
  void TSS_FmstrCall(void)
  {
    uint16_t u16DummyVersion;
    FMSTR_CONFIG_STRUCT dummy_fmstr_config;
    TSS_CONTROL_ID controlID;

    if (fmstr_write.flagreg != 0xff)
    {
      uint8_t type = (uint8_t) (fmstr_write.indexdata>>24);
      uint8_t index = (uint8_t) ((fmstr_write.indexdata>>16)&0xff);
      uint16_t data = (uint16_t) ((fmstr_write.indexdata)&0xffff);

      *((uint8_t*)&controlID)  = (uint8_t) ((fmstr_write.indexdata>>8)&0xff);
      switch (type)
      {
        case 0:
          TSS_SetSystemConfig(index, data);
          break;
        case 1:
          TSS_SetKeypadConfig(controlID, index, (uint8_t)(data&0xff));
          break;
        case 2:
          TSS_SetSliderConfig(controlID, index, (uint8_t)(data&0xff));
          break;
        case 3:
          TSS_SetRotaryConfig(controlID, index, (uint8_t)(data&0xff));
          break;
        case 4:
          TSS_SetASliderConfig(controlID, index, (uint8_t)(data&0xff));
          break;
        case 5:
          TSS_SetARotaryConfig(controlID, index, (uint8_t)(data&0xff));
          break;
        case 6:
          TSS_SetMatrixConfig(controlID, index, (uint8_t)(data&0xff));
          break;
      }
      fmstr_write.flagreg = 0xff;
    }
    /* Dummy usage of variables to avoid remove from the memory */
    u16DummyVersion = tss_cu16Version;
    u16DummyVersion++;
    dummy_fmstr_config = fmstr_config;
    dummy_fmstr_config.low_bit_mask.low_bit++;
  }

  /*
   * User callback called from FMSTR_Isr() handler
   */
  void TSS_FmstrIsrCallback(void)
  {
    /* Set Sample Interrupted flag, because SCI measures at background and it can interrupt sampling of GPIO based methods */
    TSS_SET_SAMPLE_INTERRUPTED();
  }
#else
  void TSS_FmstrIsrCallback(void);

  void TSS_FmstrIsrCallback(void)
  {

  }
#endif /* End of TSS_USE_FREEMASTER_GUI */
