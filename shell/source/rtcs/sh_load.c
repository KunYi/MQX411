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
*  Function Name :  Shell_load
*  Returned Value:  none
*  Comments  :  SHELL utility to load an executable file
*
*END*-----------------------------------------------------------------*/

int32_t  Shell_load(int32_t argc, char *argv[] )
{ /* Body */
   _ip_address          hostaddr;
   char                 hostname[MAX_HOSTNAMESIZE];
   char             *host_ptr, *file_ptr, *format_ptr;

   bool              print_usage, shorthelp = FALSE;
   int32_t               error = 0, return_code = SHELL_EXIT_SUCCESS;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc > 4 )  {
         printf("Error, %s invoked with incorrect number of arguments\n", argv[0]);
         print_usage = TRUE;
         return_code = SHELL_EXIT_ERROR;
      } else  {
         if (argc > 1)  {
            file_ptr = argv[1];
            if (argc > 2)  {
               host_ptr = argv[2];
               if (argc>3)  {
                  format_ptr = argv[3];
               } else  {
                  format_ptr = "srec";
               }
            } else  {
               printf("Error, no TFTP Server specified.\n");
               print_usage = TRUE;
               return_code = SHELL_EXIT_ERROR;
            }
         } else  {
            printf("Error, no bootfile specified.\n");
            print_usage = TRUE;
            return_code = SHELL_EXIT_ERROR;
         }
      }
      
      
      if (return_code != SHELL_EXIT_ERROR)  {
         RTCS_resolve_ip_address( host_ptr, &hostaddr, hostname, MAX_HOSTNAMESIZE ); 

         if (hostaddr)  {
            printf("Loading %s from %s [%ld.%ld.%ld.%ld]:\n", file_ptr, hostname, IPBYTES(hostaddr));
            if (strcmp(format_ptr, "srec") == 0)  {
               error = RTCS_load_TFTP_SREC(hostaddr, file_ptr);
            } else if (strcmp(format_ptr, "coff") == 0)  {
               error = RTCS_load_TFTP_BIN(hostaddr, file_ptr, NULL);
            } else {
               printf("Error, invalid format %s\n", format_ptr);
               print_usage = TRUE;
               return_code = SHELL_EXIT_ERROR;
            }
            if (error)  {
               printf("RTCS_load_TFTP_%s returned with error code = %x\n",format_ptr, error);
               return_code = SHELL_EXIT_ERROR;
            }
         } else  {
            printf("Unable to resolve host\n");
            return_code = SHELL_EXIT_ERROR;
         }
      }         
   }

   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <file> <host> <format>\n", argv[0]);
      } else  {
         printf("Usage: %s <file> <host> <format>\n", argv[0]);
         printf("   <file>   = filename\n");
         printf("   <host>   = TFTP Server ip address or name\n");
         printf("   <format> = file format, one of coff or srec\n");
      }
   }
   return return_code;
} /* Endbody */

#endif /* SHELLCFG_USES_RTCS */
/* EOF */
