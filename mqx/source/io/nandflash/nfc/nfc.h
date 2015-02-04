#ifndef _nfc_h_
#define _nfc_h_
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

#include "nandflash.h"

#define MAX_WAIT_COMMAND                       0x00100000


/*----------------------------------------------------------------------*/
/*
**                     FUNCTION PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif

void   *_bsp_get_nfc_address(void);

extern void    nfc_io_init(void);

extern uint32_t nfc_init( IO_NANDFLASH_STRUCT_PTR );

extern void    nfc_deinit( IO_NANDFLASH_STRUCT_PTR );

extern uint32_t nfc_erase_flash( IO_NANDFLASH_STRUCT_PTR );

extern uint32_t nfc_erase_block( IO_NANDFLASH_STRUCT_PTR, uint32_t, bool );

extern uint32_t nfc_read_page( IO_NANDFLASH_STRUCT_PTR, unsigned char *, 
    uint32_t, uint32_t );

extern uint32_t nfc_write_page( IO_NANDFLASH_STRUCT_PTR, unsigned char *, 
    uint32_t, uint32_t );
    
extern uint32_t nfc_reset( IO_NANDFLASH_STRUCT_PTR );

extern uint32_t nfc_check_block( IO_NANDFLASH_STRUCT_PTR, uint32_t );
    
extern uint32_t nfc_mark_block_as_bad( IO_NANDFLASH_STRUCT_PTR, uint32_t);
    
extern uint32_t nfc_read_ID( IO_NANDFLASH_STRUCT_PTR, unsigned char *, _mem_size );
    
extern _mqx_int nfc_ioctl(IO_NANDFLASH_STRUCT_PTR, _mqx_uint, void *);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
