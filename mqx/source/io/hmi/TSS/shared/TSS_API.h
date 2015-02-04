/***********************************************************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2006-2009 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
*********************************************************************************************************************//*!
*
* @file   TSS_API.h
*
* @brief  This header defines the structs, constants, Types and registers of the TSS library
*
* @version 1.0.91.0
*
* @date Dec-6-2012
*
*
***********************************************************************************************************************/

#ifndef TSS_API_H
  #define TSS_API_H

  #include "TSS_SystemSetup.h"
  #include "TSS_StatusCodes.h" /* Defines The Return Status Codes Used By The Library */
  #include "TSS_DataTypes.h"   /* Defines The Data Types */
  #include "TSS_Sensor.h"      /* Includes TSS Sensor */

  /*******************************************************
  ******************* Version Control ********************
  *******************************************************/

  #define __TSS__             3
  #define __TSS_MINOR__       1
  #define __TSS_PATCHLEVEL__  0

  /* Version = VVRRPP */
  #if defined(__TSS__)
    #if defined(__TSS_PATCHLEVEL__)
      #define __TSS_VERSION__ ( __TSS__ * 10000 \
                              + __TSS_MINOR__ *100 \
                              + __TSS_PATCHLEVEL__)
    #else
      #define __TSS_VERSION__ ( __TSS__ * 10000 \
                              + __TSS_MINOR__ *100 \
                              )
    #endif
  #endif

  /*******************************************************
   ****************** Types Definitions ******************
   *******************************************************/

  /* Electrode Control Types */
  #define TSS_CT_KEYPAD               1u
  #define TSS_CT_SLIDER               2u
  #define TSS_CT_ROTARY               3u
  #define TSS_CT_ASLIDER              4u
  #define TSS_CT_AROTARY              5u
  #define TSS_CT_MATRIX               6u

  /*******************************************************
   *********************** Macros ************************
   *******************************************************/

  /* Keypad Decoder Macros */
  #define TSS_KEYPAD_BUFFER_READ(destvar,kpcsStruct)      destvar = (kpcsStruct).BufferPtr[(kpcsStruct).BufferReadIndex]; \
                                                          (kpcsStruct).BufferReadIndex = (uint8_t) (((kpcsStruct).BufferReadIndex + 1) & 0x0Fu)
  #define TSS_KEYPAD_BUFFER_EMPTY(kpcsStruct)             ((kpcsStruct).BufferReadIndex == (kpcsStruct).BufferWriteIndex)

  /*******************************************************
   ************* Data structures enumerations ************
   *******************************************************/

  enum TSS_CSSystem_REGISTERS {
    System_Faults_Register = 0,
    System_SystemConfig_Register = 1,
    System_NSamples_Register = 3,
    System_DCTrackerRate_Register = 4,
    System_SlowDCTrackerFactor_Register = 5,
    System_ResponseTime_Register = 6,
    System_StuckKeyTimeout_Register = 7,
    System_LowPowerElectrode_Register = 8,
    System_LowPowerElectrodeSensitivity_Register = 9,
    System_SystemTrigger_Register = 10,
    System_Sensitivity_Register = 11,
    System_ElectrodeEnablers_Register = System_Sensitivity_Register + TSS_N_ELECTRODES,
    System_ElectrodeStatus_Register = System_ElectrodeEnablers_Register + (((TSS_N_ELECTRODES - 1) / 8) + 1),
    System_Baseline_Register = System_ElectrodeStatus_Register + (((TSS_N_ELECTRODES - 1) / 8) + 1),
    System_DCTrackerEnablers_Register = System_Baseline_Register + TSS_N_ELECTRODES,
    System_ConfigCheckSum_Register = System_DCTrackerEnablers_Register + (((TSS_N_ELECTRODES - 1) / 8) + 1),
    /* Removed enums for back-comaptibility */
    System_LowPowerScanPeriod_Register = 254,
    System_AutoTriggerModuloValue_Register = 255
  };

  enum TSS_CSKeypad_REGISTERS {
    Keypad_ControlId_Register = 0,
    Keypad_ControlConfig_Register = 1,
    Keypad_BufferReadIndex_Register = 2,
    Keypad_BufferWriteIndex_Register = 3,
    Keypad_Events_Register = 4,
    Keypad_MaxTouches_Register = 5,
    Keypad_AutoRepeatRate_Register = 6,
    Keypad_AutoRepeatStart_Register = 7,
    Keypad_BufferPtr_Register = 8
  };

  enum TSS_CSSlider_REGISTERS {
    Slider_ControlId_Register = 0,
    Slider_ControlConfig_Register = 1,
    Slider_DynamicStatus_Register = 2,
    Slider_StaticStatus_Register = 3,
    Slider_Events_Register = 4,
    Slider_AutoRepeatRate_Register = 5,
    Slider_MovementTimeout_Register = 6
  };

  enum TSS_CSRotary_REGISTERS {
    Rotary_ControlId_Register = 0,
    Rotary_ControlConfig_Register = 1,
    Rotary_DynamicStatus_Register = 2,
    Rotary_StaticStatus_Register = 3,
    Rotary_Events_Register = 4,
    Rotary_AutoRepeatRate_Register = 5,
    Rotary_MovementTimeout_Register = 6
  };

  enum TSS_CSASlider_REGISTERS {
    ASlider_ControlId_Register = 0,
    ASlider_ControlConfig_Register = 1,
    ASlider_DynamicStatus_Register = 2,
    ASlider_Position_Register = 3,
    ASlider_Events_Register = 4,
    ASlider_AutoRepeatRate_Register = 5,
    ASlider_MovementTimeout_Register = 6,
    ASlider_Range_Register = 7
  };

  enum TSS_CSARotary_REGISTERS {
    ARotary_ControlId_Register = 0,
    ARotary_ControlConfig_Register = 1,
    ARotary_DynamicStatus_Register = 2,
    ARotary_Position_Register = 3,
    ARotary_Events_Register = 4,
    ARotary_AutoRepeatRate_Register = 5,
    ARotary_MovementTimeout_Register = 6,
    ARotary_Range_Register = 7
  };

  enum TSS_CSMatrix_REGISTERS {
    Matrix_ControlId_Register = 0,
    Matrix_ControlConfig_Register = 1,
    Matrix_Events_Register = 2,
    Matrix_AutoRepeatRate_Register = 3,
    Matrix_MovementTimeout_Register = 4,
    Matrix_DynamicStatusX_Register = 5,
    Matrix_DynamicStatusY_Register = 6,
    Matrix_PositionX_Register = 7,
    Matrix_PositionY_Register = 8,
    Matrix_GestureDistanceX_Register = 9,
    Matrix_GestureDistanceY_Register = 10,
    Matrix_RangeX_Register = 11,
    Matrix_RangeY_Register = 12
  };

  /*******************************************************
   *********** Library Constants Definitions *************
   *******************************************************/

  /* System Register Constants */
  #define TSS_SYSTEM_EN_BIT                       15u    /* Bit 15 at SystemConfig */
  #define TSS_SWI_EN_BIT                          14u    /* Bit 14 at SystemConfig */
  #define TSS_DC_TRACKER_EN_BIT                   13u    /* Bit 13 at SystemConfig */
  #define TSS_STUCK_KEY_EN_BIT                    12u    /* Bit 12 at SystemConfig */
  #define TSS_WATER_TOLERANCE_EN_BIT              8u     /* Bit 8 at SystemConfig */
  #define TSS_PROXIMITY_EN_BIT                    7u     /* Bit 7 at SystemConfig */
  #define TSS_LOWPOWER_EN_BIT                     6u     /* Bit 6 at SystemConfig */
  #define TSS_HW_RECA_START_BIT                   2u     /* Bit 2 at SystemConfig */
  #define TSS_SYSTEM_RESET_BIT                    1u     /* Bit 1 at SystemConfig */
  #define TSS_MANUAL_RECA_START_BIT               0u     /* Bit 0 at SystemConfig */

  #define TSS_SWTRIGGER_BIT                       2u     /* Bit 2 at SystemTrigger */
  #define TSS_TRIGGER_MODE_BIT                    0u     /* Bit 0-1 at SystemTrigger */

  #define TSS_SYSTEM_EN_MASK                      ((uint16_t)(1u << TSS_SYSTEM_EN_BIT))
  #define TSS_SWI_EN_MASK                         ((uint16_t)(1u << TSS_SWI_EN_BIT))
  #define TSS_DC_TRACKER_EN_MASK                  ((uint16_t)(1u << TSS_DC_TRACKER_EN_BIT))
  #define TSS_STUCK_KEY_EN_MASK                   ((uint16_t)(1u << TSS_STUCK_KEY_EN_BIT))
  #define TSS_WATER_TOLERANCE_EN_MASK             ((uint16_t)(1u << TSS_WATER_TOLERANCE_EN_BIT))
  #define TSS_PROXIMITY_EN_MASK                   ((uint16_t)(1u << TSS_PROXIMITY_EN_BIT))
  #define TSS_LOWPOWER_EN_MASK                    ((uint16_t)(1u << TSS_LOWPOWER_EN_BIT))
  #define TSS_HW_RECA_START_MASK                  ((uint16_t)(1u << TSS_HW_RECA_START_BIT))
  #define TSS_SYSTEM_RESET_MASK                   ((uint16_t)(1u << TSS_SYSTEM_RESET_BIT))
  #define TSS_MANUAL_RECA_START_MASK              ((uint16_t)(1u << TSS_MANUAL_RECA_START_BIT))

  #define TSS_SWTRIGGER_MASK                      ((uint16_t)(1u << TSS_SWTRIGGER_BIT))
  #define TSS_TRIGGER_MODE_MASK                   ((uint16_t)(3u << TSS_TRIGGER_MODE_BIT))

  #define TSS_TRIGGER_MODE_AUTO                   0u
  #define TSS_TRIGGER_MODE_ALWAYS                 1u
  #define TSS_TRIGGER_MODE_SW                     2u

  /* KeyPad Constants */
  #define TSS_KEYPAD_CONTROL_EN_BIT               7u     /* Bit 7 at Keypad Control Config */
  #define TSS_KEYPAD_CALLBACK_EN_BIT              6u     /* Bit 6 at Keypad Control Config */

  #define TSS_KEYPAD_MAXTOUCHES_FLAG_BIT          7u     /* Bit 7 at Keypad Events */
  #define TSS_KEYPAD_BUFFER_OVF_FLAG_BIT          6u     /* Bit 6 at Keypad Events */
  #define TSS_KEYPAD_MAXTOUCHES_EN_BIT            4u     /* Bit 4 at Keypad Events */
  #define TSS_KEYPAD_BUFFER_OVF_EN_BIT            3u     /* Bit 3 at Keypad Events */
  #define TSS_KEYPAD_AUTOREPEAT_EN_BIT            2u     /* Bit 2 at Keypad Events */
  #define TSS_KEYPAD_RELEASE_EVENT_EN_BIT         1u     /* Bit 1 at Keypad Events */
  #define TSS_KEYPAD_TOUCH_EVENT_EN_BIT           0u     /* Bit 0 at Keypad Events */

  #define TSS_KEYPAD_CONTROL_EN_MASK              ((uint8_t)(1u << TSS_KEYPAD_CONTROL_EN_BIT))
  #define TSS_KEYPAD_CALLBACK_EN_MASK             ((uint8_t)(1u << TSS_KEYPAD_CALLBACK_EN_BIT))

  #define TSS_KEYPAD_MAXTOUCHES_FLAG_MASK         ((uint8_t)(1u << TSS_KEYPAD_MAXTOUCHES_FLAG_BIT))
  #define TSS_KEYPAD_BUFFER_OVF_FLAG_MASK         ((uint8_t)(1u << TSS_KEYPAD_BUFFER_OVF_FLAG_BIT))
  #define TSS_KEYPAD_MAXTOUCHES_EN_MASK           ((uint8_t)(1u << TSS_KEYPAD_MAXTOUCHES_EN_BIT))
  #define TSS_KEYPAD_BUFFER_OVF_EN_MASK           ((uint8_t)(1u << TSS_KEYPAD_BUFFER_OVF_EN_BIT))
  #define TSS_KEYPAD_AUTOREPEAT_EN_MASK           ((uint8_t)(1u << TSS_KEYPAD_AUTOREPEAT_EN_BIT))
  #define TSS_KEYPAD_RELEASE_EVENT_EN_MASK        ((uint8_t)(1u << TSS_KEYPAD_RELEASE_EVENT_EN_BIT))
  #define TSS_KEYPAD_TOUCH_EVENT_EN_MASK          ((uint8_t)(1u << TSS_KEYPAD_TOUCH_EVENT_EN_BIT))

  #define TSS_KEYPAD_BUFFER_EVENT_FLAG_BIT        7u

  #define TSS_KEYPAD_BUFFER_EVENT_FLAG_MASK       ((uint8_t)(1u << TSS_KEYPAD_BUFFER_EVENT_FLAG_BIT))
  #define TSS_KEYPAD_BUFFER_RELEASE_FLAG          ((uint8_t)(1u << TSS_KEYPAD_BUFFER_EVENT_FLAG_BIT))
  #define TSS_KEYPAD_BUFFER_TOUCH_FLAG            0

  #define TSS_KEYPAD_BUFFER_KEY_MASK              0x0F

  /* Slider Constants */
  #define TSS_SLIDER_CONTROL_EN_BIT               7u     /* Bit 7 at Control Config */
  #define TSS_SLIDER_CALLBACK_EN_BIT              6u     /* Bit 6 at Slider Control Config */

  #define TSS_SLIDER_RELEASE_EN_BIT               4u     /* Bit 4 at Slider Events */
  #define TSS_SLIDER_HOLD_AUTOREPEAT_EN_BIT       3u     /* Bit 3 at Slider Events */
  #define TSS_SLIDER_HOLD_EN_BIT                  2u     /* Bit 2 at Slider Events */
  #define TSS_SLIDER_MOVEMENT_EVENT_EN_BIT        1u     /* Bit 1 at Slider Events */
  #define TSS_SLIDER_INITIAL_TOUCH_EVENT_EN_BIT   0u     /* Bit 0 at Slider Events */

  #define TSS_SLIDER_CONTROL_EN_MASK              ((uint8_t)(1u << TSS_SLIDER_CONTROL_EN_BIT))
  #define TSS_SLIDER_CALLBACK_EN_MASK             ((uint8_t)(1u << TSS_SLIDER_CALLBACK_EN_BIT))

  #define TSS_SLIDER_RELEASE_EVENT_EN_MASK        ((uint8_t)(1u << TSS_SLIDER_RELEASE_EN_BIT))
  #define TSS_SLIDER_AUTOREPEAT_EN_MASK           ((uint8_t)(1u << TSS_SLIDER_HOLD_AUTOREPEAT_EN_BIT))
  #define TSS_SLIDER_HOLD_EN_MASK                 ((uint8_t)(1u << TSS_SLIDER_HOLD_EN_BIT))
  #define TSS_SLIDER_MOVEMENT_EVENT_EN_MASK       ((uint8_t)(1u << TSS_SLIDER_MOVEMENT_EVENT_EN_BIT))
  #define TSS_SLIDER_INITIAL_TOUCH_EVENT_EN_MASK  ((uint8_t)(1u << TSS_SLIDER_INITIAL_TOUCH_EVENT_EN_BIT))

  /* Rotary Constants */
  #define TSS_ROTARY_CONTROL_EN_BIT               7u     /* Bit 7 at Slider Control Config */
  #define TSS_ROTARY_CALLBACK_EN_BIT              6u     /* Bit 6 at Slider Control Config */

  #define TSS_ROTARY_RELEASE_EN_BIT               4u     /* Bit 4 at Slider Events */
  #define TSS_ROTARY_HOLD_AUTOREPEAT_EN_BIT       3u     /* Bit 3 at Slider Events */
  #define TSS_ROTARY_HOLD_EN_BIT                  2u     /* Bit 2 at Slider Events */
  #define TSS_ROTARY_MOVEMENT_EVENT_EN_BIT        1u     /* Bit 1 at Slider Events */
  #define TSS_ROTARY_INITIAL_TOUCH_EVENT_EN_BIT   0u     /* Bit 0 at Slider Events */

  #define TSS_ROTARY_CONTROL_EN_MASK              ((uint8_t)(1u << TSS_ROTARY_CONTROL_EN_BIT))
  #define TSS_ROTARY_CALLBACK_EN_MASK             ((uint8_t)(1u << TSS_ROTARY_CALLBACK_EN_BIT))

  #define TSS_ROTARY_RELEASE_EVENT_EN_MASK        ((uint8_t)(1u << TSS_ROTARY_RELEASE_EN_BIT))
  #define TSS_ROTARY_AUTOREPEAT_EN_MASK           ((uint8_t)(1u << TSS_ROTARY_HOLD_AUTOREPEAT_EN_BIT))
  #define TSS_ROTARY_HOLD_EN_MASK                 ((uint8_t)(1u << TSS_ROTARY_HOLD_EN_BIT))
  #define TSS_ROTARY_MOVEMENT_EVENT_EN_MASK       ((uint8_t)(1u << TSS_ROTARY_MOVEMENT_EVENT_EN_BIT))
  #define TSS_ROTARY_INITIAL_TOUCH_EVENT_EN_MASK  ((uint8_t)(1u << TSS_ROTARY_INITIAL_TOUCH_EVENT_EN_BIT))

  /* Analog Slider Constants */
  #define TSS_ASLIDER_CONTROL_EN_BIT               7u     /* Bit 7 at Analog Slider Control Config */
  #define TSS_ASLIDER_CALLBACK_EN_BIT              6u     /* Bit 6 at Analog Slider Control Config */

  #define TSS_ASLIDER_RELEASE_EN_BIT               4u     /* Bit 4 at Analog Slider Events */
  #define TSS_ASLIDER_HOLD_AUTOREPEAT_EN_BIT       3u     /* Bit 3 at Analog Slider Events */
  #define TSS_ASLIDER_HOLD_EN_BIT                  2u     /* Bit 2 at Analog Slider Events */
  #define TSS_ASLIDER_MOVEMENT_EVENT_EN_BIT        1u     /* Bit 1 at Analog Slider Events */
  #define TSS_ASLIDER_INITIAL_TOUCH_EVENT_EN_BIT   0u     /* Bit 0 at Analog Slider Events */

  #define TSS_ASLIDER_CONTROL_EN_MASK              ((uint8_t)(1u << TSS_ASLIDER_CONTROL_EN_BIT))
  #define TSS_ASLIDER_CALLBACK_EN_MASK             ((uint8_t)(1u << TSS_ASLIDER_CALLBACK_EN_BIT))

  #define TSS_ASLIDER_RELEASE_EVENT_EN_MASK        ((uint8_t)(1u << TSS_ASLIDER_RELEASE_EN_BIT))
  #define TSS_ASLIDER_AUTOREPEAT_EN_MASK           ((uint8_t)(1u << TSS_ASLIDER_HOLD_AUTOREPEAT_EN_BIT))
  #define TSS_ASLIDER_HOLD_EN_MASK                 ((uint8_t)(1u << TSS_ASLIDER_HOLD_EN_BIT))
  #define TSS_ASLIDER_MOVEMENT_EVENT_EN_MASK       ((uint8_t)(1u << TSS_ASLIDER_MOVEMENT_EVENT_EN_BIT))
  #define TSS_ASLIDER_INITIAL_TOUCH_EVENT_EN_MASK  ((uint8_t)(1u << TSS_ASLIDER_INITIAL_TOUCH_EVENT_EN_BIT))

  /* Analog Rotary Constants */
  #define TSS_AROTARY_CONTROL_EN_BIT               7u     /* Bit 7 at Analog Rotary Control Config */
  #define TSS_AROTARY_CALLBACK_EN_BIT              6u     /* Bit 6 at Analog Rotary Control Config */

  #define TSS_AROTARY_RELEASE_EN_BIT               4u     /* Bit 4 at Analog Rotary Events */
  #define TSS_AROTARY_HOLD_AUTOREPEAT_EN_BIT       3u     /* Bit 3 at Analog Rotary Events */
  #define TSS_AROTARY_HOLD_EN_BIT                  2u     /* Bit 2 at Analog Rotary Events */
  #define TSS_AROTARY_MOVEMENT_EVENT_EN_BIT        1u     /* Bit 1 at Analog Rotary Events */
  #define TSS_AROTARY_INITIAL_TOUCH_EVENT_EN_BIT   0u     /* Bit 0 at Analog Rotary Events */

  #define TSS_AROTARY_CONTROL_EN_MASK              ((uint8_t)(1u << TSS_AROTARY_CONTROL_EN_BIT))
  #define TSS_AROTARY_CALLBACK_EN_MASK             ((uint8_t)(1u << TSS_AROTARY_CALLBACK_EN_BIT))

  #define TSS_AROTARY_RELEASE_EVENT_EN_MASK        ((uint8_t)(1u << TSS_AROTARY_RELEASE_EN_BIT))
  #define TSS_AROTARY_AUTOREPEAT_EN_MASK           ((uint8_t)(1u << TSS_AROTARY_HOLD_AUTOREPEAT_EN_BIT))
  #define TSS_AROTARY_HOLD_EN_MASK                 ((uint8_t)(1u << TSS_AROTARY_HOLD_EN_BIT))
  #define TSS_AROTARY_MOVEMENT_EVENT_EN_MASK       ((uint8_t)(1u << TSS_AROTARY_MOVEMENT_EVENT_EN_BIT))
  #define TSS_AROTARY_INITIAL_TOUCH_EVENT_EN_MASK  ((uint8_t)(1u << TSS_AROTARY_INITIAL_TOUCH_EVENT_EN_BIT))

  /* Matrix Constants */
  #define TSS_MATRIX_CONTROL_EN_BIT               7u     /* Bit 7 at Matrix Control Config */
  #define TSS_MATRIX_CALLBACK_EN_BIT              6u     /* Bit 6 at Matrix Control Config */

  #define TSS_MATRIX_GESTURES_EN_BIT              5u     /* Bit 5 at Matrix Events */
  #define TSS_MATRIX_RELEASE_EN_BIT               4u     /* Bit 4 at Matrix Events */
  #define TSS_MATRIX_HOLD_AUTOREPEAT_EN_BIT       3u     /* Bit 3 at Matrix Events */
  #define TSS_MATRIX_HOLD_EN_BIT                  2u     /* Bit 2 at Matrix Events */
  #define TSS_MATRIX_MOVEMENT_EVENT_EN_BIT        1u     /* Bit 1 at Matrix Events */
  #define TSS_MATRIX_INITIAL_TOUCH_EVENT_EN_BIT   0u     /* Bit 0 at Matrix Events */

  #define TSS_MATRIX_CONTROL_EN_MASK              ((uint8_t)(1u << TSS_MATRIX_CONTROL_EN_BIT))
  #define TSS_MATRIX_CALLBACK_EN_MASK             ((uint8_t)(1u << TSS_MATRIX_CALLBACK_EN_BIT))

  #define TSS_MATRIX_GESTURES_EVENT_EN_MASK       ((uint8_t)(1u << TSS_MATRIX_GESTURES_EN_BIT))
  #define TSS_MATRIX_RELEASE_EVENT_EN_MASK        ((uint8_t)(1u << TSS_MATRIX_RELEASE_EN_BIT))
  #define TSS_MATRIX_AUTOREPEAT_EN_MASK           ((uint8_t)(1u << TSS_MATRIX_HOLD_AUTOREPEAT_EN_BIT))
  #define TSS_MATRIX_HOLD_EN_MASK                 ((uint8_t)(1u << TSS_MATRIX_HOLD_EN_BIT))
  #define TSS_MATRIX_MOVEMENT_EVENT_EN_MASK       ((uint8_t)(1u << TSS_MATRIX_MOVEMENT_EVENT_EN_BIT))
  #define TSS_MATRIX_INITIAL_TOUCH_EVENT_EN_MASK  ((uint8_t)(1u << TSS_MATRIX_INITIAL_TOUCH_EVENT_EN_BIT))

  /************** Back Compatibility Macros **************/

  #define TSS_KEYPAD_IDLE_EN_BIT                  5u
  #define TSS_KEYPAD_IDLE_EN_MASK                 ((uint8_t)(1u << TSS_KEYPAD_IDLE_EN_BIT))
  #define TSS_SLIDER_IDLE_SCAN                    5u
  #define TSS_SLIDER_IDLE_EN_MASK                 ((uint8_t)(1u << TSS_SLIDER_IDLE_SCAN))
  #define TSS_ROTARY_IDLE_SCAN                    5u
  #define TSS_ROTARY_IDLE_EN_MASK                 ((uint8_t)(1u << TSS_ROTARY_IDLE_SCAN))
  #define TSS_ASLIDER_IDLE_SCAN                   5u
  #define TSS_ASLIDER_IDLE_EN_MASK                ((uint8_t)(1u << TSS_ASLIDER_IDLE_SCAN))
  #define TSS_AROTARY_IDLE_SCAN                   5u
  #define TSS_AROTARY_IDLE_EN_MASK                ((uint8_t)(1u << TSS_AROTARY_IDLE_SCAN))
  #define TSS_MATRIX_IDLE_SCAN                    5u
  #define TSS_MATRIX_IDLE_EN_MASK                 ((uint8_t)(1u << TSS_MATRIX_IDLE_SCAN))

  /*************** System Setup Constants ****************/

  #define TSS_KEYPAD_EVENTS_BUFFER_SIZE           16u
  #define TSS_KEYPAD_CONTCONF_INIT                {0u,0u}
  #define TSS_KEYPAD_EVENTS_INIT                  {0u,0u,0u,0u,0u,0u,0u}

  #define TSS_SLIDER_CONTROL_INIT                 {0u,0u}
  #define TSS_SLIDER_DYN_INIT                     {0u,0u,0u}
  #define TSS_SLIDER_STAT_INIT                    {0u,0u,0u}
  #define TSS_SLIDER_EVENTS_INIT                  {0u,0u,0u,0u,0u}

  #define TSS_ASLIDER_CONTROL_INIT                {0u,0u}
  #define TSS_ASLIDER_DYN_INIT                    {0u,0u,0u}
  #define TSS_ASLIDER_EVENTS_INIT                 {0u,0u,0u,0u,0u,0u,0u}

  #define TSS_MATRIX_CONTROL_INIT                 {0u,0u}
  #define TSS_MATRIX_DYN_INIT                     {0u,0u,0u}
  #define TSS_MATRIX_EVENTS_INIT                  {0u,0u,0u,0u,0u,0u,0u,0u}

  /*******************************************************
   ************* Data structures definitions *************
   *******************************************************/

  #ifdef __HCS08__
    #pragma MESSAGE DISABLE C1106 /* WARNING C1106: Non-standard bitfield type */
  #endif

  /******* Definition of system's CS structure type ******/

  typedef struct {
    volatile const TSS_SYSTEM_FAULTS Faults;
    volatile const TSS_SYSTEM_SYSCONF SystemConfig;
    volatile const uint8_t NSamples;
    volatile const uint8_t DCTrackerRate;
    volatile const uint8_t SlowDCTrackerFactor;
    volatile const uint8_t ResponseTime;
    volatile const uint8_t StuckKeyTimeout;
    volatile const uint8_t LowPowerElectrode;
    volatile const uint8_t LowPowerElectrodeSensitivity;
    volatile const TSS_SYSTEM_TRIGGER SystemTrigger;
  } TSS_CSSystem;

  /****** Definitions of decoder's CS structure types ****/

  /* Keypad CS structure */
  typedef struct {
    const TSS_CONTROL_ID ControlId;
    const TSS_KEYPAD_CONTCONF ControlConfig;
          uint8_t BufferReadIndex;                /* KeyPad Buffer Read Index */
    const uint8_t BufferWriteIndex;               /* KeyPad Buffer Write Index */
    const TSS_KEYPAD_EVENTS Events;
    const uint8_t MaxTouches;
    const uint8_t AutoRepeatRate;
    const uint8_t AutoRepeatStart;
    const uint8_t * const BufferPtr;              /* KeyPad Buffer pointer */
  } TSS_CSKeypad;

  /* Slider CS structure */
  typedef struct{
    const TSS_CONTROL_ID ControlId;
    const TSS_SLIDER_CONTROL ControlConfig;
    const TSS_SLIDER_DYN DynamicStatus;
    const TSS_SLIDER_STAT StaticStatus;
    const TSS_SLIDER_EVENTS Events;
    const uint8_t AutoRepeatRate;
    const uint8_t MovementTimeout;
  } TSS_CSSlider;

  /* Public Rotary CS structure */
  typedef struct{
    const TSS_CONTROL_ID ControlId;
    const TSS_SLIDER_CONTROL ControlConfig;
    const TSS_SLIDER_DYN DynamicStatus;
    const TSS_SLIDER_STAT StaticStatus;
    const TSS_SLIDER_EVENTS Events;
    const uint8_t AutoRepeatRate;
    const uint8_t MovementTimeout;
  } TSS_CSRotary;

  /* Analog Slider CS structure */
  typedef struct{
    const TSS_CONTROL_ID ControlId;
    const TSS_ASLIDER_CONTROL ControlConfig;
    const TSS_ASLIDER_DYN DynamicStatus;
    const uint8_t Position;
    const TSS_ASLIDER_EVENTS Events;
    const uint8_t AutoRepeatRate;
    const uint8_t MovementTimeout;
    const uint8_t Range;
  } TSS_CSASlider;

  /* Analog Rotary CS structure */
  typedef struct{
    const TSS_CONTROL_ID ControlId;
    const TSS_ASLIDER_CONTROL ControlConfig;
    const TSS_ASLIDER_DYN DynamicStatus;
    const uint8_t Position;
    const TSS_ASLIDER_EVENTS Events;
    const uint8_t AutoRepeatRate;
    const uint8_t MovementTimeout;
    const uint8_t Range;
  } TSS_CSARotary;

  /* Matrix CS structure */
  typedef struct{
    const TSS_CONTROL_ID ControlId;
    const TSS_MATRIX_CONTROL ControlConfig;
    const TSS_MATRIX_EVENTS Events;
    const uint8_t AutoRepeatRate;
    const uint8_t MovementTimeout;
    const TSS_MATRIX_DYN DynamicStatusX;
    const TSS_MATRIX_DYN DynamicStatusY;
    const uint8_t PositionX;
    const uint8_t PositionY;
    const uint8_t GestureDistanceX;
    const uint8_t GestureDistanceY;
    const uint8_t RangeX;
    const uint8_t RangeY;
  } TSS_CSMatrix;

  /*******************************************************
   ********* Control's CS Structures Prototypes **********
   *******************************************************/

  /*
  *  This section includes the extern control structures that will ve used.
  *  Only the Structures needed are compiled according with the number of declared
  *  controls in SystemSetup.h.
  */

/* Includes one extern control structure per decoder declared */
/* Classes symbols for C++ wrapper */

#if TSS_N_CONTROLS > 0
  #if (TSS_C0_TYPE == TSS_CT_KEYPAD)
    extern TSS_CSKeypad TSS_C0_STRUCTURE;
    #ifndef TSS_C0_CLASS
      #define TSS_C0_CLASS TSSKeypad
    #endif
  #elif (TSS_C0_TYPE == TSS_CT_SLIDER)
    extern TSS_CSSlider TSS_C0_STRUCTURE;
    #ifndef TSS_C0_CLASS
      #define TSS_C0_CLASS TSSSlider
    #endif
  #elif (TSS_C0_TYPE == TSS_CT_ROTARY)
    extern TSS_CSRotary TSS_C0_STRUCTURE;
    #ifndef TSS_C0_CLASS
      #define TSS_C0_CLASS TSSRotary
    #endif
  #elif (TSS_C0_TYPE == TSS_CT_ASLIDER)
    extern TSS_CSASlider TSS_C0_STRUCTURE;
    #ifndef TSS_C0_CLASS
      #define TSS_C0_CLASS TSSASlider
    #endif
  #elif (TSS_C0_TYPE == TSS_CT_AROTARY)
    extern TSS_CSARotary TSS_C0_STRUCTURE;
    #ifndef TSS_C0_CLASS
      #define TSS_C0_CLASS TSSARotary
    #endif
  #else
    extern TSS_CSMatrix TSS_C0_STRUCTURE;
    #ifndef TSS_C0_CLASS
      #define TSS_C0_CLASS TSSMatrix
    #endif
  #endif
#endif

#if TSS_N_CONTROLS > 1
  #if (TSS_C1_TYPE == TSS_CT_KEYPAD)
    extern TSS_CSKeypad TSS_C1_STRUCTURE;
    #ifndef TSS_C1_CLASS
      #define TSS_C1_CLASS TSSKeypad
    #endif
  #elif (TSS_C1_TYPE == TSS_CT_SLIDER)
    extern TSS_CSSlider TSS_C1_STRUCTURE;
    #ifndef TSS_C1_CLASS
      #define TSS_C1_CLASS TSSSlider
    #endif
  #elif (TSS_C1_TYPE == TSS_CT_ROTARY)
    extern TSS_CSRotary TSS_C1_STRUCTURE;
    #ifndef TSS_C1_CLASS
      #define TSS_C1_CLASS TSSRotary
    #endif
  #elif (TSS_C1_TYPE == TSS_CT_ASLIDER)
    extern TSS_CSASlider TSS_C1_STRUCTURE;
    #ifndef TSS_C1_CLASS
      #define TSS_C1_CLASS TSSASlider
    #endif
  #elif (TSS_C1_TYPE == TSS_CT_AROTARY)
    extern TSS_CSARotary TSS_C1_STRUCTURE;
    #ifndef TSS_C1_CLASS
      #define TSS_C1_CLASS TSSARotary
    #endif
  #else
    extern TSS_CSMatrix TSS_C1_STRUCTURE;
    #ifndef TSS_C1_CLASS
      #define TSS_C1_CLASS TSSMatrix
    #endif
  #endif
#endif

#if TSS_N_CONTROLS > 2
  #if (TSS_C2_TYPE == TSS_CT_KEYPAD)
    extern TSS_CSKeypad TSS_C2_STRUCTURE;
    #ifndef TSS_C2_CLASS
      #define TSS_C2_CLASS TSSKeypad
    #endif
  #elif (TSS_C2_TYPE == TSS_CT_SLIDER)
    extern TSS_CSSlider TSS_C2_STRUCTURE;
    #ifndef TSS_C2_CLASS
      #define TSS_C2_CLASS TSSSlider
    #endif
  #elif (TSS_C2_TYPE == TSS_CT_ROTARY)
    extern TSS_CSRotary TSS_C2_STRUCTURE;
    #ifndef TSS_C2_CLASS
      #define TSS_C2_CLASS TSSRotary
    #endif
  #elif (TSS_C2_TYPE == TSS_CT_ASLIDER)
    extern TSS_CSASlider TSS_C2_STRUCTURE;
    #ifndef TSS_C2_CLASS
      #define TSS_C2_CLASS TSSASlider
    #endif
  #elif (TSS_C2_TYPE == TSS_CT_AROTARY)
    extern TSS_CSARotary TSS_C2_STRUCTURE;
    #ifndef TSS_C2_CLASS
      #define TSS_C2_CLASS TSSARotary
    #endif
  #else
    extern TSS_CSMatrix TSS_C2_STRUCTURE;
    #ifndef TSS_C2_CLASS
      #define TSS_C2_CLASS TSSMatrix
    #endif
  #endif
#endif

#if TSS_N_CONTROLS > 3
  #if (TSS_C3_TYPE == TSS_CT_KEYPAD)
    extern TSS_CSKeypad TSS_C3_STRUCTURE;
    #ifndef TSS_C3_CLASS
      #define TSS_C3_CLASS TSSKeypad
    #endif
  #elif (TSS_C3_TYPE == TSS_CT_SLIDER)
    extern TSS_CSSlider TSS_C3_STRUCTURE;
    #ifndef TSS_C3_CLASS
      #define TSS_C3_CLASS TSSSlider
    #endif
  #elif (TSS_C3_TYPE == TSS_CT_ROTARY)
    extern TSS_CSRotary TSS_C3_STRUCTURE;
    #ifndef TSS_C3_CLASS
      #define TSS_C3_CLASS TSSRotary
    #endif
  #elif (TSS_C3_TYPE == TSS_CT_ASLIDER)
    extern TSS_CSASlider TSS_C3_STRUCTURE;
    #ifndef TSS_C3_CLASS
      #define TSS_C3_CLASS TSSASlider
    #endif
  #elif (TSS_C3_TYPE == TSS_CT_AROTARY)
    extern TSS_CSARotary TSS_C3_STRUCTURE;
    #ifndef TSS_C3_CLASS
      #define TSS_C3_CLASS TSSARotary
    #endif
  #else
    extern TSS_CSMatrix TSS_C3_STRUCTURE;
    #ifndef TSS_C3_CLASS
      #define TSS_C3_CLASS TSSMatrix
    #endif
  #endif
#endif

#if TSS_N_CONTROLS > 4
  #if (TSS_C4_TYPE == TSS_CT_KEYPAD)
    extern TSS_CSKeypad TSS_C4_STRUCTURE;
    #ifndef TSS_C4_CLASS
      #define TSS_C4_CLASS TSSKeypad
    #endif
  #elif (TSS_C4_TYPE == TSS_CT_SLIDER)
    extern TSS_CSSlider TSS_C4_STRUCTURE;
    #ifndef TSS_C4_CLASS
      #define TSS_C4_CLASS TSSSlider
    #endif
  #elif (TSS_C4_TYPE == TSS_CT_ROTARY)
    extern TSS_CSRotary TSS_C4_STRUCTURE;
    #ifndef TSS_C4_CLASS
      #define TSS_C4_CLASS TSSRotary
    #endif
  #elif (TSS_C4_TYPE == TSS_CT_ASLIDER)
    extern TSS_CSASlider TSS_C4_STRUCTURE;
    #ifndef TSS_C4_CLASS
      #define TSS_C4_CLASS TSSASlider
    #endif
  #elif (TSS_C4_TYPE == TSS_CT_AROTARY)
    extern TSS_CSARotary TSS_C4_STRUCTURE;
    #ifndef TSS_C4_CLASS
      #define TSS_C4_CLASS TSSARotary
    #endif
  #else
    extern TSS_CSMatrix TSS_C4_STRUCTURE;
    #ifndef TSS_C4_CLASS
      #define TSS_C4_CLASS TSSMatrix
    #endif
  #endif
#endif

#if TSS_N_CONTROLS > 5
  #if (TSS_C5_TYPE == TSS_CT_KEYPAD)
    extern TSS_CSKeypad TSS_C5_STRUCTURE;
    #ifndef TSS_C5_CLASS
      #define TSS_C5_CLASS TSSKeypad
    #endif
  #elif (TSS_C5_TYPE == TSS_CT_SLIDER)
    extern TSS_CSSlider TSS_C5_STRUCTURE;
    #ifndef TSS_C5_CLASS
      #define TSS_C5_CLASS TSSSlider
    #endif
  #elif (TSS_C5_TYPE == TSS_CT_ROTARY)
    extern TSS_CSRotary TSS_C5_STRUCTURE;
    #ifndef TSS_C5_CLASS
      #define TSS_C5_CLASS TSSRotary
    #endif
  #elif (TSS_C5_TYPE == TSS_CT_ASLIDER)
    extern TSS_CSASlider TSS_C5_STRUCTURE;
    #ifndef TSS_C5_CLASS
      #define TSS_C5_CLASS TSSASlider
    #endif
  #elif (TSS_C5_TYPE == TSS_CT_AROTARY)
    extern TSS_CSARotary TSS_C5_STRUCTURE;
    #ifndef TSS_C5_CLASS
      #define TSS_C5_CLASS TSSARotary
    #endif
  #else
    extern TSS_CSMatrix TSS_C5_STRUCTURE;
    #ifndef TSS_C5_CLASS
      #define TSS_C5_CLASS TSSMatrix
    #endif
  #endif
#endif

#if TSS_N_CONTROLS > 6
  #if (TSS_C6_TYPE == TSS_CT_KEYPAD)
    extern TSS_CSKeypad TSS_C6_STRUCTURE;
    #ifndef TSS_C6_CLASS
      #define TSS_C6_CLASS TSSKeypad
    #endif
  #elif (TSS_C6_TYPE == TSS_CT_SLIDER)
    extern TSS_CSSlider TSS_C6_STRUCTURE;
    #ifndef TSS_C6_CLASS
      #define TSS_C6_CLASS TSSSlider
    #endif
  #elif (TSS_C6_TYPE == TSS_CT_ROTARY)
    extern TSS_CSRotary TSS_C6_STRUCTURE;
    #ifndef TSS_C6_CLASS
      #define TSS_C6_CLASS TSSRotary
    #endif
  #elif (TSS_C6_TYPE == TSS_CT_ASLIDER)
    extern TSS_CSASlider TSS_C6_STRUCTURE;
    #ifndef TSS_C6_CLASS
      #define TSS_C6_CLASS TSSASlider
    #endif
  #elif (TSS_C6_TYPE == TSS_CT_AROTARY)
    extern TSS_CSARotary TSS_C6_STRUCTURE;
    #ifndef TSS_C6_CLASS
      #define TSS_C6_CLASS TSSARotary
    #endif
  #else
    extern TSS_CSMatrix TSS_C6_STRUCTURE;
    #ifndef TSS_C6_CLASS
      #define TSS_C6_CLASS TSSMatrix
    #endif
  #endif
#endif

#if TSS_N_CONTROLS > 7
  #if (TSS_C7_TYPE == TSS_CT_KEYPAD)
    extern TSS_CSKeypad TSS_C7_STRUCTURE;
    #ifndef TSS_C7_CLASS
      #define TSS_C7_CLASS TSSKeypad
    #endif
  #elif (TSS_C7_TYPE == TSS_CT_SLIDER)
    extern TSS_CSSlider TSS_C7_STRUCTURE;
    #ifndef TSS_C7_CLASS
      #define TSS_C7_CLASS TSSSlider
    #endif
  #elif (TSS_C7_TYPE == TSS_CT_ROTARY)
    extern TSS_CSRotary TSS_C7_STRUCTURE;
    #ifndef TSS_C7_CLASS
      #define TSS_C7_CLASS TSSRotary
    #endif
  #elif (TSS_C7_TYPE == TSS_CT_ASLIDER)
    extern TSS_CSASlider TSS_C7_STRUCTURE;
    #ifndef TSS_C7_CLASS
      #define TSS_C7_CLASS TSSASlider
    #endif
  #elif (TSS_C7_TYPE == TSS_CT_AROTARY)
    extern TSS_CSARotary TSS_C7_STRUCTURE;
    #ifndef TSS_C7_CLASS
      #define TSS_C7_CLASS TSSARotary
    #endif
  #else
    extern TSS_CSMatrix TSS_C7_STRUCTURE;
    #ifndef TSS_C7_CLASS
      #define TSS_C7_CLASS TSSMatrix
    #endif
  #endif
#endif

#if TSS_N_CONTROLS > 8
  #if (TSS_C8_TYPE == TSS_CT_KEYPAD)
    extern TSS_CSKeypad TSS_C8_STRUCTURE;
    #ifndef TSS_C8_CLASS
      #define TSS_C8_CLASS TSSKeypad
    #endif
  #elif (TSS_C8_TYPE == TSS_CT_SLIDER)
    extern TSS_CSSlider TSS_C8_STRUCTURE;
    #ifndef TSS_C8_CLASS
      #define TSS_C8_CLASS TSSSlider
    #endif
  #elif (TSS_C8_TYPE == TSS_CT_ROTARY)
    extern TSS_CSRotary TSS_C8_STRUCTURE;
    #ifndef TSS_C8_CLASS
      #define TSS_C8_CLASS TSSRotary
    #endif
  #elif (TSS_C8_TYPE == TSS_CT_ASLIDER)
    extern TSS_CSASlider TSS_C8_STRUCTURE;
    #ifndef TSS_C8_CLASS
      #define TSS_C8_CLASS TSSASlider
    #endif
  #elif (TSS_C8_TYPE == TSS_CT_AROTARY)
    extern TSS_CSARotary TSS_C8_STRUCTURE;
    #ifndef TSS_C8_CLASS
      #define TSS_C8_CLASS TSSARotary
    #endif
  #else
    extern TSS_CSMatrix TSS_C8_STRUCTURE;
    #ifndef TSS_C8_CLASS
      #define TSS_C8_CLASS TSSMatrix
    #endif
  #endif
#endif

#if TSS_N_CONTROLS > 9
  #if (TSS_C9_TYPE == TSS_CT_KEYPAD)
    extern TSS_CSKeypad TSS_C9_STRUCTURE;
    #ifndef TSS_C9_CLASS
      #define TSS_C9_CLASS TSSKeypad
    #endif
  #elif (TSS_C9_TYPE == TSS_CT_SLIDER)
    extern TSS_CSSlider TSS_C9_STRUCTURE;
    #ifndef TSS_C9_CLASS
      #define TSS_C9_CLASS TSSSlider
    #endif
  #elif (TSS_C9_TYPE == TSS_CT_ROTARY)
    extern TSS_CSRotary TSS_C9_STRUCTURE;
    #ifndef TSS_C9_CLASS
      #define TSS_C9_CLASS TSSRotary
    #endif
  #elif (TSS_C9_TYPE == TSS_CT_ASLIDER)
    extern TSS_CSASlider TSS_C9_STRUCTURE;
    #ifndef TSS_C9_CLASS
      #define TSS_C9_CLASS TSSASlider
    #endif
  #elif (TSS_C9_TYPE == TSS_CT_AROTARY)
    extern TSS_CSARotary TSS_C9_STRUCTURE;
    #ifndef TSS_C9_CLASS
      #define TSS_C9_CLASS TSSARotary
    #endif
  #else
    extern TSS_CSMatrix TSS_C9_STRUCTURE;
    #ifndef TSS_C9_CLASS
      #define TSS_C9_CLASS TSSMatrix
    #endif
  #endif
#endif

#if TSS_N_CONTROLS > 10
  #if (TSS_C10_TYPE == TSS_CT_KEYPAD)
    extern TSS_CSKeypad TSS_C10_STRUCTURE;
    #ifndef TSS_C10_CLASS
      #define TSS_C10_CLASS TSSKeypad
    #endif
  #elif (TSS_C10_TYPE == TSS_CT_SLIDER)
    extern TSS_CSSlider TSS_C10_STRUCTURE;
    #ifndef TSS_C10_CLASS
      #define TSS_C10_CLASS TSSSlider
    #endif
  #elif (TSS_C10_TYPE == TSS_CT_ROTARY)
    extern TSS_CSRotary TSS_C10_STRUCTURE;
    #ifndef TSS_C10_CLASS
      #define TSS_C10_CLASS TSSRotary
    #endif
  #elif (TSS_C10_TYPE == TSS_CT_ASLIDER)
    extern TSS_CSASlider TSS_C10_STRUCTURE;
    #ifndef TSS_C10_CLASS
      #define TSS_C10_CLASS TSSASlider
    #endif
  #elif (TSS_C10_TYPE == TSS_CT_AROTARY)
    extern TSS_CSARotary TSS_C10_STRUCTURE;
    #ifndef TSS_C10_CLASS
      #define TSS_C10_CLASS TSSARotary
    #endif
  #else
    extern TSS_CSMatrix TSS_C10_STRUCTURE;
    #ifndef TSS_C10_CLASS
      #define TSS_C10_CLASS TSSMatrix
    #endif
  #endif
#endif

#if TSS_N_CONTROLS > 11
  #if (TSS_C11_TYPE == TSS_CT_KEYPAD)
    extern TSS_CSKeypad TSS_C11_STRUCTURE;
    #ifndef TSS_C11_CLASS
      #define TSS_C11_CLASS TSSKeypad
    #endif
  #elif (TSS_C11_TYPE == TSS_CT_SLIDER)
    extern TSS_CSSlider TSS_C11_STRUCTURE;
    #ifndef TSS_C11_CLASS
      #define TSS_C11_CLASS TSSSlider
    #endif
  #elif (TSS_C11_TYPE == TSS_CT_ROTARY)
    extern TSS_CSRotary TSS_C11_STRUCTURE;
    #ifndef TSS_C11_CLASS
      #define TSS_C11_CLASS TSSRotary
    #endif
  #elif (TSS_C11_TYPE == TSS_CT_ASLIDER)
    extern TSS_CSASlider TSS_C11_STRUCTURE;
    #ifndef TSS_C11_CLASS
      #define TSS_C11_CLASS TSSASlider
    #endif
  #elif (TSS_C11_TYPE == TSS_CT_AROTARY)
    extern TSS_CSARotary TSS_C11_STRUCTURE;
    #ifndef TSS_C11_CLASS
      #define TSS_C11_CLASS TSSARotary
    #endif
  #else
    extern TSS_CSMatrix TSS_C11_STRUCTURE;
    #ifndef TSS_C11_CLASS
      #define TSS_C11_CLASS TSSMatrix
    #endif
  #endif
#endif

#if TSS_N_CONTROLS > 12
  #if (TSS_C12_TYPE == TSS_CT_KEYPAD)
    extern TSS_CSKeypad TSS_C12_STRUCTURE;
    #ifndef TSS_C12_CLASS
      #define TSS_C12_CLASS TSSKeypad
    #endif
  #elif (TSS_C12_TYPE == TSS_CT_SLIDER)
    extern TSS_CSSlider TSS_C12_STRUCTURE;
    #ifndef TSS_C12_CLASS
      #define TSS_C12_CLASS TSSSlider
    #endif
  #elif (TSS_C12_TYPE == TSS_CT_ROTARY)
    extern TSS_CSRotary TSS_C12_STRUCTURE;
    #ifndef TSS_C12_CLASS
      #define TSS_C12_CLASS TSSRotary
    #endif
  #elif (TSS_C12_TYPE == TSS_CT_ASLIDER)
    extern TSS_CSASlider TSS_C12_STRUCTURE;
    #ifndef TSS_C12_CLASS
      #define TSS_C12_CLASS TSSASlider
    #endif
  #elif (TSS_C12_TYPE == TSS_CT_AROTARY)
    extern TSS_CSARotary TSS_C12_STRUCTURE;
    #ifndef TSS_C12_CLASS
      #define TSS_C12_CLASS TSSARotary
    #endif
  #else
    extern TSS_CSMatrix TSS_C12_STRUCTURE;
    #ifndef TSS_C12_CLASS
      #define TSS_C12_CLASS TSSMatrix
    #endif
  #endif
#endif

#if TSS_N_CONTROLS > 13
  #if (TSS_C13_TYPE == TSS_CT_KEYPAD)
    extern TSS_CSKeypad TSS_C13_STRUCTURE;
    #ifndef TSS_C13_CLASS
      #define TSS_C13_CLASS TSSKeypad
    #endif
  #elif (TSS_C13_TYPE == TSS_CT_SLIDER)
    extern TSS_CSSlider TSS_C13_STRUCTURE;
    #ifndef TSS_C13_CLASS
      #define TSS_C13_CLASS TSSSlider
    #endif
  #elif (TSS_C13_TYPE == TSS_CT_ROTARY)
    extern TSS_CSRotary TSS_C13_STRUCTURE;
    #ifndef TSS_C13_CLASS
      #define TSS_C13_CLASS TSSRotary
    #endif
  #elif (TSS_C13_TYPE == TSS_CT_ASLIDER)
    extern TSS_CSASlider TSS_C13_STRUCTURE;
    #ifndef TSS_C13_CLASS
      #define TSS_C13_CLASS TSSASlider
    #endif
  #elif (TSS_C13_TYPE == TSS_CT_AROTARY)
    extern TSS_CSARotary TSS_C13_STRUCTURE;
    #ifndef TSS_C13_CLASS
      #define TSS_C13_CLASS TSSARotary
    #endif
  #else
    extern TSS_CSMatrix TSS_C13_STRUCTURE;
    #ifndef TSS_C13_CLASS
      #define TSS_C13_CLASS TSSMatrix
    #endif
  #endif
#endif

#if TSS_N_CONTROLS > 14
  #if (TSS_C14_TYPE == TSS_CT_KEYPAD)
    extern TSS_CSKeypad TSS_C14_STRUCTURE;
    #ifndef TSS_C14_CLASS
      #define TSS_C14_CLASS TSSKeypad
    #endif
  #elif (TSS_C14_TYPE == TSS_CT_SLIDER)
    extern TSS_CSSlider TSS_C14_STRUCTURE;
    #ifndef TSS_C14_CLASS
      #define TSS_C14_CLASS TSSSlider
    #endif
  #elif (TSS_C14_TYPE == TSS_CT_ROTARY)
    extern TSS_CSRotary TSS_C14_STRUCTURE;
    #ifndef TSS_C14_CLASS
      #define TSS_C14_CLASS TSSRotary
    #endif
  #elif (TSS_C14_TYPE == TSS_CT_ASLIDER)
    extern TSS_CSASlider TSS_C14_STRUCTURE;
    #ifndef TSS_C14_CLASS
      #define TSS_C14_CLASS TSSASlider
    #endif
  #elif (TSS_C14_TYPE == TSS_CT_AROTARY)
    extern TSS_CSARotary TSS_C14_STRUCTURE;
    #ifndef TSS_C14_CLASS
      #define TSS_C14_CLASS TSSARotary
    #endif
  #else
    extern TSS_CSMatrix TSS_C14_STRUCTURE;
    #ifndef TSS_C14_CLASS
      #define TSS_C14_CLASS TSSMatrix
    #endif
  #endif
#endif

#if TSS_N_CONTROLS > 15
  #if (TSS_C15_TYPE == TSS_CT_KEYPAD)
    extern TSS_CSKeypad TSS_C15_STRUCTURE;
    #ifndef TSS_C15_CLASS
      #define TSS_C15_CLASS TSSKeypad
    #endif
  #elif (TSS_C15_TYPE == TSS_CT_SLIDER)
    extern TSS_CSSlider TSS_C15_STRUCTURE;
    #ifndef TSS_C15_CLASS
      #define TSS_C15_CLASS TSSSlider
    #endif
  #elif (TSS_C15_TYPE == TSS_CT_ROTARY)
    extern TSS_CSRotary TSS_C15_STRUCTURE;
    #ifndef TSS_C15_CLASS
      #define TSS_C15_CLASS TSSRotary
    #endif
  #elif (TSS_C15_TYPE == TSS_CT_ASLIDER)
    extern TSS_CSASlider TSS_C15_STRUCTURE;
    #ifndef TSS_C15_CLASS
      #define TSS_C15_CLASS TSSASlider
    #endif
  #elif (TSS_C15_TYPE == TSS_CT_AROTARY)
    extern TSS_CSARotary TSS_C15_STRUCTURE;
    #ifndef TSS_C15_CLASS
      #define TSS_C15_CLASS TSSARotary
    #endif
  #else
    extern TSS_CSMatrix TSS_C15_STRUCTURE;
    #ifndef TSS_C15_CLASS
      #define TSS_C15_CLASS TSSMatrix
    #endif
  #endif
#endif

  /*******************************************************
   ********** TSS Library Function Prototypes ************
   *******************************************************/
  #ifdef __cplusplus
    extern "C" {
  #endif

  uint8_t TSS_Task(void);
  /*
  function    TSS_Task
  brief:      Performs all the task related to the TSS library
  param:      void
  return:     Status Code (TSS_STATUS_OK, TSS_STATUS_PROCESSING)
  */

  uint8_t TSS_TaskSeq(void);
  /*
  function    TSS_TaskSeq
  brief:      Performs all the task related to the TSS library
  param:      void
  return:     Status Code (TSS_STATUS_OK, TSS_STATUS_PROCESSING)
  */

  uint8_t TSS_Init(void);
  /*
  function:   TSSInit
  brief:      Initializes all the hardware required for the TSS library
  param:      void
  return:     Status Code (TSS_STATUS_OK, TSS_STATUS_RECALIBRATION_FAULT)
  */

  uint8_t TSS_SetSystemConfig(uint8_t u8Parameter, uint16_t u16Value);
  /*
  function:   TSS_SetSystemConfig
  brief:      Configures one parameter of the TSS library
  param:      u8Parameter - Parameter that will be modified
              u16Value    - New value of the parameter
  return:     Status code
  */

  uint16_t TSS_GetSystemConfig(uint8_t u8Parameter);
  /*
  function:   TSS_GetSystemConfig
  brief:      Read one parameter of the proximity library
  param:      u8Parameter - Requested Parameter for reading
  return:     Desired result Value
  */

  int16_t TSS_LoadElectrodeData(uint8_t u8Electrode, void * pSourceAddr);
  /*
  function    TSS_LoadElectrodeData
  brief:      Copy system data to the TSS for one electrode
  param:      u8Electrode - electrode number
              *pSourceAddr - address of source area
  return:     number of required bytes, or fault
  */

  int16_t TSS_LoadAllSystemData(void * pSourceAddr);
  /*
  function    TSS_LoadAllSystemData
  brief:      Copy all system data to the TSS for one electrode
  param:      *pSourceAddr - address of source data
  return:     number of required bytes, or fault
  */

  int16_t TSS_StoreElectrodeData(uint8_t u8Electrode, void * pDestAddr, uint16_t u16Length);
  /*
  function    TSS_StoreElectrodeData
  brief:      Copy system data to defined memory area
  param:      u8Electrode - electrode number
              *pDestAddr - address of destination area
              u16Length - number of available bytes in the area
  return:     number of required bytes, or fault
  */

  int16_t TSS_StoreAllSystemData(void * pDestAddr, uint16_t u16Length);
  /*
  function    TSS_StoreAllSystemData
  brief:      Copy all system data to defined memory area
  param:      *pDestAddr - address of destination area
              u16Length - number of available bytes in the area
  return:     number of required bytes, or fault
  */

  int16_t TSS_LoadModulesData(void * pSourceAddr);
  /*
  function    TSS_LoadModulesData
  brief:      Copy all module data to the TSS
  param:      *pSourceAddr - address of source data
  return:     number of required bytes, or fault
  */

  int16_t TSS_StoreModulesData(void * pDestAddr, uint16_t u16Length);
  /*
  function    TSS_StoreModulesData
  brief:      Copy all module data to defined memory area
  param:      *pDestAddr - address of destination area
              u16Length - number of available bytes in the area
  return:     number of required bytes, or fault
  */

  uint8_t TSS_SetKeypadConfig(TSS_CONTROL_ID u8ControlId, uint8_t u8Parameter, uint8_t u8Value);
  /*
  function:   TSS_SetKeypadConfig
  brief:      Configures one parameter of the KeyPad control
  param:      u8ControlId - Control identifier
              u8Parameter - Parameter that will be modified
              u8Value     - New value of the parameter
  return:     Status code
  */

  uint16_t TSS_GetKeypadConfig(TSS_CONTROL_ID u8CtrlId, uint8_t u8Parameter);
  /*
  function    TSS_SetKeypadConfig
  brief:      Configurates a parameter of a Keypad Decoder
  param:      u8Ctrld     - Control Identifier
  param:      u8Parameter - Parameter that will be modified
  param:      u8Value     - New value
  return:     Result Value
  */

  uint8_t TSS_SetSliderConfig(TSS_CONTROL_ID u8ControlId, uint8_t u8Parameter, uint8_t u8Value);
  /*
  function:   TSS_SetSliderConfig
  brief:      Configures one parameter of the Slider control
  param:      u8ControlId - Control identifier
              u8Parameter - Parameter that will be modified
              u8Value     - New value of the parameter
  return:     Status code
  */

  uint16_t TSS_GetSliderConfig(TSS_CONTROL_ID u8CtrlId, uint8_t u8Parameter);
  /*
  function    TSS_GetSliderConfig
  brief:      Read a parameter of a Slider Decoder
  param:      u8Ctrld     - Control Identifier
  param:      u8Parameter - Parameter that will be modified
  return:     Result Value
  */

  #define TSS_SetRotaryConfig(u8ControlId, u8Parameter, u8Value) TSS_SetSliderConfig(u8ControlId, u8Parameter, u8Value)
  /* The same function is used for Slider and Rotary configuration */

  #define TSS_GetRotaryConfig(u8ControlId, u8Parameter) TSS_GetSliderConfig(u8ControlId, u8Parameter)
  /* The same function is used for Slider and Rotary configuration */

  uint8_t TSS_SetASliderConfig(TSS_CONTROL_ID u8CtrlId, uint8_t u8Parameter, uint8_t u8Value);
  /*
  function    TSS_SetASliderConfig
  brief:      Configurates a parameter of a Analog Slider Decoder
  param:      u8Ctrld     - Control Identifier
  param:      u8Parameter - Parameter that will be modified
  param:      u8Value     - New value
  return:     Configuration Status
  */

  uint16_t TSS_GetASliderConfig(TSS_CONTROL_ID u8CtrlId, uint8_t u8Parameter);
  /*
  function    TSS_GetASliderConfig
  brief:      Read a parameter of an Analog Slider Decoder
  param:      u8Ctrld     - Control Identifier
  param:      u8Parameter - Parameter that will be modified
  return:     Result Value
  */

  #define TSS_SetARotaryConfig(u8ControlId, u8Parameter, u8Value) TSS_SetASliderConfig(u8ControlId, u8Parameter, u8Value)
  #define TSS_GetARotaryConfig(u8ControlId, u8Parameter) TSS_GetASliderConfig(u8ControlId, u8Parameter)
  /* The same function is used for Analog Slider and Analog Rotary configuration */

  uint8_t TSS_SetMatrixConfig(TSS_CONTROL_ID u8CtrlId, uint8_t u8Parameter, uint8_t u8Value);
  /*
  function    TSS_SetMatrixConfig
  brief:      Configurates a parameter of a Matrix Decoder
  param:      u8Ctrld     - Control Identifier
  param:      u8Parameter - Parameter that will be modified
  param:      u8Value     - New value
  return:     Configuration Status
  */

  uint16_t TSS_GetMatrixConfig(TSS_CONTROL_ID u8CtrlId, uint8_t u8Parameter);
  /*
  function    TSS_GetMatrixConfig
  brief:      Read a parameter of a Matrix Decoder
  param:      u8Ctrld     - Control Identifier
  param:      u8Parameter - Parameter that will be modified
  return:     Result Value
  */

  uint8_t TSS_BrowseModules(TSS_ModulesBrowseCallback pFunction, uint8_t u8Command, uint8_t u8OnlyEnabledElectrodesFlag);
  /*
  function    TSS_BrowseModules
  brief:      Function goes through all electrodes and execute callback if
              new low level module is found
  param:      pFunction - pointer to Callback function
              u8Command - received command
              u8OnlyEnabledElectrodesFlag - defines if only enabled el. are checked
  return:     collected status
  */

  void* TSS_GetControlStruct(uint8_t u8CntrlNum);
 /*
  function    TSS_GetControlStruct
  brief:      Return Control's struct
  param:      u8CntrlNum     - Control number
  return:     pointer to the structure
  */

  void* TSS_InternalControlPrivateData(uint8_t u8CntrlNum, void *pData, uint8_t action);
  /*
  function:   TSS_GetControlPrivateData
  brief:      Get Control Private Data, or
              NULL if out of range
  param:      u8CntrlNum - Requested Control Number
  return:     void
  */

  void TSS_SetControlPrivateData(uint8_t u8CntrlNum, void *pData);
 /*
  function    TSS_SetControlPrivateData
  brief:      Assign pointer to the control as private data
  param:      u8CntrlNum     - Control number
  param:      u8ParamepDatater - Pointer to the data
  return:     void
  */

  void* TSS_GetControlPrivateData(uint8_t u8CntrlNum);
  /*
  function    TSS_GetControlPrivateData
  brief:      Return assigned data pointer to the control
  param:      u8CntrlNum     - Control number
  return:     Pointer to the data
  */


  uint8_t TSS_GetControlNumberOfElectrodes(uint8_t u8CntrlNum);
  /*
  function:   TSS_GetControlNumberOfElectrodes
  brief:      Get the number of electrodes in specific Control, or
              TSS_DECODER_ERROR_OUT_OF_RANGE
  param:      u8CntrlNum - Requested Control Number
  return:     Number of electrodes in specific control
  */

  uint8_t TSS_GetControlFirstElectrodeIndex(uint8_t u8CntrlNum);
  /*
  function:   TSS_GetControlFirstElectrodeIndex
  brief:      Get the index of the first electrode in the Control, or
              TSS_DECODER_ERROR_OUT_OF_RANGE
  param:      u8CntrlNum - Requested Control Number
  return:     Requested index
  */

  uint8_t TSS_GetControlLastElectrodeIndex(uint8_t u8CntrlNum);
  /*
  function:   TSS_GetControlLastElectrodeIndex
  brief:      Get the index of last electrode in the Control, or
              TSS_DECODER_ERROR_OUT_OF_RANGE
  param:      u8CntrlNum - Requested Control Number
  return:     Requested index
  */

  uint8_t TSS_GetControlElectrodeIndex(uint8_t u8CntrlNum, uint8_t u8ElecNum);
  /*
  function:   TSS_GetControlLastElectrodeIndex
  brief:      Get the index of electrode in the Control, or
              TSS_DECODER_ERROR_OUT_OF_RANGE
  param:      u8CntrlNum - input control number
              u8ElecNum - electrode number within control
  return:     Requested index, or TSS_DECODER_ERROR_OUT_OF_RANGE
  */

  uint16_t TSS_GetControlElectrodesStatus(uint8_t u8CntrlNum);
  /*
  function    TSS_GetControlElectrodesStatus
  brief:      Return touch status of electrode group in uint16 bitfield, or
              TSS_DECODER_ERROR_OUT_OF_RANGE
  param:      u8CntrlNum - control number
  return:     status of electrodes in the bitfield
  */

  #ifdef __cplusplus
    }
  #endif

  /**************** Back Compatibility Macros ****************/

  #define TSS_SystemConfig(u8Parameter, u8Value) TSS_SetSystemConfig(u8Parameter, u8Value)
  #define TSS_KeypadConfig(u8ControlId, u8Parameter, u8Value) TSS_SetKeypadConfig(u8ControlId, u8Parameter, u8Value)
  #define TSS_SliderConfig(u8ControlId, u8Parameter, u8Value) TSS_SetSliderConfig(u8ControlId, u8Parameter, u8Value)
  #define TSS_RotaryConfig(u8ControlId, u8Parameter, u8Value) TSS_SetRotaryConfig(u8ControlId, u8Parameter, u8Value)

  /***********************************************************
  **************** Automatic Macro control  ******************
  ***********************************************************/

  /* Push enabling of TSS_USE_DELTA_LOG if Analogue Control is used */
  #define TSS_DETECT_ASLIDER_CONTROL_USAGE(control)    (TSS_C##control##_TYPE == TSS_CT_ASLIDER)
  #define TSS_DETECT_AROTARY_CONTROL_USAGE(control)    (TSS_C##control##_TYPE == TSS_CT_AROTARY)
  #define TSS_DETECT_MATRIX_CONTROL_USAGE(control)     (TSS_C##control##_TYPE == TSS_CT_MATRIX)

  #if (defined(TSS_N_CONTROLS) && (TSS_N_CONTROLS > 0))

    #if TSS_N_CONTROLS > 0
      #undef TSS_ASLIDER_USED
      #undef TSS_AROTARY_USED
      #undef TSS_MATRIX_USED
      #define TSS_ASLIDER_USED                (TSS_DETECT_ASLIDER_CONTROL_USAGE(0))
      #define TSS_AROTARY_USED                (TSS_DETECT_AROTARY_CONTROL_USAGE(0))
      #define TSS_MATRIX_USED                 (TSS_DETECT_MATRIX_CONTROL_USAGE(0))
    #endif
    #if TSS_N_CONTROLS > 1
      #undef TSS_ASLIDER_USED
      #undef TSS_AROTARY_USED
      #undef TSS_MATRIX_USED
      #define TSS_ASLIDER_USED                (TSS_DETECT_ASLIDER_CONTROL_USAGE(0) || TSS_DETECT_ASLIDER_CONTROL_USAGE(1))
      #define TSS_AROTARY_USED                (TSS_DETECT_AROTARY_CONTROL_USAGE(0) || TSS_DETECT_AROTARY_CONTROL_USAGE(1))
      #define TSS_MATRIX_USED                 (TSS_DETECT_MATRIX_CONTROL_USAGE(0) || TSS_DETECT_MATRIX_CONTROL_USAGE(1))
    #endif
    #if TSS_N_CONTROLS > 2
      #undef TSS_ASLIDER_USED
      #undef TSS_AROTARY_USED
      #undef TSS_MATRIX_USED
      #define TSS_ASLIDER_USED                (TSS_DETECT_ASLIDER_CONTROL_USAGE(0) || TSS_DETECT_ASLIDER_CONTROL_USAGE(1) || TSS_DETECT_ASLIDER_CONTROL_USAGE(2))
      #define TSS_AROTARY_USED                (TSS_DETECT_AROTARY_CONTROL_USAGE(0) || TSS_DETECT_AROTARY_CONTROL_USAGE(1) || TSS_DETECT_AROTARY_CONTROL_USAGE(2))
      #define TSS_MATRIX_USED                 (TSS_DETECT_MATRIX_CONTROL_USAGE(0) || TSS_DETECT_MATRIX_CONTROL_USAGE(1) || TSS_DETECT_MATRIX_CONTROL_USAGE(2))
    #endif
    #if TSS_N_CONTROLS > 3
      #undef TSS_ASLIDER_USED
      #undef TSS_AROTARY_USED
      #undef TSS_MATRIX_USED
      #define TSS_ASLIDER_USED                (TSS_DETECT_ASLIDER_CONTROL_USAGE(0) || TSS_DETECT_ASLIDER_CONTROL_USAGE(1) || TSS_DETECT_ASLIDER_CONTROL_USAGE(2) || TSS_DETECT_ASLIDER_CONTROL_USAGE(3))
      #define TSS_AROTARY_USED                (TSS_DETECT_AROTARY_CONTROL_USAGE(0) || TSS_DETECT_AROTARY_CONTROL_USAGE(1) || TSS_DETECT_AROTARY_CONTROL_USAGE(2) || TSS_DETECT_AROTARY_CONTROL_USAGE(3))
      #define TSS_MATRIX_USED                 (TSS_DETECT_MATRIX_CONTROL_USAGE(0) || TSS_DETECT_MATRIX_CONTROL_USAGE(1) || TSS_DETECT_MATRIX_CONTROL_USAGE(2) || TSS_DETECT_MATRIX_CONTROL_USAGE(3))
    #endif
    #if TSS_N_CONTROLS > 4
      #undef TSS_ASLIDER_USED
      #undef TSS_AROTARY_USED
      #undef TSS_MATRIX_USED
      #define TSS_ASLIDER_USED                (TSS_DETECT_ASLIDER_CONTROL_USAGE(0) || TSS_DETECT_ASLIDER_CONTROL_USAGE(1) || TSS_DETECT_ASLIDER_CONTROL_USAGE(2) || TSS_DETECT_ASLIDER_CONTROL_USAGE(3) || TSS_DETECT_ASLIDER_CONTROL_USAGE(4))
      #define TSS_AROTARY_USED                (TSS_DETECT_AROTARY_CONTROL_USAGE(0) || TSS_DETECT_AROTARY_CONTROL_USAGE(1) || TSS_DETECT_AROTARY_CONTROL_USAGE(2) || TSS_DETECT_AROTARY_CONTROL_USAGE(3) || TSS_DETECT_AROTARY_CONTROL_USAGE(4))
      #define TSS_MATRIX_USED                 (TSS_DETECT_MATRIX_CONTROL_USAGE(0) || TSS_DETECT_MATRIX_CONTROL_USAGE(1) || TSS_DETECT_MATRIX_CONTROL_USAGE(2) || TSS_DETECT_MATRIX_CONTROL_USAGE(3) || TSS_DETECT_MATRIX_CONTROL_USAGE(4))
    #endif
    #if TSS_N_CONTROLS > 5
      #undef TSS_ASLIDER_USED
      #undef TSS_AROTARY_USED
      #undef TSS_MATRIX_USED
      #define TSS_ASLIDER_USED                (TSS_DETECT_ASLIDER_CONTROL_USAGE(0) || TSS_DETECT_ASLIDER_CONTROL_USAGE(1) || TSS_DETECT_ASLIDER_CONTROL_USAGE(2) || TSS_DETECT_ASLIDER_CONTROL_USAGE(3) || TSS_DETECT_ASLIDER_CONTROL_USAGE(4) || TSS_DETECT_ASLIDER_CONTROL_USAGE(5))
      #define TSS_AROTARY_USED                (TSS_DETECT_AROTARY_CONTROL_USAGE(0) || TSS_DETECT_AROTARY_CONTROL_USAGE(1) || TSS_DETECT_AROTARY_CONTROL_USAGE(2) || TSS_DETECT_AROTARY_CONTROL_USAGE(3) || TSS_DETECT_AROTARY_CONTROL_USAGE(4) || TSS_DETECT_AROTARY_CONTROL_USAGE(5))
      #define TSS_MATRIX_USED                 (TSS_DETECT_MATRIX_CONTROL_USAGE(0) || TSS_DETECT_MATRIX_CONTROL_USAGE(1) || TSS_DETECT_MATRIX_CONTROL_USAGE(2) || TSS_DETECT_MATRIX_CONTROL_USAGE(3) || TSS_DETECT_MATRIX_CONTROL_USAGE(4) || TSS_DETECT_MATRIX_CONTROL_USAGE(5))
    #endif
    #if TSS_N_CONTROLS > 6
      #undef TSS_ASLIDER_USED
      #undef TSS_AROTARY_USED
      #undef TSS_MATRIX_USED
      #define TSS_ASLIDER_USED                (TSS_DETECT_ASLIDER_CONTROL_USAGE(0) || TSS_DETECT_ASLIDER_CONTROL_USAGE(1) || TSS_DETECT_ASLIDER_CONTROL_USAGE(2) || TSS_DETECT_ASLIDER_CONTROL_USAGE(3) || TSS_DETECT_ASLIDER_CONTROL_USAGE(4) || TSS_DETECT_ASLIDER_CONTROL_USAGE(5) || TSS_DETECT_ASLIDER_CONTROL_USAGE(6))
      #define TSS_AROTARY_USED                (TSS_DETECT_AROTARY_CONTROL_USAGE(0) || TSS_DETECT_AROTARY_CONTROL_USAGE(1) || TSS_DETECT_AROTARY_CONTROL_USAGE(2) || TSS_DETECT_AROTARY_CONTROL_USAGE(3) || TSS_DETECT_AROTARY_CONTROL_USAGE(4) || TSS_DETECT_AROTARY_CONTROL_USAGE(5) || TSS_DETECT_AROTARY_CONTROL_USAGE(6))
      #define TSS_MATRIX_USED                 (TSS_DETECT_MATRIX_CONTROL_USAGE(0) || TSS_DETECT_MATRIX_CONTROL_USAGE(1) || TSS_DETECT_MATRIX_CONTROL_USAGE(2) || TSS_DETECT_MATRIX_CONTROL_USAGE(3) || TSS_DETECT_MATRIX_CONTROL_USAGE(4) || TSS_DETECT_MATRIX_CONTROL_USAGE(5) || TSS_DETECT_MATRIX_CONTROL_USAGE(6))
    #endif
    #if TSS_N_CONTROLS > 7
      #undef TSS_ASLIDER_USED
      #undef TSS_AROTARY_USED
      #undef TSS_MATRIX_USED
      #define TSS_ASLIDER_USED                (TSS_DETECT_ASLIDER_CONTROL_USAGE(0) || TSS_DETECT_ASLIDER_CONTROL_USAGE(1) || TSS_DETECT_ASLIDER_CONTROL_USAGE(2) || TSS_DETECT_ASLIDER_CONTROL_USAGE(3) || TSS_DETECT_ASLIDER_CONTROL_USAGE(4) || TSS_DETECT_ASLIDER_CONTROL_USAGE(5) || TSS_DETECT_ASLIDER_CONTROL_USAGE(6) || TSS_DETECT_ASLIDER_CONTROL_USAGE(7))
      #define TSS_AROTARY_USED                (TSS_DETECT_AROTARY_CONTROL_USAGE(0) || TSS_DETECT_AROTARY_CONTROL_USAGE(1) || TSS_DETECT_AROTARY_CONTROL_USAGE(2) || TSS_DETECT_AROTARY_CONTROL_USAGE(3) || TSS_DETECT_AROTARY_CONTROL_USAGE(4) || TSS_DETECT_AROTARY_CONTROL_USAGE(5) || TSS_DETECT_AROTARY_CONTROL_USAGE(6) || TSS_DETECT_AROTARY_CONTROL_USAGE(7))
      #define TSS_MATRIX_USED                 (TSS_DETECT_MATRIX_CONTROL_USAGE(0) || TSS_DETECT_MATRIX_CONTROL_USAGE(1) || TSS_DETECT_MATRIX_CONTROL_USAGE(2) || TSS_DETECT_MATRIX_CONTROL_USAGE(3) || TSS_DETECT_MATRIX_CONTROL_USAGE(4) || TSS_DETECT_MATRIX_CONTROL_USAGE(5) || TSS_DETECT_MATRIX_CONTROL_USAGE(6) || TSS_DETECT_MATRIX_CONTROL_USAGE(7))
    #endif
    #if TSS_N_CONTROLS > 8
      #undef TSS_ASLIDER_USED
      #undef TSS_AROTARY_USED
      #undef TSS_MATRIX_USED
      #define TSS_ASLIDER_USED                (TSS_DETECT_ASLIDER_CONTROL_USAGE(0) || TSS_DETECT_ASLIDER_CONTROL_USAGE(1) || TSS_DETECT_ASLIDER_CONTROL_USAGE(2) || TSS_DETECT_ASLIDER_CONTROL_USAGE(3) || TSS_DETECT_ASLIDER_CONTROL_USAGE(4) || TSS_DETECT_ASLIDER_CONTROL_USAGE(5) || TSS_DETECT_ASLIDER_CONTROL_USAGE(6) || TSS_DETECT_ASLIDER_CONTROL_USAGE(7) || TSS_DETECT_ASLIDER_CONTROL_USAGE(8))
      #define TSS_AROTARY_USED                (TSS_DETECT_AROTARY_CONTROL_USAGE(0) || TSS_DETECT_AROTARY_CONTROL_USAGE(1) || TSS_DETECT_AROTARY_CONTROL_USAGE(2) || TSS_DETECT_AROTARY_CONTROL_USAGE(3) || TSS_DETECT_AROTARY_CONTROL_USAGE(4) || TSS_DETECT_AROTARY_CONTROL_USAGE(5) || TSS_DETECT_AROTARY_CONTROL_USAGE(6) || TSS_DETECT_AROTARY_CONTROL_USAGE(7) || TSS_DETECT_AROTARY_CONTROL_USAGE(8))
      #define TSS_MATRIX_USED                 (TSS_DETECT_MATRIX_CONTROL_USAGE(0) || TSS_DETECT_MATRIX_CONTROL_USAGE(1) || TSS_DETECT_MATRIX_CONTROL_USAGE(2) || TSS_DETECT_MATRIX_CONTROL_USAGE(3) || TSS_DETECT_MATRIX_CONTROL_USAGE(4) || TSS_DETECT_MATRIX_CONTROL_USAGE(5) || TSS_DETECT_MATRIX_CONTROL_USAGE(6) || TSS_DETECT_MATRIX_CONTROL_USAGE(7) || TSS_DETECT_MATRIX_CONTROL_USAGE(8))
    #endif
    #if TSS_N_CONTROLS > 9
      #undef TSS_ASLIDER_USED
      #undef TSS_AROTARY_USED
      #undef TSS_MATRIX_USED
      #define TSS_ASLIDER_USED                (TSS_DETECT_ASLIDER_CONTROL_USAGE(0) || TSS_DETECT_ASLIDER_CONTROL_USAGE(1) || TSS_DETECT_ASLIDER_CONTROL_USAGE(2) || TSS_DETECT_ASLIDER_CONTROL_USAGE(3) || TSS_DETECT_ASLIDER_CONTROL_USAGE(4) || TSS_DETECT_ASLIDER_CONTROL_USAGE(5) || TSS_DETECT_ASLIDER_CONTROL_USAGE(6) || TSS_DETECT_ASLIDER_CONTROL_USAGE(7) || TSS_DETECT_ASLIDER_CONTROL_USAGE(8) || TSS_DETECT_ASLIDER_CONTROL_USAGE(9))
      #define TSS_AROTARY_USED                (TSS_DETECT_AROTARY_CONTROL_USAGE(0) || TSS_DETECT_AROTARY_CONTROL_USAGE(1) || TSS_DETECT_AROTARY_CONTROL_USAGE(2) || TSS_DETECT_AROTARY_CONTROL_USAGE(3) || TSS_DETECT_AROTARY_CONTROL_USAGE(4) || TSS_DETECT_AROTARY_CONTROL_USAGE(5) || TSS_DETECT_AROTARY_CONTROL_USAGE(6) || TSS_DETECT_AROTARY_CONTROL_USAGE(7) || TSS_DETECT_AROTARY_CONTROL_USAGE(8) || TSS_DETECT_AROTARY_CONTROL_USAGE(9))
      #define TSS_MATRIX_USED                 (TSS_DETECT_MATRIX_CONTROL_USAGE(0) || TSS_DETECT_MATRIX_CONTROL_USAGE(1) || TSS_DETECT_MATRIX_CONTROL_USAGE(2) || TSS_DETECT_MATRIX_CONTROL_USAGE(3) || TSS_DETECT_MATRIX_CONTROL_USAGE(4) || TSS_DETECT_MATRIX_CONTROL_USAGE(5) || TSS_DETECT_MATRIX_CONTROL_USAGE(6) || TSS_DETECT_MATRIX_CONTROL_USAGE(7) || TSS_DETECT_MATRIX_CONTROL_USAGE(8) || TSS_DETECT_MATRIX_CONTROL_USAGE(9))
    #endif
    #if TSS_N_CONTROLS > 10
      #undef TSS_ASLIDER_USED
      #undef TSS_AROTARY_USED
      #undef TSS_MATRIX_USED
      #define TSS_ASLIDER_USED                (TSS_DETECT_ASLIDER_CONTROL_USAGE(0) || TSS_DETECT_ASLIDER_CONTROL_USAGE(1) || TSS_DETECT_ASLIDER_CONTROL_USAGE(2) || TSS_DETECT_ASLIDER_CONTROL_USAGE(3) || TSS_DETECT_ASLIDER_CONTROL_USAGE(4) || TSS_DETECT_ASLIDER_CONTROL_USAGE(5) || TSS_DETECT_ASLIDER_CONTROL_USAGE(6) || TSS_DETECT_ASLIDER_CONTROL_USAGE(7) || TSS_DETECT_ASLIDER_CONTROL_USAGE(8) || TSS_DETECT_ASLIDER_CONTROL_USAGE(9) || TSS_DETECT_ASLIDER_CONTROL_USAGE(10))
      #define TSS_AROTARY_USED                (TSS_DETECT_AROTARY_CONTROL_USAGE(0) || TSS_DETECT_AROTARY_CONTROL_USAGE(1) || TSS_DETECT_AROTARY_CONTROL_USAGE(2) || TSS_DETECT_AROTARY_CONTROL_USAGE(3) || TSS_DETECT_AROTARY_CONTROL_USAGE(4) || TSS_DETECT_AROTARY_CONTROL_USAGE(5) || TSS_DETECT_AROTARY_CONTROL_USAGE(6) || TSS_DETECT_AROTARY_CONTROL_USAGE(7) || TSS_DETECT_AROTARY_CONTROL_USAGE(8) || TSS_DETECT_AROTARY_CONTROL_USAGE(9) || TSS_DETECT_AROTARY_CONTROL_USAGE(10))
      #define TSS_MATRIX_USED                 (TSS_DETECT_MATRIX_CONTROL_USAGE(0) || TSS_DETECT_MATRIX_CONTROL_USAGE(1) || TSS_DETECT_MATRIX_CONTROL_USAGE(2) || TSS_DETECT_MATRIX_CONTROL_USAGE(3) || TSS_DETECT_MATRIX_CONTROL_USAGE(4) || TSS_DETECT_MATRIX_CONTROL_USAGE(5) || TSS_DETECT_MATRIX_CONTROL_USAGE(6) || TSS_DETECT_MATRIX_CONTROL_USAGE(7) || TSS_DETECT_MATRIX_CONTROL_USAGE(8) || TSS_DETECT_MATRIX_CONTROL_USAGE(9) || TSS_DETECT_MATRIX_CONTROL_USAGE(10))
    #endif
    #if TSS_N_CONTROLS > 11
      #undef TSS_ASLIDER_USED
      #undef TSS_AROTARY_USED
      #undef TSS_MATRIX_USED
      #define TSS_ASLIDER_USED                (TSS_DETECT_ASLIDER_CONTROL_USAGE(0) || TSS_DETECT_ASLIDER_CONTROL_USAGE(1) || TSS_DETECT_ASLIDER_CONTROL_USAGE(2) || TSS_DETECT_ASLIDER_CONTROL_USAGE(3) || TSS_DETECT_ASLIDER_CONTROL_USAGE(4) || TSS_DETECT_ASLIDER_CONTROL_USAGE(5) || TSS_DETECT_ASLIDER_CONTROL_USAGE(6) || TSS_DETECT_ASLIDER_CONTROL_USAGE(7) || TSS_DETECT_ASLIDER_CONTROL_USAGE(8) || TSS_DETECT_ASLIDER_CONTROL_USAGE(9) || TSS_DETECT_ASLIDER_CONTROL_USAGE(10) || TSS_DETECT_ASLIDER_CONTROL_USAGE(11))
      #define TSS_AROTARY_USED                (TSS_DETECT_AROTARY_CONTROL_USAGE(0) || TSS_DETECT_AROTARY_CONTROL_USAGE(1) || TSS_DETECT_AROTARY_CONTROL_USAGE(2) || TSS_DETECT_AROTARY_CONTROL_USAGE(3) || TSS_DETECT_AROTARY_CONTROL_USAGE(4) || TSS_DETECT_AROTARY_CONTROL_USAGE(5) || TSS_DETECT_AROTARY_CONTROL_USAGE(6) || TSS_DETECT_AROTARY_CONTROL_USAGE(7) || TSS_DETECT_AROTARY_CONTROL_USAGE(8) || TSS_DETECT_AROTARY_CONTROL_USAGE(9) || TSS_DETECT_AROTARY_CONTROL_USAGE(10) || TSS_DETECT_AROTARY_CONTROL_USAGE(11))
      #define TSS_MATRIX_USED                 (TSS_DETECT_MATRIX_CONTROL_USAGE(0) || TSS_DETECT_MATRIX_CONTROL_USAGE(1) || TSS_DETECT_MATRIX_CONTROL_USAGE(2) || TSS_DETECT_MATRIX_CONTROL_USAGE(3) || TSS_DETECT_MATRIX_CONTROL_USAGE(4) || TSS_DETECT_MATRIX_CONTROL_USAGE(5) || TSS_DETECT_MATRIX_CONTROL_USAGE(6) || TSS_DETECT_MATRIX_CONTROL_USAGE(7) || TSS_DETECT_MATRIX_CONTROL_USAGE(8) || TSS_DETECT_MATRIX_CONTROL_USAGE(9) || TSS_DETECT_MATRIX_CONTROL_USAGE(10) || TSS_DETECT_MATRIX_CONTROL_USAGE(11))
    #endif
    #if TSS_N_CONTROLS > 12
      #undef TSS_ASLIDER_USED
      #undef TSS_AROTARY_USED
      #undef TSS_MATRIX_USED
      #define TSS_ASLIDER_USED                (TSS_DETECT_ASLIDER_CONTROL_USAGE(0) || TSS_DETECT_ASLIDER_CONTROL_USAGE(1) || TSS_DETECT_ASLIDER_CONTROL_USAGE(2) || TSS_DETECT_ASLIDER_CONTROL_USAGE(3) || TSS_DETECT_ASLIDER_CONTROL_USAGE(4) || TSS_DETECT_ASLIDER_CONTROL_USAGE(5) || TSS_DETECT_ASLIDER_CONTROL_USAGE(6) || TSS_DETECT_ASLIDER_CONTROL_USAGE(7) || TSS_DETECT_ASLIDER_CONTROL_USAGE(8) || TSS_DETECT_ASLIDER_CONTROL_USAGE(9) || TSS_DETECT_ASLIDER_CONTROL_USAGE(10) || TSS_DETECT_ASLIDER_CONTROL_USAGE(11) || TSS_DETECT_ASLIDER_CONTROL_USAGE(12))
      #define TSS_AROTARY_USED                (TSS_DETECT_AROTARY_CONTROL_USAGE(0) || TSS_DETECT_AROTARY_CONTROL_USAGE(1) || TSS_DETECT_AROTARY_CONTROL_USAGE(2) || TSS_DETECT_AROTARY_CONTROL_USAGE(3) || TSS_DETECT_AROTARY_CONTROL_USAGE(4) || TSS_DETECT_AROTARY_CONTROL_USAGE(5) || TSS_DETECT_AROTARY_CONTROL_USAGE(6) || TSS_DETECT_AROTARY_CONTROL_USAGE(7) || TSS_DETECT_AROTARY_CONTROL_USAGE(8) || TSS_DETECT_AROTARY_CONTROL_USAGE(9) || TSS_DETECT_AROTARY_CONTROL_USAGE(10) || TSS_DETECT_AROTARY_CONTROL_USAGE(11) || TSS_DETECT_AROTARY_CONTROL_USAGE(12))
      #define TSS_MATRIX_USED                 (TSS_DETECT_MATRIX_CONTROL_USAGE(0) || TSS_DETECT_MATRIX_CONTROL_USAGE(1) || TSS_DETECT_MATRIX_CONTROL_USAGE(2) || TSS_DETECT_MATRIX_CONTROL_USAGE(3) || TSS_DETECT_MATRIX_CONTROL_USAGE(4) || TSS_DETECT_MATRIX_CONTROL_USAGE(5) || TSS_DETECT_MATRIX_CONTROL_USAGE(6) || TSS_DETECT_MATRIX_CONTROL_USAGE(7) || TSS_DETECT_MATRIX_CONTROL_USAGE(8) || TSS_DETECT_MATRIX_CONTROL_USAGE(9) || TSS_DETECT_MATRIX_CONTROL_USAGE(10) || TSS_DETECT_MATRIX_CONTROL_USAGE(11) || TSS_DETECT_MATRIX_CONTROL_USAGE(12))
    #endif
    #if TSS_N_CONTROLS > 13
      #undef TSS_ASLIDER_USED
      #undef TSS_AROTARY_USED
      #undef TSS_MATRIX_USED
      #define TSS_ASLIDER_USED                (TSS_DETECT_ASLIDER_CONTROL_USAGE(0) || TSS_DETECT_ASLIDER_CONTROL_USAGE(1) || TSS_DETECT_ASLIDER_CONTROL_USAGE(2) || TSS_DETECT_ASLIDER_CONTROL_USAGE(3) || TSS_DETECT_ASLIDER_CONTROL_USAGE(4) || TSS_DETECT_ASLIDER_CONTROL_USAGE(5) || TSS_DETECT_ASLIDER_CONTROL_USAGE(6) || TSS_DETECT_ASLIDER_CONTROL_USAGE(7) || TSS_DETECT_ASLIDER_CONTROL_USAGE(8) || TSS_DETECT_ASLIDER_CONTROL_USAGE(9) || TSS_DETECT_ASLIDER_CONTROL_USAGE(10) || TSS_DETECT_ASLIDER_CONTROL_USAGE(11) || TSS_DETECT_ASLIDER_CONTROL_USAGE(12) || TSS_DETECT_ASLIDER_CONTROL_USAGE(13))
      #define TSS_AROTARY_USED                (TSS_DETECT_AROTARY_CONTROL_USAGE(0) || TSS_DETECT_AROTARY_CONTROL_USAGE(1) || TSS_DETECT_AROTARY_CONTROL_USAGE(2) || TSS_DETECT_AROTARY_CONTROL_USAGE(3) || TSS_DETECT_AROTARY_CONTROL_USAGE(4) || TSS_DETECT_AROTARY_CONTROL_USAGE(5) || TSS_DETECT_AROTARY_CONTROL_USAGE(6) || TSS_DETECT_AROTARY_CONTROL_USAGE(7) || TSS_DETECT_AROTARY_CONTROL_USAGE(8) || TSS_DETECT_AROTARY_CONTROL_USAGE(9) || TSS_DETECT_AROTARY_CONTROL_USAGE(10) || TSS_DETECT_AROTARY_CONTROL_USAGE(11) || TSS_DETECT_AROTARY_CONTROL_USAGE(12) || TSS_DETECT_AROTARY_CONTROL_USAGE(13))
      #define TSS_MATRIX_USED                 (TSS_DETECT_MATRIX_CONTROL_USAGE(0) || TSS_DETECT_MATRIX_CONTROL_USAGE(1) || TSS_DETECT_MATRIX_CONTROL_USAGE(2) || TSS_DETECT_MATRIX_CONTROL_USAGE(3) || TSS_DETECT_MATRIX_CONTROL_USAGE(4) || TSS_DETECT_MATRIX_CONTROL_USAGE(5) || TSS_DETECT_MATRIX_CONTROL_USAGE(6) || TSS_DETECT_MATRIX_CONTROL_USAGE(7) || TSS_DETECT_MATRIX_CONTROL_USAGE(8) || TSS_DETECT_MATRIX_CONTROL_USAGE(9) || TSS_DETECT_MATRIX_CONTROL_USAGE(10) || TSS_DETECT_MATRIX_CONTROL_USAGE(11) || TSS_DETECT_MATRIX_CONTROL_USAGE(12) || TSS_DETECT_MATRIX_CONTROL_USAGE(13))
    #endif
    #if TSS_N_CONTROLS > 14
      #undef TSS_ASLIDER_USED
      #undef TSS_AROTARY_USED
      #undef TSS_MATRIX_USED
      #define TSS_ASLIDER_USED                (TSS_DETECT_ASLIDER_CONTROL_USAGE(0) || TSS_DETECT_ASLIDER_CONTROL_USAGE(1) || TSS_DETECT_ASLIDER_CONTROL_USAGE(2) || TSS_DETECT_ASLIDER_CONTROL_USAGE(3) || TSS_DETECT_ASLIDER_CONTROL_USAGE(4) || TSS_DETECT_ASLIDER_CONTROL_USAGE(5) || TSS_DETECT_ASLIDER_CONTROL_USAGE(6) || TSS_DETECT_ASLIDER_CONTROL_USAGE(7) || TSS_DETECT_ASLIDER_CONTROL_USAGE(8) || TSS_DETECT_ASLIDER_CONTROL_USAGE(9) || TSS_DETECT_ASLIDER_CONTROL_USAGE(10) || TSS_DETECT_ASLIDER_CONTROL_USAGE(11) || TSS_DETECT_ASLIDER_CONTROL_USAGE(12) || TSS_DETECT_ASLIDER_CONTROL_USAGE(13) || TSS_DETECT_ASLIDER_CONTROL_USAGE(14))
      #define TSS_AROTARY_USED                (TSS_DETECT_AROTARY_CONTROL_USAGE(0) || TSS_DETECT_AROTARY_CONTROL_USAGE(1) || TSS_DETECT_AROTARY_CONTROL_USAGE(2) || TSS_DETECT_AROTARY_CONTROL_USAGE(3) || TSS_DETECT_AROTARY_CONTROL_USAGE(4) || TSS_DETECT_AROTARY_CONTROL_USAGE(5) || TSS_DETECT_AROTARY_CONTROL_USAGE(6) || TSS_DETECT_AROTARY_CONTROL_USAGE(7) || TSS_DETECT_AROTARY_CONTROL_USAGE(8) || TSS_DETECT_AROTARY_CONTROL_USAGE(9) || TSS_DETECT_AROTARY_CONTROL_USAGE(10) || TSS_DETECT_AROTARY_CONTROL_USAGE(11) || TSS_DETECT_AROTARY_CONTROL_USAGE(12) || TSS_DETECT_AROTARY_CONTROL_USAGE(13) || TSS_DETECT_AROTARY_CONTROL_USAGE(14))
      #define TSS_MATRIX_USED                 (TSS_DETECT_MATRIX_CONTROL_USAGE(0) || TSS_DETECT_MATRIX_CONTROL_USAGE(1) || TSS_DETECT_MATRIX_CONTROL_USAGE(2) || TSS_DETECT_MATRIX_CONTROL_USAGE(3) || TSS_DETECT_MATRIX_CONTROL_USAGE(4) || TSS_DETECT_MATRIX_CONTROL_USAGE(5) || TSS_DETECT_MATRIX_CONTROL_USAGE(6) || TSS_DETECT_MATRIX_CONTROL_USAGE(7) || TSS_DETECT_MATRIX_CONTROL_USAGE(8) || TSS_DETECT_MATRIX_CONTROL_USAGE(9) || TSS_DETECT_MATRIX_CONTROL_USAGE(10) || TSS_DETECT_MATRIX_CONTROL_USAGE(11) || TSS_DETECT_MATRIX_CONTROL_USAGE(12) || TSS_DETECT_MATRIX_CONTROL_USAGE(13) || TSS_DETECT_MATRIX_CONTROL_USAGE(14))
    #endif
    #if TSS_N_CONTROLS > 15
      #undef TSS_ASLIDER_USED
      #undef TSS_AROTARY_USED
      #undef TSS_MATRIX_USED
      #define TSS_ASLIDER_USED                (TSS_DETECT_ASLIDER_CONTROL_USAGE(0) || TSS_DETECT_ASLIDER_CONTROL_USAGE(1) || TSS_DETECT_ASLIDER_CONTROL_USAGE(2) || TSS_DETECT_ASLIDER_CONTROL_USAGE(3) || TSS_DETECT_ASLIDER_CONTROL_USAGE(4) || TSS_DETECT_ASLIDER_CONTROL_USAGE(5) || TSS_DETECT_ASLIDER_CONTROL_USAGE(6) || TSS_DETECT_ASLIDER_CONTROL_USAGE(7) || TSS_DETECT_ASLIDER_CONTROL_USAGE(8) || TSS_DETECT_ASLIDER_CONTROL_USAGE(9) || TSS_DETECT_ASLIDER_CONTROL_USAGE(10) || TSS_DETECT_ASLIDER_CONTROL_USAGE(11) || TSS_DETECT_ASLIDER_CONTROL_USAGE(12) || TSS_DETECT_ASLIDER_CONTROL_USAGE(13) || TSS_DETECT_ASLIDER_CONTROL_USAGE(14) || TSS_DETECT_ASLIDER_CONTROL_USAGE(15))
      #define TSS_AROTARY_USED                (TSS_DETECT_AROTARY_CONTROL_USAGE(0) || TSS_DETECT_AROTARY_CONTROL_USAGE(1) || TSS_DETECT_AROTARY_CONTROL_USAGE(2) || TSS_DETECT_AROTARY_CONTROL_USAGE(3) || TSS_DETECT_AROTARY_CONTROL_USAGE(4) || TSS_DETECT_AROTARY_CONTROL_USAGE(5) || TSS_DETECT_AROTARY_CONTROL_USAGE(6) || TSS_DETECT_AROTARY_CONTROL_USAGE(7) || TSS_DETECT_AROTARY_CONTROL_USAGE(8) || TSS_DETECT_AROTARY_CONTROL_USAGE(9) || TSS_DETECT_AROTARY_CONTROL_USAGE(10) || TSS_DETECT_AROTARY_CONTROL_USAGE(11) || TSS_DETECT_AROTARY_CONTROL_USAGE(12) || TSS_DETECT_AROTARY_CONTROL_USAGE(13) || TSS_DETECT_AROTARY_CONTROL_USAGE(14) || TSS_DETECT_AROTARY_CONTROL_USAGE(15))
      #define TSS_MATRIX_USED                 (TSS_DETECT_MATRIX_CONTROL_USAGE(0) || TSS_DETECT_MATRIX_CONTROL_USAGE(1) || TSS_DETECT_MATRIX_CONTROL_USAGE(2) || TSS_DETECT_MATRIX_CONTROL_USAGE(3) || TSS_DETECT_MATRIX_CONTROL_USAGE(4) || TSS_DETECT_MATRIX_CONTROL_USAGE(5) || TSS_DETECT_MATRIX_CONTROL_USAGE(6) || TSS_DETECT_MATRIX_CONTROL_USAGE(7) || TSS_DETECT_MATRIX_CONTROL_USAGE(8) || TSS_DETECT_MATRIX_CONTROL_USAGE(9) || TSS_DETECT_MATRIX_CONTROL_USAGE(10) || TSS_DETECT_MATRIX_CONTROL_USAGE(11) || TSS_DETECT_MATRIX_CONTROL_USAGE(12) || TSS_DETECT_MATRIX_CONTROL_USAGE(13) || TSS_DETECT_MATRIX_CONTROL_USAGE(14) || TSS_DETECT_MATRIX_CONTROL_USAGE(15))
    #endif

    #define TSS_ACONTROL_USED   (TSS_ASLIDER_USED || TSS_AROTARY_USED || TSS_MATRIX_USED)

    #if (TSS_ACONTROL_USED && (TSS_USE_DELTA_LOG == 0))
      #if TSS_ENABLE_DIAGNOSTIC_MESSAGES
        #warning TSS - TSS_USE_DELTA_LOG macro was enabled due to usage by Matrix or Analogue Slider/Rotary control
      #endif
      #undef TSS_USE_DELTA_LOG
      #define TSS_USE_DELTA_LOG 1
    #endif

  #else

    #define TSS_ASLIDER_USED    0
    #define TSS_AROTARY_USED    0
    #define TSS_MATRIX_USED     0
    #define TSS_ACONTROL_USED   0

  #endif

  /***********************************************************
  ***** Detection of Legacy Controls Elecrodes defintion *****
  ***********************************************************/

  #define TSS_GET_EL1_CONTROL_INPUTS(x) x+0
  #define TSS_GET_EL2_CONTROL_INPUTS(x) x+0,x+1
  #define TSS_GET_EL3_CONTROL_INPUTS(x) x+0,x+1,x+2
  #define TSS_GET_EL4_CONTROL_INPUTS(x) x+0,x+1,x+2,x+3
  #define TSS_GET_EL5_CONTROL_INPUTS(x) x+0,x+1,x+2,x+3,x+4
  #define TSS_GET_EL6_CONTROL_INPUTS(x) x+0,x+1,x+2,x+3,x+4,x+5
  #define TSS_GET_EL7_CONTROL_INPUTS(x) x+0,x+1,x+2,x+3,x+4,x+5,x+6
  #define TSS_GET_EL8_CONTROL_INPUTS(x) x+0,x+1,x+2,x+3,x+4,x+5,x+6,x+7
  #define TSS_GET_EL9_CONTROL_INPUTS(x) x+0,x+1,x+2,x+3,x+4,x+5,x+6,x+7,x+8
  #define TSS_GET_EL10_CONTROL_INPUTS(x) x+0,x+1,x+2,x+3,x+4,x+5,x+6,x+7,x+8,x+9
  #define TSS_GET_EL11_CONTROL_INPUTS(x) x+0,x+1,x+2,x+3,x+4,x+5,x+6,x+7,x+8,x+9,x+10
  #define TSS_GET_EL12_CONTROL_INPUTS(x) x+0,x+1,x+2,x+3,x+4,x+5,x+6,x+7,x+8,x+9,x+10,x+11
  #define TSS_GET_EL13_CONTROL_INPUTS(x) x+0,x+1,x+2,x+3,x+4,x+5,x+6,x+7,x+8,x+9,x+10,x+11,x+12
  #define TSS_GET_EL14_CONTROL_INPUTS(x) x+0,x+1,x+2,x+3,x+4,x+5,x+6,x+7,x+8,x+9,x+10,x+11,x+12,x+13
  #define TSS_GET_EL15_CONTROL_INPUTS(x) x+0,x+1,x+2,x+3,x+4,x+5,x+6,x+7,x+8,x+9,x+10,x+11,x+12,x+13,x+14
  #define TSS_GET_EL16_CONTROL_INPUTS(x) x+0,x+1,x+2,x+3,x+4,x+5,x+6,x+7,x+8,x+9,x+10,x+11,x+12,x+13,x+14,x+15

  #define TSS_GET_EL_CONTROL_INPUTS_CONV(elnum,offset)   TSS_GET_EL##elnum##_CONTROL_INPUTS(offset)
  #define TSS_GET_EL_CONTROL_INPUTS(elnum,offset)        TSS_GET_EL_CONTROL_INPUTS_CONV(elnum,offset)

  #ifdef TSS_N_CONTROLS
    #define TSS_C0_OFFSET           0
    #if TSS_N_CONTROLS > 0
      #if defined(TSS_C0_ELECTRODES) || defined(TSS_C0_ELECTRODES_X) || defined(TSS_C0_ELECTRODES_Y)
        #define TSS_C0_OLD_EL_DEF_USED  1
        #ifdef TSS_C0_ELECTRODES
          #define TSS_C1_OFFSET           TSS_C0_OFFSET+TSS_C0_ELECTRODES
          #ifndef TSS_C0_INPUTS
            #define TSS_C0_INPUTS       {TSS_GET_EL_CONTROL_INPUTS(TSS_C0_ELECTRODES,TSS_C0_OFFSET)}
          #else
            #error TSS - TSS_C0_INPUTS is defined, but also old style definition by TSS_C0_ELECTRODES is used.
          #endif
        #endif
        #ifdef TSS_C0_ELECTRODES_X
          #ifndef TSS_C0_INPUTS_NUM_X
            #define TSS_C0_INPUTS_NUM_X     TSS_C0_ELECTRODES_X
          #else
            #error TSS - TSS_C0_INPUTS_NUM_X is defined, but also old style definition by TSS_C0_ELECTRODES_X is used.
          #endif
          #ifdef TSS_C0_ELECTRODES_Y
            #ifndef TSS_C0_ELECTRODES
              #define TSS_C0_ELECTRODES TSS_C0_ELECTRODES_X+TSS_C0_ELECTRODES_Y
            #endif
            #define TSS_C1_OFFSET           TSS_C0_OFFSET+TSS_C0_ELECTRODES
            #ifndef TSS_C0_INPUTS
              #define TSS_C0_INPUTS     {TSS_GET_EL_CONTROL_INPUTS(TSS_C0_ELECTRODES_X,TSS_C0_OFFSET),TSS_GET_EL_CONTROL_INPUTS(TSS_C0_ELECTRODES_Y,TSS_C0_OFFSET+TSS_C0_ELECTRODES_X)}
            #else
              #error TSS - TSS_C0_INPUTS is defined, but also old style definition by TSS_C0_ELECTRODES_X and TSS_C0_ELECTRODES_Y is used.
            #endif
          #endif
        #endif
      #endif
    #endif
    #if TSS_N_CONTROLS > 1
      #if defined(TSS_C1_ELECTRODES) || defined(TSS_C1_ELECTRODES_X) || defined(TSS_C1_ELECTRODES_Y)
        #define TSS_C1_OLD_EL_DEF_USED  1
        #ifdef TSS_C1_ELECTRODES
          #define TSS_C2_OFFSET           TSS_C1_OFFSET+TSS_C1_ELECTRODES
          #ifndef TSS_C1_INPUTS
            #define TSS_C1_INPUTS       {TSS_GET_EL_CONTROL_INPUTS(TSS_C1_ELECTRODES,TSS_C1_OFFSET)}
          #else
            #error TSS - TSS_C1_INPUTS is defined, but also old style definition by TSS_C1_ELECTRODES is used.
          #endif
        #endif
        #ifdef TSS_C1_ELECTRODES_X
          #ifndef TSS_C1_INPUTS_NUM_X
            #define TSS_C1_INPUTS_NUM_X     TSS_C1_ELECTRODES_X
          #else
            #error TSS - TSS_C1_INPUTS_NUM_X is defined, but also old style definition by TSS_C1_ELECTRODES_X is used.
          #endif
          #ifdef TSS_C1_ELECTRODES_Y
            #ifndef TSS_C1_ELECTRODES
              #define TSS_C1_ELECTRODES TSS_C1_ELECTRODES_X+TSS_C1_ELECTRODES_Y
            #endif
            #define TSS_C2_OFFSET           TSS_C1_OFFSET+TSS_C1_ELECTRODES
            #ifndef TSS_C1_INPUTS
              #define TSS_C1_INPUTS     {TSS_GET_EL_CONTROL_INPUTS(TSS_C1_ELECTRODES_X,TSS_C1_OFFSET),TSS_GET_EL_CONTROL_INPUTS(TSS_C1_ELECTRODES_Y,TSS_C1_OFFSET+TSS_C1_ELECTRODES_X)}
            #else
              #error TSS - TSS_C1_INPUTS is defined, but also old style definition by TSS_C1_ELECTRODES_X and TSS_C1_ELECTRODES_Y is used.
            #endif
          #endif
        #endif
      #endif
    #endif
    #if TSS_N_CONTROLS > 2
      #if defined(TSS_C2_ELECTRODES) || defined(TSS_C2_ELECTRODES_X) || defined(TSS_C2_ELECTRODES_Y)
        #define TSS_C2_OLD_EL_DEF_USED  1
        #ifdef TSS_C2_ELECTRODES
          #define TSS_C3_OFFSET           TSS_C2_OFFSET+TSS_C2_ELECTRODES
          #ifndef TSS_C2_INPUTS
            #define TSS_C2_INPUTS       {TSS_GET_EL_CONTROL_INPUTS(TSS_C2_ELECTRODES,TSS_C2_OFFSET)}
          #else
            #error TSS - TSS_C2_INPUTS is defined, but also old style definition by TSS_C2_ELECTRODES is used.
          #endif
        #endif
        #ifdef TSS_C2_ELECTRODES_X
          #ifndef TSS_C2_INPUTS_NUM_X
            #define TSS_C2_INPUTS_NUM_X     TSS_C2_ELECTRODES_X
          #else
            #error TSS - TSS_C2_INPUTS_NUM_X is defined, but also old style definition by TSS_C2_ELECTRODES_X is used.
          #endif
          #ifdef TSS_C2_ELECTRODES_Y
            #ifndef TSS_C2_ELECTRODES
              #define TSS_C2_ELECTRODES TSS_C2_ELECTRODES_X+TSS_C2_ELECTRODES_Y
            #endif
            #define TSS_C3_OFFSET           TSS_C2_OFFSET+TSS_C2_ELECTRODES
            #ifndef TSS_C2_INPUTS
              #define TSS_C2_INPUTS     {TSS_GET_EL_CONTROL_INPUTS(TSS_C2_ELECTRODES_X,TSS_C2_OFFSET),TSS_GET_EL_CONTROL_INPUTS(TSS_C2_ELECTRODES_Y,TSS_C2_OFFSET+TSS_C2_ELECTRODES_X)}
            #else
              #error TSS - TSS_C2_INPUTS is defined, but also old style definition by TSS_C2_ELECTRODES_X and TSS_C2_ELECTRODES_Y is used.
            #endif
          #endif
        #endif
      #endif
    #endif
    #if TSS_N_CONTROLS > 3
      #if defined(TSS_C3_ELECTRODES) || defined(TSS_C3_ELECTRODES_X) || defined(TSS_C3_ELECTRODES_Y)
        #define TSS_C3_OLD_EL_DEF_USED  1
        #ifdef TSS_C3_ELECTRODES
          #define TSS_C4_OFFSET           TSS_C3_OFFSET+TSS_C3_ELECTRODES
          #ifndef TSS_C3_INPUTS
            #define TSS_C3_INPUTS       {TSS_GET_EL_CONTROL_INPUTS(TSS_C3_ELECTRODES,TSS_C3_OFFSET)}
          #else
            #error TSS - TSS_C3_INPUTS is defined, but also old style definition by TSS_C3_ELECTRODES is used.
          #endif
        #endif
        #ifdef TSS_C3_ELECTRODES_X
          #ifndef TSS_C3_INPUTS_NUM_X
            #define TSS_C3_INPUTS_NUM_X     TSS_C3_ELECTRODES_X
          #else
            #error TSS - TSS_C3_INPUTS_NUM_X is defined, but also old style definition by TSS_C3_ELECTRODES_X is used.
          #endif
          #ifdef TSS_C3_ELECTRODES_Y
            #ifndef TSS_C3_ELECTRODES
              #define TSS_C3_ELECTRODES TSS_C3_ELECTRODES_X+TSS_C3_ELECTRODES_Y
            #endif
            #define TSS_C4_OFFSET           TSS_C3_OFFSET+TSS_C3_ELECTRODES
            #ifndef TSS_C3_INPUTS
              #define TSS_C3_INPUTS     {TSS_GET_EL_CONTROL_INPUTS(TSS_C3_ELECTRODES_X,TSS_C3_OFFSET),TSS_GET_EL_CONTROL_INPUTS(TSS_C3_ELECTRODES_Y,TSS_C3_OFFSET+TSS_C3_ELECTRODES_X)}
            #else
              #error TSS - TSS_C3_INPUTS is defined, but also old style definition by TSS_C3_ELECTRODES_X and TSS_C3_ELECTRODES_Y is used.
            #endif
          #endif
        #endif
      #endif
    #endif
    #if TSS_N_CONTROLS > 4
      #if defined(TSS_C4_ELECTRODES) || defined(TSS_C4_ELECTRODES_X) || defined(TSS_C4_ELECTRODES_Y)
        #define TSS_C4_OLD_EL_DEF_USED  1
        #ifdef TSS_C4_ELECTRODES
          #define TSS_C5_OFFSET           TSS_C4_OFFSET+TSS_C4_ELECTRODES
          #ifndef TSS_C4_INPUTS
            #define TSS_C4_INPUTS       {TSS_GET_EL_CONTROL_INPUTS(TSS_C4_ELECTRODES,TSS_C4_OFFSET)}
          #else
            #error TSS - TSS_C4_INPUTS is defined, but also old style definition by TSS_C4_ELECTRODES is used.
          #endif
        #endif
        #ifdef TSS_C4_ELECTRODES_X
          #ifndef TSS_C4_INPUTS_NUM_X
            #define TSS_C4_INPUTS_NUM_X     TSS_C4_ELECTRODES_X
          #else
            #error TSS - TSS_C4_INPUTS_NUM_X is defined, but also old style definition by TSS_C4_ELECTRODES_X is used.
          #endif
          #ifdef TSS_C4_ELECTRODES_Y
            #ifndef TSS_C4_ELECTRODES
              #define TSS_C4_ELECTRODES TSS_C4_ELECTRODES_X+TSS_C4_ELECTRODES_Y
            #endif
            #define TSS_C5_OFFSET           TSS_C4_OFFSET+TSS_C4_ELECTRODES
            #ifndef TSS_C4_INPUTS
              #define TSS_C4_INPUTS     {TSS_GET_EL_CONTROL_INPUTS(TSS_C4_ELECTRODES_X,TSS_C4_OFFSET),TSS_GET_EL_CONTROL_INPUTS(TSS_C4_ELECTRODES_Y,TSS_C4_OFFSET+TSS_C4_ELECTRODES_X)}
            #else
              #error TSS - TSS_C4_INPUTS is defined, but also old style definition by TSS_C4_ELECTRODES_X and TSS_C4_ELECTRODES_Y is used.
            #endif
          #endif
        #endif
      #endif
    #endif
    #if TSS_N_CONTROLS > 5
      #if defined(TSS_C5_ELECTRODES) || defined(TSS_C5_ELECTRODES_X) || defined(TSS_C5_ELECTRODES_Y)
        #define TSS_C5_OLD_EL_DEF_USED  1
        #ifdef TSS_C5_ELECTRODES
          #define TSS_C6_OFFSET           TSS_C5_OFFSET+TSS_C5_ELECTRODES
          #ifndef TSS_C5_INPUTS
            #define TSS_C5_INPUTS       {TSS_GET_EL_CONTROL_INPUTS(TSS_C5_ELECTRODES,TSS_C5_OFFSET)}
          #else
            #error TSS - TSS_C5_INPUTS is defined, but also old style definition by TSS_C5_ELECTRODES is used.
          #endif
        #endif
        #ifdef TSS_C5_ELECTRODES_X
          #ifndef TSS_C5_INPUTS_NUM_X
            #define TSS_C5_INPUTS_NUM_X     TSS_C5_ELECTRODES_X
          #else
            #error TSS - TSS_C5_INPUTS_NUM_X is defined, but also old style definition by TSS_C5_ELECTRODES_X is used.
          #endif
          #ifdef TSS_C5_ELECTRODES_Y
            #ifndef TSS_C5_ELECTRODES
              #define TSS_C5_ELECTRODES TSS_C5_ELECTRODES_X+TSS_C5_ELECTRODES_Y
            #endif
            #define TSS_C6_OFFSET           TSS_C5_OFFSET+TSS_C5_ELECTRODES
            #ifndef TSS_C5_INPUTS
              #define TSS_C5_INPUTS     {TSS_GET_EL_CONTROL_INPUTS(TSS_C5_ELECTRODES_X,TSS_C5_OFFSET),TSS_GET_EL_CONTROL_INPUTS(TSS_C5_ELECTRODES_Y,TSS_C5_OFFSET+TSS_C5_ELECTRODES_X)}
            #else
              #error TSS - TSS_C5_INPUTS is defined, but also old style definition by TSS_C5_ELECTRODES_X and TSS_C5_ELECTRODES_Y is used.
            #endif
          #endif
        #endif
      #endif
    #endif
    #if TSS_N_CONTROLS > 6
      #if defined(TSS_C6_ELECTRODES) || defined(TSS_C6_ELECTRODES_X) || defined(TSS_C6_ELECTRODES_Y)
        #define TSS_C6_OLD_EL_DEF_USED  1
        #ifdef TSS_C6_ELECTRODES
          #define TSS_C7_OFFSET           TSS_C6_OFFSET+TSS_C6_ELECTRODES
          #ifndef TSS_C6_INPUTS
            #define TSS_C6_INPUTS       {TSS_GET_EL_CONTROL_INPUTS(TSS_C6_ELECTRODES,TSS_C6_OFFSET)}
          #else
            #error TSS - TSS_C6_INPUTS is defined, but also old style definition by TSS_C6_ELECTRODES is used.
          #endif
        #endif
        #ifdef TSS_C6_ELECTRODES_X
          #ifndef TSS_C6_INPUTS_NUM_X
            #define TSS_C6_INPUTS_NUM_X     TSS_C6_ELECTRODES_X
          #else
            #error TSS - TSS_C6_INPUTS_NUM_X is defined, but also old style definition by TSS_C6_ELECTRODES_X is used.
          #endif
          #ifdef TSS_C6_ELECTRODES_Y
            #ifndef TSS_C6_ELECTRODES
              #define TSS_C6_ELECTRODES TSS_C6_ELECTRODES_X+TSS_C6_ELECTRODES_Y
            #endif
            #define TSS_C7_OFFSET           TSS_C6_OFFSET+TSS_C6_ELECTRODES
            #ifndef TSS_C6_INPUTS
              #define TSS_C6_INPUTS     {TSS_GET_EL_CONTROL_INPUTS(TSS_C6_ELECTRODES_X,TSS_C6_OFFSET),TSS_GET_EL_CONTROL_INPUTS(TSS_C6_ELECTRODES_Y,TSS_C6_OFFSET+TSS_C6_ELECTRODES_X)}
            #else
              #error TSS - TSS_C6_INPUTS is defined, but also old style definition by TSS_C6_ELECTRODES_X and TSS_C6_ELECTRODES_Y is used.
            #endif
          #endif
        #endif
      #endif
    #endif
    #if TSS_N_CONTROLS > 7
      #if defined(TSS_C7_ELECTRODES) || defined(TSS_C7_ELECTRODES_X) || defined(TSS_C7_ELECTRODES_Y)
        #define TSS_C7_OLD_EL_DEF_USED  1
        #ifdef TSS_C7_ELECTRODES
          #define TSS_C8_OFFSET           TSS_C7_OFFSET+TSS_C7_ELECTRODES
          #ifndef TSS_C7_INPUTS
            #define TSS_C7_INPUTS       {TSS_GET_EL_CONTROL_INPUTS(TSS_C7_ELECTRODES,TSS_C7_OFFSET)}
          #else
            #error TSS - TSS_C7_INPUTS is defined, but also old style definition by TSS_C7_ELECTRODES is used.
          #endif
        #endif
        #ifdef TSS_C7_ELECTRODES_X
          #ifndef TSS_C7_INPUTS_NUM_X
            #define TSS_C7_INPUTS_NUM_X     TSS_C7_ELECTRODES_X
          #else
            #error TSS - TSS_C7_INPUTS_NUM_X is defined, but also old style definition by TSS_C7_ELECTRODES_X is used.
          #endif
          #ifdef TSS_C7_ELECTRODES_Y
            #ifndef TSS_C7_ELECTRODES
              #define TSS_C7_ELECTRODES TSS_C7_ELECTRODES_X+TSS_C7_ELECTRODES_Y
            #endif
            #define TSS_C8_OFFSET           TSS_C7_OFFSET+TSS_C7_ELECTRODES
            #ifndef TSS_C7_INPUTS
              #define TSS_C7_INPUTS     {TSS_GET_EL_CONTROL_INPUTS(TSS_C7_ELECTRODES_X,TSS_C7_OFFSET),TSS_GET_EL_CONTROL_INPUTS(TSS_C7_ELECTRODES_Y,TSS_C7_OFFSET+TSS_C7_ELECTRODES_X)}
            #else
              #error TSS - TSS_C7_INPUTS is defined, but also old style definition by TSS_C7_ELECTRODES_X and TSS_C7_ELECTRODES_Y is used.
            #endif
          #endif
        #endif
      #endif
    #endif
    #if TSS_N_CONTROLS > 8
      #if defined(TSS_C8_ELECTRODES) || defined(TSS_C8_ELECTRODES_X) || defined(TSS_C8_ELECTRODES_Y)
        #define TSS_C8_OLD_EL_DEF_USED  1
        #ifdef TSS_C8_ELECTRODES
          #define TSS_C9_OFFSET           TSS_C8_OFFSET+TSS_C8_ELECTRODES
          #ifndef TSS_C8_INPUTS
            #define TSS_C8_INPUTS       {TSS_GET_EL_CONTROL_INPUTS(TSS_C8_ELECTRODES,TSS_C8_OFFSET)}
          #else
            #error TSS - TSS_C8_INPUTS is defined, but also old style definition by TSS_C8_ELECTRODES is used.
          #endif
        #endif
        #ifdef TSS_C8_ELECTRODES_X
          #ifndef TSS_C8_INPUTS_NUM_X
            #define TSS_C8_INPUTS_NUM_X     TSS_C8_ELECTRODES_X
          #else
            #error TSS - TSS_C8_INPUTS_NUM_X is defined, but also old style definition by TSS_C8_ELECTRODES_X is used.
          #endif
          #ifdef TSS_C8_ELECTRODES_Y
            #ifndef TSS_C8_ELECTRODES
              #define TSS_C8_ELECTRODES TSS_C8_ELECTRODES_X+TSS_C8_ELECTRODES_Y
            #endif
            #define TSS_C9_OFFSET           TSS_C8_OFFSET+TSS_C8_ELECTRODES
            #ifndef TSS_C8_INPUTS
              #define TSS_C8_INPUTS     {TSS_GET_EL_CONTROL_INPUTS(TSS_C8_ELECTRODES_X,TSS_C8_OFFSET),TSS_GET_EL_CONTROL_INPUTS(TSS_C8_ELECTRODES_Y,TSS_C8_OFFSET+TSS_C8_ELECTRODES_X)}
            #else
              #error TSS - TSS_C8_INPUTS is defined, but also old style definition by TSS_C8_ELECTRODES_X and TSS_C8_ELECTRODES_Y is used.
            #endif
          #endif
        #endif
      #endif
    #endif
    #if TSS_N_CONTROLS > 9
      #if defined(TSS_C9_ELECTRODES) || defined(TSS_C9_ELECTRODES_X) || defined(TSS_C9_ELECTRODES_Y)
        #define TSS_C9_OLD_EL_DEF_USED  1
        #ifdef TSS_C9_ELECTRODES
          #define TSS_C10_OFFSET           TSS_C9_OFFSET+TSS_C9_ELECTRODES
          #ifndef TSS_C9_INPUTS
            #define TSS_C9_INPUTS       {TSS_GET_EL_CONTROL_INPUTS(TSS_C9_ELECTRODES,TSS_C9_OFFSET)}
          #else
            #error TSS - TSS_C9_INPUTS is defined, but also old style definition by TSS_C9_ELECTRODES is used.
          #endif
        #endif
        #ifdef TSS_C9_ELECTRODES_X
          #ifndef TSS_C9_INPUTS_NUM_X
            #define TSS_C9_INPUTS_NUM_X     TSS_C9_ELECTRODES_X
          #else
            #error TSS - TSS_C9_INPUTS_NUM_X is defined, but also old style definition by TSS_C9_ELECTRODES_X is used.
          #endif
          #ifdef TSS_C9_ELECTRODES_Y
            #ifndef TSS_C9_ELECTRODES
              #define TSS_C9_ELECTRODES TSS_C9_ELECTRODES_X+TSS_C9_ELECTRODES_Y
            #endif
            #define TSS_C10_OFFSET           TSS_C9_OFFSET+TSS_C9_ELECTRODES
            #ifndef TSS_C9_INPUTS
              #define TSS_C9_INPUTS     {TSS_GET_EL_CONTROL_INPUTS(TSS_C9_ELECTRODES_X,TSS_C9_OFFSET),TSS_GET_EL_CONTROL_INPUTS(TSS_C9_ELECTRODES_Y,TSS_C9_OFFSET+TSS_C9_ELECTRODES_X)}
            #else
              #error TSS - TSS_C9_INPUTS is defined, but also old style definition by TSS_C9_ELECTRODES_X and TSS_C9_ELECTRODES_Y is used.
            #endif
          #endif
        #endif
      #endif
    #endif
    #if TSS_N_CONTROLS > 10
      #if defined(TSS_C10_ELECTRODES) || defined(TSS_C10_ELECTRODES_X) || defined(TSS_C10_ELECTRODES_Y)
        #define TSS_C10_OLD_EL_DEF_USED  1
        #ifdef TSS_C10_ELECTRODES
          #define TSS_C11_OFFSET           TSS_C10_OFFSET+TSS_C10_ELECTRODES
          #ifndef TSS_C10_INPUTS
            #define TSS_C10_INPUTS       {TSS_GET_EL_CONTROL_INPUTS(TSS_C10_ELECTRODES,TSS_C10_OFFSET)}
          #else
            #error TSS - TSS_C10_INPUTS is defined, but also old style definition by TSS_C10_ELECTRODES is used.
          #endif
        #endif
        #ifdef TSS_C10_ELECTRODES_X
          #ifndef TSS_C10_INPUTS_NUM_X
            #define TSS_C10_INPUTS_NUM_X     TSS_C10_ELECTRODES_X
          #else
            #error TSS - TSS_C10_INPUTS_NUM_X is defined, but also old style definition by TSS_C10_ELECTRODES_X is used.
          #endif
          #ifdef TSS_C10_ELECTRODES_Y
            #ifndef TSS_C10_ELECTRODES
              #define TSS_C10_ELECTRODES TSS_C10_ELECTRODES_X+TSS_C10_ELECTRODES_Y
            #endif
            #define TSS_C11_OFFSET           TSS_C10_OFFSET+TSS_C10_ELECTRODES
            #ifndef TSS_C10_INPUTS
              #define TSS_C10_INPUTS     {TSS_GET_EL_CONTROL_INPUTS(TSS_C10_ELECTRODES_X,TSS_C10_OFFSET),TSS_GET_EL_CONTROL_INPUTS(TSS_C10_ELECTRODES_Y,TSS_C10_OFFSET+TSS_C10_ELECTRODES_X)}
            #else
              #error TSS - TSS_C10_INPUTS is defined, but also old style definition by TSS_C10_ELECTRODES_X and TSS_C10_ELECTRODES_Y is used.
            #endif
          #endif
        #endif
      #endif
    #endif
    #if TSS_N_CONTROLS > 11
      #if defined(TSS_C11_ELECTRODES) || defined(TSS_C11_ELECTRODES_X) || defined(TSS_C11_ELECTRODES_Y)
        #define TSS_C11_OLD_EL_DEF_USED  1
        #ifdef TSS_C11_ELECTRODES
          #define TSS_C12_OFFSET           TSS_C11_OFFSET+TSS_C11_ELECTRODES
          #ifndef TSS_C11_INPUTS
            #define TSS_C11_INPUTS       {TSS_GET_EL_CONTROL_INPUTS(TSS_C11_ELECTRODES,TSS_C11_OFFSET)}
          #else
            #error TSS - TSS_C11_INPUTS is defined, but also old style definition by TSS_C11_ELECTRODES is used.
          #endif
        #endif
        #ifdef TSS_C11_ELECTRODES_X
          #ifndef TSS_C11_INPUTS_NUM_X
            #define TSS_C11_INPUTS_NUM_X     TSS_C11_ELECTRODES_X
          #else
            #error TSS - TSS_C11_INPUTS_NUM_X is defined, but also old style definition by TSS_C11_ELECTRODES_X is used.
          #endif
          #ifdef TSS_C11_ELECTRODES_Y
            #ifndef TSS_C11_ELECTRODES
              #define TSS_C11_ELECTRODES TSS_C11_ELECTRODES_X+TSS_C11_ELECTRODES_Y
            #endif
            #define TSS_C12_OFFSET           TSS_C11_OFFSET+TSS_C11_ELECTRODES
            #ifndef TSS_C11_INPUTS
              #define TSS_C11_INPUTS     {TSS_GET_EL_CONTROL_INPUTS(TSS_C11_ELECTRODES_X,TSS_C11_OFFSET),TSS_GET_EL_CONTROL_INPUTS(TSS_C11_ELECTRODES_Y,TSS_C11_OFFSET+TSS_C11_ELECTRODES_X)}
            #else
              #error TSS - TSS_C11_INPUTS is defined, but also old style definition by TSS_C11_ELECTRODES_X and TSS_C11_ELECTRODES_Y is used.
            #endif
          #endif
        #endif
      #endif
    #endif
    #if TSS_N_CONTROLS > 12
      #if defined(TSS_C12_ELECTRODES) || defined(TSS_C12_ELECTRODES_X) || defined(TSS_C12_ELECTRODES_Y)
        #define TSS_C12_OLD_EL_DEF_USED  1
        #ifdef TSS_C12_ELECTRODES
          #define TSS_C13_OFFSET           TSS_C12_OFFSET+TSS_C12_ELECTRODES
          #ifndef TSS_C12_INPUTS
            #define TSS_C12_INPUTS       {TSS_GET_EL_CONTROL_INPUTS(TSS_C12_ELECTRODES,TSS_C12_OFFSET)}
          #else
            #error TSS - TSS_C12_INPUTS is defined, but also old style definition by TSS_C12_ELECTRODES is used.
          #endif
        #endif
        #ifdef TSS_C12_ELECTRODES_X
          #ifndef TSS_C12_INPUTS_NUM_X
            #define TSS_C12_INPUTS_NUM_X     TSS_C12_ELECTRODES_X
          #else
            #error TSS - TSS_C12_INPUTS_NUM_X is defined, but also old style definition by TSS_C12_ELECTRODES_X is used.
          #endif
          #ifdef TSS_C12_ELECTRODES_Y
            #ifndef TSS_C12_ELECTRODES
              #define TSS_C12_ELECTRODES TSS_C12_ELECTRODES_X+TSS_C12_ELECTRODES_Y
            #endif
            #define TSS_C13_OFFSET           TSS_C12_OFFSET+TSS_C12_ELECTRODES
            #ifndef TSS_C12_INPUTS
              #define TSS_C12_INPUTS     {TSS_GET_EL_CONTROL_INPUTS(TSS_C12_ELECTRODES_X,TSS_C12_OFFSET),TSS_GET_EL_CONTROL_INPUTS(TSS_C12_ELECTRODES_Y,TSS_C12_OFFSET+TSS_C12_ELECTRODES_X)}
            #else
              #error TSS - TSS_C12_INPUTS is defined, but also old style definition by TSS_C12_ELECTRODES_X and TSS_C12_ELECTRODES_Y is used.
            #endif
          #endif
        #endif
      #endif
    #endif
    #if TSS_N_CONTROLS > 13
      #if defined(TSS_C13_ELECTRODES) || defined(TSS_C13_ELECTRODES_X) || defined(TSS_C13_ELECTRODES_Y)
        #define TSS_C13_OLD_EL_DEF_USED  1
        #ifdef TSS_C13_ELECTRODES
          #define TSS_C14_OFFSET           TSS_C13_OFFSET+TSS_C13_ELECTRODES
          #ifndef TSS_C13_INPUTS
            #define TSS_C13_INPUTS       {TSS_GET_EL_CONTROL_INPUTS(TSS_C13_ELECTRODES,TSS_C13_OFFSET)}
          #else
            #error TSS - TSS_C13_INPUTS is defined, but also old style definition by TSS_C13_ELECTRODES is used.
          #endif
        #endif
        #ifdef TSS_C13_ELECTRODES_X
          #ifndef TSS_C13_INPUTS_NUM_X
            #define TSS_C13_INPUTS_NUM_X     TSS_C13_ELECTRODES_X
          #else
            #error TSS - TSS_C13_INPUTS_NUM_X is defined, but also old style definition by TSS_C13_ELECTRODES_X is used.
          #endif
          #ifdef TSS_C13_ELECTRODES_Y
            #ifndef TSS_C13_ELECTRODES
              #define TSS_C13_ELECTRODES TSS_C13_ELECTRODES_X+TSS_C13_ELECTRODES_Y
            #endif
            #define TSS_C14_OFFSET           TSS_C13_OFFSET+TSS_C13_ELECTRODES
            #ifndef TSS_C13_INPUTS
              #define TSS_C13_INPUTS     {TSS_GET_EL_CONTROL_INPUTS(TSS_C13_ELECTRODES_X,TSS_C13_OFFSET),TSS_GET_EL_CONTROL_INPUTS(TSS_C13_ELECTRODES_Y,TSS_C13_OFFSET+TSS_C13_ELECTRODES_X)}
            #else
              #error TSS - TSS_C13_INPUTS is defined, but also old style definition by TSS_C13_ELECTRODES_X and TSS_C13_ELECTRODES_Y is used.
            #endif
          #endif
        #endif
      #endif
    #endif
    #if TSS_N_CONTROLS > 14
      #if defined(TSS_C14_ELECTRODES) || defined(TSS_C14_ELECTRODES_X) || defined(TSS_C14_ELECTRODES_Y)
        #define TSS_C14_OLD_EL_DEF_USED  1
        #ifdef TSS_C14_ELECTRODES
          #define TSS_C15_OFFSET           TSS_C14_OFFSET+TSS_C14_ELECTRODES
          #ifndef TSS_C14_INPUTS
            #define TSS_C14_INPUTS       {TSS_GET_EL_CONTROL_INPUTS(TSS_C14_ELECTRODES,TSS_C14_OFFSET)}
          #else
            #error TSS - TSS_C14_INPUTS is defined, but also old style definition by TSS_C14_ELECTRODES is used.
          #endif
        #endif
        #ifdef TSS_C14_ELECTRODES_X
          #ifndef TSS_C14_INPUTS_NUM_X
            #define TSS_C14_INPUTS_NUM_X     TSS_C14_ELECTRODES_X
          #else
            #error TSS - TSS_C14_INPUTS_NUM_X is defined, but also old style definition by TSS_C14_ELECTRODES_X is used.
          #endif
          #ifdef TSS_C14_ELECTRODES_Y
            #ifndef TSS_C14_ELECTRODES
              #define TSS_C14_ELECTRODES TSS_C14_ELECTRODES_X+TSS_C14_ELECTRODES_Y
            #endif
            #define TSS_C15_OFFSET           TSS_C14_OFFSET+TSS_C14_ELECTRODES
            #ifndef TSS_C14_INPUTS
              #define TSS_C14_INPUTS     {TSS_GET_EL_CONTROL_INPUTS(TSS_C14_ELECTRODES_X,TSS_C14_OFFSET),TSS_GET_EL_CONTROL_INPUTS(TSS_C14_ELECTRODES_Y,TSS_C14_OFFSET+TSS_C14_ELECTRODES_X)}
            #else
              #error TSS - TSS_C14_INPUTS is defined, but also old style definition by TSS_C14_ELECTRODES_X and TSS_C14_ELECTRODES_Y is used.
            #endif
          #endif
        #endif
      #endif
    #endif
    #if TSS_N_CONTROLS > 15
      #if defined(TSS_C15_ELECTRODES) || defined(TSS_C15_ELECTRODES_X) || defined(TSS_C15_ELECTRODES_Y)
        #define TSS_C15_OLD_EL_DEF_USED  1
        #ifdef TSS_C15_ELECTRODES
          #define TSS_C16_OFFSET           TSS_C15_OFFSET+TSS_C15_ELECTRODES
          #ifndef TSS_C15_INPUTS
            #define TSS_C15_INPUTS       {TSS_GET_EL_CONTROL_INPUTS(TSS_C15_ELECTRODES,TSS_C15_OFFSET)}
          #else
            #error TSS - TSS_C15_INPUTS is defined, but also old style definition by TSS_C15_ELECTRODES is used.
          #endif
        #endif
        #ifdef TSS_C15_ELECTRODES_X
          #ifndef TSS_C15_INPUTS_NUM_X
            #define TSS_C15_INPUTS_NUM_X     TSS_C15_ELECTRODES_X
          #else
            #error TSS - TSS_C15_INPUTS_NUM_X is defined, but also old style definition by TSS_C15_ELECTRODES_X is used.
          #endif
          #ifdef TSS_C15_ELECTRODES_Y
            #ifndef TSS_C15_ELECTRODES
              #define TSS_C15_ELECTRODES TSS_C15_ELECTRODES_X+TSS_C15_ELECTRODES_Y
            #endif
            #define TSS_C16_OFFSET           TSS_C15_OFFSET+TSS_C15_ELECTRODES
            #ifndef TSS_C15_INPUTS
              #define TSS_C15_INPUTS     {TSS_GET_EL_CONTROL_INPUTS(TSS_C15_ELECTRODES_X,TSS_C15_OFFSET),TSS_GET_EL_CONTROL_INPUTS(TSS_C15_ELECTRODES_Y,TSS_C15_OFFSET+TSS_C15_ELECTRODES_X)}
            #else
              #error TSS - TSS_C15_INPUTS is defined, but also old style definition by TSS_C15_ELECTRODES_X and TSS_C15_ELECTRODES_Y is used.
            #endif
          #endif
        #endif
      #endif
    #endif

    #if defined(TSS_C0_OLD_EL_DEF_USED) ||  defined(TSS_C1_OLD_EL_DEF_USED) ||  defined(TSS_C2_OLD_EL_DEF_USED) ||  defined(TSS_C3_OLD_EL_DEF_USED) ||  defined(TSS_C4_OLD_EL_DEF_USED) ||  defined(TSS_C5_OLD_EL_DEF_USED) ||  defined(TSS_C6_OLD_EL_DEF_USED) ||  defined(TSS_C7_OLD_EL_DEF_USED) ||  defined(TSS_C8_OLD_EL_DEF_USED) ||  defined(TSS_C9_OLD_EL_DEF_USED) ||  defined(TSS_C10_OLD_EL_DEF_USED) ||  defined(TSS_C11_OLD_EL_DEF_USED) ||  defined(TSS_C12_OLD_EL_DEF_USED) ||  defined(TSS_C13_OLD_EL_DEF_USED) ||  defined(TSS_C14_OLD_EL_DEF_USED) ||  defined(TSS_C15_OLD_EL_DEF_USED) ||  defined(TSS_C16_OLD_EL_DEF_USED)
      #define TSS_OLD_CONTROL_EL_DEF_USED   1
    #endif
  #endif

  /*******************************************************************************
   * IIR filter coeficients for fsample/fcut ratio
   *******************************************************************************/

  #define TSS_AFID_FILTER_RATIO_4_COEF   0.50000000000000
  #define TSS_AFID_FILTER_RATIO_5_COEF   0.42080777983773
  #define TSS_AFID_FILTER_RATIO_6_COEF   0.36602540378444
  #define TSS_AFID_FILTER_RATIO_7_COEF   0.32504243302651
  #define TSS_AFID_FILTER_RATIO_8_COEF   0.29289321881345
  #define TSS_AFID_FILTER_RATIO_9_COEF   0.26684617092250
  #define TSS_AFID_FILTER_RATIO_10_COEF   0.24523727525279
  #define TSS_AFID_FILTER_RATIO_15_COEF   0.17529620340124
  #define TSS_AFID_FILTER_RATIO_20_COEF   0.13672873599732
  #define TSS_AFID_FILTER_RATIO_25_COEF   0.11216024447519
  #define TSS_AFID_FILTER_RATIO_30_COEF   0.09510798340250
  #define TSS_AFID_FILTER_RATIO_35_COEF   0.08257017843558
  #define TSS_AFID_FILTER_RATIO_40_COEF   0.07295965726827
  #define TSS_AFID_FILTER_RATIO_45_COEF   0.06535663109189
  #define TSS_AFID_FILTER_RATIO_50_COEF   0.05919070381841
  #define TSS_AFID_FILTER_RATIO_55_COEF   0.05408913994850
  #define TSS_AFID_FILTER_RATIO_60_COEF   0.04979797785108
  #define TSS_AFID_FILTER_RATIO_65_COEF   0.04613816763607
  #define TSS_AFID_FILTER_RATIO_70_COEF   0.04297982559130
  #define TSS_AFID_FILTER_RATIO_75_COEF   0.04022643104648
  #define TSS_AFID_FILTER_RATIO_80_COEF   0.03780475417090
  #define TSS_AFID_FILTER_RATIO_85_COEF   0.03565822686527
  #define TSS_AFID_FILTER_RATIO_90_COEF   0.03374245693117
  #define TSS_AFID_FILTER_RATIO_95_COEF   0.03202211805956
  #define TSS_AFID_FILTER_RATIO_100_COEF   0.03046874709125
  #define TSS_AFID_FILTER_RATIO_110_COEF   0.02777425431677
  #define TSS_AFID_FILTER_RATIO_120_COEF   0.02551771664256
  #define TSS_AFID_FILTER_RATIO_130_COEF   0.02360036304963
  #define TSS_AFID_FILTER_RATIO_140_COEF   0.02195105169432
  #define TSS_AFID_FILTER_RATIO_150_COEF   0.02051723901855
  #define TSS_AFID_FILTER_RATIO_160_COEF   0.01925927420234
  #define TSS_AFID_FILTER_RATIO_170_COEF   0.01814667285225
  #define TSS_AFID_FILTER_RATIO_180_COEF   0.01715561259646
  #define TSS_AFID_FILTER_RATIO_190_COEF   0.01626720730529
  #define TSS_AFID_FILTER_RATIO_200_COEF   0.01546629140310
  #define TSS_AFID_FILTER_RATIO_220_COEF   0.01407986368033
  #define TSS_AFID_FILTER_RATIO_240_COEF   0.01292156453916
  #define TSS_AFID_FILTER_RATIO_260_COEF   0.01193936578486
  #define TSS_AFID_FILTER_RATIO_280_COEF   0.01109594318993
  #define TSS_AFID_FILTER_RATIO_300_COEF   0.01036382463711
  #define TSS_AFID_FILTER_RATIO_350_COEF   0.00889636435319
  #define TSS_AFID_FILTER_RATIO_400_COEF   0.00779293629195
  #define TSS_AFID_FILTER_RATIO_500_COEF   0.00624403504634
  #define TSS_AFID_FILTER_RATIO_600_COEF   0.00520876234062
  #define TSS_AFID_FILTER_RATIO_700_COEF   0.00446796731277
  #define TSS_AFID_FILTER_RATIO_800_COEF   0.00391164991125
  #define TSS_AFID_FILTER_RATIO_900_COEF   0.00347853027101
  #define TSS_AFID_FILTER_RATIO_1000_COEF  0.00313176422919

  #define TSS_AFID_GET_FILTER_RATIO_CONV(conv)  TSS_AFID_FILTER_RATIO_##conv##_COEF

  #define TSS_AFID_GET_FILTER_RATIO_NAME(name)  TSS_AFID_GET_FILTER_RATIO_CONV(name)

  #define TSS_AFID_GET_FILTER_RATIO_COEF(ratio) FRAC32(TSS_AFID_GET_FILTER_RATIO_NAME(ratio))

  /***********************************************************
   **************** Default Values definition ****************
   ***********************************************************/

  #ifndef TSS_USE_PE_COMPONENT
    #define TSS_USE_PE_COMPONENT                           0
  #endif

  #ifndef TSS_USE_MQX
    #define TSS_USE_MQX                                    0
  #endif

  #ifndef TSS_USE_KEYDETECTOR_VERSION
    #define TSS_USE_KEYDETECTOR_VERSION                    1
  #endif

  #ifndef TSS_ENABLE_DIAGNOSTIC_MESSAGES
    #define TSS_ENABLE_DIAGNOSTIC_MESSAGES                0
  #endif

  #ifndef TSS_USE_IIR_FILTER
    #define TSS_USE_IIR_FILTER                             0
  #endif

  #ifndef TSS_USE_DELTA_LOG
    #define TSS_USE_DELTA_LOG                              0
  #endif

  #ifndef TSS_USE_INTEGRATION_DELTA_LOG
    #define TSS_USE_INTEGRATION_DELTA_LOG                  0
  #else
    #if (TSS_USE_KEYDETECTOR_VERSION != 2)
      #undef TSS_USE_INTEGRATION_DELTA_LOG
      #define TSS_USE_INTEGRATION_DELTA_LOG                0
      #if TSS_ENABLE_DIAGNOSTIC_MESSAGES
        #warning TSS - TSS_USE_INTEGRATION_DELTA_LOG was disabled because it is relevant only for Keydetector 2.
      #endif
    #endif
  #endif

  #ifndef TSS_USE_SIGNAL_LOG
    #define TSS_USE_SIGNAL_LOG                             0
  #endif

  #ifndef TSS_USE_DATA_CORRUPTION_CHECK
    #define TSS_USE_DATA_CORRUPTION_CHECK                  1
  #endif

  #ifndef TSS_USE_FREEMASTER_GUI
    #define TSS_USE_FREEMASTER_GUI                        0
  #endif

  #ifndef TSS_USE_CONTROL_PRIVATE_DATA
    #define TSS_USE_CONTROL_PRIVATE_DATA                  0
  #endif

  #ifndef TSS_USE_SIGNAL_SHIELDING
    #define TSS_USE_SIGNAL_SHIELDING                      0
  #endif

  #ifndef TSS_USE_SIGNAL_DIVIDER
    #define TSS_USE_SIGNAL_DIVIDER                        0
  #endif

  #ifndef TSS_USE_SIGNAL_MULTIPLIER
    #define TSS_USE_SIGNAL_MULTIPLIER                     0
  #endif

  #ifndef TSS_USE_STUCK_KEY
    #define TSS_USE_STUCK_KEY                             1
  #endif

  #ifndef TSS_USE_NEGATIVE_BASELINE_DROP
    #if (TSS_USE_KEYDETECTOR_VERSION == 2)
      #define TSS_USE_NEGATIVE_BASELINE_DROP              0
    #else
      #define TSS_USE_NEGATIVE_BASELINE_DROP              1
    #endif
  #else
    #if (TSS_USE_KEYDETECTOR_VERSION == 2)
      #undef TSS_USE_NEGATIVE_BASELINE_DROP
      #define TSS_USE_NEGATIVE_BASELINE_DROP              0
      #if TSS_ENABLE_DIAGNOSTIC_MESSAGES
        #warning TSS - TSS_USE_NEGATIVE_BASELINE_DROP was disabled because Keydetector 2 does not support this feature.
      #endif
    #endif
  #endif

  #ifndef TSS_USE_AFID_FAST_FILTER_RATIO
    #define TSS_USE_AFID_FAST_FILTER_RATIO                    50
  #endif

  #ifndef TSS_USE_AFID_SLOW_FILTER_RATIO
    #define TSS_USE_AFID_SLOW_FILTER_RATIO                    200
  #endif

  #ifndef TSS_USE_AUTO_HW_RECALIBRATION
    #define TSS_USE_AUTO_HW_RECALIBRATION                 1
  #endif

  #ifndef TSS_USE_BASELINE_INIT_DURATION
    #define TSS_USE_BASELINE_INIT_DURATION                0
  #endif

  #ifndef TSS_USE_AUTO_SENS_CALIBRATION
    #define TSS_USE_AUTO_SENS_CALIBRATION                 0
  #endif

  #ifndef TSS_USE_AUTO_SENS_CALIB_INIT_DURATION
    #if TSS_USE_AUTO_SENS_CALIBRATION
      #define TSS_USE_AUTO_SENS_CALIB_INIT_DURATION        100
    #else
      #define TSS_USE_AUTO_SENS_CALIB_INIT_DURATION        0
    #endif
  #endif

  #ifndef TSS_USE_AUTO_SENS_CALIB_LOW_LIMIT
    #define TSS_USE_AUTO_SENS_CALIB_LOW_LIMIT              0
  #endif

  /* Default settings of TSS internal functions (Not mentioned in Reference Manual) */

  #ifndef TSS_USE_DELTA_NOISE_ANALYSIS_IIR_WEIGHT1
    #if TSS_USE_AUTO_SENS_CALIBRATION
      #define TSS_USE_DELTA_NOISE_ANALYSIS_IIR_WEIGHT1     3
      #ifndef TSS_USE_DELTA_NOISE_ANALYSIS_IIR
        #define TSS_USE_DELTA_NOISE_ANALYSIS_IIR           1
      #endif
    #else
      #define TSS_USE_DELTA_NOISE_ANALYSIS_IIR_WEIGHT1     0
      #ifndef TSS_USE_DELTA_NOISE_ANALYSIS_IIR
        #define TSS_USE_DELTA_NOISE_ANALYSIS_IIR           0
      #endif
    #endif
  #endif

  #ifndef TSS_USE_DELTA_NOISE_ANALYSIS_IIR_WEIGHT2
    #if TSS_USE_AUTO_SENS_CALIBRATION
      #define TSS_USE_DELTA_NOISE_ANALYSIS_IIR_WEIGHT2    -1
      #ifndef TSS_USE_DELTA_NOISE_ANALYSIS_IIR
        #define TSS_USE_DELTA_NOISE_ANALYSIS_IIR           1
      #endif
    #else
      #define TSS_USE_DELTA_NOISE_ANALYSIS_IIR_WEIGHT2     0
      #ifndef TSS_USE_DELTA_NOISE_ANALYSIS_IIR
        #define TSS_USE_DELTA_NOISE_ANALYSIS_IIR           0
      #endif
    #endif
  #endif

  #ifndef TSS_ASLIDER_POSITION_IIR_WEIGHT
    #define TSS_ASLIDER_POSITION_IIR_WEIGHT      3u
  #endif
  #if ((TSS_ASLIDER_POSITION_IIR_WEIGHT > 0) && (TSS_ASLIDER_USED || TSS_AROTARY_USED))
    #define TSS_ASLIDER_POSITION_IIR_USED        1
  #else
    #define TSS_ASLIDER_POSITION_IIR_USED        0
  #endif

  #ifndef TSS_ASLIDER_DELAY_BUFFER_LENGTH
    #define TSS_ASLIDER_DELAY_BUFFER_LENGTH      0     /* Disabled by default */
  #endif
  #if ((TSS_ASLIDER_DELAY_BUFFER_LENGTH > 0) && (TSS_ASLIDER_USED || TSS_AROTARY_USED))
    #define TSS_ASLIDER_DELAY_USED               1
  #else
    #define TSS_ASLIDER_DELAY_USED               0
    #undef TSS_ASLIDER_DELAY_BUFFER_LENGTH
    #define TSS_ASLIDER_DELAY_BUFFER_LENGTH      1     /* The size of the buffer can not be 0 */
  #endif

  #ifndef TSS_ASLIDER_TREND_BUFFER_LENGTH
    #define TSS_ASLIDER_TREND_BUFFER_LENGTH      32u
  #endif
  #ifndef TSS_ASLIDER_TREND_THRESHOLD
    #define TSS_ASLIDER_TREND_THRESHOLD          0     /* Use Automatic Trend Treshold calculation */
  #endif
  #if ((TSS_ASLIDER_TREND_BUFFER_LENGTH > 0) && (TSS_ASLIDER_USED || TSS_AROTARY_USED))
    #define TSS_ASLIDER_TREND_USED               1
  #else
    #define TSS_ASLIDER_TREND_USED               0
    #undef TSS_ASLIDER_TREND_BUFFER_LENGTH
    #define TSS_ASLIDER_TREND_BUFFER_LENGTH      1     /* The size of the buffer can not be 0 */
  #endif

  #ifndef TSS_MATRIX_POSITION_IIR_WEIGHT
    #define TSS_MATRIX_POSITION_IIR_WEIGHT       3u
  #endif
  #if ((TSS_MATRIX_POSITION_IIR_WEIGHT > 0) && (TSS_MATRIX_USED))
    #define TSS_MATRIX_POSITION_IIR_USED         1
  #else
    #define TSS_MATRIX_POSITION_IIR_USED         0
  #endif

  #ifndef TSS_USE_LOWPOWER_CALIBRATION
    #define TSS_USE_LOWPOWER_CALIBRATION         0
  #endif

  #ifndef TSS_USE_NOISE_MODE
    #define TSS_USE_NOISE_MODE                   0
  #else
    #if TSS_USE_NOISE_MODE
      #undef TSS_USE_NEGATIVE_BASELINE_DROP
      #define TSS_USE_NEGATIVE_BASELINE_DROP     0
      #if TSS_ENABLE_DIAGNOSTIC_MESSAGES
        #warning TSS - TSS_USE_NEGATIVE_BASELINE_DROP was disabled because noise mode is enabled.
      #endif
    #endif
  #endif

  #ifndef TSS_USE_NOISE_SIGNAL_LOG
    #define TSS_USE_NOISE_SIGNAL_LOG             0
  #elif (TSS_USE_NOISE_MODE == 0)
    #undef TSS_USE_NOISE_SIGNAL_LOG
    #define TSS_USE_NOISE_SIGNAL_LOG             0
  #endif

  #ifndef TSS_USE_LOWPOWER_THRESHOLD_BASELINE
    #define TSS_USE_LOWPOWER_THRESHOLD_BASELINE  0
  #endif

  /***********************************************************
   ******* Extern Contants and Variables Declaration *********
   ***********************************************************/

  extern volatile uint8_t tss_u8SampleIntFlag;            /* Extern variable for Sample Interrupted Flag */
  extern const uint8_t * const tss_acpsCSStructs[];       /* Extern Constant Array to store the structures addresses */

  extern TSS_CSSystem tss_CSSys;                        /* System Control Extern Struct */
  extern uint8_t tss_au8Sensitivity[TSS_N_ELECTRODES];
  extern uint8_t tss_au8ElectrodeEnablers[((TSS_N_ELECTRODES - 1)/ 8) + 1];
  extern uint8_t tss_au8ElectrodeStatus[((TSS_N_ELECTRODES - 1)/ 8) + 1];
  extern uint8_t tss_au8ElectrodeDCTrackerEnablers[((TSS_N_ELECTRODES - 1)/ 8) + 1];

  #if TSS_USE_DELTA_LOG
    extern int8_t tss_ai8InstantDelta[TSS_N_ELECTRODES];
  #endif
  #if TSS_USE_SIGNAL_LOG
    extern uint16_t tss_au16InstantSignal[TSS_N_ELECTRODES];
  #endif
  #if TSS_USE_INTEGRATION_DELTA_LOG
    extern int8_t tss_ai8IntegrationDelta[TSS_N_ELECTRODES];
  #endif
  #if ((TSS_USE_NOISE_MODE == 1) && (TSS_USE_NOISE_SIGNAL_LOG == 1))
    extern uint8_t tss_au8InstantNoise[TSS_N_ELECTRODES];
  #endif

  #define TSS_SET_SAMPLE_INTERRUPTED()       tss_u8SampleIntFlag = 1u;

#endif /* TSS_API_H */
