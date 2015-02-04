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
*   This file contains The nand device driver interface presented 
*   as the media abstraction get info API.
*
*
*END************************************************************************/

#include "wl_common.h"
#include "ddi_media_internal.h"
#include "ddi/media/ddi_nand_media.h"
#include "auto_free.h"
#include <string.h>

using namespace nand;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getInfo
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED
*   - ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_INFO_TYPE
* Comments         :
*   This function reads specified information about the NAND media.
*
*END*--------------------------------------------------------------------*/
RtStatus_t Media::getInfo
(
    /* [IN] Type of information requested. */
    uint32_t Type, 
    
    /* [OUT] Pointer to information buffer to be filled. */
    void * pInfo
)
{ /* Body */
    if (!m_bInitialized)
    {
        return ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED;
    } /* Endif */
    
    //DdiNandLocker locker;
    
    switch (Type)
    {
        case kMediaInfoPageSizeInBytes:
            /* The result includes metadata bytes. */
            *((uint32_t *)pInfo) = NandHal::getParameters().pageTotalSize;
            break;

            /* 
            ** Media Info Nand Mfg Id byte (1st byte of readId nand HW cmd response)
            ** eg: 0xEC for samsung. 
            */
        case kMediaInfoMediaMfgId:
            *((uint32_t *)pInfo) = NandHal::getParameters().manufacturerCode;
            break;

            /* Media Info Nand Flash all bytes from nand HW readId command. */
        case kMediaInfoIdDetails:
            {
                /* Read the id bytes from the first nand. */
                union {
                    uint8_t bytes[8];
                    uint64_t u64;
                } idResult = {0};
                NandHal::getFirstNand()->readID(idResult.bytes);
                
                /* 
                ** returns 6 byte struct on the stack in a dword
                ** In stupdaterapp.exe, the bytes are shown LSB first. i.e., the LSB is manufacturer,
                ** next byte is device code, etc. 
                */
                *((uint64_t *)pInfo) = idResult.u64;
                break;
            }

        case kMediaInfoNumChipEnables:
            *((uint32_t *)pInfo) = 1;
            break;
            
        case kMediaInfoSectorMetadataSizeInBytes:
            *((uint32_t *)pInfo) = NandHal::getParameters().pageMetadataSize;
            break;
            
        case kMediaInfoProductName:
            {
                auto_free<char> name = (auto_free<char>)NandHal::getFirstNand()->getDeviceName();
                if (name)
                {
                    memcpy(pInfo, name, strlen(name) + 1);
                }
                else
                {
                    /* Fill in a null byte just in case the caller tries to use the string. */
                    *(uint8_t *)pInfo = 0;
                    return ERROR_DDI_LDL_UNIMPLEMENTED;
                }
                break;
            }

        default:
            return LogicalMedia::getInfo(Type, pInfo);
    } /* Endswitch */
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setInfo
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function set information of media
*
*END*--------------------------------------------------------------------*/
RtStatus_t Media::setInfo
(
    /* [IN] Selector */
    uint32_t selector, 
    
    /* [IN] value */
    const void * value
)
{ /* Body */
    if (!m_bInitialized)
    {
        return ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED;
    } /* Endif */
    
    DdiNandLocker locker;
    
    switch (selector)
    {
        case kMediaInfoExpectedTransferActivity:
            break;
            
        case kMediaInfoIsSleepAllowed:
            NandHal::getFirstNand()->enableSleep(*(bool *)value);
            break;
            
        default:
            return LogicalMedia::setInfo(selector, value);
    } /* Endswitch */
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getMediaTable
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_NAND_LMEDIA_NOT_ALLOCATED
* Comments         :
*   This function returns the current media allocation table to the caller.
*
*END*--------------------------------------------------------------------*/
RtStatus_t Media::getMediaTable
(
    /* [OUT] Pointer to the pointer for the media allocation table structure */
    MediaAllocationTable_t ** pTable
)
{ /* Body */
    if (m_eState != kMediaStateAllocated)
    {
        return ERROR_DDI_NAND_LMEDIA_NOT_ALLOCATED;
    } /* Endif */

    DriveIterator_t iter;
    unsigned myDriveCount = 0;
    DriveTag_t tag;
    LogicalDrive * drive;

    /* Create the drive iterator. */
    RtStatus_t status = DriveCreateIterator(&iter);
    if (status != SUCCESS)
    {
        return status;
    } /* Endif */

    /* Iterate over all drives known by the LDL and count my drives. */
    while (DriveIteratorNext(iter, &tag) == SUCCESS)
    {
        drive = DriveGetDriveFromTag(tag);
        if (drive && drive->m_logicalMedia == this)
        {
            ++myDriveCount;
        } /* Endif */
    } /* Endwhile */
    
    DriveIteratorDispose(iter);

    /* Recreate the iterator to scan again. */
    status = DriveCreateIterator(&iter);
    if (status != SUCCESS)
    {
        return status;
    } /* Endif */

    /* Allocate a media table with the exact number of drives belonging to us. */
    // TODO: use _wl_mem_alloc    
    MediaAllocationTable_t * table = (MediaAllocationTable_t *)_wl_mem_alloc(sizeof(MediaAllocationTable_t) - (sizeof(MediaAllocationTableEntry_t) * MAX_MEDIA_TABLE_ENTRIES) + (sizeof(MediaAllocationTableEntry_t) * myDriveCount));
    // MediaAllocationTable_t * table = (MediaAllocationTable_t *)malloc(sizeof(MediaAllocationTable_t) - (sizeof(MediaAllocationTableEntry_t) * MAX_MEDIA_TABLE_ENTRIES) + (sizeof(MediaAllocationTableEntry_t) * myDriveCount));
    if (!table)
    {
        return ERROR_OUT_OF_MEMORY;
    } /* Endif */
    
    /* We always have a data drive in each media which by default is drive 0. */
    int iNumDrives = 1;
    MediaAllocationTableEntry_t * tableEntry;
    
    /* Iterate over all drives known by the LDL. */
    while (DriveIteratorNext(iter, &tag) == SUCCESS)
    {
        drive = DriveGetDriveFromTag(tag);
        
        /* Skip this drive if it's invalid or doesn't belong to us. */
        if (!drive || drive->m_logicalMedia != this)
        {
            continue;
        } /* Endif */
        
        switch (drive->m_Type)
        {
            case kDriveTypeData:
                /* 
                ** Drive Type is Data Drive
                ** Fill Up MediaAllocationTableEntry_t 
                */
                tableEntry = &table->Entry[0];
                tableEntry->u32DriveNumber = m_u32MediaNumber;

                break;
                
            case kDriveTypeHidden:
            case kDriveTypeSystem:
                /* Fill Up MediaAllocationTableEntry_t */
                tableEntry = &table->Entry[iNumDrives];
                tableEntry->u32DriveNumber = iNumDrives;

                /* Increment the number of drives discovered in this media */
                iNumDrives++;
                assert(iNumDrives <= (int)myDriveCount);
                
                break;
        } /* Endswitch */

        /* Fill in the common parts of the media table entry. */
        tableEntry->Type = drive->m_Type;
        tableEntry->u32Tag = drive->m_u32Tag;
        tableEntry->u64SizeInBytes = drive->m_u64SizeInBytes;
        tableEntry->bRequired = false;
    } /* Endwhile */
    
    DriveIteratorDispose(iter);

    /* Fill Up MediaAllocationTable_t */
    table->u32NumEntries = iNumDrives;
    
    if (pTable)
    {
        *pTable = table;
    } /* Endif */
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : freeMediaTable
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function disposes of the table that we allocated in Media::getMediaTable().
*
*END*--------------------------------------------------------------------*/
RtStatus_t Media::freeMediaTable
(
    /* [IN] Media table */
    MediaAllocationTable_t * table
)
{ /* Body */
    if (table)
    {
        _wl_mem_free((void *)table);
    } /* Endif */
    
    return SUCCESS;
} /* Endbody */

/* EOF */
