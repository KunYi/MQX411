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
*   This file contains definition of the nand::PersistentMap class.
*
*
*END************************************************************************/

#include "wl_common.h"
#include "ddi/mapper/persistent_map.h"
#include "ddi/mapper/zone_map_section_page.h"
#include "ddi/mapper/mapper.h"

using namespace nand;

/* 
** Macros 
*/

/* Enable extra logging of map reads and writes. */
#define LOG_MAP 0

/* Macro used to help print the map signature as a four-char constant. */
#define SIG_AS_4CHARS() ((char)((m_signature >> 24) & 0xff)), ((char)((m_signature >> 16) & 0xff)), ((char)((m_signature >> 8) & 0xff)), ((char)((m_signature >> 0) & 0xff))

#if defined(__ghs__)
#pragma ghs section text=".init.text"
#endif 

#if defined (__CODEWARRIOR__)
#endif 

PersistentMap::PersistentMap(Mapper & mapper, uint32_t mapType, uint32_t metadataSignature)
:   m_mapper(mapper),
    m_block(),
    m_entrySize(0),
    m_maxEntriesPerPage(0),
    m_signature(mapType),
    m_metadataSignature(metadataSignature),
    m_topPageIndex(0),
    m_totalSectionCount(0),
    m_sectionPageOffsets()
{
}

#if defined(__ghs__)
#pragma ghs section text=default
#endif 

#if defined (__CODEWARRIOR__)
#endif 

PersistentMap::~PersistentMap()
{
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : init
* Returned Value   : void
* Comments         :
*   This function initializes PersistentMap object
*
*END*--------------------------------------------------------------------*/
void PersistentMap::init
(
    /* [IN] Size of entry */
    int entrySize, 
    
    /* [IN] Number of entries */
    int entryCount
)
{ /* Body */
    m_entrySize = entrySize;
    m_totalEntryCount = entryCount;
    
    /* The size of a single section is the NAND page size minus the map header. */
    uint32_t u32SizeOfData = NandHal::getParameters().pageDataSize - sizeof(NandMapSectionHeader_t);

    /* Compute the number of map entries per NAND page. */
    m_maxEntriesPerPage = u32SizeOfData / m_entrySize;

    /* Set total number of sections of zone map and the number of cache entries. */
    m_totalSectionCount = ROUND_UP_DIV(m_totalEntryCount, m_maxEntriesPerPage);
    
    /* Allocate the offset array. */
    m_sectionPageOffsets.init(m_totalSectionCount);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : retrieveSection
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function retrieves a section of the map which contains the entry 
*   number u32EntryNum.
*
*END*--------------------------------------------------------------------*/
RtStatus_t PersistentMap::retrieveSection
(
    /* [IN] Entry number which should be contained in section of zone-map returned. */
    uint32_t u32EntryNum,
    
    /* 
    ** [OUT] Section of Zone-map which contains the entry number contained in u32EntryNum. 
    ** This buffer will be filled with the entire contents of the map section page, 
    ** including the header. 
    */
    uint8_t *pMap,
    
    /* 
    ** [IN] Pass true to have the function automatically consolidate the zone map, 
    ** i.e. rewrite it to a new block, if the ECC_FIXED_REWRITE_SECTOR error is returned 
    ** while reading a section page. 
    */
    bool shouldConsolidateOnRewriteSectorError
)
{ /* Body */
    RtStatus_t retCode;
    uint32_t sectionNumber;
    uint8_t pageOffset;
    NandMapSectionHeader_t * header = (NandMapSectionHeader_t *)pMap;

    /* Get a buffer to hold the redundant area. */
    AuxiliaryBuffer auxBuffer;
    if ((retCode = auxBuffer.acquire()) != SUCCESS)
    {
        return retCode;
    } /* Endif */
    
    /* Disable auto-sleep while loading the section. */
    NandHal::SleepHelper disableSleep(false);
    
    /* Compute the index into the section page offset array. */
    sectionNumber = u32EntryNum / m_maxEntriesPerPage;
    
    /* Create a page object to read the section. */
    ZoneMapSectionPage sectionPage;
    sectionPage.setMapType(m_signature);
    sectionPage.setBuffers((SECTOR_BUFFER *)pMap, auxBuffer);
    
    /* See if we have a valid page offset that we can use to make this more efficient. */
    if (m_sectionPageOffsets.isOccupied(sectionNumber))
    {
        pageOffset = m_sectionPageOffsets[sectionNumber];
        
        sectionPage = PageAddress(m_block, pageOffset);
        retCode = sectionPage.read();
        
        /* If we got back valid data, first check to make sure this section is really what we expect. */
        if (is_read_status_success_or_ecc_fixed(retCode))
        {
            /* Validate the header and version. */
            if (!sectionPage.validateHeader())
            {
                return ERROR_DDI_NAND_MAPPER_LBA_CORRUPTED;
            } /* Endif */
        } /* Endif */
        
        if (is_read_status_success_or_ecc_fixed_without_decay(retCode)
                || (retCode == ERROR_DDI_NAND_HAL_ECC_FIXED_REWRITE_SECTOR && !shouldConsolidateOnRewriteSectorError))
        {
            retCode = SUCCESS;
            
            /* This better be the section we need. */
            if (!((header->startLba <= u32EntryNum) && (header->startLba + header->entryCount > u32EntryNum)))
            {
                /* The page offset array is corrupted. */
                retCode = ERROR_DDI_NAND_MAPPER_LBA_CORRUPTED;
            } /* Endif */
        } /* Endif */
    }
    else
    {
        /* We should always have a valid section offset map. */
        assert(false); 
    } /* Endif */

    /* 
    ** Handle ECC hitting the threshold when the caller wants us to automatically consolidate.
    ** The number of bit errors has hit the threshold for this ECC level, so we need to
    ** rewrite this page by consolidating the zone map into a new block. 
    */
    if (retCode == ERROR_DDI_NAND_HAL_ECC_FIXED_REWRITE_SECTOR && shouldConsolidateOnRewriteSectorError)
    {
        WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, ">>> Got ECC_FIXED_REWRITE_SECTOR error reading page %d of map block %d\n", pageOffset, m_block.get());
        WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, ">>> Rewriting by consolidating map '%c%c%c%c'\n", SIG_AS_4CHARS());
        /* Was this the map section the caller was looking for? */
        bool isTargetSection = (header->startLba <= u32EntryNum) && (header->startLba + header->entryCount > u32EntryNum);
        
        /* 
        ** Consolidate, using the data we just read if possible. This is to prevent having to 
        ** read the page any more times, each read degrading the data a little more. 
        */
        retCode = consolidate(isTargetSection, sectionNumber, pMap + sizeof(*header), header->entryCount);
        if (retCode != SUCCESS)
        {
            return retCode;
        } /* Endif */
        
        /* 
        ** After consolidating, the zone map is in a new block and the number of filled pages in
        ** the block is different. Check if the page we just read contained the map section
        ** the caller needs. If so, we can just return. Otherwise we have to start the section
        ** scan over from the beginning. 
        */
        if (isTargetSection)
        {
            return SUCCESS;
        }
        else
        {
            /* 
            ** Recursively scan the new zone map block for the desired section. Note that
            ** we don't enable auto-consolidate. This is to prevent any possibility of infinite
            ** recursion. If the contents of the new zone map block are already corrupted
            ** beyond the ability of ECC to repair, then there are some serious problems at hand.
            **
            ** The only real downside to this recursive call is that it will allocate a second
            ** auxiliary buffer. 
            */
            return retrieveSection(u32EntryNum, pMap, false);
        } /* Endif */
    } /* Endif */

    return retCode;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : buildSectionOffsetTable
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function builds section offset table
*
*END*--------------------------------------------------------------------*/
RtStatus_t PersistentMap::buildSectionOffsetTable()
{ /* Body */
    RtStatus_t status;
    bool needsRewrite = false;
    
    m_buildReadCount = 0;

    /* Wipe map to be entirely unoccupied. */
    m_sectionPageOffsets.clear();
    
    /* Create an object for reading the map page and set the buffers we're using. */
    ZoneMapSectionPage mapPage(m_block.getPage());
    mapPage.setEntrySize(m_entrySize);
    mapPage.setMapType(m_signature);
    
    status = mapPage.allocateBuffers();
    if (status != SUCCESS)
    {
        return status;
    } /* Endif */

    /* Do a binary search to find the first empty page. */
    status = findTopPageIndex(mapPage, needsRewrite);
    if (status != SUCCESS)
    {
        return status;
    } /* Endif */

    /* Read metadata of top page to get trustNumber */
    ZoneMapSectionPage lastUsedPage(m_block.getNand()->blockAndOffsetToPage(m_block, m_topPageIndex - 1));
    status = lastUsedPage.allocateBuffers();
    if (status != SUCCESS)
    {
        return status;
    } /* Endif */
    lastUsedPage.readMetadata();
    m_trustNumber = lastUsedPage.getMetadata().getTrustNumber();
    
#if LOG_MAP
    WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Took %u reads to find top page (%u) for map '%c%c%c%c'\n", m_buildReadCount, m_topPageIndex, SIG_AS_4CHARS());
#endif
    
    /* Scan the block to examine pages we missed above. */
    status = fillUnknownSectionOffsets(mapPage, needsRewrite);
    if (status != SUCCESS)
    {
        return status;
    } /* Endif */
    
    /* Rewrite the map if we hit the ECC threshold while reading one of the sections. */
    if (needsRewrite)
    {
        WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Rewriting map '%c%c%c%c' after building section offset table\n", SIG_AS_4CHARS());
        return consolidate();
    } /* Endif */
    
#if LOG_MAP
    WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Took %u reads to build map '%c%c%c%c'\n", m_buildReadCount, SIG_AS_4CHARS());
#endif

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : findTopPageIndex
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function finds top page index
*
*END*--------------------------------------------------------------------*/
RtStatus_t PersistentMap::findTopPageIndex
(
    /* [IN] Zonemap section page */
    ZoneMapSectionPage & mapPage, 
    
    /* [IN] Flag rewrite */
    bool & needsRewrite)
{ /* Body */
    RtStatus_t status;
    unsigned i = 0;
    int lowestErased = NandHal::getParameters().wPagesPerBlock;
    int highestFilled = -1;
    
    /* Binary search loop. */
    while (highestFilled < lowestErased)
    {
        mapPage = PageAddress(m_block, i);
        
        ++m_buildReadCount;

        mapPage.readMetadata();
        /* Set the counters based on whether this was an erased or filled page. */
        if (mapPage.getMetadata().isErased())
        {
            lowestErased = i;
        }
        else
        {
            /* Read the page into our buffers. */
            status = mapPage.read();
            if (status == ERROR_DDI_NAND_HAL_ECC_FIXED_REWRITE_SECTOR)
            {
                /* 
                ** We'll have to rewrite the map by consolidating once we've figured
                ** out how many pages are written to it. Unfortunately, we can't do it
                ** before then. 
                */
                needsRewrite = true;
                WL_LOG(WL_MODULE_MAPPER, WL_LOG_ERROR, "Got ECC_FIXED_REWRITE_SECTOR while reading page %d; will rewrite later\n", mapPage.get());
            }
            else if (!is_read_status_success_or_ecc_fixed(status))
            {
                return status;
            } /* Endif */
            
            highestFilled = i;
            
            /* For filled pages, also make sure this is a valid map section. */
            if (!mapPage.validateHeader())
            {
                return ERROR_DDI_NAND_MAPPER_LBA_CORRUPTED;
            } /* Endif */
        }
        
        /* Exit the loop when the known filled and erased pages meet. */
        if (highestFilled == lowestErased - 1)
        {
            break;
        } /* Endif */
        
        /* 
        ** We're going to test again, so move the test page to halfway in between the
        ** known filled and erased pages. 
        */
        i = highestFilled + (lowestErased - highestFilled) / 2;
    } /* Endwhile */
    
    /* Save the address of the first empty page, which is also the count of filled pages. */
    m_topPageIndex = lowestErased;
    
#if LOG_MAP
    WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Map '%c%c%c%c' block %d has %d filled pages\n", SIG_AS_4CHARS(), m_block.get(), i);
#endif
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : fillUnknownSectionOffsets
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function fills all unknow sections offsets
*
*END*--------------------------------------------------------------------*/
RtStatus_t PersistentMap::fillUnknownSectionOffsets
(   
    /* [IN] Zonemap section page */
    ZoneMapSectionPage & mapPage, 
    
    /* [IN] Flag rewrite */
    bool & needsRewrite
)
{ /* Body */
    RtStatus_t status;

    /* Get the starting number of filled in section offset. */
    unsigned distinctEntries = m_sectionPageOffsets.countDistinctEntries();
    
    /* Scan backwards in the block while examining pages. */
    int i = m_topPageIndex - 1;
    mapPage = PageAddress(m_block, i);
    for (; i >= 0; --i, --mapPage)
    {
        /* If all sections are filled then in we're done! */
        if (distinctEntries == (unsigned)m_totalSectionCount)
        {
            return SUCCESS;
        } /* Endif */
        
        ++m_buildReadCount;

        /* Read the page into our buffers. */
        status = mapPage.read();
        if (status == ERROR_DDI_NAND_HAL_ECC_FIXED_REWRITE_SECTOR)
        {
            /* 
            ** We'll have to rewrite the map by consolidating once we've figured
            ** out how many pages are written to it. Unfortunately, we can't do it
            ** before then. 
            */
            needsRewrite = true;
            WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Got ECC_FIXED_REWRITE_SECTOR while reading page %d; will rewrite later\n", mapPage.get());
        }
        else if (!is_read_status_success_or_ecc_fixed(status))
        {
            return status;
        } /* Endif */
        
        /* Make sure this is a valid zone map section. */
        if (!mapPage.validateHeader())
        {
            return ERROR_DDI_NAND_MAPPER_LBA_CORRUPTED;
        } /* Endif */
        
        /* Update the page offset array if this is a section we haven't seen yet. */
        unsigned sectionNumber = mapPage.getSectionNumber();
        if (!m_sectionPageOffsets.isOccupied(sectionNumber))
        {
            m_sectionPageOffsets.setEntry(sectionNumber, i);
            ++distinctEntries;
        } /* Endif */
    } /* Endfor */
    
    /* If all sections are filled then in we're OK. */
    if (distinctEntries == (unsigned)m_totalSectionCount)
    {
        return SUCCESS;
    }
    else
    {
        /* We exited the loop without finding a copy of every section. */
        return ERROR_DDI_NAND_MAPPER_LBA_CORRUPTED;
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : consolidate
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function is called when zone-map sections which have been written
*   out of order in current block have completely filled the block.  This
*   function allocates a new block, erases it, and writes a consolidated
*   and accurate zone-map to the new block.
*
*END*--------------------------------------------------------------------*/
RtStatus_t PersistentMap::consolidate
(
    /* [IN] Whether sectionData is valid. */
    bool hasValidSectionData,
    
    /*
    ** [IN] The number of the zone map section for which
    ** sectionData contains data. Ignored if hasValidSectionData is false.
    */
    uint32_t sectionNumber,
    
    /*
    ** [IN] Zone map data for the map section identified by
    ** sectionNumber. Ignored if hasValidSectionData is false. This data
    ** must not have the map section header in it. In other words, the data
    ** should be only an array of entries. The metadata that is contained in the
    ** header on the media is passed in the sectionNumber and sectionDataEntryCount
    ** variables.
    */
    uint8_t * sectionData,
    
    /* [IN] Number of entries in sectionData. */
    uint32_t sectionDataEntryCount
)
{ /* Body */
    uint32_t u32EntryNum;
    RtStatus_t ret;
    uint32_t u32NewSectorNum;
    uint32_t u32NumEntriesWritten;
    uint32_t newMapBlockNumber;

    /* Disable auto sleep while consolidating. */
    NandHal::SleepHelper disableSleep(false);
    
    /* Get the block to consolidate into. This block is guaranteed to be good and erased. */
    ret = m_mapper.getBlock(&newMapBlockNumber, kMapperBlockTypeMap, 1);
    if (SUCCESS != ret)
    {
        return ret;
    } /* Endif */

    /* Create the target block object and verify that it has been erased for us. */
    Block targetBlock(newMapBlockNumber);
    assert(targetBlock.isErased());
    
    WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Consolidating map '%c%c%c%c' from block %u to block %u\n", SIG_AS_4CHARS(), m_block.get(), newMapBlockNumber);

    /* Get a temp buffer. */
    SectorBuffer sectorBuffer;
    if ((ret = sectorBuffer.acquire()) != SUCCESS)
    {
        return ret;
    } /* Endif */

    u32NewSectorNum = 0;
    u32EntryNum     = 0;

    /* Create the section page object. */
    ZoneMapSectionPage sectionPage(targetBlock.getPage());
    sectionPage.setEntrySize(m_entrySize);
    sectionPage.setMetadataSignature(m_metadataSignature);
    sectionPage.setMapType(m_signature);
    sectionPage.allocateBuffers();

    /* 
    ** For each section, search back to front to retrieve lastest copy 
    ** and then write the retrieved section into the new block 
    */
    while (u32EntryNum < (uint32_t)m_totalEntryCount)
    {
        uint8_t * bufferToWrite;
        uint32_t bufferEntryCount;
        uint32_t thisSectionNumber = u32EntryNum / m_maxEntriesPerPage;
        
        /* 
        ** First check if we can use the data passed into this function. 
        ** It has preference over any other source (cache or media). 
        */
        if (hasValidSectionData && sectionNumber == thisSectionNumber)
        {
            bufferToWrite = sectionData;
            bufferEntryCount = sectionDataEntryCount;
        }
        else
        {
            ret = getSectionForConsolidate(u32EntryNum, thisSectionNumber, bufferToWrite, bufferEntryCount, sectorBuffer);
            if (ret == ERROR_DDI_NAND_HAL_ECC_FIX_FAILED && !m_mapper.isBuildingMaps())
            {
                /* Got an uncorrectable ECC error, so we have to completely rebuild the maps. */
                WL_LOG(WL_MODULE_MAPPER, WL_LOG_ERROR, ">>> Got uncorrectable ECC error reading map section; rebuilding maps\n");
                
                /* Free buffers before calling into RecreateZoneMap(). */
                sectorBuffer.release();
                sectionPage.releaseBuffers();
                
                /* Mark unused and erase the block we were consolidating into. */
                m_mapper.getPhymap()->markBlockFreeAndErase(newMapBlockNumber);
                
                /* Rebuild maps. */
                return m_mapper.rebuild();
            }
            else if (SUCCESS != ret)
            {
                WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, ">>> getSectionForConsolidate(%d, %d, %d) failed during consolidate(): 0x%08x\n", m_block.get(), m_topPageIndex, u32EntryNum, ret);

                return ret;
            } /* Endif */
            
        } /* Endif */
        
        /* Write the section. */
        ret = sectionPage.writeSection(u32EntryNum, bufferEntryCount, bufferToWrite, &u32NumEntriesWritten);

        /* Handle a write failure by marking the block bad and restarting to consolidate. */
        if (ret == ERROR_DDI_NAND_HAL_WRITE_FAILED)
        {
            WL_LOG(WL_MODULE_MAPPER, WL_LOG_ERROR, ">>> Got write error for page %d while consolidating: 0x%08x; marking bad\n", sectionPage.get(), ret);
            
            /* Process the new bad block. */
            m_mapper.handleNewBadBlock(targetBlock);
            
            /* 
            ** We need to Consolidate into another block.
            ** Reset variables so that the loop will start over from beginning with a new block. 
            */
            ret = m_mapper.getBlock(&newMapBlockNumber, kMapperBlockTypeMap, 1);
            if (SUCCESS != ret)
            {
                return ret;
            } /* Endif */

            /* Erase the target block before we start writing to it. */
            targetBlock = BlockAddress(newMapBlockNumber);
            assert(targetBlock.isErased());
            
            /* Set the new section page address to the beginning of the new block. */
            sectionPage = targetBlock.getPage();

            /* Start the consolidation loop over again. */
            u32NewSectorNum = 0;
            u32EntryNum     = 0;

            continue;
        }
        else if (ret != SUCCESS)
        {
            /* Unexpected error that we don't know how to handle. */
            return ret;
        } /* Endif */

        ++u32NewSectorNum;
        ++sectionPage;

        /* Advance the section start entry by how many entries were in the section just written. */
        u32EntryNum += bufferEntryCount;
    } /* Endwhile */

    /* Mark previous zone-map block as un-used and erase it. We don't care if this fails. */
    m_mapper.getPhymap()->markBlockFreeAndErase(m_block);

    /* Update the current map block number and current sector offset. */
    m_block = targetBlock;
    m_topPageIndex = u32NewSectorNum;
    
    /* 
    ** After consolidation, the each section is written exactly in order within the zone map block.
    ** We do this afterwards instead of during the consolidation process because it would be messy
    ** to deal with write failures with the page offset array half-new and half-old. 
    */
    m_sectionPageOffsets.setSortedOrder();

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getSectionForConsolidate
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function gets a section for consolidating
*
*END*--------------------------------------------------------------------*/
RtStatus_t PersistentMap::getSectionForConsolidate
(
    /* [IN] Entry number */
    uint32_t u32EntryNum,
    
    /* [IN] Section number */
    uint32_t thisSectionNumber,
    
    /* [IN] Buffer holds data to write */
    uint8_t *& bufferToWrite,
    
    /* [IN] Number of entries */
    uint32_t & bufferEntryCount,
    
    /* [IN] Buffer holds main content */
    uint8_t * sectorBuffer
)
{ /* Body */
    RtStatus_t ret;

    /* 
    ** Read the most recent copy of this section from the media.
    ** Don't auto-consolidate on an ecc rewrite sector error, since we're already copying
    ** to a new block. 
    */
    ret = retrieveSection(u32EntryNum, sectorBuffer, false);
    if (ret != SUCCESS)
    {
        return ret;
    } /* Endif */

    NandMapSectionHeader_t * header = (NandMapSectionHeader_t *)sectorBuffer;

    /* Verify the entry size. */
    if (header->entrySize != (uint32_t)m_entrySize)
    {
        return ERROR_DDI_NAND_MAPPER_LBA_CORRUPTED;
    } /* Endif */

    /* Make sure we got back the section we're expecting. */
    if (thisSectionNumber != (header->startLba / m_maxEntriesPerPage))
    {
        return ERROR_DDI_NAND_MAPPER_LBA_CORRUPTED;
    } /* Endif */
    
    /* Write the contents of the sector buffer starting after the header. */
    bufferToWrite = sectorBuffer + sizeof(*header);
    
    /* The number of entries in this section is held in the second word. */
    bufferEntryCount = header->entryCount;
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : addSection
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function adds a zone-map section to current block. The block
*   should not be completely full when this function is called.  If the
*   block does fill up as a result of this function, this function calls
*   ddi_nand_mapper_ZoneMapConsolidate to consolidate the zone-map.
*
*END*--------------------------------------------------------------------*/
#if __CWCC__
#pragma push
/* Force compiler to not optimize source code on CW */
#pragma optimization_level 0
#endif
RtStatus_t PersistentMap::addSection
(
    /*
    ** [IN] Pointer to contents of the map section 
    ** which contains the entry number contained in u32EntryNum.
    */
    uint8_t *pMap, 
    
    /* [IN] The starting entry number for the given section. */
    uint32_t u32StartingEntryNum, 
    
    /* [IN] Number of entries contained in this section. */
    uint32_t u32NumEntriesToWrite)
{ /* Body */
    uint32_t sectionNumber;
    uint32_t u32MaxNumEntries;
    RtStatus_t ret;
    NandPhysicalMedia * pPhyMediaDescriptor = m_block.getNand();
    
#if LOG_MAP
    WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFOR, "Adding section (entry %u+%u) to map '%c%c%c%c' block %u in page %u\n", u32StartingEntryNum, u32NumEntriesToWrite, SIG_AS_4CHARS(), m_block.get(), m_topPageIndex);
#endif

    /* Disable auto sleep. */
    NandHal::SleepHelper disableSleep(false);
    
    /* Make sure we're not going to write outside the bounds of the zone map block. */
    assert(m_topPageIndex <= (int)pPhyMediaDescriptor->pNANDParams->wPagesPerBlock);
    
    /* Compute the index into the section page offset array. */
    sectionNumber = u32StartingEntryNum / m_maxEntriesPerPage;

    u32MaxNumEntries = m_maxEntriesPerPage;
    u32NumEntriesToWrite = MIN(u32NumEntriesToWrite, u32MaxNumEntries);
    u32NumEntriesToWrite = MIN(u32NumEntriesToWrite, (m_totalEntryCount - u32StartingEntryNum));
    
    /* 
    ** If we would be writing the last page in the zone map block, then go ahead and consoldiate
    ** but pass in the data we were going to write. This prevents a write followed by a
    ** consolidate. 
    */
    if (m_topPageIndex >= (int)pPhyMediaDescriptor->pNANDParams->wPagesPerBlock)
    {
        /* Set the flag indicating that we consolidated. */
        m_didConsolidateDuringAddSection = true;
        
        /* 
        ** Consolidate into another block, making use of the data passed into this
        ** function in the process. This means there is nothing left for us to do. 
        */
        ret = consolidate(true, sectionNumber, (uint8_t *)pMap, u32NumEntriesToWrite);
    }
    else
    {
        uint32_t u32NumEntriesWritten;
        uint32_t u32PhysicalPageNumber = pPhyMediaDescriptor->blockAndOffsetToPage(m_block, m_topPageIndex);

        /* Create the section page object. */
        ZoneMapSectionPage sectionPage(u32PhysicalPageNumber);
        sectionPage.setEntrySize(m_entrySize);
        sectionPage.setMetadataSignature(m_metadataSignature);
        sectionPage.setMapType(m_signature);
        sectionPage.allocateBuffers();

        /* Depend on value of trustFlag, we chosse how to write section */
        /* Write the section. */
        if (m_trustFlag == true) 
        {
            ret = sectionPage.writeSection(u32StartingEntryNum, u32NumEntriesToWrite, (uint8_t *)pMap, &u32NumEntriesWritten, m_trustFlag, m_trustNumber);
        } 
        else 
        {
            ret = sectionPage.writeSection(u32StartingEntryNum, u32NumEntriesToWrite, (uint8_t *)pMap, &u32NumEntriesWritten);
        } /* Endif */
        
        
        if (ret == ERROR_DDI_NAND_HAL_WRITE_FAILED)
        {
            BlockAddress oldZoneMapBlock(m_block);
            
            WL_LOG(WL_MODULE_MAPPER, WL_LOG_ERROR, ">>> Got write error for block %d page %d while adding map '%c%c%c%c' section: 0x%08x; consolidating and marking bad\n", m_block.get(), m_topPageIndex, SIG_AS_4CHARS(), ret);
            
            /* 
            ** Consolidate into another block, making use of the data passed into this
            ** function in the process. This means there is nothing left for us to do. 
            */
            ret = consolidate(true, sectionNumber, pMap, u32NumEntriesToWrite);
            
            /* 
            ** Process and mark the old block bad. We do this after consolidating so we can read
            ** data out of the old block during the consolidation process. 
            */
            m_mapper.handleNewBadBlock(oldZoneMapBlock);
            
            /* Set the flag indicating that we consolidated. */
            m_didConsolidateDuringAddSection = true;
        }
        else if (ret == SUCCESS)
        {
            /* Record the page offset for this map section. */
            m_sectionPageOffsets.setEntry(sectionNumber, m_topPageIndex);
            
            m_topPageIndex++;
        } /* Endif */
    } /* Endif */

    return ret;
} /* Endbody */
#if __CWCC__
#pragma pop
#endif

/* EOF */
