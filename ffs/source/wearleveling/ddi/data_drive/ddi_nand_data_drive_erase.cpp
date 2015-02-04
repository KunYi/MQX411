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
*   This file handles erasing the data drive.
*
*
*END************************************************************************/

#include "wl_common.h"
#include "ddi_nand_ddi.h"
#include "ddi_nand_data_drive.h"
#include "mapper.h"

using namespace nand;

/* 
** Constants 
*/

/* Comment this line out unless actively profiling this function. */
#define PROFILE_NAND_DD_REPAIR

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : erase
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function will "Erase" the entire Data Drive 
*   which includes all blocks which are not marked as Bad.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DataDrive::erase()
{ /* Body */
    uint32_t iBlk;
    Region * pRegion;
    uint32_t u32AbsolutePhyBlockAddr;
    RtStatus_t ret;
    uint32_t u32TotalNumBlks = 0;
    
    DdiNandLocker locker;
    
    /* Make sure we're initialized */
    if (!m_bInitialized)
    {
        return ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED;
    } /* Endif */

    /* Add up the total number of logical blocks for this drive. */
    Region::Iterator it(m_ppRegion, m_u32NumRegions);
    pRegion = it.getNext();
    while (pRegion)
    {
        /* Loop through all the blocks of this region */
        u32TotalNumBlks += pRegion->m_u32NumLBlks;
        pRegion = it.getNext();
    } /* Endwhile */
    
    Mapper * mapper = m_media->getMapper();
    
    /* Loop through all the logical blocks of this drive. */
    for (iBlk=0; iBlk < u32TotalNumBlks; iBlk++)
    {
        uint32_t virtualBlock;
        
        /* Convert the logical block to a virtual block. */
        ret = logicalBlockToVirtual(iBlk, &pRegion, &virtualBlock);
        if (ret != SUCCESS)
        {
            return ret;
        } /* Endif */

        /* Get the physical block associated with the virtual block. */
        ret = mapper->getBlockInfo(virtualBlock, &u32AbsolutePhyBlockAddr);
        if (ret)
        {
            return ret;
        } /* Endif */
        
        /* if this block has not been mapped, skip the erase. */
        if (mapper->isBlockUnallocated(u32AbsolutePhyBlockAddr))
        {
            continue;
        } /* Endif */

        /* Create the block instance for our physical block. */
        Block phyBlock(u32AbsolutePhyBlockAddr);
        
        /* Erase it */
        ret = phyBlock.erase();
        if (ret == ERROR_DDI_NAND_HAL_WRITE_FAILED)
        {
            WL_LOG(WL_MODULE_LOGICALDRIVE, WL_LOG_INFO, "*** Erase failed: new bad block %u! ***\n", phyBlock.get());
            
            /* 
            ** The beauty of the LBA architecture is that we just need
            ** to mark this as a bad block.  No need to assign a new block,
            ** that will happen automatically the next time we need to
            ** access it.
            ** Mark the block as bad.
            */
            mapper->markBlock(virtualBlock, u32AbsolutePhyBlockAddr, kNandMapperBlockUsed);

            /* Mark the Block as bad on the NAND, in the block itself. */
            phyBlock.markBad();
            
            /* Add the new bad block to the correct region. */
            pRegion = m_media->getRegionForBlock(phyBlock);
            if (pRegion)
            {
                pRegion->addNewBadBlock(phyBlock);
            } /* Endif */
        }
        else if (ret != SUCCESS)
        {
            /* Some unexpected error occurred. */
            return  ret;
        }
        else
        {
            /* Erased block successfully */
            ret = mapper->markBlock(virtualBlock, u32AbsolutePhyBlockAddr, kNandMapperBlockFree);
            if (ret != SUCCESS)
            {
                return ret;
            } /* Endif */
        } /* Endif */
    } /* Endfor */
    
    /* Invalidate all NSSMs for this drive. */
    m_media->getNssmManager()->invalidateDrive(this);

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : repair
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LDRIVE_FS_FORMAT_REQUIRED The drive was erased.
* Comments         :
*   This function will "Repair" an uninitialized Data Drive by erasing all
*   physical blocks associated with the data drive. What makes this function
*   different than NANDDataDriveErase() is that we have to assume that the
*   mapper failed initialization.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DataDrive::repair
(
    /* [IN] Flag to decide scanning for bad location */
    bool bIsScanBad
)
{ /* Body */
    RtStatus_t status;
    int iPhysBlockNum;
    SectorBuffer sectorWriteBuffer;
    AuxiliaryBuffer auxWriteBuffer;
    
    SectorBuffer sectorReadBuffer;
    AuxiliaryBuffer auxReadBuffer;
    
    SECTOR_BUFFER * pBuf;
    RtStatus_t rtCode;
    uint32_t i;
    Page tmppage;
    bool isShouldMarkBad;

    if (!m_bPresent)
    {
        return ERROR_DDI_LDL_LDRIVE_MEDIA_NOT_ALLOCATED;
    } /* Endif */
    
    /* Drive can't be reparied if it is already initialized. */
    if (m_bInitialized)
    {
        return ERROR_DDI_LDL_LDRIVE_ALREADY_INITIALIZED;
    } /* Endif */
    if ((rtCode = sectorWriteBuffer.acquire()) != SUCCESS)
    {
        return ERROR_DDI_MEDIABUFMGR_ALLOC_FAILED;
    } /* Endif */

    if ((rtCode = auxWriteBuffer.acquire()) != SUCCESS)
    {
        return ERROR_DDI_MEDIABUFMGR_ALLOC_FAILED;
    } /* Endif */

    if ((rtCode = sectorReadBuffer.acquire()) != SUCCESS)
    {
        return ERROR_DDI_MEDIABUFMGR_ALLOC_FAILED;
    } /* Endif */

    if ((rtCode = auxReadBuffer.acquire()) != SUCCESS)
    {
        return ERROR_DDI_MEDIABUFMGR_ALLOC_FAILED;
    } /* Endif */


    DdiNandLocker locker;

    /* Loop through all the regions on the media. */
    Region::Iterator it = m_media->createRegionIterator();
    Region * pRegion;
    pRegion = it.getNext();
    while (pRegion)
    {
        NandPhysicalMedia *pNandDesc;
        int iLastBlockNum;

        /* Only hidden drive and data drive regions have the data drive
        ** blocks we are interested in. 
        */
        if (!pRegion->isDataRegion())
        {
            continue;
        } /* Endif */

        pNandDesc = pRegion->m_nand;
        assert(pNandDesc);
        iLastBlockNum = pRegion->m_iStartPhysAddr + pRegion->m_iNumBlks - 1;
        
        
        /* Fill random data in to sector Buffer and Aux Buffer */
        pBuf = (SECTOR_BUFFER *)sectorWriteBuffer;
        for (i = 0; i < pNandDesc->pNANDParams->pageDataSize; i++)
        {
            /* avoid <0x10 in pBuf */
            pBuf[i] = ((i & 0xFF) < 0x10) ? (i & 0xFF)+0x10: (i & 0xFF);
        } /* Endfor */

        pBuf = (SECTOR_BUFFER *)auxWriteBuffer;
        for ( i=0; i < pNandDesc->pNANDParams->pageMetadataSize;i++)
        {
            /* avoid <0x10 in pBuf */
            pBuf[i] = ((i & 0xFF) < 0x10) ? (i & 0xFF)+0x10: (i & 0xFF);
        } /* Endfor */
        

#ifdef PROFILE_NAND_DD_REPAIR
        WL_LOG(WL_MODULE_LOGICALDRIVE, WL_LOG_INFO, "start region: %d\n", pRegion->m_iStartPhysAddr);
#endif /* PROFILE_NAND_DD_REPAIR */

        /* Loop through all the blocks in this region. */
        for (iPhysBlockNum = pRegion->m_iStartPhysAddr; iPhysBlockNum <= iLastBlockNum; iPhysBlockNum++)
        {
            /* Check to see if we should erase this block. */
            if (shouldRepairEraseBlock(iPhysBlockNum, pNandDesc))
            {
                /* OK, this block is NOT mark BAD or hidden */
                isShouldMarkBad = false;
                
                if (bIsScanBad)
                {
                    /* First, erase this block */
                    if (SUCCESS != Block(BlockAddress(pNandDesc->wChipNumber, iPhysBlockNum)).erase())
                    {
                        /* 
                        ** In case we catch error when erase this block, just return true
                        ** for bad making 
                        */
                        isShouldMarkBad = true;
                    } /* Endif */
                    
                    for (i = 0; (!isShouldMarkBad )&&(i< pNandDesc->pNANDParams->wPagesPerBlock);i++)
                    {                        
                        tmppage.set(PageAddress(iPhysBlockNum, i));
                        tmppage.setBuffers(sectorWriteBuffer, auxWriteBuffer);
                        rtCode = tmppage.write();

                        if (SUCCESS != rtCode)
                        {
                            isShouldMarkBad = true;
                            break;
                        } /* Endif */

                        tmppage.setBuffers(sectorReadBuffer, auxReadBuffer);
                        rtCode = tmppage.read();
                        
                        if ((ERROR_DDI_NAND_HAL_ECC_FIXED_REWRITE_SECTOR == rtCode) || 
                                (ERROR_DDI_NAND_HAL_ECC_FIX_FAILED == rtCode))
                        {
                            WL_LOG(WL_MODULE_LOGICALDRIVE, WL_LOG_ERROR,"  block %d, page %d failed with retcode %s\n", iPhysBlockNum, i, wl_err_to_str(rtCode, kNandDontCare));
                            isShouldMarkBad = true;
                            break;
                        } /* Endif */
                    } /* Endfor */
                } /* Endif */
                
                WL_LOG(WL_MODULE_LOGICALDRIVE, WL_LOG_DEBUG,"scan block %d, should mark bad %d\n", 
                iPhysBlockNum, isShouldMarkBad);
                
                Block block(BlockAddress(pNandDesc->wChipNumber, iPhysBlockNum));
                if ( (isShouldMarkBad) || (block.erase() ==  ERROR_DDI_NAND_HAL_WRITE_FAILED) )
                {
                    WL_LOG(WL_MODULE_LOGICALDRIVE, WL_LOG_ERROR, "Handling bad block: %d\n", iPhysBlockNum);
                    /* Use handle new bad block in mapper, in case mapper is successfully initialized */
                    
                    if (m_media->getMapper()->isInitialized())
                    {
                        m_media->getMapper()->handleNewBadBlock(block);
                    }
                    else /* Use NAND HAL for bad marking */
                    {
                        block.markBad();
                    } /* Endif */
                } /* Endif */
            }
            else
            {
                
#ifdef PROFILE_NAND_DD_REPAIR
                WL_LOG(WL_MODULE_LOGICALDRIVE, WL_LOG_INFO, "Skipping: %d\n", iPhysBlockNum);
#endif /* PROFILE_NAND_DD_REPAIR */
            } /* Endif */
        } /* Endfor */
        pRegion = it.getNext();
    } /* Endwhile */

    /* Invalidate sector map entries after a media erase. */
    m_media->getNssmManager()->invalidateAll();
    
    /* Zone-map has been erased.  We need to recreate it. */
    if ((status = m_media->getMapper()->rebuild()) != SUCCESS)
    {
        return status;
    } /* Endif */

    return ERROR_DDI_LDL_LDRIVE_FS_FORMAT_REQUIRED;
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : shouldRepairEraseBlock
* Returned Value   : TRUE or FALSE
*   - TRUE if the block should be erased
*   - FALSE if the block should not be erased
* Comments         :
*   This function will return true if a block should be erased during the repair
*   process. The block should be erased if it is NOT marked bad, is NOT a hidden
*   drive block, or if the redundant area cannot be read.
*
*END*--------------------------------------------------------------------*/
bool DataDrive::shouldRepairEraseBlock
(
    /* [IN] Page address of the first page of the block */
    uint32_t u32BlockFirstPage, 
    
    /* [IN] NAND physical descriptor */
    NandPhysicalMedia *pNandDesc
)
{ /* Body */
    bool        bIsHiddenBlock;
    RtStatus_t  ReadFailErrorVal;
    bool        bIsBad;

    /* If this block is marked bad, it should not be erased. */
    bIsBad = Block(BlockAddress(u32BlockFirstPage)).isMarkedBad(NULL, &ReadFailErrorVal);

    /* The block did not contain garbage... */
    if ( ReadFailErrorVal != ERROR_DDI_NAND_HAL_ECC_FIX_FAILED &&
            /* ...and block was marked bad... */
            bIsBad )
    {
        /* ...so we believe it was bad.  The caller */
        /* should not erase a bad block. */
        #ifdef PROFILE_NAND_DD_REPAIR
        WL_LOG(WL_MODULE_LOGICALDRIVE, WL_LOG_INFO, "<bad> %d\n", u32BlockFirstPage);
        #endif /* PROFILE_NAND_DD_REPAIR */
        return false;
    } /* Endif */

    /* 
    ** See if this block belongs to a hidden drive. This mapper function doesn't really 
    ** use the map, it just reads the block's metadata. 
    */
    bIsHiddenBlock = isBlockHidden(u32BlockFirstPage);

    
    if (bIsHiddenBlock)
    {
        #ifdef PROFILE_NAND_DD_REPAIR
        WL_LOG(WL_MODULE_LOGICALDRIVE, WL_LOG_INFO, "<hidden> \n ");
        #endif /* PROFILE_NAND_DD_REPAIR */
        
        /* Should not be erased */
        return false;
    } /* Endif */    

    /* Return true if this is NOT a hidden block (i.e. it should be erased). */
    return true;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : isBlockHidden
* Returned Value   : TRUE or FALSE
*   - TRUE if Block is part of hidden drive.
*   - FALSE if Block is not part of hidden drive.
* Comments         :
*   This function determines whether or not given block is part of hidden drive.
*
*END*--------------------------------------------------------------------*/
bool DataDrive::isBlockHidden
(   
    /* [IN] Block number of block to consider. */
    uint32_t u32BlockPhysAddr
)
{ /* Body */
    /* Create a page object and allocate just an aux buffer. */
    Page thePage(BlockAddress(u32BlockPhysAddr).getPage());
    thePage.allocateBuffers(false, true);
    
    /* Read the metadata for the first page of the block. */
    RtStatus_t retCode = thePage.readMetadata();
    if (retCode != SUCCESS)
    {
        return false;
    } /* Endif */

    /* Obviously, the block is not a hidden drive block if it is erased. */
    bool bRetVal = false;
    Metadata & md = thePage.getMetadata();
    if (!md.isErased())
    {
        /* Read the flag that indicates whether the block belongs to a hidden drive. */
        bRetVal = md.isFlagSet(Metadata::kIsHiddenBlockFlag);
    } /* Endif */

    return bRetVal;
} /* Endbody */

/* EOF */
