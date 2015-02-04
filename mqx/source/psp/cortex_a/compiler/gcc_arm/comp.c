
/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
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
*   This file contains runtime support for the ARM GCC.
*   
*
*END************************************************************************/

#include <stdio.h>
#include "mqx.h"

void *malloc(_mem_size);
void *calloc(_mem_size, _mem_size);
void free(void *);
void exit(int);
void _exit(int);
void init_hardware(void);
void __libc_init_array(void);
void __libc_fini_array(void);
void atexit(void *);
int  main(void);

void *__dso_handle = NULL;

/*!
 * \brief Dummy function which avoid GCC to register destructors of global objects. 
 *  By default GCC use '__cxa_atexit' function which register destructors to 
 *  LIFO. When LIFO is full, it performs malloc, but at this point there is 
 *  no heap, so malloc fails. 
 *
 * \return int
 */
int __cxa_atexit (void (*fn) (void *), void *arg, void *d)
{
    return 0;
}

/*!
 * \brief Perform custom action before main
 */
void _init(void)
{
    ;
}

/*!
 * \brief Perform custom action before exit
 */
void _fini(void)
{
    ;
}

/*!
 * \brief Override C/C++ runtime heap allocation function
 *
 * \return pointer
 */
void *malloc(_mem_size bytes)
{
    return _mem_alloc_system(bytes);
}

/*!
 * \brief Override C/C++ reentrant runtime heap allocation function
 *
 * \return pointer
 */
void *_malloc_r(void *reent, size_t nbytes)
{
    return _mem_alloc_system(nbytes);
}

/*!
 * \brief Override C/C++ runtime heap deallocation
 *
 * \return pointer
 */
void *calloc(_mem_size n, _mem_size z)
{
    return _mem_alloc_system_zero(n*z);
}

/*!
 * \brief Override C/C++ runtime heap deallocation function
 * 
 * \param p 
 */
void free(void *p)
{
    _mem_free(p);
}

typedef struct {
    uint32_t *  TARGET;
    uint32_t    BYTESIZE;
} STARTUP_ZEROTABLE_STRUCT, * STARTUP_ZEROTABLE_STRUCT_PTR;


typedef struct {
    uint32_t *  SOURCE;
    uint32_t *  TARGET;
    uint32_t    BYTESIZE;
} STARTUP_COPYTABLE_STRUCT, * STARTUP_COPYTABLE_STRUCT_PTR;


extern STARTUP_ZEROTABLE_STRUCT __zero_table_start__[];
extern STARTUP_ZEROTABLE_STRUCT __zero_table_end__[];
extern STARTUP_COPYTABLE_STRUCT __copy_table_start__[];
extern STARTUP_COPYTABLE_STRUCT __copy_table_end__[];


/*!
 * \brief setup zero sections, copy sections
 */
void data_startup(void)
{
    register uint32_t offset, i;

// TODO: address alignment assertion
    /* zero sections */
    for (i = 0; &__zero_table_start__[i] < __zero_table_end__; i++)
    {
        for (
            offset = 0;
            offset < __zero_table_start__[i].BYTESIZE;
            offset += sizeof(uint32_t)
        )
            (*(uint32_t*)((void*)__zero_table_start__[i].TARGET + offset)) = 0;
    }

// TODO: address alignment & overlapping assertion
    /* copy sections */
    for (i = 0; &__copy_table_start__[i] < __copy_table_end__; i++)
    {
        for (
            offset = 0;
            offset < __copy_table_start__[i].BYTESIZE;
            offset += sizeof(uint32_t)
        )
            (*(uint32_t*)((void*)__copy_table_start__[i].TARGET + offset))
                = (*(uint32_t*)((void*)__copy_table_start__[i].SOURCE + offset));
    }
}

/*!
 * \brief Perform necessary toolchain startup routines before main()
 */
void toolchain_startup(void)
{
// initialize necessary hardware (clocks, ddr, ...)
    init_hardware();
// initialize data - copy and zero sections
// static variables can be used on return
    data_startup();
// register destructor calls of static objects
    atexit(__libc_fini_array);
// run constructor calls of static objects
    __libc_init_array();
// run main, if return go to exit
    exit(main());
}

/*!
 * \brief Required implementation
 * 
 * \param status 
 */
void _exit(int status)
{
// disable all interrupts, run infinite loop
    __asm("cpsid ifa");
    while(1);
}


/*!
 * \brief Required implementation, we don't use this legacy feature for more information 
 *  see http//www.unix.com/man-page/FreeBSD/2/sbrk/
 *
 * \param increment
 */
void *_sbrk(intptr_t increment)
{
    _exit(-1);
}


