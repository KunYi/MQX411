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
*   The file contains the block definitions for the AM29LV800BT
*   flash device.
*
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "flashx.h"

/* Reference sector size */
#define AMD29LV800BT_SECTOR_SIZE     (0x4000 * BSP_FLASH_DEVICES)

#define AMD29LV800BT_NUM_SECTORS_0 (15) /* block 0 */
#define AMD29LV800BT_NUM_SECTORS_1 (1)  /* block 1 */
#define AMD29LV800BT_NUM_SECTORS_2 (2)  /* block 2 */
#define AMD29LV800BT_NUM_SECTORS_3 (1)  /* block 3 */

const FLASHX_BLOCK_INFO_STRUCT _29lv800bt_block_map_16bit[] =  {
    /* sectors per block        , offset                     ,  sector size */
    { AMD29LV800BT_NUM_SECTORS_0, (0),                          (4*AMD29LV800BT_SECTOR_SIZE)},/* 64K */
    { AMD29LV800BT_NUM_SECTORS_1, (0xF0000*BSP_FLASH_DEVICES),  (2*AMD29LV800BT_SECTOR_SIZE)},/* 32K */
    { AMD29LV800BT_NUM_SECTORS_2, (0xF8000*BSP_FLASH_DEVICES),  (AMD29LV800BT_SECTOR_SIZE/2)},/* 8K  */
    { AMD29LV800BT_NUM_SECTORS_3, (0xFC000*BSP_FLASH_DEVICES),  (AMD29LV800BT_SECTOR_SIZE)},  /* 16K */
    { 0, 0, 0 }
};

/* EOF*/
