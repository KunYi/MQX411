#ifndef __ddi_media_internal_h__
#define __ddi_media_internal_h__
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
*   This file contains private interface for the Logical Drive Layer.
*
*
*END************************************************************************/

#include "ddi_media.h"
#include <string.h>
/* #include "ddi/media/ddi_nand_media.h" */

/* 
** Type definations 
*/

#if defined (__cplusplus)

/*
** Properties of a logical media.
**
** Discovered at runtime by MediaInit().
** - One per instance of the driver.
** - This struct is used mostly by the LDL.
** - pMediaInfo points to a private driver info struct. In the MMC case, this is a 
** ddi_mmc_media_info_t.
*/
class LogicalMedia
{
public:
    
    /* Default constructor. */
    LogicalMedia();
    
    /* Destructor. */
    virtual ~LogicalMedia();

    /* Logical media API */
    virtual RtStatus_t init() = 0;
    virtual RtStatus_t allocate(MediaAllocationTable_t * table) = 0;
    virtual RtStatus_t allocateFromPreDefine(MediaAllocationTable_t * table) = 0;
    virtual RtStatus_t allocateFromPreDefineRegions() = 0;
    virtual RtStatus_t discover() = 0;
    virtual RtStatus_t getMediaTable(MediaAllocationTable_t ** table) = 0;
    virtual RtStatus_t freeMediaTable(MediaAllocationTable_t * table);
    virtual RtStatus_t getInfoSize(uint32_t infoSelector, uint32_t * infoSize);
    virtual RtStatus_t getInfo(uint32_t infoSelector, void * value);
    virtual RtStatus_t setInfo(uint32_t infoSelector, const void * value);
    virtual RtStatus_t erase() = 0;
    virtual RtStatus_t shutdown() = 0;
    virtual RtStatus_t flushDrives() = 0;
    virtual RtStatus_t setBootDrive(DriveTag_t tag) = 0;
    
    /* Accessors */
    uint32_t getMediaNumber() const { return m_u32MediaNumber; }
    bool isInitialized() const { return m_bInitialized; }
    bool isWriteProtected() const { return m_bWriteProtected; }
    bool isRemovable() const { return m_isRemovable; }
    bool isAllocated() const { return m_bAllocated; }
    MediaState_t getState() const { return m_eState; }
    uint32_t getNumberOfDrives() const { return m_u32NumberOfDrives; }
    void setNumberOfDrives(uint32_t count) { m_u32NumberOfDrives = count; }
    uint64_t getSizeInBytes() const { return m_u64SizeInBytes; }
    uint32_t getAllocationUnitSizeInBytes() const { return m_u32AllocationUnitSizeInBytes; }
    PhysicalMediaType_t getPhysicalType() const { return m_PhysicalType; }

public:

    uint32_t m_u32MediaNumber;
    bool m_bInitialized;
    MediaState_t m_eState;
    bool m_bAllocated;
    bool m_bWriteProtected;
    bool m_isRemovable;
    
    /* Includes ALL drive types on this media. */
    uint32_t m_u32NumberOfDrives;
    
    uint64_t m_u64SizeInBytes;
    uint32_t m_u32AllocationUnitSizeInBytes;
    PhysicalMediaType_t m_PhysicalType;

};

/*
** Properties of a logical drive.
**
** Discovered at runtime by MediaDiscover().
**
** - Again, used mostly by the LDL.
** - One per accessibly region of the device.
** - A drive may not necessarily be equivalent to a partition, since the drive may encompass all
** sectors of a media, including the MBR. This is normally the way the MMC/SD drive is set up; it's
** sector count matches that of its media.
** - Drives may overlap. So you can have one drive that is for the entire media and contains all
** partitions, and another drive that is just one partition on the media.
** - pLogicalMediaDescriptor points to the LogicalMedia_t for that device containing the drive. All
** logical drives belonging to the same device (i.e., all partitions of a device) must point to the
** same LogicalMedia_t.
** - pMediaInfo has the same value as pLogicalMediaDescriptor->pMediaInfo.
** - pDriveInfo points to a private driver struct. For MMC, this is the ddi_mmc_drive_info_t struct.
** - Native sectors are the actual sectors read from/written to the device. The media driver always
** uses native sectors.
** - Nominal sectors are what our filesystem uses, as presented by the media cache.
*/
class LogicalDrive
{
public:
    
    /* Default constructor. */
    LogicalDrive();
    
    /* Destructor. */
    virtual ~LogicalDrive();
    
    /* Logical drive API */
    virtual RtStatus_t init() = 0;
    virtual RtStatus_t shutdown() = 0;
    virtual RtStatus_t getInfoSize(uint32_t infoSelector, uint32_t * infoSize);
    virtual RtStatus_t getInfo(uint32_t infoSelector, void * value);
    virtual RtStatus_t setInfo(uint32_t infoSelector, const void * value);
    virtual RtStatus_t readSector(uint32_t sector, SECTOR_BUFFER * buffer) = 0;
    virtual RtStatus_t writeSector(uint32_t sector, const SECTOR_BUFFER * buffer) = 0;
    virtual RtStatus_t erase() = 0;
    virtual RtStatus_t flush() = 0;
    virtual RtStatus_t repair(bool bIsScanBad = false) = 0;
    
    /* Accessors */
    bool isInitialized() const { return m_bInitialized; }
    bool isErased() const { return m_bErased; }
    bool didFailInit() const { return m_bFailedInit; }
    void setDidFailInit(bool didFail) { m_bFailedInit = didFail; }
    uint32_t getSectorCount() const { return m_u32NumberOfSectors; }
    uint32_t getNativeSectorCount() const { return m_numberOfNativeSectors; }
    uint32_t getSectorSize() const { return m_u32SectorSizeInBytes; }
    uint32_t getNativeSectorSize() const { return m_nativeSectorSizeInBytes; }
    uint32_t getNativeSectorShift() const { return m_nativeSectorShift; }
    uint32_t getEraseSize() const { return m_u32EraseSizeInBytes; }
    uint64_t getSizeInBytes() const { return m_u64SizeInBytes; }
    uint32_t getPbsStartSector() const { return m_pbsStartSector; }
    LogicalDriveType_t getType() const { return m_Type; }
    DriveTag_t getTag() const { return m_u32Tag; }
    LogicalMedia * getMedia() { return m_logicalMedia; }
    DriveState_t getState() const;
    
    /* Template forms */
    template <typename T> inline T getInfo(uint32_t selector)
    {
        T value;
        getInfo(selector, (void *)&value);
        return value;
    }

    template <typename T> inline T getInfo(uint32_t selector, RtStatus_t & status)
    {
        T value;
        status = getInfo(selector, (void *)&value);
        return value;
    }
    
    template <typename T> inline RtStatus_t setInfo(uint32_t selector, T value)
    {
        return setInfo(selector, (const void *)&value);
    }

public:

    /* True if the drive has been inited. */
    bool m_bInitialized;
    
    /* 
    ** True if an attempt was made to init the drive but it failed for some reason. 
    ** Ignored if bInitialized is true. 
    */
    bool m_bFailedInit;
    
    /* Indicates if a system drive is present. */
    bool m_bPresent;
    
    bool m_bErased;
    bool m_bWriteProtected;
    uint32_t m_u32NumberOfSectors;
    LogicalDriveType_t m_Type;
    DriveTag_t m_u32Tag;
    
    /* Total drive size in bytes. */
    uint64_t m_u64SizeInBytes;
    
    /* Nominal sector size, can be configured by the application in some cases. */
    uint32_t m_u32SectorSizeInBytes;
    
    /* Native sector size, determined by the underlying media driver. */
    uint32_t m_nativeSectorSizeInBytes;
    
    /* Number of native sectors big the drive is. */
    uint32_t m_numberOfNativeSectors;
    
    /* Shift to convert between native and nominal sector sizes. */
    uint32_t m_nativeSectorShift;

    uint32_t m_u32EraseSizeInBytes;
    
    /* Offset in nominal sectors to the PBS. */
    uint32_t m_pbsStartSector;
    
    /* Logical media that owns this drive. */
    LogicalMedia * m_logicalMedia;

};

/*
** Set of available media and drives.
*/
struct LdlInfo
{
    uint32_t m_mediaCount;
    LogicalMedia * m_media[MAX_LOGICAL_MEDIA];
    uint32_t m_driveCount;
    LogicalDrive * m_drives[MAX_LOGICAL_DRIVES];
};

extern LdlInfo g_ldlInfo;

/* Definition of logical media factory function. */
typedef LogicalMedia * (*LogicalMediaFactoryFunction_t)(const MediaDefinition_t & def);

#endif /* __cplusplus */

/* 
** Global external declarations 
*/

extern const MediaDefinition_t g_mediaDefinition[];
extern MediaAllocationTable_t g_MediaAllocationTable[];

#ifdef RTOS_THREADX
extern TX_MUTEX g_NANDThreadSafeMutex;
#endif /* RTOS_THREADX */

/* 
** Function prototypes 
*/

#if defined(__cplusplus)

LogicalMedia * MediaGetMediaFromIndex(unsigned index);

LogicalDrive * DriveGetDriveFromTag(DriveTag_t tag);

LogicalDrive ** DriveFindEmptyEntry(void);

RtStatus_t DriveAdd(LogicalDrive * newDrive);

RtStatus_t DriveRemove(DriveTag_t driveToRemove);

#endif /* __cplusplus */

#endif /* __ddi_media_internal_h__ */
/* EOF */
