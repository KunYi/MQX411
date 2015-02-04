/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains the source for the I/O example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <string.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#if ! BSPCFG_ENABLE_TTYA
#error This application requires BSPCFG_ENABLE_TTYA defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#if ! BSPCFG_ENABLE_TTYB
#error This application requires BSPCFG_ENABLE_TTYB defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


extern void main_task(uint32_t);


const TASK_TEMPLATE_STRUCT MQX_template_list[] = 
{
   /* Task Index,   Function,   Stack,  Priority, Name,   Attributes,          Param, Time Slice */
    { 10,           main_task,  2000,   8,        "Main", MQX_AUTO_START_TASK, 0,     0},
    { 0 }
};   

/*TASK*-----------------------------------------------------
* 
* Task Name    : main_task
* Comments     :
*    This task opens a second I/O channel and prints a message
* to it.
*
*END*-----------------------------------------------------*/

void main_task
   (
      uint32_t initial_data
   )
{
   void   *fh_ptr;

   printf("This is printed to the default device\n");
   fflush(stdout);

   if (!strcmp("ttya:", BSP_DEFAULT_IO_CHANNEL))
     fh_ptr = (void *)fopen("ttyb:", BSP_DEFAULT_IO_OPEN_MODE);
   else
     fh_ptr = (void *)fopen("ttya:", BSP_DEFAULT_IO_OPEN_MODE);
   if (fh_ptr == NULL) {
      printf("Cannot open the other IO device\n");
   } else {
      _io_set_handle(IO_STDOUT, fh_ptr);
      printf("This is printed to the other device\n");
   }
   
   fflush(stdout);
   if (fh_ptr != NULL)
      fclose(fh_ptr);
   
#if defined(BSPCFG_ENABLE_IODEBUG) && BSPCFG_ENABLE_IODEBUG 
   if(NULL == (fh_ptr = fopen("iodebug:", NULL))) { 
      printf("Cannot open the debug output\n"); 
   } else { 
      _io_set_handle(IO_STDOUT, fh_ptr); 
      printf("This is printed to the debug output\n"); 
   }
   
   fflush(stdout);
   if (fh_ptr != NULL)
      fclose(fh_ptr);
#endif
   
   _task_block();   

}

/* EOF */
