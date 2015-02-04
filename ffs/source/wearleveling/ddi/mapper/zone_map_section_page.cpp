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
*   This file contains implementation ò a clas to wrap a section of the zone map
*
*
*END************************************************************************/
#include "wl_common.h"
#include "ddi/mapper/zone_map_section_page.h"

using namespace nand;

ZoneMapSectionPage::ZoneMapSectionPage()
:   Page(),
    m_header(NULL),
    m_entrySize(0),
    m_metadataSignature(0),
    m_mapType(0)
{
}

ZoneMapSectionPage::ZoneMapSectionPage(const PageAddress & addr)
:   Page(addr),
    m_header(NULL),
    m_entrySize(0),
    m_metadataSignature(0),
    m_mapType(0)
{
}

ZoneMapSectionPage & ZoneMapSectionPage::operator = (const PageAddress & other)
{
    Page::operator =(other);
    return *this;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : buffersDidChange
* Returned Value   : void
* Comments         :
*   This function is the overridden version of setBuffers() also sets the header struct pointer.
*
*END*--------------------------------------------------------------------*/
void ZoneMapSectionPage::buffersDidChange()
{ /* Body */
    Page::buffersDidChange();
    m_header = (NandMapSectionHeader_t *)m_pageBuffer.getBuffer();
    m_sectionData = (uint8_t *)m_pageBuffer.getBuffer() + sizeof(*m_header);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getSectionNumber
* Returned Value   : MQX_OK or an error code
* Comments         :
*   This function returns the section number
*
*END*--------------------------------------------------------------------*/
uint32_t ZoneMapSectionPage::getSectionNumber()
{ /* Body */
    assert(m_entrySize);
    return m_header->startLba / getMaxEntriesPerPage();
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : validateHeader
* Returned Value   : TRUE or FALSE
*   - TRUE if the header is validate
*   - FALSE if the header is invalidate
* Comments         :
*   This function checks the header is validate or not
*
*END*--------------------------------------------------------------------*/
bool ZoneMapSectionPage::validateHeader()
{ /* Body */
    assert(m_mapType);
    return (m_header->signature == kNandMapHeaderSignature
    && m_header->mapType == m_mapType
    && m_header->version == kNandMapSectionHeaderVersion);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : writeSection
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function writes one page of the map block.
*
*END*--------------------------------------------------------------------*/
RtStatus_t ZoneMapSectionPage::writeSection
(
    /* [IN] Starting entry number */
    uint32_t startingEntryNum,
    
    /* [IN] Remaining entries */
    uint32_t remainingEntries,
    
    /* [IN] Starting entry */
    uint8_t * startingEntry,
    
    /* [OUT] Number of written entries */
    uint32_t * actualNumEntriesWritten,
    
    /* [IN] Flag to write trustNumber or not */
    bool trustFlag,
    
    /* [IN] TrustNumber */
    uint16_t trustNumber
)
{ /* Body */
    assert(startingEntry);
    assert(actualNumEntriesWritten);
    assert(m_pageBuffer.hasBuffer());
    assert(m_auxBuffer.hasBuffer());
    assert(m_metadataSignature);
    assert(m_mapType);
    
    RtStatus_t ret;
    uint32_t numWritten;
    
    /* 
    ** Calculate the entries per page dynamically using the size of each entry that was
    ** passed in. We do this instead of using the globals in g_MapperDescriptor because this
    ** function will be called for both zone and phy maps, which may have differing entry sizes. 
    */
    uint32_t entriesPerPage = getMaxEntriesPerPage();
    numWritten = MIN(entriesPerPage, remainingEntries);

    /* Fill in the header. */
    m_header->signature = kNandMapHeaderSignature;
    m_header->mapType = m_mapType;
    m_header->version = kNandMapSectionHeaderVersion;
    m_header->entrySize = m_entrySize;
    m_header->startLba = startingEntryNum;
    m_header->entryCount = numWritten;

    /* Copy the entries into our sector buffer, after the header. */
    memcpy(m_sectionData, startingEntry, numWritten * m_entrySize);

    /* Initialize the redundant area. Set the Stmp code to the value passed in u32String. */
    if (trustFlag == true) 
    {
        /* Do something to edit metadata */
        m_metadata.prepareWithTrust(m_metadataSignature, trustNumber);
    } 
    else 
    {
        m_metadata.prepare(m_metadataSignature);
    } /* Endif */

    // TODO: Write the page.
    ret = write();
    
    /* Set the number of entries written based on whether the write succeeded. */
    if (ret)
    {
        *actualNumEntriesWritten = 0;
    }
    else
    {
        *actualNumEntriesWritten = numWritten;
    } /* Endif */

    return ret;
} /* Endbody */

/* EOF */
