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
*   of the MFS.
*
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>

#include "mfs.h"
#include "mfs_prv.h"


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Open_file
* Returned Value   : A pointer to an MFS_HANDLE
* Comments  :   Open a an existing file.
*
*END*---------------------------------------------------------------------*/

void  *MFS_Open_file
    (
        MFS_DRIVE_STRUCT_PTR drive_ptr,
        char                *pathname,       /*[IN] directory and filename of the file to be opened */
        unsigned char       access,         /*[IN] type of access required: read, write or read/write*/
        uint32_t            *error_ptr   /*[IN/OUT] error code is written to this address */   
    )
{
    MFS_DIR_ENTRY_PTR       dir_entry_ptr;
    MFS_HANDLE_PTR          handle,open_handle;
    uint32_t                 dir_cluster, dir_index;
    _mfs_error              error_code;
    uint32_t                 prev_cluster= CLUSTER_INVALID; 

    if ( (pathname == NULL) || (*pathname == '\0') )
    {
        if ( error_ptr != NULL )
        {
            *error_ptr = MFS_INVALID_PARAMETER;
        }
        return NULL;
    }

    error_code = MFS_lock_dos_disk( drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        if ( error_ptr != NULL )
        {
            *error_ptr = error_code;
        }
        return NULL;
    }

    handle = NULL;

    dir_entry_ptr = MFS_Find_entry_on_disk(drive_ptr, pathname, &error_code, &dir_cluster, &dir_index, &prev_cluster);  

    if ( error_code == MFS_NO_ERROR )
    {

        if ( mqx_dtohc(dir_entry_ptr->ATTRIBUTE) & (MFS_ATTR_DIR_NAME | MFS_ATTR_VOLUME_NAME) )
        {
            error_code = MFS_WRITE_FAULT;
        }
        else if ( (mqx_dtohc(dir_entry_ptr->ATTRIBUTE) & MFS_ATTR_READ_ONLY) && ((access == MFS_ACCESS_WRITE_ONLY) || (access == MFS_ACCESS_READ_WRITE)) )
        {
            error_code = MFS_ACCESS_DENIED;
        }
        else
        {
            // Check to see if the file is already opened
            open_handle = MFS_Find_handle(drive_ptr, dir_cluster, dir_index);
            
            if (open_handle) {
                // If we are opening to write, the file can't already be opened.
                if ((access == MFS_ACCESS_WRITE_ONLY) || (access == MFS_ACCESS_READ_WRITE)) {
                    error_code = MFS_SHARING_VIOLATION;
                }

                // And if we the file is already opened, it can't be opened to write.
                // Note that if it is opened for write, it will be the only instance on the list
                if ((handle->ACCESS == MFS_ACCESS_WRITE_ONLY) || (handle->ACCESS == MFS_ACCESS_READ_WRITE)) {
                    error_code = MFS_SHARING_VIOLATION;
                }
            }
        
            if (error_code == MFS_NO_ERROR) {
                handle = MFS_Get_handle(drive_ptr,dir_entry_ptr);
                if ( handle != NULL )
                {
                    handle->ACCESS = access;
                    handle->CURRENT_CLUSTER = 0;
                    handle->PREVIOUS_CLUSTER = 0;
                    handle->DIR_CLUSTER = dir_cluster;
                    handle->DIR_INDEX = dir_index;
                }
                else
                {
                    error_code = MFS_INSUFFICIENT_MEMORY;
                }  
            }
        }  
    }

    MFS_unlock(drive_ptr,FALSE);

    if ( error_ptr )
    {
        *error_ptr = error_code;
    }

    return((void *) handle);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Close_file
* Returned Value   : error code
* Comments  :   Close an existing file.
*
*END*---------------------------------------------------------------------*/

int32_t  MFS_Close_file
    (
        MFS_HANDLE_PTR handle,
        MFS_DRIVE_STRUCT_PTR drive_ptr
    )
{
    TIME_STRUCT             time;
    DATE_STRUCT             clk_time;
    _mfs_error              error_code;

    error_code = MFS_lock_dos_disk( drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        return error_code;
    }

#if !MFSCFG_READ_ONLY
#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (drive_ptr))
    {
        error_code = MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif    
    if ((handle->TOUCHED) && (error_code == MFS_NO_ERROR))
    {
        _time_get(&time);
        _time_to_date(&time, &clk_time);
        NORMALIZE_DATE(&clk_time);
        mqx_htods(handle->DIR_ENTRY.TIME, PACK_TIME(clk_time));
        mqx_htods(handle->DIR_ENTRY.DATE, PACK_DATE(clk_time));
        error_code = MFS_Update_entry(drive_ptr, handle);
    }
#endif

    MFS_Free_handle(drive_ptr, handle);
    MFS_unlock(drive_ptr,TRUE);

    return(int32_t) error_code;
}  



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Move_file_pointer
* Returned Value   : current file pointer position
* Comments  :   Move the current file pointer position.
*               If filelength = 0, it returns MFS_EOF
*END*---------------------------------------------------------------------*/

uint32_t MFS_Move_file_pointer
    (
    MFS_HANDLE_PTR          handle,
    MFS_DRIVE_STRUCT_PTR    drive_ptr,
    _mfs_error_ptr          error_ptr /*[IN/OUT] resulting error code is written to this address*/
    )
{
    uint32_t                 position_after_seek, position_before_seek;
    uint32_t                 current_cluster,
                            previous_cluster,
                            first_cluster,
                            skip_clusters,
                            k;
    _mfs_error              error_code;


    error_code = MFS_lock_dos_disk( drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        if ( error_ptr != NULL )
        {
            *error_ptr = error_code;
        }
        return 0;
    }

    position_after_seek = handle->LOCATION;

    MFS_LOG(printf("seek to %d\n",position_after_seek));

    /*
    ** Cannot move ahead of the beginning of the file; force beginning.
    */
    if ( error_code == MFS_NO_ERROR )
    {

        /*
        ** There are four cases
        **
        ** 1) Seeking to beginning of file (position_after_seek=0, cluster=0)
        ** 2) No change in position (position_after_seek==position_before_seek)
        ** 2) Seeking ahead of current file pointer
        ** 3) Seeking behind current file pointer
        */
        /*
        ** Cannot move beyond the end of file !
        */
        if ( position_after_seek> mqx_dtohl(handle->DIR_ENTRY.FILE_SIZE) )     // + 1
        {
            position_after_seek = mqx_dtohl(handle->DIR_ENTRY.FILE_SIZE);  //  - 1
            error_code = MFS_EOF;
        }

        current_cluster = 0;
        previous_cluster = 0;
        first_cluster = clustoh(handle->DIR_ENTRY.HFIRST_CLUSTER, handle->DIR_ENTRY.LFIRST_CLUSTER);
        MFS_LOG(printf("first_cluster =  %d\n",first_cluster));

        /*
        ** Set the current_cluster correctly.
        ** If we're moving ahead, don't start from the beginning.
        */
        position_before_seek = CLUSTER_BOUNDARY(handle->SAVED_POSITION);

        if ( handle->CURRENT_CLUSTER==0 )
        {
            handle->CURRENT_CLUSTER = first_cluster;
            handle->PREVIOUS_CLUSTER = 0;
            position_before_seek = 0;
        }

        if ( position_after_seek == 0 )
        {
            current_cluster = first_cluster;
            previous_cluster = 0;
        }
        else if ( position_after_seek == position_before_seek )
        {
            current_cluster  = handle->CURRENT_CLUSTER;
            previous_cluster = handle->PREVIOUS_CLUSTER;
        }
        else
        {
            if ( position_after_seek < position_before_seek )
            {
                position_before_seek = 0;
                current_cluster = first_cluster;
                previous_cluster = 0;
            }
            else
            {
                current_cluster  = handle->CURRENT_CLUSTER;
                previous_cluster = handle->PREVIOUS_CLUSTER;
            }  

            MFS_LOG(printf("current cluster =  %d\n",current_cluster));

            if ( current_cluster && (current_cluster!=CLUSTER_EOF) )
            {
                /*
                ** How many clusters do we need to skip?
                */
                skip_clusters = (position_after_seek - position_before_seek) >> drive_ptr->CLUSTER_POWER_BYTES;
                for ( k = 0; k < skip_clusters; k++ )
                {
                    previous_cluster = current_cluster;

                    error_code = MFS_get_cluster_from_fat(drive_ptr, previous_cluster, &current_cluster);
                    if ( error_code != MFS_NO_ERROR )
                    {
                        break;
                    }
                    if ( current_cluster==CLUSTER_EOF )
                    {
                        error_code = MFS_EOF;
                        break;
                    }
                    else if ( (current_cluster < CLUSTER_MIN_GOOD) || (current_cluster > drive_ptr->LAST_CLUSTER) )
                    {
                        error_code = MFS_BAD_DISK_UNIT;
                        break;
                    }
                    MFS_LOG(printf("skip, current cluster =  %d\n",current_cluster));
                }  
            }
            else
            {
                error_code = MFS_EOF;
            }
        }  

        if ( (error_code == MFS_NO_ERROR) || (error_code == MFS_EOF) )
        {
            handle->SAVED_POSITION = position_after_seek;

            handle->PREVIOUS_CLUSTER  = previous_cluster;
            handle->CURRENT_CLUSTER   = current_cluster;
        }
    }
    MFS_unlock(drive_ptr,FALSE);

    if ( error_ptr )
    {
        *error_ptr = error_code;
    }

    return handle->LOCATION;
}
