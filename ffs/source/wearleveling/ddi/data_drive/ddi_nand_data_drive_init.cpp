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
*   This file handles the initialization of the data drive
*
*
*END************************************************************************/

#include "wl_common.h"
#include "ddi/common/ddi_nand_ddi.h"
#include "ddi/data_drive/ddi_nand_data_drive.h"
#include "ddi_media.h"
#include "ddi/mapper/mapper.h"
#include "ddi_nand_hal.h"
#include <stdlib.h>

using namespace nand;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DataDrive
* Returned Value   : N/A
* Comments         :
*   Constructor
*
*END*--------------------------------------------------------------------*/
DataDrive::DataDrive
(
    /* [IN] Media pointer */
    Media * media, 
    
    /* [IN] Region pointer */
    Region * region
)
:   LogicalDrive(),
m_media(media),
m_u32NumRegions(0),
m_ppRegion(NULL)
{ /* Body */
    m_bInitialized = false;
    m_bPresent = true;
    m_bErased = false;
    m_bWriteProtected = false;
    m_Type = region->m_eDriveType;
    m_u32Tag = region->m_wTag;
    m_logicalMedia = media;

    NandParameters_t & params = NandHal::getParameters();
    m_u32SectorSizeInBytes = params.pageDataSize;
    m_nativeSectorSizeInBytes = m_u32SectorSizeInBytes;
    m_nativeSectorShift = 0;

    m_u32EraseSizeInBytes = params.pageDataSize * params.wPagesPerBlock;
    
    addRegion(region);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : ~DataDrive
* Returned Value   : N/A
* Comments         :
*   Destructor
*
*END*--------------------------------------------------------------------*/
DataDrive::~DataDrive()
{ /* Body */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : addRegion
* Returned Value   : void
* Comments         :
*   This function add a region to DataDrive object
*
*END*--------------------------------------------------------------------*/
void DataDrive::addRegion
(
    /* [IN] A new region */
    Region * region
)
{ /* Body */
    m_u32NumberOfSectors += (region->m_iNumBlks - region->getBadBlockCount())    /* Number of Good Blocks */
    * (NandHal::getParameters().wPagesPerBlock);
    m_numberOfNativeSectors = m_u32NumberOfSectors;

    m_u64SizeInBytes = ((uint64_t)m_u32NumberOfSectors * m_u32SectorSizeInBytes);
    
    region->m_pLogicalDrive = this;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : _taskq_test
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS If no error has occurred.
*   - ERROR_DDI_LDL_LDRIVE_MEDIA_NOT_ALLOCATED
*   - ERROR_DDI_LDL_LDRIVE_LOW_LEVEL_MEDIA_FORMAT_REQUIRED
*   - ERROR_DDI_LDL_LDRIVE_WRITE_PROTECTED
*   - ERROR_DDI_LDL_LDRIVE_HARDWARE_FAILURE
* Comments         :
*   This function will initialize the Data drive which includes the following:
*   Initialize the Mapper interface if available.
*   - Fill in an array of Region structures for this Data Drive.
*   - Reconstruct Physical start address for each region.
*   - Allocate non-sequential sectors maps (NSSM) for the drive.
*
*   NANDDataDriveInit() sets up data structures used by the Data Drive routines.
*
*   Some data structures are expected to already be partly or wholey
*   set up by Media initialization routines
*
*END*--------------------------------------------------------------------*/
RtStatus_t DataDrive::init()
{ /* Body */
    RtStatus_t ret;
    
    DdiNandLocker locker;
    
    if (!m_bPresent)
    {
        return ERROR_DDI_LDL_LDRIVE_MEDIA_NOT_ALLOCATED;
    } /* Endif */
    
    /* If we've already been initialized, just return SUCCESS. */
    if (m_bInitialized)
    {
        return SUCCESS;
    } /* Endif */
    
    /* Build private list of Data Drive Regions */
    buildRegionsList();
    
    /* Partition NonSequential SectorsMaps memory. */
    ret = m_media->getNssmManager()->allocate(NUM_OF_MAX_SIZE_NS_SECTORS_MAPS);
    if (ret != SUCCESS)
    {
        return ret;
    } /* Endif */

    /* 
    ** The last thing we must do is initialize the mapper. This comes last because it uses
    ** the region structures and the NSSM. 
    */
    ret = m_media->getMapper()->init();
    if (ret)
    {
        return ret;
    } /* Endif */
    
    m_bInitialized = true;
    
    return SUCCESS;
} /* Endbody */

/*
** Scans for regions belonging to this drive.
** The drive's type and tag must have already been filled in when this is called. 
*/
void DataDrive::processRegions(Region ** regionArray, unsigned * regionCount)
{ /* Body */
    unsigned dataRegionsCount = 0;
    Region::Iterator it = m_media->createRegionIterator();
    Region * theRegion;
    theRegion = it.getNext();
    while (theRegion)
    {
        if (m_Type == theRegion->m_eDriveType && m_u32Tag == theRegion->m_wTag)
        {
            if (regionArray)
            {
                regionArray[dataRegionsCount] = theRegion;
            } /* Endif */

            dataRegionsCount++;
        } /* Endif */
        theRegion = it.getNext();
    } /* Endwhile */
    
    if (regionCount)
    {
        *regionCount = dataRegionsCount;
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : buildRegionsList
* Returned Value   : void
* Comments         :
*   This function will build the list of data regions based upon the
*   number of regions.
*
*END*--------------------------------------------------------------------*/
void DataDrive::buildRegionsList()
{ /* Body */
    /* Scan once to get the number of data regions for this drive. */
    unsigned dataRegionsCount = 0;
    processRegions(NULL, &dataRegionsCount);
    
    /* Allocate an array of region pointers large enough to hold all of our regions. */
    m_u32NumRegions = dataRegionsCount;
    m_ppRegion = new Region*[dataRegionsCount];
    assert(m_ppRegion);
    
    /* Scan again to fill in the region pointer array. */
    processRegions(m_ppRegion, NULL);
    
    Region * pRegion;
    int iNumSectorsPerBlk = NandHal::getParameters().wPagesPerBlock;
    uint32_t u32TotalLogicalSectors = 0; /* Logical "native" sectors */
    Region::Iterator it(m_ppRegion, m_u32NumRegions);
    pRegion = it.getNext();
    while (pRegion)
    {
        /* 
        ** As far as the mapper is concerned, all these blocks can be allocated
        ** However, some of these blocks could go bad so... 
        */
        pRegion->m_u32NumLBlks = pRegion->m_iNumBlks - pRegion->getBadBlockCount();
        
        u32TotalLogicalSectors += (pRegion->m_u32NumLBlks * iNumSectorsPerBlk);
        
        pRegion = it.getNext();
    } /* Endwhile */
    
    /* Subtract out the reserved blocks but only for the Data Drive which is large. */
    if (m_Type == kDriveTypeData)
    {
        u32TotalLogicalSectors -= m_media->getReservedBlockCount() * iNumSectorsPerBlk;
        
        /* Also subtract out the number of blocks reserved for maps by the mapper. */
        u32TotalLogicalSectors -= kNandMapperReservedBlockCount * iNumSectorsPerBlk;
    } /* Endif */
    
    /* 
    ** Update the native sector count and recompute the total drive size using the
    ** total logical sector count. 
    */
    m_numberOfNativeSectors = u32TotalLogicalSectors;
    m_u64SizeInBytes = (uint64_t)u32TotalLogicalSectors * (uint64_t)m_nativeSectorSizeInBytes;
    
    /* Convert native to nominal sectors. */
    m_u32NumberOfSectors = m_numberOfNativeSectors << m_nativeSectorShift;
} /* Endbody */

/* EOF */
