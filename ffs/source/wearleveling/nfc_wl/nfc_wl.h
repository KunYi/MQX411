#ifndef _nfc_wl_h_
#define _nfc_wl_h_
/*HEADER**********************************************************************
*
* Copyright 2010 Freescale Semiconductor, Inc.
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
*   This file contains defines and functions prototype for the  
*   NAND Flash aController (NFC) low level driver.
*
*
*END************************************************************************/

#include "nandflash_wl.h"

/* NFC ECC status word */
#define NFC_ECC_STATUS_CORFAIL       (0x80)
#define NFC_ECC_STATUS_ERROR_COUNT   (0x3F)
#define NFC_ECC_STATUS_OFFSET        (0x0C)

/*----------------------------------------------------------------------*/
/*
**                     FUNCTION PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif
    extern uint32_t nfc_wl_init( IO_NANDFLASH_STRUCT_PTR );
    extern uint32_t nfc_read_page_with_metadata( IO_NANDFLASH_WL_STRUCT_PTR, unsigned char *, unsigned char *,  uint32_t, uint32_t);
    extern uint32_t nfc_read_page_metadata (  IO_NANDFLASH_WL_STRUCT_PTR , unsigned char *, uint32_t, uint32_t);
    extern uint32_t nfc_write_page_with_metadata( IO_NANDFLASH_WL_STRUCT_PTR,  unsigned char *, unsigned char *, uint32_t, uint32_t );
    extern uint32_t nfc_write_phy_page_raw(IO_NANDFLASH_WL_STRUCT_PTR, unsigned char *, uint32_t, uint32_t );
    extern uint32_t nfc_read_phy_page_raw(IO_NANDFLASH_WL_STRUCT_PTR, unsigned char *, uint32_t, uint32_t);
#ifdef __cplusplus
}
#endif

#endif
/* EOF */
