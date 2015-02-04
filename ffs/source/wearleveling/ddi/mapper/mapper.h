#ifndef __mapper_h__
#define __mapper_h__
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
*   This file contains NAND mapper API functions and other declarations.
*
*
*END************************************************************************/

#include "ddi_nand_ddi.h"
#include "phy_map.h"
//#include "ddi/mapper/bad_block_table.h"

/* 
** Class definations 
*/

namespace nand {

/*
** Macro to compute the number of blocks that must be set aside to handle future bad blocks.
** Note that these "reserved" blocks are different than the reserved block range used to
** hold only zone and phy map blocks.
*/
#define MAPPER_COMPUTE_RESERVE(u32TotalBlocks)  ((u32TotalBlocks)>>7)
#define MAPPER_MINIMUM_THRESHOLD                4

/* The maximum numbers of blocks that can be tracked by the mapper at once. */
#define MAPPER_MAX_TOTAL_NAND_BLOCKS            (1<<24)

/* 
** The number of blocks reserved to be used only for holding the zone and phy maps.
** This value must be at least large enough to hold both the zone and phy map,
** plus another copy of the zone map used during consolidation. 
*/
const unsigned kNandMapperReservedBlockCount = 8;

/* Enumeration to indicate what type of blocks to be obtained. */
typedef enum
{
    /*
    ** Normal data block.
    ** 
    ** These blocks are mapped and write-leveled. 
    */
    kMapperBlockTypeNormal,
    
    /*
    ** Map block type.
    **
    ** These blocks hold the virtual to physical mappings of normal blocks or other
    ** related information. 
    */
    kMapperBlockTypeMap,
    
    /* 
    ** Used when a block is desired in a given range of physical blocks.  
    ** We simply pass in start address of range however since ending address
    ** or size of range ends up being superfluous information 
    */
    kMapperBlockInRange 
} MapperBlockTypes_t;

/* Enumeration to indicate what type of maps. */
typedef enum
{
    kMapperZoneMap,
    kMapperPhyMap
} MapperMapTypes_t;

/* Constant used for setting block status in the phymap. */
const bool kNandMapperBlockUsed = PhyMap::kUsed;

/* Constant used for setting block status in the phymap. */
const bool kNandMapperBlockFree = PhyMap::kFree;

/* Forward declaration */
class ZoneMapCache;
class PersistentPhyMap;

/*
** The virtual to physical block mapper.
**
** This class is responsible for managing wear leveling of the data drive. It does this
** primaily through mapping virtual block numbers to physical block numbers. This allows
** the physical location on the media of a virtual block to change at any time. The mapper
** also maintains the list of unused blocks. It only works with blocks; pages are handled
** by the NonsequentialSectorsMap class.
*/
class Mapper 
{
public:

    /* Constructor. */
    Mapper(Media * media);
    
    /* Destructor. */
    ~Mapper();

    RtStatus_t init();
    RtStatus_t shutdown();
    bool isInitialized() const { return m_isInitialized; }

    RtStatus_t rebuild();
    RtStatus_t flush();

    /* Tests whether a block address matches the unallocated address. */
    bool isBlockUnallocated(uint32_t physicalBlockAddress) { return physicalBlockAddress == m_unallocatedBlockAddress; }

    RtStatus_t getBlockInfo(uint32_t u32Lba, uint32_t *pu32PhysAddr);
    RtStatus_t setBlockInfo(uint32_t u32Lba, uint32_t u32PhysAddr);

    RtStatus_t  getPageInfo(
    
    /* Logical page logical address */
    uint32_t u32PageLogicalAddr,
    
    /* Logical Block address  */
    uint32_t *pu32LogicalBlkAddr,
    
    /* Physical blk address */
    uint32_t *pu32PhysBlkAddr,
    uint32_t *pu32PhysPageOffset);

    RtStatus_t getBlock(uint32_t * pu32PhysBlkAddr, MapperBlockTypes_t eBlkType, uint32_t u32StartRange);
    RtStatus_t getBlockAndAssign(uint32_t u32Lba, uint32_t * pu32PhysBlkAddr, MapperBlockTypes_t eBlkType, uint32_t u32StartRange);
    RtStatus_t markBlock(uint32_t u32Lba, uint32_t u32PhysBlkAddr, bool isUnused);

    /* Store the given phymap for later use. */
    void setPrebuiltPhymap(PhyMap * theMap);

    /* Returns the current phymap object in use by the mapper. */
    ZoneMapCache * getZoneMap() { return m_zoneMap; }
    PhyMap * getPhymap() { return m_physMap; }
    Media * getMedia() { return m_media; }
    PersistentPhyMap * getPersistentPhyMap() { return m_phyMapOnMedia; }
    
    bool isBuildingMaps() const { return m_isBuildingMaps; }

    RtStatus_t findMapBlock(MapperMapTypes_t eMapType, uint32_t * pu32PhysBlkAddr);
    
    /* Processes a newly discovered bad block. */
    void handleNewBadBlock(const BlockAddress & badBlockAddress);

protected:
    /* The NAND logical media that we're mapping. */
    Media * m_media;
    
    /* Our zone map cache. */
    ZoneMapCache * m_zoneMap;
    
    /* Object to save and load the phymap on the NAND. */
    PersistentPhyMap * m_phyMapOnMedia;
    
    /* The physical block map array. */
    PhyMap * m_physMap;
    
    /* A phymap built during media erase. */
    PhyMap * m_prebuiltPhymap;  
    
    /* 
    ** Special value that represents an unallocated block, 
    ** i.e. a logical block that doesn't have a physical block assigned to it. 
    */
    uint32_t m_unallocatedBlockAddress;

    /* Status flags */
    /* True if the mapper has been initialized. */
    bool m_isInitialized;      
    
    /* This flag indicates that zone map has been created. */
    bool m_isZoneMapCreated;  

    /* This flag indicates that phys map has been created. */
    bool m_isPhysMapCreated;   
    
    /* This indicates that the map has been touched. */
    bool m_isMapDirty;         
    
    /* True if in the middle of createZoneMap(). */
    bool m_isBuildingMaps;
    
    /* 
    ** Reserved block range
    **
    ** The reserved block range is a range of blocks that is only allowed to hold
    ** the zone and phy maps. No normal data blocks are allowed to placed within
    ** the range. This is to ensure that there is always a block available when
    ** the maps need to be written to media. 
    */
    struct {
        /* Absolute physical block address for the first reserved block. */
        unsigned startBlock;   
        
        /* 
        ** Number of blocks in the reserved range including bad blocks. 
        ** So this value will be #kNandMapperReservedBlockCount plus the number of bad blocks. 
        */
        unsigned blockCount;   
        
        /* Phy map coarse entry number for the start block. */
        unsigned startPhyMapEntry;  
        
        /* Phy map coarse entry number for the last entry with a reserved block. */
        unsigned endPhyMapEntry;    
        
        /* Number of phy map entries that the reserved blocks occupy. */
        unsigned phyMapEntrySpan;   
        
        /* Offset of first reserved block in its phy map entry. */
        unsigned leadingOffset;     
        
        /* Offset of last reserved block in its phy map entry. */
        unsigned trailingOffset;   

        /* True if there are unreserved blocks leading reserved ones in the same phy map entry. */
        bool hasLeadingEdge;        
        
        /* True if there are unreserved blocks following reserved ones in the same phy map entry. */
        bool hasTrailingEdge;       
    } m_reserved;
    
    /* Block allocation indexes */
    /* Block to start seaching for a map block from. */
    uint32_t m_nextMapSearchStartBlock;   
    
    /* This indicate the starting search position in the array */
    uint32_t m_u32PhysMapCurrentIdxPosition; 

    /* This indicates the position within the 32 bits map */
    uint16_t m_u16PhysMapCurrentPos;    

protected:

    RtStatus_t computeReservedBlockRange(bool* pbRangeMoved);
    RtStatus_t evacuateReservedBlockRange();

    void setDirtyFlag();
    void clearDirtyFlag();

    static void phymapDirtyListener(PhyMap * thePhymap, bool wasDirty, bool isDirty, void * refCon);

    RtStatus_t createZoneMap();
    RtStatus_t scanAndBuildPhyMap(AuxiliaryBuffer & auxBuffer);
    RtStatus_t scanAndBuildZoneMap(AuxiliaryBuffer & auxBuffer);

    void searchAndDestroy();

    RtStatus_t allocateBlock(
    uint32_t * pu32PhyAddr,     
    MapperBlockTypes_t eBlkType,
    uint32_t u32StartRange);

    bool isBlockMapBlock(uint32_t u32PhysicalBlockNum, MapperMapTypes_t eMapType, RtStatus_t *pRtStatus);
    /* 
    ** calculateAllocationIndex function to replace a part of loadAllocationIndex
    ** This function is called to calculate PhysMapCurrentIdxPosition, after phymap is built  
    */
    void calculateAllocationIndex();
    /* 
    ** calculateNextMapSearchStartBlock function to replace a part of loadAllocationIndex
    ** This function is called to calculate NextMapSearchStartBlock, after phymap and zonemap is build 
    */
    void calculateNextMapSearchStartBlock();

    int searchPhyMapEntry(uint32_t entryBitField, int startIndex, int endIndex, bool * foundFreeBlock);
    
    uint32_t sufficientNumOfFreeBlocks();

};

} /* namespace nand */

/*
** Helper class to automatically clear a flag.
*/
class AutoClearFlag
{
public:
    /* Constructor takes the flag to be cleared. */
    AutoClearFlag(bool & theFlag) : m_flag(theFlag) {}

    /* Destructor clears the flag passed into the constructor. */
    ~AutoClearFlag()
{
    m_flag = false;
}

protected:
    /* Reference to the flag we are controlling. */
    bool & m_flag;  
};

#endif /* __mapper_h__ */

/* EOF */
