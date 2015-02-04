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
#include "nandflash_wl.h"
#include "nfc_wl.h"

extern const NANDFLASH_INIT_STRUCT _bsp_nandflash_init;

const NANDFLASH_WL_INIT_STRUCT _bsp_nandflash_wl_init =
{
   &_bsp_nandflash_init,
   nfc_read_phy_page_raw,       /* PHY_PAGE_READ_RAW */
   nfc_write_phy_page_raw,      /* PHY_PAGE_WRITE_RAW */
   nfc_read_page_metadata,      /* PAGE_READ_METADATA */
   nfc_read_page_with_metadata, /* PAGE_READ_WITH_METADATA */
   nfc_write_page_with_metadata /* PAGE_WRITE_WITH_METADATA */
};

/* EOF */
