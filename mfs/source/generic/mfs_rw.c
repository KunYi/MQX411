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

#include <string.h>
#include <mqx.h>
#include <fio.h>
#if MQX_USE_IO_OLD
#include <ioctl.h>
#else
#include <nio.h>
#include <nio/ioctl.h>
#include <fcntl.h>
#include "errno.h" /* Workaround for uv4 to set including */
#endif

#include "mfs.h"
#include "mfs_prv.h"
#include "part_mgr.h"

#if !MFSCFG_READ_ONLY

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Write_device_sectors
* Returned Value   :  error_code
* Comments  :
*     Reads or writes consecutive sectors.
*END*---------------------------------------------------------------------*/
_mfs_error MFS_Write_device_sectors
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,
    uint32_t                 sector_number,  /*[IN] first sector to read/write from/to file system medium */
    uint32_t                 sector_count,   /*[IN] number of sectors to read/write from/to file system medium */
    uint32_t                 max_retries,    /*[IN] number of retries of the same low level operation if it fails */
    char                *buffer_ptr,     /*[IN/OUT] address of where data is to be stored/written */
    uint32_t             *processed       /*[OUT] number of sector successfully processed */
    )
{
    uint32_t    attempts;
    int32_t     num, expect_num, seek_loc, shifter;
    char   *data_ptr;
    _mfs_error error;

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (drive_ptr))
    {
        return MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif

    error = MFS_NO_ERROR;

    MFS_LOG(printf("MFS_Write_device_sector %d\n", sector_number));

    if ( sector_number > drive_ptr->BPB.MEGA_SECTORS )
    {
        return(MFS_SECTOR_NOT_FOUND);
    }

    if ( drive_ptr->BLOCK_MODE )
    {
        shifter    = 0;
        seek_loc   = sector_number;
        expect_num = sector_count;
    }
    else
    {
        shifter    = drive_ptr->SECTOR_POWER;
        seek_loc   = sector_number << shifter;
        expect_num = sector_count << shifter;
    }

#if MQX_USE_IO_OLD
    fseek(drive_ptr->DEV_FILE_PTR, seek_loc, IO_SEEK_SET);
#else
    lseek(drive_ptr->DEV_FILE_PTR, seek_loc, SEEK_SET);
#endif
    //TODO: check errno lseek

    data_ptr = buffer_ptr;
    attempts = 0;
    while ( expect_num > 0 && attempts <= max_retries)
    {
        num = write(drive_ptr->DEV_FILE_PTR, data_ptr, expect_num);
#if MQX_USE_IO_OLD
        if ( num == IO_ERROR )
#else
        if ( num < 0 )
#endif
        {
            error = MFS_WRITE_FAULT;
            break;
        }
        if ( num > 0 )
        {
            expect_num -= num;
            data_ptr += num << (drive_ptr->SECTOR_POWER - shifter);
            attempts = 0; /* there is a progress, reset attempts counter */
        }
        attempts++;
    }

    if ( expect_num > 0 )
    {
#if MQX_USE_IO_OLD
        error = drive_ptr->DEV_FILE_PTR->ERROR;
#else
        error = errno;
#endif
    }
    else if (drive_ptr->READBACK_SECTOR_PTR)
    {
#if MQX_USE_IO_OLD
        fseek(drive_ptr->DEV_FILE_PTR, seek_loc, IO_SEEK_SET);
#else
        lseek(drive_ptr->DEV_FILE_PTR, seek_loc, SEEK_SET);
#endif
        //TODO: check errno lseek

        data_ptr = buffer_ptr;
        expect_num = sector_count << shifter;
        while ( expect_num > 0 )
        {
            num = read(drive_ptr->DEV_FILE_PTR, drive_ptr->READBACK_SECTOR_PTR, 1 << shifter);
            if ( num != (1 << shifter) )
            {
#if MQX_USE_IO_OLD
                error = drive_ptr->DEV_FILE_PTR->ERROR;
#else
                error = errno;
#endif
                break;
            }
            if ( memcmp(data_ptr, drive_ptr->READBACK_SECTOR_PTR, drive_ptr->BPB.SECTOR_SIZE) != 0 )
            {
                error = MFS_WRITE_FAULT;
                break;
            }
            expect_num -= num;
            data_ptr += num << (drive_ptr->SECTOR_POWER - shifter);
        }
    }

    if (error == MFS_NO_ERROR && expect_num)
    {
        /* Ensure that error code is always set if less than requested data was written */
        error = MFS_WRITE_FAULT;
    }

    if (processed)
        *processed = ((sector_count<<shifter) - expect_num) >> shifter;

    return error;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Write_device_sector
* Returned Value   :  error_code
* Comments  :
*     Write one sector to the device
*END*---------------------------------------------------------------------*/
_mfs_error MFS_Write_device_sector
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,
    uint32_t                 sector_number,  /*[IN] sector number to read/write from/to file system medium */
    char                *sector_ptr      /*[IN/OUT] address of where data is to be stored/written */
    )
{
    return MFS_Write_device_sectors(drive_ptr, sector_number, 1, MFSCFG_MAX_WRITE_RETRIES, sector_ptr, NULL);
}

#endif //!MFSCFG_READ_ONLY


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Read_device_sectors
* Returned Value   :  error_code
* Comments  :
*     Reads consecutive sectors into given buffer
*END*---------------------------------------------------------------------*/
_mfs_error MFS_Read_device_sectors
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,
    uint32_t                 sector_number,  /*[IN] first sector to read/write from/to file system medium */
    uint32_t                 sector_count,   /*[IN] number of sectors to read/write from/to file system medium */
    uint32_t                 max_retries,    /*[IN] number of retries of the same low level operation if it fails */
    char                *buffer_ptr,     /*[IN/OUT] address of where data is to be stored/written */
    uint32_t             *processed       /*[OUT] number of sector successfully processed */
    )
{
    uint32_t    attempts;
    int32_t     num, expect_num, shifter, seek_loc;
    _mfs_error error;

    error = MFS_NO_ERROR;

    if ( sector_number+sector_count-1 > drive_ptr->BPB.MEGA_SECTORS )
    {
        return(MFS_SECTOR_NOT_FOUND);
    }

    MFS_LOG(printf("MFS_Read_device_sectors %d %d", sector_number, sector_count));

    if ( drive_ptr->BLOCK_MODE )
    {
        shifter    = 0;
        seek_loc   = sector_number;
        expect_num = sector_count;
    }
    else
    {
        shifter    = drive_ptr->SECTOR_POWER;
        seek_loc   = sector_number << shifter;
        expect_num = sector_count << shifter;
    }

#if MQX_USE_IO_OLD
    fseek(drive_ptr->DEV_FILE_PTR, seek_loc, IO_SEEK_SET);
#else
    lseek(drive_ptr->DEV_FILE_PTR, seek_loc, SEEK_SET);
#endif
    //TODO: check errno lseek

    attempts = 0;
    while ( expect_num > 0 && attempts <= max_retries )
    {
        num = read(drive_ptr->DEV_FILE_PTR, buffer_ptr, expect_num);
#if MQX_USE_IO_OLD
        if ( num == IO_ERROR )
#else
        if ( num < 0 )
#endif
        {
            error = MFS_READ_FAULT;
            break;
        }
        else if ( num > 0 )
        {
            expect_num -= num;
            buffer_ptr += num << (drive_ptr->SECTOR_POWER - shifter);
            attempts = 0; /* there is a progress, reset attempts counter */
        }
        attempts++;
    }

    if ( expect_num > 0 )
    {
#if MQX_USE_IO_OLD
        error = drive_ptr->DEV_FILE_PTR->ERROR;
#else
        error = errno;
#endif
    }

    if (error == MFS_NO_ERROR && expect_num)
    {
        /* Ensure that error code is always set if less than requested data was read */
        error = MFS_READ_FAULT;
    }

    if (processed)
        *processed = ((sector_count<<shifter) - expect_num) >> shifter;

    return error;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Read_device_sector
* Returned Value   :  error_code
* Comments  :
*     Reads one sector from the device
*END*---------------------------------------------------------------------*/
_mfs_error MFS_Read_device_sector
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,
    uint32_t                 sector_number,  /*[IN] sector number to read/write from/to file system medium */
    char                *sector_ptr      /*[IN/OUT] address of where data is to be stored/written */
    )
{
    return MFS_Read_device_sectors(drive_ptr, sector_number, 1, MFSCFG_MAX_READ_RETRIES, sector_ptr, NULL);
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Invalidate_directory_sector
* Returned Value   :  error_code
* Comments  :
*     Invalidates the cached sector.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Invalidate_directory_sector
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

    drive_ptr->DIR_SECTOR_NUMBER = MAX_UINT_32;
    drive_ptr->DIR_SECTOR_DIRTY=FALSE;
    return MFS_NO_ERROR;
#endif
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Flush_directory_sector_buffer
* Returned Value   :  MFS error code
* Comments  :
*   Write the sector buffer back to the disk.
*   Assumes the semaphore is already obtained.
*END*---------------------------------------------------------------------*/
_mfs_error MFS_Flush_directory_sector_buffer
    (
    MFS_DRIVE_STRUCT_PTR  drive_ptr
    )
{
#if MFSCFG_READ_ONLY
    return MFS_NO_ERROR;
#else //MFSCFG_READ_ONLY
    _mfs_error   error_code= MFS_NO_ERROR;

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (drive_ptr))
    {
        return error_code;
    }
#endif

    if ( drive_ptr->DIR_SECTOR_DIRTY )
    {
        error_code = MFS_Write_device_sector(drive_ptr,drive_ptr->DIR_SECTOR_NUMBER,drive_ptr->DIR_SECTOR_PTR);
        drive_ptr->DIR_SECTOR_DIRTY = FALSE;
    }

    return(error_code);
#endif //MFSCFG_READ_ONLY
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Read_directory_sector
* Returned Value   :  MFS pointer
* Comments  :
*   Reads ONE sector in the sector_buffer.
*   This function cannot read any sectors before the root directory.
*   The semaphore is assumed to be obtained.
*
*END*---------------------------------------------------------------------*/

void *MFS_Read_directory_sector
    (
    MFS_DRIVE_STRUCT_PTR  drive_ptr,
    uint32_t         cluster,        /*[IN] number of the cluster containing the sector to be read */
    uint16_t         sector,         /*[IN] index of the sector within the cluster */
    _mfs_error_ptr  error_ptr
    )
{
    uint32_t   abs_sector;

    if ( cluster == 0 )
    {
        /*
        ** Reading the root directory
        */
        abs_sector = drive_ptr->ROOT_START_SECTOR+sector;
        /*
        ** Overflow...
        */
        if ( abs_sector >= drive_ptr->DATA_START_SECTOR )
        {
            *error_ptr = MFS_Flush_directory_sector_buffer(drive_ptr);
            drive_ptr->DIR_SECTOR_NUMBER = 0;
            return(NULL);
        }
    }
    else
    {
        abs_sector = CLUSTER_TO_SECTOR(cluster) + sector;
    }

    if ( abs_sector > drive_ptr->BPB.MEGA_SECTORS )
    {
        *error_ptr = MFS_Flush_directory_sector_buffer(drive_ptr);
        return(NULL);
    }

    if ( abs_sector != drive_ptr->DIR_SECTOR_NUMBER )
    {
        *error_ptr = MFS_Flush_directory_sector_buffer(drive_ptr);
        if ( *error_ptr )
        {
            return(NULL);
        }
        *error_ptr = MFS_Read_device_sector(drive_ptr, abs_sector, drive_ptr->DIR_SECTOR_PTR);
        if ( *error_ptr )
        {
            MFS_Invalidate_directory_sector(drive_ptr);
            return( NULL );
        }
        drive_ptr->DIR_SECTOR_NUMBER = abs_sector;
    }
    else
    {
        *error_ptr = MFS_NO_ERROR;
    }

    return(drive_ptr->DIR_SECTOR_PTR);
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Flush_caches
* Returned Value   : error code
* Comments  :
*     Turns the write cache on or off.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Flush_caches
    (
    MFS_DRIVE_STRUCT_PTR  drive_ptr
    )
{
#if MFSCFG_READ_ONLY
    return MFS_NO_ERROR;
#else //MFSCFG_READ_ONLY
    _mfs_error        error_code, return_code = MFS_NO_ERROR;
    MQX_FILE_PTR      dev_file_ptr = drive_ptr->DEV_FILE_PTR;
#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (drive_ptr))
    {
        return return_code;
    }
#endif

    return_code = MFS_Flush_directory_sector_buffer(drive_ptr);
    error_code = MFS_Flush_data_sector_buffer(drive_ptr);
    if ( return_code == MFS_NO_ERROR ) return_code = error_code;
    error_code = MFS_Flush_fat_cache(drive_ptr);
    if ( return_code == MFS_NO_ERROR ) return_code = error_code;
    if (NULL != dev_file_ptr) {
        error_code = ioctl(dev_file_ptr, IO_IOCTL_FLUSH_OUTPUT, NULL);
        if ((IO_ERROR_INVALID_IOCTL_CMD != error_code) && ( return_code == MFS_NO_ERROR )) return_code = error_code;
    }
    return(return_code);
#endif //MFSCFG_READ_ONLY
}
