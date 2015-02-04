
/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
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
*   This file contains Vybrid-specific functions of the MCC library
*
*
*END************************************************************************/

#include "mcc_vf600.h"
#include "mcc_mqx.h"

/* This field contains CPU-to-CPU interrupt vector numbers for all device cores */
static const unsigned int mcc_cpu_to_cpu_vectors[] = { INT_CPU_to_CPU_int0, NVIC_CPU_to_CPU_int0 };

/*!
 * \brief This function gets the CPU-to-CPU vector number for the particular core.
 *
 * Platform-specific inter-CPU vector numbers for each core are defined in the mcc_cpu_to_cpu_vectors[] field.
 *
 * \param[in] core Core number.
 *
 * \return vector number for the particular core
 * \return MCC_VECTOR_NUMBER_INVALID (vector number for the particular core number not found)
 */
unsigned int mcc_get_cpu_to_cpu_vector(unsigned int core)
{
    if (core < (sizeof(mcc_cpu_to_cpu_vectors)/sizeof(mcc_cpu_to_cpu_vectors[0]))) {
        return  mcc_cpu_to_cpu_vectors[core];

    }
    return MCC_VECTOR_NUMBER_INVALID;
}

/*!
 * \brief This function clears the CPU-to-CPU interrupt flag for the particular core.
 *
 * Implementation is platform-specific.
 *
 * \param[in] core Core number.
 */
void mcc_clear_cpu_to_cpu_interrupt(unsigned int core)
{
    if(core == 0) {
        /* clear the flag in the MSCM_IRCP0IR register */
        MSCM_IRCP0IR = MSCM_IRCP0IR_INT0_MASK;
    }
    else if(core == 1) {
        /* clear the flag in the MSCM_IRCP1IR register */
        MSCM_IRCP1IR = MSCM_IRCP1IR_INT0_MASK;
   }
}

/*!
 * \brief This function triggers the CPU-to-CPU interrupt.
 *
 * Platform-specific software triggering the inter-CPU interrupts.
 */
void mcc_triger_cpu_to_cpu_interrupt(void)
{
    /* set TLF filed of the MSCM_IRCPGIR to assert directed CPU interrupts for all processors except the requesting core */
    MSCM_IRCPGIR = MSCM_IRCPGIR_TLF(1) | MSCM_IRCPGIR_INTID(0);
}
