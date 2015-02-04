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
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the global settings for DCU4 driver.
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"

const DCU4_INIT_STRUCT _bsp_dcu0_init = {
    0,                              /* DCU4 device number */
    BSP_CLOCK_SRC * 22 / 21 * 18,   /* DCU4 clock source PLL1 PFD2 */
    480,                            /* default display width */
    272,                            /* default display height */
    3                               /* interrupt priority */
};
