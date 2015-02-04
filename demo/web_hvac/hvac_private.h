/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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
*   
*
*
*END************************************************************************/
#ifndef __hvac_private_h__
#define __hvac_private_h__

#include "bsp.h"

#define HVAC_CONTROL_CYCLE_IN_SECONDS    1

#define HVAC_CONTROL_CYCLE_IN_TICKS      (HVAC_CONTROL_CYCLE_IN_SECONDS*BSP_ALARM_FREQUENCY)

#define HVAC_LOG_CYCLE_IN_CONTROL_CYCLES 15


#define HVAC_DEFAULT_TEMP     200   // in 1/10 degree C

#define HVAC_PARAMS_CHANGED  1    

#if defined(BSP_TWR_K70F120M) || defined(BSP_TWR_K60N512) || defined(BSP_TWR_K40X256) ||\
    defined(BSP_TWR_K40D100M) || defined(BSP_TWR_K60F120M) || defined(BSP_TWR_K60D100M)   
    #if BSPCFG_ENABLE_ADC1
        #define ENABLE_ADC BSPCFG_ENABLE_ADC1 
        #define MY_ADC "adc1:" /* must be #1 as the inputs are wired to ADC 1 */  
        #if defined (BSP_TWR_K40X256) || defined (BSP_TWR_K40D100M)
            #define ADC_CH_HEART BSP_ADC_CH_ACCEL_Y  
        #else
            #define ADC_CH_HEART  ADC1_SOURCE_ADPM1
        #endif 
        #define ADC_SOURCE0 ADC1_SOURCE_ADPM1
        #define MY_TRIGGER ADC_PDB_TRIGGER
    #endif /* BSPCFG_ENABLE_ADCx */

#else /* defined BSP_xxx */    
    #if BSPCFG_ENABLE_ADC
        #define ENABLE_ADC BSPCFG_ENABLE_ADC
        #define MY_ADC "adc:"    
        #define ADC_CH_HEART BSP_ADC_CH_POT
        #define ADC_SOURCE0 BSP_ADC_CH_POT
        #define MY_TRIGGER 0
    #endif /* BSPCFG_ENABLE_ADCx */
#endif /* defined BSP_xxx */


typedef struct {
   LWEVENT_STRUCT       Event;
   HVAC_Mode_t          HVACMode;
   FAN_Mode_t           FanMode;
   Temperature_Scale_t  TemperatureScale;
   uint32_t              DesiredTemperature;
} HVAC_PARAMS, * HVAC_PARAMS_PTR;

typedef struct  {
   HVAC_Mode_t    HVACState;
   bool        FanOn;
   uint32_t        ActualTemperature;
} HVAC_STATE, * HVAC_STATE_PTR;


extern HVAC_STATE  HVAC_State;
extern HVAC_PARAMS HVAC_Params;

void HVAC_InitializeParameters(void); 
bool HVAC_InitializeIO(void);
void HVAC_ResetOutputs(void);
void HVAC_SetOutput(HVAC_Output_t,bool);
bool HVAC_GetInput(HVAC_Input_t);
bool HVAC_WaitParameters(int32_t);
void HVAC_InitializeADC(void); 
_mqx_int ReadADC(void);

#endif
