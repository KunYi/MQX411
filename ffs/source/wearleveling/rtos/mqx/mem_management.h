/*HEADER**********************************************************************
*
* Copyright 2012 Freescale Semiconductor, Inc.
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   Memory management and profiling for WL Module 
*
*END************************************************************************/
#ifndef _mem_management_h_
#define _mem_management_h_

#define _mm_internal_alloc              _mem_alloc_system
#define _mm_internal_alloc_zero         _mem_alloc_system_zero
#define _mm_internal_free               _mem_free
#define _mm_internal_set                memset
#define _mm_internal_zero               _mem_zero
#define _mm_internal_memcpy             _mem_copy


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    extern void * mm_alloc(_mem_size request_size, const char * file_name, 
    int file_loc, bool isZero, bool is_from_operator);
    extern _mqx_uint mm_free(void * buf);
    extern void mm_display(void);
    extern void mm_display_unfree(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#ifdef __cplusplus
#if defined(__IAR_SYSTEMS_ICC__) || defined(__CC_ARM) || defined (__GNUG__) // TODO: IAR or KEIL
typedef unsigned int size_t;
#else
typedef _mem_size size_t;
#endif

void * operator new (size_t size, const char *fname, int floc);
void * operator new[] (size_t size, const char *fname, int floc);
void operator delete (void *p);
void operator delete[] (void *p);

#endif /* __cplusplus */

#endif /* _mem_management_h_ */

/* EOF */
