/*HEADER**********************************************************************
*
* Copyright 2010 Freescale Semiconductor, Inc.
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
*   This file defines the RealView compiler specific macros for MQX
*
*
*END************************************************************************/
#ifndef __comp_h__
#define __comp_h__   1

#ifdef __cplusplus
extern "C" {
#endif

register int _pc __asm("pc");
register int _lr __asm("lr");
#define __get_PC() _pc
#define __get_LR() _lr

extern void __set_BASEPRI(uint32_t);

#define _ASM_NOP()   __nop()
#define _ASM_WFI()   __wfi()

#define ISB()           __asm{isb}
#define DSB()           __asm{dsb}
#define DMB()           __asm{dmb}

#if PSP_MQX_CPU_IS_ARM_CORTEX_M4
#define _PSP_SET_ENABLE_SR(x)   {   \
    VCORTEX_SCB_STRUCT_PTR tmp = (VCORTEX_SCB_STRUCT_PTR)&(((CORTEX_SCS_STRUCT_PTR)CORTEX_PRI_PERIPH_IN_BASE)->SCB);    \
    if ((x & 0xf0) == 0x20) while (1) {};   \
    if (!(tmp->ICSR & (1 << 28))) tmp->SHPR3 = (tmp->SHPR3 & 0xff00ffff) | ((((x) - 0x20) & 0xff) << 16);   \
    if ((x & 0xf0) == 0x10) while (1) {};   \
    __set_BASEPRI(x);\
}
#define _PSP_SET_DISABLE_SR(x)  _PSP_SET_ENABLE_SR(x)
#elif PSP_MQX_CPU_IS_ARM_CORTEX_M0
#define _PSP_SET_ENABLE_SR(x)   __enable_irq()
#define _PSP_SET_DISABLE_SR(x)  __disable_irq()
#else
#error Invalid platform selected
#endif

#define MRC(coproc, opcode1, Rt, CRn, CRm, opcode2)     __asm{mrc p##coproc, opcode1, Rt, c##CRn, c##CRm, opcode2}
#define MCR(coproc, opcode1, Rt, CRn, CRm, opcode2)     __asm{mcr p##coproc, opcode1, Rt, c##CRn, c##CRm, opcode2}

#define _PSP_SYNC()

#define _WEAK_SYMBOL(x)     __weak x
#define _WEAK_FUNCTION(x)   __weak x

/* Kinetis User mode definitions */
#if MQX_ENABLE_USER_MODE
#define KERNEL_ACCESS  __attribute__((section(".kernel_data")))
#define USER_RW_ACCESS __attribute__((section(".rwuser")))
#define USER_RO_ACCESS __attribute__((section(".rouser")))
#define USER_NO_ACCESS __attribute__((section(".nouser")))

#else /* MQX_ENABLE_USER_MODE */

#define KERNEL_ACCESS
#define USER_RW_ACCESS
#define USER_RO_ACCESS
#define USER_NO_ACCESS

#endif /* MQX_ENABLE_USER_MODE */

/*
 *      DATATYPE MODIFIERS
 */
/* compiler dependent structure packing option - 
   dummy function prototype is added to avoid 
   "illegal empty declaration" compiler warning
   (semicolon on a new line after preprocessing) */
#define PACKED_STRUCT_BEGIN _Pragma("pack(1)")
#define PACKED_STRUCT_END ; _Pragma("pack()") \
                            void dummy(void)

/* compiler dependent union packing option - 
   dummy function prototype is added to avoid 
   "illegal empty declaration" compiler warning
   (semicolon on a new line after preprocessing) */
#define PACKED_UNION_BEGIN _Pragma("pack(1)")
#define PACKED_UNION_END ; _Pragma("pack()") \
                           void dummy(void)

#ifdef __cplusplus
}
#endif

#endif   /* __comp_h__ */
