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
*
*   This file defines the DS5 compiler specific macros for MQX
*
*
*END************************************************************************/
#ifndef __comp_h__
#define __comp_h__ 1

#ifdef __cplusplus
extern "C" {
#endif


static inline uint32_t __get_SP(void) {
	register unsigned int regSP __asm("sp");
	return regSP;
}

static inline  uint32_t __get_PC(void) {
	register unsigned int regPC __asm("pc");
	return regPC;
}

static inline  uint32_t __get_LR(void) {
	register unsigned int regLR __asm("lr");
	return regLR;
}

#define _PSP_GET_CPSR(x)        (x = __get_CPSR())


#define _PSP_SET_ENABLE_SR(x)   { ICCPMR = GIC_PMR_PRIORITY(x); }
#define _PSP_SET_DISABLE_SR(x)  _PSP_SET_ENABLE_SR(x)

#define _ASM_NOP()   __nop()
#define _ASM_WFI()   __wfi()

/* eclipse CDT parser/indexer have problem with parsing __asm{} macros bellow, this is workaround */
#ifdef __CDT_PARSER__
#define MRC(coproc, opcode1, Rt, CRn, CRm, opcode2)
#define MCR(coproc, opcode1, Rt, CRn, CRm, opcode2)
#define DSB()
#define ISB()
#else
#define MRC(coproc, opcode1, Rt, CRn, CRm, opcode2)      __asm{mrc p##coproc, opcode1, Rt, c##CRn, c##CRm, opcode2}
#define MCR(coproc, opcode1, Rt, CRn, CRm, opcode2)      __asm{mcr p##coproc, opcode1, Rt, c##CRn, c##CRm, opcode2}
#define DSB()   __asm{dsb}
#define ISB()   __asm{isb}
#endif

#define _PSP_SYNC()

#define _WEAK_SYMBOL(x)     __weak x
#define _WEAK_FUNCTION(x)   __weak x

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
