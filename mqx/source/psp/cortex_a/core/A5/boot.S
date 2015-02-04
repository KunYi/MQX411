
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

                ASM_PUBLIC(__boot)
                ASM_PUBLIC(reset)

                ASM_EXTERN(init_hardware)
                ASM_EXTERN(__BOOT_STACK_ADDRESS)
                ASM_EXTERN(__cmain)
                ASM_EXTERN(_int_kernel_isr)

                // Forward declaration of sections.
                ASM_DATA_SECTION(FIQ_STACK)
                ASM_DATA_SECTION(ABT_STACK)
                ASM_DATA_SECTION(UND_STACK)
                ASM_DATA_SECTION(SYS_STACK)

#if defined(__IAR_SYSTEMS_ICC__)  || defined (__IASMARM__)
                #define FIQ_STACK_ADDR      SFE(FIQ_STACK)
                #define ABT_STACK_ADDR      SFE(ABT_STACK)
                #define UND_STACK_ADDR      SFE(UND_STACK)
                #define SYS_STACK_ADDR      SFE(SYS_STACK)
#elif defined(__CC_ARM)
                IMPORT ||Image$$FIQ_STACKS$$ZI$$Limit||
                IMPORT ||Image$$ABT_STACKS$$ZI$$Limit||
                IMPORT ||Image$$UND_STACKS$$ZI$$Limit||
                IMPORT ||Image$$SYS_STACKS$$ZI$$Limit||

                #define FIQ_STACK_ADDR      ||Image$$FIQ_STACKS$$ZI$$Limit||
                #define ABT_STACK_ADDR      ||Image$$ABT_STACKS$$ZI$$Limit||
                #define UND_STACK_ADDR      ||Image$$UND_STACKS$$ZI$$Limit||
                #define SYS_STACK_ADDR      ||Image$$SYS_STACKS$$ZI$$Limit||
#elif defined(__GNUC__)
                #define FIQ_STACK_ADDR      _FIQ_STACK_END
                #define ABT_STACK_ADDR      _ABT_STACK_END
                #define UND_STACK_ADDR      _UND_STACK_END
                #define SYS_STACK_ADDR      _SYS_STACK_END
#else
#error unsuported compiler
#endif
                ASM_CODE_SECTION(STARTUP)
                SET_FUNCTION_ALIGNMENT
                //ARM

ASM_LABEL(__boot)
                b reset
                b undefined_handler
                b svc_handler
                b prefetch_handler
                b abort_handler
                b .                     /* reserved vector */
                b _int_kernel_isr       /* irq */
                b fiq_handle            /* fiq */

ASM_LABEL(undefined_handler)
ASM_LABEL(svc_handler)
ASM_LABEL(prefetch_handler)
ASM_LABEL(abort_handler)
ASM_LABEL(fiq_handle)
                b .

ASM_LABEL(reset)
                /* disable caches, MMU - this is not required from a cold reset */
                mrc p15, 0, r0, c1, c0, 0       /* Read CP15 System Control register */
                bic r0, r0, #(0x1 << 12)        /* Clear I bit 12 to disable I Cache */
                bic r0, r0, #(0x1 <<  2)        /* Clear C bit  2 to disable D Cache */
                bic r0, r0, #0x2                /* Clear A bit  1 to disable strict alignment */
                bic r0, r0, #0x1                /* Clear M bit  0 to disable MMU */
                bic r0, r0, #(0x1 << 11)        /* Clear Z bit 11 to disable branch prediction */
                mcr p15, 0, r0, c1, c0, 0       /* Write value back to CP15 System Control register */

                /* enable NEON MPE in secure state and nonsecure state */
                mrc p15, 0, r0, c1, c1, 2
                orr r0, r0, #3<<10           /* enable neon */
                bic r0, r0, #3<<14           /* clear nsasedis/nsd32dis */
                mcr p15, 0, r0, c1, c1, 2

                ldr r0, =(0xf << 20)
                mcr p15, 0, r0, c1, c0, 2

                mov r3, #0x40000000
                /*vmsr FPEXC, r3*/
                fmxr FPEXC, r3

                /* Go into supervisor mode.  Currently MQX will always operate out of this mode for kernel and user-land code.*/
                mrs r0, cpsr
                bic r0, r0, #0x1f
                orr r0, r0, #0xd3
                msr cpsr_c,r0

                /* We need to setup the user mode stuff.  Don't do this now because
                 * once we switch to user mode, we can't switch back out this way. */
                /*
                msr cpsr_c, #PSP_PSR_MODE_USER | PSP_PSR_CTRL_DIS_FIQ
                sub r1, r0, #PSP_STACK_USER_OFFSET
                mov sp, r1
                */

                /* set fast interrupt (FIQ) mode stack */
                msr cpsr_c, #PSP_PSR_MODE_FIQ | PSP_PSR_CTRL_DIS_FIQ | PSP_PSR_CTRL_DIS_IRQ
                ldr sp, =FIQ_STACK_ADDR

                /* set interrupt (IRQ) mode stack */
                msr cpsr_c, #PSP_PSR_MODE_IRQ | PSP_PSR_CTRL_DIS_FIQ | PSP_PSR_CTRL_DIS_IRQ
                ldr r0, =__BOOT_STACK_ADDRESS
                mov sp, r1

                /* set abort mode stack */
                msr cpsr_c, #PSP_PSR_MODE_ABORT | PSP_PSR_CTRL_DIS_FIQ | PSP_PSR_CTRL_DIS_IRQ
                ldr sp, =ABT_STACK_ADDR

                /* set undefined mode stack */
                msr cpsr_c, #PSP_PSR_MODE_UNDEF | PSP_PSR_CTRL_DIS_FIQ | PSP_PSR_CTRL_DIS_IRQ
                ldr sp, =UND_STACK_ADDR

                /* set system/user mode stack */
                msr cpsr_c, #PSP_PSR_MODE_SYS | PSP_PSR_CTRL_DIS_FIQ | PSP_PSR_CTRL_DIS_IRQ
                ldr sp, =SYS_STACK_ADDR

                /* set supervisor mode stack */
                msr cpsr_c, #PSP_PSR_MODE_SVC | PSP_PSR_CTRL_DIS_FIQ | PSP_PSR_CTRL_DIS_IRQ
                sub r1, r0, #0x40       /* __BOOT_STACK_ADDRESS - 0x40 */
                mov sp, r1

                /* perform toolchain startup routines */
                ASM_EXTERN(toolchain_startup)
                ldr r0, =ASM_PREFIX(toolchain_startup)
                bx r0


                ASM_END
