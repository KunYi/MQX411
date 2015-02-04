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
*   This file contains the initialization definition for SDCARD driver
*
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

/*
** parameters of sdcard0 initialization
*/

const SDCARD_INIT_STRUCT _bsp_sdcard0_init = { 
    _io_sdcard_esdhc_init,
    _io_sdcard_esdhc_read_block,
    _io_sdcard_esdhc_write_block,
    ESDHC_BUS_WIDTH_4BIT
};
