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
*   This file reads rountines for Nand Device Driver
*
*
*END************************************************************************/
#include <string.h>
#include "wl_common.h"
#include "ddi/mapper/mapper.h"
#include "ddi/common/ddi_nand_ddi.h"
#include "ddi/data_drive/ddi_nand_data_drive.h"
#include "ddi_nand_hal.h"
#include "buffer_manager/media_buffer.h"
#include "deferred_task_queue.h"

using namespace nand;

/* Constants */

#ifndef REPORT_ECC_FAILURES
#define REPORT_ECC_FAILURES         0
#endif /* REPORT_ECC_FAILURES */

#if !defined(REPORT_ECC_REWRITES)
#define REPORT_ECC_REWRITES         0
#endif /* REPORT_ECC_REWRITES */


#ifndef DEBUG_DDI_NAND_READ_SECTOR
#define DEBUG_DDI_NAND_READ_SECTOR
#endif /* DEBUG_DDI_NAND_READ_SECTOR */

/*
#ifdef DEBUG_DDI_NAND_READ_SECTOR
static bool bEnableReadSectorDebug = true;
#endif
*/

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : logicalBlockToVirtual
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS The conversion was successful.
*   - ERROR_DDI_NAND_DATA_DRIVE_INVALID_LOGICAL_SECTOR The logical
* Comments         :
*   Converts a logical block address to a virtual one.
*   This function performs a conversion from a logical block number belonging
*   to a particular drive to a virtual block number. Virtual block numbers are
*   real physical block numbers in the range of the total number of blocks in
*   all NANDs. However, data is not written to the virtual block addresses.
*   Instead, the virtual address is used by the zone mapper as the key to look
*   up the physical block address that holds the actual data. You can think of
*   virtual block numbers as where the data would reside if the mapper was not
*   used.
*
*   The block number conversion is very simple. This function scans all of the
*   regions associated with the drive identified by \a pDescriptor. Each region
*   has a start physical block, a physical block count, and a logical block
*   count. The logical block count is simply the physical count minus any bad
*   blocks present in the region. When the region holding the logical block
*   is found, the virtual block is constructing by adding the logical offset
*   within the region to the absolute physical start block of the region.
*   The logical offset is simply the sum of logical blocks contained in all
*   prior regions subtracted from the logical block number.
*   
*   As you can see, there may be holes in the virtual block range for a given
*   drive. This is due to the bad blocks in a region not being counted in
*   the logical blocks for that region. The bad blocks are effectively being
*   combined together at the end of the region. There is no need to skip over
*   bad blocks in the virtual address range because data is not actually
*   written to the virtual blocks.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DataDrive::logicalBlockToVirtual
(
    /* [IN] Logical block number */
    uint32_t logicalBlock,     
    
    /* [IN] The region holding the logical block. Optional. */
    Region ** region, 
    
    /* [OUT] The virtual block corresponding to the logical one. */
    uint32_t * virtualBlock)
{ /* Body */
    uint32_t totalLogicalBlocks = 0;
    Region * scanRegion;
    Region::Iterator it(m_ppRegion, m_u32NumRegions);
    scanRegion = it.getNext();
    while (scanRegion)
    {
        /* Does our logical block sit in this region of the drive? */
        if (logicalBlock >= totalLogicalBlocks && logicalBlock < totalLogicalBlocks + scanRegion->m_u32NumLBlks)
        {
            /* Return this region to the caller. */
            if (region)
            {
                *region = scanRegion;
            } /* Endif */
            
            /* Return the physical sector to the caller. */
            if (virtualBlock)
            {
                uint32_t blockWithinRegion = logicalBlock - totalLogicalBlocks;
                *virtualBlock = scanRegion->m_u32AbPhyStartBlkAddr + blockWithinRegion;
            } /* Endif */
            
            return SUCCESS;
        } /* Endif */
        
        /* Add up logical blocks for all regions we've scanned so far. */
        totalLogicalBlocks += scanRegion->m_u32NumLBlks;
        
        scanRegion = it.getNext();
    } /* Endwhile */
    
    return ERROR_DDI_NAND_DATA_DRIVE_INVALID_LOGICAL_SECTOR;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : logicalSectorToVirtual
* Returned Value   : SUCCESS or ERROR
*   SUCCESS The conversion was successful.
*   ERROR_DDI_NAND_DATA_DRIVE_INVALID_LOGICAL_SECTOR The logical
* Comments         :
*   Converts a logical sector to a virtual sector.
*   See the documentation for ddi_nand_data_drive_LogicalBlockToVirtual() for
*   details about virtual addresses.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DataDrive::logicalSectorToVirtual
(
    /* [IN] Logical sector number */
    uint32_t logicalSector, 
    
    /* [IN] The region holding the logical sector. */
    Region ** region, 
    
    /* [OUT] The virtual sector corresponding to the logical one */
    uint32_t * virtualSector)
{ /* Body */
    /* Convert the logical sector into a logical block and sector offset within that block. */
    uint32_t logicalBlock;
    uint32_t sectorOffset;
    m_ppRegion[0]->m_nand->pageToBlockAndOffset(logicalSector, &logicalBlock, &sectorOffset);
    
    uint32_t virtualBlock;
    RtStatus_t status = logicalBlockToVirtual(logicalBlock, region, &virtualBlock);
    
    if (status == SUCCESS && virtualSector)
    {
        *virtualSector = m_ppRegion[0]->m_nand->blockAndOffsetToPage(virtualBlock, sectorOffset);
    } /* Endif */
    
    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : readSector
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Read a page from the NAND.
*   This function will read a page from the NAND.  If SBlocks are enabled,
*   there is no need to search UBlocks.  If the Mapper is enabled, it should 
*   be called instead of the UBlock/RBlock search. 
*
*   If successful, the data is in pSectorData.  The auxillary data (ECC)
*   is appended at the end of the valid data.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DataDrive::readSector
(
    /* [IN] Logical Sector Number to be read. */
    uint32_t u32LogicalSectorNumber, 
        
    /* [OUT] Pointer where sector data should be stored */
    SECTOR_BUFFER * pSectorData
)
{ /* Body */
    RtStatus_t RetValue;

    RetValue = readSectorInternal(u32LogicalSectorNumber, pSectorData);

#ifdef DEBUG_DDI_NAND_READ_SECTOR
    if (RetValue != SUCCESS)
    {
        WL_LOG(WL_MODULE_LOGICALDRIVE, WL_LOG_INFO, "\nNDDRS failure 0x%X, Drive %d, Lpage 0x%X\n",
        RetValue, m_u32Tag, u32LogicalSectorNumber);
    } /* Endif */
#endif /* DEBUG_DDI_NAND_READ_SECTOR */

    return RetValue;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : readSectorInternal
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Internal implementation of NANDDataDriveReadSector.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DataDrive::readSectorInternal
(
    /* [IN] Logical Sector Number to be read. */
    uint32_t u32LogicalSectorNumber, 
    
    /* [OUT] Pointer where sector data should be stored */
    SECTOR_BUFFER * pSectorData
)
{ /* Body */
    RtStatus_t rtCode;
    uint32_t originalLogicalSectorNumber = u32LogicalSectorNumber;
    uint32_t u32PhysicalBlockNumber;
    uint32_t u32PhysicalSectorOffset;
    uint32_t u32LogicalBlockAddress;
    uint32_t u32LogicalSectorOffset;
    uint32_t physicalSectorNumber;
    Region * pRegion;
    uint32_t u32Occupied;

#if REPORT_ECC_FAILURES
    NandEccCorrectionInfo_t correctionInfo;
#endif /* REPORT_ECC_FAILURES */

    /* Make sure we're initialized */
    if (!m_bInitialized)
    {
        return ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED;
    } /* Endif */

    /* Make sure we won't go out of bounds */
    if (u32LogicalSectorNumber >= m_u32NumberOfSectors)
    {
        return ERROR_DDI_LDL_LDRIVE_SECTOR_OUT_OF_BOUNDS;
    } /* Endif */

    /* Lock the NAND for our purposes. */
    DdiNandLocker locker;

    /* Disable auto sleep for the whole read process. */
    NandHal::SleepHelper disableSleep(false);
    
    /* Convert the logical sector to a virtual sector. */
    rtCode = logicalSectorToVirtual(originalLogicalSectorNumber, NULL, &u32LogicalSectorNumber);
    if (rtCode != SUCCESS)
    {
        return rtCode;
    } /* Endif */

    /* 
    ** Now figure out where this sector is actually mapped to.  Given the
    ** Relative Sector Number, we need to figure out which LBA this is
    ** (Easy because the LBA is just the Block number for this sector) and
    ** which Physical block this is mapped to as well as the Physical
    ** Sector offset (which page this would be if LBA was linearly mapped). 
    */
    rtCode = m_media->getMapper()->getPageInfo(u32LogicalSectorNumber,
    &u32LogicalBlockAddress,
    &u32PhysicalBlockNumber,
    &u32LogicalSectorOffset);

    if (rtCode == ERROR_DDI_NAND_MAPPER_INVALID_PHYADDR)
    {
        /* 
        ** Hack to always read an erased page in the case where the caller wants
        ** to read a page that hasn't yet been allocated, so we fill the sector with 0xFF
        ** and return 
        */
        _wl_mem_set(pSectorData, 0xFF, NandHal::getFirstNand()->pNANDParams->pageDataSize);

        return SUCCESS;
    } 
    else if (rtCode != SUCCESS)
    {
        /* Got a real error while trying to lookup the physical address for this LBA. */
        return rtCode;
    } /* Endif */

    nand::NonsequentialSectorsMap * sectorMap = m_media->getNssmManager()->getMapForVirtualBlock(u32LogicalBlockAddress);
    if (!sectorMap)
    {
        return ERROR_DDI_NAND_DATA_DRIVE_CANT_ALLOCATE_USECTORS_MAPS;
    } /* Endif */

    /* 
    ** We now have the LBA and the Physical Block.  We now need to grab the
    ** NonSequential Sector Map entry from the NS Sector Map because it holds
    ** the mappings between the desired linear pages and the actual scrambled
    ** pages 
    */
    rtCode = sectorMap->getEntry(u32LogicalSectorOffset, &u32PhysicalBlockNumber, &u32PhysicalSectorOffset, &u32Occupied);
    if (rtCode != SUCCESS)
    {
        return rtCode;
    } /* Endif */

    /* 
    ** An attempt was made to read a sector which was never written to. 
    ** Return all 0xff's 
    */
    if (!u32Occupied)
    {
        /* 
        ** Hack to always read an erased page so we fill the sector with 0xFF 
        ** and return 
        */
        _wl_mem_set(pSectorData, 0xFF, NandHal::getFirstNand()->pNANDParams->pageDataSize);

        return SUCCESS;
    } /* Endif */

    /* 
    ** These calls can modify u32PhyscialBlockNumber if device uses multiple Nands 
    ** and if u32PhysicalBlockNumber indicates a block which is in a Nand other than 
    ** Nand number 0. 
    */
    pRegion = m_media->getRegionForBlock(u32PhysicalBlockNumber);
    assert(pRegion->m_eDriveType == kDriveTypeData || pRegion->m_eDriveType == kDriveTypeHidden);

    /* Compute the physical sector number. */
    physicalSectorNumber = pRegion->m_nand->blockAndOffsetToRelativePage(u32PhysicalBlockNumber, u32PhysicalSectorOffset);
    
    WL_LOG(WL_MODULE_LOGICALDRIVE, WL_LOG_INFO, "\nNDDRS Drive %d, Lpage 0x%X, LBlk 0x%X, PBlk 0x%X, Ppage 0x%X\n",
    m_u32Tag, u32LogicalSectorNumber,
    u32LogicalBlockAddress, u32PhysicalBlockNumber, physicalSectorNumber);

    /* Get a buffer. */
    AuxiliaryBuffer auxBuffer;
    if ((rtCode = auxBuffer.acquire()) != SUCCESS)
    {
        return rtCode;
    } /* Endif */

    rtCode = pRegion->m_nand->readPage(physicalSectorNumber, pSectorData, auxBuffer);

    /* Release aux buffer as soon as possible. */
    auxBuffer.release();
    
    if (is_read_status_error_excluding_ecc(rtCode))
    {
        
        return rtCode;
    } /* Endif */

    /* Verify the ECC */
    if (rtCode != SUCCESS)
    {
        WL_LOG(WL_MODULE_LOGICALDRIVE, WL_LOG_INFO, "\nNDDRS FAILED Drive %d, Lpage 0x%X, LBlk 0x%X, PBlk 0x%X, Ppage 0x%X retcode 0x%X\n",
        m_u32Tag, u32LogicalSectorNumber,
        u32LogicalBlockAddress, u32PhysicalBlockNumber, 
        physicalSectorNumber, rtCode);
#if REPORT_ECC_FAILURES
        log_ecc_failures(u32PhysicalBlockNumber, u32PhysicalSectorOffset, &correctionInfo);
#endif

        if (rtCode == ERROR_DDI_NAND_HAL_ECC_FIXED)
        {
            /* This error simply indicates that there were correctable bit errors. */
        }
        else if (rtCode == ERROR_DDI_NAND_HAL_ECC_FIX_FAILED)
        {
            /* 
            ** There were uncorrectable bit errors in the data, so there's nothing we can do
            ** except return an error. 
            */
            return rtCode;
        }
        else if (rtCode == ERROR_DDI_NAND_HAL_ECC_FIXED_REWRITE_SECTOR)
        {
            /* 
            ** The ECC hit the threshold, so we must rewrite the block contents to a different
            ** physical block, thus refreshing the data. Create a task to do it in the background. */
            RelocateVirtualBlockTask * task = new RelocateVirtualBlockTask(m_media->getNssmManager(), u32LogicalBlockAddress);
            assert(task);
            if (task)
            {
                m_media->getDeferredQueue()->post(task);
            } /* Endif */
        } /* Endif */
    } /* Endif */

    return SUCCESS;
} /* Endbody */

/* EOF */
