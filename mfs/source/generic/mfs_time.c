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
* Function Name    : MFS_Get_date_time
* Returned Value   : error_code (MFS_NO_ERROR or MFS_INVALID_FILE_HANDLE)
* Comments  :   Get the date and time of last update of a specific file.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Get_date_time
    (
        MFS_DRIVE_STRUCT_PTR drive_ptr,
        
        MFS_HANDLE_PTR       handle,
        
        uint16_t             *date_ptr,   /*[IN/OUT] file date is written to this address */ 

        uint16_t             *time_ptr    /*[IN/OUT] file time is written to this address */
    )
{
   
   _mfs_error              error_code;
   MFS_DIR_ENTRY_PTR       dir_entry_ptr;
   
   error_code = MFS_lock_dos_disk( drive_ptr );
   if ( error_code != MFS_NO_ERROR )
   {
      return error_code;
   }
   
   dir_entry_ptr = ((MFS_DIR_ENTRY_PTR) drive_ptr->DIR_SECTOR_PTR) + INDEX_TO_OFFSET(handle->DIR_INDEX);
   /*
   ** Check the date and time ptrs; if any is NULL, don't write to it.
   */
   if ( date_ptr )
   {
      *date_ptr = mqx_dtohs(dir_entry_ptr->DATE);
   }
   if ( time_ptr )
   {
      *time_ptr = mqx_dtohs(dir_entry_ptr->TIME);
   }
   
   MFS_unlock(drive_ptr,FALSE);
   
   return(error_code);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Set_date_time
* Returned Value   : error_code
* Comments  :   Set the date and time of last update of a specific file.
*
*END*---------------------------------------------------------------------*/
#if !MFSCFG_READ_ONLY

_mfs_error MFS_Set_date_time
    (
        MFS_DRIVE_STRUCT_PTR drive_ptr,
        
        MFS_HANDLE_PTR       handle,
        
        uint16_t             *date_ptr,   /*[IN] file date to be written into this file's entry */
        uint16_t             *time_ptr    /*[IN] file time to be written into this file's entry */
    )
{
   _mfs_error              error_code;
   
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

   if ( handle->ACCESS == MFS_ACCESS_READ_ONLY )
   {
      MFS_unlock(drive_ptr,FALSE);
      return MFS_ACCESS_DENIED;
   }
   
   mqx_htods(handle->DIR_ENTRY.DATE, *date_ptr);
   mqx_htods(handle->DIR_ENTRY.TIME, *time_ptr);

   error_code = MFS_Update_entry(drive_ptr, handle);
   /* we do not want to set date and time in close, so set touched to 0 */
   if ( error_code == MFS_NO_ERROR )
   {
      handle->TOUCHED = 0;
   }
   MFS_unlock(drive_ptr,TRUE);
   
   return(error_code);
}  

#endif

