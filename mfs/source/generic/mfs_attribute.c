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
* Function Name    : MFS_Get_file_attributes
* Returned Value   : error_code
* Comments  :   Get the attribute byte of the specified file.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Get_file_attributes
    (
        MFS_DRIVE_STRUCT_PTR drive_ptr,
        
        MFS_HANDLE_PTR       handle,
        
        char               *pathname,
        unsigned char      *attribute_ptr
    )
{
    MFS_DIR_ENTRY_PTR       dir_entry_ptr;
    uint32_t                 dir_cluster,dir_index;
    _mfs_error              error_code;
    unsigned char                   attribute;
    uint32_t                 prev_cluster= CLUSTER_INVALID; 


    if ( (pathname == NULL) || (*pathname == '\0') )
    {
        return MFS_INVALID_PARAMETER;
    }

    error_code = MFS_lock_dos_disk( drive_ptr );
    if ( error_code != MFS_NO_ERROR )
    {
        return(error_code);
    }

    dir_entry_ptr = MFS_Find_entry_on_disk(drive_ptr, pathname, &error_code, &dir_cluster, &dir_index, &prev_cluster);  

    /*
    ** When a function locks MFS device, on any error it should not return 
    ** without unlocking it. This can create a deadlock.
    */   
    if ( error_code == MFS_NO_ERROR )
    {
        attribute = mqx_dtohc(dir_entry_ptr->ATTRIBUTE);
        *attribute_ptr = attribute;
    }

    MFS_unlock(drive_ptr,FALSE);
    return(error_code);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Set_file_attributes
* Returned Value   : error code
* Comments  :   Change the attribute of the specified file.
*
*END*---------------------------------------------------------------------*/
#if !MFSCFG_READ_ONLY
_mfs_error MFS_Set_file_attributes
    (
        MFS_DRIVE_STRUCT_PTR drive_ptr,
        
        MFS_HANDLE_PTR       handle,
        
        char               *pathname,       /*[IN] pathname of the specific file */
        unsigned char      *attribute_ptr   /*[IN] attribute of file */
    )
{
    MFS_DIR_ENTRY_PTR       dir_entry_ptr;
    _mfs_error              error_code;
    uint32_t                 dir_cluster, dir_index;
    unsigned char                   at, attrib;
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
    attrib = *attribute_ptr;
    attrib &= MFS_ATTR_ARCHIVE | MFS_ATTR_READ_ONLY | MFS_ATTR_HIDDEN_FILE | MFS_ATTR_SYSTEM_FILE;
    dir_entry_ptr = MFS_Find_entry_on_disk(drive_ptr, pathname, &error_code, &dir_cluster, &dir_index, &prev_cluster);  
    if ( error_code == MFS_NO_ERROR )
    {
        at = mqx_dtohc(dir_entry_ptr->ATTRIBUTE);
        if ( at != *attribute_ptr )
        {
            /*
            ** The volume-label and the directory-name are mutually exclusive.
            ** The volume-label cannot act as a directory-name. Check whether
            ** you are trying to set the attributes to a volume.
            */
            if ( ! ((at & MFS_ATTR_VOLUME_NAME) || (*attribute_ptr & MFS_ATTR_VOLUME_NAME)) )
            {
                mqx_htodc(dir_entry_ptr->ATTRIBUTE, *attribute_ptr);
                drive_ptr->DIR_SECTOR_DIRTY = TRUE;
            }
            else
            {
                error_code = MFS_ACCESS_DENIED;
            }  
        }
    }

    MFS_unlock(drive_ptr,TRUE);

    return(error_code);
}  
#endif

