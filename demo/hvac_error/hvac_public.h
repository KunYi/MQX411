#ifndef __hvac_public_h__
#define __hvac_public_h__
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


// HVAC user interface
#define HVAC_TEMP_TOLERANCE   0    // in 1/10 degree 
#define HVAC_TEMP_SW_DELTA    5    // in 1/10 degree 

// ambient temperature change emulation
#define HVAC_TEMP_UPD_DELTA   1    // in 1/10 degree 
#define HVAC_TEMP_UPDATE_RATE 1    // in seconds

#define HVAC_TEMP_MINIMUM     0    // in Celsius
#define HVAC_TEMP_MAXIMUM     500  // in Celsius

typedef enum {
   HVAC_FAN_OUTPUT=0,
   HVAC_HEAT_OUTPUT,
   HVAC_COOL_OUTPUT,
   HVAC_ALIVE_OUTPUT,
   HVAC_MAX_OUTPUTS
} HVAC_Output_t;

typedef enum {
   HVAC_TEMP_UP_INPUT=0,
   HVAC_TEMP_DOWN_INPUT,
   HVAC_FAN_ON_INPUT,
   HVAC_HEAT_ON_INPUT,
   HVAC_AC_ON_INPUT,
   HVAC_MAX_INPUTS
} HVAC_Input_t;


typedef enum {
   HVAC_Off,
   HVAC_Cool,
   HVAC_Heat,
   HVAC_Auto
} HVAC_Mode_t;

typedef enum {
   Fan_Automatic,
   Fan_On
} FAN_Mode_t;

typedef enum {
   Fahrenheit,
   Celsius
} Temperature_Scale_t;

extern LWSEM_STRUCT Logging_init_sem;

extern void HVAC_SetDesiredTemperature(uint32_t);
extern uint32_t HVAC_GetDesiredTemperature(void);

extern void HVAC_SetFanMode(FAN_Mode_t);
extern FAN_Mode_t HVAC_GetFanMode(void);

extern void HVAC_SetHVACMode(HVAC_Mode_t);
extern HVAC_Mode_t HVAC_GetHVACMode(void);
extern char *HVAC_HVACModeName(HVAC_Mode_t mode);

extern uint32_t HVAC_ConvertCelsiusToDisplayTemp(uint32_t temp);
extern uint32_t HVAC_ConvertDisplayTempToCelsius(uint32_t display_temp);
extern void HVAC_SetTemperatureScale(Temperature_Scale_t);
extern Temperature_Scale_t HVAC_GetTemperatureScale(void);
extern char HVAC_GetTemperatureSymbol(void); 

extern uint32_t HVAC_GetAmbientTemperature(void);
extern uint32_t HVAC_GetActualTemperature(void);
extern void HVAC_ReadAmbientTemperature(void);

extern bool HVAC_GetOutput(HVAC_Output_t);
extern char *HVAC_GetOutputName(HVAC_Output_t);

extern void HVAC_LogCurrentState(void);



#endif
