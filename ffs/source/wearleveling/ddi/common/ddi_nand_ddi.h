#ifndef __ddi_nand_ddi_h__
#define __ddi_nand_ddi_h__
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
*   This file contains internal declarations for the NAND media layer.
*
*
*END************************************************************************/

#include "ddi_nand_hal_internal.h"
#include "sectordef.h"
#include "ddi_nand_hal.h"
#include "ddi_block.h"
#include "ddi_page.h"
#include "region.h"
#include "media_buffer.h"
#include "simple_mutex.h"
#include "bad_block_table.h"

/* 
** Forward type definations 
*/

typedef struct _BadBlockTableNand_t BadBlockTableNand_t;
typedef struct _BootBlockStruct_t BootBlockStruct_t;

namespace nand {

/* 
** Constants 
*/

/*
** Maximum region counts
**
** Each chip may have one or more data regions.  One reason to
** have more than one data region per chip is to allow multi-plane addressing.
*/
#define MAX_DATA_REGIONS_PER_CHIP               4

/*
** The maximum number of system drives is based on the typical drive arrangement
** for previous and current SDK releases.
*/
#define MAX_NAND_SYSTEM_DRIVES                  9

/*
** A typical system will have just one hidden data drive.
** Adding any vendor-specific drives will require increasing MAX_NAND_HIDDEN_DRIVES.
*/
#define MAX_NAND_HIDDEN_DRIVES                  2

/* Value for #BootBlockLocation_t::u when the boot block address is unknown. */
#define NAND_BOOT_BLOCK_UNKNOWN                 (0xffffffff)

/* 
** Macros 
*/

/*
** Each chip will have MAX_DATA_REGIONS_PER_CHIP data drive regions (e.g. 2 to
** cover 8-plane 128MB NANDs).
** Each system drive also uses one region.
*/
#define MAX_DATA_DRIVE_REGIONS                  (MAX_DATA_REGIONS_PER_CHIP * MAX_NAND_DEVICES)

/* The total maximum number of regions. Add in another region for each chip for the boot region. */
#define MAX_NAND_REGIONS                        (MAX_DATA_DRIVE_REGIONS + MAX_NAND_SYSTEM_DRIVES + MAX_NAND_HIDDEN_DRIVES + MAX_NAND_DEVICES)

    /* Values for the #BootBlockLocation_t::bfBlockProblem field. */
enum _nand_boot_block_state
{
    /*
    ** There is no problem with the boot block, and it
    ** contains actual boot-block data.
    */
    kNandBootBlockValid = 0,
        
    /* The boot block is corrupt or erased. */
    kNandBootBlockInvalid = 1,
        
    /*
    ** The boot block address is known/chosen/laid-out,
    ** but there is no data written to the boot-block.
    */
    kNandBootBlockEmpty = 2,
        
        /* The state of this boot block is currently unknown. */
    kNandBootBlockUnknown = 3
};

/* Structure to track the location of a boot block. */
typedef union _BootBlockLocation {
    /* Bit fields. */
    struct {
        /* Chip-relative block address. */
        uint32_t bfBlockAddress : 28;
        
        /* One of the #_nand_boot_block_state values. */
        uint32_t bfBlockProblem : 2;
        
        /* The number of the chip on which the boot block resides. */
        uint32_t bfNANDNumber   : 2;
    } b;
        
    /* Combined bits. */
    uint32_t u;
    
    /* Returns whether the boot block is valid. */
    inline bool isValid() const
    {
        return (b.bfBlockProblem == kNandBootBlockValid);
    }

    /*FUNCTION*----------------------------------------------------------------
    *
    * Function Name    : doesAddressMatch
    * Returned Value   : TRUE or FALSE
    *   - TRUE if the passed in address matches the boot block's location and the boot block
    *   is valid.
    *   - FASLE if the boot block is invalid and/or the provided address does not match.
    * Comments         :
    *   Compare a BootBlockLocation_t struct with a nand number and block address.
    *   If the nand number and address match those in the struct, then expression will
    *   have a value of true. In addition, the boot block location must also be valid.
    *
    *END*--------------------------------------------------------------------*/
    inline bool doesAddressMatch
    (
        /* [IN] Index of NAND chip enable. */
        uint32_t nand, 
        
        /* [IN] Chip enable relative block address to compare against the boot block location. */
        uint32_t addr
    ) const
    { /* Body */
        return (isValid() && (nand) == b.bfNANDNumber && (addr) == b.bfBlockAddress);
    } /* Endbody */

} BootBlockLocation_t;

/* The set of bad block table modes. */
enum _nand_bad_block_table_mode
{
    /* No bad block table fields are valid. */
    kNandBadBlockTableInvalid,
        
    /*
    ** Allocation mode.
    ** 
    ** The global per-chip tables and counts in #NandMediaInfo are valid.
    */
    kNandBadBlockTableAllocationMode,
        
    /*
    ** Discovery mode.
    ** 
    ** Per-chip tables and counts are invalid, but the regions'
    ** bad block tables are valid.
    */
    kNandBadBlockTableDiscoveryMode
};

/* Typedef for the bad block table mode enumeration. */
typedef enum _nand_bad_block_table_mode NandBadBlockTableMode_t;

/* Forward type declarations. */
typedef struct _FingerPrintValues FingerPrintValues_t;
typedef struct _NandConfigBlockInfo NandConfigBlockInfo_t;
typedef struct _NandConfigBlockRegionInfo NandConfigBlockRegionInfo_t;
typedef struct _NandZipConfigBlockInfo NandZipConfigBlockInfo_t;
class PhyMap;
class DiscoveredBadBlockTable;
class Mapper;
class DeferredTaskQueue;
class SystemDriveRecoveryManager;
class NssmManager;

/*
** Number of pages to skip while searching for boot blocks.
** 
** The value is always 64, because that is what the 37xx ROM uses.
*/
const uint32_t kBootBlockSearchStride = 64;

/*
** Information about the boot blocks and their locations.
**
** The DBBT locations in this struct has slightly different usage than the other boot block
** locations. They point to the beginning of the respective DBBT search area, rather than the
** actual block containing the DBBT copy. This means that to read a DBBT, you must search
** for it starting at the location in this struct. In most cases, the location specified here
** will actually contain the DBBT, but not always. It is even possible for the location here
** to be a bad block.
**
** However, the BootBlockLocation_t::b::bfBlockProblem field of the two DBBT locations is still
** valid, and indicates whether there is a valid DBBT within the search area. This field is set
** by Media::findBootControlBlocks().
*/
struct BootBlocks
{
    /* Whether the addresses for NCB1 and NCB2 are valid. Also implies that the NCB exists. */
    bool m_isNCBAddressValid;
    
    /* NAND Control Block Address */
    BootBlockLocation_t m_ncb1;
    
    /* NAND Control Block Address */
    BootBlockLocation_t m_ncb2;
    
    /* NAND Logical Device Layout Block. */
    BootBlockLocation_t m_ldlb1;
    
    /* NAND Logical Device Layout Block. */
    BootBlockLocation_t m_ldlb2;
    
    /* First Discovered Bad Block Table search area start address. */
    BootBlockLocation_t m_dbbt1;
    
    /* Second Discovered Bad Block Table search area start address. */
    BootBlockLocation_t m_dbbt2;
    
    /* Firmware Primary address, used only during allocation. */
    BootBlockLocation_t m_primaryFirmware;
    
    /* Firmware Secondary address, used only during allocation. */
    BootBlockLocation_t m_secondaryFirmware;
    
    /* Current address of FW to load. */
    BootBlockLocation_t m_currentFirmware;
    
    bool hasValidNCB() const { return m_isNCBAddressValid; }
};

/*
** NAND logical media class.
**
** This structure contains all of the information about a (the) NAND Media.
** It has a table of all of the chips' NANDDescriptors, the addresses of the
** Configuration Block for each chip, a table of all of the Regions
** on those chips, and a table of all of the Bad Blocks on those chips.
*/
class Media : public ::LogicalMedia
{
public:
        
    /* Default constructor. */
    Media();
    
    /* Destructor. */
    virtual ~Media();

    /* Logical media API */
    virtual RtStatus_t init();
    virtual RtStatus_t allocate(MediaAllocationTable_t * table) { return ERROR_DDI_LDL_GENERAL;};
    virtual RtStatus_t allocateFromPreDefine(MediaAllocationTable_t * table);
    virtual RtStatus_t allocateFromPreDefineRegions();
    virtual RtStatus_t discover() { return ERROR_DDI_LDL_GENERAL;};
    virtual RtStatus_t getMediaTable(MediaAllocationTable_t ** table);
    virtual RtStatus_t freeMediaTable(MediaAllocationTable_t * table);
    virtual RtStatus_t getInfo(uint32_t infoSelector, void * value);
    virtual RtStatus_t setInfo(uint32_t infoSelector, const void * value);
    virtual RtStatus_t erase()
    {
        return ERROR_DDI_LDL_UNIMPLEMENTED;
    }
    virtual RtStatus_t shutdown();
    virtual RtStatus_t flushDrives();
    virtual RtStatus_t setBootDrive(DriveTag_t tag)
    {
        return ERROR_DDI_LDL_UNIMPLEMENTED;
    }
    RtStatus_t discover(bool bWriteToTheDevice);
    
    /* Regions */
    unsigned getRegionCount() const { return m_iNumRegions; }
    Region * getRegion(unsigned index) { return m_pRegionInfo[index]; }
    Region * getRegionForBlock(const BlockAddress & physicalBlock);
        
    /* Quick way to create a iterator for this media object's regions. */
    Region::Iterator createRegionIterator() { return Region::Iterator(m_pRegionInfo, m_iNumRegions); }
    
    /* Block counts */
    uint32_t getTotalBlockCount() const { return m_iTotalBlksInMedia; }
    uint32_t getBadBlockCount() const { return m_iNumBadBlks; }
    /*NandBadBlockTableMode_t getBadBlockTableMode() const { return m_badBlockTableMode; }*/
    uint32_t getReservedBlockCount() const { return m_iNumReservedBlocks; }
    uint32_t getPagePerBlock() { return m_params->wPagesPerBlock; }

#if defined(__ghs__)
#pragma ghs section text=".init.text"
#endif /* defined(__ghs__) */

#if defined(__CODEWARRIOR__)
#endif /* defined(__CODEWARRIOR__) */
    /* Accessors */
    /*BootBlocks & getBootBlocks() { return m_bootBlocks; }*/
    NssmManager * getNssmManager() { return m_nssmManager; }
    Mapper * getMapper() { return m_mapper; }

    BadBlockTable * getBBT() { return m_badBlockTable; }
    
    DeferredTaskQueue * getDeferredQueue() { return m_deferredTasks; }
    /*SystemDriveRecoveryManager * getRecoveryManager() { return m_recoveryManager; }*/
    
#if defined(__ghs__)
#pragma ghs section text=default
#endif /* defined(__ghs__) */

#if defined(__CODEWARRIOR__)
#endif /* defined(__CODEWARRIOR__) */

    /* Boot blocks */    
    /*RtStatus_t recoverBootControlBlocks(bool force, SECTOR_BUFFER * pBuffer, SECTOR_BUFFER * pAuxBuffer);*/

    /* Scan a search window for a boot block matching the given fingerprints. */
    /*RtStatus_t bootBlockSearch(uint32_t u32NandDeviceNumber, const FingerPrintValues_t * pFingerPrintValues, uint32_t * p32SearchSector, SECTOR_BUFFER * pBuffer, SECTOR_BUFFER * pAuxBuffer, bool bDecode, BootBlockStruct_t **ppBCB);*/

    /* Given a starting block number, find the next good block. */
    /*RtStatus_t findFirstGoodBlock(uint32_t u32NAND, uint32_t * pu32StartingBlock, uint32_t u32SearchSize, SECTOR_BUFFER * auxBuffer, bool eraseGoodBlock);*/
    
    /* Returns the number of pages in the search window. */
    /*uint32_t getBootBlockSearchWindow() { return m_bootBlockSearchWindow; }*/
    
    /* Returns the number of blocks in the search window. */
    /*uint32_t getBootBlockSearchWindowInBlocks() { return m_bootBlockSearchWindow / m_params->wPagesPerBlock; }*/
    
    /* Set the span of pages over which the NAND driver searches for BCBs. */
    /*uint32_t setBootBlockSearchNumberAndWindow(uint32_t newSearchNumber);*/

protected:

    /* Parameters shared between all chip selects. */
    NandParameters_t * m_params;
    
    /* The manager object for all NSSMs. */
    NssmManager * m_nssmManager;
    
    /* The virtual to physical block mapper. */
    Mapper * m_mapper;
    
    /* Queue to handle deferred tasks. */
    DeferredTaskQueue * m_deferredTasks;
    
    /* Object to handle recovery from failed reads of system drives. */
    SystemDriveRecoveryManager * m_recoveryManager;
    
    /* Block addresses */
    /*BootBlocks m_bootBlocks;*/
    
    /* On the STMP3700, the Config block is the LDLB block. */
    /*int m_ConfigBlkAddr[MAX_NAND_DEVICES];*/
    
    /* Regions */
    /* Number of valid regions pointed to by #pRegionInfo. */
    unsigned m_iNumRegions;
    
    /* Pointer to the array of region structs. */
    Region ** m_pRegionInfo;
    
    /* Block counts */
    /* Total number of blocks in this media. */
    uint32_t m_iTotalBlksInMedia;
    uint32_t m_iNumBadBlks;
    uint32_t m_iNumReservedBlocks;
    
    /* Bad blocks */
    BadBlockTable * m_badBlockTable;
    
    /* Boot block search window */
    /* Number of search strides the BCB search window is composed of. */
    /*uint32_t m_bootBlockSearchNumber;*/

    /*
    ** Number of pages within which a boot block must be found.
    ** 
    ** The boot block search window size in pages is #kNandBootBlockSearchStride multiplied
    ** by #m_bootBlockSearchNumber. So if the search stride is 64 pages and the search number
    ** is 2, then the search window is 128 pages, scanned by skipping 64 pages each step.
    */
    /*uint32_t m_bootBlockSearchWindow;*/

protected:

    void deleteRegions();

    /* Determine if the NANDs are fresh from the factory. */
    /*bool areNandsFresh();*/

    /* Discover */
    /*RtStatus_t getConfigBlock1stSector(NandPhysicalMedia * pNandPhysicalMediaDesc, int * piConfigBlockPhysAdd, bool bConfirmConfigBlock, SECTOR_BUFFER * sectorBuffer, SECTOR_BUFFER * auxBuffer);*/
    /*RtStatus_t fillInNandBadBlocksInfo(SECTOR_BUFFER * sectorBuffer, SECTOR_BUFFER * auxBuffer, int attempt, bool bWriteToTheDevice);*/
    RtStatus_t createDrives();
    /*RtStatus_t fillInBadBlocksFromAllocationModeTable(SECTOR_BUFFER * sectorBuffer, SECTOR_BUFFER * auxBuffer);*/

    /* Nand media erase */
    /*uint32_t eraseScan(int iNandNumber, int iBlockPhysAddr, int remainingBlocks, NandPhysicalMedia * pNandPhysicalMediaDesc, bool convertMarkings, SECTOR_BUFFER * auxBuffer, bool * wasBad);*/
    /*void eraseBlockRange(int iNandNumber, int iBlockPhysAddr, uint32_t numberToErase, NandPhysicalMedia * pNandPhysicalMediaDesc);*/
    /*void eraseHandleBadBlock(int iNandNumber, int iBlockPhysAddr, NandPhysicalMedia * pNandPhysicalMediaDesc, bool convertMarkings);*/
    /*void eraseAddBadBlock(int iNandNumber, int iBlockPhysAddr);*/
    /*bool eraseShouldSkipBlock(int iNandNumber, int iBlockPhysAddr);*/

    /* Allocate */
    /*void findConfigBlocks();*/
    int findNextRegionInChip(int iChip, int iLastBlockFound, NandZipConfigBlockInfo_t *pNandZipConfigBlockInfo);
    /* void prepareBlockDescriptor(int iChip, NandZipConfigBlockInfo_t * pNandZipConfigBlockInfo, SECTOR_BUFFER * pSectorBuffer, SECTOR_BUFFER * pAuxBuffer); */
    /*RtStatus_t writeBootControlBlockDescriptor(NandZipConfigBlockInfo_t * pNandZipConfigBlockInfo, SECTOR_BUFFER * pSectorBuffer, SECTOR_BUFFER * pAuxBuffer);*/
    /*RtStatus_t updatePhymapWithBadBlocks(PhyMap * phymap);*/
};

} /* namespace nand */

/* 
** Global external declarations 
*/

extern nand::Media * g_nandMedia;
extern WL_MUTEX g_NANDThreadSafeMutex;

/* 
** Class definations 
*/

/* Utility class to automatically lock and unlock the NAND driver. */
class DdiNandLocker : public SimpleMutex
{
public:
    /* Locks the mutex that serialises access to the NAND driver. */
    DdiNandLocker()
    :   SimpleMutex(g_NANDThreadSafeMutex)
    {
    }
    
    /*
    ** Unlocks the NAND drive mutex.
    ** 
    ** Before the mutex is unlocked it is prioritised, which makes sure that
    ** the highest priority thread that is blocked on the mutex will be the
    ** next in line to hold it.
    */
    ~DdiNandLocker()
    {
        
    }
};

#endif /* __ddi_nand_ddi_h__ */

/* EOF */
