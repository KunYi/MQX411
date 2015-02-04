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
*   This file reads routines for Nand Device Driver.
*
*
*END************************************************************************/
#include "wl_common.h"
#include "ddi_media_internal.h"
#include "ddi/common/ddi_nand_ddi.h"
#include "ddi/data_drive/ddi_nand_data_drive.h"
#include "ddi_nand_hal.h"
#include "ddi/mapper/mapper.h"
#include "buffer_manager/media_buffer.h"

using namespace nand;

#ifndef DEBUG_DDI_NAND_WRITE_SECTOR
#define DEBUG_DDI_NAND_WRITE_SECTOR
#endif

#ifdef DEBUG_DDI_NAND_WRITE_SECTOR
bool bEnableWriteSectorDebug = false;
#endif

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : writeSector
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function will write a page to the NAND. The mapper is used to determine 
*   the actual size.
*   If successful, the data is in pSectorData.  The auxillary data (ECC) 
*   is appended at the end of the valid data.
*   The write is synchronous.  The routine does not return until the write is complete.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DataDrive::writeSector
(
    /* [IN] Logical sector number to write */
    uint32_t u32LogicalSectorNumber, 
    
    /* [IN] Pointer where sector data to be written is stored. */
    const SECTOR_BUFFER * pSectorData
)
{ /* Body */
    RtStatus_t RetValue;

    RetValue = writeSectorInternal(u32LogicalSectorNumber, pSectorData);

#ifdef DEBUG_DDI_NAND_WRITE_SECTOR
    if (RetValue != SUCCESS)
    {
        WL_LOG(WL_MODULE_LOGICALDRIVE, WL_LOG_INFO, "NDDWS failure 0x%X, Drive %d, Lpage 0x%X\r\n",
        RetValue, m_u32Tag, u32LogicalSectorNumber);
    } /* Endif */
#endif

    return RetValue;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : writeSectorInternal
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function will write a page to the NAND. The mapper is used to determine 
*   the actual size.
*   If successful, the data is in pSectorData.  The auxillary data (ECC) 
*   is appended at the end of the valid data.
*   The write is synchronous.  The routine does not return until the write is complete.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DataDrive::writeSectorInternal
(
    /* [IN] Logical sector number to write */
    uint32_t u32LogicalSectorNumber, 
    
    /* [IN] Pointer where sector data to be written is stored. */
    const SECTOR_BUFFER * pSectorData
)
{ /* Body */
    RtStatus_t rtCode;
    Region * pRegion;
    uint32_t u32PhysicalBlockNumber;
    uint32_t u32LogicalSectorOffset;
    uint32_t u32PhysicalSectorOffset;
    uint32_t u32LogicalBlockAddress;
    uint32_t u32SectorsPerBlock = NandHal::getParameters().wPagesPerBlock;
    uint32_t u32PhysicalSectorNumber;
    bool bIsLogicalOrder = false;
    bool bNeedCheckLO = false;
    AuxiliaryBuffer auxBuffer;
    nand::NonsequentialSectorsMap * sectorMap;    
    nand::Metadata md;

    /* Make sure we're initialized */
    if (!m_bInitialized)
    {
        return ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED;
    } /* Endif */

    /* Make sure we won't go out of bounds */
    if (u32LogicalSectorNumber >= m_u32NumberOfSectors)
    {
        return ERROR_DDI_LDL_LDRIVE_SECTOR_OUT_OF_BOUNDS;
    } /* Endif */

    DdiNandLocker locker;

    /* Disable auto sleep for the whole page write process. */
    NandHal::SleepHelper disableSleep(false);

    /* Convert the logical sector to a virtual sector. */
    rtCode = logicalSectorToVirtual(u32LogicalSectorNumber, NULL, &u32LogicalSectorNumber);
    if (rtCode != SUCCESS)
    {
        return rtCode;
    } /* Endif */

    /* 
    ** Use the logical page number to discover what Physical Block and logical 
    ** sector offset the logical page is mapped to. 
    */
    rtCode = m_media->getMapper()->getPageInfo(u32LogicalSectorNumber,
    &u32LogicalBlockAddress,
    &u32PhysicalBlockNumber,
    &u32LogicalSectorOffset);
    
    /* 
    ** If writing the last page in the block, we need to check whether 
    ** the block is in logical order so we can set the is-in-order metadata flag. 
    */
    if (u32LogicalSectorOffset == (u32SectorsPerBlock-1))
    {
        bNeedCheckLO = true;
    } /* Endif */
    
    /* 
    ** If this wasn't a valid PhyAddress, then the block isn't allocated yet. 
    ** So we need to allocate a new block. 
    */
    if (rtCode == ERROR_DDI_NAND_MAPPER_INVALID_PHYADDR)
    {
        /* 
        ** If we can't get valid Page Info, consider this a new Block/Page 
        ** and allocate a new one. 
        ** Grab a new Block for this LBA Address.  LBA is automatically assigned. 
        */
        rtCode = m_media->getMapper()->getBlockAndAssign(u32LogicalBlockAddress, &u32PhysicalBlockNumber, kMapperBlockTypeNormal, 1);
        if (rtCode != SUCCESS)
        {
            goto _exit_EndProfile;
        } /* Endif */
    }
    else if (rtCode != SUCCESS)
    {
        goto _exit_EndProfile;
    } /* Endif */

    if (m_media->getMapper()->isBlockUnallocated(u32PhysicalBlockNumber))
    {
        rtCode = ERROR_DDI_NAND_MAPPER_GET_BLOCK_FAILED;
        goto _exit_EndProfile;
    } /* Endif */
    
    /* Get the sector map for this virtual block. */
    sectorMap = m_media->getNssmManager()->getMapForVirtualBlock(u32LogicalBlockAddress);
    if (!sectorMap)
    {
        rtCode = ERROR_DDI_NAND_DATA_DRIVE_CANT_ALLOCATE_USECTORS_MAPS;
        goto _exit_EndProfile;
    } /* Endif */

    /* 
    ** Figure out what physical sector offset the logical sector 
    ** needs to be mapped to. 
    */
    u32PhysicalSectorOffset = sectorMap->getNextOffset();
    
    if (bNeedCheckLO)
    {
        bIsLogicalOrder = sectorMap->isInLogicalOrder();
    } /* Endif */

    /* Now we need to determine if the block is full or can fit another sector in. */
    if (u32PhysicalSectorOffset >= u32SectorsPerBlock)
    {
        /* 
        ** Allocate a new physical block.  Assign new physical block to
        ** logical block.  Make note of old physical block number.
        ** When new physical block is full, when non-sequential map 
        ** is evicted or during system shutdown, new physical block 
        ** and old physical block will be combined. 
        */
        rtCode = sectorMap->preventThrashing(&u32PhysicalBlockNumber, u32LogicalSectorOffset);
        if (rtCode != SUCCESS)
        {
            goto _exit_EndProfile;
        } /* Endif */

        /* 
        ** We need to call this again since we may have gotten 
        ** a new physical block. 
        */
        u32PhysicalSectorOffset = sectorMap->getNextOffset();
        
        if (bNeedCheckLO)
        {
            bIsLogicalOrder = sectorMap->isInLogicalOrder();
        } /* Endif */
        
        assert(u32PhysicalSectorOffset < u32SectorsPerBlock);
    } /* Endif */

    /* 
    ** Now get the region that corresponds to the Physical Block. This function call also 
    ** converts the absolute physical block number into a chip-relative block number. 
    */
    pRegion = m_media->getRegionForBlock(u32PhysicalBlockNumber);
    assert(pRegion->isDataRegion());

    /* Compute the physical sector number from the block and offset. */
    u32PhysicalSectorNumber = pRegion->m_nand->blockAndOffsetToRelativePage(u32PhysicalBlockNumber, u32PhysicalSectorOffset);

    /* Get a buffer to hold the redundant area. */
    rtCode = auxBuffer.acquire();
    if (rtCode != SUCCESS)
    {
        goto _exit_EndProfile;
    } /* Endif */

    /* 
    ** Initialize the redundant area. 
    ** Up until now, we have ignored u32LogicalSectorOffset. 
    ** We write logical sector offset into redundant area so 
    ** that NSSM may be reconstructed from physical block. 
    */
    md.setBuffer(auxBuffer);
    md.prepare(u32LogicalBlockAddress, u32LogicalSectorOffset);
    
    /* If this drive is a hidden data drive, then we need to set the RA flag indicating so. */
    if (m_Type == kDriveTypeHidden)
    {
        /* Clear the flag bit to set it. All metadata flags are set when the bit is 0. */
        md.setFlag(Metadata::kIsHiddenBlockFlag);
    } /* Endif */
    /* The pages of this block are written in logical order, we set kIsInLogicalOrderFlag  */
    if(bIsLogicalOrder)
    {
        md.setFlag(Metadata::kIsInLogicalOrderFlag);
    } /* Endif */
    
    WL_LOG(WL_MODULE_LOGICALDRIVE, WL_LOG_DEBUG, "datadrive write Lp %d, Pp %d\n",
    u32LogicalSectorNumber , u32PhysicalSectorNumber);

    /* Write the sector */
    while (1)
    {
        bool bIsLogicalOrderNew;
        rtCode = pRegion->m_nand->writePage(u32PhysicalSectorNumber, pSectorData, auxBuffer);

        if (SUCCESS == rtCode)
        {
            break;
        }
        else if (rtCode == ERROR_DDI_NAND_HAL_WRITE_FAILED)
        {
            WL_LOG(WL_MODULE_LOGICALDRIVE, WL_LOG_INFO, "*** Write failed: new bad block %u! ***\n", u32PhysicalBlockNumber);
            
            /* 
            ** Try to recover by copying data into a new block. We must skip the logical sector 
            ** that we were going to write. 
            */
            rtCode = sectorMap->recoverBadLbaBlock(
                            u32PhysicalBlockNumber,
                            &u32PhysicalBlockNumber,    /* new absolute physical block number returned here */
                            u32LogicalSectorOffset);
            if (rtCode != SUCCESS)
            {
                /* Error exit */
                goto _exit_EndProfile;
            } /* Endif */
            
            /* The new physical block number is absolute so convert it to relative. */
            pRegion = m_media->getRegionForBlock(u32PhysicalBlockNumber);
            assert(pRegion->isDataRegion());

            /* 
            ** Get the new physical sector offset to write at. When the block contents 
            ** were relocated, they were written in sequential order, 
            ** skipping any duplicate entries. So the next 
            ** unoccupied physical sector in the new block is not necessarily 
            ** the same physical sector that we were going to write to in the old block. 
            */
            u32PhysicalSectorOffset = sectorMap->getNextOffset();
            
            if (bNeedCheckLO)
            {
                bIsLogicalOrderNew = sectorMap->isInLogicalOrder();
            } /* Endif */

            /* Metadata::kIsInLogicalOrderFlag is not set at the first time */
            if ((bIsLogicalOrderNew == true) && (bIsLogicalOrder == false))
            {
                md.setFlag(Metadata::kIsInLogicalOrderFlag);
            } 
            /* Metadata::kIsInLogicalOrderFlag is set at the fisrt time, we need clear it now. */
            else if ((bIsLogicalOrderNew == false) && (bIsLogicalOrder == true))
            {
                md.clearFlag(Metadata::kIsInLogicalOrderFlag);
            } /* Endif */

            /* 
            ** We have a new physical block now, and so the physical sector number 
            ** (which is the sector number from the origin of the current NAND chip-enable) 
            ** has changed.  We must recompute it now. 
            */
            u32PhysicalSectorNumber = pRegion->m_nand->blockAndOffsetToPage(u32PhysicalBlockNumber, u32PhysicalSectorOffset);
        } /* if ERROR_DDI_NAND_HAL_WRITE_FAILED */
        else /* other errors */
        {
            /* Error exit */
            goto _exit_EndProfile;
        } /* Endif */
    } /* while (1) { WriteSector } */

    /* 
    ** Add the page to the NonSequential Sector map. 
    ** The mapping is from logical sector offset to physical sector offset. 
    */
    sectorMap->addEntry(u32LogicalSectorOffset, u32PhysicalSectorOffset);

    _exit_EndProfile:

    return rtCode;
} /* Endbody */

/* EOF */
