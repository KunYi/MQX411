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

                /* Define some external values that will be used. */
                ASM_EXTERN(_mqx_kernel_data)

                /* Set 32-bit instruction width for the scheduler. */
                /* C code can be executed in thumb mode. */
                ASM_CODE_SECTION(KERNEL)
                SET_FUNCTION_ALIGNMENT

                /* Declare global functions. */
                ASM_PUBLIC(_sched_start_internal)
                ASM_PUBLIC(_sched_run_internal)
                ASM_PUBLIC(_sched_check_scheduler_internal)
                ASM_PUBLIC(_sched_execute_scheduler_internal)
                ASM_PUBLIC(_sched_execute_scheduler_internal_isr)
                ASM_PUBLIC(_task_block)
                ASM_PUBLIC(_psp_save_fp_context_internal)


/*
 * _sched_start_internal
 *
 * Run from _mqx(), this starts the MQX scheduler and never returns.
 *
 *
 * _sched_run_internal
 *
 * Run from _task_destroy() and _task_restart_func(), invokes the scheduler.
 *
 */
                ASM_PUBLIC_BEGIN(_sched_start_internal)
                ASM_PUBLIC_FUNC(_sched_start_internal)
ASM_LABEL(_sched_start_internal)
                ASM_PUBLIC_END(_sched_start_internal)

                ASM_PUBLIC_BEGIN(_sched_run_internal)
                ASM_PUBLIC_FUNC(_sched_run_internal)
ASM_LABEL(_sched_run_internal)
                GET_KERNEL_DATA r0
                b ASM_PREFIX(_sched_internal)
                ASM_PUBLIC_END(_sched_run_internal)

/*
 * _sched_check_scheduler_internal
 *
 * This function is called to check whether scheduling is necessary.
 *
 */
                ASM_PUBLIC_BEGIN(_sched_check_scheduler_internal)
                ASM_PUBLIC_FUNC(_sched_check_scheduler_internal)
ASM_LABEL(_sched_check_scheduler_internal)
                /* NOTE: r0 and r1 are being trashed here. */

                /* Load kernel data into r0. */
                GET_KERNEL_DATA r0

                /* Load r1 with 'in ISR' status (KD_IN_ISR offset from r0). */
                ldrh r1, [r0, #KD_IN_ISR]
                /* Branch to _sched_execute_scheduler_internal_end if in ISR. */
                /*   compare and branch if non-zero (r1) to label. */
                cmp r1, #0
                bne ASM_PREFIX(_sched_check_scheduler_internal_end)

                /* Load r1 with current ready q (next ready task). */
                ldr r1, [r0, #KD_CURRENT_READY_Q]
                /* Load r0 with active ptr. */
                ldr r0, [r0, #KD_ACTIVE_PTR]
                /* Load r0 with td my queue? */
                ldr r0, [r0, #TD_MY_QUEUE]
                /* Compare KD_CURRENT_READY_Q with TD_MY_QUEUE */
                cmp r1, r0
                /* If they are equal (current task is the active task) do nothing
                 * and branch back (fall through to next label).
                 * otherwise branch to schedule. */
                bne ASM_PREFIX(_sched_execute_scheduler_internal)

/* Private label: enable IRQ exceptions and jump back to the caller. */
ASM_LABEL(_sched_check_scheduler_internal_end)
                /* ARM_TO_THUMB */

                mov pc, lr
                ASM_PUBLIC_END(_sched_check_scheduler_internal)


/*
 * _sched_execute_scheduler_internal
 *
 *  Called by a task to save it's context.
 *
 */
                ASM_PUBLIC_BEGIN(_sched_execute_scheduler_internal)
                ASM_PUBLIC_FUNC(_sched_execute_scheduler_internal)
ASM_LABEL(_sched_execute_scheduler_internal)
                /* Push registers onto the stack. */
                SAVE_REGISTERS

                /* Save the current cpsr onto the stack.  Note that interrupts are
                 * disabled in this cpsr - they will be enabled before loading. */
                mrs r0, cpsr

                tst lr, #1
                it ne
                orrne r0, r0, #0x20

                str r0, [sp, #-4]!
                cpsid i

                /* save PMR to stack */
                ldr r0, =0x40002100             /* GICC_PMR */
                ldr r0, [r0, #4]                /* 0x40002104 */
                str r0, [sp, #-4]!


ASM_LABEL(_sched_execute_scheduler_internal_isr)
                GET_KERNEL_DATA r0

                /*  Save stack pointer for the active task. */
                ldr r3, [r0, #KD_ACTIVE_PTR]
                str sp, [r3, #TD_STACK_PTR]

#if MQXCFG_ENABLE_FP && PSP_HAS_FPU

#if MQX_SAVE_FP_ALWAYS == 0
                ldr r1, [r3, #TD_FLAGS]

                /* check for fpu flag in TD */
                tst r1, #FP_TASK_MASK
                beq fpu_store_end
#endif
                /* store FPU registers */
                ldr r12, [r3, #TD_FLOAT_CONTEXT_PTR]    /* get task fpu context address */

                vmrs r1, FPSCR
                str r1, [r12], #4                       /* FPSCR */

                fmrx r1, FPEXC
                str r1, [r12], #4                       /* FPEXC */

                vstm r12!, {d0-d15}                      /* restore fpu registers */
                vstm r12!, {d16-d31}                     /* restore fpu registers */
ASM_LABEL(fpu_store_end)
#endif

#if MQX_KERNEL_LOGGING == 1
                ldr r1, [r0, #KD_LOG_CONTROL]
                ands r1, r1, #1
                beq ASM_PREFIX(_no_klog_execute_scheduler)
                stmdb sp!, {r0}
                blx ASM_PREFIX(_klog_execute_scheduler_internal)
                ldmia sp!, {r0}

ASM_LABEL(_no_klog_execute_scheduler)
#endif
/* Fall through after this to internal scheduler. */

/* This function does all major scheduling. */
ASM_LABEL(_sched_internal)
                cpsid i
                ldr r1,[r0,#KD_CURRENT_READY_Q]

ASM_LABEL(find_nonempty_queue)
                ldr r2,[r1,#0]
                cmp r2, r1
                bne activate_task
                ldr r1,[r1,#RQ_NEXT_Q]
                movs r3,r1
                beq ASM_PREFIX(no_one_to_run)
                b ASM_PREFIX(find_nonempty_queue)

/* We get here when there are no ready tasks and the idle task has
 * blocked for some reason.  Not quite sure how that happens, but we
 * should enable interrupts and constant look for something to run. */
ASM_LABEL(no_one_to_run)
                /* Set the active task to system task .*/
                add r2, r0, #KD_SYSTEM_TD
                str r2, [r0, #KD_ACTIVE_PTR]
                ldr sp, [r0, #KD_INTERRUPT_STACK_PTR]

                /* Enable interrupts and enter 'waiting' loop. */
                cpsie i
                mov r3, #1000

ASM_LABEL(_waiting)
                /* Run 1k subs with interrupts enable and then check the ready q to
                 * look for a task to run. */
                subs r3, r3, #1
                bne _waiting

                GET_KERNEL_DATA r0
                cpsid i

                ldr r1, [r0, #KD_READY_Q_LIST]
                b ASM_PREFIX(find_nonempty_queue)

ASM_LABEL(activate_task)
                str r1,[r0,#KD_CURRENT_READY_Q]
                str r2,[r0,#KD_ACTIVE_PTR]
                ldrh r3,[r2,#TD_TASK_SR]
                strh r3,[r0,#KD_ACTIVE_SR]

#if MQXCFG_ENABLE_FP && PSP_HAS_FPU

#if MQX_SAVE_FP_ALWAYS == 0
                ldr r1, [r2, #TD_FLAGS]

                /* check for fpu flag in TD */
                tst r1, #FP_TASK_MASK
                beq fpu_restore_end
#endif

                /* check if last fpu task is different */
                ldr r3, [r0, #KD_FP_ACTIVE_PTR]
                cmp r2, r3
                beq fpu_restore_end

                str r2, [r0, #KD_FP_ACTIVE_PTR]

                /* restore FPU registers */
                ldr r12, [r2, #TD_FLOAT_CONTEXT_PTR]    /* get task fpu context address */

                ldr r3, [r12], #4                       /* FPSCR */
                vmsr FPSCR, r3

                ldr r3, [r12], #4                       /* FPEXC */
                fmxr FPEXC, r3

                vldm r12!, {d0-d15}                     /* restore fpu registers */
                vldm r12!, {d16-d31}                    /* restore fpu registers */
ASM_LABEL(fpu_restore_end)
#endif /* MQXCFG_ENABLE_FP && PSP_HAS_FPU */

                ldr sp,[r2,#TD_STACK_PTR]

#if MQX_KERNEL_LOGGING == 1
                ldr r1, [r0, #KD_LOG_CONTROL]
                ands r1, r1, #1
                beq ASM_PREFIX(_no_klog_activate)
                blx ASM_PREFIX(_klog_context_switch_internal)

ASM_LABEL(_no_klog_activate)
#endif
                /* restore PMR from stack */
                ldr r0, [sp], #4
                ldr r1, =0x40002100             /* GICC_PMR */
                str r0, [r1, #4]                /* 0x40002104 */

                /* Pop the saved cpsr for this task from the stack and store it
                 * into the current spsr. */
                ldr r0, [sp], #4
                msr spsr_cxsf, r0

                /* Execute the new task.  Because we're including pc in the register
                 * list, this will branch to that new instruction.  This also copies
                 * the current mode spsr into the cpsr. *Whew* */
                ldmia sp!,{r0-r12,lr,pc}^

                ASM_PUBLIC_END(_sched_execute_scheduler_internal)

/*
 * _task_block
 *
 * Called by a task to save it's context and remove itself from the ready
 * queue.  The next runnable task is made active and dispatched.  The calling
 * task state is marked as blocked.
 *
 */
                ASM_PUBLIC_BEGIN(_task_block)
                ASM_PUBLIC_FUNC(_task_block)
ASM_LABEL(_task_block)
                
                push {r0-r3}

                GET_KERNEL_DATA r0

                cpsid i

                ldr r1, [r0, #KD_ACTIVE_PTR]

                ldr r2, [r1, #TD_STATE]
                orr r2, r2, #1
                str r2, [r1, #TD_STATE]

#if MQX_KERNEL_LOGGING == 1
                ldr r2, [r0, #KD_LOG_CONTROL]
                ands r2, r2, #1
                beq ASM_PREFIX(_no_klog_block)
                stmdb sp!, {r1, lr}
                blx ASM_PREFIX(_klog_block_internal)
                ldmia sp!, {r1, lr}

ASM_LABEL(_no_klog_block)
#endif

                /* Remove the active task from the ready q. */
                ldr r2, [r1, #TD_TD_PREV]
                ldr r3, [r1, #TD_TD_NEXT]
                str r3, [r2, #RQ_HEAD_READY_Q]
                str r2, [r3, #TD_TD_PREV]

                cpsie i

                pop {r0-r3}
                
                /* Branch to scheduler to reschedule. */
                b _sched_execute_scheduler_internal
                ASM_PUBLIC_END(_task_block)

/*
 * _psp_save_fp_context_internal
 *
 * Save floating point context for the current floating point task.
 *
 */
ASM_LABEL(_psp_save_fp_context_internal)
#if MQXCFG_ENABLE_FP && PSP_HAS_FPU
/*       Stop the floating point unit, and save the internal */
/*       floating point registers on the stack of the FP task */
                GET_KERNEL_DATA r0
                ldr r3, [r0, #KD_ACTIVE_PTR]        /* get active task descriptor */
                ldr r1, [r3, #TD_FLAGS]

                // store FPU registers
                ldr r12, [r3, #TD_FLOAT_CONTEXT_PTR]    /* get task fpu context address */

                vmrs r1, FPSCR
                str r1, [r12], #4                       /* FPSCR */

                fmrx r1, FPEXC
                str r1, [r12], #4                       /* FPEXC */

                vstm r12!, {d0-d15}                      /* store fpu registers */
                vstm r12!, {d16-d31}                     /* store fpu registers */
ASM_LABEL(_psp_save_fp_context_done)
#endif /* MQXCFG_ENABLE_FP && PSP_HAS_FPU */
                bx lr

                ASM_END
