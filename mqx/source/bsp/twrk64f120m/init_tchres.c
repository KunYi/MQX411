/*HEADER**********************************************************************
*
* Copyright 2014 Freescale Semiconductor, Inc.
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
*   This file contains the definition for the baud rate for the TCHRES
*   channel
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"

#define FULL_SCALE  ((1 << 12) - 1) //12-bit ADC
#define IO_PIN_FUNC  1
#define ADC_FUNC     0

/* Initialization structure */
const TCHRES_INIT_STRUCT _bsp_tchscr_resisitve_init = {
    /* structure with pins connected to touch_screen detection */
    {
      BSP_LCD_TCHRES_X_PLUS,       /* x + gpio pin definition */
      BSP_LCD_TCHRES_X_MINUS,      /* x - gpio pin definition */
      BSP_LCD_TCHRES_Y_PLUS,       /* y + gpio pin definition */
      BSP_LCD_TCHRES_Y_MINUS,      /* y - gpio pin definition */
      {
         IO_PIN_FUNC,                 /* x + gpio mux index */
         ADC_FUNC,                    /* x + adc mux index */
         IO_PIN_FUNC,                 /* y + gpio mux index */
         ADC_FUNC,                    /* y + adc mux index */
         IO_PIN_FUNC,                 /* x - gpio mux index */
         IO_PIN_FUNC,                 /* y - gpio mux index */
      },
   },

   /* Adc limits struct */
   {
        FULL_SCALE,             /* FULL_SCALE */
        FULL_SCALE / 8,         /* X_TOUCH_MIN */
        FULL_SCALE / 8,         /* Y_TOUCH_MIN  */
       (FULL_SCALE / 8) * 7,    /* X_TOUCH_OFFSET_MAX */
       (FULL_SCALE / 8) * 7,    /* Y_TOUCH_OFFSET_MAX */
   },

   /* Touch screen adc channel X setting structure */
    BSP_LCD_X_PLUS_ADC_CHANNEL,  /* physical ADC channel */
    BSP_TCHRES_X_TRIGGER,        /* logical trigger ID that starts this ADC channel */

   /* Touch screen adc channel Y setting structure */
    BSP_LCD_Y_PLUS_ADC_CHANNEL,  /* physical ADC channel */
    BSP_TCHRES_Y_TRIGGER,        /* logical trigger ID that starts this ADC channel */
};
/* EOF */
