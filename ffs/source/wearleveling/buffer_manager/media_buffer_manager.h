#if !defined(__media_buffer_manager_h__)
#define __media_buffer_manager_h__
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
*   This file contains public API managing shared buffers for media code.
*   The media buffer manager allows media drivers and related code to share a
*   set of common buffers, which is much more memory efficient than each code
*   module having its own buffers. It can work with both permanent, pre-allocated
*   buffers as well as temporary buffers it dynamically allocates itself. The
*   application can add any number of permanent buffers, thus controlling the
*   tradeoff between performance and memory. This also allows the application to
*   handle situations such as when NCNB memory cannot be allocated at runtime.
*   
*   The buffers under control of the buffer manager have several attributes.
*   The first is their type. Instead of specifying buffers by a size in
*   bytes, an enumerated list of types is used. This allows the caller to
*   pass along extra semantic information about the purpose of the buffer.
*   Currently this is only useful for programmers reviewing the code, but
*   it could potentially be put to other purposes in the future.
*   
*   The two types of buffers currently supported are:
*       - #kMediaBufferType_Sector: A buffer at least as large as
*       the page size of the currently attached NAND device.
*       - #kMediaBufferType_Auxiliary: A buffer large enough to
*       hold the auxiliary data required by the ECC8 engine.
*
*   In addition to a type, each buffer has a set of flags. The only flags
*   that are implemented now are as follows:
*       - #kMediaBufferFlag_NCNB: The buffer is located in non-cacheable,
*       non-bufferable memory.
*       - #kMediaBufferFlag_FastMemory: The buffer is within "fast"
*       memory, usually OCRAM.
*   
*   In order for the buffer manager to be used, it must first be initialised.
*   This is done with a call to media_buffer_init(), which takes no parameters.
*   A set of buffers is pre-allocated by the manager so that it is immediately
*   usable. If you wish to add additional buffers, see the media_buffer_add() API.
*   
*   To get a buffer to use, a driver will call media_buffer_acquire(). It provides
*   information about the buffer it needs, including the type and a set of
*   desired flags. The buffer manager will make an attempt to
*   match the specifications exactly, but if that is not possible it will
*   try to find a close match amongst the available buffers already under
*   its control. Only as a last resort will it allocate memory using the
*   malloc() and specialised related functions from the DMI API (used to
*   allocate memory with special conditions such as NCNB). Wherever the buffer
*   comes from, it will always match the requested flags. However, a larger
*   buffer may be used for a small request
*   
*   These allocated buffers are called temporary buffers due to the fact that
*   they are deallocated when the caller is finished with them. Buffers added
*   with the buffer_add() API are called permanent buffers.
*   
*   The caller can use the buffer it acquired for as long as it likes, and
*   can hold any number of buffers at once. When finished, a call to
*   media_buffer_release() puts the buffer back under control of the buffer manager
*   and makes it available for other callers to use.
*   
*   If the buffer being released is a temporary buffer, it is returned to the
*   manager just like permanent buffers, but only for a short time. If the
*   buffer is still unused after this time expires, it is completely deallocated.
*   Reacquiring a temporary buffer waiting to expire will restart the process.
*   This mechanism keeps from repeatedly allocating and freeing buffers during
*   bursts of activity, while allowing efficient use of memory during idle time.
*
*
*END************************************************************************/

/* 
** Includes 
*/

#include "wl_common.h"

/* 
** Type definations 
*/

/* 
** Buffer type
** Classes of buffers supported by the media buffer manager.
** Each buffer type has an associated size that may vary depending upon
** runtime conditions. These types are used to request buffers instead of
** exact sizes because the type carries additional semantic content that
** may be useful. 
*/
typedef enum _media_buffer_type {
    /* 
    ** A buffer at least as large as the maximum sector size 
    ** supported by the system. 
    */
    kMediaBufferType_Sector = 0,
    
    /* 
    ** Buffer at least big enough to hold the auxiliary buffer required
    ** by the ECC8 engine for the largest supported NAND page size. 
    */
    kMediaBufferType_Auxiliary,
    
    /* A buffer large enough to hold a full NAND page, including metadata. */
    kMediaBufferType_NANDPage,
    
    /* Number of buffer types. */
    kMediaBufferType_Count
} MediaBufferType_t;

/* 
** Flags
** Optional flags for use with media_buffer_acquire(). 
*/
enum _media_buffer_acquire_flags
{
    /* No flags are set. */
    kMediaBufferFlag_None = 0,
    
    /* Buffer must be in non-cached, non-bufferable memory. */
    kMediaBufferFlag_NCNB = (1 << 0)
    
    /* Buffer resides in fast memory. */
    /* kMediaBufferFlag_FastMemory = (1 << 1) */
};

/* 
** Properties
** Available properties of buffers.
** These properties can be accessed with the media_buffer_get_property() API or its templated
** variants. Just pass one of the property selectors to the API and provide appropriate
** storage for the result value. The documentation for each of the property selectors includes
** the data type of the property's value. 
*/
enum _media_buffer_properties
{
    /* The buffer's size in bytes. [uint32_t] */
    kMediaBufferProperty_Size = (('b'<<24)|('s'<<16)|('i'<<8)|('z')), /* 'bsiz' */
    
    /* The original type of the buffer. [MediaBufferType_t] */
    kMediaBufferProperty_Type = (('b'<<24)|('t'<<16)|('y'<<8)|('p')), /* 'btyp' */
    
    /* The buffer's set of flags set when the buffer was added. [uint32_t] */
    kMediaBufferProperty_Flags = (('b'<<24)|('f'<<16)|('l'<<8)|('g')), /* 'bflg' */
    
    /* 
    ** Whether the buffer is temporary or permanent. [bool]
    ** true The buffer is temporary. You can get the #kMediaBufferProperty_Timeout
    ** property to see when the buffer will expire.
    ** false The buffer is permanent. 
    */
    kMediaBufferProperty_IsTemporary = (('b'<<24)|('t'<<16)|('m'<<8)|('p')), /* 'btmp' */
    
    /* Whether the buffer is currently owned by a client. [bool] */
    kMediaBufferProperty_IsInUse = (('b'<<24)|('i'<<16)|('n'<<8)|('u')), /* 'binu' */
    
    /* 
    ** Number of references to the buffer. [uint32_t]
    ** If the buffer is not currently in use, the reference count will always be zero. 
    */
    kMediaBufferProperty_ReferenceCount = (('r'<<24)|('e'<<16)|('t'<<8)|('#')), /* 'ret#' */
    
    /* 
    ** Timeout when the buffer will expire. [uint32_t]
    ** This property only applies to temporary buffers that are not currently in use. 
    ** It's value is the absolute time in system ticks when the buffer will expire, 
    ** assuming it is not acquired before then. 
    ** If the buffer is not temporary, then the result will be -1 (0xffffffff). 
    ** If the buffer is temporary but currently in use by someone, 
    ** the result will be zero. 
    */
    kMediaBufferProperty_Timeout = (('b'<<24)|('t'<16)|('m'<<8)|('o')) /* 'btmo' */
};

/* 
** Function prototypes 
*/

#ifdef __cplusplus
extern "C" {
#endif

RtStatus_t media_buffer_init(void);

RtStatus_t media_buffer_deinit(void);

void media_buffer_report_stats(void);

RtStatus_t media_buffer_add(MediaBufferType_t bufferType, uint32_t bufferFlags, SECTOR_BUFFER * buffer);

RtStatus_t media_buffer_acquire(MediaBufferType_t bufferType, uint32_t requiredFlags, SECTOR_BUFFER ** buffer);

RtStatus_t media_buffer_retain(SECTOR_BUFFER * buffer);

RtStatus_t media_buffer_release(SECTOR_BUFFER * buffer);

RtStatus_t media_buffer_get_property(SECTOR_BUFFER * buffer, uint32_t whichProperty, void * value);

#ifdef __cplusplus
}

/* Handy template form of media_buffer_get_property(). */
template <typename T>
T media_buffer_get_property(SECTOR_BUFFER * buffer, uint32_t whichProperty, RtStatus_t * status=0) 
{
    T value = 0;
    RtStatus_t localStatus = media_buffer_get_property(buffer, whichProperty, &value);
    if (status)
    {
        *status = localStatus;
    }
    return value;
}

#endif /* __cplusplus */

#endif /* __media_buffer_manager_h__ */

/* EOF */
