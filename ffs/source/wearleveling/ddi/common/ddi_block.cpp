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
*   This file contains a class to wrap a block of a NAND.
*
*
*END************************************************************************/

#include "ddi_block.h"
#include "ddi_page.h"
#include "media_buffer.h"

using namespace nand;

Block::Block()
:   BlockAddress()
{
    m_nand = NandHal::getFirstNand();
}

Block::Block(const BlockAddress & addr)
:   BlockAddress(addr)
{
    m_nand = BlockAddress::getNand();
}

Block & Block::operator = (const Block & other)
{
    m_nand = other.m_nand;
    m_address = other.m_address;
    return *this;
}

Block & Block::operator = (const BlockAddress & addr)
{
    BlockAddress::operator=(addr);
    m_nand = BlockAddress::getNand();
    return *this;
}

Block & Block::operator = (const PageAddress & page)
{
    BlockAddress::operator=(page);
    m_nand = BlockAddress::getNand();
    return *this;
}

void Block::set(const BlockAddress & addr)
{
    BlockAddress::set(addr);
    m_nand = BlockAddress::getNand();
}

Block & Block::operator ++ ()
{
    BlockAddress::operator ++ ();
    m_nand = BlockAddress::getNand();
    return *this;
}

Block & Block::operator -- ()
{
    BlockAddress::operator -- ();
    m_nand = BlockAddress::getNand();
    return *this;
}

Block & Block::operator += (uint32_t amount)
{
    BlockAddress::operator += (amount);
    m_nand = BlockAddress::getNand();
    return *this;
}

Block & Block::operator -= (uint32_t amount)
{
    BlockAddress::operator -= (amount);
    m_nand = BlockAddress::getNand();
    return *this;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : readPage
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function reads a page from device to buffer
*
*END*--------------------------------------------------------------------*/
RtStatus_t Block::readPage
(
    /* [IN] Offset of page */
    unsigned pageOffset, 
    
    /* [OUT] Buffer pointer holds data */
    SECTOR_BUFFER * buffer, 
    
    /* [OUT] Buffer pointer holds metadata */
    SECTOR_BUFFER * auxBuffer
)
{ /* Body */
    return m_nand->readPage(PageAddress(m_address, pageOffset).getRelativePage(), buffer, auxBuffer);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : readMetadata
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function reads metadata of a page from device to buffer
*
*END*--------------------------------------------------------------------*/
RtStatus_t Block::readMetadata
(
    /* [IN] Offset of page */
    unsigned pageOffset, 
    
    /* [OUT] Buffer pointer holds metadata */
    SECTOR_BUFFER * buffer
)
{ /* Body */
    return m_nand->readMetadata(PageAddress(m_address, pageOffset).getRelativePage(), buffer);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : writePage
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function writes data and metadata to a page
*
*END*--------------------------------------------------------------------*/
RtStatus_t Block::writePage
(
    /* [IN] Offset of a page */
    unsigned pageOffset, 
    
    /* [IN] Buffer pointer holds data */
    const SECTOR_BUFFER * buffer, 
    
    /* [IN] Buffer pointer holds metadata */
    SECTOR_BUFFER * auxBuffer)
{ /* Body */
    return m_nand->writePage(PageAddress(m_address, pageOffset).getRelativePage(), buffer, auxBuffer);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : erase
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function erases the block
*
*END*--------------------------------------------------------------------*/
RtStatus_t Block::erase()
{ /* Body */
    return m_nand->eraseBlock(getRelativeBlock());
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : isMarkedBad
* Returned Value   : TRUE of FALSE
*   - TRUE if the block is bad
*   - FALSE if the block is not bad
* Comments         :
*   This function checks the block is bad or not
*
*END*--------------------------------------------------------------------*/
bool Block::isMarkedBad
(
    /* [IN] Buffer pointer holds metadata */
    SECTOR_BUFFER * auxBuffer, 
    
    /* [OUT] Pointer holds status of acquiring buffer */
    RtStatus_t * status)
{ /* Body */
    /* Allocate a temp buffer if one was not provided. */
    AuxiliaryBuffer tempBuffer;
    if (!auxBuffer)
    {
        RtStatus_t status = tempBuffer.acquire();
        if (status != SUCCESS)
        {
            /* 
            ** We couldn't allocate the buffer. In debug builds we want to call attention to this,
            ** but in release builds we just treat the block as if it's good since that is the
            ** most common case. 
            */
            assert(false);
            // return false;
        } /* Endif */
        
        auxBuffer = tempBuffer;
    } /* Endif */

    uint32_t relativeAddress = getRelativeBlock();
    
#if defined(STMP378x)
    /* 
    ** Skip NCB1 on NAND0 or NCB2 on NAND1. We must only skip if the NCB is already in place,
    ** otherwise we might misreport a truly bad block as good. However, none of this
    ** even needs to be done if the page size is larger than 2KB since 
    */
    BootBlocks & bootBlocksInfo = g_nandMedia->getBootBlocks();
    if (NandHal::getParameters().pageTotalSize > LARGE_SECTOR_TOTAL_SIZE && bootBlocksInfo.hasValidNCB())
    {
        unsigned nandNumber = m_nand->wChipNumber;
        if (bootBlocksInfo.m_ncb1.doesAddressMatch(nandNumber, relativeAddress) || bootBlocksInfo.m_ncb2.doesAddressMatch(nandNumber, relativeAddress))
        {
            return false;
        } /* Endif */
    } /* Endif */
#endif /* defined(STMP378x) */

    /* Don't check factory markings. */
    return m_nand->isBlockBad(relativeAddress, auxBuffer, false, status);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : markBad
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function marks the block as bad on device
*
*END*--------------------------------------------------------------------*/
RtStatus_t Block::markBad()
{ /* Body */
    MediaBuffer buffer;
    RtStatus_t status;
    if ((status = buffer.acquire(kMediaBufferType_NANDPage)) != SUCCESS)
    {
        return status;
    } /* Endif */

    AuxiliaryBuffer auxBuffer;
    if ((status = auxBuffer.acquire()) != SUCCESS)
    {
        return status;
    } /* Endif */
    
    return m_nand->markBlockBad(getRelativeBlock(), buffer, auxBuffer);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : eraseAndMarkOnFailure
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function erases the block and marks it bad if erasing is failed
*
*END*--------------------------------------------------------------------*/
RtStatus_t Block::eraseAndMarkOnFailure()
{ /* Body */
    RtStatus_t status = erase();
    if (status != SUCCESS)
    {
        WL_LOG(WL_MODULE_HAL, WL_LOG_WARNING, "*** Erase failed: new bad block %u! ***\n", get());
        
        markBad();
    } /* Endif */
    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : isErased
* Returned Value   : TRUE or FALSE
*   - TRUE if the block was erased
*   - FALSE if the block was not erased
* Comments         :
*   This function checks the blocks was erased or not
*
*END*--------------------------------------------------------------------*/
bool Block::isErased()
{ /* Body */
    Page firstPage(*this);
    /* Allocate only aux buffer. */
    firstPage.allocateBuffers(false, true); 
    if (firstPage.readMetadata() != SUCCESS)
    {
        return false;
    } /* Endif */
    return firstPage.getMetadata().isErased();
} /* Endbody */

/* EOF */
