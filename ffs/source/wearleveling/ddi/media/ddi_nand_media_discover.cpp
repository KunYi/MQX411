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
*   This file discovers the allocated drives on the media
*
*
*END************************************************************************/

#include "wl_common.h"
#include "ddi_media_internal.h"
#include "ddi/common/ddi_nand_ddi.h"
#include "hal/ddi_nand_hal.h"
#include "ddi/media/ddi_nand_media.h"
#include "ddi/data_drive/ddi_nand_data_drive.h"
#include "buffer_manager/media_buffer.h"
#include <stdlib.h>

using namespace nand;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : create
* Returned Value   : Region pointer
* Comments         :
*   This function creates region
*
*END*--------------------------------------------------------------------*/
Region * Region::create
(   
    /* [IN] Information of region */
    NandConfigBlockRegionInfo_t * info
)
{ /* Body */
    Region * newRegion;
    
    switch (info->eDriveType)
    {
        case kDriveTypeData:
        case kDriveTypeHidden:
            newRegion = new DataRegion;
            break;
            
        case kDriveTypeSystem:
            newRegion = new SystemRegion;
            break;
            
        case kDriveTypeUnknown:
            /* Check if this region has the special boot region tag value. */
            if (info->wTag == NandConfigBlockRegionInfo_t::kBootRegionTag)
            {
                newRegion = new BootRegion;
            }
            /* else, fall through... */
            
        default:
            /* Don't know what to do with this drive type! */
            assert(0);
            // return NULL;
    }; /* Endswitch */
    
    /* Now, init the new region. */
    assert(newRegion);
    newRegion->initFromConfigBlock(info);
    return newRegion;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : initFromConfigBlock
* Returned Value   : MQX_OK or an error code
* Comments         :
*   This function fills in the region's information from the contents of a region info
*   structure that is part of the config block.
*
*END*--------------------------------------------------------------------*/
void Region::initFromConfigBlock
(
    /* [IN] Information of region */
    NandConfigBlockRegionInfo_t * info
)
{ /* Body */
    m_iChip = info->iChip;
    m_nand = NandHal::getNand(m_iChip);

    m_pLogicalDrive = NULL;
    m_eDriveType = info->eDriveType;

    m_wTag = info->wTag;
    m_iNumBlks = info->iNumBlks;
    m_u32NumLBlks = 0;

    /* Setup iStartPhysAddr */
    m_iStartPhysAddr = info->iStartBlock;

    /* Track the absolute (over all the media chips) block number. */
    m_u32AbPhyStartBlkAddr = m_nand->baseAbsoluteBlock() + m_iStartPhysAddr;

    /* Setup bRegionInfoDirty flag to false */
    m_bRegionInfoDirty = false;
    
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : createDrives
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function will create LogicalDrive instances for each of the drives
*   described by the set of regions loaded from the config block(s).
*
*END*--------------------------------------------------------------------*/
RtStatus_t Media::createDrives() {
    bool didFindDataDrive = false;
    Region *regionInfo;
    RtStatus_t status;

    /* We have to cross all regions to account for all drives */
    Region::Iterator it = createRegionIterator();
    regionInfo = it.getNext();
    while (regionInfo) {
        /* Check Drive Type */
        switch (regionInfo->m_eDriveType)
        {
            case kDriveTypeData:
            {
                /* Drive Type is Data Drive */

                /* 
                ** The 1st time we find a kDriveTypeData, we fill up all the
                ** LogicalDrive_t for this drive.
                ** Subsequently, we will only add to the size-related parameters. 
                */
                if (!didFindDataDrive)
                {
                    DataDrive * newDataDrive = new DataDrive(this, regionInfo);
                    if (!newDataDrive)
                    {
                        return ERROR_OUT_OF_MEMORY;
                    } /* Endif */

                    /* Add our new data drive. */
                    status = DriveAdd(newDataDrive);
                    if (status != SUCCESS)
                    {
                        return status;
                    } /* Endif */

                    didFindDataDrive = true;
                }
                else
                {
                    /* Add to drive size only */
                    LogicalDrive * genericDrive = DriveGetDriveFromTag(regionInfo->m_wTag);
                    assert(genericDrive->m_u32Tag == regionInfo->m_wTag && genericDrive->m_Type == kDriveTypeData);
                    DataDrive * dataDrive = static_cast<DataDrive *>(genericDrive);

                    if (dataDrive)
                    {
                        dataDrive->addRegion(regionInfo);
                    } /* Endif */
                } /* Endif */

                break;
            }
        } /* Endswitch */
        regionInfo = it.getNext();
    } /* Endwhile */

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : addNewBadBlock
* Returned Value   : void
* Comments         :
*   This function adds new bad block to array
*
*END*--------------------------------------------------------------------*/
void DataRegion::addNewBadBlock
(
    /* [IN] Bad block address */
    const BlockAddress & addr
) 
{ /* Body */
    /* m_bbt.markBad(addr.get()); */
    ++m_badBlockCount;
    setDirty();
} /* Endbody */

/* EOF */
