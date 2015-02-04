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

#if DEMOCFG_ENABLE_RTCS
#include <ipcfg.h>
#endif


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


#if DEMOCFG_ENABLE_KLOG
    #if !MQX_KERNEL_LOGGING
    #warning Need MQX_KERNEL_LOGGING enabled in kernel (user_config.h and user_config.cw)
    #endif
    #if !defined(DEMOCFG_KLOG_ADDR) || !defined(DEMOCFG_KLOG_SIZE)
    #warning Need klog address and size
    #endif
#endif


HVAC_STATE  HVAC_State =  {HVAC_Off};



void HVAC_Task(uint32_t param)
{
   HVAC_Mode_t mode;
   uint32_t counter = HVAC_LOG_CYCLE_IN_CONTROL_CYCLES;

   _int_install_unexpected_isr();

#if DEMOCFG_ENABLE_KLOG && MQX_KERNEL_LOGGING && defined(DEMOCFG_KLOG_ADDR) && defined(DEMOCFG_KLOG_SIZE)

   /* create kernel log */
   _klog_create_at(DEMOCFG_KLOG_SIZE, 0,(void *)DEMOCFG_KLOG_ADDR);
   
   /* Enable kernel logging */
   _klog_control(KLOG_ENABLED | KLOG_CONTEXT_ENABLED |
      KLOG_INTERRUPTS_ENABLED| 
      KLOG_FUNCTIONS_ENABLED|KLOG_RTCS_FUNCTIONS, TRUE);

   _klog_log_function(HVAC_Task);
#endif


#if DEMOCFG_ENABLE_RTCS
   HVAC_initialize_networking();
#endif


   // Initialize operatiing parameters to default values
   HVAC_InitializeParameters();

   // Configure and reset outputs
   HVAC_InitializeIO();
   _task_create(0, ALIVE_TASK, 0);

   #if DEMOCFG_ENABLE_AUTO_LOGGING
       LogInit();
       _time_delay (2000);
       Log("HVAC Started\n");
   #endif

   while (TRUE) {
      // Read current temperature
      HVAC_State.ActualTemperature = HVAC_GetAmbientTemperature();

      // Examine current parameters and set state accordingly
      HVAC_State.HVACState = HVAC_Off;
      HVAC_State.FanOn = FALSE;

      mode = HVAC_GetHVACMode();
      
      if (mode == HVAC_Cool || mode == HVAC_Auto)  
      {
         if (HVAC_State.ActualTemperature > (HVAC_Params.DesiredTemperature+HVAC_TEMP_TOLERANCE)) 
         {
            HVAC_State.HVACState = HVAC_Cool;
            HVAC_State.FanOn = TRUE;
         }
      } 
      
      if (mode == HVAC_Heat || mode == HVAC_Auto) 
      {
         if (HVAC_State.ActualTemperature < (HVAC_Params.DesiredTemperature-HVAC_TEMP_TOLERANCE)) 
         {
            HVAC_State.HVACState = HVAC_Heat;
            HVAC_State.FanOn = TRUE;
         }
      }

      if (HVAC_GetFanMode() == Fan_On) {
         HVAC_State.FanOn = TRUE;
      }         

      // Set outputs to reflect new state 
      HVAC_SetOutput(HVAC_FAN_OUTPUT, HVAC_State.FanOn);
      HVAC_SetOutput(HVAC_HEAT_OUTPUT,  HVAC_State.HVACState == HVAC_Heat);
      HVAC_SetOutput(HVAC_COOL_OUTPUT,  HVAC_State.HVACState == HVAC_Cool);

      // Log Current state
      if (++counter >= HVAC_LOG_CYCLE_IN_CONTROL_CYCLES) 
      {
         counter = 0;
         HVAC_LogCurrentState();
         
      }
      
      // Wait for a change in parameters, or a new cycle
      if (HVAC_WaitParameters(HVAC_CONTROL_CYCLE_IN_TICKS)) 
      {
         counter = HVAC_LOG_CYCLE_IN_CONTROL_CYCLES;
      }
      
#if DEMOCFG_ENABLE_RTCS
      ipcfg_task_poll ();
#endif
   }
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : HeartBeat_Task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

void HeartBeat_Task(uint32_t param)
{ 
    // initialize IO before starting this task

    _mqx_int delay, value = 0;

#if ENABLE_ADC
    HVAC_InitializeADC();
#endif // BSPCFG_ENABLE_ADC
    while (TRUE) {
#if ENABLE_ADC
        delay = 100 + (1000 * ReadADC() / 0x0fff);
#else
        delay = 100;
#endif // ENABLE_ADC

        _time_delay(delay);
        HVAC_SetOutput(HVAC_ALIVE_OUTPUT, value);
        
        value ^= 1;  // toggle next value
    }

}
