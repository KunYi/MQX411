 

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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*   This file contains the assembler offsets calculated by the
*   program KRNL_OFF.C.  These offsets are to be included in
*   any assembler program that wishes to access kernel data
*   structures.
*
*
*END************************************************************************/

#include <asm_mac.h>

#if 0

/*
* 'PSP_BLOCKED_STACK_STRUCT' size = 64 , 0x40
*/
ASM_EQUATE(PSPB_ST_PC,                          60)     /*  0x3C */

/* 'PSP_BLOCKED_FP_STRUCT' */
ASM_EQUATE(FP_FPCR_OFFSET,                       0)     /* 0x00 */
ASM_EQUATE(FP_FPSR_OFFSET,                       4)     /* 0x04 */
ASM_EQUATE(FP_FPIAR_OFFSET,                      8)     /* 0x08 */
ASM_EQUATE(FP_TID_OFFSET,                        12)    /* 0x0C */
ASM_EQUATE(FP_FPR0_OFFSET,                       16)    /* 0x10 */
ASM_EQUATE(FP_FPR1_OFFSET,                       24)    /* 0x18 */
ASM_EQUATE(FP_FPR2_OFFSET,                       32)    /* 0x20 */
ASM_EQUATE(FP_FPR3_OFFSET,                       40)    /* 0x28 */
ASM_EQUATE(FP_FPR4_OFFSET,                       48)    /* 0x30 */
ASM_EQUATE(FP_FPR5_OFFSET,                       56)    /* 0x38 */
ASM_EQUATE(FP_FPR6_OFFSET,                       64)    /* 0x40 */
ASM_EQUATE(FP_FPR7_OFFSET,                       72)    /* 0x48 */

#endif
 
/*
* 'TD_STRUCT' size = 216 , 0xd8
*/
ASM_EQUATE(TD_TD_NEXT,                              0)
ASM_EQUATE(TD_TD_PREV,                              4)
ASM_EQUATE(TD_STATE,                                8)
ASM_EQUATE(TD_TASK_ID,                             12)
ASM_EQUATE(TD_STACK_BASE,                          16)
ASM_EQUATE(TD_STACK_PTR,                           20)
ASM_EQUATE(TD_STACK_LIMIT,                         24)
ASM_EQUATE(TD_MY_QUEUE,                            28)
ASM_EQUATE(TD_HOME_QUEUE,                          32)
ASM_EQUATE(TD_TASK_SR,                             36)
ASM_EQUATE(TD_RESERVED_1,                          38)
ASM_EQUATE(TD_FLAGS,                               40)
ASM_EQUATE(TD_FLOAT_CONTEXT_PTR,                   52)

/*
* 'READY_Q_STRUCT' size = 16 , 0x10
*/
ASM_EQUATE(RQ_STRUCT_SIZE,                         16)
ASM_EQUATE(RQ_HEAD_READY_Q,                         0)
ASM_EQUATE(RQ_TAIL_READY_Q,                         4)
ASM_EQUATE(RQ_NEXT_Q,                               8)
ASM_EQUATE(RQ_ENABLE_SR,                           12)
ASM_EQUATE(RQ_PRIORITY,                            14)

/*
*'INTERRUPT_TABLE_STRUCT' size = 12 , 0xc
*/
ASM_EQUATE(IT_STRUCT_SIZE,                         12)
ASM_EQUATE(IT_APP_ISR,                              0)
ASM_EQUATE(IT_APP_ISR_EXCEPTION_HANDLER,            4)
ASM_EQUATE(IT_APP_ISR_DATA,                         8)

/*
* 'KERNEL_DATA_STRUCT' size = 1156 , 0x484
*/
ASM_EQUATE(KD_ADDRESSING_CAPABILITY,                0)  /*  0x0 */
ASM_EQUATE(KD_ENDIANESS,                            4)  /*  0x4 */
ASM_EQUATE(KD_CPU_TYPE,                             8)  /*  0x8 */
ASM_EQUATE(KD_PSP_CFG_MEMORY_ALIGNMENT,            10)  /*  0xa */
ASM_EQUATE(KD_PSP_CFG_STACK_ALIGNMENT,             12)  /*  0xc */
ASM_EQUATE(KD_PSP_CFG_MEM_STOREBLOCK_ALIGNMENT,    14)  /*  0xe */
ASM_EQUATE(KD_CONFIG1,                             16)  /*  0x10 */
ASM_EQUATE(KD_CONFIG2,                             18)  /*  0x12 */
ASM_EQUATE(KD_FLAGS,                               20)  /*  0x14 */
ASM_EQUATE(KD_DISABLE_SR,                          22)  /*  0x16 */
ASM_EQUATE(KD_IN_ISR,                              24)  /*  0x18 */
ASM_EQUATE(KD_ACTIVE_SR,                           26)  /*  0x1a */
ASM_EQUATE(KD_ACTIVE_PTR,                          28)  /*  0x1c */
ASM_EQUATE(KD_READY_Q_LIST,                        32)  /*  0x20 */
ASM_EQUATE(KD_CURRENT_READY_Q,                     36)  /*  0x24 */
ASM_EQUATE(KD_DEFAULT_ISR,                         40)  /*  0x28 */
ASM_EQUATE(KD_FIRST_USER_ISR_VECTOR,               44)  /*  0x2c */
ASM_EQUATE(KD_LAST_USER_ISR_VECTOR,                48)  /*  0x30 */
ASM_EQUATE(KD_INTERRUPT_CONTEXT_PTR,               52)  /*  0x34 */
ASM_EQUATE(KD_INTERRUPT_TABLE_PTR,                 56)  /*  0x38 */
ASM_EQUATE(KD_INTERRUPT_STACK_PTR,                 60)  /*  0x3c */
ASM_EQUATE(KD_LOG_CONTROL,                         64)  /*  0x40 */
ASM_EQUATE(LOG_OLD_TD,                             68)  /*  0x44 */
ASM_EQUATE(KD_FP_ACTIVE_PTR,                       72)  /*  0x48 */
ASM_EQUATE(KD_DSP_ACTIVE_PTR,                      76)  /*  0x4c */
ASM_EQUATE(KD_SYSTEM_TD,                           80)  /*  0x50 */
