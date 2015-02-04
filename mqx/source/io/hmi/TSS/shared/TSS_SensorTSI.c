/***********************************************************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2006-2009 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
*********************************************************************************************************************//*!
*
* @file   TSS_SensorTSI.c
*
* @brief  Contains functions to Perform the sensing to the electrodes and set the status for each electrode
*
* @version 1.0.87.0
*
* @date Dec-21-2012
*
*
***********************************************************************************************************************/

#include "TSS_SensorTSI.h"


#if TSS_DETECT_METHOD(TSI)

  #if TSS_HCS08_MCU || TSS_CFV1_MCU

  #elif TSS_KINETIS_MCU || TSS_CFM_MCU

    /************************ Prototypes **************************/

    uint8_t TSI_MethodControl(uint8_t u8ElNum, uint8_t u8Command);

    /*******************  Modules definition **********************/

    #if TSS_DETECT_MODULE(TSI)
      #if TSS_KINETIS_MCU
        const TSS_TSI_METHOD_ROMDATA TSS_TSI_METHOD_ROMDATA_CONTEXT = {TSI_MethodControl, (uint32_t*) TSI_BASE_PTR, INT_TSI-16, TSS_DETECT_AUTOTRIGGER_SOURCE_USAGE(TSI), TSS_DETECT_LOWPOWER_USAGE(TSI)};
      #elif TSS_CFM_MCU
        const TSS_TSI_METHOD_ROMDATA TSS_TSI_METHOD_ROMDATA_CONTEXT = {TSI_MethodControl, (uint32_t*) TSI_BASE_PTR, Vtsi-16, TSS_DETECT_AUTOTRIGGER_SOURCE_USAGE(TSI), TSS_DETECT_LOWPOWER_USAGE(TSI)};
      #endif
      volatile TSS_TSI_METHOD_RAMDATA TSS_TSI_METHOD_RAMDATA_CONTEXT = {TSI_EOSF_STATE_NONE, 0u
                                                                       #if TSS_USE_LOWPOWER_CALIBRATION
                                                                         ,100u
                                                                       #endif
                                                                       };
    #endif
    #if TSS_DETECT_MODULE(TSI0)
      #if TSS_KINETIS_MCU
        const TSS_TSI_METHOD_ROMDATA TSS_TSI0_METHOD_ROMDATA_CONTEXT = {TSI_MethodControl, (uint32_t*) TSI0_BASE_PTR, INT_TSI0-16, TSS_DETECT_AUTOTRIGGER_SOURCE_USAGE(TSI0), TSS_DETECT_LOWPOWER_USAGE(TSI0)};
      #elif TSS_CFM_MCU
        const TSS_TSI_METHOD_ROMDATA TSS_TSI0_METHOD_ROMDATA_CONTEXT = {TSI_MethodControl, (uint32_t*) TSI0_BASE_PTR, Vtsi0-16, TSS_DETECT_AUTOTRIGGER_SOURCE_USAGE(TSI0), TSS_DETECT_LOWPOWER_USAGE(TSI0)};
      #endif
      volatile TSS_TSI_METHOD_RAMDATA TSS_TSI0_METHOD_RAMDATA_CONTEXT = {TSI_EOSF_STATE_NONE, 0u
                                                                        #if TSS_USE_LOWPOWER_CALIBRATION
                                                                          ,100u
                                                                        #endif
                                                                        };
    #endif
    #if TSS_DETECT_MODULE(TSI1)
      #if TSS_KINETIS_MCU
        const TSS_TSI_METHOD_ROMDATA TSS_TSI1_METHOD_ROMDATA_CONTEXT = {TSI_MethodControl, (uint32_t*) TSI1_BASE_PTR, INT_TSI1-16, TSS_DETECT_AUTOTRIGGER_SOURCE_USAGE(TSI1), TSS_DETECT_LOWPOWER_USAGE(TSI1)};
      #elif TSS_CFM_MCU
        const TSS_TSI_METHOD_ROMDATA TSS_TSI1_METHOD_ROMDATA_CONTEXT = {TSI_MethodControl, (uint32_t*) TSI1_BASE_PTR, Vtsi1-16, TSS_DETECT_AUTOTRIGGER_SOURCE_USAGE(TSI1), TSS_DETECT_LOWPOWER_USAGE(TSI1)};
      #endif
      volatile TSS_TSI_METHOD_RAMDATA TSS_TSI1_METHOD_RAMDATA_CONTEXT = {TSI_EOSF_STATE_NONE, 0u
                                                                        #if TSS_USE_LOWPOWER_CALIBRATION
                                                                          ,100u
                                                                        #endif
                                                                        };
    #endif

    #include "TSS_SensorTSI_def.h" /* ROMDATA and RAMDATA TSI definitions */

    /***************************************************************************//*!
    *                                 Types
    ****************************************************************************/

    /* TSI Recalibration Struct */

    typedef struct {
      uint8_t u8TSIPS;
      uint8_t u8TSIExtChrg;
      uint16_t u16Distance;
    } TSI_CALIBRATION;

    /************************** Variables ***************************/

    extern const uint8_t tss_cau8SignalDivider[];
    extern const uint8_t tss_cau8SignalMultiplier[];

    /***************************************************************************//*!
    *        Table for calculation of Internal Reference OSC Current
    ****************************************************************************/
    #if (TSS_TSI_VERSION == 1)
      const uint8_t u8RefChrgCalcTable[8][8] = {
        {10, 15, 20, 25, 30, 40, 50, 60},
        {12, 18, 24, 30, 36, 48, 60, 72},
        {14, 21, 28, 35, 42, 56, 70, 84},
        {16, 24, 32, 40, 48, 64, 80, 96},
        {18, 27, 36, 45, 54, 72, 90, 108},
        {20, 30, 40, 50, 60, 80, 100, 120},
        {22, 33, 44, 55, 66, 88, 110, 132},
        {24, 36, 48, 60, 72, 96, 120, 144}
      };
    #endif
    /************************** Prototypes ***************************/

    uint8_t TSI_InitModule(TSS_TSI_MemMapPtr psModuleStruct);
    uint8_t TSI_SetNSamples(TSS_TSI_MemMapPtr psModuleStruct);
    uint8_t TSI_InitLowPower(TSS_TSI_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSI_MemMapPtr psModuleStruct);
    uint8_t TSI_InitPEN(TSS_TSI_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSI_MemMapPtr psModuleStruct);
    uint8_t TSI_Recalibrate(TSS_TSI_MemMapPtr psModuleStruct, TSS_TSI_METHOD_RAMDATA *psMethodRAMDataStruct);
    uint8_t TSI_InitThresholds(TSS_TSI_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSI_MemMapPtr psModuleStruct, TSS_TSI_METHOD_RAMDATA *psMethodRAMDataStruct);
    uint8_t TSI_InitTrigger(TSS_TSI_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSI_MemMapPtr psModuleStruct, TSS_TSI_METHOD_RAMDATA *psMethodRAMDataStruct);
    uint8_t TSI_ReadCounter(TSS_TSI_ELECTRODE_ROMDATA *psElectrodeROMDataStruct, TSS_TSI_MemMapPtr psModuleStruct, TSS_TSI_METHOD_RAMDATA *psMethodRAMDataStruct);

    /***************************************************************************//*!
    *
    * @brief  Control function for TSI General Initialization
    *
    * @param  psModuleStruct - Specification of TSI module
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t TSI_InitModule(TSS_TSI_MemMapPtr psModuleStruct)
    {
      uint8_t u8result = TSS_INIT_STATUS_OK;

      /* GENCS Settings */
      psModuleStruct->GENCS = 0u; /* Erase GENCS - Turn OFF TSI*/
      /* Low Power TSI definition */
      #if (TSS_TSI_VERSION==1) || (TSS_TSI_VERSION==2)
        psModuleStruct->GENCS |= TSI_GENCS_LPCLKS(TSS_TSI_LPCLKS);  /* Set Low Power Mode Clock Source 0 = LPOCLK, 1 = VLPOSCCLK */
      #endif
      /* SCANC Settings */
      psModuleStruct->SCANC = 0u; /* Erase SCANC */
      /* Active Mode Clock Settings */
      psModuleStruct->SCANC |= TSI_SCANC_AMCLKS(TSS_TSI_AMCLKS);  /* Set Input Active Mode Clock Source - Set by the user 0 = BUSclk, 1 = MCGIRCLK, 2 = OSCERCLK, 3 - NA */
      psModuleStruct->SCANC |= TSI_SCANC_AMPSC(TSS_TSI_AMPSC);    /* Set Input Active Mode Clock Prescaler - Set by the user 1 = divide 1, 7 = divide 128 */
      #if (TSS_TSI_VERSION == 1)
        psModuleStruct->SCANC |= TSI_SCANC_AMCLKDIV(TSS_TSI_AMCLKDIV);    /* Set Input Active Mode Clock Divider - Set by the user 0 = divide 1, 1 = divide 2048 */
        /* Both OSC settings */
        psModuleStruct->SCANC |= TSI_SCANC_DELVOL(TSS_TSI_DELVOL);        /* Delta Voltage  settings for ElOSC and RefOSC 0 = 100mV, 7 = 600mV */
        /* TSI RefOSC definitions */
        psModuleStruct->SCANC |= TSI_SCANC_CAPTRM(TSS_TSI_SCANC_CAPTRM);  /* Internal Capacitance Trim value 0=0.5pF,7=1.2pF */
      #endif
      #if (TSS_TSI_VERSION == 2)
        psModuleStruct->SCANC |= TSI_SCANC_REFCHRG(TSS_TSI_SCANC_REFCHRG); /* Internal Ref OSC Charge Current 0= 1uA, 16 = 32uA */
      #elif (TSS_TSI_VERSION == 3)
        psModuleStruct->SCANC |= TSI_SCANC_REFCHRG(TSS_TSI_SCANC_REFCHRG); /* Internal Ref OSC Charge Current 0= 0.5uA, 7 = 64uA */
        psModuleStruct->SCANC |= TSI_SCANC_DVOLT(TSS_TSI_DVOLT);           /* Voltage Rails */
      #elif (TSS_TSI_VERSION == 1)
        psModuleStruct->SCANC |= TSI_SCANC_REFCHRG(TSS_CALC_TSI_REFCHRG_TABLE); /* Internal Ref OSC Charge Current 0= 1uA, 31 = 32uA */
      #endif

      return u8result;
    }

    /***************************************************************************//*!
    *
    * @brief  Control function for TSI Number of Samples Settings
    *
    * @param  psModuleStruct - Specification of TSI module
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t TSI_SetNSamples(TSS_TSI_MemMapPtr psModuleStruct)
    {
      uint8_t u8result = TSS_INIT_STATUS_OK;
      uint8_t u8NSamples;

      psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK;          /* Turn OFF TSI Module*/;
      psModuleStruct->GENCS &= ~TSI_GENCS_NSCN_MASK;           /* Erase NSCN */
      /* Number of Samples settings */
      u8NSamples = tss_CSSys.NSamples;
      psModuleStruct->GENCS |= TSI_GENCS_NSCN(u8NSamples - 1u);  /* Number of Consecutive Scans per Electrode 0 = 1 scan, 32 = 32 scans*/

      return u8result;
    }

    /***************************************************************************//*!
    *
    * @brief  Control function for TSI Low Power Settings
    *
    * @param  *psMethodROMDataStruct - Specificcation of ROMDATA Struct
    *         psModuleStruct - Specification of TSI module
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t TSI_InitLowPower(TSS_TSI_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSI_MemMapPtr psModuleStruct)
    {
      uint8_t u8result = TSS_INIT_STATUS_OK;

      psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK;  /* Turn OFF TSI Module*/;
      #if (TSS_TSI_VERSION == 1) || (TSS_TSI_VERSION == 2)
        psModuleStruct->GENCS &= ~TSI_GENCS_LPSCNITV_MASK;  /* Erase LPSCNITV */
      #endif
      if (psMethodROMDataStruct->tsi_u8LowPowerControlSource == 1u)  /* This TSI module is Low Power Control source */
      {
        /* Low Power TSI definition */
        #if (TSS_TSI_VERSION == 1) || (TSS_TSI_VERSION == 2)
          psModuleStruct->GENCS |= TSI_GENCS_LPSCNITV(TSS_TSI_LPSCNITV);  /* Setup TSI Low Power Mode Scan Interval 0 = 1ms - 15 = 500 ms */
        #endif
        if (tss_CSSys.SystemConfig.LowPowerEn)
        {
          psModuleStruct->GENCS |= TSI_GENCS_STPE_MASK;   /* TSI Stop Enable while in Low Power Mode (0 = Disable TSI when in LPM, 1 = Allow run of TSI in LPM) */
        }
        else
        {
          psModuleStruct->GENCS &= ~TSI_GENCS_STPE_MASK;  /* TSI Stop Enable while in Low Power Mode (0 = Disable TSI when in LPM, 1 = Allow run of TSI in LPM) */
        }
        u8result |= TSS_INIT_STATUS_LOWPOWER_SET;         /* For Indication of state if LowPower was set */
      }
      else
      {
        /* Low Power TSI definition */
        #if (TSS_TSI_VERSION == 1) || (TSS_TSI_VERSION == 2)
          psModuleStruct->GENCS |= TSI_GENCS_LPSCNITV(0u);  /* Setup TSI Low Power Mode Scan Interval 0 = 1ms - 15 = 500 ms */
        #endif
        psModuleStruct->GENCS &= ~TSI_GENCS_STPE_MASK;  /* TSI Stop Enable while in Low Power Mode (0 = Disable TSI when in LPM, 1 = Allow run of TSI in LPM) */
      }

      return u8result;
    }

    /***************************************************************************//*!
    *
    * @brief  Control function for TSI Pin Enable Settings
    *
    * @param  *psMethodROMDataStruct - Specificcation of ROMDATA Struct
    *         psModuleStruct - Specification of TSI module
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t TSI_InitPEN(TSS_TSI_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSI_MemMapPtr psModuleStruct)
    {
      uint8_t u8result = TSS_INIT_STATUS_OK;
      uint8_t u8ElCounter;
      uint16_t u16Temp;

      psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK; /* Turn OFF TSI Module if TSI was enabled */

      /* PEN Clearing */
      if (tss_CSSys.SystemConfig.ProximityEn || tss_CSSys.SystemConfig.LowPowerEn)
      {
        u16Temp = (1u << ((TSS_TSI_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[tss_CSSys.LowPowerElectrode]))->tsi_cu32ChannelNum);
        psModuleStruct->PEN &= (TSI_PEN_LPSP_MASK | u16Temp);
      }
      else
      {
        for(u8ElCounter = 0u; u8ElCounter < TSS_N_ELECTRODES; u8ElCounter++) /* Find all with the same module */
        {
          if ((TSS_GENERIC_METHOD_ROMDATA*) psMethodROMDataStruct == (TSS_GENERIC_METHOD_ROMDATA *)((TSS_GENERIC_ELECTRODE_ROMDATA *)tss_acp8ElectrodeROMData[u8ElCounter])->gen_cpsMethodROMData)
          {
            /* Disable Standard Electrode */
            if (!(tss_au8ElectrodeEnablers[u8ElCounter >> 3u] & (1u << (u8ElCounter & 0x07u))))
            {
              if ((psModuleStruct->PEN & (1u << ((TSS_TSI_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsi_cu32ChannelNum)) != 0u)
              {
                /* If the Electrode is enabled then disable Electrode */
                psModuleStruct->PEN &= ~(1u << ((TSS_TSI_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsi_cu32ChannelNum);
              }
            }
          }
        }
        /* PEN Enabling - The loop must be performed twice because one PEN can be assigned to more TSS electrodes */
        for(u8ElCounter = 0u; u8ElCounter < TSS_N_ELECTRODES; u8ElCounter++) /* Find all with the same module */
        {
          if ((TSS_GENERIC_METHOD_ROMDATA*) psMethodROMDataStruct == (TSS_GENERIC_METHOD_ROMDATA *)((TSS_GENERIC_ELECTRODE_ROMDATA *)tss_acp8ElectrodeROMData[u8ElCounter])->gen_cpsMethodROMData)
          {
            /* Enable Standard Electrode */
            if (tss_au8ElectrodeEnablers[u8ElCounter >> 3u] & (1u << (u8ElCounter & 0x07u)))
            {
              if ((psModuleStruct->PEN & (1u << ((TSS_TSI_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsi_cu32ChannelNum)) == 0u)
              {
                /* If the Electrode is disabled then Enable  Electrode */
                psModuleStruct->PEN |= (1u << ((TSS_TSI_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsi_cu32ChannelNum);
              }
            }
          }
        }
      }

      psModuleStruct->GENCS |= TSI_GENCS_TSIEN_MASK; /* Turn ON TSI Module if TSI was disabled */

      return u8result;
    }

    /***************************************************************************//*!
    *
    * @brief  Control function for TSI Recalibration
    *
    * @param  psModuleStruct - Specification of TSI module
    *         *psMethodRAMDataStruct - Specificcation of RAMDATA Struct
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    #if ((TSS_TSI_EXTCHRG_RANGE != 1) || (TSS_TSI_PS_RANGE != 1) || defined(TSS_ONPROXIMITY_CALLBACK))
      uint8_t TSI_Recalibrate(TSS_TSI_MemMapPtr psModuleStruct, TSS_TSI_METHOD_RAMDATA *psMethodRAMDataStruct)
      {
        TSI_CALIBRATION FinalTSICalibration;
        TSI_CALIBRATION FinalTSICalibrationBelow;
        TSI_CALIBRATION TempTSICalibration;
        TSI_CALIBRATION OldTSICalibration;

        uint8_t u8result = TSS_INIT_STATUS_OK;
        uint16_t u16ElCounter;
        uint8_t u8Counter;
        uint8_t u8TSIPSTemp;
        uint8_t u8TSIExtChrgLowTemp;
        uint8_t u8TSIExtChrgTemp;
        uint8_t u8CalibrationComplete;
        uint16_t u16CapSampleTemp;
        uint8_t u8Iteration;
        uint16_t u16ResolutionValue;
        uint8_t u8EXTCHRGRange;
        uint8_t u8PSHighLimit;
        uint16_t u16CapSampleAverage;
        uint8_t u8FinalTSICalibrationFound;
        uint8_t u8Found;

        /* Default Variables Init */
        FinalTSICalibrationBelow.u8TSIPS = 0u;
        FinalTSICalibrationBelow.u8TSIExtChrg = 0u;
        /* Save previous TSI Calibration */
        OldTSICalibration.u8TSIPS = (uint8_t) ((psModuleStruct->GENCS & TSI_GENCS_PS_MASK) >> TSI_GENCS_PS_SHIFT);
        OldTSICalibration.u8TSIExtChrg = (uint8_t) ((psModuleStruct->SCANC & TSI_SCANC_EXTCHRG_MASK) >> TSI_SCANC_EXTCHRG_SHIFT);
        /* Init TSI for Auto calibration */
        psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK; /* Turn OFF TSI Module */
        psModuleStruct->GENCS &= ~TSI_GENCS_TSIIE_MASK; /* Disable Interrupts */
        #if (TSS_TSI_VERSION == 1)
          /* Set Default Thresholds on all electrodes */
          for(u16ElCounter = 0u; u16ElCounter < 16u; u16ElCounter++) /* Find all with the same module */
          {
            /* Set Universal Threshold Values for all electrodes */
            psModuleStruct->THRESHOLD[u16ElCounter] = TSI_THRESHOLD_HTHH(TSS_TSI_DEFAULT_HIGH_THRESHOLD) | TSI_THRESHOLD_LTHH(TSS_TSI_DEFAULT_LOW_THRESHOLD);
          }
        #elif (TSS_TSI_VERSION == 2) || (TSS_TSI_VERSION == 3)
          /* Set Universal Threshold Values for all electrodes */
          psModuleStruct->THRESHOLD = TSI_THRESHOLD_HTHH(TSS_TSI_DEFAULT_HIGH_THRESHOLD ) | TSI_THRESHOLD_LTHH(TSS_TSI_DEFAULT_LOW_THRESHOLD );
        #endif

        /* Set Variables */
        if (tss_CSSys.SystemConfig.ProximityEn)
        {
          u16ResolutionValue = TSS_TSI_PROX_RESOLUTION_VALUE;
          u8EXTCHRGRange = TSS_TSI_PROX_EXTCHRG_RANGE;
          u8PSHighLimit = TSS_TSI_PROX_PS_HIGH_LIMIT;
          u8TSIPSTemp = TSS_TSI_PROX_PS_LOW_LIMIT;
          u8TSIExtChrgLowTemp = TSS_TSI_PROX_EXTCHRG_LOW_LIMIT;
        }
        else
        {
          u16ResolutionValue = TSS_TSI_RESOLUTION_VALUE;
          u8EXTCHRGRange = TSS_TSI_EXTCHRG_RANGE;
          u8PSHighLimit = TSS_TSI_PS_HIGH_LIMIT;
          u8TSIPSTemp = TSS_TSI_PS_LOW_LIMIT;
          u8TSIExtChrgLowTemp = TSS_TSI_EXTCHRG_LOW_LIMIT;
        }

        /* Do Autocalibration */
        FinalTSICalibration.u16Distance = 0xFFFFu;
        FinalTSICalibrationBelow.u16Distance = 0xFFFFu;
        u8FinalTSICalibrationFound = 0u;
        do
        {
          /* Set initial EXTHCRG */
          u8TSIExtChrgTemp = u8TSIExtChrgLowTemp + (u8EXTCHRGRange >> 1u);
          u8Iteration = 0u;
          TempTSICalibration.u16Distance = 0xFFFFu;
          u8Found = 0u;
          do
          {
            /* Increase iteration level */
            u8Iteration += 1u;

            /* Set TSI registers for Single measurement */
            psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK;      /* Disable TSI */
            psModuleStruct->GENCS &= ~TSI_GENCS_PS_MASK;         /* Erase PS */
            psModuleStruct->GENCS |= TSI_GENCS_PS(u8TSIPSTemp);  /* Set Prescaler for Electrode OSC - Set by the user 1 = divide 1, 7 = divide 128 */
            psModuleStruct->SCANC &= ~TSI_SCANC_EXTCHRG_MASK;    /* Erase ExtChrg */
            psModuleStruct->SCANC |= TSI_SCANC_EXTCHRG(u8TSIExtChrgTemp); /* External OSC Charge Current 0= 1uA, 31 = 32uA */
            /* Start Measurement */
            psModuleStruct->GENCS &= ~TSI_GENCS_STM_MASK;  /* Set Scan Trigger Mode (0 = SW Trigger, 1 = Periodical Scan) */
            psModuleStruct->GENCS |= TSI_GENCS_TSIEN_MASK;
            psModuleStruct->GENCS |= TSI_GENCS_SWTS_MASK;
            #ifdef _TSS_TEST
              _TSS_TSIGEN_ON;                                       /* Start generator for test purpose */
            #endif
            do
            {
            /* Measurement Running */
            } while (!(psModuleStruct->GENCS & TSI_GENCS_EOSF_MASK)); /* If TSI End of Scan Flag is reported*/
            #ifdef _TSS_TEST
              _TSS_TSIGEN_OFF;                                      /* Stop generator */
            #endif
            /* Wait for data ready */
            for(u8Counter = 0u; u8Counter < 20u; u8Counter++)       /* Delay */
            {
              #if defined(__ARMCC_VERSION)
                __nop();
              #else
                asm ("NOP");
              #endif
            }
            #if (TSS_TSI_VERSION == 1)
              /* Erase all Fault Flags for sure */
              psModuleStruct->STATUS = 0xFFFFFFFFu;
            #elif (TSS_TSI_VERSION == 2) || (TSS_TSI_VERSION == 3)
              psModuleStruct->GENCS |= TSI_GENCS_EXTERF_MASK;
            #endif
            /* Check if all electrodes has proper Counter value */
            u8CalibrationComplete = 1u;
            u16CapSampleAverage = 0u;
            for(u16ElCounter = 0u; u16ElCounter < 16u; u16ElCounter++) /* Go through all enabled pins */
            {
              /* Consider just enabled electrodes */
              if (psModuleStruct->PEN & (1u << u16ElCounter))
              {
                /* Read TSI Counter */
                #if TSS_KINETIS_MCU
                  u16CapSampleTemp = (psModuleStruct->tsi_u16Counter[u16ElCounter]);
                #elif TSS_CFM_MCU
                  u16CapSampleTemp = (psModuleStruct->tsi_u16Counter[(u16ElCounter & 0x01u) ? u16ElCounter - 1u : u16ElCounter + 1u]);  /* Big endian */
                #endif

                if (u16CapSampleTemp < u16ResolutionValue)
                {
                  u8CalibrationComplete = 0u; /* Capacitance value is small */
                }
                if (u16CapSampleAverage == 0u)
                {
                  u16CapSampleAverage = u16CapSampleTemp;
                }
                else
                {
                  u16CapSampleAverage = (u16CapSampleAverage >> 1u) + (u16CapSampleTemp >> 1u);
                }
              }
            }

            /* Evaluate Measurement - Reuse of u16ECounter */
            if(u16CapSampleAverage > u16ResolutionValue )
            {
              u16ElCounter = u16CapSampleAverage - u16ResolutionValue;
            }
            else
            {
              u16ElCounter = u16ResolutionValue - u16CapSampleAverage;
            }

            /* All samples > resolution AND (Avarage > resolution OR already found and tune it) */
            if ((u8CalibrationComplete == 1u ) && ((u16CapSampleAverage > u16ResolutionValue) || (u8Found && (TempTSICalibration.u16Distance >= u16ElCounter))))
            {
              TempTSICalibration.u8TSIExtChrg = u8TSIExtChrgTemp;
              TempTSICalibration.u8TSIPS = u8TSIPSTemp;
              TempTSICalibration.u16Distance = u16ElCounter;
              u8Found++; /* Match for set PS found */
            }
            else if (TempTSICalibration.u16Distance >= u16ElCounter && (u8Found ==0))
            {
              TempTSICalibration.u8TSIExtChrg = u8TSIExtChrgTemp;
              TempTSICalibration.u8TSIPS = u8TSIPSTemp;
              TempTSICalibration.u16Distance = u16ElCounter;
            }
            /* Change ExtCharge within the interval */
            if(u8CalibrationComplete == 1u)
            {
              u8TSIExtChrgTemp += ((u8EXTCHRGRange >> 1u) >> u8Iteration);
            }
            else
            {
              u8TSIExtChrgTemp -= ((u8EXTCHRGRange >> 1u) >> u8Iteration);
            }
          } while (((u8EXTCHRGRange >> 1u) >> u8Iteration) > 0u);
          /* The value only if distance is lower and was found with sufficient resolution */
          if ((FinalTSICalibration.u16Distance >= TempTSICalibration.u16Distance) && (u8Found > 0))
          {
            FinalTSICalibration.u8TSIPS = TempTSICalibration.u8TSIPS;
            FinalTSICalibration.u8TSIExtChrg = TempTSICalibration.u8TSIExtChrg;
            FinalTSICalibration.u16Distance = TempTSICalibration.u16Distance;
            u8FinalTSICalibrationFound++;
          }
          else if ((FinalTSICalibrationBelow.u16Distance >= TempTSICalibration.u16Distance) && (u8FinalTSICalibrationFound == 0))
          {
            /* Calibration not sufficient to the set resolution */
            FinalTSICalibrationBelow.u8TSIPS = TempTSICalibration.u8TSIPS;
            FinalTSICalibrationBelow.u8TSIExtChrg = TempTSICalibration.u8TSIExtChrg;
            FinalTSICalibrationBelow.u16Distance = TempTSICalibration.u16Distance;
          }

          u8TSIPSTemp++; /* Increment PS current */
        } while (u8TSIPSTemp < (u8PSHighLimit + 1u));

        /* Check if searching was succesful */
        if (u8FinalTSICalibrationFound)
        {
          psMethodRAMDataStruct->tsi_au16Resolution = u16ResolutionValue;
        }
        else
        {
          FinalTSICalibration.u8TSIPS = FinalTSICalibrationBelow.u8TSIPS;
          FinalTSICalibration.u8TSIExtChrg = FinalTSICalibrationBelow.u8TSIExtChrg;
          FinalTSICalibration.u16Distance = FinalTSICalibrationBelow.u16Distance;
          /* Reuse of u8Counter & u16ElCounter */
          u8Counter = 0u;
          u16ElCounter = u16ResolutionValue - FinalTSICalibrationBelow.u16Distance;
          /* Determine TSS Resolution (highest bit set) */
          while (u16ElCounter >> u8Counter)
          {
            u8Counter++;
          }
          /* Store the present resolution value */
          psMethodRAMDataStruct->tsi_au16Resolution = (1u << (u8Counter - 1u));
        }
        /* Set Final Found values */
        psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK;   /* Disable TSI */
        psModuleStruct->GENCS &= ~TSI_GENCS_PS_MASK;      /* Erase PS */
        psModuleStruct->GENCS |= TSI_GENCS_PS(FinalTSICalibration.u8TSIPS);       /* Set Prescaler for Electrode OSC - Set by the user 1 = divide 1, 7 = divide 128 */
        psModuleStruct->SCANC &= ~TSI_SCANC_EXTCHRG_MASK; /* Erase ExtChrg */
        psModuleStruct->SCANC |= TSI_SCANC_EXTCHRG(FinalTSICalibration.u8TSIExtChrg);  /* External OSC Charge Current 0= 1uA, 31 = 32uA */
        /* Save Value for Proximity function */
        #ifdef TSS_ONPROXIMITY_CALLBACK
          if (tss_CSSys.SystemConfig.ProximityEn)
          {
            tss_u16CapSample = FinalTSICalibration.u16Distance + u16ResolutionValue;
          }
        #endif
        /* Indication of changed configuration */
        if ((OldTSICalibration.u8TSIPS != FinalTSICalibration.u8TSIPS) || (OldTSICalibration.u8TSIExtChrg != FinalTSICalibration.u8TSIExtChrg))
        {
          /* Save result */
          u8result |= TSS_INIT_STATUS_CALIBRATION_CHANGED;
        }

        return u8result;
      }
    #else
      uint8_t TSI_Recalibrate(TSS_TSI_MemMapPtr psModuleStruct, TSS_TSI_METHOD_RAMDATA *psMethodRAMDataStruct)
      {
        uint8_t u8result = TSS_INIT_STATUS_OK;

        /* Set Final Found values */
        #if (TSS_TSI_VERSION == 1)
          /* Erase all Fault Flags for sure */
          psModuleStruct->STATUS = 0xFFFFFFFFu;
        #elif (TSS_TSI_VERSION == 2) || (TSS_TSI_VERSION == 3)
          psModuleStruct->GENCS |= TSI_GENCS_EXTERF_MASK;
        #endif
        psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK;   /* Disable TSI */
        psModuleStruct->GENCS &= ~TSI_GENCS_PS_MASK;      /* Erase PS */
        psModuleStruct->SCANC &= ~TSI_SCANC_EXTCHRG_MASK; /* Erase ExtChrg */
        /* Set configuration directly */
        psModuleStruct->GENCS |= TSI_GENCS_PS(TSS_TSI_PS_HIGH_LIMIT);  /* Set Prescaler for Electrode OSC - Set by the user 1 = divide 1, 7 = divide 128 */
        psModuleStruct->SCANC |= TSI_SCANC_EXTCHRG(TSS_TSI_EXTCHRG_HIGH_LIMIT);  /* External OSC Charge Current 0= 1uA, 31 = 32uA */

        psMethodRAMDataStruct->tsi_au16Resolution = TSS_TSI_RESOLUTION_VALUE;  /* Store the resolution value */

        return u8result;
      }
    #endif

    /***************************************************************************//*!
    *
    * @brief  Control function for TSI Thresholds Settings
    *
    * @param  *psMethodROMDataStruct - Specificcation of ROMDATA Struct
    *         psModuleStruct - Specification of TSI module
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t TSI_InitThresholds(TSS_TSI_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSI_MemMapPtr psModuleStruct, TSS_TSI_METHOD_RAMDATA *psMethodRAMDataStruct)
    {
      uint8_t u8result = TSS_INIT_STATUS_OK;
      uint8_t u8LowPowerEl;
      uint8_t u8Counter;
      uint16_t u16CapSampleTemp;
      uint8_t u8LowPowerElectrodeSensitivity;
      uint8_t u8ChannelNum;

      psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK; /* Turn OFF TSI Module */
      psModuleStruct->GENCS &= ~TSI_GENCS_TSIIE_MASK; /* Disable Interrupts */
      #if (TSS_TSI_VERSION == 1)
        /*** Set Default Low Power Thresholds for all electrodes within module***/
        for(u8Counter = 0u; u8Counter < 16u; u8Counter++) /* Find all within the same module */
        {
          /* Set Universal Threshold Values for all electrodes */
          psModuleStruct->THRESHOLD[u8Counter] = TSI_THRESHOLD_HTHH(TSS_TSI_DEFAULT_HIGH_THRESHOLD) | TSI_THRESHOLD_LTHH(TSS_TSI_DEFAULT_LOW_THRESHOLD);
        }
      #elif (TSS_TSI_VERSION == 2) || (TSS_TSI_VERSION == 3)
        /* Set Universal Threshold Values for all electrodes */
        psModuleStruct->THRESHOLD = TSI_THRESHOLD_HTHH(TSS_TSI_DEFAULT_HIGH_THRESHOLD ) | TSI_THRESHOLD_LTHH(TSS_TSI_DEFAULT_LOW_THRESHOLD );
      #endif
      /* If Module is selected for Low Power Control do following */
      if (psMethodROMDataStruct->tsi_u8LowPowerControlSource == 1u) /* This TSI module is Low Power Control source */
      {
        u8LowPowerEl = tss_CSSys.LowPowerElectrode;
        /* Check if Low Power Electrode is not out of range */
        if (u8LowPowerEl < TSS_N_ELECTRODES)/* This electrode is selected as source for Low Power Control */
        {
          /* Check if low power electrode is member of the module */
          if ((TSS_GENERIC_METHOD_ROMDATA *) psMethodROMDataStruct == (TSS_GENERIC_METHOD_ROMDATA *)((TSS_GENERIC_ELECTRODE_ROMDATA *)tss_acp8ElectrodeROMData[u8LowPowerEl])->gen_cpsMethodROMData)
          {
            /* Get Channel Num */
            u8ChannelNum = (uint8_t) (((TSS_TSI_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8LowPowerEl]))->tsi_cu32ChannelNum);
            /* Check if PEN is enabled */
            if (psModuleStruct->PEN & (1u << u8ChannelNum))
            {
              /******** Low Power Threshold Settings *********/
            #if TSS_USE_LOWPOWER_THRESHOLD_BASELINE
              /* Set Low Power Threshold  from Baseline value if defined */
              u16CapSampleTemp = TSS_GetSystemConfig(System_Baseline_Register + u8LowPowerEl);
              /* Do correction of the signal to original */
              #if TSS_USE_SIGNAL_DIVIDER
                u16CapSampleTemp *= tss_cau8SignalDivider[u8LowPowerEl];
              #endif
              #if TSS_USE_SIGNAL_MULTIPLIER
                u16CapSampleTemp /= tss_cau8SignalMultiplier[u8LowPowerEl];
              #endif
              /* if Baseline is not defined then measure new threshold value */
              if (u16CapSampleTemp == 0u)
            #endif
              {
                /* Init TSI registers for measurement */
                psModuleStruct->GENCS &= ~TSI_GENCS_STM_MASK;  /* Set Scan Trigger Mode (0 = SW Trigger, 1 = Periodical Scan) */
                psModuleStruct->GENCS |= TSI_GENCS_TSIEN_MASK;
                psModuleStruct->GENCS |= TSI_GENCS_SWTS_MASK;
                #ifdef _TSS_TEST
                  _TSS_TSIGEN_ON;  /* Start generator for test purpose */
                #endif
                do
                {
                /* Measurement Running */
                } while (!(psModuleStruct->GENCS & TSI_GENCS_EOSF_MASK)); /* If TSI End of Scan Flag is reported*/
                #ifdef _TSS_TEST
                  _TSS_TSIGEN_OFF;  /* Stop generator */
                #endif
                /* Wait for data ready */
                for(u8Counter = 0u; u8Counter < 20u; u8Counter++) /* Delay */
                {
                  #if defined(__ARMCC_VERSION)
                    __nop();
                  #else
                    asm ("NOP");
                  #endif
                }
                #if (TSS_TSI_VERSION == 1)
                  /* Erase all Fault Flags for sure */
                  psModuleStruct->STATUS = 0xFFFFFFFFu;
                #elif (TSS_TSI_VERSION == 2) || (TSS_TSI_VERSION == 3)
                  psModuleStruct->GENCS |= TSI_GENCS_EXTERF_MASK;
                #endif
                /* Read TSI Counter */
                #if TSS_KINETIS_MCU
                  u16CapSampleTemp = (psModuleStruct->tsi_u16Counter[u8ChannelNum]);
                #elif TSS_CFM_MCU
                  u16CapSampleTemp = (psModuleStruct->tsi_u16Counter[(u8ChannelNum & 0x01u) ? u8ChannelNum - 1u : u8ChannelNum + 1u]); /* Big endian */
                #endif
              }
              /* Calculation of Threshold values */
              u8LowPowerElectrodeSensitivity = tss_CSSys.LowPowerElectrodeSensitivity;
              #if TSS_USE_LOWPOWER_CALIBRATION
                /* Do not calculate threshold if Threshold Calibration is enabled, use only u16CapSampleTemp for wake up */
                if (psMethodRAMDataStruct->tsi_u8LowPowerThresholdRatio != 0u)
                {
                  /* Avoid to overflow by typecasting to uint32_t */
                  u16CapSampleTemp = (uint16_t)(((uint32_t)u16CapSampleTemp * (uint32_t)(psMethodRAMDataStruct->tsi_u8LowPowerThresholdRatio)) / 100u);
                  u16CapSampleTemp += u8LowPowerElectrodeSensitivity;
                }
              #else
                u16CapSampleTemp += u8LowPowerElectrodeSensitivity;
                (void) psMethodRAMDataStruct;
              #endif
              #if (TSS_TSI_VERSION == 1)
                /* Set Threshold for Low Power Wake up */
                psModuleStruct->THRESHOLD[u8ChannelNum] = TSI_THRESHOLD_HTHH(u16CapSampleTemp) | TSI_THRESHOLD_LTHH(TSS_TSI_DEFAULT_LOW_THRESHOLD);
              #elif (TSS_TSI_VERSION == 2) || (TSS_TSI_VERSION == 3)
                psModuleStruct->THRESHOLD = TSI_THRESHOLD_HTHH(u16CapSampleTemp) | TSI_THRESHOLD_LTHH(TSS_TSI_DEFAULT_LOW_THRESHOLD);
              #endif
              /*** Set Active Electrode for Low Power Mode ***/
              psModuleStruct->PEN &= ~TSI_PEN_LPSP_MASK;
              psModuleStruct->PEN |= TSI_PEN_LPSP(u8ChannelNum);
              u8result |= TSS_INIT_STATUS_LOWPOWER_ELEC_SET; /* For Indication of state if LowPower Electrode was found */
            }
          }
        }
      }

      return u8result;
    }

    /***************************************************************************//*!
    *
    * @brief  Control function for TSI Trigger Settings
    *
    * @param  *psMethodROMDataStruct - Specificcation of ROMDATA Struct
    *         psModuleStruct - Specification of TSI module
    *         *psMethodRAMDataStruct - Specificcation of RAMDATA Struct
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t TSI_InitTrigger(TSS_TSI_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSI_MemMapPtr psModuleStruct, TSS_TSI_METHOD_RAMDATA *psMethodRAMDataStruct)
    {
      uint8_t u8result = TSS_INIT_STATUS_OK;
      #if TSS_KINETIS_MCU
        NVIC_MemMapPtr psNVICStruct = NVIC_BASE_PTR;
      #endif

      psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK;  /* Turn OFF TSI Module*/;
      psModuleStruct->SCANC &= ~TSI_SCANC_SMOD_MASK;   /* Erase SMOD */
      #if (TSS_TSI_VERSION == 1)
        /* Erase all Fault Flags for sure */
        psModuleStruct->STATUS = 0xFFFFFFFFu;
      #elif (TSS_TSI_VERSION == 2) || (TSS_TSI_VERSION == 3)
        psModuleStruct->GENCS |= TSI_GENCS_EXTERF_MASK;
      #endif
      /* Trigger Setting */
      if ((psMethodROMDataStruct->tsi_u8TriggerSource == 1u) && (tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_AUTO))  /* This TSI module is Trigger source & Auto mode*/
      {
        /* This TSI module is Trigger source & Auto mode*/
        /* Active Mode Periodic Scan */
        psModuleStruct->SCANC |= TSI_SCANC_SMOD(TSS_TSI_SMOD);              /* Scan period (0 -255) */
        psModuleStruct->GENCS |= TSI_GENCS_STM_MASK;                        /* Set Scan Trigger Mode (0 = SW Trigger, 1 = Periodical Scan) */
        /* Indication that AutoTrigger was set correctly */
        u8result |= TSS_INIT_STATUS_AUTOTRIGGER_SET;
      }
      else
      { /* Other & ALWAYS & SW mode */
        /* Active Mode Periodic Scan */
        psModuleStruct->SCANC |= TSI_SCANC_SMOD(0u);   /* Active mode periodic scan period module 0 = Continue scan, Other = scan module */
        psModuleStruct->GENCS &= ~TSI_GENCS_STM_MASK;  /* Set Scan Trigger Mode (0 = SW Trigger, 1 = Periodical Scan) */
        u8result |= TSS_INIT_STATUS_TRIGGER_SET;
      }
      /* Enable EOSF interrupt */
      psModuleStruct->GENCS &= ~TSI_GENCS_ERIE_MASK;  /* Error (Short&Overrun) Interrupt Enable (0 = disabled, 1 = enabled) */
      psModuleStruct->GENCS |= TSI_GENCS_ESOR_MASK;   /* End-of-Scan|Out-of-Range Interrupt Selection (0 = Out-of-Range allow, 1 = End-of-Scan allow) */
      psModuleStruct->GENCS |= TSI_GENCS_TSIIE_MASK;  /* TSI Interrupt Enable (0 = disabled, 1 = enabled) */
      #if TSS_KINETIS_MCU
        /* Enable TSI Interrupt */
        psNVICStruct->ISER[psMethodROMDataStruct->tsi_u8ModuleIRQNum >> 5u] = (1u << (psMethodROMDataStruct->tsi_u8ModuleIRQNum & 0x1Fu));
      #endif
      psMethodRAMDataStruct->tsi_eEndOfScanState = TSI_EOSF_STATE_NONE; /* Reset EndOfScanState */

      /* Setup Low Power */
      if ((tss_CSSys.SystemConfig.LowPowerEn) && (psMethodROMDataStruct->tsi_u8LowPowerControlSource == 1u))
      {
        /* Set periodic measurement mode */
        psModuleStruct->GENCS |= TSI_GENCS_STM_MASK;    /* Set Scan Trigger Mode (0 = SW Trigger, 1 = Periodical Scan) */
        #if (TSS_TSI_VERSION == 3)
          psModuleStruct->SCANC &= ~TSI_SCANC_SMOD_MASK;
          psModuleStruct->SCANC |= TSI_SCANC_SMOD(TSS_TSI_SMOD);  /* SMOD != 0 is periodic scan, 0 is continuous mode */
        #endif
        /* Enable EOSF interrupt */
        psModuleStruct->GENCS &= ~TSI_GENCS_ERIE_MASK;  /* Error (Short&Overrun) Interrupt Enable (0 = disabled, 1 = enabled) */
        psModuleStruct->GENCS &= ~TSI_GENCS_ESOR_MASK;  /* Set Out of Range interrupt (0 = Out-of-Range allow, 1 = End-of-Scan allow) */
        psModuleStruct->GENCS |= TSI_GENCS_TSIIE_MASK;  /* TSI Interrupt Disable (0 = disabled, 1 = enabled) */
        #if TSS_KINETIS_MCU
          /* Enable TSI Interrupt */
          psNVICStruct->ISER[psMethodROMDataStruct->tsi_u8ModuleIRQNum >> 5u] = (1u << (psMethodROMDataStruct->tsi_u8ModuleIRQNum & 0x1Fu));
        #endif
        /* TSI enable and start to periodic measurement */
        psModuleStruct->GENCS |= TSI_GENCS_TSIEN_MASK;  /* TSI Enable (0 = disabled, 1 = enabled) */
        /* Indication that LowPower was set correctly */
        u8result |= TSS_INIT_STATUS_LOWPOWER_SET;       /* For Indication of state if LowPower was set */
        u8result &= ~TSS_INIT_STATUS_TRIGGER_SET;       /* Remove status that Trigger was set correctly */
        u8result &= ~TSS_INIT_STATUS_AUTOTRIGGER_SET;   /* Remove status that Auto Trigger was set correctly */
      }

      return u8result;
    }

    /***************************************************************************//*!
    *
    * @brief  Initializes the modules for the sensing of the electrodes
    *
    * @param  uint8_t u8ElNum - Processed Electrode Number
    *         uint8_t u8Command - Command intended to process
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t TSI_MethodControl(uint8_t u8ElNum, uint8_t u8Command)
    {
      uint8_t u8result = TSS_INIT_STATUS_OK;
      TSS_TSI_METHOD_ROMDATA *psMethodROMDataStruct;
      TSS_TSI_ELECTRODE_ROMDATA *psElectrodeROMDataStruct;
      TSS_TSI_MemMapPtr psModuleStruct;
      TSS_TSI_METHOD_RAMDATA *psMethodRAMDataStruct;

      /* Pointers Decoding */
      psElectrodeROMDataStruct = (TSS_TSI_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElNum]);
      psMethodROMDataStruct = (TSS_TSI_METHOD_ROMDATA *) (psElectrodeROMDataStruct->tsi_cpsu32MethodROMData);
      psModuleStruct = (TSS_TSI_MemMapPtr) (psMethodROMDataStruct->tsi_cpsu32Module);
      psMethodRAMDataStruct = (TSS_TSI_METHOD_RAMDATA *) (psElectrodeROMDataStruct->tsi_u8MethodRAMData);

      /************* Do TSI Init *******************/
      switch(u8Command)
      {
        case TSS_INIT_COMMAND_INIT_MODULES:
          u8result |= TSI_InitModule(psModuleStruct);
          u8result |= TSI_SetNSamples(psModuleStruct);
          u8result |= TSI_InitLowPower(psMethodROMDataStruct, psModuleStruct);
          u8result |= TSI_InitPEN(psMethodROMDataStruct, psModuleStruct);
          u8result |= TSI_Recalibrate(psModuleStruct, psMethodRAMDataStruct);
          u8result |= TSI_InitThresholds(psMethodROMDataStruct, psModuleStruct, psMethodRAMDataStruct);
          u8result |= TSI_InitTrigger(psMethodROMDataStruct, psModuleStruct, psMethodRAMDataStruct);
        break;
        /************* Set NSamples ***************/
        case TSS_INIT_COMMAND_SET_NSAMPLES:
          u8result |= TSI_SetNSamples(psModuleStruct);
          u8result |= TSI_Recalibrate(psModuleStruct, psMethodRAMDataStruct);
          u8result |= TSI_InitThresholds(psMethodROMDataStruct, psModuleStruct, psMethodRAMDataStruct);
          u8result |= TSI_InitTrigger(psMethodROMDataStruct, psModuleStruct, psMethodRAMDataStruct);
        break;
        /************ Low Power Init *************/
        case TSS_INIT_COMMAND_INIT_LOWPOWER:
          u8result |= TSI_InitLowPower(psMethodROMDataStruct, psModuleStruct);
          u8result |= TSI_InitThresholds(psMethodROMDataStruct, psModuleStruct, psMethodRAMDataStruct);
          u8result |= TSI_InitTrigger(psMethodROMDataStruct, psModuleStruct, psMethodRAMDataStruct);
        break;
        /************* Goto Low Power ************/
        case TSS_INIT_COMMAND_GOTO_LOWPOWER:
          u8result |= TSI_InitLowPower(psMethodROMDataStruct, psModuleStruct);
          #if TSS_USE_LOWPOWER_THRESHOLD_BASELINE
            u8result |= TSI_InitThresholds(psMethodROMDataStruct, psModuleStruct, psMethodRAMDataStruct);
          #endif
          u8result |= TSI_InitTrigger(psMethodROMDataStruct, psModuleStruct, psMethodRAMDataStruct);
        break;
        /*************** Do Auto calibration setting of TSI EXTCHRG and TSI PS **************/
        case TSS_INIT_COMMAND_RECALIBRATE:
          u8result |= TSI_Recalibrate(psModuleStruct, psMethodRAMDataStruct);
          u8result |= TSI_InitThresholds(psMethodROMDataStruct, psModuleStruct, psMethodRAMDataStruct);
          u8result |= TSI_InitTrigger(psMethodROMDataStruct, psModuleStruct, psMethodRAMDataStruct);
        break;
        /************ Electrode Enablers & Low Power Thresholds & Low Power Electrode Specification *************/
        case TSS_INIT_COMMAND_ENABLE_ELECTRODES:
          u8result |= TSI_InitPEN(psMethodROMDataStruct, psModuleStruct);
        break;
        /************* Triggering Init ************/
        case TSS_INIT_COMMAND_INIT_TRIGGER:
          u8result |= TSI_InitTrigger(psMethodROMDataStruct, psModuleStruct, psMethodRAMDataStruct);
        break;
      }
      /************* SWTrigger ************/
      if (u8Command == TSS_INIT_COMMAND_SW_TRIGGER)
      {
        psMethodRAMDataStruct->tsi_eEndOfScanState = TSI_EOSF_STATE_NONE;  /* Reset EndOfScanState */
        psModuleStruct->GENCS |= TSI_GENCS_SWTS_MASK;  /* Toggle SW Trigger */
        u8result |= TSS_INIT_STATUS_TRIGGER_SET;
        #ifdef _TSS_TEST
          _TSS_TSIGEN_ON;                                 /* Start generator for test purpose */
        #endif
      }
      /************* LowPower Calibration Enable ************/
      #if TSS_USE_LOWPOWER_CALIBRATION
        if (u8Command == TSS_INIT_COMMAND_ENABLE_LOWPOWER_CALIB)
        {
          /* Setup LowPower Calibration */
          psMethodRAMDataStruct->tsi_u8LowPowerThresholdRatio = 0u;
        }
      #endif

      /* Exit */

      return u8result;
    }

    /***************************************************************************//*!
    *
    * @brief  Reads TSI capacitance counter and returns a status code
    *
    * @param  *psElectrodeROMDataStruct - Specificcation of electrode ROMDATA Struct
    *         psModuleStruct - Specification of TSI module
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t TSI_ReadCounter(TSS_TSI_ELECTRODE_ROMDATA *psElectrodeROMDataStruct, TSS_TSI_MemMapPtr psModuleStruct, TSS_TSI_METHOD_RAMDATA *psMethodRAMDataStruct)
    {
      uint8_t u8result;

      #if (TSS_TSI_VERSION == 1)
        /* Read Status register */
        u8result = (uint8_t) ((psModuleStruct->STATUS & ((1u << psElectrodeROMDataStruct->tsi_cu32ChannelNum) << 16u)) >> (psElectrodeROMDataStruct->tsi_cu32ChannelNum + 16u));
      #elif (TSS_TSI_VERSION == 2) || (TSS_TSI_VERSION == 3)
        u8result = (uint8_t) (psModuleStruct->GENCS & TSI_GENCS_EXTERF_MASK);
      #endif
      /* Evaluate Status */
      if (u8result != 0u)
      {
        #if (TSS_TSI_VERSION == 1)
          psModuleStruct->STATUS |= ((1u << psElectrodeROMDataStruct->tsi_cu32ChannelNum) << 16u); /* Erase Fault Flag */
        #elif (TSS_TSI_VERSION == 2) || (TSS_TSI_VERSION == 3)
          psModuleStruct->GENCS |= TSI_GENCS_EXTERF_MASK;
        #endif
        u8result = TSS_SAMPLE_ERROR_CHARGE_TIMEOUT;
      }
      else
      {
        /* Read TSI Counter */
        #if TSS_KINETIS_MCU
          tss_u16CapSample = (psModuleStruct->tsi_u16Counter[psElectrodeROMDataStruct->tsi_cu32ChannelNum]); /* Read TSI Counter */
        #elif TSS_CFM_MCU
          tss_u16CapSample = (psModuleStruct->tsi_u16Counter[(psElectrodeROMDataStruct->tsi_cu32ChannelNum & 0x01u) ? psElectrodeROMDataStruct->tsi_cu32ChannelNum - 1u : psElectrodeROMDataStruct->tsi_cu32ChannelNum + 1u]); /* Read TSI Counter, Big endian*/
        #endif
        /* Evaluation of the Measured Value */
        if (tss_u16CapSample > TSS_TSI_CAP_HIGH_LIMIT(psMethodRAMDataStruct->tsi_au16Resolution))
        {
          u8result = TSS_SAMPLE_ERROR_CHARGE_TIMEOUT;  /* Too High Capacity is detected */
        }
        else if (tss_u16CapSample > TSS_TSI_RECALIBRATION_HIGH_THRESHOLD(psMethodRAMDataStruct->tsi_au16Resolution))
        {
          u8result = TSS_SAMPLE_RECALIB_REQUEST_HICAP;  /* Re-calibration is needed */
        }
        else if (tss_u16CapSample < TSS_TSI_CAP_LOW_LIMIT(psMethodRAMDataStruct->tsi_au16Resolution))
        {
          u8result = TSS_SAMPLE_ERROR_SMALL_CAP;        /* Too Low Capacity is detected */
        }
        else if (tss_u16CapSample < TSS_TSI_RECALIBRATION_LOW_THRESHOLD(psMethodRAMDataStruct->tsi_au16Resolution))
        {
          u8result = TSS_SAMPLE_RECALIB_REQUEST_LOCAP;  /* Re-calibration is needed */
        }
        else
        {
          u8result = TSS_SAMPLE_STATUS_OK;
        }
      }

      return u8result;
    }

    /***************************************************************************//*!
    *
    * @brief  Performs a valid reading of one electrode stores the timer values
    *         and returns a status code
    *
    * @param  u8ElecNum Electrode number to be scanned
    *         u8Command Requested command
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t TSI_SampleElectrode(uint8_t u8ElecNum, uint8_t u8Command)
    {
      uint8_t u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
      uint8_t u8ElCounter;

      TSS_TSI_METHOD_ROMDATA *psMethodROMDataStruct;
      TSS_TSI_ELECTRODE_ROMDATA *psElectrodeROMDataStruct;
      TSS_TSI_METHOD_RAMDATA *psMethodRAMDataStruct;
      TSS_TSI_MemMapPtr psModuleStruct;

      /* Pointers decoding */
      psElectrodeROMDataStruct = (TSS_TSI_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElecNum]);
      psMethodROMDataStruct = (TSS_TSI_METHOD_ROMDATA *) (psElectrodeROMDataStruct->tsi_cpsu32MethodROMData);
      psModuleStruct = (TSS_TSI_MemMapPtr) (psMethodROMDataStruct->tsi_cpsu32Module);
      psMethodRAMDataStruct = (TSS_TSI_METHOD_RAMDATA *) (psElectrodeROMDataStruct->tsi_u8MethodRAMData);

      /* Module Control */
      switch (u8Command)
      {
        case TSS_SAMPLE_COMMAND_RESTART:
          /* Do TSI init */
          if (!(psModuleStruct->GENCS & TSI_GENCS_TSIEN_MASK)) /* If TSI is disabled then enable */
          {
            psModuleStruct->GENCS |= TSI_GENCS_TSIEN_MASK;
          }
          /* Evaluate status */
          if ( tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_AUTO)
          {
            if (psMethodRAMDataStruct->tsi_eEndOfScanState == TSI_EOSF_STATE_ERROR)
            {
              u8ElectrodeStatus = TSS_SAMPLE_ERROR_SMALL_TRIGGER_PERIOD; /* Small TriggerModuloValue */
              psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK;           /* Disable whole TSS due triggering crash */
              #ifdef _TSS_TEST
                 _TSS_TSIGEN_OFF;           /* Stop generator for test purpose */
              #endif
            }
            else
            {
              u8ElectrodeStatus = TSS_SAMPLE_STATUS_PROCESSING;
            }
            psMethodRAMDataStruct->tsi_eEndOfScanState = TSI_EOSF_STATE_NONE; /* Reset EndOfScanState */
          }
          else
          { /* If Always & SW Mode */
            /* If Always Mode restart TSI measurement by SW Trigger Toggle */
            if ( tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_ALWAYS)
            {
              /* TSI scanning is in progress or one TSI measurement done */
              if (( psModuleStruct->GENCS & TSI_GENCS_SCNIP_MASK ) || (psMethodRAMDataStruct->tsi_eEndOfScanState == TSI_EOSF_STATE_REGULAR))
              {
                /* Nothing */
              }
              else
              {
                psMethodRAMDataStruct->tsi_eEndOfScanState = TSI_EOSF_STATE_NONE; /* Reset EndOfScanState */
                psModuleStruct->GENCS |= TSI_GENCS_SWTS_MASK;  /* Toggle SW Trigger */
                #ifdef _TSS_TEST
                  _TSS_TSIGEN_ON;          /* Start generator for test purpose */
                #endif
              }
            }
            u8ElectrodeStatus = TSS_SAMPLE_STATUS_PROCESSING;
          }
        break;
        case TSS_SAMPLE_COMMAND_PROCESS:
          /* If TSI is disabled then enable */
          if (!(psModuleStruct->GENCS & TSI_GENCS_TSIEN_MASK))
          {
            psModuleStruct->GENCS |= TSI_GENCS_TSIEN_MASK;
          }
          /* Evaluate status */
          if ( tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_AUTO)
          {
            #ifdef _TSS_TEST
              _TSS_TSIGEN_ON;              /* Start generator for test purpose */
            #endif
            if ((psMethodRAMDataStruct->tsi_eEndOfScanState == TSI_EOSF_STATE_NONE) || ((psMethodRAMDataStruct->tsi_eEndOfScanState == TSI_EOSF_STATE_REGULAR) && (!(psModuleStruct->GENCS & TSI_GENCS_SCNIP_MASK))))
            {
              u8ElectrodeStatus = TSS_SAMPLE_STATUS_PROCESSING;
            }
            else if ( (psMethodRAMDataStruct->tsi_eEndOfScanState == TSI_EOSF_STATE_EXCESS) || ((psMethodRAMDataStruct->tsi_eEndOfScanState == TSI_EOSF_STATE_REGULAR) && (psModuleStruct->GENCS & TSI_GENCS_SCNIP_MASK)))
            {
              /* Read TSI Counter */
              u8ElectrodeStatus = TSI_ReadCounter(psElectrodeROMDataStruct, psModuleStruct, psMethodRAMDataStruct);
            }
            else
            {
              u8ElectrodeStatus = TSS_SAMPLE_ERROR_SMALL_TRIGGER_PERIOD; /* Small TriggerModuloValue */
              psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK;            /* Disable whole TSS due triggering crash */
              psMethodRAMDataStruct->tsi_eEndOfScanState = TSI_EOSF_STATE_NONE; /* Reset EndOfScanState */
              #ifdef _TSS_TEST
                _TSS_TSIGEN_OFF;           /* Stop generator for test purpose */
              #endif
            }
          }
          else
          {
            /* Always & SW Mode */
            if (psModuleStruct->GENCS & TSI_GENCS_SCNIP_MASK) /* TSI scanning is in progress */
            {
              u8ElectrodeStatus = TSS_SAMPLE_STATUS_PROCESSING;
            }
            else
            {
              /* If TSI End of Scan Flag is reported */
              if (psMethodRAMDataStruct->tsi_eEndOfScanState >= TSI_EOSF_STATE_REGULAR)
              {
                psMethodRAMDataStruct->tsi_eEndOfScanState = TSI_EOSF_STATE_PROCESS;
                #ifdef _TSS_TEST
                  _TSS_TSIGEN_OFF;  /* Stop generator */
                #endif
                /* Read TSI Counter */
                u8ElectrodeStatus = TSI_ReadCounter(psElectrodeROMDataStruct, psModuleStruct, psMethodRAMDataStruct);
              }
              else
              {
                /* Toggle TSI SW Trigger again if process was interrupted for sure */
                if (tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_ALWAYS)
                {
                  psMethodRAMDataStruct->tsi_eEndOfScanState = TSI_EOSF_STATE_NONE; /* Reset EndOfScanState */
                  psModuleStruct->GENCS |= TSI_GENCS_SWTS_MASK;
                }
                #ifdef _TSS_TEST
                  _TSS_TSIGEN_ON;  /* Start generator for test purpose */
                #endif
                u8ElectrodeStatus = TSS_SAMPLE_STATUS_PROCESSING;
              }
            }
          }
        break;
        case TSS_SAMPLE_COMMAND_GET_NEXT_ELECTRODE:
          /* Find the next electrode within the electrode module */
          u8ElCounter = u8ElecNum;
          do
          {
            if (u8ElCounter < (TSS_N_ELECTRODES - 1u))
            {
              u8ElCounter++;
            }
            else
            {
              u8ElCounter = 0u;
            }
          } while ((TSS_GENERIC_METHOD_ROMDATA*) psMethodROMDataStruct != (TSS_GENERIC_METHOD_ROMDATA *)((TSS_GENERIC_ELECTRODE_ROMDATA *)tss_acp8ElectrodeROMData[u8ElCounter])->gen_cpsMethodROMData);
          /* Return found electrode number */
          u8ElectrodeStatus = u8ElCounter;
        break;
        case TSS_SAMPLE_COMMAND_RECALIB:
          u8ElectrodeStatus = TSI_MethodControl(u8ElecNum, TSS_INIT_COMMAND_RECALIBRATE);

          if (u8ElectrodeStatus & TSS_INIT_ERROR_RECALIB_FAULT)
          {
            /* Fault happened, so disable actual electrode */
            u8ElectrodeStatus = TSS_SAMPLE_ERROR_CHARGE_TIMEOUT;
          }
          else if (u8ElectrodeStatus & TSS_INIT_STATUS_CALIBRATION_CHANGED)
          {
            /* Calibration changed */
            u8ElectrodeStatus = TSS_SAMPLE_STATUS_CALIBRATION_CHANGED;
          }
          else
          {
            /* Old calibration was correct */
            u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
          }
        break;
        case TSS_SAMPLE_COMMAND_ENABLE_ELECTRODE:
          u8ElectrodeStatus = TSI_MethodControl(u8ElecNum, TSS_INIT_COMMAND_ENABLE_ELECTRODES);

          u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
        break;
        case TSS_SAMPLE_COMMAND_SET_LOWLEVEL_CONFIG:
          psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK;   /* Disable TSI */
          psModuleStruct->GENCS &= ~TSI_GENCS_PS_MASK;      /* Erase PS */
          psModuleStruct->SCANC &= ~TSI_SCANC_EXTCHRG_MASK; /* Erase ExtChrg */
          /* Set configuration directly */
          psModuleStruct->GENCS |= TSI_GENCS_PS(tss_u16CapSample & 0x7u);  /* Set Prescaler for Electrode OSC - Set by the user 1 = divide 1, 7 = divide 128 */
          psModuleStruct->SCANC |= TSI_SCANC_EXTCHRG((tss_u16CapSample >> 3u) & 0x1Fu);  /* External OSC Charge Current 0= 1uA, 31 = 32uA */

          u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
        break;
        case TSS_SAMPLE_COMMAND_GET_LOWLEVEL_CONFIG:
          u8ElectrodeStatus = (uint8_t) ((psModuleStruct->GENCS & TSI_GENCS_PS_MASK) >> TSI_GENCS_PS_SHIFT);
          tss_u16CapSample = (uint16_t) ((((psModuleStruct->SCANC & TSI_SCANC_EXTCHRG_MASK) >> TSI_SCANC_EXTCHRG_SHIFT) << 3u) | (uint16_t) u8ElectrodeStatus);

          u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
        break;
      }

      return u8ElectrodeStatus;   /* Return status code */

    }

    /***************************************************************************//*!
    *
    * @brief  TSI ISR routines
    *
    * @remarks
    *
    ****************************************************************************/
    #if TSS_DETECT_MODULE(TSI)
      #if TSS_KINETIS_MCU
        #if TSS_ENABLE_DIAGNOSTIC_MESSAGES
          #warning TSS - Please assign ISR function TSS_TSIIsr to vector INT_TSI
        #endif
      #elif TSS_CFM_MCU
        #if TSS_ENABLE_DIAGNOSTIC_MESSAGES
          #warning TSS - Please assign ISR function TSS_TSIIsr to vector Vtsi
        #endif
        #if !TSS_USE_MQX
          interrupt
        #endif
      #endif
      void TSS_TSIIsr(
        #if TSS_USE_MQX
          void *_isrParameter
        #else
          void
        #endif
        )
      {
        uint16_t u16temp;

        ((TSS_TSI_MemMapPtr) (TSS_TSI_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->GENCS |= TSI_GENCS_TSIEN_MASK; /* Due to 0M33Z maskset (unfreeze after restart) */
        if (tss_CSSys.SystemConfig.LowPowerEn)
        {
          #if (TSS_TSI_VERSION==1)
            ((TSS_TSI_MemMapPtr) (TSS_TSI_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->STATUS = 0xFFFFFFFFu;  /* Clear OUTFRG Status Flag */
          #endif
          ((TSS_TSI_MemMapPtr) (TSS_TSI_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->GENCS |= TSI_GENCS_OUTRGF_MASK; /* Clear OUTFRG Flag */
          ((TSS_TSI_MemMapPtr) (TSS_TSI_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->GENCS &= ~TSI_GENCS_TSIIE_MASK; /* Disable TSI Interrupt */
          #if TSS_USE_LOWPOWER_CALIBRATION
            /* Do not calculate threshold if Threshold Calibration is enabled, use only u16CapSampleTemp for wake up */
            if (TSS_TSI_METHOD_RAMDATA_CONTEXT.tsi_u8LowPowerThresholdRatio == 0u)
            {
              u16temp = (((TSS_TSI_MemMapPtr) (TSS_TSI_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->WUCNTR) & TSI_WUCNTR_WUCNT_MASK;
              TSS_TSI_METHOD_RAMDATA_CONTEXT.tsi_u8LowPowerThresholdRatio = (uint8_t) ( ((uint32_t)u16temp * 100u) /  ((((TSS_TSI_MemMapPtr) (TSS_TSI_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->THRESHOLD & TSI_THRESHOLD_HTHH_MASK) >> TSI_THRESHOLD_HTHH_SHIFT));
              /* Set New Threshold */
              ((TSS_TSI_MemMapPtr) (TSS_TSI_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->THRESHOLD = TSI_THRESHOLD_HTHH(u16temp + tss_CSSys.LowPowerElectrodeSensitivity) | TSI_THRESHOLD_LTHH(TSS_TSI_DEFAULT_LOW_THRESHOLD);
            }
          #endif
          /* Disables LowPower Wake Up */
          u16temp = TSS_GetSystemConfig(System_SystemConfig_Register);
          (void) TSS_SetSystemConfig(System_SystemConfig_Register, (u16temp & ~TSS_LOWPOWER_EN_MASK));
        }
        else
        {
          ((TSS_TSI_MemMapPtr) (TSS_TSI_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->GENCS |= TSI_GENCS_EOSF_MASK;  /* Clear Flag */
          #ifdef _TSS_TEST
            _TSS_TSIGEN_OFF;  /* Stop generator */
          #endif
          if (TSS_TSI_METHOD_RAMDATA_CONTEXT.tsi_eEndOfScanState != TSI_EOSF_STATE_ERROR)
          {
            /* Increment tsi_u8EndOfScanState */
            TSS_TSI_METHOD_RAMDATA_CONTEXT.tsi_eEndOfScanState = (TSS_TSI_EOSF_STATES) ((uint8_t)TSS_TSI_METHOD_RAMDATA_CONTEXT.tsi_eEndOfScanState + 1u);
          }
        }
        /* Set Sample Interrupted flag, because TSI measures at background and it can interrupt sampling of GPIO based methods */
        TSS_SET_SAMPLE_INTERRUPTED();
      }
    #endif
    #if TSS_DETECT_MODULE(TSI0)
      #if TSS_KINETIS_MCU
        #if TSS_ENABLE_DIAGNOSTIC_MESSAGES
          #warning TSS - Please assign ISR function TSS_TSI0Isr to vector INT_TSI0
        #endif
      #elif TSS_CFM_MCU
        #if TSS_ENABLE_DIAGNOSTIC_MESSAGES
          #warning TSS - Please assign ISR function TSS_TSI0Isr to vector Vtsi0
        #endif
        #if !TSS_USE_MQX
          interrupt
        #endif
      #endif
      void TSS_TSI0Isr(
        #if TSS_USE_MQX
          void *_isrParameter
        #else
          void
        #endif
        )
      {
        uint16_t u16temp;

        ((TSS_TSI_MemMapPtr) (TSS_TSI0_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->GENCS |= TSI_GENCS_TSIEN_MASK; /* Due to 0M33Z maskset (unfreeze after restart) */
        if (tss_CSSys.SystemConfig.LowPowerEn)
        {
          #if (TSS_TSI_VERSION==1)
            ((TSS_TSI_MemMapPtr) (TSS_TSI0_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->STATUS = 0xFFFFFFFFu;  /* Clear OUTFRG Status Flag */
          #endif
          ((TSS_TSI_MemMapPtr) (TSS_TSI0_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->GENCS |= TSI_GENCS_OUTRGF_MASK; /* Clear OUTFRG Flag */
          ((TSS_TSI_MemMapPtr) (TSS_TSI0_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->GENCS &= ~TSI_GENCS_TSIIE_MASK; /* Disable TSI Interrupt */
          #if TSS_USE_LOWPOWER_CALIBRATION
            /* Do not calculate threshold if Threshold Calibration is enabled, use only u16CapSampleTemp for wake up */
            if (TSS_TSI0_METHOD_RAMDATA_CONTEXT.tsi_u8LowPowerThresholdRatio == 0u)
            {
              u16temp = (((TSS_TSI_MemMapPtr) (TSS_TSI0_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->WUCNTR) & TSI_WUCNTR_WUCNT_MASK;
              TSS_TSI0_METHOD_RAMDATA_CONTEXT.tsi_u8LowPowerThresholdRatio = (uint8_t) ( ((uint32_t)u16temp * 100u) /  ((((TSS_TSI_MemMapPtr) (TSS_TSI0_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->THRESHOLD & TSI_THRESHOLD_HTHH_MASK) >> TSI_THRESHOLD_HTHH_SHIFT));
              /* Set New Threshold */
              ((TSS_TSI_MemMapPtr) (TSS_TSI0_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->THRESHOLD = TSI_THRESHOLD_HTHH(u16temp + tss_CSSys.LowPowerElectrodeSensitivity) | TSI_THRESHOLD_LTHH(TSS_TSI_DEFAULT_LOW_THRESHOLD);
            }
          #endif
          /* Disables LowPower Wake Up */
          u16temp = TSS_GetSystemConfig(System_SystemConfig_Register);
          (void)TSS_SetSystemConfig(System_SystemConfig_Register,(u16temp & ~TSS_LOWPOWER_EN_MASK));
        }
        else
        {
          ((TSS_TSI_MemMapPtr) (TSS_TSI0_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->GENCS |= TSI_GENCS_EOSF_MASK;  /* Clear Flag */
          #ifdef _TSS_TEST
            _TSS_TSIGEN_OFF;         /* Stop generator */
          #endif
          if (TSS_TSI0_METHOD_RAMDATA_CONTEXT.tsi_eEndOfScanState != TSI_EOSF_STATE_ERROR)
          {
            /* Increment tsi_u8EndOfScanState */
            TSS_TSI0_METHOD_RAMDATA_CONTEXT.tsi_eEndOfScanState = (TSS_TSI_EOSF_STATES) ((uint8_t)TSS_TSI0_METHOD_RAMDATA_CONTEXT.tsi_eEndOfScanState + 1u);
          }
        }
        /* Set Sample Interrupted flag, because TSI measures at background and it can interrupt sampling of GPIO based methods */
        TSS_SET_SAMPLE_INTERRUPTED();
      }
    #endif
    #if TSS_DETECT_MODULE(TSI1)
      #if TSS_KINETIS_MCU
        #if TSS_ENABLE_DIAGNOSTIC_MESSAGES
          #warning TSS - Please assign ISR function TSS_TSI1Isr to vector INT_TSI1
        #endif
      #elif TSS_CFM_MCU
        #if TSS_ENABLE_DIAGNOSTIC_MESSAGES
          #warning TSS - Please assign ISR function TSS_TSI1Isr to vector Vtsi1
        #endif
        #if !TSS_USE_MQX
          interrupt
        #endif
      #endif
      void TSS_TSI1Isr(
        #if TSS_USE_MQX
          void *_isrParameter
        #else
          void
        #endif
        )
      {
        uint16_t u16temp;

        ((TSS_TSI_MemMapPtr) (TSS_TSI1_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->GENCS |= TSI_GENCS_TSIEN_MASK; /* Due to 0M33Z maskset (unfreeze after restart) */
        if (tss_CSSys.SystemConfig.LowPowerEn)
        {
          #if (TSS_TSI_VERSION==1)
            ((TSS_TSI_MemMapPtr) (TSS_TSI1_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->STATUS = 0xFFFFFFFFu;  /* Clear OUTFRG Status Flag */
          #endif
          ((TSS_TSI_MemMapPtr) (TSS_TSI1_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->GENCS |= TSI_GENCS_OUTRGF_MASK; /* Clear OUTFRG Flag */
          ((TSS_TSI_MemMapPtr) (TSS_TSI1_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->GENCS &= ~TSI_GENCS_TSIIE_MASK; /* Disable TSI Interrupt */
          #if TSS_USE_LOWPOWER_CALIBRATION
            /* Do not calculate threshold if Threshold Calibration is enabled, use only u16CapSampleTemp for wake up */
            if (TSS_TSI1_METHOD_RAMDATA_CONTEXT.tsi_u8LowPowerThresholdRatio == 0u)
            {
              u16temp = (((TSS_TSI_MemMapPtr) (TSS_TSI1_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->WUCNTR) & TSI_WUCNTR_WUCNT_MASK;
              TSS_TSI1_METHOD_RAMDATA_CONTEXT.tsi_u8LowPowerThresholdRatio = (uint8_t) ( ((uint32_t)u16temp * 100u) /  ((((TSS_TSI_MemMapPtr) (TSS_TSI1_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->THRESHOLD & TSI_THRESHOLD_HTHH_MASK) >> TSI_THRESHOLD_HTHH_SHIFT));
              /* Set New Threshold */
              ((TSS_TSI_MemMapPtr) (TSS_TSI1_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->THRESHOLD = TSI_THRESHOLD_HTHH(u16temp + tss_CSSys.LowPowerElectrodeSensitivity) | TSI_THRESHOLD_LTHH(TSS_TSI_DEFAULT_LOW_THRESHOLD);
            }
          #endif
          /* Disables LowPower Wake Up */
          u16temp = TSS_GetSystemConfig(System_SystemConfig_Register);
          (void)TSS_SetSystemConfig(System_SystemConfig_Register,(u16temp & ~TSS_LOWPOWER_EN_MASK));
        }
        else
        {
          ((TSS_TSI_MemMapPtr) (TSS_TSI1_METHOD_ROMDATA_CONTEXT.tsi_cpsu32Module))->GENCS |= TSI_GENCS_EOSF_MASK;  /* Clear Flag */
          #ifdef _TSS_TEST
            _TSS_TSIGEN_OFF;         /* Stop generator */
          #endif
          if (TSS_TSI1_METHOD_RAMDATA_CONTEXT.tsi_eEndOfScanState != TSI_EOSF_STATE_ERROR)
          {
            /* Increment tsi_u8EndOfScanState */
            TSS_TSI1_METHOD_RAMDATA_CONTEXT.tsi_eEndOfScanState = (TSS_TSI_EOSF_STATES) ((uint8_t)TSS_TSI1_METHOD_RAMDATA_CONTEXT.tsi_eEndOfScanState + 1u);
          }
        }
        /* Set Sample Interrupted flag, because TSI measures at background and it can interrupt sampling of GPIO based methods */
        TSS_SET_SAMPLE_INTERRUPTED();
      }
    #endif

  #else /* End of TSS_KINETIS_MCU || TSS_CFM_MCU */
    #error "TSS - Not supported MCU used"
  #endif

#endif /* End of TSS_DETECT_METHOD(TSI) */
