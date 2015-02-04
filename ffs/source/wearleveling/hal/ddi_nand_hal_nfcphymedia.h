#if !defined(__ddi_nand_hal_nfcphymedia_h__)
#define __ddi_nand_hal_nfcphymedia_h__
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
*   Header for Nand Physical Media for NFC.
*
*
*END************************************************************************/

class NFCNandMedia: public NandPhysicalMedia {
public:
    typedef struct {
        uint32_t ID;
    } NFCNandID_t;
    
    NFCNandMedia(uint32_t chipNumber) ;

    virtual ~NFCNandMedia() {}

    void initChipParam();
    void initHalContextParams();
    virtual RtStatus_t reset() { return SUCCESS;}
    virtual RtStatus_t readID(uint8_t * pReadIDCode);
    virtual RtStatus_t readRawData(uint32_t wSectorNum, uint32_t columnOffset,
    uint32_t readByteCount, SECTOR_BUFFER * pBuf);
    virtual RtStatus_t readPage(uint32_t uSectorNumber, SECTOR_BUFFER * pBuffer,
    SECTOR_BUFFER * pAuxiliary);
    virtual RtStatus_t readMetadata(uint32_t uSectorNumber, SECTOR_BUFFER * pBuffer) ;
    virtual RtStatus_t writeRawData(uint32_t pageNumber, uint32_t columnOffset,
    uint32_t writeByteCount, const SECTOR_BUFFER * data);
    virtual RtStatus_t writePage(uint32_t uSectorNum, const SECTOR_BUFFER * pBuffer,
    const SECTOR_BUFFER * pAuxiliary) ;
    virtual RtStatus_t writeFirmwarePage(uint32_t uSectorNum,
    const SECTOR_BUFFER * pBuffer, const SECTOR_BUFFER * pAuxiliary) ;
    virtual RtStatus_t readFirmwarePage(uint32_t uSectorNumber,
    SECTOR_BUFFER * pBuffer, SECTOR_BUFFER * pAuxiliary) ;

    virtual RtStatus_t eraseBlock(uint32_t uBlockNumber) ;
    virtual RtStatus_t eraseMultipleBlocks(uint32_t startBlockNumber,
    uint32_t requestedBlockCount, uint32_t * actualBlockCount) ;
    virtual RtStatus_t copyPages(NandPhysicalMedia * targetNand,
    uint32_t wSourceStartSectorNum, uint32_t wTargetStartSectorNum,
    uint32_t wNumSectors, SECTOR_BUFFER * sectorBuffer,
    SECTOR_BUFFER * auxBuffer, NandCopyPagesFilter * filter,
    uint32_t * successfulPages);
    virtual bool isBlockBad(uint32_t blockAddress, SECTOR_BUFFER * auxBuffer,
    bool checkFactoryMarkings = false, RtStatus_t * readStatus = NULL);
    virtual RtStatus_t markBlockBad(uint32_t blockAddress, SECTOR_BUFFER * pageBuffer,
    SECTOR_BUFFER * auxBuffer);
    
    virtual RtStatus_t enableSleep(bool isEnabled) { return SUCCESS; }
    virtual bool isSleepEnabled()  { return false; }
    virtual char * getDeviceName(){ return NULL; }

    // TODO: add getAuxSize function to get aux space
    uint32_t getAuxSize() { return pNANDParams->pageMetadataSize; }
    RtStatus_t checkSpaceForMetadata();



protected:
    MQX_FILE_PTR m_fd_ptr;
};

#endif /*  __ddi_nand_hal_nfcphymedia_h__ */

/* EOF */
