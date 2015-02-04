
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
*
*   This file contains the functions used to access the vector
*   table locations.
*
*
*END************************************************************************/

#include "mqx_inc.h"

#if MQX_USE_INTERRUPTS
/*!
 * \brief Changes the location of the interrupt vector table.
 *
 * \param[in] new Address of the new interrupt vector table.
 *
 * \return Address of the previous vector table.
 *
 * \warning Behavior depends on the BSP and the PSP.
 *
 * \see _int_get_vector_table
 * \see _int_get_previous_vector_table
 */
_psp_code_addr _int_set_vector_table
    (
        _psp_code_addr new
    )
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    PSP_SUPPORT_STRUCT_PTR psp_support_ptr;
    _psp_code_addr old;

    _GET_KERNEL_DATA(kernel_data);

    psp_support_ptr = (PSP_SUPPORT_STRUCT_PTR)kernel_data->PSP_SUPPORT_PTR;
    old = psp_support_ptr->VBR;
    psp_support_ptr->VBR = new;

    return old;
}

/*!
 * \brief Gets the address of the current interrupt vector table. The function
 * depends on the PSP.
 *
 * \return Address of the current interrupt vector table.
 *
 * \see _int_set_vector_table
 * \see _int_get_previous_vector_table
 */
_psp_code_addr _int_get_vector_table
    (
        void
    )
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    PSP_SUPPORT_STRUCT_PTR   psp_support_ptr;

    _GET_KERNEL_DATA(kernel_data);
    psp_support_ptr = (PSP_SUPPORT_STRUCT_PTR)kernel_data->PSP_SUPPORT_PTR;

    return (psp_support_ptr->VBR);
}

#endif
