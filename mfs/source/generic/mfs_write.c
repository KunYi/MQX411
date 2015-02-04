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



#if !MFSCFG_READ_ONLY

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Write
* Returned Value   : number of bytes writen
* Comments  :   Write a specific number of bytes to an open file.
*
*END*---------------------------------------------------------------------*/

uint32_t MFS_Write
    (
        MFS_HANDLE_PTR handle,
        
        MFS_DRIVE_STRUCT_PTR drive_ptr,
      
        uint32_t            num_bytes,      /*[IN] number of bytes to be written */ 

        char                *buffer_address, /*[IN/OUT] bytes are written from this buffer */

        _mfs_error_ptr      error_ptr       /*[IN/OUT] error code is written to this address */
    )
{
    uint32_t                 bytes_written;
    uint32_t                 copy_size;
    uint32_t                 cluster_offset;
    uint32_t                 sector_number, sector_index;
    uint32_t                 sector_offset;
    uint32_t                 whole_sectors;
    uint32_t                 cont_sectors;
    uint32_t                 proc_sectors;
    _mfs_error              error, temp_error;
    uint32_t                 file_size;
    uint32_t                 next_cluster;
    bool                 need_hwread;
    uint32_t                 location;
    uint32_t                 num_zeros;
    uint32_t                 zeros_written;
    uint32_t                 zero_size;

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (drive_ptr))
    {
        MFS_set_error_and_return(error_ptr,MFS_DISK_IS_WRITE_PROTECTED,0);
    }
#endif

    if ( buffer_address == NULL )
    {
        MFS_set_error_and_return(error_ptr,MFS_INVALID_PARAMETER,0);
    }

    if ( num_bytes == 0 )
    {
        MFS_set_error_and_return(error_ptr,MFS_NO_ERROR,0);
    }

    error = MFS_lock_dos_disk( drive_ptr );
    if ( error != MFS_NO_ERROR )
    {
        MFS_set_error_and_return(error_ptr,error,0);
    }

    if ( handle->ACCESS == MFS_ACCESS_READ_ONLY )
    {
        MFS_unlock(drive_ptr,FALSE);
        MFS_set_error_and_return(error_ptr,MFS_ACCESS_DENIED,0);
    }

    /*
    ** Setup the current cluster. If this is the first time writing to the file, a cluster needs to be added.
    */
    if ( handle->CURRENT_CLUSTER == 0 )
    {
        handle->PREVIOUS_CLUSTER = 0;
        handle->CURRENT_CLUSTER = clustoh(handle->DIR_ENTRY.HFIRST_CLUSTER, handle->DIR_ENTRY.LFIRST_CLUSTER);
        if ( handle->CURRENT_CLUSTER==0 )
        {
            next_cluster = MFS_Find_unused_cluster_from(drive_ptr,drive_ptr->NEXT_FREE_CLUSTER);
            if ( next_cluster != CLUSTER_INVALID )
            {
                clustod(handle->DIR_ENTRY.HFIRST_CLUSTER, handle->DIR_ENTRY.LFIRST_CLUSTER, next_cluster);
                handle->TOUCHED = 1;

                error = MFS_Put_fat(drive_ptr, next_cluster, CLUSTER_EOF);
                if ( error == MFS_NO_ERROR )
                {
                    handle->CURRENT_CLUSTER = next_cluster;
                }
                else
                {
                    MFS_unlock(drive_ptr,FALSE);
                    MFS_set_error_and_return(error_ptr,error,0);
                }
            }
            else
            {
                MFS_unlock(drive_ptr,FALSE);
                MFS_set_error_and_return(error_ptr,MFS_DISK_FULL,0);
            }
        }
    }
    else if ( handle->CURRENT_CLUSTER == CLUSTER_EOF )
    {
        error = MFS_Add_cluster_to_chain(drive_ptr, handle->PREVIOUS_CLUSTER, &handle->CURRENT_CLUSTER);
        if ( MFS_NO_ERROR != error )
        {
            MFS_unlock(drive_ptr,FALSE);
            MFS_set_error_and_return(error_ptr,error,0);
        }
    }
    else if ( handle->CURRENT_CLUSTER > drive_ptr->LAST_CLUSTER )
    {
        MFS_unlock(drive_ptr,FALSE);
        MFS_set_error_and_return(error_ptr,MFS_DISK_FULL,0);
    }

    /* Make sure location (local variable) never points behind the end of file */
    file_size = mqx_dtohl(handle->DIR_ENTRY.FILE_SIZE);
    location = (handle->LOCATION > file_size) ? file_size : handle->LOCATION;

    /* Calculate sector number and offsets within cluster and sector */
    cluster_offset = OFFSET_WITHIN_CLUSTER(location);
    sector_index = CLUSTER_OFFSET_TO_SECTOR(cluster_offset);
    sector_number = CLUSTER_TO_SECTOR(handle->CURRENT_CLUSTER) + sector_index;
    sector_offset = OFFSET_WITHIN_SECTOR(location);

    /* Calculate possible gap to fill in by zeros if writing behind the end of file */
    num_zeros = handle->LOCATION - location;

    zeros_written = 0;
    bytes_written = 0;

    /* Write zeros to fill in gap if LOCATION points behind the end of file */
    while (zeros_written < num_zeros)
    {
        /* If offset is non-zero, then reading the data is required */
        error = MFS_Read_data_sector(drive_ptr, handle, sector_number, sector_offset != 0);
        if ( error != MFS_NO_ERROR )
            break;

        /* Zero the buffer  */
        zero_size = min(num_zeros-zeros_written, drive_ptr->BPB.SECTOR_SIZE-sector_offset);
        _mem_zero(&drive_ptr->DATA_SECTOR_PTR[sector_offset], zero_size);
        drive_ptr->DATA_SECTOR_DIRTY = TRUE;

        if ( drive_ptr->WRITE_CACHE_POLICY == MFS_WRITE_THROUGH_CACHE )
        {
            error = MFS_Flush_data_sector_buffer(drive_ptr);
            if ( error != MFS_NO_ERROR )
                break;
        }

        zeros_written += zero_size;
        sector_offset += zero_size;

        /*
        ** Check to see if we need to advance to the next sector, which has
        ** the side effect of increasing the cluster number if required.
        */
        if ( sector_offset >= drive_ptr->BPB.SECTOR_SIZE )
        {
            temp_error = MFS_next_data_sector(drive_ptr, handle, &sector_index, &sector_number);
            if (temp_error == MFS_EOF)
            {
                /* Always allocate new cluster, there is something to be written for sure */
                error = MFS_Add_cluster_to_chain(drive_ptr, handle->PREVIOUS_CLUSTER, &handle->CURRENT_CLUSTER);
                if ( MFS_NO_ERROR == error )
                {
                    sector_number = CLUSTER_TO_SECTOR(handle->CURRENT_CLUSTER);
                    sector_index = 0;
                }

            }
            else
            {
                error = temp_error;
            }
            sector_offset = 0;
        }
    }

    /* Write partial sector if sector_offset is non-zero */
    if ((sector_offset != 0) && (error == MFS_NO_ERROR))
    {
        /* Offset is non-zero, reading the data is required */
        error = MFS_Read_data_sector(drive_ptr, handle, sector_number, TRUE);

        if (error == MFS_NO_ERROR)
        {
            /* The requested lenght of data may span the sector to it's end  */
            copy_size = min(num_bytes, drive_ptr->BPB.SECTOR_SIZE-sector_offset);

            _mem_copy(buffer_address, &drive_ptr->DATA_SECTOR_PTR[sector_offset], copy_size);
            drive_ptr->DATA_SECTOR_DIRTY = TRUE;

            if (drive_ptr->WRITE_CACHE_POLICY == MFS_WRITE_THROUGH_CACHE)
            {
                error = MFS_Flush_data_sector_buffer(drive_ptr);
            }

            if (error == MFS_NO_ERROR)
            {
                bytes_written = copy_size;

                /*
                ** Check to see if we need to advance to the next sector, which has
                ** the side effect of increasing the cluster number if required.
                */
                if ((sector_offset + bytes_written) >= drive_ptr->BPB.SECTOR_SIZE)
                {
                    temp_error = MFS_next_data_sector(drive_ptr, handle, &sector_index, &sector_number);
                    /* Only an error if we are not done writing and can't extend the chain */
                    if (bytes_written < num_bytes)
                    {
                        if (temp_error == MFS_EOF)
                        {
                            /* Allocate new cluster */
                            error = MFS_Add_cluster_to_chain(drive_ptr, handle->PREVIOUS_CLUSTER, &handle->CURRENT_CLUSTER);
                            /* Update sector_number and index unconditionally - if there was an error the value is never used anyways. */
                            sector_number = CLUSTER_TO_SECTOR(handle->CURRENT_CLUSTER);
                            sector_index = 0;
                        }
                        else
                        {
                            error = temp_error;
                        }
                    }
                }
            }
        }
    }

    /* Check whether the application buffer is properly aligned */
    if ((((uint32_t)buffer_address+bytes_written) & drive_ptr->ALIGNMENT_MASK) == 0)
    {
        /* Yes, use zero copy approach */
        whole_sectors = (num_bytes - bytes_written) >> drive_ptr->SECTOR_POWER;
        while ((whole_sectors > 0) && (error == MFS_NO_ERROR))
        {
            cont_sectors = drive_ptr->BPB.SECTORS_PER_CLUSTER - sector_index;
            if (cont_sectors > whole_sectors)
                cont_sectors = whole_sectors;

            error = MFS_Write_device_sectors(drive_ptr, sector_number, cont_sectors, MFSCFG_MAX_WRITE_RETRIES, buffer_address+bytes_written, &proc_sectors);

            if (proc_sectors > 0)
            {
                bytes_written += proc_sectors * drive_ptr->BPB.SECTOR_SIZE;
                whole_sectors -= proc_sectors;

                /* Advance to next unprocessed sector */
                sector_index += proc_sectors - 1;
                temp_error = MFS_next_data_sector(drive_ptr, handle, &sector_index, &sector_number);
                /* Go on only if we are not done writing yet */
                if ((error == MFS_NO_ERROR) && (bytes_written < num_bytes))
                {
                    if (temp_error == MFS_EOF)
                    {
                        /* Allocate new cluster */
                        error = MFS_Add_cluster_to_chain(drive_ptr, handle->PREVIOUS_CLUSTER, &handle->CURRENT_CLUSTER);
                        /* Update sector_number and index unconditionally - if there was an error the value is never used anyways. */
                        sector_number = CLUSTER_TO_SECTOR(handle->CURRENT_CLUSTER);
                        sector_index = 0;

                    }
                    else
                    {
                        error = temp_error;
                    }
                }
            }
        }
    }
    else
    {
        /* No, copy through aligned buffer has to be used */
        whole_sectors = (num_bytes - bytes_written) >> drive_ptr->SECTOR_POWER;
        while ((whole_sectors > 0) && (error == MFS_NO_ERROR))
        {
            error = MFS_Read_data_sector(drive_ptr, handle, sector_number, FALSE);
            if (error != MFS_NO_ERROR)
                break;

            _mem_copy(buffer_address+bytes_written, &drive_ptr->DATA_SECTOR_PTR[0], drive_ptr->BPB.SECTOR_SIZE);
            drive_ptr->DATA_SECTOR_DIRTY = TRUE;
            if (drive_ptr->WRITE_CACHE_POLICY == MFS_WRITE_THROUGH_CACHE)
            {
                error = MFS_Flush_data_sector_buffer(drive_ptr);
                if (error != MFS_NO_ERROR)
                    break;
            }

            bytes_written += drive_ptr->BPB.SECTOR_SIZE;
            whole_sectors--;

            /* Advance to next sector */
            temp_error = MFS_next_data_sector(drive_ptr, handle, &sector_index, &sector_number);
            /* Handle error/EOF only if we are not done writing */
            if (bytes_written < num_bytes)
            {
                if (temp_error == MFS_EOF)
                {
                    /* Allocate new cluster */
                    error = MFS_Add_cluster_to_chain(drive_ptr, handle->PREVIOUS_CLUSTER, &handle->CURRENT_CLUSTER);
                    /* Update sector_number and index unconditionally - if there was an error the value is never used anyways. */
                    sector_number = CLUSTER_TO_SECTOR(handle->CURRENT_CLUSTER);
                    sector_index = 0;
                }
                else
                {
                    error = temp_error;
                }
            }
        }
    }

    /* write last missing data */
    if ( (bytes_written < num_bytes) && (error == MFS_NO_ERROR) )
    {
        /* reading of data required only if we are not going to overwrite the file to the end */
        need_hwread = (handle->LOCATION + num_bytes) < file_size;
        error = MFS_Read_data_sector(drive_ptr, handle, sector_number, need_hwread);

        if ( error == MFS_NO_ERROR )
        {
            _mem_copy(buffer_address+bytes_written, &drive_ptr->DATA_SECTOR_PTR[0], num_bytes-bytes_written);
            bytes_written = num_bytes;
            drive_ptr->DATA_SECTOR_DIRTY = TRUE;
            if ( drive_ptr->WRITE_CACHE_POLICY == MFS_WRITE_THROUGH_CACHE )
            {
                error = MFS_Flush_data_sector_buffer(drive_ptr);
            }
        }
    }

    if ( zeros_written || bytes_written )
    {
        handle->TOUCHED = 1;
    }

    location += zeros_written + bytes_written;
    handle->SAVED_POSITION = location;

    if ( location > file_size )
    {
        handle->SIZE = location;
        mqx_htodl(handle->DIR_ENTRY.FILE_SIZE, handle->SIZE);
        if ( (drive_ptr->WRITE_CACHE_POLICY == MFS_WRITE_THROUGH_CACHE) && (error == MFS_NO_ERROR) )
        {
            error = MFS_Update_entry(drive_ptr, handle);
        }
    }

    /* LOCATION moves forward only by actual data written, the zero fill does not count */
    handle->LOCATION += bytes_written;

    temp_error = MFS_unlock(drive_ptr, FALSE);
    if (error == MFS_NO_ERROR)
        error = temp_error;

    MFS_set_error_and_return(error_ptr, error, bytes_written);
}

#endif

