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
*   CPU specific LWADC driver header file
*
*
*END************************************************************************/

#ifndef __lwadc_vadc_prv_h__
#define __lwadc_vadc_prv_h__

#ifdef __cplusplus
extern "C" {
#endif


#define LWADC_CFG_AVGS_4                ADC_CFG_AVGS(0x00)
#define LWADC_CFG_AVGS_8                ADC_CFG_AVGS(0x01)
#define LWADC_CFG_AVGS_16               ADC_CFG_AVGS(0x02)
#define LWADC_CFG_AVGS_32               ADC_CFG_AVGS(0x03)
#define LWADC_CFG_AVGE                  ADC_CFG_AVGS(0x04)
#define LWADC_CFG_ADCO                  ADC_CFG_AVGS(0x08)
#define LWADC_CFG_CALF                  ADC_CFG_AVGS(0x40)
#define LWADC_CFG_CAL                   ADC_CFG_AVGS(0x80)

#define LWADC_CFG_ADIV_1                ADC_CFG_ADIV(0x00)
#define LWADC_CFG_ADIV_2                ADC_CFG_ADIV(0x01)
#define LWADC_CFG_ADIV_4                ADC_CFG_ADIV(0x02)
#define LWADC_CFG_ADIV_8                ADC_CFG_ADIV(0x03)

#define LWADC_CFG_ADICLK_BUSCLK         ADC_CFG_ADICLK(0x00)
#define LWADC_CFG_ADICLK_BUSCLK2        ADC_CFG_ADICLK(0x01)
#define LWADC_CFG_ADICLK_ALTCLK         ADC_CFG_ADICLK(0x02)
#define LWADC_CFG_ADICLK_ADACK          ADC_CFG_ADICLK(0x03)


extern void    *_bsp_get_adc_base_address(uint8_t);


#ifdef __cplusplus
}
#endif

#endif
