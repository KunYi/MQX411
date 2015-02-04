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
*   This file contains class to wrap a page of a NAND.
*
*
*END************************************************************************/

#include "ddi_page.h"
#include "ddi_block.h"
#include "media_buffer.h"

using namespace nand;

#if !defined(__ghs__)
//#pragma mark --Page--
#endif

Page::Page()
:   PageAddress(),
    m_nand(NandHal::getFirstNand()),
    m_pageBuffer(),
    m_auxBuffer(),
    m_metadata()
{
}

Page::Page(const PageAddress & addr)
:   PageAddress(addr),
    m_pageBuffer(),
    m_auxBuffer(),
    m_metadata()
{
    m_nand = PageAddress::getNand();
}

Page::Page(const BlockAddress & addr)
:   PageAddress(addr),
    m_pageBuffer(),
    m_auxBuffer(),
    m_metadata()
{
    m_nand = PageAddress::getNand();
}

Page::Page(const Page & other)
:   PageAddress(other),
    m_pageBuffer(other.m_pageBuffer),
    m_auxBuffer(other.m_auxBuffer),
    m_metadata()
{
    m_nand = PageAddress::getNand();
    
    if (m_auxBuffer.hasBuffer())
    {
        m_metadata.setBuffer(m_auxBuffer);
    } /* Endif */
}

Page & Page::operator = (const Page & other)
{
    m_address = other.m_address;
    m_nand = other.m_nand;
    
    /* 
    ** We must cast to non-const SECTOR_BUFFER since the other's buffer objects
    ** are const in this case. 
    */
    setBuffers((SECTOR_BUFFER *)other.m_pageBuffer.getBuffer(), (SECTOR_BUFFER *)other.m_pageBuffer.getBuffer());
    return *this;
}

Page & Page::operator = (const PageAddress & addr)
{
    PageAddress::operator=(addr);
    m_nand = PageAddress::getNand();
    return *this;
}

Page & Page::operator = (const BlockAddress & addr)
{
    PageAddress::operator=(addr);
    m_nand = PageAddress::getNand();
    return *this;
}

Page::~Page()
{
    releaseBuffers();
}

void Page::set(const PageAddress & addr)
{
    PageAddress::set(addr);
    m_nand = PageAddress::getNand();
}

Page & Page::operator ++ ()
{
    PageAddress::operator++();
    m_nand = PageAddress::getNand();
    return *this;
}

Page & Page::operator -- ()
{
    PageAddress::operator -- ();
    m_nand = PageAddress::getNand();
    return *this;
}

Page & Page::operator += (uint32_t amount)
{
    PageAddress::operator += (amount);
    m_nand = PageAddress::getNand();
    return *this;
}

Page & Page::operator -= (uint32_t amount)
{
    PageAddress::operator -= (amount);
    m_nand = PageAddress::getNand();
    return *this;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setBuffers
* Returned Value   : void
* Comments         :
*   Set value of buffers
*
*END*--------------------------------------------------------------------*/
void Page::setBuffers
(
    /* [IN] Data buffer */
    SECTOR_BUFFER * pageBuffer, 

    /* [IN] Metadata buffer */
    SECTOR_BUFFER * auxBuffer
)
{ /* Body */
    /* Changing the buffer values will release previous buffers if necessary. */
    m_pageBuffer = pageBuffer;
    m_auxBuffer = auxBuffer;

    /* Updated related pointers. */
    buffersDidChange();
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : allocateBuffers
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Allocate buffers
*
*END*--------------------------------------------------------------------*/
RtStatus_t Page::allocateBuffers
(
    /* [IN] Flag to decide allocating buffer for data */
    bool page, 
    
    /* [IN] Flag to decide allocating buffer for metadata */
    bool aux
)
{ /* Body */
    RtStatus_t status;
    
    if (page)
    {
        status = m_pageBuffer.acquire(kMediaBufferType_Sector);
        if (status != SUCCESS)
        {
            return status;
        } /* Endif */
    } /* Endif */
    
    
    if (aux)
    {
        status = m_auxBuffer.acquire(kMediaBufferType_Auxiliary);
        if (status != SUCCESS)
        {
            if (page)
            {
                m_pageBuffer.release();
            } /* Endif */
            
            return status;
        } /* Endif */
    } /* Endif */
    
    /* 
    ** Call the virtual method to update buffers to let subclasses update any of
    ** their own pointers. 
    */
    buffersDidChange();
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : releaseBuffers
* Returned Value   : void
* Comments         :
*   This function release all allocated buffers 
*
*END*--------------------------------------------------------------------*/
void Page::releaseBuffers()
{ /* Body */
    m_pageBuffer.release();
    m_auxBuffer.release();

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : read
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function calls readPage functions to read data and metadata of a page
*   from device
*
*END*--------------------------------------------------------------------*/
RtStatus_t Page::read()
{ /* Body */
    assert(m_pageBuffer.hasBuffer());
    assert(m_auxBuffer.hasBuffer());
    return m_nand->readPage(getRelativePage(), m_pageBuffer, m_auxBuffer);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : readMetadata
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function calls readMetadata functions to read metadata of a page
*   from device
*
*END*--------------------------------------------------------------------*/
RtStatus_t Page::readMetadata()
{ /* Body */
    assert(m_auxBuffer.hasBuffer());
    return m_nand->readMetadata(getRelativePage(), m_auxBuffer);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : write
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function calls writePage function to write data and metadata
*   to a page in device
*
*END*--------------------------------------------------------------------*/
RtStatus_t Page::write()
{ /* Body */
    assert(m_pageBuffer.hasBuffer());
    assert(m_auxBuffer.hasBuffer());
    return m_nand->writePage(getRelativePage(), m_pageBuffer, m_auxBuffer);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : writeAndMarkOnFailure
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function writes data and metadata to a page in device and marks
*   the page as bad
*
*END*--------------------------------------------------------------------*/
RtStatus_t Page::writeAndMarkOnFailure()
{ /* Body */
    RtStatus_t status = write();

    /* 
    ** If error is type ERROR_DDI_NAND_HAL_WRITE_FAILED, then the block we try to write
    ** to is BAD. We need to mark it physically as such. 
    */
    if (status == ERROR_DDI_NAND_HAL_WRITE_FAILED)
    {
        WL_LOG(WL_MODULE_MEDIA, WL_LOG_INFO, "*** Write failed: new bad block %u! ***\n", getBlock().get());        
        /* Mark the block bad */
        Block(*this).markBad();
    } /* Endif */

    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : isErased
* Returned Value   : TRUE or FALSE
*   - TRUE if the page was erased
*   - FALSE if the page was not erased
* Comments         :
*   This function checks the page was erased or not
*
*END*--------------------------------------------------------------------*/
bool Page::isErased(RtStatus_t * status)
{ /* Body */
    RtStatus_t localStatus = readMetadata();
    bool pageIsErased = m_metadata.isErased();
    if (status)
    {
        *status = localStatus;
    } /* Endif */
    return pageIsErased;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : _taskq_test
* Returned Value   : MQX_OK or an error code
* Comments         :
*   This function tests all task queues for correctness
*
*END*--------------------------------------------------------------------*/
void Page::buffersDidChange()
{ /* Body */
    m_metadata.setBuffer(m_auxBuffer);
} /* Endbody */

#if !defined(__ghs__)
//#pragma mark --BootPage--
#endif /* (__ghs__) */

BootPage::BootPage()
:   Page(),
m_doRawWrite(false)
{
}

BootPage::BootPage(const PageAddress & addr)
:   Page(addr),
m_doRawWrite(false)
{
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : write
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function writes a boot page
*
*END*--------------------------------------------------------------------*/
RtStatus_t BootPage::write()
{ /* Body */
    /* Page buffer is required for both raw and ECC writes. */
    assert(m_pageBuffer.hasBuffer());

    /* Perform the write. */
    if (m_doRawWrite)
    {
        return m_nand->writeRawData(getRelativePage(), 0, m_nand->pNANDParams->pageTotalSize, m_pageBuffer);
    }
    else
    {
        assert(m_auxBuffer.hasBuffer());
        return m_nand->writeFirmwarePage(getRelativePage(), m_pageBuffer, m_auxBuffer);
    } /* Endif */
} /* Endbody */

/* EOF */
