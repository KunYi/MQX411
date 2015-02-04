#ifndef __mfs_prv_h__
#define __mfs_prv_h__
/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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
*   This file contains the structure definitions and constants
*   that are internal to the Embedded MS-DOS File System (MFS)
*
*
*END************************************************************************/

#include <partition.h>

/*
** Defines specific to MFS
*/
#define MFS_EXTRACT_UTF8      0

#define BOOT_SECTOR           0
#define FSINFO_SECTOR         1     /* FAT32 only */
#define BKBOOT_SECTOR         6     /* FAT32 only */

#define MFS_SECTOR_READ       5
#define MFS_SECTOR_WRITE      10

#define MFS_DOS30_JMP         0xEB /* 2 byte jmp 80x86 opcode */
#define MFS_DOS30_B           0xE9 /* 2 byte b 80x86 opcode */
#define MFS_DEL_FILE          0xE5 /* DOS byte used for a deleted file */

/* FAT 32 Filesystem info signatures */
#define FSI_LEADSIG           0x41615252UL
#define FSI_STRUCTSIG         0x61417272UL
#define FSI_TRAILSIG          0xAA550000UL
#define FSI_UNKNOWN           0xFFFFFFFFUL

/* This is the vesion of tfe FAT32 driver */
#define MFS_FAT32_VER         0x0000


#define MFS_LFN_END           0x40

#define ATTR_EXCLUSIVE        0x40
#define MFS_ATTR_ANY          0x80

/* defines used in FAT   */
#define CLUSTER_INVALID       0x00000001L
#define CLUSTER_UNUSED        0x00000000L
#define CLUSTER_MIN_GOOD      0x00000002L
#define CLUSTER_MAX_GOOD      0xFFFFFEFUL
#define CLUSTER_MIN_RESERVED  0xFFFFFF0UL
#define CLUSTER_MAX_RESERVED  0xFFFFFF6UL
#define CLUSTER_BAD           0xFFFFFF7UL
#define CLUSTER_MIN_LAST      0xFFFFFF8UL
#define CLUSTER_EOF           0xFFFFFFFUL
#define CLUSTER_MAX_12        0x0000FF8UL
#define CLUSTER_MAX_16        0x000FFF8UL
#define CLUSTER_MAX_32        0xFFFFFF8UL

#define MFS_VALID             0x4d465356UL

#ifndef min
    #define min(a,b) ((a<b)?a:b)
#endif

#define MFS_set_error_and_return(error_ptr, error, retval) { if (error_ptr != NULL) { *error_ptr = error;} return(retval ); }
#define ROOT_CLUSTER(drive_ptr) (drive_ptr->FAT_TYPE == MFS_FAT32 ? drive_ptr->BPB32.ROOT_CLUSTER : 0)
#define MFS_LOG(x)  // puts(x);


/* structure of a long file name slot */
typedef struct mfs_lname_enty
{
    unsigned char    ID;
    unsigned char    NAME0_4[10];
    unsigned char    ATTR;
    unsigned char    RESERVE;
    unsigned char    ALIAS_CHECKSUM;
    unsigned char    NAME5_10[12];
    unsigned char    START[2];
    unsigned char    NAME11_12[4];
} MFS_LNAME_ENTRY, * MFS_LNAME_ENTRY_PTR;



/* structure of the BIOS Parameter Block maintained in the
** boot sector as stored in drive.
*/
typedef struct bios_param_struct
{
    char     RESERVED[11];
    unsigned char    RESERVED2;
    uint16_t  SECTOR_SIZE;
    unsigned char    SECTORS_PER_CLUSTER;
    unsigned char    RESERVED3;
    uint16_t  RESERVED_SECTORS;
    unsigned char    NUMBER_OF_FAT;
    unsigned char    RESERVED4;
    uint16_t  ROOT_ENTRIES;
    uint16_t  NUMBER_SECTORS;
    unsigned char    MEDIA_TYPE;
    unsigned char    RESERVED5;
    uint16_t  SECTORS_PER_FAT;
    uint16_t  SECTORS_PER_TRACK;
    uint16_t  NUM_HEADS;
    uint32_t  HIDDEN_SECTORS;
    uint32_t  MEGA_SECTORS;
} BIOS_PARAM_STRUCT, * BIOS_PARAM_STRUCT_PTR;

/* structure of the BIOS Parameter Block maintained in the
** boot sector as read off the disk
*/
typedef struct bios_param_struct_disk
{
    char     RESERVED[11];
    unsigned char    SECTOR_SIZE[2];
    unsigned char    SECTORS_PER_CLUSTER[1];
    unsigned char    RESERVED_SECTORS[2];
    unsigned char    NUMBER_OF_FAT[1];
    unsigned char    ROOT_ENTRIES[2];
    unsigned char    NUMBER_SECTORS[2];
    unsigned char    MEDIA_TYPE[1];
    unsigned char    SECTORS_PER_FAT[2];
    unsigned char    SECTORS_PER_TRACK[2];
    unsigned char    NUM_HEADS[2];
    unsigned char    HIDDEN_SECTORS[4];
    unsigned char    MEGA_SECTORS[4];
} BIOS_PARAM_STRUCT_DISK, * BIOS_PARAM_STRUCT_DISK_PTR;

/* structure of the BIOS Parameter Block maintained in the
** boot sector as read off the disk (FAT32 extension)
*/
typedef struct bios_param32_struct_disk
{
    unsigned char    FAT_SIZE[4];
    unsigned char    EXT_FLAGS[2];
    unsigned char    FS_VER[2];
    unsigned char    ROOT_CLUSTER[4];
    unsigned char    FS_INFO[2];
    unsigned char    BK_BOOT_SEC[2];
    unsigned char    RESERVED[12];
} BIOS_PARAM32_STRUCT_DISK, * BIOS_PARAM32_STRUCT_DISK_PTR;


typedef struct bios_param32_struct
{
    uint32_t  FAT_SIZE;
    uint16_t  EXT_FLAGS;
    uint16_t  FS_VER;
    uint32_t  ROOT_CLUSTER;
    uint16_t  FS_INFO;
    uint16_t  BK_BOOT_SEC;
    unsigned char    RESERVED[12];
} BIOS_PARAM32_STRUCT, * BIOS_PARAM32_STRUCT_PTR;


/* The file system info struct. For FAT32 only */
typedef struct filesystem_info_disk
{
    unsigned char    LEAD_SIG[4];
    unsigned char    RESERVED1[480];
    unsigned char    STRUCT_SIG[4];
    unsigned char    FREE_COUNT[4];
    unsigned char    NEXT_FREE[4];
    unsigned char    RESERVED2[12];
    unsigned char    TRAIL_SIG[4];
} FILESYSTEM_INFO_DISK, * FILESYSTEM_INFO_DISK_PTR;

typedef struct filesystem_info
{
    uint32_t  FREE_COUNT;
    uint32_t  NEXT_FREE;
} FILESYSTEM_INFO, FILESYSTEM_INFO_PTR;

/* configuration data for the MFS */
typedef struct mfs_drive_struct
{
    uint32_t                   DRV_NUM;
    BIOS_PARAM_STRUCT          BPB;
    BIOS_PARAM32_STRUCT        BPB32;

    _mfs_cache_policy          WRITE_CACHE_POLICY;
    bool                       DOS_DISK;
    bool                       BLOCK_MODE;
    bool                       READ_ONLY;

    uint32_t                   ALIGNMENT;
    uint32_t                   ALIGNMENT_MASK;

    uint16_t                   SECTOR_POWER;

    uint32_t                   CLUSTER_SIZE_BYTES;
    uint16_t                   CLUSTER_POWER_BYTES;
    uint16_t                   CLUSTER_POWER_SECTORS;

    uint16_t                   RESERVED;
    uint16_t                   ENTRIES_PER_SECTOR;

    uint32_t                   FAT_START_SECTOR;
    uint32_t                   ROOT_START_SECTOR;
    uint32_t                   DATA_START_SECTOR;
    QUEUE_STRUCT               HANDLE_LIST;
    uint32_t                   FAT_TYPE;
    uint32_t                   CUR_DIR_CLUSTER;
    uint32_t                   LAST_CLUSTER;
    char                       CURRENT_DIR[PATHNAME_SIZE + 1];
    unsigned char              RESERVED2[3];
    LWSEM_STRUCT               SEM;
    _partition_id              HANDLE_PARTITION;
#if MQX_USE_IO_OLD
    MQX_FILE_PTR               DEV_FILE_PTR;
#else
    int                        DEV_FILE_PTR;
#endif
    int                        MFS_FILE_PTR;

    bool                       DIR_SECTOR_DIRTY;
    uint32_t                   DIR_SECTOR_NUMBER;
    char                       *DIR_SECTOR_PTR;

    _mfs_cache_policy          FAT_CACHE_POLICY;
    uint32_t                   FAT_CACHE_START;
    uint32_t                   FAT_CACHE_DIRTY;
    char                       *FAT_CACHE_PTR;
    uint32_t                   FAT_CACHE_SIZE;

    bool                       DATA_SECTOR_DIRTY;
    uint32_t                   DATA_SECTOR_NUMBER;
    char                       *DATA_SECTOR_PTR;

    /* Count of free clusters. 0xFFFFFFFF means unknown. Must be recalculated */
    uint32_t                   FREE_COUNT;
    /* Is set to the last allocated cluster. If = 0xFFFFFFF start search at 2 */
    uint32_t                   NEXT_FREE_CLUSTER;

    bool                       VERIFY_WRITES;
    unsigned char              *READBACK_SECTOR_PTR;

} MFS_DRIVE_STRUCT, * MFS_DRIVE_STRUCT_PTR;

/*
** MFS Macro code
*/



/*
** CLUSTER_BOUNDARY ... takes a file offset and resets it to point to the
**                      first byte within the same cluster.
**       All this by masking off the least significant bits.
*/
#define  CLUSTER_BOUNDARY(x)   ((x) & (~(drive_ptr->CLUSTER_SIZE_BYTES - 1)))

/*
** The offset within the cluster is obtained by masking off the MSB's
*/
#define  OFFSET_WITHIN_CLUSTER(x)  ((x) & (drive_ptr->CLUSTER_SIZE_BYTES - 1))
#define  CLUSTER_OFFSET_TO_SECTOR(x)  (x>>drive_ptr->SECTOR_POWER)

#define  OFFSET_WITHIN_SECTOR(x)  ((x) & (drive_ptr->BPB.SECTOR_SIZE - 1))

/*
**  Returns the # of the first sector in the cluster.
*/
#define  CLUSTER_TO_SECTOR(x) ((x - CLUSTER_MIN_GOOD) *\
   drive_ptr->BPB.SECTORS_PER_CLUSTER + drive_ptr->DATA_START_SECTOR)

/*
** INDEX_TO_SECTOR converts an entry_index within the cluster or directory
** to the sector # in which that entry is found.
*/
#define INDEX_TO_SECTOR(x)  ((x) / drive_ptr->ENTRIES_PER_SECTOR)


/* Used to fix bug in date calculation */
#define NORMALIZE_DATE(c_ptr) \
   if ((c_ptr)->YEAR < 1980) (c_ptr)->YEAR = 1980

/* Macros for byte exchanges between host and disk */

/*
** This set of macros will always work on all processors.
** The sets of macros above are just optimizations for
** specific architectures.
*/
#if (PSP_MEMORY_ADDRESSING_CAPABILITY == 8)
/* Special Macros for reading and writing the FAT */
    #define htof0(p,x)    ((p)[0]  = (unsigned char) (x)        , \
                       (p)[1] &=              0x000000F0L, \
                       (p)[1] |= (unsigned char)((x) >> 8) & 0x0000000FL  \
                      )

    #define htof1(p,x)    ((p)[0] &=               0x0000000FL, \
                       (p)[0] |= (unsigned char)((x) <<  4) & 0x000000F0L, \
                       (p)[1]  =(unsigned char) ((x) >>  4)   \
                      )

    #define ftoh0(p)      (((uint32_t)((p)[0]       )       ) | \
                       ((uint32_t)((p)[1] & 0x0F) <<  8 ))

    #define ftoh1(p)      (((uint32_t)((p)[0] & 0xF0) >>  4 ) | \
                       ((uint32_t)((p)[1]       ) <<  4 ))

    #define htof32(p,x)   ((p)[3] = (unsigned char)(((p)[3] & 0xF0) | (((x) >> 24) & 0x0F)), \
                       (p)[2] = (unsigned char)((x) >> 16) , \
                       (p)[1] = (unsigned char)((x) >>  8) , \
                       (p)[0] = (unsigned char)((x)      ) )
#else
/*
** This set of macros will always work on all processors.
** The sets of macros above are just optimizations for
** specific architectures.
*/

/* Special Macros for reading and writing the FAT */

    #define htof0(p,x)    ((p)[0]  = (x)        & 0x000000FFL, \
                       (p)[1] &=              0x000000F0L, \
                       (p)[1] |= ((x) >> 8) & 0x0000000FL  \
                      )

    #define htof1(p,x)    ((p)[0] &=               0x0000000FL, \
                       (p)[0] |= ((x) <<  4) & 0x000000F0L, \
                       (p)[1]  = ((x) >>  4) & 0x000000FFL  \
                      )

    #define ftoh0(p)      (((uint32_t)((p)[0]       )       & 0x000000FFL) | \
                       ((uint32_t)((p)[1] & 0x0F) <<  8 & 0x0000FF00L))

    #define ftoh1(p)      (((uint32_t)((p)[0] & 0xF0) >>  4 & 0x000000FFL) | \
                       ((uint32_t)((p)[1]       ) <<  4 & 0x00000FF0L))

    #define htof32(p,x)   ((p)[3] = ((p)[3] & 0xF0) | (((x) >> 24) & 0x0F), \
                       (p)[2] = ((x) >> 16) & 0xFF, \
                       (p)[1] = ((x) >>  8) & 0xFF, \
                       (p)[0] = ((x)      ) & 0xFF)
#endif


/*
** Macros for getting/setting cluster, using a sperate high and low word
*/
#if (PSP_MEMORY_ADDRESSING_CAPABILITY == 8)

    #define clustoh(high,low) \
  (((((uint32_t)(low)[0]  )      )  | \
    (((uint32_t)(low)[1]  ) <<  8)  | \
    (((uint32_t)(high)[0] ) << 16)  | \
    (((uint32_t)(high)[1] ) << 24)) & \
   ((drive_ptr->FAT_TYPE == MFS_FAT32) ? 0xFFFFFFFF : 0x0000FFFF))

    #define clustod(high,low,x)   \
   ((low)[0]  = (unsigned char) ((x)      ) , \
    (low)[1]  = (unsigned char) ((x) >>  8) , \
    (high)[0] = (unsigned char) ((x) >> 16) , \
    (high)[1] = (unsigned char) ((x) >> 24))
#else

    #define clustoh(high,low) \
  (((((uint32_t)(low)[0]  & 0xFF)      )  | \
    (((uint32_t)(low)[1]  & 0xFF) <<  8)  | \
    (((uint32_t)(high)[0] & 0xFF) << 16)  | \
    (((uint32_t)(high)[1] & 0xFF) << 24)) & \
   ((drive_ptr->FAT_TYPE == MFS_FAT32) ? 0xFFFFFFFF : 0x0000FFFF))

    #define clustod(high,low,x)   \
   ((low)[0]  = ((x)      ) & 0xFF, \
    (low)[1]  = ((x) >>  8) & 0xFF, \
    (high)[0] = ((x) >> 16) & 0xFF, \
    (high)[1] = ((x) >> 24) & 0xFF)
#endif


/*
** extern statements for MFS
*/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _mqx_str_h_
    extern uint32_t _strnlen(char *, uint32_t);
#endif

uint32_t          _mfs_partition_destroy(_partition_id);
#if MQX_USE_IO_OLD
_mfs_error        _mfs_validate_device(MQX_FILE_PTR, uint32_t *, bool *);
#else
_mfs_error        _mfs_validate_device(int, uint32_t *, bool *);
#endif
_mfs_error        MFS_Add_cluster_to_chain(MFS_DRIVE_STRUCT_PTR, uint32_t, uint32_t *);
bool              MFS_Attribute_match(unsigned char, unsigned char);
uint32_t          MFS_Bad_clusters(MFS_DRIVE_STRUCT_PTR);
uint32_t          MFS_Change_current_dir(MFS_DRIVE_STRUCT_PTR, char *);
uint32_t          MFS_Check_dir_exist(MFS_DRIVE_STRUCT_PTR, char *);
bool              MFS_Check_search_string_for_8dot3(char *);
bool              MFS_Check_search_string_for_all(char *);
uint32_t          MFS_Clear_cluster(MFS_DRIVE_STRUCT_PTR, uint32_t);
int32_t           MFS_Close_Device(MFS_DRIVE_STRUCT_PTR);
int32_t           MFS_Close_file(MFS_HANDLE_PTR, MFS_DRIVE_STRUCT_PTR);
void              MFS_Compress_nondotfile(char *, char *);
MFS_DIR_ENTRY_PTR MFS_Create_directory_entry(MFS_DRIVE_STRUCT_PTR, char *, char, uint32_t *, uint32_t *, uint32_t *);
MFS_DIR_ENTRY_PTR MFS_Create_entry_slave(MFS_DRIVE_STRUCT_PTR, unsigned char, char *, uint32_t *, uint32_t *, uint32_t *, bool);
void              *MFS_Create_file(MFS_DRIVE_STRUCT_PTR, unsigned char, char *, uint32_t *);
void              *MFS_Create_new_file(MFS_DRIVE_STRUCT_PTR, unsigned char, char *, uint32_t *);
_mfs_error        MFS_Create_subdir(MFS_DRIVE_STRUCT_PTR, char *);
void              *MFS_Create_temp_file(MFS_DRIVE_STRUCT_PTR, unsigned char, char *, uint32_t *);
void              MFS_Decrement_free_clusters(MFS_DRIVE_STRUCT_PTR);
_mfs_error        MFS_Default_Format(MFS_DRIVE_STRUCT_PTR);
uint32_t          MFS_Delete_file(MFS_DRIVE_STRUCT_PTR, MFS_HANDLE_PTR, char *);
int32_t           MFS_device_write_internal(MFS_DRIVE_STRUCT_PTR, uint32_t, char *, int32_t *, char **, int32_t);
bool              MFS_Dirname_valid(char *);
void              MFS_Expand_dotfile(char *, char *);
void              MFS_Expand_wildcard(char *, char *);
_mfs_error        MFS_Extend_chain(MFS_DRIVE_STRUCT_PTR, uint32_t, uint32_t, uint32_t *);
bool              MFS_Filename_valid(char *);
uint32_t          MFS_Find_directory(MFS_DRIVE_STRUCT_PTR, char *, uint32_t);
MFS_DIR_ENTRY_PTR MFS_Find_directory_entry(MFS_DRIVE_STRUCT_PTR, char *, uint32_t *, uint32_t *, uint32_t *, unsigned char, uint32_t *);
MFS_DIR_ENTRY_PTR MFS_Find_entry_on_disk(MFS_DRIVE_STRUCT_PTR, char *, uint32_t *, uint32_t *, uint32_t *, uint32_t *);
uint32_t          MFS_Find_first_file(MFS_DRIVE_STRUCT_PTR, unsigned char, char *, MFS_SEARCH_DATA_PTR);
MFS_HANDLE_PTR    MFS_Find_handle(MFS_DRIVE_STRUCT_PTR, uint32_t, uint32_t);
uint32_t          MFS_Find_next_file(MFS_DRIVE_STRUCT_PTR, MFS_SEARCH_DATA_PTR);
uint32_t          MFS_Find_next_slave(MFS_DRIVE_STRUCT_PTR, void *);
uint32_t          MFS_Find_unused_cluster_from(MFS_DRIVE_STRUCT_PTR, uint32_t);
_mfs_error        MFS_Flush_caches(MFS_DRIVE_STRUCT_PTR);
uint32_t          MFS_Flush_data_sector_buffer(MFS_DRIVE_STRUCT_PTR);
int32_t           MFS_Flush_Device(MFS_DRIVE_STRUCT_PTR, MFS_HANDLE_PTR);
uint32_t          MFS_Flush_directory_sector_buffer(MFS_DRIVE_STRUCT_PTR);
uint32_t          MFS_Flush_fat_cache(MFS_DRIVE_STRUCT_PTR);
uint32_t          MFS_Format(MFS_DRIVE_STRUCT_PTR, MFS_FORMAT_DATA_PTR);
uint32_t          MFS_Format_and_test(MFS_DRIVE_STRUCT_PTR, MFS_FORMAT_DATA_PTR, uint32_t *);
void              MFS_Free_handle(MFS_DRIVE_STRUCT_PTR, MFS_HANDLE_PTR);
_mfs_error        MFS_get_cluster_from_fat(MFS_DRIVE_STRUCT_PTR, uint32_t, uint32_t *);
uint32_t          MFS_Get_current_dir(MFS_DRIVE_STRUCT_PTR, char *);
uint32_t          MFS_Get_date_time(MFS_DRIVE_STRUCT_PTR, MFS_HANDLE_PTR, uint16_t *, uint16_t *);
uint64_t          MFS_Get_disk_free_space(MFS_DRIVE_STRUCT_PTR);
uint32_t          MFS_Get_disk_free_space_internal(MFS_DRIVE_STRUCT_PTR, uint32_t *);
uint32_t          MFS_Get_file_attributes(MFS_DRIVE_STRUCT_PTR, MFS_HANDLE_PTR, char *, unsigned char *);
MFS_HANDLE_PTR    MFS_Get_handle(MFS_DRIVE_STRUCT_PTR, MFS_DIR_ENTRY_PTR);
uint32_t          MFS_get_lfn(MFS_DRIVE_STRUCT_PTR, char *, char *);
uint32_t          MFS_get_lfn_dir_cluster(MFS_DRIVE_STRUCT_PTR, void *, char *, char *);
_mfs_error        MFS_get_lfn_of_entry(MFS_DRIVE_STRUCT_PTR, MFS_DIR_ENTRY_PTR, uint32_t, uint32_t, uint32_t, char *);
uint32_t          MFS_get_prev_cluster(MFS_DRIVE_STRUCT_PTR, uint32_t *, uint32_t);
uint32_t          MFS_Get_volume(MFS_DRIVE_STRUCT_PTR, char *);
uint32_t          MFS_Increment_dir_index(MFS_DRIVE_STRUCT_PTR, uint32_t *, uint32_t *, uint32_t *);
void              MFS_Increment_free_clusters(MFS_DRIVE_STRUCT_PTR);
void              MFS_increment_lfn(char *);
_mfs_error        MFS_Invalidate_data_sector(MFS_DRIVE_STRUCT_PTR);
_mfs_error        MFS_Invalidate_directory_sector(MFS_DRIVE_STRUCT_PTR);
uint16_t          MFS_Is_dot_directory(char *);
bool              MFS_is_read_only(MFS_DRIVE_STRUCT_PTR);
bool              MFS_is_valid_lfn(char *);
uint32_t          MFS_Last_cluster(MFS_DRIVE_STRUCT_PTR);
unsigned char     MFS_lfn_checksum(char *);
bool              MFS_lfn_dirname_valid(char *);
int               MFS_lfn_extract(MFS_LNAME_ENTRY_PTR, char *);
bool              MFS_lfn_match(char *, char *, uint32_t) ;
uint32_t          MFS_lfn_name_to_entry(char *, MFS_LNAME_ENTRY_PTR);
uint32_t          MFS_lfn_to_sfn(char *, char *);
uint32_t          MFS_lock(MFS_DRIVE_STRUCT_PTR);
uint32_t          MFS_lock_dos_disk(MFS_DRIVE_STRUCT_PTR);
uint32_t          MFS_Mount_drive_internal(MFS_DRIVE_STRUCT_PTR);
uint32_t          MFS_Move_file_pointer(MFS_HANDLE_PTR, MFS_DRIVE_STRUCT_PTR, uint32_t *);
_mfs_error        MFS_next_data_sector(MFS_DRIVE_STRUCT_PTR, MFS_HANDLE_PTR, uint32_t *, uint32_t *);
int32_t           MFS_Open_Device(MFS_DRIVE_STRUCT_PTR);
void              *MFS_Open_file(MFS_DRIVE_STRUCT_PTR, char *, unsigned char, uint32_t *);
char              *MFS_Parse_next_filename(char *, char *);
char              *MFS_Parse_Out_Device_Name(char *);
uint32_t          MFS_Parse_pathname(char *, char *, char *);
uint32_t          MFS_Put_fat(MFS_DRIVE_STRUCT_PTR, uint32_t, uint32_t);
uint32_t          MFS_Read(MFS_HANDLE_PTR, MFS_DRIVE_STRUCT_PTR, uint32_t, char *, uint32_t *);
_mfs_error        MFS_Read_data_sector(MFS_DRIVE_STRUCT_PTR, MFS_HANDLE_PTR, uint32_t, bool);
_mfs_error        MFS_Read_device_sectors(MFS_DRIVE_STRUCT_PTR, uint32_t, uint32_t, uint32_t, char *, uint32_t *);
_mfs_error        MFS_Read_device_sector(MFS_DRIVE_STRUCT_PTR, uint32_t, char *);
void              *MFS_Read_directory_sector(MFS_DRIVE_STRUCT_PTR, uint32_t, uint16_t, uint32_t *);
uint32_t          MFS_Read_fat(MFS_DRIVE_STRUCT_PTR, uint32_t);
_mfs_error        MFS_Release_chain(MFS_DRIVE_STRUCT_PTR, uint32_t);
uint32_t          MFS_remove_lfn_entries(MFS_DRIVE_STRUCT_PTR, uint32_t, uint32_t, uint32_t);
uint32_t          MFS_Remove_subdir(MFS_DRIVE_STRUCT_PTR, char *);
uint32_t          MFS_Rename_file(MFS_DRIVE_STRUCT_PTR, char *, char *);
uint32_t          MFS_Set_date_time(MFS_DRIVE_STRUCT_PTR, MFS_HANDLE_PTR, uint16_t *, uint16_t *);
uint32_t          MFS_Set_file_attributes(MFS_DRIVE_STRUCT_PTR, MFS_HANDLE_PTR, char *, unsigned char *);
uint32_t          MFS_Set_volume(MFS_DRIVE_STRUCT_PTR, char *);
bool              MFS_strcmp(char *, char *, uint32_t);
void              MFS_strupr(char *);
uint32_t          MFS_Test_unused_clusters(MFS_DRIVE_STRUCT_PTR, uint32_t *);
_mfs_error        MFS_unlock(MFS_DRIVE_STRUCT_PTR, bool);
extern uint32_t   MFS_Update_entry(MFS_DRIVE_STRUCT_PTR, MFS_HANDLE_PTR);
extern bool       MFS_Wildcard_match(char *, char *);
uint32_t          MFS_Write(MFS_HANDLE_PTR, MFS_DRIVE_STRUCT_PTR, uint32_t, char *, uint32_t *);
extern _mfs_error MFS_Write_back_fat(MFS_DRIVE_STRUCT_PTR);
extern uint32_t   MFS_Write_back_fat(MFS_DRIVE_STRUCT_PTR);
extern _mfs_error MFS_Write_device_sectors(MFS_DRIVE_STRUCT_PTR, uint32_t, uint32_t, uint32_t, char *, uint32_t *);
extern _mfs_error MFS_Write_device_sector(MFS_DRIVE_STRUCT_PTR, uint32_t, char *);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
