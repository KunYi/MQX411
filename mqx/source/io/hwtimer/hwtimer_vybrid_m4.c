
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
*   This file contains vybrid cortext M4 specific functions of the hwtimer
*   component.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "hwtimer.h"

/*!
 * \cond DOXYGEN_PRIVATE
 * Macro return number of item in pit_vectors_table array
 */
#define PIT_INTERRUPT_COUNT 1

/*!
 * \cond DOXYGEN_PRIVATE
 * Macro return size of pit_hwtimers_array.
 */
#define PIT_HWTIMERS_ARRAY_SIZE 8
/*!
 * \cond DOXYGEN_PRIVATE
 * \brief Array of PIT interrupt vectors
 */
const _mqx_uint pit_vectors_table[] =
    {
        NVIC_PIT
    };

/*!
 * \cond DOXYGEN_PRIVATE
 * \brief Array where hwtimers should be stored. 
 *
 * Size of array is count of all channels for all pit devices
 */
HWTIMER_PTR pit_hwtimers_array[PIT_HWTIMERS_ARRAY_SIZE] = {0};

/*!
 * \cond DOXYGEN_PRIVATE
 *
 * \brief This function performs BSP-specific initialization related to pit
 *
 * \param dev_num[in]   Number of PIT module.
 *
 * \return MQX_OK Success.
 *
 * \see pit_get_vectors
 * \see pit_get_hwtimers_array
 */
_mqx_int pit_io_init
(
    uint32_t dev_num
)
{
    switch (dev_num)
    {
        case 0:
            CCM_CCGR1 |= CCM_CCGR1_CG7(0x3);
            break;
        default:
            /* Do nothing if bad dev_num was selected */
            return -1;
    }

    return MQX_OK;
}

/*!
 * \cond DOXYGEN_PRIVATE
 *
 * \brief This function get array of vectors and count of items in this array.
 *
 * \param pit_vectors_table_ptr[out]  Used to get pit_vectors_table.
 *
 * \return PIT_INTERRUPT_COUNT Count of interrupt vectors for PIT module.
 *
 * \see pit_io_init
 * \see pit_get_hwtimers_array
 */
uint32_t pit_get_vectors
(
    uint32_t pit_number,
    const _mqx_uint **pit_vectors_table_ptr
)
{
    switch (pit_number)
    {
        case 0:
            *pit_vectors_table_ptr = pit_vectors_table;
            break;
        default:
            *pit_vectors_table_ptr = NULL;
    }

    return PIT_INTERRUPT_COUNT;
}

/*!
 * \cond DOXYGEN_PRIVATE
 *
 * \brief This function get array of hwtimer and count of items in this array.
 *
 * \param pit_vectors_table_ptr[out]  Used to get pit_hwtimers_array.
 *
 * \return PIT_HWTIMERS_ARRAY_SIZE Size of pit_hwtimers_array
 *
 * \see pit_io_init
 * \see pit_get_vectors
 */
uint32_t pit_get_hwtimers_array
(
    HWTIMER_PTR ** hwtimers_array
)
{
    *hwtimers_array = pit_hwtimers_array;
    return PIT_HWTIMERS_ARRAY_SIZE;
}

/*******************SysTick********************/

/*!
 * \cond DOXYGEN_PRIVATE
 *
 * \brief This function get Interrupt Number.
 *
 * \param pit_vectors_table_ptr[out]  Used to get pit_vectors_table.
 *
 * \return Interrupt Number for SysTick module.
 *
 */
uint32_t systick_get_vector()
{
    return NVIC_SysTick;
}
