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
*   This file contains functions used to manipulate and manage NAND regions
*
*
*END************************************************************************/

#include "wl_common.h"
#include "ddi/common/ddi_nand_ddi.h"
#include <string.h>
#include "ddi/media/ddi_nand_media.h"

using namespace nand;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getRegionForBlock
* Returned Value   : A pointer to the region holding the physicalBlock is returned.
* Comments         :
*   This function  will return the region pointer parameter passed into it
*   by checking the physical sector parameter against the region boundaries.
*
*END*--------------------------------------------------------------------*/
Region * Media::getRegionForBlock
(
    /* [IN] Absolute physical block to match. */
    const BlockAddress & physicalBlock
)
{ /* Body */
    uint32_t u32AbsoluteOffset;
    int32_t i32NumBlocksInRegion;
    Region * pRegionInfo;  
    
    /* Search through all the regions */
    Region::Iterator it = createRegionIterator();
    pRegionInfo = it.getNext();
    while (pRegionInfo)
    {
        u32AbsoluteOffset = pRegionInfo->m_u32AbPhyStartBlkAddr;
        i32NumBlocksInRegion = pRegionInfo->m_iNumBlks;
        
        /* 
        ** We don't need to check for greater than start block address
        ** because we're scanning sequentially. 
        */
        if ((physicalBlock < (u32AbsoluteOffset + i32NumBlocksInRegion))
                && (physicalBlock >= u32AbsoluteOffset))
        {
            /* Return a pointer to the region found */
            return pRegionInfo;
        } /* Endif */
        pRegionInfo = it.getNext();
    } /* Endwhile */
    
    return NULL;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setDirty
* Returned Value   : void
* Comments         :
*   This function marks region as dirty
*
*END*--------------------------------------------------------------------*/
void Region::setDirty()
{ /* Body */
    /* Figure out how and when to clear region dirty flag, or if it's even necessary. */
    m_bRegionInfoDirty = true;
} /* Endbody */

/* EOF */
