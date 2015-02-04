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
*   This file allocates the drives on the media
*
*
*END************************************************************************/
#include "wl_common.h"
#include <string.h>
#include <stdlib.h>
#include "ddi_media_internal.h"
#include "ddi/common/ddi_nand_ddi.h"
#include "ddi/mapper/mapper.h"
#include "ddi/media/ddi_nand_media.h"
#include "ddi_nand_hal.h"
#include "buffer_manager/media_buffer.h"

using namespace nand;

namespace nand {

#define MINIMUM_DATA_DRIVE_SIZE 8

} /* namespace nand */

extern nand::NandZipConfigBlockInfo_t g_nandZipConfigBlockInfo;


/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : allocateFromPreDefineRegions
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function allocates drives from pre-define structure
*
*END*--------------------------------------------------------------------*/
RtStatus_t Media::allocateFromPreDefineRegions()
{
    Region * newRegion;
    int32_t i;
    
    if (m_bInitialized == false)
    {
        return ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED;
    } /* Endif */

    /* Lock the NAND for our purposes. */
    DdiNandLocker locker;

    /* If this assert hits, we're overflowing the regions map.  Investigate whether adding more regions is necessary. */
    assert(g_nandZipConfigBlockInfo.iNumEntries < MAX_NAND_REGIONS);

    for (i = 0; i < g_nandZipConfigBlockInfo.iNumEntries; i++)
    {
        if (g_nandZipConfigBlockInfo.Regions[i].eDriveType == kDriveTypeData)
        {
            /* Create a region object of the required type. */
            newRegion = Region::create(&g_nandZipConfigBlockInfo.Regions[i]);
            assert(newRegion);

            /* Add the region into the region array. */
            assert(m_iNumRegions < MAX_NAND_REGIONS);
            m_pRegionInfo[m_iNumRegions++] = newRegion;
        }        
    }

    if (!m_iNumRegions)
    {
        // m_eState = kMediaStateUnknown;
        return ERROR_DDI_NAND_LMEDIA_NO_REGIONS_IN_MEDIA;
    }

    return createDrives();
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : allocateFromPreDefine
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function allocates drives from pre-define structure
*
*END*--------------------------------------------------------------------*/
RtStatus_t Media::allocateFromPreDefine
(   
/* [IN] Media allocation table */
MediaAllocationTable_t * pTable
) 
{ /* Body */
    bool bDataDriveFound = false;
    NandConfigBlockRegionInfo_t zipRegionInfo;
    uint32_t mediaCount = NUM_LOGICAL_MEDIA;
    uint32_t mediaCounter;
    
    if (m_bInitialized == false)
    {
        return ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED;
    } /* Endif */

    for (mediaCounter = 0; mediaCounter < mediaCount; mediaCounter++) 
    {
        for (uint32_t i = 0; i < pTable[mediaCounter].u32NumEntries; i++) 
        {
            if (pTable[mediaCounter].Entry[i].Type == kDriveTypeData) 
            {
                bDataDriveFound = true;
                break;
            } /* Endif */
        }
        if (bDataDriveFound) 
        {
            break;
        } /* Endif */
    } /* Endfor */
    
    if (bDataDriveFound == true) 
    {
        NandParameters_t param = NandHal::getParameters();
        uint32_t blockSize = param.pageTotalSize * param.wPagesPerBlock;

        for (mediaCounter = 0 ; mediaCounter < mediaCount ; mediaCounter++) 
        {
            zipRegionInfo.iChip = 0;
            zipRegionInfo.iNumBlks = 0;
            for (uint32_t i = 0; i < pTable[mediaCounter].u32NumEntries; i++) 
            {
                zipRegionInfo.eDriveType = pTable[mediaCounter].Entry[i].Type;
                zipRegionInfo.wTag = pTable[mediaCounter].Entry[i].u32Tag;
                zipRegionInfo.iStartBlock = 0;
                if (pTable[mediaCounter].Entry[i].u64SizeInBytes % blockSize == 0) {
                    zipRegionInfo.iNumBlks = zipRegionInfo.iNumBlks + 
                    pTable[mediaCounter].Entry[i].u64SizeInBytes / blockSize;
                } 
                else 
                {
                    zipRegionInfo.iNumBlks = zipRegionInfo.iNumBlks + 
                    pTable[mediaCounter].Entry[i].u64SizeInBytes / blockSize + 1;
                } /* Endif */
                /* Create a region object of the required type. */
                Region * newRegion = Region::create(&zipRegionInfo);
                assert(newRegion);
                
                /* Add the region into the region array. */
                assert(m_iNumRegions < MAX_NAND_REGIONS);
                m_pRegionInfo[m_iNumRegions++] = newRegion;
            } /* Endfor */
        } /* Endfor */
        return createDrives();
    } /* Endif */
    
    return !SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : findNextRegionInChip
* Returned Value   : Region Number if exists, else returns -1.
* Comments         :
*   This function finds the allocated region that immediately follows the */
/* block number passed in as an argument.
*
*END*--------------------------------------------------------------------*/
int Media::findNextRegionInChip
(
/* [IN] Chip Number */
int iChip, 

/* [IN] Any block number allocated in the  preceeding region. */
int iBlock,

/* 
    ** [OUT] Pointer to NandZipConfigBlockInfo_t structure. 
    ** This structure has all the regions information for the entire media. 
    */
NandZipConfigBlockInfo_t * pNandZipConfigBlockInfo
)
{ /* Body */
    int i;
    int iRegion = -1, iUpperLimitBlock = 0x7fffffff;

    for(i = 0 ; i < pNandZipConfigBlockInfo->iNumEntries ; i++)
    {
        if(pNandZipConfigBlockInfo->Regions[i].iChip == iChip)
        {
            if(pNandZipConfigBlockInfo->Regions[i].iStartBlock > iBlock)
            {
                /* This Region is a good candidate */
                if(pNandZipConfigBlockInfo->Regions[i].iStartBlock < iUpperLimitBlock)
                {
                    iUpperLimitBlock = pNandZipConfigBlockInfo->Regions[i].iStartBlock;
                    iRegion = i;
                } /* Endif */
            } /* EndIf */
        } /* Endif */
    } /* Endfor */

    return(iRegion);
} /* Endbody */

/* EOF */
