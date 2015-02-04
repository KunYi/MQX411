
/*HEADER**********************************************************************
*
* Copyright 2008-2011 Freescale Semiconductor, Inc.
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
*   This file contains runtime support for the KEIL Compiler.
*
*
*END************************************************************************/
#include "mqx.h"
#include "bsp.h"

int __aeabi_atexit(void *, void *, void *);
void *malloc(unsigned int);
void *calloc(unsigned int, unsigned int);
void free(void *);
void init_hardware(void);
void __main(void);

/*!
 * \brief Override C/C++ runtime __aeabi_atexit function in KEIL to avoid the call 
 *  to malloc and free before MQX starts http//infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka3951.html
 * 
 * \param void *object_ptr 
 * \param void *destructor_ptr 
 * \param void *__dso_handle_ptr 
 *
 * \return int
 */
int __aeabi_atexit(void *object_ptr, void *destructor_ptr, void *__dso_handle_ptr)
{
   return 1;
}

/*!
 * \brief Override C/C++ runtime heap allocation function in KEIL
 *
 * \return pointer
 */
void *malloc(unsigned int bytes)
{
  return _mem_alloc_system(bytes);
}

/*!
 * \brief Override C/C++ runtime heap allocation function in KEIL
 *
 * \return pointer
 */
void *calloc(unsigned int n, unsigned int z)
{
   return _mem_alloc_system_zero(n*z);
} 

/*!
 * \brief Override C/C++ runtime heap deallocation function in KEIL
 * 
 * \param void *p 
 */
void free(void *p)
{
   _mem_free(p);
}

/*!
 * \brief Perform necessary toolchain startup routines before main()
 */
void toolchain_startup(void)
{
    init_hardware();
    __main();
}

