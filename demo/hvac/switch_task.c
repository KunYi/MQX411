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
#include "hvac_public.h"
#include "hvac_private.h"

static bool InputState[HVAC_MAX_INPUTS] = {0};
static bool LastInputState[HVAC_MAX_INPUTS] = {0};

void Switch_Poll(void) 
{
   HVAC_Input_t   input;

   // Save previous input states
   for (input=(HVAC_Input_t)0;input<HVAC_MAX_INPUTS;input++) {
      LastInputState[input]=InputState[input];
      InputState[input]=HVAC_GetInput(input);
   }

   // Check each input for a low-high transition (button press)
   if (InputState[HVAC_TEMP_UP_INPUT]) {
      if (!LastInputState[HVAC_TEMP_UP_INPUT] && !InputState[HVAC_TEMP_DOWN_INPUT]) {
         if (HVAC_GetDesiredTemperature() < HVAC_ConvertCelsiusToDisplayTemp(HVAC_TEMP_MAXIMUM - HVAC_TEMP_SW_DELTA)) 
         {
            HVAC_SetDesiredTemperature(HVAC_GetDesiredTemperature()+HVAC_TEMP_SW_DELTA);
         }
      }
   } else if (InputState[HVAC_TEMP_DOWN_INPUT] && !LastInputState[HVAC_TEMP_DOWN_INPUT] ) {
      if (HVAC_GetDesiredTemperature() > HVAC_ConvertCelsiusToDisplayTemp(HVAC_TEMP_MINIMUM + HVAC_TEMP_SW_DELTA)) 
      {
         HVAC_SetDesiredTemperature(HVAC_GetDesiredTemperature()-HVAC_TEMP_SW_DELTA);
      }
   }

   if (InputState[HVAC_FAN_ON_INPUT] & !LastInputState[HVAC_FAN_ON_INPUT] ) {
      HVAC_SetFanMode(HVAC_GetFanMode()==Fan_On?Fan_Automatic:Fan_On);
   }

   if (InputState[HVAC_HEAT_ON_INPUT] & !LastInputState[HVAC_HEAT_ON_INPUT] ) {
      HVAC_SetHVACMode(HVAC_Heat);
   } else if (InputState[HVAC_AC_ON_INPUT] & !LastInputState[HVAC_AC_ON_INPUT] ) {
      HVAC_SetHVACMode(HVAC_Cool);
   } else if (LastInputState[HVAC_HEAT_ON_INPUT] | LastInputState[HVAC_AC_ON_INPUT]) {
      HVAC_SetHVACMode(HVAC_Off);
   }

   HVAC_ReadAmbientTemperature();
}

#if DEMOCFG_ENABLE_SWITCH_TASK
void Switch_Task(uint32_t param)
{
 
   while (TRUE) {
      Switch_Poll();
      
      // Delay 50 ms 
      _time_delay(50);
   }
}
#endif
