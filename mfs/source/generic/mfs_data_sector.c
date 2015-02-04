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
*   This file contains the functions that are used to read and write to the 
*   device under MFS. 
*
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>

#include "mfs.h"
#include "mfs_prv.h"
#include "part_mgr.h"



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Flush_data_sector_buffer
* Returned Value   :  error_code
* Comments  :
*     Writes the cached sector back to disk if dirty.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Flush_data_sector_buffer
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr     /*[IN] the drive on which to operate */
    )
{
#if MFSCFG_READ_ONLY
    return MFS_NO_ERROR;
#else
    _mfs_error   error_code = MFS_NO_ERROR;

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (drive_ptr))
    {
        return error_code;
    }
#endif

    if ( drive_ptr->DATA_SECTOR_DIRTY )
    {
        error_code = MFS_Write_device_sector(drive_ptr,drive_ptr->DATA_SECTOR_NUMBER,drive_ptr->DATA_SECTOR_PTR);

        if ( error_code == MFS_NO_ERROR )
        {
            drive_ptr->DATA_SECTOR_DIRTY=FALSE;
        }
    }

    return error_code;
#endif
}  



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Invalidate_data_sector
* Returned Value   :  error_code
* Comments  :
*     Invalidates the cached sector.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Invalidate_data_sector
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr   /*[IN] the drive on which to operate */
    )
{
#if MFSCFG_READ_ONLY
    return MFS_NO_ERROR;
#else

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (drive_ptr))
    {
        return MFS_NO_ERROR;
    }
#endif

    drive_ptr->DATA_SECTOR_NUMBER = MAX_UINT_32;
    drive_ptr->DATA_SECTOR_DIRTY=FALSE;
    return MFS_NO_ERROR;
#endif
}  



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Read_data_sector
* Returned Value   :  error_code
* Comments  :
*     Readsone sector into the DATA_SECTOR buffer
*END*---------------------------------------------------------------------*/
_mfs_error MFS_Read_data_sector
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,
    MFS_HANDLE_PTR          handle,         /*[IN]  handle of the file for which we are doing the read/write  */
    uint32_t                 sector_number,  /*[IN] sector number to read/write from/to file system medium */
    bool                 hw_read         /*[IN] if read of a sector is needed or just validation of a new one */
    )
{
    _mfs_error error;

    error = MFS_NO_ERROR;

    if ( sector_number != drive_ptr->DATA_SECTOR_NUMBER )
    {
        MFS_Flush_data_sector_buffer(drive_ptr);
        MFS_Invalidate_data_sector(drive_ptr);

        if (hw_read)
        {
            error = MFS_Read_device_sector(drive_ptr,sector_number,drive_ptr->DATA_SECTOR_PTR);
        }
        else
        {
            error = MFS_NO_ERROR;
        }

        if ( error == MFS_NO_ERROR )
        {
            drive_ptr->DATA_SECTOR_NUMBER =  sector_number;
        }
    }

    return(error);
}
