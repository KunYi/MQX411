#ifndef __ddi_nand_media_h__
#define __ddi_nand_media_h__
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
*   This file contains internal declarations for the NAND media layer
*
*
*END************************************************************************/

#include "ddi_nand_ddi.h"
/*#include "drivers/media/nand/rom_support/rom_nand_boot_blocks.h" */

namespace nand {

#if !defined(ALLOW_BB_TABLE_READ_SKIP)
/* #define ALLOW_BB_TABLE_READ_SKIP */
#endif

#if !defined(DEBUG_BOOT_BLOCK_ALLOCATION_DISCOVER)
/* #define DEBUG_BOOT_BLOCK_ALLOCATION_DISCOVER */
#endif
#if !defined(DEBUG_BOOT_BLOCK_SEARCH)
/* #define DEBUG_BOOT_BLOCK_SEARCH */
#endif

/* Define setting the NAND Chip Enable to GPMI_CE0. */
#define NAND0       0

/* Define setting the NAND Chip Enable to GPMI_CE1. */
#define NAND1       1

/* Define setting the NAND Chip Enable to GPMI_CE1. */
#define OTHER_NAND_FOR_SECONDARY_BCBS   (NAND1)

/* Size in bytes of the data area of a 2K page. */
#define NAND_PAGE_SIZE_2K               (2048)

/* 
** Load parameter flag constants
**
** These constants tell Media::findNCB() and Media::findLDLB() to
** either load and save the values in the respective boot control block or to
** simply find the BCB and ignore its contents. 
*/
enum _load_parameters_constants
{
    /* Load BCB contents. */
    kLoadParameters = true,
        
    /* Only find the BCB; do not load contents. */
    kDontLoadParameters = false  
};

/* 
** Erase block flag constants
** 
** These constants are used for the eraseGoodBlock argument of
** Media::findFirstGoodBlock(). They tell the function whether
** to erase the next good block that is found within the search window. 
*/
enum _erase_blocks_constants
{
    /* Erase the block. */
    kEraseFoundBlock = true,

    /* Don't erase the block. */
    kDontEraseFoundBlock = false  
};

/* Constants for use when calling Media::findBootControlBlocks(). */
enum _allow_recovery_constants {
    /* Allow boot blocks to be recovered. */
    kAllowRecovery = true,

    /* Don't allow boot blocks to be recovered. */
    kDontAllowRecovery = false  
};

/* Config block constants */
/* Offset of sector within Block ( 0 based) - 1 is the 2nd sector of the block. */
#define CONFIG_BLOCK_SECTOR_OFFSET  1
#define NAND_CONFIG_BLOCK_MAGIC_COOKIE  0x010203
#define NAND_CONFIG_BLOCK_VERSION       0x00000b
#define NAND_MAGIC_COOKIE_WORD_POS      0
#define NAND_VERSION_WORD_POS           1

/* 
** Type defination 
*/

/* Config block region info */
typedef struct _NandConfigBlockRegionInfo {

    /* 
    ** Constants for region tag values.
    ** 
    ** These constants define special values for the \a wTag field of a config block region
    ** info structure. In addition to these values, the normal drive tag values are valid. 
    */
    enum _region_tag_constants
    {
        /* Tag value for a boot region in the config block. */
        kBootRegionTag = 0x7fffffff
    };

    /* Some System Drive, or Data Drive */
    LogicalDriveType_t eDriveType;       
    
    /* Drive Tag */
    uint32_t wTag;              
    
    /* Size, in blocks, of whole Region. Size includes embedded Bad Blocks */
    int iNumBlks;         
    
    /* Chip number that region is located on. */
    int iChip;            
    
    /* Region's start block relative to chip. */
    int iStartBlock;      
} NandConfigBlockRegionInfo_t;

/* Configuration block info sector */
typedef struct _NandConfigBlockInfo {
    /* #NAND_CONFIG_BLOCK_MAGIC_COOKIE */
    int iMagicCookie;       
    
    /* #NAND_CONFIG_BLOCK_VERSION */
    int iVersionNum;        
    
    /* Number Bad Blocks on this Chip */
    int iNumBadBlks;        
    
    /* Number of regions in the region array. */
    int iNumRegions;        
    
    /* Total number of reserved blocks on this chip enable. */
    int iNumReservedBlocks; 
    
    /* Information about the regions on this chip enable. */
    NandConfigBlockRegionInfo_t Regions[1]; 
} NandConfigBlockInfo_t;

/* 
** Nand Fingerprint structure.
**
** Fingerprints are used in conjunction with the ECC to determine
** whether or not a block is valid.  They are strategically placed
** in both the NCB and LDLB blocks. 
*/
typedef struct _FingerPrintValues {
    uint32_t m_u32FingerPrint1;
    uint32_t m_u32FingerPrint2;
    uint32_t m_u32FingerPrint3;
} FingerPrintValues_t;

/*
** Nand Zip Config Block
*/
typedef struct _NandZipConfigBlockInfo {
    int iNumEntries;
    NandConfigBlockRegionInfo_t Regions[MAX_NAND_REGIONS];
} NandZipConfigBlockInfo_t;

/* 
** Global external declarations 
*/

/* 
** NCB Fingerprint values.
**
** These are the fingerprints that are spaced at defined values in the
** first page of the block to indicate this is a NCB.   
*/
extern const FingerPrintValues_t zNCBFingerPrints;

/* 
** LDLB Fingerprint values.
**
** These are the fingerprints that are spaced at defined values in the
** first page of the block to indicate this is a LDLB.   
*/
extern const FingerPrintValues_t zLDLBFingerPrints;

/* 
** DBBT Fingerprint values.
** 
** These are the fingerprints that are spaced at defined values in the
** first page of the block to indicate this is a DBBT.   
*/
extern const FingerPrintValues_t zDBBTFingerPrints;

/* 
** BBRC Fingerprint values. 
**
** These are the fingerprints that are spaced at defined values in the
** BBRC page to indicate this is a BBRC.   
*/
extern const FingerPrintValues_t zBBRCFingerPrints;

/* 
** Function prototypes 
*/

bool ddi_nand_media_doFingerprintsMatch(BootBlockStruct_t * pBootBlock, const FingerPrintValues_t * pFingerPrintValues);

} /* namespace nand */

#endif /* __ddi_nand_media_h__ */

/* EOF */
