/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
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
*   This file contains the default initialization record for the
*   flash device.
*
*
*END************************************************************************/
#ifndef __flash_mpxs30_h__
#define __flash_mpxs30_h__


#include "flashx.h"

#define MPXS30_CFLASH_LAS_SIZE      (256*1024)
#define MPXS30_CFLASH_MAS_SIZE      (256*1024)
#define MPXS30_CFLASH_HAS_SIZE      (512*1024)
#define MPXS30_CFLASH_SHADOW_SIZE   (16*1024)
#define MPXS30_DFLASH_LAS_SIZE      (64*1024)

#define MPXS30_CFLASH0_LAS_ADDR     (0x000000)
#define MPXS30_CFLASH1_LAS_ADDR     (0x040000)
#define MPXS30_CFLASH_MAS_ADDR      (0x080000)
#define MPXS30_CFLASH_HAS_ADDR      (0x100000)
#define MPXS30_CFLASH0_SHADOW_ADDR  (0x200000)
#define MPXS30_CFLASH1_SHADOW_ADDR  (0x280000)

/*----------------------------------------------------------------------*/
/*
**              DEFINED VARIABLES
*/

extern const FLASHX_BLOCK_INFO_STRUCT _mpxs30_data_flash_block_map[];
extern const FLASHX_BLOCK_INFO_STRUCT _mpxs30_code_flash_block_map[];

extern const FLASHX_DEVICE_IF_STRUCT _flashx_c90i_code_if;

#endif
