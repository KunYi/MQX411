/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   Shell RTCS_get_route implementation.
*
*
*END************************************************************************/

#include <mqx.h>
#include "shell.h"
#if SHELLCFG_USES_RTCS

#include <rtcs.h>
#include "sh_rtcs.h"



/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_getroute
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

int32_t Shell_getroute(int32_t argc, char *argv[] )
{ /* Body */
   bool           print_usage, shorthelp = FALSE;
   int32_t            return_code = SHELL_EXIT_SUCCESS;

   _ip_address       gate_ipaddr = 0;
   _ip_address       ipaddr = 0;
   _ip_address       ipmask = 0;
  
   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if ((argc < 2) || (argc > 3)) {
         return_code = SHELL_EXIT_ERROR;
         print_usage = TRUE;
      } else  {
         if (! Shell_parse_ip_address( argv[1], &ipaddr  ))  {
            printf("Error, invalid ip address\n");
            return_code = SHELL_EXIT_ERROR;
         } else if (argc == 3)  {
            if (! Shell_parse_ip_address( argv[2], &ipmask  ))  {
               printf("Error, invalid ip mask\n");
               return_code = SHELL_EXIT_ERROR;
            }
         }          
      }          
         
         
      if (return_code == SHELL_EXIT_SUCCESS)  {
         gate_ipaddr = RTCS_get_route(ipaddr, ipmask);
         printf("The route to ip address: %d.%d.%d.%d, netmask: %d.%d.%d.%d is gateway %d.%d.%d.%d\n",
            IPBYTES(ipaddr),IPBYTES(ipmask),IPBYTES(gate_ipaddr));
      } 
   }

   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <ipaddr> <netmask>\n", argv[0]);
      } else  {
         printf("Usage: %s <ipaddr> <netmask>\n", argv[0]);
         printf("   <ipaddr>  = IP address \n");
         printf("   <netmask> = Network mask\n");
      }
   }

   return return_code;
} /* Endbody */

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_getroute
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

int32_t Shell_walkroute(int32_t argc, char *argv[] )
{ /* Body */
   bool           print_usage, shorthelp = FALSE;
   int32_t            return_code = SHELL_EXIT_SUCCESS;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc >1)  {
         return_code = SHELL_EXIT_ERROR;
         print_usage = TRUE;
      }          
         
         
      if (return_code == SHELL_EXIT_SUCCESS)  {
         RTCS_walk_route();
      } 
   }

   if (print_usage)  {
      if (shorthelp)  {
         printf("%s\n", argv[0]);
      } else  {
         printf("Usage: %s\n", argv[0]);
      }
   }

   return return_code;
} /* Endbody */

#endif /* SHELLCFG_USES_RTCS */

/* EOF */

