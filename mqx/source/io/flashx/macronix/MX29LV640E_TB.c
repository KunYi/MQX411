/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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
*   The file contains the block definitions for the
*   flash device.
*
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "flashx.h"

#define MX29LV640E_LARGE_SECTOR_SIZE (0x8000)
#define MX29LV640E_NUM_LARGE_SECTORS (127)
#define MX29LV640E_SMALL_SECTOR_SIZE (0x1000)
#define MX29LV640E_NUM_SMALL_SECTORS (8)


const FLASHX_BLOCK_INFO_STRUCT _mx29lv640e_block_map_16bit[] =  { 
    { MX29LV640E_NUM_LARGE_SECTORS, 0, MX29LV640E_LARGE_SECTOR_SIZE * 2 },
    { MX29LV640E_NUM_SMALL_SECTORS, MX29LV640E_NUM_LARGE_SECTORS * MX29LV640E_LARGE_SECTOR_SIZE * 2, MX29LV640E_SMALL_SECTOR_SIZE * 2 },
    { 0, 0, 0 }
};
