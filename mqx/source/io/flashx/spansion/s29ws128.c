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

#define S28WS128_SECTOR_SIZE_32K    (0x8000)
#define S28WS128_NUM_SECTORS_1      (4)
#define S28WS128_SECTOR_SIZE_128K   (0x20000)
#define S28WS128_NUM_SECTORS_2      (126)
#define S28WS128_NUM_SECTORS_3      (4)
#define S28WS128_BANK0_START        (0)

#define S28WS128_BANK1_START        (S28WS128_BANK0_START + S28WS128_NUM_SECTORS_1 * S28WS128_SECTOR_SIZE_32K)
#define S28WS128_BANK2_START        (S28WS128_BANK1_START + S28WS128_NUM_SECTORS_2 * S28WS128_SECTOR_SIZE_128K)

const FLASHX_BLOCK_INFO_STRUCT _S28WS128_block_map_16bit[] =  {
    { S28WS128_NUM_SECTORS_1, S28WS128_BANK0_START, S28WS128_SECTOR_SIZE_32K},
    { S28WS128_NUM_SECTORS_2, S28WS128_BANK1_START, S28WS128_SECTOR_SIZE_128K},
    { S28WS128_NUM_SECTORS_3, S28WS128_BANK2_START, S28WS128_SECTOR_SIZE_32K},
    { 0, 0, 0 }
};

/* EOF*/
