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
*   This file contains the default initialization record for the NAND
*   flash.
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "nfc.h"

extern NANDFLASH_INFO_STRUCT _MT29F2G16_organization_16bit[];

const NANDFLASH_INIT_STRUCT _bsp_nandflash_init =
{
   /* NAME                 */   "nandflash:",
   /* INIT                 */   nfc_init,
   /* DEINIT               */   nfc_deinit,
   /* CHIP_ERASE           */   nfc_erase_flash,
   /* BLOCK_ERASE          */   nfc_erase_block,
   /* PAGE_READ            */   nfc_read_page,
   /* PAGE_PROGRAM         */   nfc_write_page,
   /* WRITE_PROTECT        */   NULL,
   /* IS_BLOCK_BAD         */   nfc_check_block,
   /* MARK_BLOCK_AS_BAD    */   nfc_mark_block_as_bad,
   /* IOCTL                */   nfc_ioctl,
   /* NANDFLASH_INFO_PTR   */   _MT29F2G16_organization_16bit,
   /* VIRTUAL_PAGE_SIZE    */   0,
   /* NUM_VIRTUAL_PAGES    */   0,
   /* PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO */   
                                0,
   /* ECC_SIZE             */   0,
   /* WRITE_VERIFY         */   0,
   /* DEVICE_SPECIFIC_DATA */   0
};

/* EOF */
