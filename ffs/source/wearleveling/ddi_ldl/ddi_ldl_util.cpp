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
*   This file utilities used by the logical drive layer.
*
*
*END************************************************************************/

#include "wl_common.h"
#include <string.h>
#include "ddi_media_internal.h"
#include "ddi_media.h"
#include "sectordef.h"

/* The minimum sector size that a caller can set. */
#define MIN_SECTOR_SIZE         (512)

/* 
** When this is unpacked ascii, get nand media SN returns as in sdk4.410 and earlier which
** is 1 ascii SN byte per 4 byte word. 
*/
typedef enum {
    UNPACKED_ASCII=0,
    PACKED_ASCII=1,
    RAW=2
} eNAND_MEDIA_SN_RETURN_FORM;

/* Set next line to media SN format you want: UNPACKED_ASCII or PACKED_ASCII or RAW. */
#define NAND_SN_RETURN_FORM UNPACKED_ASCII

extern LdlInfo g_ldlInfo;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaGetMediaFromIndex
* Returned Value   : Either a pointer to the media object is returned 
*   or NULL if the index is out of range.
* Comments         :
*   Returns the media structure given a media index.
*
*END*--------------------------------------------------------------------*/
LogicalMedia * MediaGetMediaFromIndex
(
    /* [IN] Zero based index into the available media */
    unsigned index
)
{ /* Body */
    if ((uint32_t)index < (uint32_t)g_ldlInfo.m_mediaCount) 
    {
        return g_ldlInfo.m_media[index];
    } 
    else 
    {
        return NULL;
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveGetDriveFromTag
* Returned Value   : Either a pointer to the drive structure is returned
*   or NULL if a drive with the specified tag does not exist.
* Comments         :
*   Returns the drive structure given a drive tag.
*
*END*--------------------------------------------------------------------*/
LogicalDrive * DriveGetDriveFromTag
(
    /* [IN] The unique tag for the drive. */
    DriveTag_t tag
)
{ /* Body */
    unsigned i;
    LogicalDrive ** drive = g_ldlInfo.m_drives;

    /* Scan all drives for the tag. */
    for (i = 0; i < MAX_LOGICAL_DRIVES; i++, drive++)
    {
        /* Only consider drives that have a valid API set on them. */
        if (*drive && (*drive)->getTag() == tag)
        {
            return *drive;
        } /* Endif */
    } /* Endfor */

    /* The tag wasn't found. */
    return NULL;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveFindEmptyEntry
* Returned Value   : Either a pointer to the drive structure is returned 
*   or NULL if no empty entries exist in the drive array
* Comments         :
*   Find an emptry drive array entry if one exists
*
*END*--------------------------------------------------------------------*/
LogicalDrive ** DriveFindEmptyEntry()
{ /* Body */
    /* Make sure at least one slot free before scanning each entry */
    if (g_ldlInfo.m_driveCount >= MAX_LOGICAL_DRIVES)
    {
        /* All full */
        return NULL;
    } /* Endif */

    /* Scan all drives for the tag. */
    unsigned i;
    for (i = 0; i < MAX_LOGICAL_DRIVES; i++)
    {
        if (!g_ldlInfo.m_drives[i])
        {
            return &g_ldlInfo.m_drives[i];
        } /* Endif */
    } /* Endfor */

    /* Drive array is full */
    return NULL;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaGetMediaTable
* Returned Value   : SUCCESS or ERROR
*   - SUCCESs
*   - ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER
*   - ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED
* Comments         :
*   This function returns the current description of drives for a logical media.
*
*END*--------------------------------------------------------------------*/
RtStatus_t MediaGetMediaTable
(
    /* [IN] Logical media number */
    uint32_t u32LogMediaNumber, 

    /* [IN] On successful exit, this will point to a newly allocated mediaallocation table.*/
    MediaAllocationTable_t ** pMediaTable
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

    return media->getMediaTable(pMediaTable);
} /* Endbody*/

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaFreeMediaTable
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER
*   - ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED
* Comments         :
*   Dispose of a media table.
*   Frees memory for a media allocation table that was returned from a call
*   to MediaGetMediaTable().
*
*END*--------------------------------------------------------------------*/
RtStatus_t MediaFreeMediaTable
(
    /* [IN] Media number */
    uint32_t mediaNumber, 

    /* [IN/OUT] Pointer of MediaAllocationTable struct */
    MediaAllocationTable_t * table
)
{ /* Body */
    LogicalMedia * media = MediaGetMediaFromIndex(mediaNumber);

    if (!media)
    {
        return ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER;
    }
    else if (!media->isInitialized())
    {
        return ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED;
    } /* Endif */

    return media->freeMediaTable(table);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : freeMediaTable
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   The default implementation simply returns SUCCESS.
*
*END*--------------------------------------------------------------------*/
RtStatus_t LogicalMedia::freeMediaTable(MediaAllocationTable_t * table)
{ /* Body */
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaGetInfo
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER
*   - ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED
* Comments         :
*   For the requested logical media number, get the info Type requested into *pInfo.
*   Type must be one of those supported by the media driver
*   as defined
*
*END*--------------------------------------------------------------------*/
RtStatus_t MediaGetInfo
(
    /* [IN] Logical media number */
    uint32_t u32LogMediaNumber, 

    /* [IN] Type of media */
    uint32_t Type, 

    /* [IN/OUT] Infomation of the media*/
    void * pInfo
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
        /* 
        ** Handle the initialized property as a special case, so callers can get it before the
        ** media is actually initialized.
        */
        if (Type == kMediaInfoIsInitialized)
        {
            *((bool *)pInfo) = false;
            return SUCCESS;
        } /* Endif */
        
        return ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED;
    } /* Endif */

    return media->getInfo(Type, pInfo);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaGetCount
* Returned Value   : Number of media
* Comments         :
*   This function returns number of media
*
*END*--------------------------------------------------------------------*/
unsigned MediaGetCount(void)
{ /* Body */
    return g_ldlInfo.m_mediaCount;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaGetInfoSize
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS 
*   - ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER
*   - ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED
* Comments         :
*   This function returns value of input selector
*
*END*--------------------------------------------------------------------*/
RtStatus_t MediaGetInfoSize
(
    /* [IN] Media number */
    unsigned mediaNumber, 

    /* [IN] Selector */
    uint32_t selector, 

    /* [OUT] Size of property */
    uint32_t * propertySize
)
{ /* Body */
    /* Get media depends on its index */
    LogicalMedia * media = MediaGetMediaFromIndex(mediaNumber);

    if (!media)
    {
        return ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER;
    }
    else if (!media->isInitialized())
    {
        return ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED;
    } /* Endif */

    return media->getInfoSize(selector, propertySize);
} /* Endbody */ 

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaSetInfo
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER
*   - ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED
* Comments         :
*   This function set new value to info of media
*
*END*--------------------------------------------------------------------*/
RtStatus_t MediaSetInfo
(
    /* [IN] Media number */
    unsigned mediaNumber, 

    /* [IN] Selector */
    uint32_t selector, 

    /* [OUT] Value of selected property */
    const void * value
)
{ /* Body */
    /* Get media depends its index */
    LogicalMedia * media = MediaGetMediaFromIndex(mediaNumber);

    if (!media)
    {
        return ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER;
    }
    else if (!media->isInitialized())
    {
        return ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED;
    } /*Endif*/

    return media->setInfo(selector, value);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaFlushDrives
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER
*   - ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED
* Comments         :
*   If the media number is valid and was initialized, this function calls media's flush drives API.
*
*END*--------------------------------------------------------------------*/
RtStatus_t MediaFlushDrives
(
    /* [IN] Logical media number */
    uint32_t u32LogMediaNumber
)
{ /* Body */
    /* Get media depends its index */
    LogicalMedia * media = MediaGetMediaFromIndex(u32LogMediaNumber);

    if (!media)
    {
        return ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER;
    }
    else if (!media->isInitialized())
    {
        return ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED;
    } /* Endif */

    return media->flushDrives();
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaSetBootDrive
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER
*   - ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED
* Comments         :
*   This function sets bootable firmware to primary or secondary firmware.
*
*END*--------------------------------------------------------------------*/
RtStatus_t MediaSetBootDrive
(
    /* [IN] Logical media number */
    uint32_t u32LogMediaNumber, 

    /* [IN] Tag of the mdia */
    DriveTag_t u32Tag
)
{ /* Body */
    /* Get media depends its index */
    LogicalMedia * media = MediaGetMediaFromIndex(u32LogMediaNumber);

    if (!media)
    {
        return ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER;
    }
    else if (!media->isInitialized())
    {
        return ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED;
    } /* Endif */

    return media->setBootDrive(u32Tag);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveGetCount
* Returned Value   : Number of drives
* Comments         :
*   This function returns number of drives
*
*END*--------------------------------------------------------------------*/
unsigned DriveGetCount(void)
{ /* Body */
    return g_ldlInfo.m_driveCount;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveGetState
* Returned Value   : State of drive
* Comments         :
*   This function returns state of drive depends of its tag
*
*END*--------------------------------------------------------------------*/
DriveState_t DriveGetState
(
    /* [IN] Tag of drive */
    DriveTag_t tag
)
{ /* Body */
    /* Get drive depends on its tag */
    LogicalDrive * drive = DriveGetDriveFromTag(tag);

    if (drive)
    {
        return drive->getState();
    }
    else
    {
        return kDriveNotPresent;
    } /* Endif */
} /* Endbody */

/*#pragma ghs section text=".static.text" */
#if defined(__ghs__)
#pragma ghs section text=".init.text"
#endif 

#if defined (__CODEWARRIOR__)
#endif 

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getState
* Returned Value   : State of drive
* Comments         :
*   This function return state of drive
*
*END*--------------------------------------------------------------------*/
DriveState_t LogicalDrive::getState() const
{ /* Body */
    if (isInitialized())
    {
        return kDriveReady;
    }
    else if (didFailInit())
    {
        return kDriveFailedInitialization;
    }
    else
    {
        return kDriveUninitialized;
    } /* Endif */
} /* Endbody */

/*#pragma ghs section text=default */
#if defined(__ghs__)
#pragma ghs section text=default
#endif 

#if defined (__CODEWARRIOR__)
#endif 

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getInfoSize
* Returned Value   : MQX_OK or an error code
* Comments         :
*   This function handler for getting the size of common drive info selectors.
*   
*   Handles these selectors:
*   - #kDriveInfoTag
*   - #kDriveInfoSectorSizeInBytes
*   - #kDriveInfoSectorOffsetInParent 
*
*END*--------------------------------------------------------------------*/
RtStatus_t LogicalDrive::getInfoSize(uint32_t selector, uint32_t * propertySize)
{ /* Body */
    switch (selector)
    {
        case kDriveInfoTag:
        case kDriveInfoSectorSizeInBytes:
        case kDriveInfoSectorOffsetInParent:
            *propertySize = sizeof(uint32_t);
            break;

        default:
            return ERROR_DDI_LDL_LDRIVE_INVALID_INFO_TYPE;
    } /* Endswitch */

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveGetInfoSize
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_NUMBER
*   - ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED
* Comments         :
*   This function returns size of property
*
*END*--------------------------------------------------------------------*/
RtStatus_t DriveGetInfoSize
(
    /* [IN] Drive tag */
    DriveTag_t tag, 

    /* [IN] Selector */
    uint32_t selector, 

    /* [OUT] Size of property */
    uint32_t * propertySize
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

    /* Let the media driver's handler try first. */
    return drive->getInfoSize(selector, propertySize);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setInfo
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS 
*   - ERROR_DDI_LDL_LDRIVE_INVALID_INFO_TYPE
*   - ERROR_DDI_LDL_LDRIVE_INVALID_SECTOR_SIZE
* Comments         :
*   This function is handler for setting common drive info values.
*
*   Handles these selectors:
*   - #kDriveInfoTag
*   - #kDriveInfoSectorSizeInBytes
*   - #kDriveInfoSectorOffsetInParent
*
*END*--------------------------------------------------------------------*/
RtStatus_t LogicalDrive::setInfo
(
    /* [IN] Unique type */
    uint32_t Type, 
    
    /* [OUT] Pointer holds information */
    const void * pInfo
)
{ /* Body */
    switch (Type)
    {
        case kDriveInfoTag:
            m_u32Tag = *((uint32_t *)pInfo);
            break;

        /* 
        ** The caller wants to modify the runtime sector size of this data drive.
        ** We will have to recalculate several other fields based on this. 
        */
        case kDriveInfoSectorSizeInBytes:
            {
                uint32_t newSectorSize = *(uint32_t *)pInfo;

                /* 
                ** If the drive is a system drive, just verify that the new sector size
                ** is the native size. 
                */
                if (m_Type == kDriveTypeSystem && newSectorSize != m_nativeSectorSizeInBytes)
                {
                    return ERROR_DDI_LDL_LDRIVE_INVALID_SECTOR_SIZE;
                }

                /* Make sure the requested size is within range. */
                if (newSectorSize < MIN_SECTOR_SIZE || newSectorSize > m_nativeSectorSizeInBytes)
                {
                    return ERROR_DDI_LDL_LDRIVE_INVALID_SECTOR_SIZE;
                }

                /* 
                ** Compute the shift to get from the native to new nominal sector size.
                ** The maximum shift is 15, which is absolutely huge. 
                */
                int shift;
                for (shift = 0; shift < 16; shift++)
                {
                    if (m_nativeSectorSizeInBytes >> shift == newSectorSize)
                    {
                        break;
                    }
                }

                /* 
                ** If we didn't find a matching shift value, then the requested sector size must not
                ** a power of two, so return an error. 
                */
                if (shift == 16)
                {
                    return ERROR_DDI_LDL_LDRIVE_INVALID_SECTOR_SIZE;
                }

                /* Update drive descriptor. */
                m_u32SectorSizeInBytes = newSectorSize;
                m_u32NumberOfSectors = m_numberOfNativeSectors * (m_nativeSectorSizeInBytes / m_u32SectorSizeInBytes);
                m_nativeSectorShift = shift;
                break;
            }

        case kDriveInfoSectorOffsetInParent:
            m_pbsStartSector = *(uint32_t *)pInfo;
            break;

        default:
            return ERROR_DDI_LDL_LDRIVE_INVALID_INFO_TYPE;
    } /* Endswitch */

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveSetInfo
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_NUMBER
*   - ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED
* Comments         :
*   For the requested logical drive, set the *pInfo of Type specified.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DriveSetInfo
(
    /* [IN] Unique tag for the drive to operate on. */
    DriveTag_t tag, 

    /* [IN] Type of information to set */
    uint32_t Type, 

    /* [OUT] Buffer to copy the information from */
    const void * pInfo
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

    /* Let the media driver's handler try first. */
    return drive->setInfo(Type, pInfo);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaGetMaximumSectorSize
* Returned Value   : Value of maximum sector size
* Comments         :
*   This function returns the runtime maximum sector size for all active media.
*
*END*--------------------------------------------------------------------*/
uint32_t MediaGetMaximumSectorSize(void)
{ /* Body */
    unsigned i;
    uint32_t maxSize = 0;
    
    /* Iterate over all available media and find the largest sector size. */
    for (i=0; i < g_ldlInfo.m_mediaCount; ++i)
    {
        uint32_t theSize = 0;
        if (MediaGetInfo(i, kMediaInfoSectorSizeInBytes, &theSize) != SUCCESS)
        {
            continue;
        } /* Endif */
        
        if (theSize > maxSize)
        {
            maxSize = theSize;
        } /* Endif */
    } /* Endfor */
    
    /* Backup if perhaps no media are inited yet. */
    if (maxSize == 0)
    {
        maxSize = MAX_SECTOR_DATA_SIZE;
    } /* Endif */

    return maxSize;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : MediaGetMaximumMetadataSize
* Returned Value   : Value of maximum metadata size
* Comments         :
*   This function returns the runtime maximum metadata size for all active media.
*
*END*--------------------------------------------------------------------*/
uint32_t MediaGetMaximumAuxSize(void)
{ /* Body */
    unsigned i;
    uint32_t maxSize = 0;
    
    /* Iterate over all available media and find the largest sector size. */
    for (i=0; i < g_ldlInfo.m_mediaCount; ++i)
    {
        uint32_t theSize = 0;
        if (MediaGetInfo(i, kMediaInfoSectorMetadataSizeInBytes, &theSize) != SUCCESS)
        {
            continue;
        } /* Endif */
        
        if (theSize > maxSize)
        {
            maxSize = theSize;
        } /* Endif */
    } /* Endfor */
    
    /* Backup if perhaps no media are inited yet. */
    if (maxSize == 0)
    {
        maxSize = REDUNDANT_AREA_BUFFER_ALLOCATION;
    } /* Endif */

    return maxSize;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getInfoSize
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function returns size of information
*
*END*--------------------------------------------------------------------*/
RtStatus_t LogicalMedia::getInfoSize
(
    /* [IN] Selector */
    uint32_t infoSelector, 
    
    /* [OUT] Pointer holds result */
    uint32_t * infoSize
)
{ /* Body */
    assert(infoSize);
    *infoSize = sizeof(uint32_t);
    return SUCCESS;
} /* Endbody */

/*#pragma ghs section text=".init.text" */
#if defined(__ghs__)
#pragma ghs section text=".init.text"
#endif 

#if defined (__CODEWARRIOR__)
#endif 

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getInfo
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function returns information depending on selector
*
*END*--------------------------------------------------------------------*/
RtStatus_t LogicalMedia::getInfo
(
    /* [IN] Selector */
    uint32_t infoSelector, 
    
    /* [OUT] Result */
    void * value
)
{ /* Body */
    switch (infoSelector)
    {
        case kMediaInfoNumberOfDrives:
            *((uint32_t *)value) = m_u32NumberOfDrives;
            break;
            
        case kMediaInfoSizeInBytes:
            *((uint64_t *)value) = m_u64SizeInBytes;            
            break;
            
        case kMediaInfoAllocationUnitSizeInBytes:
            *((uint32_t *)value) = m_u32AllocationUnitSizeInBytes;
            break;

        case kMediaInfoIsInitialized:
            *((bool *)value) = true;
            break;
            
        case kMediaInfoMediaState:
            *((MediaState_t *)value) = m_eState;
            break;

        case kMediaInfoIsWriteProtected:
            *((bool *)value) = m_bWriteProtected;
            break;

        case kMediaInfoPhysicalMediaType:
            *((PhysicalMediaType_t *)value) = m_PhysicalType;
            break;
            
        case kMediaInfoSizeOfSerialNumberInBytes:
            {                
                break;
            }
            
        case kMediaInfoSerialNumber:
            {                
                break;
            }

        case kMediaInfoIsSystemMedia:
            /* The internal media is always the "system" media. */
            *((bool *)value) = (getMediaNumber() == kInternalNandMedia);
            break;

        case kMediaInfoIsMediaPresent:
            /* need to report if the media is present. For Internal NAND this is always true. */
            /* for other devices, that may not be the case and will need to be figured out. */
            *((bool *)value) = true;
            break;

        default:
            return ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_INFO_TYPE;
    } /* Endswitch */
    
    return SUCCESS;
} /* Endbody */

/*#pragma ghs section text=default */
#if defined(__ghs__)
#pragma ghs section text=default
#endif 

#if defined (__CODEWARRIOR__)
#endif 

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setInfo
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function set new information
*
*END*--------------------------------------------------------------------*/
RtStatus_t LogicalMedia::setInfo
(
    /* [IN] Selector */
    uint32_t infoSelector, 
    
    /* [IN] Value */
    const void * value
)
{ /* Body */
    return ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_INFO_TYPE;
} /* Endbody */

void ddi_ldl_push_media_task(const char * taskName) {}
void ddi_ldl_pop_media_task(void) {}
unsigned ddi_ldl_get_media_task_count(void) { return 0; }
unsigned ddi_ldl_get_media_task_stack(const char ** tasks, unsigned maxTasks) { return 0; }


/* EOF */

