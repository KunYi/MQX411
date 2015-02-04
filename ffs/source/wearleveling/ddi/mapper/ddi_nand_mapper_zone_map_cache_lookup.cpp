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
*   This file contains NAND mapper zone map cache implementation
*
*
*END************************************************************************/

#include "ddi/mapper/zone_map_cache.h"
#include "ddi/common/ddi_nand_ddi.h"
#include "ddi/data_drive/ddi_nand_data_drive.h"
#include "ddi_nand_hal.h"
#include "ddi_media.h"
#include <string.h>
#include "buffer_manager/media_buffer.h"
#include "simple_timer.h"

using namespace nand;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : lookupCacheEntry
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function identifies the cache entry which is to be used with logical block.
*
*   This function either identifies the cache entry which contains zone-map
*   section containing u32Lba or identifies the cache entry which should be
*   loaded with zone-map section containing u32Lba.
*
*END*--------------------------------------------------------------------*/
RtStatus_t ZoneMapCache::lookupCacheEntry
(
    /* [IN] Logical Block Address. */
    uint32_t u32Lba, 
    
    /* [OUT] Entry for zone-map section containing u32Lba */
    int32_t * pi32SelectedEntryNum
)
{ /* Body */
    uint64_t u32MinTimeStamp;
    uint64_t u32SaveMinTimeStamp;
    uint32_t  i;
    uint32_t u32StartingEntry;
    uint32_t u32NumEntries;
    uint32_t u32TimeStamp;
    uint32_t bCacheValid;
    
    /* was -1 causes mem corruption when counter flips. 0 ok. */
    *pi32SelectedEntryNum = 0; 
    
    /* First see if there exists a cache entry which already  contains u32Lba. */
    for (i=0; i < m_cacheSectionCount; ++i)
    {
        u32StartingEntry = m_descriptors[i].m_firstLBA;
        u32NumEntries    = m_descriptors[i].m_entryCount;
        bCacheValid      = m_descriptors[i].m_isValid;
        
        if ((bCacheValid) && 
                (u32Lba >= u32StartingEntry) && 
                (u32Lba < (u32StartingEntry + u32NumEntries)))
        {
            *pi32SelectedEntryNum = i;
            return SUCCESS;
        } /* Endif */
    } /* Endfor */
    u32MinTimeStamp = wl_hw_get_microsecond();
    u32SaveMinTimeStamp = u32MinTimeStamp;
    
    /* 
    ** If there is an entry which has not been occupied yet, used it.  
    ** Otherwise, pick the entry which has the earliest time-stamp (i.e LRU).  
    */
    for (i=0; i < m_cacheSectionCount; ++i)
    {
        u32TimeStamp = m_descriptors[i].m_timestamp;
        bCacheValid = m_descriptors[i].m_isValid;
        
        /* 
        ** The entries have to be aged.  Otherwise, an early
        ** flurry of accesses will insure that a cache entry
        ** will stay in Cache forever. 
        */
        
        if (!bCacheValid)
        {
            *pi32SelectedEntryNum = i;
            return SUCCESS;   
        }
        else if (u32TimeStamp < u32MinTimeStamp)
        {
            u32MinTimeStamp = u32TimeStamp;
            *pi32SelectedEntryNum = i;  
        } /* Endif */
    } /* Endfor */

    /* 
    ** This happens once in a blue moon.  The microsecond timer has rolled over, and the "current"
    ** time is less than all of the time-stamps.  In this case, we'll just return with original
    ** value of pi32SelectedEntryNum, which is 0.  But we have to fix the time-stamps since they're
    ** likely to be greater than micro-second timer for the foreseeable future.  So, refresh all of
    ** the time-stamps. 
    */
    if (u32MinTimeStamp == u32SaveMinTimeStamp)
    {        
        for (i=0; i < m_cacheSectionCount; ++i)
        {
            m_descriptors[i].m_timestamp = u32MinTimeStamp;
        } /* Endfor */
        
    } /* Endif */
    
    assert(!(((*pi32SelectedEntryNum) < 0) || ((uint32_t)(*pi32SelectedEntryNum) >= m_cacheSectionCount)));
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : evictAndLoad
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function evicts zone-map section currently contained in cache entry
*   i32SelectedEntry and loads new zone-map section which contains logical
*   block u32Lba
*
*END*--------------------------------------------------------------------*/
RtStatus_t ZoneMapCache::evictAndLoad
(
    /* [IN] Logical Block Address. */
    uint32_t u32Lba, 
    
    /* [IN] Entry for zone-map section. */
    int32_t i32SelectedEntry
)
{ /* Body */
    uint32_t bCacheValid;
    uint32_t u32StartingEntry;
    uint32_t u32NumEntries;
    RtStatus_t ret;
    CacheEntry & entry = m_descriptors[i32SelectedEntry];
    
    bCacheValid = entry.m_isValid;
    u32StartingEntry = entry.m_firstLBA;
    u32NumEntries = entry.m_entryCount;
    
    if (!bCacheValid)
    {
        ret = loadCacheEntry(u32Lba, i32SelectedEntry);
        if (ret)
        {
            return ret;
        } /* Endif */
    }
    else if ((u32Lba < u32StartingEntry) || (u32Lba >= (u32StartingEntry + u32NumEntries)))
    {
        /* 
        ** Logical address was not found in cache, we need to evict an entry
        ** and read in the section which contains the logical address.  
        */
        if (entry.m_isDirty)
        {
            ret = addSection(entry.m_entries, entry.m_firstLBA, entry.m_entryCount);
            if (ret)
            {
                return ret;
            } /* Endif */
        } /* Endif */
        
        ret = loadCacheEntry(u32Lba, i32SelectedEntry);
        if (ret)
        {
            return ret;
        } /* Endif */
    } /* Endif */
    
    return SUCCESS;
} /* Endbody */

/* EOF */
