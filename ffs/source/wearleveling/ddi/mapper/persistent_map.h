#if !defined(__persistent_map_h__)
#define __persistent_map_h__
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

#include "ddi_block.h"
#include "page_order_map.h"

namespace nand
{

/* Forward declarations */
class Mapper;
class ZoneMapSectionPage;

/*
** Base class for a map that is stored on the NAND.
**
** This class implements a map composed of integer entries that is broken into one or
** more sections, each the size of a NAND page. The map is stored on the NAND in an
** efficient manner, by writing sections sequentially to pages within a block.
**
** As a new version of a section becomes available, it is written to the next page in the
** block. The sections can be in any order in the block, and there can be multiple copies
** of any given section, but only the most recent copy of a section will be recognized.
** Only when the block is completely full, with no free pages, will the map be copied
** (consolidated) to a new block.
**
** The content for sections of the map is not handled by this class. It is the
** responsibility of subclasses or users of the class to provide that content.
**
** Right now, this class only supports storing the map within a single block. But it is
** possible that in the future this restriction may be relaxed, in order to store maps
** that are larger than will fit within one block.
*/
class PersistentMap
{
public:

    /* Default constructor. */
    PersistentMap(Mapper & mapper, uint32_t mapType, uint32_t metadataSignature);
    
    /* Destructor. */
    virtual ~PersistentMap();
    
    /* Initializer. */
    void init(int entrySize, int entryCount);
    
    /* Does the given block belong to this map? */
    bool isMapBlock(const BlockAddress & address) { return m_block == address; }

    /* Get block holding PhyMap or ZoneMap */
    uint32_t getMapBlock() { 
        return m_block.get(); 
    }

    /* Rebuild the map into a new block. */
    virtual RtStatus_t consolidate(
    bool hasValidSectionData=false,
    uint32_t sectionNumber=0,
    uint8_t * sectionData=NULL,
    uint32_t sectionDataEntryCount=0);
    
    /* Write an updated section of the map. */
    RtStatus_t addSection(
    uint8_t *pMap,
    uint32_t u32StartingEntryNum,
    uint32_t u32NumEntriesToWrite);

    /* Load section of the map. */
    RtStatus_t retrieveSection(
    uint32_t u32EntryNum,
    uint8_t *pMap,
    bool shouldConsolidateOnRewriteSectorError);

    /* TRUST helper */
    /* Number is saved to metadata */
    uint16_t m_trustNumber; 
    /* A flag to ask lower classes to save data with TRUST number */
    bool m_trustFlag; 

protected:
    /* Our parent mapper instance. */
    Mapper & m_mapper;      
    
    /* The block containing this map. */
    BlockAddress m_block;   
    
    /* Size of each map entry in bytes. */
    int m_entrySize;        
    
    /* Number of entries that fit in one NAND page. */
    int m_maxEntriesPerPage;  

    /* The map type signature. */
    uint32_t m_signature;   
    
    /* A signature written into the metadata of each map section page. */
    uint32_t m_metadataSignature;   
    
    /* Number of sections currently in the map's block. */
    int m_topPageIndex;  

    /* Total number of entries in the entire map. */
    int m_totalEntryCount;  
    
    /* Total number of sections in the entire map. */
    int m_totalSectionCount;    
    
    /* Map from zone map section number to page offset within the zone map block. */
    PageOrderMap m_sectionPageOffsets;   
    
    /* Set to true if addSection() does a consolidate. */
    bool m_didConsolidateDuringAddSection;  
    int m_buildReadCount;

    /* Scan the map's block an build the section offset table. */
    RtStatus_t buildSectionOffsetTable();
    
    /* Do a binary search to find the first empty page. */
    RtStatus_t findTopPageIndex(ZoneMapSectionPage & mapPage, bool & needsRewrite);
    
    /* Scan to find the most recent copies of each section. */
    RtStatus_t fillUnknownSectionOffsets(ZoneMapSectionPage & mapPage, bool & needsRewrite);

    /* 
    ** Read a section during consolidation.
    **
    ** The default implementation simply uses retrieveSection() to load the data.
    ** Having this function virtual makes it possible for subclasses to override and provide
    ** additional methods for obtaining the section data, for instance from a cache. 
    */
    virtual RtStatus_t getSectionForConsolidate(
    uint32_t u32EntryNum,
    uint32_t thisSectionNumber,
    uint8_t *& bufferToWrite,
    uint32_t & bufferEntryCount,
    uint8_t * sectorBuffer);
    
};

} /* namespace nand */

#endif /* __persistent_map_h__ */

/* EOF */
