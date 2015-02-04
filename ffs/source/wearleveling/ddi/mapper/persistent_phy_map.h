#if !defined(__persistent_phy_map_h__)
#define __persistent_phy_map_h__
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
*   This file contains declaration of the persistent phy map class.
*
*
*END************************************************************************/

#include "persistent_map.h"

/* 
** Class definations 
*/

namespace nand {

class Mapper;
class PhyMap;

/* Handles storage of a PhyMap on the NAND. */
class PersistentPhyMap : public PersistentMap
{
public:

    /* Constructor. */
    PersistentPhyMap(Mapper & mapper);
    
    /* Destructor. */
    virtual ~PersistentPhyMap();
    
    /* Initializer. */
    void init();
    
    /* Finds and loads the map. */
    RtStatus_t load();
    
    /* Saves the map into the current block, consolidating if necessary. */
    RtStatus_t save();
    
    /* Allocates a new block and writes the map to it. */
    RtStatus_t saveNewCopy();
    
    PhyMap * getPhyMap();
    void setPhyMap(PhyMap * theMap);

    /* set&get of trust element */
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
    /* The map that is being persisted. */
    PhyMap * m_phymap;

    /* True if we're in the middle of loading the phymap. */
    bool m_isLoading;   

    virtual RtStatus_t getSectionForConsolidate(
    uint32_t u32EntryNum,
    uint32_t thisSectionNumber,
    uint8_t *& bufferToWrite,
    uint32_t & bufferEntryCount,
    uint8_t * sectorBuffer);

    
};

} /* namespace nand */

#endif /* __persistent_phy_map_h__ */

/* EOF */
