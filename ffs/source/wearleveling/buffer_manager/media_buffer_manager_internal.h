#ifndef __media_buffer_manager_internal_h__
#define __media_buffer_manager_internal_h__
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
*   This file contains internal definitions for the buffer manager.
*
*
*END************************************************************************/

/* 
** Constants 
*/

#ifndef MEDIA_BUFFER_DEBUG
#define MEDIA_BUFFER_DEBUG                  (0)
#endif /* MEDIA_BUFFER_DEBUG */

/* Set the default value of this macro. */
#if !defined(RECORD_BUFFER_STATS)
#define RECORD_BUFFER_STATS                 (1)
#endif

/* Maximum number of buffers that can be tracked at once. */
#define MAX_BUFFER_COUNT                    (10)

/* Used to indicate that there is no temporary buffer waiting to time out. */
#define NO_NEXT_TIMEOUT                     (-1)

/* Timeout in milliseconds for temporary buffers. */
#define TEMPORARY_BUFFER_TIMEOUT_MS         (200)

/* Number of milliseconds to delay before attempting to post a DPC again. */
#define TIMER_RETRY_DELAY_MS                (50)

/* 
** Type definations 
*/

/*
** Internal flags applied to buffers.
**
** These internal flags are flags that the buffer manager may apply
** to a buffer to keep track of state, but that clients will never pass
** in when requesting a buffer. All internal flags are within the bit
** range of 16 to 31.
*/
enum _media_buffer_internal_flags
{
    /* The buffer is in use. */
    kMediaBufferFlag_InUse = (1 << 16),
    
    /* The buffer should be disposed of when released. */
    kMediaBufferFlag_Temporary = (1 << 17),
    
    /* The buffer has been realigned. */
    kMediaBufferFlag_Realigned = (1 << 18),
    
    /* Mask for all internal flags. */
    kMediaBufferManager_InternalFlagsMask = (0xffff << 16)
};

/*
** Buffer information.
**
** This structure holds information about each buffer being controlled by the
** media buffer manager. The global context has an array of these structures,
** where the first #MediaBufferManagerContext_t::slotCount elements are potentially
** valid. For any given instance of one of these structs, it is valid if and
** only if the #MediaBufferInfo_t::data field is non-zero.
**
** Temporary buffers, those that are dynamically allocated at runtime, are
** retained for a certain length of time after they are released back to the
** buffer manager. The #MediaBufferInfo_t::timeout field here will be set to the
** system clock time in ticks when the temporary buffer should finally be freed.
** Until that time, the buffer is available to match incoming requests, and if
** it is used the timeout is reset.
**
** The #MediaBufferInfo_t::bufferType field is an optional field only present
** when buffer statistics are enabled. Normally the #MediaBufferInfo_t::length
** field is sufficient, but to track statistics by buffer type we need to know
** the original type used when a buffer was created.
*/
typedef struct _buffer_info {
    /* Size of this buffer in bytes. */
    _mem_size length;
    
    /* Pointer to the buffer itself. This is always non-zero for valid buffer entries. */
    SECTOR_BUFFER * data;
    
    /* Flags pertaining to this buffer. */
    uint32_t flags;
    
    /* Number of references to this buffer. */
    uint32_t refCount;
    
    /* Absolute time in ticks when this buffer expires. Only applies to temporary buffers. */
    MQX_TICK_STRUCT timeout;
    
    /* If the buffer has been realigned, then this field points to the original */
    SECTOR_BUFFER * originalBuffer;
    
    /* Result of the allocation; this is the pointer that should be passed to free(). */

#if RECORD_BUFFER_STATS
    /* Statistics */
    /* The type of this buffer. */
    MediaBufferType_t bufferType;
    
    /* Number of times the buffer has been acquired. */
    unsigned acquiredCount;
    
    /* Microsecond timestamp when the buffer was created (added). */
    uint64_t createdTimestamp;
    
    /* Microsecond timestamp when the buffer was last acquired. */
    uint64_t acquiredTimestamp;
    
    /* Microsecond timestamp for when the buffer was last released. */
    uint64_t releasedTimestamp;
    
    /* Accumulator for computing the averageUsageTimespan. */
    uint64_t averageUsageAccumulator;
    
    /* Average number of microseconds the buffer is being held. */
    uint64_t averageUsageTimespan;
    
#endif /* RECORD_BUFFER_STATS */
} MediaBufferInfo_t;

#if RECORD_BUFFER_STATS
/* Statistics information about buffer usage. */
typedef struct _media_buffer_statistics {
    /* Total number of allocations. */
    unsigned totalAllocs;
    
    /* Current number of buffers in use. */
    unsigned concurrentAllocs;
    
    /* Highest number of buffers in use at the same time. */
    unsigned maxConcurrentAllocs;
    
    /* 
    ** For temporary buffer, this is the number of new buffers that were allocated. 
    ** Unused by permanent buffers. 
    */
    unsigned newAllocs;
    
    /* Number of buffer allocations that had to be realigned. */
    unsigned realignedAllocs;
} MediaBufferStatistics_t;
#endif /* RECORD_BUFFER_STATS */

/*
** Contains all global information for the buffer manager.
**
** The array of buffer structures, #MediaBufferManagerContext_t::buffers, holds
** information about all of the buffers under the control of the media buffer
** manager. The first #MediaBufferManagerContext_t::slotCount elements in the array
** are the only ones that may be valid, but not all of them are required to be so.
** If #MediaBufferManagerContext_t::bufferCount is less than the used slot count,
** then invalid (unused) buffer array elements are present and must be skipped.
** The requirements for a valid buffer structure are described in the documentation for
** #MediaBufferInfo_t.
*/
typedef struct _media_buffer_manager_context {
    /* General */
    /* Whether the buffer manager has been initialised. */
    bool isInited;
    
    /* Mutex used to protect this context structure. */
    MUTEX_STRUCT mutex;
        
    /*
    ** Buffer array
    **
    ** Only bufferCount buffers out of the first slotCount elements of the buffer
    ** array are valid. And out of those, only freeCount buffers are available for
    ** use by clients of the buffer manager.
    */
    /* Array of buffers. */
    MediaBufferInfo_t buffers[MAX_BUFFER_COUNT];
    
    /* The number of buffer array entries that must be searched. */
    unsigned slotCount;
    
    /* The number of buffers of all types in the buffers array. */
    unsigned bufferCount;
    
    /* The number of unused buffers of all types in the buffers array. */
    unsigned freeCount;
    
    /*
    ** Temporary timeouts
    **
    ** These fields are used to time out temporary buffers after they have been returned
    ** to the buffer manager.
    */
    /* ThreadX timer used to time out temporary buffers. */
    _timer_id timeoutTimer;
    
    /* Index of the temporary buffer that will next time out. #NO_NEXT_TIMEOUT if there is none. */
    int nextTimeout;
    
    /* Index of temporary buffer that has timed out and should be permanently disposed of. Will be set to #NO_NEXT_TIMEOUT if there is no buffer to dispose. */
    int bufferToDispose;
    
#if RECORD_BUFFER_STATS
    /*
    ** Statistics
    **
    ** In debug builds, these fields of the context are used to keep useful
    ** statistics about allocations. This data can be used to tune the number
    ** of buffers in an application to get the best match between memory
    ** and performance.
    */
    /* Statistics for permanent buffers. */
    MediaBufferStatistics_t permStats;
    
    /* Statistics for temporary buffers. */
    MediaBufferStatistics_t tempStats;
    
    /* Statistics for permanent buffers by type. */
    MediaBufferStatistics_t permTypeStats[kMediaBufferType_Count];
    
    /* Statistics for temporary buffers by type. */
    MediaBufferStatistics_t tempTypeStats[kMediaBufferType_Count];
    
    /* Number of allocations where a buffer was selected that wasn't a perfect match in size. */
    unsigned mismatchedSizeAllocs;
    
#endif /* RECORD_BUFFER_STATS */
} MediaBufferManagerContext_t;

/* 
** Global external declarations 
*/

extern MediaBufferManagerContext_t g_mediaBufferManagerContext;
extern const _mem_size kMediaBufferTypeSizes[];

/* 
** Function prototypes 
*/

void media_buffer_timeout(_timer_id id, void *data_ptr, uint32_t seconds, uint32_t milliseconds);

uint32_t media_buffer_count_all_temporary(void);

void media_buffer_force_dispose_temporary(void);

#endif /* __media_buffer_manager_internal_h__ */

/* EOF */
