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
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   Shell_rmdir
* Returned Value   :  int32_t error code
* Comments  :  mount a filesystem on a device.
*
* Usage:  rmdir <directory>
*
*END*---------------------------------------------------------------------*/

int32_t  Shell_rmdir(int32_t argc, char *argv[] )
{ /* Body */
   bool                    print_usage, temp, shorthelp = FALSE;
   int32_t                     error = 0, return_code = SHELL_EXIT_SUCCESS;
   MQX_FILE_PTR               fs_ptr;
   SHELL_CONTEXT_PTR          shell_ptr = Shell_get_context( argv );
   char                   *abs_path = NULL;      
   
   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc !=  2) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage=TRUE;
      } else  {
         if (MFS_alloc_path(&abs_path) != MFS_NO_ERROR) {
            printf("Error, unable to allocate memory for paths\n" );
            return_code = SHELL_EXIT_ERROR;
         }
         else
         {
            _io_get_dev_for_path(abs_path,&temp,PATHNAME_SIZE,(char *)argv[1],Shell_get_current_filesystem_name(shell_ptr));
            fs_ptr = _io_get_fs_by_name(abs_path);
            if (fs_ptr == NULL)  {
               printf("Error, file system not mounted\n"  );
               return_code = SHELL_EXIT_ERROR;
            } else {
               error = _io_rel2abs(abs_path,shell_ptr->CURRENT_DIR,(char *) argv[1],PATHNAME_SIZE,Shell_get_current_filesystem_name(shell_ptr));
               if(!error)
               {            
                  // change mfs internal current dir (avoid deleting current dir problems)
                  error = ioctl(fs_ptr, IO_IOCTL_CHANGE_CURRENT_DIR, shell_ptr->CURRENT_DIR);
                  error = ioctl(fs_ptr, IO_IOCTL_REMOVE_SUBDIR, (void *) abs_path);
                  if (error)  {
                     printf("Error removing directory %s\n", abs_path);
                  }
               }
               else
               {
                  printf("Invalid path %s\n", argv[1]);
               }
            }
            MFS_free_path(abs_path);
         }
      }
   }
      
   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <directory> \n", argv[0]);
      } else  {
         printf("Usage: %s <directory>\n", argv[0]);
         printf("   <directory> = name of directory to remove\n");
      }
   }
   return return_code;
}
#endif //SHELLCFG_USES_MFS
