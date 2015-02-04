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
*   Example using RTCS Library.
*
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <mfs.h>
#include <rtcs.h>
#include <shell.h>
#include "config.h"  
#include <fio.h>

#if DEMOCFG_ENABLE_PPP
#include <ppp.h>

#if ! RTCSCFG_ENABLE_VIRTUAL_ROUTES
#error This application requires RTCSCFG_ENABLE_VIRTUAL_ROUTES defined non-zero in user_config.h. Please recompile libraries (BSP, RTCS) with this option.
#endif

#if ! RTCSCFG_ENABLE_GATEWAYS
#error This application requires RTCSCFG_ENABLE_GATEWAYS defined non-zero in user_config.h. Please recompile libraries (BSP, RTCS) with this option.
#endif

#warning This application requires PPP device to be defined manually and being different from the default IO channel (BSP_DEFAULT_IO_CHANNEL). See PPP_DEVICE in config.h.

#warning This application requires PPP device QUEUE_SIZE  to be more than size of input LCP packet (set BSPCFG_UARTX_QUEUE_SIZE to 128 in user_config.h) If no, you can lose LCP packets.

#if PPP_SECRETS_NOT_SHARED
#error This application requires PPP_SECRETS_NOT_SHARED defined to zero in /src/rtcs/source/include/ppp.h. Please recompile RTCS with this option.
#endif

#endif /* DEMOCFG_ENABLE_PPP */


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#define MAIN_TASK 1

extern const SHELL_COMMAND_STRUCT Shell_commands[];
extern void Main_task (uint32_t);
extern void Ram_disk_start(void);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   /* Task Index,   Function,   Stack,  Priority,   Name,   Attributes,          Param, Time Slice */
    { MAIN_TASK,    Main_task,  2500,   9,          "Main", MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};

#if DEMOCFG_ENABLE_RAMDISK

#if (DEMOCFG_ENABLE_MRAM_RAMDISK) /* MRAM RAM disk enabled */

/* The MRam disk base address definition taken from linker command file*/
#if defined (BSP_EXTERNAL_MRAM_RAM_BASE) && defined (BSP_EXTERNAL_MRAM_RAM_SIZE)
    #define RAM_DISK_BASE   BSP_EXTERNAL_MRAM_RAM_BASE 
    #define RAM_DISK_SIZE   BSP_EXTERNAL_MRAM_RAM_SIZE
#else
   #error MRAM RAM disk needs space to be defined
#endif

#endif /* MRAM RAM disk enabled */
/*FUNCTION*------------------------------------------------
* 
* Function Name: Ram_disk_start
* Comments     :
*    
*
*END*-----------------------------------------------------*/
void Ram_disk_start(void)
{
    MQX_FILE_PTR               dev_handle1, a_fd_ptr;
    int32_t                     error_code;
    _mqx_uint                  mqx_status;
    uint32_t                    size;
    unsigned char                  *base;
    a_fd_ptr = 0;
#if DEMOCFG_ENABLE_MRAM_RAMDISK /* MRAM RAM disk enabled */
    size = RAM_DISK_SIZE;
    base = RAM_DISK_BASE;
#else 
    
	#if 1 /* Constant size 8KB.*/
		
		size = 8*1024;
		
	#else    /* Determine RAMDISK size  - start on 128 KiB and go down to 16 KiB */
		
		for (size = 128*1024; size > 16*1024; size /= 2)
		{
			base = (unsigned char *) _mem_alloc(size);
			if (base != NULL)
			{
				_mem_free(base);
				base = NULL;
				break;
			}
		}
		/* If memory block is big half its size, otherwise leave it on 8 KiB */
		if (size > 8*1024)
		{
			size /= 2;
		}
		
	#endif    
		
    base = (unsigned char *) _mem_alloc(size);
    if (base == NULL)
    {
        printf("\nError - unable to allocate space for RAM disk - NULL pointer");
        _task_block();
    }
#endif /* MRAM RAM disk enabled */
    
    /* Install device */
    mqx_status = _io_mem_install("mfs_ramdisk:", (unsigned char *)base, (_file_size)size);
    if ( mqx_status != MQX_OK ) 
    {
        printf("\nError installing memory device (0x%x)", mqx_status);
        _task_block();
    }

    /* Open the device which MFS will be installed on */
    dev_handle1 = fopen("mfs_ramdisk:", 0);
    if ( dev_handle1 == NULL ) 
    {
        printf("\nUnable to open Ramdisk device");
        _task_block();
    }

    /* Install MFS  */
    mqx_status = _io_mfs_install(dev_handle1, "a:", (_file_size)0);
    if (mqx_status != MFS_NO_ERROR) 
    {
        printf("\nError initializing a:");
        _task_block();
    } 
    else 
    {
        printf("\nInitialized Ram Disk to a:\\");
    }

    /* Open the filesystem and detect, if format is required */
    a_fd_ptr = fopen("a:", NULL);
    error_code = ferror(a_fd_ptr);
    if ((error_code != MFS_NO_ERROR) && (error_code != MFS_NOT_A_DOS_DISK))
    {
        printf("\nError while opening a:\\ (%s)", MFS_Error_text((uint32_t)(uint32_t)error_code));
        _task_block();
    }
    if (error_code == MFS_NOT_A_DOS_DISK) 
    {
        printf("\nNOT A DOS DISK! You must format to continue.");
    }
    
} 

#endif /* DEMOCFG_ENABLE_RAMDISK */

/*TASK*-----------------------------------------------------------------
*
* Task Name      : Main_task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void Main_task(uint32_t temp)
{
    uint32_t     error;

   /* runtime RTCS configuration for devices with small RAM, for others the default BSP setting is used */
   _RTCSPCB_init = 4;
   _RTCSPCB_grow = 2;
   _RTCSPCB_max = 20;
   _RTCS_msgpool_init = 4;
   _RTCS_msgpool_grow = 2;
   _RTCS_msgpool_max  = 20;
   _RTCS_socket_part_init = 4;
   _RTCS_socket_part_grow = 2;
   _RTCS_socket_part_max  = 20;

    _RTCSTASK_stacksize = 3000;
    error = RTCS_create();
    if (error != RTCS_OK) 
    {
        printf("\nRTCS failed to initialize, error = %X", error);
        _task_block();
    }
    /* Enable IP forwarding */
    _IP_forward = TRUE;
        
#if DEMOCFG_ENABLE_RAMDISK  
    Ram_disk_start();
#endif /* DEMOCFG_ENABLE_RAMDISK */

    /* Run the shell */
    Shell(Shell_commands, NULL);
    for(;;) 
    {
        printf("Shell exited, restarting...\n");
        Shell(Shell_commands, NULL);
    }
}
/* EOF */
