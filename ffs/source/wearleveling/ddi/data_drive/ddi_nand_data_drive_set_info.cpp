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
*   This file contains a function to set certain information about the data drive.
*
*
*END************************************************************************/

#include "wl_common.h"
#include "ddi/common/ddi_nand_ddi.h"
#include "ddi/data_drive/ddi_nand_data_drive.h"

using namespace nand;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setInfo
* Returned Value   : SUCCESS or ERROR
*   - ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED Drive is not initialised.
*   - ERROR_DDI_LDL_LDRIVE_INVALID_INFO_TYPE Cannot modify the requested data field.
*   - SUCCESS Data was set successfully.
* Comments         :
*   Set specified information about the data drive 
*
*   Only a small subset of drive info selectors can be modified. Attempting
*   to set a selector that cannot be changed will result in an error.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DataDrive::setInfo
(
    /* [IN] Type of info requested: Tag, Component Version, Project Version, etc. */
    uint32_t Type, 
    
    /* [IN] Pointer to data to set. */
    const void * pInfo
)
{ /* Body */
    if (Type != kDriveInfoSectorSizeInBytes && !m_bInitialized)
    {
        return ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED;
    } /* Endif */
    
    switch (Type)
    {
        /* Change the number of non-sequential sector maps allocated for this drive. */
        case kDriveInfoNSSMCount:
            {
                uint32_t newCount;
                RtStatus_t result;
                
                DdiNandLocker locker;
                
                newCount = *(uint32_t *)pInfo;
                result = m_media->getNssmManager()->allocate(newCount);
                
                return result;
            }
            
        default:
            return LogicalDrive::setInfo(Type, pInfo);
    } /* Endswitch */
} /* Endbody */

/* EOF */
