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
*   This file contains functions related to using file handles.
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
* Function Name    :  MFS_Get_handle
* Returned Value   :  a file handle
* Comments  :
*    Creates a new file handle and adds it the to queue of open handles.
*
*END*---------------------------------------------------------------------*/

MFS_HANDLE_PTR MFS_Get_handle
    (
    MFS_DRIVE_STRUCT_PTR   drive_ptr,       /*[IN] the drive on which to operate */
    MFS_DIR_ENTRY_PTR      dir_entry_ptr    /* [IN] pointer to the directory information for this file handle */
    )
{
    MFS_HANDLE_PTR    handle_ptr;

    handle_ptr = _partition_alloc_system_zero( drive_ptr->HANDLE_PARTITION );

    /*
    ** initialize the handle
    */
    if ( handle_ptr != NULL )
    {

        _queue_enqueue( &drive_ptr->HANDLE_LIST, (QUEUE_ELEMENT_STRUCT_PTR) handle_ptr);

        handle_ptr->VALID = MFS_VALID;

        if ( dir_entry_ptr )
        {
            _mem_copy(dir_entry_ptr, &handle_ptr->DIR_ENTRY,sizeof(MFS_DIR_ENTRY));
        }
    }
    return(handle_ptr);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Free_handle
* Returned Value   :  none
* Comments  :
*      Add the handle to the freelist of handles kept in the configuration
*       data.
*END*---------------------------------------------------------------------*/

void MFS_Free_handle
    (
    MFS_DRIVE_STRUCT_PTR      drive_ptr,    /*[IN] the drive on which to operate */
    MFS_HANDLE_PTR            handle_ptr    /*[IN] handle to be released */
    )
{
    if ( handle_ptr != NULL )
    {
        /* Unlink handle from HANDLE_LIST */
        _queue_unlink( &drive_ptr->HANDLE_LIST, (QUEUE_ELEMENT_STRUCT_PTR) handle_ptr);

        /* clear out the handle and add to the freelist */
        handle_ptr->VALID = ~handle_ptr->VALID;
        _partition_free(handle_ptr);
    }

}  



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Find_handle
* Returned Value   :  void
* Comments  :
*
*END*---------------------------------------------------------------------*/

MFS_HANDLE_PTR MFS_Find_handle
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,    /*[IN] the drive on which to operate */
    uint32_t                dir_cluster,
    uint32_t                dir_index
    )
{
   MFS_HANDLE_PTR   next_handle;

    next_handle = (MFS_HANDLE_PTR) _queue_head(&(drive_ptr->HANDLE_LIST));
    while ( next_handle )
    {
        if ((next_handle->DIR_CLUSTER == dir_cluster) && (next_handle->DIR_INDEX == dir_index))  
        {
            break;
        }
        next_handle = (MFS_HANDLE_PTR) _queue_next(&drive_ptr->HANDLE_LIST, (QUEUE_ELEMENT_STRUCT_PTR) next_handle);
    }  
    return next_handle;
}  
