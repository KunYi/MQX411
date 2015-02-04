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
#include <string.h>
#include <shell.h>

#include "hvac_public.h"
#include "hvac_shell_commands.h"



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_temp
* Returned Value   :  int32_t error code
* Comments  :  Reads from a file .
*
*END*---------------------------------------------------------------------*/

int32_t  Shell_temp(int32_t argc, char *argv[] )
{
   bool           print_usage, shorthelp = FALSE;
   int32_t            return_code = SHELL_EXIT_SUCCESS;
   uint32_t           temp,temp_fract;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc > 2) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else {
         if (argc == 2) {
            temp_fract = 0;
            if (sscanf(argv[1],"%d.%d",&temp,&temp_fract)>=1) {
               if (temp_fract<10) {
                  HVAC_SetDesiredTemperature(temp*10+temp_fract);
               } else {
                  printf("Invalid temperature specified, format is dd.d\n");
               }
            } else {
               printf("Invalid temperature specified\n");
            } 
         }

         temp  = HVAC_GetDesiredTemperature();
         printf("Desired Temperature is %d.%1d %c\n", temp/10, temp%10, HVAC_GetTemperatureSymbol());
         temp  = HVAC_GetActualTemperature();
         printf("Actual Temperature is %d.%1d %c\n", temp/10, temp%10, HVAC_GetTemperatureSymbol());
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s [<temperature>]\n", argv[0]);
      } else  {
         printf("Usage: %s [<temperature>]\n", argv[0]);
         printf("   <temperature> = desired temperature (degrees)\n");
      }
   }
   return return_code;
} 


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_scale
* Returned Value   :  int32_t error code
* Comments  :  Reads from a file .
*
*END*---------------------------------------------------------------------*/

int32_t  Shell_scale(int32_t argc, char *argv[] )
{
   bool           print_usage, shorthelp = FALSE;
   int32_t            return_code = SHELL_EXIT_SUCCESS;
   uint32_t           temp;
 
   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc > 2) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else {
         if (argc == 2) {
            if (*argv[1] == 'f') {
               HVAC_SetTemperatureScale(Fahrenheit);
            } else if (*argv[1] == 'c') {
               HVAC_SetTemperatureScale(Celsius);
            } else {
               printf("Invalid temperature scale\n");
            } 
         }

         if (HVAC_GetTemperatureScale() == Fahrenheit) {
            printf("Temperature scale is Fahrenheit\n");
         } else {
            printf("Temperature scale is Celsius\n");
         }         
         temp  = HVAC_GetDesiredTemperature();
         printf("Desired Temperature is %d.%1d %c\n", temp/10, temp%10, HVAC_GetTemperatureSymbol());
         temp  = HVAC_GetActualTemperature();
         printf("Actual Temperature is %d.%1d %c\n", temp/10, temp%10, HVAC_GetTemperatureSymbol());
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s [c | f]\n", argv[0]);
      } else  {
         printf("Usage: %s [c | f]\n", argv[0]);
         printf("   c = switch to Celsius scale\n");
         printf("   f = switch to Fahrenheit scale\n");
      }
   }
   return return_code;
} 

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_fan
* Returned Value   :  int32_t error code
* Comments  :  Reads from a file .
*
*END*---------------------------------------------------------------------*/

int32_t  Shell_fan(int32_t argc, char *argv[] )
{
   bool           print_usage, shorthelp = FALSE;
   int32_t            return_code = SHELL_EXIT_SUCCESS;
   FAN_Mode_t        fan;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc > 2) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else {
         if (argc == 2) {
            if (strcmp(argv[1],"on")==0) {
               HVAC_SetFanMode(Fan_On);
            } else if (strcmp(argv[1],"off")==0) {
               HVAC_SetFanMode(Fan_Automatic);
            } else {
               printf("Invalid fan mode specified\n");
            } 
         }

         fan  = HVAC_GetFanMode();
         printf("Fan mode is %s\n", fan == Fan_Automatic ? "Automatic" : "On");
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s [<mode>]\n", argv[0]);
      } else  {
         printf("Usage: %s [<mode>]\n", argv[0]);
         printf("   <mode> = on or off (off = automatic mode)\n");
      }
   }
   return return_code;
} 

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_hvac
* Returned Value   :  int32_t error code
* Comments  :  Reads from a file .
*
*END*---------------------------------------------------------------------*/

int32_t  Shell_hvac(int32_t argc, char *argv[] )
{
   bool           print_usage, shorthelp = FALSE;
   int32_t            return_code = SHELL_EXIT_SUCCESS;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc > 2) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else {
         if (argc == 2) {
            if (strcmp(argv[1],"off")==0) {
               HVAC_SetHVACMode(HVAC_Off);
            } else if (strcmp(argv[1],"cool")==0) {
               HVAC_SetHVACMode(HVAC_Cool);
            } else if (strcmp(argv[1],"heat")==0) {
               HVAC_SetHVACMode(HVAC_Heat);
            } else if (strcmp(argv[1],"auto")==0) {
               HVAC_SetHVACMode(HVAC_Auto);
            } else {
               printf("Invalid hvac mode specified\n");
            } 
         }

         printf("HVAC mode is %s\n", HVAC_HVACModeName(HVAC_GetHVACMode()));
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s [<mode>]\n", argv[0]);
      } else  {
         printf("Usage: %s [<mode>]\n", argv[0]);
         printf("   <mode> = off, cool, heat or auto\n");
      }
   }
   return return_code;
} 

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_info
* Returned Value   :  int32_t error code
* Comments  :  Reads from a file .
*
*END*---------------------------------------------------------------------*/

int32_t  Shell_info(int32_t argc, char *argv[] )
{
   bool           print_usage, shorthelp = FALSE;
   int32_t            return_code = SHELL_EXIT_SUCCESS;
   uint32_t           temp;
   FAN_Mode_t        fan;
   HVAC_Output_t     output;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc > 1) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else {
         printf("HVAC mode:    %s\n",  HVAC_HVACModeName(HVAC_GetHVACMode()));
         temp  = HVAC_GetDesiredTemperature();
         printf("Desired Temp: %d.%1d %c\n", temp/10, temp%10, HVAC_GetTemperatureSymbol());
         temp  = HVAC_GetActualTemperature();
         printf("Actual Temp:  %d.%1d %c\n", temp/10, temp%10, HVAC_GetTemperatureSymbol());
         fan  = HVAC_GetFanMode();
         printf("Fan mode:     %s\n", fan == Fan_Automatic ? "Automatic" : "On");

         for(output=(HVAC_Output_t)0;output<HVAC_MAX_OUTPUTS;output++) {
            printf("%12s: %s\n", HVAC_GetOutputName(output), HVAC_GetOutput(output) ? "On" : "Off");
         }
       }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s \n", argv[0]);
      } else  {
         printf("Usage: %s \n", argv[0]);
      }
   }
   return return_code;
} 

#if DEMOCFG_ENABLE_USB_FILESYSTEM

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_log
*  Returned Value:  none
*  Comments  :  SHELL utility to type a file
*
*END*-----------------------------------------------------------------*/

int32_t  Shell_log(int32_t argc, char *argv[] )
{ /* Body */
   bool              print_usage, shorthelp = FALSE;
   int32_t               return_code = SHELL_EXIT_SUCCESS;
   MQX_FILE_PTR         fd;
   _mqx_int             c;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc == 1)  {
         
         fd = fopen(LOG_FILE, "r");
         if (fd)  {
            while ((c = fgetc(fd)) >= 0) {
               fputc(c, stdout);
            } 
            fclose(fd);
            printf("\n");
         } else  {
            printf("Error, unable to open file %s.\n", LOG_FILE );
            return_code = SHELL_EXIT_ERROR;
         }
      } else  {
         printf("Error, %s invoked with incorrect number of arguments\n", argv[0]);
         return_code = SHELL_EXIT_ERROR;
         print_usage = TRUE;
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s \n", argv[0]);
      } else  {
         printf("Usage: %s \n", argv[0]);
      }
   }
   return return_code;
} /* Endbody */

#endif
/* EOF*/
