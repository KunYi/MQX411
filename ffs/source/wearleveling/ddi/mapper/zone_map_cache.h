#if !defined(__zone_map_cache_h__)
#define __zone_map_cache_h__
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
*   This file contains declaration of the virtual to physical map class
*
*
*END************************************************************************/

#include "persistent_map.h"

namespace nand {

class Mapper;

/* 
** Set this macro to 1 to use 24-bit zone map entries regardless of the
** total number of blocks in the NAND configuration. 
*/
#define NAND_MAPPER_FORCE_24BIT_ZONE_MAP_ENTRIES 0

/* Zone map cache */
/* The number of cache entries varies depending on whether we have an SDRAM or no-SDRAM build. */
#if defined(NO_SDRAM)
/* Number of cached zone map sections. */
#define MAPPER_CACHE_COUNT          (1)
#else
/* Number of cached zone map sections. */
#define MAPPER_CACHE_COUNT          (2)
#endif

/* Zone map entry size constants */
/* 16-bit entry. */
const unsigned kNandZoneMapSmallEntry = 2; 

/* 24-bit entry. */
const unsigned kNandZoneMapLargeEntry = 3;  

/* Maximum number of blocks to use the small zone map entries for. */
const unsigned kNandZoneMapSmallEntryMaxBlockCount = 32768;

/* 16-bit unallocated block value. */
const unsigned kNandMapperSmallUnallocatedBlockAddress = 0xffff;   

/* 24-bit unallocated block value. */
const unsigned kNandMapperLargeUnallocatedBlockAddress = 0xffffff; 

/* Map of virtual to physical block numbers. */
class ZoneMapCache : public PersistentMap
{
public:
    /* Constructor. */
    ZoneMapCache(Mapper & mapper);
    
    /* Destructor. */
    virtual ~ZoneMapCache();

    void init();
    void shutdown();

    RtStatus_t writeEmptyMap();

    RtStatus_t findZoneMap();

    RtStatus_t flush();
    
    RtStatus_t getBlockInfo(uint32_t u32Lba, uint32_t *pu32PhysAddr);
    RtStatus_t setBlockInfo(uint32_t u32Lba, uint32_t u32PhysAddr);
    
    virtual RtStatus_t consolidate(
    bool hasValidSectionData,
    uint32_t sectionNumber,
    uint8_t * sectionData,
    uint32_t sectionDataEntryCount);

    // TODO: set&get of trust element 
    inline void setTrustFlag(bool trustFlag) {
        m_trustFlag = trustFlag;
    }
    
    inline bool getTrustFlag() {
        return m_trustFlag;
    }

    inline void setTrustNumber(uint16_t trustNumber) {
        m_trustNumber = trustNumber;
    }

    inline uint16_t getTrustNumber() {
        return m_trustNumber;
    }
    
protected:

    
    /* Information about a cached section of the zone map. */
    struct CacheEntry
    {
        /* True if this entry is valid. */
        bool m_isValid;         
        
        /* True if this entry is dirty. */
        bool m_isDirty;         
        
        /* The modification timestamp for this entry in microseconds since boot. */
        uint64_t m_timestamp;   
        
        /* LBA number for the first entry in this section. */
        uint32_t m_firstLBA;    
        
        /* Number of valid entries in this section. */
        uint32_t m_entryCount;  
        
        /* Pointer to section entry data. */
        uint8_t * m_entries;    
    };
    
    /* Number of cached zone map sections. */
    uint32_t m_cacheSectionCount; 
    
    /* Information about each of the cached sections. This array is #sectionCount entries long. */
    CacheEntry * m_descriptors;  
    
    /* Dynamically allocated buffer for the cached zone map sections. */
    uint8_t * m_cacheBuffers;  
    bool m_wroteCacheEntryDuringConsolidate;

    RtStatus_t loadCacheEntry(uint32_t u32Lba, int32_t i32SelectedEntry);
    RtStatus_t lookupCacheEntry(uint32_t u32Lba, int32_t * pi32SelectedEntryNum);
    RtStatus_t evictAndLoad(uint32_t u32Lba, int32_t i32SelectedEntry);

    uint32_t readMapEntry(CacheEntry * zoneMapSection, uint32_t lba);
    void writeMapEntry(CacheEntry * zoneMapSection, uint32_t lba, uint32_t physicalAddress);

    virtual RtStatus_t getSectionForConsolidate(
    uint32_t u32EntryNum,
    uint32_t thisSectionNumber,
    uint8_t *& bufferToWrite,
    uint32_t & bufferEntryCount,
    uint8_t * sectorBuffer);
    
};

} /* namespace nand */

#endif /* __zone_map_cache_h__ */

/* EOF */
