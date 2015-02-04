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
*   This file contains implementation of the media buffer manager.
*
*
*END************************************************************************/

#include "wl_common.h"
#include "mutex.h"
#include "timer.h"   
#include "media_buffer.h"
#include "media_buffer_manager_internal.h"
#include "ddi_media.h"
#include "ddi_media_errordefs.h"

/* 
** Macros 
*/

#if !defined(INT32_MAX)
#define INT32_MAX ( (int32_t) ( ((uint32_t)-1) /2 )) 
#endif

/* 
** Global external declarations 
*/

/* Global state information for the buffer manager. */
MediaBufferManagerContext_t g_mediaBufferManagerContext;

/* 
** Function prototypes 
*/

static _mem_size media_buffer_get_type_size(MediaBufferType_t bufferType);

static RtStatus_t media_buffer_add_internal(MediaBufferType_t bufferType, uint32_t bufferFlags, uint32_t refCount, SECTOR_BUFFER * buffer, SECTOR_BUFFER * originalBuffer, unsigned * insertIndex);

static bool media_buffer_search(_mem_size length, bool exactLengthMatch, uint32_t flags, unsigned * result);

static SECTOR_BUFFER * media_buffer_allocate_internal(_mem_size length, uint32_t flags, bool physicallyContiguous);

static SECTOR_BUFFER * media_buffer_allocate(_mem_size length, uint32_t flags, uint32_t * resultFlags, SECTOR_BUFFER ** original);

static void media_buffer_shrink_slots(void);

static void media_buffer_setup_next_timeout(void);

static void media_buffer_dispose_temporary(uint32_t unused);

#if RECORD_BUFFER_STATS
static void media_buffer_update_stats(MediaBufferStatistics_t * stats, bool isAcquire);
#endif /* RECORD_BUFFER_STATS */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_get_type_size
* Returned Value   : A buffer length in bytes.
* Comments         :
*   This function returns the size in bytes of a buffer type.
*   This function will return the size of a class of buffer. 
*   It takes knowledge available only at runtime into account, 
*   such as the page size of the currently attached NAND device.
*
*END*--------------------------------------------------------------------*/
static _mem_size media_buffer_get_type_size
(
    /* [IN] The class of the buffer. */
    MediaBufferType_t bufferType
)
{ /* Body */
    switch (bufferType)
    {
        case kMediaBufferType_Sector:
            return MediaGetMaximumSectorSize();
            
        case kMediaBufferType_Auxiliary:
            /* return REDUNDANT_AREA_BUFFER_ALLOCATION; */
            return MediaGetMaximumAuxSize();
            
        case kMediaBufferType_NANDPage:
            /* 
            ** This is an ugly kludge that lets us avoid a dependency upon MediaGetInfo(), 
            ** which is very useful for keeping unit tests simple.
            ** As above, use the maximum size possible if the media is not ready yet. 
            */
            return MAX_SECTOR_TOTAL_SIZE;

        default:
            return 0;
    } /* Endswitch */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_add_internal
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS
*   - ERROR_DDI_MEDIABUFMGR_NO_ROOM
* Comments         :
*   Internal add function that returns the new buffer's index.
*
*END*--------------------------------------------------------------------*/
static RtStatus_t media_buffer_add_internal
(
    /* [IN] Buffer type */
    MediaBufferType_t bufferType, 
    
    /* [IN] Buffer flag */
    uint32_t bufferFlags, 

    /* [IN] Counter */
    uint32_t refCount, 

    /* [IN] Buffer */
    SECTOR_BUFFER * buffer, 

    /* [IN] Orignal buffer */
    SECTOR_BUFFER * originalBuffer, 
    
    /* [IN] Insert index */
    unsigned * insertIndex
)
{ /* Body */
    unsigned i;
    MediaBufferInfo_t * info;

    assert(buffer != NULL);
    /* Make sure the buffer is word aligned. */
    assert(((uint32_t)buffer & 0x3) == 0);
    assert(g_mediaBufferManagerContext.isInited);

    /* Acquire mutex. */
    _mutex_lock(&g_mediaBufferManagerContext.mutex);

    /* Can't add a buffer when there is no more room. */
    if (g_mediaBufferManagerContext.bufferCount == MAX_BUFFER_COUNT)
    {
        _mutex_unlock(&g_mediaBufferManagerContext.mutex);
        return ERROR_DDI_MEDIABUFMGR_NO_ROOM;
    } /* Endif */

    /* 
    ** Find an empty slot to insert the new buffer into. If we don't find one,
    ** then when the loop is finished i will point at the next unused slot. 
    */
    for (i = 0; i < g_mediaBufferManagerContext.slotCount; ++i)
    {
        if (g_mediaBufferManagerContext.buffers[i].data == NULL)
        {
            break;
        } /* Endif */
    } /* Endfor */

    assert(i <= MAX_BUFFER_COUNT);

    /* Fill in new buffer information. */
    info = &g_mediaBufferManagerContext.buffers[i];
    info->length = media_buffer_get_type_size(bufferType);
    info->data = buffer;
    info->flags = bufferFlags;
    info->refCount = refCount;
    _time_init_ticks(&info->timeout, 0);
    info->originalBuffer = originalBuffer;

#if RECORD_BUFFER_STATS
    /* 
    ** In debug builds we keep track of the buffer's type 
    ** for statistics generation purposes. 
    */
    info->bufferType = bufferType;
#endif /* RECORD_BUFFER_STATS */

    /* Increment buffer count and free count. */
    g_mediaBufferManagerContext.bufferCount++;
    g_mediaBufferManagerContext.freeCount++;

    /* Increment the used count if we added to the end. */
    if (i == g_mediaBufferManagerContext.slotCount)
    {
        g_mediaBufferManagerContext.slotCount++;
    } /* Endif */

    /* Return the index of the newly added buffer. */
    if (insertIndex)
    {
        *insertIndex = i;
    } /* Endif */

    /* Release mutex. */
    _mutex_unlock(&g_mediaBufferManagerContext.mutex);
    return SUCCESS;
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_add
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS The new buffer was added.
*   - ERROR_DDI_MEDIABUFMGR_NO_ROOM There is no room to add another buffer.
* Comments         :
*   This function makes a new buffer available for use by buffer manager clients.
* Immediately after a successful return, the buffer passed into this function
*   may be returned from a call to media_buffer_acquire(). There is no way to free or 
*   remove the buffer once it is handed to the buffer manager. The presumption
*   is that the buffer remain available until the running program stops
*   execution.
*
*END*--------------------------------------------------------------------*/
RtStatus_t media_buffer_add
(
    /* [IN] Which size class this buffer is. */
    MediaBufferType_t bufferType, 

    /* [IN] Flags that apply to this buffer. */
    uint32_t bufferFlags, 

    /* [IN] Pointer to the buffer itself. Should be word aligned. */
    SECTOR_BUFFER * buffer
)
{ /* Body */
    return media_buffer_add_internal(bufferType, bufferFlags, 0, buffer, NULL, NULL);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : bool media_buffer_search
* Returned Value   : TRUE or FALSE
*   - TRUE A matching, free buffer was found. 
*   The result parameter holds the index of the matching buffer.
*   - FALSE No buffer is available that matches the request.
* Comments         :
*   This function searches the buffer list for a free buffer with the desired attributes.
*
*END*--------------------------------------------------------------------*/
#if __CWCC__
#pragma push
/* Force compiler to not optimize source code on CW */
#pragma optimization_level 0
#endif
static bool media_buffer_search
(
    /* [IN] Desired length in bytes of the buffer. */
    _mem_size length, 
    
    /* 
    ** [IN] Whether length must match the buffer's size exactly, 
    ** or the buffer can be larger than the requested size. 
    */
    bool exactLengthMatch, 

    /* [IN] Flags that the buffer must have set. */
    uint32_t flags, 
    
    /* [OUT] On successful exit, this parameter is set to the index of the buffer that was found. */
    unsigned * result
)
{ /* Body */
    unsigned i;

    assert(result != NULL);

    /* Search for free buffer. */
    for (i=0; i < g_mediaBufferManagerContext.slotCount; ++i)
    {
        MediaBufferInfo_t * info = &g_mediaBufferManagerContext.buffers[i];
        bool isMatch;

        /* Skip the buffer if it's data pointer is null. */
        if (info->data == NULL)
        {
            continue;
        } /* Endif */

        /* 
        ** A buffer matches the request if:
        **  - it is not busy
        **  - the request flags are set on the buffer
        **  - exactLengthMatch is:
        **      - TRUE: length matches exactly
        **      - FALSE: buffer length is >= the request length 
        */
        isMatch = ((info->flags & kMediaBufferFlag_InUse) == 0)
        && (((info->flags & ~kMediaBufferManager_InternalFlagsMask) & flags) == flags);

        if (exactLengthMatch)
        {
            /* Length must match exactly. */
            isMatch = isMatch && (info->length == length);
        }
        else
        {
            /* Match as long as the buffer's length is large enough for the request. */
            isMatch = isMatch && (info->length >= length);
        } /* Endif */

        /* Return the buffer index if we've hit a match. */
        if (isMatch)
        {
            *result = i;
            return TRUE;
        } /* Endif */
    } /* Endfor */

    /* No buffer is available that matches the request. */
    return FALSE;
}
#if __CWCC__
#pragma pop
#endif

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_allocate_internal
* Returned Value   : Pointer buffer
* Comments         :
*   This function allocates a buffer modified by the flags.
*
*END*--------------------------------------------------------------------*/
static SECTOR_BUFFER * media_buffer_allocate_internal
(
    /* [IN] Buffer length */
    _mem_size length, 
    
    /* [IN] Flag */
    uint32_t flags, 
    
    /* [IN] Contigous flag */
    bool physicallyContiguous
)
{ /* Body */
    SECTOR_BUFFER * resultBuffer;
    
    switch (flags & (kMediaBufferFlag_NCNB))
    {
        /* Only NCNB memory required. */
        case kMediaBufferFlag_NCNB:
            resultBuffer = (SECTOR_BUFFER *)_wl_mem_alloc_uncache(length);
            break;
            
        /* No special requirements for the memory type. */
        default:
            {
                resultBuffer = (SECTOR_BUFFER *)_wl_mem_alloc(length);
            }
            break;
    } /* Endswitch */
    
    return resultBuffer;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_allocate
* Returned Value   : A pointer to a newly-allocated buffer is returned.
*   NULL Returned if the memory could not be allocated for some reason, 
*   such as the system being out of memory.
* Comments         :
*   This function allocates a buffer modified by the flags.
*
*   The two flags that this function honours are:
*       - kBufferManager_NCNBFlag
*       - kBufferManager_FastMemoryFlag
*
*   Thus, the memory allocated by this function can be any combination of
*   standard or fast memory, cached or non-cached. The returned buffer
*   should be deallocated with a call to free().
*
*   When allocating CB memory, this routine will ensure that the returned buffer is aligned
*   to the start of a data cache line. Also, the buffer's size is rounded up to the next
*   cache line. All of this is to prevent the buffer from being modified due to a cache flush
*   after a partial cache line has been modified by other code. NCNB allocations do not need
*   to be aligned like this because they do not exist in the data cache (and NCNB regions are
*   always aligned at VM page boundaries, anyway).
*
*END*--------------------------------------------------------------------*/
static SECTOR_BUFFER * media_buffer_allocate
(
    /* [IN] Number of bytes required in the buffer. Must be greater */
    _mem_size length, 
    
    /* 
    ** [IN] Flags that modify how the buffer is allocated. 
    ** Any flags not supported by the function may be set 
    ** and they will simply be ignored. 
    */
    uint32_t flags, 
    
    /* 
    ** [OUT] If the buffer had to be realigned, 
    ** then #kMediaBufferFlag_Realigned will be returned through this parameter. 
    ** Otherwise this is set to 0. 
    */
    uint32_t * resultFlags, 
    
    /* 
    ** [OUT] If the allocated buffer had to be realigned, 
    ** indicated by the value of resultFlags upon return, 
    ** then this parameter is set to the unaligned result of 
    ** the allocation that must be passed to free(). 
    */
    SECTOR_BUFFER ** original
)
{ /* Body */
    SECTOR_BUFFER * resultBuffer;
    _mem_size roundedLength;
    bool allocateContiguous = FALSE;

    assert(length > 0);

    /* Set default result flags. */
    *resultFlags = 0;

    /* Set allocation length to the request size rounded up to the next full cache line. */
    roundedLength = length;
    
    /* 
    ** Allocate NCNB memory if that flag is set. Same for fast memory.
    ** Otherwise normal memory will do. 
    */
    resultBuffer = media_buffer_allocate_internal(roundedLength, flags, allocateContiguous);
    
    /* Catch an error before dealing with alignment. */
    if (!resultBuffer)
    {
        return NULL;
    } /* Endif */
    
    return resultBuffer;
} /* Endbody */

#if RECORD_BUFFER_STATS
/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_update_stats
* Returned Value   : void
* Comments         :
*   This function updates statistics.
*
*END*--------------------------------------------------------------------*/
static void media_buffer_update_stats
(
    MediaBufferStatistics_t * stats, 
    
    bool isAcquire
)
{ /* Body */
    if (isAcquire)
    {
        /* Update stats for a buffer acquire. */
        stats->totalAllocs++;
        stats->concurrentAllocs++;

        if (stats->concurrentAllocs > stats->maxConcurrentAllocs)
        {
            stats->maxConcurrentAllocs = stats->concurrentAllocs;
        } /* Endif */
    }
    else
    {
        /* Update stats for a buffer release. */
        stats->concurrentAllocs--;
    } /* Endif */
} /* Endbody */
#endif /* RECORD_BUFFER_STATS */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_acquire
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS A free buffer was returned in the buffer parameter.
*   - ERROR_DDI_MEDIABUFMGR_NO_ROOM Allocating a new temporary buffer was
*   required in order to service the request, but there is no room to add 
*   another buffer.
*   - ERROR_DDI_MEDIABUFMGR_ALLOC_FAILED The buffer manager was unable to
*   allocate a new temporary buffer for some reason.
* Comments         :
*   Obtain a free buffer of the requested type.
*   Clients call this function to get a buffer that they can use for any length
*   of time. The parameters specify the desired attributes of the buffer,
*   including its type. The buffer manager first makes an attempt to find a
*   pre-existing buffer that exactly matches the requested attributes. If that
*   fails, further attempts are made, looking for a buffer that is as close a
*   match as possible.
*   Flags take precedence over size. In other words, given a choice between
*   buffer "A" that matches required flags but is larger, and
*   buffer "B" that is an exact size match but doesn't match all of the
*   flags, the buffer manager will choose buffer "A".
*   In the case where no pre-existing buffers are available that match, this
*   function will allocate a temporary buffer from dynamic memory. The
*   allocated buffer will match both required and optional flags. For example,
*   if NCNB is required and fast memory is optional, a temporary buffer will be
*   allocated with both NCNB and fast memory.
*
*END*--------------------------------------------------------------------*/
RtStatus_t media_buffer_acquire
(
    /* [IN] Requested size of the buffer. */
    MediaBufferType_t bufferType, 

    /* [IN Flags that must be set on the returned buffer. */
    uint32_t requiredFlags, 

    /* [OUT] On successful return, this parameter will hold the buffer's address. */
    SECTOR_BUFFER ** buffer
)
{ /* Body */
    _mem_size typeSize = media_buffer_get_type_size(bufferType);
    SECTOR_BUFFER * data;
    RtStatus_t status;

    assert(buffer != NULL);
    assert(g_mediaBufferManagerContext.isInited);

    /* Acquire mutex. */
    _mutex_lock(&g_mediaBufferManagerContext.mutex);

    /* Is there a free buffer available? */
    if (g_mediaBufferManagerContext.freeCount > 0)
    {
        unsigned matchIndex;
        bool isMatch;

        /* 
        ** Try a first time to find a matching free buffer that has the exact
        ** length being requested and matches all flags. 
        */
        isMatch = media_buffer_search(typeSize, TRUE, requiredFlags, &matchIndex);

        /* 
        ** If that doesn't pan out, try again to find a buffer at least as large
        ** as the requested size and has all flags set. 
        */
        if (!isMatch)
        {
            isMatch = media_buffer_search(typeSize, FALSE, requiredFlags, &matchIndex);
        } /* Endif */
        
#if MEDIA_BUFFER_DEBUG
        WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_INFO,
            "bufmgr: acquire, found buf at %d \n", matchIndex);
#endif /* MEDIA_BUFFER_DEBUG */

        /* Handle when we've found a buffer the caller can use. */
        if (isMatch)
        {
            /* Mark the buffer as used and set the return value. */
            MediaBufferInfo_t * info = &g_mediaBufferManagerContext.buffers[matchIndex];
            info->flags |= kMediaBufferFlag_InUse;
            info->refCount = 1;
            *buffer = info->data;

            g_mediaBufferManagerContext.freeCount--;

            /* Deal with temporary buffers. */
            if (info->flags & kMediaBufferFlag_Temporary)
            {
                /* Clear the timeout. */
                _time_init_ticks(&info->timeout, 0);

                /* 
                ** If this was the buffer that was next going to timeout, 
                ** deactivate the timer. 
                */
                if (g_mediaBufferManagerContext.nextTimeout == (int)matchIndex)
                {
#if MEDIA_BUFFER_DEBUG
                    WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_INFO,
                        "bufmgr: acquire, cancel queued dispose timer %d \n", matchIndex);
#endif /* MEDIA_BUFFER_DEBUG */
                    /* Kill the timer, it will be reactivated below if needed. */
                    _timer_cancel(g_mediaBufferManagerContext.timeoutTimer);

                    /* 
                    ** See if there is a next buffer to timeout. This will update
                    ** the nextTimeout field of the context. 
                    */
                    media_buffer_setup_next_timeout();
                } /* Endif */
 
                /* If this buffer was queued to be disposed, cancel that. */
                if (g_mediaBufferManagerContext.bufferToDispose == (int)matchIndex)
                {
                    g_mediaBufferManagerContext.bufferToDispose = NO_NEXT_TIMEOUT;
                } /* Endif */
            } /* Endif */

#if RECORD_BUFFER_STATS
            /* Update statistics. */
            if (info->flags & kMediaBufferFlag_Temporary)
            {
                media_buffer_update_stats(&g_mediaBufferManagerContext.tempStats, TRUE);
                media_buffer_update_stats(&g_mediaBufferManagerContext.tempTypeStats[info->bufferType], TRUE);
            }
            else
            {
                media_buffer_update_stats(&g_mediaBufferManagerContext.permStats, TRUE);
                media_buffer_update_stats(&g_mediaBufferManagerContext.permTypeStats[info->bufferType], TRUE);
            } /* Endif */

            if (info->bufferType != bufferType)
            {
                g_mediaBufferManagerContext.mismatchedSizeAllocs++;
            } /* Endif */

            /* Log the permanent allocation. */
#if MEDIA_BUFFER_DEBUG
            WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_INFO,
                "bufmgr: allocated perm buffer %x [#%d, size=%d, flags=%x]\n", 
                (uint32_t)info->data, matchIndex, typeSize, requiredFlags);
#endif /* MEDIA_BUFFER_DEBUG */

#endif /* RECORD_BUFFER_STATS */

            _mutex_unlock(&g_mediaBufferManagerContext.mutex);
            return SUCCESS;
        } /* Endif */
    } /* Endif */

    {
        SECTOR_BUFFER * originalBuffer = NULL;
        uint32_t resultFlags;
        uint32_t bufferFlags;

        /* 
        ** There are no buffers available in the list, or no match was found,
        ** so create a temporary one. 
        */
        data = media_buffer_allocate(typeSize, requiredFlags, &resultFlags, &originalBuffer);

        /* Error out if the allocation failed. */
        if (!data)
        {
            _mutex_unlock(&g_mediaBufferManagerContext.mutex);
            return ERROR_DDI_MEDIABUFMGR_ALLOC_FAILED;
        } /* Endif */

        /* 
        ** Build the combined flags that are set for this buffer when it is added below.
        ** The buffer is marked as temporary and in use. Marking it as temporary
        ** will cause it to be freed when the caller releases it. 
        */
        bufferFlags = requiredFlags | resultFlags | kMediaBufferFlag_Temporary | kMediaBufferFlag_InUse;

#if RECORD_BUFFER_STATS
        /* Update statistics. */
        media_buffer_update_stats(&g_mediaBufferManagerContext.tempStats, TRUE);
        media_buffer_update_stats(&g_mediaBufferManagerContext.tempTypeStats[bufferType], TRUE);

        /* Increment the number of new temporary buffers allocated. */
        g_mediaBufferManagerContext.tempStats.newAllocs++;
        g_mediaBufferManagerContext.tempTypeStats[bufferType].newAllocs++;

        /* Increment realigned buffer count if appropriate. */
        if (resultFlags & kMediaBufferFlag_Realigned)
        {
            g_mediaBufferManagerContext.tempStats.realignedAllocs++;
            g_mediaBufferManagerContext.tempTypeStats[bufferType].realignedAllocs++;
        } /* Endif */

        /* Log the temporary allocation. */
#if MEDIA_BUFFER_DEBUG
        WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_DEBUG, 
            "bufmgr: allocated temp buffer %x [size=%d, flags=%x]\n", 
            (uint32_t)data, typeSize, bufferFlags);
#endif /* MEDIA_BUFFER_DEBUG */
#endif /* RECORD_BUFFER_STATS */

        /* 
        ** Add the new buffer to our list with the combined flags and a
        ** reference count of 1. 
        */
        status = media_buffer_add_internal(bufferType, bufferFlags, 1, data, originalBuffer, NULL);
        if (status != SUCCESS)
        {
            if (resultFlags & kMediaBufferFlag_Realigned)
            {
                _wl_mem_free(originalBuffer);
            }
            else
            {
                _wl_mem_free(data);
            } /* Endif */

            _mutex_unlock(&g_mediaBufferManagerContext.mutex);
            return status;
        } /* Endif */

        /* Return this new buffer to the caller. */
        *buffer = data;

        /* Decrement the free count. buffer_add() increments it so we need to counter this. */
        g_mediaBufferManagerContext.freeCount--;
    }

    /* Release mutex. */
    _mutex_unlock(&g_mediaBufferManagerContext.mutex);
    
    return SUCCESS;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_shrink_slots
* Returned Value   : void
* Comments         :
*   This function determines if the slot count can be reduced.
*   The array of buffers is examined starting at the end and moving towards
*   the beginning, stopping as soon as a valid entry is found. If there
*   were one or more contiguous invalid entries starting at the end, the
*   active slot count is reduced. This helps to speed searches for buffers
*   in media_buffer_acquire().
*
*END*--------------------------------------------------------------------*/
static void media_buffer_shrink_slots(void)
{ /* Body */
    int i;

    /* 
    ** Scan the buffer array backwards. When we exit the loop, i will
    ** be set to the number of slots from the first to the last valid one. 
    */
    for (i = g_mediaBufferManagerContext.slotCount; i > 0; --i)
    {
        /* Exit the loop when the slot prior to this one is valid. */
        if (g_mediaBufferManagerContext.buffers[i - 1].data)
        {
            break;
        } /* Endif */
    } /* Endfor */

    /* Shrink it! */
    g_mediaBufferManagerContext.slotCount = i;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_retain
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   Add a reference to an already acquired buffer.
*   This API lets a caller add a reference to a buffer that has previously been
*   returned from media_buffer_acquire(), probably by another piece of code.
*   When the caller is done with the buffer, they can call media_buffer_release()
*   just like normal. Only when the reference count for the buffer falls to
*   zero will it actually be released.
*
*END*--------------------------------------------------------------------*/
RtStatus_t media_buffer_retain
(
    /* [IN/OUT] Buffer */
    SECTOR_BUFFER * buffer
)
{ /* Body */
    unsigned i;
    RtStatus_t result = ERROR_DDI_MEDIABUFMGR_INVALID_BUFFER;

    assert(buffer != NULL);
    assert(g_mediaBufferManagerContext.isInited);

    /* Acquire mutex. */
    _mutex_lock(&g_mediaBufferManagerContext.mutex);

    /* Scan the buffer array looking for a matching data pointer. */
    for (i = 0; i < g_mediaBufferManagerContext.slotCount; ++i)
    {
        MediaBufferInfo_t * info = &g_mediaBufferManagerContext.buffers[i];

        /* Is this the matching buffer? */
        if (info->data == buffer)
        {
            /* Add one reference. */
            ++info->refCount;
            result = SUCCESS;
            
#if MEDIA_BUFFER_DEBUG
            WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_INFO,
                "bufmgr: retain, increment refcount of buf %d to %d \n", 
                i, info->refCount);
#endif /* MEDIA_BUFFER_DEBUG */
            
            break;
        } /* Endif */
    } /* Endfor */

    /* Put the mutex and return the status code. */
    _mutex_unlock(&g_mediaBufferManagerContext.mutex);
    return result;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_release
* Returned Value   : SUCCESS or ERROR
*   - SUCCESS The buffer is now available for other use.
*   - ERROR_DDI_MEDIABUFMGR_INVALID_BUFFER The buffer passed into this
*   function was not obtained from the buffer manager, or something
*   serious has gone wrong.
* Comments         :
*   This function return a previously acquired buffer to the buffer manager's control.
*   The reference count for the provided buffer is decremented by one. Only when
*   this count reaches zero will the buffer actually be released. The type of buffer,
*   either permanent or temporary, determines what happens when the buffer is
*   released.
*   Permanent buffers that are released are made immediately available to other
*   callers, as soon as this function returns.
*   Temporary buffers are not immediately deallocated. They become available
*   for other callers for a limited amount of time, currently 200 ms. If not
*   re-acquired during that time, the temporary buffer is freed for good.
*   However, if the buffer is used again before the timeout expires then the
*   process starts over again when it is next released.
*
*END*--------------------------------------------------------------------*/
RtStatus_t media_buffer_release
(
    /* 
    ** [IN] Pointer to the buffer to release. 
    ** This buffer must have been previously obtained by a call to buffer_acquire().
    */
    SECTOR_BUFFER * buffer
)
{ /* Body */
    unsigned i;
    RtStatus_t result = ERROR_DDI_MEDIABUFMGR_INVALID_BUFFER;

    assert(buffer != NULL);
    assert(g_mediaBufferManagerContext.isInited);

    /* Acquire mutex. */
    _mutex_lock(&g_mediaBufferManagerContext.mutex);

    /* Scan the buffer array looking for a matching data pointer. */
    for (i = 0; i < g_mediaBufferManagerContext.slotCount; ++i)
    {
        MediaBufferInfo_t * info = &g_mediaBufferManagerContext.buffers[i];

        if (info->data == buffer)
        {
            /* Decrement the reference count. */
            if (--info->refCount > 0)
            {
                /* There are still references to the buffer, so don't actually
                release it yet! */
                result = SUCCESS;
                break;
            } /* Endif */
            
            /* Different actions depending on whether the buffer is temporary. */
            if (info->flags & kMediaBufferFlag_Temporary)
            {
                /* Temporary buffer. */

#if RECORD_BUFFER_STATS
                /* Log the release. */
#if MEDIA_BUFFER_DEBUG
                WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_DEBUG, 
                    "bufmgr: temp buffer %x will timeout in %d ms\n", 
                    (uint32_t)info->data, TEMPORARY_BUFFER_TIMEOUT_MS);
#endif /* MEDIA_BUFFER_DEBUG */
#endif /* RECORD_BUFFER_STATS */

                /* Reset the buffer's timeout. */
                _time_get_ticks(&info->timeout);
                _time_add_msec_to_ticks(&info->timeout,  TEMPORARY_BUFFER_TIMEOUT_MS);

                /* If there is not already a temp buffer waiting to timeout, set this one up. */
                if (g_mediaBufferManagerContext.nextTimeout == NO_NEXT_TIMEOUT)
                {
#if MEDIA_BUFFER_DEBUG
                    WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_INFO,
                        "bufmgr: release, set timer for delete buf %d \n", i);
#endif /* MEDIA_BUFFER_DEBUG */

                    g_mediaBufferManagerContext.nextTimeout = i;
                    
                    g_mediaBufferManagerContext.timeoutTimer  = _timer_start_oneshot_after( media_buffer_timeout, 0, TIMER_ELAPSED_TIME_MODE, TEMPORARY_BUFFER_TIMEOUT_MS);
                } /* Endif */
            }
            else
            {
                /* Permanent buffer. */

#if RECORD_BUFFER_STATS
                /* Update statistics. */
                media_buffer_update_stats(&g_mediaBufferManagerContext.permStats, FALSE);
                media_buffer_update_stats(&g_mediaBufferManagerContext.permTypeStats[info->bufferType], FALSE);

                /* Log the release. */
#if MEDIA_BUFFER_DEBUG
                WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_DEBUG,
                "bufmgr: releasing perm buffer %x [#%d]\n", (uint32_t)info->data, i);
#endif /* MEDIA_BUFFER_DEBUG */
#endif /* RECORD_BUFFER_STATS */
            } /* Endif */

            /* Now make this buffer available for another caller to use. */
            info->flags &= ~kMediaBufferFlag_InUse;

            /* Increment the number of available buffers. */
            g_mediaBufferManagerContext.freeCount++;

            /* All good, so return success. We can exit the loop now. */
            result = SUCCESS;
            break;
        } /* Endif */
    } /* Endfor */
    
    /* Put the mutex and return the status code. */
    _mutex_unlock(&g_mediaBufferManagerContext.mutex);
    return result;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_setup_next_timeout
* Returned Value   : void
* Comments         :
*   This function looks for the next temporary buffer to timeout and set up the timer.
*   PRE: The media buffer manager's mutex must have already been acquired.
*   POST: g_mediaBufferManagerContext.nextTimeout is set to the index of the
*   temporary buffer that will timeout next.
*
*END*--------------------------------------------------------------------*/
static void media_buffer_setup_next_timeout(void)
{ /* Body */
    unsigned i;
    int nextToTimeout = NO_NEXT_TIMEOUT;
    int32_t timeoutms = INT32_MAX;
    MQX_TICK_STRUCT scanStartTime;
    int32_t msec ;

    /* Assume that we don't have any waiting timer in g_mediaBufferManagerContext */

    _time_get_ticks(&scanStartTime);
    /* Search for another inactive temporary buffer. */
    for (i = 0; i < g_mediaBufferManagerContext.slotCount; ++i)
    {
        MediaBufferInfo_t * info = &g_mediaBufferManagerContext.buffers[i];

        /* Is this a valid, inactive, temporary buffer? */
        if (info->data && (info->flags & kMediaBufferFlag_Temporary) && !(info->flags & kMediaBufferFlag_InUse))
        {
            /* 
            ** Convert absolute timeout to relative time. The timeout time should hopefully
            ** be bigger than the current time, although this is not guaranteed. 
            */
            bool isOverflow=FALSE;
            msec = _time_diff_seconds(&scanStartTime,  &info->timeout, &isOverflow);

            /* 
            ** Make sure we never have a negative time. 
            ** Again, make sure the diff time not too large: !isOverflow
            */
            if ( (msec < 0) || isOverflow)
            {
                msec = 0;
            } /* Endif */
            
            /* Keep track of the buffer with the lowest interval to timeout. */
            if (msec < timeoutms )
            {
                timeoutms = msec;
                nextToTimeout = i;
            } /* Endif */
        } /* Endif */
    } /* Endfor */

    /* Do we have another buffer waiting to timeout? */
    if (nextToTimeout != NO_NEXT_TIMEOUT)
    {
        /* Delay at least two ticks. */
        if (timeoutms == 0)
        {
            timeoutms = 2*1000 / _time_get_ticks_per_sec();
        } /* Endif */
        
        assert(timeoutms < 300);
        g_mediaBufferManagerContext.timeoutTimer  = _timer_start_oneshot_after(
        media_buffer_timeout, 0, TIMER_ELAPSED_TIME_MODE, timeoutms);
    } /* Endif */    

    g_mediaBufferManagerContext.nextTimeout = nextToTimeout;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_dispose_temporary
* Returned Value   : void
* Comments         :
*   Deferred procedure call to dispose of a temporary buffer.
*
*END*--------------------------------------------------------------------*/
static void media_buffer_dispose_temporary
(
    uint32_t unused
)
{ /* Body */
    MediaBufferInfo_t * info;
#if MEDIA_BUFFER_DEBUG
    WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_INFO,
    "bufmgr: dispose, try to dispose buf %d \n", g_mediaBufferManagerContext.bufferToDispose);
#endif /* MEDIA_BUFFER_DEBUG */
    
    /* Acquire mutex. */
    _mutex_lock(&g_mediaBufferManagerContext.mutex);

    /* 
    ** Check to make sure there is still a buffer to dispose and someone hasn't
    ** come along and acquired it between when the timer fired and the DPC
    ** actually started executing. 
    */
    if (g_mediaBufferManagerContext.bufferToDispose == NO_NEXT_TIMEOUT)
    {
        _mutex_unlock(&g_mediaBufferManagerContext.mutex);
        return;
    } /* Endif */

    assert(g_mediaBufferManagerContext.bufferToDispose >= 0 && g_mediaBufferManagerContext.bufferToDispose < (int)g_mediaBufferManagerContext.slotCount);

    /* Get the buffer info. */
    info = &g_mediaBufferManagerContext.buffers[g_mediaBufferManagerContext.bufferToDispose];

#if RECORD_BUFFER_STATS
    /* Update statistics. */
    media_buffer_update_stats(&g_mediaBufferManagerContext.tempStats, FALSE);
    media_buffer_update_stats(&g_mediaBufferManagerContext.tempTypeStats[info->bufferType], FALSE);

    /* Log the release. */
#if MEDIA_BUFFER_DEBUG
    WL_LOG(WL_MODULE_MEDIABUFFER, WL_LOG_DEBUG, 
    "bufmgr: freeing temp buffer %x\n", (uint32_t)info->data);
#endif /* MEDIA_BUFFER_DEBUG */

#endif /* RECORD_BUFFER_STATS */

    /* 
    ** Dispose of this temporary buffer. If we realigned the buffer then we have to
    ** pass the actual pointer back and not the aligned one. 
    */
    if (info->flags & kMediaBufferFlag_Realigned)
    {
        _wl_mem_free(info->originalBuffer);
    }
    else
    {
        _wl_mem_free(info->data);
    } /* Endif */
    
    memset(info, 0, sizeof(*info));

    /* Decrement the number of buffers in the array. */
    g_mediaBufferManagerContext.bufferCount--;
    g_mediaBufferManagerContext.freeCount--;

    /* Decrement the slot count if possible. */
    media_buffer_shrink_slots();

    /* Search for another buffer to timeout. */
    _timer_cancel(g_mediaBufferManagerContext.timeoutTimer);
    
    media_buffer_setup_next_timeout();

    /* No buffer pending disposal. */
    g_mediaBufferManagerContext.bufferToDispose = NO_NEXT_TIMEOUT;

    /* Release the mutex. */
    _mutex_unlock(&g_mediaBufferManagerContext.mutex);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_timeout
* Returned Value   : void
* Comments         :
*   Timer expiration function to time out temporary buffers.
*   All this timer function does is queue up a DPC to do the actual work. This
*   is necessary because application timers have severe limits on the ThreadX
*   APIs that can be called.
*
*END*--------------------------------------------------------------------*/
void media_buffer_timeout
(
    /* [IN] Timer id */
    _timer_id id, 
    
    /* [IN] Pointer holds data */
    void *data_ptr, 
    
    /* [IN] Seconds */
    uint32_t seconds, 
    
    /* [IN] Milliseconds */
    uint32_t milliseconds
)
{ /* Body */
    /* RtStatus_t status; */

    /* We want to dispose of the buffer that just timed out. */
    g_mediaBufferManagerContext.bufferToDispose = g_mediaBufferManagerContext.nextTimeout;

    media_buffer_dispose_temporary(0);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_get_property
* Returned Value   : SUCCESS or ERROR
* Comments         :
*   This function reads an arbitrary property of a buffer.
*
*END*--------------------------------------------------------------------*/
RtStatus_t media_buffer_get_property
(
    /* [IN] Pointer buffer */
    SECTOR_BUFFER * buffer, 

    /* [IN] Property */
    uint32_t whichProperty, 

    /* [OUT] Value of property */
    void * value
)
{ /* Body */
    unsigned i;
    RtStatus_t result = ERROR_DDI_MEDIABUFMGR_INVALID_BUFFER;
    MediaBufferInfo_t * info = NULL;

    assert(buffer != NULL);
    assert(value != NULL);
    assert(g_mediaBufferManagerContext.isInited);

    /* Acquire mutex. */
    _mutex_lock(&g_mediaBufferManagerContext.mutex);

    /* Scan the buffer array looking for a matching data pointer. */
    for (i = 0; i < g_mediaBufferManagerContext.slotCount; ++i)
    {
        info = &g_mediaBufferManagerContext.buffers[i];

        if (info->data == buffer)
        {
            /* Found our buffer, so exit the scan loop! */
            break;
        } /* Endif */
    } /* Endfor */
    
    if (info)
    {
        switch (whichProperty)
        {
            case kMediaBufferProperty_Size:
                *(uint32_t *)value = info->length;
                break;
            
#if RECORD_BUFFER_STATS
            case kMediaBufferProperty_Type:
                *(MediaBufferType_t *)value = info->bufferType;
                break;
#endif /* RECORD_BUFFER_STATS */
            
            case kMediaBufferProperty_Flags:
                *(uint32_t *)value = info->flags;
                break;
            
            case kMediaBufferProperty_IsTemporary:
                *(bool *)value = (info->flags & kMediaBufferFlag_Temporary) != 0;
                break;
                
            case kMediaBufferProperty_IsInUse:
                *(bool *)value = (info->flags & kMediaBufferFlag_InUse) != 0;
                break;
                
            case kMediaBufferProperty_ReferenceCount:
                *(uint32_t *)value = info->refCount;
                break;
            
            case kMediaBufferProperty_Timeout:
                if (!(info->flags & kMediaBufferFlag_Temporary))
                {
                    /* Set the result for permanent buffers to -1. */
                    *(uint32_t *)value = (uint32_t)-1;
                }
                else if (info->flags & kMediaBufferFlag_InUse)
                {
                    /* Temp buffers that are currently in use don't have a timeout, yet. */
                    *(uint32_t *)value = 0;
                }
                else
                {
                    *(MQX_TICK_STRUCT *)value = info->timeout;
                } /* Endif */
                break;
            
            default:
                result = ERROR_DDI_MEDIABUFMGR_INVALID_PROPERTY;
        } /* Endswitch */
    } /* Endif */
    
    /* Put the mutex and return the status code. */
    _mutex_unlock(&g_mediaBufferManagerContext.mutex);
    return result;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_count_all_temporary
* Returned Value   : Number of temporary buffer
* Comments         :
*   This function returns number of temporary buffer
*
*END*--------------------------------------------------------------------*/
uint32_t media_buffer_count_all_temporary()
{ /* Body */
    uint32_t i, count = 0;
    for (i=0; i< g_mediaBufferManagerContext.slotCount; i++)
    {
        if (g_mediaBufferManagerContext.buffers[i].flags & kMediaBufferFlag_Temporary)
        {
            count ++;
        } /* Endif */
        
    } /* Endfor */
    return count;      
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : media_buffer_force_dispose_temporary
* Returned Value   : void
* Comments         :
*   This function disposes temporary buffer
*
*END*--------------------------------------------------------------------*/
void media_buffer_force_dispose_temporary()
{   
   uint32_t i;
   MediaBufferInfo_t * info;
   /*uint32_t bufCount;*/
   
   // TODO: review force dispose temporary function, since some temporary func still have refcount > 0 
   
   _mutex_lock(&g_mediaBufferManagerContext.mutex);
   /*bufCount = g_mediaBufferManagerContext.bufferCount;*/
   for (i=0; i< g_mediaBufferManagerContext.slotCount ; i++ )
   {
      if (g_mediaBufferManagerContext.buffers[i].flags & kMediaBufferFlag_Temporary)
      {
            
            /* Get the buffer info.*/
         info = &g_mediaBufferManagerContext.buffers[i];

#if RECORD_BUFFER_STATS
            /* Update statistics. */
            media_buffer_update_stats(&g_mediaBufferManagerContext.tempStats, FALSE);
            media_buffer_update_stats(&g_mediaBufferManagerContext.tempTypeStats[info->bufferType], FALSE);
#endif /* RECORD_BUFFER_STATS */

            /* 
            ** Dispose of this temporary buffer. If we realigned the buffer then we have to
            ** pass the actual pointer back and not the aligned one. 
            */
            if (info->flags & kMediaBufferFlag_Realigned)
            {
                _wl_mem_free(info->originalBuffer);
            }
            else
            {
                _wl_mem_free(info->data);
            } /* Endif */
            memset(info, 0, sizeof(*info));

         /* Decrement the number of buffers in the array.*/
         g_mediaBufferManagerContext.bufferCount--;
         g_mediaBufferManagerContext.freeCount--;
      }
         
   } /* End for */
      
   _mutex_unlock(&g_mediaBufferManagerContext.mutex);
   
}

/* EOF */
