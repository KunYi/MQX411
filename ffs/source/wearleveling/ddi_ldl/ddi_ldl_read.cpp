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
*   This file contains hardware independent read functions.
*
*
*END************************************************************************/

#include "wl_common.h"
#include "ddi_media_internal.h"
#include "ddi_media.h"
#include "buffer_manager/media_buffer.h"

#if DEBUG
int32_t iCallsMediaRead = 0, iCallsMediaWrite = 0; 
uint64_t iMicrosecondsMediaRead = 0, iMicrosecondsMediaWrite = 0;
#endif /* DEBUG */

#if (defined(USE_NAND_STACK) && defined(NO_SDRAM))

#include "os/threadx/tx_api.h"

extern TX_MUTEX g_NANDThreadSafeMutex;

uint32_t g_u32NandStack[NAND_STACK_SIZE/4];
StackContext_t g_NewNandStackContext = { 0 , g_u32NandStack, 0, NAND_STACK_SIZE };
StackContext_t g_OldNandStackContext;

#endif /* USE_NAND_STACK && NO_SDRAM */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : DriveReadSector
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_TAG
*   - ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED
* Comments         :
*   Read a sector from the indicated drive.
*
*END*--------------------------------------------------------------------*/
RtStatus_t DriveReadSector
(
    /* [IN] Unique tag for the drive to operate on. */
    DriveTag_t tag, 

    /* [IN] Sector to read; 0-based at the start of the drive. */
    uint32_t u32SectorNumber, 

    /* [OUT] Pointer to buffer to fill with sector data. */
    SECTOR_BUFFER * pSectorData
)
{/* Body */
    /* Get drive depending on its tag */
    LogicalDrive * drive = DriveGetDriveFromTag(tag);
    
    if (!drive)
    {
        return ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_TAG;
    }
    else if (!drive->isInitialized())
    {
        return ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED;
    } /* Endif */

    return drive->readSector(u32SectorNumber, pSectorData);
} /* Endbody */

/* EOF */
