#if !defined(__phymap_h__)
#define __phymap_h__
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
*   This file contains defination of the nand::PhyMap class.
*
*
*END************************************************************************/

#include "sectordef.h"

namespace nand
{

/*
** A bitmap of the occupied blocks on the NANDs.
**
** The phymap, or physical map, is a bitmap of all blocks on all NAND chip enables. The main
** purpose of the phymap is to enable efficient searching for available blocks when writing new
** data to a drive, or when relocating data from another block. Each block in the map can be
** marked either free or used. A free block is erased and is not allocated to any purpose.
**
** If a block is marked as used, then it may actually be in one of several states,
** but the point is that it is not available for use to hold new data. Used blocks may
** contain valid data for any one of the drives, including system drives. They may be boot
** blocks or other blocks used by the NAND driver for its own purposes. Finally, all bad blocks
** are marked as used.
*/
class PhyMap
{
public:
    /* Constants for phymap entries. */
    enum _entry_constants
    {
        kEntrySizeInBytes = sizeof(uint32_t),
        kBlocksPerEntry = 32,
        
        /* An entry with a value of 0 means that all blocks are occupied. */
        kFullEntry = 0  
    };
    
    /* Constants to use for marking blocks in the phymap. */
    enum _free_or_used
    {
        /* The block is free and available for use. */
        kFree = true,  
        
        /* The block either contains valid data or is bad. */
        kUsed = false    
    };
    
    /* Constants for the auto-erase parameter of markBlock(). */
    enum _auto_erase
    {
        /* When marking a block free, automatically erase the block if it's not already erased. */
        kAutoErase = true, 

        /* Never erase the block when marking it free. */
        kDontAutoErase = false  
    };
    
    /* Callback used to signal changes to the dirty state. */
    typedef void (*DirtyCallback_t)(PhyMap * thePhymap, bool wasDirty, bool isDirty, void * refCon);
    
    /* Computes the number of entries required to hold a given number of blocks. */
    static uint32_t getEntryCountForBlockCount(uint32_t blockCount) { return ROUND_UP_DIV(blockCount, kBlocksPerEntry); }
    
    /* Init and cleanup */
    /* Initializer. */
    RtStatus_t init(uint32_t totalBlockCount);
    
    /* Destructor. */
    ~PhyMap();
    
    /* Marking entries */
    /* Set all entries to one state. */
    void markAll(bool isFree);
    
    /* Mark a block as either free or used. */
    RtStatus_t markBlock(uint32_t absoluteBlock, bool isFree, bool doAutoErase=kDontAutoErase);
    
    /* Mark a block as free. */
    inline RtStatus_t markBlockFree(uint32_t absoluteBlock) { return markBlock(absoluteBlock, kFree); }
    
    /* Mark a block as free and perform the auto-erase function. */
    inline RtStatus_t markBlockFreeAndErase(uint32_t absoluteBlock) { return markBlock(absoluteBlock, kFree, kAutoErase); }
    
    /* Mark a block as used. */
    inline RtStatus_t markBlockUsed(uint32_t absoluteBlock) { return markBlock(absoluteBlock, kUsed); }
    
    /* Mark a range of blocks as either free or used. */
    RtStatus_t markRange(uint32_t absoluteStartBlock, uint32_t blockCount, bool isFree, bool doAutoErase=kDontAutoErase);
    
    /* Counts */
    /* Returns the total number of blocks. */
    inline uint32_t getBlockCount() const { return m_blockCount; }
    
    /* Returns the total number of entries. */
    inline uint32_t getEntryCount() const { return m_entryCount; }
    
    /* Computes the number of free blocks. */
    uint32_t getFreeCount() const;
    
    /* Getting block state */
    /* Returns the state of one block. */
    bool isBlockFree(uint32_t absoluteBlock) const;
    
    /* Returns true if the block is marked as used. */
    inline bool isBlockUsed(uint32_t absoluteBlock) const { return !isBlockFree(absoluteBlock); }
    
    /* Direct entry access */
    /* Returns a pointer to the entire map array. */
    inline uint32_t * getAllEntries() { return m_entries; }
    
    /* Read/write entry access operator. */
    inline uint32_t & operator [] (uint32_t entryIndex) { return m_entries[entryIndex]; }
    
    /* Gives up ownership of the map array. */
    void relinquishEntries();
    
    /* Dirty flag */
    /* Returns true if the map is dirty. */
    inline bool isDirty() const { return m_isDirty; }
    
    /* Sets the dirty flag. */
    void setDirty();
    
    /* Clears the dirty flag. */
    void clearDirty();
    
    /* Sets the dirty change callback. */
    inline void setDirtyCallback(DirtyCallback_t callback, void * refCon) { m_dirtyListener = callback; m_dirtyRefCon = refCon; }

protected:
    /* Total number of blocks represented in the map. */
    uint32_t m_blockCount;  
    
    /* Number of phymap entries. */
    uint32_t m_entryCount;  
    
    /* Phymap entry array. */
    uint32_t * m_entries;   
    
    /* Whether the phymap has been modified recently. */
    bool m_isDirty; 

    /* Callback function to invoke when the dirty state changes. */
    DirtyCallback_t m_dirtyListener; 

    /* Arbitrary value passed to dirty listener. */
    void * m_dirtyRefCon;    
};

} /* namespace nand */

#endif /* __phymap_h__ */

/* EOF */
