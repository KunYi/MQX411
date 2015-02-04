/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2011 Embedded Access Inc.
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
*   This file contains the ADC driver CPU specific functions
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <lwadc_vadc_prv.h>

static ADC_CONTEXT      adc_context[ADC_NUM_DEVICES] = {0};
static uint32_t          adc_max_frq;


/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_init
 * Returned Value   : TRUE for success, FALSE for failure
 * Comments         :
 *    This function initializes the ADC device with global parameters
 *    (conversion frequency, resolution, number format). It does  not start
 *    any conversions.
 *
 *END*----------------------------------------------------------------------*/

bool _lwadc_init(const LWADC_INIT_STRUCT *init_ptr)
{
    ADC_MemMapPtr   adc_ptr;
    uint8_t          adc_max_frq_index;
    ADC_CONTEXT_PTR  context_ptr;

    /* This table is made from datasheet values for ADC module in Kinetis */
    const static uint32_t adc_max_frq_table[] = {
        2500000,  /* 2.5 MHz  for low power,    normal speed, 12b resolution */
        5000000,  /* 5.0 MHz  for low power,    normal speed, lower resolution */
        5000000,  /* 5.0 MHz  for low power,    high speed,   12b resolution */
        8000000,  /* 8.0 MHz  for low power,    high speed,   lower resolution */
        8000000,  /* 8.0 MHz  for normal power, normal speed, 12b resolution */
        12000000, /* 12.0 MHz for normal power, normal speed, lower resolution */
        12000000, /* TBD MHz  for normal power, high speed,   12b resolution */
        22000000, /* TBD MHz  for normal power, high speed,   lower resolution */
    };
    #if PSP_HAS_DEVICE_PROTECTION
    if (!_bsp_adc_enable_access(init_ptr->ADC_NUMBER)) {
        return FALSE;
    }
    #endif

    adc_ptr = _bsp_get_adc_base_address(init_ptr->ADC_NUMBER);
    if (adc_ptr == NULL) {
         return FALSE;
    }

    // enable clock for adc module
    if (_bsp_adc_io_init(init_ptr->ADC_NUMBER) != IO_OK)
    {
        return FALSE;
    }

    context_ptr =  &adc_context[init_ptr->ADC_NUMBER];
    context_ptr->channels = 0;
    context_ptr->default_numerator   = init_ptr->REFERENCE;
    context_ptr->default_denominator = LWADC_RESOLUTION_DEFAULT;
    context_ptr->adc_ptr = adc_ptr;

    /* First, use 3 bit mask to create index to the max. frq. table */
    adc_max_frq_index = 0;
    switch (context_ptr->default_denominator) {
        case LWADC_RESOLUTION_12BIT:
            adc_max_frq_index |= 0x01;
            break;
        case LWADC_RESOLUTION_10BIT:
        case LWADC_RESOLUTION_8BIT:
            break;
        default:
            return FALSE; /* bad resolution set */
    }
    if (init_ptr->SPEED == LWADC_HSC_HIGH)
        adc_max_frq_index |= 0x02;
    if (init_ptr->POWER == LWADC_LPC_NORMAL)
        adc_max_frq_index |= 0x04;
    adc_max_frq = adc_max_frq_table[adc_max_frq_index];

    /* set AD clock to be as fast as possible */
     switch (init_ptr->CLOCK_SOURCE) {
         case LWADC_CLK_BUSCLK_ANY:
             if (init_ptr->CLOCK_DIV == LWADC_DIV_ANY) {
                 if(BSP_BUS_CLOCK <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_1 | LWADC_CFG_ADICLK_BUSCLK;
                 else if ((BSP_BUS_CLOCK / 2) <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_2 | LWADC_CFG_ADICLK_BUSCLK;
                 else if ((BSP_BUS_CLOCK / 4) <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_4 | LWADC_CFG_ADICLK_BUSCLK;
                 else if ((BSP_BUS_CLOCK / 8) <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_8 | LWADC_CFG_ADICLK_BUSCLK;
                 else if ((BSP_BUS_CLOCK / 16) <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_8 | LWADC_CFG_ADICLK_BUSCLK2;
                 else
                     return FALSE; /* cannot set ADC clock to be less than adc_max_frq */
             }
             else {
                 if ((BSP_BUS_CLOCK / (1 << init_ptr->CLOCK_DIV)) <= adc_max_frq)
                     adc_ptr->CFG = ADC_CFG_ADIV(init_ptr->CLOCK_DIV) | LWADC_CFG_ADICLK_BUSCLK;
                 else if ((BSP_BUS_CLOCK / 2 / (1 << init_ptr->CLOCK_DIV)) <= adc_max_frq)
                     adc_ptr->CFG = ADC_CFG_ADIV(init_ptr->CLOCK_DIV) | LWADC_CFG_ADICLK_BUSCLK2;
                 else
                     return FALSE; /* such clock combination is too fast for ADC */
             }
             break;

         case LWADC_CLK_BUSCLK:
             if (init_ptr->CLOCK_DIV == LWADC_DIV_ANY) {
                 if (BSP_BUS_CLOCK <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_1 | LWADC_CFG_ADICLK_BUSCLK;
                 else if ((BSP_BUS_CLOCK / 2) <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_2 | LWADC_CFG_ADICLK_BUSCLK;
                 else if ((BSP_BUS_CLOCK / 4) <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_4 | LWADC_CFG_ADICLK_BUSCLK;
                 else if ((BSP_BUS_CLOCK / 8) <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_8 | LWADC_CFG_ADICLK_BUSCLK;
                 else
                     return FALSE; /* cannot set ADC clock to be less than ADC max. frequency */
             }
             else if ((BSP_BUS_CLOCK / (1 << init_ptr->CLOCK_DIV)) <= adc_max_frq)
                 adc_ptr->CFG = ADC_CFG_ADIV(init_ptr->CLOCK_DIV) | LWADC_CFG_ADICLK_BUSCLK;
             else
                 return FALSE; /* such clock combination is too fast for ADC */
             break;

         case LWADC_CLK_BUSCLK2:
             if (init_ptr->CLOCK_DIV == LWADC_DIV_ANY) {
                 if ((BSP_BUS_CLOCK / 16) <= adc_max_frq)
                     adc_ptr->CFG = LWADC_CFG_ADIV_8 | LWADC_CFG_ADICLK_BUSCLK2;
                 if ((BSP_BUS_CLOCK / 16) <= adc_max_frq)
                     return FALSE; /* cannot set ADC clock to be less than ADC max. frequency */
             }
             else if ((BSP_BUS_CLOCK / 2 / (1 << init_ptr->CLOCK_DIV)) <= adc_max_frq)
                 adc_ptr->CFG = ADC_CFG_ADIV(init_ptr->CLOCK_DIV) | LWADC_CFG_ADICLK_BUSCLK2;
             else
                 return FALSE; /* such clock combination is too fast for ADC */
             break;

         case LWADC_CLK_ALTERNATE:
             if (init_ptr->CLOCK_DIV == LWADC_DIV_ANY)
                 return FALSE; /* alternate clock + any division is not supported now */
             adc_ptr->CFG = ADC_CFG_ADIV(init_ptr->CLOCK_DIV) | LWADC_CFG_ADICLK_ALTCLK;
             break;

         case LWADC_CLK_ASYNC:
             if (init_ptr->CLOCK_DIV == LWADC_DIV_ANY)
                 return FALSE; /* async clock + any division is not supported now */
             adc_ptr->CFG = ADC_CFG_ADIV(init_ptr->CLOCK_DIV) | LWADC_CFG_ADICLK_ADACK;
             break;

         default:
             return FALSE; /* invalid clock source */

     }
    adc_ptr->CFG |= ADC_CFG_ADLPC_MASK;
    adc_ptr->CFG |= ADC_CFG_MODE(0x02);
    adc_ptr->CFG |= LWADC_CFG_AVGS_32;
    adc_ptr->CFG &= ~ADC_CFG_ADTRG_MASK; /* set SW trigger */
    adc_ptr->GC |= ADC_GC_AVGE_MASK | ADC_GC_ADCO_MASK;
    context_ptr->init_ptr = init_ptr;

    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_init_input
 * Returned Value   : TRUE for success, FALSE for failure
 * Comments         :
 * This function prepares and initialize the LWADC_STRUCT
 * with all data it will need later for quick control of the input.
 * The structure initialized here is used in all subsequent calls to any
 * LWADC driver and is uniquely identifying the input.
 * This function calls a BSP supplied function to enable the ADC input pin.
 * It adds the input to the list of channels being converted, and place the
 * ADC in continuous conversion mode if not already in this mode.
 *
 *END*----------------------------------------------------------------------*/

bool _lwadc_init_input(LWADC_STRUCT_PTR lwadc_ptr, uint32_t input)
{
    ADC_MemMapPtr    adc_ptr;
    uint32_t          device, channel;

    if (NULL == lwadc_ptr){
        return FALSE;
    }

    device  = ADC_DEVICE(input);
    channel = ADC_CHANNEL(input);

    /* Make sure channel & device is in range */
    if ((device > ADC_NUM_DEVICES ) || (channel > ADC_HW_CHANNELS )) {
        return FALSE;
    }

    lwadc_ptr->context_ptr = &adc_context[device];

    adc_ptr = lwadc_ptr->context_ptr->adc_ptr;
    if (NULL == adc_ptr){
        return FALSE;
    }

    /* Store all channels in context ptr*/
    lwadc_ptr->context_ptr->channels |= (1 << channel);

    /* Enable channels port control */
    adc_ptr->PCTL = lwadc_ptr->context_ptr->channels;

    /* Enable ADC I/O Pin */
    if (_bsp_adc_channel_io_init(input) != IO_OK) {
        return FALSE;
    }

    /* set channel specific parameters */
    lwadc_ptr->input   = input;
    lwadc_ptr->numerator   = lwadc_ptr->context_ptr->default_numerator;
    lwadc_ptr->denominator = lwadc_ptr->context_ptr->default_denominator;

    /* Set channel to convert */
    adc_ptr->HC1 = channel;
    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_read_raw
 * Returned Value   : TRUE for success, FALSE for failure
 * Comments         :
 *    This function Read the current value of the ADC input and return
 *    the result without applying any scaling.
 *
 *END*----------------------------------------------------------------------*/

bool _lwadc_read_raw(LWADC_STRUCT_PTR lwadc_ptr, LWADC_VALUE_PTR outSample)
{
    ADC_MemMapPtr   adc_ptr;
    LWADC_VALUE     sample;

    if (NULL == lwadc_ptr){
        return FALSE;
    }

    if (lwadc_ptr->context_ptr == NULL) {
        return FALSE;
    }
    adc_ptr = lwadc_ptr->context_ptr->adc_ptr;

    if ((adc_ptr->GC & ADC_GC_ADCO_MASK) == 0) {
        /* ADC is not in continuous mode. */
        adc_ptr->GC |= ADC_GC_ADCO_MASK;
    }

    sample = adc_ptr->R1;

    *outSample = sample & ADC_CDR_CDATA_MASK;

    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_read
 * Returned Value   : TRUE for success, FALSE for failure
 * Comments         :
 *    This function Read the current value of the ADC input, applies any
 *    scaling, and return the result.
 *
 *END*----------------------------------------------------------------------*/

bool _lwadc_read(LWADC_STRUCT_PTR lwadc_ptr, LWADC_VALUE_PTR outSample)
{
    LWADC_VALUE rawSample;

    if (NULL == lwadc_ptr) {
        return FALSE;
    }
    if (lwadc_ptr->context_ptr == NULL) {
        return FALSE;
    }
    if (_lwadc_read_raw(lwadc_ptr,&rawSample)) {
        *outSample = ((rawSample*lwadc_ptr->numerator) + (lwadc_ptr->denominator>>1))/ lwadc_ptr->denominator;
        return TRUE;
    }

    *outSample = 0;
    return FALSE;
}


/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_wait_next
 * Returned Value   : TRUE for success, FALSE for failure
 * Comments         :
 *    This function Waits for a new value to be available on the specified ADC input.
 *
 *END*----------------------------------------------------------------------*/

bool _lwadc_wait_next(LWADC_STRUCT_PTR lwadc_ptr)
{
    ADC_MemMapPtr   adc_ptr;
    uint32_t         channel;
    uint32_t         input;

    if (NULL == lwadc_ptr){
        return FALSE;
    }

    if (lwadc_ptr->context_ptr == NULL) {
        return FALSE;
    }
    input = lwadc_ptr->input;
    channel = ADC_CHANNEL(input);
    adc_ptr = lwadc_ptr->context_ptr->adc_ptr;

    if ((adc_ptr->GC & ADC_GC_ADCO_MASK) == 0) {
        /* ADC is not in continuous mode. */
        adc_ptr->GC |= ADC_GC_ADCO_MASK;
    }

    /* Select channel for converting */
    adc_ptr->HC1 = channel;

    while((adc_ptr->HS & ADC_HS_COCO1_MASK) == 0){}
    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_read_average
 * Returned Value   : TRUE for success, FALSE for failure
 * Comments         :
 *    This function Read the current value of the ADC input, applies any
 *    scaling, and return the result.
 *
 *END*----------------------------------------------------------------------*/

bool _lwadc_read_average(LWADC_STRUCT_PTR lwadc_ptr, uint32_t num_samples, LWADC_VALUE_PTR outSample)
{
    LWADC_VALUE     rawSample,sum = 0;
    uint32_t         i;

    if (num_samples > (MAX_UINT_32/LWADC_RESOLUTION_DEFAULT)) {
        return FALSE;
    }

    for (i=0;i<num_samples;i++) {
        if (!_lwadc_wait_next(lwadc_ptr)) {
            return FALSE;
        }

        if (!_lwadc_read_raw(lwadc_ptr,&rawSample)) {
            return FALSE;
        }
        sum += rawSample;
    }

    *outSample = (((sum/num_samples)*lwadc_ptr->numerator) + (lwadc_ptr->denominator>>1))/ lwadc_ptr->denominator;
    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_set_attribute
 * Returned Value   : TRUE for success, FALSE for failure
 * Comments         :
 * This function sets attributes for the specified ADC input. Attributes
 * could include single/differential mode, reference, scaling numerator or
 * denominator, etc.
 *
 *END*----------------------------------------------------------------------*/

bool _lwadc_set_attribute(LWADC_STRUCT_PTR lwadc_ptr, LWADC_ATTRIBUTE attribute,  uint32_t value)
{
    bool            result = TRUE;
    ADC_MemMapPtr   adc_ptr;
    uint32_t        channel, input;

    if (NULL == lwadc_ptr){
        return FALSE;
    }
    if (lwadc_ptr->context_ptr == NULL) {
        return FALSE;
    }
    input = lwadc_ptr->input;
    channel = ADC_CHANNEL(input);
    adc_ptr = lwadc_ptr->context_ptr->adc_ptr;

    switch (attribute) {
        case LWADC_NUMERATOR:
            lwadc_ptr->numerator = value;
            break;

        case LWADC_DENOMINATOR:
            lwadc_ptr->denominator = value;
            break;

        case LWADC_DEFAULT_NUMERATOR:
            lwadc_ptr->context_ptr->default_numerator = value;
            break;

        case LWADC_DEFAULT_DENOMINATOR:
            lwadc_ptr->context_ptr->default_denominator = value;
            break;

        case LWADC_FREQUENCY:
            /* We don't support changing frequency when ADC module was initialized */
            result = FALSE;
            break;

        case LWADC_DIVIDER:
            /* We support a divider, as long as it is 1. */
            result = (value == 1)?TRUE:FALSE;
            break;

        case LWADC_DIFFERENTIAL:
            /* We don't support differential, so return FALSE if asked for it, TRUE otherwise */
            result = !value;
            break;

        case LWADC_INPUT_CONVERSION_ENABLE:
            if (value) {
                adc_ptr->HC1 = channel;
            } else {
                adc_ptr->HC1 = ADC_HC1_ADCH1_MASK;
            }
            break;

        default:
            return FALSE;
    }
    return result;
}


/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _lwadc_get_attribute
 * Returned Value   : TRUE for success, FALSE for failure
 * Comments         :
 *    This function This function gets attributes for the specified ADC input,
 *    or for the ADC module as a whole. Attributes could include
 *    single/differential mode, reference, scaling numerator or denominator, etc.
 *
 *END*----------------------------------------------------------------------*/

bool _lwadc_get_attribute(LWADC_STRUCT_PTR lwadc_ptr, LWADC_ATTRIBUTE attribute,  uint32_t *value_ptr)
{
    ADC_MemMapPtr   adc_ptr;

    if (NULL == lwadc_ptr){
        return FALSE;
    }
    if (lwadc_ptr->context_ptr == NULL) {
        return FALSE;
    }
    adc_ptr = lwadc_ptr->context_ptr->adc_ptr;

    if (value_ptr == NULL) {
        return FALSE;
    }

    switch (attribute) {
        case LWADC_NUMERATOR:
            *value_ptr = lwadc_ptr->numerator;
            break;

        case LWADC_DENOMINATOR:
            *value_ptr = lwadc_ptr->denominator;
            break;

        case LWADC_DEFAULT_NUMERATOR:
            *value_ptr = lwadc_ptr->context_ptr->default_numerator;
            break;

        case LWADC_DEFAULT_DENOMINATOR:
            *value_ptr = lwadc_ptr->context_ptr->default_denominator;
            break;

        case LWADC_POWER_DOWN:
            *value_ptr = (adc_ptr->CFG&ADC_CFG_ADLPC_MASK)?TRUE:FALSE;
            break;

        case LWADC_RESOLUTION:
            *value_ptr = LWADC_RESOLUTION_DEFAULT;
            break;

        case LWADC_REFERENCE:
            *value_ptr = lwadc_ptr->context_ptr->init_ptr->REFERENCE;
            break;

        case LWADC_FREQUENCY:
            *value_ptr = adc_max_frq;
            break;

        case LWADC_DIVIDER:
            *value_ptr = 1;
            break;

        case LWADC_DIFFERENTIAL:
            *value_ptr = FALSE;
            break;

        case LWADC_FORMAT:
            *value_ptr = FALSE;
            break;

        case LWADC_INPUT_CONVERSION_ENABLE:
            *value_ptr = (ADC_HC1_ADCH1_MASK == adc_ptr->HC1)?FALSE:TRUE;
            break;

        default:
            return FALSE;
    }
    return TRUE;
}
