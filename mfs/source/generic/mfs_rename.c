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
*   This file contains the file specific interface functions
*   of MFS which need a filename as an input parameter.
*
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <fio.h>

#include "mfs.h"
#include "mfs_prv.h"




/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Rename_file
* Returned Value   : error_code
* Comments  :   Rename an existing file.
*
*END*---------------------------------------------------------------------*/
#if !MFSCFG_READ_ONLY

_mfs_error  MFS_Rename_file
    (
        MFS_DRIVE_STRUCT_PTR drive_ptr,
        char                *old_pathname,   /*[IN] directory and file name of file to be renamed */
        char                *new_pathname    /*[IN] directory and file name to be given to the file */
    )
{
    MFS_DIR_ENTRY_PTR       source_entry, dest_entry;
    MFS_DIR_ENTRY           temp_dir_entry;
    char                *dir_name, *new_filename, *old_filename;
    uint32_t                 source_dir, /* 1st cluster of the source dir      */
                            dest_dir,   /* 1st cluster of the destination dir */
                            source_dir_bak,
                            dest_dir_bak;
    uint32_t                 dir_index,dir_index_bak;    
    _mfs_error              error_code;
    char                    temp_filename[SFILENAME_SIZE+1];  
    uint32_t                 dest_prev_cluster = CLUSTER_INVALID, source_prev_cluster = CLUSTER_INVALID; 

    if ( old_pathname == NULL || *old_pathname == '\0' || new_pathname == NULL || *new_pathname == '\0' )
    {
        return MFS_INVALID_PARAMETER;
    }

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (drive_ptr))
    {
        return MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif    

    error_code = MFS_alloc_path(&dir_name);
    if ( error_code != MFS_NO_ERROR )
    {
        return( error_code );
    }
    error_code = MFS_alloc_2paths(&new_filename,&old_filename);
    if ( error_code != MFS_NO_ERROR )
    {
        MFS_free_path(dir_name);
        return( error_code );
    }

    error_code = MFS_lock_dos_disk( drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        goto error_free_paths;
    }

    /*
    ** Find the destination directory
    */
    MFS_Parse_pathname (dir_name, new_filename, new_pathname);
    dest_dir = MFS_Find_directory(drive_ptr, dir_name, drive_ptr->CUR_DIR_CLUSTER);
    if ( dest_dir == CLUSTER_INVALID )
    {
        error_code = MFS_PATH_NOT_FOUND;
        goto error_unlock_and_free_paths;
    }

    /*
    ** Find the source directory
    */
    MFS_Parse_pathname (dir_name, old_filename, old_pathname);
    source_dir =  MFS_Find_directory(drive_ptr, dir_name, drive_ptr->CUR_DIR_CLUSTER);
    if ( source_dir == CLUSTER_INVALID )
    {
        error_code = MFS_PATH_NOT_FOUND;
        goto error_unlock_and_free_paths;
    }

    /*
    ** Find the source file
    */
    if ( *new_filename != '.' && *old_filename != '.' )
    {
        dir_index = 0;
        dest_dir_bak = dest_dir;
        dest_entry = MFS_Find_directory_entry(drive_ptr, new_filename, &dest_dir_bak, &dir_index, &dest_prev_cluster, MFS_ATTR_ANY, &error_code); 
        if ( dest_entry == NULL && !error_code )
        {
            dir_index = 0;
            dir_index_bak = dir_index;
            source_dir_bak = source_dir;
            source_entry = MFS_Find_directory_entry(drive_ptr, old_filename, &source_dir_bak, &dir_index, &source_prev_cluster, MFS_ATTR_ANY, &error_code); 
            if ( source_entry != NULL && !error_code )
            {
                if ( !(mqx_dtohc(source_entry->ATTRIBUTE) & MFS_ATTR_VOLUME_NAME) )
                {
                    _mem_copy(source_entry, &temp_dir_entry, sizeof (MFS_DIR_ENTRY));
                    if ( source_dir != dest_dir )
                    {
                        /*
                        ** Moving the file into different directory
                        */
                        if ( !(mqx_dtohc(source_entry->ATTRIBUTE) & 
                            MFS_ATTR_READ_ONLY) )
                        {
                            dest_entry = MFS_Create_directory_entry (drive_ptr, new_filename, mqx_dtohc(source_entry->ATTRIBUTE), 
                                &dest_dir, &dir_index, &error_code);

                            if ( error_code == MFS_NO_ERROR )
                            {
                                strcpy(temp_filename,dest_entry->NAME);
                                _mem_copy (&temp_dir_entry, dest_entry,sizeof (MFS_DIR_ENTRY));
                                strcpy(dest_entry->NAME,temp_filename);
                                drive_ptr->DIR_SECTOR_DIRTY = TRUE;

                                //        error_code = MFS_Write_back_directory_sector_buffer(drive_ptr);
                                //        if (error_code == MFS_NO_ERROR) {
                                source_entry = MFS_Find_directory_entry(drive_ptr,
                                    old_filename, &source_dir,&dir_index_bak, &source_prev_cluster, MFS_ATTR_ANY, &error_code);
                                if ( error_code )
                                {
                                    goto error_unlock_and_free_paths;
                                }
                                *source_entry->NAME = MFS_DEL_FILE;
                                drive_ptr->DIR_SECTOR_DIRTY = TRUE;

                                error_code = MFS_remove_lfn_entries(drive_ptr, source_dir, dir_index_bak, source_prev_cluster); 
                            }
                        }
                        else
                        {
                            error_code = MFS_ACCESS_DENIED;
                        } 
                    }
                    else
                    {
                        /*
                        ** Renaming the file in the same directory
                        */
                        *source_entry->NAME = MFS_DEL_FILE;
                        drive_ptr->DIR_SECTOR_DIRTY = TRUE;

                        error_code = MFS_remove_lfn_entries(drive_ptr,source_dir,dir_index, source_prev_cluster); 
                        if ( error_code )
                        {
                            goto error_unlock_and_free_paths;
                        }
                        dest_entry = MFS_Create_directory_entry (drive_ptr, new_filename, mqx_dtohc(temp_dir_entry.ATTRIBUTE), 
                            &source_dir_bak, &dir_index, &error_code);

                        *(dest_entry->ATTRIBUTE) = *(temp_dir_entry.ATTRIBUTE);
                        mqx_htodl(dest_entry->FILE_SIZE, mqx_dtohl(temp_dir_entry.FILE_SIZE));                  
                        mqx_htods(dest_entry->HFIRST_CLUSTER, mqx_dtohs(temp_dir_entry.HFIRST_CLUSTER));                  
                        mqx_htods(dest_entry->LFIRST_CLUSTER, mqx_dtohs(temp_dir_entry.LFIRST_CLUSTER));                  
                    }  
                }
                else
                {
                    error_code = MFS_WRITE_FAULT;
                }  
            }
            else if ( source_entry == NULL && !error_code )
            {
                error_code = MFS_FILE_NOT_FOUND;
            }
        }
        else if ( dest_entry != NULL && !error_code )
        {
            error_code = MFS_FILE_EXISTS;
        }
    }
    else
    {
        error_code = MFS_INVALID_PARAMETER;
    }  


    error_unlock_and_free_paths:   
    MFS_unlock(drive_ptr,TRUE);
    error_free_paths:   
    MFS_free_path(dir_name);
    MFS_free_path(new_filename);
    MFS_free_path(old_filename);

    return(error_code);
}  
#endif

/* EOF */
