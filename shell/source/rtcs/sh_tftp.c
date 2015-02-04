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
*  Function Name :  Shell_TFTP_client
*  Returned Value:  none
*  Comments  :  SHELL utility to TFTP to or from a host
*  Usage:  tftp host get source [destination] [mode] 
*
*END*-----------------------------------------------------------------*/

int32_t  Shell_TFTP_client(int32_t argc, char *argv[] )
{
   _ip_address          hostaddr;
   char                 hostname[MAX_HOSTNAMESIZE]="";
   char             *file_ptr;
   uint32_t              tftp_handle, buffer_size,byte_number;
   unsigned char            *buffer_ptr;
   TFTP_DATA_STRUCT     tftp_data;
   MQX_FILE_PTR         fd;
   int32_t               error;
   bool              print_usage, shorthelp = FALSE;
   int32_t               return_code = SHELL_EXIT_SUCCESS;
   char                 path[SHELL_MAX_FILELEN];  
   bool              trans = FALSE;
   
   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
   
      if ((argc >= 3) && (argc <= 5))  {
         RTCS_resolve_ip_address( argv[1], &hostaddr, hostname, MAX_HOSTNAMESIZE ); 

         if (!hostaddr)  {
            printf("Unable to resolve host.\n");
            return_code = SHELL_EXIT_ERROR;
         } else  {
            tftp_data.SERVER   = hostaddr;
            tftp_data.FILENAME = argv[2];
            tftp_data.FILEMODE = "netascii";
            if (argc > 3)  {
               file_ptr = argv[3];
               if (argc > 4) {
                  tftp_data.FILEMODE = argv[4];
               } else {
                  tftp_data.FILEMODE = "netascii";
               }
            } else {
               file_ptr = argv[2];
            }
#if SHELLCFG_USES_MFS  
            
            Shell_create_prefixed_filename(path, file_ptr, argv);
            fd = fopen(path,"a");
            if (fd)  {
               printf("\nDownloading file %s from TFTP server: %s [%ld.%ld.%ld.%ld]\n",
                  tftp_data.FILENAME,hostname, IPBYTES(hostaddr));
               tftp_handle = (*FT_TFTP->OPEN)( (void *) &tftp_data );
               if ( tftp_handle != RTCS_OK )  {
                  printf("\nError opening file %s\n",tftp_data.FILENAME);
                  return_code = SHELL_EXIT_ERROR;
               } else  {
                if (! (*FT_TFTP->EOFT)())  {
                   do {
                     buffer_ptr = (*FT_TFTP->READ)( &buffer_size );
                     if ((buffer_ptr != NULL) && (buffer_size))  {
                        _io_write(fd,buffer_ptr,buffer_size); 
                        trans = TRUE;
                     } else {
                   
                         switch (buffer_size) {
                         case 0:
                            // end of file
                           break;
                         case (RTCSERR_TFTP_ERROR + 1):
                            printf("\nFile %s not found\n", tftp_data.FILENAME);
                            break;
                         case (RTCSERR_TFTP_ERROR + 2):
                            printf("\nAccess violation\n");
                            break;
                         case (RTCSERR_TFTP_ERROR + 3):
                            printf("\nDisk full or allocation exceeded\n");
                            break;
                         case (RTCSERR_TFTP_ERROR + 4):
                            printf("\nIllegal TFTP operation\n");
                            break;
                         case (RTCSERR_TFTP_ERROR + 5):
                            printf("\nUnknown transfer ID\n");
                            break;
                         case (RTCSERR_TFTP_ERROR + 6):
                            printf("\nFile already exists\n");
                            break;
                         case (RTCSERR_TFTP_ERROR + 7):
                            printf("\nNo such user\n");
                            break;
                         default:
                            if(trans) 
                            {
                              trans =FALSE;
                              break;
                            }
                          else
                            printf("\nError reading file %s\n", tftp_data.FILENAME);
                         } /* Endswitch */
                        }
                     } while((buffer_ptr !=NULL) && buffer_size &&  (! (*FT_TFTP->EOFT)()) );
                     fclose(fd);
                  }
                  
                  error = (*FT_TFTP->CLOSE)();
                  
               }
               
            } else  {
               printf("\nError opening local file %s\n",file_ptr);
               return_code = SHELL_EXIT_ERROR;
            }
#else
            tftp_handle = (*FT_TFTP->OPEN)( (void *) &tftp_data );
            if ( tftp_handle != RTCS_OK )  {
               printf("\nError opening file %s\n",tftp_data.FILENAME);
               return_code = SHELL_EXIT_ERROR;
            } else  {
               printf("SHELLCFG_USES_MFS is not set to 1 in user_config.h - file wont be written to disk\n");
            }
            error = (*FT_TFTP->CLOSE)();
#endif            
         }
      } else  {
         printf("Error, %s invoked with incorrect number of arguments\n", argv[0]);
         print_usage = TRUE;
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <host> <source> [<dest>] [<mode>]\n", argv[0]);
      } else  {
         printf("Usage: %s <host> <source> [<dest>] [<mode>]\n", argv[0]);
         printf("   <host>   = host ip address or name\n");
         printf("   <source> = remote file name\n");
         printf("   <dest>   = local file name\n");
         printf("   <mode>   = file transfer mode (netascii, etc.)\n");
      }
   }
   return return_code;
} /* Endbody */

#endif /* SHELLCFG_USES_RTCS */

/* EOF */
