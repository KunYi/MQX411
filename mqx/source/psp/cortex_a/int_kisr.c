
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
*   This file contains the function for installing a kernel level isr.
*
*
*END************************************************************************/

#include "mqx_inc.h"

/*!
 * \brief Installs the kernel ISR handler. The kernel ISR depends on the PSP.
 *
 * Some real-time applications need special event handling to occur outside the
 * scope of MQX. The need might arise that the latency in servicing an interrupt
 * be less than the MQX interrupt latency. If this is the case, an application can
 * use _int_install_kernel_isr() to bypass MQX and let the interrupt be serviced
 * immediately.
 * \n Because the function returns the previous kernel ISR, applications can
 * temporarily install an ISR or chain ISRs so that each new one calls the one
 * installed before it.
 * \n A kernel ISR must save the registers that it needs and must service the
 * hardware interrupt. When the kernel ISR is finished, it must restore the
 * registers and perform a return-from-interrupt instruction.
 * \n A kernel ISR cannot call MQX functions. However, it can put data in global
 * data, which a task can access.
 *
 * \note The function is not available for all PSPs.
 *
 * \param[in] vector  Vector where the ISR is to be installed.
 * \param[in] isr_ptr Pointer to the ISR to install into the vector table.
 *
 * \return Pointer to the previous kernel ISR for the vector (Success.).
 * \return NULL
 *
 * \see _int_kernel_isr
 * \see _int_get_kernel_isr
 */
INT_KERNEL_ISR_FPTR _int_install_kernel_isr
(
    uint32_t vector,
    INT_KERNEL_ISR_FPTR isr_ptr
)
{
    /* This functionality is not supported in MQX for Cortex A5 */
    return NULL;
}
