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

/* Flash device series definition to be used by istrataprv.h */
#define I28F160_SECTOR_SIZE     (0x2000) /* 4KW */
#define I28F160_NUM_SECTORS_1   (8)
#define I28F160_NUM_SECTORS_2   (31)

/* Bottom Boot sector map */
const FLASHX_BLOCK_INFO_STRUCT _i28f160bot_block_map_16bit[] =  {
    { I28F160_NUM_SECTORS_1, (0),       I28F160_SECTOR_SIZE},     /* 4KW sectors */
    { I28F160_NUM_SECTORS_2, (0x10000), ( 8 * I28F160_SECTOR_SIZE)}, /* 32KW sectors */
    { 0, 0, 0 }
};

/* Top Boot sector map */
const FLASHX_BLOCK_INFO_STRUCT _i28f160top_block_map_16bit[] =  {
    { I28F160_NUM_SECTORS_2, (0),        ( 8 * I28F160_SECTOR_SIZE)},  /* 32KW sectors */
    { I28F160_NUM_SECTORS_1, (0x1F0000), (I28F160_SECTOR_SIZE)},    /* 4KW sectors */
{ 0, 0, 0 }
};

/* EOF*/

/* EOF*/
