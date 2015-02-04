/***********************************************************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2006-2009 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
*********************************************************************************************************************//*!
*
* @file   TSS_SensorTSIL.c
*
* @brief  Contains functions to Perform the sensing to the electrodes and set the status for each electrode
*
* @version 1.0.34.0
*
* @date Dec-21-2012
*
*
***********************************************************************************************************************/

#include "TSS_SensorTSIL.h"

#if TSS_DETECT_METHOD(TSIL)

  #if TSS_HCS08_MCU || TSS_CFV1_MCU

    #if TSS_USE_SIMPLE_LOW_LEVEL

      #include "TSS_SensorTSIL_def.h" /* ROMDATA and RAMDATA TSIL definitions */

      /************************************************************************
      *                                 Types
      *************************************************************************/

      /* TSIL Recalibration Struct */

      typedef struct {
        uint8_t u8TSIPS;
        uint8_t u8TSIExtChrg;
        uint16_t u16Distance;
      } TSIL_CALIBRATION;

      static uint16_t tsi_au16Resolution; /* TSIL resolution */

      /**************************** Prototypes *******************************/
      uint8_t TSIL_InitModule(void);
      uint8_t TSIL_SetNSamples(void);
      uint8_t TSIL_Recalibrate(void);
      uint8_t TSIL_InitTrigger(void);

      /***************************************************************************//*!
      *
      * @brief  Control function for TSIL General Initialization
      *
      * @param  psModuleStruct - Specification of TSIL module
      *
      * @return Status Code
      *
      * @remarks
      *
      ****************************************************************************/
      uint8_t TSIL_InitModule(void)
      {
        uint8_t u8Result = TSS_INIT_STATUS_OK;

        /* TSI_CS0 Settings */
        TSS_GET_TSIL_REGISTER(CS0) = 0; /* Erase CS0 - Turn OFF TSI*/

        /* TSI_CS2 Settings */
        TSS_GET_TSIL_REGISTER(CS2) = 0; /* Erase CS2 */
        /* Both OSC settings */
        #if TSS_TSIL_DVOLT != 0
          TSS_GET_TSIL_REGISTER(CS2) |= TSIL_CS2_DVOLT_FIT(TSS_TSIL_DVOLT);     /* Delta Voltage settings for ElOSC and RefOSC 0 = 1.03 V, 3 = 0.29 V */
        #endif
        /* TSI RefOSC definitions */
        TSS_GET_TSIL_REGISTER(CS2) |= TSIL_CS2_REFCHRG_FIT(TSS_TSIL_REFCHRG);   /* Reference Oscillator Charge Current 0 = 500 nA, 7 = 64uA */

        return u8Result;
      }

      /***************************************************************************//*!
      *
      * @brief  Control function for TSIL Number of Samples Settings
      *
      * @param  psModuleStruct - Specification of TSIL module
      *
      * @return Status Code
      *
      * @remarks
      *
      ****************************************************************************/
      uint8_t TSIL_SetNSamples(void)
      {
        uint8_t u8Result = TSS_INIT_STATUS_OK;
        uint8_t u8NSamples;

        TSS_GET_TSIL_REGISTER(CS0) &= ~((uint8_t)TSI_CS0_TSIEN_MASK);  /* Turn OFF TSI Module*/
        TSS_GET_TSIL_REGISTER(CS1) &= ~((uint8_t)TSI_CS1_NSCN_MASK);   /* Erase NSCN */
        /* Number of Samples settings */
        u8NSamples = tss_CSSys.NSamples;
        TSS_GET_TSIL_REGISTER(CS1) |= TSIL_CS1_NSCN_FIT(u8NSamples-1u);  /* Number of Consecutive Scans per Electrode 0 = 1 scan, 31 = 32 scans*/

        return u8Result;
      }

      /***************************************************************************//*!
      *
      * @brief  Control function for TSIL Recalibration
      *
      * @param  *psMethodROMDataStruct - Specification of ROMDATA Struct
      *         psModuleStruct - Specification of TSIL module
      *
      * @return Status Code
      *
      * @remarks
      *
      ****************************************************************************/
      #if ((TSS_TSIL_EXTCHRG_RANGE == 1) && (TSS_TSIL_PS_RANGE == 1))
        uint8_t TSIL_Recalibrate(void)
        {
          uint8_t u8Result = TSS_INIT_STATUS_OK;

          TSS_GET_TSIL_REGISTER(CS0) &= ~((uint8_t)TSI_CS0_TSIEN_MASK);    /* Turn OFF TSI Module */
          TSS_GET_TSIL_REGISTER(CS1) &= ~((uint8_t)TSI_CS1_PS_MASK);       /* Erase PS */
          TSS_GET_TSIL_REGISTER(CS1) |= TSIL_CS1_PS_FIT(TSS_TSIL_PS_LOW_LIMIT); /* Set Prescaler for Electrode OSC - Set by the user 0 = divide 1, 7 = divide 128 */
          TSS_GET_TSIL_REGISTER(CS2) &= ~((uint8_t)TSI_CS2_EXTCHRG_MASK);  /* Erase ExtChrg */
          TSS_GET_TSIL_REGISTER(CS2) |= TSIL_CS2_EXTCHRG_FIT(TSS_TSIL_EXTCHRG_LOW_LIMIT); /* External OSC Charge Current 0= 500nA, 7 = 64uA */

          tsi_au16Resolution = TSS_TSIL_RESOLUTION_VALUE;  /* Store the resolution value */

          return u8Result;
        }
      #else
        #ifdef TSS_ONPROXIMITY_CALLBACK
          uint8_t TSIL_Recalibrate(void)
          {
            TSIL_CALIBRATION FinalTSICalibration;
            TSIL_CALIBRATION FinalTSICalibrationBelow;
            TSIL_CALIBRATION TempTSICalibration;
            TSIL_CALIBRATION OldTSICalibration;
            uint8_t u8Result = TSS_INIT_STATUS_OK;
            uint8_t u8ElCounter;
            uint8_t u8TSIPSTemp;
            uint8_t u8TSIExtChrgLowTemp;
            uint8_t u8TSIExtChrgTemp;
            uint8_t u8CalibrationComplete;
            uint16_t u16CapSampleTemp;
            uint8_t u8Iteration;
            uint16_t u16CapSampleAverage;
            uint8_t u8FinalTSICalibrationFound;
            uint8_t u8Temp;
            uint16_t u16ResolutionValue;
            uint8_t u8EXTCHRGRange;
            uint8_t u8PSHighLimit;
            uint8_t u8Found;

            /* Save previous TSI Calibration */
            OldTSICalibration.u8TSIPS = (uint8_t) ((TSS_GET_TSIL_REGISTER(CS1) & TSI_CS1_PS_MASK) >> TSI_CS1_PS_BITNUM);
            OldTSICalibration.u8TSIExtChrg = (uint8_t) ((TSS_GET_TSIL_REGISTER(CS2) & TSI_CS2_EXTCHRG_MASK) >> TSI_CS2_EXTCHRG_BITNUM);
            /* Set Variables */
            if (tss_CSSys.SystemConfig.ProximityEn)
            {
              u16ResolutionValue = TSS_TSIL_PROX_RESOLUTION_VALUE;
              u8EXTCHRGRange = TSS_TSIL_PROX_EXTCHRG_RANGE;
              u8PSHighLimit = TSS_TSIL_PROX_PS_HIGH_LIMIT;
              u8TSIPSTemp = TSS_TSIL_PROX_PS_LOW_LIMIT;
              u8TSIExtChrgLowTemp = TSS_TSIL_PROX_EXTCHRG_LOW_LIMIT;
            }
            else
            {
              u16ResolutionValue = TSS_TSIL_RESOLUTION_VALUE;
              u8EXTCHRGRange = TSS_TSIL_EXTCHRG_RANGE;
              u8PSHighLimit = TSS_TSIL_PS_HIGH_LIMIT;
              u8TSIPSTemp = TSS_TSIL_PS_LOW_LIMIT;
              u8TSIExtChrgLowTemp = TSS_TSIL_EXTCHRG_LOW_LIMIT;
            }
            /* Do Autocalibration */
            FinalTSICalibration.u16Distance = 0xFFFFu;
            FinalTSICalibrationBelow.u16Distance = 0xFFFFu;
            u8FinalTSICalibrationFound = 0u;
            /* Set TSI registers for Single measurement */
            TSS_GET_TSIL_REGISTER(CS0) &= ~((uint8_t)TSI_CS0_TSIEN_MASK);     /* Turn OFF TSI Module */
            TSS_GET_TSIL_REGISTER(CS0) &= ~((uint8_t)TSI_CS0_TSIIEN_MASK);    /* Disable Interrupts */
            TSS_GET_TSIL_REGISTER(CS0) &= ~((uint8_t)TSI_CS0_STM_MASK);    /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
            /* Check if all electrodes has proper Counter value */
            do
            {
              u8TSIExtChrgTemp = u8TSIExtChrgLowTemp + (u8EXTCHRGRange >> 1u);
              u8Iteration = 0u;
              TempTSICalibration.u16Distance = 0xFFFFu;
              u8Found = 0u;
              do
              {
                /* Increase iteration level */
                u8Iteration += 1u;
                /* Check if all electrodes has proper Counter value */
                u8CalibrationComplete = 1u;
                u16CapSampleAverage = 0u;
                /* Go through all enabled pins */
                for(u8ElCounter = 0u; u8ElCounter < TSS_N_ELECTRODES; u8ElCounter++)
                {
                  if (TSS_GET_MODULE_ID(TSS_TSIL_MAIN_MODULE_NAME) == ((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsil_cu8ModuleID)
                  {
                    u8Temp = TSS_INIT_STATUS_OK;
                    if (tss_CSSys.SystemConfig.ProximityEn || tss_CSSys.SystemConfig.LowPowerEn)
                    {
                      if (u8ElCounter == tss_CSSys.LowPowerElectrode)
                      {
                        u8Temp = TSS_INIT_STATUS_LOWPOWER_SET;
                      }
                    }
                    else if (tss_au8ElectrodeEnablers[u8ElCounter >> 3u] & ( 1u << (u8ElCounter & 0x07u)))
                    {
                      u8Temp = TSS_INIT_STATUS_LOWPOWER_SET;
                    }
                    /* Read TSI Counter if allowed */
                    if (u8Temp == TSS_INIT_STATUS_LOWPOWER_SET)
                    {
                      /* Configure TSIL */
                      TSS_GET_TSIL_REGISTER(CS1) &= ~((uint8_t)TSI_CS1_PS_MASK);      /* Erase PS */
                      TSS_GET_TSIL_REGISTER(CS1) |= TSIL_CS1_PS_FIT(u8TSIPSTemp);   /* Set Prescaler for Electrode OSC - Set by the user 0 = divide 1, 7 = divide 128 */
                      TSS_GET_TSIL_REGISTER(CS2) &= ~((uint8_t)TSI_CS2_EXTCHRG_MASK); /* Erase ExtChrg */
                      TSS_GET_TSIL_REGISTER(CS2) |= TSIL_CS2_EXTCHRG_FIT(u8TSIExtChrgTemp); /* External OSC Charge Current 0= 500nA, 7 = 64uA */
                      /* Read TSI Counter */
                      u16CapSampleTemp = tss_faSampleElectrodeLow[u8ElCounter]();             /* Samples one electrode */
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
                }
                /* Evaluate Measurement - Reuse of u16CapSampleTemp */
                if(u16CapSampleAverage > u16ResolutionValue )
                {
                  u16CapSampleTemp = u16CapSampleAverage - u16ResolutionValue;
                }
                else
                {
                  u16CapSampleTemp = u16ResolutionValue - u16CapSampleAverage;
                }
                /* All samples > resolution AND (Avarage > resolution OR already found and tune it) */
                if ((u8CalibrationComplete == 1u ) && ((u16CapSampleAverage > u16ResolutionValue) || (u8Found && (TempTSICalibration.u16Distance >= u16CapSampleTemp))))
                {
                  TempTSICalibration.u8TSIExtChrg = u8TSIExtChrgTemp;
                  TempTSICalibration.u8TSIPS = u8TSIPSTemp;
                  TempTSICalibration.u16Distance = u16CapSampleTemp;
                  u8Found++; /* Match for set PS found */
                }
                else if (TempTSICalibration.u16Distance >= u16CapSampleTemp && (u8Found ==0))
                {
                  TempTSICalibration.u8TSIExtChrg = u8TSIExtChrgTemp;
                  TempTSICalibration.u8TSIPS = u8TSIPSTemp;
                  TempTSICalibration.u16Distance = u16CapSampleTemp;
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
              /* The value only if a distance is lower and was found with sufficient resolution */
              if ((FinalTSICalibration.u16Distance >= TempTSICalibration.u16Distance) && (u8Found > 0))
              {
                FinalTSICalibration.u8TSIPS = TempTSICalibration.u8TSIPS;
                FinalTSICalibration.u8TSIExtChrg = TempTSICalibration.u8TSIExtChrg;
                FinalTSICalibration.u16Distance = TempTSICalibration.u16Distance;
                u8FinalTSICalibrationFound++;
              }
              else if (FinalTSICalibrationBelow.u16Distance >= TempTSICalibration.u16Distance && u8FinalTSICalibrationFound == 0)
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
              tsi_au16Resolution = u16ResolutionValue;  /* Store the resolution value */
            }
            else
            {
              FinalTSICalibration.u8TSIPS = FinalTSICalibrationBelow.u8TSIPS;
              FinalTSICalibration.u8TSIExtChrg = FinalTSICalibrationBelow.u8TSIExtChrg;
              FinalTSICalibration.u16Distance = FinalTSICalibrationBelow.u16Distance;
              /* Reuse of u8Iteration & u16CapSampleTemp*/
              u8Iteration = 0u;
              u16CapSampleTemp = u16ResolutionValue - FinalTSICalibrationBelow.u16Distance;
              /* Determine TSS Resolution (highest bit set) */
              while (u16CapSampleTemp >> u8Iteration)
              {
                u8Iteration++;
              }
              /* Store the present resolution value */
              tsi_au16Resolution = (1u << (uint8_t)(u8Iteration - 1u));
            }
            /* Set Final Found values */
            TSS_GET_TSIL_REGISTER(CS0) &= ~((uint8_t)TSI_CS0_TSIEN_MASK);     /* Turn OFF TSI Module */
            TSS_GET_TSIL_REGISTER(CS1) &= ~((uint8_t)TSI_CS1_PS_MASK);        /* Erase PS */
            TSS_GET_TSIL_REGISTER(CS1) |= TSIL_CS1_PS_FIT(FinalTSICalibration.u8TSIPS); /* Set Prescaler for Electrode OSC - Set by the user 0 = divide 1, 7 = divide 128 */
            TSS_GET_TSIL_REGISTER(CS2) &= ~((uint8_t)TSI_CS2_EXTCHRG_MASK);   /* Erase ExtChrg */
            TSS_GET_TSIL_REGISTER(CS2) |= TSIL_CS2_EXTCHRG_FIT(FinalTSICalibration.u8TSIExtChrg); /* External OSC Charge Current 0= 500nA, 7 = 64uA */
            /* Save Value for Proximity function */
            if (tss_CSSys.SystemConfig.ProximityEn)
            {
              tss_u16CapSample = FinalTSICalibration.u16Distance + u16ResolutionValue;
            }
            /* Indication of changed configuration */
            if ((OldTSICalibration.u8TSIPS != FinalTSICalibration.u8TSIPS) || (OldTSICalibration.u8TSIExtChrg != FinalTSICalibration.u8TSIExtChrg))
            {
              u8Result |= TSS_INIT_STATUS_CALIBRATION_CHANGED;
            }

            return u8Result;
          }
        #else
          uint8_t TSIL_Recalibrate(void)
          {
            TSIL_CALIBRATION FinalTSICalibration;
            TSIL_CALIBRATION FinalTSICalibrationBelow;
            TSIL_CALIBRATION TempTSICalibration;
            TSIL_CALIBRATION OldTSICalibration;
            uint8_t u8result = TSS_INIT_STATUS_OK;
            uint8_t u8ElCounter;
            uint8_t u8TSIPSTemp;
            uint8_t u8TSIExtChrgTemp;
            uint8_t u8CalibrationComplete;
            uint16_t u16CapSampleTemp;
            uint8_t u8Iteration;
            uint16_t u16CapSampleAverage;
            uint8_t u8FinalTSICalibrationFound;
            uint8_t u8Found;

            /* Save previous TSI Calibration */
            OldTSICalibration.u8TSIPS = (uint8_t) ((TSS_GET_TSIL_REGISTER(CS1) & TSI_CS1_PS_MASK) >> TSI_CS1_PS_BITNUM);
            OldTSICalibration.u8TSIExtChrg = (uint8_t) ((TSS_GET_TSIL_REGISTER(CS2) & TSI_CS2_EXTCHRG_MASK) >> TSI_CS2_EXTCHRG_BITNUM);
            /* Do Autocalibration */
            u8TSIPSTemp = TSS_TSIL_PS_LOW_LIMIT;
            FinalTSICalibration.u16Distance = 0xFFFFu;
            FinalTSICalibrationBelow.u16Distance = 0xFFFFu;
            u8FinalTSICalibrationFound = 0u;
            /* Set TSI registers for Single measurement */
            TSS_GET_TSIL_REGISTER(CS0) &= ~((uint8_t)TSI_CS0_TSIEN_MASK);     /* Turn OFF TSI Module */
            TSS_GET_TSIL_REGISTER(CS0) &= ~((uint8_t)TSI_CS0_TSIIEN_MASK);    /* Disable Interrupts */
            TSS_GET_TSIL_REGISTER(CS0) &= ~((uint8_t)TSI_CS0_STM_MASK);    /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
            /* Check if all electrodes has proper Counter value */
            do
            {
              #if (TSS_TSIL_EXTCHRG_RANGE > 1)
                u8TSIExtChrgTemp = TSS_TSIL_EXTCHRG_LOW_LIMIT + (TSS_TSIL_EXTCHRG_RANGE / 2u);
              #else
                u8TSIExtChrgTemp = TSS_TSIL_EXTCHRG_LOW_LIMIT;
              #endif
              u8Iteration = 0u;
              TempTSICalibration.u16Distance = 0xFFFFu;
              u8Found = 0u;
              #if (TSS_TSIL_EXTCHRG_RANGE > 1)
                do
                {
              #endif
                /* Increase iteration level */
                u8Iteration += 1u;
                /* Check if all electrodes has proper Counter value */
                u8CalibrationComplete = 1u;
                u16CapSampleAverage = 0u;
                /* Go through all enabled pins */
                for(u8ElCounter = 0u; u8ElCounter < TSS_N_ELECTRODES; u8ElCounter++)
                {
                  if (TSS_GET_MODULE_ID(TSS_TSIL_MAIN_MODULE_NAME) == ((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsil_cu8ModuleID)
                  {
                    if (tss_au8ElectrodeEnablers[u8ElCounter >> 3u] & ( 1u << (u8ElCounter & 0x07u)))
                    {
                      /* Configure TSIL */
                      TSS_GET_TSIL_REGISTER(CS1) &= ~((uint8_t)TSI_CS1_PS_MASK);      /* Erase PS */
                      TSS_GET_TSIL_REGISTER(CS1) |= TSIL_CS1_PS_FIT(u8TSIPSTemp);   /* Set Prescaler for Electrode OSC - Set by the user 0 = divide 1, 7 = divide 128 */
                      TSS_GET_TSIL_REGISTER(CS2) &= ~((uint8_t)TSI_CS2_EXTCHRG_MASK); /* Erase ExtChrg */
                      TSS_GET_TSIL_REGISTER(CS2) |= TSIL_CS2_EXTCHRG_FIT(u8TSIExtChrgTemp); /* External OSC Charge Current 0= 500nA, 7 = 64uA */
                      /* Read TSI Counter */
                      u16CapSampleTemp = tss_faSampleElectrodeLow[u8ElCounter]();   /* Samples one electrode */
                      if (u16CapSampleTemp < TSS_TSIL_RESOLUTION_VALUE)
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
                }
                /* Evaluate Measurement - Reuse of u16CapSampleTemp */
                if(u16CapSampleAverage > TSS_TSIL_RESOLUTION_VALUE )
                {
                  u16CapSampleTemp = u16CapSampleAverage - TSS_TSIL_RESOLUTION_VALUE;
                }
                else
                {
                  u16CapSampleTemp = TSS_TSIL_RESOLUTION_VALUE - u16CapSampleAverage;
                }
                /* All samples > resolution AND (Average > resolution OR already found and tune it) */
                if ((u8CalibrationComplete == 1u ) && ((u16CapSampleAverage > TSS_TSIL_RESOLUTION_VALUE) || (u8Found && (TempTSICalibration.u16Distance >= u16CapSampleTemp))))
                {
                  TempTSICalibration.u8TSIExtChrg = u8TSIExtChrgTemp;
                  TempTSICalibration.u8TSIPS = u8TSIPSTemp;
                  TempTSICalibration.u16Distance = u16CapSampleTemp;
                  u8Found++; /* Match for set PS found */
                }
                else if ((TempTSICalibration.u16Distance >= u16CapSampleTemp) && (u8Found ==0))
                {
                  TempTSICalibration.u8TSIExtChrg = u8TSIExtChrgTemp;
                  TempTSICalibration.u8TSIPS = u8TSIPSTemp;
                  TempTSICalibration.u16Distance = u16CapSampleTemp;
                }
                #if (TSS_TSIL_EXTCHRG_RANGE > 1)
                  /* Change ExtCharge within the interval */
                  if(u8CalibrationComplete == 1u)
                  {
                    u8TSIExtChrgTemp += ((TSS_TSIL_EXTCHRG_RANGE / 2u) >> u8Iteration);
                  }
                  else
                  {
                    u8TSIExtChrgTemp -= ((TSS_TSIL_EXTCHRG_RANGE / 2u) >> u8Iteration);
                  }
                #endif
              } while (((TSS_TSIL_EXTCHRG_RANGE / 2u) >> u8Iteration) > 0u);
              /* The value only if a distance is lower and was found with sufficient resolution */
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
            } while (u8TSIPSTemp < (TSS_TSIL_PS_HIGH_LIMIT + 1u));
            /* Check if searching was succesful */
            if (u8FinalTSICalibrationFound)
            {
              tsi_au16Resolution = TSS_TSIL_RESOLUTION_VALUE;
            }
            else
            {
              FinalTSICalibration.u8TSIPS = FinalTSICalibrationBelow.u8TSIPS;
              FinalTSICalibration.u8TSIExtChrg = FinalTSICalibrationBelow.u8TSIExtChrg;
              FinalTSICalibration.u16Distance = FinalTSICalibrationBelow.u16Distance;
              /* Reuse of u8Iteration & u16CapSampleTemp */
              u8Iteration = 0u;
              u16CapSampleTemp = TSS_TSIL_RESOLUTION_VALUE - FinalTSICalibrationBelow.u16Distance;
              /* Determine TSS Resolution (highest bit set) */
              while (u16CapSampleTemp >> u8Iteration)
              {
                u8Iteration++;
              }
              /* Store the present resolution value */
              tsi_au16Resolution = (1u << (u8Iteration - 1u));
            }
            /* Set Final Found values */
            TSS_GET_TSIL_REGISTER(CS0) &= ~((uint8_t)TSI_CS0_TSIEN_MASK);     /* Turn OFF TSI Module */
            TSS_GET_TSIL_REGISTER(CS1) &= ~((uint8_t)TSI_CS1_PS_MASK);        /* Erase PS */
            TSS_GET_TSIL_REGISTER(CS1) |= TSIL_CS1_PS_FIT(FinalTSICalibration.u8TSIPS); /* Set Prescaler for Electrode OSC - Set by the user 0 = divide 1, 7 = divide 128 */
            TSS_GET_TSIL_REGISTER(CS2) &= ~((uint8_t)TSI_CS2_EXTCHRG_MASK);   /* Erase ExtChrg */
            TSS_GET_TSIL_REGISTER(CS2) |= TSIL_CS2_EXTCHRG_FIT(FinalTSICalibration.u8TSIExtChrg); /* External OSC Charge Current 0= 500nA, 7 = 64uA */
            /* Indication of changed configuration */
            if ((OldTSICalibration.u8TSIPS != FinalTSICalibration.u8TSIPS) || (OldTSICalibration.u8TSIExtChrg != FinalTSICalibration.u8TSIExtChrg))
            {
              u8result |= TSS_INIT_STATUS_CALIBRATION_CHANGED;
            }

            return u8result;
          }
        #endif
      #endif

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
      uint8_t TSIL_InitTrigger(void)
      {
        uint8_t u8Result = TSS_INIT_STATUS_OK;

        TSS_GET_TSIL_REGISTER(CS0) &= ~((uint8_t)TSI_CS0_TSIEN_MASK); /* Turn OFF TSI Module */
        /* Setup TSI */
        TSS_GET_TSIL_REGISTER(CS0) &= ~((uint8_t)TSI_CS0_STM_MASK);   /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
        TSS_GET_TSIL_REGISTER(CS0) &= ~((uint8_t)TSI_CS0_TSIIEN_MASK);  /* TSI Interrupt Disable (0 = disabled, 1 = enabled) */

        u8Result |= TSS_INIT_STATUS_TRIGGER_SET;

        return u8Result;
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
      uint8_t TSIL_MethodControl(uint8_t u8Command)
      {
        uint8_t u8Result = TSS_INIT_STATUS_OK;

        switch(u8Command)
        {
          /************* Do TSIL Init *******************/
          case TSS_INIT_COMMAND_INIT_MODULES:
            u8Result |= TSIL_InitModule();
            u8Result |= TSIL_SetNSamples();
            u8Result |= TSIL_Recalibrate();
            u8Result |= TSIL_InitTrigger();
          break;
          /************* Set NSamples ***************/
          case TSS_INIT_COMMAND_SET_NSAMPLES:
            u8Result |= TSIL_SetNSamples();
            u8Result |= TSIL_Recalibrate();
            u8Result |= TSIL_InitTrigger();
          break;
          /*** Do Auto calibration setting of TSIL EXTCHRG and TSIL PS ***/
          case TSS_INIT_COMMAND_RECALIBRATE:
            u8Result |= TSIL_Recalibrate();
            u8Result |= TSIL_InitTrigger();
          break;
          /************* Triggering Init ************/
          case TSS_INIT_COMMAND_INIT_TRIGGER:
            u8Result |= TSIL_InitTrigger();
          break;
        }
        /* Exit */
        return u8Result;
      }

      /***************************************************************************//*!
      *
      * @brief  Initializes the modules for the sensing of the electrodes
      *
      * @param  Nothing
      *
      * @return Status Code
      *
      * @remarks
      *
      ****************************************************************************/
      uint8_t TSIL_SensorInit(uint8_t u8Command)
      {
        uint8_t u8Result = TSS_INIT_STATUS_OK;

        u8Result = TSIL_MethodControl(u8Command);

        return u8Result;
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
      uint8_t TSIL_SampleElectrode(uint8_t u8ElecNum, uint8_t u8Command)
      {
        uint8_t u8ElCounter;
        uint8_t u8ModuleID;
        uint8_t u8ElectrodeStatus;

        u8ModuleID = ((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElecNum]))->tsil_cu8ModuleID;
        /* Module Control */
        switch (u8Command)
        {
          case TSS_SAMPLE_COMMAND_RESTART:
            u8ElectrodeStatus = TSS_SAMPLE_STATUS_PROCESSING;
          break;
          case TSS_SAMPLE_COMMAND_PROCESS:
            #ifdef _TSS_TEST
              _TSS_TSIGEN_ON;  /* Start generator for test purpose */
            #endif
            tss_u16CapSample = tss_faSampleElectrodeLow[u8ElecNum]();             /* Samples one electrode */
            #ifdef _TSS_TEST
              _TSS_TSIGEN_OFF;  /* Stop generator for test purpose */
            #endif
            if (tss_u16CapSample > TSS_TSIL_CAP_HIGH_LIMIT(tsi_au16Resolution))
            {
              u8ElectrodeStatus = TSS_SAMPLE_ERROR_CHARGE_TIMEOUT;    /* Too High Capacity is detected */
            }
            else if (tss_u16CapSample > TSS_TSIL_RECALIBRATION_HIGH_THRESHOLD(tsi_au16Resolution))
            {
              u8ElectrodeStatus = TSS_SAMPLE_RECALIB_REQUEST_HICAP;   /* Re-calibration is needed */
            }
            else if (tss_u16CapSample < TSS_TSIL_CAP_LOW_LIMIT(tsi_au16Resolution))
            {
              u8ElectrodeStatus = TSS_SAMPLE_ERROR_SMALL_CAP;         /* Too Low Capacity is detected */
            }
            else if (tss_u16CapSample < TSS_TSIL_RECALIBRATION_LOW_THRESHOLD(tsi_au16Resolution))
            {
              u8ElectrodeStatus = TSS_SAMPLE_RECALIB_REQUEST_LOCAP;   /* Re-calibration is needed */
            }
            else
            {
              u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
            }
          break;
          case TSS_SAMPLE_COMMAND_GET_NEXT_ELECTRODE:
            /* Find the next electrode within the electrode module */
            u8ElCounter = u8ElecNum;
            #if TSS_N_ELECTRODES > 1
              do
              {
                if (u8ElCounter < (TSS_N_ELECTRODES-1))
                {
                  u8ElCounter++;
                }
                else
                {
                  u8ElCounter = 0u;
                }
              } while (u8ModuleID != ((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsil_cu8ModuleID);
            #endif
            /* Return found electrode number */
            u8ElectrodeStatus = u8ElCounter;
          break;
          case TSS_SAMPLE_COMMAND_RECALIB:
            u8ElectrodeStatus = TSIL_MethodControl(TSS_INIT_COMMAND_RECALIBRATE);
            /* Evaluate result */
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
          case TSS_SAMPLE_COMMAND_SET_LOWLEVEL_CONFIG:
            TSS_GET_TSIL_REGISTER(CS0) &= ~((uint8_t)TSI_CS0_TSIEN_MASK);     /* Turn OFF TSI Module */
            TSS_GET_TSIL_REGISTER(CS1) &= ~((uint8_t)TSI_CS1_PS_MASK);        /* Erase PS */
            TSS_GET_TSIL_REGISTER(CS1) |= TSIL_CS1_PS_FIT(tss_u16CapSample & 0x7u); /* Set Prescaler for Electrode OSC - Set by the user 0 = divide 1, 7 = divide 128 */
            TSS_GET_TSIL_REGISTER(CS2) &= ~((uint8_t)TSI_CS2_EXTCHRG_MASK);   /* Erase ExtChrg */
            TSS_GET_TSIL_REGISTER(CS2) |= TSIL_CS2_EXTCHRG_FIT((tss_u16CapSample >> 3u) & 0x7u); /* External OSC Charge Current 0= 500nA, 7 = 64uA */

            u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
          break;
          case TSS_SAMPLE_COMMAND_GET_LOWLEVEL_CONFIG:
            u8ElectrodeStatus = (uint8_t) ((TSS_GET_TSIL_REGISTER(CS1) & TSI_CS1_PS_MASK) >> TSI_CS1_PS_BITNUM);
            tss_u16CapSample = (uint16_t) ((((TSS_GET_TSIL_REGISTER(CS2) & TSI_CS2_EXTCHRG_MASK) >> TSI_CS2_EXTCHRG_BITNUM) << 3u) | u8ElectrodeStatus);

            u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
          break;
        }

        return u8ElectrodeStatus;   /* Return status code */
      }

    #else /* TSS_USE_SIMPLE_LOW_LEVEL == 0 */

      /************************ Prototypes **************************/

      uint8_t TSIL_MethodControl(uint8_t u8ElNum, uint8_t u8Command);

      /*******************  Modules definition **********************/
      #if TSS_DETECT_MODULE(TSIL)
        uint16_t TSS_TSIL_CHANNEL_COUNTER_BUFFER[16];
        const TSS_TSIL_METHOD_ROMDATA TSS_TSIL_METHOD_ROMDATA_CONTEXT = {TSIL_MethodControl, (uint8_t*) &TSI_CS0, TSS_TSIL_CHANNEL_COUNTER_BUFFER};
        volatile TSS_TSIL_METHOD_RAMDATA TSS_TSIL_METHOD_RAMDATA_CONTEXT = {TSIL_EOSF_STATE_NONE, 0u, TSIL_SCNIP_STATE_NONE};
      #endif
      #if TSS_DETECT_MODULE(TSIL0)
        uint16_t TSS_TSIL0_CHANNEL_COUNTER_BUFFER[16];
        const TSS_TSIL_METHOD_ROMDATA TSS_TSIL0_METHOD_ROMDATA_CONTEXT = {TSIL_MethodControl, (uint8_t*) &TSI0_CS0, TSS_TSIL0_CHANNEL_COUNTER_BUFFER};
        volatile TSS_TSIL_METHOD_RAMDATA TSS_TSIL0_METHOD_RAMDATA_CONTEXT = {TSIL_EOSF_STATE_NONE, 0u, TSIL_SCNIP_STATE_NONE};
      #endif
      #if TSS_DETECT_MODULE(TSIL1)
        uint16_t TSS_TSIL1_CHANNEL_COUNTER_BUFFER[16];
        const TSS_TSIL_METHOD_ROMDATA TSS_TSIL1_METHOD_ROMDATA_CONTEXT = {TSIL_MethodControl, (uint8_t*) &TSI1_CS0, TSS_TSIL1_CHANNEL_COUNTER_BUFFER};
        volatile TSS_TSIL_METHOD_RAMDATA TSS_TSIL1_METHOD_RAMDATA_CONTEXT = {TSIL_EOSF_STATE_NONE, 0u, TSIL_SCNIP_STATE_NONE};
      #endif

      #include "TSS_SensorTSIL_def.h" /* ROMDATA and RAMDATA TSIL definitions */

      /***************************************************************************//*!
      *                                 Types
      ****************************************************************************/

      /* TSIL Recalibration Struct */

      typedef struct {
        uint8_t u8TSIPS;
        uint8_t u8TSIExtChrg;
        uint16_t u16Distance;
      } TSIL_CALIBRATION;

      static uint16_t tsi_au16Resolution; /* TSIL resolution */

      /************************** Prototypes ***************************/

      uint8_t TSIL_InitModule(TSS_TSIL_MemMapPtr psModuleStruct);
      uint8_t TSIL_SetNSamples(TSS_TSIL_MemMapPtr psModuleStruct);
      uint8_t TSIL_InitPEN(TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSIL_MemMapPtr psModuleStruct);
      uint8_t TSIL_Recalibrate(TSS_TSIL_MemMapPtr psModuleStruct);
      uint8_t TSIL_InitTrigger(TSS_TSIL_MemMapPtr psModuleStruct, TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct);
      uint8_t TSIL_ReadCounter(TSS_TSIL_ELECTRODE_ROMDATA *psElectrodeROMDataStruct, TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct);

      /***************************************************************************//*!
      *
      * @brief  Control function for TSIL General Initialization
      *
      * @param  psModuleStruct - Specification of TSIL module
      *
      * @return Status Code
      *
      * @remarks
      *
      ****************************************************************************/
      uint8_t TSIL_InitModule(TSS_TSIL_MemMapPtr psModuleStruct)
      {
        uint8_t u8Result = TSS_INIT_STATUS_OK;

        /* TSI_CS0 Settings */
        psModuleStruct->CS0 = 0; /* Erase CS0 - Turn OFF TSI*/

        /* TSI_CS2 Settings */
        psModuleStruct->CS2 = 0; /* Erase CS2 */
        /* Both OSC settings */
        #if TSS_TSIL_DVOLT != 0
          psModuleStruct->CS2 |= TSIL_CS2_DVOLT_FIT(TSS_TSIL_DVOLT);         /* Delta Voltage settings for ElOSC and RefOSC 0 = 1.03 V, 3 = 0.29 V */
        #endif
        /* TSI RefOSC definitions */
        psModuleStruct->CS2 |= TSIL_CS2_REFCHRG_FIT(TSS_TSIL_REFCHRG);   /* Reference Oscillator Charge Current 0 = 500 nA, 7 = 64uA */

        return u8Result;
      }

      /***************************************************************************//*!
      *
      * @brief  Control function for TSIL Number of Samples Settings
      *
      * @param  psModuleStruct - Specification of TSIL module
      *
      * @return Status Code
      *
      * @remarks
      *
      ****************************************************************************/
      uint8_t TSIL_SetNSamples(TSS_TSIL_MemMapPtr psModuleStruct)
      {
        uint8_t u8Result = TSS_INIT_STATUS_OK;
        uint8_t u8NSamples;

        psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_TSIEN_MASK);     /* Turn OFF TSI Module*/
        psModuleStruct->CS1 &= ~((uint8_t)TSI_CS1_NSCN_MASK);      /* Erase NSCN */
        /* Number of Samples settings */
        u8NSamples = tss_CSSys.NSamples;
        psModuleStruct->CS1 |= TSIL_CS1_NSCN_FIT(u8NSamples-1u);  /* Number of Consecutive Scans per Electrode 0 = 1 scan, 31 = 32 scans*/

        return u8Result;
      }

      /***************************************************************************//*!
      *
      * @brief  Control function for TSIL Pin Enable Settings
      *
      * @param  *psMethodROMDataStruct - Specificcation of ROMDATA Struct
      *         psModuleStruct - Specification of TSIL module
      *
      * @return Status Code
      *
      * @remarks
      *
      ****************************************************************************/
      uint8_t TSIL_InitPEN(TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSIL_MemMapPtr psModuleStruct)
      {
        uint8_t u8Result = TSS_INIT_STATUS_OK;
        uint8_t u8ElCounter;
        uint8_t u8ChannelNum;

        /* PEN Clearing */
        if (tss_CSSys.SystemConfig.ProximityEn || tss_CSSys.SystemConfig.LowPowerEn)
        {
          psModuleStruct->PEN.Word = 0u;
        }
        else
        {
          for(u8ElCounter = 0; u8ElCounter < TSS_N_ELECTRODES; u8ElCounter++) /* Find all with the same module */
          {
            if (psMethodROMDataStruct == (TSS_TSIL_METHOD_ROMDATA *) (((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsil_cpsMethodROMData))
            {
              /* Disable Electrode */
              if (!(tss_au8ElectrodeEnablers[u8ElCounter >> 3u] & (1u << (u8ElCounter & 0x07u))))
              {
                u8ChannelNum = ((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsil_cu8ChannelNum;
                if ((psModuleStruct->PEN.Byte[u8ChannelNum >> 3u] & (1u << (u8ChannelNum & 0x07u))) != 0u)
                {
                  psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_TSIEN_MASK); /* Turn OFF TSI Module if TSI was enabled */
                  /* If the Electrode is enabled then disable Electrode */
                  psModuleStruct->PEN.Byte[u8ChannelNum >> 3u] &= ~((uint8_t)(1u << (u8ChannelNum & 0x07u)));
                }
              }
            }
          }
        }
        /* PEN Enabling - The loop must be performed twice because one PEN can be assigned to more TSS electrodes */
        for(u8ElCounter = 0; u8ElCounter < TSS_N_ELECTRODES; u8ElCounter++) /* Find all with the same module */
        {
          if (psMethodROMDataStruct == (TSS_TSIL_METHOD_ROMDATA *) (((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsil_cpsMethodROMData))
          {
            /* Enable Proximity & Low Power Electrode */
            if (tss_CSSys.SystemConfig.ProximityEn || tss_CSSys.SystemConfig.LowPowerEn)
            {
              if (u8ElCounter == tss_CSSys.LowPowerElectrode)
              {
                u8ChannelNum = ((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsil_cu8ChannelNum;
                psModuleStruct->PEN.Byte[u8ChannelNum >> 3u] |= ((uint8_t)(1u << (u8ChannelNum & 0x07u)));
              }
            }
            else
            {
              /* Enable Standard Electrode */
              if (tss_au8ElectrodeEnablers[u8ElCounter >> 3u] & (1u << (u8ElCounter & 0x07u)))
              {
                u8ChannelNum = ((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsil_cu8ChannelNum;
                if ((psModuleStruct->PEN.Byte[u8ChannelNum >> 3u] & (1u << (u8ChannelNum & 0x07u))) == 0u)
                {
                  psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_TSIEN_MASK); /* Turn OFF TSI Module if TSI was enabled */
                  /* If the Electrode is enabled then disable Electrode */
                  psModuleStruct->PEN.Byte[u8ChannelNum >> 3u] |= ((uint8_t)(1u << (u8ChannelNum & 0x07u)));
                }
              }
            }
          }
        }

        return u8Result;
      }

      /***************************************************************************//*!
      *
      * @brief  Control function for TSIL Recalibration
      *
      * @param  *psMethodROMDataStruct - Specification of ROMDATA Struct
      *         psModuleStruct - Specification of TSIL module
      *
      * @return Status Code
      *
      * @remarks
      *
      ****************************************************************************/
      #if ((TSS_TSIL_EXTCHRG_RANGE == 1) || (TSS_TSIL_PS_RANGE == 1))
        uint8_t TSIL_Recalibrate(TSS_TSIL_MemMapPtr psModuleStruct)
        {
          uint8_t u8Result = TSS_INIT_STATUS_OK;

          psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_TSIEN_MASK);     /* Turn OFF TSI Module */
          psModuleStruct->CS1 &= ~((uint8_t)TSI_CS1_PS_MASK);        /* Erase PS */
          psModuleStruct->CS1 |= TSIL_CS1_PS_FIT(TSS_TSIL_PS_LOW_LIMIT); /* Set Prescaler for Electrode OSC - Set by the user 0 = divide 1, 7 = divide 128 */
          psModuleStruct->CS2 &= ~((uint8_t)TSI_CS2_EXTCHRG_MASK);   /* Erase ExtChrg */
          psModuleStruct->CS2 |= TSIL_CS2_EXTCHRG_FIT(TSS_TSIL_EXTCHRG_LOW_LIMIT); /* External OSC Charge Current 0= 500nA, 7 = 64uA */

          tsi_au16Resolution = TSS_TSIL_RESOLUTION_VALUE; /* Store the resolution value */

          return u8Result;
        }
      #else
        #ifdef TSS_ONPROXIMITY_CALLBACK
          uint8_t TSIL_Recalibrate(TSS_TSIL_MemMapPtr psModuleStruct)
          {
            TSIL_CALIBRATION FinalTSICalibration;
            TSIL_CALIBRATION FinalTSICalibrationBelow;
            TSIL_CALIBRATION TempTSICalibration;
            TSIL_CALIBRATION OldTSICalibration;
            uint8_t u8Result = TSS_INIT_STATUS_OK;
            uint8_t u8ElCounter;
            uint8_t u8TSIPSTemp;
            uint8_t u8TSIExtChrgLowTemp;
            uint8_t u8TSIExtChrgTemp;
            uint8_t u8CalibrationComplete;
            uint16_t u16CapSampleTemp;
            uint8_t u8Iteration;
            uint16_t u16PENtemp;
            uint16_t u16CapSampleAverage;
            uint8_t u8FinalTSICalibrationFound;
            uint16_t u16ResolutionValue;
            uint8_t u8EXTCHRGRange;
            uint8_t u8PSHighLimit;
            uint8_t u8Found;

            /* Save previous TSI Calibration */
            OldTSICalibration.u8TSIPS = (uint8_t) ((psModuleStruct->CS1 & TSI_CS1_PS_MASK) >> TSI_CS1_PS_BITNUM);
            OldTSICalibration.u8TSIExtChrg = (uint8_t) ((psModuleStruct->CS2 & TSI_CS2_EXTCHRG_MASK) >> TSI_CS2_EXTCHRG_BITNUM);
            /* Set Variables */
            if (tss_CSSys.SystemConfig.ProximityEn)
            {
              u16ResolutionValue = TSS_TSIL_PROX_RESOLUTION_VALUE;
              u8EXTCHRGRange = TSS_TSIL_PROX_EXTCHRG_RANGE;
              u8PSHighLimit = TSS_TSIL_PROX_PS_HIGH_LIMIT;
              u8TSIPSTemp = TSS_TSIL_PROX_PS_LOW_LIMIT;
              u8TSIExtChrgLowTemp = TSS_TSIL_PROX_EXTCHRG_LOW_LIMIT;
            }
            else
            {
              u16ResolutionValue = TSS_TSIL_RESOLUTION_VALUE;
              u8EXTCHRGRange = TSS_TSIL_EXTCHRG_RANGE;
              u8PSHighLimit = TSS_TSIL_PS_HIGH_LIMIT;
              u8TSIPSTemp = TSS_TSIL_PS_LOW_LIMIT;
              u8TSIExtChrgLowTemp = TSS_TSIL_EXTCHRG_LOW_LIMIT;
            }
            /* Do Autocalibration */
            FinalTSICalibration.u16Distance = 0xFFFFu;
            FinalTSICalibrationBelow.u16Distance = 0xFFFFu;
            u8FinalTSICalibrationFound = 0u;
            /* Set TSI registers for Single measurement */
            psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_TSIEN_MASK);  /* Turn OFF TSI Module */
            psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_TSIIEN_MASK); /* Disable Interrupts */
            psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_STM_MASK);    /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
            psModuleStruct->CS0 |= TSI_CS0_TSIEN_MASK;            /* Turn On TSI Module */
            /* Read enabled pins */
            u16PENtemp = ((((uint16_t) psModuleStruct->PEN.Byte[1]) << 8u) | ((uint16_t)psModuleStruct->PEN.Byte[0]));

            do
            {
              u8TSIExtChrgTemp = u8TSIExtChrgLowTemp + (u8EXTCHRGRange >> 1u);
              u8Iteration = 0u;
              TempTSICalibration.u16Distance = 0xFFFFu;
              u8Found = 0u;
              do
              {
                /* Increase iteration level */
                u8Iteration += 1u;
                /* Check if all electrodes has proper Counter value */
                u8CalibrationComplete = 1u;
                u16CapSampleAverage = 0u;
                /* Go through all enabled pins */
                for(u8ElCounter = 0u; u8ElCounter < 16u; u8ElCounter++)
                {
                  if (u16PENtemp & (1u << u8ElCounter))
                  {
                    /* Configure TSIL */
                    psModuleStruct->CS1 &= ~((uint8_t)TSI_CS1_PS_MASK);      /* Erase PS */
                    psModuleStruct->CS1 |= TSIL_CS1_PS_FIT(u8TSIPSTemp);    /* Set Prescaler for Electrode OSC - Set by the user 0 = divide 1, 7 = divide 128 */
                    psModuleStruct->CS2 &= ~((uint8_t)TSI_CS2_EXTCHRG_MASK); /* Erase ExtChrg */
                    psModuleStruct->CS2 |= TSIL_CS2_EXTCHRG_FIT(u8TSIExtChrgTemp); /* External OSC Charge Current 0= 500nA, 7 = 64uA */
                    /* Start Measurement */
                    psModuleStruct->CS3 &= ~((uint8_t)TSI_CS3_TSICH_MASK);   /* Erase TSICH */
                    psModuleStruct->CS3 = TSIL_CS3_TSICH_FIT(u8ElCounter);  /* Set Channel */
                    psModuleStruct->CS0 |= TSI_CS0_SWTS_MASK;              /* Toggle SW trigger */
                    #ifdef _TSS_TEST
                      _TSS_TSIGEN_ON;                                      /* Start generator for test purpose */
                    #endif
                    do
                    {
                    /* Measurement Running */
                    } while ((psModuleStruct->CS0 & TSI_CS0_EOSF_MASK) == 0u); /* If TSI End of Scan Flag is reported*/
                    #ifdef _TSS_TEST
                      _TSS_TSIGEN_OFF;  /* Stop generator */
                    #endif
                    /* Reset EOSF Flag */
                    psModuleStruct->CS0 |= TSI_CS0_EOSF_MASK;  /* Toggle EOSF flag */
                    /* Wait for data ready */
                    #if defined(__ARMCC_VERSION)
                      __nop(); __nop();
                    #elif defined(__COSMIC_COMPILER)
                      _asm ("NOP\n"); _asm("NOP\n");
                    #else
                      asm ("NOP"); asm ("NOP");
                    #endif
                    /* Read TSI Counter */
                    u16CapSampleTemp = psModuleStruct->CNT.Word;
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
                  u16CapSampleTemp = u16CapSampleAverage - u16ResolutionValue;
                }
                else
                {
                  u16CapSampleTemp = u16ResolutionValue - u16CapSampleAverage;
                }
                /* All samples > resolution AND (Average > resolution OR already found and tune it) */
                if ((u8CalibrationComplete == 1u ) && ((u16CapSampleAverage > u16ResolutionValue) || (u8Found && (TempTSICalibration.u16Distance >= u16CapSampleTemp))))
                {
                  TempTSICalibration.u8TSIExtChrg = u8TSIExtChrgTemp;
                  TempTSICalibration.u8TSIPS = u8TSIPSTemp;
                  TempTSICalibration.u16Distance = u16CapSampleTemp;
                  u8Found++; /* Match for set PS found */
                }
                else if (TempTSICalibration.u16Distance >= u16CapSampleTemp && (u8Found ==0))
                {
                  TempTSICalibration.u8TSIExtChrg = u8TSIExtChrgTemp;
                  TempTSICalibration.u8TSIPS = u8TSIPSTemp;
                  TempTSICalibration.u16Distance = u16CapSampleTemp;
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
              /* The value only if a distance is lower and was found with sufficient resolution */
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
              tsi_au16Resolution = u16ResolutionValue;  /* Store the resolution value */
            }
            else
            {
              FinalTSICalibration.u8TSIPS = FinalTSICalibrationBelow.u8TSIPS;
              FinalTSICalibration.u8TSIExtChrg = FinalTSICalibrationBelow.u8TSIExtChrg;
              FinalTSICalibration.u16Distance = FinalTSICalibrationBelow.u16Distance;
              /* Reuse of u8Iteration & u16CapSampleTemp */
              u8Iteration = 0u;
              u16CapSampleTemp = u16ResolutionValue - FinalTSICalibrationBelow.u16Distance;
              /* Determine TSS Resolution (highest bit set) */
              while (u16CapSampleTemp >> u8Iteration)
              {
                u8Iteration++;
              }
              /* Store the present resolution value */
              tsi_au16Resolution = (1u << (uint8_t)(u8Iteration - 1u));
            }
            /* Set Final Found values */
            psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_TSIEN_MASK);     /* Turn OFF TSI Module */
            psModuleStruct->CS1 &= ~((uint8_t)TSI_CS1_PS_MASK);        /* Erase PS */
            psModuleStruct->CS1 |= TSIL_CS1_PS_FIT(FinalTSICalibration.u8TSIPS); /* Set Prescaler for Electrode OSC - Set by the user 0 = divide 1, 7 = divide 128 */
            psModuleStruct->CS2 &= ~((uint8_t)TSI_CS2_EXTCHRG_MASK);   /* Erase ExtChrg */
            psModuleStruct->CS2 |= TSIL_CS2_EXTCHRG_FIT(FinalTSICalibration.u8TSIExtChrg); /* External OSC Charge Current 0= 500nA, 7 = 64uA */
            /* Save Value for Proximity function */
            if (tss_CSSys.SystemConfig.ProximityEn)
            {
              tss_u16CapSample = FinalTSICalibration.u16Distance + u16ResolutionValue;
            }
            /* Indication of changed configuration */
            if ((OldTSICalibration.u8TSIPS != FinalTSICalibration.u8TSIPS) || (OldTSICalibration.u8TSIExtChrg != FinalTSICalibration.u8TSIExtChrg))
            {
              u8Result |= TSS_INIT_STATUS_CALIBRATION_CHANGED;
            }

            return u8Result;
          }
        #else
          uint8_t TSIL_Recalibrate(TSS_TSIL_MemMapPtr psModuleStruct)
          {
            TSIL_CALIBRATION FinalTSICalibration;
            TSIL_CALIBRATION FinalTSICalibrationBelow;
            TSIL_CALIBRATION TempTSICalibration;
            TSIL_CALIBRATION OldTSICalibration;
            uint8_t u8Result = TSS_INIT_STATUS_OK;
            uint8_t u8ElCounter;
            uint8_t u8TSIPSTemp;
            uint8_t u8TSIExtChrgTemp;
            uint8_t u8CalibrationComplete;
            uint16_t u16CapSampleTemp;
            uint8_t u8Iteration;
            uint16_t u16PENtemp;
            uint16_t u16CapSampleAverage;
            uint8_t u8FinalTSICalibrationFound;
            uint8_t u8Found;

            /* Save previous TSI Calibration */
            OldTSICalibration.u8TSIPS = (uint8_t) ((psModuleStruct->CS1 & TSI_CS1_PS_MASK) >> TSI_CS1_PS_BITNUM);
            OldTSICalibration.u8TSIExtChrg = (uint8_t) ((psModuleStruct->CS2 & TSI_CS2_EXTCHRG_MASK) >> TSI_CS2_EXTCHRG_BITNUM);
            /* Do Autocalibration */
            u8TSIPSTemp = TSS_TSIL_PS_LOW_LIMIT;
            FinalTSICalibration.u16Distance = 0xFFFFu;
            FinalTSICalibrationBelow.u16Distance = 0xFFFFu;
            u8FinalTSICalibrationFound = 0u;
            /* Set TSI registers for Single measurement */
            psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_TSIEN_MASK);  /* Turn OFF TSI Module */
            psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_TSIIEN_MASK); /* Disable Interrupts */
            psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_STM_MASK);    /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
            psModuleStruct->CS0 |= TSI_CS0_TSIEN_MASK;            /* Turn On TSI Module */
            /* Read enabled pins */
            u16PENtemp = ((((uint16_t) psModuleStruct->PEN.Byte[1]) << 8u) | ((uint16_t)psModuleStruct->PEN.Byte[0]));

            do
            {
              u8TSIExtChrgTemp = TSS_TSIL_EXTCHRG_LOW_LIMIT + (TSS_TSIL_EXTCHRG_RANGE / 2u);
              u8Iteration = 0u;
              TempTSICalibration.u16Distance = 0xFFFFu;
              u8Found = 0u;
              do
              {
                /* Increase iteration level */
                u8Iteration += 1u;
                /* Check if all electrodes has proper Counter value */
                u8CalibrationComplete = 1u;
                u16CapSampleAverage = 0u;
                /* Go through all enabled pins */
                for(u8ElCounter = 0u; u8ElCounter < 16u; u8ElCounter++)
                {
                  if (u16PENtemp & (1u << u8ElCounter))
                  {
                    /* Configure TSIL */
                    psModuleStruct->CS1 &= ~((uint8_t)TSI_CS1_PS_MASK);      /* Erase PS */
                    psModuleStruct->CS1 |= TSIL_CS1_PS_FIT(u8TSIPSTemp);    /* Set Prescaler for Electrode OSC - Set by the user 0 = divide 1, 7 = divide 128 */
                    psModuleStruct->CS2 &= ~((uint8_t)TSI_CS2_EXTCHRG_MASK); /* Erase ExtChrg */
                    psModuleStruct->CS2 |= TSIL_CS2_EXTCHRG_FIT(u8TSIExtChrgTemp); /* External OSC Charge Current 0= 500nA, 7 = 64uA */
                    /* Start Measurement */
                    psModuleStruct->CS3 &= ~((uint8_t)TSI_CS3_TSICH_MASK);   /* Erase TSICH */
                    psModuleStruct->CS3 = TSIL_CS3_TSICH_FIT(u8ElCounter);  /* Set Channel */
                    psModuleStruct->CS0 |= TSI_CS0_SWTS_MASK;              /* Toggle SW trigger */
                    #ifdef _TSS_TEST
                      _TSS_TSIGEN_ON;  /* Start generator for test purpose */
                    #endif
                    do
                    {
                    /* Measurement Running */
                    } while ((psModuleStruct->CS0 & TSI_CS0_EOSF_MASK) == 0u); /* If TSI End of Scan Flag is reported*/
                    #ifdef _TSS_TEST
                      _TSS_TSIGEN_OFF;  /* Stop generator */
                    #endif
                    /* Reset EOSF Flag */
                    psModuleStruct->CS0 |= TSI_CS0_EOSF_MASK;  /* Toggle EOSF flag */
                    /* Wait for data ready */
                    #if defined(__ARMCC_VERSION)
                      __nop(); __nop();
                    #elif defined(__COSMIC_COMPILER)
                      _asm ("NOP\n"); _asm("NOP\n");
                    #else
                      asm ("NOP"); asm ("NOP");
                    #endif
                    /* Read TSI Counter */
                    u16CapSampleTemp = psModuleStruct->CNT.Word;
                    if (u16CapSampleTemp < TSS_TSIL_RESOLUTION_VALUE)
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
                if(u16CapSampleAverage > TSS_TSIL_RESOLUTION_VALUE )
                {
                  u16CapSampleTemp = u16CapSampleAverage - TSS_TSIL_RESOLUTION_VALUE;
                }
                else
                {
                  u16CapSampleTemp = TSS_TSIL_RESOLUTION_VALUE - u16CapSampleAverage;
                }
                /* All samples > resolution AND (Average > resolution OR already found and tune it) */
                if ((u8CalibrationComplete == 1u ) && ((u16CapSampleAverage > TSS_TSIL_RESOLUTION_VALUE) || (u8Found && (TempTSICalibration.u16Distance >= u16CapSampleTemp))))
                {
                  TempTSICalibration.u8TSIExtChrg = u8TSIExtChrgTemp;
                  TempTSICalibration.u8TSIPS = u8TSIPSTemp;
                  TempTSICalibration.u16Distance = u16CapSampleTemp;
                  u8Found++; /* Match for set PS found */
                }
                else if ((TempTSICalibration.u16Distance >= u16CapSampleTemp) && (u8Found ==0))
                {
                  TempTSICalibration.u8TSIExtChrg = u8TSIExtChrgTemp;
                  TempTSICalibration.u8TSIPS = u8TSIPSTemp;
                  TempTSICalibration.u16Distance = u16CapSampleTemp;
                }
                /* Change ExtCharge within the interval */
                if(u8CalibrationComplete == 1u)
                {
                  u8TSIExtChrgTemp += ((TSS_TSIL_EXTCHRG_RANGE / 2u) >> u8Iteration);
                }
                else
                {
                  u8TSIExtChrgTemp -= ((TSS_TSIL_EXTCHRG_RANGE / 2u) >> u8Iteration);
                }
              } while (((TSS_TSIL_EXTCHRG_RANGE / 2u) >> u8Iteration) > 0u);
              /* The value only if a distance is lower and was found with sufficient resolution */
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
            } while (u8TSIPSTemp < (TSS_TSIL_PS_HIGH_LIMIT + 1u));
            /* Check if searching was succesful */
            if (u8FinalTSICalibrationFound)
            {
              tsi_au16Resolution = TSS_TSIL_RESOLUTION_VALUE; /* Store the resolution value */
            }
            else
            {
              FinalTSICalibration.u8TSIPS = FinalTSICalibrationBelow.u8TSIPS;
              FinalTSICalibration.u8TSIExtChrg = FinalTSICalibrationBelow.u8TSIExtChrg;
              FinalTSICalibration.u16Distance = FinalTSICalibrationBelow.u16Distance;
              /* Reuse of u8Iteration & u16CapSampleTemp */
              u8Iteration = 0u;
              u16CapSampleTemp = TSS_TSIL_RESOLUTION_VALUE - FinalTSICalibrationBelow.u16Distance;
              /* Determine TSS Resolution (highest bit set) */
              while (u16CapSampleTemp >> u8Iteration)
              {
                u8Iteration++;
              }
              /* Store the present resolution value */
              tsi_au16Resolution = (1u << (uint8_t) (u8Iteration - 1u));
            }
            /* Set Final Found values */
            psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_TSIEN_MASK);     /* Turn OFF TSI Module */
            psModuleStruct->CS1 &= ~((uint8_t)TSI_CS1_PS_MASK);        /* Erase PS */
            psModuleStruct->CS1 |= TSIL_CS1_PS_FIT(FinalTSICalibration.u8TSIPS); /* Set Prescaler for Electrode OSC - Set by the user 0 = divide 1, 7 = divide 128 */
            psModuleStruct->CS2 &= ~((uint8_t)TSI_CS2_EXTCHRG_MASK);   /* Erase ExtChrg */
            psModuleStruct->CS2 |= TSIL_CS2_EXTCHRG_FIT(FinalTSICalibration.u8TSIExtChrg); /* External OSC Charge Current 0= 500nA, 7 = 64uA */
            /* Indication of changed configuration */
            if ((OldTSICalibration.u8TSIPS != FinalTSICalibration.u8TSIPS) || (OldTSICalibration.u8TSIExtChrg != FinalTSICalibration.u8TSIExtChrg))
            {
              u8Result |= TSS_INIT_STATUS_CALIBRATION_CHANGED;
            }

            return u8Result;
          }
        #endif
      #endif

      /***************************************************************************//*!
      *
      * @brief  Control function for TSIL Trigger Settings
      *
      * @param  *psMethodROMDataStruct - Specificcation of ROMDATA Struct
      *         psModuleStruct - Specification of TSIL module
      *         *psMethodRAMDataStruct - Specification of RAMDATA Struct
      *
      * @return Status Code
      *
      * @remarks
      *
      ****************************************************************************/
      uint8_t TSIL_InitTrigger(TSS_TSIL_MemMapPtr psModuleStruct, TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct)
      {
        uint8_t u8Result = TSS_INIT_STATUS_OK;

        psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_TSIEN_MASK);  /* Turn OFF TSI Module */
        /* Setup TSI */
        if (tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_AUTO)
        {
          psModuleStruct->CS0 |= TSI_CS0_STM_MASK;  /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
        }
        else /* Always & SW Mode */
        {
          psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_STM_MASK);  /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
        }
        psModuleStruct->CS0 |= TSI_CS0_TSIIEN_MASK;  /* TSI Interrupt Enable (0 = disabled, 1 = enabled) */
        psMethodRAMDataStruct->tsil_eEndOfScanState = TSIL_EOSF_STATE_NONE;
        psMethodRAMDataStruct->tsil_eScanInProgressState = TSIL_SCNIP_STATE_NONE;

        u8Result |= TSS_INIT_STATUS_TRIGGER_SET;

        return u8Result;
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
      uint8_t TSIL_MethodControl(uint8_t u8ElNum, uint8_t u8Command)
      {
        uint8_t u8Result = TSS_INIT_STATUS_OK;
        TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct;
        TSS_TSIL_ELECTRODE_ROMDATA *psElectrodeROMDataStruct;
        TSS_TSIL_MemMapPtr psModuleStruct;
        TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct;

        /* Pointers Decoding */
        psElectrodeROMDataStruct = (TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElNum]);
        psMethodROMDataStruct = (TSS_TSIL_METHOD_ROMDATA *) (psElectrodeROMDataStruct->tsil_cpsMethodROMData);
        psModuleStruct = (TSS_TSIL_MemMapPtr) (psMethodROMDataStruct->tsil_cpsModule);
        psMethodRAMDataStruct = (TSS_TSIL_METHOD_RAMDATA *) (psElectrodeROMDataStruct->tsil_cpsMethodRAMData);

        switch (u8Command)
        {
          /************* SWTrigger ************/
          case TSS_INIT_COMMAND_SW_TRIGGER:
            if (psMethodRAMDataStruct->tsil_eScanInProgressState == TSIL_SCNIP_STATE_WAIT)
            {
              psMethodRAMDataStruct->tsil_eEndOfScanState = TSIL_EOSF_STATE_NONE; /* Reset EndOfScanState */
              psMethodRAMDataStruct->tsil_eScanInProgressState = TSIL_SCNIP_STATE_PROCESS; /* Set processing */
              psModuleStruct->CS0 |= TSI_CS0_SWTS_MASK; /* Toggle SW Trigger */
              u8Result |= TSS_INIT_STATUS_TRIGGER_SET;
              #ifdef _TSS_TEST
                _TSS_TSIGEN_ON;                         /* Start generator for test purpose */
              #endif
            }
          break;
          /************* Do TSIL Init *******************/
          case TSS_INIT_COMMAND_INIT_MODULES:
            u8Result |= TSIL_InitModule(psModuleStruct);
            u8Result |= TSIL_SetNSamples(psModuleStruct);
            u8Result |= TSIL_InitPEN(psMethodROMDataStruct, psModuleStruct);
            u8Result |= TSIL_Recalibrate(psModuleStruct);
            u8Result |= TSIL_InitTrigger(psModuleStruct, psMethodRAMDataStruct);
          break;
          /************* Set NSamples ***************/
          case TSS_INIT_COMMAND_SET_NSAMPLES:
            u8Result |= TSIL_SetNSamples(psModuleStruct);
            u8Result |= TSIL_Recalibrate(psModuleStruct);
            u8Result |= TSIL_InitTrigger(psModuleStruct, psMethodRAMDataStruct);
          break;
          /***** Do Auto calibration setting of TSIL EXTCHRG and TSIL PS *******/
          case TSS_INIT_COMMAND_RECALIBRATE:
            u8Result |= TSIL_Recalibrate(psModuleStruct);
            u8Result |= TSIL_InitTrigger(psModuleStruct, psMethodRAMDataStruct);
          break;
          /************ Electrode Enablers *************/
          case TSS_INIT_COMMAND_ENABLE_ELECTRODES:
            u8Result |= TSIL_InitPEN(psMethodROMDataStruct, psModuleStruct);
          break;
          /************* Triggering Init ************/
          case TSS_INIT_COMMAND_INIT_TRIGGER:
            u8Result |= TSIL_InitTrigger(psModuleStruct, psMethodRAMDataStruct);
          break;
        }

        /* Exit */
        return u8Result;
      }

      /***************************************************************************//*!
      *
      * @brief  Reads TSIL capacitance counter and returns a status code
      *
      * @param  *psElectrodeROMDataStruct - Specificcation of electrode ROMDATA Struct
      *         psModuleStruct - Specification of TSIL module
      *
      * @return Status Code
      *
      * @remarks
      *
      ****************************************************************************/
      uint8_t TSIL_ReadCounter(TSS_TSIL_ELECTRODE_ROMDATA *psElectrodeROMDataStruct, TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct)
      {
        uint8_t u8Result;

        /* Read TSIL Counter */
        tss_u16CapSample = ((uint16_t *) (psMethodROMDataStruct->tsil_cpu16ChannelCounterBuffer))[psElectrodeROMDataStruct->tsil_cu8ChannelNum];
        /* Evaluation of the Measured Value */
        if (tss_u16CapSample > TSS_TSIL_CAP_HIGH_LIMIT(tsi_au16Resolution))
        {
          u8Result = TSS_SAMPLE_ERROR_CHARGE_TIMEOUT;    /* Too High Capacity is detected */
        }
        else if (tss_u16CapSample > TSS_TSIL_RECALIBRATION_HIGH_THRESHOLD(tsi_au16Resolution))
        {
          u8Result = TSS_SAMPLE_RECALIB_REQUEST_HICAP;   /* Re-calibration is needed */
        }
        else if (tss_u16CapSample < TSS_TSIL_CAP_LOW_LIMIT(tsi_au16Resolution))
        {
          u8Result = TSS_SAMPLE_ERROR_SMALL_CAP;         /* Too Low Capacity is detected */
        }
        else if (tss_u16CapSample < TSS_TSIL_RECALIBRATION_LOW_THRESHOLD(tsi_au16Resolution))
        {
          u8Result = TSS_SAMPLE_RECALIB_REQUEST_LOCAP;   /* Re-calibration is needed */
        }
        else
        {
          u8Result = TSS_SAMPLE_STATUS_OK;
        }

        return u8Result;
      }

      /***************************************************************************//*!
      *
      * @brief  Performs and evalautes measurement
      *
      * @param  *psMethodROMDataStruct - Specification of ROMDATA Struct
      *         *psMethodRAMDataStruct - Specification of RAMDATA Struct
      *         psModuleStruct - Specification of TSIL module
      *
      * @return Status Code
      *
      * @remarks
      *
      ****************************************************************************/
      uint8_t TSIL_HandleMeasurement(TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct, TSS_TSIL_MemMapPtr psModuleStruct, uint8_t u8ElChannel)
      {
        uint8_t u8Result;

        /* Establish measurement */
        if (tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_ALWAYS)
        {
          if (psMethodRAMDataStruct->tsil_eEndOfScanState != TSIL_EOSF_STATE_REGULAR)
          {
            /* Init measurement */
            psMethodRAMDataStruct->tsil_u8StartElectrode = u8ElChannel;
            /* Start First Measurement */
            psModuleStruct->CS3 &= ~((uint8_t)TSI_CS3_TSICH_MASK);  /* Erase TSICH */
            psModuleStruct->CS3 = TSIL_CS3_TSICH_FIT(u8ElChannel);  /* Set Channel */
            /* Set Status Flags */
            psMethodRAMDataStruct->tsil_eEndOfScanState = TSIL_EOSF_STATE_NONE; /* Reset EndOfScanState */
            psMethodRAMDataStruct->tsil_eScanInProgressState = TSIL_SCNIP_STATE_PROCESS; /* Set processing */
            psModuleStruct->CS0 |= TSI_CS0_SWTS_MASK;    /* Toggle SW trigger */
          }
          u8Result = TSS_SAMPLE_STATUS_PROCESSING;
        }
        else /* If AUTO & SW Mode */
        {
          /* Init measurement */
          psMethodRAMDataStruct->tsil_u8StartElectrode = u8ElChannel;
          psMethodRAMDataStruct->tsil_eScanInProgressState = TSIL_SCNIP_STATE_WAIT;
          /* Start First Measurement */
          psModuleStruct->CS3 &= ~((uint8_t)TSI_CS3_TSICH_MASK);   /* Erase TSICH */
          psModuleStruct->CS3 = TSIL_CS3_TSICH_FIT(u8ElChannel);  /* Set Channel */
          /* Handle measuremnt status */
          if (tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_AUTO)
          {
            /* Evaluate Auto Trigger period */
            if (psMethodRAMDataStruct->tsil_eEndOfScanState == TSIL_EOSF_STATE_ERROR)
            {
              u8Result = TSS_SAMPLE_ERROR_SMALL_TRIGGER_PERIOD;  /* Small TriggerModuloValue */
              psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_TSIEN_MASK);  /* Disable whole TSS due triggering crash */
              #ifdef _TSS_TEST
                 _TSS_TSIGEN_OFF;  /* Stop generator for test purpose */
              #endif
            }
            else
            {
              u8Result = TSS_SAMPLE_STATUS_PROCESSING;
            }
            psMethodRAMDataStruct->tsil_eEndOfScanState = TSIL_EOSF_STATE_NONE; /* Reset EndOfScanState */
          }
          else
          {
            u8Result = TSS_SAMPLE_STATUS_PROCESSING;
          }
        }
        #ifdef _TSS_TEST
          _TSS_TSIGEN_ON;  /* Start generator for test purpose */
        #endif

        return u8Result;
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
      uint8_t TSIL_SampleElectrode(uint8_t u8ElecNum, uint8_t u8Command)
      {
        uint8_t u8ElectrodeStatus;
        uint8_t u8ElCounter;
        uint8_t u8ElChannel;

        TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct;
        TSS_TSIL_ELECTRODE_ROMDATA *psElectrodeROMDataStruct;
        TSS_TSIL_MemMapPtr psModuleStruct;
        TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct;

        /* Pointers decoding */
        psElectrodeROMDataStruct = (TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElecNum]);
        psMethodROMDataStruct = (TSS_TSIL_METHOD_ROMDATA *) (psElectrodeROMDataStruct->tsil_cpsMethodROMData);
        psModuleStruct = (TSS_TSIL_MemMapPtr) (psMethodROMDataStruct->tsil_cpsModule);
        psMethodRAMDataStruct = (TSS_TSIL_METHOD_RAMDATA *) (psElectrodeROMDataStruct->tsil_cpsMethodRAMData);
        u8ElChannel = psElectrodeROMDataStruct->tsil_cu8ChannelNum;

        /* Module Control */
        switch (u8Command)
        {
          case TSS_SAMPLE_COMMAND_RESTART:
            /* If TSIL is disabled then enable */
            if ((psModuleStruct->CS0 & TSI_CS0_TSIEN_MASK) == 0u)
            {
              psModuleStruct->CS0 |= TSI_CS0_TSIEN_MASK;     /* Turn ON TSI Module */
            }
            /* Handle Measurement */
            if (psMethodRAMDataStruct->tsil_eScanInProgressState == TSIL_SCNIP_STATE_NONE)
            {
              u8ElectrodeStatus = TSIL_HandleMeasurement(psMethodRAMDataStruct, psModuleStruct, u8ElChannel);
            }
            else
            {
              u8ElectrodeStatus = TSS_SAMPLE_STATUS_PROCESSING;
            }
          break;
          case TSS_SAMPLE_COMMAND_PROCESS:
            /* If TSIL is disabled then enable */
            if ((psModuleStruct->CS0 & TSI_CS0_TSIEN_MASK) == 0u)
            {
              psModuleStruct->CS0 |= TSI_CS0_TSIEN_MASK;     /* Turn ON TSI Module */
            }
            /* Handle Auto Trigger Mode */
            if (psMethodRAMDataStruct->tsil_eScanInProgressState == TSIL_SCNIP_STATE_NONE) /* TSIL scanning is not in progress */
            {
              /* If TSI End of Scan Flag is reported */
              if (psMethodRAMDataStruct->tsil_eEndOfScanState >= TSIL_EOSF_STATE_REGULAR)
              {
                psMethodRAMDataStruct->tsil_eEndOfScanState = TSIL_EOSF_STATE_PROCESS;
                #ifdef _TSS_TEST
                  _TSS_TSIGEN_OFF;                                    /* Stop generator */
                #endif
                /* Read TSI Counter */
                u8ElectrodeStatus = TSIL_ReadCounter(psElectrodeROMDataStruct, psMethodROMDataStruct);
              }
              else
              {
                u8ElectrodeStatus = TSIL_HandleMeasurement(psMethodRAMDataStruct, psModuleStruct, u8ElChannel);
              }
            }
            else
            {
              u8ElectrodeStatus = TSS_SAMPLE_STATUS_PROCESSING;
            }
          break;
          case TSS_SAMPLE_COMMAND_GET_NEXT_ELECTRODE:
            /* Find the next electrode within the electrode module */
            u8ElCounter = u8ElecNum;
            #if TSS_N_ELECTRODES > 1
              do
              {
                if (u8ElCounter < (TSS_N_ELECTRODES-1))
                {
                  u8ElCounter++;
                }
                else
                {
                  u8ElCounter = 0u;
                }
              } while (psMethodROMDataStruct != (TSS_TSIL_METHOD_ROMDATA *) (((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsil_cpsMethodROMData));
            #endif
            /* Return found electrode number */
            u8ElectrodeStatus = u8ElCounter;
          break;
          case TSS_SAMPLE_COMMAND_RECALIB:
            u8ElectrodeStatus = TSIL_MethodControl(u8ElecNum, TSS_INIT_COMMAND_RECALIBRATE);

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
            u8ElectrodeStatus = TSIL_MethodControl(u8ElecNum, TSS_INIT_COMMAND_ENABLE_ELECTRODES);

            u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
          break;
          case TSS_SAMPLE_COMMAND_SET_LOWLEVEL_CONFIG:
            psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_TSIEN_MASK);     /* Turn OFF TSI Module */
            psModuleStruct->CS1 &= ~((uint8_t)TSI_CS1_PS_MASK);        /* Erase PS */
            psModuleStruct->CS1 |= TSIL_CS1_PS_FIT(tss_u16CapSample & 0x7u); /* Set Prescaler for Electrode OSC - Set by the user 0 = divide 1, 7 = divide 128 */
            psModuleStruct->CS2 &= ~((uint8_t)TSI_CS2_EXTCHRG_MASK);   /* Erase ExtChrg */
            psModuleStruct->CS2 |= TSIL_CS2_EXTCHRG_FIT((tss_u16CapSample >> 3u) & 0x7u); /* External OSC Charge Current 0= 500nA, 7 = 64uA */

            u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
          break;
          case TSS_SAMPLE_COMMAND_GET_LOWLEVEL_CONFIG:
            u8ElectrodeStatus = (uint8_t) ((psModuleStruct->CS1 & TSI_CS1_PS_MASK) >> TSI_CS1_PS_BITNUM);
            tss_u16CapSample = (uint16_t) ((((psModuleStruct->CS2 & TSI_CS2_EXTCHRG_MASK) >> TSI_CS2_EXTCHRG_BITNUM) << 3u) | u8ElectrodeStatus);

            u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
          break;
        }

        return u8ElectrodeStatus;  /* Return status code */
      }

      /***************************************************************************//*!
      *
      * @brief  TSIL ISR routines
      *
      * @remarks
      *
      ****************************************************************************/
      #if TSS_DETECT_MODULE(TSIL)
        #if defined(__COSMIC_COMPILER)
          @interrupt
        #else
          interrupt
          #if !TSS_USE_PE_COMPONENT
            VectorNumber_Vtsi
          #endif
        #endif
        void TSS_TSIIsr(void)
        {
          TSS_TSIL_MemMapPtr psModuleStruct;
          uint8_t u8Channel;
          uint16_t u16PEN;
          uint8_t u8Timeout;

          /* Pointers decoding */
          psModuleStruct = (TSS_TSIL_MemMapPtr) (TSS_TSIL_METHOD_ROMDATA_CONTEXT.tsil_cpsModule);

          /* Clear EOSF flag */
          psModuleStruct->CS0 |= TSI_CS0_EOSF_MASK;

          if (TSS_TSIL_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState == TSIL_EOSF_STATE_NONE)
          {
            /* Disable HW trigger */
            psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_STM_MASK);  /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
            /* Save Actual channel */
            u8Channel = ((psModuleStruct->CS3 & TSI_CS3_TSICH_MASK) >> TSI_CS3_TSICH_BITNUM);
            /* Read Counter */
            ((uint16_t *)(TSS_TSIL_METHOD_ROMDATA_CONTEXT.tsil_cpu16ChannelCounterBuffer))[u8Channel] = psModuleStruct->CNT.Word;
            /* Read PEN */
            u16PEN = ((((uint16_t) psModuleStruct->PEN.Byte[1]) << 8u) | ((uint16_t)psModuleStruct->PEN.Byte[0]));
            /* Find next electrode */
            u8Timeout = 18u;
            do
            {
              if (u8Channel < 15u)
              {
                u8Channel++;
              }
              else
              {
                u8Channel = 0u;
              }
              u8Timeout--;
            } while (((u16PEN & (uint16_t)(1u << u8Channel)) == 0u) && (u8Timeout > 0u));
            /* Evaluate */
            if (u8Channel == TSS_TSIL_METHOD_RAMDATA_CONTEXT.tsil_u8StartElectrode)
            {
              #ifdef _TSS_TEST
                _TSS_TSIGEN_OFF;  /* Stop generator */
              #endif
              TSS_TSIL_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState = TSIL_EOSF_STATE_REGULAR;
              TSS_TSIL_METHOD_RAMDATA_CONTEXT.tsil_eScanInProgressState = TSIL_SCNIP_STATE_NONE;
              if (tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_AUTO)
              {
                /* Enable HW trigger for period measurement in the case of AUTO trigger */
                psModuleStruct->CS0 |= TSI_CS0_STM_MASK;  /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
              }
            }
            else
            {
              psModuleStruct->CS3 &= ~((uint8_t)TSI_CS3_TSICH_MASK); /* Erase TSICH */
              psModuleStruct->CS3 = TSIL_CS3_TSICH_FIT(u8Channel); /* Set Channel */
              psModuleStruct->CS0 |= TSI_CS0_SWTS_MASK;            /* Toggle SW trigger */
            }
          }
          else
          {
            /* Indication of short autotrigger period */
            TSS_TSIL_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState = TSIL_EOSF_STATE_ERROR;
          }

          /* Set Sample Interrupted flag, because TSI measures at background and it can interrupt sampling of GPIO based methods */
          TSS_SET_SAMPLE_INTERRUPTED();
        }
      #endif

      #if TSS_DETECT_MODULE(TSIL0)
        #if defined(__COSMIC_COMPILER)
          @interrupt
        #else
          interrupt
          #if !TSS_USE_PE_COMPONENT
            VectorNumber_Vtsi0
          #endif
        #endif
        void TSS_TSI0Isr(void)
        {
          TSS_TSIL_MemMapPtr psModuleStruct;
          uint8_t u8Channel;
          uint16_t u16PEN;
          uint8_t u8Timeout;

          /* Pointers decoding */
          psModuleStruct = (TSS_TSIL_MemMapPtr) (TSS_TSIL0_METHOD_ROMDATA_CONTEXT.tsil_cpsModule);

          /* Clear EOSF flag */
          psModuleStruct->CS0 |= TSI_CS0_EOSF_MASK;

          if (TSS_TSIL0_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState == TSIL_EOSF_STATE_NONE)
          {
            /* Disable HW trigger */
            psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_STM_MASK);  /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
            /* Save Actual channel */
            u8Channel = ((psModuleStruct->CS3 & TSI_CS3_TSICH_MASK) >> TSI_CS3_TSICH_BITNUM);
            /* Read Counter */
            ((uint16_t *)(TSS_TSIL0_METHOD_ROMDATA_CONTEXT.tsil_cpu16ChannelCounterBuffer))[u8Channel] = psModuleStruct->CNT.Word;
            /* Read PEN */
            u16PEN = ((((uint16_t) psModuleStruct->PEN.Byte[1]) << 8u) | ((uint16_t)psModuleStruct->PEN.Byte[0]));
            /* Find next electrode */
            u8Timeout = 18u;
            do
            {
              if (u8Channel < 15u)
              {
                u8Channel++;
              }
              else
              {
                u8Channel = 0u;
              }
              u8Timeout--;
            } while (((u16PEN & (uint16_t)(1u << u8Channel)) == 0u) && (u8Timeout > 0u));
            /* Evaluate */
            if (u8Channel == TSS_TSIL0_METHOD_RAMDATA_CONTEXT.tsil_u8StartElectrode)
            {
              #ifdef _TSS_TEST
                _TSS_TSIGEN_OFF;  /* Stop generator */
              #endif
              TSS_TSIL0_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState = TSIL_EOSF_STATE_REGULAR;
              TSS_TSIL0_METHOD_RAMDATA_CONTEXT.tsil_eScanInProgressState = TSIL_SCNIP_STATE_NONE;
              if (tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_AUTO)
              {
                /* Enable HW trigger for period measurement in the case of AUTO trigger */
                psModuleStruct->CS0 |= TSI_CS0_STM_MASK;   /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
              }
            }
            else
            {
              psModuleStruct->CS3 &= ~((uint8_t)TSI_CS3_TSICH_MASK); /* Erase TSICH */
              psModuleStruct->CS3 = TSIL_CS3_TSICH_FIT(u8Channel); /* Set Channel */
              psModuleStruct->CS0 |= TSI_CS0_SWTS_MASK;  /* Toggle SW trigger */
            }
          }
          else
          {
            /* Indication of short autotrigger period */
            TSS_TSIL0_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState = TSIL_EOSF_STATE_ERROR;
          }
          /* Set Sample Interrupted flag, because TSIL measures at background and it can interrupt sampling of GPIO based methods */
          TSS_SET_SAMPLE_INTERRUPTED();
        }
      #endif

      #if TSS_DETECT_MODULE(TSIL1)
        #if defined(__COSMIC_COMPILER)
          @interrupt
        #else
          interrupt
          #if !TSS_USE_PE_COMPONENT
            VectorNumber_Vtsi1
          #endif
        #endif
        void TSS_TSI1Isr(void)
        {
          TSI_MemMapPtr psModuleStruct;
          uint8_t u8Channel;
          uint16_t u16PEN;
          uint8_t u8Timeout;

          /* Pointers decoding */
          psModuleStruct = (TSS_TSIL_MemMapPtr) (TSS_TSIL1_METHOD_ROMDATA_CONTEXT.tsil_cpsModule);

          /* Clear EOSF flag */
          psModuleStruct->CS0 |= TSI_CS0_EOSF_MASK;

          if (TSS_TSIL1_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState == TSIL_EOSF_STATE_NONE)
          {
            /* Disable HW trigger */
            psModuleStruct->CS0 &= ~((uint8_t)TSI_CS0_STM_MASK);  /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
            /* Save Actual channel */
            u8Channel = ((psModuleStruct->CS3 & TSI_CS3_TSICH_MASK) >> TSI_CS3_TSICH_BITNUM);
            /* Read Counter */
            ((uint16_t *)(TSS_TSIL1_METHOD_ROMDATA_CONTEXT.tsil_cpu16ChannelCounterBuffer))[u8Channel] = psModuleStruct->CNT.Word;
            /* Read PEN */
            u16PEN = ((((uint16_t) psModuleStruct->PEN.Byte[1]) << 8u) | ((uint16_t)psModuleStruct->PEN.Byte[0]));
            /* Find next electrode */
            u8Timeout = 18u;
            do
            {
              if (u8Channel < 15u)
              {
                u8Channel++;
              }
              else
              {
                u8Channel = 0u;
              }
              u8Timeout--;
            } while (((u16PEN & (uint16_t)(1u << u8Channel)) == 0u) && (u8Timeout > 0u));
            /* Evaluate */
            if (u8Channel == TSS_TSIL1_METHOD_RAMDATA_CONTEXT.tsil_u8StartElectrode)
            {
              #ifdef _TSS_TEST
                _TSS_TSIGEN_OFF;  /* Stop generator */
              #endif
              TSS_TSIL1_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState = TSIL_EOSF_STATE_REGULAR;
              TSS_TSIL1_METHOD_RAMDATA_CONTEXT.tsil_eScanInProgressState = TSIL_SCNIP_STATE_NONE;
              if (tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_AUTO)
              {
                /* Enable HW trigger for period measurement in the case of AUTO trigger */
                psModuleStruct->CS0 |= TSI_CS0_STM_MASK;  /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
              }
            }
            else
            {
              psModuleStruct->CS3 &= ~((uint8_t)TSI_CS3_TSICH_MASK);  /* Erase TSICH */
              psModuleStruct->CS3 = TSIL_CS3_TSICH_FIT(u8Channel);  /* Set Channel */
              psModuleStruct->CS0 |= TSI_CS0_SWTS_MASK;             /* Toggle SW trigger */
            }
          }
          else
          {
            /* Indication of short autotrigger period */
            TSS_TSIL1_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState = TSIL_EOSF_STATE_ERROR;
          }
          /* Set Sample Interrupted flag, because TSI measures at background and it can interrupt sampling of GPIO based methods */
          TSS_SET_SAMPLE_INTERRUPTED();
        }
      #endif
    #endif /* End of TSS_USE_SIMPLE_LOW_LEVEL == 0 */

  #elif TSS_KINETIS_MCU || TSS_CFM_MCU

    /************************ Prototypes **************************/

    uint8_t TSIL_MethodControl(uint8_t u8ElNum, uint8_t u8Command);

    /*******************  Modules definition **********************/
    /* CMSIS redefinition */
    #if !defined(TSI_BASE_PTR)
      #define TSI_BASE_PTR      TSI
    #endif
    #if !defined(TSI0_BASE_PTR)
      #define TSI0_BASE_PTR      TSI0
    #endif
    #if !defined(TSI1_BASE_PTR)
      #define TSI1_BASE_PTR     TSI1
    #endif
    #if defined(__CM0PLUS_CMSIS_VERSION_MAIN)
      #define INT_TSI TSI_IRQn+16
    #endif
    #if defined(__CM0PLUS_CMSIS_VERSION_MAIN)
      #define INT_TSI0 TSI0_IRQn+16
    #endif
    #if defined(__CM0PLUS_CMSIS_VERSION_MAIN)
      #define INT_TSI1 TSI1_IRQn+16
    #endif
    #if !defined(NVIC_BASE_PTR)
      #define NVIC_BASE_PTR   NVIC
      #define NVIC_MemMapPtr  NVIC_Type*
      #define ICPR            ICPR[0]
      #define ISER            ISER[0]
    #endif

    #if TSS_DETECT_MODULE(TSIL)
      uint16_t TSS_TSIL_CHANNEL_COUNTER_BUFFER[16];
      const TSS_TSIL_METHOD_ROMDATA TSS_TSIL_METHOD_ROMDATA_CONTEXT = {TSIL_MethodControl, (uint32_t*) TSI_BASE_PTR, TSS_TSIL_CHANNEL_COUNTER_BUFFER, INT_TSI-16, TSS_DETECT_LOWPOWER_USAGE(TSI)};
      #if (TSS_USE_NOISE_MODE == 1)
        volatile TSS_TSIL_METHOD_RAMDATA TSS_TSIL_METHOD_RAMDATA_CONTEXT = {TSIL_EOSF_STATE_NONE, 0u, TSIL_SCNIP_STATE_NONE, TSS_MODE_CAP, 0u};
      #else
       volatile TSS_TSIL_METHOD_RAMDATA TSS_TSIL_METHOD_RAMDATA_CONTEXT = {TSIL_EOSF_STATE_NONE, 0u, TSIL_SCNIP_STATE_NONE;
      #endif
    #endif
    #if TSS_DETECT_MODULE(TSIL0)
      uint16_t TSS_TSIL0_CHANNEL_COUNTER_BUFFER[16];
      const TSS_TSIL_METHOD_ROMDATA TSS_TSIL0_METHOD_ROMDATA_CONTEXT = {TSIL_MethodControl, (uint32_t*) TSI0_BASE_PTR, TSS_TSIL0_CHANNEL_COUNTER_BUFFER, INT_TSI0-16, TSS_DETECT_LOWPOWER_USAGE(TSI0)};
      #if (TSS_USE_NOISE_MODE == 1)
        volatile TSS_TSIL_METHOD_RAMDATA TSS_TSIL0_METHOD_RAMDATA_CONTEXT = {TSIL_EOSF_STATE_NONE, 0u, TSIL_SCNIP_STATE_NONE, TSS_MODE_CAP, 0u};
      #else
        volatile TSS_TSIL_METHOD_RAMDATA TSS_TSIL0_METHOD_RAMDATA_CONTEXT = {TSIL_EOSF_STATE_NONE, 0u, TSIL_SCNIP_STATE_NONE};
      #endif
    #endif
    #if TSS_DETECT_MODULE(TSIL1)
      uint16_t TSS_TSIL1_CHANNEL_COUNTER_BUFFER[16];
      const TSS_TSIL_METHOD_ROMDATA TSS_TSIL1_METHOD_ROMDATA_CONTEXT = {TSIL_MethodControl, (uint32_t*) TSI1_BASE_PTR, TSS_TSIL1_CHANNEL_COUNTER_BUFFER, INT_TSI1-16, TSS_DETECT_LOWPOWER_USAGE(TSI1)};
      #if (TSS_USE_NOISE_MODE == 1)
        volatile TSS_TSIL_METHOD_RAMDATA TSS_TSIL1_METHOD_RAMDATA_CONTEXT = {TSIL_EOSF_STATE_NONE, 0u, TSIL_SCNIP_STATE_NONE, TSS_MODE_CAP, 0u};
      #else
        volatile TSS_TSIL_METHOD_RAMDATA TSS_TSIL1_METHOD_RAMDATA_CONTEXT = {TSIL_EOSF_STATE_NONE, 0u, TSIL_SCNIP_STATE_NONE};
      #endif
    #endif

    #include "TSS_SensorTSIL_def.h" /* ROMDATA and RAMDATA TSIL definitions */

    /***************************************************************************//*!
    *                                 Types
    ****************************************************************************/

    /* TSIL Recalibration Struct */

    typedef struct {
      uint8_t u8TSIPS;
      uint8_t u8TSIExtChrg;
      uint16_t u16Distance;
    } TSIL_CALIBRATION;

    /************************** Variables ***************************/

    extern const uint8_t tss_cau8SignalDivider[];
    extern const uint8_t tss_cau8SignalMultiplier[];

    /************************** Prototypes ***************************/

    uint8_t TSIL_InitModule(TSS_TSIL_MemMapPtr psModuleStruct);
    uint8_t TSIL_SetNSamples(TSS_TSIL_MemMapPtr psModuleStruct);
    uint8_t TSIL_InitLowPower(TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSIL_MemMapPtr psModuleStruct);
    uint8_t TSIL_InitPEN(TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct, TSS_TSIL_MemMapPtr psModuleStruct);
    uint8_t TSIL_Recalibrate(TSS_TSIL_MemMapPtr psModuleStruct, TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct);
    uint8_t TSIL_InitThresholds(TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSIL_MemMapPtr psModuleStruct);
    uint8_t TSIL_InitTrigger(TSS_TSIL_MemMapPtr psModuleStruct, TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct, TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct);

    /***************************************************************************//*!
    *
    * @brief  Control function for TSIL General Initialization
    *
    * @param  psModuleStruct - Specification of TSIL module
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t TSIL_InitModule(TSS_TSIL_MemMapPtr psModuleStruct)
    {
      uint8_t u8Result = TSS_INIT_STATUS_OK;

      /* TSI GENCS Settings */
      psModuleStruct->GENCS = 0u; /* Erase GENCS - Turn OFF TSI*/
      /* TSI DATA Settings */
      psModuleStruct->DATA = 0u; /* Erase DATA */
      /* Both OSC settings */
      #if TSS_TSIL_DVOLT != 0
        psModuleStruct->GENCS |= TSI_GENCS_DVOLT(TSS_TSIL_DVOLT);  /* Delta Voltage settings for ElOSC and RefOSC 0 = 1.03 V, 3 = 0.29 V */
      #endif
      /* TSI RefOSC definitions */
      psModuleStruct->GENCS |= TSI_GENCS_REFCHRG(TSS_TSIL_REFCHRG);  /* Reference Oscillator Charge Current 0 = 500 nA, 7 = 64uA */

      return u8Result;
    }

    /***************************************************************************//*!
    *
    * @brief  Initialization of Capacitive mode
    *
    * @param  psModuleStruct - Specification of TSIL module
    *         psMethodRAMDataStruct - Specificcation of RAMDATA Struct
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    #if (TSS_USE_NOISE_MODE == 1)
      uint8_t TSIL_InitModuleCapacitive(TSS_TSIL_MemMapPtr psModuleStruct, TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct)
      {
        uint8_t u8AutoDataTemp;
        uint8_t u8Result = TSS_INIT_STATUS_OK;

        psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_TSIEN_MASK);  /* Turn OFF TSI Module */
        psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_MODE_MASK);   /* Capacitive mode */

        /* Set autocalibration data or user's settings */
        #if ((TSS_TSIL_EXTCHRG_RANGE != 1) || (TSS_TSIL_PS_RANGE != 1) || defined(TSS_ONPROXIMITY_CALLBACK))
          u8AutoDataTemp =  (uint8_t)(psMethodRAMDataStruct->tsil_u8AutocalibData & 0xFFu);
          psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_PS_MASK);
          psModuleStruct->GENCS |= TSI_GENCS_PS(u8AutoDataTemp);
          u8AutoDataTemp =  (uint8_t)((uint8_t)(psMethodRAMDataStruct->tsil_u8AutocalibData >> 4u) & 0xFFu);
          psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_EXTCHRG_MASK);
          psModuleStruct->GENCS |= TSI_GENCS_EXTCHRG(u8AutoDataTemp);
        #else
          psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_PS_MASK);
          psModuleStruct->GENCS |= TSI_GENCS_PS(TSS_TSIL_PS_LOW_LIMIT);
          psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_EXTCHRG_MASK);
          psModuleStruct->GENCS |= TSI_GENCS_EXTCHRG(TSS_TSIL_EXTCHRG_LOW_LIMIT);
        #endif
        psModuleStruct->GENCS |= TSI_GENCS_REFCHRG(TSS_TSIL_REFCHRG);

        return u8Result;
      }

      /***************************************************************************//*!
      *
      * @brief  Initialization of Noise mode
      *
      * @param  psModuleStruct - Specification of TSIL module
      *
      * @return Status Code
      *
      * @remarks
      *
      ****************************************************************************/
      uint8_t TSIL_InitModuleNoise(TSS_TSIL_MemMapPtr psModuleStruct)
      {
        uint8_t u8Result = TSS_INIT_STATUS_OK;

        psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_TSIEN_MASK); /* Turn OFF TSI Module */
        psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_PS_MASK);
        psModuleStruct->GENCS |= TSI_GENCS_PS(TSS_TSIL_NOISE_PS);
        psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_EXTCHRG_MASK);
        psModuleStruct->GENCS |= TSI_GENCS_EXTCHRG(TSS_TSIL_NOISE_EXTCHRG);
        psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_REFCHRG_MASK);
        psModuleStruct->GENCS |= TSI_GENCS_REFCHRG(TSS_TSIL_NOISE_REFCHRG);

        return u8Result;
      }
    #endif

    /***************************************************************************//*!
    *
    * @brief  Control function for TSIL Number of Samples Settings
    *
    * @param  psModuleStruct - Specification of TSIL module
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t TSIL_SetNSamples(TSS_TSIL_MemMapPtr psModuleStruct)
    {
      uint8_t u8Result = TSS_INIT_STATUS_OK;
      uint8_t u8NSamples;

      psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_TSIEN_MASK);  /* Turn OFF TSI Module*/
      psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_NSCN_MASK);   /* Erase NSCN */
      /* Number of Samples settings */
      u8NSamples = tss_CSSys.NSamples;
      psModuleStruct->GENCS |= TSI_GENCS_NSCN(u8NSamples-1u);  /* Number of Consecutive Scans per Electrode 0 = 1 scan, 31 = 32 scans*/

      return u8Result;
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
    uint8_t TSIL_InitLowPower(TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSIL_MemMapPtr psModuleStruct)
    {
      uint8_t u8result = TSS_INIT_STATUS_OK;

      psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK;  /* Turn OFF TSI Module*/;

      if (psMethodROMDataStruct->tsil_u8LowPowerControlSource == 1u)  /* This TSI module is Low Power Control source */
      {
        /* Low Power TSI definition */
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
        psModuleStruct->GENCS &= ~TSI_GENCS_STPE_MASK;    /* TSI Stop Enable while in Low Power Mode (0 = Disable TSI when in LPM, 1 = Allow run of TSI in LPM) */
      }

      return u8result;
    }

    /***************************************************************************//*!
    *
    * @brief  Control function for TSIL Pin Enable Settings
    *
    * @param  *psMethodROMDataStruct - Specificcation of ROMDATA Struct
    *         psModuleStruct - Specification of TSIL module
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t TSIL_InitPEN(TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct, TSS_TSIL_MemMapPtr psModuleStruct)
    {
      uint8_t u8Result = TSS_INIT_STATUS_OK;
      uint8_t u8ElCounter;
      uint8_t u8ChannelNum;

      /* PEN Clearing */
      if (tss_CSSys.SystemConfig.ProximityEn || tss_CSSys.SystemConfig.LowPowerEn)
      {
        psMethodRAMDataStruct->tsil_u16PEN = 0u;
      }
      else
      {
        for(u8ElCounter = 0; u8ElCounter < TSS_N_ELECTRODES; u8ElCounter++)  /* Find all within the same module */
        {
          if (psMethodROMDataStruct == (TSS_TSIL_METHOD_ROMDATA *) (((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsil_cpsMethodROMData))
          {
            /* Disable Electrode */
            if (!(tss_au8ElectrodeEnablers[u8ElCounter >> 3u] & (1u << (u8ElCounter & 0x07u))))
            {
              u8ChannelNum = ((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsil_cu8ChannelNum;
              if ((psMethodRAMDataStruct->tsil_u16PEN & (uint16_t)(1u << u8ChannelNum )) != 0u)
              {
                /* Turn OFF TSI Module if TSI was enabled */
                psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_TSIEN_MASK);
                /* If the Electrode is enabled then disable Electrode */
                psMethodRAMDataStruct->tsil_u16PEN &= ~((uint16_t)(1u << u8ChannelNum));
              }
            }
          }
        }
      }
      /* PEN Enabling - The loop must be performed twice because one PEN can be assigned to more TSS electrodes */
      for(u8ElCounter = 0; u8ElCounter < TSS_N_ELECTRODES; u8ElCounter++) /* Find all with the same module */
      {
        if (psMethodROMDataStruct == (TSS_TSIL_METHOD_ROMDATA *) (((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsil_cpsMethodROMData))
        {
          /* Enable Proximity & Low Power Electrode */
          if (tss_CSSys.SystemConfig.ProximityEn || tss_CSSys.SystemConfig.LowPowerEn)
          {
            if (u8ElCounter == tss_CSSys.LowPowerElectrode)
            {
              u8ChannelNum = ((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsil_cu8ChannelNum;
              psMethodRAMDataStruct->tsil_u16PEN |= ((uint16_t)(1u << u8ChannelNum));
            }
          }
          else
          {
            /* Enable Standard Electrode */
            if (tss_au8ElectrodeEnablers[u8ElCounter >> 3u] & (1u << (u8ElCounter & 0x07u)))
            {
              u8ChannelNum = ((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsil_cu8ChannelNum;
              if ((psMethodRAMDataStruct->tsil_u16PEN & (uint16_t)(1u << u8ChannelNum )) == 0u)
              {
                /* Turn OFF TSI Module if TSI was enabled */
                psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_TSIEN_MASK);
                /* If the Electrode is enabled then disable Electrode */
                psMethodRAMDataStruct->tsil_u16PEN |= ((uint16_t)(1u << u8ChannelNum));
              }
            }
          }
        }
      }

      return u8Result;
    }

    /***************************************************************************//*!
    *
    * @brief  Control function for TSIL Recalibration
    *
    * @param  *psMethodROMDataStruct - Specification of ROMDATA Struct
    *         psModuleStruct - Specification of TSIL module
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    #if ((TSS_TSIL_EXTCHRG_RANGE != 1) || (TSS_TSIL_PS_RANGE != 1) || defined(TSS_ONPROXIMITY_CALLBACK))
      uint8_t TSIL_Recalibrate(TSS_TSIL_MemMapPtr psModuleStruct, TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct)
      {
        TSIL_CALIBRATION FinalTSICalibration;
        TSIL_CALIBRATION FinalTSICalibrationBelow = {0u,0u,0xFFFFu};
        TSIL_CALIBRATION TempTSICalibration;
        TSIL_CALIBRATION OldTSICalibration;
        uint8_t u8Result = TSS_INIT_STATUS_OK;
        uint8_t u8ElCounter;
        uint8_t u8TSIPSTemp;
        uint8_t u8TSIExtChrgLowTemp;
        uint8_t u8TSIExtChrgTemp;
        uint8_t u8CalibrationComplete;
        uint16_t u16CapSampleTemp;
        uint8_t u8Iteration;
        uint16_t u16PENtemp;
        uint16_t u16CapSampleAverage;
        uint8_t u8FinalTSICalibrationFound;
        uint16_t u16ResolutionValue;
        uint8_t u8EXTCHRGRange;
        uint8_t u8PSHighLimit;
        uint8_t u8Found;

        /* Save previous TSI Calibration */
        OldTSICalibration.u8TSIPS = (uint8_t) ((psModuleStruct->GENCS & TSI_GENCS_PS_MASK) >> TSI_GENCS_PS_SHIFT);
        OldTSICalibration.u8TSIExtChrg = (uint8_t) ((psModuleStruct->GENCS & TSI_GENCS_EXTCHRG_MASK) >> TSI_GENCS_EXTCHRG_SHIFT);
        /* Set Variables */
        if (tss_CSSys.SystemConfig.ProximityEn)
        {
          u16ResolutionValue = TSS_TSIL_PROX_RESOLUTION_VALUE;
          u8EXTCHRGRange = TSS_TSIL_PROX_EXTCHRG_RANGE;
          u8PSHighLimit = TSS_TSIL_PROX_PS_HIGH_LIMIT;
          u8TSIPSTemp = TSS_TSIL_PROX_PS_LOW_LIMIT;
          u8TSIExtChrgLowTemp = TSS_TSIL_PROX_EXTCHRG_LOW_LIMIT;
        }
        else
        {
          u16ResolutionValue = TSS_TSIL_RESOLUTION_VALUE;
          u8EXTCHRGRange = TSS_TSIL_EXTCHRG_RANGE;
          u8PSHighLimit = TSS_TSIL_PS_HIGH_LIMIT;
          u8TSIPSTemp = TSS_TSIL_PS_LOW_LIMIT;
          u8TSIExtChrgLowTemp = TSS_TSIL_EXTCHRG_LOW_LIMIT;
        }
        /* Do Autocalibration */
        FinalTSICalibration.u16Distance = 0xFFFFu;
        FinalTSICalibrationBelow.u16Distance = 0xFFFFu;
        u8FinalTSICalibrationFound = 0u;
        /* Set Universal Threshold Values for all electrodes */
        psModuleStruct->TSHD = TSI_TSHD_THRESH(TSS_TSIL_DEFAULT_HIGH_THRESHOLD ) | TSI_TSHD_THRESL(TSS_TSIL_DEFAULT_LOW_THRESHOLD );
        /* Set TSI registers for Single measurement */
        psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_TSIEN_MASK);   /* Turn OFF TSI Module */
        psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_TSIIEN_MASK);  /* Disable Interrupts */
        psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_STM_MASK);     /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
        psModuleStruct->GENCS |= TSI_GENCS_TSIEN_MASK;  /* Turn On TSI Module */
        /* Read enabled pins */
        u16PENtemp = psMethodRAMDataStruct->tsil_u16PEN;
        /* Do Sensitivty autocalibration */
        do
        {
          u8TSIExtChrgTemp = u8TSIExtChrgLowTemp + (u8EXTCHRGRange >> 1u);
          u8Iteration = 0u;
          TempTSICalibration.u16Distance = 0xFFFFu;
          u8Found = 0u;
          do
          {
            /* Increase iteration level */
            u8Iteration += 1u;
            /* Check if all electrodes has proper Counter value */
            u8CalibrationComplete = 1u;
            u16CapSampleAverage = 0u;
            /* Go through all enabled pins */
            for(u8ElCounter = 0u; u8ElCounter < 16u; u8ElCounter++)
            {
              if (u16PENtemp & (1u << u8ElCounter))
              {
                /* Configure TSIL */
                psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_PS_MASK);        /* Erase PS */
                psModuleStruct->GENCS |= TSI_GENCS_PS(u8TSIPSTemp);    /* Set Prescaler for Electrode OSC - Set by the user 0 = divide 1, 7 = divide 128 */
                psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_EXTCHRG_MASK);   /* Erase ExtChrg */
                psModuleStruct->GENCS |= TSI_GENCS_EXTCHRG(u8TSIExtChrgTemp); /* External OSC Charge Current 0= 500nA, 7 = 64uA */
                /* Start Measurement */
                psModuleStruct->DATA &= ~((uint32_t)TSI_DATA_TSICH_MASK);  /* Erase TSICH */
                psModuleStruct->DATA |= TSI_DATA_TSICH(u8ElCounter);      /* Set Channel */
                psModuleStruct->DATA |= TSI_DATA_SWTS_MASK;              /* Toggle SW trigger */
                #ifdef _TSS_TEST
                  _TSS_TSIGEN_ON;  /* Start generator for test purpose */
                #endif
                do
                {
                /* Measurement Running */
                } while ((psModuleStruct->GENCS & TSI_GENCS_EOSF_MASK) == 0u);  /* If TSI End of Scan Flag is reported*/
                #ifdef _TSS_TEST
                  _TSS_TSIGEN_OFF;  /* Stop generator */
                #endif
                /* Reset EOSF Flag */
                psModuleStruct->GENCS |= TSI_GENCS_EOSF_MASK;  /* Toggle EOSF flag */
                /* Wait for data ready */
                #if defined(__ARMCC_VERSION)
                  __nop(); __nop();
                #else
                  asm ("NOP"); asm ("NOP");
                #endif
                /* Read TSI Counter */
                u16CapSampleTemp = (uint16_t) (psModuleStruct->DATA & TSI_DATA_TSICNT_MASK);
                if (u16CapSampleTemp < u16ResolutionValue)
                {
                  u8CalibrationComplete = 0u;  /* Capacitance value is small */
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
              u16CapSampleTemp = u16CapSampleAverage - u16ResolutionValue;
            }
            else
            {
              u16CapSampleTemp = u16ResolutionValue - u16CapSampleAverage;
            }
            /* All samples > resolution AND (Average > resolution OR already found and tune it) */
            if ((u8CalibrationComplete == 1u ) && ((u16CapSampleAverage > u16ResolutionValue) || (u8Found && (TempTSICalibration.u16Distance >= u16CapSampleTemp))))
            {
              TempTSICalibration.u8TSIExtChrg = u8TSIExtChrgTemp;
              TempTSICalibration.u8TSIPS = u8TSIPSTemp;
              TempTSICalibration.u16Distance = u16CapSampleTemp;
              u8Found++;  /* Match for set PS found */
            }
            else if (TempTSICalibration.u16Distance >= u16CapSampleTemp && (u8Found ==0))
            {
              TempTSICalibration.u8TSIExtChrg = u8TSIExtChrgTemp;
              TempTSICalibration.u8TSIPS = u8TSIPSTemp;
              TempTSICalibration.u16Distance = u16CapSampleTemp;
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
          /* The value only if a distance is lower and was found with sufficient resolution */
          if ((FinalTSICalibration.u16Distance >= TempTSICalibration.u16Distance) && (u8Found > 0))
          {
            FinalTSICalibration.u8TSIPS = TempTSICalibration.u8TSIPS;
            FinalTSICalibration.u8TSIExtChrg = TempTSICalibration.u8TSIExtChrg;
            FinalTSICalibration.u16Distance = TempTSICalibration.u16Distance;
            u8FinalTSICalibrationFound++;
          }
          else if ((FinalTSICalibrationBelow.u16Distance >= TempTSICalibration.u16Distance) && (u8FinalTSICalibrationFound == 0u))
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
          psMethodRAMDataStruct->tsi_au16Resolution = u16ResolutionValue;  /* Store the resolution value */
        }
        else
        {
          FinalTSICalibration.u8TSIPS = FinalTSICalibrationBelow.u8TSIPS;
          FinalTSICalibration.u8TSIExtChrg = FinalTSICalibrationBelow.u8TSIExtChrg;
          FinalTSICalibration.u16Distance = FinalTSICalibrationBelow.u16Distance;
          /* Reuse of u8Iteration & u16CapSampleTemp */
          u8Iteration = 0u;
          u16CapSampleTemp = u16ResolutionValue - FinalTSICalibrationBelow.u16Distance;
          /* Determine TSS Resolution (highest bit set) */
          while (u16CapSampleTemp >> u8Iteration)
          {
            u8Iteration++;
          }
          /* Store the present resolution value */
          psMethodRAMDataStruct->tsi_au16Resolution = (1u << (uint8_t)(u8Iteration - 1u));
        }
        /* Set Final Found values */
        psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_TSIEN_MASK);     /* Turn OFF TSI Module */
        psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_PS_MASK);        /* Erase PS */
        psModuleStruct->GENCS |= TSI_GENCS_PS(FinalTSICalibration.u8TSIPS); /* Set Prescaler for Electrode OSC - Set by the user 0 = divide 1, 7 = divide 128 */
        psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_EXTCHRG_MASK);   /* Erase ExtChrg */
        psModuleStruct->GENCS |= TSI_GENCS_EXTCHRG(FinalTSICalibration.u8TSIExtChrg); /* External OSC Charge Current 0= 500nA, 7 = 64uA */
        #if (TSS_USE_NOISE_MODE == 1)
          /* Store found PS, EXTCHRG for future reference */
          psMethodRAMDataStruct->tsil_u8AutocalibData = (uint8_t)(FinalTSICalibration.u8TSIPS | (uint8_t)(FinalTSICalibration.u8TSIExtChrg << 4u));
        #endif
        /* Save Value for Proximity function */
        if (tss_CSSys.SystemConfig.ProximityEn)
        {
          tss_u16CapSample = FinalTSICalibration.u16Distance + u16ResolutionValue;
        }
        /* Indication of changed configuration */
        if ((OldTSICalibration.u8TSIPS != FinalTSICalibration.u8TSIPS) || (OldTSICalibration.u8TSIExtChrg != FinalTSICalibration.u8TSIExtChrg))
        {
          u8Result |= TSS_INIT_STATUS_CALIBRATION_CHANGED;
        }

        return u8Result;
      }
    #else
      uint8_t TSIL_Recalibrate(TSS_TSIL_MemMapPtr psModuleStruct, TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct)
      {
        uint8_t u8Result = TSS_INIT_STATUS_OK;

        psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_TSIEN_MASK);     /* Turn OFF TSI Module */
        psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_PS_MASK);        /* Erase PS */
        psModuleStruct->GENCS |= TSI_GENCS_PS(TSS_TSIL_PS_LOW_LIMIT); /* Set Prescaler for Electrode OSC - Set by the user 0 = divide 1, 7 = divide 128 */
        psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_EXTCHRG_MASK);   /* Erase ExtChrg */
        psModuleStruct->GENCS |= TSI_GENCS_EXTCHRG(TSS_TSIL_EXTCHRG_LOW_LIMIT); /* External OSC Charge Current 0= 500nA, 7 = 64uA */

        psMethodRAMDataStruct->tsi_au16Resolution = TSS_TSIL_RESOLUTION_VALUE;  /* Store the resolution value */

        return u8Result;
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
    uint8_t TSIL_InitThresholds(TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSIL_MemMapPtr psModuleStruct)
    {
      uint8_t u8result = TSS_INIT_STATUS_OK;
      uint8_t u8LowPowerEl;
      uint16_t u16CapSampleTemp;
      uint8_t u8LowPowerElectrodeSensitivity;

      psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_TSIEN_MASK);  /* Turn OFF TSI Module */
      psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_TSIIEN_MASK); /* Disable Interrupts */
      /* Set Universal Threshold Values for all electrodes */
      psModuleStruct->TSHD = TSI_TSHD_THRESH(TSS_TSIL_DEFAULT_HIGH_THRESHOLD ) | TSI_TSHD_THRESL(TSS_TSIL_DEFAULT_LOW_THRESHOLD );
      /* If Module is selected for Low Power Control do following */
      if (psMethodROMDataStruct->tsil_u8LowPowerControlSource == 1u) /* This TSI module is Low Power Control source */
      {
        u8LowPowerEl = tss_CSSys.LowPowerElectrode;
        /* Check if Low Power Electrode is not out of range */
        if (u8LowPowerEl < TSS_N_ELECTRODES)/* This electrode is selected as source for Low Power Control */
        {
          if ((TSS_GENERIC_METHOD_ROMDATA *) psMethodROMDataStruct == (TSS_GENERIC_METHOD_ROMDATA *)((TSS_GENERIC_ELECTRODE_ROMDATA *)tss_acp8ElectrodeROMData[u8LowPowerEl])->gen_cpsMethodROMData)
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
              psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_STM_MASK);  /* Set Scan Trigger Mode (0 = SW Trigger, 1 = Periodical Scan) */
              psModuleStruct->DATA &= ~((uint32_t)TSI_DATA_TSICH_MASK);  /* Erase TSICH */
              psModuleStruct->DATA |= TSI_DATA_TSICH(((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8LowPowerEl]))->tsil_cu8ChannelNum);  /* Set Channel */
              psModuleStruct->GENCS |= TSI_GENCS_TSIEN_MASK;
              psModuleStruct->DATA |= TSI_DATA_SWTS_MASK;
              #ifdef _TSS_TEST
                _TSS_TSIGEN_ON;  /* Start generator for test purpose */
              #endif
              do
              {
              /* Measurement Running */
              } while (!(psModuleStruct->GENCS & TSI_GENCS_EOSF_MASK));  /* If TSI End of Scan Flag is reported*/
              #ifdef _TSS_TEST
                _TSS_TSIGEN_OFF;  /* Stop generator */
              #endif
              /* Wait for data ready */
              #if defined(__ARMCC_VERSION)
                __nop(); __nop();
              #else
                asm ("NOP"); asm ("NOP");
              #endif
              /* Read TSI Counter */
              u16CapSampleTemp = (uint16_t) (psModuleStruct->DATA & TSI_DATA_TSICNT_MASK);
            }
            /* Calculation of Threshold values */
            u8LowPowerElectrodeSensitivity = tss_CSSys.LowPowerElectrodeSensitivity;
            /* Set Threshold for Low Power Wake up */
            psModuleStruct->TSHD = TSI_TSHD_THRESH(u16CapSampleTemp + u8LowPowerElectrodeSensitivity) | TSI_TSHD_THRESL(TSS_TSIL_DEFAULT_LOW_THRESHOLD );
            /* Result */
            u8result |= TSS_INIT_STATUS_LOWPOWER_ELEC_SET;  /* For Indication of state if LowPower Electrode was found */
          }
        }
      }

      return u8result;
    }

    /***************************************************************************//*!
    *
    * @brief  Control function for TSIL Trigger Settings
    *
    * @param  *psMethodROMDataStruct - Specificcation of ROMDATA Struct
    *         psModuleStruct - Specification of TSIL module
    *         *psMethodRAMDataStruct - Specification of RAMDATA Struct
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t TSIL_InitTrigger(TSS_TSIL_MemMapPtr psModuleStruct, TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct, TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct)
    {
      uint8_t u8Result = TSS_INIT_STATUS_OK;
      uint8_t u8LowPowerEl;
      NVIC_MemMapPtr psNVICStruct = NVIC_BASE_PTR;

      psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_TSIEN_MASK);  /* Turn OFF TSI Module */
      /* Setup TSI */
      if (tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_AUTO)
      {
        psModuleStruct->GENCS |= TSI_GENCS_STM_MASK;  /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
        /* Indication that AutoTrigger was set correctly */
        u8Result |= TSS_INIT_STATUS_AUTOTRIGGER_SET;
      }
      else  /* Always & SW Mode */
      {
        psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_STM_MASK);  /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
        u8Result |= TSS_INIT_STATUS_TRIGGER_SET;
      }
      psModuleStruct->GENCS |= TSI_GENCS_TSIIEN_MASK; /* TSI Interrupt Enable (0 = disabled, 1 = enabled) */
      psModuleStruct->GENCS |= TSI_GENCS_ESOR_MASK;   /* End-of-Scan|Out-of-Range Interrupt Selection (0 = Out-of-Range allow, 1 = End-of-Scan allow) */
      /* Enable Interrupt */
      psNVICStruct->ICPR |= (1u << (psMethodROMDataStruct->tsil_u8ModuleIRQNum & 0x1Fu));
      psNVICStruct->ISER |= (1u << (psMethodROMDataStruct->tsil_u8ModuleIRQNum & 0x1Fu));
      /* Set Initial State */
      psMethodRAMDataStruct->tsil_eEndOfScanState = TSIL_EOSF_STATE_NONE;
      psMethodRAMDataStruct->tsil_eScanInProgressState = TSIL_SCNIP_STATE_NONE;
      /* Setup Low Power */
      if ((tss_CSSys.SystemConfig.LowPowerEn) && (psMethodROMDataStruct->tsil_u8LowPowerControlSource == 1u))
      {
        /* Check if Low Power Threshold was succesfully set in the past */
        if ((psModuleStruct->TSHD & TSI_TSHD_THRESH_MASK) != TSI_TSHD_THRESH(TSS_TSIL_DEFAULT_HIGH_THRESHOLD))
        {
          /* Set Low Power electrode for measurement */
          psModuleStruct->DATA &= ~((uint32_t)TSI_DATA_TSICH_MASK);  /* Erase TSICH */
          u8LowPowerEl = tss_CSSys.LowPowerElectrode;
          psModuleStruct->DATA |= TSI_DATA_TSICH(((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8LowPowerEl]))->tsil_cu8ChannelNum);      /* Set Channel */
        }
        /* Set periodic measurement mode */
        psModuleStruct->GENCS |= TSI_GENCS_STM_MASK;    /* Set Scan Trigger Mode (0 = SW Trigger, 1 = Periodical Scan) */
        /* Enable EOSF interrupt */
        psModuleStruct->GENCS &= ~TSI_GENCS_ESOR_MASK;  /* Set Out of Range interrupt (0 = Out-of-Range allow, 1 = End-of-Scan allow) */
        psModuleStruct->GENCS |= TSI_GENCS_TSIIEN_MASK;  /* TSI Interrupt Enable (0 = disabled, 1 = enabled) */
        /* TSI enable and start to periodic measurement */
        psModuleStruct->GENCS |= TSI_GENCS_TSIEN_MASK;  /* TSI Enable (0 = disabled, 1 = enabled) */
        /* Indication that LowPower was set correctly */
        u8Result |= TSS_INIT_STATUS_LOWPOWER_SET;       /* For Indication of state if LowPower was set */
        u8Result &= ~TSS_INIT_STATUS_TRIGGER_SET;       /* Remove status that Trigger was set correctly */
        u8Result &= ~TSS_INIT_STATUS_AUTOTRIGGER_SET;   /* Remove status that Auto Trigger was set correctly */
      }

      return u8Result;
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
    uint8_t TSIL_MethodControl(uint8_t u8ElNum, uint8_t u8Command)
    {
      uint8_t u8Result = TSS_INIT_STATUS_OK;
      TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct;
      TSS_TSIL_ELECTRODE_ROMDATA *psElectrodeROMDataStruct;
      TSS_TSIL_MemMapPtr psModuleStruct;
      TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct;

      /* Pointers Decoding */
      psElectrodeROMDataStruct = (TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElNum]);
      psMethodROMDataStruct = (TSS_TSIL_METHOD_ROMDATA *) (psElectrodeROMDataStruct->tsil_cpsMethodROMData);
      psModuleStruct = (TSS_TSIL_MemMapPtr) (psMethodROMDataStruct->tsil_cpsModule);
      psMethodRAMDataStruct = (TSS_TSIL_METHOD_RAMDATA *) (psElectrodeROMDataStruct->tsil_cpsMethodRAMData);

      switch (u8Command)
      {
        /************* SWTrigger ************/
        case TSS_INIT_COMMAND_SW_TRIGGER:
          if (psMethodRAMDataStruct->tsil_eScanInProgressState == TSIL_SCNIP_STATE_WAIT)
          {
            psMethodRAMDataStruct->tsil_eEndOfScanState = TSIL_EOSF_STATE_NONE; /* Reset EndOfScanState */
            psMethodRAMDataStruct->tsil_eScanInProgressState = TSIL_SCNIP_STATE_PROCESS; /* Set processing */
            psModuleStruct->DATA |= TSI_DATA_SWTS_MASK; /* Toggle SW Trigger */
            u8Result |= TSS_INIT_STATUS_TRIGGER_SET;
            #ifdef _TSS_TEST
              _TSS_TSIGEN_ON;  /* Start generator for test purpose */
            #endif
          }
        break;
        /************* Do TSIL Init *******************/
        case TSS_INIT_COMMAND_INIT_MODULES:
          u8Result |= TSIL_InitModule(psModuleStruct);
          u8Result |= TSIL_SetNSamples(psModuleStruct);
          u8Result |= TSIL_InitLowPower(psMethodROMDataStruct, psModuleStruct);
          u8Result |= TSIL_InitPEN(psMethodROMDataStruct, psMethodRAMDataStruct, psModuleStruct);
          u8Result |= TSIL_Recalibrate(psModuleStruct, psMethodROMDataStruct, psMethodRAMDataStruct);
          u8Result |= TSIL_InitThresholds(psMethodROMDataStruct, psModuleStruct);
          u8Result |= TSIL_InitTrigger(psModuleStruct, psMethodRAMDataStruct, psMethodROMDataStruct);
        break;
        #if (TSS_USE_NOISE_MODE == 1)
          case TSS_INIT_COMMAND_MODE_CAP:
            psMethodRAMDataStruct->tsil_u8ActiveMode = TSS_MODE_CAP;
            u8Result |= TSIL_InitModuleCapacitive(psModuleStruct, psMethodRAMDataStruct);
          break;
          case TSS_INIT_COMMAND_MODE_NOISE:
            psMethodRAMDataStruct->tsil_u8ActiveMode = TSS_MODE_NOISE;
            u8Result |= TSIL_InitModuleNoise(psModuleStruct);
          break;
          case TSS_INIT_COMMAND_MODE_GET:
            /* Return active mode */
            u8Result = psMethodRAMDataStruct->tsil_u8ActiveMode;
          break;
        #endif
        /************* Set NSamples ***************/
        case TSS_INIT_COMMAND_SET_NSAMPLES:
          u8Result |= TSIL_SetNSamples(psModuleStruct);
          u8Result |= TSIL_Recalibrate(psModuleStruct, psMethodROMDataStruct, psMethodRAMDataStruct);
          u8Result |= TSIL_InitThresholds(psMethodROMDataStruct, psModuleStruct);
          u8Result |= TSIL_InitTrigger(psModuleStruct, psMethodRAMDataStruct, psMethodROMDataStruct);
        break;
        /************ Low Power Init *************/
        case TSS_INIT_COMMAND_INIT_LOWPOWER:
          #if (TSS_USE_NOISE_MODE == 1)
            if (psMethodRAMDataStruct->tsil_u8ActiveMode == TSS_MODE_NOISE)
            {
              /* Low power not set if a noise mode is active */
              u8Result &= ~TSS_INIT_STATUS_LOWPOWER_SET;
              return u8Result;
            }
          #endif
          u8Result |= TSIL_InitLowPower(psMethodROMDataStruct, psModuleStruct);
          u8Result |= TSIL_InitThresholds(psMethodROMDataStruct, psModuleStruct);
          u8Result |= TSIL_InitTrigger(psModuleStruct, psMethodRAMDataStruct, psMethodROMDataStruct);
        break;
        /************* Goto Low Power ************/
        case TSS_INIT_COMMAND_GOTO_LOWPOWER:
          #if (TSS_USE_NOISE_MODE == 1)
            if (psMethodRAMDataStruct->tsil_u8ActiveMode == TSS_MODE_NOISE)
            {
              /* Low power not set if a noise mode is active */
              u8Result &= ~TSS_INIT_STATUS_LOWPOWER_SET;
              return u8Result;
            }
          #endif
          u8Result |= TSIL_InitLowPower(psMethodROMDataStruct, psModuleStruct);
          #if TSS_USE_LOWPOWER_THRESHOLD_BASELINE
            u8Result |= TSIL_InitThresholds(psMethodROMDataStruct, psModuleStruct);
          #endif
          u8Result |= TSIL_InitTrigger(psModuleStruct, psMethodRAMDataStruct, psMethodROMDataStruct);
        break;
        /*************** Do Auto calibration setting of TSIL EXTCHRG and TSIL PS **************/
        case TSS_INIT_COMMAND_RECALIBRATE:
          u8Result |= TSIL_Recalibrate(psModuleStruct, psMethodROMDataStruct, psMethodRAMDataStruct);
          u8Result |= TSIL_InitThresholds(psMethodROMDataStruct, psModuleStruct);
          u8Result |= TSIL_InitTrigger(psModuleStruct, psMethodRAMDataStruct, psMethodROMDataStruct);
        break;
        /************ Electrode Enablers *************/
        case TSS_INIT_COMMAND_ENABLE_ELECTRODES:
          u8Result |= TSIL_InitPEN(psMethodROMDataStruct, psMethodRAMDataStruct, psModuleStruct);
        break;
        /************* Triggering Init ************/
        case TSS_INIT_COMMAND_INIT_TRIGGER:
          u8Result |= TSIL_InitTrigger(psModuleStruct, psMethodRAMDataStruct, psMethodROMDataStruct);
        break;
      }

      /* Exit */
      return u8Result;
    }

    /***************************************************************************//*!
    *
    * @brief  Reads TSIL capacitance counter and returns a status code
    *
    * @param  psElectrodeROMDataStruct - Specification of electrode ROMDATA Struct
    *         psModuleStruct - Specification of TSIL module
    *         psMethodRAMDataStruct - Specification of electrode RAMDATA Struct
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t TSIL_ReadCounter(TSS_TSIL_ELECTRODE_ROMDATA *psElectrodeROMDataStruct, TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct)
    {
      uint8_t u8Result;

      #if (TSS_USE_NOISE_MODE == 1)
        if (psMethodRAMDataStruct->tsil_u8ActiveMode == TSS_MODE_NOISE)
        {
          /* We load entire counter buffer after noise measurement is completed */
          return TSS_SAMPLE_STATUS_OK;
        }
      #endif
      /* Read TSIL Counter */
      tss_u16CapSample = ((uint16_t *) (psMethodROMDataStruct->tsil_cpu16ChannelCounterBuffer))[psElectrodeROMDataStruct->tsil_cu8ChannelNum];
      /* Evaluation of the Measured Value */
      if (tss_u16CapSample > TSS_TSIL_CAP_HIGH_LIMIT(psMethodRAMDataStruct->tsi_au16Resolution))
      {
        u8Result = TSS_SAMPLE_ERROR_CHARGE_TIMEOUT;   /* Too High Capacity is detected */
      }
      else if (tss_u16CapSample > TSS_TSIL_RECALIBRATION_HIGH_THRESHOLD(psMethodRAMDataStruct->tsi_au16Resolution))
      {
        u8Result = TSS_SAMPLE_RECALIB_REQUEST_HICAP;  /* Re-calibration is needed */
      }
      else if (tss_u16CapSample < TSS_TSIL_CAP_LOW_LIMIT(psMethodRAMDataStruct->tsi_au16Resolution))
      {
        u8Result = TSS_SAMPLE_ERROR_SMALL_CAP;        /* Too Low Capacity is detected */
      }
      else if (tss_u16CapSample < TSS_TSIL_RECALIBRATION_LOW_THRESHOLD(psMethodRAMDataStruct->tsi_au16Resolution))
      {
        u8Result = TSS_SAMPLE_RECALIB_REQUEST_LOCAP;  /* Re-calibration is needed */
      }
      else
      {
        u8Result = TSS_SAMPLE_STATUS_OK;
      }

      return u8Result;
    }

    /***************************************************************************//*!
    *
    * @brief  Performs and evalautes measurement
    *
    * @param  *psMethodROMDataStruct - Specification of ROMDATA Struct
    *         *psMethodRAMDataStruct - Specification of RAMDATA Struct
    *         psModuleStruct - Specification of TSIL module
    *
    * @return Status Code
    *
    * @remarks
    *
    ****************************************************************************/
    uint8_t TSIL_HandleMeasurement(TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct, TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct, TSS_TSIL_MemMapPtr psModuleStruct, uint8_t u8ElChannel)
    {
      uint8_t u8Result;

      #if (TSS_USE_NOISE_MODE == 1)
        if (psMethodRAMDataStruct->tsil_u8ActiveMode == TSS_MODE_NOISE)
        {
          psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK;
          psModuleStruct->GENCS |= TSI_GENCS_TSIEN_MASK;
          psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_MODE_MASK);  /* Noise mode active -> enable noise mode */
          psModuleStruct->GENCS |= TSI_GENCS_MODE(TSS_TSIL_MODE_NOISE_AUTO);
        }
      #endif

      /* Establish measurement */
      if (tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_ALWAYS)
      {
        if (psMethodRAMDataStruct->tsil_eEndOfScanState != TSIL_EOSF_STATE_REGULAR)
        {
          /* Init measurement */
          psMethodRAMDataStruct->tsil_u8StartElectrode = u8ElChannel;
          /* Start First Measurement */
          psModuleStruct->DATA &= ~((uint32_t)TSI_DATA_TSICH_MASK);  /* Erase TSICH */
          psModuleStruct->DATA |= TSI_DATA_TSICH(u8ElChannel);  /* Set Channel */
          /* Set Status Flags */
          psMethodRAMDataStruct->tsil_eEndOfScanState = TSIL_EOSF_STATE_NONE; /* Reset EndOfScanState */
          psMethodRAMDataStruct->tsil_eScanInProgressState = TSIL_SCNIP_STATE_PROCESS; /* Set processing */
          psModuleStruct->DATA |= TSI_DATA_SWTS_MASK;  /* Toggle SW trigger */
        }
        u8Result = TSS_SAMPLE_STATUS_PROCESSING;
      }
      else  /* If AUTO & SW Mode */
      {
        /* Init measurement */
        psMethodRAMDataStruct->tsil_u8StartElectrode = u8ElChannel;
        psMethodRAMDataStruct->tsil_eScanInProgressState = TSIL_SCNIP_STATE_WAIT;
        /* Start First Measurement */
        psModuleStruct->DATA &= ~((uint32_t)TSI_DATA_TSICH_MASK);   /* Erase TSICH */
        psModuleStruct->DATA |= TSI_DATA_TSICH(u8ElChannel);  /* Set Channel */
        /* Handle measurement status */
        if (tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_AUTO)
        {
          /* Evaluate Auto Trigger period */
          if (psMethodRAMDataStruct->tsil_eEndOfScanState == TSIL_EOSF_STATE_ERROR)
          {
            u8Result = TSS_SAMPLE_ERROR_SMALL_TRIGGER_PERIOD; /* Small TriggerModuloValue */
            psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_TSIEN_MASK); /* Disable whole TSS due triggering crash */
            #ifdef _TSS_TEST
               _TSS_TSIGEN_OFF;  /* Stop generator for test purpose */
            #endif
          }
          else
          {
            u8Result = TSS_SAMPLE_STATUS_PROCESSING;
          }
          psMethodRAMDataStruct->tsil_eEndOfScanState = TSIL_EOSF_STATE_NONE;  /* Reset EndOfScanState */
        }
        else
        {
          u8Result = TSS_SAMPLE_STATUS_PROCESSING;
        }
      }
      #ifdef _TSS_TEST
        _TSS_TSIGEN_ON;  /* Start generator for test purpose */
      #endif

      return u8Result;
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
    uint8_t TSIL_SampleElectrode(uint8_t u8ElecNum, uint8_t u8Command)
    {
      uint8_t u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
      uint8_t u8ElCounter;
      uint8_t u8ElChannel;

      TSS_TSIL_METHOD_ROMDATA *psMethodROMDataStruct;
      TSS_TSIL_ELECTRODE_ROMDATA *psElectrodeROMDataStruct;
      TSS_TSIL_MemMapPtr psModuleStruct;
      TSS_TSIL_METHOD_RAMDATA *psMethodRAMDataStruct;

      /* Pointers decoding */
      psElectrodeROMDataStruct = (TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElecNum]);
      psMethodROMDataStruct = (TSS_TSIL_METHOD_ROMDATA *) (psElectrodeROMDataStruct->tsil_cpsMethodROMData);
      psModuleStruct = (TSS_TSIL_MemMapPtr) (psMethodROMDataStruct->tsil_cpsModule);
      psMethodRAMDataStruct = (TSS_TSIL_METHOD_RAMDATA *) (psElectrodeROMDataStruct->tsil_cpsMethodRAMData);
      u8ElChannel = psElectrodeROMDataStruct->tsil_cu8ChannelNum;

      switch (u8Command)
      {
        /* Module Control */
        case TSS_SAMPLE_COMMAND_RESTART:
          /* If TSIL is disabled then enable */
          if ((psModuleStruct->GENCS & TSI_GENCS_TSIEN_MASK) == 0u)
          {
            psModuleStruct->GENCS |= TSI_GENCS_TSIEN_MASK;     /* Turn ON TSI Module */
          }
          /* Handle Measurement */
          if (psMethodRAMDataStruct->tsil_eScanInProgressState == TSIL_SCNIP_STATE_NONE)
          {
            u8ElectrodeStatus = TSIL_HandleMeasurement(psMethodRAMDataStruct, psMethodROMDataStruct, psModuleStruct, u8ElChannel);
          }
          else
          {
            u8ElectrodeStatus = TSS_SAMPLE_STATUS_PROCESSING;
          }
        break;
        case TSS_SAMPLE_COMMAND_PROCESS:
          /* If TSIL is disabled then enable */
          if ((psModuleStruct->GENCS & TSI_GENCS_TSIEN_MASK) == 0u)
          {
            psModuleStruct->GENCS |= TSI_GENCS_TSIEN_MASK;     /* Turn ON TSI Module */
          }
          /* Handle Auto Trigger Mode */
          if (psMethodRAMDataStruct->tsil_eScanInProgressState == TSIL_SCNIP_STATE_NONE) /* TSIL scanning is not in progress */
          {
            /* If TSI End of Scan Flag is reported */
            if (psMethodRAMDataStruct->tsil_eEndOfScanState >= TSIL_EOSF_STATE_REGULAR)
            {
              psMethodRAMDataStruct->tsil_eEndOfScanState = TSIL_EOSF_STATE_PROCESS;
              #ifdef _TSS_TEST
                _TSS_TSIGEN_OFF;  /* Stop generator */
              #endif
              /* Read TSI Counter */
              u8ElectrodeStatus = TSIL_ReadCounter(psElectrodeROMDataStruct, psMethodROMDataStruct, psMethodRAMDataStruct);
            }
            else
            {
              u8ElectrodeStatus = TSIL_HandleMeasurement(psMethodRAMDataStruct, psMethodROMDataStruct, psModuleStruct, u8ElChannel);
            }
          }
          else
          {
            u8ElectrodeStatus = TSS_SAMPLE_STATUS_PROCESSING;
          }
        break;
        case TSS_SAMPLE_COMMAND_GET_NEXT_ELECTRODE:
          /* Find the next electrode within the electrode module */
          u8ElCounter = u8ElecNum;
          #if TSS_N_ELECTRODES > 1
            do
            {
              if (u8ElCounter < (TSS_N_ELECTRODES-1))
              {
                u8ElCounter++;
              }
              else
              {
                u8ElCounter = 0u;
              }
            } while (psMethodROMDataStruct != (TSS_TSIL_METHOD_ROMDATA *) (((TSS_TSIL_ELECTRODE_ROMDATA *) (tss_acp8ElectrodeROMData[u8ElCounter]))->tsil_cpsMethodROMData));
          #endif
          /* Return found electrode number */
          u8ElectrodeStatus = u8ElCounter;
        break;
        case TSS_SAMPLE_COMMAND_RECALIB:
          u8ElectrodeStatus = TSIL_MethodControl(u8ElecNum, TSS_INIT_COMMAND_RECALIBRATE);
          /* Evaluate return status */
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
          u8ElectrodeStatus = TSIL_MethodControl(u8ElecNum, TSS_INIT_COMMAND_ENABLE_ELECTRODES);

          u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
        break;
        case TSS_SAMPLE_COMMAND_SET_LOWLEVEL_CONFIG:
          psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_TSIEN_MASK);     /* Turn OFF TSI Module */
          psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_PS_MASK);        /* Erase PS */
          psModuleStruct->GENCS |= TSI_GENCS_PS(tss_u16CapSample & 0x7u); /* Set Prescaler for Electrode OSC - Set by the user 0 = divide 1, 7 = divide 128 */
          psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_EXTCHRG_MASK);   /* Erase ExtChrg */
          psModuleStruct->GENCS |= TSI_GENCS_EXTCHRG((tss_u16CapSample >> 3u) & 0x7u); /* External OSC Charge Current 0= 500nA, 7 = 64uA */

          u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
        break;
        case TSS_SAMPLE_COMMAND_GET_LOWLEVEL_CONFIG:
          u8ElectrodeStatus = (uint8_t) ((psModuleStruct->GENCS & TSI_GENCS_PS_MASK) >> TSI_GENCS_PS_SHIFT);
          tss_u16CapSample = (uint16_t) ((((psModuleStruct->GENCS & TSI_GENCS_EXTCHRG_MASK) >> TSI_GENCS_EXTCHRG_SHIFT) << 3u) | (uint16_t) u8ElectrodeStatus);

          u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
        break;
        #if (TSS_USE_NOISE_MODE == 1)
          case TSS_SAMPLE_COMMAND_GET_NOISE_VALUE:
            if (psMethodRAMDataStruct->tsil_u8ActiveMode == TSS_MODE_NOISE)
            {
              /* Return measured noise value */
              tss_u16CapSample = ((uint16_t *) (psMethodROMDataStruct->tsil_cpu16ChannelCounterBuffer))[psElectrodeROMDataStruct->tsil_cu8ChannelNum];
              u8ElectrodeStatus = TSS_SAMPLE_STATUS_OK;
            }
            else
            {
              /* Noise value not available */
              u8ElectrodeStatus = TSS_SAMPLE_STATUS_PROCESSING;
            }
          break;
        #endif
      }

      return u8ElectrodeStatus;   /* Return status code */
    }

    /***************************************************************************//*!
    *
    * @brief  TSIL ISR routines
    *
    * @remarks
    *
    ****************************************************************************/
    #if TSS_DETECT_MODULE(TSIL)
      #if TSS_ENABLE_DIAGNOSTIC_MESSAGES
        #warning TSS - Please assign ISR function TSS_TSIIsr to vector INT_TSI
      #endif
      void TSS_TSIIsr(
        #if TSS_USE_MQX
          void *_isrParameter
        #else
          void
        #endif
        )
      {
        TSS_TSIL_MemMapPtr psModuleStruct;
        uint8_t u8Channel;
        uint16_t u16PEN;
        uint8_t u8Timeout;

        /* Pointers decoding */
        psModuleStruct = (TSS_TSIL_MemMapPtr) (TSS_TSIL_METHOD_ROMDATA_CONTEXT.tsil_cpsModule);

        if (tss_CSSys.SystemConfig.LowPowerEn)
        {
          psModuleStruct->GENCS |= TSI_GENCS_OUTRGF_MASK; /* Clear OUTFRG Flag */
          psModuleStruct->GENCS |= TSI_GENCS_ESOR_MASK;   /* End-of-Scan|Out-of-Range Interrupt Selection (0 = Out-of-Range allow, 1 = End-of-Scan allow) */
          psModuleStruct->GENCS &= ~TSI_GENCS_TSIIEN_MASK; /* Disable TSI Interrupt */
          /* Disables LowPower Wake Up */
          u16PEN = TSS_GetSystemConfig(System_SystemConfig_Register); /* Reuse of u16PEN for temp purpose */
          (void)TSS_SetSystemConfig(System_SystemConfig_Register,(u16PEN & ~TSS_LOWPOWER_EN_MASK));
        }
        else
        {
          /* Reuse (any store into GENC wipes out MODE value (noise counter) */
          u16PEN = (uint16_t)((psModuleStruct->GENCS & TSI_GENCS_MODE_MASK) >> TSI_GENCS_MODE_SHIFT);
          /* Clear EOSF flag */
          psModuleStruct->GENCS |= TSI_GENCS_EOSF_MASK;

          if (TSS_TSIL_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState == TSIL_EOSF_STATE_NONE)
          {
            /* Disable HW trigger */
            psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_STM_MASK);   /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
            /* Save Actual channel */
            u8Channel = ((psModuleStruct->DATA & TSI_DATA_TSICH_MASK) >> TSI_DATA_TSICH_SHIFT);

            if (TSS_TSIL_METHOD_RAMDATA_CONTEXT.tsil_u8ActiveMode == TSS_MODE_NOISE)
            {
              /* Read Noise Counter */
              ((uint16_t *)(TSS_TSIL_METHOD_ROMDATA_CONTEXT.tsil_cpu16ChannelCounterBuffer))[u8Channel] = u16PEN;
            }
            else
            {
              /* Fix errata on L2KM an L4KM family TSI */
            #if defined(TSS_MCU_SUBFAMILY_KLX6)
              if (((uint16_t) (psModuleStruct->DATA & TSI_DATA_TSICNT_MASK)) != 0xFFFFu)
            #endif
              {
                /* Read Capacitive Counter */
                ((uint16_t *)(TSS_TSIL_METHOD_ROMDATA_CONTEXT.tsil_cpu16ChannelCounterBuffer))[u8Channel] = (uint16_t) (psModuleStruct->DATA & TSI_DATA_TSICNT_MASK);
              }
            }

            /* Read PEN */
            u16PEN = TSS_TSIL_METHOD_RAMDATA_CONTEXT.tsil_u16PEN;
            /* Find next electrode */
            u8Timeout = 18u;
            do
            {
              if (u8Channel < 15u)
              {
                u8Channel++;
              }
              else
              {
                u8Channel = 0u;
              }
              u8Timeout--;
            } while (((u16PEN & (uint16_t)(1u << u8Channel)) == 0u) && (u8Timeout > 0u));
            /* Evaluate */
            if (u8Channel == TSS_TSIL_METHOD_RAMDATA_CONTEXT.tsil_u8StartElectrode)
            {
              #ifdef _TSS_TEST
                _TSS_TSIGEN_OFF;  /* Stop generator */
              #endif
              TSS_TSIL_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState = TSIL_EOSF_STATE_REGULAR;
              TSS_TSIL_METHOD_RAMDATA_CONTEXT.tsil_eScanInProgressState = TSIL_SCNIP_STATE_NONE;
              if ((TSS_TSIL_METHOD_ROMDATA_CONTEXT.tsil_u8TriggerSource == 1u) && (tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_AUTO))
              {
                /* Enable HW trigger for period measurement in the case of AUTO trigger */
                psModuleStruct->GENCS |= TSI_GENCS_STM_MASK;  /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
              }
            }
            else
            {
              #if (TSS_USE_NOISE_MODE == 1)
                if (TSS_TSIL_METHOD_RAMDATA_CONTEXT.tsil_u8ActiveMode == TSS_MODE_NOISE)
                {
                  psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK; /* Enable/Disable TSI & Enable noise mode */
                  psModuleStruct->GENCS |= TSI_GENCS_TSIEN_MASK;
                  psModuleStruct->GENCS |= TSI_GENCS_MODE(TSS_TSIL_MODE_NOISE_AUTO);
                }
              #endif
              psModuleStruct->DATA &= ~((uint32_t)TSI_DATA_TSICH_MASK); /* Erase TSICH */
              psModuleStruct->DATA |= TSI_DATA_TSICH(u8Channel); /* Set Channel */
              psModuleStruct->DATA |= TSI_DATA_SWTS_MASK;  /* Toggle SW trigger */
            }
          }
          else
          {
            /* Indication of short autotrigger period */
            TSS_TSIL_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState = TSIL_EOSF_STATE_ERROR;
          }
        }

        /* Set Sample Interrupted flag, because TSI measures at background and it can interrupt sampling of GPIO based methods */
        TSS_SET_SAMPLE_INTERRUPTED();
      }
    #endif

    #if TSS_DETECT_MODULE(TSIL0)
      #if TSS_ENABLE_DIAGNOSTIC_MESSAGES
        #warning TSS - Please assign ISR function TSS_TSI0Isr to vector INT_TSI0
      #endif
      void TSS_TSI0Isr(
        #if TSS_USE_MQX
          void *_isrParameter
        #else
          void
        #endif
        )
      {
        TSS_TSIL_MemMapPtr psModuleStruct;
        uint8_t u8Channel;
        uint16_t u16PEN;
        uint8_t u8Timeout;

        /* Pointers decoding */
        psModuleStruct = (TSS_TSIL_MemMapPtr) (TSS_TSIL0_METHOD_ROMDATA_CONTEXT.tsil_cpsModule);

        if (tss_CSSys.SystemConfig.LowPowerEn)
        {
          psModuleStruct->GENCS |= TSI_GENCS_OUTRGF_MASK;  /* Clear OUTFRG Flag */
          psModuleStruct->GENCS |= TSI_GENCS_ESOR_MASK;    /* End-of-Scan|Out-of-Range Interrupt Selection (0 = Out-of-Range allow, 1 = End-of-Scan allow) */
          psModuleStruct->GENCS &= ~TSI_GENCS_TSIIEN_MASK; /* Disable TSI Interrupt */
          /* Disables LowPower Wake Up */
          u16PEN = TSS_GetSystemConfig(System_SystemConfig_Register); /* Reuse of u16PEN for temp purpose */
          (void)TSS_SetSystemConfig(System_SystemConfig_Register,(u16PEN & ~TSS_LOWPOWER_EN_MASK));
        }
        else
        {
          /* Reuse (any store into GENC wipes out MODE value (noise counter) */
          u16PEN = (uint16_t)((psModuleStruct->GENCS & TSI_GENCS_MODE_MASK) >> TSI_GENCS_MODE_SHIFT);
          /* Clear EOSF flag */
          psModuleStruct->GENCS |= TSI_GENCS_EOSF_MASK;

          if (TSS_TSIL0_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState == TSIL_EOSF_STATE_NONE)
          {
            /* Disable HW trigger */
            psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_STM_MASK);   /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
            /* Save Actual channel */
            u8Channel = ((psModuleStruct->DATA & TSI_DATA_TSICH_MASK) >> TSI_DATA_TSICH_SHIFT);

            if (TSS_TSIL0_METHOD_RAMDATA_CONTEXT.tsil_u8ActiveMode == TSS_MODE_NOISE)
            {
              /* Read Noise Counter */
              ((uint16_t *)(TSS_TSIL0_METHOD_ROMDATA_CONTEXT.tsil_cpu16ChannelCounterBuffer))[u8Channel] = u16PEN;
            }
            else
            {
              /* Fix errata on L2KM an L4KM family TSI */
            #if defined(TSS_MCU_SUBFAMILY_KLX6)
              if (((uint16_t) (psModuleStruct->DATA & TSI_DATA_TSICNT_MASK)) != 0xFFFFu)
            #endif
              {
                /* Read Capacitive Counter */
                ((uint16_t *)(TSS_TSIL0_METHOD_ROMDATA_CONTEXT.tsil_cpu16ChannelCounterBuffer))[u8Channel] = (uint16_t) (psModuleStruct->DATA & TSI_DATA_TSICNT_MASK);
              }
            }
            /* Read PEN */
            u16PEN = TSS_TSIL0_METHOD_RAMDATA_CONTEXT.tsil_u16PEN;
            /* Find next electrode */
            u8Timeout = 18u;
            do
            {
              if (u8Channel < 15u)
              {
                u8Channel++;
              }
              else
              {
                u8Channel = 0u;
              }
              u8Timeout--;
            } while (((u16PEN & (uint16_t)(1u << u8Channel)) == 0u) && (u8Timeout > 0u));
            /* Evaluate */
            if (u8Channel == TSS_TSIL0_METHOD_RAMDATA_CONTEXT.tsil_u8StartElectrode)
            {
              #ifdef _TSS_TEST
                _TSS_TSIGEN_OFF;                                   /* Stop generator */
              #endif
              TSS_TSIL0_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState = TSIL_EOSF_STATE_REGULAR;
              TSS_TSIL0_METHOD_RAMDATA_CONTEXT.tsil_eScanInProgressState = TSIL_SCNIP_STATE_NONE;
              if (tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_AUTO)
              {
                /* Enable HW trigger for period measurement in the case of AUTO trigger */
                psModuleStruct->GENCS |= TSI_GENCS_STM_MASK;   /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
              }
            }
            else
            {
              #if (TSS_USE_NOISE_MODE == 1)
                if (TSS_TSIL0_METHOD_RAMDATA_CONTEXT.tsil_u8ActiveMode == TSS_MODE_NOISE)
                {
                  psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK; /* Enable/Disable TSI & Enable noise mode */
                  psModuleStruct->GENCS |= TSI_GENCS_TSIEN_MASK;
                  psModuleStruct->GENCS |= TSI_GENCS_MODE(TSS_TSIL_MODE_NOISE_AUTO);
                }
              #endif
              psModuleStruct->DATA &= ~((uint32_t)TSI_DATA_TSICH_MASK);  /* Erase TSICH */
              psModuleStruct->DATA |= TSI_DATA_TSICH(u8Channel);       /* Set Channel */
              psModuleStruct->DATA |= TSI_DATA_SWTS_MASK;              /* Toggle SW trigger */
            }
          }
          else
          {
            /* Indication of short autotrigger period */
            TSS_TSIL0_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState = TSIL_EOSF_STATE_ERROR;
          }
        }
        /* Set Sample Interrupted flag, because TSIL measures at background and it can interrupt sampling of GPIO based methods */
        TSS_SET_SAMPLE_INTERRUPTED();
      }
    #endif

    #if TSS_DETECT_MODULE(TSIL1)
      #if TSS_ENABLE_DIAGNOSTIC_MESSAGES
        #warning TSS - Please assign ISR function TSS_TSI1Isr to vector INT_TSI1
      #endif
      void TSS_TSI1Isr(
      #if TSS_USE_MQX
        void *_isrParameter
      #else
        void
      #endif
      )
      {
        TSI_MemMapPtr psModuleStruct;
        uint8_t u8Channel;
        uint16_t u16PEN;
        uint8_t u8Timeout;

        /* Pointers decoding */
        psModuleStruct = (TSS_TSIL_MemMapPtr) (TSS_TSIL1_METHOD_ROMDATA_CONTEXT.tsil_cpsModule);

        if (tss_CSSys.SystemConfig.LowPowerEn)
        {
           psModuleStruct->GENCS |= TSI_GENCS_OUTRGF_MASK;  /* Clear OUTFRG Flag */
           psModuleStruct->GENCS |= TSI_GENCS_ESOR_MASK;    /* End-of-Scan|Out-of-Range Interrupt Selection (0 = Out-of-Range allow, 1 = End-of-Scan allow) */
           psModuleStruct->GENCS &= ~TSI_GENCS_TSIIEN_MASK; /* Disable TSI Interrupt */
          /* Disables LowPower Wake Up */
          u16PEN = TSS_GetSystemConfig(System_SystemConfig_Register); /* Reuse of u16PEN for temp purpose */
          (void)TSS_SetSystemConfig(System_SystemConfig_Register,(u16PEN & ~TSS_LOWPOWER_EN_MASK));
        }
        else
        {
          /* Reuse (any store into GENC wipes out MODE value (noise counter) */
          u16PEN = (uint16_t)((psModuleStruct->GENCS & TSI_GENCS_MODE_MASK) >> TSI_GENCS_MODE_SHIFT);
          /* Clear EOSF flag */
          psModuleStruct->GENCS |= TSI_GENCS_EOSF_MASK;

          if (TSS_TSIL1_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState == TSIL_EOSF_STATE_NONE)
          {
            /* Disable HW trigger */
            psModuleStruct->GENCS &= ~((uint32_t)TSI_GENCS_STM_MASK);  /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
            /* Save Actual channel */
            u8Channel = ((psModuleStruct->DATA & TSI_DATA_TSICH_MASK) >> TSI_DATA_TSICH_SHIFT);

            if (TSS_TSIL1_METHOD_RAMDATA_CONTEXT.tsil_u8ActiveMode == TSS_MODE_NOISE)
            {
              /* Read Noise Counter */
              ((uint16_t *)(TSS_TSIL1_METHOD_ROMDATA_CONTEXT.tsil_cpu16ChannelCounterBuffer))[u8Channel] = u16PEN;
            }
            else
            {
              /* Fix errata on L2KM an L4KM family TSI */
            #if defined(TSS_MCU_SUBFAMILY_KLX6)
              if (((uint16_t) (psModuleStruct->DATA & TSI_DATA_TSICNT_MASK)) != 0xFFFFu)
            #endif
              {
                /* Read Capacitive Counter */
                ((uint16_t *)(TSS_TSIL1_METHOD_ROMDATA_CONTEXT.tsil_cpu16ChannelCounterBuffer))[u8Channel] = (uint16_t) (psModuleStruct->DATA & TSI_DATA_TSICNT_MASK);
              }
            }
            /* Read PEN */
            u16PEN = TSS_TSIL1_METHOD_RAMDATA_CONTEXT.tsil_u16PEN;
            /* Find next electrode */
            u8Timeout = 18u;
            do
            {
              if (u8Channel < 15u)
              {
                u8Channel++;
              }
              else
              {
                u8Channel = 0u;
              }
              u8Timeout--;
            } while (((u16PEN & (uint16_t)(1u << u8Channel)) == 0u) && (u8Timeout > 0u));
            /* Evaluate */
            if (u8Channel == TSS_TSIL1_METHOD_RAMDATA_CONTEXT.tsil_u8StartElectrode)
            {
              #ifdef _TSS_TEST
                _TSS_TSIGEN_OFF;  /* Stop generator */
              #endif
              TSS_TSIL1_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState = TSIL_EOSF_STATE_REGULAR;
              TSS_TSIL1_METHOD_RAMDATA_CONTEXT.tsil_eScanInProgressState = TSIL_SCNIP_STATE_NONE;
              if ((TSS_TSIL1_METHOD_ROMDATA_CONTEXT.tsil_u8TriggerSource == 1u) && (tss_CSSys.SystemTrigger.TriggerMode == TSS_TRIGGER_MODE_AUTO))
              {
                /* Enable HW trigger for period measurement in the case of AUTO trigger */
                psModuleStruct->GENCS |= TSI_GENCS_STM_MASK;  /* Set Scan Trigger Mode (0 = SW Trigger, 1 = HW Trigger) */
              }
            }
            else
            {
              #if (TSS_USE_NOISE_MODE == 1)
                if (TSS_TSIL1_METHOD_RAMDATA_CONTEXT.tsil_u8ActiveMode == TSS_MODE_NOISE)
                {
                  psModuleStruct->GENCS &= ~TSI_GENCS_TSIEN_MASK; /* Enable/Disable TSI & Enable noise mode */
                  psModuleStruct->GENCS |= TSI_GENCS_TSIEN_MASK;
                  psModuleStruct->GENCS |= TSI_GENCS_MODE(TSS_TSIL_MODE_NOISE_AUTO);
                }
              #endif
              psModuleStruct->DATA &= ~((uint32_t)TSI_DATA_TSICH_MASK); /* Erase TSICH */
              psModuleStruct->DATA |= TSI_DATA_TSICH(u8Channel); /* Set Channel */
              psModuleStruct->DATA |= TSI_DATA_SWTS_MASK;  /* Toggle SW trigger */
            }
          }
          else
          {
            /* Indication of short autotrigger period */
            TSS_TSIL1_METHOD_RAMDATA_CONTEXT.tsil_eEndOfScanState = TSIL_EOSF_STATE_ERROR;
          }
        }

        /* Set Sample Interrupted flag, because TSI measures at background and it can interrupt sampling of GPIO based methods */
        TSS_SET_SAMPLE_INTERRUPTED();
      }
    #endif

  #else /* End of TSS_KINETIS_MCU || TSS_CFM_MCU */
    #error "TSS - Not supported MCU used"
  #endif

#endif /* End of TSS_DETECT_METHOD(TSIL) */
