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
*   This file contains the source for an MFS shell function.
*
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <fio.h>
#include <mfs.h>
#include <shell.h>
#include <sh_prv.h>

#if SHELLCFG_USES_MFS

#define MAX_BUFF_SIZE 8192

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_write
* Returned Value   :  int32_t error code
* Comments  :  Reads from a file .
*
*END*---------------------------------------------------------------------*/

int32_t  Shell_write(int32_t argc, char *argv[] )
{ /* Body */
   bool           print_usage, shorthelp = FALSE;
   int32_t            return_code = SHELL_EXIT_SUCCESS;
   uint32_t           count=0;
   int32_t            offset=0;
   int32_t            seek_mode=0;
   char              c;
   MQX_FILE_PTR      fd = NULL;
   char          *abs_path;   
   bool           cache_enabled=TRUE;
   SHELL_CONTEXT_PTR    shell_ptr = Shell_get_context( argv );
   char             *buf = NULL;
   int32_t               writesize = MAX_BUFF_SIZE;
   int32_t               error = 0;
   _mqx_int             result;
   _mqx_int             bi;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if ((argc < 2) || (argc > 5)) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } 
      /* check if filesystem is mounted */ 
      else if (NULL == Shell_get_current_filesystem(argv))  
      {
         printf("Error, file system not mounted\n");
         return_code = SHELL_EXIT_ERROR;
      }
      else  {
         count = 0;
         offset = 0;
         seek_mode = IO_SEEK_CUR; 
         if (argc >= 3)  {
            if (! Shell_parse_uint_32(argv[2], &count ))  {
               printf("Error, invalid length\n");
               return_code = SHELL_EXIT_ERROR;
               print_usage=TRUE;
            } else  {
               if (argc >= 5)  {
                  if (strcmp(argv[3], "begin") == 0) {
                     seek_mode = IO_SEEK_SET;
                  } else if (strcmp(argv[3], "end") == 0) { 
                     seek_mode = IO_SEEK_END;
                  } else if (strcmp(argv[3], "current") == 0) { 
                     seek_mode = IO_SEEK_CUR;
                  } else { 
                     printf("Error, invalid seek type\n");
                     return_code = SHELL_EXIT_ERROR;
                     print_usage=TRUE;
                  }
                  
                  if (return_code == SHELL_EXIT_SUCCESS)  {
                     if (! Shell_parse_int_32(argv[4], &offset ))  {
                        printf("Error, invalid seek value\n");
                        return_code = SHELL_EXIT_ERROR;
                        print_usage=TRUE;
                     }
                  }
               }
            }
         }
      }
     
      if (return_code == SHELL_EXIT_SUCCESS)  {
         if (MFS_alloc_path(&abs_path) != MFS_NO_ERROR) {
            printf("Error, unable to allocate memory for paths\n" );
            return_code = SHELL_EXIT_ERROR;
         } else {
            error = _io_rel2abs(abs_path,shell_ptr->CURRENT_DIR,(char *) argv[1],PATHNAME_SIZE,shell_ptr->CURRENT_DEVICE_NAME);
             
            do {
               buf = _mem_alloc(writesize);
               if (buf != NULL) break;
               else writesize >>= 1;
            } while (writesize > 0);
             
            if (buf != NULL)
            {   
               if(!error)
               {
                  fd = fopen(abs_path, "a");
               }
               if (fd && !error) {
                  if (fseek(fd, offset, seek_mode ) != IO_ERROR) {
                   
                     c = '0';
                     while (count) {

                         /* generate data to buf */
                         for (bi = 0; (bi < count) && (bi < writesize); bi++) {
                             buf[bi] = c;
                             c = (c == 'z') ? '0' : c+1;
                         }

                         result = write(fd, buf, bi);

                         if (result != bi) {
                             /* incomplete write */
                             printf("Error writing file %s.\n", argv[1] );
                             error = ferror(fd);
                             if (error == MFS_DISK_FULL)
                                 printf("Disk full.\n" );
                             return_code = SHELL_EXIT_ERROR;
                             break;
                         }

                         count -= result;
                     }                    
                     
                  } else {
                     printf("Error, unable to seek file %s.\n", argv[1] );
                     return_code = SHELL_EXIT_ERROR;
                  }

                  fclose(fd);

               } else  {
                  printf("Error, unable to open file %s.\n", argv[1] );
                  return_code = SHELL_EXIT_ERROR;
               }
               MFS_free_path(abs_path);
               _mem_free(buf);
            }
            else {
               printf("Error, unable to allocate memory for write buffer\n");
               return_code = SHELL_EXIT_ERROR;
            }
         }
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <filename> <bytes> [<seek_mode>] [<offset>]\n", argv[0]);
      } else  {
         printf("Usage: %s <filename> <bytes> [<seek_mode>] [<offset>]\n", argv[0]);
         printf("   <filename>   = filename to write\n");
         printf("   <bytes>      = number of bytes to write\n");
         printf("   <seek_mode>  = one of: begin, end or current\n");
         printf("   <offset>     = seek offset\n");
      }
   }
   return return_code;
} /* Endbody */

#endif //SHELLCFG_USES_MFS
/* EOF */
