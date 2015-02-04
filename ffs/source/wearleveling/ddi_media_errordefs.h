#ifndef __di_media_erordefs_h__
#define __di_media_erordefs_h__
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
*   This file defines error definitions for all media components.
*
*
*END************************************************************************/

#include "nandflash.h"

/* 
** Constants 
*/

#define ERROR_DDI_LDL_GROUP                 ( NANDFLASHERR_WL_BASE + 1 )
#define ERROR_DDI_MEDIABUFMGR_GROUP         ( NANDFLASHERR_WL_BASE + 40 )
#define ERROR_DDI_LBA_NAND_GROUP            ( NANDFLASHERR_WL_BASE + 55 )
#define ERROR_DDI_NAND_GROUP                ( NANDFLASHERR_WL_BASE + 65 )

/*
** Logical drive layer errors
** Errors returned from the logical drive layer for all media types.
*/
/* Generic LDL error */
#define ERROR_DDI_LDL_GENERAL                                   (ERROR_DDI_LDL_GROUP)
#define ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_NUMBER               (ERROR_DDI_LDL_GROUP + 1)
#define ERROR_DDI_LDL_LDRIVE_NOT_INITIALIZED                    (ERROR_DDI_LDL_GROUP + 2)

/* General hardware failure. */
#define ERROR_DDI_LDL_LDRIVE_HARDWARE_FAILURE                   (ERROR_DDI_LDL_GROUP + 3)
#define ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_TYPE                 (ERROR_DDI_LDL_GROUP + 4)
#define ERROR_DDI_LDL_LDRIVE_INVALID_INFO_TYPE                  (ERROR_DDI_LDL_GROUP + 5)
#define ERROR_DDI_LDL_LDRIVE_SECTOR_OUT_OF_BOUNDS               (ERROR_DDI_LDL_GROUP + 6)
#define ERROR_DDI_LDL_LDRIVE_WRITE_PROTECTED                    (ERROR_DDI_LDL_GROUP + 7)
#define ERROR_DDI_LDL_LDRIVE_WRITE_ABORT                        (ERROR_DDI_LDL_GROUP + 11)
#define ERROR_DDI_LDL_LDRIVE_MEDIA_NOT_ALLOCATED                (ERROR_DDI_LDL_GROUP + 12)
#define ERROR_DDI_LDL_LDRIVE_LOW_LEVEL_MEDIA_FORMAT_REQUIRED    (ERROR_DDI_LDL_GROUP + 13)

/* General hardware failure. */
#define ERROR_DDI_LDL_LMEDIA_HARDWARE_FAILURE                   (ERROR_DDI_LDL_GROUP + 16)
#define ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_NUMBER               (ERROR_DDI_LDL_GROUP + 17)
#define ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_INITIALIZED              (ERROR_DDI_LDL_GROUP + 18)
#define ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_DISCOVERED               (ERROR_DDI_LDL_GROUP + 19)
#define ERROR_DDI_LDL_LMEDIA_INVALID_MEDIA_INFO_TYPE            (ERROR_DDI_LDL_GROUP + 20)
#define ERROR_DDI_LDL_LMEDIA_ALLOCATION_TOO_LARGE               (ERROR_DDI_LDL_GROUP + 21)
#define ERROR_DDI_LDL_LMEDIA_MEDIA_NOT_ERASED                   (ERROR_DDI_LDL_GROUP + 22)
#define ERROR_DDI_LDL_LMEDIA_MEDIA_ERASED                       (ERROR_DDI_LDL_GROUP + 23)
#define ERROR_DDI_LDL_LMEDIA_MEDIA_ALLOCATE_BOUNDS_EXCEEDED     (ERROR_DDI_LDL_GROUP + 24)

/* Indicates that the corrupted drive is not recoverable. */
#define ERROR_DDI_LDL_LDRIVE_DRIVE_NOT_RECOVERABLE              (ERROR_DDI_LDL_GROUP + 27)

/* Cannot set the sector size to the given value. */
#define ERROR_DDI_LDL_LDRIVE_INVALID_SECTOR_SIZE                (ERROR_DDI_LDL_GROUP + 28)

/* Cannot do something because the drive is already initialized. */
#define ERROR_DDI_LDL_LDRIVE_ALREADY_INITIALIZED                (ERROR_DDI_LDL_GROUP + 29)

/* There is no drive with the given tag value. */
#define ERROR_DDI_LDL_LDRIVE_INVALID_DRIVE_TAG                  (ERROR_DDI_LDL_GROUP + 30)

/* An LDL iterator has no more items to return. */
#define ERROR_DDI_LDL_ITERATOR_DONE                             (ERROR_DDI_LDL_GROUP + 31)

/* An LDL API is unimplemented by the underlying media driver. */
#define ERROR_DDI_LDL_UNIMPLEMENTED                             (ERROR_DDI_LDL_GROUP + 32)

/* A file system format is required on the drive. */
#define ERROR_DDI_LDL_LDRIVE_FS_FORMAT_REQUIRED                 (ERROR_DDI_LDL_GROUP + 33)

/* Init media/datadrive failed */
#define ERROR_DDI_LDL_LDRIVE_REPAIR_REQUIRED                    (ERROR_DDI_LDL_GROUP + 34)

/* 
** Media buffer manager errors 
** Errors that the media buffer manager will return from its APIs.
*/

/* General error. Not used. */
#define ERROR_DDI_MEDIABUFMGR_GENERAL                           (ERROR_DDI_MEDIABUFMGR_GROUP)

/* There is no room to add another buffer to the buffer manager. */
#define ERROR_DDI_MEDIABUFMGR_NO_ROOM                           (ERROR_DDI_MEDIABUFMGR_GROUP + 1)

/* The buffer passed into buffer_release() was not obtained from the buffer manager. */
#define ERROR_DDI_MEDIABUFMGR_INVALID_BUFFER                    (ERROR_DDI_MEDIABUFMGR_GROUP + 2)

/* The buffer manager was unable to allocate a new temporary buffer for some reason. */
#define ERROR_DDI_MEDIABUFMGR_ALLOC_FAILED                      (ERROR_DDI_MEDIABUFMGR_GROUP + 3)

/* Attempt to read an unknown or invalid property. */
#define ERROR_DDI_MEDIABUFMGR_INVALID_PROPERTY                  (ERROR_DDI_MEDIABUFMGR_GROUP + 4)

/* 
** LBA-NAND errors 
*/

/* Internal errors returned from the LBA-NAND driver. */

/* Generic LDL error */
#define ERROR_DDI_LBA_NAND_GENERAL                              (ERROR_DDI_LBA_NAND_GROUP)

/* Unknown device type code */
#define ERROR_DDI_LBA_NAND_UNKNOWN_DEVICE_TYPE                  (ERROR_DDI_LBA_NAND_GROUP + 1)

/* Requested VFP size is too large. */
#define ERROR_DDI_LBA_NAND_VFP_SIZE_TOO_LARGE                   (ERROR_DDI_LBA_NAND_GROUP + 2)

/* Partition mode was not set as expected. */
#define ERROR_DDI_LBA_NAND_MODE_NOT_SET                         (ERROR_DDI_LBA_NAND_GROUP + 3)

/* Sector address is out of range. */
#define ERROR_DDI_LBA_NAND_ADDRESS_OUT_OF_RANGE                 (ERROR_DDI_LBA_NAND_GROUP + 4)

/* All spare blocks have been used. */
#define ERROR_DDI_LBA_NAND_SPARE_BLOCKS_EXHAUSTED               (ERROR_DDI_LBA_NAND_GROUP + 5)

/* Unknown VFP capacity model type was returned from the device. */
#define ERROR_DDI_LBA_NAND_UNKNOWN_VFP_CAPACITY_MODEL_TYPE      (ERROR_DDI_LBA_NAND_GROUP + 6)

/* VFP was allocated neither by normal nor EX commands. */
#define ERROR_DDI_LBA_NAND_VFP_SIZE_PARADOX                     (ERROR_DDI_LBA_NAND_GROUP + 7)

/* An attempted write command failed. */
#define ERROR_DDI_LBA_NAND_WRITE_FAILED                         (ERROR_DDI_LBA_NAND_GROUP + 8)

/* The VFP was not set to the expected size. */
#define ERROR_DDI_LBA_NAND_SET_VFP_SIZE_FAILED                  (ERROR_DDI_LBA_NAND_GROUP + 9)

/* 
** NAND driver errors
** Errors returned from the NAND driver through direct calls to it. Note that
** many of these errors are no longer reported and are here for historical
** information only. Also note that the NAND driver will return LDL errors from
** its functions that implement the logical drive and logical media interfaces. 
*/

/* Unused general error. */
#define ERROR_DDI_NAND_GROUP_GENERAL                                (ERROR_DDI_NAND_GROUP)

#define ERROR_DDI_NAND_LMEDIA_NO_REGIONS_IN_MEDIA                   (ERROR_DDI_NAND_GROUP + 1)

#define ERROR_DDI_NAND_LMEDIA_DRIVES_MAX_OUT                        (ERROR_DDI_NAND_GROUP + 2)

#define ERROR_DDI_NAND_LMEDIA_MEDIA_WRITE_PROTECTED                 (ERROR_DDI_NAND_GROUP + 3)

#define ERROR_DDI_NAND_LMEDIA_BAD_BLOCKS_MAX_OUT                    (ERROR_DDI_NAND_GROUP + 4)

#define ERROR_DDI_NAND_LMEDIA_NOT_ALLOCATED                         (ERROR_DDI_NAND_GROUP + 5)

#define ERROR_DDI_NAND_CONFIG_BLOCK_NOT_FOUND                       (ERROR_DDI_NAND_GROUP + 6)

#define ERROR_DDI_NAND_DATA_DRIVE_CANT_RECYCLE_USECTOR_MAP          (ERROR_DDI_NAND_GROUP + 15)

/* 
** The LBA in a page's metadata has an unexpected value.
** The LBA values in all the pages of a block must match, or this error will be returned. 
*/
#define ERROR_DDI_NAND_DATA_DRIVE_UBLOCK_LBAS_INCONSISTENT          (ERROR_DDI_NAND_GROUP + 16)

/* 
** The LSI in a page's metadata is out of range.
** The page is corrupted, or the wrong page was read. 
*/
#define ERROR_DDI_NAND_DATA_DRIVE_UBLOCK_HSECTORIDX_OUT_OF_RANGE    (ERROR_DDI_NAND_GROUP + 17)

/* 
** There are no available NSSMs.
** Usually caused by previous errors that didn't put the NSSM back into the LRU. Once
** enough of these errors pile up, there are no longer any NSSMs in the LRU. 
*/
#define ERROR_DDI_NAND_DATA_DRIVE_CANT_RECYCLE_UBLOCK               (ERROR_DDI_NAND_GROUP + 18)

#define ERROR_DDI_NAND_DATA_DRIVE_BBTABLE_FULL                      (ERROR_DDI_NAND_GROUP + 19)

#define ERROR_DDI_NAND_DATA_DRIVE_CANT_ALLOCATE_USECTORS_MAPS       (ERROR_DDI_NAND_GROUP + 33)

#define ERROR_DDI_NAND_DATA_DRIVE_INVALID_LOGICAL_SECTOR            (ERROR_DDI_NAND_GROUP + 59)

/* Not all chip enables are the same NAND type. */
#define ERROR_DDI_NAND_HAL_NANDTYPE_MISMATCH                        (ERROR_DDI_NAND_GROUP + 64)

/* This NAND is not supported by the HAL. */
#define ERROR_DDI_NAND_HAL_LOOKUP_ID_FAILED                         (ERROR_DDI_NAND_GROUP + 65)

/* The write or erase operation failed. */
#define ERROR_DDI_NAND_HAL_WRITE_FAILED                             (ERROR_DDI_NAND_GROUP + 71)

/* 
** The following three error codes are, and must remain, in ascending numerical order of severity.
** Data had bit errors, but ECC was able to correct them. 
*/
#define ERROR_DDI_NAND_HAL_ECC_FIXED                                (ERROR_DDI_NAND_GROUP + 81)

/* ECC was able to correct all bit errors, but the number of errors was above a threshold. */
#define ERROR_DDI_NAND_HAL_ECC_FIXED_REWRITE_SECTOR                 (ERROR_DDI_NAND_GROUP + 82)

/* Data had more bit errors than ECC could correct. */
#define ERROR_DDI_NAND_HAL_ECC_FIX_FAILED                           (ERROR_DDI_NAND_GROUP + 83)

/* DMA timed out. */
#define ERROR_DDI_NAND_DMA_TIMEOUT                                  (ERROR_DDI_NAND_GROUP + 87)

/* Block erase command failed. */
#define ERROR_DDI_NAND_ERASE_FAILED                                 (ERROR_DDI_NAND_GROUP + 88)

/* Reset command failed. */
#define ERROR_DDI_NAND_RESET_FAILED                                 (ERROR_DDI_NAND_GROUP + 89)

/* Returned when marking a bad block failed. */
#define ERROR_DDI_NAND_PROGRAM_FAILED                               (ERROR_DDI_NAND_GROUP + 90)

#define ERROR_DDI_NAND_D_BAD_BLOCK_TABLE_BLOCK_NOT_FOUND            (ERROR_DDI_NAND_GROUP + 95)

#define ERROR_DDI_NAND_CANT_ALLOCATE_DBBT_BLOCK                     (ERROR_DDI_NAND_GROUP + 96)

#define ERROR_DDI_NAND_MEDIA_FINDING_NEXT_VALID_BLOCK               (ERROR_DDI_NAND_GROUP + 97)

/* 
** Failed to place a boot block in its search area.
** This usually means that all blocks within a boot block's search area are bad. 
*/
#define ERROR_DDI_NAND_MEDIA_CANT_ALLOCATE_BCB_BLOCK                (ERROR_DDI_NAND_GROUP + 98)

#define ERROR_DDI_NAND_DRIVER_NO_BCB                                (ERROR_DDI_NAND_GROUP + 99)

#define ERROR_DDI_NAND_BCB_SEARCH_FAILED                            (ERROR_DDI_NAND_GROUP + 100)

#define ERROR_DDI_NAND_INVALID_BOOT_IMAGE_FORMAT                    (ERROR_DDI_NAND_GROUP + 101)

#define ERROR_DDI_NAND_NO_MATCHING_SECTION                          (ERROR_DDI_NAND_GROUP + 102)

/* Mapper is not initialized yet. */
#define ERROR_DDI_NAND_MAPPER_UNITIALIZED                           (ERROR_DDI_NAND_GROUP + 103)

#define ERROR_DDI_NAND_MAPPER_INVALID_PHYADDR                       (ERROR_DDI_NAND_GROUP + 104)

#define ERROR_DDI_NAND_MAPPER_GET_BLOCK_FAILED                      (ERROR_DDI_NAND_GROUP + 106)

#define ERROR_DDR_NAND_MAPPER_PHYMAP_MAPFULL                        (ERROR_DDI_NAND_GROUP + 107)

#define ERROR_DDI_NAND_MAPPER_FIND_LBAMAP_BLOCK_FAILED              (ERROR_DDI_NAND_GROUP + 109)

/* Lba is out of range. */
#define ERROR_DDI_NAND_MAPPER_LBA_OUTOFBOUND                        (ERROR_DDI_NAND_GROUP + 110)

/* Logical page address is out of range. */
#define ERROR_DDI_NAND_MAPPER_PAGE_OUTOFBOUND                       (ERROR_DDI_NAND_GROUP + 111)

#define ERROR_DDI_NAND_MAPPER_LBA_CORRUPTED                         (ERROR_DDI_NAND_GROUP + 113)

#define ERROR_DDI_NAND_MAPPER_ZONE_MAP_CACHE_INIT_FAILED            (ERROR_DDI_NAND_GROUP + 114)

#define ERROR_DDI_NAND_MAPPER_ZONE_MAP_INSUFFICIENT_NUM_FREE_BLOCKS (ERROR_DDI_NAND_GROUP + 115)

#define ERROR_DDI_NAND_DATA_DRIVE_SECTOR_OUT_OF_BOUNDS              (ERROR_DDI_NAND_GROUP + 118)

/* Failed to find the correct status. */
#define ERROR_DDI_NAND_HAL_CHECK_STATUS_FAILED                      (ERROR_DDI_NAND_GROUP + 119)

/* Cannot initiate another firmware refresh because one is already in progress. */
#define ERROR_DDI_NAND_FIRMWARE_REFRESH_BUSY                        (ERROR_DDI_NAND_GROUP + 120)

/* The version of the config block found on the NAND does not match the one expected by the firmware. */
#define ERROR_DDI_NAND_CONFIG_BLOCK_VERSION_MISMATCH                (ERROR_DDI_NAND_GROUP + 121)

/* Error codes from decoding software ecc encoded NCB block. */
#define ERROR_DDI_NAND_DRIVER_NCB_TRIPLE_RED_CHK_FAILED             (ERROR_DDI_NAND_GROUP + 122)

#define ERROR_DDI_NAND_DRIVER_NCB_SYNDROME_TABLE_MISMATCH           (ERROR_DDI_NAND_GROUP + 123)

#define ERROR_DDI_NAND_DRIVER_NCB_HAMMING_DOUBLE_ERROR              (ERROR_DDI_NAND_GROUP + 124)

/* The ONFI parameter page is corrupted. */
#define ERROR_DDI_NAND_HAL_INVALID_ONFI_PARAM_PAGE                  (ERROR_DDI_NAND_GROUP + 126)

#endif /* __di_media_erordefs_h__ */
/* EOF */
