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
*   This file contains details of the NSSM class and NSSM manager.
*
*
*END************************************************************************/
#include "wl_common.h"
#include "ddi/common/ddi_nand_ddi.h"
#include "ddi/data_drive/ddi_nand_data_drive.h"
#include "hal/ddi_nand_hal.h"
#include <string.h>
#include <stdlib.h>
#include "ddi/mapper/mapper.h"
#include "buffer_manager/media_buffer.h"

using namespace nand;

#define LOG_NSSM_METADATA_ECC_LEVELS        0

#define MAX_BUILD_NSSM_READ_TRIES           2

/* The number of pages per block that the NSSM count is defined in. */
#define NSSM_BASE_PAGE_PER_BLOCK_COUNT      (128)

#if DEBUG && NSSM_INDUCE_ONE_PAGE_FAILURE
/* A flag to cause one sector to be omitted from the NSSM. */
static bool stc_bNSSMInduceOnePageFailure = false;
#endif

#if !defined(__ghs__)
//#pragma mark --NssmManager--
#endif

#if defined(__ghs__)
#pragma ghs section text=".init.text"
#endif 

#if defined (__CODEWARRIOR__)
#endif 

NssmManager::NssmManager(Media * nandMedia)
:   m_media(nandMedia),
    m_mapper(NULL),
    m_mapCount(0),
    m_mapsArray(NULL),
    m_index(),
    m_lru(0, 0, 0)
{
    /* Clear all statistics values to 0. */
    _wl_mem_set(&m_statistics, 0, sizeof(m_statistics));

    m_mapper = getMedia()->getMapper();
}

#if defined(__ghs__)
#pragma ghs section text=default
#endif 

#if defined (__CODEWARRIOR__)
#endif 

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : allocate
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS If no error has occurred.
*   - ERROR_DDI_NAND_DATA_DRIVE_CANT_ALLOCATE_USECTORS_MAPS
* Comments         :
*   This function dynamically allocates the NSSectorsMaps memory for the given
*   number of map entries, whose sizes depend on the quantity of sectors per block.
*   The actual quantity of entries in NSSectorsMaps memory may be adjusted up or
*   down from the requested value uMapsPerBaseNSSMs, depending on the quantity 
*   of sectors per block in the NAND.
*   uMapsPerBaseNSSMs is normalized to NSSM_BASE_PAGE_PER_BLOCK_COUNT pages per block.
*
*   The new NSSMs are pushed onto the NSSM LRU. If the NSSM array has previously
*   been allocated and the requested uMapsPerBaseNSSMs is the same, then this function
*   does nothing and returns immediately.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NssmManager::allocate
(   
    /* 
    ** [IN] Number of maps to allocate, normalized to a NAND
    ** with \a NSSM_BASE_PAGE_PER_BLOCK_COUNT pages per block.
    ** Must be greater than 0. 
    */
    unsigned uMapsPerBaseNSSMs)
{ /* Body */
    int32_t i32NumSectorsPerBlock = NandHal::getParameters().wPagesPerBlock;    
    unsigned mapsCount;

    if (uMapsPerBaseNSSMs == 0)
    {
        /* Could not fit even one map... */
        return ERROR_DDI_NAND_DATA_DRIVE_CANT_ALLOCATE_USECTORS_MAPS;
    } /* Endif */

    /* 
    ** Adjust the number of maps to allocate based on how many pages per block the NAND has.
    ** The number of maps is defined in terms of NSSM_BASE_PAGE_PER_BLOCK_COUNT (nominally 128) 
    ** pages per block. So if there are fewer 
    ** pages per block, then the number of maps is increased. Vice versa for more pages per 
    ** block--the number of maps is decreased. 
    */
    if (i32NumSectorsPerBlock < NSSM_BASE_PAGE_PER_BLOCK_COUNT)
    {
        mapsCount = uMapsPerBaseNSSMs * (NSSM_BASE_PAGE_PER_BLOCK_COUNT / i32NumSectorsPerBlock);
    }
    else if (i32NumSectorsPerBlock > NSSM_BASE_PAGE_PER_BLOCK_COUNT)
    {
        mapsCount = uMapsPerBaseNSSMs / (i32NumSectorsPerBlock / NSSM_BASE_PAGE_PER_BLOCK_COUNT);
    }
    else
    {
        mapsCount = uMapsPerBaseNSSMs;
    } /* Endif */

    /* 
    ** Handle if there is already a NSSM array allocated. We either need to do nothing if 
    ** the array is already the size being requested, or dispose of the old array so we can 
    ** create a new one. 
    */
    if (m_mapsArray)
    {
        /* No need to reallocate maps if they're already the desired size. */
        if (m_mapCount == mapsCount)
        {
            return SUCCESS;
        }
        /* Clean up any previous set of maps. */
        else
        {
            /* Evict and merge maps. */
            flushAll();
            
            /* Dispose of previously allocated maps. */
            delete [] m_mapsArray;
            m_mapsArray = NULL;
            m_mapCount = 0;
        } /* Endif */
    } /* Endif */

    /* 
    ** Now set up the pointers to the NSSM descriptor array and each of the NSSM maps.
    ** The descriptors array goes at the beginning of the buffer, followed by the maps.
    ** Here we also fill in the global NSSM information. 
    */
    m_mapsArray = new NonsequentialSectorsMap[mapsCount];
    m_mapCount = mapsCount;

    /* 
    ** Initialize each of the descriptor structs, and set the pointers to the two 
    ** maps to point into the appropriate place in the same buffer. 
    */
    unsigned iMap;

    for (iMap=0; iMap < mapsCount; iMap++)
    {
        m_mapsArray[iMap].init(this, iMap);
        m_mapsArray[iMap].insertToLRU();
    } /* Endfor */

    return SUCCESS;
} /* Endbody */

NssmManager::~NssmManager()
{ /* Body */
    if (m_mapsArray)
    {
        delete [] m_mapsArray;
        m_mapsArray = NULL;
    } /* Endif */

    m_mapCount = 0;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getBaseNssmCount
* Returned Value   : A count Non Sequential Sectors Maps.
* Comments         :
*   This function provides a count of map entries that is normalized
*   to a NAND with NSSM_BASE_PAGE_PER_BLOCK_COUNT pages per block.
*   This normalization is the same as the units used for the argument to
*   ParitionNSSectorsMaps().
*
*END*--------------------------------------------------------------------*/
unsigned NssmManager::getBaseNssmCount()
{ /* Body */
    int32_t  i32NumSectorsPerBlock = NandHal::getParameters().wPagesPerBlock;    
    unsigned uMapsPerBaseNSSMs;
    unsigned mapsCount = m_mapCount;

    /* 
    ** Adjust the number of maps to allocate based on how many pages per block the NAND has.
    ** The number of maps is defined in terms of NSSM_BASE_PAGE_PER_BLOCK_COUNT (nominally 128)
    ** pages per block. So if there are fewer
    ** pages per block, then the number of maps is increased. Vice versa for more pages per 
    ** block--the number of maps is decreased. 
    */
    if (i32NumSectorsPerBlock < NSSM_BASE_PAGE_PER_BLOCK_COUNT)
    {
        uMapsPerBaseNSSMs = mapsCount / (NSSM_BASE_PAGE_PER_BLOCK_COUNT / i32NumSectorsPerBlock);
    }
    else if (i32NumSectorsPerBlock > NSSM_BASE_PAGE_PER_BLOCK_COUNT)
    {
        uMapsPerBaseNSSMs = mapsCount * (i32NumSectorsPerBlock / NSSM_BASE_PAGE_PER_BLOCK_COUNT);
    }
    else
    {
        uMapsPerBaseNSSMs = mapsCount;
    } /* Endif */

    return ( uMapsPerBaseNSSMs );
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : flushAll
* Returned Value   : void
* Comments         :
*   Flush Non Sequential Sector Map for all drives.
*
*END*--------------------------------------------------------------------*/
void NssmManager::flushAll()
{ /* Body */
    uint32_t u32NS_SectorsMapIdx;

#ifdef NDD_LBA_DEBUG_ENABLE
    WL_LOG(WL_INFOR, "\r\n FlushNSSectorMap, %d\r\n\r\n", pDriveDescriptor->u32Tag);
#endif

    /* Look for the Non Sequential SectorMap in the active maps array */
    for (u32NS_SectorsMapIdx=0; u32NS_SectorsMapIdx < m_mapCount; u32NS_SectorsMapIdx++)
    {
        NonsequentialSectorsMap * map = getMapForIndex(u32NS_SectorsMapIdx);
        map->flush();
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : invalidateAll
* Returned Value   : void
* Comments         :
*   Invalidate all Sector map entries
*
*END*--------------------------------------------------------------------*/
void NssmManager::invalidateAll()
{ /* Body */
    uint32_t iMap;

    /* Reset LRU list. */
    m_lru.clear();

    for (iMap= 0 ; iMap < m_mapCount; iMap++)
    {
        NonsequentialSectorsMap * map = getMapForIndex(iMap);
        assert(map);
        map->invalidate();
        map->insertToLRU();
    } /* Endfor */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : invalidateDrive
* Returned Value   : void
* Comments         :
*   Invalidate Sector map entries for a single drive.
*
*END*--------------------------------------------------------------------*/
void NssmManager::invalidateDrive(LogicalDrive * pDriveDescriptor)
{
    uint32_t u32NS_SectorsMapIdx;

    /* Look for the Non Sequential SectorMap in the active maps array */
    for (u32NS_SectorsMapIdx = 0; u32NS_SectorsMapIdx < m_mapCount; u32NS_SectorsMapIdx++)
    {
        NonsequentialSectorsMap * map = getMapForIndex(u32NS_SectorsMapIdx);
        assert(map);
        
        Region * region = map->getRegion();
        
        /* 
        ** See if the region containing this map's virtual block belongs to the
        ** drive the caller passed in. 
        */
        if (region && region->m_pLogicalDrive == pDriveDescriptor)
        {
            /* 
            ** Remove from LRU list before invalidating, since the invalidate() method clears
            ** the LRU list links. 
            */
            map->removeFromLRU();
            map->invalidate();
            
            /* Reinsert the entry in the LRU list. */
            map->insertToLRU();
        } /* Endif */
    } /* Endfor */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : buildMap
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function builds the NonSequential Sector Map from the NAND's Redundant Area.
*   This function will read the redundant areas for a LBA to rebuild the
*   NonSequential Sector Map.  The result is placed in one of the SectorMaps
*   in RAM. It will evict another NS Sectors Map if a blank one is not available,
*   using an LRU algorithm.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NssmManager::buildMap
(
    /* [IN] Virtual block number to rebuild NS Sector Map for. */
    uint32_t u32LBABlkAddr, 
    
    /* [OUT] Pointer to nonsequential sector map for this Block. */
    NonsequentialSectorsMap ** resultMap
)
{ /* Body */
    /* Make sure we actually have some maps available! */
    if (0 == m_mapCount)
    {   
        /* There are no NS_SectorMaps to recycle. */
        return ERROR_DDI_NAND_DATA_DRIVE_CANT_RECYCLE_USECTOR_MAP;
    } /* Endif */

    /* Get the least recently used map. */
    NonsequentialSectorsMap * map = static_cast<NonsequentialSectorsMap *>(m_lru.select());
    if (!map)
    {
        /* Didn't find one we can recycle */
        return ERROR_DDI_NAND_DATA_DRIVE_CANT_RECYCLE_USECTOR_MAP;
    } /* Endif */

    /* If the entry we just evicted has a back-up block, merge them. */
    RtStatus_t retCode = map->flush();
    if (retCode)
    {
        return retCode;
    } /* Endif */

    /* Reinitialize the NSSM with the new virtual block. */
    map->prepareForBlock(u32LBABlkAddr);

    /* Return the chosen map. */
    if (resultMap)
    {
        *resultMap = map;
    } /* Endif */

    /* Get the actual Physical block that we're mapped to. */
    return map->buildFromMetadata();
} /* Endbody */

#if !defined(__ghs__)
//#pragma mark --NonsequentialSectorsMap--
#endif

NonsequentialSectorsMap::NonsequentialSectorsMap()
:   RedBlackTree::Node(),
    WeightedLRUList::Node(),
    m_map(),
    m_backupMap(),
    m_virtualBlock(kInvalidAddress),
    m_backupPhysicalBlock(kInvalidAddress),
    m_currentPageCount(0),
    m_index(0)
{
}

NonsequentialSectorsMap::~NonsequentialSectorsMap()
{
    /* Invalidate to make sure we are removed from the NSSM index. */
    invalidate();
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : init
* Returned Value   : void
* Comments         :
*   This function initializes the map
*
*END*--------------------------------------------------------------------*/
void NonsequentialSectorsMap::init
(
    /* [IN] Nonsequential sectors map manager */
    NssmManager * manager, 
    
    /* [IN] Index of this map in the array of NSSMs */
    uint16_t mapIndex
)
{ /* Body */
    m_manager = manager;
    m_index = mapIndex;

    unsigned pagesPerBlock = NandHal::getParameters().wPagesPerBlock;
    m_map.init(pagesPerBlock);
    m_backupMap.init(pagesPerBlock);

    invalidate();
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : prepareForBlock
* Returned Value   : void
* Comments         :
*   This function prepares a physical block for the virtual block
*
*END*--------------------------------------------------------------------*/
void NonsequentialSectorsMap::prepareForBlock(uint32_t blockNumber)
{ /* Body */
    invalidate();

    /* Set the main virtual block number. */
    m_virtualBlock = blockNumber;

    /* Insert ourself into the NSSM index now that we have a valid block number. */
    m_manager->m_index.insert(this);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : flush
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function performs a block merging if necessary
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::flush()
{ /* Body */
    /* If a NSSM has a back-up block, it and primary block have to be merged together.         */
    if (m_virtualBlock != kInvalidAddress && m_backupPhysicalBlock != kInvalidAddress)
    {
#ifdef NDD_LBA_DEBUG_ENABLE
        WL_LOG(WL_INFOR, "\r\n MERGING BLOCKS\r\n\r\n");
#endif
        
        return mergeBlocks();
    } /* Endif */

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : invalidate
* Returned Value   : void
* Comments         :
*   This function clears all fields.
*   Be careful to not invalidate a map that needs to be flushed.
*
*END*--------------------------------------------------------------------*/
void NonsequentialSectorsMap::invalidate()
{ /* Body */
    /* 
    ** Remove ourself from the NSSM index before our virtual block number becomes invalid.
    ** But only remove if we were in the index to begin with. 
    */
    if (m_virtualBlock != kInvalidAddress)
    {
        m_manager->m_index.remove(this);
    } /* Endif */

    /* Set initial values for map entries */
    m_virtualBlock = kInvalidAddress;
    m_backupPhysicalBlock = kInvalidAddress;
    m_currentPageCount  = 0;
    removeFromLRU();

    /* Reset the page map. */
    m_map.clear();
    m_backupMap.clear();
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getRegion
* Returned Value   : A Region object
* Comments         :
*   This function returns the region associated with this map's virtual block.
*
*END*--------------------------------------------------------------------*/
Region * NonsequentialSectorsMap::getRegion()
{ /* Body */
    /* Skip already invalid entries. */
    if (m_virtualBlock == kInvalidAddress)
    {
        return NULL;
    } /* Endif */

    /* This function converts the block from absolute to relative. */
    return getMedia()->getRegionForBlock(m_virtualBlock);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getPhysicalBlock
* Returned Value   : A block object
* Comments         :
*   This function returns the absolute physical block number for this map's virtual block.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::getPhysicalBlock(uint32_t * physicalBlock)
{
    assert(physicalBlock);
    
    /* Get the actual Physical block that we're mapped to. */
    return getMapper()->getBlockInfo(m_virtualBlock, physicalBlock);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : buildFromMetadata
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function builds Nonsequential Sectors Map from metadata of a block
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::buildFromMetadata()
{ /* Body */
    RtStatus_t retCode;
    uint32_t physicalBlock;

    retCode = getPhysicalBlock(&physicalBlock);
    if (retCode != SUCCESS)
    {
        return retCode;
    }

    /* Don't need to fill in data from a virtual block with no assigned physical block. */
    if (getMapper()->isBlockUnallocated(physicalBlock))
    {
        assert(kInvalidAddress == m_backupPhysicalBlock);
        assert(0 == m_currentPageCount);
        
        return SUCCESS;
    } /* Endif */

    uint32_t sectorCount;
    retCode = buildMapFromMetadata(m_map, physicalBlock, &sectorCount);
    m_currentPageCount = sectorCount;

    return retCode;
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : buildMapFromMetadata
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS if any part of the NSSM was built successfully
*   - Other errors if none of the NSSM was built successfully, then an
*   error code is propagated upward from lower layers.
* Comments         :
*   This function will read the redundant areas for a LBA to rebuild the
*   NonSequential Sector Map.  The result is placed in one of the SectorMaps
*   in RAM.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::buildMapFromMetadata
(
    /* [IN] The page order map to be filled in from the page metadata. */
    nand::PageOrderMap & map,
    
    /* [IN] Absolute physical number of this block. */
    uint32_t physicalBlock,

    /* [OUT] The number of pages holding data in the block. */
    uint32_t * filledSectorCount
)
{
    uint32_t u32SectorsPerBlock = NandHal::getParameters().wPagesPerBlock;
    uint32_t u32NS_SectorIdx;
    RtStatus_t retCode;
    RtStatus_t retCodeLocal;
    uint32_t u32LogicalSectorIdx;
    uint32_t u32LogicalBlockAddr;
    uint32_t u321stLBA = 0;
    uint8_t u8LastPageHandled = kNssmLastPageNotHandled;
    uint32_t u32NS_SectorLoopCnt;
    BlockAddress theBlock(physicalBlock);

    /* Create the page object and get a buffer to hold the metadata. */
    Page thePage;
    retCode = thePage.allocateBuffers(false, true);
    if (retCode != SUCCESS)
    {
        return retCode;
    } /* Endif */

    /* Go ahead and get our metadata instance since the buffer addresses won't change. */
    Metadata & md = thePage.getMetadata();

    /* 
    ** Read the RA of last page to check whether kIsInLogicalOrderFlag is set
    ** if it is set, it means the pages of this block are written in logical order,
    ** we don't need to read all pages' metadata to build NSSM 
    */
    {
        u32NS_SectorIdx= u32SectorsPerBlock -1;
        thePage.set(PageAddress(theBlock, u32NS_SectorIdx));
        
        int iReads = 0;

        do
        {
            /* read Redundant Area of Sector */
            retCodeLocal = thePage.readMetadata();

            /* Convert ECC_FIXED or ECC_FIXED_REWRITE_SECTOR to SUCCESS... */
            if (is_read_status_success_or_ecc_fixed(retCodeLocal))
            {
                retCodeLocal = SUCCESS;
            }
            
            /* ...and note other errors. */
            if (retCodeLocal != SUCCESS)
            {
                WL_LOG(WL_MODULE_MAPPER, WL_LOG_ERROR, "lba_Build_NS_SectorMap read %d failed on page 0x%X, status 0x%x\r\n", 
                iReads, thePage.get(), retCodeLocal);
            }
        } while ( retCodeLocal != SUCCESS  &&  ++iReads < MAX_BUILD_NSSM_READ_TRIES ); /* Endwhile */


        if (SUCCESS == retCodeLocal)
        {
            /* Get Logical Block Address and Relative Sector Index from RA */
            u32LogicalBlockAddr = md.getLba();
            u32LogicalSectorIdx = md.getLsi();
            
            /* if Erased  */
            if (md.isErased())
            {
                u8LastPageHandled = kNssmLastPageErased;
            }
            else if ( u32LogicalSectorIdx >= u32SectorsPerBlock)
            {
                /* if LSI is invalid */
                WL_LOG(WL_MODULE_MAPPER, WL_LOG_ERROR, "lba_Build_NS_SectorMap: LSI out of range (%d >= %d)\r\n", u32LogicalSectorIdx, u32SectorsPerBlock);

                return ERROR_DDI_NAND_DATA_DRIVE_UBLOCK_HSECTORIDX_OUT_OF_RANGE;
            }
            else if (md.isFlagSet(Metadata::kIsInLogicalOrderFlag) && u32LogicalSectorIdx == u32NS_SectorIdx)
            {
                /* The pages of this block are in logical order */
                map.setSortedOrder();
                
                if (filledSectorCount)
                {
                    *filledSectorCount = u32SectorsPerBlock;
                }

                m_manager->getStatistics().orderedBuildCount++;
                return SUCCESS;
            }
            else
            {                
                map.setEntry(u32LogicalSectorIdx, u32NS_SectorIdx);
                
                if (filledSectorCount)
                {
                    *filledSectorCount = u32SectorsPerBlock;
                } /* Endif */
                
                u8LastPageHandled = kNssmLastPageOccupied;
            } /* Endif */
        } /* Endif */
    }

    if(u8LastPageHandled > 0)
    {
        /* RA of last page is read already, we don't need read it in the below loop */
        u32NS_SectorLoopCnt = u32SectorsPerBlock - 1;
    }
    else
    {
        u32NS_SectorLoopCnt = u32SectorsPerBlock;
    } /* Endif */

    u32NS_SectorLoopCnt = u32SectorsPerBlock;
    for (u32NS_SectorIdx=0; u32NS_SectorIdx<u32NS_SectorLoopCnt; u32NS_SectorIdx++)
    {
        thePage.set(PageAddress(theBlock, u32NS_SectorIdx));

        /* 
        ** Reading this information is very important.  If there is
        ** some kind of failure, we will re-try. 
        */
        int iReads = 0;
        do
        {
            /* read Redundant Area of Sector */
            retCodeLocal = thePage.readMetadata();

#if DEBUG && NSSM_INDUCE_ONE_PAGE_FAILURE
            /* A flag to cause one sector to be omitted from the NSSM. */
            if ( stc_bNSSMInduceOnePageFailure )
            {
                retCodeLocal = ERROR_GENERIC;
            } /* Endif */
#endif
            
#if LOG_NSSM_METADATA_ECC_LEVELS
            if (retCodeLocal)
            {
                log_ecc_failures(pRegion, physicalBlock, u32NS_SectorIdx, &eccInfo);
            } /* Endif */
#endif /* LOG_NSSM_METADATA_ECC_LEVELS */
            
            if (retCodeLocal == ERROR_DDI_NAND_HAL_ECC_FIXED_REWRITE_SECTOR)
            {
                WL_LOG(WL_MODULE_MAPPER,WL_LOG_INFO, ">>> Got ECC_FIXED_REWRITE_SECTOR reading metadata of block %d page %d\n", physicalBlock, u32NS_SectorIdx);
            } /* Endif */

            /* Convert ECC_FIXED or ECC_FIXED_REWRITE_SECTOR to SUCCESS... */
            if (is_read_status_success_or_ecc_fixed(retCodeLocal))
            {
                retCodeLocal = SUCCESS;
            } /* Endif */
            
            /* ...and note other errors. */
            if (retCodeLocal != SUCCESS)
            {
                /* Print an advisory message that there was an error on one page. */
                WL_LOG(WL_MODULE_MAPPER, WL_LOG_ERROR, "lba_Build_NS_SectorMap read %d failed on page 0x%X, status 0x%x\r\n", 
                iReads, thePage.get(), retCodeLocal);
            } /* Endif */
        } while ( retCodeLocal != SUCCESS  &&  ++iReads < MAX_BUILD_NSSM_READ_TRIES ); /* Endwhile */

#if DEBUG && NSSM_INDUCE_ONE_PAGE_FAILURE
        /* A flag to cause one sector to be omitted from the NSSM. */
        stc_bNSSMInduceOnePageFailure = false;
#endif

        /* Okay, did the reads work? */
        if (SUCCESS != retCodeLocal)
        {
            /* 
            ** No, the reads did not work.
            ** We still want to use any remaining sectors, so we will continue on. 
            */
            continue;
        } /* Endif */

        /* 
        ** If we got here, then we were successful reading the sector.
        ** We set retCode accordingly, to indicate that SOMETHING worked. 
        */
        retCode = SUCCESS;

        /* 
        ** if Erased, break (Physical sectors are written "in order", 
        ** so there's no more data beyond this.)... 
        */
        if (md.isErased())
        {
            break;
        } /* Endif */
        
        /* Get Logical Block Address and Relative Sector Index from RA */
        u32LogicalBlockAddr = md.getLba();
        u32LogicalSectorIdx = md.getLsi();

        /* Do a sanity check */
        if (u32NS_SectorIdx == 0)
        {
            u321stLBA = u32LogicalBlockAddr;
        }
        else if (u321stLBA != u32LogicalBlockAddr)
        {
            WL_LOG(WL_MODULE_MAPPER, WL_LOG_ERROR, "lba_Build_NS_SectorMap: LBA mismatch (%d != %d)\r\n", u321stLBA, u32LogicalBlockAddr);

            return ERROR_DDI_NAND_DATA_DRIVE_UBLOCK_LBAS_INCONSISTENT;
        } /* Endif */

        /* Another sanity check */
        if (u32LogicalSectorIdx >= u32SectorsPerBlock)
        {
            WL_LOG(WL_MODULE_MAPPER, WL_LOG_ERROR, "lba_Build_NS_SectorMap: LSI out of range (%d >= %d)\r\n", u32LogicalSectorIdx, u32SectorsPerBlock);

            return ERROR_DDI_NAND_DATA_DRIVE_UBLOCK_HSECTORIDX_OUT_OF_RANGE;
        } /* Endif */

        /*    stuff the map bytes */        
        map.setEntry(u32LogicalSectorIdx, u32NS_SectorIdx);
    } /* for (...u32SectorsPerBlock */

    if (filledSectorCount && (u8LastPageHandled == kNssmLastPageNotHandled || u8LastPageHandled == kNssmLastPageErased))
    {
        /* The last page is not used, get the last used page here */
        *filledSectorCount = u32NS_SectorIdx;
    } /* Endif */

    /* 
    ** Increment the count of instances in which the NSSM was built 
    ** (used for performance measurements). 
    */
    m_manager->getStatistics().buildCount++;

    /* 
    ** The return-code is as follows:
    **  - If any of the reads worked, then retCode was set to SUCCESS, 
    **  and that is what gets returned.
    **  - If none of the reads worked, then retCode is not SUCCESS, 
    **  and retCodeLocal contains the code from the last failure. 
    */
    return (SUCCESS == retCode ? retCode : retCodeLocal) ;
} /* Body */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : addEntry
* Returned Value   : void
* Comments         :
*   This function adds a sector to the NonSequential Sector Map.  If the
*   NS Sectors Map for a given LBA Block is in RAM, it gets updated, but it
*   doesn't try to refresh the map if it is not in RAM.  We don't worry about
*   it because it will be properly constructed, based on the Redundant Areas
*   in the LBA Block when it is needed.
*
*END*--------------------------------------------------------------------*/
void NonsequentialSectorsMap::addEntry
(
    /* [IN] Logical sector index */
    uint32_t u32LBASectorIdx, 
    
    /* [IN] Physical sector index */
    uint32_t u32ActualSectorIdx
)
{ /* Body */
    /* Verify the logical sector index. */
    assert(u32LBASectorIdx < NandHal::getParameters().wPagesPerBlock);

    /* 
    ** The LBA was found above so now add it to the NonSequential Sector Map.
    ** u32LBASectorIdx  is the index into the array, The u32ActualSectorIdx 
    ** is the value stored in the array.
    */
    m_map.setEntry(u32LBASectorIdx, u32ActualSectorIdx);

    /* Verify that we're writing into the correct location. */
    assert(m_currentPageCount == (int32_t)u32ActualSectorIdx);

    /* Increment the SectorCount. */
    m_currentPageCount++;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : isInLogicalOrder
* Returned Value   : TRUE or FALSE
*   - TRUE if the map is in logical order
*   - FALSE if the map is not in logical order
* Comments         :
*   This function checks whether the first N-1 pages are written in logical order
*
*END*--------------------------------------------------------------------*/
bool NonsequentialSectorsMap::isInLogicalOrder()
{ /* Body */
    return m_map.isInSortedOrder(m_map.getEntryCount() - 1);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : sortAndCopyLbaBlock
* Returned Value   : Status of call or error
*   SUCCESS If no error has occurred & u32NewBlkAddr filled
* Comments         :
*   This function will sort and copy an old LBA block to a new LBA block, 
* get a new LBA block, then copy the written sectors from the old block to
* the new block, then mark the old LBA block as bad.
* If successful, the data from the old block has been copied to the
* new block.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::sortAndCopyLbaBlock
(
    /* [IN] Physical Address of the old block to be copied */
    uint32_t u32OldBlkAddr,
    
    /* [IN] Physical Address of new block to copy to */
    uint32_t u32NewBlkAddr,
    
    /* [IN] Starting sector in the block to leave out */
    uint32_t u32SkipSectorStart,
    
    /* [IN] Number of sector in the block to leave out */
    uint32_t u32NumSectorsToSkip
)
{
    uint32_t    sectorLogicalOffset;
    RtStatus_t retCode;
    uint32_t   newSectorPhysicalOffset;
    uint32_t   u32SourceSectorPhysicalOffset;
    Region * pSourceRegion;
    Region * pTargetRegion;
    uint32_t u32OccupiedBit;
    uint32_t u32NumSectorsPerBlock = NandHal::getParameters().wPagesPerBlock;

    /* 
    ** Get region that the old block resides in.  We use this to determine
    ** whether we do a copyback or the read/write (slow) copy.
    */
    pSourceRegion = getMedia()->getRegionForBlock(u32OldBlkAddr);
    if (!pSourceRegion)
    {
        return ERROR_DDI_NAND_DATA_DRIVE_INVALID_LOGICAL_SECTOR;
    }
    u32OldBlkAddr = pSourceRegion->m_nand->blockToRelative(u32OldBlkAddr);

    /* Get region that the target block resides in. */
    pTargetRegion = getMedia()->getRegionForBlock(u32NewBlkAddr);
    if (!pTargetRegion)
    {
        return ERROR_DDI_NAND_DATA_DRIVE_INVALID_LOGICAL_SECTOR;
    }
    u32NewBlkAddr = pTargetRegion->m_nand->blockToRelative(u32NewBlkAddr);
    
    nand::PageOrderMap tempMap;
    retCode = tempMap.init(u32NumSectorsPerBlock);
    if (retCode != SUCCESS)
    {
        return retCode;
    }

    newSectorPhysicalOffset = 0;
    for (sectorLogicalOffset=0; sectorLogicalOffset<u32NumSectorsPerBlock ; sectorLogicalOffset++)
    {
       /* Check to see if this sector is in the range we want to skip. */
       if ((sectorLogicalOffset >= u32SkipSectorStart) && (sectorLogicalOffset < (u32SkipSectorStart+u32NumSectorsToSkip)))
       {
           continue;
       }

       u32OccupiedBit = m_map.isOccupied(sectorLogicalOffset);

       /* If the location is occupied by a valid sector, it needs to be copied. */
       if (u32OccupiedBit)
       {
            u32SourceSectorPhysicalOffset = m_map[sectorLogicalOffset];
            
            retCode = copyPages(pSourceRegion->m_nand, 
                                pTargetRegion->m_nand,
                                u32OldBlkAddr, 
                                u32SourceSectorPhysicalOffset,
                                u32NewBlkAddr, 
                                newSectorPhysicalOffset, 
                                1);
            
            /* Handle an error copying the sector. */
            if (retCode != SUCCESS)
            {
                return retCode;
            }

            /* Build up the new sector map. */
            tempMap.setEntry(sectorLogicalOffset, newSectorPhysicalOffset);
            
            newSectorPhysicalOffset++;
        }
    }

    /* Copy the new sector map over the old one. */
    m_map = tempMap;
    
    /* Update the sector count. */
    m_currentPageCount = newSectorPhysicalOffset;

    return retCode;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : shortCircuitMerge
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function simply frees back-up block. This function is called
*   when "new" block completely overwrites the back-up block and
*   therefore merge is not necessary.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::shortCircuitMerge()
{
    uint32_t u32SaveBackUpPhysBlkNum = m_backupPhysicalBlock;

    m_backupPhysicalBlock = kInvalidAddress;
    m_backupMap.clear();

    getMedia()->getMapper()->getPhymap()->markBlockFreeAndErase(u32SaveBackUpPhysBlkNum);

    return SUCCESS;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : quickMerge
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function merges "old" block into "new" block in place.  This function
*   is only called when there is enough free space in "new" block to accommodate
*   all sectors in "old" block which are not overshadowed by sectors in "new"
*   block.
*   As a result, when this function is finished, the "new" block should be
*   completely full.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::quickMerge()
{
    uint32_t u32OldBlockNum;
    uint32_t u32SaveOldBlockNum;
    uint32_t u32NewBlockNum;
    uint32_t u32NumSectorsPerBlock;
    uint32_t  i;
    Region * pOldBlockRegion;
    Region * pNewBlockRegion;
    RtStatus_t retCode;

    u32NumSectorsPerBlock = NandHal::getParameters().wPagesPerBlock;
    u32OldBlockNum = m_backupPhysicalBlock;
        
    /* Convert virtual block to physical. */
    retCode = getPhysicalBlock(&u32NewBlockNum);
    if (retCode)
    {
        return retCode;
    }
    
    /* Save the absolute physical backup block number. */
    u32SaveOldBlockNum = m_backupPhysicalBlock;

    /* Get regions and convert absolute blocks to relative. */
    pOldBlockRegion = getMedia()->getRegionForBlock(u32OldBlockNum);
    if (!pOldBlockRegion)
    {
        return ERROR_DDI_NAND_DATA_DRIVE_INVALID_LOGICAL_SECTOR;
    }
    u32OldBlockNum = pOldBlockRegion->m_nand->blockToRelative(u32OldBlockNum);

    pNewBlockRegion = getMedia()->getRegionForBlock(u32NewBlockNum);
    if (!pNewBlockRegion)
    {
        return ERROR_DDI_NAND_DATA_DRIVE_INVALID_LOGICAL_SECTOR;
    }
    u32NewBlockNum = pNewBlockRegion->m_nand->blockToRelative(u32NewBlockNum);

    /* 
    ** For each sector, first look up the sector in new Non-sequential sector map.
    ** If entry in new non-sequential sector map is invalid, look up in old
    ** non-sequential sector map.
    */
    for(i=0; i < u32NumSectorsPerBlock; i++)
    {
        bool u32OldOccupied = m_backupMap.isOccupied(i);
        bool u32NewOccupied = m_map.isOccupied(i);
        
        if ((!u32NewOccupied) && (u32OldOccupied))
        {
            uint32_t u32SourceSectorIdx = m_backupMap[i];

            assert(u32SourceSectorIdx < u32NumSectorsPerBlock);

            retCode = copyPages(pOldBlockRegion->m_nand,
                                pNewBlockRegion->m_nand,
                                u32OldBlockNum,
                                u32SourceSectorIdx,
                                u32NewBlockNum,
                                m_currentPageCount,
                                1);

            if (retCode)
            {
                return retCode;
            }
    
            m_map.setEntry(i, m_currentPageCount);
            
            m_currentPageCount++;
        }
    }
    
    assert(m_currentPageCount <= (int32_t)u32NumSectorsPerBlock);

    /* Remove our reference to the backup block. */
    m_backupPhysicalBlock = kInvalidAddress;
    m_backupMap.clear();

    /* Erase the backup block and mark it free in the phymap. */
    getMedia()->getMapper()->getPhymap()->markBlockFreeAndErase(u32SaveOldBlockNum);

    return SUCCESS;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : mergeBlocksSkippingPage
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function combines paired blocks into a single new block.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::mergeBlocksSkippingPage(uint32_t u32NewSectorNumber)
{
    uint32_t u32NewBlockNum;
    RtStatus_t retCode;
    uint32_t u32NumSectorsPerBlock;
    uint32_t u32NumDistinct;
    
    u32NumSectorsPerBlock = NandHal::getParameters().wPagesPerBlock;

    /* Get the number of distinct sectors in the new block. */
    u32NumDistinct = m_map.countDistinctEntries();
            
    /* 
    ** If the number of unique sectors in "current" block is equal to total number of
    ** sectors in block, the current block completely overwrites the back-up block.  
    ** In that case, no merge is necessary. 
    */
    if (u32NumDistinct==u32NumSectorsPerBlock)
    {
        retCode = shortCircuitMerge();
    }
    else if (m_currentPageCount == (int32_t)u32NumDistinct)
    {
        /* 
        ** QuickMerge is only called when MergeBlocks is called
        ** due to NSSM being flushed.  If MergeBlocks is called
        ** from PreventThrashing, "new" block is completely full,
        ** in which case we would do the full scale merge (merge
        ** "old" and "new" into "third" block) or do a Short-Circuit
        ** merge. 
        */
        retCode = quickMerge();
    }
    else
    {
        retCode = getPhysicalBlock(&u32NewBlockNum);
        if (retCode) 
        {
            return retCode;

        }

        retCode = mergeBlocksCore(u32NewSectorNumber, u32NewBlockNum);
    }
    
    return retCode;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : mergeBlocksCore
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function combines the primary and backup blocks into a newly allocated block.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::mergeBlocksCore
(
    uint32_t u32NewSectorNumber, 
    uint32_t u32NewBlockNum
)
{
    bool isOldOccupied;
    bool isNewOccupied;    
    uint32_t u32OldBlockNum;
    uint32_t u32ThirdBlockNum;
    uint32_t u32SaveOldBlockNum;
    uint32_t u32SaveNewBlockNum;
    uint32_t u32SaveThirdBlockNum;
    uint32_t u32SourceSectorIdx;
    uint32_t u32NumSectorsPerBlock;
    uint32_t u32ThirdBlkSectorIdx;
    uint32_t u32RetryCount = 0;
    uint32_t i;
    Region * pOldBlockRegion;
    Region * pNewBlockRegion;
    Region * pThirdBlockRegion;
    RtStatus_t retCode;
    uint32_t u32LbaBlockNumber;

    u32NumSectorsPerBlock = NandHal::getParameters().wPagesPerBlock;
    
    nand::PageOrderMap thirdMap;
    retCode = thirdMap.init(u32NumSectorsPerBlock);
    if (retCode != SUCCESS)
    {
        return retCode;
    }

    u32OldBlockNum    = m_backupPhysicalBlock;
    u32LbaBlockNumber = m_virtualBlock;

    /* Get a third block and assign it to LBA. */
    retCode = getMedia()->getMapper()->getBlockAndAssign(u32LbaBlockNumber, &u32ThirdBlockNum, kMapperBlockTypeNormal, 1);
    if (retCode != SUCCESS)
    {
        return retCode;
    }

    /* 
    ** Save Block number to another variable
    ** because ddi_nand_media_regions_GetRegionFromPhyBlk
    ** modifies its first parameter. 
    */
    u32SaveOldBlockNum = u32OldBlockNum;
    pOldBlockRegion = getMedia()->getRegionForBlock(u32OldBlockNum);
    if (!pOldBlockRegion)
    {
        return ERROR_DDI_NAND_DATA_DRIVE_INVALID_LOGICAL_SECTOR;
    }
    u32OldBlockNum = pOldBlockRegion->m_nand->blockToRelative(u32OldBlockNum);

    u32SaveNewBlockNum = u32NewBlockNum;
    pNewBlockRegion = getMedia()->getRegionForBlock(u32NewBlockNum);
    if (!pNewBlockRegion)
    {
        return ERROR_DDI_NAND_DATA_DRIVE_INVALID_LOGICAL_SECTOR;
    }
    u32NewBlockNum = pNewBlockRegion->m_nand->blockToRelative(u32NewBlockNum);

    u32SaveThirdBlockNum = u32ThirdBlockNum;
    pThirdBlockRegion = getMedia()->getRegionForBlock(u32ThirdBlockNum);
    if (!pThirdBlockRegion)
    {
        return ERROR_DDI_NAND_DATA_DRIVE_INVALID_LOGICAL_SECTOR;
    }
    u32ThirdBlockNum = pThirdBlockRegion->m_nand->blockToRelative(u32ThirdBlockNum);

    u32ThirdBlkSectorIdx = 0;

    /* 
    ** For each sector, first look up the sector in new Non-sequential sector map.
    ** If entry in new non-sequential sector map is invalid, look up in old
    ** non-sequential sector map. 
    */
    for(i=0; i < u32NumSectorsPerBlock; i++)
    {
        /* If we are not replacing this sector */
        if (i!=u32NewSectorNumber)
        {
            isOldOccupied = m_backupMap.isOccupied(i);
            isNewOccupied = m_map.isOccupied(i);
            
            if (isNewOccupied || isOldOccupied)
            {
                if (isNewOccupied)
                {
                    u32SourceSectorIdx = m_map[i];

                    assert(u32SourceSectorIdx<u32NumSectorsPerBlock);

                    retCode = copyPages(pNewBlockRegion->m_nand,
                                        pThirdBlockRegion->m_nand,
                                        u32NewBlockNum,
                                        u32SourceSectorIdx,
                                        u32ThirdBlockNum,
                                        u32ThirdBlkSectorIdx,
                                        1);
                } /* if */
                else
                {
retry_on_old:
                    assert(isOldOccupied);

                    u32SourceSectorIdx = m_backupMap[i];

                    assert(u32SourceSectorIdx<u32NumSectorsPerBlock);

                    retCode = copyPages(pOldBlockRegion->m_nand,
                                        pThirdBlockRegion->m_nand,
                                        u32OldBlockNum,
                                        u32SourceSectorIdx,
                                        u32ThirdBlockNum,
                                        u32ThirdBlkSectorIdx,
                                        1);
                } /* else */

                thirdMap.setEntry(i, u32ThirdBlkSectorIdx);
                u32ThirdBlkSectorIdx++;

                /* Deal with different error codes from the page copy. */
                if (retCode == ERROR_DDI_NAND_HAL_WRITE_FAILED)
                {
                    /* 
                    ** Writing to the third block failed, so mark the block as bad, pick a
                    ** new target block, and restart the merge sequence. We'll repeat this up
                    ** to 10 times. 
                    */
                    u32RetryCount++;
                    if (u32RetryCount>10)
                    {
                        return retCode;
                    }

                    getMedia()->getMapper()->getPhymap()->markBlockUsed(u32SaveThirdBlockNum);
                    Block(BlockAddress(pThirdBlockRegion->m_iChip, u32ThirdBlockNum)).markBad();

                    retCode = getMedia()->getMapper()->getBlockAndAssign(u32LbaBlockNumber, &u32ThirdBlockNum, kMapperBlockTypeNormal, 1);
                    if (retCode != SUCCESS)
                    {
                        return retCode;
                    }

                    u32SaveThirdBlockNum = u32ThirdBlockNum;
                    pThirdBlockRegion = getMedia()->getRegionForBlock(u32ThirdBlockNum);
                    if (!pThirdBlockRegion)
                    {
                        return ERROR_DDI_NAND_DATA_DRIVE_INVALID_LOGICAL_SECTOR;
                    }
                    u32ThirdBlockNum = pThirdBlockRegion->m_nand->blockToRelative(u32ThirdBlockNum);

                    /* Reset the target block map. */
                    u32ThirdBlkSectorIdx = 0;
                    thirdMap.clear();
                    
                    /* Need to set loop counter to -1 because continue executes the increment. */
                    i = -1;
                    continue;
                }
                else if (retCode == ERROR_DDI_NAND_HAL_ECC_FIX_FAILED)
                {
                    if (isNewOccupied && isOldOccupied)
                    {
                        isNewOccupied=0;
                        goto retry_on_old;
                    }
                    /* 
                    ** Handle an ECC failed error specially.
                    ** (This is a placeholder to add code in the future to report ECC failures.)
                    */
                    return retCode;
                }
                else if (retCode)
                {
                    /* Got some other error while copying pages, so just return it. */
                    return retCode;
                }
            } /* if sector is in old or new NS map */
        } /* if sector is not new sector which will be overwritten */
    } /* for */
    
    // Copy the target map into our primary map.
    m_map = thirdMap;
    m_backupMap.clear();
    
    m_backupPhysicalBlock = kInvalidAddress;
    m_currentPageCount = u32ThirdBlkSectorIdx;

    getMedia()->getMapper()->getPhymap()->markBlockFreeAndErase(u32SaveNewBlockNum);
    getMedia()->getMapper()->getPhymap()->markBlockFreeAndErase(u32SaveOldBlockNum);

    return SUCCESS;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getNewBlock
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function allocates a new block to assist in thrashing prevention.
*
*   Previous NAND architecture had a worst case write performance that was O(MN),
*   where M is the number of blocks per sector and N is the number of sectors
*   written.  Although, M can be though of as a constant in theory, in practice,
*   it cannot be so easily dismissed.  The old architecture had to resort to
*   nand pinning to get around worst case performance issue.  Nand pinning was
*   just not compatible with LENA, and we thought of a smarter way of handling
*   worst case write performance issue.
*
*   When a block is written to after it has been completely filled, we simply
*   allocate a new block and make that the new primary block.  All updates go
*   to the new block.  When new block is filled, we have choices.  If new block
*   completely overwrites old block, we simply release the old block.  If not,
*   we have to merge new block and old block.  In either case, worst case
*   write performance is still O(N).
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::getNewBlock
(
    /* [IN] Logical Block number of LBA to be killed */
    uint32_t u32LbaBlockNumber, 
    
    /* [IN/OUT] Physical Address */
    uint32_t * pu32PhysicalBlkAddr, 
    
    /* [IN] Number of new sector which needs to be written */
    uint32_t u32NumSectorsPerBlock
)
{
    uint32_t u32OldPhysicalBlkAddr;
    RtStatus_t retCode;

    assert(m_virtualBlock == u32LbaBlockNumber);
    assert(kInvalidAddress == m_backupPhysicalBlock);

    /* 
    ** Get the physical block currently associated with the LBA. 
    ** This block will become the backup block. 
    */
    retCode = getPhysicalBlock(&u32OldPhysicalBlkAddr);
    if (retCode)
    {
        return retCode;
    }

    assert(u32OldPhysicalBlkAddr == *pu32PhysicalBlkAddr);

    /* 
    ** This call is guaranteed to work because we
    ** maintain a set of reserve blocks just for
    ** this purpose and for replacement purposes. 
    */
    retCode = getMedia()->getMapper()->getBlock(pu32PhysicalBlkAddr, kMapperBlockTypeNormal, 0);
    if (retCode)
    {
        return retCode;
    }

    /* Assign the newly allocated block to our LBA. */
    retCode = getMedia()->getMapper()->setBlockInfo(u32LbaBlockNumber, *pu32PhysicalBlkAddr);
    if (retCode)
    {
        return retCode;
    }

    /* First copy current sector map to "old" sector map, same with occupied map. */
    m_backupMap = m_map;
    
    /* Now, clear current sector map */
    m_map.clear();
    
    assert(u32OldPhysicalBlkAddr != *pu32PhysicalBlkAddr);
    assert(!getMedia()->getMapper()->isBlockUnallocated(*pu32PhysicalBlkAddr));

    m_backupPhysicalBlock = u32OldPhysicalBlkAddr;
    m_currentPageCount = 0;

    return SUCCESS;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : preventThrashing
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function prevents the thrashing that can occur when block is full
*   and each subsequent write triggers an erase and M writes where M is the
*   number of sectors per block.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::preventThrashing
(
    uint32_t * pu32PhysicalBlkAddr, 
    uint32_t u32NewSectorNumber
)
{
    RtStatus_t retCode;
    int32_t    i32NumDistinctEntries;
    uint32_t u32NumSectorsPerBlock;
    uint32_t u32SaveBackUpPhysBlkNum;

    u32NumSectorsPerBlock = NandHal::getParameters().wPagesPerBlock;

    /* This function should only be called when the following condition is true. */
    assert(u32NumSectorsPerBlock == (uint32_t)m_currentPageCount);

    if (m_backupPhysicalBlock != kInvalidAddress)
    {
        /* 
        ** What we have here is the case where old block and
        ** new block are both completely full. First figure
        ** out if the new block has N distinct sector entries
        ** where N is the number of pages per block.  If there
        ** are no duplicates, the old block can simply be ignored.
        ** If there are duplicates, the old block has to be used
        ** in reconstructing a complete block. 
        */

        i32NumDistinctEntries = m_map.countDistinctEntries();

        if (i32NumDistinctEntries == (int32_t)u32NumSectorsPerBlock)
        {
            /* 
            ** New Block completely overwrites old block.  So,
            ** simply erase old block, make the new block the old
            ** block and get a new new block. 
            */
            u32SaveBackUpPhysBlkNum = m_backupPhysicalBlock;
            m_backupPhysicalBlock = kInvalidAddress;

            retCode = getNewBlock(m_virtualBlock, pu32PhysicalBlkAddr, u32NumSectorsPerBlock);
            if (retCode)
            {
                return retCode;
            }

            getMedia()->getMapper()->getPhymap()->markBlockFreeAndErase(u32SaveBackUpPhysBlkNum);
        }
        else
        {
            /* 
            ** In this case, we have two blocks which are completely full and
            ** the two have to be merged together.	In addition, a third newer
            ** block is needed to house the combination of old block and
            ** new block. 
            */

            retCode = mergeBlocksSkippingPage(u32NewSectorNumber);
            if (retCode)
            {
                return retCode;
            }

            /* 
            ** This call is necessary to fill in *pu32PhysicalBlkAddr so that
            ** caller knows which physical block to write to. 
            */
            retCode = getPhysicalBlock(pu32PhysicalBlkAddr);
            if (retCode)
            {
                return retCode;
            }
        }
    }
    else
    {
        retCode = getNewBlock(m_virtualBlock, pu32PhysicalBlkAddr, u32NumSectorsPerBlock);
        if (retCode)
        {
            return retCode;
        }
    }

    return SUCCESS;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : recoverBadLbaBlock
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function will recover from a bad write to an LBA. First it must
* get a new LBA block, then copy the written sectors from the old block to
* the new block, then mark the old LBA block as bad.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::recoverBadLbaBlock
(
    uint32_t u32OldBlkAddr,
    uint32_t *pu32NewBlkAddr,
    uint32_t u32SectorToSkip
)
{
    RtStatus_t retCode;

    /* Allocate a new block and copy the original block to it. */
    retCode = copyToNewBlock(u32OldBlkAddr, pu32NewBlkAddr, u32SectorToSkip, 1);
    if (retCode != SUCCESS)
    {
        return retCode;
    }

    /* Mark the original block as bad in the phymap and erase it. */
    retCode = getMedia()->getMapper()->getPhymap()->markBlockUsed(u32OldBlkAddr);
    if (retCode != SUCCESS)
    {
        return retCode;
    }

    /* Now write the bad block markers. */
    nand::Block badBlock(u32OldBlkAddr);
    retCode = badBlock.markBad();
    if (retCode != SUCCESS)
    {
        return retCode;
    }

    return retCode;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : copyToNewBlock
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function Allocates a new block and copies another block into it.
*
*   The LBA logicalBlockNumber is updated to point to the newly allocated
* block. If there is a write error while copying, the process is started over
* by selecting another new block to copy to.
*
*   The LBA must not have a backup block for this call to work. If this
* is a possibility then use the ddi_nand_ndd_lba_RelocateLBA() function instead.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::copyToNewBlock
(
    uint32_t physicalBlockNumber,
    uint32_t * newPhysicalBlockNumber,
    uint32_t skipSectorStart,
    uint32_t skipSectorCount)
{
    RtStatus_t status;
    uint32_t newPhysicalBlock;

    do {
        /* Get the new physical block where we'll be copying to. */
        status = getMedia()->getMapper()->getBlockAndAssign(m_virtualBlock, &newPhysicalBlock, kMapperBlockTypeNormal, 1);
        if (status != SUCCESS)
        {
            return status;
        }
        
        /* Return the new physical block to the caller. */
        if (newPhysicalBlockNumber)
        {
            *newPhysicalBlockNumber = newPhysicalBlock;
        }
        
        /* Sort and copy the pages. The NSSM is updated by this function. */
        status = sortAndCopyLbaBlock(physicalBlockNumber, newPhysicalBlock, skipSectorStart, skipSectorCount);
        
        /* Handle a failed copy. */
        if (status == ERROR_DDI_NAND_HAL_WRITE_FAILED)
        {
            /* Mark the block we tried to copy to as used in the phymap. */
            status = getMedia()->getMapper()->getPhymap()->markBlockUsed(newPhysicalBlock);
            if (status != SUCCESS)
            {
                return status;
            }

            /* And mark the block itself bad. We ignore the marking status. */
            nand::Block badBlock(newPhysicalBlock);
            badBlock.markBad();
        }
    } while (status != SUCCESS);
    
    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : relocateVirtualBlock
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function copies data to a new location.
*
*   All we have to do is merge since that copies the block contents to a new block
*   by its very nature.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::relocateVirtualBlock
(
    uint32_t physicalBlockNumber, 
    uint32_t * newPhysicalBlockNumber
)
{
    RtStatus_t status;

    /* 
    ** If the block has a backup block, then all we have to do is merge since that copies
    ** the block contents to a new block by its very nature. Otherwise, if the block doesn't
    ** have a backup, we just need to copy it into a new block. 
    */
    if (m_backupPhysicalBlock != kInvalidAddress)
    {
        status = mergeBlocks();
        
        /* Return the new physical block to the caller if requested. */
        if (status == SUCCESS && newPhysicalBlockNumber)
        {
            uint32_t newPhysicalBlock;
            
            status = getPhysicalBlock(&newPhysicalBlock);
            if (status == SUCCESS)
            {
                *newPhysicalBlockNumber = newPhysicalBlock;
            }
        }
    }
    else
    {
        /* Allocate a new block and copy the original block to it. */
        status = copyToNewBlock(physicalBlockNumber, newPhysicalBlockNumber, NS_SECTORS_MAP_MAX_NUM_SECTORS, 0);
        if (status != SUCCESS)
        {
            return status;
        }

        /* Mark the original physical block for reuse and erase it. */
        getMedia()->getMapper()->getPhymap()->markBlockFreeAndErase(physicalBlockNumber);
    }
    
    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : resolveConflict
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function resolves conflicting zone-map assignment arising from power-loss.
*   We resolve this difficulty by counting the number of unique logical sector entries
*   found in each block.  The one with more logical sector entries is likely the "old"
*   block.  An alternative approach is to simply discard the "new" block and keep the
*   "old" block.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::resolveConflict
(
    uint32_t blockNumber, 
    uint32_t physicalBlock1, 
    uint32_t physicalBlock2
)
{
    uint32_t newPhysicalBlock;
    uint32_t u32NumUsedSectors1;
    uint32_t u32NumUsedSectors2;
    uint32_t u32PagesPerBlock;
    RtStatus_t ret;
    RtStatus_t retEccFixFail1 = SUCCESS;
    RtStatus_t retEccFixFail2 = SUCCESS;
    
    /* Get some media info. */
    u32PagesPerBlock = NandHal::getParameters().wPagesPerBlock;
    
    m_virtualBlock = blockNumber;
    
    /* Create two map instances to hold the page indexes read from metadata. */
    nand::PageOrderMap map1;
    ret = map1.init(u32PagesPerBlock);
    if (ret != SUCCESS)
    {
        return ret;
    }
    
    nand::PageOrderMap map2;
    ret = map2.init(u32PagesPerBlock);
    if (ret != SUCCESS)
    {
        return ret;
    }
    
    /* Build temporary nonsequential sectors maps for the old and new blocks. */
    ret = buildMapFromMetadata(map1, physicalBlock1, &u32NumUsedSectors1);
    if (ret == ERROR_DDI_NAND_HAL_ECC_FIX_FAILED)
    {
        retEccFixFail1 = ret;
    }
    else if (ret != SUCCESS)
    {
        return ret;
    }
    
    ret = buildMapFromMetadata(map2, physicalBlock2, &u32NumUsedSectors2);
    if (ret == ERROR_DDI_NAND_HAL_ECC_FIX_FAILED)
    {
        retEccFixFail2 = ret;
    }
    else if (ret != SUCCESS)
    {
        return ret;
    }

    /* Handle cases where one or both of the blocks has uncorrectable ECC errors. */
    if ((retEccFixFail1 != SUCCESS) && (retEccFixFail2 != SUCCESS))
    {
        /* Mark them as unused, which will also erase them. */
        getMedia()->getMapper()->getPhymap()->markBlockFreeAndErase(physicalBlock2);
        
        /* Update zonemap to mark this LBA as unallocated */
        getMedia()->getMapper()->markBlock(blockNumber, physicalBlock1, kNandMapperBlockFree);
        
        return ret;
    }
    else if (retEccFixFail1 != SUCCESS)
    {
        /* Mark the erroneous block as unused, which will also erase it. */
        getMedia()->getMapper()->getPhymap()->markBlockFreeAndErase(physicalBlock1);
        
        /* Update zone map to map LBA to the right physical block */
        getMedia()->getMapper()->setBlockInfo(blockNumber, physicalBlock2);
        
        return SUCCESS;
    }
    else if (retEccFixFail2 != SUCCESS)
    {
        /* Mark the erroneous block as unused, which will also erase it. */
        getMedia()->getMapper()->getPhymap()->markBlockFreeAndErase(physicalBlock2);

        /* Update zone map to map LBA to the right physical block */
        getMedia()->getMapper()->setBlockInfo(blockNumber, physicalBlock1);
        
        return SUCCESS;
    }

    /* Select the block which has more used sectors as the old block. */
    if (u32NumUsedSectors1 > u32NumUsedSectors2)
    {
        /* Block 1 is the old block. */
        newPhysicalBlock = physicalBlock2;
        
        m_backupPhysicalBlock = physicalBlock1;
        m_currentPageCount = u32NumUsedSectors2;
        m_backupMap = map1;
        m_map = map2;
    }
    else
    {
        /* Block 2 is the old block. */
        newPhysicalBlock = physicalBlock1;
        
        m_backupPhysicalBlock = physicalBlock2;
        m_currentPageCount = u32NumUsedSectors1;
        m_backupMap = map2;
        m_map = map1;
    }
    
    /* Now merge the two blocks. This will also clean up everything. */
    return mergeBlocksCore(NS_SECTORS_MAP_MAX_NUM_SECTORS, newPhysicalBlock);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : copyPages
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function This function will copies a group of pages from one block to a different
* block on the NAND.  If available, internal copyback can be used but this
* depends upon the functionality available in the HAL for this NAND.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::copyPages
(
    NandPhysicalMedia * sourceNand,
    NandPhysicalMedia * targetNand,
    uint32_t u32SrcBlkAddr,
    uint32_t u32SrcSectorIdx,
    uint32_t u32DestBlkAddr,
    uint32_t u32DestSectorIdx,
    uint32_t u32NumSectorsToCopy
) 
{
    RtStatus_t ret;
    uint32_t u32RelSrcSector;
    uint32_t u32RelDestSector;
    uint32_t successfulPages = 0;
    
    /* Get a sector buffer. */
    SectorBuffer sectorBuffer;
    if ((ret = sectorBuffer.acquire()) != SUCCESS)
    {
        return ret;
    }

    AuxiliaryBuffer auxBuffer;
    if ((ret = auxBuffer.acquire()) != SUCCESS)
    {
        return ret;
    }

    u32RelSrcSector = sourceNand->blockAndOffsetToRelativePage(u32SrcBlkAddr, u32SrcSectorIdx);
    u32RelDestSector = targetNand->blockAndOffsetToRelativePage(u32DestBlkAddr, u32DestSectorIdx);
    
    /* Create our filter. */
    CopyPagesFlagFilter filter;
    
    /* We assume that the source address is in this particular region. */
    ret = sourceNand->copyPages(
        targetNand,
        u32RelSrcSector,
        u32RelDestSector,
        u32NumSectorsToCopy,
        sectorBuffer,
        auxBuffer,
        &filter,
        &successfulPages);
   
    /* Handle benign ECC stati. */
    if ( is_read_status_success_or_ecc_fixed( ret ) )
    {
        ret = SUCCESS;
    }

    return ret;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : filter
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   The "in logical order" metadata flag is checked, and if it is set it will be cleared.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::CopyPagesFlagFilter::filter
(
    /* [IN] NAND object for the source page. */
    NandPhysicalMedia * fromNand,

    /* [IN] NAND object for the destination page. May be the same as #fromNand. */
    NandPhysicalMedia * toNand,

    /* [IN] Relative address of the source page. */
    uint32_t fromPage,

    /* [IN] Relative address of the destination page. */
    uint32_t toPage,

    /* [IN] Buffer containing the page data. */
    SECTOR_BUFFER * sectorBuffer,

    /* [IN] Buffer holding the page's metadata. */
    SECTOR_BUFFER * auxBuffer,

    /*
    ** [OUT] The filter method should set this parameter to true if it has
    ** modified the page in any way. This will let the HAL know that it cannot use copyback
    ** commands to write the destination page.
    */
    bool * didModifyPage)
{ /* Body */
    /* Create a metadata object so we can work with flags. */
    Metadata md(auxBuffer);

    /* Check if the is-in-order flag is set on this page. */
    if (md.isFlagSet(Metadata::kIsInLogicalOrderFlag))
    {
        /* Clear the flag. */
        md.clearFlag(Metadata::kIsInLogicalOrderFlag);
        
        /* Inform the HAL that we changed the page contents. */
        assert(didModifyPage);
        *didModifyPage = true;
    } /* Endif */

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getKey
* Returned Value   : A virtual block
* Comments         :
*   This function returns the virtual block
*
*END*--------------------------------------------------------------------*/
RedBlackTree::Key_t NonsequentialSectorsMap::getKey() const
{ /* Body */
    return m_virtualBlock;
} /* Endbody */

#if !defined(__ghs__)
//#pragma mark --RelocateVirtualBlockTask--
#endif

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : N/A
* Returned Value   : N/A
* Comments         :
*   Constructor of RelocateVirtualBlockTask
*
*END*--------------------------------------------------------------------*/
RelocateVirtualBlockTask::RelocateVirtualBlockTask(NssmManager * manager, uint32_t virtualBlockToRelocate)
:   DeferredTask(kTaskPriority),
    m_manager(manager),
    m_virtualBlock(virtualBlockToRelocate)
{ /* Body */
    WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Add a RelocateVirtualBlockTask for block id #%d ", virtualBlockToRelocate);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getTaskTypeID
* Returned Value   : Task type id
* Comments         :
*   This function returns task type id
*
*END*--------------------------------------------------------------------*/
uint32_t RelocateVirtualBlockTask::getTaskTypeID() const
{ /* Body */
    return kTaskTypeID;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : examineOne
* Returned Value   : TRUE or FALSE
* Comments         :
*   
*
*END*--------------------------------------------------------------------*/
bool RelocateVirtualBlockTask::examineOne(DeferredTask * task)
{ /* Body */
    if (task->getTaskTypeID() == kTaskTypeID)
    {
        RelocateVirtualBlockTask * relocateTask = static_cast<RelocateVirtualBlockTask *>(task);
        if (relocateTask->getVirtualBlock() == m_virtualBlock)
        {
            /* 
            ** This task exactly matches me, so return true to indicate that I don't want to
            ** be placed into the deferred queue. There's no reason to relocate the block more
            ** than once. 
            */
            return true;
        } /* Endif */
    } /* Endif */

    return false;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : task
* Returned Value   : void
* Comments         :
*   
*
*END*--------------------------------------------------------------------*/
void RelocateVirtualBlockTask::task()
{ /* Body */
    WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Run a relocate virtual block task \n");

    /* Get the NSSM instance for this virtual block. */
    NonsequentialSectorsMap * map = g_nandMedia->getNssmManager()->getMapForVirtualBlock(m_virtualBlock);
    if (!map)
    {
        WL_LOG(WL_MODULE_MAPPER, WL_LOG_ERROR,  "Failed to get NSSM for virtual block %d\n", m_virtualBlock);
        return;
    } /* Endif */

    RtStatus_t status;

    WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "NAND ECC reached threshold, rewriting virtual block=%d, physical block=%d\n", m_virtualBlock, map->getPhysicalBlock().get());

    /* Now relocate the block contents. */
    uint32_t newPhysicalBlockNumber;
    status = map->relocateVirtualBlock(NonsequentialSectorsMap::kInvalidAddress, &newPhysicalBlockNumber);
    if (status != SUCCESS)
    {
        WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "..failed to move virtual block %d to new physical block (0x%08x)\n", m_virtualBlock, status);
    } /* Endif */

    WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "..moved virtual block %d to physical block %d\n", m_virtualBlock, newPhysicalBlockNumber);
}

/* EOF */
