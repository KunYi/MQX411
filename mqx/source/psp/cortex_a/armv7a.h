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
*END************************************************************************/

#ifndef __armv7a_h__
#define __armv7a_h__

/*==========================================================================*/
/*
**                   MQX REQUIRED DEFINITIONS
**
** Other MQX kernel and component functions require these definitions to exist.
*/

/* Indicate which endian this PSP is */
#if defined(__ARM__) || defined(__GHS__)
   #ifdef __BIG_ENDIAN
      #define PSP_ENDIAN              MQX_BIG_ENDIAN
   #else
      #define PSP_ENDIAN              MQX_LITTLE_ENDIAN
   #endif
#elif defined(__MET__)
   #ifdef _BE
      #define PSP_ENDIAN              MQX_BIG_ENDIAN
   #else
      #define PSP_ENDIAN              MQX_LITTLE_ENDIAN
   #endif
#elif defined(__GNUC__)
   #ifdef BYTES_BIG_ENDIAN
      #define PSP_ENDIAN              MQX_BIG_ENDIAN
   #else
      #define PSP_ENDIAN              MQX_LITTLE_ENDIAN
   #endif
#else
   #define PSP_ENDIAN                 MQX_LITTLE_ENDIAN
#endif

/*
** Memory alignment requirements.
** The alignment indicates how memory is to be aligned for all memory
** accesses.   This is to avoid mis-aligned transfers of data, thus
** optimizing memory accesses.
*/
#define PSP_MEMORY_ALIGNMENT       (PSP_CACHE_LINE_SIZE-1)
#define PSP_MEMORY_ALIGNMENT_MASK  (~PSP_MEMORY_ALIGNMENT)

/*
** Stack alignment requirements.
** The alignment indicates how the stack is to be initially aligned.
** This is to avoid mis-aligned types on the stack
*/
#define PSP_STACK_ALIGNMENT       PSP_MEMORY_ALIGNMENT
#define PSP_STACK_ALIGNMENT_MASK  PSP_MEMORY_ALIGNMENT_MASK

/*
** Indicate the direction of the stack
*/
#define PSP_STACK_GROWS_TO_LOWER_MEM               (1)

/*
** Indicate addressing capability of the CPU
** This is the memory width. i.e., the number of bits addressed
** by each memory location.
*/
#define PSP_MEMORY_ADDRESSING_CAPABILITY (8)

/*
** Indicate alignment restrictions on memory accesses
** For an n-bit memory access,
**
** if n <  PSP_MEMORY_ACCESSING_CAPABILITY,
**         n-bit accesses must be n-bit-aligned
**
** if n >= PSP_MEMORY_ACCESSING_CAPABILITY,
**         n-bit accesses must be PSP_MEMORY_ACCESSING_CAPABILITY-bit-aligned
*/
#define PSP_MEMORY_ACCESSING_CAPABILITY (32)

/*
** Define padding need to make the STOREBLOCK_STRUCT aligned properly
*/
#define PSP_MEM_STOREBLOCK_ALIGNMENT               (10)

/*
**   MINIMUM STACK SIZE FOR A TASK
*/

/*
** Memory overhead on users stack before switching over to the interrupt stack.
**
**
*/
#define PSP_STACK_INTERRUPT_OVERHEAD  (116)

/* This much extra stack is required for the logging of mqx functions */
#if MQX_KERNEL_LOGGING
#define PSP_STACK_KLOG_OVERHEAD       (128)
#else
#define PSP_STACK_KLOG_OVERHEAD         (0)
#endif

/* Minimum stack size for all tasks. */
#define PSP_MINSTACKSIZE \
   (sizeof(PSP_STACK_START_STRUCT) + \
   PSP_STACK_INTERRUPT_OVERHEAD + \
   PSP_STACK_KLOG_OVERHEAD)

/* Minimum stack size for the Idle Task
**   Idle task written in C uses at most 8 bytes of stack:
**     1 Link Instruction, 1 local variable (void *)
*/
#define PSP_IDLE_STACK_SIZE (PSP_MINSTACKSIZE + 8)

/* Init task stack size
*/
#define PSP_INIT_STACK_SIZE                         (PSP_MINSTACKSIZE * 10)

/* Motorola addresses do not need to be normalized
** (ie as for example the Intel chips in real mode do require this)
*/
#define _PSP_NORMALIZE_MEMORY(x) (x)


/*==========================================================================*/
/*
**                    PSP SPECIFIC DEFINITIONS
**
** These definitions will change from PSP to PSP
*/


/*
** Generic ARM Exception numbers
*/
#define PSP_RESET_EXCEPTION                  (0)
#define PSP_UNDEF_INSTR_EXCEPTION            (1)
#define PSP_SOFTWARE_EXCEPTION               (2)
#define PSP_ABORT_PREFETCH_EXCEPTION         (3)
#define PSP_ABORT_DATA_EXCEPTION             (4)
#define PSP_UNUSED_EXCEPTION                 (5)
#define PSP_IRQ_EXCEPTION                    (6)
#define PSP_FIQ_EXCEPTION                    (7)

/*
** The maximum number of hardware exceptions
*/
#define PSP_MAXIMUM_EXCEPTIONS   (PSP_FIQ_EXCEPTION+1)

/*
** This defines the maximum distance the vector jump table
** can be from the real vector table
*/
#define PSP_MAX_INT_JUMP_TABLE_OFFSET    (0x1000)

/*
** The number of processor modes
*/
#define PSP_NUMBER_OF_PROCESSOR_MODES   (7)

/*
** Control bits
*/
#define PSP_PSR_CTRL_DIS_IRQ      (0x00000080)
#define PSP_PSR_CTRL_DIS_FIQ      (0x00000040)
#define PSP_PSR_CTRL_IRQ_FIQ      (0x000000C0)
#define PSP_PSR_CTRL_THUMB_INST   (0x00000020)

/*
** The following define the Mode bits in the PSR register
*/
#define PSP_PSR_MODE_USER       (0x00000010)
#define PSP_PSR_MODE_FIQ        (0x00000011)
#define PSP_PSR_MODE_IRQ        (0x00000012)
#define PSP_PSR_MODE_SVC        (0x00000013)
#define PSP_PSR_MODE_ABORT      (0x00000017)
#define PSP_PSR_MODE_UNDEF      (0x0000001B)
#define PSP_PSR_MODE_SYS        (0x0000001F)
#define PSP_PSR_MODE_MASK       (0x0000001F)

/*
** The following define the Condition code bits in the PSR register
*/
#define PSP_PSR_CC_NEG          (0x80000000)
#define PSP_PSR_CC_ZERO         (0x40000000)
#define PSP_PSR_CC_CARRY        (0x20000000)
#define PSP_PSR_CC_OVERFLOW     (0x10000000)
#define PSP_PSR_CC_DNM_RAZ_MASK (0x0FFFFF00)

/*==========================================================================*/
/*                         PROCESSOR SPECIFIC DATA TYPES                    */

#ifndef __ASM__

/*-----------------------------------------------------------------------*/
/*
** PSP FUNCTION CALL STRUCT
**
** This structure defines what a function call pushes onto the stack
*/
typedef struct psp_function_call_struct
{

   /* Frame pointer (r11) pushed onto the stack for back tracing */
   void                *FP_REGISTER;

   /* Pushed onto the stack by the JSR instruction */
   void     (_CODE_PTR_ RETURN_ADDRESS)(void);

} PSP_FUNCTION_CALL_STRUCT, * PSP_FUNCTION_CALL_STRUCT_PTR;


/*==========================================================================*/
/*                         MQX DATA TYPES                                   */


/*-----------------------------------------------------------------------*/
/*
** PSP BLOCKED STACK STRUCT
**
** This is what a stack looks like for a task that is NOT the active
** task.
** Note the link register is stored twice so it can popped off once to setup
** a jump to a new PC and again to restore the original LR.
*/
typedef struct psp_blocked_stack_struct
{
    uint32_t PMR;
    uint32_t PSR;

    uint32_t R0;
    uint32_t R1;
    uint32_t R2;
    uint32_t R3;
    uint32_t R4;
    uint32_t R5;
    uint32_t R6;
    uint32_t R7;
    uint32_t R8;
    uint32_t R9;
    uint32_t R10;
    uint32_t R11;
    uint32_t R12;

    uint32_t LR;
    uint32_t PC;
} PSP_BLOCKED_STACK_STRUCT, * PSP_BLOCKED_STACK_STRUCT_PTR;

#if PSP_HAS_FPU
/*-----------------------------------------------------------------------*/
/*
** PSP BLOCKED FP STRUCT
**
** This structure defines the registers stored by MQX when a floating
** point task is blocked.  When the FLOATING_POINT_REGISTERS_STORED bit
** is set task's FLAGS, then this structure is valid.
*/
typedef struct psp_blocked_fp_struct {
    uint32_t FPSCR;      /* FP Status Control Register                   */
    uint32_t FPEXC;      /* mozno zrusit... Floating-Point Exception Register */
    /* The floating point computation registers     */
    union {
        float S[32];
        double D[32];
    } reg;
} PSP_BLOCKED_FP_STRUCT, * PSP_BLOCKED_FP_STRUCT_PTR;

#endif

/*-----------------------------------------------------------------------*/
/*
** PSP INTERRUPT CONTEXT STRUCT
**
** This structure provides a "context" for mqx primitives to use while executing
** in an interrupt handler.  A link list of these contexts is built on the
** interrupt stack.  The head of this list (the current interrupt context) is
** pointed to by the INTERRUPT_CONTEXT_PTR field of the kernel data structure.
**
*/
typedef struct psp_int_context_struct
{
   /* The CPSR for this interrupt frame */
   uint32_t   CPSR;

   /* The exception number for this interrupt frame */
   uint32_t   EXCEPTION_NUMBER;

   /* The "task" error code for use by mqx functions while in the ISR */
   uint32_t   ERROR_CODE;

   /* Used by the _int_enable function while in the ISR */
   uint32_t   ENABLE_SR;

   /* The link register contents */
   uint32_t   LR;

   /* Address of previous context, NULL if none */
   struct psp_int_context_struct      *PREV_CONTEXT;
} PSP_INT_CONTEXT_STRUCT, * PSP_INT_CONTEXT_STRUCT_PTR;


/*-----------------------------------------------------------------------*/
/*
** PSP BASIC INT FRAME STRUCT
**
** This structure is what is pushed onto the memory on the current stack
** when an interrupt occurs.
**
** For the first interrupt that interrupts a running task, this structure
** defines what the top of the stack for the task looks like,
** and the STACK_PTR field of the task descriptor will point to this structure.
**
** The rest of the interrupt frame is then built upon the interrupt stack.
**
** For a nested interrupt, this basic frame is also built upon the interrupt
** stack.
**
*/

typedef struct psp_basic_int_frame_struct
{

   /* The registers pushed by _int_kernel_isr
   ** Not all registers are stored to..
   */
   PSP_BLOCKED_STACK_STRUCT STACK;

} PSP_BASIC_INT_FRAME_STRUCT, * PSP_BASIC_INT_FRAME_STRUCT_PTR;


/*-----------------------------------------------------------------------*/
/*
** PSP INTERRUPT FRAME STRUCT
**
** This structure defines the stack as it appears presented to an interrupt
** service routine.  Note that this frame is on the interrupt stack.
**
** However for the very first interrupt (IN_ISR == 1)
** part of this structure (BASIC_INT_FRAME_STRUCT) is found on the
** stack of the interrupted task.
**
*/

typedef struct  psp_interrupt_frame_struct
{
   PSP_INT_CONTEXT_STRUCT           INTERRUPT_CONTEXT;

} PSP_INTERRUPT_FRAME_STRUCT, * PSP_INTERRUPT_FRAME_STRUCT_PTR;

/*-----------------------------------------------------------------------*/
/*
** PSP STACK START STRUCT
**
** This structure is used during the initialization of a new task.
** It is overlaid on the bottom of the task's stack
**
*/
typedef struct psp_stack_start_struct
{
   /* The start up registers for the task */
   PSP_BLOCKED_STACK_STRUCT     INITIAL_CONTEXT;
   
   /* The end of INITIAL CONTEXT has to be aligned at 8B boundary. Presuming that the end of this structure is
   ** aligned at PSP_STACK_ALIGN boundary, we must add reserved bytes to achieve alignment.
   ** The reserved space is computed from the used space from the end (5 * 32-bit value).
   */
   uint8_t                      RESERVED[(PSP_STACK_ALIGNMENT_MASK - 5 * sizeof(uint32_t)) & 0x7];
   
   /* Previous stack pointer for exit return */
   void                        *PREVIOUS_STACK_POINTER;

   /* The function to call when the task "returns" */
   void             (_CODE_PTR_ EXIT_ADDRESS)();

   /* The task's parameter */
   uint32_t                 PARAMETER;

   /* The following two fields are used to create a "bottom" of stack
   ** that debuggers will recognize
   */
   /* End stack pointer    */
   void                        *ZERO_STACK_POINTER;

   /* close the stack frame with a return address of 0 */
   uint32_t                      ZERO_RETURN_ADDRESS;

} PSP_STACK_START_STRUCT, * PSP_STACK_START_STRUCT_PTR;

#endif // __ASM__

#endif
