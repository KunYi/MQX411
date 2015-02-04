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
*   This file contains the utilities needed to handle LBA ddi layer functions.
*
*
*END************************************************************************/
#include "wl_common.h"
#include "ddi/common/ddi_nand_ddi.h"
#include "ddi/data_drive/ddi_nand_data_drive.h"
#include "ddi_nand_hal.h"
#include <string.h>
#include "ddi/mapper/mapper.h"
#include "buffer_manager/media_buffer.h"

using namespace nand;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getMapForVirtualBlock
* Returned Value   : The map of the requested virtual block number. 
*   If NULL is returned, then an unrecoverable error occurred.
* Comments         :
*   Get the appropriate Non-Sequential Sector Map.
*   This function will return the NonSequential Sector map entry for the given
*   LBA Block.  If the NSSM is not in the table, then build it from the data 
*   in the NAND.
*
*END*--------------------------------------------------------------------*/
NonsequentialSectorsMap * NssmManager::getMapForVirtualBlock
(
    /* [IN] Virtual block number to search for. */
    uint32_t blockNumber
)
{ /* Body */
    RtStatus_t  ret;

    /* Use the index to search for a matching map. */
    NonsequentialSectorsMap * map = static_cast<NonsequentialSectorsMap *>(m_index.find(blockNumber));
    if (map)
    {
        /* Reinsert the map in LRU order. */
        map->removeFromLRU();
        map->insertToLRU();
        
        return map;
    } /* Endif */

    /* If it wasn't found, we'll need to build it. */
    ret = buildMap(blockNumber, &map);
    if (ret != SUCCESS)
    {
        /* Something bad happened... */
        return NULL;
    } /* Endif */

    /* Insert the newly built map into the LRU list. */
    map->insertToLRU();
    return map;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getEntry
* Returned Value   : Status of call or error.
* Comments         :
*   This function gets the physical sector in the remapped LBA corresponding
*   to a given linear sector offset.  Basically this function returns the
*   byte array entry of the linear sector offset which is the remapped
*   sector.
*
*END*--------------------------------------------------------------------*/
RtStatus_t NonsequentialSectorsMap::getEntry
(
    /* [IN] Logical sector index */
    uint32_t u32LBASectorIdx,
    
    /* [OUT] Physical block number */
    uint32_t * pu32PhysicalBlockNumber,
    
    /* [OUT] Sector offset of the actual sector. */
    uint32_t * pu32NS_ActualSectorOffset,
    
    /* [OUT] State of the sector */
    uint32_t * pu32Occupied
)
{ /* Body */
    uint32_t u32SectorOffset;

    /* 
    ** The LBA was found above so now using the linear expected LBA sector,
    ** grab the value in the NonSequential Map Sector. 
    */
    u32SectorOffset = m_map[u32LBASectorIdx];
    *pu32Occupied = m_map.isOccupied(u32LBASectorIdx);

    /* See if We have a back-up block. */
    if (!(*pu32Occupied) && (m_backupPhysicalBlock != kInvalidAddress))
    {
        /* 
        ** If we have a back-up block, return back-up block's physical block 
        ** number and physical offset into back-up block. 
        */
        u32SectorOffset = m_backupMap[u32LBASectorIdx];
        *pu32Occupied = m_backupMap.isOccupied(u32LBASectorIdx);
        
        *pu32PhysicalBlockNumber = m_backupPhysicalBlock;
    } /* Endif */
    
    *pu32NS_ActualSectorOffset = u32SectorOffset;

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : insertToLRU
* Returned Value   : void
* Comments         :
*   This function inserts the nonsequentialsectormap object to list
*
*END*--------------------------------------------------------------------*/
void NonsequentialSectorsMap::insertToLRU()
{ /* Body */
    m_manager->m_lru.insert(this);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : removeFromLRU
* Returned Value   : void
* Comments         :
*   This function removes a nonsequentialsectormap object from list
*
*END*--------------------------------------------------------------------*/
void NonsequentialSectorsMap::removeFromLRU()
{ /* Body */
    m_manager->m_lru.remove(this);
} /* Endbody */

/* EOF */
