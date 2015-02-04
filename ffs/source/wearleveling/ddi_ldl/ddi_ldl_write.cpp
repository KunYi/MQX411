/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains write functions for the logical drive layer.
*
*
*END************************************************************************/

#include "wl_common.h"
#include <string.h>
#include "ddi_media_internal.h"
#include "ddi_media.h"
#include "buffer_manager/media_buffer.h"

#if defined(NO_SDRAM)
#include "os/threadx/tx_api.h"
TX_MUTEX g_WriteSectorMutex;

#if defined(USE_NAND_STACK)
extern TX_MUTEX g_NANDThreadSafeMutex;
#endif /* defined(USE_NAND_STACK) */
#endif /* defined(NO_SDRAM) */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : ddi_ldl_shutdown_media_drives
* Returned Value   : MQX_OK or an error code
* Comments         :
*   This function shuts down all drives belonging to a particular media.
*
*END*--------------------------------------------------------------------*/
static RtStatus_t ddi_ldl_shutdown_media_drives
(
    /* [IN] Media number */
    unsigned mediaNumber
)
{ /* Body */
    /* Before shutting down the media, shut down all of its drives. */
    DriveTag_t tag;
    DriveIterator_t iter;
    if (DriveCreateIterator(&iter) == SUCCESS)
    {
        while (DriveIteratorNext(iter, &tag) == SUCCESS)
        {
            LogicalDrive * drive = DriveGetDriveFromTag(tag);
            
            /* Shut down this drive if it belongs to the media we're interested in. */
            if (drive && (drive->getMedia() && drive->getMedia()->getMediaNumber() == mediaNumber))
            {
                /* Only shutdown the drive if it was actually inited. */
                if (drive->isInitialized())
                {
                    DriveShutdown(tag);
                } /* Endif */
                
                DriveRemove(tag);
            } /* Endif */
        } /* Endwhile */
        
        DriveIteratorDispose(iter);
    } /* Endif */
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaErase
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function tests all task queues for correctness
*
*END*--------------------------------------------------------------------*/
RtStatus_t MediaErase
(
    /* [IN] Logical media number */
    uint32_t u32LogMediaNumber, 

    /* [IN] Temporary unused */
    uint32_t u32MagicNumber, 

    /* [IN] Flag to indicate hidden data drive should not be erased.*/
    uint8_t u8NoEraseHidden
)
{ /* Body */
    /* Get media depends on its index */
    LogicalMedia * media = MediaGetMediaFromIndex(u32LogMediaNumber);

    if (!media)
    {
        return ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER;
    }
    else if (!media->isInitialized())
    {
        return ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED;
    } /* Endif */

    /* First shutdown all drives belonging to this media. */
    ddi_ldl_shutdown_media_drives(u32LogMediaNumber);
    
    /* Now erase the media. */
    return media->erase();
} /* Endbody */

#ifdef NO_SDRAM
#pragma ghs section text=".static.text"
#endif

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveWriteSector
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_NUMBER
*   - ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED
*   - Others possible from drive type's WriteSector API
* Comments         :
*   Writes a sector of a logical drive.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DriveWriteSector
(
    /* [IN] Unique tag for the drive to operate on. */
    DriveTag_t tag, 

    /* [IN] Sector to write; 0-based at the start of the drive. */
    uint32_t u32SectorNumber, 

    /* [IN] Pointer to buffer of sector data to write. */
    const SECTOR_BUFFER * pSectorData
)
{ /* Body */
    /* Get drive depending on its tag */
    LogicalDrive * drive = DriveGetDriveFromTag(tag);
    
    if (!drive)
    {
        return ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_NUMBER;
    }
    else if (!drive->isInitialized())
    {
        return ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED;
    } /* Endif */
    
#if defined(USE_NAND_STACK) && defined(NO_SDRAM)
    if (drive->getMedia()->getPhysicalType() != kMediaTypeMMC)
    {
        static RtStatus_t s_RetValue;
        TX_THREAD *pCurrentThread;
        
        tx_mutex_get(&g_NANDThreadSafeMutex, TX_WAIT_FOREVER);
        pCurrentThread = tx_thread_identify();
        if (pCurrentThread != NULL)
        {
            os_thi_SaveStackContext(&g_NewNandStackContext, pCurrentThread, &g_OldNandStackContext, 40);
        }

        s_RetValue = drive->writeSector(u32SectorNumber, pSectorData);
        
        if (pCurrentThread != NULL)
        {
            os_thi_RestoreStackContext(&g_OldNandStackContext, pCurrentThread);
        }
        tx_mutex_put(&g_NANDThreadSafeMutex);

        return s_RetValue;
    } /* Endif */
#endif /* Defined(USE_NAND_STACK) && defined(NO_SDRAM) */

    return drive->writeSector(u32SectorNumber, pSectorData);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveFlush
* Returned Value   : SUCCESS of ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_NUMBER
*   - ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED
*   - ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED
*   - MMC_DATA_DRIVE_ERROR_WRITE_SECTOR_FAIL
*   - ERROR_DDI_LDL_LDRIVE_HARDWARE_FAILURE
* Comments         :
*   Flush the logical drive number's contents from RAM to physical media.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DriveFlush
(
    /* [IN] Unique tag for the drive to operate on. */
    DriveTag_t tag
)
{ /* Body */
    /* Get drive object */
    LogicalDrive * drive = DriveGetDriveFromTag(tag);
    
    if (!drive)
    {
        return ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_NUMBER;
    }
    else if (!drive->isInitialized() )
    {
        return ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED;
    } /* Endif */

#if (defined(USE_NAND_STACK) && defined(NO_SDRAM))
    if (drive->getMedia()->getPhysicalType() != kMediaTypeMMC)
    {
        static RtStatus_t s_RetValue;
        TX_THREAD *pCurrentThread;
        
        tx_mutex_get(&g_NANDThreadSafeMutex, TX_WAIT_FOREVER);
        pCurrentThread=tx_thread_identify();
        if (pCurrentThread != NULL)
        {
            os_thi_SaveStackContext(&g_NewNandStackContext, pCurrentThread, &g_OldNandStackContext, 40);
        } /* Endif */

        s_RetValue = drive->flush();

        if (pCurrentThread != NULL)
        {
            os_thi_RestoreStackContext(&g_OldNandStackContext, pCurrentThread);
        } /* Endif */
        tx_mutex_put(&g_NANDThreadSafeMutex);
        
        return s_RetValue;
    } /* Endif */
#endif

    /* Flush */
    return drive->flush();
} /* Endbody */

#ifdef NO_SDRAM
#pragma ghs section text=default
#endif

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveErase
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_NUMBER
*   - ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED
*   - Others possible from drive type's erase API
* Comments         :
*   Erase the logical drive if the drive num is valid and already init.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DriveErase
(
    /* [IN] Drive tag */
    DriveTag_t tag, 

    /* [IN] Temporary unused */
    uint32_t u32MagicNumber
)
{ /* Body */
    /* Get drive depending on its drive */
    LogicalDrive * drive = DriveGetDriveFromTag(tag);
    
    if (!drive)
    {
        return ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_NUMBER;
    }
    else if (!drive->isInitialized() )
    {
        return ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED;
    } /* Endif */

    return drive->erase();
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaShutdown
* Returned Value   : SUCCESS or an error code
*   - SUCCESS
*   - ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER.
*   - ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED.
* Comments         :
*   This function shutdowns the an initialized media
*
*END*--------------------------------------------------------------------*/
RtStatus_t MediaShutdown
(
    /* [IN] Logical media number */
    uint32_t u32LogMediaNumber
)
{ /* Body */
    /* Get media depends on its index */
    LogicalMedia * media = MediaGetMediaFromIndex(u32LogMediaNumber);

    if (!media)
    {
        return ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER;
    }
    else if (!media->isInitialized())
    {
        return ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED;
    } /* Endif */

    /* First shutdown all drives belonging to this media. */
    ddi_ldl_shutdown_media_drives(u32LogMediaNumber);
    
    /* Now shutdown the media. */
    RtStatus_t status = media->shutdown();
    
    delete media;
    g_ldlInfo.m_media[u32LogMediaNumber] = NULL;
    --g_ldlInfo.m_mediaCount;
    
    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveShutdown
* Returned Value   : SUCCESS or ERROR
* - SUCCESS
* - ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_NUMBER
* - ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED
* - Other errors from shutdown API
* Comments         :
*   DriveFlush and if allowed shutdown the logical data drive.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DriveShutdown
(
    /* [IN] Unique drive tag */
    DriveTag_t tag
)
{ /* Body */ 
    /* Flush the drive before shutting down. */
    RtStatus_t RetValue = DriveFlush(tag);
    if (RetValue != SUCCESS)
    {
        return RetValue;
    } /* Endif */

    /* Look up the drive by tag and perform sanity checks. */
    LogicalDrive * drive = DriveGetDriveFromTag(tag);
    if (!drive)
    {
        return ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_NUMBER;
    }
    else if (!drive->isInitialized() )
    {
        return ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED;
    } /* Endif */

    /* Invoke the API. */
    return drive->shutdown();
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveRepair
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_NUMBER
*   - ERROR_DDI_LDL_LDRIVE_FS_FORMAT_REQUIRED
*   - Others possible from drive repair API.
* Comments         :
*   Attempt to repair a drive by doing a low-level format.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DriveRepair
(
    /* [IN] Unique tag for the drive to operate on */
    DriveTag_t tag, 

    /* [IN] Temporary unused */
    uint32_t u32MagicNumber, 

    /* [IN] A flag to decide scanning bad blocks or not */
    bool bIsScanBad
)
{ /* Body */
    RtStatus_t status;

    /* Get the drive object. */
    LogicalDrive * drive = DriveGetDriveFromTag(tag);
    if (!drive)
    {
        return ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_NUMBER;
    } /* Endif */

    /* Shutdown the drive if it was already initialized. */
    if (drive->isInitialized())
    {
        status = DriveShutdown(tag);
        if (status != SUCCESS)
        {
            return status;
        } /* Endif */
    } /* Endif */

    bool bMediaErased = false;

    /* Repair */
    status = drive->repair(bIsScanBad);
    
    /* If media was erased, remember to return this error to the caller. */
    if (status == ERROR_DDI_LDL_LDRIVE_FS_FORMAT_REQUIRED)
    {
        bMediaErased = true;
        status = SUCCESS;
    } /* Endif */
    
    if (status != SUCCESS)
    {
        return status;
    } /* Endif */

    /* Initialize the drive. */
    status = DriveInit(tag);
    if (status != SUCCESS)
    {
        return status;
    } /* Endif */

    return bMediaErased ? ERROR_DDI_LDL_LDRIVE_FS_FORMAT_REQUIRED : SUCCESS;
} /* Endbody */

/* EOF */
