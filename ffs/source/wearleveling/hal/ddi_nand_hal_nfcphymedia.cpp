/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
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
*   The Nand Physical Media for NFC.
*
*
*END************************************************************************/

#include "wl_common.h"
#include "ddi_nand_hal.h"
#include "ddi_nand_hal_internal.h"
#include "nandflash.h"
#include "nandflashprv.h"
#include "nandflash_wl_prv.h"

#include "ddi_metadata.h"
#include "hal/ddi_nand_hal_nfcphymedia.h"

/* extern NandHalContext_t g_nandHalContext; */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::NFCNandMedia
* Returned Value   : N/A
* Comments         :
*   Ctor
*
*END*--------------------------------------------------------------------*/
NFCNandMedia::NFCNandMedia
(
/* [IN] chip number for enabling */
uint32_t chipNumber
)
{ /* Body */
    wChipNumber = chipNumber;
    
    m_fd_ptr = g_nandHalFDPTR[chipNumber];

}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::initChipParam
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Init all chip params such as pages per block, page size, etc.
*
*END*--------------------------------------------------------------------*/
void NFCNandMedia::initChipParam() 
{/* Body */
    /* The number of pages in this chip. */
    ioctl(m_fd_ptr, NANDFLASH_IOCTL_GET_NUM_BLOCKS,
    (_mqx_uint_ptr) &wTotalBlocks); 
    
    /* The number of pages in this chip. */
    ioctl(m_fd_ptr, NANDFLASH_IOCTL_GET_NUM_VIRT_PAGES,
    (_mqx_uint_ptr) &totalPages); 

    /* The number of die in this chip. */
    wTotalInternalDice = 1; 
    
    /* The number of blocks in a die. */
    wBlocksPerDie = wTotalBlocks; 

    /* First absolute block of this chip. */
    m_firstAbsoluteBlock = 0; 
    
    /* First absolute page of this chip. */
    m_firstAbsolutePage = 0; 

}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::initHalContextParams
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Init pNANDParams's variable such as pagesPerBlock, totalBlock, etc.
*
*END*--------------------------------------------------------------------*/
void NFCNandMedia::initHalContextParams() 
{/* Body */
    // TODO: move to RTOS dependent
    IO_DEVICE_STRUCT_PTR io_dev_ptr = m_fd_ptr->DEV_PTR;
    IO_NANDFLASH_STRUCT_PTR handle_ptr =
    (IO_NANDFLASH_STRUCT_PTR) io_dev_ptr->DRIVER_INIT_PTR;

    assert(pNANDParams);
    
    /* Maximum percent of blocks that can go bad during the NAND's lifetime. */
    pNANDParams->maxBadBlockPercentage = kDefaultMaxBadBlockPercentage; 

    /* The size of a page's data area. */
    pNANDParams->pageDataSize = handle_ptr->VIRTUAL_PAGE_SIZE;
    
    /* The size of a page's redundant area. */
    /* The number of pages in this chip. */
    ioctl(m_fd_ptr, NANDFLASH_IOCTL_GET_PAGE_METADATA_AREA_SIZE,
    (_mqx_uint_ptr) &pNANDParams->pageMetadataSize); 

    pNANDParams->pageTotalSize = pNANDParams->pageMetadataSize + pNANDParams->pageDataSize;

    /* The number of pages in a block. */
    pNANDParams->wPagesPerBlock = handle_ptr->NUM_VIRTUAL_PAGES
    / handle_ptr->NANDFLASH_INFO_PTR->NUM_BLOCKS; 

    /* Shift a sector number this many bits to the right to get the number of the containing block. */
    pNANDParams->pageToBlockShift = countBits(pNANDParams->wPagesPerBlock, 0); 
    
    /* Use this mask on a sector number to get the number of the sector within the containing block. */
    pNANDParams->pageInBlockMask = (1 << pNANDParams->pageToBlockShift) - 1;
    ; 
}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::readID
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Read the physical device ID 
*
*END*--------------------------------------------------------------------*/
RtStatus_t NFCNandMedia::readID
(
    /* [OUT] Return Nandflash ID code */
    uint8_t * pReadIDCode
) 
{/* Body */
    return (RtStatus_t) ioctl(m_fd_ptr,
    (_mqx_uint) NANDFLASH_IOCTL_GET_ID, pReadIDCode);
    // return SUCCESS;

}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::readRawData
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Read raw data on given sector No#
*
*END*--------------------------------------------------------------------*/
RtStatus_t NFCNandMedia::readRawData
(
    /* [IN] Sector number for reading */
    uint32_t wSectorNum,
    
    /* [IN] Column offset for reading */
    uint32_t columnOffset, 
    
    /* [IN] Number of byte for reading */
    uint32_t readByteCount, 
    
    /* [OUT] Return read buffer */
    SECTOR_BUFFER * pBuf
) 
{/* Body */
    return SUCCESS;

}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::readPage
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Read data & its metadata on given physical sector No#
*
*END*--------------------------------------------------------------------*/
RtStatus_t NFCNandMedia::readPage
(
    /* [IN] Sector number for reading */
    uint32_t uSectorNumber,
    
    /* [OUT] Return read data buffer */
    SECTOR_BUFFER * pBuffer, 
    
    /* [OUT] Return read auxiliary(metadata) buffer */
    SECTOR_BUFFER * pAuxiliary
)
{/* Body */
    _mqx_int result;
    NANDFLASH_PAGE_BUFF_STRUCT page;
    
    fseek(m_fd_ptr, uSectorNumber, IO_SEEK_SET);
    
    page.data = pBuffer;
    page.metadata = pAuxiliary;
    
    result = ioctl(m_fd_ptr, NANDFLASH_IOCTL_READ_WITH_METADATA, (void *) &page);
    
    WL_LOG(WL_MODULE_HAL, WL_LOG_DEBUG, "NFCNandMedia::readPage sectorNumber=%d, result=%d, errCodeConvertToHal=%d \n",
    uSectorNumber, result, os_err_code_to_wl(result, kNandReadPage));
    return os_err_code_to_wl(result, kNandReadPage);
}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::readMetadata
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Read metadata on given physical sector No#
*
*END*--------------------------------------------------------------------*/
RtStatus_t NFCNandMedia::readMetadata
(
    /* [IN] Sector number for reading */
    uint32_t uSectorNumber,
    
    /* [OUT] Return read auxiliary(metadata) buffer */
    SECTOR_BUFFER * pBuffer
)
{/* Body */
    _mqx_int result;

    fseek(m_fd_ptr, uSectorNumber, IO_SEEK_SET);

    result = ioctl(m_fd_ptr, NANDFLASH_IOCTL_READ_METADATA, (_mqx_uint_ptr) pBuffer);
    
    WL_LOG(WL_MODULE_HAL, WL_LOG_DEBUG, "NFCNandMedia::readMetadata sectorNumber=%d, result=%d, errCodeConvertToHal=%d \n",
    uSectorNumber, result, os_err_code_to_wl(result, kNandReadMetadata));
    

    return os_err_code_to_wl(NANDFLASHERR_NO_ERROR, kNandReadMetadata);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::writeRawData
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Write raw data on given physical sector No#
*
*END*--------------------------------------------------------------------*/
RtStatus_t NFCNandMedia::writeRawData
(
    /* [IN] Sector number for writing */
    uint32_t pageNumber,
    
    /* [IN] Column offset for writing */
    uint32_t columnOffset, 
    
    /* [IN] Number of byte for writing */
    uint32_t writeByteCount,
    
    /* [IN] Return write buffer */
    const SECTOR_BUFFER * data
) 
{/* Body */
    return SUCCESS;

}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::writePage
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Write data & its metadata on given physical sector No#
*
*END*--------------------------------------------------------------------*/
RtStatus_t NFCNandMedia::writePage
(
    /* [IN] Sector number for writing */
    uint32_t uSectorNum,
    
    /* [IN] Given data buffer for writing */
    const SECTOR_BUFFER * pBuffer, 
    
    /* [IN] Given Auxiliary buffer for writing*/
    const SECTOR_BUFFER * pAuxiliary
) 
{/* Body */
    _mqx_int result;
    NANDFLASH_PAGE_BUFF_STRUCT page;
    
    page.data = (unsigned char *)pBuffer;
    page.metadata = (unsigned char *)pAuxiliary;
    fseek(m_fd_ptr, uSectorNum, IO_SEEK_SET);
    result = ioctl(m_fd_ptr, NANDFLASH_IOCTL_WRITE_WITH_METADATA, (void *) &page);
    
    WL_LOG(WL_MODULE_HAL, WL_LOG_DEBUG, "NFCNandMedia::writePage sectorNumber=%d, result=%d, errCodeConvertToHal=%d \n",
    uSectorNum, result, os_err_code_to_wl(result, kNandWritePage));
    
    return os_err_code_to_wl(result, kNandWritePage);

}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::writeFirmwarePage
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Write firmware data on given physical sector No#
*
*END*--------------------------------------------------------------------*/
RtStatus_t NFCNandMedia::writeFirmwarePage
(
    /* [IN] Sector number for writing */
    uint32_t uSectorNum,
    
    /* [IN] Data buffer for writing */
    const SECTOR_BUFFER * pBuffer, 
    
    /* [IN] Auxiliary buffer for writing */
    const SECTOR_BUFFER * pAuxiliary
) 
{/* Body */
    return SUCCESS;
}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::readFirmwarePage
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Read firmware data on given physical sector No#
*
*END*--------------------------------------------------------------------*/
RtStatus_t NFCNandMedia::readFirmwarePage
(
    /* [IN] Sector number for writing */
    uint32_t uSectorNumber,
    
    /* [OUT] Buffer for data reading */
    SECTOR_BUFFER * pBuffer, 
    
    /* [OUT] Auxiliary buffer for reading */
    SECTOR_BUFFER * pAuxiliary
) 
{/* Body */
    return SUCCESS;
}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::eraseBlock
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Erase a given physical block No#
*
*END*--------------------------------------------------------------------*/
RtStatus_t NFCNandMedia::eraseBlock
(
    /* [IN] Given block for erase request */
    uint32_t uBlockNumber
)
{/* Body */
    _mqx_int result;
    
    result = ioctl(m_fd_ptr, (_mqx_uint) NANDFLASH_IOCTL_ERASE_BLOCK_FORCE, (void *)uBlockNumber);

    WL_LOG(WL_MODULE_HAL, WL_LOG_DEBUG, "NFCNandMedia::eraseBlock uBlockNumber=%d, result=%d, errCodeConvertToHal=%d \n",
    uBlockNumber, result, os_err_code_to_wl(result, kNandEraseBlock));
    return os_err_code_to_wl(result, kNandEraseBlock);
}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::eraseMultipleBlocks
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Erase multiple physical blocks
*
*END*--------------------------------------------------------------------*/
RtStatus_t NFCNandMedia::eraseMultipleBlocks
(
    /* [IN] Start block No# for erase request */
    uint32_t startBlockNumber,
    
    /* [IN] Number of block for erase request */
    uint32_t requestedBlockCount,
    
    /* [OUT] Actual erased block */
    uint32_t * actualBlockCount
)
{ /* Body */
    _mqx_int result;
    
    *actualBlockCount = 0;
    for (uint32_t i = startBlockNumber; i < requestedBlockCount; i++) {
        result = ioctl(m_fd_ptr, (_mqx_uint)NANDFLASH_IOCTL_ERASE_BLOCK, (void *)i);
        if (NANDFLASHERR_NO_ERROR == result)
        {
            (*actualBlockCount)++;
        } /* Endif */
    } /*Endfor */
    
    WL_LOG(WL_MODULE_HAL, WL_LOG_DEBUG, "NFCNandMedia::eraseMultipleBlocks requestedBlockCount=%d, actualBlockCount=%d, lastResult=%d, errCodeConvertToHal=%d \n ",
    requestedBlockCount, *actualBlockCount, result, os_err_code_to_wl(result, kNandEraseMultiBlock));
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::copyPages
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Copy multiple pages (data & metadata) from source Nand to target Nand
*
*END*--------------------------------------------------------------------*/
RtStatus_t NFCNandMedia::copyPages
(
    /* [IN] Target Nand is request for copy to */
    NandPhysicalMedia * targetNand,
    
    /* [IN] Start from sector No# */
    uint32_t wSourceStartSectorNum, 
    
    /* [IN] Copy to target's sector No# */
    uint32_t wTargetStartSectorNum,
    
    /* [IN] Number of sector for copying */
    uint32_t wNumSectors, 
    
    /* [IN] The temporary sector buffer */
    SECTOR_BUFFER * sectorBuffer,
    
    /* [IN] The temporary auxilary buffer  */
    SECTOR_BUFFER * auxBuffer, 
    
    /* [IN] For copy filtering purpose */
    NandCopyPagesFilter * filter,
    
    /* [OUT] Actual number of copied pages */
    uint32_t * successfulPages
)
{/* Body */
    RtStatus_t status;
    uint32_t copiedPages = 0;
    
    /* 
    ** Note that we don't explicitly lock the HAL here. It will be locked by the read and
    ** write page methods. If we were to add copyback support, then this would have to change. 
    */
    while (wNumSectors) {
        /*  Read in the source page. */
        status = readPage(wSourceStartSectorNum, sectorBuffer, auxBuffer);

        /*  Detect unrecoverable ECC notices here. */
        if ( !nand::is_read_status_success_or_ecc_fixed( status ) )
        {
            break;
        }/* Endif */

        if (filter)
        {
            /*  The modify flag is ignored for now since we don't use copyback. */
            bool didModifyPage = false;
            status = filter->filter(this, targetNand, wSourceStartSectorNum,
            wTargetStartSectorNum, sectorBuffer, auxBuffer, &didModifyPage);
            if (status != SUCCESS)
            {
                break;
            }/* Endif */
        }/* Endif */

        /*  
        ** Write out the target page. Even if the source page was empty (erased), we have to
        ** copy it to the target block, since you cannot skip writing pages within a block. 
        */
        if (targetNand->writePage(wTargetStartSectorNum, sectorBuffer, auxBuffer) != SUCCESS)
        {
            status = ERROR_DDI_NAND_HAL_WRITE_FAILED;
            break;
        }/* Endif */

        wNumSectors--;
        wSourceStartSectorNum++;
        wTargetStartSectorNum++;
        ++copiedPages;
    }/* Endwhile */

    /*  Convert benign ECC notices to SUCCESS here. */
    if (nand::is_read_status_success_or_ecc_fixed(status)) {
        status = SUCCESS;
    }/* Endif */

    /*  Set number of successfully copied pages. */
    if (successfulPages) {
        *successfulPages = copiedPages;
    }/* Endif */
    WL_LOG(WL_MODULE_HAL, WL_LOG_DEBUG, "NFCNandMedia::copyPages status=%d \n", status);
    /*  Return. */
    return status;

}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::isBlockBad
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Check for whether a given block is bad or not
*
*END*--------------------------------------------------------------------*/
bool NFCNandMedia::isBlockBad
(
    /* [IN] Given block No# */
    uint32_t blockAddress, 
    
    /* [IN] The temporary sector buffer */
    SECTOR_BUFFER * auxBuffer,
    
    /* [IN] Indicate whether we should check the factory marking position,
    ** NFCNandMedia currently does implement in lower layer 
    */
    bool checkFactoryMarkings, 
    
    /* [OUT] Read status */
    RtStatus_t * readStatus
)
{ /* Body */

    uint32_t sectorNumber = blockAndOffsetToPage(blockAddress, 0);
    
    /* nfc currently does not support readStatus, it only returns given block is bad or not */
    this->readMetadata(sectorNumber, auxBuffer);

    if ((auxBuffer[0]==0x00) || (auxBuffer[1]==0x00))
    {
        WL_LOG(WL_MODULE_HAL, WL_LOG_INFO,  "NFCNandMedia::isBlockBad, block %d is BAD \n", blockAddress);
    }/* Endif */
    
    
    return (auxBuffer[0]==0x00) || (auxBuffer[1]==0x00);
    
}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::markBlockBad
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Mark a given block is bad 
*
*END*--------------------------------------------------------------------*/
RtStatus_t NFCNandMedia::markBlockBad
(
    /* [IN] Given block No# */
    uint32_t blockAddress,
    
    /* [IN] The temporary sector buffer */
    SECTOR_BUFFER * pageBuffer, 
    
    /* [IN] The temporary sector auxiliary buffer */
    SECTOR_BUFFER * auxBuffer
)
{/* Body */
    _mqx_int result;
    // TODO: NFC skips pageBuffer as well as auxBuffer 
    /* Due to the endianless problem, NAND WL does not use a low-layer mark block function */
    //result = ioctl(m_fd_ptr, NANDFLASH_IOCTL_MARK_BLOCK_AS_BAD, (pointer) blockAddress);    

    /* Erase this block first */
    result = eraseBlock(blockAddress);
    if (result == MQX_OK) 
    {
        /* Prepare aux buffer */
        auxBuffer[0] = 0x00;
        auxBuffer[1] = 0x00;
        /* Mark bad */
        result = this->writePage(blockAndOffsetToPage(blockAddress, 0), pageBuffer, auxBuffer);

        WL_LOG(WL_MODULE_HAL, WL_LOG_INFO,  "NFCNandMedia::markBlockBad, mark block %d bad, reuslt=%d, errCodeConvertToHal=%d \n", 
        blockAddress, result, os_err_code_to_wl(result, kNandMarkBlockBad));

        return (os_err_code_to_wl(result, kNandMarkBlockBad));
    }
    else
    {
        WL_LOG(WL_MODULE_HAL, WL_LOG_DEBUG, "NFCNandMedia::eraseBlock uBlockNumber=%d, result=%d, errCodeConvertToHal=%d \n",
        blockAddress, result, os_err_code_to_wl(result, kNandEraseBlock));
        return os_err_code_to_wl(result, kNandEraseBlock); 
    }
    
}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : NFCNandMedia::isEnoughSpaceForMetadata
* Returned Value   : TRUE or FALSE
*   - TRUE if there is enough space in spare area to work with metadata
*   - FALSE if there is not enough space in spare area to work with metadata
* Comments         :
*   Mark a given block is bad 
*
*END*--------------------------------------------------------------------*/
RtStatus_t NFCNandMedia::checkSpaceForMetadata()
{ /* Body */
    uint32_t freeSpace;
    _mqx_int result = SUCCESS;

    freeSpace = getAuxSize();

    if (result != MQX_OK)
    {
        return result;
    }

    if (freeSpace >= nand::Metadata::getMetadataStructSize()) 
    {
        return SUCCESS;
    }
    else
    {
        return ERROR_DDI_LBA_NAND_SPARE_BLOCKS_EXHAUSTED;
    } /* Endif */
} /* Endbody */

/* EOF */
