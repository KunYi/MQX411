#if !defined(__nonsequentialsectorsmap_h__)
#define __nonsequentialsectorsmap_h__
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
*   This file contains definition of the NSSM class and NSSM manager.
*
*
*END************************************************************************/
#include "wl_common.h"
#include "ddi_block.h"
#include "ddi_page.h"
#include "page_order_map.h"
#include "deferred_task.h"
#include "red_black_tree.h"
#include "wlru.h"
#include "ddi_nand_ddi.h"

/* 
** Class definations 
*/

namespace nand {

#if defined(NO_SDRAM)
/* Set the default number of maps for the data drive. Maps are allocated dynamically. */
#define NUM_OF_MAX_SIZE_NS_SECTORS_MAPS             9
#elif defined(BSP_TWR_VF65GS10_M4)
/* Set the default number of maps for the data drive. Maps are allocated dynamically. */
#define NUM_OF_MAX_SIZE_NS_SECTORS_MAPS             64
#else
#define NUM_OF_MAX_SIZE_NS_SECTORS_MAPS             128
#endif

#define NS_SECTORS_MAP_MAX_NUM_SECTORS              (0x100)

/* 
** Status of last page of block. 
**
** These status constants are used to track the state of the last page in the block
** when reading it to determine whether pages are in sorted logical order. 
*/
enum
{
    /* Haven't read the last page yet. */
    kNssmLastPageNotHandled = 0,
    
    /* The last page was erased. */
    kNssmLastPageErased = 1,
    
    /* The last page contains valid data. */
    kNssmLastPageOccupied = 2
};

class Region;
class Mapper;
class NssmManager;

/*
** Map of logical to physical sector order.
**
** The nonsequential sectors map (NSSM) is responsible for tracking the physical location within
** a block of that block's logical sectors. It also manages the mechanism for updating block
** contents in an efficient manner. All data drive sector reads and writes must utilize a
** nonsequential sectors map in order to find the physical location of a logical sector, or
** to get the page where a new sector should be written.
**
** The NSSM is composed of two key components. First, it has a map of logical sector to physical
** page within the block. This allows logical sectors to be written to in any order to the block,
** which is important in ensuring that pages are only written sequentially within the block as
** required by NANDs. The map also enables logical sectors to be written to the block more than
** once, with the most recent copy taking precedence.
**
** The second element is a backup block. This backup block contains the previous contents of
** the block, and allows only new sectors to be written to the primary block. If a logical sector
** is not present in the primary block it can be read from the backup block. When the primary
** block becomes full, the primary and backup are merged into a new block. Merging takes the most
** recent version of each logical sector from either the primary or backup and writes it into
** the new block.
**
** Another important aspect of the NSSM is that each NSSM is associated with a virtual block
** number, not a physical block. This allows the data associated with the virtual block to
** move around on the media as necessary.
*/
class NonsequentialSectorsMap : public RedBlackTree::Node, public WeightedLRUList::Node
{
public:

    enum _nssm_constants
    {
        /* 
        ** Value used to indicate that no block is set for either the virtual block 
        ** or backup physical block. 
        */
        kInvalidAddress = 0xffffffff
    };
    
    /* Default constuctor. */
    /* Make sure to call init() after construction. */
    NonsequentialSectorsMap();
    
    /* Destructor. */
    virtual ~NonsequentialSectorsMap();
        
    void init(NssmManager * manager, uint16_t mapIndex);
        
    void prepareForBlock(uint32_t blockNumber);
    
    RtStatus_t resolveConflict(uint32_t blockNumber, uint32_t physicalBlock1, uint32_t physicalBlock2);
    
    RtStatus_t flush();
    
    /* Returns the absolute physical block number for this map's virtua block. */
    RtStatus_t getPhysicalBlock(uint32_t * physicalBlock);    
    
    void invalidate();
    
    /* Returns whether this map has a valid association with a virtual block. */
    inline bool isValid() const { return m_virtualBlock != kInvalidAddress; }
    
    /* Whether the virtual block has a backup block. */
    inline bool hasBackup() const { return m_backupPhysicalBlock != kInvalidAddress; }
    
    Region * getRegion();
    
    /* Returns the physical page offset within the primary block for the next page to be written. */
    inline unsigned getNextOffset() { return m_currentPageCount; }
    
    /* Determines whether the pages of the block are in logical order. */
    bool isInLogicalOrder();
    
    /* Merge primary and backup blocks without skipping any pages. */
    RtStatus_t mergeBlocks() { return mergeBlocksSkippingPage(kInvalidAddress); }
    
    /* Merge primary and backup blocks, but exclude a given logical sector offset. */
    RtStatus_t mergeBlocksSkippingPage(uint32_t u32NewSectorNumber);
    
    RtStatus_t preventThrashing(uint32_t * pu32PhysicalBlkAddr, uint32_t u32NewSectorNumber);
    
    RtStatus_t recoverBadLbaBlock(uint32_t u32OldBlkAddr, uint32_t *pu32NewBlkAddr, uint32_t u32SectorToSkip);
	RtStatus_t relocateVirtualBlock(uint32_t physicalBlockNumber, uint32_t * newPhysicalBlockNumber);

    
    /* name Entries */
    RtStatus_t getEntry(uint32_t u32LBASectorIdx,
    uint32_t * pu32PhysicalBlockNumber,
    uint32_t * pu32NS_ActualSectorOffset,
    uint32_t * pu32Occupied);
    
    void addEntry(uint32_t u32LBASectorIdx, uint32_t u32ActualSectorIdx);
    
    void insertToLRU();
    void removeFromLRU();
    
    /* Determines if the node is valid. */
    virtual bool isNodeValid() const { return isValid(); }
    
    /* Returns the node's weight value. */
    virtual int getWeight() const { return 0; }
    
    /* Red-Black tree methods */
    virtual RedBlackTree::Key_t getKey() const;
    
    /* Related objects */
    NssmManager * getManager() { return m_manager; }
    inline Media * getMedia();
    inline Mapper * getMapper();

protected:

    /* Copy pages filter class for data drive blocks. */
    class CopyPagesFlagFilter : public NandCopyPagesFilter
    {
    public:
        /* Filter method. */
        virtual RtStatus_t filter(
        NandPhysicalMedia * fromNand,
        NandPhysicalMedia * toNand,
        uint32_t fromPage,
        uint32_t toPage,
        SECTOR_BUFFER * sectorBuffer,
        SECTOR_BUFFER * auxBuffer,
        bool * didModifyPage);
    };

    RtStatus_t buildFromMetadata();
    RtStatus_t buildMapFromMetadata(PageOrderMap & map, uint32_t physicalBlock, uint32_t * filledSectorCount);
    
    RtStatus_t mergeBlocksCore(uint32_t u32NewSectorNumber, uint32_t u32NewBlockNum);
    RtStatus_t shortCircuitMerge();
    RtStatus_t quickMerge();
    
    RtStatus_t getNewBlock(uint32_t u32LbaBlockNumber, uint32_t * pu32PhysicalBlkAddr, uint32_t u32NumSectorsPerBlock);
    
    RtStatus_t sortAndCopyLbaBlock(
        uint32_t u32OldBlkAddr,
        uint32_t u32NewBlkAddr,
        uint32_t u32SkipSectorStart,
        uint32_t u32NumSectorsToSkip);

    RtStatus_t copyToNewBlock(
        uint32_t physicalBlockNumber, 
        uint32_t * newPhysicalBlockNumber, 
        uint32_t skipSectorStart, uint32_t 
        skipSectorCount);

    RtStatus_t copyPages(
        NandPhysicalMedia * sourceNand,
        NandPhysicalMedia * targetNand,
        uint32_t u32SrcBlkAddr, 
        uint32_t u32SrcSectorIdx, 
        uint32_t u32DestBlkAddr, 
        uint32_t u32DestSectorIdx, 
        uint32_t u32NumSectorsToCopy);

protected:

    /* Manager object that owns me. */
    NssmManager * m_manager;
    
    /* Map for the primary physical data block. */
    PageOrderMap m_map;
    
    /* Map for the backup (original) physical block. */
    PageOrderMap m_backupMap;
    
    /* Which virtual block this map goes with, or #kInvalidAddress. */
    uint32_t m_virtualBlock;
    
    /* Physical block containing old copies of sectors. */
    BlockAddress m_backupPhysicalBlock;
    
    /* 
    ** The number of actual Sectors that have been written to the Block.  
    ** They are written sequentially, so this is also the index for
    ** the next Sector write
    */
    int32_t m_currentPageCount;
    
    /* This map's index in the NSSM array. */
    uint16_t m_index;

    friend class NssmManager;    
};

/*
** Manages the array of nonsequential sector maps.
**
** An array of nonsequential sector maps are shared by all data drive regions, to hold
** a mapping of the order in which sectors have been written to open block splits.
*/
class NssmManager
{
public:
    /* Statistics about map usage. */
    struct Statistics
    {
        /* 
        ** Number of times an NSSM entry had to be built by reading metadata from the NAND.
        ** This happens when an NSSM entry is not found already in the NSSM list. Therefore,
        ** the count of "builds" is synonymous with the count of "misses". 
        */
        uint32_t buildCount;      
        
        /* Number of blocks whose pages are written in logical order */
        uint32_t orderedBuildCount;

        /* 
        ** Merges
        **
        ** Number of times two blocks were merged. There are three different algorithms possible,
        ** and each instance is counted separately. 
        */
        /* Normal merge between old and new blocks into a newly allocated third block. */
        uint32_t mergeCountCore;
        
        /* Old block is simply discarded. */
        uint32_t mergeCountShortCircuit;
        
        /* Old block is merged into new block in-place, without allocating a third block. */
        uint32_t mergeCountQuick;
        
        uint32_t lruHits;
    };
    
    /* Constructor. */
    NssmManager(Media * nandMedia);
    
    /* Destructor. */
    /* Frees memory allocated for the NSSM array. */
    ~NssmManager();
    
    /* Allocates or reallocates the array of NSSMs. */
    RtStatus_t allocate(unsigned uMapsPerBaseNSSMs);
    
    /* Returns the size of the NSSM array in terms of the base block size. */
    unsigned getBaseNssmCount();
    
    /* Flush and invalidate */
    void flushAll();
    void invalidateAll();
    void invalidateDrive(LogicalDrive * pDriveDescriptor);
    
    NonsequentialSectorsMap * getMapForIndex(unsigned index) { return &m_mapsArray[index]; }
    NonsequentialSectorsMap * getMapForVirtualBlock(uint32_t blockNumber);
    
    RtStatus_t buildMap(uint32_t u32LBABlkAddr, NonsequentialSectorsMap ** resultMap);
    
    Statistics & getStatistics() { return m_statistics; }
    
    Media * getMedia() { return m_media; }
    Mapper * getMapper() { return m_mapper; }
    unsigned getNSSMapCount() { return m_mapCount; }

protected:
    /* The NAND media object. */
    Media * m_media;
    
    /* The virtual to logical mapper object. */
    Mapper * m_mapper;
    
    /* Number of descriptors pointed to by the m_mapsArray field. */
    unsigned m_mapCount;
    
    /* 
    ** Pointer to the shared array of non-sequential sectors map objects. 
    ** All data-type drives use this same array. 
    */
    NonsequentialSectorsMap * m_mapsArray;
    
    /* Index of the maps. */
    RedBlackTree m_index;
    
    /* LRU for the maps. */
    WeightedLRUList m_lru;
    
    /* Statistics about map usage. */
    Statistics m_statistics;
    
    friend class NonsequentialSectorsMap;
};

/*
** Task to move a virtual block to a new physical block.
**
** This task is used to copy the contents of a virtual block to a new physical block when
** the data drive read sector method sees the bit error level has reached a threshold.
*/
class RelocateVirtualBlockTask : public DeferredTask
{
public:
    
    /* Constants for the block update task. */
    enum _task_constants
    {
        /* Unique ID for the type of this task. */
        kTaskTypeID = (('b'<<24)|('l'<<16)|('k'<<8)|('r')), /* 'blkr' */
        
        /* Priority for this task type. */
        kTaskPriority = 15
    };

    /* Constructor. */
    RelocateVirtualBlockTask(NssmManager * manager, uint32_t virtualBlockToRelocate);
    
    /* Return a unique ID for this task type. */
    virtual uint32_t getTaskTypeID() const;
    
    /* Check for preexisting duplicate tasks in the queue. */
    virtual bool examineOne(DeferredTask * task);

    /* Return the logical block that needs to be refreshed. */
    uint32_t getVirtualBlock() const { return m_virtualBlock; }

protected:

    /* The manager for the virtual block we're working with. */
    NssmManager * m_manager;
    
    /* Virtual block number whose contents will be relocated to a new physical block. */
    uint32_t m_virtualBlock;

    /* The relocate task implementation. */
    virtual void task();
    
};

/* Implemented down here after NssmManager is fully declared. */
inline Media * NonsequentialSectorsMap::getMedia()
{
    return m_manager->getMedia();
}

/* Implemented down here after NssmManager is fully declared. */
inline Mapper * NonsequentialSectorsMap::getMapper()
{
    return m_manager->getMapper();
}

} /* namespace nand */

#endif /* __nonsequentialsectorsmap_h__ */

/* EOF */
