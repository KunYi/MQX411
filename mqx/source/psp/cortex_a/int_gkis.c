
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
*   This file contains the functions for returning the kernel isr for an interrupt.
*
*
*END************************************************************************/

#include "mqx_inc.h"

/*!
 * \brief Gets a pointer to the kernel ISR for the specified vector number. The
 * kernel ISR depends on the PSP.
 *
 * \param[in] vector Vector number whose kernel ISR is requested.
 *
 * \return Pointer to the kernel ISR (Success.)
 * \return NULL
 *
 * \warning On failure, calls _task_set_error() to set the task error code:
 * \li MQX_INVALID_VECTORED_INTERRUPT
 *
 * \see _int_kernel_isr
 * \see _int_install_kernel_isr
 */
INT_KERNEL_ISR_FPTR _int_get_kernel_isr
(
    uint32_t vector
)
{
    /* This functionality is not supported in MQX for Cortex A5 */
    return NULL;
}
