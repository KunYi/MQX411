/*HEADER********************************************************************
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
*   This assembler header file contains private declarations for 
*   use with the mqx assembler files
*
*
*END************************************************************************/

#include <asm_mac.h>
/*------------------------------------------------------------------------
*                         EXTERNAL REFERENCES
*/

#if MQX_KERNEL_LOGGING
        ASM_EXTERN(_klog_block_internal)
        ASM_EXTERN(_klog_execute_scheduler_internal)
        ASM_EXTERN(_klog_yield_internal)
        ASM_EXTERN(_klog_context_switch_internal)
        ASM_EXTERN(_klog_isr_start_internal)
        ASM_EXTERN(_klog_isr_end_internal)
#endif
        ASM_EXTERN(_mqx_kernel_data)
        ASM_EXTERN(_mqx_system_stack)

        ASM_EXTERN(_psp_irq_sp)
        ASM_EXTERN(_psp_fiq_sp)
        ASM_EXTERN(_psp_undef_sp)
        ASM_EXTERN(_psp_abort_sp)

/*------------------------------------------------------------------------
*                       CONSTANT DECLARATIONS
*
* The following are the bits in the CONFIG field of the kernel data structure
* to set for the psp options.
* NOTE: These must agree with values in mqx_prv.h
*/

ASM_EQUATE(PSP_CNFG_MQX_KERNEL_LOGGING, 0x8000)
ASM_EQUATE(PSP_CNFG_FP_REGISTERS_EXIST, 0x4000)

#ifndef MQX_FP_REGISTERS_EXIST
#define MQX_FP_REGISTERS_EXIST 0
#endif

#if MQX_KERNEL_LOGGING == 1
#if MQX_FP_REGISTERS_EXIST == 1
ASM_EQUATE(PSP_CNFG, (PSP_CNFG_MQX_KERNEL_LOGGING | PSP_CNFG_FP_REGISTERS_EXIST))
#else
ASM_EQUATE(PSP_CNFG, PSP_CNFG_MQX_KERNEL_LOGGING)
#endif
#else
#if MQX_FP_REGISTERS_EXIST == 1
ASM_EQUATE(PSP_CNFG, PSP_CNFG_FP_REGISTERS_EXIST)
#else
ASM_EQUATE(PSP_CNFG, 0)
#endif
#endif


/*
*  Task FLAGS bits
*  These must match definitions in mqx_prv.h
*/

ASM_EQUATE(FP_TASK_MASK, 0x0002)
ASM_EQUATE(FP_CONTEXT_SAVED_MASK, 0x0200)
ASM_EQUATE(FP_CONTEXT_CLEAR_MASK, 0xfdff)
ASM_EQUATE(PREEMPTION_DISABLED, 0x1000)

/*------------------------------------------------------------------------
*                          MACRO DECLARATIONS
*/
#ifdef __IASMARM__ /* IAR */

/* This macro returns the address of the kernel data in the specified register */
GET_KERNEL_DATA MACRO reg
        ldr reg, =ASM_PREFIX(_mqx_kernel_data)
        ldr reg, [reg, #0]
        ENDM
 
/* Align assembler functions to 4 bytes (2^2) */
SET_FUNCTION_ALIGNMENT MACRO
                ALIGNROM 2
        ENDM

/* This macro calls the kernel logging function, if logging enabled */
KLOG    MACRO KDATA, KLOG_FUNCTION
#if MQX_KERNEL_LOGGING
        LOCAL klog_end
        push {r1}
        ldr r1, [KDATA, #KD_LOG_CONTROL]
        tst r1, #1
        beq klog_end
                
        push {lr}
        bl KLOG_FUNCTION
        pop {lr}
klog_end:
        pop {r1}
#endif
        ENDM

/* cache_line_size - get the cache line size from the CSIDR register
 * (available on ARMv7+). It assumes that the CSSR register was configured
 * to access the L1 data cache CSIDR. 
 * Note: Code from linux*/
 

cache_line_size MACRO reg, tmp
        mrc p15, 1, tmp, c0, c0, 0      /* read CSIDR */
        and tmp, tmp, #7                /* cache line size encoding */
        mov reg, #16                    /* size offset */
        mov reg, reg, lsl tmp           /* actual cache line size */
        ENDM

/* Push registers onto the stack to save them for a context switch. */
SAVE_REGISTERS MACRO
	str lr, [sp, #-4]!
	stmdb sp!, {r0-r12,lr}
	nop
        ENDM

#elif defined __CC_ARM /* Keil */

/* This macro returns the address of the kernel data in the specified register */
        MACRO
        GET_KERNEL_DATA $reg
        ldr $reg, =ASM_PREFIX(_mqx_kernel_data)
        ldr $reg, [$reg, #0]
        MEND
 
/* Align assembler functions to 4 bytes (2^2) */
        MACRO
        SET_FUNCTION_ALIGNMENT
        ALIGN 2
        MEND

        MACRO
        KLOG $KDATA, $KLOG_FUNCTION
#if MQX_KERNEL_LOGGING
        push {r1}
        ldr r1, [$KDATA, #KD_LOG_CONTROL]
        tst r1, #1
        beq %FT1
	
        push {lr}
        bl $KLOG_FUNCTION
        pop {lr}
1
        pop {r1}
#endif
    	MEND

/* Push registers onto the stack to save them for a context switch. */
		MACRO
		SAVE_REGISTERS
		str lr, [sp, #-4]!
		stmdb sp!, {r0-r12,lr}
		nop
        MEND

#elif defined(__GNUC__)

/* This macro returns the address of the kernel data in the specified register */
GET_KERNEL_DATA: .macro reg
        ldr \reg, =ASM_PREFIX(_mqx_kernel_data)
        ldr \reg, [\reg, #0]
        .endm
 
/* Align assembler functions to 4 bytes */
SET_FUNCTION_ALIGNMENT: .macro
        .balign 4
        .endm

/* This macro calls the kernel logging function, if logging enabled */
KLOG:   .macro KDATA, KLOG_FUNCTION
        .if MQX_KERNEL_LOGGING
        push {r1}
        ldr r1, [\KDATA, #KD_LOG_CONTROL]
        tst r1, #1
        beq klog_end\@

        push {lr}
        bl \KLOG_FUNCTION
        pop {lr}
klog_end\@:
        pop {r1}
        .endif
        .endm

SAVE_REGISTERS: .macro
        str lr, [sp, #-4]!
        stmdb sp!, {r0-r12,lr}
        nop
        .endm
        
#else

#error "Unsupported compiler"

#endif
