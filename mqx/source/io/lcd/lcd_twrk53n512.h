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
* Freescale TWRPI-SLCD display public types and function prototypes
*
*
*END************************************************************************/

#ifndef __LCD_TWRK53N512_H
    #define __LCD_TWRK53N512_H


/**********************************************************************
**
** Enumeration type for symbols of Freescale TWRPI-SLCD display.
**
*/                                                                                                                                

typedef enum  
{
    LCD_FREESCALE,
    LCD_AM,
    LCD_PM,
    LCD_DEGREES,
    LCD_PERCENT,
    LCD_COL1,
    LCD_ONE,
    LCD_LAST_SYMBOL
}  eLCD_Symbols;

/********************************************************************** 
**
** Public function prototypes
**
*/ 

extern int32_t _lcd_init(void);
extern void   _lcd_segments(bool state); 
extern int32_t _lcd_symbol(eLCD_Symbols symbol_index, bool state);
extern void   _lcd_puts(const char * pString);

#endif /* __LCD_TWRK53N512_H */
