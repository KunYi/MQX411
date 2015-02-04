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
* Function Name    : MFS_next_data_sector
* Returned Value   :
* Comments  :
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_next_data_sector(
    MFS_DRIVE_STRUCT_PTR    drive_ptr,
    MFS_HANDLE_PTR          handle,
    uint32_t             *sector_index_ptr,
    uint32_t             *sector_number_ptr
    )
{
    _mfs_error  error = MFS_NO_ERROR;
    uint32_t     next_cluster;

    if ( handle->CURRENT_CLUSTER != CLUSTER_EOF )
    {
        (*sector_index_ptr)++;
        if ( *sector_index_ptr >= drive_ptr->BPB.SECTORS_PER_CLUSTER )
        {
            // New cluster
            error = MFS_get_cluster_from_fat(drive_ptr, handle->CURRENT_CLUSTER, &next_cluster);
            if ( error == MFS_NO_ERROR )
            {
                handle->PREVIOUS_CLUSTER = handle->CURRENT_CLUSTER;
                handle->CURRENT_CLUSTER = next_cluster;
                *sector_index_ptr = 0;

                if ( next_cluster == CLUSTER_EOF )
                {
                    error = MFS_EOF;
                }
            }
        }

        if ( error == MFS_NO_ERROR )
        {
            *sector_number_ptr = CLUSTER_TO_SECTOR(handle->CURRENT_CLUSTER) + *sector_index_ptr;
        }
    }
    else
    {
        error = MFS_EOF;
    }
    return error;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Read
* Returned Value   : number of bytes read
* Comments  :   Read a specific number of bytes from an open file.
*
*END*---------------------------------------------------------------------*/


uint32_t  MFS_Read
    (
    MFS_HANDLE_PTR          handle, /*[IN] file handle upon which the action is to be taken */
    MFS_DRIVE_STRUCT_PTR    drive_ptr,
    uint32_t                num_bytes,      /*[IN] number of bytes to be read */
    char                    *buffer_address, /*[IN/OUT] bytes are read into this buffer */
    _mfs_error_ptr          error_ptr       /*[IN/OUT] error code is written to this address */
    )
{
    uint32_t                 location;
    uint32_t                 bytes_read;
    uint32_t                 bytes_left_in_file;
    uint32_t                 copy_size;
    uint32_t                 cluster_offset;
    uint32_t                 sector_number, sector_index;
    uint32_t                 sector_offset;
    uint32_t                 whole_sectors;
    uint32_t                 cont_sectors;
    uint32_t                 proc_sectors;
    _mfs_error               error, temp_error;
    bool                     eof_reached;
    uint32_t                 file_size;

    if ( num_bytes == 0 )
    {
        MFS_set_error_and_return(error_ptr,MFS_NO_ERROR,0);
    }

    if ( buffer_address == NULL )
    {
        MFS_set_error_and_return(error_ptr,MFS_INVALID_PARAMETER,0);
    }

    error = MFS_lock_dos_disk(drive_ptr);
    if ( error != MFS_NO_ERROR )
    {
        MFS_set_error_and_return(error_ptr,error,0);
    }

    if ( handle->ACCESS != MFS_ACCESS_READ_ONLY &&
        handle->ACCESS != MFS_ACCESS_READ_WRITE )
    {
        MFS_unlock(drive_ptr,FALSE);
        MFS_set_error_and_return(error_ptr,MFS_ACCESS_DENIED,0);
    }


    if ( handle->CURRENT_CLUSTER == 0 )
    {
        handle->PREVIOUS_CLUSTER = 0;
        handle->CURRENT_CLUSTER = clustoh(handle->DIR_ENTRY.HFIRST_CLUSTER, handle->DIR_ENTRY.LFIRST_CLUSTER);
    }
    else if ( handle->CURRENT_CLUSTER == CLUSTER_EOF )
    {
        MFS_unlock(drive_ptr,FALSE);
        MFS_set_error_and_return(error_ptr,MFS_EOF,0);
    }


    bytes_read = 0;
    eof_reached = FALSE;
    location = handle->LOCATION;

    /*
    ** Can't read past file size
    */
    file_size = mqx_dtohl(handle->DIR_ENTRY.FILE_SIZE);
    if ( location > file_size )
    {
        location = file_size;
    }
    bytes_left_in_file = file_size - location;
    if ( num_bytes > bytes_left_in_file )
    {
        eof_reached = TRUE;
        num_bytes = bytes_left_in_file;
    }

    if ( bytes_left_in_file )
    {
        /*
        ** Read the number of bytes from the current file
        ** position to the end of the current cluster
        */
        cluster_offset = OFFSET_WITHIN_CLUSTER(location);
        sector_index = CLUSTER_OFFSET_TO_SECTOR(cluster_offset);
        sector_number = CLUSTER_TO_SECTOR(handle->CURRENT_CLUSTER) + sector_index;
        sector_offset = OFFSET_WITHIN_SECTOR(location);

        /* Read partial sector if sector_offet is non-zero */
        if (sector_offset != 0)
        {
            error = MFS_Read_data_sector(drive_ptr, handle, sector_number, TRUE);
            if ( error == MFS_NO_ERROR )
            {
                /* The requested lenght of data may span the sector to it's end  */
                copy_size = min(num_bytes, drive_ptr->BPB.SECTOR_SIZE-sector_offset);

                _mem_copy(&drive_ptr->DATA_SECTOR_PTR[sector_offset], buffer_address, copy_size);
                bytes_read=copy_size;

                /*
                ** Check to see if we need to advance to the next sector, which has
                ** the side effect of increasing the cluster number if required.
                */
                if ( (sector_offset+bytes_read) >= drive_ptr->BPB.SECTOR_SIZE )
                {
                    temp_error = MFS_next_data_sector(drive_ptr, handle, &sector_index, &sector_number);
                    /* Only an error if we are not done reading */
                    if ( bytes_read<num_bytes )
                    {
                        error = temp_error;
                    }
                }
            }
        }

        /* Check whether the application buffer is properly aligned */
        if ((((uint32_t)buffer_address+bytes_read) & drive_ptr->ALIGNMENT_MASK) == 0)
        {
            /* Yes, use zero copy approach */
            whole_sectors = (num_bytes - bytes_read) >> drive_ptr->SECTOR_POWER;
            while ((whole_sectors > 0) && (error == MFS_NO_ERROR))
            {
                cont_sectors = drive_ptr->BPB.SECTORS_PER_CLUSTER - sector_index;
                if (cont_sectors > whole_sectors)
                    cont_sectors = whole_sectors;

                error = MFS_Read_device_sectors(drive_ptr, sector_number, cont_sectors, MFSCFG_MAX_READ_RETRIES, buffer_address+bytes_read, &proc_sectors);

                if (proc_sectors > 0)
                {
                    bytes_read += proc_sectors * drive_ptr->BPB.SECTOR_SIZE;
                    whole_sectors -= proc_sectors;

                    /* Advance to next unprocessed sector */
                    sector_index += proc_sectors - 1;
                    temp_error = MFS_next_data_sector(drive_ptr, handle, &sector_index, &sector_number);
                    /* Only an error if we are not done reading */
                    if ((error==MFS_NO_ERROR) && (bytes_read<num_bytes))
                    {
                        error = temp_error;
                    }
                }
            }
        }
        else
        {
            /* No, copy through aligned buffer has to be used */
            whole_sectors = (num_bytes - bytes_read) >> drive_ptr->SECTOR_POWER;
            while ((whole_sectors > 0) && (error == MFS_NO_ERROR))
            {
                error = MFS_Read_data_sector(drive_ptr, handle, sector_number, TRUE);
                if (error == MFS_NO_ERROR)
                {
                    _mem_copy(&drive_ptr->DATA_SECTOR_PTR[0], buffer_address+bytes_read, drive_ptr->BPB.SECTOR_SIZE);
                    bytes_read += drive_ptr->BPB.SECTOR_SIZE;
                    whole_sectors--;
                    temp_error = MFS_next_data_sector(drive_ptr, handle, &sector_index, &sector_number);
                    /* Only an error if we are not done reading */
                    if (bytes_read<num_bytes)
                    {
                        error = temp_error;
                    }
                }
            }
        }

        /* Read the tail (last incomplete sector) if any */
        if ( (bytes_read < num_bytes) && (error == MFS_NO_ERROR) )
        {
            error = MFS_Read_data_sector(drive_ptr, handle, sector_number, TRUE);
            if ( error == MQX_OK )
            {
                _mem_copy(&drive_ptr->DATA_SECTOR_PTR[0], buffer_address+bytes_read, num_bytes-bytes_read);
                bytes_read = num_bytes;
            }
        }

        handle->LOCATION = location + bytes_read;
        handle->SAVED_POSITION = handle->LOCATION;
    }

    error = MFS_unlock(drive_ptr, FALSE);

    if ((error == MFS_NO_ERROR) && eof_reached)
    {
        error = MFS_EOF;
    }

    MFS_set_error_and_return(error_ptr,error,bytes_read);
}
