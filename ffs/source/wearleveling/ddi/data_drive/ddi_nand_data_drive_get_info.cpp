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
*   This file contains a function to get certain information about the data drive
*
*
*END************************************************************************/
#include "wl_common.h"
#include "ddi_nand_ddi.h"
#include "ddi_nand_data_drive.h"
#include "mapper.h"

using namespace nand;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : _taskq_test
* Returned Value   : MQX_OK or an error code
* Comments         :
*   This function tests all task queues for correctness
*
*END*--------------------------------------------------------------------*/
RtStatus_t DataDrive::getInfo(uint32_t selector, void * pInfo)
{ /* Body */
    uint32_t u32SectorsPerBlock = NandHal::getParameters().wPagesPerBlock;
    uint32_t temp;

    if (!m_bInitialized)
    {
        return ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED;
    } /* Endif */

    switch (selector)
    {
        case kDriveInfoSizeInSectors:
            temp = m_u32NumberOfSectors;
            temp /= u32SectorsPerBlock;
            /* Prepare enouhg amount of reserved blocks for backup */
            temp -= (m_media->getNssmManager()->getNSSMapCount() + MAPPER_COMPUTE_RESERVE(NandHal::getFirstNand()->wTotalBlocks));
            temp *= u32SectorsPerBlock;
            *((uint64_t *)pInfo) = temp;
            break;
            
        /* Always TRUE */
        case kDriveInfoMediaPresent:
            *((bool *)pInfo) = true;
            break;

        /* Always FALSE - Can't change. */
        case kDriveInfoMediaChange:
            *((bool *)pInfo) = false;
            break;
            
        /* Return the number of non-sequential sector maps allocated for this drive. */
        case kDriveInfoNSSMCount:
            *(uint32_t *)pInfo = g_nandMedia->getNssmManager()->getBaseNssmCount();
            break;
        
        default:
            return LogicalDrive::getInfo(selector, pInfo);
    } /* Endswitch */

    return SUCCESS;
} /* Endbody */

/* EOF */
