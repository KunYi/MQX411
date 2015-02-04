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
*   This file device Driver Interface to the Logical Drive Layer's init calls.
*
*
*END************************************************************************/

#include "wl_common.h"
#include "ddi_media.h"
#include "ddi_media_internal.h"
#include "ddi/media/ddi_nand_media.h"
#include <string.h>

/* 
** Global external declarations 
*/

bool g_bFrameworkExternalDriveOrFsInit = false;

LdlInfo g_ldlInfo = {0};

extern MediaAllocationTable_t g_mediaTablePreDefine[];

extern NandHalContext_t g_nandHalContext;

RtStatus_t MediaBuildFromPreDefine();

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NandHalSetMqxPtr
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS     Initialization completed.
*   - ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED
*   - ERROR_OS_FILESYSTEM_FILESYSTEM_NOT_FOUND
*   - ERROR_DDI_MEDIABUFMGR_ALLOC_FAILED 
*   - ERROR_OS_FILESYSTEM_MAX_DEVICES_EXCEEDED
* Comments         :
*   Initializes the logical data drive on the selected device.
*   Determines the sector location of the logical data drive on the selected
*   device. Reads the media's master boot record (MBR) from sector 0, and
*   saves the starting sector for partition 0, which is the data drive.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NandHalSetMqxPtr
(
    /* [IN] Nand Flash Id */
    uint8_t nandFlashId, 

    /* [IN] Nand Flash handle */
    MQX_FILE_PTR fd_ptr
) 
{ /* Body */
    g_nandHalFDPTR[nandFlashId] = fd_ptr;
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaInit
* Returned Value   : Status of the call, SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER
*   - ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER
* Comments         :
*   This function creates and initializes a logical media.
*   The specification of the logical media to be created is defined in a
*   global array of type MediaDefinition_t with the name g_mediaDefinition.
*   This array lets the application tell the LDL what types of media drivers
*   it wants to use.
*
*END*--------------------------------------------------------------------*/
RtStatus_t MediaInit
( 
    /* [IN] Logical media number */
    uint32_t u32LogMediaNumber
)
{ /* Body */ 
    g_bFrameworkExternalDriveOrFsInit = false;

    if (u32LogMediaNumber > MAX_LOGICAL_MEDIA)
    {
        return ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER;
    } /* Endif */
    
    LogicalMedia * media = new nand::Media();

    if (!media)
    {
        return ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER;
    } /* Endif */

    assert(g_ldlInfo.m_media[u32LogMediaNumber] == NULL);
    g_ldlInfo.m_media[u32LogMediaNumber] = media;
    g_ldlInfo.m_mediaCount++;

    return media->init();
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaBuildFromPreDefine
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function builds media from pre-defined information
*
*END*--------------------------------------------------------------------*/
RtStatus_t MediaBuildFromPreDefine() 
{ /* Body */
    // return MediaAllocateFromPreDefine(0, g_mediaTablePreDefine);
    return MediaAllocateFromPreDefineRegions(0);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaAllocateFromPreDefine
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER
*   - ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED
* Comments         :
*   This function builds media from pre-defined information
*
*END*--------------------------------------------------------------------*/
RtStatus_t MediaAllocateFromPreDefine
(
    /* [IN] Logical media number */
    uint32_t u32LogMediaNumber, 

    /* [IN] Pointer to Media Table structure */
    MediaAllocationTable_t * pMediaTable
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

    return media->allocateFromPreDefine(pMediaTable);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaAllocateFromPreDefineRegions
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER
*   - ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED
* Comments         :
*   This function builds media from pre-defined information
*
*END*--------------------------------------------------------------------*/
RtStatus_t MediaAllocateFromPreDefineRegions
(
    /* [IN] Logical media number */
    uint32_t u32LogMediaNumber
)
{
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

    return media->allocateFromPreDefineRegions();
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveInit
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_TAG
* Comments         :
*   This function initializes the specified drive.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DriveInit
(
    /* [IN] Unique tag for the drive to operate on */
    DriveTag_t tag
)
{ /* Body */
    RtStatus_t RetValue;

    /* Get drive depending on its tag */
    LogicalDrive * drive = DriveGetDriveFromTag(tag);

    if (drive)
    {
        RetValue = drive->init();

        /* Keep track of whether the init succeeded. */
        drive->setDidFailInit(RetValue != SUCCESS);
    }
    else
    {
        RetValue = ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_TAG;
    } /* Endif */

    return RetValue;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveInitAll
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function initializes drive
*
*END*--------------------------------------------------------------------*/
RtStatus_t DriveInitAll(void)
{ /* Body */
    DriveIterator_t iter;
    RtStatus_t status = DriveCreateIterator(&iter);
    if (status == SUCCESS)
    {
        /* Iterate over all drives. */
        DriveTag_t tag;
        while (DriveIteratorNext(iter, &tag) == SUCCESS)
        {
            /* 
            **Init only uninitialized drives. If a drive failed init 
            ** then we don't want to try initing it again.
            */
            DriveState_t state = DriveGetState(tag);
            if (state == kDriveUninitialized)
            {
                DriveInit(tag);
            } /* Endif */
        } /* Endwhile */

        DriveIteratorDispose(iter);
    } /* Endif */

    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveAdd
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_GENERAL
* Comments         :
*   Adds a new drive to the LDL.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DriveAdd
(
    /* [IN] New drive */
    LogicalDrive * newDrive
)
{ /* Body */
    LogicalDrive ** drive = NULL;

    /* 
    ** Just copy the new drive data into the next free slot on the drive array
    ** and update the drive count.
    */
    drive = DriveFindEmptyEntry();
    if ( drive )
    {
        *drive = newDrive;
        g_ldlInfo.m_driveCount++;
        
        /* Update the number of drives for the media. */
        LogicalMedia * media = newDrive->getMedia();
        if (media)
        {
            media->setNumberOfDrives(media->getNumberOfDrives() + 1);
        }

        return SUCCESS;
    }
    else
    {
        return ERROR_DDI_LDL_GENERAL;
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveRemove
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_TAG
* Comments         :
*   Removes a specific drive from the LDL.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DriveRemove
(
    /* [IN] Drive to remove */
    DriveTag_t driveToRemove
)
{ /* Body */
    unsigned i;
    LogicalDrive ** drive = g_ldlInfo.m_drives;

    /* Scan all drives for the tag. */
    for (i = 0; i < MAX_LOGICAL_DRIVES; i++, drive++)
    {
        LogicalDrive * thisDrive = *drive;
        if (thisDrive && thisDrive->getTag() == driveToRemove)
        {
            /* Update the number of drives for the media. */
            LogicalMedia * media = thisDrive->getMedia();
            if (media)
            {
                media->setNumberOfDrives(media->getNumberOfDrives() - 1);
            } /* Endif */
            
            delete thisDrive;

            /* Clear the entry in the drive array. */
            *drive = NULL;
            g_ldlInfo.m_driveCount--;

            return SUCCESS;
        } /* Endif */
    } /* Endfor */

    /* No drive with this tag. */
    return ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_TAG;
} /* Endbody */

LogicalMedia::LogicalMedia()
:   m_u32MediaNumber(0),
    m_bInitialized(false),
    m_eState(kMediaStateUnknown),
    m_bAllocated(false),
    m_bWriteProtected(false),
    m_isRemovable(false),
    m_u32NumberOfDrives(0),
    m_u64SizeInBytes(0),
    m_u32AllocationUnitSizeInBytes(0),
    m_PhysicalType(kMediaTypeNand)
{
}

LogicalMedia::~LogicalMedia()
{
}

LogicalDrive::LogicalDrive()
:   m_bInitialized(false),
    m_bFailedInit(false),
    m_bPresent(false),
    m_bErased(false),
    m_bWriteProtected(false),
    m_u32NumberOfSectors(0),
    m_Type(kDriveTypeUnknown),
    m_u32Tag(0),
    m_u64SizeInBytes(0),
    m_u32SectorSizeInBytes(0),
    m_nativeSectorSizeInBytes(0),
    m_numberOfNativeSectors(0),
    m_nativeSectorShift(0),
    m_u32EraseSizeInBytes(0),
    m_pbsStartSector(0),
    m_logicalMedia(NULL)
{
}

LogicalDrive::~LogicalDrive()
{
}

/* EOF */
