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


#include "hvac.h"

#include "logging_public.h"
#include "hvac_public.h"
#include "hvac_private.h"

HVAC_PARAMS HVAC_Params = {0};

const char *HVACModeName[] = {"Off", "Cool", "Heat", "Auto"};


void HVAC_InitializeParameters(void) 
{
   _lwevent_create(&HVAC_Params.Event, 0);
   HVAC_Params.HVACMode = HVAC_Auto;
   HVAC_Params.FanMode = Fan_Automatic;
   HVAC_Params.TemperatureScale = Celsius;
   HVAC_Params.DesiredTemperature = HVAC_DEFAULT_TEMP;
}


bool HVAC_WaitParameters(int32_t timeout) 
{
    bool catched;
    #if DEMOCFG_ENABLE_SWITCH_TASK
        // switch-sensing task is running, we can simply wait for the event or timeout
        _lwevent_wait_ticks(&HVAC_Params.Event, HVAC_PARAMS_CHANGED, TRUE, timeout);
        
    #else
        // poll the keys frequenly until the event or timeout expires
        uint32_t poll_wait = BSP_ALARM_FREQUENCY/20; // 50ms
        int32_t  elapsed;
        MQX_TICK_STRUCT tickstart, ticknow;
        
        _time_get_elapsed_ticks(&tickstart);

        do
        {
            Switch_Poll();
            _lwevent_wait_ticks(&HVAC_Params.Event, HVAC_PARAMS_CHANGED, TRUE, poll_wait);

            _time_get_elapsed_ticks(&ticknow);
            elapsed = _time_diff_ticks_int32 (&ticknow, &tickstart, NULL);
            
        } while (elapsed < timeout);
        
    #endif          
    catched = (HVAC_Params.Event.VALUE & HVAC_PARAMS_CHANGED) != 0;
    _lwevent_clear(&HVAC_Params.Event, HVAC_PARAMS_CHANGED);
    return catched;
    
}


char *HVAC_HVACModeName(HVAC_Mode_t mode)
{
   return (char *)HVACModeName[mode];
}

uint32_t HVAC_ConvertCelsiusToDisplayTemp(uint32_t temp)
{
   uint32_t  display_temp;
   
   if (HVAC_Params.TemperatureScale ==  Celsius) {
      display_temp = temp;
   } else {
      display_temp = temp*9/5+320;
   }
   
   return display_temp;
}

uint32_t HVAC_ConvertDisplayTempToCelsius(uint32_t display_temp)
{
   uint32_t  temp;
   
   if (HVAC_Params.TemperatureScale ==  Celsius) {
      temp = display_temp;
   } else {
      temp = (display_temp-320)*5/9;
   }
   
   return temp;
}
     

uint32_t HVAC_GetDesiredTemperature(void) {
   return HVAC_ConvertCelsiusToDisplayTemp(HVAC_Params.DesiredTemperature);
}


void HVAC_SetDesiredTemperature(uint32_t temp)
{
   HVAC_Params.DesiredTemperature = HVAC_ConvertDisplayTempToCelsius(temp);
   _lwevent_set(&HVAC_Params.Event, HVAC_PARAMS_CHANGED);
}


FAN_Mode_t HVAC_GetFanMode(void) {
   return HVAC_Params.FanMode;
}


void HVAC_SetFanMode(FAN_Mode_t mode)
{
   HVAC_Params.FanMode = mode;
   _lwevent_set(&HVAC_Params.Event, HVAC_PARAMS_CHANGED);
}


HVAC_Mode_t HVAC_GetHVACMode(void) 
{
   return HVAC_Params.HVACMode;
}


void HVAC_SetHVACMode(HVAC_Mode_t mode)
{
   HVAC_Params.HVACMode = mode;
   _lwevent_set(&HVAC_Params.Event, HVAC_PARAMS_CHANGED);
}


uint32_t HVAC_GetActualTemperature(void) {
   return HVAC_ConvertCelsiusToDisplayTemp(HVAC_State.ActualTemperature);
}


Temperature_Scale_t HVAC_GetTemperatureScale(void) 
{
   return HVAC_Params.TemperatureScale;
}


void HVAC_SetTemperatureScale(Temperature_Scale_t scale)
{
   HVAC_Params.TemperatureScale = scale;
   _lwevent_set(&HVAC_Params.Event, HVAC_PARAMS_CHANGED);
}

char HVAC_GetTemperatureSymbol(void) 
{
   return (HVAC_Params.TemperatureScale==Celsius)?'C':'F';
}




void HVAC_LogCurrentState(void) 
{
   char     Message[LOG_MESSAGE_SIZE];
   uint32_t  temp_d,temp_a,output,i;

   temp_d = HVAC_GetDesiredTemperature();
   temp_a = HVAC_GetActualTemperature();
   
   i=sprintf(Message,"mode: %s, set:%2d.%1d %c temp:%2d.%1d %c Fan: %s ",
      HVAC_HVACModeName(HVAC_GetHVACMode()), 
      temp_d/10, temp_d%10, HVAC_GetTemperatureSymbol(), 
      temp_a/10, temp_a%10, HVAC_GetTemperatureSymbol(),
      HVAC_GetFanMode()==Fan_Automatic?"Auto":"On  ");

   for(output=0;output<HVAC_MAX_OUTPUTS;output++) {
      i+=sprintf(&Message[i],"%s %s, ", HVAC_GetOutputName((HVAC_Output_t)output), HVAC_GetOutput((HVAC_Output_t)output)?"On":"Off");
   }
   sprintf(&Message[i],"\n");

   Log(Message);

}

/* EOF */
