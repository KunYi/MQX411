
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
*   This file contains functions of the low level Global Timer module for hwtimer
*   component.
*
*
*END************************************************************************/

#include "hwtimer.h"
#include "hwtimer_gtim.h"
#include <bsp.h>

extern uint32_t gtim_get_vector(void);

static void hwtimer_gtim_isr(void *);
static _mqx_int hwtimer_gtim_init(HWTIMER_PTR , uint32_t, uint32_t);
static _mqx_int hwtimer_gtim_deinit(HWTIMER_PTR);
static _mqx_int hwtimer_gtim_set_div(HWTIMER_PTR, uint32_t);
static _mqx_int hwtimer_gtim_start(HWTIMER_PTR);
static _mqx_int hwtimer_gtim_stop(HWTIMER_PTR);
static _mqx_int hwtimer_gtim_get_time(HWTIMER_PTR , HWTIMER_TIME_PTR);


const HWTIMER_DEVIF_STRUCT gtim_devif =
{
    hwtimer_gtim_init,
    hwtimer_gtim_deinit,
    hwtimer_gtim_set_div,
    hwtimer_gtim_start,
    hwtimer_gtim_stop,
    hwtimer_gtim_get_time
};

/*!
 * \cond DOXYGEN_PRIVATE
 *
 * \brief Interrupt service routine.
 *
 * This ISR is used when gtim counted to 0.
 * Checks whether callback_func is not NULL,
 * and unless callback is blocked by callback_blocked being non-zero it calls the callback function with callback_data as parameter,
 * otherwise callback_pending is set to non-zero value.
 *
 * \param p[in]   Pointer to hwtimer struct.
 *
 * \return void
 *
 * \see hwtimer_gtim_deinit
 * \see hwtimer_gtim_set_div
 * \see hwtimer_gtim_start
 * \see hwtimer_gtim_stop
 * \see hwtimer_gtim_get_time
 */
static void hwtimer_gtim_isr(void *p)
{
    HWTIMER_PTR hwtimer       = (HWTIMER_PTR) p;
    GTIM_MemMapPtr gtim_base  = (GTIM_MemMapPtr) hwtimer->ll_context[0];

    /* Clear interrupt flag */
    GTIM_ISTAT_REG(gtim_base) = 0x1;
    /* Check if interrupt is enabled for this gtim. Cancel spurious interrupt */
    if (!(GTIM_CTRL_INT_MASK & GTIM_CTRL_REG(gtim_base)))
    {
        return;
    }

    /* Following part of function is typically the same for all low level hwtimer drivers */
    hwtimer->ticks++;

    if (NULL != hwtimer->callback_func)
    {
        if (hwtimer->callback_blocked)
        {
            hwtimer->callback_pending = 1;
        }
        else
        {
            /* Run user function*/
            hwtimer->callback_func(hwtimer->callback_data);
        }
    }
}


/*!
 * \cond DOXYGEN_PRIVATE
 *
 * \brief This function initializes caller allocated structure according to given
 * numerical identifier of the timer.
 *
 * Called by hwtimer_init().
 * Initializes the HWTIMER structure.
 * Sets interrupt priority and registers ISR.
 *
 * \param hwtimer[in]    Returns initialized hwtimer structure handle.
 * \param gtim_id[in] Determines Systick modul( Always 0).
 * \param isr_prior[in]  Interrupt priority for PIT
 *
 * \return MQX_OK                       Success.
 * \return MQX_INVALID_PARAMETER        When gtim_id does not exist(When gtim_id is not zero).
 * \return IO_ERROR                     When gtim is used byt PE or when _int_install_isr faild.
 *
 * \see hwtimer_gtim_deinit
 * \see hwtimer_gtim_set_div
 * \see hwtimer_gtim_start
 * \see hwtimer_gtim_stop
 * \see hwtimer_gtim_get_time
 * \see hwtimer_gtim_isr
 */
static _mqx_int hwtimer_gtim_init (HWTIMER_PTR hwtimer, uint32_t gtim_id, uint32_t isr_prior)
{
    PSP_INTERRUPT_TABLE_INDEX   vector;
    GTIM_MemMapPtr              gtim_base;

    /* We count only with one gtim module inside core */
    #if MQX_CHECK_ERRORS
    if ( 0 < gtim_id)
    {
        return MQX_INVALID_PARAMETER;
    }
    #endif

    /* We need to store Global Timer base in context struct */
    gtim_base = GTIM_BASE_PTR;

    #if PE_LDD_VERSION
    if (PE_PeripheralUsed((uint32_t)gtim_base))
    {
        return IO_ERROR;
    }
    #endif

    hwtimer->ll_context[0] = (uint32_t) gtim_base;

    /* We dont wont to stop timer if is runing from another core . Also we want prescaler equal to zero*/
    GTIM_CTRL_REG(gtim_base) &= ~(GTIM_CTRL_COMP_MASK | GTIM_CTRL_INT_MASK | GTIM_CTRL_PRESC_MASK);

    /* Clear any pending interrupt flag */
    GTIM_ISTAT_REG(gtim_base) = 0x1;

    /* Set control register*/
    GTIM_CTRL_REG(gtim_base) |= GTIM_CTRL_INCR_MASK | GTIM_CTRL_INT_MASK;

    /* Set isr for timer*/
    vector = (PSP_INTERRUPT_TABLE_INDEX) gtim_get_vector();
    if (NULL == _int_install_isr(vector, (INT_ISR_FPTR) hwtimer_gtim_isr, (void *) hwtimer))
    {
        return IO_ERROR;
    }
    /* Set interrupt priority and enable interrupt */
    if (MQX_OK != _bsp_int_init(vector, isr_prior, 0, TRUE))
    {
        return IO_ERROR;
    }

    return MQX_OK;
}

/*!
 * \cond DOXYGEN_PRIVATE
 *
 * \brief Initialization of gtim timer module
 *
 * Called by hwtimer_deinit.
 * Disables the peripheral.
 * Unregisters ISR.
 *
 * \param hwtimer[in] Pointer to hwtimer structure.
 *
 * \return MQX_OK                       Success.
 * \return MQX_INVALID_COMPONENT_HANDLE When doesnt have any interrupt vectors, or gtim does not exist.
 *
 * \see hwtimer_gtim_init
 * \see hwtimer_gtim_set_div
 * \see hwtimer_gtim_start
 * \see hwtimer_gtim_stop
 * \see hwtimer_gtim_get_time
 * \see hwtimer_gtim_isr
 */
static _mqx_int hwtimer_gtim_deinit(HWTIMER_PTR hwtimer)
{
    GTIM_MemMapPtr              gtim_base = (GTIM_MemMapPtr) hwtimer->ll_context[0];
    PSP_INTERRUPT_TABLE_INDEX   vector;

    /* Reset CTRL register, disable timer */
    GTIM_CTRL_REG(gtim_base) &= ~(GTIM_CTRL_COMP_MASK | GTIM_CTRL_INT_MASK | GTIM_CTRL_INCR_MASK);

    /* Clear any pending interrupt flag */
    GTIM_ISTAT_REG(gtim_base) = 0x1;

    /* Every channel has own interrupt vector */
    vector = (PSP_INTERRUPT_TABLE_INDEX) gtim_get_vector();
    /* Disable interrupt on vector */
    _bsp_int_disable(vector);
    /* Install default isr routine for our gtim */
    _int_install_isr(vector, _int_get_default_isr(), NULL);

    return MQX_OK;
}

/*!
 * \cond DOXYGEN_PRIVATE
 *
 * \brief Sets up timer with divider settings closest to the requested total divider factor.
 *
 * Called by hwtimer_set_freq() and hwtimer_set_period().
 * Fills in the divider (actual total divider) and modulo (sub-tick resolution) members of the HWTIMER structure.
 *
 * \param hwtimer[in] Pointer to hwtimer structure.
 * \param divider[in] Value which divide input clock of gtim timer module to obtain requested period of timer.
 *
 * \return MQX_OK                Success.
 * \return MQX_INVALID_PARAMETER Divider is equal to zero.
 *
 * \see hwtimer_gtim_init
 * \see hwtimer_gtim_deinit
 * \see hwtimer_gtim_start
 * \see hwtimer_gtim_stop
 * \see hwtimer_gtim_get_time
 * \see hwtimer_gtim_isr
 */
static _mqx_int hwtimer_gtim_set_div(HWTIMER_PTR hwtimer, uint32_t divider)
{
    GTIM_MemMapPtr           gtim_base = (GTIM_MemMapPtr) hwtimer->ll_context[0];

    #if MQX_CHECK_ERRORS
    if (0 == divider )
    {
        return MQX_INVALID_PARAMETER;
    }
    #endif

     GTIM_INCR_REG(gtim_base) = GTIM_INCR_INCR(divider);

     hwtimer->divider    = divider;
     hwtimer->modulo     = divider;

    return MQX_OK;
}


/*!
 * \cond DOXYGEN_PRIVATE
 *
 * \brief Start gtim timer module
 *
 * This function enables the timer and leaves it running, timer is
 * periodically generating interrupts.
 *
 * \param hwtimer[in] Pointer to hwtimer structure.
 *
 * \return MQX_OK Success.
 *
 * \see hwtimer_gtim_init
 * \see hwtimer_gtim_deinit
 * \see hwtimer_gtim_set_div
 * \see hwtimer_gtim_stop
 * \see hwtimer_gtim_get_time
 * \see hwtimer_gtim_isr
 */
static _mqx_int hwtimer_gtim_start(HWTIMER_PTR hwtimer)
{
    GTIM_MemMapPtr gtim_base = (GTIM_MemMapPtr) hwtimer->ll_context[0];
    uint64_t tmp;
    uint32_t cnt_up, cnt_low;

    /* Clear any pending interrupt flag */
    GTIM_ISTAT_REG(gtim_base) = 0x1;

    /* Clear compare bit in control register to stop comparing */
    GTIM_CTRL_REG(gtim_base) &= ~(GTIM_CTRL_COMP_MASK);

    /* Read counter registers to temp variable */
    _int_disable();
    cnt_up  = GTIM_CNT1_REG(gtim_base);
    cnt_low = GTIM_CNT0_REG(gtim_base);
    if (cnt_up != GTIM_CNT1_REG(gtim_base))
    {
        cnt_up  = GTIM_CNT1_REG(gtim_base);
        cnt_low = GTIM_CNT0_REG(gtim_base);
    }

    tmp = ((uint64_t)cnt_up) << 32;
    tmp |= cnt_low;
    /* Add increment value from auto increment register */
    tmp += GTIM_INCR_REG(gtim_base);
    GTIM_CMP0_REG(gtim_base) = GTIM_CMP0_CMP(tmp);
    GTIM_CMP1_REG(gtim_base) = GTIM_CMP1_CMP(tmp >> 32);

    /* Run timer and enable interrupt */
    GTIM_CTRL_REG(gtim_base) = GTIM_CTRL_ENA_MASK | GTIM_CTRL_COMP_MASK | GTIM_CTRL_INT_MASK | GTIM_CTRL_INCR_MASK;
    _int_enable();

    return MQX_OK;
}

/*!
 * \cond DOXYGEN_PRIVATE
 *
 * \brief Stop gtim timer module
 *
 * Disable timer and interrupt
 *
 * \param hwtimer[in] Pointer to hwtimer structure.
 *
 * \return MQX_OK Success.
 *
 * \see hwtimer_gtim_init
 * \see hwtimer_gtim_deinit
 * \see hwtimer_gtim_set_div
 * \see hwtimer_gtim_start
 * \see hwtimer_gtim_get_time
 * \see hwtimer_gtim_isr
 */
static _mqx_int hwtimer_gtim_stop(HWTIMER_PTR hwtimer)
{
    GTIM_MemMapPtr gtim_base = (GTIM_MemMapPtr) hwtimer->ll_context[0];

    /* Disable to compare counter registers with compare registers. Counter register is still incremented */
    GTIM_CTRL_REG(gtim_base) &= ~(GTIM_CTRL_COMP_MASK | GTIM_CTRL_ENA_MASK);

    return MQX_OK;
}

/*!
 * \cond DOXYGEN_PRIVATE
 *
 * \brief Atomically captures current time into HWTIMER_TIME_STRUCT structure
 *
 * Corrects/normalizes the values if necessary (interrupt pending, etc.)
 *
 * \param hwtimer[in] Pointer to hwtimer structure.
 * \param time[out]   Pointer to time structure. This value is filled with current value of the timer.
 *
 * \return MQX_OK Success.
 *
 * \warning This function calls _int_enable and _int_disable functions
 *
 * \see hwtimer_gtim_init
 * \see hwtimer_gtim_deinit
 * \see hwtimer_gtim_set_div
 * \see hwtimer_gtim_start
 * \see hwtimer_gtim_stop
 * \see hwtimer_gtim_isr
 */
static _mqx_int hwtimer_gtim_get_time(HWTIMER_PTR hwtimer, HWTIMER_TIME_PTR time)
{
    GTIM_MemMapPtr gtim_base = (GTIM_MemMapPtr) hwtimer->ll_context[0];
    uint64_t cnt, comp;
    uint32_t cnt_up, cnt_low;

    _int_disable();
    time->TICKS = hwtimer->ticks;
    /* Check gtim pending interrupt flag */
    if (GTIM_ISTAT_REG(gtim_base))
    {
        _int_enable();
        time->SUBTICKS = hwtimer->modulo - 1;
    }
    else
    {
        /* When counter(64bit value) reach compare(64bit value) compare is incremented with autoincrement value */
        /* Way to get subticks is : counter - (compare - autoinc)*/
        /* Get 64 bit counter value */
        cnt_up  = GTIM_CNT1_REG(gtim_base);
        cnt_low = GTIM_CNT0_REG(gtim_base);
        if (cnt_up != GTIM_CNT1_REG(gtim_base))
        {
            cnt_up  = GTIM_CNT1_REG(gtim_base);
            cnt_low = GTIM_CNT0_REG(gtim_base);
        }
        cnt = ((uint64_t)cnt_up) << 32;
        cnt |= cnt_low;

        /* Get compare value */
        comp = ((uint64_t)GTIM_CMP1_REG(gtim_base)) << 32;
        comp |= GTIM_CMP0_REG(gtim_base);
        /* Now if counter reach compare , then interrupt flag is set and subticks are equal to zero */
        if (GTIM_ISTAT_REG(gtim_base))
        {
            _int_enable();
            time->SUBTICKS = hwtimer->modulo - 1;
        }
        else
        {
            comp -= GTIM_INCR_REG(gtim_base);

            time->SUBTICKS = (uint32_t) cnt - comp;
            _int_enable();
        }
    }

    return MQX_OK;
}
