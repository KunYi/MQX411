/***********************************************************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2006-2009 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
*********************************************************************************************************************//*!
*
* @file   TSS_SystemSetupData.c
*
* @brief  Defines the structs and information needed for the configuration and decoders usage
*
* @version 1.0.91.0
*
* @date Dec-3-2012
*
* All the declarations of structures and information are bassed in the values
* provided by the user in the SystemSetup.h file
*
***********************************************************************************************************************/


#include "TSS_SystemSetup.h"
#include "TSS_API.h"
#include "TSS_SystemSetupVal.h"
#include "derivative.h"

/* Prototypes */

uint16_t TSS_IIRFilterEquation(uint16_t u16CurrentSample, uint16_t u16PreviousSample);

/* Decoding macros */

#if TSS_ASLIDER_POSITION_IIR_USED
  #define TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_RESULT(res)     &TSS_C##res##_POSFILTER_CONTEXT
  #define TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_CONV(text)      TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_RESULT(text)
  #define TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(decoder)   TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_CONV(decoder)
#else
  #define TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(decoder)   NULL
#endif

#if TSS_ASLIDER_DELAY_USED
  #define TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_RESULT(res)     &TSS_C##res##_DELAY_CONTEXT
  #define TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_CONV(text)      TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_RESULT(text)
  #define TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(decoder)   TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_CONV(decoder)

  #define TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_RESULT(res)      TSS_C##res##_DELAY_BUFFER
  #define TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_CONV(text)       TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_RESULT(text)
  #define TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(decoder)    TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_CONV(decoder)
#else
  #define TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(decoder)   NULL
  #define TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(decoder)    NULL
#endif

#if TSS_ASLIDER_TREND_USED
  #define TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_RESULT(res)     &TSS_C##res##_TREND_CONTEXT
  #define TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_CONV(text)      TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_RESULT(text)
  #define TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(decoder)   TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_CONV(decoder)

  #define TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_RESULT(res)      TSS_C##res##_TREND_BUFFER
  #define TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_CONV(text)       TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_RESULT(text)
  #define TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(decoder)    TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_CONV(decoder)
#else
  #define TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(decoder)   NULL
  #define TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(decoder)    NULL
#endif

#if TSS_MATRIX_POSITION_IIR_USED
  #define TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_RESULT(res)     &TSS_C##res##_MATRIX_POSFILTER_IIR_CONTEXT
  #define TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_CONV(text)      TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_RESULT(text)
  #define TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(decoder)   TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_CONV(decoder)
#else
  #define TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(decoder)   NULL
#endif

#define TSS_COMPILE_TIME_ERROR(cond, msg) typedef char msg[(cond) ? 1 : -1];

/* The section below declares all the structures needed for each decoder */

#if TSS_N_CONTROLS > 0
  #if (TSS_C0_TYPE == TSS_CT_MATRIX)
    const uint8_t TSS_C0_INPUTS_GROUP[] =  TSS_C0_INPUTS;
    #define TSS_C0_INPUTS_COUNT sizeof(TSS_C0_INPUTS_GROUP)/sizeof(TSS_C0_INPUTS_GROUP[0])
    #define TSS_C0_INPUTS_X_COUNT TSS_C0_INPUTS_NUM_X
  #else
    const uint8_t TSS_C0_INPUTS_GROUP[] = TSS_C0_INPUTS;
    #define TSS_C0_INPUTS_COUNT sizeof(TSS_C0_INPUTS_GROUP)/sizeof(TSS_C0_INPUTS_GROUP[0])
  #endif
  #if (TSS_C0_TYPE == TSS_CT_KEYPAD)
    TSS_COMPILE_TIME_ERROR((TSS_C0_INPUTS_COUNT>0)&&(TSS_C0_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C0_INPUTS)
    uint8_t TSS_C0_KEYPAD_EVENTS_BUFFER[TSS_KEYPAD_EVENTS_BUFFER_SIZE];
    #ifdef TSS_C0_KEYS
      const uint16_t TSS_C0_KEYS_GROUP[] = TSS_C0_KEYS;
      #define TSS_C0_KEYS_COUNT sizeof(TSS_C0_KEYS_GROUP)/sizeof(TSS_C0_KEYS_GROUP[0])
      TSS_COMPILE_TIME_ERROR((TSS_C0_KEYS_COUNT<65536)&&(TSS_C0_KEYS_COUNT>0),TSS_Invalid_quantity_of_TSS_C0_KEYS)
    #else
      #define TSS_C0_KEYS_GROUP NULL
      #define TSS_C0_KEYS_COUNT TSS_C0_INPUTS_COUNT
    #endif
    TSS_CSKeypad TSS_C0_STRUCTURE = { {0u, TSS_C0_TYPE}, TSS_KEYPAD_CONTCONF_INIT, 0u,0u, TSS_KEYPAD_EVENTS_INIT, 0u,0u,0u, TSS_C0_KEYPAD_EVENTS_BUFFER };
    TSS_KeypadRAMContext TSS_C0_STRUCTURE_RAM_CONTEXT = { {0u},0u,0u,0u }; 
    const TSS_KeypadROMContext TSS_C0_STRUCTURE_ROM_CONTEXT = { {TSS_C0_INPUTS_GROUP, TSS_C0_INPUTS_COUNT}, TSS_C0_KEYS_GROUP, TSS_C0_KEYS_COUNT };
    #define C0_DECODER      TSS_KeypadDecoder
  #elif (TSS_C0_TYPE == TSS_CT_SLIDER)
    TSS_COMPILE_TIME_ERROR((TSS_C0_INPUTS_COUNT>1)&&(TSS_C0_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C0_INPUTS)
    TSS_CSSlider TSS_C0_STRUCTURE = { {0u, TSS_C0_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C0_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C0_STRUCTURE_ROM_CONTEXT = { TSS_C0_INPUTS_GROUP, TSS_C0_INPUTS_COUNT };
    #define C0_DECODER      TSS_SliderDecoder
  #elif (TSS_C0_TYPE == TSS_CT_ROTARY)
    TSS_COMPILE_TIME_ERROR((TSS_C0_INPUTS_COUNT>2)&&(TSS_C0_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C0_INPUTS)
    TSS_CSRotary TSS_C0_STRUCTURE = { {0u, TSS_C0_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C0_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C0_STRUCTURE_ROM_CONTEXT = { TSS_C0_INPUTS_GROUP, TSS_C0_INPUTS_COUNT };
    #define C0_DECODER      TSS_SliderDecoder
  #elif (TSS_C0_TYPE == TSS_CT_MATRIX)
    TSS_COMPILE_TIME_ERROR((TSS_C0_INPUTS_COUNT>3)&&(TSS_C0_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C0_INPUTS)
    TSS_CSMatrix TSS_C0_STRUCTURE = { {0u, TSS_C0_TYPE}, TSS_MATRIX_CONTROL_INIT, TSS_MATRIX_EVENTS_INIT, 0u,0u, TSS_MATRIX_DYN_INIT, TSS_MATRIX_DYN_INIT, 0u,0u,0u,0u,0u,0u };
    TSS_MatrixTouchContext TSS_C0_MATRIX_TOUCH_CONTEXT = { 0u,0u,0u,0u };
    TSS_MatrixPosIIRFilterContext TSS_C0_MATRIX_POSFILTER_IIR_CONTEXT = { 0u,0u };
    TSS_GenericRAMContext TSS_C0_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_MatrixROMContext TSS_C0_STRUCTURE_ROM_CONTEXT = { {TSS_C0_INPUTS_GROUP, TSS_C0_INPUTS_COUNT}, TSS_C0_INPUTS_X_COUNT,
                                                               &TSS_C0_MATRIX_TOUCH_CONTEXT,
                                                               TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(0)
                                                             };
    #define C0_DECODER      TSS_MatrixDecoder
  #else /* TSS_CT_ASLIDER || TSS_CT_AROTARY */
    #if (TSS_C0_TYPE == TSS_CT_ASLIDER)
      TSS_COMPILE_TIME_ERROR((TSS_C0_INPUTS_COUNT>1)&&(TSS_C0_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C0_INPUTS)
      TSS_CSASlider TSS_C0_STRUCTURE = { {0u, TSS_C0_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #else
      TSS_COMPILE_TIME_ERROR((TSS_C0_INPUTS_COUNT>2)&&(TSS_C0_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C0_INPUTS)
      TSS_CSARotary TSS_C0_STRUCTURE = { {0u, TSS_C0_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #endif
    TSS_ASliderPosFilterContext TSS_C0_POSFILTER_CONTEXT;
    TSS_ASliderDelayContext TSS_C0_DELAY_CONTEXT;
    TSS_ASliderTrendContext TSS_C0_TREND_CONTEXT;
    uint8_t TSS_C0_DELAY_BUFFER[TSS_ASLIDER_DELAY_BUFFER_LENGTH];
    int8_t TSS_C0_TREND_BUFFER[TSS_ASLIDER_TREND_BUFFER_LENGTH];
    TSS_GenericRAMContext TSS_C0_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_ASliderROMContext TSS_C0_STRUCTURE_ROM_CONTEXT = { {TSS_C0_INPUTS_GROUP, TSS_C0_INPUTS_COUNT},
                                                                  TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(0),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(0),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(0),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(0),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(0)
                                                                };     /* Declares the context ASlider Structure */
    #define C0_DECODER      TSS_ASliderDecoder
  #endif
  void TSS_C0_CALLBACK(TSS_CONTROL_ID u8ControlId);
  uint8_t C0_DECODER(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

#if TSS_N_CONTROLS > 1
  #if (TSS_C1_TYPE == TSS_CT_MATRIX)
    const uint8_t TSS_C1_INPUTS_GROUP[] =  TSS_C1_INPUTS;
    #define TSS_C1_INPUTS_COUNT sizeof(TSS_C1_INPUTS_GROUP)/sizeof(TSS_C1_INPUTS_GROUP[0])
    #define TSS_C1_INPUTS_X_COUNT TSS_C1_INPUTS_NUM_X
  #else
    const uint8_t TSS_C1_INPUTS_GROUP[] = TSS_C1_INPUTS;
    #define TSS_C1_INPUTS_COUNT sizeof(TSS_C1_INPUTS_GROUP)/sizeof(TSS_C1_INPUTS_GROUP[0])
  #endif
  #if (TSS_C1_TYPE == TSS_CT_KEYPAD)
    TSS_COMPILE_TIME_ERROR((TSS_C1_INPUTS_COUNT>0)&&(TSS_C1_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C1_INPUTS)
    uint8_t TSS_C1_KEYPAD_EVENTS_BUFFER[TSS_KEYPAD_EVENTS_BUFFER_SIZE];
    #ifdef TSS_C1_KEYS
      const uint16_t TSS_C1_KEYS_GROUP[] = TSS_C1_KEYS;
      #define TSS_C1_KEYS_COUNT sizeof(TSS_C1_KEYS_GROUP)/sizeof(TSS_C1_KEYS_GROUP[0])
      TSS_COMPILE_TIME_ERROR((TSS_C1_KEYS_COUNT<65536)&&(TSS_C1_KEYS_COUNT>0),TSS_Invalid_quantity_of_TSS_C1_KEYS)
    #else
      #define TSS_C1_KEYS_GROUP NULL
      #define TSS_C1_KEYS_COUNT TSS_C1_INPUTS_COUNT
    #endif
    TSS_CSKeypad TSS_C1_STRUCTURE = { {1u, TSS_C1_TYPE}, TSS_KEYPAD_CONTCONF_INIT, 0u,0u, TSS_KEYPAD_EVENTS_INIT, 0u,0u,0u, TSS_C1_KEYPAD_EVENTS_BUFFER };
    TSS_KeypadRAMContext TSS_C1_STRUCTURE_RAM_CONTEXT = { {0u},0u,0u,0u }; 
    const TSS_KeypadROMContext TSS_C1_STRUCTURE_ROM_CONTEXT = { {TSS_C1_INPUTS_GROUP, TSS_C1_INPUTS_COUNT}, TSS_C1_KEYS_GROUP, TSS_C1_KEYS_COUNT };
    #define C1_DECODER      TSS_KeypadDecoder
  #elif (TSS_C1_TYPE == TSS_CT_SLIDER)
    TSS_COMPILE_TIME_ERROR((TSS_C1_INPUTS_COUNT>1)&&(TSS_C1_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C1_INPUTS)
    TSS_CSSlider TSS_C1_STRUCTURE = { {1u, TSS_C1_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C1_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C1_STRUCTURE_ROM_CONTEXT = { TSS_C1_INPUTS_GROUP, TSS_C1_INPUTS_COUNT };
    #define C1_DECODER      TSS_SliderDecoder
  #elif (TSS_C1_TYPE == TSS_CT_ROTARY)
    TSS_COMPILE_TIME_ERROR((TSS_C1_INPUTS_COUNT>2)&&(TSS_C1_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C1_INPUTS)
    TSS_CSRotary TSS_C1_STRUCTURE = { {1u, TSS_C1_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C1_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C1_STRUCTURE_ROM_CONTEXT = { TSS_C1_INPUTS_GROUP, TSS_C1_INPUTS_COUNT };
    #define C1_DECODER      TSS_SliderDecoder
  #elif (TSS_C1_TYPE == TSS_CT_MATRIX)
    TSS_COMPILE_TIME_ERROR((TSS_C1_INPUTS_COUNT>3)&&(TSS_C1_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C1_INPUTS)
    TSS_CSMatrix TSS_C1_STRUCTURE = { {1u, TSS_C1_TYPE}, TSS_MATRIX_CONTROL_INIT, TSS_MATRIX_EVENTS_INIT, 0u,0u, TSS_MATRIX_DYN_INIT, TSS_MATRIX_DYN_INIT, 0u,0u,0u,0u,0u,0u };
    TSS_MatrixTouchContext TSS_C1_MATRIX_TOUCH_CONTEXT = { 0u,0u,0u,0u };
    TSS_MatrixPosIIRFilterContext TSS_C1_MATRIX_POSFILTER_IIR_CONTEXT = { 0u,0u };
    TSS_GenericRAMContext TSS_C1_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_MatrixROMContext TSS_C1_STRUCTURE_ROM_CONTEXT = { {TSS_C1_INPUTS_GROUP, TSS_C1_INPUTS_COUNT}, TSS_C1_INPUTS_X_COUNT,
                                                               &TSS_C1_MATRIX_TOUCH_CONTEXT,
                                                               TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(1)
                                                             };
    #define C1_DECODER      TSS_MatrixDecoder
  #else /* TSS_CT_ASLIDER || TSS_CT_AROTARY */
    #if (TSS_C1_TYPE == TSS_CT_ASLIDER)
      TSS_COMPILE_TIME_ERROR((TSS_C1_INPUTS_COUNT>1)&&(TSS_C1_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C1_INPUTS)
      TSS_CSASlider TSS_C1_STRUCTURE = { {1u, TSS_C1_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #else
      TSS_COMPILE_TIME_ERROR((TSS_C1_INPUTS_COUNT>2)&&(TSS_C1_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C1_INPUTS)
      TSS_CSARotary TSS_C1_STRUCTURE = { {1u, TSS_C1_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #endif
    TSS_ASliderPosFilterContext TSS_C1_POSFILTER_CONTEXT;
    TSS_ASliderDelayContext TSS_C1_DELAY_CONTEXT;
    TSS_ASliderTrendContext TSS_C1_TREND_CONTEXT;
    uint8_t TSS_C1_DELAY_BUFFER[TSS_ASLIDER_DELAY_BUFFER_LENGTH];
    int8_t TSS_C1_TREND_BUFFER[TSS_ASLIDER_TREND_BUFFER_LENGTH];
    TSS_GenericRAMContext TSS_C1_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_ASliderROMContext TSS_C1_STRUCTURE_ROM_CONTEXT = { {TSS_C1_INPUTS_GROUP, TSS_C1_INPUTS_COUNT},
                                                                  TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(1),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(1),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(1),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(1),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(1)
                                                                };     /* Declares the context ASlider Structure */
    #define C1_DECODER      TSS_ASliderDecoder
  #endif
  void TSS_C1_CALLBACK(TSS_CONTROL_ID u8ControlId);
  uint8_t C1_DECODER(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

#if TSS_N_CONTROLS > 2
  #if (TSS_C2_TYPE == TSS_CT_MATRIX)
    const uint8_t TSS_C2_INPUTS_GROUP[] =  TSS_C2_INPUTS;
    #define TSS_C2_INPUTS_COUNT sizeof(TSS_C2_INPUTS_GROUP)/sizeof(TSS_C2_INPUTS_GROUP[0])
    #define TSS_C2_INPUTS_X_COUNT TSS_C2_INPUTS_NUM_X
  #else
    const uint8_t TSS_C2_INPUTS_GROUP[] = TSS_C2_INPUTS;
    #define TSS_C2_INPUTS_COUNT sizeof(TSS_C2_INPUTS_GROUP)/sizeof(TSS_C2_INPUTS_GROUP[0])
  #endif
  #if (TSS_C2_TYPE == TSS_CT_KEYPAD)
    TSS_COMPILE_TIME_ERROR((TSS_C2_INPUTS_COUNT>0)&&(TSS_C2_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C2_INPUTS)
    uint8_t TSS_C2_KEYPAD_EVENTS_BUFFER[TSS_KEYPAD_EVENTS_BUFFER_SIZE];
    #ifdef TSS_C2_KEYS
      const uint16_t TSS_C2_KEYS_GROUP[] = TSS_C2_KEYS;
      #define TSS_C2_KEYS_COUNT sizeof(TSS_C2_KEYS_GROUP)/sizeof(TSS_C2_KEYS_GROUP[0])
      TSS_COMPILE_TIME_ERROR((TSS_C2_KEYS_COUNT<65536)&&(TSS_C2_KEYS_COUNT>0),TSS_Invalid_quantity_of_TSS_C2_KEYS)
    #else
      #define TSS_C2_KEYS_GROUP NULL
      #define TSS_C2_KEYS_COUNT TSS_C2_INPUTS_COUNT
    #endif
    TSS_CSKeypad TSS_C2_STRUCTURE = { {2u, TSS_C2_TYPE}, TSS_KEYPAD_CONTCONF_INIT, 0u,0u, TSS_KEYPAD_EVENTS_INIT, 0u,0u,0u, TSS_C2_KEYPAD_EVENTS_BUFFER };
    TSS_KeypadRAMContext TSS_C2_STRUCTURE_RAM_CONTEXT = { {0u},0u,0u,0u }; 
    const TSS_KeypadROMContext TSS_C2_STRUCTURE_ROM_CONTEXT = { {TSS_C2_INPUTS_GROUP, TSS_C2_INPUTS_COUNT}, TSS_C2_KEYS_GROUP, TSS_C2_KEYS_COUNT };
    #define C2_DECODER      TSS_KeypadDecoder
  #elif (TSS_C2_TYPE == TSS_CT_SLIDER)
    TSS_COMPILE_TIME_ERROR((TSS_C2_INPUTS_COUNT>1)&&(TSS_C2_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C2_INPUTS)
    TSS_CSSlider TSS_C2_STRUCTURE = { {2u, TSS_C2_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C2_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C2_STRUCTURE_ROM_CONTEXT = { TSS_C2_INPUTS_GROUP, TSS_C2_INPUTS_COUNT };
    #define C2_DECODER      TSS_SliderDecoder
  #elif (TSS_C2_TYPE == TSS_CT_ROTARY)
    TSS_COMPILE_TIME_ERROR((TSS_C2_INPUTS_COUNT>2)&&(TSS_C2_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C2_INPUTS)
    TSS_CSRotary TSS_C2_STRUCTURE = { {2u, TSS_C2_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C2_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C2_STRUCTURE_ROM_CONTEXT = { TSS_C2_INPUTS_GROUP, TSS_C2_INPUTS_COUNT };
    #define C2_DECODER      TSS_SliderDecoder
  #elif (TSS_C2_TYPE == TSS_CT_MATRIX)
    TSS_COMPILE_TIME_ERROR((TSS_C2_INPUTS_COUNT>3)&&(TSS_C2_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C2_INPUTS)
    TSS_CSMatrix TSS_C2_STRUCTURE = { {2u, TSS_C2_TYPE}, TSS_MATRIX_CONTROL_INIT, TSS_MATRIX_EVENTS_INIT, 0u,0u, TSS_MATRIX_DYN_INIT, TSS_MATRIX_DYN_INIT, 0u,0u,0u,0u,0u,0u };
    TSS_MatrixTouchContext TSS_C2_MATRIX_TOUCH_CONTEXT = { 0u,0u,0u,0u };
    TSS_MatrixPosIIRFilterContext TSS_C2_MATRIX_POSFILTER_IIR_CONTEXT = { 0u,0u };
    TSS_GenericRAMContext TSS_C2_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_MatrixROMContext TSS_C2_STRUCTURE_ROM_CONTEXT = { {TSS_C2_INPUTS_GROUP, TSS_C2_INPUTS_COUNT}, TSS_C2_INPUTS_X_COUNT,
                                                               &TSS_C2_MATRIX_TOUCH_CONTEXT,
                                                               TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(2)
                                                             };
    #define C2_DECODER      TSS_MatrixDecoder
  #else /* TSS_CT_ASLIDER || TSS_CT_AROTARY */
    #if (TSS_C2_TYPE == TSS_CT_ASLIDER)
      TSS_COMPILE_TIME_ERROR((TSS_C2_INPUTS_COUNT>1)&&(TSS_C2_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C2_INPUTS)
      TSS_CSASlider TSS_C2_STRUCTURE = { {2u, TSS_C2_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #else
      TSS_COMPILE_TIME_ERROR((TSS_C2_INPUTS_COUNT>2)&&(TSS_C2_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C2_INPUTS)
      TSS_CSARotary TSS_C2_STRUCTURE = { {2u, TSS_C2_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #endif
    TSS_ASliderPosFilterContext TSS_C2_POSFILTER_CONTEXT;
    TSS_ASliderDelayContext TSS_C2_DELAY_CONTEXT;
    TSS_ASliderTrendContext TSS_C2_TREND_CONTEXT;
    uint8_t TSS_C2_DELAY_BUFFER[TSS_ASLIDER_DELAY_BUFFER_LENGTH];
    int8_t TSS_C2_TREND_BUFFER[TSS_ASLIDER_TREND_BUFFER_LENGTH];
    TSS_GenericRAMContext TSS_C2_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_ASliderROMContext TSS_C2_STRUCTURE_ROM_CONTEXT = { {TSS_C2_INPUTS_GROUP, TSS_C2_INPUTS_COUNT},
                                                                  TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(2),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(2),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(2),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(2),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(2)
                                                                };     /* Declares the context ASlider Structure */
    #define C2_DECODER      TSS_ASliderDecoder
  #endif
  void TSS_C2_CALLBACK(TSS_CONTROL_ID u8ControlId);
  uint8_t C2_DECODER(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

#if TSS_N_CONTROLS > 3
  #if (TSS_C3_TYPE == TSS_CT_MATRIX)
    const uint8_t TSS_C3_INPUTS_GROUP[] =  TSS_C3_INPUTS;
    #define TSS_C3_INPUTS_COUNT sizeof(TSS_C3_INPUTS_GROUP)/sizeof(TSS_C3_INPUTS_GROUP[0])
    #define TSS_C3_INPUTS_X_COUNT TSS_C3_INPUTS_NUM_X
  #else
    const uint8_t TSS_C3_INPUTS_GROUP[] = TSS_C3_INPUTS;
    #define TSS_C3_INPUTS_COUNT sizeof(TSS_C3_INPUTS_GROUP)/sizeof(TSS_C3_INPUTS_GROUP[0])
  #endif
  #if (TSS_C3_TYPE == TSS_CT_KEYPAD)
    TSS_COMPILE_TIME_ERROR((TSS_C3_INPUTS_COUNT>0)&&(TSS_C3_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C3_INPUTS)
    uint8_t TSS_C3_KEYPAD_EVENTS_BUFFER[TSS_KEYPAD_EVENTS_BUFFER_SIZE];
    #ifdef TSS_C3_KEYS
      const uint16_t TSS_C3_KEYS_GROUP[] = TSS_C3_KEYS;
      #define TSS_C3_KEYS_COUNT sizeof(TSS_C3_KEYS_GROUP)/sizeof(TSS_C3_KEYS_GROUP[0])
      TSS_COMPILE_TIME_ERROR((TSS_C3_KEYS_COUNT<65536)&&(TSS_C3_KEYS_COUNT>0),TSS_Invalid_quantity_of_TSS_C3_KEYS)
    #else
      #define TSS_C3_KEYS_GROUP NULL
      #define TSS_C3_KEYS_COUNT TSS_C3_INPUTS_COUNT
    #endif
    TSS_CSKeypad TSS_C3_STRUCTURE = { {3u, TSS_C3_TYPE}, TSS_KEYPAD_CONTCONF_INIT, 0u,0u, TSS_KEYPAD_EVENTS_INIT, 0u,0u,0u, TSS_C3_KEYPAD_EVENTS_BUFFER };
    TSS_KeypadRAMContext TSS_C3_STRUCTURE_RAM_CONTEXT = { {0u},0u,0u,0u }; 
    const TSS_KeypadROMContext TSS_C3_STRUCTURE_ROM_CONTEXT = { {TSS_C3_INPUTS_GROUP, TSS_C3_INPUTS_COUNT}, TSS_C3_KEYS_GROUP, TSS_C3_KEYS_COUNT };
    #define C3_DECODER      TSS_KeypadDecoder
  #elif (TSS_C3_TYPE == TSS_CT_SLIDER)
    TSS_COMPILE_TIME_ERROR((TSS_C3_INPUTS_COUNT>1)&&(TSS_C3_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C3_INPUTS)
    TSS_CSSlider TSS_C3_STRUCTURE = { {3u, TSS_C3_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C3_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C3_STRUCTURE_ROM_CONTEXT = { TSS_C3_INPUTS_GROUP, TSS_C3_INPUTS_COUNT };
    #define C3_DECODER      TSS_SliderDecoder
  #elif (TSS_C3_TYPE == TSS_CT_ROTARY)
    TSS_COMPILE_TIME_ERROR((TSS_C3_INPUTS_COUNT>2)&&(TSS_C3_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C3_INPUTS)
    TSS_CSRotary TSS_C3_STRUCTURE = { {3u, TSS_C3_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C3_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C3_STRUCTURE_ROM_CONTEXT = { TSS_C3_INPUTS_GROUP, TSS_C3_INPUTS_COUNT };
    #define C3_DECODER      TSS_SliderDecoder
  #elif (TSS_C3_TYPE == TSS_CT_MATRIX)
    TSS_COMPILE_TIME_ERROR((TSS_C3_INPUTS_COUNT>3)&&(TSS_C3_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C3_INPUTS)
    TSS_CSMatrix TSS_C3_STRUCTURE = { {3u, TSS_C3_TYPE}, TSS_MATRIX_CONTROL_INIT, TSS_MATRIX_EVENTS_INIT, 0u,0u, TSS_MATRIX_DYN_INIT, TSS_MATRIX_DYN_INIT, 0u,0u,0u,0u,0u,0u };
    TSS_MatrixTouchContext TSS_C3_MATRIX_TOUCH_CONTEXT = { 0u,0u,0u,0u };
    TSS_MatrixPosIIRFilterContext TSS_C3_MATRIX_POSFILTER_IIR_CONTEXT = { 0u,0u };
    TSS_GenericRAMContext TSS_C3_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_MatrixROMContext TSS_C3_STRUCTURE_ROM_CONTEXT = { {TSS_C3_INPUTS_GROUP, TSS_C3_INPUTS_COUNT}, TSS_C3_INPUTS_X_COUNT,
                                                               &TSS_C3_MATRIX_TOUCH_CONTEXT,
                                                               TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(3)
                                                             };
    #define C3_DECODER      TSS_MatrixDecoder
  #else /* TSS_CT_ASLIDER || TSS_CT_AROTARY */
    #if (TSS_C3_TYPE == TSS_CT_ASLIDER)
      TSS_COMPILE_TIME_ERROR((TSS_C3_INPUTS_COUNT>1)&&(TSS_C3_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C3_INPUTS)
      TSS_CSASlider TSS_C3_STRUCTURE = { {3u, TSS_C3_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #else
      TSS_COMPILE_TIME_ERROR((TSS_C3_INPUTS_COUNT>2)&&(TSS_C3_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C3_INPUTS)
      TSS_CSARotary TSS_C3_STRUCTURE = { {3u, TSS_C3_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #endif
    TSS_ASliderPosFilterContext TSS_C3_POSFILTER_CONTEXT;
    TSS_ASliderDelayContext TSS_C3_DELAY_CONTEXT;
    TSS_ASliderTrendContext TSS_C3_TREND_CONTEXT;
    uint8_t TSS_C3_DELAY_BUFFER[TSS_ASLIDER_DELAY_BUFFER_LENGTH];
    int8_t TSS_C3_TREND_BUFFER[TSS_ASLIDER_TREND_BUFFER_LENGTH];
    TSS_GenericRAMContext TSS_C3_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_ASliderROMContext TSS_C3_STRUCTURE_ROM_CONTEXT = { {TSS_C3_INPUTS_GROUP, TSS_C3_INPUTS_COUNT},
                                                                  TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(3),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(3),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(3),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(3),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(3)
                                                                };     /* Declares the context ASlider Structure */
    #define C3_DECODER      TSS_ASliderDecoder
  #endif
  void TSS_C3_CALLBACK(TSS_CONTROL_ID u8ControlId);
  uint8_t C3_DECODER(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

#if TSS_N_CONTROLS > 4
  #if (TSS_C4_TYPE == TSS_CT_MATRIX)
    const uint8_t TSS_C4_INPUTS_GROUP[] =  TSS_C4_INPUTS;
    #define TSS_C4_INPUTS_COUNT sizeof(TSS_C4_INPUTS_GROUP)/sizeof(TSS_C4_INPUTS_GROUP[0])
    #define TSS_C4_INPUTS_X_COUNT TSS_C4_INPUTS_NUM_X
  #else
    const uint8_t TSS_C4_INPUTS_GROUP[] = TSS_C4_INPUTS;
    #define TSS_C4_INPUTS_COUNT sizeof(TSS_C4_INPUTS_GROUP)/sizeof(TSS_C4_INPUTS_GROUP[0])
  #endif
  #if (TSS_C4_TYPE == TSS_CT_KEYPAD)
    TSS_COMPILE_TIME_ERROR((TSS_C4_INPUTS_COUNT>0)&&(TSS_C4_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C4_INPUTS)
    uint8_t TSS_C4_KEYPAD_EVENTS_BUFFER[TSS_KEYPAD_EVENTS_BUFFER_SIZE];
    #ifdef TSS_C4_KEYS
      const uint16_t TSS_C4_KEYS_GROUP[] = TSS_C4_KEYS;
      #define TSS_C4_KEYS_COUNT sizeof(TSS_C4_KEYS_GROUP)/sizeof(TSS_C4_KEYS_GROUP[0])
      TSS_COMPILE_TIME_ERROR((TSS_C4_KEYS_COUNT<65536)&&(TSS_C4_KEYS_COUNT>0),TSS_Invalid_quantity_of_TSS_C4_KEYS)
    #else
      #define TSS_C4_KEYS_GROUP NULL
      #define TSS_C4_KEYS_COUNT TSS_C4_INPUTS_COUNT
    #endif
    TSS_CSKeypad TSS_C4_STRUCTURE = { {4u, TSS_C4_TYPE}, TSS_KEYPAD_CONTCONF_INIT, 0u,0u, TSS_KEYPAD_EVENTS_INIT, 0u,0u,0u, TSS_C4_KEYPAD_EVENTS_BUFFER };
    TSS_KeypadRAMContext TSS_C4_STRUCTURE_RAM_CONTEXT = { {0u},0u,0u,0u }; 
    const TSS_KeypadROMContext TSS_C4_STRUCTURE_ROM_CONTEXT = { {TSS_C4_INPUTS_GROUP, TSS_C4_INPUTS_COUNT}, TSS_C4_KEYS_GROUP, TSS_C4_KEYS_COUNT };
    #define C4_DECODER      TSS_KeypadDecoder
  #elif (TSS_C4_TYPE == TSS_CT_SLIDER)
    TSS_COMPILE_TIME_ERROR((TSS_C4_INPUTS_COUNT>1)&&(TSS_C4_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C4_INPUTS)
    TSS_CSSlider TSS_C4_STRUCTURE = { {4u, TSS_C4_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C4_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C4_STRUCTURE_ROM_CONTEXT = { TSS_C4_INPUTS_GROUP, TSS_C4_INPUTS_COUNT };
    #define C4_DECODER      TSS_SliderDecoder
  #elif (TSS_C4_TYPE == TSS_CT_ROTARY)
    TSS_COMPILE_TIME_ERROR((TSS_C4_INPUTS_COUNT>2)&&(TSS_C4_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C4_INPUTS)
    TSS_CSRotary TSS_C4_STRUCTURE = { {4u, TSS_C4_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C4_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C4_STRUCTURE_ROM_CONTEXT = { TSS_C4_INPUTS_GROUP, TSS_C4_INPUTS_COUNT };
    #define C4_DECODER      TSS_SliderDecoder
  #elif (TSS_C4_TYPE == TSS_CT_MATRIX)
    TSS_COMPILE_TIME_ERROR((TSS_C4_INPUTS_COUNT>3)&&(TSS_C4_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C4_INPUTS)
    TSS_CSMatrix TSS_C4_STRUCTURE = { {4u, TSS_C4_TYPE}, TSS_MATRIX_CONTROL_INIT, TSS_MATRIX_EVENTS_INIT, 0u,0u, TSS_MATRIX_DYN_INIT, TSS_MATRIX_DYN_INIT, 0u,0u,0u,0u,0u,0u };
    TSS_MatrixTouchContext TSS_C4_MATRIX_TOUCH_CONTEXT = { 0u,0u,0u,0u };
    TSS_MatrixPosIIRFilterContext TSS_C4_MATRIX_POSFILTER_IIR_CONTEXT = { 0u,0u };
    TSS_GenericRAMContext TSS_C4_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_MatrixROMContext TSS_C4_STRUCTURE_ROM_CONTEXT = { {TSS_C4_INPUTS_GROUP, TSS_C4_INPUTS_COUNT}, TSS_C4_INPUTS_X_COUNT,
                                                               &TSS_C4_MATRIX_TOUCH_CONTEXT,
                                                               TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(4)
                                                             };
    #define C4_DECODER      TSS_MatrixDecoder
  #else /* TSS_CT_ASLIDER || TSS_CT_AROTARY */
    #if (TSS_C4_TYPE == TSS_CT_ASLIDER)
      TSS_COMPILE_TIME_ERROR((TSS_C4_INPUTS_COUNT>1)&&(TSS_C4_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C4_INPUTS)
      TSS_CSASlider TSS_C4_STRUCTURE = { {4u, TSS_C4_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #else
      TSS_COMPILE_TIME_ERROR((TSS_C4_INPUTS_COUNT>2)&&(TSS_C4_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C4_INPUTS)
      TSS_CSARotary TSS_C4_STRUCTURE = { {4u, TSS_C4_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #endif
    TSS_ASliderPosFilterContext TSS_C4_POSFILTER_CONTEXT;
    TSS_ASliderDelayContext TSS_C4_DELAY_CONTEXT;
    TSS_ASliderTrendContext TSS_C4_TREND_CONTEXT;
    uint8_t TSS_C4_DELAY_BUFFER[TSS_ASLIDER_DELAY_BUFFER_LENGTH];
    int8_t TSS_C4_TREND_BUFFER[TSS_ASLIDER_TREND_BUFFER_LENGTH];
    TSS_GenericRAMContext TSS_C4_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_ASliderROMContext TSS_C4_STRUCTURE_ROM_CONTEXT = { {TSS_C4_INPUTS_GROUP, TSS_C4_INPUTS_COUNT},
                                                                  TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(4),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(4),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(4),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(4),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(4)
                                                                };     /* Declares the context ASlider Structure */
    #define C4_DECODER      TSS_ASliderDecoder
  #endif
  void TSS_C4_CALLBACK(TSS_CONTROL_ID u8ControlId);
  uint8_t C4_DECODER(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

#if TSS_N_CONTROLS > 5
  #if (TSS_C5_TYPE == TSS_CT_MATRIX)
    const uint8_t TSS_C5_INPUTS_GROUP[] =  TSS_C5_INPUTS;
    #define TSS_C5_INPUTS_COUNT sizeof(TSS_C5_INPUTS_GROUP)/sizeof(TSS_C5_INPUTS_GROUP[0])
    #define TSS_C5_INPUTS_X_COUNT TSS_C5_INPUTS_NUM_X
  #else
    const uint8_t TSS_C5_INPUTS_GROUP[] = TSS_C5_INPUTS;
    #define TSS_C5_INPUTS_COUNT sizeof(TSS_C5_INPUTS_GROUP)/sizeof(TSS_C5_INPUTS_GROUP[0])
  #endif
  #if (TSS_C5_TYPE == TSS_CT_KEYPAD)
    TSS_COMPILE_TIME_ERROR((TSS_C5_INPUTS_COUNT>0)&&(TSS_C5_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C5_INPUTS)
    uint8_t TSS_C5_KEYPAD_EVENTS_BUFFER[TSS_KEYPAD_EVENTS_BUFFER_SIZE];
    #ifdef TSS_C5_KEYS
      const uint16_t TSS_C5_KEYS_GROUP[] = TSS_C5_KEYS;
      #define TSS_C5_KEYS_COUNT sizeof(TSS_C5_KEYS_GROUP)/sizeof(TSS_C5_KEYS_GROUP[0])
      TSS_COMPILE_TIME_ERROR((TSS_C5_KEYS_COUNT<65536)&&(TSS_C5_KEYS_COUNT>0),TSS_Invalid_quantity_of_TSS_C5_KEYS)
    #else
      #define TSS_C5_KEYS_GROUP NULL
      #define TSS_C5_KEYS_COUNT TSS_C5_INPUTS_COUNT
    #endif
    TSS_CSKeypad TSS_C5_STRUCTURE = { {5u, TSS_C5_TYPE}, TSS_KEYPAD_CONTCONF_INIT, 0u,0u, TSS_KEYPAD_EVENTS_INIT, 0u,0u,0u, TSS_C5_KEYPAD_EVENTS_BUFFER };
    TSS_KeypadRAMContext TSS_C5_STRUCTURE_RAM_CONTEXT = { {0u},0u,0u,0u }; 
    const TSS_KeypadROMContext TSS_C5_STRUCTURE_ROM_CONTEXT = { {TSS_C5_INPUTS_GROUP, TSS_C5_INPUTS_COUNT}, TSS_C5_KEYS_GROUP, TSS_C5_KEYS_COUNT };
    #define C5_DECODER      TSS_KeypadDecoder
  #elif (TSS_C5_TYPE == TSS_CT_SLIDER)
    TSS_COMPILE_TIME_ERROR((TSS_C5_INPUTS_COUNT>1)&&(TSS_C5_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C5_INPUTS)
    TSS_CSSlider TSS_C5_STRUCTURE = { {5u, TSS_C5_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C5_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C5_STRUCTURE_ROM_CONTEXT = { TSS_C5_INPUTS_GROUP, TSS_C5_INPUTS_COUNT };
    #define C5_DECODER      TSS_SliderDecoder
  #elif (TSS_C5_TYPE == TSS_CT_ROTARY)
    TSS_COMPILE_TIME_ERROR((TSS_C5_INPUTS_COUNT>2)&&(TSS_C5_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C5_INPUTS)
    TSS_CSRotary TSS_C5_STRUCTURE = { {5u, TSS_C5_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C5_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C5_STRUCTURE_ROM_CONTEXT = { TSS_C5_INPUTS_GROUP, TSS_C5_INPUTS_COUNT };
    #define C5_DECODER      TSS_SliderDecoder
  #elif (TSS_C5_TYPE == TSS_CT_MATRIX)
    TSS_COMPILE_TIME_ERROR((TSS_C5_INPUTS_COUNT>3)&&(TSS_C5_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C5_INPUTS)
    TSS_CSMatrix TSS_C5_STRUCTURE = { {5u, TSS_C5_TYPE}, TSS_MATRIX_CONTROL_INIT, TSS_MATRIX_EVENTS_INIT, 0u,0u, TSS_MATRIX_DYN_INIT, TSS_MATRIX_DYN_INIT, 0u,0u,0u,0u,0u,0u };
    TSS_MatrixTouchContext TSS_C5_MATRIX_TOUCH_CONTEXT = { 0u,0u,0u,0u };
    TSS_MatrixPosIIRFilterContext TSS_C5_MATRIX_POSFILTER_IIR_CONTEXT = { 0u,0u };
    TSS_GenericRAMContext TSS_C5_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_MatrixROMContext TSS_C5_STRUCTURE_ROM_CONTEXT = { {TSS_C5_INPUTS_GROUP, TSS_C5_INPUTS_COUNT}, TSS_C5_INPUTS_X_COUNT,
                                                               &TSS_C5_MATRIX_TOUCH_CONTEXT,
                                                               TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(5)
                                                             };
    #define C5_DECODER      TSS_MatrixDecoder
  #else /* TSS_CT_ASLIDER || TSS_CT_AROTARY */
    #if (TSS_C5_TYPE == TSS_CT_ASLIDER)
      TSS_COMPILE_TIME_ERROR((TSS_C5_INPUTS_COUNT>1)&&(TSS_C5_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C5_INPUTS)
      TSS_CSASlider TSS_C5_STRUCTURE = { {5u, TSS_C5_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #else
      TSS_COMPILE_TIME_ERROR((TSS_C5_INPUTS_COUNT>2)&&(TSS_C5_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C5_INPUTS)
      TSS_CSARotary TSS_C5_STRUCTURE = { {5u, TSS_C5_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #endif
    TSS_ASliderPosFilterContext TSS_C5_POSFILTER_CONTEXT;
    TSS_ASliderDelayContext TSS_C5_DELAY_CONTEXT;
    TSS_ASliderTrendContext TSS_C5_TREND_CONTEXT;
    uint8_t TSS_C5_DELAY_BUFFER[TSS_ASLIDER_DELAY_BUFFER_LENGTH];
    int8_t TSS_C5_TREND_BUFFER[TSS_ASLIDER_TREND_BUFFER_LENGTH];
    TSS_GenericRAMContext TSS_C5_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_ASliderROMContext TSS_C5_STRUCTURE_ROM_CONTEXT = { {TSS_C5_INPUTS_GROUP, TSS_C5_INPUTS_COUNT},
                                                                  TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(5),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(5),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(5),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(5),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(5)
                                                                };     /* Declares the context ASlider Structure */
    #define C5_DECODER      TSS_ASliderDecoder
  #endif
  void TSS_C5_CALLBACK(TSS_CONTROL_ID u8ControlId);
  uint8_t C5_DECODER(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

#if TSS_N_CONTROLS > 6
  #if (TSS_C6_TYPE == TSS_CT_MATRIX)
    const uint8_t TSS_C6_INPUTS_GROUP[] =  TSS_C6_INPUTS;
    #define TSS_C6_INPUTS_COUNT sizeof(TSS_C6_INPUTS_GROUP)/sizeof(TSS_C6_INPUTS_GROUP[0])
    #define TSS_C6_INPUTS_X_COUNT TSS_C6_INPUTS_NUM_X
  #else
    const uint8_t TSS_C6_INPUTS_GROUP[] = TSS_C6_INPUTS;
    #define TSS_C6_INPUTS_COUNT sizeof(TSS_C6_INPUTS_GROUP)/sizeof(TSS_C6_INPUTS_GROUP[0])
  #endif
  #if (TSS_C6_TYPE == TSS_CT_KEYPAD)
    TSS_COMPILE_TIME_ERROR((TSS_C6_INPUTS_COUNT>0)&&(TSS_C6_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C6_INPUTS)
    uint8_t TSS_C6_KEYPAD_EVENTS_BUFFER[TSS_KEYPAD_EVENTS_BUFFER_SIZE];
    #ifdef TSS_C6_KEYS
      const uint16_t TSS_C6_KEYS_GROUP[] = TSS_C6_KEYS;
      #define TSS_C6_KEYS_COUNT sizeof(TSS_C6_KEYS_GROUP)/sizeof(TSS_C6_KEYS_GROUP[0])
      TSS_COMPILE_TIME_ERROR((TSS_C6_KEYS_COUNT<65536)&&(TSS_C6_KEYS_COUNT>0),TSS_Invalid_quantity_of_TSS_C6_KEYS)
    #else
      #define TSS_C6_KEYS_GROUP NULL
      #define TSS_C6_KEYS_COUNT TSS_C6_INPUTS_COUNT
    #endif
    TSS_CSKeypad TSS_C6_STRUCTURE = { {6u, TSS_C6_TYPE}, TSS_KEYPAD_CONTCONF_INIT, 0u,0u, TSS_KEYPAD_EVENTS_INIT, 0u,0u,0u, TSS_C6_KEYPAD_EVENTS_BUFFER };
    TSS_KeypadRAMContext TSS_C6_STRUCTURE_RAM_CONTEXT = { {0u},0u,0u,0u }; 
    const TSS_KeypadROMContext TSS_C6_STRUCTURE_ROM_CONTEXT = { {TSS_C6_INPUTS_GROUP, TSS_C6_INPUTS_COUNT}, TSS_C6_KEYS_GROUP, TSS_C6_KEYS_COUNT };
    #define C6_DECODER      TSS_KeypadDecoder
  #elif (TSS_C6_TYPE == TSS_CT_SLIDER)
    TSS_COMPILE_TIME_ERROR((TSS_C6_INPUTS_COUNT>1)&&(TSS_C6_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C6_INPUTS)
    TSS_CSSlider TSS_C6_STRUCTURE = { {6u, TSS_C6_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C6_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C6_STRUCTURE_ROM_CONTEXT = { TSS_C6_INPUTS_GROUP, TSS_C6_INPUTS_COUNT };
    #define C6_DECODER      TSS_SliderDecoder
  #elif (TSS_C6_TYPE == TSS_CT_ROTARY)
    TSS_COMPILE_TIME_ERROR((TSS_C6_INPUTS_COUNT>2)&&(TSS_C6_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C6_INPUTS)
    TSS_CSRotary TSS_C6_STRUCTURE = { {6u, TSS_C6_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C6_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C6_STRUCTURE_ROM_CONTEXT = { TSS_C6_INPUTS_GROUP, TSS_C6_INPUTS_COUNT };
    #define C6_DECODER      TSS_SliderDecoder
  #elif (TSS_C6_TYPE == TSS_CT_MATRIX)
    TSS_COMPILE_TIME_ERROR((TSS_C6_INPUTS_COUNT>3)&&(TSS_C6_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C6_INPUTS)
    TSS_CSMatrix TSS_C6_STRUCTURE = { {6u, TSS_C6_TYPE}, TSS_MATRIX_CONTROL_INIT, TSS_MATRIX_EVENTS_INIT, 0u,0u, TSS_MATRIX_DYN_INIT, TSS_MATRIX_DYN_INIT, 0u,0u,0u,0u,0u,0u };
    TSS_MatrixTouchContext TSS_C6_MATRIX_TOUCH_CONTEXT = { 0u,0u,0u,0u };
    TSS_MatrixPosIIRFilterContext TSS_C6_MATRIX_POSFILTER_IIR_CONTEXT = { 0u,0u };
    TSS_GenericRAMContext TSS_C6_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_MatrixROMContext TSS_C6_STRUCTURE_ROM_CONTEXT = { {TSS_C6_INPUTS_GROUP, TSS_C6_INPUTS_COUNT}, TSS_C6_INPUTS_X_COUNT,
                                                               &TSS_C6_MATRIX_TOUCH_CONTEXT,
                                                               TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(6)
                                                             };
    #define C6_DECODER      TSS_MatrixDecoder
  #else /* TSS_CT_ASLIDER || TSS_CT_AROTARY */
    #if (TSS_C6_TYPE == TSS_CT_ASLIDER)
      TSS_COMPILE_TIME_ERROR((TSS_C6_INPUTS_COUNT>1)&&(TSS_C6_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C6_INPUTS)
      TSS_CSASlider TSS_C6_STRUCTURE = { {6u, TSS_C6_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #else
      TSS_COMPILE_TIME_ERROR((TSS_C6_INPUTS_COUNT>2)&&(TSS_C6_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C6_INPUTS)
      TSS_CSARotary TSS_C6_STRUCTURE = { {6u, TSS_C6_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #endif
    TSS_ASliderPosFilterContext TSS_C6_POSFILTER_CONTEXT;
    TSS_ASliderDelayContext TSS_C6_DELAY_CONTEXT;
    TSS_ASliderTrendContext TSS_C6_TREND_CONTEXT;
    uint8_t TSS_C6_DELAY_BUFFER[TSS_ASLIDER_DELAY_BUFFER_LENGTH];
    int8_t TSS_C6_TREND_BUFFER[TSS_ASLIDER_TREND_BUFFER_LENGTH];
    TSS_GenericRAMContext TSS_C6_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_ASliderROMContext TSS_C6_STRUCTURE_ROM_CONTEXT = { {TSS_C6_INPUTS_GROUP, TSS_C6_INPUTS_COUNT},
                                                                  TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(6),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(6),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(6),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(6),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(6)
                                                                };     /* Declares the context ASlider Structure */
    #define C6_DECODER      TSS_ASliderDecoder
  #endif
  void TSS_C6_CALLBACK(TSS_CONTROL_ID u8ControlId);
  uint8_t C6_DECODER(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

#if TSS_N_CONTROLS > 7
  #if (TSS_C7_TYPE == TSS_CT_MATRIX)
    const uint8_t TSS_C7_INPUTS_GROUP[] =  TSS_C7_INPUTS;
    #define TSS_C7_INPUTS_COUNT sizeof(TSS_C7_INPUTS_GROUP)/sizeof(TSS_C7_INPUTS_GROUP[0])
    #define TSS_C7_INPUTS_X_COUNT TSS_C7_INPUTS_NUM_X
  #else
    const uint8_t TSS_C7_INPUTS_GROUP[] = TSS_C7_INPUTS;
    #define TSS_C7_INPUTS_COUNT sizeof(TSS_C7_INPUTS_GROUP)/sizeof(TSS_C7_INPUTS_GROUP[0])
  #endif
  #if (TSS_C7_TYPE == TSS_CT_KEYPAD)
    TSS_COMPILE_TIME_ERROR((TSS_C7_INPUTS_COUNT>0)&&(TSS_C7_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C7_INPUTS)
    uint8_t TSS_C7_KEYPAD_EVENTS_BUFFER[TSS_KEYPAD_EVENTS_BUFFER_SIZE];
    #ifdef TSS_C7_KEYS
      const uint16_t TSS_C7_KEYS_GROUP[] = TSS_C7_KEYS;
      #define TSS_C7_KEYS_COUNT sizeof(TSS_C7_KEYS_GROUP)/sizeof(TSS_C7_KEYS_GROUP[0])
      TSS_COMPILE_TIME_ERROR((TSS_C7_KEYS_COUNT<65536)&&(TSS_C7_KEYS_COUNT>0),TSS_Invalid_quantity_of_TSS_C7_KEYS)
    #else
      #define TSS_C7_KEYS_GROUP NULL
      #define TSS_C7_KEYS_COUNT TSS_C7_INPUTS_COUNT
    #endif
    TSS_CSKeypad TSS_C7_STRUCTURE = { {7u, TSS_C7_TYPE}, TSS_KEYPAD_CONTCONF_INIT, 0u,0u, TSS_KEYPAD_EVENTS_INIT, 0u,0u,0u, TSS_C7_KEYPAD_EVENTS_BUFFER };
    TSS_KeypadRAMContext TSS_C7_STRUCTURE_RAM_CONTEXT = { {0u},0u,0u,0u }; 
    const TSS_KeypadROMContext TSS_C7_STRUCTURE_ROM_CONTEXT = { {TSS_C7_INPUTS_GROUP, TSS_C7_INPUTS_COUNT}, TSS_C7_KEYS_GROUP, TSS_C7_KEYS_COUNT };
    #define C7_DECODER      TSS_KeypadDecoder
  #elif (TSS_C7_TYPE == TSS_CT_SLIDER)
    TSS_COMPILE_TIME_ERROR((TSS_C7_INPUTS_COUNT>1)&&(TSS_C7_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C7_INPUTS)
    TSS_CSSlider TSS_C7_STRUCTURE = { {7u, TSS_C7_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C7_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C7_STRUCTURE_ROM_CONTEXT = { TSS_C7_INPUTS_GROUP, TSS_C7_INPUTS_COUNT };
    #define C7_DECODER      TSS_SliderDecoder
  #elif (TSS_C7_TYPE == TSS_CT_ROTARY)
    TSS_COMPILE_TIME_ERROR((TSS_C7_INPUTS_COUNT>2)&&(TSS_C7_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C7_INPUTS)
    TSS_CSRotary TSS_C7_STRUCTURE = { {7u, TSS_C7_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C7_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C7_STRUCTURE_ROM_CONTEXT = { TSS_C7_INPUTS_GROUP, TSS_C7_INPUTS_COUNT };
    #define C7_DECODER      TSS_SliderDecoder
  #elif (TSS_C7_TYPE == TSS_CT_MATRIX)
    TSS_COMPILE_TIME_ERROR((TSS_C7_INPUTS_COUNT>3)&&(TSS_C7_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C7_INPUTS)
    TSS_CSMatrix TSS_C7_STRUCTURE = { {7u, TSS_C7_TYPE}, TSS_MATRIX_CONTROL_INIT, TSS_MATRIX_EVENTS_INIT, 0u,0u, TSS_MATRIX_DYN_INIT, TSS_MATRIX_DYN_INIT, 0u,0u,0u,0u,0u,0u };
    TSS_MatrixTouchContext TSS_C7_MATRIX_TOUCH_CONTEXT = { 0u,0u,0u,0u };
    TSS_MatrixPosIIRFilterContext TSS_C7_MATRIX_POSFILTER_IIR_CONTEXT = { 0u,0u };
    TSS_GenericRAMContext TSS_C7_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_MatrixROMContext TSS_C7_STRUCTURE_ROM_CONTEXT = { {TSS_C7_INPUTS_GROUP, TSS_C7_INPUTS_COUNT}, TSS_C7_INPUTS_X_COUNT,
                                                               &TSS_C7_MATRIX_TOUCH_CONTEXT,
                                                               TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(7)
                                                             };
    #define C7_DECODER      TSS_MatrixDecoder
  #else /* TSS_CT_ASLIDER || TSS_CT_AROTARY */
    #if (TSS_C7_TYPE == TSS_CT_ASLIDER)
      TSS_COMPILE_TIME_ERROR((TSS_C7_INPUTS_COUNT>1)&&(TSS_C7_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C7_INPUTS)
      TSS_CSASlider TSS_C7_STRUCTURE = { {7u, TSS_C7_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #else
      TSS_COMPILE_TIME_ERROR((TSS_C7_INPUTS_COUNT>2)&&(TSS_C7_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C7_INPUTS)
      TSS_CSARotary TSS_C7_STRUCTURE = { {7u, TSS_C7_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #endif
    TSS_ASliderPosFilterContext TSS_C7_POSFILTER_CONTEXT;
    TSS_ASliderDelayContext TSS_C7_DELAY_CONTEXT;
    TSS_ASliderTrendContext TSS_C7_TREND_CONTEXT;
    uint8_t TSS_C7_DELAY_BUFFER[TSS_ASLIDER_DELAY_BUFFER_LENGTH];
    int8_t TSS_C7_TREND_BUFFER[TSS_ASLIDER_TREND_BUFFER_LENGTH];
    TSS_GenericRAMContext TSS_C7_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_ASliderROMContext TSS_C7_STRUCTURE_ROM_CONTEXT = { {TSS_C7_INPUTS_GROUP, TSS_C7_INPUTS_COUNT},
                                                                  TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(7),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(7),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(7),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(7),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(7)
                                                                };     /* Declares the context ASlider Structure */
    #define C7_DECODER      TSS_ASliderDecoder
  #endif
  void TSS_C7_CALLBACK(TSS_CONTROL_ID u8ControlId);
  uint8_t C7_DECODER(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

#if TSS_N_CONTROLS > 8
  #if (TSS_C8_TYPE == TSS_CT_MATRIX)
    const uint8_t TSS_C8_INPUTS_GROUP[] =  TSS_C8_INPUTS;
    #define TSS_C8_INPUTS_COUNT sizeof(TSS_C8_INPUTS_GROUP)/sizeof(TSS_C8_INPUTS_GROUP[0])
    #define TSS_C8_INPUTS_X_COUNT TSS_C8_INPUTS_NUM_X
  #else
    const uint8_t TSS_C8_INPUTS_GROUP[] = TSS_C8_INPUTS;
    #define TSS_C8_INPUTS_COUNT sizeof(TSS_C8_INPUTS_GROUP)/sizeof(TSS_C8_INPUTS_GROUP[0])
  #endif
  #if (TSS_C8_TYPE == TSS_CT_KEYPAD)
    TSS_COMPILE_TIME_ERROR((TSS_C8_INPUTS_COUNT>0)&&(TSS_C8_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C8_INPUTS)
    uint8_t TSS_C8_KEYPAD_EVENTS_BUFFER[TSS_KEYPAD_EVENTS_BUFFER_SIZE];
    #ifdef TSS_C8_KEYS
      const uint16_t TSS_C8_KEYS_GROUP[] = TSS_C8_KEYS;
      #define TSS_C8_KEYS_COUNT sizeof(TSS_C8_KEYS_GROUP)/sizeof(TSS_C8_KEYS_GROUP[0])
      TSS_COMPILE_TIME_ERROR((TSS_C8_KEYS_COUNT<65536)&&(TSS_C8_KEYS_COUNT>0),TSS_Invalid_quantity_of_TSS_C8_KEYS)
    #else
      #define TSS_C8_KEYS_GROUP NULL
      #define TSS_C8_KEYS_COUNT TSS_C8_INPUTS_COUNT
    #endif
    TSS_CSKeypad TSS_C8_STRUCTURE = { {8u, TSS_C8_TYPE}, TSS_KEYPAD_CONTCONF_INIT, 0u,0u, TSS_KEYPAD_EVENTS_INIT, 0u,0u,0u, TSS_C8_KEYPAD_EVENTS_BUFFER };
    TSS_KeypadRAMContext TSS_C8_STRUCTURE_RAM_CONTEXT = { {0u},0u,0u,0u }; 
    const TSS_KeypadROMContext TSS_C8_STRUCTURE_ROM_CONTEXT = { {TSS_C8_INPUTS_GROUP, TSS_C8_INPUTS_COUNT}, TSS_C8_KEYS_GROUP, TSS_C8_KEYS_COUNT };
    #define C8_DECODER      TSS_KeypadDecoder
  #elif (TSS_C8_TYPE == TSS_CT_SLIDER)
    TSS_COMPILE_TIME_ERROR((TSS_C8_INPUTS_COUNT>1)&&(TSS_C8_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C8_INPUTS)
    TSS_CSSlider TSS_C8_STRUCTURE = { {8u, TSS_C8_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C8_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C8_STRUCTURE_ROM_CONTEXT = { TSS_C8_INPUTS_GROUP, TSS_C8_INPUTS_COUNT };
    #define C8_DECODER      TSS_SliderDecoder
  #elif (TSS_C8_TYPE == TSS_CT_ROTARY)
    TSS_COMPILE_TIME_ERROR((TSS_C8_INPUTS_COUNT>2)&&(TSS_C8_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C8_INPUTS)
    TSS_CSRotary TSS_C8_STRUCTURE = { {8u, TSS_C8_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C8_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C8_STRUCTURE_ROM_CONTEXT = { TSS_C8_INPUTS_GROUP, TSS_C8_INPUTS_COUNT };
    #define C8_DECODER      TSS_SliderDecoder
  #elif (TSS_C8_TYPE == TSS_CT_MATRIX)
    TSS_COMPILE_TIME_ERROR((TSS_C8_INPUTS_COUNT>3)&&(TSS_C8_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C8_INPUTS)
    TSS_CSMatrix TSS_C8_STRUCTURE = { {8u, TSS_C8_TYPE}, TSS_MATRIX_CONTROL_INIT, TSS_MATRIX_EVENTS_INIT, 0u,0u, TSS_MATRIX_DYN_INIT, TSS_MATRIX_DYN_INIT, 0u,0u,0u,0u,0u,0u };
    TSS_MatrixTouchContext TSS_C8_MATRIX_TOUCH_CONTEXT = { 0u,0u,0u,0u };
    TSS_MatrixPosIIRFilterContext TSS_C8_MATRIX_POSFILTER_IIR_CONTEXT = { 0u,0u };
    TSS_GenericRAMContext TSS_C8_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_MatrixROMContext TSS_C8_STRUCTURE_ROM_CONTEXT = { {TSS_C8_INPUTS_GROUP, TSS_C8_INPUTS_COUNT}, TSS_C8_INPUTS_X_COUNT,
                                                               &TSS_C8_MATRIX_TOUCH_CONTEXT,
                                                               TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(8)
                                                             };
    #define C8_DECODER      TSS_MatrixDecoder
  #else /* TSS_CT_ASLIDER || TSS_CT_AROTARY */
    #if (TSS_C8_TYPE == TSS_CT_ASLIDER)
      TSS_COMPILE_TIME_ERROR((TSS_C8_INPUTS_COUNT>1)&&(TSS_C8_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C8_INPUTS)
      TSS_CSASlider TSS_C8_STRUCTURE = { {8u, TSS_C8_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #else
      TSS_COMPILE_TIME_ERROR((TSS_C8_INPUTS_COUNT>2)&&(TSS_C8_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C8_INPUTS)
      TSS_CSARotary TSS_C8_STRUCTURE = { {8u, TSS_C8_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #endif
    TSS_ASliderPosFilterContext TSS_C8_POSFILTER_CONTEXT;
    TSS_ASliderDelayContext TSS_C8_DELAY_CONTEXT;
    TSS_ASliderTrendContext TSS_C8_TREND_CONTEXT;
    uint8_t TSS_C8_DELAY_BUFFER[TSS_ASLIDER_DELAY_BUFFER_LENGTH];
    int8_t TSS_C8_TREND_BUFFER[TSS_ASLIDER_TREND_BUFFER_LENGTH];
    TSS_GenericRAMContext TSS_C8_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_ASliderROMContext TSS_C8_STRUCTURE_ROM_CONTEXT = { {TSS_C8_INPUTS_GROUP, TSS_C8_INPUTS_COUNT},
                                                                  TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(8),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(8),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(8),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(8),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(8)
                                                                };     /* Declares the context ASlider Structure */
    #define C8_DECODER      TSS_ASliderDecoder
  #endif
  void TSS_C8_CALLBACK(TSS_CONTROL_ID u8ControlId);
  uint8_t C8_DECODER(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

#if TSS_N_CONTROLS > 9
  #if (TSS_C9_TYPE == TSS_CT_MATRIX)
    const uint8_t TSS_C9_INPUTS_GROUP[] =  TSS_C9_INPUTS;
    #define TSS_C9_INPUTS_COUNT sizeof(TSS_C9_INPUTS_GROUP)/sizeof(TSS_C9_INPUTS_GROUP[0])
    #define TSS_C9_INPUTS_X_COUNT TSS_C9_INPUTS_NUM_X
  #else
    const uint8_t TSS_C9_INPUTS_GROUP[] = TSS_C9_INPUTS;
    #define TSS_C9_INPUTS_COUNT sizeof(TSS_C9_INPUTS_GROUP)/sizeof(TSS_C9_INPUTS_GROUP[0])
  #endif
  #if (TSS_C9_TYPE == TSS_CT_KEYPAD)
    TSS_COMPILE_TIME_ERROR((TSS_C9_INPUTS_COUNT>0)&&(TSS_C9_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C9_INPUTS)
    uint8_t TSS_C9_KEYPAD_EVENTS_BUFFER[TSS_KEYPAD_EVENTS_BUFFER_SIZE];
    #ifdef TSS_C9_KEYS
      const uint16_t TSS_C9_KEYS_GROUP[] = TSS_C9_KEYS;
      #define TSS_C9_KEYS_COUNT sizeof(TSS_C9_KEYS_GROUP)/sizeof(TSS_C9_KEYS_GROUP[0])
      TSS_COMPILE_TIME_ERROR((TSS_C9_KEYS_COUNT<65536)&&(TSS_C9_KEYS_COUNT>0),TSS_Invalid_quantity_of_TSS_C9_KEYS)
    #else
      #define TSS_C9_KEYS_GROUP NULL
      #define TSS_C9_KEYS_COUNT TSS_C9_INPUTS_COUNT
    #endif
    TSS_CSKeypad TSS_C9_STRUCTURE = { {9u, TSS_C9_TYPE}, TSS_KEYPAD_CONTCONF_INIT, 0u,0u, TSS_KEYPAD_EVENTS_INIT, 0u,0u,0u, TSS_C9_KEYPAD_EVENTS_BUFFER };
    TSS_KeypadRAMContext TSS_C9_STRUCTURE_RAM_CONTEXT = { {0u},0u,0u,0u }; 
    const TSS_KeypadROMContext TSS_C9_STRUCTURE_ROM_CONTEXT = { {TSS_C9_INPUTS_GROUP, TSS_C9_INPUTS_COUNT}, TSS_C9_KEYS_GROUP, TSS_C9_KEYS_COUNT };
    #define C9_DECODER      TSS_KeypadDecoder
  #elif (TSS_C9_TYPE == TSS_CT_SLIDER)
    TSS_COMPILE_TIME_ERROR((TSS_C9_INPUTS_COUNT>1)&&(TSS_C9_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C9_INPUTS)
    TSS_CSSlider TSS_C9_STRUCTURE = { {9u, TSS_C9_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C9_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C9_STRUCTURE_ROM_CONTEXT = { TSS_C9_INPUTS_GROUP, TSS_C9_INPUTS_COUNT };
    #define C9_DECODER      TSS_SliderDecoder
  #elif (TSS_C9_TYPE == TSS_CT_ROTARY)
    TSS_COMPILE_TIME_ERROR((TSS_C9_INPUTS_COUNT>2)&&(TSS_C9_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C9_INPUTS)
    TSS_CSRotary TSS_C9_STRUCTURE = { {9u, TSS_C9_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C9_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C9_STRUCTURE_ROM_CONTEXT = { TSS_C9_INPUTS_GROUP, TSS_C9_INPUTS_COUNT };
    #define C9_DECODER      TSS_SliderDecoder
  #elif (TSS_C9_TYPE == TSS_CT_MATRIX)
    TSS_COMPILE_TIME_ERROR((TSS_C9_INPUTS_COUNT>3)&&(TSS_C9_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C9_INPUTS)
    TSS_CSMatrix TSS_C9_STRUCTURE = { {9u, TSS_C9_TYPE}, TSS_MATRIX_CONTROL_INIT, TSS_MATRIX_EVENTS_INIT, 0u,0u, TSS_MATRIX_DYN_INIT, TSS_MATRIX_DYN_INIT, 0u,0u,0u,0u,0u,0u };
    TSS_MatrixTouchContext TSS_C9_MATRIX_TOUCH_CONTEXT = { 0u,0u,0u,0u };
    TSS_MatrixPosIIRFilterContext TSS_C9_MATRIX_POSFILTER_IIR_CONTEXT = { 0u,0u };
    TSS_GenericRAMContext TSS_C9_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_MatrixROMContext TSS_C9_STRUCTURE_ROM_CONTEXT = { {TSS_C9_INPUTS_GROUP, TSS_C9_INPUTS_COUNT}, TSS_C9_INPUTS_X_COUNT,
                                                               &TSS_C9_MATRIX_TOUCH_CONTEXT,
                                                               TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(9)
                                                             };
    #define C9_DECODER      TSS_MatrixDecoder
  #else /* TSS_CT_ASLIDER || TSS_CT_AROTARY */
    #if (TSS_C9_TYPE == TSS_CT_ASLIDER)
      TSS_COMPILE_TIME_ERROR((TSS_C9_INPUTS_COUNT>1)&&(TSS_C9_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C9_INPUTS)
      TSS_CSASlider TSS_C9_STRUCTURE = { {9u, TSS_C9_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #else
      TSS_COMPILE_TIME_ERROR((TSS_C9_INPUTS_COUNT>2)&&(TSS_C9_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C9_INPUTS)
      TSS_CSARotary TSS_C9_STRUCTURE = { {9u, TSS_C9_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #endif
    TSS_ASliderPosFilterContext TSS_C9_POSFILTER_CONTEXT;
    TSS_ASliderDelayContext TSS_C9_DELAY_CONTEXT;
    TSS_ASliderTrendContext TSS_C9_TREND_CONTEXT;
    uint8_t TSS_C9_DELAY_BUFFER[TSS_ASLIDER_DELAY_BUFFER_LENGTH];
    int8_t TSS_C9_TREND_BUFFER[TSS_ASLIDER_TREND_BUFFER_LENGTH];
    TSS_GenericRAMContext TSS_C9_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_ASliderROMContext TSS_C9_STRUCTURE_ROM_CONTEXT = { {TSS_C9_INPUTS_GROUP, TSS_C9_INPUTS_COUNT},
                                                                  TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(9),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(9),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(9),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(9),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(9)
                                                                };     /* Declares the context ASlider Structure */
    #define C9_DECODER      TSS_ASliderDecoder
  #endif
  void TSS_C9_CALLBACK(TSS_CONTROL_ID u8ControlId);
  uint8_t C9_DECODER(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

#if TSS_N_CONTROLS > 10
  #if (TSS_C10_TYPE == TSS_CT_MATRIX)
    const uint8_t TSS_C10_INPUTS_GROUP[] =  TSS_C10_INPUTS;
    #define TSS_C10_INPUTS_COUNT sizeof(TSS_C10_INPUTS_GROUP)/sizeof(TSS_C10_INPUTS_GROUP[0])
    #define TSS_C10_INPUTS_X_COUNT TSS_C10_INPUTS_NUM_X
  #else
    const uint8_t TSS_C10_INPUTS_GROUP[] = TSS_C10_INPUTS;
    #define TSS_C10_INPUTS_COUNT sizeof(TSS_C10_INPUTS_GROUP)/sizeof(TSS_C10_INPUTS_GROUP[0])
  #endif
  #if (TSS_C10_TYPE == TSS_CT_KEYPAD)
    TSS_COMPILE_TIME_ERROR((TSS_C10_INPUTS_COUNT>0)&&(TSS_C10_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C10_INPUTS)
    uint8_t TSS_C10_KEYPAD_EVENTS_BUFFER[TSS_KEYPAD_EVENTS_BUFFER_SIZE];
    #ifdef TSS_C10_KEYS
      const uint16_t TSS_C10_KEYS_GROUP[] = TSS_C10_KEYS;
      #define TSS_C10_KEYS_COUNT sizeof(TSS_C10_KEYS_GROUP)/sizeof(TSS_C10_KEYS_GROUP[0])
      TSS_COMPILE_TIME_ERROR((TSS_C10_KEYS_COUNT<65536)&&(TSS_C10_KEYS_COUNT>0),TSS_Invalid_quantity_of_TSS_C10_KEYS)
    #else
      #define TSS_C10_KEYS_GROUP NULL
      #define TSS_C10_KEYS_COUNT TSS_C10_INPUTS_COUNT
    #endif
    TSS_CSKeypad TSS_C10_STRUCTURE = { {10u, TSS_C10_TYPE}, TSS_KEYPAD_CONTCONF_INIT, 0u,0u, TSS_KEYPAD_EVENTS_INIT, 0u,0u,0u, TSS_C10_KEYPAD_EVENTS_BUFFER };
    TSS_KeypadRAMContext TSS_C10_STRUCTURE_RAM_CONTEXT = { {0u},0u,0u,0u }; 
    const TSS_KeypadROMContext TSS_C10_STRUCTURE_ROM_CONTEXT = { {TSS_C10_INPUTS_GROUP, TSS_C10_INPUTS_COUNT}, TSS_C10_KEYS_GROUP, TSS_C10_KEYS_COUNT };
    #define C10_DECODER      TSS_KeypadDecoder
  #elif (TSS_C10_TYPE == TSS_CT_SLIDER)
    TSS_COMPILE_TIME_ERROR((TSS_C10_INPUTS_COUNT>1)&&(TSS_C10_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C10_INPUTS)
    TSS_CSSlider TSS_C10_STRUCTURE = { {10u, TSS_C10_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C10_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C10_STRUCTURE_ROM_CONTEXT = { TSS_C10_INPUTS_GROUP, TSS_C10_INPUTS_COUNT };
    #define C10_DECODER      TSS_SliderDecoder
  #elif (TSS_C10_TYPE == TSS_CT_ROTARY)
    TSS_COMPILE_TIME_ERROR((TSS_C10_INPUTS_COUNT>2)&&(TSS_C10_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C10_INPUTS)
    TSS_CSRotary TSS_C10_STRUCTURE = { {10u, TSS_C10_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C10_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C10_STRUCTURE_ROM_CONTEXT = { TSS_C10_INPUTS_GROUP, TSS_C10_INPUTS_COUNT };
    #define C10_DECODER      TSS_SliderDecoder
  #elif (TSS_C10_TYPE == TSS_CT_MATRIX)
    TSS_COMPILE_TIME_ERROR((TSS_C10_INPUTS_COUNT>3)&&(TSS_C10_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C10_INPUTS)
    TSS_CSMatrix TSS_C10_STRUCTURE = { {10u, TSS_C10_TYPE}, TSS_MATRIX_CONTROL_INIT, TSS_MATRIX_EVENTS_INIT, 0u,0u, TSS_MATRIX_DYN_INIT, TSS_MATRIX_DYN_INIT, 0u,0u,0u,0u,0u,0u };
    TSS_MatrixTouchContext TSS_C10_MATRIX_TOUCH_CONTEXT = { 0u,0u,0u,0u };
    TSS_MatrixPosIIRFilterContext TSS_C10_MATRIX_POSFILTER_IIR_CONTEXT = { 0u,0u };
    TSS_GenericRAMContext TSS_C10_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_MatrixROMContext TSS_C10_STRUCTURE_ROM_CONTEXT = { {TSS_C10_INPUTS_GROUP, TSS_C10_INPUTS_COUNT}, TSS_C10_INPUTS_X_COUNT,
                                                               &TSS_C10_MATRIX_TOUCH_CONTEXT,
                                                               TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(10)
                                                             };
    #define C10_DECODER      TSS_MatrixDecoder
  #else /* TSS_CT_ASLIDER || TSS_CT_AROTARY */
    #if (TSS_C10_TYPE == TSS_CT_ASLIDER)
      TSS_COMPILE_TIME_ERROR((TSS_C10_INPUTS_COUNT>1)&&(TSS_C10_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C10_INPUTS)
      TSS_CSASlider TSS_C10_STRUCTURE = { {10u, TSS_C10_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #else
      TSS_COMPILE_TIME_ERROR((TSS_C10_INPUTS_COUNT>2)&&(TSS_C10_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C10_INPUTS)
      TSS_CSARotary TSS_C10_STRUCTURE = { {10u, TSS_C10_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #endif
    TSS_ASliderPosFilterContext TSS_C10_POSFILTER_CONTEXT;
    TSS_ASliderDelayContext TSS_C10_DELAY_CONTEXT;
    TSS_ASliderTrendContext TSS_C10_TREND_CONTEXT;
    uint8_t TSS_C10_DELAY_BUFFER[TSS_ASLIDER_DELAY_BUFFER_LENGTH];
    int8_t TSS_C10_TREND_BUFFER[TSS_ASLIDER_TREND_BUFFER_LENGTH];
    TSS_GenericRAMContext TSS_C10_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_ASliderROMContext TSS_C10_STRUCTURE_ROM_CONTEXT = { {TSS_C10_INPUTS_GROUP, TSS_C10_INPUTS_COUNT},
                                                                  TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(10),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(10),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(10),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(10),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(10)
                                                                };     /* Declares the context ASlider Structure */
    #define C10_DECODER      TSS_ASliderDecoder
  #endif
  void TSS_C10_CALLBACK(TSS_CONTROL_ID u8ControlId);
  uint8_t C10_DECODER(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

#if TSS_N_CONTROLS > 11
  #if (TSS_C11_TYPE == TSS_CT_MATRIX)
    const uint8_t TSS_C11_INPUTS_GROUP[] =  TSS_C11_INPUTS;
    #define TSS_C11_INPUTS_COUNT sizeof(TSS_C11_INPUTS_GROUP)/sizeof(TSS_C11_INPUTS_GROUP[0])
    #define TSS_C11_INPUTS_X_COUNT TSS_C11_INPUTS_NUM_X
  #else
    const uint8_t TSS_C11_INPUTS_GROUP[] = TSS_C11_INPUTS;
    #define TSS_C11_INPUTS_COUNT sizeof(TSS_C11_INPUTS_GROUP)/sizeof(TSS_C11_INPUTS_GROUP[0])
  #endif
  #if (TSS_C11_TYPE == TSS_CT_KEYPAD)
    TSS_COMPILE_TIME_ERROR((TSS_C11_INPUTS_COUNT>0)&&(TSS_C11_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C11_INPUTS)
    uint8_t TSS_C11_KEYPAD_EVENTS_BUFFER[TSS_KEYPAD_EVENTS_BUFFER_SIZE];
    #ifdef TSS_C11_KEYS
      const uint16_t TSS_C11_KEYS_GROUP[] = TSS_C11_KEYS;
      #define TSS_C11_KEYS_COUNT sizeof(TSS_C11_KEYS_GROUP)/sizeof(TSS_C11_KEYS_GROUP[0])
      TSS_COMPILE_TIME_ERROR((TSS_C11_KEYS_COUNT<65536)&&(TSS_C11_KEYS_COUNT>0),TSS_Invalid_quantity_of_TSS_C11_KEYS)
    #else
      #define TSS_C11_KEYS_GROUP NULL
      #define TSS_C11_KEYS_COUNT TSS_C11_INPUTS_COUNT
    #endif
    TSS_CSKeypad TSS_C11_STRUCTURE = { {11u, TSS_C11_TYPE}, TSS_KEYPAD_CONTCONF_INIT, 0u,0u, TSS_KEYPAD_EVENTS_INIT, 0u,0u,0u, TSS_C11_KEYPAD_EVENTS_BUFFER };
    TSS_KeypadRAMContext TSS_C11_STRUCTURE_RAM_CONTEXT = { {0u},0u,0u,0u }; 
    const TSS_KeypadROMContext TSS_C11_STRUCTURE_ROM_CONTEXT = { {TSS_C11_INPUTS_GROUP, TSS_C11_INPUTS_COUNT}, TSS_C11_KEYS_GROUP, TSS_C11_KEYS_COUNT };
    #define C11_DECODER      TSS_KeypadDecoder
  #elif (TSS_C11_TYPE == TSS_CT_SLIDER)
    TSS_COMPILE_TIME_ERROR((TSS_C11_INPUTS_COUNT>1)&&(TSS_C11_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C11_INPUTS)
    TSS_CSSlider TSS_C11_STRUCTURE = { {11u, TSS_C11_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C11_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C11_STRUCTURE_ROM_CONTEXT = { TSS_C11_INPUTS_GROUP, TSS_C11_INPUTS_COUNT };
    #define C11_DECODER      TSS_SliderDecoder
  #elif (TSS_C11_TYPE == TSS_CT_ROTARY)
    TSS_COMPILE_TIME_ERROR((TSS_C11_INPUTS_COUNT>2)&&(TSS_C11_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C11_INPUTS)
    TSS_CSRotary TSS_C11_STRUCTURE = { {11u, TSS_C11_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C11_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C11_STRUCTURE_ROM_CONTEXT = { TSS_C11_INPUTS_GROUP, TSS_C11_INPUTS_COUNT };
    #define C11_DECODER      TSS_SliderDecoder
  #elif (TSS_C11_TYPE == TSS_CT_MATRIX)
    TSS_COMPILE_TIME_ERROR((TSS_C11_INPUTS_COUNT>3)&&(TSS_C11_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C11_INPUTS)
    TSS_CSMatrix TSS_C11_STRUCTURE = { {11u, TSS_C11_TYPE}, TSS_MATRIX_CONTROL_INIT, TSS_MATRIX_EVENTS_INIT, 0u,0u, TSS_MATRIX_DYN_INIT, TSS_MATRIX_DYN_INIT, 0u,0u,0u,0u,0u,0u };
    TSS_MatrixTouchContext TSS_C11_MATRIX_TOUCH_CONTEXT = { 0u,0u,0u,0u };
    TSS_MatrixPosIIRFilterContext TSS_C11_MATRIX_POSFILTER_IIR_CONTEXT = { 0u,0u };
    TSS_GenericRAMContext TSS_C11_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_MatrixROMContext TSS_C11_STRUCTURE_ROM_CONTEXT = { {TSS_C11_INPUTS_GROUP, TSS_C11_INPUTS_COUNT}, TSS_C11_INPUTS_X_COUNT,
                                                               &TSS_C11_MATRIX_TOUCH_CONTEXT,
                                                               TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(11)
                                                             };
    #define C11_DECODER      TSS_MatrixDecoder
  #else /* TSS_CT_ASLIDER || TSS_CT_AROTARY */
    #if (TSS_C11_TYPE == TSS_CT_ASLIDER)
      TSS_COMPILE_TIME_ERROR((TSS_C11_INPUTS_COUNT>1)&&(TSS_C11_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C11_INPUTS)
      TSS_CSASlider TSS_C11_STRUCTURE = { {11u, TSS_C11_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #else
      TSS_COMPILE_TIME_ERROR((TSS_C11_INPUTS_COUNT>2)&&(TSS_C11_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C11_INPUTS)
      TSS_CSARotary TSS_C11_STRUCTURE = { {11u, TSS_C11_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #endif
    TSS_ASliderPosFilterContext TSS_C11_POSFILTER_CONTEXT;
    TSS_ASliderDelayContext TSS_C11_DELAY_CONTEXT;
    TSS_ASliderTrendContext TSS_C11_TREND_CONTEXT;
    uint8_t TSS_C11_DELAY_BUFFER[TSS_ASLIDER_DELAY_BUFFER_LENGTH];
    int8_t TSS_C11_TREND_BUFFER[TSS_ASLIDER_TREND_BUFFER_LENGTH];
    TSS_GenericRAMContext TSS_C11_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_ASliderROMContext TSS_C11_STRUCTURE_ROM_CONTEXT = { {TSS_C11_INPUTS_GROUP, TSS_C11_INPUTS_COUNT},
                                                                  TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(11),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(11),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(11),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(11),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(11)
                                                                };     /* Declares the context ASlider Structure */
    #define C11_DECODER      TSS_ASliderDecoder
  #endif
  void TSS_C11_CALLBACK(TSS_CONTROL_ID u8ControlId);
  uint8_t C11_DECODER(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

#if TSS_N_CONTROLS > 12
  #if (TSS_C12_TYPE == TSS_CT_MATRIX)
    const uint8_t TSS_C12_INPUTS_GROUP[] =  TSS_C12_INPUTS;
    #define TSS_C12_INPUTS_COUNT sizeof(TSS_C12_INPUTS_GROUP)/sizeof(TSS_C12_INPUTS_GROUP[0])
    #define TSS_C12_INPUTS_X_COUNT TSS_C12_INPUTS_NUM_X
  #else
    const uint8_t TSS_C12_INPUTS_GROUP[] = TSS_C12_INPUTS;
    #define TSS_C12_INPUTS_COUNT sizeof(TSS_C12_INPUTS_GROUP)/sizeof(TSS_C12_INPUTS_GROUP[0])
  #endif
  #if (TSS_C12_TYPE == TSS_CT_KEYPAD)
    TSS_COMPILE_TIME_ERROR((TSS_C12_INPUTS_COUNT>0)&&(TSS_C12_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C12_INPUTS)
    uint8_t TSS_C12_KEYPAD_EVENTS_BUFFER[TSS_KEYPAD_EVENTS_BUFFER_SIZE];
    #ifdef TSS_C12_KEYS
      const uint16_t TSS_C12_KEYS_GROUP[] = TSS_C12_KEYS;
      #define TSS_C12_KEYS_COUNT sizeof(TSS_C12_KEYS_GROUP)/sizeof(TSS_C12_KEYS_GROUP[0])
      TSS_COMPILE_TIME_ERROR((TSS_C12_KEYS_COUNT<65536)&&(TSS_C12_KEYS_COUNT>0),TSS_Invalid_quantity_of_TSS_C12_KEYS)
    #else
      #define TSS_C12_KEYS_GROUP NULL
      #define TSS_C12_KEYS_COUNT TSS_C12_INPUTS_COUNT
    #endif
    TSS_CSKeypad TSS_C12_STRUCTURE = { {12u, TSS_C12_TYPE}, TSS_KEYPAD_CONTCONF_INIT, 0u,0u, TSS_KEYPAD_EVENTS_INIT, 0u,0u,0u, TSS_C12_KEYPAD_EVENTS_BUFFER };
    TSS_KeypadRAMContext TSS_C12_STRUCTURE_RAM_CONTEXT = { {0u},0u,0u,0u }; 
    const TSS_KeypadROMContext TSS_C12_STRUCTURE_ROM_CONTEXT = { {TSS_C12_INPUTS_GROUP, TSS_C12_INPUTS_COUNT}, TSS_C12_KEYS_GROUP, TSS_C12_KEYS_COUNT };
    #define C12_DECODER      TSS_KeypadDecoder
  #elif (TSS_C12_TYPE == TSS_CT_SLIDER)
    TSS_COMPILE_TIME_ERROR((TSS_C12_INPUTS_COUNT>1)&&(TSS_C12_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C12_INPUTS)
    TSS_CSSlider TSS_C12_STRUCTURE = { {12u, TSS_C12_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C12_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C12_STRUCTURE_ROM_CONTEXT = { TSS_C12_INPUTS_GROUP, TSS_C12_INPUTS_COUNT };
    #define C12_DECODER      TSS_SliderDecoder
  #elif (TSS_C12_TYPE == TSS_CT_ROTARY)
    TSS_COMPILE_TIME_ERROR((TSS_C12_INPUTS_COUNT>2)&&(TSS_C12_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C12_INPUTS)
    TSS_CSRotary TSS_C12_STRUCTURE = { {12u, TSS_C12_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C12_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C12_STRUCTURE_ROM_CONTEXT = { TSS_C12_INPUTS_GROUP, TSS_C12_INPUTS_COUNT };
    #define C12_DECODER      TSS_SliderDecoder
  #elif (TSS_C12_TYPE == TSS_CT_MATRIX)
    TSS_COMPILE_TIME_ERROR((TSS_C12_INPUTS_COUNT>3)&&(TSS_C12_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C12_INPUTS)
    TSS_CSMatrix TSS_C12_STRUCTURE = { {12u, TSS_C12_TYPE}, TSS_MATRIX_CONTROL_INIT, TSS_MATRIX_EVENTS_INIT, 0u,0u, TSS_MATRIX_DYN_INIT, TSS_MATRIX_DYN_INIT, 0u,0u,0u,0u,0u,0u };
    TSS_MatrixTouchContext TSS_C12_MATRIX_TOUCH_CONTEXT = { 0u,0u,0u,0u };
    TSS_MatrixPosIIRFilterContext TSS_C12_MATRIX_POSFILTER_IIR_CONTEXT = { 0u,0u };
    TSS_GenericRAMContext TSS_C12_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_MatrixROMContext TSS_C12_STRUCTURE_ROM_CONTEXT = { {TSS_C12_INPUTS_GROUP, TSS_C12_INPUTS_COUNT}, TSS_C12_INPUTS_X_COUNT,
                                                               &TSS_C12_MATRIX_TOUCH_CONTEXT,
                                                               TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(12)
                                                             };
    #define C12_DECODER      TSS_MatrixDecoder
  #else /* TSS_CT_ASLIDER || TSS_CT_AROTARY */
    #if (TSS_C12_TYPE == TSS_CT_ASLIDER)
      TSS_COMPILE_TIME_ERROR((TSS_C12_INPUTS_COUNT>1)&&(TSS_C12_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C12_INPUTS)
      TSS_CSASlider TSS_C12_STRUCTURE = { {12u, TSS_C12_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #else
      TSS_COMPILE_TIME_ERROR((TSS_C12_INPUTS_COUNT>2)&&(TSS_C12_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C12_INPUTS)
      TSS_CSARotary TSS_C12_STRUCTURE = { {12u, TSS_C12_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #endif
    TSS_ASliderPosFilterContext TSS_C12_POSFILTER_CONTEXT;
    TSS_ASliderDelayContext TSS_C12_DELAY_CONTEXT;
    TSS_ASliderTrendContext TSS_C12_TREND_CONTEXT;
    uint8_t TSS_C12_DELAY_BUFFER[TSS_ASLIDER_DELAY_BUFFER_LENGTH];
    int8_t TSS_C12_TREND_BUFFER[TSS_ASLIDER_TREND_BUFFER_LENGTH];
    TSS_GenericRAMContext TSS_C12_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_ASliderROMContext TSS_C12_STRUCTURE_ROM_CONTEXT = { {TSS_C12_INPUTS_GROUP, TSS_C12_INPUTS_COUNT},
                                                                  TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(12),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(12),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(12),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(12),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(12)
                                                                };     /* Declares the context ASlider Structure */
    #define C12_DECODER      TSS_ASliderDecoder
  #endif
  void TSS_C12_CALLBACK(TSS_CONTROL_ID u8ControlId);
  uint8_t C12_DECODER(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

#if TSS_N_CONTROLS > 13
  #if (TSS_C13_TYPE == TSS_CT_MATRIX)
    const uint8_t TSS_C13_INPUTS_GROUP[] =  TSS_C13_INPUTS;
    #define TSS_C13_INPUTS_COUNT sizeof(TSS_C13_INPUTS_GROUP)/sizeof(TSS_C13_INPUTS_GROUP[0])
    #define TSS_C13_INPUTS_X_COUNT TSS_C13_INPUTS_NUM_X
  #else
    const uint8_t TSS_C13_INPUTS_GROUP[] = TSS_C13_INPUTS;
    #define TSS_C13_INPUTS_COUNT sizeof(TSS_C13_INPUTS_GROUP)/sizeof(TSS_C13_INPUTS_GROUP[0])
  #endif
  #if (TSS_C13_TYPE == TSS_CT_KEYPAD)
    TSS_COMPILE_TIME_ERROR((TSS_C13_INPUTS_COUNT>0)&&(TSS_C13_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C13_INPUTS)
    uint8_t TSS_C13_KEYPAD_EVENTS_BUFFER[TSS_KEYPAD_EVENTS_BUFFER_SIZE];
    #ifdef TSS_C13_KEYS
      const uint16_t TSS_C13_KEYS_GROUP[] = TSS_C13_KEYS;
      #define TSS_C13_KEYS_COUNT sizeof(TSS_C13_KEYS_GROUP)/sizeof(TSS_C13_KEYS_GROUP[0])
      TSS_COMPILE_TIME_ERROR((TSS_C13_KEYS_COUNT<65536)&&(TSS_C13_KEYS_COUNT>0),TSS_Invalid_quantity_of_TSS_C13_KEYS)
    #else
      #define TSS_C13_KEYS_GROUP NULL
      #define TSS_C13_KEYS_COUNT TSS_C13_INPUTS_COUNT
    #endif
    TSS_CSKeypad TSS_C13_STRUCTURE = { {13u, TSS_C13_TYPE}, TSS_KEYPAD_CONTCONF_INIT, 0u,0u, TSS_KEYPAD_EVENTS_INIT, 0u,0u,0u, TSS_C13_KEYPAD_EVENTS_BUFFER };
    TSS_KeypadRAMContext TSS_C13_STRUCTURE_RAM_CONTEXT = { {0u},0u,0u,0u }; 
    const TSS_KeypadROMContext TSS_C13_STRUCTURE_ROM_CONTEXT = { {TSS_C13_INPUTS_GROUP, TSS_C13_INPUTS_COUNT}, TSS_C13_KEYS_GROUP, TSS_C13_KEYS_COUNT };
    #define C13_DECODER      TSS_KeypadDecoder
  #elif (TSS_C13_TYPE == TSS_CT_SLIDER)
    TSS_COMPILE_TIME_ERROR((TSS_C13_INPUTS_COUNT>1)&&(TSS_C13_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C13_INPUTS)
    TSS_CSSlider TSS_C13_STRUCTURE = { {13u, TSS_C13_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C13_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C13_STRUCTURE_ROM_CONTEXT = { TSS_C13_INPUTS_GROUP, TSS_C13_INPUTS_COUNT };
    #define C13_DECODER      TSS_SliderDecoder
  #elif (TSS_C13_TYPE == TSS_CT_ROTARY)
    TSS_COMPILE_TIME_ERROR((TSS_C13_INPUTS_COUNT>2)&&(TSS_C13_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C13_INPUTS)
    TSS_CSRotary TSS_C13_STRUCTURE = { {13u, TSS_C13_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C13_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C13_STRUCTURE_ROM_CONTEXT = { TSS_C13_INPUTS_GROUP, TSS_C13_INPUTS_COUNT };
    #define C13_DECODER      TSS_SliderDecoder
  #elif (TSS_C13_TYPE == TSS_CT_MATRIX)
    TSS_COMPILE_TIME_ERROR((TSS_C13_INPUTS_COUNT>3)&&(TSS_C13_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C13_INPUTS)
    TSS_CSMatrix TSS_C13_STRUCTURE = { {13u, TSS_C13_TYPE}, TSS_MATRIX_CONTROL_INIT, TSS_MATRIX_EVENTS_INIT, 0u,0u, TSS_MATRIX_DYN_INIT, TSS_MATRIX_DYN_INIT, 0u,0u,0u,0u,0u,0u };
    TSS_MatrixTouchContext TSS_C13_MATRIX_TOUCH_CONTEXT = { 0u,0u,0u,0u };
    TSS_MatrixPosIIRFilterContext TSS_C13_MATRIX_POSFILTER_IIR_CONTEXT = { 0u,0u };
    TSS_GenericRAMContext TSS_C13_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_MatrixROMContext TSS_C13_STRUCTURE_ROM_CONTEXT = { {TSS_C13_INPUTS_GROUP, TSS_C13_INPUTS_COUNT}, TSS_C13_INPUTS_X_COUNT,
                                                               &TSS_C13_MATRIX_TOUCH_CONTEXT,
                                                               TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(13)
                                                             };
    #define C13_DECODER      TSS_MatrixDecoder
  #else /* TSS_CT_ASLIDER || TSS_CT_AROTARY */
    #if (TSS_C13_TYPE == TSS_CT_ASLIDER)
      TSS_COMPILE_TIME_ERROR((TSS_C13_INPUTS_COUNT>1)&&(TSS_C13_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C13_INPUTS)
      TSS_CSASlider TSS_C13_STRUCTURE = { {13u, TSS_C13_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #else
      TSS_COMPILE_TIME_ERROR((TSS_C13_INPUTS_COUNT>2)&&(TSS_C13_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C13_INPUTS)
      TSS_CSARotary TSS_C13_STRUCTURE = { {13u, TSS_C13_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #endif
    TSS_ASliderPosFilterContext TSS_C13_POSFILTER_CONTEXT;
    TSS_ASliderDelayContext TSS_C13_DELAY_CONTEXT;
    TSS_ASliderTrendContext TSS_C13_TREND_CONTEXT;
    uint8_t TSS_C13_DELAY_BUFFER[TSS_ASLIDER_DELAY_BUFFER_LENGTH];
    int8_t TSS_C13_TREND_BUFFER[TSS_ASLIDER_TREND_BUFFER_LENGTH];
    TSS_GenericRAMContext TSS_C13_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_ASliderROMContext TSS_C13_STRUCTURE_ROM_CONTEXT = { {TSS_C13_INPUTS_GROUP, TSS_C13_INPUTS_COUNT},
                                                                  TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(13),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(13),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(13),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(13),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(13)
                                                                };     /* Declares the context ASlider Structure */
    #define C13_DECODER      TSS_ASliderDecoder
  #endif
  void TSS_C13_CALLBACK(TSS_CONTROL_ID u8ControlId);
  uint8_t C13_DECODER(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

#if TSS_N_CONTROLS > 14
  #if (TSS_C14_TYPE == TSS_CT_MATRIX)
    const uint8_t TSS_C14_INPUTS_GROUP[] =  TSS_C14_INPUTS;
    #define TSS_C14_INPUTS_COUNT sizeof(TSS_C14_INPUTS_GROUP)/sizeof(TSS_C14_INPUTS_GROUP[0])
    #define TSS_C14_INPUTS_X_COUNT TSS_C14_INPUTS_NUM_X
  #else
    const uint8_t TSS_C14_INPUTS_GROUP[] = TSS_C14_INPUTS;
    #define TSS_C14_INPUTS_COUNT sizeof(TSS_C14_INPUTS_GROUP)/sizeof(TSS_C14_INPUTS_GROUP[0])
  #endif
  #if (TSS_C14_TYPE == TSS_CT_KEYPAD)
    TSS_COMPILE_TIME_ERROR((TSS_C14_INPUTS_COUNT>0)&&(TSS_C14_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C14_INPUTS)
    uint8_t TSS_C14_KEYPAD_EVENTS_BUFFER[TSS_KEYPAD_EVENTS_BUFFER_SIZE];
    #ifdef TSS_C14_KEYS
      const uint16_t TSS_C14_KEYS_GROUP[] = TSS_C14_KEYS;
      #define TSS_C14_KEYS_COUNT sizeof(TSS_C14_KEYS_GROUP)/sizeof(TSS_C14_KEYS_GROUP[0])
      TSS_COMPILE_TIME_ERROR((TSS_C14_KEYS_COUNT<65536)&&(TSS_C14_KEYS_COUNT>0),TSS_Invalid_quantity_of_TSS_C14_KEYS)
    #else
      #define TSS_C14_KEYS_GROUP NULL
      #define TSS_C14_KEYS_COUNT TSS_C14_INPUTS_COUNT
    #endif
    TSS_CSKeypad TSS_C14_STRUCTURE = { {14u, TSS_C14_TYPE}, TSS_KEYPAD_CONTCONF_INIT, 0u,0u, TSS_KEYPAD_EVENTS_INIT, 0u,0u,0u, TSS_C14_KEYPAD_EVENTS_BUFFER };
    TSS_KeypadRAMContext TSS_C14_STRUCTURE_RAM_CONTEXT = { {0u},0u,0u,0u }; 
    const TSS_KeypadROMContext TSS_C14_STRUCTURE_ROM_CONTEXT = { {TSS_C14_INPUTS_GROUP, TSS_C14_INPUTS_COUNT}, TSS_C14_KEYS_GROUP, TSS_C14_KEYS_COUNT };
    #define C14_DECODER      TSS_KeypadDecoder
  #elif (TSS_C14_TYPE == TSS_CT_SLIDER)
    TSS_COMPILE_TIME_ERROR((TSS_C14_INPUTS_COUNT>1)&&(TSS_C14_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C14_INPUTS)
    TSS_CSSlider TSS_C14_STRUCTURE = { {14u, TSS_C14_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C14_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C14_STRUCTURE_ROM_CONTEXT = { TSS_C14_INPUTS_GROUP, TSS_C14_INPUTS_COUNT };
    #define C14_DECODER      TSS_SliderDecoder
  #elif (TSS_C14_TYPE == TSS_CT_ROTARY)
    TSS_COMPILE_TIME_ERROR((TSS_C14_INPUTS_COUNT>2)&&(TSS_C14_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C14_INPUTS)
    TSS_CSRotary TSS_C14_STRUCTURE = { {14u, TSS_C14_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C14_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C14_STRUCTURE_ROM_CONTEXT = { TSS_C14_INPUTS_GROUP, TSS_C14_INPUTS_COUNT };
    #define C14_DECODER      TSS_SliderDecoder
  #elif (TSS_C14_TYPE == TSS_CT_MATRIX)
    TSS_COMPILE_TIME_ERROR((TSS_C14_INPUTS_COUNT>3)&&(TSS_C14_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C14_INPUTS)
    TSS_CSMatrix TSS_C14_STRUCTURE = { {14u, TSS_C14_TYPE}, TSS_MATRIX_CONTROL_INIT, TSS_MATRIX_EVENTS_INIT, 0u,0u, TSS_MATRIX_DYN_INIT, TSS_MATRIX_DYN_INIT, 0u,0u,0u,0u,0u,0u };
    TSS_MatrixTouchContext TSS_C14_MATRIX_TOUCH_CONTEXT = { 0u,0u,0u,0u };
    TSS_MatrixPosIIRFilterContext TSS_C14_MATRIX_POSFILTER_IIR_CONTEXT = { 0u,0u };
    TSS_GenericRAMContext TSS_C14_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_MatrixROMContext TSS_C14_STRUCTURE_ROM_CONTEXT = { {TSS_C14_INPUTS_GROUP, TSS_C14_INPUTS_COUNT}, TSS_C14_INPUTS_X_COUNT,
                                                               &TSS_C14_MATRIX_TOUCH_CONTEXT,
                                                               TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(14)
                                                             };
    #define C14_DECODER      TSS_MatrixDecoder
  #else /* TSS_CT_ASLIDER || TSS_CT_AROTARY */
    #if (TSS_C14_TYPE == TSS_CT_ASLIDER)
      TSS_COMPILE_TIME_ERROR((TSS_C14_INPUTS_COUNT>1)&&(TSS_C14_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C14_INPUTS)
      TSS_CSASlider TSS_C14_STRUCTURE = { {14u, TSS_C14_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #else
      TSS_COMPILE_TIME_ERROR((TSS_C14_INPUTS_COUNT>2)&&(TSS_C14_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C14_INPUTS)
      TSS_CSARotary TSS_C14_STRUCTURE = { {14u, TSS_C14_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #endif
    TSS_ASliderPosFilterContext TSS_C14_POSFILTER_CONTEXT;
    TSS_ASliderDelayContext TSS_C14_DELAY_CONTEXT;
    TSS_ASliderTrendContext TSS_C14_TREND_CONTEXT;
    uint8_t TSS_C14_DELAY_BUFFER[TSS_ASLIDER_DELAY_BUFFER_LENGTH];
    int8_t TSS_C14_TREND_BUFFER[TSS_ASLIDER_TREND_BUFFER_LENGTH];
    TSS_GenericRAMContext TSS_C14_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_ASliderROMContext TSS_C14_STRUCTURE_ROM_CONTEXT = { {TSS_C14_INPUTS_GROUP, TSS_C14_INPUTS_COUNT},
                                                                  TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(14),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(14),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(14),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(14),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(14)
                                                                };     /* Declares the context ASlider Structure */
    #define C14_DECODER      TSS_ASliderDecoder
  #endif
  void TSS_C14_CALLBACK(TSS_CONTROL_ID u8ControlId);
  uint8_t C14_DECODER(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

#if TSS_N_CONTROLS > 15
  #if (TSS_C15_TYPE == TSS_CT_MATRIX)
    const uint8_t TSS_C15_INPUTS_GROUP[] =  TSS_C15_INPUTS;
    #define TSS_C15_INPUTS_COUNT sizeof(TSS_C15_INPUTS_GROUP)/sizeof(TSS_C15_INPUTS_GROUP[0])
    #define TSS_C15_INPUTS_X_COUNT TSS_C15_INPUTS_NUM_X
  #else
    const uint8_t TSS_C15_INPUTS_GROUP[] = TSS_C15_INPUTS;
    #define TSS_C15_INPUTS_COUNT sizeof(TSS_C15_INPUTS_GROUP)/sizeof(TSS_C15_INPUTS_GROUP[0])
  #endif
  #if (TSS_C15_TYPE == TSS_CT_KEYPAD)
    TSS_COMPILE_TIME_ERROR((TSS_C15_INPUTS_COUNT>0)&&(TSS_C15_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C15_INPUTS)
    uint8_t TSS_C15_KEYPAD_EVENTS_BUFFER[TSS_KEYPAD_EVENTS_BUFFER_SIZE];
    #ifdef TSS_C15_KEYS
      const uint16_t TSS_C15_KEYS_GROUP[] = TSS_C15_KEYS;
      #define TSS_C15_KEYS_COUNT sizeof(TSS_C15_KEYS_GROUP)/sizeof(TSS_C15_KEYS_GROUP[0])
      TSS_COMPILE_TIME_ERROR((TSS_C15_KEYS_COUNT<65536)&&(TSS_C15_KEYS_COUNT>0),TSS_Invalid_quantity_of_TSS_C15_KEYS)
    #else
      #define TSS_C15_KEYS_GROUP NULL
      #define TSS_C15_KEYS_COUNT TSS_C15_INPUTS_COUNT
    #endif
    TSS_CSKeypad TSS_C15_STRUCTURE = { {15u, TSS_C15_TYPE}, TSS_KEYPAD_CONTCONF_INIT, 0u,0u, TSS_KEYPAD_EVENTS_INIT, 0u,0u,0u, TSS_C15_KEYPAD_EVENTS_BUFFER };
    TSS_KeypadRAMContext TSS_C15_STRUCTURE_RAM_CONTEXT = { {0u},0u,0u,0u }; 
    const TSS_KeypadROMContext TSS_C15_STRUCTURE_ROM_CONTEXT = { {TSS_C15_INPUTS_GROUP, TSS_C15_INPUTS_COUNT}, TSS_C15_KEYS_GROUP, TSS_C15_KEYS_COUNT };
    #define C15_DECODER      TSS_KeypadDecoder
  #elif (TSS_C15_TYPE == TSS_CT_SLIDER)
    TSS_COMPILE_TIME_ERROR((TSS_C15_INPUTS_COUNT>1)&&(TSS_C15_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C15_INPUTS)
    TSS_CSSlider TSS_C15_STRUCTURE = { {15u, TSS_C15_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C15_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C15_STRUCTURE_ROM_CONTEXT = { TSS_C15_INPUTS_GROUP, TSS_C15_INPUTS_COUNT };
    #define C15_DECODER      TSS_SliderDecoder
  #elif (TSS_C15_TYPE == TSS_CT_ROTARY)
    TSS_COMPILE_TIME_ERROR((TSS_C15_INPUTS_COUNT>2)&&(TSS_C15_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C15_INPUTS)
    TSS_CSRotary TSS_C15_STRUCTURE = { {15u, TSS_C15_TYPE}, TSS_SLIDER_CONTROL_INIT, TSS_SLIDER_DYN_INIT, TSS_SLIDER_STAT_INIT, TSS_SLIDER_EVENTS_INIT, 0u,0u};
    TSS_GenericRAMContext TSS_C15_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_GenericROMContext TSS_C15_STRUCTURE_ROM_CONTEXT = { TSS_C15_INPUTS_GROUP, TSS_C15_INPUTS_COUNT };
    #define C15_DECODER      TSS_SliderDecoder
  #elif (TSS_C15_TYPE == TSS_CT_MATRIX)
    TSS_COMPILE_TIME_ERROR((TSS_C15_INPUTS_COUNT>3)&&(TSS_C15_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C15_INPUTS)
    TSS_CSMatrix TSS_C15_STRUCTURE = { {15u, TSS_C15_TYPE}, TSS_MATRIX_CONTROL_INIT, TSS_MATRIX_EVENTS_INIT, 0u,0u, TSS_MATRIX_DYN_INIT, TSS_MATRIX_DYN_INIT, 0u,0u,0u,0u,0u,0u };
    TSS_MatrixTouchContext TSS_C15_MATRIX_TOUCH_CONTEXT = { 0u,0u,0u,0u };
    TSS_MatrixPosIIRFilterContext TSS_C15_MATRIX_POSFILTER_IIR_CONTEXT = { 0u,0u };
    TSS_GenericRAMContext TSS_C15_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_MatrixROMContext TSS_C15_STRUCTURE_ROM_CONTEXT = { {TSS_C15_INPUTS_GROUP, TSS_C15_INPUTS_COUNT}, TSS_C15_INPUTS_X_COUNT,
                                                               &TSS_C15_MATRIX_TOUCH_CONTEXT,
                                                               TSS_GET_CONTROL_MATRIX_POSFILTER_IIR_CONTEXT_TYPE(15)
                                                             };
    #define C15_DECODER      TSS_MatrixDecoder
  #else /* TSS_CT_ASLIDER || TSS_CT_AROTARY */
    #if (TSS_C15_TYPE == TSS_CT_ASLIDER)
      TSS_COMPILE_TIME_ERROR((TSS_C15_INPUTS_COUNT>1)&&(TSS_C15_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C15_INPUTS)
      TSS_CSASlider TSS_C15_STRUCTURE = { {15u, TSS_C15_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #else
      TSS_COMPILE_TIME_ERROR((TSS_C15_INPUTS_COUNT>2)&&(TSS_C15_INPUTS_COUNT<17),TSS_Invalid_quantity_of_TSS_C15_INPUTS)
      TSS_CSARotary TSS_C15_STRUCTURE = { {15u, TSS_C15_TYPE}, TSS_ASLIDER_CONTROL_INIT, TSS_ASLIDER_DYN_INIT, 0u, TSS_ASLIDER_EVENTS_INIT, 0u,0u,0u};
    #endif
    TSS_ASliderPosFilterContext TSS_C15_POSFILTER_CONTEXT;
    TSS_ASliderDelayContext TSS_C15_DELAY_CONTEXT;
    TSS_ASliderTrendContext TSS_C15_TREND_CONTEXT;
    uint8_t TSS_C15_DELAY_BUFFER[TSS_ASLIDER_DELAY_BUFFER_LENGTH];
    int8_t TSS_C15_TREND_BUFFER[TSS_ASLIDER_TREND_BUFFER_LENGTH];
    TSS_GenericRAMContext TSS_C15_STRUCTURE_RAM_CONTEXT = { 0u };
    const TSS_ASliderROMContext TSS_C15_STRUCTURE_ROM_CONTEXT = { {TSS_C15_INPUTS_GROUP, TSS_C15_INPUTS_COUNT},
                                                                  TSS_GET_CONTROL_ASLIDER_POSFILTER_CONTEXT_TYPE(15),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_CONTEXT_TYPE(15),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_CONTEXT_TYPE(15),
                                                                  TSS_GET_CONTROL_ASLIDER_DELAY_BUFFER_TYPE(15),
                                                                  TSS_GET_CONTROL_ASLIDER_TREND_BUFFER_TYPE(15)
                                                                };     /* Declares the context ASlider Structure */
    #define C15_DECODER      TSS_ASliderDecoder
  #endif
  void TSS_C15_CALLBACK(TSS_CONTROL_ID u8ControlId);
  uint8_t C15_DECODER(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

/*
 * The section below defines an array of pointers to
 * all the callback functions defined by the user for each decoder
 *
 */

#if TSS_N_CONTROLS != 0
  /* Pointer to Callback functions */
  void (* const tss_faCallback[TSS_N_CONTROLS])(TSS_CONTROL_ID u8ControlId) =
  {
    TSS_C0_CALLBACK,
    #if TSS_N_CONTROLS > 1
      TSS_C1_CALLBACK,
    #endif
    #if TSS_N_CONTROLS > 2
      TSS_C2_CALLBACK,
    #endif
    #if TSS_N_CONTROLS > 3
      TSS_C3_CALLBACK,
    #endif
    #if TSS_N_CONTROLS > 4
      TSS_C4_CALLBACK,
    #endif
    #if TSS_N_CONTROLS > 5
      TSS_C5_CALLBACK,
    #endif
    #if TSS_N_CONTROLS > 6
      TSS_C6_CALLBACK,
    #endif
    #if TSS_N_CONTROLS > 7
      TSS_C7_CALLBACK,
    #endif
    #if TSS_N_CONTROLS > 8
      TSS_C8_CALLBACK,
    #endif
    #if TSS_N_CONTROLS > 9
      TSS_C9_CALLBACK,
    #endif
    #if TSS_N_CONTROLS > 10
      TSS_C10_CALLBACK,
    #endif
    #if TSS_N_CONTROLS > 11
      TSS_C11_CALLBACK,
    #endif
    #if TSS_N_CONTROLS > 12
      TSS_C12_CALLBACK,
    #endif
    #if TSS_N_CONTROLS > 13
      TSS_C13_CALLBACK,
    #endif
    #if TSS_N_CONTROLS > 14
      TSS_C14_CALLBACK,
    #endif
    #if TSS_N_CONTROLS > 15
      TSS_C15_CALLBACK,
    #endif
  };
#else  /* If there are no defined controlers a single element array is created */
  void (* const tss_faCallback[1])(TSS_CONTROL_ID u8ControlId) = {NULL};
#endif


/*
 * The section below declares an array of pointers in RAM with the
 * direction of all the decoders controls context structures
 * created in the sections above
 */

/* Array of pointers to decoders controls context data */
#if TSS_N_CONTROLS != 0
  TSS_GenericRAMContext * const tss_acpsDecRAMContext[TSS_N_CONTROLS] = {
    (TSS_GenericRAMContext *)&TSS_C0_STRUCTURE_RAM_CONTEXT,
    #if TSS_N_CONTROLS > 1
      (TSS_GenericRAMContext *)&TSS_C1_STRUCTURE_RAM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 2
      (TSS_GenericRAMContext *)&TSS_C2_STRUCTURE_RAM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 3
      (TSS_GenericRAMContext *)&TSS_C3_STRUCTURE_RAM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 4
      (TSS_GenericRAMContext *)&TSS_C4_STRUCTURE_RAM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 5
      (TSS_GenericRAMContext *)&TSS_C5_STRUCTURE_RAM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 6
      (TSS_GenericRAMContext *)&TSS_C6_STRUCTURE_RAM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 7
      (TSS_GenericRAMContext *)&TSS_C7_STRUCTURE_RAM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 8
      (TSS_GenericRAMContext *)&TSS_C8_STRUCTURE_RAM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 9
      (TSS_GenericRAMContext *)&TSS_C9_STRUCTURE_RAM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 10
      (TSS_GenericRAMContext *)&TSS_C10_STRUCTURE_RAM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 11
      (TSS_GenericRAMContext *)&TSS_C11_STRUCTURE_RAM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 12
      (TSS_GenericRAMContext *)&TSS_C12_STRUCTURE_RAM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 13
      (TSS_GenericRAMContext *)&TSS_C13_STRUCTURE_RAM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 14
      (TSS_GenericRAMContext *)&TSS_C14_STRUCTURE_RAM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 15
      (TSS_GenericRAMContext *)&TSS_C15_STRUCTURE_RAM_CONTEXT,
    #endif
  };
#else  /* If there are no defined controls, a single element array is created */
  TSS_GenericRAMContext * const tss_acpsDecRAMContext[1];
#endif

/*
 * The section below declares an array of pointers in ROM with the
 * direction of all the decoders controls context structures
 * created in the sections above
 */

/* Array of pointers to decoders controls context data */
#if TSS_N_CONTROLS != 0
  const TSS_GenericROMContext * const tss_acpsDecROMContext[TSS_N_CONTROLS] = {
    (TSS_GenericROMContext *)&TSS_C0_STRUCTURE_ROM_CONTEXT,
    #if TSS_N_CONTROLS > 1
      (TSS_GenericROMContext *)&TSS_C1_STRUCTURE_ROM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 2
      (TSS_GenericROMContext *)&TSS_C2_STRUCTURE_ROM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 3
      (TSS_GenericROMContext *)&TSS_C3_STRUCTURE_ROM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 4
      (TSS_GenericROMContext *)&TSS_C4_STRUCTURE_ROM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 5
      (TSS_GenericROMContext *)&TSS_C5_STRUCTURE_ROM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 6
      (TSS_GenericROMContext *)&TSS_C6_STRUCTURE_ROM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 7
      (TSS_GenericROMContext *)&TSS_C7_STRUCTURE_ROM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 8
      (TSS_GenericROMContext *)&TSS_C8_STRUCTURE_ROM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 9
      (TSS_GenericROMContext *)&TSS_C9_STRUCTURE_ROM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 10
      (TSS_GenericROMContext *)&TSS_C10_STRUCTURE_ROM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 11
      (TSS_GenericROMContext *)&TSS_C11_STRUCTURE_ROM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 12
      (TSS_GenericROMContext *)&TSS_C12_STRUCTURE_ROM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 13
      (TSS_GenericROMContext *)&TSS_C13_STRUCTURE_ROM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 14
      (TSS_GenericROMContext *)&TSS_C14_STRUCTURE_ROM_CONTEXT,
    #endif
    #if TSS_N_CONTROLS > 15
      (TSS_GenericROMContext *)&TSS_C15_STRUCTURE_ROM_CONTEXT,
    #endif
  };
#else  /* If there are no defined controls, a single element array is created */
  const TSS_GenericROMContext * const tss_acpsDecROMContext[1];
#endif

/*
 * The section below declares an array of pointers with the
 * direction of all the decoders controls system structures
 * created in the sections above
 */

#if TSS_N_CONTROLS != 0
  /* Array of pointers to controls structure data */
  const uint8_t * const tss_acpsCSStructs[TSS_N_CONTROLS] = {
    (uint8_t *)&TSS_C0_STRUCTURE,
    #if TSS_N_CONTROLS > 1
      (uint8_t *)&TSS_C1_STRUCTURE,
    #endif
    #if TSS_N_CONTROLS > 2
      (uint8_t *)&TSS_C2_STRUCTURE,
    #endif
    #if TSS_N_CONTROLS > 3
      (uint8_t *)&TSS_C3_STRUCTURE,
    #endif
    #if TSS_N_CONTROLS > 4
      (uint8_t *)&TSS_C4_STRUCTURE,
    #endif
    #if TSS_N_CONTROLS > 5
      (uint8_t *)&TSS_C5_STRUCTURE,
    #endif
    #if TSS_N_CONTROLS > 6
      (uint8_t *)&TSS_C6_STRUCTURE,
    #endif
    #if TSS_N_CONTROLS > 7
      (uint8_t *)&TSS_C7_STRUCTURE,
    #endif
    #if TSS_N_CONTROLS > 8
      (uint8_t *)&TSS_C8_STRUCTURE,
    #endif
    #if TSS_N_CONTROLS > 9
      (uint8_t *)&TSS_C9_STRUCTURE,
    #endif
    #if TSS_N_CONTROLS > 10
      (uint8_t *)&TSS_C10_STRUCTURE,
    #endif
    #if TSS_N_CONTROLS > 11
      (uint8_t *)&TSS_C11_STRUCTURE,
    #endif
    #if TSS_N_CONTROLS > 12
      (uint8_t *)&TSS_C12_STRUCTURE,
    #endif
    #if TSS_N_CONTROLS > 13
      (uint8_t *)&TSS_C13_STRUCTURE,
    #endif
    #if TSS_N_CONTROLS > 14
      (uint8_t *)&TSS_C14_STRUCTURE,
    #endif
    #if TSS_N_CONTROLS > 15
      (uint8_t *)&TSS_C15_STRUCTURE,
    #endif
  };
#else  /* If there are no defined controlers a single element array es created */
  const uint8_t * const tss_acpsCSStructs[1] = {NULL};
#endif

/*
 * The section below defines an array of pointers to
 * all the decoders functions defined above
 *
 */

#if TSS_N_CONTROLS != 0
  /* Pointer to Decoders functions */
  uint8_t (* const tss_faDecoders[TSS_N_CONTROLS])(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command) = {
    C0_DECODER,
    #if TSS_N_CONTROLS > 1
      C1_DECODER,
    #endif
    #if TSS_N_CONTROLS > 2
      C2_DECODER,
    #endif
    #if TSS_N_CONTROLS > 3
      C3_DECODER,
    #endif
    #if TSS_N_CONTROLS > 4
      C4_DECODER,
    #endif
    #if TSS_N_CONTROLS > 5
      C5_DECODER,
    #endif
    #if TSS_N_CONTROLS > 6
      C6_DECODER,
    #endif
    #if TSS_N_CONTROLS > 7
      C7_DECODER,
    #endif
    #if TSS_N_CONTROLS > 8
      C8_DECODER,
    #endif
    #if TSS_N_CONTROLS > 9
      C9_DECODER,
    #endif
    #if TSS_N_CONTROLS > 10
      C10_DECODER,
    #endif
    #if TSS_N_CONTROLS > 11
      C11_DECODER,
    #endif
    #if TSS_N_CONTROLS > 12
      C12_DECODER,
    #endif
    #if TSS_N_CONTROLS > 13
      C13_DECODER,
    #endif
    #if TSS_N_CONTROLS > 14
      C14_DECODER,
    #endif
    #if TSS_N_CONTROLS > 15
      C15_DECODER,
    #endif
  };
#else  /* If there are no defined controlers a single element array es created */
  uint8_t (* const tss_faDecoders[1])(uint8_t u8CtrlNum, uint16_t u16TouchFlag, uint8_t u8NumberOfElec, uint8_t u8Command);
#endif

/*
 * Initializes the Shield Config array
 */

#if TSS_USE_SIGNAL_SHIELDING

  #define TSS_SHIELD_NOT_USED             0x7F
  #define TSS_SHIELD_ELECTRODE_MASK       0x7F
  #define TSS_SHIELD_ELECTRODE_FLAG       0x80
  #define TSS_SHIELD_ELECTRODE_FLAG_MASK  0x80
  #define TSS_SHIELD_ELECTRODE_USED(x,y)  (TSS_E##x##_SHIELD_DATA == y)
  #define TSS_SHIELD_ELECTRODES_USED(x)   (TSS_SHIELD_ELECTRODE_USED(0,x) || TSS_SHIELD_ELECTRODE_USED(1,x) || TSS_SHIELD_ELECTRODE_USED(2,x) || TSS_SHIELD_ELECTRODE_USED(3,x) || TSS_SHIELD_ELECTRODE_USED(4,x) || TSS_SHIELD_ELECTRODE_USED(5,x) || TSS_SHIELD_ELECTRODE_USED(6,x) || TSS_SHIELD_ELECTRODE_USED(7,x) || TSS_SHIELD_ELECTRODE_USED(8,x) || TSS_SHIELD_ELECTRODE_USED(9,x) || TSS_SHIELD_ELECTRODE_USED(10,x) || TSS_SHIELD_ELECTRODE_USED(11,x) || TSS_SHIELD_ELECTRODE_USED(12,x) || TSS_SHIELD_ELECTRODE_USED(13,x) || TSS_SHIELD_ELECTRODE_USED(14,x) || TSS_SHIELD_ELECTRODE_USED(15,x) || TSS_SHIELD_ELECTRODE_USED(16,x) || TSS_SHIELD_ELECTRODE_USED(17,x) || TSS_SHIELD_ELECTRODE_USED(18,x) || TSS_SHIELD_ELECTRODE_USED(19,x) || TSS_SHIELD_ELECTRODE_USED(20,x) || TSS_SHIELD_ELECTRODE_USED(21,x) || TSS_SHIELD_ELECTRODE_USED(22,x) || TSS_SHIELD_ELECTRODE_USED(23,x) || TSS_SHIELD_ELECTRODE_USED(24,x) || TSS_SHIELD_ELECTRODE_USED(25,x) || TSS_SHIELD_ELECTRODE_USED(26,x) || TSS_SHIELD_ELECTRODE_USED(27,x) || TSS_SHIELD_ELECTRODE_USED(28,x) || TSS_SHIELD_ELECTRODE_USED(29,x) || TSS_SHIELD_ELECTRODE_USED(30,x) || TSS_SHIELD_ELECTRODE_USED(31,x) || TSS_SHIELD_ELECTRODE_USED(32,x) || TSS_SHIELD_ELECTRODE_USED(33,x) || TSS_SHIELD_ELECTRODE_USED(34,x) || TSS_SHIELD_ELECTRODE_USED(35,x) || TSS_SHIELD_ELECTRODE_USED(36,x) || TSS_SHIELD_ELECTRODE_USED(37,x) || TSS_SHIELD_ELECTRODE_USED(38,x) || TSS_SHIELD_ELECTRODE_USED(39,x) || TSS_SHIELD_ELECTRODE_USED(40,x) || TSS_SHIELD_ELECTRODE_USED(41,x) || TSS_SHIELD_ELECTRODE_USED(42,x) || TSS_SHIELD_ELECTRODE_USED(43,x) || TSS_SHIELD_ELECTRODE_USED(44,x) || TSS_SHIELD_ELECTRODE_USED(45,x) || TSS_SHIELD_ELECTRODE_USED(46,x) || TSS_SHIELD_ELECTRODE_USED(47,x) || TSS_SHIELD_ELECTRODE_USED(48,x) || TSS_SHIELD_ELECTRODE_USED(49,x) || TSS_SHIELD_ELECTRODE_USED(50,x) || TSS_SHIELD_ELECTRODE_USED(51,x) || TSS_SHIELD_ELECTRODE_USED(52,x) || TSS_SHIELD_ELECTRODE_USED(53,x) || TSS_SHIELD_ELECTRODE_USED(54,x) || TSS_SHIELD_ELECTRODE_USED(55,x) || TSS_SHIELD_ELECTRODE_USED(56,x) || TSS_SHIELD_ELECTRODE_USED(57,x) || TSS_SHIELD_ELECTRODE_USED(58,x) || TSS_SHIELD_ELECTRODE_USED(59,x) || TSS_SHIELD_ELECTRODE_USED(60,x) || TSS_SHIELD_ELECTRODE_USED(61,x) || TSS_SHIELD_ELECTRODE_USED(62,x) || TSS_SHIELD_ELECTRODE_USED(63,x))

  #ifdef TSS_E0_SHIELD_ELECTRODE
    #define TSS_E0_SHIELD_DATA TSS_E0_SHIELD_ELECTRODE
  #else
    #define TSS_E0_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E1_SHIELD_ELECTRODE
    #define TSS_E1_SHIELD_DATA TSS_E1_SHIELD_ELECTRODE
  #else
    #define TSS_E1_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E2_SHIELD_ELECTRODE
    #define TSS_E2_SHIELD_DATA TSS_E2_SHIELD_ELECTRODE
  #else
    #define TSS_E2_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E3_SHIELD_ELECTRODE
    #define TSS_E3_SHIELD_DATA TSS_E3_SHIELD_ELECTRODE
  #else
    #define TSS_E3_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E4_SHIELD_ELECTRODE
    #define TSS_E4_SHIELD_DATA TSS_E4_SHIELD_ELECTRODE
  #else
    #define TSS_E4_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E5_SHIELD_ELECTRODE
    #define TSS_E5_SHIELD_DATA TSS_E5_SHIELD_ELECTRODE
  #else
    #define TSS_E5_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E6_SHIELD_ELECTRODE
    #define TSS_E6_SHIELD_DATA TSS_E6_SHIELD_ELECTRODE
  #else
    #define TSS_E6_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E7_SHIELD_ELECTRODE
    #define TSS_E7_SHIELD_DATA TSS_E7_SHIELD_ELECTRODE
  #else
    #define TSS_E7_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E8_SHIELD_ELECTRODE
    #define TSS_E8_SHIELD_DATA TSS_E8_SHIELD_ELECTRODE
  #else
    #define TSS_E8_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E9_SHIELD_ELECTRODE
    #define TSS_E9_SHIELD_DATA TSS_E9_SHIELD_ELECTRODE
  #else
    #define TSS_E9_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E10_SHIELD_ELECTRODE
    #define TSS_E10_SHIELD_DATA TSS_E10_SHIELD_ELECTRODE
  #else
    #define TSS_E10_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E11_SHIELD_ELECTRODE
    #define TSS_E11_SHIELD_DATA TSS_E11_SHIELD_ELECTRODE
  #else
    #define TSS_E11_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E12_SHIELD_ELECTRODE
    #define TSS_E12_SHIELD_DATA TSS_E12_SHIELD_ELECTRODE
  #else
    #define TSS_E12_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E13_SHIELD_ELECTRODE
    #define TSS_E13_SHIELD_DATA TSS_E13_SHIELD_ELECTRODE
  #else
    #define TSS_E13_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E14_SHIELD_ELECTRODE
    #define TSS_E14_SHIELD_DATA TSS_E14_SHIELD_ELECTRODE
  #else
    #define TSS_E14_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E15_SHIELD_ELECTRODE
    #define TSS_E15_SHIELD_DATA TSS_E15_SHIELD_ELECTRODE
  #else
    #define TSS_E15_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E16_SHIELD_ELECTRODE
    #define TSS_E16_SHIELD_DATA TSS_E16_SHIELD_ELECTRODE
  #else
    #define TSS_E16_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E17_SHIELD_ELECTRODE
    #define TSS_E17_SHIELD_DATA TSS_E17_SHIELD_ELECTRODE
  #else
    #define TSS_E17_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E18_SHIELD_ELECTRODE
    #define TSS_E18_SHIELD_DATA TSS_E18_SHIELD_ELECTRODE
  #else
    #define TSS_E18_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E19_SHIELD_ELECTRODE
    #define TSS_E19_SHIELD_DATA TSS_E19_SHIELD_ELECTRODE
  #else
    #define TSS_E19_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E20_SHIELD_ELECTRODE
    #define TSS_E20_SHIELD_DATA TSS_E20_SHIELD_ELECTRODE
  #else
    #define TSS_E20_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E21_SHIELD_ELECTRODE
    #define TSS_E21_SHIELD_DATA TSS_E21_SHIELD_ELECTRODE
  #else
    #define TSS_E21_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E22_SHIELD_ELECTRODE
    #define TSS_E22_SHIELD_DATA TSS_E22_SHIELD_ELECTRODE
  #else
    #define TSS_E22_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E23_SHIELD_ELECTRODE
    #define TSS_E23_SHIELD_DATA TSS_E23_SHIELD_ELECTRODE
  #else
    #define TSS_E23_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E24_SHIELD_ELECTRODE
    #define TSS_E24_SHIELD_DATA TSS_E24_SHIELD_ELECTRODE
  #else
    #define TSS_E24_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E25_SHIELD_ELECTRODE
    #define TSS_E25_SHIELD_DATA TSS_E25_SHIELD_ELECTRODE
  #else
    #define TSS_E25_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E26_SHIELD_ELECTRODE
    #define TSS_E26_SHIELD_DATA TSS_E26_SHIELD_ELECTRODE
  #else
    #define TSS_E26_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E27_SHIELD_ELECTRODE
    #define TSS_E27_SHIELD_DATA TSS_E27_SHIELD_ELECTRODE
  #else
    #define TSS_E27_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E28_SHIELD_ELECTRODE
    #define TSS_E28_SHIELD_DATA TSS_E28_SHIELD_ELECTRODE
  #else
    #define TSS_E28_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E29_SHIELD_ELECTRODE
    #define TSS_E29_SHIELD_DATA TSS_E29_SHIELD_ELECTRODE
  #else
    #define TSS_E29_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E30_SHIELD_ELECTRODE
    #define TSS_E30_SHIELD_DATA TSS_E30_SHIELD_ELECTRODE
  #else
    #define TSS_E30_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E31_SHIELD_ELECTRODE
    #define TSS_E31_SHIELD_DATA TSS_E31_SHIELD_ELECTRODE
  #else
    #define TSS_E31_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E32_SHIELD_ELECTRODE
    #define TSS_E32_SHIELD_DATA TSS_E32_SHIELD_ELECTRODE
  #else
    #define TSS_E32_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E33_SHIELD_ELECTRODE
    #define TSS_E33_SHIELD_DATA TSS_E33_SHIELD_ELECTRODE
  #else
    #define TSS_E33_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E34_SHIELD_ELECTRODE
    #define TSS_E34_SHIELD_DATA TSS_E34_SHIELD_ELECTRODE
  #else
    #define TSS_E34_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E35_SHIELD_ELECTRODE
    #define TSS_E35_SHIELD_DATA TSS_E35_SHIELD_ELECTRODE
  #else
    #define TSS_E35_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E36_SHIELD_ELECTRODE
    #define TSS_E36_SHIELD_DATA TSS_E36_SHIELD_ELECTRODE
  #else
    #define TSS_E36_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E37_SHIELD_ELECTRODE
    #define TSS_E37_SHIELD_DATA TSS_E37_SHIELD_ELECTRODE
  #else
    #define TSS_E37_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E38_SHIELD_ELECTRODE
    #define TSS_E38_SHIELD_DATA TSS_E38_SHIELD_ELECTRODE
  #else
    #define TSS_E38_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E39_SHIELD_ELECTRODE
    #define TSS_E39_SHIELD_DATA TSS_E39_SHIELD_ELECTRODE
  #else
    #define TSS_E39_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E40_SHIELD_ELECTRODE
    #define TSS_E40_SHIELD_DATA TSS_E40_SHIELD_ELECTRODE
  #else
    #define TSS_E40_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E41_SHIELD_ELECTRODE
    #define TSS_E41_SHIELD_DATA TSS_E41_SHIELD_ELECTRODE
  #else
    #define TSS_E41_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E42_SHIELD_ELECTRODE
    #define TSS_E42_SHIELD_DATA TSS_E42_SHIELD_ELECTRODE
  #else
    #define TSS_E42_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E43_SHIELD_ELECTRODE
    #define TSS_E43_SHIELD_DATA TSS_E43_SHIELD_ELECTRODE
  #else
    #define TSS_E43_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E44_SHIELD_ELECTRODE
    #define TSS_E44_SHIELD_DATA TSS_E44_SHIELD_ELECTRODE
  #else
    #define TSS_E44_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E45_SHIELD_ELECTRODE
    #define TSS_E45_SHIELD_DATA TSS_E45_SHIELD_ELECTRODE
  #else
    #define TSS_E45_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E46_SHIELD_ELECTRODE
    #define TSS_E46_SHIELD_DATA TSS_E46_SHIELD_ELECTRODE
  #else
    #define TSS_E46_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E47_SHIELD_ELECTRODE
    #define TSS_E47_SHIELD_DATA TSS_E47_SHIELD_ELECTRODE
  #else
    #define TSS_E47_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E48_SHIELD_ELECTRODE
    #define TSS_E48_SHIELD_DATA TSS_E48_SHIELD_ELECTRODE
  #else
    #define TSS_E48_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E49_SHIELD_ELECTRODE
    #define TSS_E49_SHIELD_DATA TSS_E49_SHIELD_ELECTRODE
  #else
    #define TSS_E49_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E50_SHIELD_ELECTRODE
    #define TSS_E50_SHIELD_DATA TSS_E50_SHIELD_ELECTRODE
  #else
    #define TSS_E50_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E51_SHIELD_ELECTRODE
    #define TSS_E51_SHIELD_DATA TSS_E51_SHIELD_ELECTRODE
  #else
    #define TSS_E51_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E52_SHIELD_ELECTRODE
    #define TSS_E52_SHIELD_DATA TSS_E52_SHIELD_ELECTRODE
  #else
    #define TSS_E52_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E53_SHIELD_ELECTRODE
    #define TSS_E53_SHIELD_DATA TSS_E53_SHIELD_ELECTRODE
  #else
    #define TSS_E53_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E54_SHIELD_ELECTRODE
    #define TSS_E54_SHIELD_DATA TSS_E54_SHIELD_ELECTRODE
  #else
    #define TSS_E54_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E55_SHIELD_ELECTRODE
    #define TSS_E55_SHIELD_DATA TSS_E55_SHIELD_ELECTRODE
  #else
    #define TSS_E55_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E56_SHIELD_ELECTRODE
    #define TSS_E56_SHIELD_DATA TSS_E56_SHIELD_ELECTRODE
  #else
    #define TSS_E56_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E57_SHIELD_ELECTRODE
    #define TSS_E57_SHIELD_DATA TSS_E57_SHIELD_ELECTRODE
  #else
    #define TSS_E57_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E58_SHIELD_ELECTRODE
    #define TSS_E58_SHIELD_DATA TSS_E58_SHIELD_ELECTRODE
  #else
    #define TSS_E58_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E59_SHIELD_ELECTRODE
    #define TSS_E59_SHIELD_DATA TSS_E59_SHIELD_ELECTRODE
  #else
    #define TSS_E59_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E60_SHIELD_ELECTRODE
    #define TSS_E60_SHIELD_DATA TSS_E60_SHIELD_ELECTRODE
  #else
    #define TSS_E60_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E61_SHIELD_ELECTRODE
    #define TSS_E61_SHIELD_DATA TSS_E61_SHIELD_ELECTRODE
  #else
    #define TSS_E61_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E62_SHIELD_ELECTRODE
    #define TSS_E62_SHIELD_DATA TSS_E62_SHIELD_ELECTRODE
  #else
    #define TSS_E62_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif
  #ifdef TSS_E63_SHIELD_ELECTRODE
    #define TSS_E63_SHIELD_DATA TSS_E63_SHIELD_ELECTRODE
  #else
    #define TSS_E63_SHIELD_DATA TSS_SHIELD_NOT_USED
  #endif

  const uint8_t tss_cau8ShieldPairs[TSS_N_ELECTRODES] = {
    #if TSS_N_ELECTRODES > 0
      #if TSS_SHIELD_ELECTRODES_USED(0)
        TSS_E0_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E0_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 1
      #if TSS_SHIELD_ELECTRODES_USED(1)
        TSS_E1_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E1_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 2
      #if TSS_SHIELD_ELECTRODES_USED(2)
        TSS_E2_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E2_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 3
      #if TSS_SHIELD_ELECTRODES_USED(3)
        TSS_E3_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E3_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 4
      #if TSS_SHIELD_ELECTRODES_USED(4)
        TSS_E4_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E4_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 5
      #if TSS_SHIELD_ELECTRODES_USED(5)
        TSS_E5_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E5_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 6
      #if TSS_SHIELD_ELECTRODES_USED(6)
        TSS_E6_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E6_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 7
      #if TSS_SHIELD_ELECTRODES_USED(7)
        TSS_E7_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E7_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 8
      #if TSS_SHIELD_ELECTRODES_USED(8)
        TSS_E8_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E8_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 9
      #if TSS_SHIELD_ELECTRODES_USED(9)
        TSS_E9_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E9_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 10
      #if TSS_SHIELD_ELECTRODES_USED(10)
        TSS_E10_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E10_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 11
      #if TSS_SHIELD_ELECTRODES_USED(11)
        TSS_E11_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E11_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 12
      #if TSS_SHIELD_ELECTRODES_USED(12)
        TSS_E12_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E12_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 13
      #if TSS_SHIELD_ELECTRODES_USED(13)
        TSS_E13_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E13_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 14
      #if TSS_SHIELD_ELECTRODES_USED(14)
        TSS_E14_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E14_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 15
      #if TSS_SHIELD_ELECTRODES_USED(15)
        TSS_E15_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E15_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 16
      #if TSS_SHIELD_ELECTRODES_USED(16)
        TSS_E16_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E16_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 17
      #if TSS_SHIELD_ELECTRODES_USED(17)
        TSS_E17_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E17_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 18
      #if TSS_SHIELD_ELECTRODES_USED(18)
        TSS_E18_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E18_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 19
      #if TSS_SHIELD_ELECTRODES_USED(19)
        TSS_E19_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E19_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 20
      #if TSS_SHIELD_ELECTRODES_USED(20)
        TSS_E20_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E20_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 21
      #if TSS_SHIELD_ELECTRODES_USED(21)
        TSS_E21_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E21_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 22
      #if TSS_SHIELD_ELECTRODES_USED(22)
        TSS_E22_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E22_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 23
      #if TSS_SHIELD_ELECTRODES_USED(23)
        TSS_E23_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E23_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 24
      #if TSS_SHIELD_ELECTRODES_USED(24)
        TSS_E24_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E24_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 25
      #if TSS_SHIELD_ELECTRODES_USED(25)
        TSS_E25_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E25_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 26
      #if TSS_SHIELD_ELECTRODES_USED(26)
        TSS_E26_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E26_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 27
      #if TSS_SHIELD_ELECTRODES_USED(27)
        TSS_E27_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E27_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 28
      #if TSS_SHIELD_ELECTRODES_USED(28)
        TSS_E28_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E28_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 29
      #if TSS_SHIELD_ELECTRODES_USED(29)
        TSS_E29_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E29_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 30
      #if TSS_SHIELD_ELECTRODES_USED(30)
        TSS_E30_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E30_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 31
      #if TSS_SHIELD_ELECTRODES_USED(31)
        TSS_E31_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E31_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 32
      #if TSS_SHIELD_ELECTRODES_USED(32)
        TSS_E32_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E32_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 33
      #if TSS_SHIELD_ELECTRODES_USED(33)
        TSS_E33_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E33_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 34
      #if TSS_SHIELD_ELECTRODES_USED(34)
        TSS_E34_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E34_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 35
      #if TSS_SHIELD_ELECTRODES_USED(35)
        TSS_E35_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E35_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 36
      #if TSS_SHIELD_ELECTRODES_USED(36)
        TSS_E36_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E36_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 37
      #if TSS_SHIELD_ELECTRODES_USED(37)
        TSS_E37_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E37_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 38
      #if TSS_SHIELD_ELECTRODES_USED(38)
        TSS_E38_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E38_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 39
      #if TSS_SHIELD_ELECTRODES_USED(39)
        TSS_E39_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E39_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 40
      #if TSS_SHIELD_ELECTRODES_USED(40)
        TSS_E40_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E40_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 41
      #if TSS_SHIELD_ELECTRODES_USED(41)
        TSS_E41_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E41_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 42
      #if TSS_SHIELD_ELECTRODES_USED(42)
        TSS_E42_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E42_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 43
      #if TSS_SHIELD_ELECTRODES_USED(43)
        TSS_E43_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E43_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 44
      #if TSS_SHIELD_ELECTRODES_USED(44)
        TSS_E44_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E44_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 45
      #if TSS_SHIELD_ELECTRODES_USED(45)
        TSS_E45_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E45_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 46
      #if TSS_SHIELD_ELECTRODES_USED(46)
        TSS_E46_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E46_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 47
      #if TSS_SHIELD_ELECTRODES_USED(47)
        TSS_E47_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E47_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 48
      #if TSS_SHIELD_ELECTRODES_USED(48)
        TSS_E48_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E48_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 49
      #if TSS_SHIELD_ELECTRODES_USED(49)
        TSS_E49_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E49_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 50
      #if TSS_SHIELD_ELECTRODES_USED(50)
        TSS_E50_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E50_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 51
      #if TSS_SHIELD_ELECTRODES_USED(51)
        TSS_E51_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E51_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 52
      #if TSS_SHIELD_ELECTRODES_USED(52)
        TSS_E52_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E52_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 53
      #if TSS_SHIELD_ELECTRODES_USED(53)
        TSS_E53_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E53_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 54
      #if TSS_SHIELD_ELECTRODES_USED(54)
        TSS_E54_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E54_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 55
      #if TSS_SHIELD_ELECTRODES_USED(55)
        TSS_E55_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E55_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 56
      #if TSS_SHIELD_ELECTRODES_USED(56)
        TSS_E56_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E56_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 57
      #if TSS_SHIELD_ELECTRODES_USED(57)
        TSS_E57_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E57_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 58
      #if TSS_SHIELD_ELECTRODES_USED(58)
        TSS_E58_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E58_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 59
      #if TSS_SHIELD_ELECTRODES_USED(59)
        TSS_E59_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E59_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 60
      #if TSS_SHIELD_ELECTRODES_USED(60)
        TSS_E60_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E60_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 61
      #if TSS_SHIELD_ELECTRODES_USED(61)
        TSS_E61_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E61_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 62
      #if TSS_SHIELD_ELECTRODES_USED(62)
        TSS_E62_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E62_SHIELD_DATA,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 63
      #if TSS_SHIELD_ELECTRODES_USED(63)
        TSS_E63_SHIELD_DATA | TSS_SHIELD_ELECTRODE_FLAG,
      #else
        TSS_E63_SHIELD_DATA,
      #endif
    #endif
  };
#else
  const uint8_t tss_cau8ShieldPairs[1] = {0u};
#endif

/*
 * Initializes the Divider Config array
 */

#define TSS_SINAL_DIVIDER_NOT_USED     1u

#if TSS_USE_SIGNAL_DIVIDER
  /* Array of Signal Divider Config values */
  const uint8_t tss_cau8SignalDivider[TSS_N_ELECTRODES] = {
    #if TSS_N_ELECTRODES > 0
      #if defined(TSS_E0_SIGNAL_DIVIDER)
        #if (TSS_E0_SIGNAL_DIVIDER > 0)
          TSS_E0_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 1
      #if defined(TSS_E1_SIGNAL_DIVIDER)
        #if (TSS_E1_SIGNAL_DIVIDER > 0)
          TSS_E1_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 2
      #if defined(TSS_E2_SIGNAL_DIVIDER)
        #if (TSS_E2_SIGNAL_DIVIDER > 0)
          TSS_E2_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 3
      #if defined(TSS_E3_SIGNAL_DIVIDER)
        #if (TSS_E3_SIGNAL_DIVIDER > 0)
          TSS_E3_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 4
      #if defined(TSS_E4_SIGNAL_DIVIDER)
        #if (TSS_E4_SIGNAL_DIVIDER > 0)
          TSS_E4_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 5
      #if defined(TSS_E5_SIGNAL_DIVIDER)
        #if (TSS_E5_SIGNAL_DIVIDER > 0)
          TSS_E5_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 6
      #if defined(TSS_E6_SIGNAL_DIVIDER)
        #if (TSS_E6_SIGNAL_DIVIDER > 0)
          TSS_E6_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 7
      #if defined(TSS_E7_SIGNAL_DIVIDER)
        #if (TSS_E7_SIGNAL_DIVIDER > 0)
          TSS_E7_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 8
      #if defined(TSS_E8_SIGNAL_DIVIDER)
        #if (TSS_E8_SIGNAL_DIVIDER > 0)
          TSS_E8_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 9
      #if defined(TSS_E9_SIGNAL_DIVIDER)
        #if (TSS_E9_SIGNAL_DIVIDER > 0)
          TSS_E9_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 10
      #if defined(TSS_E10_SIGNAL_DIVIDER)
        #if (TSS_E10_SIGNAL_DIVIDER > 0)
          TSS_E10_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 11
      #if defined(TSS_E11_SIGNAL_DIVIDER)
        #if (TSS_E11_SIGNAL_DIVIDER > 0)
          TSS_E11_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 12
      #if defined(TSS_E12_SIGNAL_DIVIDER)
        #if (TSS_E12_SIGNAL_DIVIDER > 0)
          TSS_E12_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 13
      #if defined(TSS_E13_SIGNAL_DIVIDER)
        #if (TSS_E13_SIGNAL_DIVIDER > 0)
          TSS_E13_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 14
      #if defined(TSS_E14_SIGNAL_DIVIDER)
        #if (TSS_E14_SIGNAL_DIVIDER > 0)
          TSS_E14_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 15
      #if defined(TSS_E15_SIGNAL_DIVIDER)
        #if (TSS_E15_SIGNAL_DIVIDER > 0)
          TSS_E15_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 16
      #if defined(TSS_E16_SIGNAL_DIVIDER)
        #if (TSS_E16_SIGNAL_DIVIDER > 0)
          TSS_E16_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 17
      #if defined(TSS_E17_SIGNAL_DIVIDER)
        #if (TSS_E17_SIGNAL_DIVIDER > 0)
          TSS_E17_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 18
      #if defined(TSS_E18_SIGNAL_DIVIDER)
        #if (TSS_E18_SIGNAL_DIVIDER > 0)
          TSS_E18_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 19
      #if defined(TSS_E19_SIGNAL_DIVIDER)
        #if (TSS_E19_SIGNAL_DIVIDER > 0)
          TSS_E19_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 20
      #if defined(TSS_E20_SIGNAL_DIVIDER)
        #if (TSS_E20_SIGNAL_DIVIDER > 0)
          TSS_E20_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 21
      #if defined(TSS_E21_SIGNAL_DIVIDER)
        #if (TSS_E21_SIGNAL_DIVIDER > 0)
          TSS_E21_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 22
      #if defined(TSS_E22_SIGNAL_DIVIDER)
        #if (TSS_E22_SIGNAL_DIVIDER > 0)
          TSS_E22_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 23
      #if defined(TSS_E23_SIGNAL_DIVIDER)
        #if (TSS_E23_SIGNAL_DIVIDER > 0)
          TSS_E23_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 24
      #if defined(TSS_E24_SIGNAL_DIVIDER)
        #if (TSS_E24_SIGNAL_DIVIDER > 0)
          TSS_E24_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 25
      #if defined(TSS_E25_SIGNAL_DIVIDER)
        #if (TSS_E25_SIGNAL_DIVIDER > 0)
          TSS_E25_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 26
      #if defined(TSS_E26_SIGNAL_DIVIDER)
        #if (TSS_E26_SIGNAL_DIVIDER > 0)
          TSS_E26_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 27
      #if defined(TSS_E27_SIGNAL_DIVIDER)
        #if (TSS_E27_SIGNAL_DIVIDER > 0)
          TSS_E27_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 28
      #if defined(TSS_E28_SIGNAL_DIVIDER)
        #if (TSS_E28_SIGNAL_DIVIDER > 0)
          TSS_E28_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 29
      #if defined(TSS_E29_SIGNAL_DIVIDER)
        #if (TSS_E29_SIGNAL_DIVIDER > 0)
          TSS_E29_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 30
      #if defined(TSS_E30_SIGNAL_DIVIDER)
        #if (TSS_E30_SIGNAL_DIVIDER > 0)
          TSS_E30_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 31
      #if defined(TSS_E31_SIGNAL_DIVIDER)
        #if (TSS_E31_SIGNAL_DIVIDER > 0)
          TSS_E31_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 32
      #if defined(TSS_E32_SIGNAL_DIVIDER)
        #if (TSS_E32_SIGNAL_DIVIDER > 0)
          TSS_E32_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 33
      #if defined(TSS_E33_SIGNAL_DIVIDER)
        #if (TSS_E33_SIGNAL_DIVIDER > 0)
          TSS_E33_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 34
      #if defined(TSS_E34_SIGNAL_DIVIDER)
        #if (TSS_E34_SIGNAL_DIVIDER > 0)
          TSS_E34_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 35
      #if defined(TSS_E35_SIGNAL_DIVIDER)
        #if (TSS_E35_SIGNAL_DIVIDER > 0)
          TSS_E35_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 36
      #if defined(TSS_E36_SIGNAL_DIVIDER)
        #if (TSS_E36_SIGNAL_DIVIDER > 0)
          TSS_E36_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 37
      #if defined(TSS_E37_SIGNAL_DIVIDER)
        #if (TSS_E37_SIGNAL_DIVIDER > 0)
          TSS_E37_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 38
      #if defined(TSS_E38_SIGNAL_DIVIDER)
        #if (TSS_E38_SIGNAL_DIVIDER > 0)
          TSS_E38_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 39
      #if defined(TSS_E39_SIGNAL_DIVIDER)
        #if (TSS_E39_SIGNAL_DIVIDER > 0)
          TSS_E39_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 40
      #if defined(TSS_E40_SIGNAL_DIVIDER)
        #if (TSS_E40_SIGNAL_DIVIDER > 0)
          TSS_E40_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 41
      #if defined(TSS_E41_SIGNAL_DIVIDER)
        #if (TSS_E41_SIGNAL_DIVIDER > 0)
          TSS_E41_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 42
      #if defined(TSS_E42_SIGNAL_DIVIDER)
        #if (TSS_E42_SIGNAL_DIVIDER > 0)
          TSS_E42_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 43
      #if defined(TSS_E43_SIGNAL_DIVIDER)
        #if (TSS_E43_SIGNAL_DIVIDER > 0)
          TSS_E43_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 44
      #if defined(TSS_E44_SIGNAL_DIVIDER)
        #if (TSS_E44_SIGNAL_DIVIDER > 0)
          TSS_E44_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 45
      #if defined(TSS_E45_SIGNAL_DIVIDER)
        #if (TSS_E45_SIGNAL_DIVIDER > 0)
          TSS_E45_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 46
      #if defined(TSS_E46_SIGNAL_DIVIDER)
        #if (TSS_E46_SIGNAL_DIVIDER > 0)
          TSS_E46_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 47
      #if defined(TSS_E47_SIGNAL_DIVIDER)
        #if (TSS_E47_SIGNAL_DIVIDER > 0)
          TSS_E47_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 48
      #if defined(TSS_E48_SIGNAL_DIVIDER)
        #if (TSS_E48_SIGNAL_DIVIDER > 0)
          TSS_E48_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 49
      #if defined(TSS_E49_SIGNAL_DIVIDER)
        #if (TSS_E49_SIGNAL_DIVIDER > 0)
          TSS_E49_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 50
      #if defined(TSS_E50_SIGNAL_DIVIDER)
        #if (TSS_E50_SIGNAL_DIVIDER > 0)
          TSS_E50_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 51
      #if defined(TSS_E51_SIGNAL_DIVIDER)
        #if (TSS_E51_SIGNAL_DIVIDER > 0)
          TSS_E51_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 52
      #if defined(TSS_E52_SIGNAL_DIVIDER)
        #if (TSS_E52_SIGNAL_DIVIDER > 0)
          TSS_E52_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 53
      #if defined(TSS_E53_SIGNAL_DIVIDER)
        #if (TSS_E53_SIGNAL_DIVIDER > 0)
          TSS_E53_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 54
      #if defined(TSS_E54_SIGNAL_DIVIDER)
        #if (TSS_E54_SIGNAL_DIVIDER > 0)
          TSS_E54_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 55
      #if defined(TSS_E55_SIGNAL_DIVIDER)
        #if (TSS_E55_SIGNAL_DIVIDER > 0)
          TSS_E55_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 56
      #if defined(TSS_E56_SIGNAL_DIVIDER)
        #if (TSS_E56_SIGNAL_DIVIDER > 0)
          TSS_E56_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 57
      #if defined(TSS_E57_SIGNAL_DIVIDER)
        #if (TSS_E57_SIGNAL_DIVIDER > 0)
          TSS_E57_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 58
      #if defined(TSS_E58_SIGNAL_DIVIDER)
        #if (TSS_E58_SIGNAL_DIVIDER > 0)
          TSS_E58_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 59
      #if defined(TSS_E59_SIGNAL_DIVIDER)
        #if (TSS_E59_SIGNAL_DIVIDER > 0)
          TSS_E59_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 60
      #if defined(TSS_E60_SIGNAL_DIVIDER)
        #if (TSS_E60_SIGNAL_DIVIDER > 0)
          TSS_E60_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 61
      #if defined(TSS_E61_SIGNAL_DIVIDER)
        #if (TSS_E61_SIGNAL_DIVIDER > 0)
          TSS_E61_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 62
      #if defined(TSS_E62_SIGNAL_DIVIDER)
        #if (TSS_E62_SIGNAL_DIVIDER > 0)
          TSS_E62_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 63
      #if defined(TSS_E63_SIGNAL_DIVIDER)
        #if (TSS_E63_SIGNAL_DIVIDER > 0)
          TSS_E63_SIGNAL_DIVIDER,
        #else
          TSS_SINAL_DIVIDER_NOT_USED,
        #endif
      #else
        TSS_SINAL_DIVIDER_NOT_USED,
      #endif
    #endif
  };
#else
  const uint8_t tss_cau8SignalDivider[1u] = {0u};
#endif

/*
 * Initializes the Signal Multiplier Config array
 */

#define TSS_SINAL_MULTIPLIER_NOT_USED     1u

#if TSS_USE_SIGNAL_MULTIPLIER
  /* Array of Signal Multiplier Config values */
  const uint8_t tss_cau8SignalMultiplier[TSS_N_ELECTRODES] = {
    #if TSS_N_ELECTRODES > 0
      #if defined(TSS_E0_SIGNAL_MULTIPLIER)
        #if (TSS_E0_SIGNAL_MULTIPLIER > 0)
          TSS_E0_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 1
      #if defined(TSS_E1_SIGNAL_MULTIPLIER)
        #if (TSS_E1_SIGNAL_MULTIPLIER > 0)
          TSS_E1_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 2
      #if defined(TSS_E2_SIGNAL_MULTIPLIER)
        #if (TSS_E2_SIGNAL_MULTIPLIER > 0)
          TSS_E2_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 3
      #if defined(TSS_E3_SIGNAL_MULTIPLIER)
        #if (TSS_E3_SIGNAL_MULTIPLIER > 0)
          TSS_E3_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 4
      #if defined(TSS_E4_SIGNAL_MULTIPLIER)
        #if (TSS_E4_SIGNAL_MULTIPLIER > 0)
          TSS_E4_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 5
      #if defined(TSS_E5_SIGNAL_MULTIPLIER)
        #if (TSS_E5_SIGNAL_MULTIPLIER > 0)
          TSS_E5_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 6
      #if defined(TSS_E6_SIGNAL_MULTIPLIER)
        #if (TSS_E6_SIGNAL_MULTIPLIER > 0)
          TSS_E6_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 7
      #if defined(TSS_E7_SIGNAL_MULTIPLIER)
        #if (TSS_E7_SIGNAL_MULTIPLIER > 0)
          TSS_E7_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 8
      #if defined(TSS_E8_SIGNAL_MULTIPLIER)
        #if (TSS_E8_SIGNAL_MULTIPLIER > 0)
          TSS_E8_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 9
      #if defined(TSS_E9_SIGNAL_MULTIPLIER)
        #if (TSS_E9_SIGNAL_MULTIPLIER > 0)
          TSS_E9_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 10
      #if defined(TSS_E10_SIGNAL_MULTIPLIER)
        #if (TSS_E10_SIGNAL_MULTIPLIER > 0)
          TSS_E10_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 11
      #if defined(TSS_E11_SIGNAL_MULTIPLIER)
        #if (TSS_E11_SIGNAL_MULTIPLIER > 0)
          TSS_E11_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 12
      #if defined(TSS_E12_SIGNAL_MULTIPLIER)
        #if (TSS_E12_SIGNAL_MULTIPLIER > 0)
          TSS_E12_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 13
      #if defined(TSS_E13_SIGNAL_MULTIPLIER)
        #if (TSS_E13_SIGNAL_MULTIPLIER > 0)
          TSS_E13_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 14
      #if defined(TSS_E14_SIGNAL_MULTIPLIER)
        #if (TSS_E14_SIGNAL_MULTIPLIER > 0)
          TSS_E14_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 15
      #if defined(TSS_E15_SIGNAL_MULTIPLIER)
        #if (TSS_E15_SIGNAL_MULTIPLIER > 0)
          TSS_E15_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 16
      #if defined(TSS_E16_SIGNAL_MULTIPLIER)
        #if (TSS_E16_SIGNAL_MULTIPLIER > 0)
          TSS_E16_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 17
      #if defined(TSS_E17_SIGNAL_MULTIPLIER)
        #if (TSS_E17_SIGNAL_MULTIPLIER > 0)
          TSS_E17_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 18
      #if defined(TSS_E18_SIGNAL_MULTIPLIER)
        #if (TSS_E18_SIGNAL_MULTIPLIER > 0)
          TSS_E18_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 19
      #if defined(TSS_E19_SIGNAL_MULTIPLIER)
        #if (TSS_E19_SIGNAL_MULTIPLIER > 0)
          TSS_E19_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 20
      #if defined(TSS_E20_SIGNAL_MULTIPLIER)
        #if (TSS_E20_SIGNAL_MULTIPLIER > 0)
          TSS_E20_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 21
      #if defined(TSS_E21_SIGNAL_MULTIPLIER)
        #if (TSS_E21_SIGNAL_MULTIPLIER > 0)
          TSS_E21_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 22
      #if defined(TSS_E22_SIGNAL_MULTIPLIER)
        #if (TSS_E22_SIGNAL_MULTIPLIER > 0)
          TSS_E22_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 23
      #if defined(TSS_E23_SIGNAL_MULTIPLIER)
        #if (TSS_E23_SIGNAL_MULTIPLIER > 0)
          TSS_E23_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 24
      #if defined(TSS_E24_SIGNAL_MULTIPLIER)
        #if (TSS_E24_SIGNAL_MULTIPLIER > 0)
          TSS_E24_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 25
      #if defined(TSS_E25_SIGNAL_MULTIPLIER)
        #if (TSS_E25_SIGNAL_MULTIPLIER > 0)
          TSS_E25_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 26
      #if defined(TSS_E26_SIGNAL_MULTIPLIER)
        #if (TSS_E26_SIGNAL_MULTIPLIER > 0)
          TSS_E26_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 27
      #if defined(TSS_E27_SIGNAL_MULTIPLIER)
        #if (TSS_E27_SIGNAL_MULTIPLIER > 0)
          TSS_E27_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 28
      #if defined(TSS_E28_SIGNAL_MULTIPLIER)
        #if (TSS_E28_SIGNAL_MULTIPLIER > 0)
          TSS_E28_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 29
      #if defined(TSS_E29_SIGNAL_MULTIPLIER)
        #if (TSS_E29_SIGNAL_MULTIPLIER > 0)
          TSS_E29_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 30
      #if defined(TSS_E30_SIGNAL_MULTIPLIER)
        #if (TSS_E30_SIGNAL_MULTIPLIER > 0)
          TSS_E30_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 31
      #if defined(TSS_E31_SIGNAL_MULTIPLIER)
        #if (TSS_E31_SIGNAL_MULTIPLIER > 0)
          TSS_E31_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 32
      #if defined(TSS_E32_SIGNAL_MULTIPLIER)
        #if (TSS_E32_SIGNAL_MULTIPLIER > 0)
          TSS_E32_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 33
      #if defined(TSS_E33_SIGNAL_MULTIPLIER)
        #if (TSS_E33_SIGNAL_MULTIPLIER > 0)
          TSS_E33_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 34
      #if defined(TSS_E34_SIGNAL_MULTIPLIER)
        #if (TSS_E34_SIGNAL_MULTIPLIER > 0)
          TSS_E34_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 35
      #if defined(TSS_E35_SIGNAL_MULTIPLIER)
        #if (TSS_E35_SIGNAL_MULTIPLIER > 0)
          TSS_E35_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 36
      #if defined(TSS_E36_SIGNAL_MULTIPLIER)
        #if (TSS_E36_SIGNAL_MULTIPLIER > 0)
          TSS_E36_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 37
      #if defined(TSS_E37_SIGNAL_MULTIPLIER)
        #if (TSS_E37_SIGNAL_MULTIPLIER > 0)
          TSS_E37_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 38
      #if defined(TSS_E38_SIGNAL_MULTIPLIER)
        #if (TSS_E38_SIGNAL_MULTIPLIER > 0)
          TSS_E38_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 39
      #if defined(TSS_E39_SIGNAL_MULTIPLIER)
        #if (TSS_E39_SIGNAL_MULTIPLIER > 0)
          TSS_E39_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 40
      #if defined(TSS_E40_SIGNAL_MULTIPLIER)
        #if (TSS_E40_SIGNAL_MULTIPLIER > 0)
          TSS_E40_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 41
      #if defined(TSS_E41_SIGNAL_MULTIPLIER)
        #if (TSS_E41_SIGNAL_MULTIPLIER > 0)
          TSS_E41_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 42
      #if defined(TSS_E42_SIGNAL_MULTIPLIER)
        #if (TSS_E42_SIGNAL_MULTIPLIER > 0)
          TSS_E42_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 43
      #if defined(TSS_E43_SIGNAL_MULTIPLIER)
        #if (TSS_E43_SIGNAL_MULTIPLIER > 0)
          TSS_E43_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 44
      #if defined(TSS_E44_SIGNAL_MULTIPLIER)
        #if (TSS_E44_SIGNAL_MULTIPLIER > 0)
          TSS_E44_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 45
      #if defined(TSS_E45_SIGNAL_MULTIPLIER)
        #if (TSS_E45_SIGNAL_MULTIPLIER > 0)
          TSS_E45_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 46
      #if defined(TSS_E46_SIGNAL_MULTIPLIER)
        #if (TSS_E46_SIGNAL_MULTIPLIER > 0)
          TSS_E46_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 47
      #if defined(TSS_E47_SIGNAL_MULTIPLIER)
        #if (TSS_E47_SIGNAL_MULTIPLIER > 0)
          TSS_E47_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 48
      #if defined(TSS_E48_SIGNAL_MULTIPLIER)
        #if (TSS_E48_SIGNAL_MULTIPLIER > 0)
          TSS_E48_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 49
      #if defined(TSS_E49_SIGNAL_MULTIPLIER)
        #if (TSS_E49_SIGNAL_MULTIPLIER > 0)
          TSS_E49_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 50
      #if defined(TSS_E50_SIGNAL_MULTIPLIER)
        #if (TSS_E50_SIGNAL_MULTIPLIER > 0)
          TSS_E50_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 51
      #if defined(TSS_E51_SIGNAL_MULTIPLIER)
        #if (TSS_E51_SIGNAL_MULTIPLIER > 0)
          TSS_E51_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 52
      #if defined(TSS_E52_SIGNAL_MULTIPLIER)
        #if (TSS_E52_SIGNAL_MULTIPLIER > 0)
          TSS_E52_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 53
      #if defined(TSS_E53_SIGNAL_MULTIPLIER)
        #if (TSS_E53_SIGNAL_MULTIPLIER > 0)
          TSS_E53_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 54
      #if defined(TSS_E54_SIGNAL_MULTIPLIER)
        #if (TSS_E54_SIGNAL_MULTIPLIER > 0)
          TSS_E54_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 55
      #if defined(TSS_E55_SIGNAL_MULTIPLIER)
        #if (TSS_E55_SIGNAL_MULTIPLIER > 0)
          TSS_E55_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 56
      #if defined(TSS_E56_SIGNAL_MULTIPLIER)
        #if (TSS_E56_SIGNAL_MULTIPLIER > 0)
          TSS_E56_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 57
      #if defined(TSS_E57_SIGNAL_MULTIPLIER)
        #if (TSS_E57_SIGNAL_MULTIPLIER > 0)
          TSS_E57_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 58
      #if defined(TSS_E58_SIGNAL_MULTIPLIER)
        #if (TSS_E58_SIGNAL_MULTIPLIER > 0)
          TSS_E58_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 59
      #if defined(TSS_E59_SIGNAL_MULTIPLIER)
        #if (TSS_E59_SIGNAL_MULTIPLIER > 0)
          TSS_E59_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 60
      #if defined(TSS_E60_SIGNAL_MULTIPLIER)
        #if (TSS_E60_SIGNAL_MULTIPLIER > 0)
          TSS_E60_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 61
      #if defined(TSS_E61_SIGNAL_MULTIPLIER)
        #if (TSS_E61_SIGNAL_MULTIPLIER > 0)
          TSS_E61_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 62
      #if defined(TSS_E62_SIGNAL_MULTIPLIER)
        #if (TSS_E62_SIGNAL_MULTIPLIER > 0)
          TSS_E62_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
    #if TSS_N_ELECTRODES > 63
      #if defined(TSS_E63_SIGNAL_MULTIPLIER)
        #if (TSS_E63_SIGNAL_MULTIPLIER > 0)
          TSS_E63_SIGNAL_MULTIPLIER,
        #else
          TSS_SINAL_MULTIPLIER_NOT_USED,
        #endif
      #else
        TSS_SINAL_MULTIPLIER_NOT_USED,
      #endif
    #endif
  };
#else
  const uint8_t tss_cau8SignalMultiplier[1u] = {0u};
#endif

/*
 * The section below defines an array of pointers to
 * private dat for each control
 *
 */

#if TSS_USE_CONTROL_PRIVATE_DATA
  void * tss_apsControlPrivateData[TSS_N_CONTROLS] = {
    NULL,
    #if TSS_N_CONTROLS > 1
      NULL,
    #endif
    #if TSS_N_CONTROLS > 2
      NULL,
    #endif
    #if TSS_N_CONTROLS > 3
      NULL,
    #endif
    #if TSS_N_CONTROLS > 4
      NULL,
    #endif
    #if TSS_N_CONTROLS > 5
      NULL,
    #endif
    #if TSS_N_CONTROLS > 6
      NULL,
    #endif
    #if TSS_N_CONTROLS > 7
      NULL,
    #endif
    #if TSS_N_CONTROLS > 8
      NULL,
    #endif
    #if TSS_N_CONTROLS > 9
      NULL,
    #endif
    #if TSS_N_CONTROLS > 10
      NULL,
    #endif
    #if TSS_N_CONTROLS > 11
      NULL,
    #endif
    #if TSS_N_CONTROLS > 12
      NULL,
    #endif
    #if TSS_N_CONTROLS > 13
      NULL,
    #endif
    #if TSS_N_CONTROLS > 14
      NULL,
    #endif
    #if TSS_N_CONTROLS > 15
      NULL,
    #endif
  };
  extern void* TSS_InternalControlPrivateData(uint8_t u8CntrlNum, void *pData, uint8_t action);
  void* (* const tss_fSetInternalPrivateData)(uint8_t u8CntrlNum, void *pData, uint8_t action) = TSS_InternalControlPrivateData;
#else
  void* (* const tss_fSetInternalPrivateData)(uint8_t u8CntrlNum, void *pData, uint8_t action) = NULL;
  void *tss_apsControlPrivateData[1u] = {NULL};
#endif

/*
 * Declaration of main structures and arrays used by TSS
 */

TSS_CSSystem tss_CSSys;
uint8_t tss_au8Sensitivity[TSS_N_ELECTRODES];
uint8_t tss_au8ElectrodeEnablers[((TSS_N_ELECTRODES - 1)/ 8) + 1];
uint8_t tss_au8ElectrodeStatus[((TSS_N_ELECTRODES - 1)/ 8) + 1];
uint16_t tss_au16ElecBaseline[TSS_N_ELECTRODES];
uint8_t tss_au8ElectrodeMeasured[((TSS_N_ELECTRODES - 1)/ 8) + 1];
uint8_t tss_au8ElectrodeMarker[((TSS_N_ELECTRODES - 1)/ 8) + 1];
uint8_t tss_au8ElectrodeLowThresholdEnablers[((TSS_N_ELECTRODES - 1)/ 8) + 1];
uint8_t tss_au8ElectrodeDCTrackerEnablers[((TSS_N_ELECTRODES - 1)/ 8) + 1];

/* Declaration of arrays used to store electrode behavior values */
uint8_t tss_au8DebounceCount[TSS_N_ELECTRODES];
uint8_t tss_au8TouchCount[TSS_N_ELECTRODES];
uint8_t tss_au8ReleaseCount[TSS_N_ELECTRODES];
uint8_t tss_au8ElecState[TSS_N_ELECTRODES];

/* Evaluate No of Controls */
#if TSS_N_CONTROLS != 0
  extern void TSS_DecodersControl(uint8_t u8Command);
  void (* const tss_fDecodersControl) (uint8_t u8Command) = TSS_DecodersControl;

  uint8_t tss_au8ControlScheduleCount[TSS_N_CONTROLS];
#else
  uint8_t tss_au8ControlScheduleCount[1];
  void (* const tss_fDecodersControl) (uint8_t u8Command) = NULL;
#endif

const uint8_t tss_cu8NumElecs = (uint8_t)TSS_N_ELECTRODES;
const uint8_t tss_cu8NumCtrls = (uint8_t)TSS_N_CONTROLS;
const uint8_t tss_cu8ElecBitFldLn = (uint8_t)(((uint8_t)TSS_N_ELECTRODES - 1U) / 8U) + 1U;
const uint16_t tss_cu16Version = __TSS_VERSION__;

/*
 * The section below defines separation of the functions
 * called in low level functions according to selected TSS features by user
 *
 */

#if TSS_USE_IIR_FILTER
  extern void TSS_KeyDetectorCalculateIIRFilter(uint8_t u8ElecNum);

  void (* const tss_fCalculateIIRFilter) (uint8_t u8ElecNum) = TSS_KeyDetectorCalculateIIRFilter;
#else
  void (* const tss_fCalculateIIRFilter) (uint8_t u8ElecNum) = NULL;
#endif

#if TSS_USE_SIGNAL_SHIELDING
  extern void TSS_KeyDetectorCalculateShielding(uint8_t u8ElecNum);

  void (* const tss_fCalculateShielding) (uint8_t u8ElecNum) = TSS_KeyDetectorCalculateShielding;
#else
  void (* const tss_fCalculateShielding) (uint8_t u8ElecNum) = NULL;
#endif

#if TSS_USE_SIGNAL_SHIELDING || TSS_USE_IIR_FILTER
  extern void TSS_KeyDetectorInitPrevCapSample(uint8_t u8ElecNum);

  void (* const tss_fInitPrevCapSample) (uint8_t u8ElecNum) = TSS_KeyDetectorInitPrevCapSample;

  uint16_t tss_au16PrevCapSample[TSS_N_ELECTRODES];
#else
  void (* const tss_fInitPrevCapSample) (uint8_t u8ElecNum) = NULL;

  uint16_t tss_au16PrevCapSample[1]; /* Dummy */
#endif

#if TSS_USE_DELTA_LOG
  extern void TSS_KeyDetectorFillDeltaLogBuffer(uint8_t u8ElecNum, int8_t i8Delta);

  void (* const tss_fFillDeltaLogBuffer) (uint8_t u8ElecNum, int8_t i8Delta) = TSS_KeyDetectorFillDeltaLogBuffer;

  int8_t tss_ai8InstantDelta[TSS_N_ELECTRODES];
#else
  void (* const tss_fFillDeltaLogBuffer) (uint8_t u8ElecNum, int8_t i8Delta) = NULL;

  int8_t tss_ai8InstantDelta[1]; /* Dummy */
#endif

#if TSS_USE_INTEGRATION_DELTA_LOG
  extern void TSS_KeyAFIDFillIntegrDeltaLogBuffer(uint8_t u8ElecNum);

  void (* const tss_fFillIntegrDeltaLogBuffer) (uint8_t u8ElecNum) = TSS_KeyAFIDFillIntegrDeltaLogBuffer;

  int8_t tss_ai8IntegrationDelta[TSS_N_ELECTRODES];
#else
  void (* const tss_fFillIntegrDeltaLogBuffer) (uint8_t u8ElecNum) = NULL;

  int8_t tss_ai8IntegrationDelta[1]; /* Dummy */
#endif

#if TSS_USE_SIGNAL_LOG
  extern void TSS_KeyDetectorFillSignalLogBuffer(uint8_t u8ElecNum);

  void (* const tss_fFillSignalLogBuffer) (uint8_t u8ElecNum) = TSS_KeyDetectorFillSignalLogBuffer;

  uint16_t tss_au16InstantSignal[TSS_N_ELECTRODES];
#else
  void (* const tss_fFillSignalLogBuffer) (uint8_t u8ElecNum) = NULL;

  uint16_t tss_au16InstantSignal[1]; /* Dummy */
#endif

#if TSS_USE_DATA_CORRUPTION_CHECK
  extern void TSS_DataCorruptionCheck(uint8_t u8Command);

  void (* const tss_fDataCorruptionCheck) (uint8_t u8Command) = TSS_DataCorruptionCheck;

  uint8_t tss_u8ConfCheckSum;
#else
  void (* const tss_fDataCorruptionCheck) (uint8_t u8Command) = NULL;

  uint8_t tss_u8ConfCheckSum; /* Dummy */
#endif

#if TSS_USE_FREEMASTER_GUI
  extern void TSS_FmstrCall(void);

  void (* const tss_fFmstrCall) (void) = TSS_FmstrCall;
#else
  void (* const tss_fFmstrCall) (void) = NULL;
#endif

#if TSS_USE_TRIGGER_FUNCTION
  extern uint8_t TSS_TriggerConfig(uint8_t u8Parameter, uint8_t u8Value);
  extern uint8_t TSS_TriggerControl(uint8_t u8Command);

  uint8_t (* const tss_fTriggerConfig) (uint8_t u8Parameter, uint8_t u8Value) = TSS_TriggerConfig;
  uint8_t (* const tss_fTriggerControl) (uint8_t u8Command) = TSS_TriggerControl;
#else
  uint8_t (* const tss_fTriggerConfig) (uint8_t u8Parameter, uint8_t u8Value) = NULL;
  uint8_t (* const tss_fTriggerControl) (uint8_t u8Command) = NULL;
#endif

#if TSS_LOWPOWER_USED || defined(TSS_ONPROXIMITY_CALLBACK)
  extern uint8_t TSS_LowPowerConfig(uint8_t u8Parameter, uint8_t u8Value);

  uint8_t (* const tss_fLowPowerConfig) (uint8_t u8Parameter, uint8_t u8Value) = TSS_LowPowerConfig;
#else
  uint8_t (* const tss_fLowPowerConfig) (uint8_t u8Parameter, uint8_t u8Value) = NULL;
#endif

#ifdef TSS_ONFAULT_CALLBACK
  extern void TSS_ONFAULT_CALLBACK(uint8_t u8FaultElecNum);

  void (* const tss_fOnFault) (uint8_t u8FaultElecNum) = TSS_ONFAULT_CALLBACK;
#else
  void (* const tss_fOnFault) (uint8_t u8FaultElecNum) = NULL;
#endif

#ifdef TSS_ONINIT_CALLBACK
  extern void TSS_ONINIT_CALLBACK(void);

  void (* const tss_fOnInit) (void) = TSS_ONINIT_CALLBACK;
#else
  extern void TSS_fOnInit(void);

  void (* const tss_fOnInit) (void) = TSS_fOnInit;
#endif

#if TSS_ASLIDER_POSITION_IIR_USED
  const uint8_t tss_cu8APosFilterWeight = TSS_ASLIDER_POSITION_IIR_WEIGHT;
  extern uint16_t TSS_ASliderCalculatePosIIRFilter(uint8_t u8CtrlId, uint8_t u8Command, uint16_t u16Position);

  uint16_t (* const tss_fASliderCalculatePosIIRFilter) (uint8_t u8CtrlId, uint8_t u8Command, uint16_t u16Position) = TSS_ASliderCalculatePosIIRFilter;
#else
  const uint8_t tss_cu8APosFilterWeight = 1u;
  uint16_t (* const tss_fASliderCalculatePosIIRFilter) (uint8_t u8CtrlId, uint8_t u8Command, uint16_t u16Position) = NULL;
#endif

#if TSS_ASLIDER_DELAY_USED
  const uint8_t tss_cu8ASliderDelayBufferLength = TSS_ASLIDER_DELAY_BUFFER_LENGTH;
  extern uint8_t TSS_ASliderDelayControl(uint8_t u8CtrlId, uint8_t u8Command, uint8_t *u8Pos);

  uint8_t (* const tss_fASliderDelayControl) (uint8_t u8CtrlId, uint8_t u8Command, uint8_t *u8Pos) = TSS_ASliderDelayControl;
#else
  const uint8_t tss_cu8ASliderDelayBufferLength = 1u;
  uint8_t (* const tss_fASliderDelayControl) (uint8_t u8CtrlId, uint8_t u8Command, uint8_t *u8Pos) = NULL;
#endif

#if TSS_ASLIDER_TREND_USED
  const uint8_t tss_cu8ASliderTrendBufferLength = TSS_ASLIDER_TREND_BUFFER_LENGTH;
  #if TSS_ASLIDER_TREND_THRESHOLD
    const int8_t tss_i8ASliderTrendThreshold = TSS_ASLIDER_TREND_THRESHOLD;
    uint8_t (* const tss_fASliderTrendThreshold) (uint8_t u8MaxPos1, uint8_t u8MaxPos2) = NULL;
  #else
    const int8_t tss_i8ASliderTrendThreshold = 40;  /* Coeficient for automatic threshold calculation */
    extern uint8_t TSS_ASliderTrendThreshold(uint8_t u8MaxPos1, uint8_t u8MaxPos2);

    uint8_t (* const tss_fASliderTrendThreshold) (uint8_t u8MaxPos1, uint8_t u8MaxPos2) = TSS_ASliderTrendThreshold;
  #endif

  extern uint8_t TSS_ASliderTrendControl(uint8_t u8CtrlId, uint8_t u8Command, uint8_t u8MaxPos1, uint8_t u8MaxPos2);

  uint8_t (* const tss_fASliderTrendControl) (uint8_t u8CtrlId, uint8_t u8Command, uint8_t u8MaxPos1, uint8_t u8MaxPos2) = TSS_ASliderTrendControl;
#else
  const int8_t tss_i8ASliderTrendThreshold = TSS_ASLIDER_TREND_THRESHOLD;
  uint8_t (* const tss_fASliderTrendThreshold) (uint8_t u8MaxPos1, uint8_t u8MaxPos2) = NULL;
  const uint8_t tss_cu8ASliderTrendBufferLength = 1u;
  uint8_t (* const tss_fASliderTrendControl) (uint8_t u8CtrlId, uint8_t u8Command, uint8_t u8MaxPos1, uint8_t u8MaxPos2) = NULL;
#endif

#if TSS_MATRIX_POSITION_IIR_USED
  const uint8_t tss_cu8MatrixPosFilterWeight = TSS_MATRIX_POSITION_IIR_WEIGHT;

  extern uint16_t TSS_MatrixCalculatePosIIRFilter(uint16_t u16Position, uint16_t *pu16PrevPosition);
  uint16_t (* const tss_fMatrixCalculatePosIIRFilter) (uint16_t u16Position, uint16_t *pu16PrevPosition) = TSS_MatrixCalculatePosIIRFilter;
#else
  const uint8_t tss_cu8MatrixPosFilterWeight = 1u;
  uint16_t (* const tss_fMatrixCalculatePosIIRFilter) (uint16_t u16Position, uint16_t *pu16PrevPosition) = NULL;
#endif

#ifdef TSS_ONPROXIMITY_CALLBACK
  extern void TSS_ONPROXIMITY_CALLBACK(void);
  extern uint8_t TSS_ProximityConfig(uint8_t u8Value);

  void (* const tss_fOnProximity) (void) = TSS_ONPROXIMITY_CALLBACK;
  uint8_t (* const tss_fProximityConfig) (uint8_t u8Value) = TSS_ProximityConfig;
#else
  void (* const tss_fOnProximity) (void) = NULL;
  uint8_t (* const tss_fProximityConfig) (uint8_t u8Value) = NULL;
#endif

#if TSS_USE_STUCK_KEY
  #if (TSS_USE_KEYDETECTOR_VERSION == 1)
    extern void TSS_KeyBasicStuckKeyControl(uint8_t u8ElecNum, int8_t i8Delta);
    void (* const tss_fStuckKeyControl) (uint8_t u8ElecNum, int8_t i8Delta) = TSS_KeyBasicStuckKeyControl;
  #elif (TSS_USE_KEYDETECTOR_VERSION == 2)
    extern void TSS_KeyAFIDStuckKeyControl(uint8_t u8ElecNum, int8_t i8Delta);
    void (* const tss_fStuckKeyControl) (uint8_t u8ElecNum, int8_t i8Delta) = TSS_KeyAFIDStuckKeyControl;
  #endif

  extern void TSS_KeyDetectorResetStuckKeyCounter(uint8_t u8ElecNum);
  void (* const tss_fResetStuckKeyCounter) (uint8_t u8ElecNum) = TSS_KeyDetectorResetStuckKeyCounter;

  uint8_t tss_au8StuckCount[TSS_N_ELECTRODES];
#else
  void (* const tss_fStuckKeyControl) (uint8_t u8ElecNum, int8_t i8Delta) = NULL;
  void (* const tss_fResetStuckKeyCounter) (uint8_t u8ElecNum) = NULL;

  uint8_t tss_au8StuckCount[1];  /* Dummy */
#endif

#if TSS_USE_NEGATIVE_BASELINE_DROP
  extern void TSS_KeyBasicNegativeBaselineDrop(uint8_t u8ElecNum, int8_t i8Delta);

  void (* const tss_fNegativeBaselineDrop) (uint8_t u8ElecNum, int8_t i8Delta) = TSS_KeyBasicNegativeBaselineDrop;
#else
  void (* const tss_fNegativeBaselineDrop) (uint8_t u8ElecNum, int8_t i8Delta) = NULL;
#endif

#if TSS_USE_AUTO_HW_RECALIBRATION
  extern uint8_t TSS_KeyDetectorRecalibrationRequest(uint8_t u8ElecNum);

  uint8_t (* const tss_fKeyDetectorRecalibrationRequest) (uint8_t u8ElecNum) = TSS_KeyDetectorRecalibrationRequest;
#else
  uint8_t (* const tss_fKeyDetectorRecalibrationRequest) (uint8_t u8ElecNum) = NULL;
#endif

#if TSS_USE_SIGNAL_DIVIDER
  extern void TSS_KeyDetectorSignalDivider(uint8_t u8ElecNum);

  void (* const tss_fSignalDivider) (uint8_t u8ElecNum) = TSS_KeyDetectorSignalDivider;
#else
  void (* const tss_fSignalDivider) (uint8_t u8ElecNum) = NULL;
#endif

#if TSS_USE_SIGNAL_MULTIPLIER
  extern void TSS_KeyDetectorSignalMultiplier(uint8_t u8ElecNum);

  void (* const tss_fSignalMultiplier) (uint8_t u8ElecNum) = TSS_KeyDetectorSignalMultiplier;
#else
  void (* const tss_fSignalMultiplier) (uint8_t u8ElecNum) = NULL;
#endif

/*************************** AUTOCALIBRATION **************************/

#if TSS_USE_AUTO_SENS_CALIBRATION
  #if (TSS_USE_KEYDETECTOR_VERSION == 1)
    extern uint8_t TSS_KeyBasicSensAutoCalibration(uint8_t u8ElecNum, uint8_t u8Command);

    uint8_t (* const tss_fAutoCalibration) (uint8_t u8ElecNum, uint8_t u8Command) = TSS_KeyBasicSensAutoCalibration;
  #elif (TSS_USE_KEYDETECTOR_VERSION == 2)
    extern uint8_t TSS_KeyAFIDSensAutoCalibration(uint8_t u8ElecNum, uint8_t u8Command);

    uint8_t (* const tss_fAutoCalibration) (uint8_t u8ElecNum, uint8_t u8Command) = TSS_KeyAFIDSensAutoCalibration;
  #endif
  int8_t tss_ai8MaxDeltaAmplitude[TSS_N_ELECTRODES];
  int8_t tss_ai8DeltaAmplitude[TSS_N_ELECTRODES];

  TSS_AutoSensCalibrationData tss_asAutoCalibrationData[TSS_N_ELECTRODES];

  #if TSS_USE_AUTO_SENS_CALIB_INIT_DURATION
    int8_t tss_ai8NoiseAmplitude[TSS_N_ELECTRODES];
  #else
    int8_t tss_ai8NoiseAmplitude[1];
  #endif
  #if TSS_USE_AUTO_SENS_CALIB_LOW_LIMIT
    extern int8_t TSS_SensitivityLowLimitControl(uint8_t u8ElecNum, int8_t i8Sensitivity);

    int8_t (* const tss_fSensitivityLowLimitControl) (uint8_t u8ElecNum, int8_t i8Sensitivity) = TSS_SensitivityLowLimitControl;
    int8_t tss_ai8SensitivityLowLimit[TSS_N_ELECTRODES];
  #else
    int8_t (* const tss_fSensitivityLowLimitControl) (uint8_t u8ElecNum, int8_t i8Sensitivity) = NULL;
    int8_t tss_ai8SensitivityLowLimit[1];
  #endif

#else
  uint8_t (* const tss_fAutoCalibration) (uint8_t u8ElecNum, uint8_t u8Command) = NULL;
  int8_t tss_ai8MaxDeltaAmplitude[1];
  int8_t tss_ai8DeltaAmplitude[1];
  int8_t tss_ai8NoiseAmplitude[1];
  int8_t tss_ai8SensitivityLowLimit[1];
  int8_t (* const tss_fSensitivityLowLimitControl) (uint8_t u8ElecNum, int8_t i8Sensitivity) = NULL;
  TSS_AutoSensCalibrationData tss_asAutoCalibrationData[1];
#endif

#if TSS_USE_DELTA_NOISE_ANALYSIS_IIR
  extern uint8_t TSS_KeyBasicDeltaNoiseAnalysisIIRControl(uint8_t u8ElecNum, int8_t i8Delta, uint8_t u8Command);

  uint8_t (* const tss_fDeltaNoiseAnalysisIIRControl) (uint8_t u8ElecNum, int8_t i8Delta, uint8_t u8Command) = TSS_KeyBasicDeltaNoiseAnalysisIIRControl;

  TSS_DeltaNoiseAnalysisIIRData tss_asDeltaNoiseAnalysisIIRData[TSS_N_ELECTRODES];
#else
  uint8_t (* const tss_fDeltaNoiseAnalysisIIRControl) (uint8_t u8ElecNum, int8_t i8Delta, uint8_t u8Command) = NULL;
  TSS_DeltaNoiseAnalysisIIRData tss_asDeltaNoiseAnalysisIIRData[1];
#endif

const uint8_t tss_cu8BaselineInitLength = TSS_USE_BASELINE_INIT_DURATION;
const uint8_t tss_cu8AutoCalibInitLength = TSS_USE_AUTO_SENS_CALIB_INIT_DURATION;
const int8_t tss_ci8DeltaNoiseAnalysisIIRWeights[2u] = { TSS_USE_DELTA_NOISE_ANALYSIS_IIR_WEIGHT1, TSS_USE_DELTA_NOISE_ANALYSIS_IIR_WEIGHT2 };
const TSS_KeyBasicAutoSensCalibContext tss_csKeyBasicASCContext = {4,100u,255u,50,20,0u};
const TSS_KeyAFIDAutoSensCalibContext tss_csKeyAFIDASCContext = {2,255u,75,15,0u};

/*************************** KEYDETECTOR2 **************************/

#if (TSS_USE_KEYDETECTOR_VERSION == 1)
  extern uint8_t TSS_KeyBasicElectrodeEval(uint8_t u8ElecNum, uint8_t u8Mode);

  uint8_t (* const tss_fKeyDetectorElectrodeEval) (uint8_t u8ElecNum, uint8_t u8Mode) = TSS_KeyBasicElectrodeEval;
  void (* const tss_fKeyDetectorIntegrationResetEval) (uint8_t u8Command) = NULL;
#elif (TSS_USE_KEYDETECTOR_VERSION == 2)
  extern uint8_t TSS_KeyAFIDElectrodeEval(uint8_t u8ElecNum, uint8_t u8Mode);
  extern void TSS_KeyAFIDIntegrationResetEval(uint8_t u8Command);

  uint8_t (* const tss_fKeyDetectorElectrodeEval) (uint8_t u8ElecNum, uint8_t u8Mode) = TSS_KeyAFIDElectrodeEval;
  void (* const tss_fKeyDetectorIntegrationResetEval) (uint8_t u8Command) = TSS_KeyAFIDIntegrationResetEval;

  TSS_KeyDetectorFiltersData tss_asKeyDetectorFiltersData[TSS_N_ELECTRODES];
  int16_t tss_ai16KeyDetectorFiltersIntegrationValue[TSS_N_ELECTRODES];
  const Frac32 tss_cfr32IIR1FilterConstant = TSS_AFID_GET_FILTER_RATIO_COEF(TSS_USE_AFID_FAST_FILTER_RATIO); /* Ratio is fsample/fcutoff */
  const Frac32 tss_cfr32IIR2FilterConstant = TSS_AFID_GET_FILTER_RATIO_COEF(TSS_USE_AFID_SLOW_FILTER_RATIO); /* Ratio is fsample/fcutoff */
  uint16_t tss_u16IntegrationResetCounter;
  const uint16_t tss_cu16IntegrationResetPeriod = 1000u; /* Period for Integration reset */
#endif

/************************* KEYDETECTOR NOISE ************************/

#if (TSS_USE_NOISE_MODE == 1)

  #define TSS_USE_NOISE_MODE_LOW_THRESHOLD_MINIMUM  1   /* Minimum Value for Noise Low Threshold */
  #define TSS_USE_NOISE_MODE_LOW_THRESHOLD_RATIO    20  /* Noise Low Threshold Ratio = Detected Threshld Level / Final Noise Low Threshold (1 unit = 10%) */
  #define TSS_USE_NOISE_MODE_TOUCH_RANGE            2   /* Consider only touches within max value  +/- touch_range */

  extern void TSS_KeyNoiseSchedulerInit(uint8_t u8CounterOffset);
  extern void TSS_KeyNoiseSchedulerRun(uint8_t u8ActiveMode);

  void (* const tss_fKeyNoiseSchedulerInit)(uint8_t u8CounterOffset) = TSS_KeyNoiseSchedulerInit;
  void (* const tss_fKeyNoiseSchedulerRun)(uint8_t u8ActiveMode) = TSS_KeyNoiseSchedulerRun;

  uint8_t tss_u8NoiseModeLowThreshold;
  const uint8_t tss_cu8NoiseModeLowThresholdMinimum = TSS_USE_NOISE_MODE_LOW_THRESHOLD_MINIMUM;
  const uint8_t tss_cu8NoiseModeLowThresholdRatio = TSS_USE_NOISE_MODE_LOW_THRESHOLD_RATIO;
  const uint8_t tss_cu8NoiseModeTouchRange =  TSS_USE_NOISE_MODE_TOUCH_RANGE;

  #if TSS_USE_NOISE_SIGNAL_LOG
    extern void TSS_KeyDetectorFillNoiseSignalLogBuffer(uint8_t u8ElecNum);
    void (* const tss_fFillNoiseSignalLogBuffer)(uint8_t u8ElecNum) = TSS_KeyDetectorFillNoiseSignalLogBuffer;
    uint8_t tss_au8InstantNoise[TSS_N_ELECTRODES];
  #else
    void (* const tss_fFillNoiseSignalLogBuffer)(uint8_t u8ElecNum) = NULL;
  #endif

  #if TSS_USE_IIR_FILTER
    uint8_t tss_au8PrevNoiseSample[TSS_N_ELECTRODES];
    extern void TSS_KeyNoiseIIRControl(uint8_t u8ElecNum, uint8_t u8Command);
    void (* const tss_fKeyNoiseIIRControl)(uint8_t u8ElecNum, uint8_t u8Command) = TSS_KeyNoiseIIRControl;
  #else
    uint8_t tss_au8PrevNoiseSample[1];
    void (* const tss_fKeyNoiseIIRControl)(uint8_t u8ElecNum, uint8_t u8Command) = NULL;
  #endif
#else
  void (* const tss_fKeyNoiseSchedulerInit)(uint8_t u8CounterOffset) = NULL;
  void (* const tss_fKeyNoiseSchedulerRun)(uint8_t u8ActiveMode) = NULL;
  void (* const tss_fKeyNoiseIIRControl)(uint8_t u8ElecNum, uint8_t u8Command) = NULL;
#endif

/**************************** Load/StoreSystemData ****************************/

#if (TSS_USE_KEYDETECTOR_VERSION == 1)
  extern uint8_t TSS_KeyBasicGetSystemDataContext(uint8_t u8Parameter, uint8_t u8Electrode, uint8_t **pu8DataAddress);

  uint8_t (* const tss_fGetSystemDataContext) (uint8_t u8Parameter, uint8_t u8Electrode, uint8_t **pu8DataAddress) = TSS_KeyBasicGetSystemDataContext;
#elif (TSS_USE_KEYDETECTOR_VERSION == 2)
  extern uint8_t TSS_KeyAFIDGetSystemDataContext(uint8_t u8Parameter, uint8_t u8Electrode, uint8_t **pu8DataAddress);

  uint8_t (* const tss_fGetSystemDataContext) (uint8_t u8Parameter, uint8_t u8Electrode, uint8_t **pu8DataAddress) = TSS_KeyAFIDGetSystemDataContext;
#endif

/***************************************************************************//*!
*
* @brief  Performs a calculation of new signal value by user defined
*         IIR filter equation from the current signal capacitance value and
*         previous signal value for a certain electrode
*
* @param  u16CurrentSample - Current measured signal value
*         u16PreviousSample - Previous signal value
*
* @return New calculated signal value processed by IIR filter
*
* @remarks The principle of IIR Filter is to modify u16CurrentSample by any
*          weight of u16PreviousSample and the calculated value is
*          returned to the library by the return value of TSS_IIRFilterEquation().
*          The user may edit the equation according to his requirements,
*          but he has to follow previous rules.
*          Warning: Design new equation with respect to unsigned integer value
*                   range of variables and number of samples used by TSS.
*
****************************************************************************/
#if TSS_USE_IIR_FILTER
  #define IIR_WEIGHT 3u

  uint16_t TSS_IIRFilterEquation(uint16_t u16CurrentSample, uint16_t u16PreviousSample)
  {
    #if TSS_HCS08_MCU
      return (uint16_t) ((u16CurrentSample/(IIR_WEIGHT+1)) + (IIR_WEIGHT * (u16PreviousSample/(IIR_WEIGHT+1))));
    #else
      return (uint16_t) ((u16CurrentSample + ((uint32_t)IIR_WEIGHT * u16PreviousSample)) /(IIR_WEIGHT+1));
    #endif
  }
#else
  /* Dummy Function - just for relations, anyway removed from flash */
  uint16_t TSS_IIRFilterEquation(uint16_t u16CurrentSample, uint16_t u16PreviousSample)
  {
    (void) u16CurrentSample; /* suppress 'unused variable' warning */
    (void) u16PreviousSample; /* suppress 'unused variable' warning */

    return TSS_STATUS_OK;
  }
#endif
