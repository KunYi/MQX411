#if !defined(__region_h__)
#define __region_h__
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
*   This file definition of the nand::Region class and its subclasses.
*
*
*END************************************************************************/

#include "wl_common.h"
#include "sectordef.h"
#include "ddi_media_internal.h"

/* 
** Class definations 
*/

/* Forward declarations. */
/* typedef struct _BadBlockTableNand_t BadBlockTableNand_t; */
class NandPhysicalMedia;

namespace nand {

/* Forward declarations. */
typedef struct _NandConfigBlockRegionInfo NandConfigBlockRegionInfo_t;
class Media;
/* class DiscoveredBadBlockTable; */

typedef enum _region_types
{
    kUnknownRegionType = 0,
    kBootRegionType,
    kSystemRegionType,
    kDataRegionType
} RegionType_t;

/*
** A region of the NAND media.
**
** A region is a subsection of one of the physical NAND chip enables. Regions never span multiple
** chip enables, though they can encompass an entire one. Usually, regions are no larger than a
** single die. A logical drive is composed of one or more regions that do not have to be
** contiguous.
*/
class Region
{
public:

    /* Iterator for NAND regions. */
    
    class Iterator
    {
    public:
        /* Constructor. */
        Iterator(Region ** regionList, unsigned count) : m_list(regionList), m_index(0), m_count(count) {}
        
        /* Returns the next available region. */
        Region * getNext()
        {
            return (m_list && m_index < m_count)
            ? m_list[m_index++]
            : NULL;
        }
        
        /* Restarts the iterator so the next getNext() call will return the first region. */
        void reset() { m_index = 0; }

    protected:
        /* The array of regions we're iterating. */
        Region ** m_list;    
        
        /* Current iterator index. */
        unsigned m_index;  

        /* Total number of regions in the list. */
        unsigned m_count;   
    };
    
    /* Region factory function. */
    static Region * create(NandConfigBlockRegionInfo_t * info);
    
    /* Default constructor. */
    Region();
    
    /* Destructor. */
    virtual ~Region() {}
    
    void initFromConfigBlock(NandConfigBlockRegionInfo_t * info);
    
    /* Return this region's number. */
    unsigned getRegionNumber() const { return m_regionNumber; }
    
    /* Return the chip on which the region resides. */
    unsigned getChip() const { return m_iChip; }
    
    /* Get the region's NAND object. */
    NandPhysicalMedia * getNand() { return m_nand; }
    
    /* Get the logical drive that the region belongs to. */
    LogicalDrive * getLogicalDrive() { return m_pLogicalDrive; }
    
    /* Get the region's start address. */
    const BlockAddress & getStartBlock() const { return m_u32AbPhyStartBlkAddr; }
    
    /* Get the region's length in blocks. */
    uint32_t getBlockCount() const { return m_iNumBlks; }
    
    /* Get the address of the last block in the region. */
    BlockAddress getLastBlock() const { return m_u32AbPhyStartBlkAddr + m_iNumBlks - 1; }
    
    /* Returns the type of this region. */
    virtual RegionType_t getRegionType() const = 0;

    /*#pragma ghs section text=".static.text" */
#if defined(__ghs__)
#pragma ghs section text=".init.text"
#endif 

#if defined (__CODEWARRIOR__)
#endif 

    /* Returns true if the region belongs to a drive-type drive. */
    virtual bool isDataRegion() const { return getRegionType() == kDataRegionType; }
    
    /* Returns true if the region belongs to a system drive. */
    virtual bool isSystemRegion() const { return getRegionType() == kSystemRegionType; }
    
    /*#pragma ghs section text=default */
#if defined(__ghs__)
#pragma ghs section text=default
#endif 

#if defined (__CODEWARRIOR__)
#endif    
    /* Returns the number of bad blocks within the region. */
    virtual uint32_t getBadBlockCount() const = 0; 
    
    /* Insert a new bad block into the region. */
    virtual void addNewBadBlock(const BlockAddress & addr) = 0;

    /* Mark the region as dirty. */
    /* Setting the region dirty will force a background update of the DBBT on the NAND. */
    void setDirty();
    
public:
    /* This region's region number. */
    unsigned m_regionNumber;    
    
    /* Index of NAND Chip containing this Region */
    unsigned m_iChip;                 
    
    /* NAND descriptor */
    NandPhysicalMedia * m_nand; 
    
    /* Pointer back to our grandparent */
    LogicalDrive * m_pLogicalDrive;  
    
    /* Some System Drive, or Data Drive */
    LogicalDriveType_t m_eDriveType; 
    
    /* Drive Tag */
    uint32_t m_wTag;                 
    
    /* Starting Block number for region relative to chip */
    int32_t m_iStartPhysAddr;        
    
    /* Size, in blocks, of whole region. Size includes embedded bad blocks. */
    int32_t m_iNumBlks;              
    
    /* 
    ** Logical block info
    **
    ** The following elements, relating to Logical Blocks, are set and used
    ** by the Data Drive, only.  They are included in this structure for convenience.
    ** Size, in blocks, of Logical Blocks area. 
    */
    uint32_t m_u32NumLBlks;           
    /* Absolute Physical starting block within media. */
    BlockAddress m_u32AbPhyStartBlkAddr;
        
    /* Bad blocks */
    /* If TRUE, the bad block information has updates */
    bool m_bRegionInfoDirty;  
    
};

/*#pragma ghs section text=".static.text" */
#if defined(__ghs__)
#pragma ghs section text=".init.text"
#endif 

#if defined (__CODEWARRIOR__)
#endif 

/*
** Region for a system drive.
**
** A system region keeps a full bad block table that is accessible with the getBadBlocks()
** method.
*/
class SystemRegion : public Region
{
public:
    /* Default constructor. */
    SystemRegion();
    
    /* Returns the type of this region. */
    virtual RegionType_t getRegionType() const { return kSystemRegionType; }
    
    virtual uint32_t getBadBlockCount() const { return 0; }
    
    /* Insert a new bad block into the region. */
    virtual void addNewBadBlock(const BlockAddress & addr) {};

protected:
};

/*#pragma ghs section text=default */
#if defined(__ghs__)
#pragma ghs section text=default
#endif 

#if defined (__CODEWARRIOR__)
#endif 

/* Region representing an area of the NAND containing boot blocks. */
class BootRegion : public SystemRegion
{
public:
    /* Constructor. */
    BootRegion() {}
    
    /* Returns the type of this region. */
    virtual RegionType_t getRegionType() const { return kBootRegionType; }

};

/*
** Region for a data drive or hidden data drive.
**
** Data regions form either the main data drive or hidden data drives. Because the mapper uses
** the phy map for block allocation, data regions do not have to maintain a full bad block table.
** Thus, the getBadBlocks() method will always return NULL. However, a count of the bad blocks
** within the region is kept. When a new bad block is added by calling addNewBadBlock(), the
** region's bad block count will be incremented.
*/
class DataRegion : public Region
{
public:
    /* Default constructor. */
    DataRegion();
    
    /* Returns the type of this region. */
    virtual RegionType_t getRegionType() const { return kDataRegionType; }
    
    virtual uint32_t getBadBlockCount() const { return m_badBlockCount; }
    
    /* Insert a new bad block into the region. */
    virtual void addNewBadBlock(const BlockAddress & addr);

protected:
    uint32_t m_badBlockCount;   /* Number of bad blocks in this region. */

};

} /* namespace nand */

#endif /* __region_h__ */

/* EOF */
