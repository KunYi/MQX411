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
*   The file contains function prototypes and defines for the internal 
*   flash driver.
*
*
*END************************************************************************/
#ifndef __flash_c90i_prv_h__
#define __flash_c90i_prv_h__


#include "flashx.h"
#include "flashxprv.h"


/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

// SPECIAL_TAG definitions
#define C90_ASPACE_MASK     0x0000000f
#define C90_CONTROLLER_MASK 0x000000f0
#define C90_WIDTH_MASK      0x0000ff00
#define C90_ASPACE_SHIFT        (0)
#define C90_CONTROLLER_SHIFT    (4)
#define C90_WIDTH_SHIFT         (8)

#define C90_CONTROLLER_0    0x00000010
#define C90_CONTROLLER_1    0x00000020
#define C90_INTERLEAVED     (C90_CONTROLLER_0|C90_CONTROLLER_1)

#define C90_WIDTH_32        (32<<C90_WIDTH_SHIFT)
#define C90_WIDTH_64        (64<<C90_WIDTH_SHIFT)
#define C90_WIDTH_128       (128<<C90_WIDTH_SHIFT)

#define C90_ASPACE(x)       ((x&C90_ASPACE_MASK)>>C90_ASPACE_SHIFT)
#define C90_WIDTH(x)        ((x&C90_WIDTH_MASK)>>C90_WIDTH_SHIFT)

/*----------------------------------------------------------------------*/
/*
**                    Structure Definitions
*/

typedef struct c90i_flash_internal_struct 
{
    VC90_REG_STRUCT_PTR reg0_ptr;
    VC90_REG_STRUCT_PTR reg1_ptr;
   void (_CODE_PTR_ flash_exec)(VC90_REG_STRUCT_PTR);
   uint32_t aspace_sectors[C90_ASPACE_COUNT];
} C90I_FLASH_INTERNAL_STRUCT, * C90I_FLASH_INTERNAL_STRUCT_PTR;


/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

bool c90i_flash_init (IO_FLASHX_STRUCT_PTR);
void    c90i_flash_deinit (IO_FLASHX_STRUCT_PTR);
bool c90i_flash_sector_erase (IO_FLASHX_STRUCT_PTR, char *, _mem_size);
bool c90i_flash_sector_program (IO_FLASHX_STRUCT_PTR, char *, char *, _mem_size);
bool c90i_flash_chip_erase (IO_FLASHX_STRUCT_PTR);
bool c90i_flash_write_protect(IO_FLASHX_STRUCT_PTR, _mqx_uint);

#ifdef __cplusplus
}
#endif

#endif
