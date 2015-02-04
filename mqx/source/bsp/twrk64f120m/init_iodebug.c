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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the initialization definition for debug driver
*
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"


#if defined(__CC_ARM)
/* ITM - KEIL default*/
const IODEBUG_INIT_STRUCT _bsp_iodebug_init = { 
    IODEBUG_MODE_ITM,       /* Driver mode */ 
    0,                      /* Length of buffered data */ 
    '\n'                    /* Default flush character */
};


#else
/* SEMIHOST - CW, IAR */
const IODEBUG_INIT_STRUCT _bsp_iodebug_init = { 
    IODEBUG_MODE_SEMIHOST,  /* Driver mode */ 
    127,                    /* Length of buffered data */ 
    '\n'                    /* Default flush character */
};
#endif 

/*
** CW : If semihost used, data length should be less/eq than 1023
** IAR: If semihost used, data length should be less/eq than 127
*/
