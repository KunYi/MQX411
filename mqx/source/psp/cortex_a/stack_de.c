
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
*   This file contains the functions for manipulating the user
*   context on the stack.
*
*
*END************************************************************************/

#include "mqx_inc.h"

/*!
 * \brief  This function performs any PSP specific destruction for a task context
 * 
 * \param[in] td_ptr the task descriptor whose stack needs to be destroyed
 */
void _psp_destroy_stack_frame
    (
        /* [IN] the task descriptor whose stack needs to be destroyed */
        TD_STRUCT_PTR td_ptr
    )
{
}
