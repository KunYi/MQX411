/**********************************************************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2006-2009 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
********************************************************************************************************************//*!
*
* @file   TSS_StatusCodes.h
*
* @brief  Header file that defines the Return Status Codes used by the TSS Library
*
* @version 1.0.35.0
*
* @date Jan-15-2013
*
* These Return Status Codes are used by the User Application Level and also internally in the Library
*
***********************************************************************************************************************/


#ifndef TSS_STATUSCODES_H
  #define TSS_STATUSCODES_H

  /* TSS Task & TSS Init Status Codes */
  #define TSS_STATUS_OK                           0u
  #define TSS_STATUS_PROCESSING                   1u
  #define TSS_STATUS_RECALIBRATION_FAULT          2u
  #define TSS_STATUS_BUSY                         3u

  /* TSS measurement modes */
  #define TSS_MODE_GPIO                           0u
  #define TSS_MODE_CAP                            1u
  #define TSS_MODE_NOISE                          2u
  
  /* Sample Electrode Commands */
  #define TSS_SAMPLE_COMMAND_DUMMY                0x00u   /* Command for SampleElectrode function */
  #define TSS_SAMPLE_COMMAND_RESTART              0x01u   /* Command for SampleElectrode function */
  #define TSS_SAMPLE_COMMAND_PROCESS              0x02u   /* Command for SampleElectrode function */
  #define TSS_SAMPLE_COMMAND_RECALIB              0x03u   /* Command for SampleElectrode function */
  #define TSS_SAMPLE_COMMAND_GET_NEXT_ELECTRODE   0x04u   /* Command for SampleElectrode function */
  #define TSS_SAMPLE_COMMAND_ENABLE_ELECTRODE     0x05u   /* Command for SampleElectrode function */
  #define TSS_SAMPLE_COMMAND_RESTART_NOISE        0x06u   /* Command for SampleElectrode function */
  #define TSS_SAMPLE_COMMAND_GET_NOISE_VALUE      0x07u   /* Command for SampleElectrode function */
  #define TSS_SAMPLE_COMMAND_SET_LOWLEVEL_CONFIG  0x08u   /* Command for SampleElectrode function */
  #define TSS_SAMPLE_COMMAND_GET_LOWLEVEL_CONFIG  0x09u   /* Command for SampleElectrode function */

  /* Sample Electrode Status Codes */
  #define TSS_SAMPLE_STATUS_OK                    0x00u   /* Return value from the SampleElectrode function */
  #define TSS_SAMPLE_STATUS_PROCESSING            0x01u   /* Return value from the SampleElectrode function */
  #define TSS_SAMPLE_STATUS_CALIBRATION_CHANGED   0x02u   /* Return value from the SampleElectrode function */
  #define TSS_SAMPLE_RECALIB_REQUEST_LOCAP        0x03u   /* Return value from the SampleElectrode function */
  #define TSS_SAMPLE_RECALIB_REQUEST_HICAP        0x04u   /* Return value from the SampleElectrode function */
  #define TSS_SAMPLE_ERROR_SMALL_TRIGGER_PERIOD   0x05u   /* Return value from the SampleElectrode function */
  #define TSS_SAMPLE_ERROR_CHARGE_TIMEOUT         0x06u   /* Return value from the SampleElectrode function */
  #define TSS_SAMPLE_ERROR_SMALL_CAP              0x07u   /* Return value from the SampleElectrode function */
  #define TSS_SAMPLE_ERROR_RESULT_NA              0x08u   /* Return value from the SampleElectrode function */
  #define TSS_SAMPLE_ERROR_SMALL_AUTOTRG_PERIOD   TSS_SAMPLE_ERROR_SMALL_TRIGGER_PERIOD

  /* Sensor Init Commands Commands */
  #define TSS_INIT_COMMAND_DUMMY                  0x00u   /* Command for SensorInit function */
  #define TSS_INIT_COMMAND_INIT_MODULES           0x01u   /* Command for SensorInit function */
  #define TSS_INIT_COMMAND_ENABLE_ELECTRODES      0x02u   /* Command for SensorInit function */
  #define TSS_INIT_COMMAND_SET_NSAMPLES           0x03u   /* Command for SensorInit function */
  #define TSS_INIT_COMMAND_INIT_TRIGGER           0x04u   /* Command for SensorInit function */
  #define TSS_INIT_COMMAND_SW_TRIGGER             0x05u   /* Command for SensorInit function */
  #define TSS_INIT_COMMAND_INIT_LOWPOWER          0x06u   /* Command for SensorInit function */
  #define TSS_INIT_COMMAND_GOTO_LOWPOWER          0x07u   /* Command for SensorInit function */
  #define TSS_INIT_COMMAND_RECALIBRATE            0x08u   /* Command for SensorInit function */
  #define TSS_INIT_COMMAND_ENABLE_LOWPOWER_CALIB  0x09u   /* Command for SensorInit function */
  /* Sensor Init Mode Commands */
  #define TSS_INIT_COMMAND_MODE_CAP               0x10u   /* Command for SensorInit function */
  #define TSS_INIT_COMMAND_MODE_NOISE             0x11u   /* Command for SensorInit function */
  #define TSS_INIT_COMMAND_MODE_GET               0x12u   /* Command for SensorInit function */

  /* Sensor Init Return Codes */
  #define TSS_INIT_STATUS_OK                      0x00u   /* Return values from SensorInit function */
  #define TSS_INIT_STATUS_LOWPOWER_SET            0x01u   /* Return values from SensorInit function */
  #define TSS_INIT_STATUS_LOWPOWER_ELEC_SET       0x02u   /* Return values from SensorInit function */
  #define TSS_INIT_STATUS_TRIGGER_SET             0x04u   /* Return values from SensorInit function */
  #define TSS_INIT_STATUS_AUTOTRIGGER_SET         0x08u   /* Return values from SensorInit function */
  #define TSS_INIT_STATUS_CALIBRATION_CHANGED     0x10u   /* Return values from SensorInit function */
  #define TSS_INIT_ERROR_RECALIB_FAULT            0x20u   /* Return values from SensorInit function */

  /* Decoder Commands */
  #define TSS_DECODER_COMMAND_DUMMY               0x00u   /* Command for Decoder function */
  #define TSS_DECODER_COMMAND_PROCESS             0x01u   /* Command for Decoder function */
  #define TSS_DECODER_COMMAND_INIT                0x02u   /* Command for Decoder function */

  /* Decoder Return Codes */
  #define TSS_DECODER_STATUS_OK                   0x00u   /* Return values from Decoder function */
  #define TSS_DECODER_ERROR_ILLEGAL_CONTROL_TYPE  0x01u   /* Return values from Decoder function */
  #define TSS_DECODER_STATUS_BUSY                 0x02u   /* Return values from Decoder function */
  #define TSS_DECODER_ERROR_OUT_OF_RANGE          0xFFu   /* Return values from Decoder function */

  /* Key Detector Status Codes */
  #define TSS_KEYDETECT_STATUS_OK                 0x00u   /* Return value from the KeyDetector function */
  #define TSS_KEYDETECT_STATUS_PROCESSING         0x01u   /* Return value from the KeyDetector function */
  #define TSS_KEYDETECT_ERROR_LARGE_CAP           0x02u   /* Return value from the KeyDetector function */
  #define TSS_KEYDETECT_ERROR_SMALL_CAP           0x04u   /* Return value from the KeyDetector function */
  #define TSS_KEYDETECT_ERROR_SMALL_TRIGGER_VAL   0x08u   /* Return value from the KeyDetector function */
  #define TSS_KEYDETECT_STATUS_DECODER_BUSY       0x10u   /* Return value from the KeyDetector function */
  #define TSS_KEYDETECT_STATUS_TRIGGER_WAIT       0x20u   /* Return value from the KeyDetector function */
  #define TSS_KEYDETECT_STATUS_PROX_WAIT          0x40u   /* Return value from the KeyDetector function */
  #define TSS_KEYDETECT_STATUS_PROX_DONE          0x80u   /* Return value from the KeyDetector function */

  /* Keypad Decoder Config Status Codes */
  #define TSS_ERROR_KEYPAD_ILLEGAL_PARAMETER      0x11u   /* Return values from the KeyPadConfig function */
  #define TSS_ERROR_KEYPAD_READ_ONLY_PARAMETER    0x12u   /* Return values from the KeyPadConfig function */
  #define TSS_ERROR_KEYPAD_ILLEGAL_VALUE          0x13u   /* Return values from the KeyPadConfig function */
  #define TSS_ERROR_KEYPAD_OUT_OF_RANGE           0x14u   /* Return values from the KeyPadConfig function */
  #define TSS_ERROR_KEYPAD_ILLEGAL_CONTROL_TYPE   0x15u   /* Return values from the KeyPadConfig function */

  /* Slider Decoder Config Status Codes */
  #define TSS_ERROR_SLIDER_ILLEGAL_PARAMETER      0x21u   /* Return values from the TSS_SliderConfig function */
  #define TSS_ERROR_SLIDER_READ_ONLY_PARAMETER    0x22u   /* Return values from the TSS_SliderConfig function */
  #define TSS_ERROR_SLIDER_ILLEGAL_VALUE          0x23u   /* Return values from the TSS_SliderConfig function */
  #define TSS_ERROR_SLIDER_OUT_OF_RANGE           0x24u   /* Return values from the TSS_SliderConfig function */
  #define TSS_ERROR_SLIDER_ILLEGAL_CONTROL_TYPE   0x25u   /* Return values from the TSS_SliderConfig function */

  /* Rotary Decoder Config Status Codes */
  #define TSS_ERROR_ROTARY_ILLEGAL_PARAMETER      TSS_ERROR_SLIDER_ILLEGAL_PARAMETER
  #define TSS_ERROR_ROTARY_READ_ONLY_PARAMETER    TSS_ERROR_SLIDER_READ_ONLY_PARAMETER
  #define TSS_ERROR_ROTARY_ILLEGAL_VALUE          TSS_ERROR_SLIDER_ILLEGAL_VALUE
  #define TSS_ERROR_ROTARY_OUT_OF_RANGE           TSS_ERROR_SLIDER_OUT_OF_RANGE
  #define TSS_ERROR_ROTARY_ILLEGAL_CONTROL_TYPE   TSS_ERROR_SLIDER_ILLEGAL_CONTROL_TYPE

  /* Analog Slider Decoder Config Status Codes */
  #define TSS_ERROR_ASLIDER_ILLEGAL_PARAMETER     0x31u   /* Return values from the TSS_ASliderConfig function */
  #define TSS_ERROR_ASLIDER_READ_ONLY_PARAMETER   0x32u   /* Return values from the TSS_ASliderConfig function */
  #define TSS_ERROR_ASLIDER_ILLEGAL_VALUE         0x33u   /* Return values from the TSS_ASliderConfig function */
  #define TSS_ERROR_ASLIDER_OUT_OF_RANGE          0x34u   /* Return values from the TSS_ASliderConfig function */
  #define TSS_ERROR_ASLIDER_ILLEGAL_CONTROL_TYPE  0x35u   /* Return values from the TSS_ASliderConfig function */

  /* Analog Rotary Decoder Config Status Codes */
  #define TSS_ERROR_AROTARY_ILLEGAL_PARAMETER     TSS_ERROR_ASLIDER_ILLEGAL_PARAMETER
  #define TSS_ERROR_AROTARY_READ_ONLY_PARAMETER   TSS_ERROR_ASLIDER_READ_ONLY_PARAMETER
  #define TSS_ERROR_AROTARY_ILLEGAL_VALUE         TSS_ERROR_ASLIDER_ILLEGAL_VALUE
  #define TSS_ERROR_AROTARY_OUT_OF_RANGE          TSS_ERROR_ASLIDER_OUT_OF_RANGE
  #define TSS_ERROR_AROTARY_ILLEGAL_CONTROL_TYPE  TSS_ERROR_ASLIDER_ILLEGAL_CONTROL_TYPE

  /* Matrix Decoder Config Status Codes */
  #define TSS_ERROR_MATRIX_ILLEGAL_PARAMETER      0x41u   /* Return values from the TSS_MatrixConfig function */
  #define TSS_ERROR_MATRIX_READ_ONLY_PARAMETER    0x42u   /* Return values from the TSS_MatrixConfig function */
  #define TSS_ERROR_MATRIX_ILLEGAL_VALUE          0x43u   /* Return values from the TSS_MatrixConfig function */
  #define TSS_ERROR_MATRIX_OUT_OF_RANGE           0x44u   /* Return values from the TSS_MatrixConfig function */
  #define TSS_ERROR_MATRIX_ILLEGAL_CONTROL_TYPE   0x45u   /* Return values from the TSS_MatrixConfig function */

  /* System Config & Mgnt Status Codes */
  #define TSS_ERROR_CONFSYS_BUSY                  0x01u   /* Return values from the TSS_SetSystemConfig function */
  #define TSS_ERROR_CONFSYS_ILLEGAL_PARAMETER     0x02u   /* Return values from the TSS_SetSystemConfig function */
  #define TSS_ERROR_CONFSYS_READ_ONLY_PARAMETER   0x03u   /* Return values from the TSS_SetSystemConfig function */
  #define TSS_ERROR_CONFSYS_OUT_OF_RANGE          0x04u   /* Return values from the TSS_SetSystemConfig function */
  #define TSS_ERROR_CONFSYS_LOWPOWER_SOURCE_NA    0x05u   /* Return values from the TSS_SetSystemConfig function */
  #define TSS_ERROR_CONFSYS_INCORRECT_LOWPOWER_EL 0x06u   /* Return values from the TSS_SetSystemConfig function */
  #define TSS_ERROR_CONFSYS_TRIGGER_SOURCE_NA     0x07u   /* Return values from the TSS_SetSystemConfig function */
  #define TSS_ERROR_CONFSYS_PROXIMITY_CALLBACK_NA 0x08u   /* Return values from the TSS_SetSystemConfig function */
  #define TSS_ERROR_CONFSYS_RECALIBRATION_FAULT   0x09u   /* Return values from the TSS_SetSystemConfig function */
  #define TSS_ERROR_CONFSYS_SHIELD_NA             0x0Au   /* Return values from the TSS_SetSystemConfig function */
  #define TSS_ERROR_CONFSYS_HWRECALIB_PENDING     0x0Bu   /* Return values from the TSS_SetSystemConfig function */
  #define TSS_ERROR_CONFSYS_MANRECALIB_PENDING    0x0Cu   /* Return values from the TSS_SetSystemConfig function */
  #define TSS_ERROR_CONFSYS_DCTRACKER_RATE_ZERO   0x0Du   /* Return values from the TSS_SetSystemConfig function */
  #define TSS_ERROR_CONFSYS_STUCKKEY_TIMEOUT_ZERO 0x0Eu   /* Return values from the TSS_SetSystemConfig function */

  /* Get Config Status Codes */
  #define TSS_ERROR_GETCONF_ILLEGAL_PARAMETER     0xFFFFu  /* Return values from the GetConfig functions */
  #define TSS_ERROR_GETCONF_ILLEGAL_CONTROL_TYPE  0xFFFEu  /* Return values from the GetConfig functions */

  /* System Data Status Codes */
  #define TSS_ERROR_DATASYS_OUT_OF_RANGE          (-1)    /* Return values from the TSS_Store/LoadSystemData function */
  #define TSS_ERROR_DATASYS_CHECKSUM              (-2)    /* Return values from the TSS_Store/LoadSystemData function */
  #define TSS_ERROR_DATASYS_MEM_NULL              (-3)    /* Return values from the TSS_Store/LoadSystemData function */
  #define TSS_ERROR_DATASYS_NOT_ENOUGH_DATA       (-4)    /* Return values from the TSS_Store/LoadSystemData function */
  #define TSS_ERROR_DATASYS_READ_ONLY             (-5)    /* Return values from the TSS_Store/LoadSystemData function */

  /* Modules Browse Commands */
  #define TSS_BROWSE_COMMAND_ALL                  0x00u   /* Command for Modules Browse function */
  #define TSS_BROWSE_COMMAND_ENABLED              0x01u   /* Command for Modules Browse function */

  /* System fault */
  #define TSS_FAULT_UNDEFINED_ELECTRODE           0xFFu   /* System fault */

#endif /* TSS_STATUSCODES_H */
