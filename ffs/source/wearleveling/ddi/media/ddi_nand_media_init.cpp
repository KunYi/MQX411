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
*   This file initializes the NAND Media
*
*
*END************************************************************************/

#include "wl_common.h"
#include <string.h>
#include "ddi_media_internal.h"
#include "ddi/media/ddi_nand_media.h"
#include "ddi/common/ddi_nand_ddi.h"
#include "hal/ddi_nand_hal.h"
#include "ddi/data_drive/ddi_nand_data_drive.h"
#include "ddi/mapper/mapper.h"
#include "auto_free.h"
#include "deferred_task.h"
#include "deferred_task_queue.h"
#include "ddi/data_drive/nonsequential_sectors_map.h"

using namespace nand;

/* 
** Global external declarations 
*/

/* Global information about the NAND media. */
Media * g_nandMedia = NULL;

/* 
** Function prototype 
*/

extern "C" LogicalMedia * nand_media_factory(const MediaDefinition_t & def);
uint32_t Nand_SetNandBootBlockSearchNumberAndWindow( uint32_t u32NandBootBlockSearchNumber );

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : nand_media_factory
* Returned Value   : Logical media
* Comments         :
*   This function creates factory of nand media
*
*END*--------------------------------------------------------------------*/
LogicalMedia * nand_media_factory
(
    /* [IN] Media defination */
    const MediaDefinition_t & def
)
{ /* Body */
    LogicalMedia * media = new Media();
    media->m_u32MediaNumber = def.m_mediaNumber;
    media->m_isRemovable = def.m_isRemovable;
    media->m_PhysicalType = def.m_mediaType;
    
    return media;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : N/A
* Returned Value   : N/A
* Comments         :
*   Constructor
*
*END*--------------------------------------------------------------------*/
Media::Media()
:   LogicalMedia(),
    m_params(NULL),
    m_nssmManager(NULL),
    m_mapper(NULL),
    m_deferredTasks(NULL),
    m_iNumRegions(0),
    m_pRegionInfo(NULL),
    m_iTotalBlksInMedia(0),
    m_iNumBadBlks(0),
    m_iNumReservedBlocks(0),
    m_badBlockTable()
    /*,
    m_badBlockTableMode(kNandBadBlockTableInvalid),
    m_bootBlockSearchNumber(0),
    m_bootBlockSearchWindow(0)*/
{
    /*_wl_mem_set(m_ConfigBlkAddr, 0, sizeof(m_ConfigBlkAddr));*/
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : N/A
* Returned Value   : N/A
* Comments         :
*   Destructor
*
*END*--------------------------------------------------------------------*/
Media::~Media()
{
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : init
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function is responsible for initializing the pieces of the following descriptors:
*   - #LogicalMedia_t
*   - #NandMediaInfo_t
*   - #NandPhysicalMedia[] via NandHalInit
*   - #NandParameters_t via NandHalInit
*
*   This routine also initializes the NAND Hardware via NandHalInit.
*
*END*--------------------------------------------------------------------*/
RtStatus_t Media::init()
{ /* Body */
    RtStatus_t Status;
    
    /* Set global NAND media object. */
    g_nandMedia = this;

    /* Initialize our synchronization objects.     */
    if (!SimpleMutex::InitMutex(&g_NANDThreadSafeMutex))
    {
        return MQX_INVALID_HANDLE;
    } /* Endif */
    
    /* Init the HAL library. */
    Status = NandHal::init();
    if (Status != SUCCESS)
    {
        return Status;
    } /* Endif */
    
    /* Save parameters pointer. */
    m_params = NandHal::getFirstNand()->pNANDParams;

    m_iNumRegions = 0;
    m_pRegionInfo = new Region*[MAX_NAND_REGIONS];
    assert(m_pRegionInfo);
    
    _wl_mem_set(m_pRegionInfo, 0, sizeof(Region*) * MAX_NAND_REGIONS);
    
    m_iTotalBlksInMedia = NandHal::getTotalBlockCount();
/*    m_badBlockTableMode = kNandBadBlockTableInvalid;*/

    /* 
    ** Init boot block addresses (9 of them) so that their state is unknown. 
    ** The bfBlockProblem field of the block address structure has a value of
    ** kNandBootBlockUnknown when all bits are set. 
    */
    /*_wl_mem_set(&m_bootBlocks, 0xff, sizeof(m_bootBlocks));
    m_bootBlocks.m_isNCBAddressValid = false;*/

    /* Initialize the LogicalMedia fields */
    m_PhysicalType = kMediaTypeNand;
    m_bWriteProtected = false;
    m_bInitialized = true;
    m_u32AllocationUnitSizeInBytes = m_params->pageDataSize;
    m_eState = kMediaStateUnknown;
    m_u64SizeInBytes = (uint64_t)(m_iTotalBlksInMedia << m_params->pageToBlockShift) * (uint64_t)m_u32AllocationUnitSizeInBytes;
    
    /* Create the deferred task queue. */
    m_deferredTasks = new DeferredTaskQueue;
    assert(m_deferredTasks);
    Status = m_deferredTasks->init();
    if (Status != SUCCESS)
    {
        return Status;
    }
    
    /* Create the mapper instance. It won't be inited until the first data drive is inited. */
    m_mapper = new Mapper(this);
    assert(m_mapper);
    
    /* Create the NSSM manager. */
    m_nssmManager = new NssmManager(this);
    assert(m_nssmManager);

    /* Create the BadBlockTable instance. After that, scan Nand flash device to build BBT */
    m_badBlockTable = new BadBlockTable();
    assert(m_badBlockTable);
    if (!m_badBlockTable->isAllocated())
    {
        m_badBlockTable->allocate(NandHal::getTotalBlockCount());
        m_badBlockTable->scanAndBuildBBT();
    }
    m_iNumBadBlks = m_badBlockTable->getNumberBadBlocks();
    
#if DEBUG
    /* Print out the NAND device name. */
    auto_free<char> name = (auto_free<char>)NandHal::getFirstNand()->getDeviceName();
    if (name)
    {
        WL_LOG(~0, "NAND: %s\n", name.get());
    }
#endif /* DEBUG */

    return SUCCESS;
} /* Endbody */


Region::Region()
:   m_regionNumber(0),
    m_iChip(0),
    m_nand(NULL),
    m_pLogicalDrive(NULL),
    m_eDriveType(kDriveTypeUnknown),
    m_wTag(0),
    m_iStartPhysAddr(0),
    m_iNumBlks(0),
    m_u32NumLBlks(0),
    m_u32AbPhyStartBlkAddr(0),
    m_bRegionInfoDirty(false)
{
}

SystemRegion::SystemRegion()
:   Region()
{
    
}

DataRegion::DataRegion()
:   Region(),
    m_badBlockCount(0)
{
}

/* EOF */
