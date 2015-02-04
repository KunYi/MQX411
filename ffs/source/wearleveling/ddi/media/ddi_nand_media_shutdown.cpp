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
*   This file contains code to shutdown the NAND media layer.
*
*
*END************************************************************************/

#include "wl_common.h"
#include <string.h>
#include "ddi_media_internal.h"
#include "ddi/media/ddi_nand_media.h"
#include "ddi/common/ddi_nand_ddi.h"
#include "ddi/mapper/mapper.h"
#include "hal/ddi_nand_hal.h"
#include <stdlib.h>
#include "deferred_task.h"
#include "deferred_task_queue.h"
#include "ddi/data_drive/nonsequential_sectors_map.h"

using namespace nand;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : flushDrives
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function is responsible for flushing the drives on the nand media.
*
*END*--------------------------------------------------------------------*/
RtStatus_t Media::flushDrives()
{ /* Body */
    /* Flush NSSMs. */
    if (m_nssmManager)
    {
        m_nssmManager->flushAll();
    } /* Endif */
    
    /* Flush the mapper. */
    if (m_mapper && m_mapper->isInitialized())
    {
        m_mapper->flush();
    } /* Endif */
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : deleteRegions
* Returned Value   : void
* Comments         :
*   This function deletes all regions in media
*
*END*--------------------------------------------------------------------*/
void  Media::deleteRegions()
{ /* Body */
    /* Delete each of the valid Region objects. */
    unsigned i;
    for (i=0; i < m_iNumRegions; ++i)
    {
        if (m_pRegionInfo[i])
        {
            delete m_pRegionInfo[i];
            m_pRegionInfo[i] = NULL;
        } /* Endif */
    } /* Endfor */

    m_iNumRegions = 0;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : shutdown
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function is responsible for shutting down the NAND media. All memory
*   allocated by the NAND driver is freed. Even the NAND HAL is shutdown, which
*   in turn shuts down the GPMI driver.
*
*END*--------------------------------------------------------------------*/
RtStatus_t Media::shutdown()
{ /* Body */
    /* Don't shutdown if we aren't initialized yet. */
    if (!m_bInitialized)
    {
        return SUCCESS;
    } /* Endif */
    
    /* Flush everything. */
    flushDrives();
    
    /* First thing is to wait until all deferred tasks are finished. */
    m_deferredTasks->drain();
    
    /* Free the NSSM map memory. */
    if (m_nssmManager)
    {
        delete m_nssmManager;
        m_nssmManager = NULL;
    } /* Endif */
    
    /* Shut down the mapper. */
    if (m_mapper)
    {
        m_mapper->shutdown();
        delete m_mapper;
        m_mapper = NULL;
    } /* Endif */
    
    /* Shut down the deferred tasks. */
    if (m_deferredTasks)
    {
        delete m_deferredTasks;
        m_deferredTasks = NULL;
    } /* Endif */

    /* Clear Bad Block Table */
    if (m_badBlockTable)
    {
        delete m_badBlockTable;
        m_badBlockTable = NULL;
    }

    /* Zero out the LogicalMedia fields. */
    m_u64SizeInBytes = 0;
    m_PhysicalType = kMediaTypeNand;
    m_bWriteProtected = 0;
    m_bInitialized = 0;
    m_u32AllocationUnitSizeInBytes = 0;
    m_eState = kMediaStateUnknown;

    /* Free regions. */
    deleteRegions();

    delete [] m_pRegionInfo;
    m_pRegionInfo = NULL;
    
    /* Shut down the HAL library. */
    NandHal::shutdown();
    
    /* Destroy our synchronization objects. */
    SimpleMutex::DestroyMutex(&g_NANDThreadSafeMutex);

    return SUCCESS;
} /* Endbody */

/* EOF */
