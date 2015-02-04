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
*   This file contains the TWRPI-SLCD display driver functions
*
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <bsp.h>  


#include "lcd_twrk40d100m_prv.h"


#define ASCII_0_OFFSET  48
#define ASCII_A_OFFSET  55
#define ASCII_SMALL_A_OFFSET  87
/* this macro get index number 0-15 according to ASCII 0 - F chars */
#define ASCII_OFFSET( ascii ) ( ascii >= 'a' ? ASCII_SMALL_A_OFFSET : ascii >= 'A' ? ASCII_A_OFFSET : ASCII_0_OFFSET)
 
static const char aAscii [ ] =
{
    _0,
    _1,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,
    _9,
    _A,
    _B,
    _C,
    _D,
    _E,
    _F, 
};

static const LCD_SYMBOL_STRUCT lcd_symbol_table[] =
{
    /* LCD_FREESCALE */ { (uint32_t volatile *)&LCD_WF15TO12, 0x08000000 },
    /* LCD_AM        */ { (uint32_t volatile *)&LCD_WF23TO20, 0x00000400 },
    /* LCD_PM        */ { (uint32_t volatile *)&LCD_WF23TO20, 0x00000800 },
    /* LCD_DEGREES   */ { (uint32_t volatile *)&LCD_WF23TO20, 0x00000100 },   
    /* LCD_PERCENT   */ { (uint32_t volatile *)&LCD_WF23TO20, 0x00000200 },   
    /* LCD_COLON     */ { (uint32_t volatile *)&LCD_WF15TO12, 0x00000800 },
    /* LCD_ONE       */ { (uint32_t volatile *)&LCD_WF23TO20, 0x00080000 }, 
};

const LCD_PIN_REG_STRUCT digit1_ad =
{
    (uint32_t volatile *)&LCD_WF15TO12,
    LCD_WF_WF14_SHIFT
};

const LCD_PIN_REG_STRUCT digit1_eg =
{
    (uint32_t volatile *)&LCD_WF15TO12,
    LCD_WF_WF15_SHIFT
};

const LCD_PIN_REG_STRUCT digit2_ad =
{
    (uint32_t volatile *)&LCD_WF15TO12,
    LCD_WF_WF12_SHIFT   
};

const LCD_PIN_REG_STRUCT digit2_eg =
{
    (uint32_t volatile *)&LCD_WF15TO12,
    LCD_WF_WF13_SHIFT   
};

const LCD_PIN_REG_STRUCT digit3_ad =
{
    (uint32_t volatile *)&LCD_WF23TO20,
    LCD_WF_WF20_SHIFT 
};

const LCD_PIN_REG_STRUCT digit3_eg =
{
    (uint32_t volatile *)&LCD_WF23TO20,
    LCD_WF_WF22_SHIFT    
};

const LCD_PIN_REG_STRUCT_PTR   digit_io_map[3][2] = 
{
    (LCD_PIN_REG_STRUCT_PTR)&digit3_ad, (LCD_PIN_REG_STRUCT_PTR)&digit3_eg,
    (LCD_PIN_REG_STRUCT_PTR)&digit2_ad, (LCD_PIN_REG_STRUCT_PTR)&digit2_eg,
    (LCD_PIN_REG_STRUCT_PTR)&digit1_ad, (LCD_PIN_REG_STRUCT_PTR)&digit1_eg
};


/*FUNCTION****************************************************************
*                             
* Function Name    : _lcd_init
* Returned Value   : MQX error code
* Comments         :
*    This function (re)initializes/clears/enables LCD module.
*
*END*********************************************************************/
int32_t _lcd_init(void)                                                                                                          
{     
#if PE_LDD_VERSION
    if (PE_PeripheralUsed((uint32_t)LCD_BASE_PTR))
    {
        /* IO Device used by PE Component*/
        return IO_ERROR;
    }
#endif    
    
    /* enable internal oscillator */
    MCG_C1 |= MCG_C1_IRCLKEN_MASK;
    
    /* 1. GCR
    ** a. Configure LCD clock source (SOURCE bit).
    ** b. Select 1.0 V or 1.67 V for 3 or 5 V glass (HREFSEL).
    ** c. Enable regulated voltage (RVEN).
    ** d. Trim the regulated voltage (RVTRIM).
    ** e. Enable charge pump (CPSEL bit).
    ** f. Configure LCD controller for doubler or tripler mode (HREFSEL bit).
    ** g. Configure charge pump clock (LADJ[1:0]).
    ** h. Configure LCD power supply (VSUPPLY[1:0]).
    ** i. Configure LCD frame frequency interrupt (LCDIEN bit).
    ** j. Configure LCD behavior in low power mode (LCDWAIT and LCDSTP bits).
    ** k. Configure LCD duty cycle (DUTY[2:0]).
    ** l. Select and configure LCD frame frequency (LCLK[2:0]).  
    */
    LCD_GCR |= LCD_GCR_VSUPPLY(1);
    LCD_GCR |= LCD_GCR_CPSEL_MASK;
    LCD_GCR |= LCD_GCR_DUTY(3);
    LCD_GCR |= LCD_GCR_SOURCE_MASK; /* select alt clock IRC */
    LCD_GCR |= LCD_GCR_LCLK(4); 
    
    /* 2. AR  
    ** a. Configure display mode (ALT and BLANK bits).
    ** b. Configure blink mode (BMODE).
    ** c. Configure blink frequency (BRATE[2:0]).
    */
    
    /* 3. PENn
    ** a. Enable LCD controller pins (PEN[63:0] bits).
    ** enable LCD pins 0, 1, 2, 3, 12, 13 14 15 20 21 22   
    */
    LCD_PENL = 0x0070F00F;      
      
    /* 4. BPENn
    ** a. Enable LCD pins to operate as an LCD backplane (BPEN[63:0]).
    */
    LCD_BPENL = 0x0000000F;
    
    /* 5. WFyTOx
    ** a. Initialize WFyTOx registers with backplane configuration and an initial 
    ** display screen.
    */
    LCD_WF3TO0 = 0x08040201;
    
    LCD_WF15TO12 = 0x0f0f0f0f;
    LCD_WF23TO20 = 0x000f0f0f;
    
    /* 6. GCR
    ** a. Enable LCD controller (LCDEN bit).
    */
    LCD_GCR |= LCD_GCR_LCDEN_MASK; 
    
    return MQX_OK;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _lcd_segments
* Returned Value   : none
* Comments         :
*   Function controls graphical symbols on Freescale TWRPI-SLCD display.
*   If (state = TRUE)  all segments on display are turned on
*   else if (state = FALSE) all segments on display are turned off
*
*END**********************************************************************/
void _lcd_segments(bool state) 
{

    eLCD_Symbols  symbol = LCD_FREESCALE;
    
    /* special symbols */
    while( symbol != LCD_LAST_SYMBOL )
    {
        _lcd_symbol( symbol++, state);  
    }
    
    /* digits */
    _lcd_puts( state == TRUE ? "1888" : "xxxx" );         
}

/*FUNCTION****************************************************************
* 
* Function Name    : _lcd_symbol
* Returned Value   : MQX_OK if success, -1 if symbol doesn't exists
* Comments         :
*    Functin controls graphical symbols on Freescale TWRPI-SLCD display.
*
*END**********************************************************************/
int32_t _lcd_symbol(eLCD_Symbols symbol_index, bool state)
{                                                         
    /* check if requested symbol is in table */
    if (  (symbol_index >= LCD_FREESCALE)
        &&(symbol_index <  LCD_LAST_SYMBOL))
    { 
        /* turn symbol ON */
        if (state == TRUE)
        {
            *lcd_symbol_table[symbol_index].WF_REG |= lcd_symbol_table[symbol_index].MASK;  
        }
        /* turn symbol OFF */
        else
        {
            *lcd_symbol_table[symbol_index].WF_REG &= ~lcd_symbol_table[symbol_index].MASK;
        }
        
        return MQX_OK;
    }
    else
    {
        /* LCD symbol index does out of range */
        return -1;
    }    
}

/*FUNCTION****************************************************************
* 
* Function Name    : _lcd_puts
* Returned Value   : none
* Comments         : The _lcd_puts() writes string pointed by pString
*                    to LCD display
*
*END*********************************************************************/
void _lcd_puts
(
    /* [IN] the string to be printed out. */
    const char * pString
)
{
    _mqx_int possition = 0;
    LCD_PIN_REG_STRUCT_PTR   pin_ptr;
    unsigned char    pos;
    _mqx_int len;
    
    len = strlen(pString);
    
    /* string overflow protection */
    if( len > DIGITS_NUM )
    {  
        len = DIGITS_NUM;
    }
    
    /* most left symbol can be only 1, driven by one bit */
    if( len == 4 )
    {  
        if( pString[possition++] == '1')
        {
            _lcd_symbol(LCD_ONE, TRUE);    
        }
        else 
        {
            _lcd_symbol(LCD_ONE, FALSE); 
        }
        
        len --;
    }
    
    while( len-- )
    {
        pin_ptr = digit_io_map[len][AD_POS];
        
        pos = (_mqx_int)(pString[possition]-ASCII_OFFSET(pString[possition]))*2;
        possition++;
        
        /* clear segments */
        *pin_ptr->WF_REG &= ~(SEG_AD_MASK << pin_ptr->SHIFT);             
        /* write A-D segment */
        if( pos < _END_OF_LIST )
        {
            *(pin_ptr->WF_REG) |= aAscii[pos] << pin_ptr->SHIFT;
        }
        
        pin_ptr = digit_io_map[len][EG_POS];
        /* clear segment */                          
        *pin_ptr->WF_REG &= ~(SEG_EG_MASK << pin_ptr->SHIFT);
        /* write E-G segment */
        if( pos < _END_OF_LIST )
        {
            *pin_ptr->WF_REG |= aAscii[pos+1] << pin_ptr->SHIFT;  
        }
    }     
}
/* EOF */

