/**********************************************************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2006-2009 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
********************************************************************************************************************//*!
*
* @file   TSS_DataTypes.h
*
* @brief  Header file that define Types, Structs Types and Constants of the TSS library
*
* @version 1.0.30.0
*
* @date Dec-3-2012
*
* These Types, Structs Types and Constants are used by the User Application Level and also internally in the Library
*
***********************************************************************************************************************/


#ifndef TSS_DATATYPES_H
  #define TSS_DATATYPES_H

  /***************************************************************************/
  /* Standard Definitions:                                                   */
  /* These defines allow for easier porting to other compilers. if porting   */
  /* change these defines to the required values for the chosen compiler.    */
  /***************************************************************************/

  /* C99 standard types definition */
  #if defined(__HCS08__) || defined(_lint)
    typedef unsigned char   uint8_t;      /* unsigned 8 bit definition */
    typedef unsigned short  uint16_t;     /* unsigned 16 bit definition */
    typedef unsigned long   uint32_t;     /* unsigned 32 bit definition */
    typedef signed char     int8_t;       /* signed 8 bit definition */
    typedef short           int16_t;      /* signed 16 bit definition */
    typedef long int        int32_t;      /* signed 32 bit definition */
  #else
    #include <stdint.h>
  #endif

  /* Legacy TSS types for back-compatibility */
  typedef unsigned char   UINT8;      /* unsigned 8 bit definition */
  typedef unsigned short  UINT16;     /* unsigned 16 bit definition */
  typedef unsigned long   UINT32;     /* unsigned 32 bit definition */
  typedef signed char     INT8;       /* signed 8 bit definition */
  typedef short           INT16;      /* signed 16 bit definition */
  typedef long int        INT32;      /* signed 32 bit definition */

  typedef uint8_t ( *TSS_ModulesBrowseCallback) (uint8_t u8Electrode, uint8_t u8Command);

  #ifndef NULL
    #define NULL          ((void *)0) /* NULL as a generic pointer */
  #endif

  #ifdef __HCS08__
    #pragma MESSAGE DISABLE C1106     /* WARNING C1106: Non-standard bitfield type */
  #endif

  /************************************************************************/
  /******************* Data Type structures definitions *******************/
  /************************************************************************/
  #if defined(__ARMCC_VERSION)
    #pragma diag_suppress 368
  #endif

  typedef struct {
    uint8_t ControlNumber     :4;
    uint8_t ControlType       :4;
  } TSS_CONTROL_ID;

  typedef struct {                    /* Struct for KeyPad Decoder */
    uint8_t                   :6;       /* This struct is for bit-fields only */
    uint8_t CallbackEn        :1;
    uint8_t ControlEn         :1;
  } TSS_KEYPAD_CONTCONF;

  typedef struct {                    /* Struct for KeyPad Decoder */
    uint8_t TouchEventEn      :1;       /* This struct is for bit-fields only */
    uint8_t ReleaseEventEn    :1;
    uint8_t AutoRepeatEventEn :1;
    uint8_t BufferFullOvfEn   :1;
    uint8_t KeysExceededEn    :1;
    uint8_t                   :1;
    uint8_t BufferOvfFlag     :1;
    uint8_t MaxKeysFlag       :1;
  } TSS_KEYPAD_EVENTS;

  typedef struct {                    /* Struct for Slider Decoder */
    uint8_t                   :6;       /* This struct is for bit-fields only */
    uint8_t CallbackEnabler   :1;
    uint8_t ControlEnabler    :1;
  } TSS_SLIDER_CONTROL;

  typedef struct {                    /* Struct for Slider Decoder */
    uint8_t Displacement      :4;       /* This struct is for bit-fields only */
    uint8_t                   :1;
    uint8_t                   :1;
    uint8_t Direction         :1;
    uint8_t Movement          :1;
  } TSS_SLIDER_DYN;

  typedef struct {                    /* Struct for Slider Decoder */
    uint8_t Position          :5;       /* This struct is for bit-fields only */
    uint8_t                   :1;
    uint8_t InvalidPos        :1;
    uint8_t Touch             :1;
  } TSS_SLIDER_STAT;

  typedef struct {                    /* Struct for Slider Decoder */
    uint8_t InitialTouchEn    :1;       /* This struct is for bit-fields only */
    uint8_t MovementEn        :1;
    uint8_t HoldEn            :1;
    uint8_t AutoRepeatEn      :1;
    uint8_t ReleaseEnabler    :1;
    uint8_t                   :1;
    uint8_t                   :1;
    uint8_t                   :1;
  } TSS_SLIDER_EVENTS;

  typedef struct {                    /* Struct for Analog Slider Decoder */
    uint8_t                   :6;       /* This struct is for bit-fields only */
    uint8_t CallbackEnabler   :1;
    uint8_t ControlEnabler    :1;
  } TSS_ASLIDER_CONTROL;

  typedef struct {                    /* Struct for Analog Slider Decoder */
    uint8_t Displacement      :6;       /* This struct is for bit-fields only */
    uint8_t Direction         :1;
    uint8_t Movement          :1;
  } TSS_ASLIDER_DYN;

  typedef struct{                     /* Struct for Analog Slider Decoder */
    uint8_t InitialTouchEn    :1;       /* This struct is for bit-fields only */
    uint8_t MovementEn        :1;
    uint8_t HoldEn            :1;
    uint8_t AutoRepeatEn      :1;
    uint8_t ReleaseEnabler    :1;
    uint8_t                   :1;
    uint8_t InvalidPos        :1;
    uint8_t Touch             :1;
  } TSS_ASLIDER_EVENTS;

  typedef struct {                    /* Struct for Matrix Decoder */
    uint8_t                   :6;       /* This struct is for bit-fields only */
    uint8_t CallbackEnabler   :1;
    uint8_t ControlEnabler    :1;
  } TSS_MATRIX_CONTROL;

  typedef struct {                    /* Struct for Matrix Decoder */
    uint8_t Displacement      :6;       /* This struct is for bit-fields only */
    uint8_t Direction         :1;
    uint8_t Movement          :1;
  } TSS_MATRIX_DYN;

  typedef struct {                    /* Struct for Matrix Decoder */
    uint8_t InitialTouchEn    :1;       /* This struct is for bit-fields only */
    uint8_t MovementEn        :1;
    uint8_t HoldEn            :1;
    uint8_t AutoRepeatEn      :1;
    uint8_t ReleaseEnabler    :1;
    uint8_t GesturesEn        :1;
    uint8_t Gesture           :1;
    uint8_t Touch             :1;
  } TSS_MATRIX_EVENTS;

  typedef struct {                    /* Struct for System Setup */
    uint8_t ChargeTimeout      :1;      /* This struct is for bit-fields only */
    uint8_t SmallCapacitor     :1;
    uint8_t DataCorruption     :1;
    uint8_t SmallTriggerPeriod :1;
    uint8_t NoiseMode          :1;
    uint8_t                    :1;
    uint8_t                    :1;
    uint8_t                    :1;
  } TSS_SYSTEM_FAULTS;

  /* LSB first in the structure else MSB first */
  #if defined(__BITFIELD_LSBIT_FIRST__)
    typedef struct {                  /* Struct for System Setup */
      uint16_t WaterToleranceEn  :1;
      uint16_t                   :1;
      uint16_t                   :1;
      uint16_t                   :1;
      uint16_t StuckKeyEn        :1;
      uint16_t DCTrackerEn       :1;
      uint16_t SWIEn             :1;
      uint16_t SystemEn          :1;
      uint16_t ManRecalStarter   :1;    /* This struct is for bit-fields only */
      uint16_t SystemReset       :1;
      uint16_t HWRecalStarter    :1;
      uint16_t                   :1;
      uint16_t                   :1;
      uint16_t                   :1;
      uint16_t LowPowerEn        :1;
      uint16_t ProximityEn       :1;
    } TSS_SYSTEM_SYSCONF;
  #else
    typedef struct {                  /* Struct for System Setup */
      uint16_t ManRecalStarter   :1;    /* This struct is for bit-fields only */
      uint16_t SystemReset       :1;
      uint16_t HWRecalStarter    :1;
      uint16_t                   :1;
      uint16_t                   :1;
      uint16_t                   :1;
      uint16_t LowPowerEn        :1;
      uint16_t ProximityEn       :1;
      uint16_t WaterToleranceEn  :1;
      uint16_t                   :1;
      uint16_t                   :1;
      uint16_t                   :1;
      uint16_t StuckKeyEn        :1;
      uint16_t DCTrackerEn       :1;
      uint16_t SWIEn             :1;
      uint16_t SystemEn          :1;
    } TSS_SYSTEM_SYSCONF;
  #endif

  typedef struct {                    /* Struct for Triggering setup */
    uint8_t TriggerMode       :2;
    uint8_t SWTrigger         :1;
    uint8_t Reserved          :5;
  } TSS_SYSTEM_TRIGGER;

  /******************************************************************/
  /****** Definition of control's context data for each decoder *****/
  /******************************************************************/

  /* Generic part of control context */

  typedef struct {
    uint16_t u16LastTouchFlag;
  } TSS_GenericRAMContext;

  typedef struct {
    const uint8_t *pu8Inputs;
    const uint8_t u8NumberOfInputs;
  } TSS_GenericROMContext;

  /* Structs for the context of a KeyPad Context*/

  typedef struct {
    TSS_GenericRAMContext sGenericRAMContext;
    uint16_t u16LastSignalChange;
    int8_t  i8KeyCount;
    uint8_t u8LastTouch;
  } TSS_KeypadRAMContext;

  typedef struct {
    TSS_GenericROMContext sGenericROMContext;
    const uint16_t *pu16KeyGroup;
    const uint8_t u8NumberOfKeys;
  } TSS_KeypadROMContext;

  /* Structs for the context of a ASlider Context*/

  typedef struct {
    uint16_t u16PrevPosition;
  } TSS_ASliderPosFilterContext;

  typedef struct {
    uint8_t u8BufferCounter;
  } TSS_ASliderDelayContext;

  typedef struct {
    uint8_t u8BufferCounter;
    int8_t i8PrevDelta1;
    int8_t i8PrevDelta2;
  } TSS_ASliderTrendContext;

  typedef struct {
    TSS_GenericROMContext sGenericROMContext;
    const TSS_ASliderPosFilterContext *psPosFilterContext;
    const TSS_ASliderDelayContext *psDelayContext;
    const TSS_ASliderTrendContext *psTrendContext;
    const uint8_t *pu8DelayBuffer;
    const int8_t *pi8TrendBuffer;
  } TSS_ASliderROMContext;

  /* Structs for the context of a Matrix Context*/

  typedef struct {
    uint8_t u8MaxPosX;
    uint8_t u8MinPosX;
    uint8_t u8MaxPosY;
    uint8_t u8MinPosY;
  } TSS_MatrixTouchContext;

  typedef struct {
    uint16_t u16PrevPositionX;
    uint16_t u16PrevPositionY;
  } TSS_MatrixPosIIRFilterContext;

  typedef struct {
    TSS_GenericROMContext sGenericROMContext;
    const uint8_t u8NumberOfInputsX;
    const TSS_MatrixTouchContext *cpsTouchData;
    const TSS_MatrixPosIIRFilterContext *psPosIIRFilterContext;
  } TSS_MatrixROMContext;

  /******************************************************************/
  /*************** Sensitivity Auto Calibration Data ****************/
  /******************************************************************/

  typedef struct {
    uint8_t u8FallNoiseAmplitudeCounter;
    int8_t i8FallDestinationDelta;
    uint8_t u8FallDestinationDeltaCounter;
  } TSS_AutoSensCalibrationData;

  typedef struct {
    int8_t i8AverageNoiseAmplitude;
    uint8_t u8NoiseAnalysisCounter;
  } TSS_DeltaNoiseAnalysisBufferData;

  typedef struct {
    int8_t i8InstantNoiseAmplitude;
    int16_t i16PrevIIRDeltaValue1;
    int16_t i16PrevIIRDeltaValue2;
  } TSS_DeltaNoiseAnalysisIIRData;

  typedef struct {
    const int8_t ci8NoiseLevelMinimum;           /* Minimum Noise Level */
    const uint8_t cu8NoiseLevelFallTimeout;      /* Max 255 */
    const uint8_t cu8MaxDeltaFallUpdateRate;     /* Max 255 */
    const int8_t ci8MaxDeltaToFallDeltaRatio;    /* Max Delta / Destination Fall Delta (1 unit = 1%) */
    const int8_t ci8ThresholdToNoiseRatio;       /* Threshld Level / Noise Percentage (1 unit = 10%) */
    const uint8_t cu8ThresholdToDeltaRatio;      /* Delta / Threshold Level (1 unit = 1%), 0 = auto */
  } TSS_KeyBasicAutoSensCalibContext;

  typedef struct {
    const int8_t ci8NoiseLevelMinimum;           /* Minimum Noise Level */
    const uint8_t cu8MaxDeltaFallUpdateRate;     /* Max 255 */
    const int8_t ci8MaxDeltaToFallDeltaRatio;    /* Max Delta / Destination Fall Delta (1 unit = 1%) */
    const int8_t ci8ThresholdToNoiseRatio;       /* Number of resets Threshld Level / Noise Percentage (1 unit = 10%) */
    const uint8_t cu8ResetsForTouch;             /* Number of required reset for touch, 0 = auto */
  } TSS_KeyAFIDAutoSensCalibContext;

  /******************************************************************************
  * FRACLIB common data type definitions					*
  ******************************************************************************/
  
  #if defined(__HCS08__)
    /* AFID is not used for PT60 anyway */
    typedef short         Frac64; /* Not available for PT60 */
    typedef short         Word64; /* Not available for PT60 */
    typedef short         Frac32; /* Not available for PT60 */
    typedef short         Word32; /* Not available for PT60 */
    typedef short         Frac16; /* Not available for PT60 */
    typedef short         Word16; /* Not available for PT60 */
  #else
    typedef long long     Frac64; /*!< User defined 64-bit fractional data type.  */
    typedef long long     Word64; /*!< User defined 64-bit integer data type.     */
    typedef long 	        Frac32; /*!< User defined 32-bit fractional data type.  */
    typedef long          Word32; /*!< User defined 32-bit integer data type.     */
    typedef short int     Frac16; /*!< User defined 16-bit fractional data type.  */
    typedef short int     Word16; /*!< User defined 16-bit integer data type.     */
  #endif
    
  /******************************************************************************
   * FRACLIB common macro definitions                                            *
   ******************************************************************************/
  /*lint -save -e961 suppress "Function-like macro defined [MISRA 2004 Rule 19.7]"*/  
  /*!
   * Macro for conversion double precission 64-bit floating point value into
   * 16-bit fractional value.
   */
  #define FRAC16(x)       (Frac64)((x)*(((x) > 0)?0x7fff:0x8000))
  #define F16TODBL(x)     (double)(((double)(x))/(double)0x8000)

  /*!
   * Macro for conversion double precission 64-bit floating point value into
   * 24-bit fractional value.
   */
  #define FRAC24(x)       (Frac64) ((x)*(((x) > 0)?0x7fffff:0x800000))
  #define F24TODBL(x)     (double) (((double) (x))/(double)0x800000)

  /*!
   * Macro for conversion double precission 64-bit floating point value into
   * 32-bit fractional value.
   */

  #define FRAC32(x)       (Frac64) ((x)*(((x) > 0)?0x7fffffff:0x80000000))
  #define F32TODBL(x)     (double) (((double) (x))/(double)0x80000000)

  /*!
   * Macro for conversion double precission 64-bit floating point value into
   * 48-bit fractional value.
   */
  #define FRAC48(x)       (Frac64) ((x)*(((x) > 0)?0x7fffffffffff:0x800000000000))
  #define F48TODBL(x)     (double) (((double) (x))/(double)0x800000000000)

  /*!
   * Macro for conversion double precission 64-bit floating point value into
   * 64-bit fractional value.
   */
  #define FRAC64(x)       (Frac64) ((x)*(((x) > 0)?0x7fffffffffffffff:0x8000000000000000))
  #define F64TODBL(x)     (double) (((double) (x))/(double)0x8000000000000000)
  /*lint -restore*/
    
  /******************************************************************/
  /*********************** Keydetector 2 Data ***********************/
  /******************************************************************/

  typedef struct
  {
    Frac16 xIIR[2u]; /*!< previous values of the prefilter inputs    */
    Frac32 yIIR[2u]; /*!< output value of the prefilter              */
  } TSS_KeyDetectorFiltersData;


  /********************************************************************/

#endif /* TSS_DATATYPES_H */
