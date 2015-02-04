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
*   This file contains implementation of the physical NAND block occupied status bitmap
*
*
*END************************************************************************/

#include "wl_common.h"
#include "ddi/mapper/phy_map.h"
#include "ddi/mapper/mapper.h"
#include "ddi/common/ddi_block.h"
#include <string.h>

using namespace nand;

/* For speed: */
/* Each array element contains the number of ones in its nibble sized index. */
const uint8_t kNumBitsHighForAnIndex[16] =
{
    /* index 0x0==0000 binary: 0 bits high. */
    0, 
    
    /* index 0x1==0001 binary: 1 bit  high. */
    1, 
    
    /* index 0x2==0010 binary: 1 bit  high. */
    1, 
    
    /* index 0x3==0011 binary: 2 bits high, so store a 2 here as num high bits for that index. */
    2, 
    
    /* index 0x4==0100 binary: 1 bit  high */
    1, 
    
    /* index 0x5==0101 binary: 2 bits high */
    2, 
    
    /* index 0x6==1010 binary: 2 bits high */
    2, 
    
    /* index 0x7==1011 binary: 3 bits high */
    3, 
    
    /* index 0x8==1000 binary: 1 bit  high */
    1, 
    
    /* index 0x9==1001 binary: 2 bits high */
    2, 
    
    /* index 0xA==1010 binary: 2 bits high */
    2, 
    
    /* index 0xB==1011 binary: 3 bits high */
    3, 
    
    /* index 0xC==1100 binary: 2 bits high */
    2,
    
    /* index 0xD==1101 binary: 3 bits high */
    3, 
    
    /* index 0xE==1110 binary: 3 bits high */
    3, 
    
    /* index 0xF==1111 binary: 4 bits high */
    4  
};

/* 
** Function prototypes 
*/

uint32_t count_ones_16(uint32_t);
uint32_t count_ones_32(uint32_t);

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : count_ones_16
* Returned Value   : Number of bits which are set in u16Value.
* Comments         :
*   This function counts the number of bits in given 16-bit word which are set.
*   It accomplishes this by table look-up of all constituent nibbles.
*
*END*--------------------------------------------------------------------*/
uint32_t count_ones_16
(
    /* [IN] The word whose set bits this function will count. */
    uint32_t u16Value
)
{ /* Body */
    uint32_t u32CountFirstNibble  = kNumBitsHighForAnIndex[ u16Value        & 0xF];
    uint32_t u32CountSecondNibble = kNumBitsHighForAnIndex[(u16Value >> 4)  & 0xF];
    uint32_t u32CountThirdNibble  = kNumBitsHighForAnIndex[(u16Value >> 8)  & 0xF];
    uint32_t u32CountFourthNibble = kNumBitsHighForAnIndex[(u16Value >> 12) & 0xF];

    return (u32CountFirstNibble + u32CountSecondNibble + u32CountThirdNibble + u32CountFourthNibble);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : count_ones_32
* Returned Value   : Number of bits which are set in value.
* Comments         :
*   This function counts the number of set bits in a 32-bit word.
*
*END*--------------------------------------------------------------------*/
uint32_t count_ones_32(uint32_t value)
{ /* Body */
    return count_ones_16(value & 0xffff) + count_ones_16(value >> 16);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : init
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function initializes a PhyMap object
*
*END*--------------------------------------------------------------------*/
RtStatus_t PhyMap::init
(
    /* [IN] Total number of blocks */
    uint32_t totalBlockCount
)
{
    /* Clear listener callback so we don't try to call it when marking all below. */
    m_dirtyListener = NULL;
    m_dirtyRefCon = 0;
    
    /* Save block count. */
    m_blockCount = totalBlockCount;
    
    /* Allocate an array large enough to have entries for every block. */
    m_entryCount = getEntryCountForBlockCount(totalBlockCount);
    m_entries = new uint32_t[m_entryCount];
    assert(m_entries);
    
    /* The entries start out all marked as used. */
    markAll(kUsed);

    /* Clear the dirty flag that was just set by markAll(). */
    clearDirty();
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : ~PhyMap
* Returned Value   : N/A
* Comments         :
*   Destructotr
*
*END*--------------------------------------------------------------------*/
PhyMap::~PhyMap()
{ /* Body */
    if (m_entries)
    {
        delete [] m_entries;
        m_entries = NULL;
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : relinquishEntries
* Returned Value   : void
* Comments         :
*   This function clear all entries
*
*END*--------------------------------------------------------------------*/
void PhyMap::relinquishEntries()
{ /* Body */
    m_entries = NULL;
    clearDirty();
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : markAll
* Returned Value   : void
* Comments         :
*   This function marks all entries as free or used
*
*END*--------------------------------------------------------------------*/
void PhyMap::markAll(bool isFree)
{ /* Body */
    /* Used entries are marked 0, free are marked 1. */
    _wl_mem_set(m_entries, isFree ? 0xff : 0, m_entryCount * kEntrySizeInBytes);
    
    /* Set the map to be dirty. */
    setDirty();
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : markBlock
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function marks a block as free or used
*
*END*--------------------------------------------------------------------*/
RtStatus_t PhyMap::markBlock
(
    /* [IN] Absolute block */
    uint32_t absoluteBlock, 
    
    /* [IN] Flag to mark as frre or used */
    bool isFree, 
    
    /* [IN] Flag to erase the block or not */
    bool doAutoErase
)
{ /* Body */
    /* Validate block address. */
    assert(absoluteBlock < m_blockCount);
    assert(m_entries);
    
    /* Find the array index where this phys block belongs */
    uint32_t coarseIndex = absoluteBlock / kBlocksPerEntry;
    uint32_t fineIndex = absoluteBlock % kBlocksPerEntry;
    uint32_t entryValue = m_entries[coarseIndex];
    uint32_t blockMask = (1 << fineIndex);

    /* Set the bit or clear it accordingly */
    if (isFree)
    {
        /* Mark the block as free by setting its bit in the entry. */
        entryValue |= blockMask;

        /* Ensure that the block is actually erased. */
        Block block(absoluteBlock);
        if (doAutoErase && !block.isErased())
        {
            /* As well as setting the bit, erase physical block */
            RtStatus_t retCode = block.eraseAndMarkOnFailure();

            if (retCode)
            {
                /* 
                ** Mark the block bad and return success.
                ** Must be able to add this new bad block to BBRC and update DBBT! 
                */
                entryValue &= ~blockMask;
                
                /* Add this new bad block to the appropriate region. */
                Region * region = g_nandMedia->getRegionForBlock(block);
                if (region)
                {
                    region->addNewBadBlock(block);
                } /* Endif */
            } /* Endif */
        } /* Endif */
    } 
    else
    {
        /* Mark the block as used by clearing its bit in the entry. */
        entryValue &= ~blockMask;
    } /* Endif */

    /* Update the map entry. */
    m_entries[coarseIndex] = entryValue;
    
    /* The phy map is now dirty. */
    setDirty();
    
    return SUCCESS;
} /* Endbody */

/*! \ Optimize this in the case where it is not auto erasing, so it can set entire */
/*!     entries at once (if the range is large enough). */
/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : markRange
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function marks a range of blocks as free or used
*
*END*--------------------------------------------------------------------*/
RtStatus_t PhyMap::markRange
(
    /* [IN] Start block */
    uint32_t absoluteStartBlock, 
    
    /* [IN] Number of blocks */
    uint32_t blockCount, 
    
    /* [IN] Flag to mark as free or used */
    bool isFree, 
    
    /* [IN] Flag to erase blocks or not */
    bool doAutoErase
)
{ /* Body */
    uint32_t theBlock;
    for (theBlock = absoluteStartBlock; theBlock < absoluteStartBlock + blockCount; ++theBlock)
    {
        RtStatus_t status = markBlock(theBlock, isFree, doAutoErase);
        if (status != SUCCESS)
        {
            return status;
        } /* Endif */
    } /* Endfor */
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : isBlockFree
* Returned Value   : TRUE or FALSE
*   - TRUE if the block is free
*   - FALSE if the block is used
* Comments         :
*   This function is used to check a block is free or used
*
*END*--------------------------------------------------------------------*/
bool PhyMap::isBlockFree
(
    /* [IN] The block */
    uint32_t absoluteBlock
) const
{ /* Body */
    /* Find the array index where this phys block belongs */
    uint32_t coarseIndex = absoluteBlock / kBlocksPerEntry;
    uint32_t fineIndex = absoluteBlock % kBlocksPerEntry;
    uint32_t blockMask = (1 << fineIndex);

    /* The block is free if the bit is nonzero. */
    return (m_entries[coarseIndex] & blockMask) != 0;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getFreeCount
* Returned Value   : Number of free blocks on all NANDs.
* Comments         :
*   This function counts the number of unused blocks in the map by counting 
*   the number of bits which are set.
*
*END*--------------------------------------------------------------------*/
uint32_t PhyMap::getFreeCount() const
{ /* Body */
    unsigned i;
    uint32_t u32FreeCount = 0;

    for (i = 0; i < m_entryCount; i++)
    {
        u32FreeCount += count_ones_32(m_entries[i]);
    } /* Endfor */

    return u32FreeCount;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setDirty
* Returned Value   : void
* Comments         :
*   This function marks the map as dirty
*
*END*--------------------------------------------------------------------*/
void PhyMap::setDirty()
{ /* Body */
    bool oldDirty = m_isDirty;
    m_isDirty = true;
    
    /* Invoke dirty callback. */
    if (m_dirtyListener)
    {
        m_dirtyListener(this, oldDirty, m_isDirty, m_dirtyRefCon);
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : clearDirty
* Returned Value   : void
* Comments         :
*   This function marks the map as clear
*
*END*--------------------------------------------------------------------*/
void PhyMap::clearDirty()
{ /* Body */
    bool oldDirty = m_isDirty;
    m_isDirty = false;
    
    /* Invoke dirty callback. */
    if (m_dirtyListener)
    {
        m_dirtyListener(this, oldDirty, m_isDirty, m_dirtyRefCon);
    } /* Endif */
} /* Endbody */

/* EOF */
