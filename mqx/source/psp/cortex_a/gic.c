
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
* ARM Generic Interrupt Controller version 2.0
*
*
*END************************************************************************/

#include "gic.h"

/*!
 * \brief 	Initialize a specific interrupt in the GIC
 * 
 * \param[in] irq Interrupt number
 * \param[in] prior Interrupt priority 
 * \param[in] enable enable the interrupt now?
 *
 * \return _mqx_uint MQX_OK or error code
 */
_mqx_uint _gic_int_init
   (
      // [IN] Interrupt number
      _mqx_uint irq,

      // [IN] Interrupt priority
      _mqx_uint prior,

      // [IN] enable the interrupt now?
      bool enable
   )
{
    _mqx_uint gic_irq_no = irq - 16;

    // check priority value, must be below maximal enabled/set value
    if (prior >= (1 << GIC_PRIOR_IMPL)) {
        return MQX_INVALID_PARAMETER;
    }

    if (irq >= PSP_INT_FIRST_INTERNAL && irq <= PSP_INT_LAST_INTERNAL) {
        // set priority
        GIC_ICDIPR(gic_irq_no >> 2) = (GIC_ICDIPR(gic_irq_no >> 2) & ~(0xff << ((gic_irq_no & 3) * 8))) | (((prior << GIC_PRIOR_SHIFT) & GIC_PRIOR_MASK) << ((gic_irq_no & 3) * 8));

        if (enable)
            _gic_int_enable(irq);
        else
            _gic_int_disable(irq);
    }
    else
        return MQX_INVALID_PARAMETER;

    return MQX_OK;
}

/*!
 * \brief 	Enable interrupt on GIC
 *
 * \param irq
 *
 * \return _mqx_uint MQX_OK or error code
 */
_mqx_uint _gic_int_enable(_mqx_uint irq) {
    _mqx_uint gic_irq_no = irq - 16;

    if (irq >= PSP_INT_FIRST_INTERNAL && irq <= PSP_INT_LAST_INTERNAL) {
        GICD_ISENABLER(gic_irq_no / 32) = (1 << (gic_irq_no % 32));
    }
    else
        return MQX_INVALID_PARAMETER;

    return MQX_OK;
}

/*!
 * \brief 	Disable interrupt on GIC
 *
 * \param irq
 *
 * \return _mqx_uint MQX_OK or error code
 */
_mqx_uint _gic_int_disable(_mqx_uint irq) {
    _mqx_uint gic_irq_no = irq - 16;

    if (irq >= PSP_INT_FIRST_INTERNAL && irq <= PSP_INT_LAST_INTERNAL) {
        GICD_ICENABLER(gic_irq_no / 32) = (1 << (gic_irq_no % 32));
    }
    else
        return MQX_INVALID_PARAMETER;

    return MQX_OK;
}
