#if !defined(__pageordermap_h__)
#define __pageordermap_h__
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

/* 
** Class definations 
*/

namespace nand
{

/*
** Map of logical page index to physical page index.
**
** In addition to being a simple logical to physical map, this class tracks
** whether each entry has been set to a valid value, i.e. whether it is
** occupied.
**
** This class uses a single malloc'd block to hold both the map and occupied
** arrays. The occupied array is at the beginning of the block, following by
** the map array. This is slightly more efficient than two separate
** allocations.
**
** Note that the number of entries doesn't necessarily have to be equal to the
** number of pages in a block. The physical offset associated with each entry
** can be any number within a certain range (8-bit or 16-bit). So if you have
** fewer logical entries than the number of pages per block, you can still
** track their location across the full block.
*/
class PageOrderMap
{
public:
    /* Init and cleanup */
    /* Default constructor. */
    PageOrderMap() : m_entryCount(0), m_map(0), m_occupied(0) {}
    
    /* Destructor. */
    ~PageOrderMap() { cleanup(); }
    
    /* Assignment operator. */
    PageOrderMap & operator = (const PageOrderMap & other);
    
    /* Init method taking the number of entries. */
    RtStatus_t init(unsigned entryCount, unsigned maxEntryValue=255);
    
    /* Frees map memory. */
    void cleanup();
    
    /* Returns the number of entries. */
    inline unsigned getEntryCount() const { return m_entryCount; }
    
    uint8_t getEntry(unsigned logicalIndex) const;

    void setEntry(unsigned logicalIndex, uint8_t physicalIndex);
    
    bool isOccupied(unsigned logicalIndex) const;
    
    void setOccupied(unsigned logicalIndex, bool isOccupied=true);
    
    uint8_t operator [] (unsigned logicalIndex) const;
    
    /* Checks whether logical is equal to physical through a specified entry.
    **
    ** This method scans \a entriesToCheck number of entries, starting at the first. It
    ** looks for whether each entry's associated value is equal to that entry's index. If
    ** this is true for all the examined entries, then the map is considered to be in
    ** sorted order and true is returned. If the value of any examined entry is something
    ** other than that entry's index then false is returned.
    */
    bool isInSortedOrder(unsigned entriesToCheck) const;
    
    /* Set all entries to the sorted order. */
    void setSortedOrder();
    
    /* Set a range of entries to a sorted order. */
    void setSortedOrder(unsigned startEntry, unsigned count, unsigned startValue);
    
    /* Sets the map so all entries are unoccupied. */
    void clear();
    
    /* Count number of distinct entries in the map.
    **
    ** This function counts the number of actual entries contained in physical
    ** block which is represented by the map.  Duplicate entries overwrite each
    ** other.  So, it is sufficient to simply count number of entries which
    ** are occupied.
    **
    */
    unsigned countDistinctEntries() const;
    
    /* Counts entries that exist in this map but not another.
    **
    ** Given another page order map, this method determines how many logical entries
    ** exist only in this map and not the other.
    **
    ** If the two maps have different numbers of entries then 0 will be returned.
    */
    unsigned countEntriesNotInOtherMap(const PageOrderMap & other) const;

protected:
    /* Number of entries. */
    unsigned m_entryCount;
    
    /* Array of map entries. Points just after m_occupied. */
    uint8_t * m_map;
    
    /* 
    ** Bitmap of occupied status for the entries. 
    ** This is the real pointer to the malloc'd memory. 
    */
    uint32_t * m_occupied;
};

} /* namespace nand */

#endif /* __pageordermap_h__ */

/* EOF */
