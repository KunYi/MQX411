#ifndef _nandflash_wl_h_
#define _nandflash_wl_h_
/*HEADER**********************************************************************
*
* Copyright 2010 Freescale Semiconductor, Inc.
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
*   The file contains functions prototype, defines, structure 
*   definitions specific for the NAND flash driver.
*
*
*END************************************************************************/
#include "nandflash_wl_config.h"
#include "ioctl.h"
#include "nandflash.h"

#define NFC_DEBUG                                       1

#define NANDFLASHERR_ECC_CORRECTED_EXCEED_THRESHOLD   (NANDFLASH_ERROR_BASE | 0x0A)
#define NANDFLASHERR_WL_BASE                          (NANDFLASH_ERROR_BASE | 0x0B)

typedef struct nandflash_page_buff
{
    unsigned char * data;
    unsigned char * metadata;
} NANDFLASH_PAGE_BUFF_STRUCT, * NANDFLASH_PAGE_BUFF_STRUCT_PTR;

typedef struct io_nandflash_wl_struct
{
   /* Core NANDFLASH */
   IO_NANDFLASH_STRUCT          CORE_NANDFLASH;
   uint32_t (_CODE_PTR_          PHY_PAGE_READ_RAW)(struct io_nandflash_wl_struct *, unsigned char *, uint32_t, uint32_t);
   uint32_t (_CODE_PTR_          PHY_PAGE_WRITE_RAW)(struct io_nandflash_wl_struct *, unsigned char *, uint32_t, uint32_t);
   uint32_t (_CODE_PTR_          PAGE_READ_METADATA)(struct io_nandflash_wl_struct *, unsigned char *, uint32_t, uint32_t);   
   uint32_t (_CODE_PTR_          PAGE_READ_WITH_METADATA)(struct io_nandflash_wl_struct *, unsigned char *, unsigned char *, uint32_t, uint32_t);      
   uint32_t (_CODE_PTR_          PAGE_WRITE_WITH_METADATA)(struct io_nandflash_wl_struct *, unsigned char *, unsigned char *, uint32_t, uint32_t);
} IO_NANDFLASH_WL_STRUCT, * IO_NANDFLASH_WL_STRUCT_PTR;

typedef struct nandflash_wl_init_struct
{
   /* Core NANDFLASH init */
   NANDFLASH_INIT_STRUCT_CPTR   CORE_NANDFLASH_INIT_PTR;
   uint32_t (_CODE_PTR_          PHY_PAGE_READ_RAW)(struct io_nandflash_wl_struct *, unsigned char *, uint32_t, uint32_t);
   uint32_t (_CODE_PTR_          PHY_PAGE_WRITE_RAW)(struct io_nandflash_wl_struct *, unsigned char *, uint32_t, uint32_t);
   uint32_t (_CODE_PTR_          PAGE_READ_METADATA)(struct io_nandflash_wl_struct *, unsigned char *, uint32_t, uint32_t);   
   uint32_t (_CODE_PTR_          PAGE_READ_WITH_METADATA)(struct io_nandflash_wl_struct *, unsigned char *, unsigned char *, uint32_t, uint32_t);      
   uint32_t (_CODE_PTR_          PAGE_WRITE_WITH_METADATA)(struct io_nandflash_wl_struct *, unsigned char *, unsigned char *, uint32_t, uint32_t);
} NANDFLASH_WL_INIT_STRUCT;

typedef const struct nandflash_wl_init_struct * NANDFLASH_WL_INIT_STRUCT_CPTR;

#ifdef __cplusplus
extern "C" {
#endif

extern const NANDFLASH_WL_INIT_STRUCT _bsp_nandflash_wl_init;

#ifdef __cplusplus
}
#endif

#endif /* _nandflash_wl_h_ */
/* EOF */
