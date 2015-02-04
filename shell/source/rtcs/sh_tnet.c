/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
* Copyright 1989-2008 ARC International
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
*   This file contains the RTCS shell.
*
*
*END************************************************************************/

#include <ctype.h>
#include <string.h>
#include <mqx.h>
#include "shell.h"
#if SHELLCFG_USES_RTCS

#include <rtcs.h>
#include "sh_rtcs.h"




/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_telnet
*  Returned Value:  none
*  Comments  :  SHELL utility to telnet to a host
*
*END*-----------------------------------------------------------------*/

int32_t  Shell_Telnet_client(int32_t argc, char *argv[] )
{ /* Body */
   _ip_address    hostaddr;
   char           hostname[MAX_HOSTNAMESIZE];
   int32_t         error;
   bool              print_usage, shorthelp = FALSE;
   int32_t               return_code = SHELL_EXIT_SUCCESS;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {

      if (argc == 2)  {
         RTCS_resolve_ip_address( argv[1], &hostaddr, hostname, MAX_HOSTNAMESIZE ); 

         if (hostaddr)  {
            printf("Connecting to %s [%ld.%ld.%ld.%ld]:\n", hostname, IPBYTES(hostaddr));

            error = TELNET_connect(hostaddr);
            if (error) {
               printf("Connection failed, error 0x%04lX\n", error);
               return_code = SHELL_EXIT_ERROR;
            } 
            printf("Connection closed\n", error);
         } else  {
            printf("Unable to resolve host\n");
            return_code = SHELL_EXIT_ERROR;
         }
      } else  {
         printf("Error, %s invoked with incorrect number of arguments\n", argv[0]);
         print_usage = TRUE;
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <host>\n", argv[0]);
      } else  {
         printf("Usage: %s  <host>\n", argv[0]);
         printf("   <host>   = host ip address or name\n");
      }
   }
   return return_code;
} /* Endbody */

#endif /* SHELLCFG_USES_RTCS */         
/* EOF */
