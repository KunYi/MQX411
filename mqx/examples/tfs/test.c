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
*   Test application for trivial file system (TFS)
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <tfs.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


extern const TFS_DIR_ENTRY tfs_data[];


extern void main_task(uint32_t);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   /* Task Index, Function,     Stack, Priority,  Name,     Attributes,          Param, Time Slice */
    { 10L,        main_task,    1400L, 8L,        "Main",   MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};

/*TASK*-------------------------------------------------------------------
* 
* Task Name : main_task
* Comments  :
*
*END*----------------------------------------------------------------------*/


void main_task
   (
      uint32_t dummy
   )
{
   MQX_FILE_PTR                 file1;
   MQX_FILE_PTR                 file2;
   int32_t                       result;
   int32_t                       i;
   unsigned char                        buffer[20];
   uint32_t                      param;
   unsigned char                    *name;
     
   result = _io_tfs_install ("tfs:", tfs_data);
   printf ("\ninstall returned: %08x", result); 
     
   file1 = fopen("tfs:\\INDEX.html", "r");
   printf ("\nfopen1 returned: %p", file1); 

   file2 = fopen("tfs:/", "r");
   printf ("\nfopen2 returned: %p", file2); 

   file2 = fopen("tfs:/readme.txt", "r");
   printf ("\nfopen2 returned: %p", file2); 

   result = fread(buffer, 1, 20, file1);
   printf ("\nfread1 returned: %d and\n", result);
   for (i=0;i<result;i++) printf ("%c",buffer[i]);

   result = ioctl(file1, IO_IOCTL_TFS_GET_ATTRIBUTES, &param);
   printf ("\nioctl1 get flags returned: %08x and %08x", result, param);

   result = ioctl(file2, IO_IOCTL_TFS_GET_NAME, &name);
   printf ("\nioctl2 get name returned: %08x and %s", result, name);

   result = ioctl(file2, IO_IOCTL_TFS_GET_LENGTH, &param);
   printf ("\nioctl2 get length returned: %08x and %d", result, param);

   result = fwrite(buffer, 1, 10, file2);
   printf ("\nfwrite2 returned: %d", result);

   result = ioctl(file2, IO_IOCTL_TFS_GET_LAST_ERROR, NULL);
   printf ("\nioctl2 get last error returned: %08x", result);

   result = fclose(file1);
   printf ("\nfclose1 returned: %d", result);

   result = fread(buffer, 1, 10, file2);
   printf ("\nfread2 returned: %d and ", result);
   for (i=0;i<result;i++) printf ("%c",buffer[i]);

   result = fread(buffer, 1, 10, file2);
   printf ("\nfread2 returned: %d and ", result);
   for (i=0;i<result;i++) printf ("%c",buffer[i]);

   result = fseek (file2, 0, IO_SEEK_END);
   printf ("\nfseek2 to end returned: %08x", result);

   result = fread(buffer, 1, 10, file2);
   printf ("\nfread2 returned: %d and ", result);
   for (i=0;i<result;i++) printf ("%c",buffer[i]);

   result = fseek (file2, 0, IO_SEEK_SET);
   printf ("\nfseek2 to start returned: %08x", result);

   printf ("\nfread2 complete file:\n");
   do
   {
        result = fread(buffer, 1, 4, file2);
        for (i=0;i<result;i++) printf ("%c",buffer[i]);
   } while (result != 0);

   result = fclose(file2);
   printf ("\nfclose2 returned: %d", result);

   _task_block();
 
} /* Endbody */

/* EOF */
