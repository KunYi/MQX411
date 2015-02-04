/*HEADER***********************************************************************
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

                ASM_EXTERN(_mqx_kernel_data)
                ASM_PUBLIC(_mem_test_and_set)
                ASM_PUBLIC(__set_SP)
                ASM_PUBLIC(_psp_push_fp_context)
                ASM_PUBLIC(_psp_pop_fp_context)
                ASM_PUBLIC(__set_mode_SP)
                ASM_PUBLIC(__get_CPSR)
                
                ASM_CODE_SECTION(.text)
        SET_FUNCTION_ALIGNMENT

/*FUNCTION*******************************************************************
*
* Function Name    : _mem_test_and_set
* Returned Value   : returns the previous value stored in that address
* Comments         : Used for mutex locking purposes.
*                    Writes 0x80 to the address pointed to in r0
*
*END************************************************************************/
                ASM_PUBLIC_BEGIN(_mem_test_and_set)
                ASM_PUBLIC_FUNC(_mem_test_and_set)
ASM_LABEL(_mem_test_and_set)
                mov r2, #0x80

ASM_LABEL(_mem_test_and_set_loop)
                ldrexb r1, [r0]
                cmp r1, #0                          /* it is free? */
                ite eq
                strexbeq r3, r2, [r0]               /* yes, try lock */
                bne _mem_test_and_set_end
                cmp r3, #0
                bne _mem_test_and_set_loop          /* store success? */

ASM_LABEL(_mem_test_and_set_end)
                mov r0, r1
                bx  lr

                ASM_PUBLIC_END(_mem_test_and_set)

/*FUNCTION*******************************************************************
*
* Function Name    : __set_SP
* Returned Value   : none
* Comments         : This function sets SP (stack pointer) for active mode
*
*END************************************************************************/
        ASM_PUBLIC_BEGIN(__set_SP)
        ASM_PUBLIC_FUNC(__set_SP)
ASM_LABEL(__set_SP)
                mov sp, r0
                bx  lr
                ASM_PUBLIC_END(__set_SP)


                ASM_PUBLIC(neon_copy_offset_0)
                ASM_PUBLIC(neon_copy_offset_1)
                ASM_PUBLIC(neon_copy_offset_2)
                ASM_PUBLIC(neon_copy_offset_3)
                ASM_PUBLIC(aligned_neon_copy)

                /*neon copy for bias 0 case*/
                ASM_PUBLIC_BEGIN(neon_copy_offset_0)
                ASM_PUBLIC_FUNC(neon_copy_offset_0)
ASM_LABEL(neon_copy_offset_0)
                pld [r1, #0]

                /* copy 32 bytes in one vldm and vstm loop */
ASM_LABEL(neon_copy_offset_0_3)
                cmp r2, #32
                blt neon_copy_offset_0_2
                pld [r1, #96]
                vldm r1!, {d0-d3}
                vstm r0!, {d0-d3}
                subs r2, r2, #32
                bgt neon_copy_offset_0_3

                /* copy 16 bytes in one vldm and vstm loop */
ASM_LABEL(neon_copy_offset_0_2)
                cmp r2, #16
                blt neon_copy_offset_0_1
                pld [r1, #64]
                vldm r1!, {d0-d1}
                vstm r0!, {d0-d1}
                subs r2, r2, #16
                bgt neon_copy_offset_0_2

                /* return the actual copy size */
ASM_LABEL(neon_copy_offset_0_1)
                mov r0, r2
                bx  lr
                ASM_PUBLIC_END(neon_copy_offset_0)


                ASM_PUBLIC_BEGIN(neon_copy_offset_1)
                ASM_PUBLIC_FUNC(neon_copy_offset_1)
                /* neon copy for bias 1 case */
ASM_LABEL(neon_copy_offset_1)
                push  {r4-r6}
                vstmdb sp!, {d4-d5}

ASM_LABEL(neon_copy_offset_1_2)
                cmp r2, #23
                blt neon_copy_offset_1_1

                /* load 24 bytes to d0-d2 registers*/
                pld [r1, #64]
                vldm r1, {d0-d2}

                /* extract bytes from d0 register, d0 offset = 1*/
                vmov.u8 r4, d0[1]
                vmov.u16 r5, d0[1]
                vmov.u8 r6, d0[4]

                /* reorgnize the 4 bytes and move to bottom word of d4 registers*/
                orr r4, r4, r5, lsl #8
                orr r4, r4, r6, lsl #24
                vmov.32 d4[0], r4

                /* extract bytes from d0 and d1 register*/
                vmov.u8 r4, d0[5]
                vmov.u16 r5, d0[3]
                vmov.u8 r6, d1[0]

                /* reorgnize the 4 bytes and move to upper word of d4 registers*/
                orr r4, r4, r5, lsl #8
                orr r4, r4, r6, lsl #24
                vmov.32 d4[1], r4

                /* extract bytes from d1 register*/
                vmov.u8 r4, d1[1]
                vmov.u16 r5, d1[1]
                vmov.u8 r6, d1[4]

                /* reorgnize the 4 bytes and move to bottom word of d5 registers*/
                orr r4, r4, r5, lsl #8
                orr r4, r4, r6, lsl #24
                vmov.32 d5[0], r4

                /* extract bytes from d1 and d2 register*/
                vmov.u8 r4, d1[5]
                vmov.u16 r5, d1[3]
                vmov.u8 r6, d2[0]

                /* reorgnize the 4 bytes and move to upper word of d5 registers*/
                orr r4, r4, r5, lsl #8
                orr r4, r4, r6, lsl #24
                vmov.32 d5[1], r4

                /* write d4 and d5 content to memory*/
                vstm r0!, {d4-d5}
                add r1, r1, #16
                subs r2, r2, #16
                bgt neon_copy_offset_1_2

ASM_LABEL(neon_copy_offset_1_1)
                vldmia sp!, {d4-d5}
                pop {r4-r6}
                mov r0, r2
                bx lr
                ASM_PUBLIC_END(neon_copy_offset_1)

                /* neon copy for bias 2 case */
                ASM_PUBLIC_BEGIN(neon_copy_offset_2)
                ASM_PUBLIC_FUNC(neon_copy_offset_2)
ASM_LABEL(neon_copy_offset_2)
                push {r4-r5}
                vstmdb sp!, {d4-d5}

ASM_LABEL(neon_copy_offset_2_2)
                cmp r2, #22
                blt neon_copy_offset_2_1

                /* load 24 bytes to d0-d2 registers */
                pld [r1, #64]
                vldm r1, {d0-d2}

                /* extract bytes from d0 register, d0 offset = 2 */
                vmov.u16 r4, d0[1]
                vmov.u16 r5, d0[2]

                /* reorgnize the 4 bytes and move to bottom word of d4 */
                orr r4, r4, r5, lsl #16
                vmov.32 d4[0], r4

                /* extract bytes from d0 and d1 register */
                vmov.u16 r4, d0[3]
                vmov.u16 r5, d1[0]

                /* reorgnize the 4 bytes and move to upper word of d4 */
                orr r4, r4, r5, lsl #16
                vmov.32 d4[1], r4

                /* extract bytes from d1 register */
                vmov.u16 r4, d1[1]
                vmov.u16 r5, d1[2]

                /* reorgnize the 4 bytes and move to bottom word of d5 */
                orr r4, r4, r5, lsl #16
                vmov.32 d5[0], r4

                /* extract bytes from d1 and d2 register */
                vmov.u16 r4, d1[3]
                vmov.u16 r5, d2[0]

                /* reorgnize the 4 bytes and move to upper word of d5 */
                orr r4, r4, r5, lsl #16
                vmov.32 d5[1], r4

                /* write d4 and d5 content to memory */
                vstm r0!, {d4-d5}
                add r1, r1, #16
                subs r2, r2, #16
                bgt neon_copy_offset_2_2

ASM_LABEL(neon_copy_offset_2_1)
                vldmia sp!, {d4-d5}
                pop {r4-r5}
                mov r0, r2
                bx  lr
                ASM_PUBLIC_END(neon_copy_offset_2)


                /* neon copy for bias 3 case */
                ASM_PUBLIC_BEGIN(neon_copy_offset_3)
                ASM_PUBLIC_FUNC(neon_copy_offset_3)
ASM_LABEL(neon_copy_offset_3)
                push {r4-r6}
                vstmdb sp!, {d4-d5}

ASM_LABEL(neon_copy_offset_3_2)
                cmp r2, #21
                blt neon_copy_offset_3_1

                /* load 24 bytes to d0-d2 registers */
                pld [r1, #64]
                vldm r1, {d0-d2}

                /* extract bytes from d0 register, d0 offset = 3 */
                vmov.u8 r4, d0[3]
                vmov.u16 r5, d0[2]
                vmov.u8 r6, d0[6]

                /* reorgnize the 4 bytes and move to bottom word of d4 */
                orr r4, r4, r5, lsl #8
                orr r4, r4, r6, lsl #24
                vmov.32 d4[0], r4

                /* extract bytes from d0 and d1 register */
                vmov.u8 r4, d0[7]
                vmov.u16 r5, d1[0]
                vmov.u8 r6, d1[2]

                /* reorgnize the 4 bytes and move to upper word of d4 */
                orr r4, r4, r5, lsl #8
                orr r4, r4, r6, lsl #24
                vmov.32 d4[1], r4

                /* extract bytes from d1 register */
                vmov.u8 r4, d1[3]
                vmov.u16 r5, d1[2]
                vmov.u8 r6, d1[6]

                /* reorgnize the 4 bytes and move to bottom word of d5 */
                orr r4, r4, r5, lsl #8
                orr r4, r4, r6, lsl #24
                vmov.32 d5[0], r4

                /* extract bytes from d1 and d2 register */
                vmov.u8 r4, d1[7]
                vmov.u16 r5, d2[0]
                vmov.u8 r6, d2[2]

                /* reorgnize the 4 bytes and move to upper word of d5 */
                orr r4, r4, r5, lsl #8
                orr r4, r4, r6, lsl #24
                vmov.32 d5[1], r4

                /* write d4 and d5 content to memory */
                vstm r0!, {d4-d5}
                add r1, r1, #16
                subs r2, r2, #16
                bgt neon_copy_offset_3_2

ASM_LABEL(neon_copy_offset_3_1)
                vldmia sp!, {d4-d5}
                pop {r4-r6}
                mov r0, r2
                bx lr
                ASM_PUBLIC_END(neon_copy_offset_3)

                /*another neon copying implementation which will*/
                /*handle all offset cases, but no much performance improved*/
                ASM_PUBLIC_BEGIN(aligned_neon_copy)
                ASM_PUBLIC_FUNC(aligned_neon_copy)
ASM_LABEL(aligned_neon_copy)
                tst r0, #1
                beq aligned_neon_copy_1
                cmp r2, #1
                blt aligned_neon_copy_1
                vld1.8 {d0[0]}, [r1]!
                vst1.8 {d0[0]}, [r0]!
                sub r2, r2, #1
ASM_LABEL(aligned_neon_copy_1)
                tst r0, #2
                beq aligned_neon_copy_2
                cmp r2, #2
                blt aligned_neon_copy_2
#if defined(__GNUC__)
                vld2.8 {d0[0], d1[0]}, [r1, :16]!
                vst2.8 {d0[0], d1[0]}, [r0, :16]!
#else
                vld2.8 {d0[0], d1[0]}, [r1@16]!
                vst2.8 {d0[0], d1[0]}, [r0@16]!
#endif
                sub r2, r2, #2
ASM_LABEL(aligned_neon_copy_2)
                tst r0, #4
                beq aligned_neon_copy_3
                cmp r2, #4
                blt aligned_neon_copy_3
#if defined(__GNUC__)
                vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r1, :32]!
                vst4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0, :32]!
#else
                vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r1@32]!
                vst4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0@32]!
#endif
                sub r2, r2, #4
ASM_LABEL(aligned_neon_copy_3)
                tst r0, #8
                beq aligned_neon_copy_4
                cmp r2, #8
                blt aligned_neon_copy_4
                vldm r1!, {d0}
                vstm r0!, {d0}
                sub r2, r2, #8

                pld [r1, #0]
ASM_LABEL(aligned_neon_copy_4)
                cmp r2, #32
                blt aligned_neon_copy_5
                pld [r1, #32]
                pld [r1, #64]
                vldm r1!, {d0-d3}
                vstm r0!, {d0-d3}
                subs r2, r2, #32
                bgt aligned_neon_copy_4
                b   aligned_neon_copy_5

ASM_LABEL(aligned_neon_copy_5)
                tst r2, #16
                beq aligned_neon_copy_6
                vldm r1!, {d0-d1}
                vstm r0!, {d0-d1}
                sub r2, r2, #16
ASM_LABEL(aligned_neon_copy_6)
                tst r2, #8
                beq aligned_neon_copy_7
                vldm r1!, {d0}
                vstm r0!, {d0}
                sub r2, r2, #8
ASM_LABEL(aligned_neon_copy_7)
                tst r2, #4
                beq aligned_neon_copy_8
#if defined(__GNUC__)
                vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r1, :32]!
                vst4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0, :32]!
#else
                vld4.8 {d0[0], d1[0], d2[0], d3[0]}, [r1@32]!
                vst4.8 {d0[0], d1[0], d2[0], d3[0]}, [r0@32]!
#endif
                sub r2, r2, #4
ASM_LABEL(aligned_neon_copy_8)
                tst r2, #2
                beq aligned_neon_copy_9

#if defined(__GNUC__)
                vld2.8 {d0[0], d1[0]}, [r1, :16]!
                vst2.8 {d0[0], d1[0]}, [r0, :16]!
#else
                vld2.8 {d0[0], d1[0]}, [r1@16]!
                vst2.8 {d0[0], d1[0]}, [r0@16]!
#endif
                sub r2, r2, #2
ASM_LABEL(aligned_neon_copy_9)
                tst r2, #1
                beq aligned_neon_copy_10
                vld1.8 {d0[0]}, [r1]!
                vst1.8 {d0[0]}, [r0]!
                sub r2, r2, #1
ASM_LABEL(aligned_neon_copy_10)
                bx  lr
                ASM_PUBLIC_END(aligned_neon_copy)



/*FUNCTION*******************************************************************
*
* Function Name    : _psp_push_fp
* Returned Value   : none
* Comments         : Save FPU and NEON registers to stack
*
*END************************************************************************/
                ASM_PUBLIC_BEGIN(_psp_push_fp_context)
                ASM_PUBLIC_FUNC(_psp_push_fp_context)
ASM_LABEL(_psp_push_fp_context)
                str r1, [sp, #-268]                     /* save r1 to possition in stack (1word reg (r1) + 2word regs (FPSCR, FPEXC) + 32double word registers) */

                /* save FPU, FPU status and NEON registers */
                vmrs r1, FPSCR
                str r1, [sp, #-4]!                      /* FPSCR */

                fmrx r1, FPEXC
                str r1, [sp, #-4]!                      /* FPEXC */

                vstmdb sp!, {d0-d15}                    /* store fpu registers */
                vstmdb sp!, {d16-d31}                   /* store fpu registers */

                ldr r1, [sp, #-4]                       /* restore changed r1 */

                bx lr
                ASM_PUBLIC_END(_psp_push_fp_context)

/*FUNCTION*******************************************************************
*
* Function Name    : _psp_pop_fp
* Returned Value   : none
* Comments         : Restore FPU and NEON registers from stack
*
*END************************************************************************/
                ASM_PUBLIC_BEGIN(_psp_pop_fp_context)
                ASM_PUBLIC_FUNC(_psp_pop_fp_context)
ASM_LABEL(_psp_pop_fp_context)
                str r1, [sp, #-4]                       /* save r1 */

                /* restore FPU, FPU status and NEON registers */
                vldm sp!, {d16-d31}                     /* restore fpu registers */
                vldm sp!, {d0-d15}                      /* restore fpu registers */

                ldr r1, [sp], #4                        /* FPEXC */
                fmxr FPEXC, r1

                ldr r1, [sp], #4                        /* FPSCR */
                vmsr FPSCR, r1

                ldr r1, [sp, #-268]                     /* restore r1 */        

                bx lr
                ASM_PUBLIC_END(_psp_pop_fp_context)

/*FUNCTION*******************************************************************
*
* Function Name    : __set_mode_SP
* Returned Value   : none
* Comments         : set stack address (R0) of processor mode (R1)
*
*END************************************************************************/
                ASM_PUBLIC_BEGIN(__set_mode_SP)
                ASM_PUBLIC_FUNC(__set_mode_SP)
ASM_LABEL(__set_mode_SP)
                push {r2}                               /* save r2 */
                mrs r2, cpsr                            /* store old cpsr */
                msr cpsr_c, r1                          /* set new cpsr */
                mov sp, r0                              /* set stack */
                msr cpsr_c, r2                          /* restore old cpsr */
                pop {r2}                                /* restore r2 */
                bx lr
                ASM_PUBLIC_END(__set_mode_SP)

/*FUNCTION*******************************************************************
*
* Function Name    : __get_CPSR
* Returned Value   : cpsr value 
* Comments         : return cpsr value of current processor mode
*
*END************************************************************************/
                ASM_PUBLIC_BEGIN(__get_CPSR)
                ASM_PUBLIC_FUNC(__get_CPSR)
ASM_LABEL(__get_CPSR)
                mrs r0, cpsr
                bx lr
                ASM_PUBLIC_END(__get_CPSR)

                ASM_END

