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
*   This file contains the functions that are used to modify and update
*   the FAT table on the disk.
*
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>

#include "mfs.h"
#include "mfs_prv.h"



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Flush_fat_cache
* Returned Value   :  MFS error code
* Comments  :
*   Always write the FAT buffer back to the disk. Assumes the semaphore is 
*   already locked.  This function is identical to MFS_Write_back_fat, except it 
*   always works, whether or not the write cache is on.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Flush_fat_cache
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr   /*[IN] the drive on which to operate */
    )
{
#if MFSCFG_READ_ONLY
    return MFS_NO_ERROR;
#else
    _mfs_error  error_code;
    uint32_t     fat_size,fat,i;

    error_code = MFS_NO_ERROR;

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (drive_ptr))
    {
        return error_code;
    }
#endif

    if ( drive_ptr->FAT_CACHE_DIRTY )
    {
        if ( drive_ptr->FAT_TYPE == MFS_FAT32 )
        {
            fat_size = drive_ptr->BPB32.FAT_SIZE;
        }
        else
        {
            fat_size = drive_ptr->BPB.SECTORS_PER_FAT;
        }  

        /* Backup all copies of fat */
        for ( fat=0; (fat<drive_ptr->BPB.NUMBER_OF_FAT) && (error_code==MFS_NO_ERROR); fat++ )
        {
            for ( i=0; (i<drive_ptr->FAT_CACHE_SIZE) && (error_code==MFS_NO_ERROR); i++ )
            {
                error_code = MFS_Write_device_sector(drive_ptr, 
                    drive_ptr->FAT_START_SECTOR + drive_ptr->FAT_CACHE_START + i + (fat * fat_size), 
                    (void *) &drive_ptr->FAT_CACHE_PTR[drive_ptr->BPB.SECTOR_SIZE*i]);
            }
        }  

        if ( error_code == MFS_NO_ERROR )
        {
            drive_ptr->FAT_CACHE_DIRTY = FALSE;
        }
    }

    return(error_code);
#endif
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Write_back_fat
* Returned Value   :  MFS error code
* Comments  :
*   Write the FAT buffer back to the disk. Assumes the semaphore is already
*   locked.
*
*END*---------------------------------------------------------------------*/
#if !MFSCFG_READ_ONLY

_mfs_error MFS_Write_back_fat
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr
    )
{

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (drive_ptr))
    {
        return MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif

    if ( drive_ptr->FAT_CACHE_POLICY == MFS_WRITE_THROUGH_CACHE )
    {
        return( MFS_Flush_fat_cache(drive_ptr) );
    }
    else
    {
        return( MFS_NO_ERROR );
    } 
}  

#endif

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Read_fat
* Returned Value   :  MFS error code
* Comments  :
*   Read FAT into the FAT buffer. Assumes the semaphore to be locked.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Read_fat
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,
    uint32_t                 offset      /*[IN] the offset byte in the FAT that we are looking for */
    )
{
    uint32_t     needed_sector,needed_end_sector,i;
    _mfs_error  error_code;

    error_code = MFS_NO_ERROR;
    needed_sector = offset / drive_ptr->BPB.SECTOR_SIZE;

    MFS_LOG(printf("\nRead fat: Offset = %d\n", offset));

    if ( drive_ptr->FAT_TYPE == MFS_FAT12 ) 
    {
       needed_end_sector = (offset+1)/ drive_ptr->BPB.SECTOR_SIZE;
    } 
    else 
    {
       needed_end_sector = needed_sector;
    }


    /*
    ** If the FAT fragment that we already have in memory is requested, do
    ** not update or re-read it.
    */
    if ( (needed_sector < drive_ptr->FAT_CACHE_START) || (needed_end_sector  > drive_ptr->FAT_CACHE_START + drive_ptr->FAT_CACHE_SIZE - 1) )
    {
        error_code = MFS_Flush_fat_cache(drive_ptr);
        if ( error_code )
        {
            return(error_code);
        }

        if ( drive_ptr->FAT_TYPE == MFS_FAT32 )
        {
            if ( needed_sector > (drive_ptr->BPB32.FAT_SIZE - drive_ptr->FAT_CACHE_SIZE) )
            {
                needed_sector = drive_ptr->BPB32.FAT_SIZE - drive_ptr->FAT_CACHE_SIZE;
            }
        }
        else
        {
            if ( needed_sector > (drive_ptr->BPB.SECTORS_PER_FAT - drive_ptr->FAT_CACHE_SIZE) )
            {
                needed_sector = drive_ptr->BPB.SECTORS_PER_FAT -  drive_ptr->FAT_CACHE_SIZE;
            }
        }

        for ( i=0;i<(drive_ptr->FAT_CACHE_SIZE) && (error_code==MFS_NO_ERROR);i++ )
        {
            error_code = MFS_Read_device_sector (drive_ptr, drive_ptr->FAT_START_SECTOR + needed_sector + i,
                (void *) &drive_ptr->FAT_CACHE_PTR[drive_ptr->BPB.SECTOR_SIZE*i]);
        }

        if ( error_code == MFS_NO_ERROR )
        {
            drive_ptr->FAT_CACHE_START = needed_sector;
            drive_ptr->FAT_CACHE_DIRTY = FALSE;
        }
    }

    return(error_code);
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_get_cluster_from_fat
* Returned Value   :  cluster number in the specified fat entry
* Comments  :
*      Get the cluster number from the FAT stored in the buffer pointed
*      to by the FAT buffer. Assumes FAT in sync and sem obtained.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_get_cluster_from_fat
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,
    uint32_t        fat_entry,           /*[IN] FAT entry index */
    uint32_t    *cluster_number_ptr   /* [OUT] Error code pointer */
    )
{
    _mfs_error       error;
    uint32_t          cluster_number = CLUSTER_EOF; 
    uint32_t      *fat_ptr;
    unsigned char        *fat_12;
    uint32_t          offset;

    if ( fat_entry > drive_ptr->LAST_CLUSTER )
    {
        error = MFS_INVALID_CLUSTER_NUMBER;
    }
    else if ( fat_entry < CLUSTER_MIN_GOOD )
    {
        error = MFS_INVALID_CLUSTER_NUMBER;
    }
    else
    {
        if ( drive_ptr->FAT_TYPE == MFS_FAT16 )
        {
            offset = fat_entry << 1;
        }
        else if ( drive_ptr->FAT_TYPE == MFS_FAT12 )
        {
            offset = fat_entry + (fat_entry >> 1);
        }
        else
        {
            offset = fat_entry << 2; 
        }  

        error = MFS_Read_fat(drive_ptr, offset);

        if ( error == MFS_NO_ERROR )
        {
            fat_ptr = (uint32_t *)(drive_ptr->FAT_CACHE_PTR + offset - 
                      (drive_ptr->FAT_CACHE_START*drive_ptr->BPB.SECTOR_SIZE ));

            /* Now we know where the relevant info is. (uP independent) */

            /* If it was 12-bit, MASK and shift, so
            ** we can't tell from outside this function if we're dealing w/12, 16
            ** or 32 bits. This helps speed up error checking.
            */
            if ( drive_ptr->FAT_TYPE == MFS_FAT12 )
            {
                fat_12 = (unsigned char *) fat_ptr;

                if ( (fat_entry & 0x0001) == 0 )
                {
                    /*
                    ** For an even cluster number (on byte boundary), the lower 12
                    ** bits are the relevant ones.
                    */
                    cluster_number = ftoh0(fat_12);
                }
                else
                {
                    /*
                    ** For an odd cluster number (not on byte boundary), the
                    ** higher 12 bits are the relevant ones.
                    */
                    cluster_number = ftoh1(fat_12);
                }  

                /*
                ** Counting of clusters start with zero and the last
                ** valid cluster is CLUSTER_MAX - 1.
                */
                if ( cluster_number >= CLUSTER_MAX_12 )
                {
                    cluster_number = CLUSTER_EOF;
                }
            }
            else if ( drive_ptr->FAT_TYPE == MFS_FAT16 )
            {
                cluster_number = mqx_dtohs((unsigned char *)fat_ptr);
                if ( cluster_number >= CLUSTER_MAX_16 )
                {
                    cluster_number = CLUSTER_EOF;
                }
                /*
                ** After reading it;
                ** If it is a 16 bit fat, the cluster number register
                ** will look like HHLL Little endian or LLHH Big endian
                */
            }
            else
            {
                cluster_number = mqx_dtohl((unsigned char *)fat_ptr);
                if ( cluster_number >= CLUSTER_MAX_32 )
                {
                    cluster_number = CLUSTER_EOF;
                }
            }  
        }
    } 

    *cluster_number_ptr = (cluster_number & 0x0FFFFFFF);

    return error;
}  


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Put_fat
* Returned Value   :  error_code
* Comments  :
*      Write a cluster number in the specified slot of the FAT.
*      Assume FAT in sync and locked semaphore.
*END*---------------------------------------------------------------------*/

#if !MFSCFG_READ_ONLY

_mfs_error MFS_Put_fat
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,
    uint32_t                 fat_entry,  /*[IN] FAT entry index */
    uint32_t                 code        /*[IN] code to write into the FAT index */
    )
{
    uint32_t      *fat_ptr;
    unsigned char        *fat_12;
    _mfs_error       error_code;
    uint32_t          offset;
    uint32_t          code_prev;

#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (drive_ptr))
    {
        return MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif

    error_code = MFS_NO_ERROR;

    MFS_LOG(printf("\nPut fat: Cluster = %d, code = %d\n",fat_entry,code));

    if ( fat_entry > drive_ptr->LAST_CLUSTER )
    {
        error_code = MFS_INVALID_CLUSTER_NUMBER;
    }
    else if ( fat_entry < CLUSTER_MIN_GOOD )
    {
        error_code = MFS_INVALID_CLUSTER_NUMBER;
    }
    else
    {

        if ( drive_ptr->FAT_TYPE == MFS_FAT16 )
        {
            offset = fat_entry << 1;
        }
        else if ( drive_ptr->FAT_TYPE == MFS_FAT12 )
        {
            offset = fat_entry + (fat_entry >> 1);
        }
        else
        {
            offset = fat_entry <<2;
        } 

        error_code = MFS_Read_fat(drive_ptr, offset);
        if ( error_code )
        {
            return(error_code);
        }

        fat_ptr = (uint32_t *)(drive_ptr->FAT_CACHE_PTR + offset - 
                  (drive_ptr->FAT_CACHE_START*drive_ptr->BPB.SECTOR_SIZE ));
        // Mark FAT cache dirty, as it is about to be changed
        drive_ptr->FAT_CACHE_DIRTY = TRUE;

        /* Now we know where the relevant info is. (uP independent) */

        /* If it was 12-bit, MASK and shift, then keeping the nibble that is
        ** not part of our code, add the relevant nibbles.
        */
        if ( drive_ptr->FAT_TYPE == MFS_FAT12 )
        {
            fat_12 = (unsigned char *) fat_ptr;

            /*
            ** After reading it;
            ** If it is a 12 bit FAT it will look like:
            ** 0HLL (Little endian, even)  HLL0 (Little endian, odd) OR:
            ** LL0H (Big endian,  even)  L0HL (Big endian,  odd)
            ** Before transformation of the 12 bit entries, we need to swap
            ** endian if Big endian.
            */

            if ( (fat_entry & 0x0001) == 0 )
            {
                /*
                ** For an even cluster number (on byte boundary), the lower 12
                ** bits are the relevant ones.
                */
                code_prev = ftoh0(fat_12);
                htof0(fat_12,code);
            }
            else
            {
                /*
                ** For an odd cluster number (not on byte boundary), the
                ** higher 12 bits are the relevant ones.
                */
                code_prev = ftoh1(fat_12);
                htof1(fat_12,code);
            }  
        }
        else if ( drive_ptr->FAT_TYPE == MFS_FAT16 )
        {
            code_prev = mqx_dtohs((unsigned char *)fat_ptr);
            mqx_htods((unsigned char *)fat_ptr, code);
        }
        else
        {
            code_prev = mqx_dtohl((unsigned char *)fat_ptr);
            htof32((unsigned char *)fat_ptr, code);
        }

        /* Update free cluster counter */
        if ( code == CLUSTER_UNUSED && code_prev != CLUSTER_UNUSED )
        {
            /* Previously used cluster was marked as free */
            MFS_Increment_free_clusters(drive_ptr);
        }
        else if ( code != CLUSTER_UNUSED && code_prev == CLUSTER_UNUSED ) 
        {
            /* Previously unused cluster was marked as allocated */
            MFS_Decrement_free_clusters(drive_ptr);
        }          
    }  

    return(error_code);
}  

#endif
