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
*   This file contains implementation of the persistent phy map class.
*
*
*END************************************************************************/

#include "ddi/mapper/persistent_phy_map.h"
#include "ddi/mapper/mapper.h"
#include "ddi/mapper/phy_map.h"
#include "ddi/mapper/zone_map_section_page.h"

using namespace nand;

PersistentPhyMap::PersistentPhyMap(Mapper & mapper)
:   PersistentMap(mapper, kNandPhysMapSignature, PHYS_STRING_PAGE1),
    m_phymap(NULL),
    m_isLoading(false)
{
    m_trustFlag = false;
    m_trustNumber = 0;
}

PersistentPhyMap::~PersistentPhyMap()
{
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : init
* Returned Value   : void
* Comments         :
*   This function initializes PersistentPhyMap object
*
*END*--------------------------------------------------------------------*/
void PersistentPhyMap::init()
{ /* Body */
    uint32_t count = PhyMap::getEntryCountForBlockCount(m_mapper.getMedia()->getTotalBlockCount());
    PersistentMap::init(PhyMap::kEntrySizeInBytes, count);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : load
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function loads phymap from device
*
*END*--------------------------------------------------------------------*/
RtStatus_t PersistentPhyMap::load()
{ /* Body */
    assert(m_phymap);

    /* Automatically clear the is-loading flag when we leave this scope. */
    m_isLoading = true;    
    AutoClearFlag clearLoading(m_isLoading);
    
    /* Search the nand for the location of the phy map. */
    uint32_t mapPhysicalBlock;
    RtStatus_t status = m_mapper.findMapBlock(kMapperPhyMap, &mapPhysicalBlock);
    if (status != SUCCESS)
    {
        return status;
    } /* Endif */
    
    WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Loading phymap from block %u\n", mapPhysicalBlock);
    
    /* Save the phy map location. */
    m_block = mapPhysicalBlock;
    
    /* Scan the block. */
    status = buildSectionOffsetTable();
    if (status != SUCCESS)
    {
        return status;
    } /* Endif */
    
    /* Get a temp buffer. */
    SectorBuffer buffer;
    if ((status = buffer.acquire()) != SUCCESS)
    {
        return status;
    } /* Endif */
    
    /* Prepare buffer pointers. */
    NandMapSectionHeader_t * header = (NandMapSectionHeader_t *)buffer.getBuffer();
    uint32_t *pSectionPtr = ((uint32_t *)(buffer.getBuffer()) + SIZE_IN_WORDS(sizeof(*header)));
    uint8_t * pStartAddr = (uint8_t *)m_phymap->getAllEntries();
    uint32_t startEntryNumber = 0;
    
    /* Read each of the map sections from the NAND. */
    while (startEntryNumber < (uint32_t)m_totalEntryCount)
    {
        /* 
        ** This call loads the entire section page into our buffer, including the header.
        ** It also verifies the header, so we know the section is valid unless an error
        ** is returned. 
        */
        status = retrieveSection(startEntryNumber, buffer, true);
        if (status != SUCCESS)
        {
            return status;
        } /* Endif */

        /* Copy section data into the phymap. */
        uint32_t u32NumBytes = header->entryCount * PhyMap::kEntrySizeInBytes;
        memcpy(pStartAddr, pSectionPtr, u32NumBytes);
        
        /* Advance to the next section. */
        startEntryNumber += header->entryCount;
        pStartAddr += u32NumBytes;
    } /* Endwhile */
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : save
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function saves phymap to device
*
*END*--------------------------------------------------------------------*/
RtStatus_t PersistentPhyMap::save()
{ /* Body */
    assert(m_phymap);
    assert(m_block.isValid());
    
    RtStatus_t status;
    uint32_t currentEntryNumber = 0;
    int remainingEntries = m_totalEntryCount;
    
    WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "Saving phymap to block %u\n", m_block.get());
    
    while (remainingEntries > 0)
    {
        /* Write this section. */
        status = addSection((uint8_t *)&(*m_phymap)[currentEntryNumber], currentEntryNumber, remainingEntries);
        
        if (status != SUCCESS)
        {
            return status;
        } /* Endif */

        if (m_trustFlag == true) {
            WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "\nTrust number in phy = %d\n", m_trustNumber);
        } /* Endif */
        
        /* Advance to the next section. */
        currentEntryNumber += m_maxEntriesPerPage;
        remainingEntries -= m_maxEntriesPerPage;
    } /* Endwhile */
    
    WL_LOG(WL_MODULE_MAPPER, WL_LOG_INFO, "New top page index of phymap is %u (block #%u)\n", m_topPageIndex, m_block.get());
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : saveNewCopy
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function saves a copy of map to device
*
*END*--------------------------------------------------------------------*/
RtStatus_t PersistentPhyMap::saveNewCopy()
{ /* Body */
    uint32_t physicalBlock;
    
    /* Use the phymap to allocate a block from the block range reserved for maps. */
    RtStatus_t ret = m_mapper.getBlock(&physicalBlock, kMapperBlockTypeMap, 0);
    if (ret != SUCCESS)
    {
        return ret;
    } /* Endif */

    m_block = physicalBlock;
    m_topPageIndex = 0;
    
    return save();
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getPhyMap
* Returned Value   : PhyMap object
* Comments         :
*   This function returns the PhyMap object
*
*END*--------------------------------------------------------------------*/
PhyMap * PersistentPhyMap::getPhyMap()
{ /* Body */
    return m_phymap;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setPhyMap
* Returned Value   : void
* Comments         :
*   This function replaces a new PhyMap for the old one
*
*END*--------------------------------------------------------------------*/
void PersistentPhyMap::setPhyMap(PhyMap * theMap)
{ /* Body */
    m_phymap = theMap;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getSectionForConsolidate
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function gets a section for consolidating
*
*END*--------------------------------------------------------------------*/
RtStatus_t PersistentPhyMap::getSectionForConsolidate
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
    /* 
    ** If we're loading the phymap, then we should read the requested section from the
    ** NAND to get the latest copy. 
    */
    if (m_isLoading)
    {
        return PersistentMap::getSectionForConsolidate(u32EntryNum, thisSectionNumber, bufferToWrite, bufferEntryCount, sectorBuffer);
    } /* Endif */
    
    /* Otherwise we can assume that the map in memory is the latest copy. */
    assert(m_phymap);
    bufferToWrite = (uint8_t *)&(*m_phymap)[u32EntryNum];
    bufferEntryCount = (uint32_t)MIN(m_maxEntriesPerPage, m_totalEntryCount - (int)u32EntryNum);
    
    return SUCCESS;
} /* Endbody */

/* EOF */
