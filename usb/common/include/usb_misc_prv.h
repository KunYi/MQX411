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
*   This file contains USB Device API defines for state and function
*   returns.
*
*
*END************************************************************************/
#ifndef __usb_misc_prv_h__
#define __usb_misc_prv_h__ 1

#define USB_lock()                           _int_disable()
#define USB_unlock()                         _int_enable()
#define USB_install_isr(vector, isr, data)   _int_install_isr(vector, isr, data)

#if PSP_HAS_DATA_CACHE
// Not needed if we are allocating everything out of uncached memory
#define USB_dcache_invalidate()              _DCACHE_INVALIDATE()
#define USB_dcache_invalidate_line(p)        _DCACHE_INVALIDATE_LINE(p)
/* Based on the targets it should be modified, for coldfire it is MBYTES */
#define USB_dcache_invalidate_mlines(p,n)    _DCACHE_INVALIDATE_MBYTES(p, n)
#define USB_dcache_flush_line(p)             _DCACHE_FLUSH_LINE(p)
/* Based on the targets it should be modified, for coldfire it is MBYTES */
#define USB_dcache_flush_mlines(p,n)         _DCACHE_FLUSH_MBYTES(p, n)

#else

#define USB_dcache_invalidate()
#define USB_dcache_invalidate_line(p)
/* Based on the targets it should be modified, for coldfire it is MBYTES */
#define USB_dcache_invalidate_mlines(p,n)
#define USB_dcache_flush_line(p)
/* Based on the targets it should be modified, for coldfire it is MBYTES */
#define USB_dcache_flush_mlines(p,n)

#endif // PSP_HAS_DATA_CACHE

#if PSP_HAS_DATA_CACHE
    #define USB_mem_alloc_uncached(n)         _mem_alloc_system_uncached(n)
    #define USB_mem_alloc_uncached_zero(n)    _mem_alloc_system_zero_uncached(n)
    #define USB_mem_alloc_align_uncached(n,a) _mem_alloc_align_uncached(n, a)
#else
    #define USB_mem_alloc_uncached(n)         _mem_alloc_system(n)
    #define USB_mem_alloc_uncached_zero(n)    _mem_alloc_system_zero(n)
    #define USB_mem_alloc_align_uncached(n,a) _mem_alloc_align(n, a)
#endif // PSP_HAS_DATA_CACHE

#define USB_mem_alloc(n)                      _mem_alloc_system(n)
#define USB_mem_alloc_zero(n)                 _mem_alloc_system_zero(n)
#define USB_mem_free(ptr)                     _mem_free(ptr)
#define USB_mem_zero(ptr,n)                   _mem_zero(ptr,n)
#define USB_mem_copy(src,dst,n)               _mem_copy(src,dst,n)

#endif
