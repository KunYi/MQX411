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
*   This file contains implementations of PageOrderMap object
*
*
*END************************************************************************/

#include "wl_common.h"
#include "page_order_map.h"
#include <string.h>
#include "sectordef.h"

using namespace nand;

/* 
** Macros 
*/

/* Number of bits in a word. */
#define BITS_PER_WORD           (32)

/* Computes the size of the map array. */
#define MAP_SIZE(n)             (sizeof(uint8_t) * (n))

/* Computes the size of the occupied array. */
#define OCCUPIED_SIZE(n)        (ROUND_UP_DIV((n), BITS_PER_WORD) * sizeof(uint32_t))

PageOrderMap & PageOrderMap::operator = (const PageOrderMap & other)
{ /* Body */
    /* If the map sizes are different, the reallocate our map to match the other's size. */
    assert(m_entryCount == other.m_entryCount);

    memcpy(m_map, other.m_map, MAP_SIZE(m_entryCount));
    memcpy(m_occupied, other.m_occupied, OCCUPIED_SIZE(m_entryCount));

    return *this;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : init
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function initializes PageOrderMap object
*
*END*--------------------------------------------------------------------*/
RtStatus_t PageOrderMap::init(unsigned entryCount, unsigned maxEntryValue)
{ /* Body */
    /* 
    ** Make sure entries will fit within a byte value. If this assert hits, 
    ** you probably need to change to uint16_t entries. 
    */
    assert(entryCount < 257);
    m_entryCount = entryCount;

    assert(!m_occupied);

    /* Allocate block to be shared by the occupied bitmap and map array. */
    m_occupied = (uint32_t *)_wl_mem_alloc(OCCUPIED_SIZE(entryCount) + MAP_SIZE(entryCount));
    if (!m_occupied)
    {
        return ERROR_OUT_OF_MEMORY;
    } /* Endif */

    /* Point map array in the allocated block, just after the bitmap. */
    m_map = (uint8_t *)m_occupied + OCCUPIED_SIZE(entryCount);

    /* Wipe map to entirely unoccupied. */
    clear();

    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : cleanup
* Returned Value   : void
* Comments         :
*   This function release all allocated memory
*
*END*--------------------------------------------------------------------*/
void PageOrderMap::cleanup()
{ /* Body */
    if (m_occupied)
    {
        /* Free the one block we allocated. */
        _wl_mem_free(m_occupied);
        m_occupied = NULL;
        m_map = NULL;
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : getEntry
* Returned Value   : Physical index
* Comments         :
*   This function tests all task queues for correctness
*
*END*--------------------------------------------------------------------*/
uint8_t PageOrderMap::getEntry
(   
    /* [IN] Logical index */
    unsigned logicalIndex
) const
{ /* Body */
    assert(logicalIndex < m_entryCount);
    return m_map[logicalIndex];
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setEntry
* Returned Value   : void
* Comments         :
*   This function set value at logicalIndex as value of physicalIndex
*
*END*--------------------------------------------------------------------*/
void PageOrderMap::setEntry
(
    /* [IN] Logical index */
    unsigned logicalIndex, 
    
    /* [IN] Physical index */
    uint8_t physicalIndex
)
{ /* Body */
    assert(logicalIndex < m_entryCount);
    m_map[logicalIndex] = physicalIndex;
    setOccupied(logicalIndex);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : isOccupied
* Returned Value   : TRUE or FALSE
*   - TRUE if physical page is being used
*   - FALSE if physical page is not being used
* Comments         :
*   This function checks a physical page is being used or not
*
*END*--------------------------------------------------------------------*/
bool PageOrderMap::isOccupied
(
    /* [IN] Logical index */
    unsigned logicalIndex
) const
{ /* Body */
    assert(logicalIndex < m_entryCount);
    unsigned coarse = logicalIndex / BITS_PER_WORD;
    unsigned fine = logicalIndex % BITS_PER_WORD;
    return ((m_occupied[coarse] >> fine) & 0x1) != 0;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setOccupied
* Returned Value   : void
* Comments         :
*   This function set state of a physical page
*
*END*--------------------------------------------------------------------*/
void PageOrderMap::setOccupied
(
    /* [IN] Logical index */
    unsigned logicalIndex, 
    
    /* [IN] Flag to mark state of the physical page */
    bool isOccupied
)
{ /* Body */
    assert(logicalIndex < m_entryCount);
    unsigned coarse = logicalIndex / BITS_PER_WORD;
    unsigned fine = logicalIndex % BITS_PER_WORD;
    uint32_t mask = 0x1 << fine;
    if (isOccupied)
    {
        m_occupied[coarse] |= mask;
    }
    else
    {
        m_occupied[coarse] &= ~mask;
    } /* Endif */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : operator []
* Returned Value   : Physical index of logical page
* Comments         :
*   This function returns physical index of logical page
*
*END*--------------------------------------------------------------------*/
uint8_t PageOrderMap::operator [] 
(
    /* [IN] Logical index */
    unsigned logicalIndex
) const
{ /* Body */
    assert(logicalIndex < m_entryCount);
    return m_map[logicalIndex];
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : isInSortedOrder
* Returned Value   : TRUE or FALSE
*   - TRUE if the map is in sorted order
*   - FALSE if the map is not in sorted order
* Comments         :
*   This function checks the map is in sorted order or not
*
*END*--------------------------------------------------------------------*/
bool PageOrderMap::isInSortedOrder(unsigned entriesToCheck) const
{ /* Body */
    unsigned i;
    entriesToCheck = MIN(entriesToCheck, m_entryCount);
    for (i = 0; i < entriesToCheck; i++)
    {
        if (m_map[i] != i || !isOccupied(i))
        {
            return false;
        } /* Endif */
    } /* Endfor */

    return true;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setSortedOrder
* Returned Value   : void
* Comments         :
*   This function set the map in sorted order
*
*END*--------------------------------------------------------------------*/
void PageOrderMap::setSortedOrder()
{ /* Body */
    unsigned i;

    /* Set each entry's physical index equal to the logical index. */
    for (i=0; i < m_entryCount; ++i)
    {
        m_map[i] = i;
    } /* Endfor */

    /* Set all entries occupied. */
    _wl_mem_set(m_occupied, 0xff, OCCUPIED_SIZE(m_entryCount));
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : setSortedOrder
* Returned Value   : void
* Comments         :
*   This function set a range of the map in sorted order
*
*END*--------------------------------------------------------------------*/
void PageOrderMap::setSortedOrder
(
    /* [IN] Position of start entry */
    unsigned startEntry, 
    
    /* [IN] Number entries */
    unsigned count, unsigned startValue)
{ /* Body */
    unsigned i;

    /* Set each entry's physical index equal to the logical index. */
    count = MIN(count, m_entryCount - startEntry);
    for (i=0; i < count; ++i)
    {
        unsigned thisEntry = startEntry + i;
        m_map[thisEntry] = startValue + i;
        setOccupied(thisEntry);
    } /* Endfor */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : _taskq_test
* Returned Value   : MQX_OK or an error code
* Comments         :
*   This function tests all task queues for correctness
*
*END*--------------------------------------------------------------------*/
void PageOrderMap::clear()
{ /* Body */
    _wl_mem_set(m_map, 0, MAP_SIZE(m_entryCount));
    _wl_mem_set(m_occupied, 0, OCCUPIED_SIZE(m_entryCount));
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : _taskq_test
* Returned Value   : MQX_OK or an error code
* Comments         :
*   This function tests all task queues for correctness
*
*END*--------------------------------------------------------------------*/
unsigned PageOrderMap::countDistinctEntries() const
{ /* Body */
    unsigned i32DistinctCount = 0;
    unsigned i;
    for (i=0; i < m_entryCount; i++)
    {
        if (isOccupied(i))
        {
            i32DistinctCount++;
        } /* Endif */
    } /* Endfor */

    return i32DistinctCount;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : _taskq_test
* Returned Value   : MQX_OK or an error code
* Comments         :
*   This function tests all task queues for correctness
*
*END*--------------------------------------------------------------------*/
unsigned PageOrderMap::countEntriesNotInOtherMap(const PageOrderMap & other) const
{ /* Body */
    /* Just exit if the other map has a different number of entries than me. */
    if (m_entryCount != other.getEntryCount())
    {
        return 0;
    } /* Endif */

    unsigned entriesOnlyInMe = 0;
    unsigned i;
    for (i=0; i < m_entryCount; ++i)
    {
        if (isOccupied(i) && !other.isOccupied(i))
        {
            ++entriesOnlyInMe;
        } /* Endif */
    } /* Endfor */

    return entriesOnlyInMe;
} /* Endbody */

/* EOF */
