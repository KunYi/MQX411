/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
Provide MFS file system on internal NAND Flash memory.
*
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <mfs.h>
#include <shell.h>
#include <part_mgr.h>
#include "nandflash_wl_ffs.h"

#if ! SHELLCFG_USES_MFS
#error This application requires SHELLCFG_USES_MFS defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

extern const NANDFLASH_WL_INIT_STRUCT _bsp_nandflash_wl_init;

void shell_task(uint32_t);

int32_t Shell_ffs_open(int32_t argc, char * argv[] );
int32_t Shell_ffs_close(int32_t argc, char * argv[] );
int32_t Shell_ffs_flush(int32_t argc, char * argv[] );
int32_t Shell_ffs_erase_drive(int32_t argc, char * argv[] );
int32_t Shell_ffs_repair(int32_t argc, char * argv[] );
int32_t Shell_nand_erase_ffs(int32_t argc, char * argv[] );
int32_t Shell_nand_get_bad_block_table(int32_t argc, char * argv[] );
int32_t mfs_nandflash_wl_open(void);
int32_t mfs_nandflash_wl_close(void);

void ffs_erase_drive(void);
void ffs_repair(void);
void nand_erase_ffs(void);
void nand_get_bad_block_table(void);

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index,  Function,       Stack,  Priority,   Name,       Attributes,             Param,  Time Slice */
    { 1,            shell_task,     3000,  12,         "Shell",    MQX_AUTO_START_TASK,    0,      0 },
    { 0 }
};

const SHELL_COMMAND_STRUCT Shell_commands[] = {
    { "cd",                 Shell_cd },
    { "copy",               Shell_copy },
    { "create",             Shell_create },
    { "del",                Shell_del },
    { "df",                 Shell_df },
    { "disect",             Shell_disect},
    { "dir",                Shell_dir },
    { "di",                 Shell_di },
    { "exit",               Shell_exit },
    { "format",             Shell_format },
    { "help",               Shell_help },
    { "mkdir",              Shell_mkdir },
    { "pwd",                Shell_pwd },
    { "read",               Shell_read },
    { "ren",                Shell_rename },
    { "rmdir",              Shell_rmdir },
    { "sh",                 Shell_sh },
    { "type",               Shell_type },
    { "write",              Shell_write },
    { "ffs_open",           Shell_ffs_open },
    { "ffs_close",          Shell_ffs_close },
    { "ffsflush",           Shell_ffs_flush },
    { "ffs_erase_drive",    Shell_ffs_erase_drive },
    { "ffs_repair",         Shell_ffs_repair },
    { "nand_erase_ffs",     Shell_nand_erase_ffs },
    { "nand_get_bbt",       Shell_nand_get_bad_block_table },
    { "?",                  Shell_command_list },
    { NULL,                 NULL }
};

bool inserted = TRUE, readonly = FALSE, last = FALSE;
_mqx_int error_code;
_mqx_uint param;
MQX_FILE_PTR com_handle, nandflash_handle, filesystem_handle, partman_handle;
char filesystem_name[] = "a:";
char partman_name[] = "pm:";

#define FFS_DEVICE "nandflash_wl:"

/*TASK*-----------------------------------------------------------------
*
* Function Name  : ffs_erase_drive
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void ffs_erase_drive()
{
    if (nandflash_handle == NULL)
    {
        nandflash_handle = fopen(FFS_DEVICE, NANDWL_OPEN_FORCE_REPAIR_WHEN_FAILED);
    }

    printf("\nErasing FFS drive ... \n");

    if (MQX_OK != ioctl(nandflash_handle, NANDFLASH_IOCTL_ERASE, (void*)0))
    {
        printf("\nSome errors occurred. Please close the device and try again.\n");
        return;
    }
    else
    {
        if (filesystem_handle == NULL)
        {
            error_code = _io_mfs_install(nandflash_handle, filesystem_name, (_file_size)0);
            if (error_code != MFS_NO_ERROR)
            {
                printf("Error initializing MFS: %s\n", MFS_Error_text((uint32_t)error_code));
                _task_block();
            }
            /* Open file system */
            filesystem_handle = fopen(filesystem_name, NULL);
            error_code = ferror (filesystem_handle);
            if ((error_code != MFS_NO_ERROR) && (error_code != MFS_NOT_A_DOS_DISK))
            {
                printf("Error opening filesystem: %s\n", MFS_Error_text((uint32_t)error_code));
                _task_block();
            }

            printf ("NAND flash installed to %s\n", filesystem_name);

            if ( error_code == MFS_NOT_A_DOS_DISK )
            {
                printf("NOT A DOS DISK! You must format to continue.\n");
                return ;
            }
        }
        printf("\nFFS drive was erased completely. Please run \"ffs_close\" command to close device then run \"ffs_open\" command to open again.\n");
    }
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : nandflash_erase_chip
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void nand_erase_ffs()
{
    int i;

    if (nandflash_handle != NULL)
    {
        printf("\nFilesystem must be closed before erase \n");
        return;
    }

    nandflash_handle = fopen(FFS_DEVICE, NANDWL_OPEN_IGNORE_INIT_MEDIA);

    printf("\nErasing FFS area from block #%d to block #%d... \n", NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
    for (i = NANDFLASH_1ST_DATA_DRIVE_START_BLOCK; i < NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK; i++)
    {
        if (MQX_OK != ioctl(nandflash_handle, NANDFLASH_IOCTL_ERASE_BLOCK_FORCE, (void*) i))
        {
            printf("\nErase block %d FAILED", i);
        }
    }

    fclose(nandflash_handle);
    nandflash_handle = NULL;
    printf("\nPlease use \"ffs_open\" command to re-open NAND flash device.\n");

    return;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : nandflash_get_bad_block_table
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void nand_get_bad_block_table()
{
    _mqx_int    i;
    uint8_t     *bbt;
    _mqx_uint   num_blocks, bad_count;

    if (nandflash_handle != NULL)
    {
        printf("\nFilesystem must be closed before getting bad block table \n");
        return;
    }
    
    nandflash_handle = fopen(FFS_DEVICE, NANDWL_OPEN_IGNORE_INIT_MEDIA);

    if(MQX_OK == ioctl(nandflash_handle, NANDFLASH_IOCTL_GET_NUM_BLOCKS, &num_blocks))
        printf("\nNumber of blocks: %d", num_blocks);
    else
       printf("\nNumber of blocks not obtained");

    /* Get Bad Block Table */
    bbt = (uint8_t *)_mem_alloc_zero(num_blocks);
    bad_count = 0;
    if(MQX_OK == ioctl(nandflash_handle, NANDFLASH_IOCTL_GET_BAD_BLOCK_TABLE, bbt))
    {
        for (i = 0; i < (_mqx_int)num_blocks; i++)
        {
            if(bbt[i] == 0)
            {
                bad_count++;
                printf("\nBlock #%d is bad", i);
            }
        }
    }
    else
    {
        printf("\nReading Bad block table not successful ");
    }
    printf("\nThere are %d bad blocks\n", bad_count);
    _mem_free(bbt);

    fclose(nandflash_handle);
    nandflash_handle = NULL;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : ffs_repair
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void ffs_repair()
{
    if (nandflash_handle != NULL)
    {
        fclose(nandflash_handle);
        nandflash_handle = NULL;
    }
    printf("\nRepairing...\n");

    /* Open nand flash wl in force_repair mode */
    nandflash_handle = fopen(FFS_DEVICE, NANDWL_OPEN_FORCE_REPAIR_WHEN_FAILED);

    if (nandflash_handle != NULL)
    {
        if (filesystem_handle == NULL)
        {
            error_code = _io_mfs_install(nandflash_handle, filesystem_name, (_file_size)0);
            if (error_code != MFS_NO_ERROR)
            {
                printf("Error initializing MFS: %s\n", MFS_Error_text((uint32_t)error_code));
                _task_block();
            }
            /* Open file system */
            filesystem_handle = fopen(filesystem_name, NULL);
            error_code = ferror (filesystem_handle);
            if ((error_code != MFS_NO_ERROR) && (error_code != MFS_NOT_A_DOS_DISK))
            {
                printf("Error opening filesystem: %s\n", MFS_Error_text((uint32_t)error_code));
                _task_block();
            }

            printf ("NAND flash installed to %s\n", filesystem_name);

            if ( error_code == MFS_NOT_A_DOS_DISK )
            {
                printf("NOT A DOS DISK! You must format to continue.\n");
                return ;
            }
        }
        printf("\nRepair successfully\n");
    } else {
        printf("\nYou must re-open\n");
    }
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : mfs_nandflash_wl_open
* Returned Value : int32_t
* Comments       :
*
*END------------------------------------------------------------------*/
int32_t mfs_nandflash_wl_open()
{
    if (nandflash_handle == NULL)
    {
        /* Open nand flash wl in normal mode */
        nandflash_handle = fopen(FFS_DEVICE, NANDWL_OPEN_NORMAL);
        if ( nandflash_handle == NULL )
        {
            printf("Unable to open NAND Flash device.\n");
            printf("\nYou must run \"ffs_repair\" command\n");
            return 0;
        }

        /* Install MFS over nand_wl driver */
        error_code = _io_mfs_install(nandflash_handle, filesystem_name, (_file_size)0);
        if (error_code != MFS_NO_ERROR)
        {
            printf("Error initializing MFS: %s\n", MFS_Error_text((uint32_t)error_code));
            _task_block();
        }

        if (filesystem_handle == NULL)
        {
            /* Open file system */
            filesystem_handle = fopen(filesystem_name, NULL);
            error_code = ferror (filesystem_handle);
            if ((error_code != MFS_NO_ERROR) && (error_code != MFS_NOT_A_DOS_DISK))
            {
                printf("Error opening filesystem: %s\n", MFS_Error_text((uint32_t)error_code));
                _task_block();
            }

            printf ("NAND flash installed to %s\n", filesystem_name);

            if ( error_code == MFS_NOT_A_DOS_DISK )
            {
                printf("NOT A DOS DISK! You must format to continue.\n");
                return 0;
            }
        }
        else
        {
            printf("\nNAND flash was installed.\n");
        }
    }
    else
    {
        printf("\nNAND flash device was opened.\n");
    }

    return 0;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : mfs_nandflash_wl_close
* Returned Value : int32_t
* Comments       :
*
*END------------------------------------------------------------------*/
int32_t mfs_nandflash_wl_close()
{
    if (filesystem_handle != NULL)
    {
        /* Close the filesystem */
        if (MQX_OK != fclose (filesystem_handle))
        {
            printf("Error closing filesystem.\n");
            _task_block();
        }
        filesystem_handle = NULL;
    }

    if (nandflash_handle != NULL)
    {
        /* Uninstall MFS  */
        error_code = _io_dev_uninstall(filesystem_name);
        if (error_code != MFS_NO_ERROR)
        {
            printf("Error uninstalling filesystem.\n");
            _task_block();
        }

        /* Close the nand flash device */
        if (MQX_OK != fclose (nandflash_handle))
        {
            printf("Unable to close NAND Flash device.\n");
            _task_block();
        }
        nandflash_handle = NULL;

        printf ("NAND Flash was uninstalled.\n");
    }

    return 0;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : shell_task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void shell_task(uint32_t temp)
{
    /* Run the shell on the serial port */
    printf("This is the Flash File System example, type help for list of available commands\n");
    printf("If this is the first time you use the demo, you should run \"nand_erase_ffs\" command first.\n");

    if (_io_nandflash_wl_install(&_bsp_nandflash_wl_init, FFS_DEVICE) != MQX_OK)
    {
        printf("Can't install FFS!\n");
        _task_block();
    }

    for(;;)
    {
        Shell(Shell_commands, NULL);
        printf("Shell exited, restarting...\n");
    }
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_ffs_open
* Returned Value : int32_t
* Comments       :
*
*END------------------------------------------------------------------*/
int32_t Shell_ffs_open(int32_t argc, char * argv[] )
{
    bool        print_usage, shorthelp = FALSE;
    int32_t     return_code = SHELL_EXIT_SUCCESS;

    print_usage = Shell_check_help_request(argc, argv, &shorthelp );

    if (!print_usage)  {
        if (argc != 1)
        {
            printf("Error, invalid number of parameters\n");
            return_code = SHELL_EXIT_ERROR;
            print_usage=TRUE;
        }
        else
        {
            mfs_nandflash_wl_open();
        }
    }

    if (print_usage)  {
        if (shorthelp)  {
            printf("%s \n", argv[0]);
        } else  {
            printf("Usage: %s\n", argv[0]);
            printf("   Open the Flash file system\n");
        }
    }
    return return_code;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_ffs_close
* Returned Value : int32_t
* Comments       :
*
*END------------------------------------------------------------------*/
int32_t Shell_ffs_close(int32_t argc, char * argv[] )
{
    bool        print_usage, shorthelp = FALSE;
    int32_t     return_code = SHELL_EXIT_SUCCESS;

    print_usage = Shell_check_help_request(argc, argv, &shorthelp );

    if (!print_usage)  {
        if (argc != 1)
        {
            printf("Error, invalid number of parameters\n");
            return_code = SHELL_EXIT_ERROR;
            print_usage=TRUE;
        }
        else
        {
            mfs_nandflash_wl_close();
        }
    }

    if (print_usage)  {
        if (shorthelp)  {
            printf("%s \n", argv[0]);
        } else  {
            printf("Usage: %s\n", argv[0]);
            printf ("   Close the Flash file system\n");
        }
    }
    return return_code;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_ffsflush
* Returned Value : int32_t
* Comments       :
*
*END------------------------------------------------------------------*/
int32_t  Shell_ffs_flush(int32_t argc, char * argv[] )
{
    bool        print_usage, shorthelp = FALSE;
    int32_t     return_code = SHELL_EXIT_SUCCESS;

    print_usage = Shell_check_help_request(argc, argv, &shorthelp );

    if (!print_usage)  {  
        if (argc != 1)  
        {
            printf("Error, invalid number of parameters\n");
            return_code = SHELL_EXIT_ERROR;
            print_usage=TRUE;
        } 
        else
        {
            if (filesystem_handle == NULL)
            {
                printf("\nError, file system is not opened \n");
                return_code = SHELL_EXIT_ERROR;
            } else
            {
                return_code = fflush(filesystem_handle);
                if (return_code != MQX_OK)
                    printf("\nError, unable to flush file system \n");
            }
        }
    }

    if (print_usage)  {
        if (shorthelp)  {
            printf("%s \n", argv[0]);
        } else  {
            printf("Usage: %s\n", argv[0]);
            printf ("   Flush the Flash file system\n");
        }
    }
    return return_code;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_ffs_erase_drive
* Returned Value : int32_t
* Comments       :
*
*END------------------------------------------------------------------*/
int32_t Shell_ffs_erase_drive(int32_t argc, char * argv[] )
{
    bool        print_usage, shorthelp = FALSE;
    int32_t     return_code = SHELL_EXIT_SUCCESS;

    print_usage = Shell_check_help_request(argc, argv, &shorthelp );

    if (!print_usage)  {
        if (argc != 1)
        {
            printf("Error, invalid number of parameters\n");
            return_code = SHELL_EXIT_ERROR;
            print_usage=TRUE;
        }
        else
        {
            ffs_erase_drive();
        }
    }

    if (print_usage)  {
        if (shorthelp)  {
            printf("%s \n", argv[0]);
        } else  {
            printf("Usage: %s\n", argv[0]);
            printf ("   Erase the Flash file system drive\n");
        }
    }
    return return_code;

}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_nand_erase_ffs
* Returned Value : int32_t
* Comments       :
*
*END------------------------------------------------------------------*/
int32_t Shell_nand_erase_ffs(int32_t argc, char * argv[] )
{
    bool        print_usage, shorthelp = FALSE;
    int32_t     return_code = SHELL_EXIT_SUCCESS;

    print_usage = Shell_check_help_request(argc, argv, &shorthelp );

    if (!print_usage)  {
        if (argc != 1)
        {
            printf("Error, invalid number of parameters\n");
            return_code = SHELL_EXIT_ERROR;
            print_usage=TRUE;
        }
        else
        {
            nand_erase_ffs();
        }
    }

    if (print_usage)  {
        if (shorthelp)  {
            printf("%s \n", argv[0]);
        } else  {
            printf("Usage: %s\n", argv[0]);
            printf("   Erase FFS area\n");
        }
    }
    return return_code;

}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_nand_get_bad_block_table
* Returned Value : int32_t
* Comments       :
*
*END------------------------------------------------------------------*/
int32_t Shell_nand_get_bad_block_table(int32_t argc, char * argv[] )
{
    bool        print_usage, shorthelp = FALSE;
    int32_t     return_code = SHELL_EXIT_SUCCESS;

    print_usage = Shell_check_help_request(argc, argv, &shorthelp );

    if (!print_usage)  {
        if (argc != 1)
        {
            printf("Error, invalid number of parameters\n");
            return_code = SHELL_EXIT_ERROR;
            print_usage=TRUE;
        }
        else
        {
            nand_get_bad_block_table();
        }
    }

    if (print_usage)  {
        if (shorthelp)  {
            printf("%s \n", argv[0]);
        } else  {
            printf("Usage: %s\n", argv[0]);
            printf("   Get bad block table of NAND Flash memory\n");
        }
    }
    return return_code;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_ffs_repair
* Returned Value : int32_t
* Comments       :
*
*END------------------------------------------------------------------*/
int32_t Shell_ffs_repair(int32_t argc, char * argv[] )
{
    bool        print_usage, shorthelp = FALSE;
    int32_t     return_code = SHELL_EXIT_SUCCESS;

    print_usage = Shell_check_help_request(argc, argv, &shorthelp );

    if (!print_usage)  {
        if (argc != 1)
        {
            printf("Error, invalid number of parameters\n");
            return_code = SHELL_EXIT_ERROR;
            print_usage=TRUE;
        }
        else
        {
            ffs_repair();
        }
    }

    if (print_usage)  {
        if (shorthelp)  {
            printf("%s \n", argv[0]);
        } else  {
            printf("Usage: %s\n", argv[0]);
            printf("    Repair the Flash file system\n");
        }
    }
    return return_code;
}

/* EOF */
