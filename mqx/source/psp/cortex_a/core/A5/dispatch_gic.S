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
*
*
*END************************************************************************/

#include <asm_mac.h>

#include "mqx_cnfg.h"
#include "types.inc"
#include "psp_prv.inc"

#define __ASM__
#include "psp_cpu.h"
#include "mqx_prv.h"
#undef __ASM__

                ASM_CODE_SECTION(KERNEL)
                SET_FUNCTION_ALIGNMENT

                ASM_EXTERN(_mqx_kernel_data)
                ASM_EXTERN(_sched_execute_scheduler_internal_isr)
                ASM_EXTERN(reset)
                ASM_EXTERN(_psp_push_fp_context)
                ASM_EXTERN(_psp_pop_fp_context)

                ASM_PUBLIC(_psp_exception_return)
                ASM_PUBLIC(_int_kernel_isr_return_internal)
                ASM_PUBLIC(_int_kernel_isr)
                ASM_PUBLIC(_gic_init)

                ASM_PUBLIC_BEGIN(_gic_init)
                ASM_PUBLIC_FUNC(_gic_init)
ASM_LABEL(_gic_init)
                push {r0}
                ldr r0, =_vector_tbl
                mcr p15, 0, r0, c12, c0, 0      /* write VBAR register */

                /* SCTLR bit V - location of exception vectors -> 0x00000000-0x0000001C */
                mrc p15, 0, r0, c1, c0, 0       /* read SCTLR */
                bic r0, r0, #0x2000
                mcr p15, 0, r0, c1, c0, 0       /* write SCTLR */
                pop {r0}

                bx lr
                ASM_PUBLIC_END(_gic_init)


/*
 * This is the entry point for IRQ exception.  On entry, mode is changed to
 * IRQ and interrupts are disabled.  The lr and sp are banked and spsr
 * contains the previous cpsr from svc mode.
 */

/* interrupt prologue */
                ASM_PUBLIC_BEGIN(_int_kernel_isr)
                ASM_PUBLIC_FUNC(_int_kernel_isr)
ASM_LABEL(_int_kernel_isr)
                stmfd sp!, {r0-r12, lr}

                GET_KERNEL_DATA r3              /* get the kernel data address */

                /* increment in interrupt counter */
                ldrh r1, [r3, #KD_IN_ISR]
                add r1, r1, #1
                strh r1, [r3, #KD_IN_ISR]


                /* create interrupt content */
                ldr r0, [r3, #KD_INTERRUPT_CONTEXT_PTR] /* previous interrupt content */
                push {r0}                               /* store in stack */
                push {lr}

                ldr r5, =0x40002100             /* GICC */
                ldr r4, [r5, #4]                /* GICC_PMR 0x40002104 */
                ldr r2, =0                      /* error code (set 0) */
                ldr r1, [r5, #0x0c]             /* GICC_IAR */
                mrs r0, cpsr                    /* cpsr */
                push {r0-r2, r4}

                str sp, [r3, #KD_INTERRUPT_CONTEXT_PTR] /* previous interrupt content */

                /* check for spurious interrupt */

                /* store IAR */
                stmfd sp!, {r1}


#if MQX_KERNEL_LOGGING
                ldr r0, [r3, #KD_LOG_CONTROL]
                tst r0, #0x00000001
                beq _isr_no_logging

                /* prepare parameters for klog function, r0 = vector idx */
                mov r0, r1
                push {r0-r3}
                blx ASM_PREFIX(_klog_isr_start_internal)
                pop {r0-r3}
ASM_LABEL(_isr_no_logging)
#endif // MQX_KERNEL_LOGGING

                // check vector range
		
                // reserved ?
                cmp r1, #1020
                bhs _int_kernel_isr_spurious
                
                // GIC interrupt ID to vector number
                add r1, r1, #16

                // check if isr vector is greater then last user vector
                ldr r0, [r3, #KD_LAST_USER_ISR_VECTOR]
                cmp r1, r0
                bhi _int_kernel_isr_vect_default

                // check if isr vector is lower than first user vector
                ldr r0, [r3, #KD_FIRST_USER_ISR_VECTOR]
                subs r0, r1, r0                         // r0 = actual exception number in table
                blo _int_kernel_isr_vect_default
		
		b _int_kernel_isr_vect_ok

ASM_LABEL(_int_kernel_isr_spurious)
                // spurious ?
                cmp r1, #1024
                blo _int_kernel_isr_return_internal     // yes, spurious interrupt

ASM_LABEL(_int_kernel_isr_vect_default)
                mov r0, r1      // ISR number as function parameter
                // reserved vector - we will call default ISR
                ldr r1, [r3, #KD_DEFAULT_ISR]           // load address of default ISR
                b _isr_execute
ASM_LABEL(_int_kernel_isr_vect_ok)
                /* calculate offset in table */
                /* each table entry is 12 bytes in size */
                mov r1, #12
                mul r0, r0, r1

                ldr r1, [r3, #KD_INTERRUPT_TABLE_PTR]       /* pointer to interrupt table begin */
                add r0, r0, r1                              /* get address of entry in table */
                ldr r1, [r0, #0]
                ldr r0, [r0, #IT_APP_ISR_DATA]              /* move notifier data into r0 = first parameter in C func */

ASM_LABEL(_isr_execute)
                /* r0 = first parameter in C func */
                /* r1 contain interrupt function address */

                stmfd sp!, {r3}
                
#if MQX_SAVE_FP_ALWAYS && PSP_HAS_FPU
                blx _psp_push_fp_context
#endif

                /* Execute the ISR */
                blx r1

#if MQX_SAVE_FP_ALWAYS && PSP_HAS_FPU
                blx _psp_pop_fp_context
#endif

ASM_LABEL(_int_kernel_isr_epilog)
                ldmfd sp!, {r3}

ASM_LABEL(_int_kernel_isr_return_internal)

                // load IAR
                ldmfd sp!, {r0}

#if MQX_KERNEL_LOGGING
                ldr r1, [r3, #KD_LOG_CONTROL]
                tst r1, #0x00000001
                beq _isr_return_no_logging

                /* prepare parameters for klog function, r0 = exception number */
                push {r0-r3}
                blx ASM_PREFIX(_klog_isr_end_internal)
                pop {r0-r3}
ASM_LABEL(_isr_return_no_logging)
#endif /* MQX_KERNEL_LOGGING */

                /* remove interrupt content */
                pop {r0-r2, r4-r6}
                str r6, [r3, #KD_INTERRUPT_CONTEXT_PTR] /* update pointer to interrupt content */

                // R1 = IAR

                /* decrement interrupt counter */
                ldrh r2, [r3, #KD_IN_ISR]
                subs r2, r2, #1
                strh r2, [r3, #KD_IN_ISR]

                // end of interrupt
                ldr r0, =0x40002100     /* GICC base address */
                str r1, [r0, #0x10]     /* GICC_EOIR */

                /* Check the result of IN_ISR.  If it's > 0 at this point, we were
                   more than one level deep.  Don't reschedule in that case. */
                tst r2, #0
                bne _isr_no_preempt

                // Check to see if we need to reschedule.
                ldr r1, [r3, #KD_ACTIVE_PTR]

                // If the task has preemption disabled, don't reschedule no matter what.
                ldr r2, [r1, #TD_FLAGS]
                ands r2, r2, #TASK_PREEMPTION_DISABLED
                bne _isr_no_preempt

                ldr r2, [r3, #KD_CURRENT_READY_Q]
                ldr r2, [r2, #0]
                cmp r1, r2
                bne _isr_sched_start_internal

ASM_LABEL(_isr_no_preempt)
                // return from interrupt
                ldmfd sp!, {r0-r12, lr}
                subs pc, lr, #4

ASM_LABEL(_isr_sched_start_internal)
                /* At this point, we are in IRQ exception mode and we need to
                 * reschedule.  We need to save the sp_irq, lr_irq, and spsr_irq
                 * before changing modes from IRQ to SVC. */
                 
                /* prepare stack in SVC mode for correct task switch */
                /* interrupt stack (IRQ): r0-r12, lr */
                /* task stack (SVC): PMR, CPSR, r0-r12, lr, pc */
                mov r12, sp
                add sp, sp, #56     /* fake unstack - move sp to correct possition (ldmfd sp!, {r0-r12, lr}) */

                mrs r2, spsr
                mov r0, r2
                orr r0, r0, #0xc0	/* Disable interrupts */
                bic r0, r0, #0x20	/* Clean thumb bit (stay in ARM mode) */
                msr cpsr_cxsf, r0
                
                /* svc (supervisor mode) */
                
                /* load PMR to r1 */
                ldr r0, =0x40002100
                ldr r1, [r0, #4]                /* GICC_PMR 0x40002104 */
                
                /* load the lr_irq from interrupt stack and store it in actual stack */
                ldr r0, [r12, #52]      /* get lr_irq from interrupt stack */
                sub r0, r0, #4          /* Subtract 4 from lr_irq because arm loads pc+4 into the lr when entering IRQ */
                str r0, [sp, #-4]!      /* save as new pc for task - after new task switch switch */
                
                str lr, [sp, #-4]       /* save actual lr (lr_svc) to stack */
                
                /* move sp - create task from lowest address */
                sub sp, sp, #64

                stmia sp!, {r1,r2}      /* PMR, PSR */
                
                /* store r0 - r11 */
                ldmfd r12!, {r0-r11}
                stmia sp!, {r0-r11}

                ldmfd r12!, {r0-r1}     /* load registers r12, lr stored in irq stack */
                stmia sp!, {r0}         /* save r12 directly, lr was prepared before */
                
                sub sp, sp, #60         /* correct sp to actual stack position */

                /* Branch to internal scheduler.  This skips the saving of the stack
                 * frame (since we already did our context store) and launches right
                 * into reschedule. */
                b _sched_execute_scheduler_internal_isr
                ASM_PUBLIC_END(_int_kernel_isr)


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _psp_exception_return
* Returned Value   : none
* Comments         : This functions returns us from an isr exception
*
*END*----------------------------------------------------------------------*/
                ASM_PUBLIC_BEGIN(_psp_exception_return)
                ASM_PUBLIC_FUNC(_psp_exception_return)
ASM_LABEL(_psp_exception_return)
                /*
                 * This function is NOT IMPLEMENTED, because we DO NOT NEED it.
                 * Actual version of MQX, does not support nested interrupts.
                 * Situation can't occure when interrupt IRQ (which is handled
                 * by MQX) is interrupted by another interrupt wired to IRQ.
                 */
                bx lr
                ASM_PUBLIC_END(_psp_exception_return)


#if defined(__IAR_SYSTEMS_ICC__)  || defined (__IASMARM__)
/* IAR (6.50 and older) has problem with ASM_CODE_SECTION(.vector_tbl) */
                SECTION .vector_tbl : CODE
#elif defined(__CC_ARM)
/* Realview assembler requires a new area for 32b aligment */
                AREA |.vector_tbl|, CODE, ALIGN=5
#else
                ASM_ALIGN(32)
#endif

/* this section must be aligned to 32B (VBAR) */
ASM_LABEL(_vector_tbl)
                b reset
                b undefined_handler
                b svc_handler
                b prefetch_handler
                b abort_handler
                b .                 /* reserved vector */
                b _int_kernel_isr   /* irq */
                b fiq_handle        /* fiq */

ASM_LABEL(undefined_handler)
ASM_LABEL(svc_handler)
ASM_LABEL(prefetch_handler)
ASM_LABEL(abort_handler)
ASM_LABEL(fiq_handle)
                b .

                ASM_END

