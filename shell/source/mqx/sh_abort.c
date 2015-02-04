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
*   This file contains the abort command.
*
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>
#include "shell.h"



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_abort
*  Returned Value:  none
*  Comments  :  SHELL utility to abort a task
*  Usage:   
*
*END*-----------------------------------------------------------------*/

int32_t  Shell_abort(int32_t argc, char *argv[] )
{
   bool     print_usage, shorthelp = FALSE;
   int32_t      return_code = SHELL_EXIT_SUCCESS;
   _task_id    task_id;
   uint32_t     result;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      
      if (argc == 2)  {
         task_id = _task_get_id_from_name( argv[1] );
         if (task_id == MQX_NULL_TASK_ID)  {
            printf("No task named %s running.\n",argv[1]);
            return_code = SHELL_EXIT_ERROR;
         } else  {
            result = _task_abort(task_id);
            if (result == MQX_OK)  {  
               printf("Task %s aborted.\n",argv[1]);
            } else  {
               printf("Unable to abort task %s.\n",argv[1]);
               return_code = SHELL_EXIT_ERROR;
            }
         }
      } else  {
         printf("Error, %s invoked with incorrect number of arguments\n", argv[0]);
         print_usage = TRUE;
      }
   }

   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <taskname>\n", argv[0]);
      } else  {
         printf("Usage: %s <taskname>\n", argv[0]);
         printf("   <taskname> = MQX Task name\n");
      }
   }

   return return_code;
} /* Endbody */

/* EOF */
