#ifndef __ddi_nand_hal_h__
#define __ddi_nand_hal_h__
/*HEADER**********************************************************************
*
* Copyright 2012 Freescale Semiconductor, Inc.
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   This file embodies the NAND HAL interface. Files that use the NAND HAL
*   must include this file, and must not include any other.
*
*   If you're including another NAND HAL header file besides this one, you have
*   almost certainly made a mistake.
*
*
*
*END************************************************************************/

#include "wl_common.h"
#include "ddi_media_errordefs.h"
#include "sectordef.h"
#include "media_buffer.h"

/*
*******************************************************************************
**  Definitions 
*******************************************************************************
*/

/* Flags to use when acquiring a buffer for the NAND driver through the media buffer manager. */
const unsigned kNandBufferFlags = kMediaBufferFlag_None;

/*
** Possible cell types for a NAND. 
**
** The cell type of the NAND determines how many bits are encoded per cell. 
** A single-level cell (SLC) encodes one bit per cell, where the cell voltage 
** swings between Vcc and Vss/GND. A multi-level cell encodes at least two bits 
** per cell by using multiple voltage levels between Vcc and Vss. So an MLC 
** that encodes two bits has four voltage levels. 
*/
typedef enum _nand_hal_cell_type {
    /* Single-level cell. */
    kNandSLC, 
    
    /* Multi-level cell. */
    kNandMLC 
} NandCellType_t;

/*
** Describes the underlying NAND hardware. 
** 
** This structure describes attributes of the NANDs that are shared by all chip selects. 
** Although instances of NandPhysicalMedia point to a copy of this structure, there is 
** in fact only one NAND parameters struct for all chip selects. 
** 
** The basic unit of data transfer for the NAND HAL API is the "page." 
** The NAND HAL decides how large a page will be at initialization time 
** based on the determined device type. Each page consists of both a data portion 
** and a metadata, or redundant area, portion. The metadata portion holds a few 
** bytes of metadata about the page and/or block plus parity bytes for ECC. 
** 
** Due to limitations of the STMP boot ROM, some NANDs will store less than the full amount 
** of data in pages read by the ROM. The firmware page parameters in this structure describe 
** the size of the firmware pages, i.e., those pages read by the ROM. For many NANDs, these 
** will be the same as regular page sizes. 
** 
** Some of the parameters in this structure refer to planes. Note that some manufacturers, 
** notably Toshiba, refer to planes as "districts". Other than name, there is no difference. 
*/
typedef struct _nand_parameters {

    /* Read ID */
    /* Manufacturer code from read ID command results. */
    uint8_t manufacturerCode; 
    
    /* Device code value from read ID commands results. */
    uint8_t deviceCode; 

    /* Type information */
    /* Cell type for this NAND. */
    NandCellType_t cellType; 

    /* Bad blocks */
    /* Maximum percent of blocks that can go bad during the NAND's lifetime. */
    unsigned maxBadBlockPercentage; 

    /* Page Parameters */
    /* The total page size, both data and metadata. */
    uint32_t pageTotalSize; 
    
    /* The size of a page's data area. */
    uint32_t pageDataSize; 
    
    /* The size of a page's redundant area. */
    uint32_t pageMetadataSize; 

    /* Block Parameters */
    /* Shift a sector number this many bits to the right to get the number of the containing block. */
    uint32_t pageToBlockShift;

    /* Use this mask on a sector number to get the number of the sector within the containing block. */
    uint32_t pageInBlockMask; 
    
    /* The number of pages in a block. */
    uint32_t wPagesPerBlock; 

    /* Device Addressing Parameters */
    /* The number of bytes in a column address. */
    uint32_t wNumColumnBytes; 
    
    /* The number of bytes in a row address. */
    uint32_t wNumRowBytes; 

    /* Plane parameters */
    /* Number of planes. */
    uint32_t planesPerDie; 
} NandParameters_t;

/*  Forward declaration. */
class NandCopyPagesFilter;

/*
** Abstract class representing a single NAND device or chip select. 
** 
** This is the root of a per-chip collection of data structures that describe 
** the underlying NAND hardware and provide function pointers for fundamental 
** operations. It contains virtual methods to perform all commands supported 
** by the HAL library. The methods are implemented in NAND type-specific 
** subclasses, thereby providing a common interface to many device types. 
** 
** While the form of these data structures suggests that each NAND chip could 
** be different and independently controlled, this is <i>not</i> the case. In 
** fact, each NAND chip must be exactly the same. 
*/
class NandPhysicalMedia {
public:
    virtual ~NandPhysicalMedia() {}

    /* Parameters common to all of the underlying NAND chips. */
    NandParameters_t * pNANDParams;

    /* Chip properties */
    /* The number of the chip select to which this structure applies. */
    uint32_t wChipNumber; 
    
    /* The number of pages in this chip. */
    uint32_t totalPages; 
    
    /* The number of blocks in this chip. */
    uint32_t wTotalBlocks; 
    
    /* The number of die in this chip. */
    uint32_t wTotalInternalDice; 
    
    /* The number of blocks in a die. */
    uint32_t wBlocksPerDie; 
    
    /* First absolute block of this chip. */
    uint32_t m_firstAbsoluteBlock; 
    
    /* First absolute page of this chip. */
    uint32_t m_firstAbsolutePage; 
    
    /* Address conversion */    
    /*FUNCTION*----------------------------------------------------------------
    *
    * Function Name    : blockToPage
    * Returned Value   : The page address for the first page of block.
    * Comments         :
    *   This function converts a block number to a page number.
    *
    *END*--------------------------------------------------------------------*/
    inline uint32_t blockToPage
    (
        /* [IN] A block number */
        uint32_t block
    ) 
    { /* Body */
        return block << pNANDParams->pageToBlockShift;
    } /* Endbody */

    /*FUNCTION*----------------------------------------------------------------
    *
    * Function Name    : blockAndOffsetToPage
    * Returned Value   : The page address for page index within block.
    * Comments         :
    *   This function converts a block number and relative page index to a page number.
    *
    *END*--------------------------------------------------------------------*/
    inline uint32_t blockAndOffsetToPage
    ( 
        /* [IN] A block number.  */
        uint32_t block, 
        /* [IN] The relative page offset within block. Must not be greater than the number of pages per block.  */
        uint32_t offset
    ) 
    { /* Body */
        return (block << pNANDParams->pageToBlockShift) + offset;
    } /* Endbody */

    /*FUNCTION*----------------------------------------------------------------
    *
    * Function Name    : blockAndOffsetToRelativePage
    * Returned Value   : The page address for page index within block, relative to this chip.
    * Comments         :
    *   This function converts an absolute block number and page offset to a chip-relative page number.
    *
    *END*--------------------------------------------------------------------*/
    inline uint32_t blockAndOffsetToRelativePage
    ( 
        /* [IN] An absolute block number, although a relative block will also work.  */
        uint32_t block,
        
        /* [IN] The offset of the apge within block. Must not be greater than the number of pages per block. */
        uint32_t offset
    ) 
    { /* Body */
        return (blockToRelative(block) << pNANDParams->pageToBlockShift)
        + offset;
    } /* Endbody */

    /*FUNCTION*----------------------------------------------------------------
    *
    * Function Name    : pageToBlock
    * Returned Value   : The block number containing page.
    * Comments         :
    *   This function converts a page address to a block number.
    *
    *END*--------------------------------------------------------------------*/
    inline uint32_t pageToBlock
    (
        /* [IN] The page address to convert.  */
        uint32_t page
    ) 
    { /* Body */
        return page >> pNANDParams->pageToBlockShift;
    } /* Endbody */

    /*FUNCTION*----------------------------------------------------------------
    *
    * Function Name    : pageToBlockAndOffset
    * Returned Value   : void
    * Comments         :
    *   This function converts a page address to a block number plus relative page index.
    *
    *END*--------------------------------------------------------------------*/
    inline void pageToBlockAndOffset
    (
        /* [IN] The page address */
        uint32_t page, 
        
        /* 
        ** [OUT] The block number holds page.  When the range of page 
        ** is [0..pages-in-this-NAND), the range of block is [0..blocks-in-this-NAND).
        ** However, since all NANDs present are the same type and same size, 
        ** this function works equally well with the ranges as 
        ** [0..pages-in-all-NANDs) and [0..blocks-in-all-NANDs), respectively. 
        */
        uint32_t * block,
    
        /* [OUT] The relative page number within block for page. */
        uint32_t * offset
    ) 
    { /* Body */
        *block = page >> pNANDParams->pageToBlockShift;
        *offset = page & pNANDParams->pageInBlockMask;
    } /* Endbody */

    /*FUNCTION*----------------------------------------------------------------
    *
    * Function Name    : blockToRelative
    * Returned Value   : The relative block address. 
    * Comments         :
    *   This function makes a block address relative to this chip. 
    *
    *END*--------------------------------------------------------------------*/
    inline uint32_t blockToRelative
    (
        /* [IN] Absolute block address.  */
        uint32_t block
    ) 
    { /* Body */
        return block & (wTotalBlocks - 1);
    } /* Endbody */

    /*FUNCTION*----------------------------------------------------------------
    *
    * Function Name    : pageToRelative
    * Returned Value   : The page address relative to this chip. 
    * Comments         :
    *   This function makes a page address relative to this chip. 
    *
    *END*--------------------------------------------------------------------*/
    inline uint32_t pageToRelative
    (
        /* [IN] Absolute page address. */
        uint32_t page
    ) 
    { /* Body */
        return page & ((wTotalBlocks << pNANDParams->pageToBlockShift) - 1);
    } /* Endbody */

    /*FUNCTION*----------------------------------------------------------------
    *
    * Function Name    : baseAbsoluteBlock
    * Returned Value   : The absolute address of the first block of this chip.
    * Comments         :
    *   This function returns the absolute address of the first block of this chip.
    *
    *END*--------------------------------------------------------------------*/
    inline uint32_t baseAbsoluteBlock() 
    { /* Body */
        return m_firstAbsoluteBlock;
    } /* Endbody */

    /*FUNCTION*----------------------------------------------------------------
    *
    * Function Name    : baseAbsolutePage
    * Returned Value   : The absolute address of the first page of this chip.
    * Comments         :
    *   This function returns the absolute address of the first page of this chip.
    *
    *END*--------------------------------------------------------------------*/
    inline uint32_t baseAbsolutePage() 
    { /* Body */
        return m_firstAbsolutePage;
    } /* Endbody */

    /* Basic operations */

    virtual RtStatus_t reset() = 0;
    
    virtual RtStatus_t readID(uint8_t * pReadIDCode) = 0;

    /* Reading */

    virtual RtStatus_t readRawData(uint32_t wSectorNum, uint32_t columnOffset,
    uint32_t readByteCount, SECTOR_BUFFER * pBuf) = 0;

    virtual RtStatus_t
    readPage(uint32_t uSectorNumber, SECTOR_BUFFER * pBuffer,
    SECTOR_BUFFER * pAuxiliary) = 0;
    
    virtual RtStatus_t readMetadata(uint32_t uSectorNumber,
    SECTOR_BUFFER * pBuffer) = 0;
    
    virtual RtStatus_t writeRawData(uint32_t pageNumber, uint32_t columnOffset,
    uint32_t writeByteCount, const SECTOR_BUFFER * data) = 0;
    
    virtual RtStatus_t writePage(uint32_t uSectorNum,
    const SECTOR_BUFFER * pBuffer, const SECTOR_BUFFER * pAuxiliary) = 0;

    virtual RtStatus_t writeFirmwarePage(uint32_t uSectorNum,
    const SECTOR_BUFFER * pBuffer, const SECTOR_BUFFER * pAuxiliary) = 0;

    virtual RtStatus_t
    readFirmwarePage(uint32_t uSectorNumber, SECTOR_BUFFER * pBuffer,
    SECTOR_BUFFER * pAuxiliary) = 0;
    
    virtual RtStatus_t eraseBlock(uint32_t uBlockNumber) = 0;
    
    virtual RtStatus_t eraseMultipleBlocks(uint32_t startBlockNumber,
    uint32_t requestedBlockCount, uint32_t * actualBlockCount) = 0;
    
    virtual RtStatus_t copyPages(NandPhysicalMedia * targetNand,
    uint32_t uSourceStartSectorNum, uint32_t uTargetStartSectorNum,
    uint32_t wNumSectors, SECTOR_BUFFER * sectorBuffer,
    SECTOR_BUFFER * auxBuffer, NandCopyPagesFilter * filter,
    uint32_t * successfulPages) = 0;
    
    /* Bad blocks */
    virtual bool
    isBlockBad(uint32_t blockAddress, SECTOR_BUFFER * auxBuffer,
    bool checkFactoryMarkings = false, RtStatus_t * readStatus =
    NULL) = 0;
    
    virtual RtStatus_t markBlockBad(uint32_t blockAddress,
    SECTOR_BUFFER * pageBuffer, SECTOR_BUFFER * auxBuffer) = 0;

    /* Sleep */    
    virtual RtStatus_t enableSleep(bool isEnabled) = 0;
    
    virtual bool isSleepEnabled() = 0;
    
    /* Device properties */
    virtual char * getDeviceName() = 0;
};

/*
** Abstract class for filtering page contents during a copy operation.
*/
class NandCopyPagesFilter {
public:
    virtual RtStatus_t filter(NandPhysicalMedia * fromNand,
    NandPhysicalMedia * toNand, uint32_t fromPage, uint32_t toPage,
    SECTOR_BUFFER * sectorBuffer, SECTOR_BUFFER * auxBuffer,
    bool * didModifyPage) = 0;
};

/*
** Static interface to NAND HAL.
*/
class NandHal {
public:

    /* Init and shutdown */
    /* Initialize the entire HAL and identify connected NAND devices. */
    static RtStatus_t init();

    /* Shutdown the HAL, preventing further access to the NANDs. */
    static RtStatus_t shutdown();
    
    /* Chip selects */
    /* Return the number of active chip selects. */
    static unsigned getChipSelectCount();

    /* Return the chip select number given an absolute block address. */
    static unsigned getChipSelectForAbsoluteBlock(uint32_t block);

    /* The chip select number given an absolute page address. */
    static unsigned getChipSelectForAbsolutePage(uint32_t page);

    /* Returns the combined number of blocks of all chip selects. */
    static uint32_t getTotalBlockCount();

    /* Determine whether a block address is valid. */
    static bool isAbsoluteBlockValid(uint32_t block);

    /* Determine whether a page address is valid. */
    static bool isAbsolutePageValid(uint32_t page);

    /* Nand objects */

    /* Returns the first NAND object. */
    static inline NandPhysicalMedia * getFirstNand() {
        return getNand(0);
    }

    /* Return the NAND object for a given chip select. */
    static NandPhysicalMedia * getNand(unsigned chipSelect);

    /* Return the NAND object for a given absolute block address. */
    static NandPhysicalMedia * getNandForAbsoluteBlock(uint32_t block);

    /* Return the NAND object for a given absolute page address. */
    static NandPhysicalMedia * getNandForAbsolutePage(uint32_t page);
    
    /* Access the shared parameters object. */
    static NandParameters_t & getParameters();
    
    /*
    ** Helper class to temporarily adjust sleep enablement.
    */
    class SleepHelper {
    public:
        /* Constructor. Saves previous sleep state and changes to new. */
        SleepHelper(bool isEnabled) {}

        /* Destructor. Restores sleep state to the previous value. */
        ~SleepHelper() {}

    protected:
    };

};

namespace nand {

class PageAddress;

#if defined(__ghs__)
#pragma ghs section text=".static.text"
#endif 

#if defined (__CODEWARRIOR__)
#endif 

/*
** Helper class to represent block addresses.
*/
class BlockAddress {
public:

    inline BlockAddress() :
    m_address(0) {
    }
    inline BlockAddress(uint32_t absoluteBlock) :
    m_address(absoluteBlock) {
    }
    inline BlockAddress(uint32_t nand, uint32_t relativeBlock) {
        m_address = NandHal::getNand(nand)->baseAbsoluteBlock() + relativeBlock;
    }
    explicit inline BlockAddress(const PageAddress & page) {
        set(page);
    }
    inline BlockAddress(const BlockAddress & other) :
    m_address(other.m_address) {
    }

    inline BlockAddress & operator =(const BlockAddress & other) {
        set(other);
        return *this;
    }

    /* Returns the absolute block address. */
    inline uint32_t get() const {
        return m_address;
    }

    /* Change the address. */
    inline void set(const BlockAddress & addr) {
        m_address = addr.m_address;
    }

    /* Change the address. */
    inline void set(const PageAddress & addr);

    /* Returns the block as a page. */
    inline PageAddress getPage() const;

    /* Returns the absolute block address. */
    inline operator uint32_t() const 
    {
        return m_address;
    }

    /* Conversion operator to page address. */
    inline operator PageAddress() const;

    /* Returns true if the block address is valid. */
    inline bool isValid() const 
    {
        return NandHal::isAbsoluteBlockValid(m_address);
    }

    /* Get the block's NAND object. */
    inline NandPhysicalMedia * getNand() const 
    {
        return isValid() ? NandHal::getNandForAbsoluteBlock(m_address) : NULL;
    }

    /* Get the block address as a NAND relative block. */
    inline uint32_t getRelativeBlock() const 
    {
        return getNand()->blockToRelative(m_address);
    }

    /* Prefix increment operator to advance the address to the next block. */
    inline BlockAddress & operator ++() 
    {
        ++m_address;
        return *this;
    }

    /* Prefix decrement operator. */
    inline BlockAddress & operator --() 
    {
        --m_address;
        return *this;
    }

    /* Increment operator. */
    inline BlockAddress & operator +=(uint32_t amount) 
    {
        m_address += amount;
        return *this;
    }

    /* Decrement operator. */
    inline BlockAddress & operator -=(uint32_t amount) 
    {
        m_address -= amount;
        return *this;
    }

protected:
    /* Absolute block address. */
    uint32_t m_address; 
};

/*
** Helper class to represent page addresses.
*/
class PageAddress {
public:

    inline PageAddress() :
    m_address(0) 
    {
    }
    
    inline PageAddress(uint32_t absolutePage) :
    m_address(absolutePage) 
    {
    }
    
    inline PageAddress(uint32_t absoluteBlock, uint32_t pageOffset) 
    {
        m_address
        = NandHal::getNandForAbsoluteBlock(absoluteBlock)->blockAndOffsetToPage(
        absoluteBlock, pageOffset);
    }
    
    inline PageAddress(NandPhysicalMedia * nand, uint32_t relativePage) 
    {
        m_address = nand->baseAbsolutePage() + relativePage;
    }
    
    inline PageAddress(uint32_t nandNumber, uint32_t relativeBlock,
    uint32_t pageOffset) 
    {
        NandPhysicalMedia * nand = NandHal::getNand(nandNumber);
        m_address = nand->baseAbsolutePage() + nand->blockAndOffsetToPage(
        relativeBlock, pageOffset);
    }
    
    explicit inline PageAddress(const BlockAddress & block,
    uint32_t pageOffset = 0) 
    {
        m_address
        = NandHal::getNandForAbsoluteBlock(block.get())->blockAndOffsetToPage(
        block.get(), pageOffset);
    }
    
    inline PageAddress(const PageAddress & other) :
    m_address(other.m_address) 
    {
    }

    inline PageAddress & operator =(const PageAddress & other) 
    {
        set(other);
        return *this;
    }

    /* Returns the absolute page address. */
    inline uint32_t get() const 
    {
        return m_address;
    }

    /* Change the address. */
    inline void set(const PageAddress & addr) 
    {
        m_address = addr.m_address;
    }

    /* Change the address. */
    inline void set(const BlockAddress & addr) 
    {
        m_address = addr.get() << NandHal::getParameters().pageToBlockShift;
    }

    /* Returns the page as a block. */
    inline BlockAddress getBlock() const 
    {
        return BlockAddress(*this);
    }

    /* Returns the absolute page address. */
    inline operator uint32_t() const 
    {
        return m_address;
    }

    /* Conversion operator to block address. */
    inline operator BlockAddress() const 
    {
        return getBlock();
    }

    /* Returns true if the page address is valid. */
    inline bool isValid() const 
    {
        return NandHal::isAbsolutePageValid(m_address);
    }

    /* Get the page's NAND object. */
    inline NandPhysicalMedia * getNand() const 
    {
        return isValid() ? NandHal::getNandForAbsolutePage(m_address) : NULL;
    }

    /* Get the page as a NAND relative page address. */
    inline uint32_t getRelativePage() const 
    {
        return getNand()->pageToRelative(m_address);
    }

    /* Get the page offset within the block. */
    inline uint32_t getPageOffset() const 
    {
        return m_address % NandHal::getParameters().wPagesPerBlock;
    }

    /* Prefix increment operator to advance the page address to the next page. */
    inline PageAddress & operator ++() 
    {
        ++m_address;
        return *this;
    }

    /* Prefix decrement operator. */
    inline PageAddress & operator --() 
    {
        --m_address;
        return *this;
    }

    /* Increment operator. */
    inline PageAddress & operator +=(uint32_t amount) 
    {
        m_address += amount;
        return *this;
    }

    /* Decrement operator. */
    inline PageAddress & operator -=(uint32_t amount) 
    {
        m_address -= amount;
        return *this;
    }

protected:
    /* The absolute page address. */
    uint32_t m_address; 
};

inline void BlockAddress::set(const PageAddress & addr) 
{
    m_address = addr.get() >> NandHal::getParameters().pageToBlockShift;
}

inline PageAddress BlockAddress::getPage() const 
{
    return PageAddress(*this, 0);
}

inline BlockAddress::operator PageAddress() const 
{
    return getPage();
}

#if defined(__ghs__)
#pragma ghs section text=default
#endif 

#if defined (__CODEWARRIOR__)
#endif 

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : is_read_status_success_or_ecc_fixed_without_decay
* Returned Value   : TRUE or FALSE
* Comments         :
*   This function tests for a successful read which was perfect, 
*   or in which all errors were corrected and the page shows no decay.
*
*END*--------------------------------------------------------------------*/
inline bool is_read_status_success_or_ecc_fixed_without_decay(RtStatus_t status) 
{ /* Body */
    return (status == SUCCESS || status == ERROR_DDI_NAND_HAL_ECC_FIXED);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : is_read_status_ecc_fixed
* Returned Value   : TRUE or FALSE
* Comments         :
*   This function tests for a read in which all errors were corrected 
*   and decay is allowed.
*
*END*--------------------------------------------------------------------*/
inline bool is_read_status_ecc_fixed(RtStatus_t status) 
{ /* Body */
    return (status == ERROR_DDI_NAND_HAL_ECC_FIXED || status
    == ERROR_DDI_NAND_HAL_ECC_FIXED_REWRITE_SECTOR);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : is_read_status_success_or_ecc_fixed
* Returned Value   : MQX_OK or an error code
* Comments         :
*   This function tests for a successful read which was perfect, 
*   or one in which all errors were corrected and decay is allowed. 
*
*END*--------------------------------------------------------------------*/
inline bool is_read_status_success_or_ecc_fixed(RtStatus_t status) 
{ /* Body */
    return (status == SUCCESS || status == ERROR_DDI_NAND_HAL_ECC_FIXED
    || status == ERROR_DDI_NAND_HAL_ECC_FIXED_REWRITE_SECTOR);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : is_read_status_error_excluding_ecc
* Returned Value   : TRUE or FALSE
* Comments         :
*   This function tests for a read that failed, but not because of ECC problems.
*
*END*--------------------------------------------------------------------*/
inline bool is_read_status_error_excluding_ecc(RtStatus_t status) 
{ /* Body */
    return (status != SUCCESS && status != ERROR_DDI_NAND_HAL_ECC_FIXED
    && status != ERROR_DDI_NAND_HAL_ECC_FIXED_REWRITE_SECTOR && status
    != ERROR_DDI_NAND_HAL_ECC_FIX_FAILED);
} /* Endbody */

} /* namespace nand */

#endif /* __ddi_nand_hal_h__ */

/* EOF */
