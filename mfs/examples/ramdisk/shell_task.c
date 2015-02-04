/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
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
Provide MFS file system on external RAM
*
*   
*
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include "mfs.h"
#include <shell.h>

#if ! SHELLCFG_USES_MFS
#error This application requires SHELLCFG_USES_MFS defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

void Shell_Task(uint32_t);
void Ram_disk_start(void);

/* The MRam disk base address definition */
#if defined (BSP_EXTERNAL_MRAM_RAM_BASE) && defined (BSP_EXTERNAL_MRAM_RAM_SIZE)
    #warning Please check that the MRAM memory is available on your EVB (Tower). 
    #define RAM_DISK_BASE   BSP_EXTERNAL_MRAM_RAM_BASE 
    #define RAM_DISK_SIZE   BSP_EXTERNAL_MRAM_RAM_SIZE
#else 
    #warning Modify the RAM_DISK_SIZE if you need more or less ramdisk space. 
    #define RAM_DISK_SIZE   0x1800
    static unsigned char   ram_disk[RAM_DISK_SIZE];
    #define RAM_DISK_BASE   &ram_disk[0]
#endif

const SHELL_COMMAND_STRUCT Shell_commands[] = {   
   { "cd",        Shell_cd },      
   { "copy",      Shell_copy },
   { "create",    Shell_create },
   { "del",       Shell_del },       
   { "disect",    Shell_disect},      
   { "dir",       Shell_dir },      
   { "df",        Shell_df },      
   { "exit",      Shell_exit }, 
   { "format",    Shell_format },
   { "help",      Shell_help }, 
   { "mkdir",     Shell_mkdir },     
   { "pwd",       Shell_pwd },       
   { "read",      Shell_read },      
   { "ren",       Shell_rename },    
   { "rmdir",     Shell_rmdir },
   { "sh",        Shell_sh },
   { "type",      Shell_type },
   { "write",     Shell_write },     
   { "?",         Shell_command_list },     
   
   { NULL,        NULL } 
};

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_Task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void Shell_Task(uint32_t temp)
{ 
   (void)temp; /* suppress 'unused variable' warning */

   /* Run the shell on the serial port */
   printf("Demo start\n\r");
   Ram_disk_start();
   for(;;)  {
      Shell(Shell_commands, NULL);
      printf("Shell exited, restarting...\n");
   }
}
 
/*TASK*-----------------------------------------------------
* 
* Task Name    : Ram_disk_start
* Comments     : Open device and install MFS on device
*    
*
*END*-----------------------------------------------------*/
void Ram_disk_start(void)
{ /* Body */
   MQX_FILE_PTR               dev_handle1,
                              a_fd_ptr;
   int32_t                     error_code;
   _mqx_uint                  mqx_status;

   a_fd_ptr = 0;

   /* install device */
   mqx_status = _io_mem_install("mfs_ramdisk:", (unsigned char *)RAM_DISK_BASE,
      (_file_size)RAM_DISK_SIZE);
   /* Number of sectors is returned by ioctl IO_IOCTL_GET_NUM_SECTORS function */
   /* If another disc structure is desired, use MFS_FORMAT_DATA structure to   */
   /* define it and call standart format function instead default_format       */   
   if ( mqx_status != MQX_OK ) {
      printf("\nError installing memory device (0x%x)", mqx_status);
      _task_block();
   } /* Endif */

   /* Open the device which MFS will be installed on */
   dev_handle1 = fopen("mfs_ramdisk:", "b");
   if ( dev_handle1 == NULL ) {
      printf("\nUnable to open Ramdisk device");
      _task_block();
   } /* Endif */

   /* Install MFS  */
   mqx_status = _io_mfs_install(dev_handle1, "a:", (_file_size)0);
   if (mqx_status != MFS_NO_ERROR) {
      printf("\nError initializing a:");
      _task_block();
   } else {
      printf("\nInitialized Ram Disk to a:\\");
   } /* Endif */

   /* Open the filesystem and format detect, if format is required */
   a_fd_ptr = fopen("a:", NULL);
   error_code    = ferror(a_fd_ptr);
   if ((error_code != MFS_NO_ERROR) && (error_code != MFS_NOT_A_DOS_DISK))
   {
      printf("\nError while opening a:\\ (%s)", MFS_Error_text((uint32_t)error_code));
      _task_block();
   } /* Endif */
   if ( error_code == MFS_NOT_A_DOS_DISK ) {
      printf("\nNOT A DOS DISK! You must format to continue.");
   } /* Endif */
    
} /* Endbody */ 

/* EOF */
