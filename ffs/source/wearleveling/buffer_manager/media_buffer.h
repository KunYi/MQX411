#if !defined(__media_buffer_h__)
#define __media_buffer_h__
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
*   This file contains classes managing media buffer
*
*
*END************************************************************************/

#include "media_buffer_manager.h"
#include "ddi_media.h"
#include "sectordef.h"
#include <string.h>

/* 
** Type definitions 
*/

#if defined(__cplusplus)

/* Utility class to manage a media buffer. */
class MediaBuffer
{
public:
    /* Default constructor. */
    inline MediaBuffer()
    :   m_buffer(NULL)
    {
    }
    
    /* Constructor taking a previously acquired buffer. */
    inline MediaBuffer(void * buf)
    :   m_buffer((SECTOR_BUFFER *)buf)
    {
        if (buf)
        {
#if DEBUG
            assert(media_buffer_retain(m_buffer) == SUCCESS);
#else
            /* We cannot return an error, so ignore it. */
            media_buffer_retain(m_buffer);
#endif /* Endif DEBUG */
        }
        
    }
    
    /* 
    ** Copy constructor.
    ** When copying another buffer object, the other's buffer is simply retained. 
    */
    MediaBuffer(const MediaBuffer & other)
    :   m_buffer(other.m_buffer)
    {
#if DEBUG
        assert(media_buffer_retain(m_buffer) == SUCCESS);
#else
        /* We cannot return an error, so ignore it. */
        media_buffer_retain(m_buffer);
#endif /* Endif DEBUG */
    }
    
    /* Destructor that releases the buffer. */
    inline ~MediaBuffer()
    {
        release();
    }
    
    /* 
    ** Acquire a new buffer.
    ** It is an error to call this method when the instance already has a buffer associated
    ** with it. 
    */
    inline RtStatus_t acquire(MediaBufferType_t bufferType, uint32_t flags=kMediaBufferFlag_None)
    {
        /* Make sure we don't already have a buffer. */
        release();
        
        /* Allocate or reuse a buffer. */
        return media_buffer_acquire(bufferType, flags, &m_buffer);
    }
    
    /* 
    ** Set the buffer to a previously acquired one.
    ** buf Pointer to a buffer returned from media_buffer_acquire(). You can also
    ** pass NULL to clear the internal buffer pointer. 
    */
    inline RtStatus_t set(void * buf)
    {
        /* Release any previous buffer. */
        release();
        
        /* Save the provided buffer address. */
        m_buffer = reinterpret_cast<SECTOR_BUFFER *>(buf);

        /* Check for a non-NULL buffer. */
        if (buf)
        {
            /* Retain the buffer we were given. */
            return media_buffer_retain(m_buffer);
        }
        else
        {
            return SUCCESS;
        }
    }
    
    /* Assignment operator. Simply retains the buffer. */
    MediaBuffer & operator = (SECTOR_BUFFER * buf)
    {
        set(buf);
        return *this;
    }
    
    /* Returns whether the buffer was succesfully acquired. */
    inline bool hasBuffer() const { return m_buffer != NULL; }
    
    /* Accessor for the buffer. */
    inline SECTOR_BUFFER * getBuffer() const { return m_buffer; }
    
    /* 
    ** Get the size of the buffer in bytes.
    ** If there is no buffer associated with this object, then 0 will be returned. 
    */
    inline unsigned getLength() const { return hasBuffer() ? getProperty<uint32_t>(kMediaBufferProperty_Size) : 0; }
    
    /* Conversion operators */
    inline SECTOR_BUFFER * operator * () { return m_buffer; }

    inline const SECTOR_BUFFER * operator * () const { return m_buffer; }
    
    inline operator SECTOR_BUFFER * () { return m_buffer; }
    
    inline operator const SECTOR_BUFFER * () const { return m_buffer; }
    
    inline operator void * () { return m_buffer; }
    
    inline operator const void * () const { return m_buffer; }
    
    inline operator bool () const { return hasBuffer(); }
    
    /* Fill the buffer with a pattern. */
    inline void fill(uint8_t value)
    {
        memset(m_buffer, value, getLength());
    }
    
    /* Get a property of the buffer. */
    template <typename T>
    inline T getProperty(uint32_t which) const
    {
        return media_buffer_get_property<T>(m_buffer, which);
    }
    
    /* Get a property of the buffer. */
    inline RtStatus_t getProperty(uint32_t which, void * value) const
    {
        return media_buffer_get_property(m_buffer, which, value);
    }
    
    /* Release the buffer back to the buffer manager's control. */
    inline void release()
    {
        if (hasBuffer())
        {
            media_buffer_release(m_buffer);
            m_buffer = NULL;
        }
    }
    
protected:
    /* The media buffer. */
    SECTOR_BUFFER * m_buffer;
    
};

/* Wraps a sector-sized media buffer. */
class SectorBuffer : public MediaBuffer
{
public:
    /* Constructor. */
    inline SectorBuffer() : MediaBuffer() {}
    
    /* Constructor taking a previously allocated buffer. */
    inline SectorBuffer(void * buf) : MediaBuffer(buf) {}
    
    /* Copy constructor. */
    inline SectorBuffer(const SectorBuffer & other) : MediaBuffer(other) {}
    
    /* Acquire a sector buffer. */
    inline RtStatus_t acquire() { return MediaBuffer::acquire(kMediaBufferType_Sector); }
};

/* Wraps an auxiliary buffer. */
class AuxiliaryBuffer : public MediaBuffer
{
public:
    /* Constructor. */
    inline AuxiliaryBuffer() : MediaBuffer() {}
    
    /* Constructor taking a previously allocated buffer. */
    inline AuxiliaryBuffer(void * buf) : MediaBuffer(buf) {}
    
    /* Copy constructor. */
    inline AuxiliaryBuffer(const AuxiliaryBuffer & other) : MediaBuffer(other) {}
    
    /* Acquire an auxiliary buffer. */
    inline RtStatus_t acquire() { return MediaBuffer::acquire(kMediaBufferType_Auxiliary); }
};

#endif /* defined(__cplusplus) */

#endif /* __media_buffer_h__ */

/* EOF */
