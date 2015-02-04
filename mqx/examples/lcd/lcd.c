/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 1989-2008 ARC International
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
*   This file contains the source for the hello example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h> 
#include <fio.h>
#include <string.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


/* Task IDs */
#define LCD_TASK 5

extern void lcd_task(uint32_t);


const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{ 
   /* Task Index, Function,   Stack,  Priority, Name,       Attributes,          Param, Time Slice */
    { LCD_TASK,   lcd_task,   1500,   8,        "LCD Task", MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};

char lcd_bufer[10];


/*TASK********************************************************************
* 
* Function Name    : lcd_task
* Returned Value   : void
* Comments         :
*   Demonstrate LCD driver features.
*
*END*********************************************************************/


void lcd_task
   (
      uint32_t initial_data
   )
{

    eLCD_Symbols lcd_symbol;
    uint16_t      delay = 500; /* 500ms delay */
    int16_t       i;

    (void)_lcd_init();


    while (1) 
    {
        /* Turn all segments on display on */        
        _lcd_segments(TRUE);
        
        _time_delay(delay);        
        
        /* Turn all segments on display off */                
        _lcd_segments(FALSE);


        /* display all special symbols on display */
        for (lcd_symbol = LCD_FREESCALE; lcd_symbol < LCD_LAST_SYMBOL; lcd_symbol++)
        {
            (void)_lcd_symbol(lcd_symbol, TRUE);
            _time_delay(delay / 10);
        } 

        for (lcd_symbol = LCD_FREESCALE; lcd_symbol < LCD_LAST_SYMBOL; lcd_symbol++)
        {
           (void)_lcd_symbol(lcd_symbol, FALSE);
           _time_delay(delay / 10);
        }

#ifdef LCD_DT7
        _lcd_symbol(LCD_DT7, FALSE);
#endif
        /* Display text messages */
#if defined (BSP_M51EMDEMO)
        _lcd_home();
#endif
        _lcd_puts("Hallo");
        _time_delay(delay);    

#if defined (BSP_M51EMDEMO)
        _lcd_home();
#endif
        _lcd_puts("World");
        _time_delay(delay);    
        
        /* Display numbers */
#ifdef LCD_DT4
        _lcd_symbol(LCD_DT4, TRUE);
#endif
        i = -100;
        while (i < 100)
        {
            sprintf(lcd_bufer, "%04d", i++);
#if defined (BSP_M51EMDEMO)
            _lcd_home();
#endif
            _lcd_puts(lcd_bufer);
            _time_delay(delay / 10);
        }
#ifdef LCD_DT4
        _lcd_symbol(LCD_DT4, FALSE);        
#endif
        /* Display special characters */
#if defined (BSP_M51EMDEMO)
        _lcd_home();
#endif
        _lcd_puts("+-/\\>=<");
        _time_delay(delay * 4); 


        /* Turn segments on display off */        
        _lcd_segments(FALSE);
        _time_delay(delay);    
    }
   
    _task_block();

}

/* EOF */
