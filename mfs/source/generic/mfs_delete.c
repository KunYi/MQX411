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


#if !MFSCFG_READ_ONLY


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Delete_file
* Returned Value   : error_code
* Comments  :   Delete a specific file.
*
*END*---------------------------------------------------------------------*/

_mfs_error  MFS_Delete_file
    (
        MFS_DRIVE_STRUCT_PTR drive_ptr,
        
        MFS_HANDLE_PTR       handle,
        
        char                *pathname    /*[IN] directory and file name of the file to delete */
    )
{
    MFS_DIR_ENTRY_PTR       dir_entry_ptr;
    _mfs_error              error_code, saved_code;
    uint32_t                 dir_cluster, dir_index;
    uint32_t                 first_cluster;
    uint32_t                 prev_cluster= CLUSTER_INVALID;


    if ( (pathname == NULL) || (*pathname == '\0') )
    {
        return MFS_INVALID_PARAMETER;
    }

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (drive_ptr))
    {
        return MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif

    error_code = MFS_lock_dos_disk( drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        return error_code;
    }

    dir_entry_ptr = MFS_Find_entry_on_disk(drive_ptr, pathname, &error_code, &dir_cluster, &dir_index, &prev_cluster);

    if ( dir_entry_ptr != NULL )
    {
        if (mqx_dtohc(dir_entry_ptr->ATTRIBUTE) & (MFS_ATTR_DIR_NAME | MFS_ATTR_VOLUME_NAME | MFS_ATTR_READ_ONLY))
        {
            /* Not a file or a read only file */
            error_code = MFS_ACCESS_DENIED;
        }
        else if (MFS_Find_handle(drive_ptr, dir_cluster, dir_index) != NULL)
        {
            /* File is currently open */
            error_code = MFS_SHARING_VIOLATION;
        }
        else
        {
            first_cluster = clustoh(dir_entry_ptr->HFIRST_CLUSTER, dir_entry_ptr->LFIRST_CLUSTER);

            if (first_cluster)
            {
                error_code = MFS_Release_chain(drive_ptr, first_cluster);
            }

            if (error_code == MFS_NO_ERROR || error_code == MFS_LOST_CHAIN)
            {
                *dir_entry_ptr->NAME = MFS_DEL_FILE;
                drive_ptr->DIR_SECTOR_DIRTY = TRUE;

                saved_code = MFS_remove_lfn_entries(drive_ptr,dir_cluster,dir_index, prev_cluster);
                if (error_code == MFS_NO_ERROR)
                {
                    error_code = saved_code;
                }
            }
        }
    }
    MFS_unlock(drive_ptr,TRUE);

    return(error_code);
}

#endif
