
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*   This file contains the functions for manipulating the user
*	context on the stack.
*
*
*END************************************************************************/

#include "mqx_inc.h"

/*!
 * \brief  This function sets up the stack frame of a new task descriptor.
 * 
 * \param[in] td_ptr the address of the task descriptor
 * \param[in] stack_ptr the address of the stack memory block
 * \param[in] stack_size the size of the stack
 * \param[in] template_ptr the task's template
 * \param[in] status_register the status register to use in creating the task
 * \param[in] create_parameter the task creation parameter
 */
bool _psp_build_stack_frame
    (
        /* [IN] the address of the task descriptor */
        TD_STRUCT_PTR td_ptr,

        /* [IN] the address of the stack memory block */
        void   *stack_ptr,

        /* [IN] the size of the stack */
        _mem_size stack_size,

        /* [IN] the task's template */
        TASK_TEMPLATE_STRUCT_PTR template_ptr,

        /* [IN] the status register to use in creating the task */
        _mqx_uint status_register,

        /* [IN] the task creation parameter */
        uint32_t create_parameter
    )
{
    unsigned char *stack_base_ptr;
    PSP_STACK_START_STRUCT_PTR stack_start_ptr;
    uint32_t reg_val;
    bool res = TRUE;

    stack_base_ptr  = (unsigned char *)_GET_STACK_BASE(stack_ptr, stack_size);
    stack_start_ptr = (PSP_STACK_START_STRUCT_PTR)(stack_base_ptr - sizeof(PSP_STACK_START_STRUCT));

    td_ptr->STACK_BASE  = (void *)stack_base_ptr;
    td_ptr->STACK_LIMIT = _GET_STACK_LIMIT(stack_ptr, stack_size);
    td_ptr->STACK_PTR   = stack_start_ptr;

    /*
    ** Build the task's initial stack frame. This contains the initialized
    ** registers, and an exception frame which will cause the task to
    ** "return" to the start of the task when it is dispatched.
    */
    _mem_zero(stack_start_ptr, (_mem_size)sizeof(PSP_STACK_START_STRUCT));

    _PSP_GET_CPSR(reg_val);
    reg_val &= ~(PSP_PSR_CTRL_DIS_IRQ | PSP_PSR_CTRL_DIS_FIQ);

    if ((uint32_t)(template_ptr->TASK_ADDRESS) & 1) {
        reg_val |= PSP_PSR_CTRL_THUMB_INST;
    }

    stack_start_ptr->INITIAL_CONTEXT.PSR = reg_val;
    stack_start_ptr->INITIAL_CONTEXT.PC = (uint32_t)template_ptr->TASK_ADDRESS;
    stack_start_ptr->INITIAL_CONTEXT.LR = (uint32_t)_task_exit_function_internal;
    stack_start_ptr->INITIAL_CONTEXT.R0 = (uint32_t)create_parameter;
    stack_start_ptr->INITIAL_CONTEXT.PMR = (uint32_t)status_register;     // GICC interrupt priority mask
    stack_start_ptr->PREVIOUS_STACK_POINTER = (void *)&stack_start_ptr->ZERO_STACK_POINTER;
    stack_start_ptr->EXIT_ADDRESS = _task_exit_function_internal;
    stack_start_ptr->PARAMETER = create_parameter;

#if MQXCFG_ENABLE_FP && PSP_HAS_FPU
    if (td_ptr->FLAGS & MQX_FLOATING_POINT_TASK) {
        res = _psp_build_float_context(td_ptr);
    }
#endif /* MQXCFG_ENABLE_FP && PSP_HAS_FPU */
    return res;
}

#if MQXCFG_ENABLE_FP && PSP_HAS_FPU
/*!
 * \brief This function sets up the FLOATING POINT context of a new task descriptor.
 * 
 * \param[in] td_ptr the address of the task descriptor
 */
bool _psp_build_float_context
   (
      /* [IN] the address of the task descriptor */
      TD_STRUCT_PTR    td_ptr
   )
{
    PSP_BLOCKED_FP_STRUCT_PTR fp_ptr;

    /* Allocate space for saving/restoring the DSP registers */
    fp_ptr = (PSP_BLOCKED_FP_STRUCT_PTR)_mem_alloc_zero((_mem_size)sizeof(PSP_BLOCKED_FP_STRUCT));

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (!fp_ptr) {
        /* Couldn't allocate memory for the DSP register context */
        _task_set_error_td_internal(td_ptr, MQX_OUT_OF_MEMORY);
        return FALSE;
    }
#endif

    _mem_set_type(fp_ptr, MEM_TYPE_FP_CONTEXT);
    /*
    ** Transfer the block to the task being created. This will ensure the
    ** float context will be freed if the task is destroyed.
    */
    _mem_transfer_internal((void *)fp_ptr, td_ptr);

    /* This field should never be overwitten */
    //fp_ptr->TID = td_ptr->TASK_ID;
    fp_ptr->FPSCR = 0x03000000;
    fp_ptr->FPEXC = 0x40000000;

    td_ptr->FLOAT_CONTEXT_PTR = (void *)fp_ptr;

    return TRUE;
}
#endif /* MQXCFG_ENABLE_FP && PSP_HAS_FPU */
