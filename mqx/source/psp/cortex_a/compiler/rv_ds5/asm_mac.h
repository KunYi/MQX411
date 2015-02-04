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
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*   This file contains macros used by the Keil assembler
*
*
*END************************************************************************/

#ifndef __asm_mac_h__
#define __asm_mac_h__ 1

#define ASM_PREFIX(x) x
          
#define ASM_EXTERN(label)        EXTERN label

#define ASM_ALIGN(value)         ALIGN value
#define ASM_LABEL(label)         label
#define ASM_PUBLIC(label)        EXPORT label
#define ASM_SET(label,value)     label SETA value
#define ASM_CONST16(value)       DCWU value
#define ASM_CONST32(value)       DCDU value
#define ASM_LABEL_CONST32(l,v)   ASM_LABEL(label) ASM_CONST32(value)
#define ASM_DATA_SECTION(label)  AREA |label|, DATA
#define ASM_CODE_SECTION(label)  AREA |label|, CODE, ALIGN=2
#define ASM_END                  END
#define ASM_EQUATE(label, value) label   EQU  value

#define ASM_COMP_SPECIFIC_DIRECTIVES
#define ASM_PUBLIC_BEGIN(name)
#define ASM_PUBLIC_FUNC(name)
#define ASM_PUBLIC_END(name)
 PRESERVE8 {TRUE}

#endif /* __asm_mac_h__ */
