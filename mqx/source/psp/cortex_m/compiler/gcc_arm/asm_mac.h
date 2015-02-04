
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
*   This file contains macros used by the GCC assembler
*
*
*END************************************************************************/

#ifndef __asm_mac_h__
#define __asm_mac_h__   1

#define ASM_PREFIX(x)   x

#define ASM_EQUATE(label,value) .equ label, value
#define ASM_EXTERN(label)       .extern ASM_PREFIX(label)
#define ASM_ALIGN(value)        .balign value
#define ASM_PUBLIC(label)       .global ASM_PREFIX(label)
#define ASM_CONST16(value)      .short value
#define ASM_CONST32(value)      .long value
#define ASM_LABEL(label)        label:
#define ASM_LABEL_CONST32(label,value) label: .long value

#define ASM_CODE_SECTION(name)  .section name, "x"
#define ASM_DATA_SECTION(name)  .section name, "w"
#define ASM_END                 .end

#define ASM_COMP_SPECIFIC_DIRECTIVES .syntax unified

#define ASM_PUBLIC_BEGIN(name) .thumb_func
#define ASM_PUBLIC_FUNC(name)
#define ASM_PUBLIC_END(name)

#define ASM_CODE        .thumb
#define ASM_DATA
#endif /* __asm_mac_h__ */
