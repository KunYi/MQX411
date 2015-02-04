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
*   This file contains local NAND Logical Block Address Mapper functions.
*
*
*END************************************************************************/

#include "wl_common.h"
#include "ddi/common/ddi_nand_ddi.h"
#include "ddi/data_drive/ddi_nand_data_drive.h"
#include "ddi_nand_hal.h"
#include "ddi_media.h"
#include "ddi/mapper/mapper.h"
#include <string.h>
#include "buffer_manager/media_buffer.h"
#include "ddi/mapper/persistent_phy_map.h"
#include "ddi/mapper/zone_map_cache.h"
#include "ddi/mapper/zone_map_section_page.h"

using namespace nand;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : searchPhyMapEntry
* Returned Value   : The index of an available block is returned, or -1 is returned if
*   the scanned range of the entry is completely occupied.
* Comments         :
*   This function searches a phy map entry for an empty block.
*
*END*--------------------------------------------------------------------*/
int Mapper::searchPhyMapEntry
(
    /* [IN] The 16-bit bit field to search, where a 1 bit is an available slot and 0 is occupied. */
    uint32_t entryBitField, 
    
    /* [IN] Starting bit to search from. */
    int startIndex, 
    
    /* 
    ** [IN] The bit number at which the search will be stopped. 
    ** This is one position after the last bit examined. 
    */
    int endIndex, 
    
    /* [OUT] Pointer to a bool which is set to true upon a positive match. */
    bool * foundFreeBlock
)
{ /* Body */
    int index = startIndex;
    
    /* Check index ranges. */
    assert(startIndex >= 0 && startIndex < PhyMap::kBlocksPerEntry);
    assert(endIndex >= 0 && endIndex <= PhyMap::kBlocksPerEntry);
    
    /* Shift entry value to start index. */
    if (startIndex)
    {
        entryBitField >>= startIndex;
    } /* Endif */
    
    /* Scan each bit looking for a 1. */
    while (index < endIndex && (entryBitField & 1) == 0)
    {
        index++;
        entryBitField >>= 1;
    } /* Endwhile */
    
    *foundFreeBlock = index < endIndex;
    
    /* Return a -1 if no available slot was found. */
    return index < endIndex ? index : -1;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : allocateBlock
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function gets an unused phy block from the array.
*
*   The phymap is an array that will indicate whether a physical block has been
*   allocated or not. In the current design, in order to save memory, each bit
*   in this array and its corresponding location tells us the exactly the
*   status of a corresponding physical block in the nand.
*   The two available status are:
*   - Free, unallocated;
*   - Bad or has been allocated
*
*END*--------------------------------------------------------------------*/
RtStatus_t Mapper::allocateBlock
(
    /* [IN] Physical Block Address to mark as avaliable. */
    uint32_t *pu32PhyAddr,
    
    /* [IN] Type of block required. */
    MapperBlockTypes_t eBlkType,
    
    /* 
    ** [IN] Start of range of block in which block should
    ** be allocated if one is available in given range.  
    ** Since if block cannot be found in
    ** range we will not evict an existing block
    ** to enforce the range, the size of the range ends up being suprerfluous. 
    */
    uint32_t u32StartRange
)
{ /* Body */
    uint32_t coarseIndex;
    uint32_t endCoarseIndex;
    uint32_t fineIndex;
    uint32_t *phyMapArrayPtr;
    /* Each of the bits, when set, represent a free block. */
    uint32_t physMapAvailBitField; 
    uint32_t u32PhyMapSize;
    bool foundAvailableBlock = false;
    uint32_t u32PhysCurrentIdxPosition;
    int currentBitPosition;
    bool lookingForMapBlock = false;
    int segmentCounter;
    int numberOfSegments;

    /* Get the total number of phymap entries. */
    u32PhyMapSize = m_physMap->getEntryCount();

    /* Set return value to an initial value. */
    *pu32PhyAddr = 0;

    /* The requested block type determines the starting point in the phy map for the search. */
    switch (eBlkType)
    {
        case kMapperBlockInRange:
            u32PhysCurrentIdxPosition = u32StartRange / PhyMap::kBlocksPerEntry;
            currentBitPosition = u32StartRange % PhyMap::kBlocksPerEntry;
            break;
            
        case kMapperBlockTypeMap:
            /* Start the map block search from the block after the last one we found. */
            u32PhysCurrentIdxPosition = m_nextMapSearchStartBlock / PhyMap::kBlocksPerEntry;
            currentBitPosition = m_nextMapSearchStartBlock % PhyMap::kBlocksPerEntry;
            
            lookingForMapBlock = true;
            break;

        case kMapperBlockTypeNormal:
            /* 
            ** eBlkType requested is normal. Limit the current index and entry position to the
            ** number of entries in the phymap, just in case either one somehow got out of whack. 
            */
            u32PhysCurrentIdxPosition = m_u32PhysMapCurrentIdxPosition % m_physMap->getEntryCount();
            currentBitPosition = m_u16PhysMapCurrentPos % PhyMap::kBlocksPerEntry;
    }

    /* 
    ** Scan all the entries in the phymap array for available block
    ** We assume that 1 is available and 0 is taken
    ** Do a coarse search
    ** We start a search from the current position to the very end 
    */
    phyMapArrayPtr = &(*m_physMap)[u32PhysCurrentIdxPosition];
    
    /* Init search loop variables. */
    coarseIndex = u32PhysCurrentIdxPosition;
    segmentCounter = 0;
    
    /* 
    ** The number of search segments is computed differently depending on whether we're looking
    ** for a map block or a regular data block. 
    */
    if (lookingForMapBlock)
    {
        numberOfSegments = m_nextMapSearchStartBlock == m_reserved.startBlock ? 1 : 2;
        endCoarseIndex = m_reserved.endPhyMapEntry + 1;
    }
    else
    {
        /* 
        ** The number of search segments depends on whether the search is starting from the
        ** very beginning of the phy map or not. If it is, then the only segment is the whole
        ** phy map. Otherwise there are two segments, first from the search start to the end,
        ** second from the phy map beginning to the search start. 
        */
        numberOfSegments = coarseIndex == 0 ? 1 : 2;
        endCoarseIndex = u32PhyMapSize;
    } /* Endif */
    
    do {
        /* Get this phy map entry. */
        physMapAvailBitField = *phyMapArrayPtr++;
        
        /* Don't bother with the entry if it is full. */
        if (physMapAvailBitField != PhyMap::kFullEntry)
        {
            /* Handle searches within the reserved block range specially. */
            // TODO: change if condition to make sure driver not always chose the first block behind reserve blocks.
            if (lookingForMapBlock || coarseIndex < m_reserved.startPhyMapEntry || coarseIndex > m_reserved.endPhyMapEntry)
            {
                int startBit = 0;
                
                /* Use the search start bit to pick up from where the last search left off. */
                if (coarseIndex == u32PhysCurrentIdxPosition)
                {
                    startBit = currentBitPosition;
                }
                
                /* 
                ** Either we're looking for a map block, which can come from anywhere, or
                ** we're not in the reserved block range, so we can go ahead and do the fine search. 
                */
                fineIndex = searchPhyMapEntry(physMapAvailBitField, startBit, PhyMap::kBlocksPerEntry, &foundAvailableBlock);
            }
            else
            {
                /* 
                ** We're looking for a normal block within the reserved block range, which
                ** requires special attention. In order to keep the logic simpler here, we
                ** do not pay attention to the currentBitPosition variable. 
                */
                
                /* Search for blocks before the reserved range starts. */
                if (m_reserved.hasLeadingEdge && coarseIndex == m_reserved.startPhyMapEntry)
                {
                    /* 
                    ** Start the search at the first bit and end at the point where the
                    ** reserved range begins. 
                    */
                    fineIndex = searchPhyMapEntry(physMapAvailBitField, 0, m_reserved.leadingOffset, &foundAvailableBlock);
                }
                
                /* Search for blocks after the reserved range ends. */
                if (!foundAvailableBlock && m_reserved.hasTrailingEdge && coarseIndex == m_reserved.endPhyMapEntry)
                {
                    /* 
                    ** Start the bit search just after the reserved block range, and go
                    ** until the end of the entry. 
                    */
                    fineIndex = searchPhyMapEntry(physMapAvailBitField, m_reserved.trailingOffset, PhyMap::kBlocksPerEntry, &foundAvailableBlock);
                } /* Endif */
            } /* Endif */
            
            /* Exit the search loop if we've found an available block. */
            if (foundAvailableBlock)
            {
                /* Make sure the fine index is within range. */
                assert(fineIndex >= 0 && fineIndex < PhyMap::kBlocksPerEntry);
                
                break;
            } /* Endif */
        } /* Endif */
        
        /* 
        ** Increment the coarse index to move to the next phy map entry. If we hit the end of
        ** the phy map, start over at the beginning. Only if there are remaining segments will
        ** we actually loop, though. 
        */
        if (++coarseIndex >= endCoarseIndex)
        {
            segmentCounter++;
            
            if (lookingForMapBlock)
            {
                /* 
                ** When looking for a map block, we loop back to the beginning of the
                ** reserved range, not the beginning of the nand. 
                */
                coarseIndex = m_reserved.startPhyMapEntry;
                currentBitPosition = m_reserved.leadingOffset;
            }
            else
            {
                /* Loop back to the beginning of the nand. */
                coarseIndex = 0;
                currentBitPosition = 0;
            } /* Endif */
            
            endCoarseIndex = u32PhysCurrentIdxPosition;
            phyMapArrayPtr = &(*m_physMap)[coarseIndex];
        } /* Endif */
    } while (segmentCounter < numberOfSegments); /* Endwhile */

    /* Return an error if there was no match. */
    if (!foundAvailableBlock)
    {
        return ERROR_DDR_NAND_MAPPER_PHYMAP_MAPFULL;
    } /* Endif */

    /* Compute and return the physical block address. */
    *pu32PhyAddr = coarseIndex * PhyMap::kBlocksPerEntry + fineIndex;

    /* Start the next search from the location _after_ the one we just found. */
    if (lookingForMapBlock)
    {
        m_nextMapSearchStartBlock = *pu32PhyAddr + 1;
        
        /* Wrap back to the beginning of the reserved range if needed. */
        if (m_nextMapSearchStartBlock >= m_reserved.startBlock + m_reserved.blockCount)
        {
            m_nextMapSearchStartBlock = m_reserved.startBlock;
        } /* Endif */
    }
    else if (kMapperBlockInRange != eBlkType)
    {
        m_u32PhysMapCurrentIdxPosition = coarseIndex;
        m_u16PhysMapCurrentPos = fineIndex + 1;
        
        /* Wrap current position. */
        if (m_u16PhysMapCurrentPos >= PhyMap::kBlocksPerEntry)
        {
            m_u16PhysMapCurrentPos = 0;
            if (++m_u32PhysMapCurrentIdxPosition >= u32PhyMapSize)
            {
                m_u32PhysMapCurrentIdxPosition = 0;
            } /* Endif */
        } /* Endif */
    } /* Endif */

    return SUCCESS;
} /* Endbody */

// extern MediaAllocationTable_t g_mediaTablePreDefine[];
extern nand::NandZipConfigBlockInfo_t g_nandZipConfigBlockInfo;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : calculateAllocationIndex
* Returned Value   : void
* Comments         :
*   This function calculates allocation index
*
*END*--------------------------------------------------------------------*/
void Mapper::calculateAllocationIndex() 
{
    Media * currentMedia;
    BadBlockTable * currentBBT;
    uint8_t * backupBBTEntries;
    uint8_t * predefinedBlocksMap; // TODO: Idea at 13/12/11 (The 3rd time)
    uint32_t * entriesBackup;
    int32_t i;
    uint32_t j;
    uint32_t maxRange = 0;
    
    currentMedia = getMedia();
    currentBBT = currentMedia->getBBT();
    
    /* 
    ** Calculate m_u32PhysMapCurrentIdxPosition
    ** In loadAllocationIndex function, m_u32PhysMapCurrentIdxPosition variable is loaded from rom 
    */
    m_u32PhysMapCurrentIdxPosition = 0;
    m_u16PhysMapCurrentPos = 0;

    // uint32_t maxRange = g_mediaTablePreDefine[0].Entry[0].u64SizeInBytes / 
    // (NandHal::getParameters().pageTotalSize * NandHal::getParameters().wPagesPerBlock);
    maxRange = g_nandZipConfigBlockInfo.Regions[0].iStartBlock;
    for (i = 0; i < g_nandZipConfigBlockInfo.iNumEntries; i++)
    {
        maxRange += g_nandZipConfigBlockInfo.Regions[i].iNumBlks;
    }

    // TODO: Create predefinedBlocksMap. Idea at 13/12/11 (The 3rd time)
    /* Create predefinedBlocksMap */
    predefinedBlocksMap = (uint8_t *)_wl_mem_alloc_zero(sizeof(uint8_t) * PhyMap::kBlocksPerEntry * ((maxRange / PhyMap::kBlocksPerEntry) + 1));
    for (i = 0; i < g_nandZipConfigBlockInfo.iNumEntries; i++)
    {
        for (j = (uint32_t)g_nandZipConfigBlockInfo.Regions[i].iStartBlock; 
                j < (uint32_t)(g_nandZipConfigBlockInfo.Regions[i].iStartBlock + g_nandZipConfigBlockInfo.Regions[i].iNumBlks); j++)
        {
            if (g_nandZipConfigBlockInfo.Regions[i].eDriveType == kDriveTypeData)
            {
                predefinedBlocksMap[j] = 1;
            }
        }
    }
    
    /* Backup m_entries in m_physMap */
    entriesBackup = (uint32_t *)_wl_mem_alloc(sizeof(uint32_t) * ((maxRange / PhyMap::kBlocksPerEntry) + 1));
    _wl_mem_copy(entriesBackup, m_physMap->getAllEntries(), sizeof(uint32_t) * ((maxRange / PhyMap::kBlocksPerEntry) + 1));
    
    /* Backup m_entries in BBT and make it suitable with m_entries in m_physmap */
    backupBBTEntries = (uint8_t *)_wl_mem_alloc_zero(sizeof(uint8_t) * PhyMap::kBlocksPerEntry * ((maxRange / PhyMap::kBlocksPerEntry) + 1));
    _wl_mem_copy(backupBBTEntries, currentBBT->getEntries(), maxRange);

    i = 0;

    /* Change "bad" bit to "free" bit */
    for (i = 0; i < (int32_t)(PhyMap::kBlocksPerEntry * ((maxRange / PhyMap::kBlocksPerEntry) + 1)); i++) 
    {
        // TODO: Change to make work with multi DATA regions. Now, if a block was bad, or if it is unavailable (means it's not DataType)
        // TODO: then in new entries map, it will be set 1. Must re-check.
        // TODO: The 1st time
        // if (backupBBTEntries[i] == 0) 
        // TODO: The 2nd time
        /* if ((backupBBTEntries[i] == 0) 
            || (backupBBTEntries[i] == 1 
                && ((entriesBackup[i / PhyMap::kBlocksPerEntry] & (1 << (i % PhyMap::kBlocksPerEntry))) >> (i % PhyMap::kBlocksPerEntry)) == 0)) */
        // TODO: The 3rd time
        if ((backupBBTEntries[i] == 0) || (predefinedBlocksMap[i] == 0))
        {
            entriesBackup[i / PhyMap::kBlocksPerEntry] |= (1 << (i % PhyMap::kBlocksPerEntry));
        } /* Endif */
    } /* Endfor */
    
    uint8_t scanedIdx = 0;
    do 
    {
        for (i = maxRange / PhyMap::kBlocksPerEntry; i >= scanedIdx; i--) 
        {
            if (entriesBackup[i] != 0 && entriesBackup[i] != 0xFFFFFFFF) 
            {
                m_u32PhysMapCurrentIdxPosition = i;
                /* Quit  */
                break;
            } /* Endif */

            if (m_u16PhysMapCurrentPos == PhyMap::kBlocksPerEntry && i == scanedIdx) 
            {
                m_u32PhysMapCurrentIdxPosition++;
                break;
            } /* Endif */
        } /* Endfor */
        
        /* Find out index of the first zero bit from left to right */
        uint32_t current = entriesBackup[m_u32PhysMapCurrentIdxPosition];

        for (i = 0; i < (int32_t)sizeof(uint32_t) * 8; i++) 
        {
            uint32_t mask = (1 << (sizeof(uint32_t) * 8 - 1 - i));
            uint32_t tmp = current & mask;
            if (tmp == 0) 
            {
                m_u16PhysMapCurrentPos = sizeof(uint32_t) * 8 - i;
                break;
            } 
            else 
            {
                m_u16PhysMapCurrentPos = 0;
            } /* Endif */
        } /* Endif */

        if (m_u16PhysMapCurrentPos == PhyMap::kBlocksPerEntry) {
            scanedIdx = m_u32PhysMapCurrentIdxPosition + 1;
        } /* Endfor */
    } while (m_u16PhysMapCurrentPos == PhyMap::kBlocksPerEntry); /* Endwhile */

    if (m_u32PhysMapCurrentIdxPosition >= m_physMap->getEntryCount()) 
    {
        m_u32PhysMapCurrentIdxPosition = 0;
    } /* Endif */
    
    /* Free allocated pointers */
    _wl_mem_free(entriesBackup);
    _wl_mem_free(backupBBTEntries);
    _wl_mem_free(predefinedBlocksMap);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : calculateNextMapSearchStartBlock
* Returned Value   : MQX_OK or an error code
* Comments         :
*   This function calculates and finds out the next block to use with map
*
*END*--------------------------------------------------------------------*/
void Mapper::calculateNextMapSearchStartBlock() 
{ /* Body */
    m_nextMapSearchStartBlock = MAX(m_phyMapOnMedia->getMapBlock(), m_zoneMap->getMapBlock()) + 1;
    if (m_nextMapSearchStartBlock >= m_reserved.startBlock + m_reserved.blockCount) 
    {
        m_nextMapSearchStartBlock = m_reserved.startBlock;
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : isBlockMapBlock
* Returned Value   : TRUE or FALSE
*   - TRUE if block is zone-map.
*   - FALSE if block is not zone-map. 
* Comments         :
*   This function determine whether or not Block is LBA block.
*   Figure out whether or not the given block number is of a block which
*   contains zone-map (LBA).
*
*END*--------------------------------------------------------------------*/
bool Mapper::isBlockMapBlock
(   
    /* [IN] Number of physical block to consider */
    uint32_t u32PhysicalBlockNum, 
    
    /* [IN] Type of Map */
    MapperMapTypes_t eMapType, 
    
    /* [IN] Pointer to receive RtStatus_t of this function.  */
    RtStatus_t * pRtStatus
)
{ /* Body */
    /* Read the redundant area of the first page. */
    Page firstPage(PageAddress(u32PhysicalBlockNum, 0));
    firstPage.allocateBuffers(false, true);
    RtStatus_t status = firstPage.readMetadata();
    
    if (pRtStatus)
    {
        *pRtStatus = status;
    } /* Endif */

    if (status != SUCCESS)
    {
        return false;
    } /* Endif */

    /* Determine the map type */
    uint32_t u32LbaCode1;
    switch (eMapType)
    {
        case kMapperZoneMap:
            u32LbaCode1 = (uint32_t)LBA_STRING_PAGE1;
            break;
        case kMapperPhyMap:
            u32LbaCode1 = (uint32_t)PHYS_STRING_PAGE1;
            break;
    } /* Endswitch */

    /* Read the Stmp code */
    return (firstPage.getMetadata().getSignature() == u32LbaCode1);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setPrebuiltPhymap
* Returned Value   : void
* Comments         :
*   This function sets mew PHyMap
*
*END*--------------------------------------------------------------------*/
void Mapper::setPrebuiltPhymap(PhyMap * theMap)
{ /* Body */
    m_prebuiltPhymap = theMap;
} /* Endbody */

/* EOF */
