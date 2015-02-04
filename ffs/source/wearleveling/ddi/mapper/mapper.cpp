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
*   This file contains common NAND Logical Block Address Mapper functions.
*
*
*END************************************************************************/
#include "ddi/common/ddi_nand_ddi.h"
#include "ddi/data_drive/ddi_nand_data_drive.h"
#include "ddi/media/ddi_nand_media.h"
#include "hal/ddi_nand_hal.h"
#include "ddi_media.h"
#include "ddi/mapper/mapper.h"
#include "buffer_manager/media_buffer.h"
#include "ddi/common/ddi_block.h"
#include "ddi/common/ddi_page.h"
#include "ddi/mapper/zone_map_section_page.h"
#include "ddi/mapper/zone_map_cache.h"
#include "ddi/mapper/persistent_phy_map.h"
#include <stdlib.h>
#include <string.h>
#include "simple_timer.h"

using namespace nand;

#if defined(__ghs__)
#pragma ghs section text=".init.text"
#endif 

#if defined (__CODEWARRIOR__)
#endif 

Mapper::Mapper(Media * media)
:   m_media(media),
    m_zoneMap(NULL),
    m_phyMapOnMedia(NULL),
    m_physMap(NULL),
    m_prebuiltPhymap(NULL),
    m_unallocatedBlockAddress(0),
    m_isInitialized(false),
    m_isZoneMapCreated(false),
    m_isPhysMapCreated(false),
    m_isMapDirty(false),
    m_isBuildingMaps(false),
    m_nextMapSearchStartBlock(0),
    m_u32PhysMapCurrentIdxPosition(0),
    m_u16PhysMapCurrentPos(0)
{ /* Body */    
    /* Clear the reserved range info fields in one call. */
    _wl_mem_set(&m_reserved, 0, sizeof(m_reserved));
} /* Endbody */

#if defined(__ghs__)
#pragma ghs section text=default
#endif 

#if defined (__CODEWARRIOR__)
#endif 

Mapper::~Mapper()
{
    
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : init
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function initializes the mapper. It will perform the following tasks:
*   - Initialize and obtain all necessary memory
*   - Create the zone and phy maps in RAM, either by
*       1. Loading from archived maps on the NAND
*       2. Rebuilding by scanning addresses from RA on the NAND.
*
*END*--------------------------------------------------------------------*/
RtStatus_t Mapper::init()
{ /* Body */
    RtStatus_t retCode = SUCCESS;
    uint32_t trustMediaResidentMaps;
    bool bRangeMoved=false;

    /* Only need to initialize these values once. */
    if (!m_isInitialized)
    {
        /* The value for the unallocated block address depends on the zone map entry size. */
#if !NAND_MAPPER_FORCE_24BIT_ZONE_MAP_ENTRIES
        if (m_media->getTotalBlockCount() < kNandZoneMapSmallEntryMaxBlockCount)
        {
            m_unallocatedBlockAddress = kNandMapperSmallUnallocatedBlockAddress;
        }
        else
#endif /* !NAND_MAPPER_FORCE_24BIT_ZONE_MAP_ENTRIES */
        {
            m_unallocatedBlockAddress = kNandMapperLargeUnallocatedBlockAddress;
        } /* Endif */
    } /* Endif */

    /* Allocate the phy map, unless we were provided with a prebuilt one. */
    if (!m_physMap && !m_prebuiltPhymap)
    {
        m_physMap = new PhyMap;
        m_physMap->init(m_media->getTotalBlockCount());
        m_physMap->setDirtyCallback(phymapDirtyListener, this);
    } /* Endif */

    /* Allocate the zone map. */
    if (!m_zoneMap)
    {
        m_zoneMap = new ZoneMapCache(*this);
        m_zoneMap->init();
    } /* Endif */

    /* Allocate the persistent phymap. */
    if (!m_phyMapOnMedia)
    {
        m_phyMapOnMedia = new PersistentPhyMap(*this);
        m_phyMapOnMedia->init();
        
        if (m_physMap)
        {
            m_phyMapOnMedia->setPhyMap(m_physMap);
        } /* Endif */
    } /* Endif */

    /* 
    ** We need the know the reserved block range before doing anything that touches
    ** the zone or phy maps on the media. 
    */
    retCode = computeReservedBlockRange(&bRangeMoved);
    if (retCode)
    {
        return retCode;
    } /* Endif */

    /* Check to see if we already inialized */
    if ((bRangeMoved == false) && m_isInitialized)
    {
        return (SUCCESS);
    } /* Endif */

    /* 
    ** We are here if either isInitialized is false or the reserved block range is different from
    ** previous allocation. Following state variables may not be false if reserved block range is
    ** moved but isInitialized is true. They should be set to false in order for proper zone map
    ** recreation. */
    m_isInitialized = false;
    m_isZoneMapCreated = false;
    m_isPhysMapCreated = false;
    m_isBuildingMaps = false;

    /* Start searching for map blocks from the first reserved block. */
    m_nextMapSearchStartBlock = m_reserved.startBlock;
    m_u32PhysMapCurrentIdxPosition = 0;
    m_u16PhysMapCurrentPos = 0;

    /* 
    ** Restore the allocation pointer from persistent bits. 
    ** If we've lost persistent bits, then this 
    ** value reverts to 0 which is perfectly fine. 
    */
    WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Mapper allocation starts from block %d\n", m_u32PhysMapCurrentIdxPosition * PhyMap::kBlocksPerEntry);

    if (bRangeMoved)
    {
        /* If the range has moved then we want to always recreate zone map so we can force relocation of map. */
        trustMediaResidentMaps = false;
    }
    else
    {
        /* 
        ** If this persistent bit is set, it means that the device was gracefully shutdown
        ** and we should trust maps stored on the media. 
        */
        trustMediaResidentMaps = true;
    } /* Endif */

    if (trustMediaResidentMaps)
    {
        /* Try to load the zone and phy maps from media. */
        WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Loading maps from media\n");
                
        /* Find and load the phy map, then get trustNumber of phymap */
        retCode = m_phyMapOnMedia->load();
        
        if (retCode == SUCCESS)
        {
            calculateAllocationIndex();
            /* Locate and init the zone map, then get trustNumber of zonemap */
            retCode = m_zoneMap->findZoneMap();
        } /* Endif */

        if (retCode == SUCCESS)
        {
            /* Compare two above numbers */
            WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "\nTrust number in phy = %d\n", m_phyMapOnMedia->getTrustNumber());
            WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "\nTrust number in zone = %d\n", m_zoneMap->getTrustNumber());
            if (m_zoneMap->getTrustNumber() + 1 == (uint16_t)-1) 
            {
                if (m_phyMapOnMedia->getTrustNumber() == 0) {
                    calculateNextMapSearchStartBlock();
                    m_isZoneMapCreated = true;
                    m_isPhysMapCreated = true;
                } 
                else 
                {
                    trustMediaResidentMaps = false;
                } /* Endif */
            } 
            else 
            {
                if (m_phyMapOnMedia->getTrustNumber() == m_zoneMap->getTrustNumber() + 1) 
                {
                    calculateNextMapSearchStartBlock();
                    m_isZoneMapCreated = true;
                    m_isPhysMapCreated = true;
                } 
                else 
                {
                    trustMediaResidentMaps = false;
                } /* Endif */
            } /* Endif */
        }
        else
        {
            WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Loading maps failed with error 0x%08x\n", retCode);
        } /* Endif */
    } /* Endif */

    if (!trustMediaResidentMaps || retCode != SUCCESS)
    {
        /* 
        ** The maps are corrupted or can not be found on the media, or the system was
        ** shutdown uncleanly and we cannot trust the maps. 
        */
        WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Scanning media to create maps\n");

        /* 
        ** Rebuild the Zone Map and Phy Map from RA data on the media.
        ** This function will also erase any pre-existing maps which are stored 
        ** on the media. 
        */
        retCode = createZoneMap();
        
        if (retCode != SUCCESS)
        {
            return retCode;            
        } /* Endif */
    } /* Endif */

    m_isInitialized = true;

    /* 
    ** Go clean out the reserved block range of any blocks that shouldn't be there.
    ** This is necessary because the reserved block range may potentially move or grow
    ** between boots due to new bad blocks. 
    */
    retCode = evacuateReservedBlockRange();
    if (retCode)
    {
        return retCode;
    } /* Endif */

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : rebuild
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   If the zone and phy maps in RAM have already been initialized,
*   then this function reinitializes them.
*   They are reinitialized either by loading them from archived
*   copies on the media, or by scanning the RA of the media and rebuilding them.
*   On the other hand, if the maps in RAM are currently uninitialized, then no 
*   action is taken.
*   Generally, you would want to call this function if the
*   maps in RAM do not match the true state of the media.
*   This function repairs the maps in RAM.
*
*END*--------------------------------------------------------------------*/
RtStatus_t Mapper::rebuild()
{ /* Body */
    RtStatus_t ret = SUCCESS;

    /* If zone-map was not created, there is nothing to re-create. */
    if (m_isInitialized)
    {
        /* Must flush NSSMs before rebuilding to avoid conflicts. */
        m_media->getNssmManager()->flushAll();

        m_isInitialized    = FALSE;
        m_isZoneMapCreated = FALSE;
        m_isPhysMapCreated = FALSE;

        /* 
        ** Set the dirty flag to make sure we actually recreate the zone map
        ** instead of just loading it from media. 
        */
        setDirtyFlag();

        /* Allocate needed buffers, and fill in the zone and  phy maps in RAM. */
        ret = init();
    } /* Endif */

    return ret;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : computeReservedBlockRange
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS Reserved range computed.
*   - ERROR_DDI_NAND_LMEDIA_NO_REGIONS_IN_MEDIA
* Comments         :
*   This function determines the block range reserved for the zone and phy maps.
*   The requirements for the reserved block range are as follows:
*   - The range must contain at least #kNandMapperReservedBlockCount good
*   blocks that are allocated to a data or hidden drive.
*   - It must start after all boot blocks.
*   - The reserved range must never extend beyond LBA search range, the first
*   200 blocks on the first chip.
*
*   It is alright for the reserved block range to span across system drives.
*   This is because system drive regions are marked as used or bad in the
*   phy map, so the mapper will never attempt to allocate those blocks.
*
*   The pbRangeMoved is added to let the caller know that the reserved block ranges
*   has moved since last allocation. The range will be different if there is a change
*   in config blocks layout or blocks within the reserved block range gone bad.
*
*END*--------------------------------------------------------------------*/
RtStatus_t Mapper::computeReservedBlockRange
(
    /* [OUT] Flag holds state of reserved blocks */
    bool * pbRangeMoved
)
{ /* Body */
    uint32_t startBlock;
    uint32_t blockNumber;
    uint32_t count;

    Region * region = NULL;
    bool foundGoodBlock;

    /* There must be at least one region. */
    assert(m_media->getRegionCount());

    /* We start by finding the first data-type region. */
    Region::Iterator it = m_media->createRegionIterator();
    region = it.getNext();
    while (region)
    {
        /* Exit loop if this is a data-type region. */
        if (region->isDataRegion())
        {
            break;
        } /* Endif */
        region = it.getNext();
    } /* Endwhile */

    /* Validate the region. */
    if (!(region && region->m_iChip == 0))
    {
        return ERROR_DDI_NAND_LMEDIA_NO_REGIONS_IN_MEDIA;
    } /* Endif */

    /* The reserved range starts with the first block of the first data-type region. */
    startBlock = region->m_u32AbPhyStartBlkAddr;

    /* Prepare for the search loop. */
    blockNumber = startBlock;
    count = 0;
    foundGoodBlock = false;

    /* 
    ** Get a buffer to hold the redundant area. We allocate the buffer here and pass it
    ** to isMarkedBad() instead of letting it continually reallocate the buffer. 
    */
    AuxiliaryBuffer auxBuffer;
    RtStatus_t status = auxBuffer.acquire();
    if (status != SUCCESS)
    {
        return status;
    } /* Endif */

    /* 
    ** Scan and count up the required number of good reserved blocks. This loop also observes
    ** regions, so that system drive blocks are skipped over. Until the first good block is
    ** found, the start of the reserved region is moved forward each bad block. 
    */
    while (count < kNandMapperReservedBlockCount)
    {
        /* Have we moved beyond the end of the current region? */
        if (blockNumber - region->m_u32AbPhyStartBlkAddr >= (uint32_t)region->m_iNumBlks)
        {
            /* Move to the next data-type region. */
            region = it.getNext();
            while (region)
            {
                /* Break if the region is a data-type region. System regions are skipped. */
                if (region->isDataRegion())
                {
                    break;
                } /* Endif */
                region = it.getNext();
            } /* Endwhile */
            
            /* Make sure we still have a valid region. */
            if (!region)
            {
                return ERROR_DDI_NAND_LMEDIA_NO_REGIONS_IN_MEDIA;
            } /* Endif */
            
            /* Update block number to start at this region. */
            blockNumber = region->m_u32AbPhyStartBlkAddr;
        } /* Endif */
        
        /* Check if this block is bad. */
        if (!Block(BlockAddress(blockNumber)).isMarkedBad(auxBuffer))
        {
            /* This is a good block, so include it in the reserved block count. */
            count++;
            foundGoodBlock = true;
        } /* Endif */
        
        /* Move to the next block. */
        blockNumber++;
        
        /* Adjust the start of the reserved region until the first good block is found. */
        if (!foundGoodBlock)
        {
            startBlock = blockNumber;
        } /* Endif */
    } /* Endwhile */

    /* Initialize default value in return parameter */
    *pbRangeMoved = false;    

    /* 
    ** We chose two parameters that can tell if there is a change to reserved blocks from last
    ** configuration, the start block and reserved block count. The start block will be different
    ** if there is a change in layout of boot blocks and reserved block count will increase if
    ** blocks within the reserved block range gone bad. Any of the two changes then we need to
    ** return true in pbRangeMoved.
    ** 
    ** m_isInitialized should be true before verifying reserved blocks range has moved.  
    */
    if (m_isInitialized && ((m_reserved.startBlock != startBlock) ||
                (m_reserved.blockCount != (blockNumber - startBlock))))
    {
        *pbRangeMoved = true;    
    } /* Endif */

    /* 
    ** Record a bunch of precomputed information about the reserved blocks, all to be
    ** used to speed up looking for an available block. 
    */
    m_reserved.startBlock = startBlock;
    m_reserved.blockCount = blockNumber - startBlock;
    m_reserved.startPhyMapEntry = startBlock / PhyMap::kBlocksPerEntry;

    /* Adjust start block to be the offset into the start entry of the first reserved block. */
    startBlock -= m_reserved.startPhyMapEntry * PhyMap::kBlocksPerEntry;

    /* Compute how many phy map entries the reserved blocks span. */
    m_reserved.phyMapEntrySpan = PhyMap::getEntryCountForBlockCount(startBlock + m_reserved.blockCount);

    /* Offsets of the leading and trailing ragged edges. */
    m_reserved.leadingOffset = startBlock;
    m_reserved.trailingOffset = (startBlock + m_reserved.blockCount) - (m_reserved.phyMapEntrySpan - 1) * PhyMap::kBlocksPerEntry;

    m_reserved.hasLeadingEdge = m_reserved.leadingOffset > 0;
    m_reserved.hasTrailingEdge = m_reserved.trailingOffset < PhyMap::kBlocksPerEntry - 1;

    /* Last entry containing a reserved block. */
    m_reserved.endPhyMapEntry = m_reserved.startPhyMapEntry + m_reserved.phyMapEntrySpan - 1;

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : evacuateReservedBlockRange
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_NAND_LMEDIA_NO_REGIONS_IN_MEDIA
* Comments         :
*   This function evicts any undesired blocks from the reserved block range.
*   The mapper must be fully initialised before this function is called.
*   In particular, either CreateZoneMap or LoadZoneMap must have been called.
*
*END*--------------------------------------------------------------------*/
RtStatus_t Mapper::evacuateReservedBlockRange()
{ /* Body */
    unsigned blockNumber;
    RtStatus_t status;
    Region * region = NULL;
    unsigned reservedStartBlock = m_reserved.startBlock;
    unsigned regionStart;
    unsigned regionEnd;

    /* Find the region that holds the first block of the reserved range. */
    Region::Iterator it = m_media->createRegionIterator();
    region = it.getNext();
    while (region)
    {
        regionStart = region->m_u32AbPhyStartBlkAddr;
        regionEnd = regionStart + region->m_iNumBlks;
        
        /* Exit loop if this is the matching region. */
        if (reservedStartBlock >= regionStart && reservedStartBlock < regionEnd)
        {
            break;
        } /* Endif */
        region = it.getNext();
    } /* Endwhile */

    /* Validate the region. */
    if (!region)
    {
        return ERROR_DDI_NAND_LMEDIA_NO_REGIONS_IN_MEDIA;
    } /* Endif */

    /* Get a buffer to hold the redundant area. */
    AuxiliaryBuffer auxBuffer;
    if ((status = auxBuffer.acquire()) != SUCCESS)
    {
        return status;
    } /* Endif */

    /* Iterate over all blocks in the reserved block range. */
    Block scanBlock(reservedStartBlock);
    for (blockNumber = 0; blockNumber < m_reserved.blockCount; ++blockNumber, ++scanBlock)
    {
        bool isMapBlock;
        unsigned blockPhysicalAddress = reservedStartBlock + blockNumber;
        
        /* Have we gone beyond the current region's end? */
        if (blockPhysicalAddress >= regionEnd)
        {
            /* Advance the region while skipping over system regions. */
            region = it.getNext();
            while (region)
            {
                /* Exit the loop unless this is a system region. */
                if (region->m_eDriveType != kDriveTypeSystem)
                {
                    break;
                } /* Endif */
                
                /* 
                ** We're skipping over a region, 
                ** so we need to advance the block counter to match. 
                */
                blockNumber += region->m_iNumBlks;
                
                region = it.getNext();
            } /* Endwhile */
            
            /* Make sure we still have a valid region. */
            if (!region)
            {
                return ERROR_DDI_NAND_LMEDIA_NO_REGIONS_IN_MEDIA;
            } /* Endif */
            
            /* Update region info. */
            regionStart = region->m_u32AbPhyStartBlkAddr;
            regionEnd = regionStart + region->m_iNumBlks;
            
            /* Recompute the current block address. */
            blockPhysicalAddress = reservedStartBlock + blockNumber;
            scanBlock = BlockAddress(blockPhysicalAddress);
        } /* Endif */
        
        /* We can just ignore bad blocks. */
        if (scanBlock.isMarkedBad(auxBuffer))
        {
            continue;
        } /* Endif */
        
        /* Check if this is a zone map block. */
        isMapBlock = isBlockMapBlock(blockPhysicalAddress, kMapperZoneMap, &status);
        if (status)
        {
            break;
        } /* Endif */
        
        /* Leave the current zone map block in place. */
        if (isMapBlock && m_zoneMap && m_zoneMap->isMapBlock(blockPhysicalAddress))
        {
            continue;
        } /* Endif */
        
        /* Check for a phy map block. */
        if (!isMapBlock)
        {
            isMapBlock = isBlockMapBlock(blockPhysicalAddress, kMapperPhyMap, &status);
            if (status)
            {
                break;
            } /* Endif */
            
            /* Don't erase the current phy map block. */
            if (isMapBlock && m_phyMapOnMedia && m_phyMapOnMedia->isMapBlock(scanBlock))
            {
                continue;
            } /* Endif */
        } /* Endif */
        
        /* Handle different block types separately. */
        if (isMapBlock)
        {
            /* 
            ** Map blocks get erased and marked unused. This is OK because we've already
            ** made sure that we're not erasing the current zone or phy map blocks above. 
            */
            status = m_physMap->markBlockFreeAndErase(blockPhysicalAddress);
            if (status)
            {
                break;
            } /* Endif */
        }
        else
        {
            /* 
            ** We have a potential data block here, so we need to read its metadata. This will
            ** both tell us if the block is erased and its LBA if not. 
            */
            
            /* Read the metadata of the data block's first page so we can determine its LBA. */
            status = scanBlock.readMetadata(kFirstPageInBlock, auxBuffer);
            if (!is_read_status_success_or_ecc_fixed(status))
            {
                break;
            } /* Endif */
            
            /* Check if this is an erased block. */
            Metadata md(auxBuffer);
            if (!md.isErased())
            {
                /* Evacuate this data block to somewhere out of the reserved range. */
                NonsequentialSectorsMap * map = m_media->getNssmManager()->getMapForVirtualBlock(md.getLba());
                if (map)
                {
                    /* This call will pick a new physical block for us. */
                    status = map->relocateVirtualBlock(blockPhysicalAddress, NULL);
                }
                else
                {
                    /* 
                    ** We didn't get an NSSM for the virtual block, so the block must be 
                    ** invalid or something. Just erase it. 
                    */
                    status = m_physMap->markBlockFreeAndErase(blockPhysicalAddress);
                } /* Endif */
                
                if (status)
                {
                    break;
                } /* Endif */
            } /* Endif */
        } /* Endif */ 
    } /* Endfor */

    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : phymapDirtyListener
* Returned Value   : void
* Comments         :
*   This function handler for dirty state changes of the phymap.
*
*END*--------------------------------------------------------------------*/
void Mapper::phymapDirtyListener
(   
    /* [IN] The phy map */
    PhyMap * thePhymap, 
    
    /* [IN] Dirty flag (before) */
    bool wasDirty, 
    
    /* [IN] Dirty flag (present) */
    bool isDirty, 
    
    /* [IN] Mapper */
    void * refCon
)
{ /* Body */
    Mapper * _this = (Mapper *)refCon;
    assert(_this);

    assert(thePhymap == _this->m_physMap); 

    /* 
    ** We only need to handle the case where the map is becoming dirty for the first time
    ** after being clean. 
    */
    if (isDirty && !wasDirty)
    {
        _this->setDirtyFlag();
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setDirtyFlag
* Returned Value   : void
* Comments         :
*   Call this function any time either the zone map or phy map is modifed
*   in order to set the dirty flag. This will cause the maps to be written
*   to media when flush() is called.
*
*END*--------------------------------------------------------------------*/
void Mapper::setDirtyFlag()
{ /* Body */
    if (!m_isMapDirty)
    {
        /* Indicate that the zone map has been touched. */
        m_isMapDirty = true;
        
        /* Clear the persistent bit that says it's safe to load from media. */
        WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Nand mapper is dirty\n");
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : clearDirtyFlag
* Returned Value   : void
* Comments         :
*   This function records that the maps match those on the media.
*
*END*--------------------------------------------------------------------*/
void Mapper::clearDirtyFlag()
{ /* Body */
    if (m_isMapDirty)
    {
        m_isMapDirty = false;
        
        /* 
        ** Set the persistent bit that says we can trust the maps resident on the media.
        ** This bit will get cleared when the map is marked dirty. 
        */
        WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Nand mapper is clean\n");
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : shutdown
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function shuts down the mapper. It will perform the following tasks:
*   - Free up all memory
*   - Flush the Zone map to Nand
*   - Remove all system resources such as mutex, semaphore, etc. if necessary.
*   If successful, the Zone Map Table and Erase Block Table have been
*   flushed to the NAND.
*
*END*--------------------------------------------------------------------*/
RtStatus_t Mapper::shutdown()
{ /* Body */
    RtStatus_t ret;

    if (!m_isInitialized)
    {
        return SUCCESS;
    } /* Endif */

    /* Flush the zone map to nand. */
    ret = flush();
    if (ret)
    {
        return ret;
    } /* Endif */

    /* Shutdown the zone map cache. */
    if (m_zoneMap)
    {
        delete m_zoneMap;
        m_zoneMap = NULL;
    } /* Endif */

    /* Free the dynamically allocated phy map. */
    if (m_physMap)
    {
        delete m_physMap;
        m_physMap = NULL;
    } /* Endif */

    /* Free the dynamically allocated phy map. */
    if (m_phyMapOnMedia)
    {
        delete m_phyMapOnMedia;
        m_phyMapOnMedia = NULL;
    } /* Endif */

    /* Clear the prebuilt phymap. */
    m_prebuiltPhymap = NULL;

    /* Mark as uninitialized. */
    m_isInitialized = false;
    m_isZoneMapCreated = false;
    m_isPhysMapCreated = false;

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setBlockInfo
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function updates information of a Logical block address
*
*END*--------------------------------------------------------------------*/
RtStatus_t Mapper::setBlockInfo
(
    /* [IN] Logical block address */
    uint32_t u32Lba, 
    
    /* [IN] Physical block address */
    uint32_t u32PhysAddr
)
{ /* Body */
    /* Update the zone map. */
    RtStatus_t ret = m_zoneMap->setBlockInfo(u32Lba, u32PhysAddr);
    if (ret)
    {
        return ret;
    } /* Endif */

    /* Mark this block as used in the phymap. */
    if (!isBlockUnallocated(u32PhysAddr))
    {
        ret = m_physMap->markBlockUsed(u32PhysAddr);
        if (ret)
        {
            return ret;
        } /* Endif */
    } /* Endif */

    /* Indicate that the zone map has been touched */
    setDirtyFlag();

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getBlock
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS If no error has occurred.
*   - ERROR_DDI_NAND_MAPPER_GET_BLOCK_FAILED No more blocks are available.
* Comments         :
*   This function allocates a physical block. The block is guaranteed to be
*   erased and ready for use when the call returns.
*
*END*--------------------------------------------------------------------*/
RtStatus_t Mapper::getBlock
(
    /* [IN] Pointer to actual Physical Block Address */
    uint32_t * pu32PhysBlkAddr, 
    
    /* [IN] Characteristic */
    MapperBlockTypes_t eBlkType, 
    
    /* [IN] How many contiguous blocks are desired */
    uint32_t u32StartRange)
{ /* Body */
    RtStatus_t rtCode;

    do
    {
        /* Grab a free block */
        rtCode = allocateBlock(pu32PhysBlkAddr, eBlkType, u32StartRange);
        if (rtCode != SUCCESS )
        {
            return rtCode;
        } /* Endif */

        /* Mark the location in the available block as taken. */
        rtCode = m_physMap->markBlockUsed(*pu32PhysBlkAddr);
        if (rtCode != SUCCESS)
        {
            return rtCode;
        } /* Endif */

        /* As well as setting the bit, erase physical block. */
        Block block(*pu32PhysBlkAddr);
        if (!block.isErased())
        {
            /* 
            ** If the erase fails, then loop again and try again with another block. We have
            ** already marked the bad block as used in the phy map, so we just need to mark the
            ** block itself as bad. 
            */
            rtCode = block.erase();
            if (rtCode == ERROR_DDI_NAND_HAL_WRITE_FAILED)
            {
                /* This will mark the block used in the phymap again, but not a big deal. */
                handleNewBadBlock(block);
            } /* Endif */
        } /* Endif */
    } 
    while (rtCode != SUCCESS); /* Endwhile */

    return rtCode;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : sufficientNumOfFreeBlocks
* Returned Value   : Status of call or error.
* - TRUE if there is sufficient number of free blocks.
* - FALSE if there is not sufficient number of free blocks.
* Comments         :
*   This function determines whether or not there are sufficient number
* of free blocks to allow GetBlockAndAssign function to succeed in the
* case where a new block is being allocated without freeing up previously
* used block.
*
* We need a block into which we can copy an used block.  We cannot let
* ourselves get into a situation where all blocks are used and we cannot
* erase any other block without losing data.  Also, we need to allow for
* blocks going bad.
*
*END*--------------------------------------------------------------------*/
uint32_t Mapper::sufficientNumOfFreeBlocks()
{
    uint32_t u32PhymapFreeCount = m_physMap->getFreeCount();
    uint32_t u32TotalBlocks = NandHal::getFirstNand()->wTotalBlocks;
    uint32_t u32Threshold = MAPPER_COMPUTE_RESERVE(u32TotalBlocks);

    /* 
    ** Need to allow for 1 Free block and at least three
    ** free blocks which go bad. 
    */
    assert(u32Threshold > MAPPER_MINIMUM_THRESHOLD);

    return (u32PhymapFreeCount >= u32Threshold);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getBlockAndAssign
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function allocates a physical nand block to a LBA. The application can
*   request the allocated physical block to have to following characteristics:
*   - LBA: this block is to be used to store the ZONE map. In order to speed
*   up the search for ZONE map during startup, it is better to allocate
*   this Zone map block in the first 200 blocks of the NAND. In the case
*   that all the blocks in the first 200 blocks have been occupied, then
*   this function must evict a block within this area to another area.
*   - Odd: Allocate an odd Physical block. This might be used for multi-page programming.
*   - Even: Allocate an even Physical block. This might be used for multi-page programming.
*
*END*--------------------------------------------------------------------*/
RtStatus_t  Mapper::getBlockAndAssign
(
    /* [IN] Logical Block Address to convert */
    uint32_t           u32Lba,
    
    /* [IN] Pointer to actual Physical Block Address */
    uint32_t           *pu32PhysBlkAddr,
    
    /* [IN] Characteristic */
    MapperBlockTypes_t eBlkType,

    /* [IN] How many contiguous blocks are desired.  */
    uint32_t           u32StartRange
)
{ /* Body */
    RtStatus_t rtCode;
    uint32_t u32PrevPhysBlkAddr;

    /* See if there is already a physical block assigned to this LBA. */
    rtCode = getBlockInfo(u32Lba, &u32PrevPhysBlkAddr);
    if (rtCode)
    {
        return rtCode;
    }

    /* 
    ** If this is the first time we're alloccating a block to this LBA, we need
    ** to make sure that there are enough free blocks remaining.
    */
    if (isBlockUnallocated(u32PrevPhysBlkAddr))
    {
        if (!sufficientNumOfFreeBlocks())
        {
            return ERROR_DDI_NAND_MAPPER_ZONE_MAP_INSUFFICIENT_NUM_FREE_BLOCKS;
        }
    }

    /* Allocate the block. */
    rtCode = getBlock(pu32PhysBlkAddr, eBlkType, u32StartRange);
    if (rtCode)
    {
        return rtCode;
    } /* Endif */

    /* Assign this physical block to LBA in zone-map */
    rtCode = setBlockInfo(u32Lba, *pu32PhysBlkAddr);
    if (rtCode)
    {
        return rtCode;
    } /* Endif */

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : markBlock
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS If no error has occurred.
*   - ERROR_DDI_NAND_MAPPER_INVALID_PHYADDR
* Comments         :
*   This function marks a block as bad/used or unused.
*
*   A block has been allocated by the mapper. The mapper should mark this
*   physical address as bad or unallocated in the Erased Block Table (essentially this block
*   is never released.  It should be noted that the LBA should never be marked
*   bad. The Erased Block Table must be updated .  Thus in this case, the
*   mapper will:
*   - Mark the physical block as bad or unused; deallocate the LBA <-> physical block
*   association, so that it looks like this LBA location has not been
*   allocated yet.
*   - Update the Erased Block table to show this block as unerased so that
*   it won't be allocated.
*   - Same as above. In addition to that, it will allocate a new physical
*   block and returns this new address to the caller.
*
*END*--------------------------------------------------------------------*/
RtStatus_t Mapper::markBlock
(
    /* [IN] Logical block address to mark as bad. */
    uint32_t u32Lba, 
    
    /* [IN] Pointer to actual physical block address. */
    uint32_t u32PhysBlkAddr, 
    
    /* [IN] Unused == true, used = false. */
    bool bUsedorUnused
)
{ /* Body */
    uint32_t u32ComparePhysBlkAddr;
    RtStatus_t ret;

    /* 
    ** Read the Physical block address from the zone map and confirm that the two values
    ** one from the API and one the Zone map are identical 
    */
    ret = getBlockInfo(u32Lba, &u32ComparePhysBlkAddr);
    if (ret)
    {
        return ret;
    } /* Endif */

    /* Verify that the physical block associated with the LBA is what we expect. */
    if (u32PhysBlkAddr != u32ComparePhysBlkAddr)
    {
        return ERROR_DDI_NAND_MAPPER_INVALID_PHYADDR;
    } /* Endif */

    /* 
    ** This does not mean that the lba is bad, it really means that the physical block
    ** associated with this lba is bad. As a result, we should only mark the
    ** the corresponding location in the available phy block location as allocated 
    */
    ret = m_physMap->markBlock(u32PhysBlkAddr, bUsedorUnused, PhyMap::kAutoErase);
    if (ret)
    {
        return ret;
    } /* Endif */

    /* We also mark the Zone map associated with this LBA as unallocated. */
    ret = setBlockInfo(u32Lba, m_unallocatedBlockAddress);
    if (ret)
    {
        return ret;
    } /* Endif */

#ifdef DEBUG_MAPPER2
    if (bUsedorUnused)
    {
        WL_LOG(WL_INFOR, "Marking P%d as Used.\n", u32PhysBlkAddr);
    }
    else
    {
        WL_LOG(WL_INFOR, "Marking P%d as Unused.\n", u32PhysBlkAddr);
    } /* Endif */
#endif

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : flush
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function flushs the contents of the zone map and phy map to the NAND.
*
*   Writes any dirty sections of the zone map held in the cache to the zone map
*   block. This will trigger a consolidation of the zone map if the block
*   becomes full. The phy map is also written to media in its own block. A new
*   block is allocated and erased by this function for the phy map.
*
*END*--------------------------------------------------------------------*/
RtStatus_t Mapper::flush()
{ /* Body */
    RtStatus_t ret;

    /* Don't do anything if the cache is clean. */
    if (!m_isMapDirty)
    {
        return SUCCESS;
    } /* Endif */

start_flush:

    /* Maps are no longer dirty. */
    m_physMap->isDirty();
    m_physMap->clearDirty();
    clearDirtyFlag();

    /* Flush out the zone map. */
    /* We save zonemap with TRUST flag to media */
    ret = m_zoneMap->flush();
    if (ret != SUCCESS)
    {
        return ret;
    } /* Endif */    

    /* Force saving phymap */
    m_phyMapOnMedia->setTrustFlag(m_zoneMap->getTrustFlag());
    if (m_zoneMap->getTrustNumber() + 1 == (uint16_t)-1) 
    {
        m_phyMapOnMedia->setTrustNumber((uint16_t)0);
    } 
    else 
    {
        m_phyMapOnMedia->setTrustNumber(m_zoneMap->getTrustNumber() + 1);
    } /* Endif */
    ret = m_phyMapOnMedia->save();
    if (ret)
    {
        return ret;
    } /* Endif */

    /* 
    ** Handle the case where writing one of the maps caused the other map to become
    ** dirty by flushing everything again. This can happen if one of the maps is full
    ** and has to be consolidated into a newly allocated block. 
    */
    if (m_isMapDirty)
    {
        WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "maps were dirtied during flush! trying to flush again...\n");
        goto start_flush;
    } /* Endif */

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : findMapBlock
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function searches the reserved block range for a zone map block.
*
*END*--------------------------------------------------------------------*/
RtStatus_t Mapper::findMapBlock
(
    /* [IN] Which Map type */
    MapperMapTypes_t eMapType, 
    
    /* [OUT] Pointer to actual Physical Block Address */
    uint32_t * pu32PhysBlkAddr
)
{ /* Body */
    uint32_t i;
    bool bBlockIsLBA;
    RtStatus_t RtStatus;

    /* Start searching at the first region. */
    for (i = m_reserved.startBlock; i < m_reserved.startBlock + m_reserved.blockCount; i++)
    {
        bBlockIsLBA = isBlockMapBlock(i, eMapType, &RtStatus);

        if (SUCCESS != RtStatus)
        {
            return RtStatus;
        } /* Endif */
        
        /* If there isn't a match, continue search. */
        if (bBlockIsLBA)
        {
            *pu32PhysBlkAddr = i;
            return SUCCESS;
        } /* Endif */
    } /* Endfor */

    return ERROR_DDI_NAND_MAPPER_FIND_LBAMAP_BLOCK_FAILED;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : handleNewBadBlock
* Returned Value   : MQX_OK or an error code
* Comments         :
*   This function should be called whenever a new bad block is encountered in
*   the area of the NAND managed by the mapper. It updates the phymap, marks
*   the block itself as bad, and updates the region that owns the block. For
*   data regions this only means incrementing the bad block count. The DBBT is
*   scheduled for update as a result of updating the region.
*
*END*--------------------------------------------------------------------*/
void Mapper::handleNewBadBlock
(
    /* [IN] New bad block address */
    const BlockAddress & badBlockAddress
)
{ /* Body */
    WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "*** New bad block %u! ***\n", badBlockAddress.get());

    /* Mark the block as bad in the phymap. */
    m_physMap->markBlockUsed(badBlockAddress);

    /* Now write the bad block markers. */
    Block(badBlockAddress).markBad();

    getMedia()->getBBT()->markBad(badBlockAddress.get());
    
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : searchAndDestroy
* Returned Value   : void
* Comments         :
*   This function searches for and erases all occurrences of zone-map and phymap.
*   This is done when we find out that power was lost.  Consequently, we cannot trust
*   zone-map and phys-map which is stored in Nand.
*
*END*--------------------------------------------------------------------*/
void Mapper::searchAndDestroy()
{ /* Body */
    uint32_t    i;
    RtStatus_t retCode;

    /* Start searching at the first region. */
    for (i = m_reserved.startBlock; i < m_reserved.startBlock + m_reserved.blockCount; i++)
    {
        /* If there isn't a match, continue search. */
        if (isBlockMapBlock(i, kMapperZoneMap, &retCode)
                || isBlockMapBlock(i, kMapperPhyMap, &retCode))
        {
            m_physMap->markBlockFreeAndErase(i);
        } /* Endif */

    } /* Endfor */

    /* Clear the valid flags for the maps. */
    m_isZoneMapCreated = false;
    m_isPhysMapCreated = false;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : createZoneMap
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function will search the entire NAND,
*   by reading the RA, and extract the LBA <-> Physical Block Address
*   information so that a Zone map can be created.
*
*END*--------------------------------------------------------------------*/
RtStatus_t Mapper::createZoneMap()
{
    RtStatus_t ret;

    /* Mark that we are building the maps from scratch. */
    m_isBuildingMaps = true;
    AutoClearFlag buildingMapsFlagController(m_isBuildingMaps);

    /* 
    ** Erase any pre-existing map blocks from the media. This is necessary, for instance,
    ** if the phy map was written successfully but upon init the zone map could not be
    ** found for some reason, thus causing CreateZoneMap() to be called. Repeat this
    ** process over and over, and you leak phy map blocks. 
    */
    searchAndDestroy();

    /* Get a buffer to hold the redundant area. */
    AuxiliaryBuffer auxBuffer;
    if ((ret = auxBuffer.acquire()) != SUCCESS)
    {
        return ret;
    } /* Endif */

    /* 
    ** This is needed to satisfy check at beginning of setBlockInfo(),
    ** which is called from inside the following loop. 
    */
    m_isInitialized = true;

    /* Don't let the NAND go to sleep during the scans. */
    NandHal::SleepHelper disableSleep(false);

    if (m_prebuiltPhymap)
    {
        WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Using phymap built during allocation instead of scanning again\n");

        /* Validate the number of entries. */
        assert(m_prebuiltPhymap->getEntryCount() == PhyMap::getEntryCountForBlockCount(m_media->getTotalBlockCount()));
        
        /* Make use of the prebuilt phymap that someone so kindly provided us. */
        m_physMap = m_prebuiltPhymap;
        m_prebuiltPhymap = NULL;
        m_phyMapOnMedia->setPhyMap(m_physMap);

        /* 
        ** Set our dirty change listener in the phymap, since it won't be set since we didn't
        ** create this phymap instance. 
        */
        m_physMap->setDirtyCallback(phymapDirtyListener, this);
    }
    else
    {
        /* Nobody gave us a phymap, so we have to build one of our own. */
        ret = scanAndBuildPhyMap(auxBuffer);
        if (ret != SUCCESS)
        {
            return ret;
        } /* Endif */
    } /* Endif */

    /* This function is used to replace a part of loadAllocationIndex() */
    calculateAllocationIndex();

    /* 
    ** The phymap has been filled in, so we want to write it out to the NAND. We have to 
    ** save a new copy because we erased all resident maps above. 
    */
    ret = m_phyMapOnMedia->saveNewCopy();
    if (ret)
    {
        return ret;
    } /* Endif */

    /* 
    ** This function writes the cache buffer with all unallocated entries for every
    ** section of the zone map. This is done so that there is at least a default entry
    ** for every zone map section and entry. 
    */
    ret = m_zoneMap->writeEmptyMap();
    if (ret)
    {
        return ret;
    } /* Endif */

    /* Scan the NAND to build the zone map. */
    ret = scanAndBuildZoneMap(auxBuffer);
    if (ret != SUCCESS)
    {
        return ret;
    } /* Endif */

    /* The maps have now been created. */
    m_isZoneMapCreated = true;
    m_isPhysMapCreated = true;

    /* We want to assign value to m_nextMapSearchStartBlock variable here, after scanning and building PhyMap + ZoneMap */
    calculateNextMapSearchStartBlock();

    /* 
    ** We want zone-map to be written out during FlushToNand 
    ** regardless of whether or not anything has changed.
    ** Otherwise the next time device boots up, zone-map
    ** will be created again instead of being loaded. 
    */
    setDirtyFlag();

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : scanAndBuildPhyMap
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function scans devices and build phymap
*
*END*--------------------------------------------------------------------*/
RtStatus_t Mapper::scanAndBuildPhyMap
(
    /* [IN] Buffer to holds metadata */
    AuxiliaryBuffer & auxBuffer
)
{ /* Body */
    assert(m_physMap);

    RtStatus_t ret;

    /* Zero out the phys map so that all blocks are marked used. */
    m_physMap->markAll(PhyMap::kUsed);

    /* Create an iterator over all of the media's regions. */
    Region::Iterator it = m_media->createRegionIterator();
    Region * region;

    SimpleTimer timer;

    /* first loop to fill in phy-map */
    region = it.getNext();
    while (region)
    {
        /* System Drives need to be marked as used in the map so only check Data Drives. */
        if (!region->isDataRegion())
        {
            continue;
        } /* Endif */
        
        int32_t numBlocksInRegion = region->m_iNumBlks;
        Block blockInRegion(region->m_u32AbPhyStartBlkAddr);
        
        for (; numBlocksInRegion; --numBlocksInRegion, ++blockInRegion)
        {
            assert(blockInRegion < m_media->getTotalBlockCount());
            
            /* Check to see if the block is bad or not */
            ret = blockInRegion.readMetadata(kFirstPageInBlock, auxBuffer);
            /* Get Logical Block Address and Relative Sector Index from RA */
            Metadata md(auxBuffer);
            
            if (md.isMarkedBad())
            {
                /* 
                ** Mark the block bad in phys map
                ** Since this array contains the map across all chips, we need to add the
                ** offset from all previous chips. 
                */
                ret = m_physMap->markBlockUsed(blockInRegion);
                if (ret)
                {
                    return ret;
                } /* Endif */
                
                continue;
            } /* Endif */
            
            /* The block is good, so what kind of block is it?             */
            if (ret == ERROR_DDI_NAND_HAL_ECC_FIX_FAILED)
            {
                /* 
                ** Mark the location in the available block as unused, which will also erase it.
                ** Note that this will destroy data, but there is no other choice at this point. 
                */
                ret = m_physMap->markBlockFreeAndErase(blockInRegion);
                
                /* On to the next block */
                continue;
            }
            else if (!is_read_status_success_or_ecc_fixed(ret))
            {
                /* Some other error occurred, that we cannot process. */
                #ifdef DEBUG_MAPPER2
                WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO,
                "Problem reading first page of block %u, ret=0x%08x\n", blockInRegion.get(), ret);
                #endif
                
                return ret;
            } /* Endif */

            /* if Erased, the this block has not been allocated */
            if (md.isErased())
            {
                /* 
                ** Mark the location in the available block as free. No need to erase since
                ** we've already checked that. 
                */
                ret = m_physMap->markBlockFree(blockInRegion);
            }
            else
            {
                /* Mark the location in the available block as taken */
                ret = m_physMap->markBlockUsed(blockInRegion);
            } /* Endif */

            if (ret)
            {
                return ret;
            } /* Endif */
        }
        region = it.getNext();
    } /* Endwhile */

    WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Scanning to build phy map took %d ms\n", uint32_t(timer.getElapsed() / 1000));

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : scanAndBuildZoneMap
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function scans device and build zonemap
*
*END*--------------------------------------------------------------------*/
RtStatus_t Mapper::scanAndBuildZoneMap
(
    /* [IN] Buffer holds metadata */
    AuxiliaryBuffer & auxBuffer
)
{ /* Body */
    RtStatus_t ret;

    Region::Iterator it = m_media->createRegionIterator();
    Region * region;

    SimpleTimer timer;

    /* second loop to fill in zone-map */
    region = it.getNext();
    while (region)
    {
        if (!region->isDataRegion())
        {
            continue;
        } /* Endif */
        
        int32_t numBlocksInRegion = region->m_iNumBlks;
        Block blockInRegion(region->m_u32AbPhyStartBlkAddr);
        
        for (; numBlocksInRegion; --numBlocksInRegion, ++blockInRegion)
        {
            assert(blockInRegion < m_media->getTotalBlockCount());
            
            ret = blockInRegion.readMetadata(kFirstPageInBlock, auxBuffer);
            Metadata md(auxBuffer);
            
            /* Skip over blocks that are not marked as used in the phymap or are marked bad. */
            if (!m_physMap->isBlockUsed(blockInRegion) || md.isMarkedBad())
            {
                continue;
            } /* Endif */
            
            
            if (ret == ERROR_DDI_NAND_HAL_ECC_FIX_FAILED)
            {
                /* 
                ** Mark the location in the available block as unused, which will also erase it.
                ** Note that this will destroy data, but there is no other choice at this point. 
                */
                m_physMap->markBlockFreeAndErase(blockInRegion);
                
                /* On to the next block */
                continue;
            }
            else if (!is_read_status_success_or_ecc_fixed(ret))
            {
                /* Some other error occurred, that we cannot process. */
                #ifdef DEBUG_MAPPER2
                WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO,
                "Problem reading first page of block %u, ret=0x%08x\n", blockInRegion.get(), ret);
                #endif
                
                return ret;
            } /* Endif */

            /* Get Logical Block Address and Relative Sector Index from RA */
            
            uint32_t u32LogicalBlockAddr = md.getLba();

            /* if Erased, the this block has not been allocated */
            if (md.isErased())
            {
                continue;
            } /* Endif */
            
            /* 
            ** Check to see if this is a system block or not. If it is then ignore the LBA.
            ** The bottom half-word of the Stmp code is equivalent to the RSI. 
            */
            uint16_t rsiFull = md.getSignature() & 0xffff;
            uint8_t rsi1 = rsiFull & 0xff; // Low byte of the Stmp code.

            /* 
            ** If this block is the zone or phy map (indicated by a valid Stmp code),
            ** then skip it. It's not the zone map if either the full RSI half-word is 0,
            ** or the high byte of the RSI is 0 and the LBA is valid (within range). 
            */
            
            if (((rsi1 == 0) && (u32LogicalBlockAddr < m_media->getTotalBlockCount())) || (rsiFull == 0))
            {
                /* Allocated this block in the zone map */
                if (u32LogicalBlockAddr > m_media->getTotalBlockCount())
                {
                    /* 
                    ** Something is seriously wrong with what was in 
                    ** redundant area.  Ignore for now and continue.
                    ** Mark the location in the available block as unused, which will also erase it.
                    ** Note that this will destroy data, but there is no other choice at this point. 
                    */
                    m_physMap->markBlockFreeAndErase(blockInRegion);
                    
                    continue;
                } /* Endif */

                uint32_t u32PhysicalBlockNumber;
                ret = getBlockInfo(u32LogicalBlockAddr, &u32PhysicalBlockNumber);
                if (ret)
                {
                    return ret;
                } /* Endif */

                if (!isBlockUnallocated(u32PhysicalBlockNumber) && (u32PhysicalBlockNumber != blockInRegion))
                {
                    /* 
                    ** This can only occur in the case where 
                    ** power is shut off before "old" and "new"
                    ** blocks assigned to given LBA are merged. 
                    */
                    NonsequentialSectorsMap map;
                    map.init(m_media->getNssmManager(), 0);
                    ret = map.resolveConflict(u32LogicalBlockAddr, u32PhysicalBlockNumber, blockInRegion);
                }
                else
                {
                    ret = setBlockInfo(u32LogicalBlockAddr, blockInRegion);
                } /* Endif */

                if (ret)
                {
                    return ret;
                } /* Endif */
            } /* Endif */
        } /* Endfor */
        region = it.getNext();
    } /* Endwhile */

    WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Scanning to build zone map took %d ms\n", uint32_t(timer.getElapsed() / 1000));

    return SUCCESS;
} /* Endbody */

/* EOF */
