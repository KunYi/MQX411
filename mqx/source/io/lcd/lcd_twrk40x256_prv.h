/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains the private LCD driver definitions and types
*
*
*END************************************************************************/

#ifndef __LCD_TWRK40X256_PRV_H
    #define __LCD_TWRK40X256_PRV_H

/* 
** Map segment to COM mask 
*/
#define  SEGA   0x01
#define  SEGB   0x02
#define  SEGC   0x04
#define  SEGD   0x08

#define  SEGE   0x04
#define  SEGF   0x01
#define  SEGG   0x02

#define SEG_AD_MASK  (SEGA | SEGB | SEGC | SEGD)
#define SEG_EG_MASK  (SEGE | SEGF | SEGG)

/* 
** definition of segment masks for specific character
*/  
/*           segment A-D drive pin      segment E-G drive pin mask */
#define  _0 ( SEGA| SEGB| SEGC| SEGD) , ( SEGE| SEGF )
#define  _1 ( SEGB| SEGC )            , ( 0x00 )
#define  _2 ( SEGA| SEGB| SEGD )      , ( SEGG| SEGE )
#define  _3 ( SEGA| SEGB| SEGC| SEGD) , ( SEGG )
#define  _4 ( SEGB| SEGC )            , ( SEGF | SEGG )
#define  _5 ( SEGA| SEGC| SEGD )      , ( SEGF| SEGG )
#define  _6 ( SEGA| SEGC| SEGD )      , ( SEGF| SEGE| SEGG )
#define  _7 ( SEGA| SEGB| SEGC )      , ( 0x00 )
#define  _8 ( SEGA| SEGB| SEGC| SEGD) , ( SEGE| SEGF| SEGG )
#define  _9 ( SEGA| SEGB| SEGC| SEGD) , ( SEGF| SEGG )
#define  _A ( SEGA| SEGB| SEGC )      , ( SEGE| SEGF| SEGG )
#define  _B ( SEGC| SEGD )            , ( SEGE| SEGF| SEGG )
#define  _C ( SEGA| SEGD )            , ( SEGE| SEGF )
#define  _D ( SEGB| SEGC| SEGD )      , ( SEGE| SEGG )
#define  _E ( SEGA| SEGD )            , ( SEGE| SEGF| SEGG )
#define  _F ( SEGA )                  , ( SEGE| SEGF| SEGG )
#define  _END_OF_LIST   31

#define AD_POS 		0
#define EG_POS 		1

#define DIGITS_NUM	4

typedef struct lcd_symbol_struct
{
   uint32_t volatile *   WF_REG;
   uint32_t  MASK;
} LCD_SYMBOL_STRUCT, * LCD_SYMBOL_STRUCT_PTR;


typedef struct lcd_pin_reg_struct
{
   uint32_t volatile *  WF_REG;
   uint32_t  SHIFT;
} LCD_PIN_REG_STRUCT, * LCD_PIN_REG_STRUCT_PTR;

#endif /* __LCD_TWRK40X256_PRV_H */
