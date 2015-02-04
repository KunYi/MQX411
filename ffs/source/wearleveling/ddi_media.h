#ifndef __ddi_media_h__
#define __ddi_media_h__
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
*   This file contains public interface for the Logical Drive Layer.
*
*
*END************************************************************************/

#include <wl_common.h>
#include "ddi_media_errordefs.h"
#include "drive_tags.h"

/* 
** Constants 
*/

/* Maximum number of entries in the media allocation table. */
#define MAX_MEDIA_TABLE_ENTRIES                 8

/* Maximum number of media supported at once. */
#define MAX_LOGICAL_MEDIA                       2

/* 
** Maximum number of drives supported at once.
** The current set of drives is as follows:
**  - Internal data
**  - External data
**  - Hidden 1
**  - Hidden 2
**  - System primary
**  - System secondary
**  - System master
** With a few extra slots added for safety, the maximum is set to 10. 
*/
#define MAX_LOGICAL_DRIVES                      8

/* Constant to be passed to erase functions. */
#define DDI_MAGIC_NUMBER                        0xADEADE

/* 
** Write types
** These values are passed to the read and write functions to give a hint as to
** the sequence of I/O operations that is being performed. 
*/
#define WRITE_TYPE_RANDOM                       0
#define WRITE_TYPE_SEQ_FIRST                    1
#define WRITE_TYPE_SEQ_NEXT                     2
#define WRITE_TYPE_NOREADBACK                   3

/* Number of Logical Media */
#define NUM_LOGICAL_MEDIA 1

/* Data drive ID */
#define DATA_DRIVE_ID_INTERNAL                  0
#define DATA_DRIVE_ID_EXTERNAL                  1

/* 
** Marcos 
*/

/* 4 Public API accessor macros to check external 'media, drive, and FS' init status.  */
#if (!defined(WIN32))

/* 
** Returns true if the external drive's FS is present and ready to be used, else false.
** See IsExternalDriveFsReady() in FSapi.h 
*/

/* Returns true if the external drive is init and present, else returns false. */
#define IsExternalDriveInit()           (g_bFrameworkExternalDriveOrFsInit)

/* 
** After exiting the media init pending state (can poll for media init no longer pending), 
** then this becomes valid so check this to see if media init result. 
*/
#define IsExternalMediaPresentAndInit() ((g_eFrameworkExternalMediaInit==eInitSucceeded)?true:false)

/* 
** if the external media's init attempt has not yet completed, returns true; 
** else the init attempt is complete so return false. 
*/
#define IsExternalMediaInitPending()    ((g_eFrameworkExternalMediaInit==eInitPending)? true:false)

#else 
#define IsExternalDriveInit()           false
#define IsExternalMediaPresentAndInit() false
#define IsExternalMediaInitPending()    false
#endif /* Endif (!defined(WIN32)) */

/* 
** Type definations 
*/

/* Abstract reference for a logical drive. This value should be unique for each drive. */
typedef uint8_t DriveTag_t;

/* State of media */
typedef enum {
    /* Unknow state */
    kMediaStateUnknown,
    
    /* Erased state */
    kMediaStateErased,
    
    /* Allocated state */
    kMediaStateAllocated
} MediaState_t;

/* 
** This initialization state type can be used to signify if media or drive 
** or FS init has been attempted yet and whether it succeeded.
** Note: See related SCSI_MEDIA_STATE (logical drive state) & its state machine 
** spec in scsi.h. and see g_eMtpExternalStoreState. 
*/
typedef enum {    
    /* Default state indicates that init attempt hasn't completed yet, so can wait on it. */
    eInitPending = 0,
    
    /* Initialized failed */
    eInitFailed,
    
    /* Initialized. */
    eInitSucceeded,
    
    /* After a shutdown, the state of entity initialization is terminated. */
    eInitTerminated
} InitializationState_t; 

/* An enumeration of physical storage media types supported by STMP3xxx SDK5 */
typedef enum {
    /* NAND flash is typically the primary storage media */
    kMediaTypeNand,
    
    /* MultiMedia Card (MMC) or Secure Digital (SD) card may be removable. */
    kMediaTypeMMC,
    
    /* Hard Disk Drive (HDD) spinning random access R/W storge media */
    kMediaTypeHDD,
    
    /* RAM based virtual disk for mounting an FS in fast access RAM (if extra available). */
    kMediaTypeRAM,
    
    /* Toshiba LBA-NAND device. */
    kMediaTypeLbaNand
} PhysicalMediaType_t;

/* 
** Logical media info 
*/
enum _logical_media_info
{
    /* Count of drives belonging to this media. (uint32_t) */
    kMediaInfoNumberOfDrives = 0,
    
    /* Total size of the media in bytes. (uint64_t) */
    kMediaInfoSizeInBytes = 1,
    
    /* 
    ** Size in bytes of the media's sectors. (uint32_t)
    ** This sector size is always "native". 
    ** Only drives have the concept of a nominal sector size. 
    */
    kMediaInfoAllocationUnitSizeInBytes = 2,
    
    /* This selector is an alias to #kMediaInfoAllocationUnitSizeInBytes. */
    kMediaInfoSectorSizeInBytes = kMediaInfoAllocationUnitSizeInBytes,
    
    /* True if the media has been initialized successfully. (bool) */
    kMediaInfoIsInitialized = 3,
    
    /* Whether the media is erased or allocated. (MediaState_t) */
    kMediaInfoMediaState = 4,
    
    /* True if the media is read-only. (bool) */
    kMediaInfoIsWriteProtected = 5,
    
    /* The type of storage device for this media. (PhysicalMediaType_t) */
    kMediaInfoPhysicalMediaType = 6,
    
    /* Number of bytes for the serial number. (uint32_t) */
    kMediaInfoSizeOfSerialNumberInBytes = 7,
    
    /* 
    ** The serial number value.
    ** The length of this property is the value returned by 
    ** getting the #kMediaInfoSizeOfSerialNumberInBytes selector. 
    */
    kMediaInfoSerialNumber = 8,
    
    /* Whether the media has any system drives belonging to it. (bool) */
    kMediaInfoIsSystemMedia = 9,
    
    /* 
    ** Whether the media is currently mounted. (bool)
    ** This value will always be true unless the media is removable. 
    */
    kMediaInfoIsMediaPresent = 10,

    /* 
    ** Total size of the NAND page in bytes. (uint32_t)
    ** This property only applies to NAND media. It's value is the size of the entire NAND
    ** page in bytes, including both the data and metadata/redundant portion of the page.
    ** Common page sizes are 2112, 4224, and 4314 bytes. 
    */
    kMediaInfoPageSizeInBytes = 11,
    
    /* 
    ** NAND manufacturer ID. (uint32_t)
    ** For raw NAND, this value is the first byte of the NAND Read ID command results.
    ** For eMMC, this value comes from the CID register. 
    */
    kMediaInfoMediaMfgId = 12,
    
    /* NAND Read ID command results. (uint64_t) */
    kMediaInfoIdDetails = 13,
    
    /* 
    ** Active number of chip enables. (uint32_t)
    ** This property only has any meaning if the media uses chip enables. 
    */
    kMediaInfoNumChipEnables = 14,
    
    /* Whether the media is removable. (bool) */
    kMediaInfoIsRemovable = 15,
    
    /* 
    ** Expected transfer activity in this drive. (TransferActivityType_t)
    ** The value should be one of:
    **  - #kTransferActivity_Random
    **  - #kTransferActivity_Sequential 
    */
    kMediaInfoExpectedTransferActivity = 17,
    
    /* Selector for enabling the power saving feature in the media, driver specific. (bool) */
    kMediaInfoLbaNandIsPowerSaveForcedOn = 18,
    
    /* 
    ** Size in bytes of metadata portion of media sector. (uint32_t)    
    ** If the media has sectors with metadata, such as with NAND pages, this value will be
    ** the size in bytes of that metadata.  
    */
    kMediaInfoSectorMetadataSizeInBytes = 19,
    
    /* 
    ** Full size of the media's sectors in bytes. (uint32_t)
    ** This property includes both the data and metadata, if any, portions of the media's
    ** sectors. For NANDs, this would be the full page size. If the media does not have
    ** metadata, then this value will be the same as #kMediaInfoSectorSizeInBytes. 
    */
    kMediaInfoSectorTotalSizeInBytes = kMediaInfoPageSizeInBytes,
    
    /* 
    ** Whether the driver allows the media device to enter sleep.
    ** Normally this property is set to true. The driver will use its normal algorithms to put
    ** the device to sleep when it is not being used. But if this property is set to false, then
    ** the driver will not allow the device to enter its sleep mode. 
    */
    kMediaInfoIsSleepAllowed = 20,

    /* 
    ** Device Product Name (32 bytes)
    ** For eMMC, this value comes from the CID register. For ONFI NANDs, this value comes from
    ** the parameter page. Not all devices will return a value.
    ** The resulting value is a null-terminated string with a maximum of 31 characters plus the
    ** NULL byte. 
    */
    kMediaInfoProductName = 21
};

/* 
** List of drive info selectors.
** These selectors are passed to DriveGetInfo() and DriveSetInfo() to read and write
** certain fields per drive. Not all selectors apply to every drive type or media type,
** and only a few selectors are mutable with DriveSetInfo(). 
*/
enum _logical_drive_info
{
    /* 
    ** Nominal sector size in bytes for this drive. (uint32_t, r/w)    
    ** This sector size is the "nominal" sector size. It may be different (i.e., smaller)
    ** from the native sector size that is determined by the underlying media. Use this value
    ** if you are using the media cache to read and write.
    **
    ** The drive's nominal sector size can be overridden by a call to
    ** DriveSetInfo(). The new value must be a power of two between 512 and the value of
    ** the #kDriveInfoNativeSectorSizeInBytes selector. However, the sector size can only
    ** be set \i before the given drive is initialized.
    **
    ** Return:
    **  - SUCCESS
    **  - ERROR_DDI_LDL_LDRIVE_INVALID_SECTOR_SIZE
    **  - ERROR_DDI_LDL_LDRIVE_ALREADY_INITIALIZED 
    */
    kDriveInfoSectorSizeInBytes = 0,
    
    /* 
    ** The size of the minimum area that can be erased. (uint32_t, r/o)
    **
    ** This value corresponds to the size of a block for a NAND drive. On media that don't
    ** have erase regions different from sectors, this will be the size of a native sector. 
    */
    kDriveInfoEraseSizeInBytes = 1,
    
    /* Total size of the drive in bytes. (uint64_t, r/o) */
    kDriveInfoSizeInBytes = 2,
    
    /* Total size of the drive in megabytes. (uint32_t, r/o) */
    kDriveInfoSizeInMegaBytes = 3,
    
    /* Number of nominal sectors that the drive occupies. (uint32_t, r/o) */
    kDriveInfoSizeInSectors = 4,
    
    /* 
    ** The type of the drive. (LogicalDriveType_t, r/o)
    **
    ** Drives may be one of several distict types. Those are data drives, hidden data drives,
    ** and system drives. Both data and hidden data drives 
    */
    kDriveInfoType = 5,
    
    /* Unique tag value for the drive. (uint32_t, r/w) */
    kDriveInfoTag = 6,
    
    /* Component version number. (SystemVersion_t, r/o) */
    kDriveInfoComponentVersion = 7,
    
    /* Project version number. (SystemVersion_t, r/o) */
    kDriveInfoProjectVersion = 8,
    
    /* Whether the drive is read only. (bool, r/o) */
    kDriveInfoIsWriteProtected = 9,
    
    /* Number of bytes occupied by the serial number. (uint32_t, r/o) */
    kDriveInfoSizeOfSerialNumberInBytes = 10,
    
    /* 
    ** Unique serial number for the drive in ASCII form. (r/o)
    **
    ** The size of this property's value is the value of the 
    ** #kDriveInfoSizeOfSerialNumberInBytes
    ** property. 
    */
    kDriveInfoSerialNumber = 11,
    
    /* Whether the drive's media is currently present. (bool, r/o) */
    kDriveInfoMediaPresent = 12,
    
    /* Whether the media has changed recently. (bool, r/o) */
    kDriveInfoMediaChange = 13,
    
    /* Size in bytes of the raw serial number. (uint32_t, r/o) */
    kDriveInfoSizeOfRawSerialNumberInBytes = 15,
    
    /* Unique serial number for the drive in raw form. (r/o)
    ** The #kDriveInfoSizeOfRawSerialNumberInBytes property specifies the size in bytes of
    ** this property's value. 
    */
    kDriveInfoRawSerialNumber = 16,

    /* 
    ** Selector for the number of non-sequential sector maps the drive uses. (uint32_t, r/w)
    **
    ** The parameter is a single uint32_t value that is the count of NSS maps for
    ** the given drive. DriveGetInfo() with this selector will just return the current
    ** count of maps. The array of maps is shared between all data-type drives.
    ** DriveSetInfo() allows you to change the number
    ** of maps to the value passed in. All previous maps will be
    ** flushed immediately and memory for the maps reallocated.
    **
    ** Return:
    **  - SUCCESS
    **  - ERROR_DDI_NAND_DATA_DRIVE_CANT_ALLOCATE_USECTORS_MAPS
    **
    ** Note: This selector only applies to data drives. System drives will return an error. 
    */
    kDriveInfoNSSMCount = 19,
    
    /* 
    ** Sector size in bytes that the underlying media has given this drive. (uint32_t, r/o)
    
    ** Use the native sector size when working with DriveReadSector(), DriveWriteSector(),
    ** MediaRead(), and MediaWrite(). */
    kDriveInfoNativeSectorSizeInBytes = 20,
    
    /* Number of native sectors consumed by this drive. (uint32_t, r/o) */
    kDriveInfoSizeInNativeSectors = 21,
    
    /* 
    ** Offset to beginning of this partition from the first sector of its parent drive. 
    ** (uint32_t, r/w) 
    */
    kDriveInfoSectorOffsetInParent = 23
};

enum _ldl_media_numbers
{
    /* Media number for the internal, non-removable media. */
    kInternalNandMedia = 0,
    
    /* kInternalNorMedia, */
    
    /* Media number for external media, usually SD/MMC. */
    kExternalMedia
};

/* Possible types of a logical drive. */
typedef enum {
    /* 
    ** Public data drive.
    **
    ** Read/write with random access. Typically accessible through the filesystem. Accessible via
    ** USB mass storage and MTP. 
    */
    kDriveTypeData = 0,
    
    /* 
    ** System drive.
    **
    ** Designed to hold firmware or system resources that are accessed frequently but written very
    ** rarely. Must be erased before it can be written, and sectors must be written in sequential
    ** order with none skipped. Also, each sector can only be written to once. 
    */
    kDriveTypeSystem,
    
    /* 
    ** Hidden data drive.
    ** 
    ** Similar to #kDriveTypeData, except that hidden data drives are not accessible through
    ** USB mass storage or MTP. 
    */
    kDriveTypeHidden,
    
    /* 
    ** Unknown drive type.
    **
    ** Drives should never be this type. 
    */
    kDriveTypeUnknown
} LogicalDriveType_t;

/* Describes one drive in a media allocation table. */
typedef struct {
    /* Drive number */
    uint32_t u32DriveNumber;
    
    /* Type of logical drive */
    LogicalDriveType_t Type;
    
    /* Tag */
    uint32_t u32Tag;
    
    /* Size in bytes */
    uint64_t u64SizeInBytes;
    
    /* Required flag */
    bool bRequired;
} MediaAllocationTableEntry_t;

/* 
** Table used to list details of a media's drives.
**
** This table is used both for allocation to specify the set of drives that should be created
** on the media, as well as to get the list of a media's current set of drives. 
*/
typedef struct {
    /* Number of entries */
    uint32_t u32NumEntries;
    
    /* Array of media table entries */
    MediaAllocationTableEntry_t Entry[MAX_MEDIA_TABLE_ENTRIES];
} MediaAllocationTable_t;

/* System version */
typedef union {
    /* Partial Version */
    struct {
        uint8_t MinorL;
        uint8_t MinorH;
        uint8_t MiddleL;
        uint8_t MiddleH;
        uint8_t MajorL;
        uint8_t MajorH;
    } partialVersion;
    
    /* Version */
    uint64_t   Version;
} SystemVersion_t;

/* Possible states for a logical drive. */
typedef enum {
    /* Drive which is not present */
    kDriveNotPresent,
    
    /* Drive which is uninitialized */
    kDriveUninitialized,
    
    /* Drive which is initialized failed */
    kDriveFailedInitialization,
    
    /* Drive which is ready */
    kDriveReady
} DriveState_t;

/* An opaque type for a drive iterator. */
typedef struct OpaqueDriveIterator * DriveIterator_t;

/* Values for #kDriveInfoExpectedTransferActivity media info property. */
typedef enum{
    /* Most accesses will be sequential to one another. */
    kTransferActivity_Sequential = 0,
    
    /* There will be no predictable order to accesses. */
    kTransferActivity_Random
}TransferActivityType_t;

/* Struct used by application to declare media types. */
typedef struct _MediaDefinition {
    /* Media number */
    unsigned m_mediaNumber;
    
    /* Media type */
    PhysicalMediaType_t m_mediaType;
    
    /* Removealbe flag */
    bool m_isRemovable;
} MediaDefinition_t;

/* 
** External references 
*/

extern bool                  g_bFrameworkExternalDriveOrFsInit;
extern InitializationState_t    g_eFrameworkExternalMediaInit;
extern const int                g_MinDataDriveSize;
extern const int                g_MinDataDriveSize;

/* 
** Function prototypes 
*/

#ifdef __cplusplus
extern "C" {
#endif /* Endif __cplusplus */

RtStatus_t NandHalSetMqxPtr(uint8_t, MQX_FILE_PTR);

unsigned MediaGetCount(void);

RtStatus_t MediaGetInfoSize(unsigned mediaNumber, uint32_t selector, uint32_t * propertySize);

RtStatus_t MediaSetInfo(unsigned mediaNumber, uint32_t selector, const void * value);

uint32_t MediaGetMaximumSectorSize(void);

// TODO: add MediaGetMaximumAuxSize function to check there is enough space for metadata or not
uint32_t MediaGetMaximumAuxSize(void); 

RtStatus_t MediaInit(uint32_t u32LogMediaNumber);

RtStatus_t MediaBuildFromPreDefine(void);

RtStatus_t MediaAllocateFromPreDefine(uint32_t u32LogMediaNumber, MediaAllocationTable_t * pMediaTable);

RtStatus_t MediaAllocateFromPreDefineRegions(uint32_t u32LogMediaNumber);

RtStatus_t MediaGetMediaTable(uint32_t u32LogMediaNumber, MediaAllocationTable_t ** pMediaTable);

RtStatus_t MediaFreeMediaTable(uint32_t mediaNumber, MediaAllocationTable_t * table);

RtStatus_t MediaGetInfo(uint32_t u32LogMediaNumber, uint32_t Type, void * pInfo);

RtStatus_t MediaErase(uint32_t u32LogMediaNumber, uint32_t u32MagicNumber, uint8_t u8NoEraseHidden);

RtStatus_t MediaShutdown(uint32_t u32LogMediaNumber);

RtStatus_t MediaFlushDrives(uint32_t u32LogMediaNumber);

RtStatus_t MediaSetBootDrive(uint32_t u32LogMediaNumber, DriveTag_t u32DriveTag);

unsigned DriveGetCount(void);

RtStatus_t DriveCreateIterator(DriveIterator_t * iter);

RtStatus_t DriveIteratorNext(DriveIterator_t iter, DriveTag_t * tag);

void DriveIteratorDispose(DriveIterator_t iter);

DriveState_t DriveGetState(DriveTag_t tag);

RtStatus_t DriveInitAll(void);

RtStatus_t DriveGetInfoSize(DriveTag_t tag, uint32_t selector, uint32_t * propertySize);

RtStatus_t DriveInit(DriveTag_t tag);

RtStatus_t DriveGetInfo(DriveTag_t tag, uint32_t Type, void * pInfo);

RtStatus_t DriveSetInfo(DriveTag_t tag, uint32_t Type, const void * pInfo);

RtStatus_t DriveReadSector(DriveTag_t tag, uint32_t u32SectorNumber, SECTOR_BUFFER * pSectorData);

RtStatus_t DriveWriteSector(DriveTag_t tag, uint32_t u32SectorNumber, const SECTOR_BUFFER * pSectorData);

RtStatus_t DriveErase(DriveTag_t tag, uint32_t u32MagicNumber);

RtStatus_t DriveRepair(DriveTag_t tag, uint32_t u32MagicNumber, bool bIsScanBad);

RtStatus_t DriveFlush(DriveTag_t tag);

RtStatus_t DriveShutdown(DriveTag_t tag);

/* 
** Media task tracking
**
** These functions are used by the high level code to provide debugging information
** about the context of media operations.
**
** These functions are only available in debug builds. In release builds they
** simply turn into empty stubs.
** Set this macro to 1 to enable media task tracking. 
*/
#define DDI_LDL_ENABLE_MEDIA_TASKS              0

// #if DEBUG && DDI_LDL_ENABLE_MEDIA_TASKS
void ddi_ldl_push_media_task(const char * taskName);
void ddi_ldl_pop_media_task(void);
unsigned ddi_ldl_get_media_task_count(void);
unsigned ddi_ldl_get_media_task_stack(const char ** tasks, unsigned maxTasks);
// #else
// void ddi_ldl_push_media_task(const char * taskName) {}
// void ddi_ldl_pop_media_task(void) {}
// unsigned ddi_ldl_get_media_task_count(void) { return 0; }
// unsigned ddi_ldl_get_media_task_stack(const char ** tasks, unsigned maxTasks) { return 0; }
// #endif /* Endif DEBUG && DDI_LDL_ENABLE_MEDIA_TASKS */

#ifdef __cplusplus
} /* extern "C" */

/* 
** Helper for media tasks. 
*/
class MediaTask
{
public:
    MediaTask(const char * name)
    {
        ddi_ldl_push_media_task(name);
    }
    
    ~MediaTask()
    {
        ddi_ldl_pop_media_task();
    }
};

/* Convenient template form of MediaGetInfo(). */
template <typename T>
inline T MediaGetInfoTyped(uint32_t mediaNumber, uint32_t selector)
{
    T value = 0;
    MediaGetInfo(mediaNumber, selector, &value);
    return value;
}

/* Convenient template form of MediaGetInfo(). */
template <typename T>
inline T MediaGetInfoTyped(uint32_t mediaNumber, uint32_t selector, RtStatus_t & status)
{
    T value = 0;
    status = MediaGetInfo(mediaNumber, selector, &value);
    return value;
}

/* Convenient template form of DriveGetInfo(). */
template <typename T> inline T DriveGetInfoTyped(DriveTag_t drive, uint32_t propertyName)
{
    T value = 0;
    DriveGetInfo(drive, propertyName, (void *)&value);
    return value;
}

/* Convenient template form of DriveGetInfo(). */
template <typename T> inline T DriveGetInfoTyped(DriveTag_t drive, uint32_t propertyName, RtStatus_t & status)
{
    T value = 0;
    status = DriveGetInfo(drive, propertyName, (void *)&value);
    return value;
}

/* 
** Convenient template form of DriveSetInfo().
** This function is named differently than DriveSetInfo() because the GHS C++ compiler
** wants to create an instatiation loop if they're named the same. Maybe this is because
** the base function uses C linkage? 
*/
template <typename T> inline RtStatus_t DriveSetInfoTyped(DriveTag_t drive, uint32_t propertyName, T value)
{
    return DriveSetInfo(drive, propertyName, (const void *)&value);
}

#endif /* __cplusplus */

#endif /* __ddi_media_h__ */
/* EOF */
