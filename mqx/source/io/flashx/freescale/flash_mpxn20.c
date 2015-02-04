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
*   flash device. The 8313e-rdb uses a Spansion 29lv640 The
*   8323e-rdb uses a Intel Strata 28f128j3.
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "flashx.h"
#include "flash_c90.h"
#include "flash_c90_prv.h"


const FLASHX_BLOCK_INFO_STRUCT _mpxn20_block_map[] =
{
    /* count, address, size */

    /* LAS */
    { 8,        0,  0x4000, C90_ASPACE_LAS|C90_WIDTH_128|C90_ALIGN_64 },    /* L0..L7    8*16 KB */
    { 2,  0x20000, 0x10000, C90_ASPACE_LAS|C90_WIDTH_128|C90_ALIGN_64 },    /* L8..L9    2*64 KB */
    
    /* MAS */    
    { 2,  0x40000, 0x20000, C90_ASPACE_MAS|C90_WIDTH_128|C90_ALIGN_64 },    /* M0..M1    2*128 KB */
    
    /* HAS */    
    { 6,  0x80000, 0x40000, C90_ASPACE_HAS|C90_WIDTH_128|C90_ALIGN_64 },    /* H0..H5    6*256 KB */
    
    /* Shadow row */
    { 1, 0xFFC000,  0x4000, C90_ASPACE_SHADOW|C90_WIDTH_128|C90_ALIGN_64 }, /* shadow row, 16 KB */

    { 0, 0, 0 }
};


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_c90_address
* Returned Value   : Address upon success
* Comments         :
*    This function returns the base register address of the C90 flash controller.
*
*END*----------------------------------------------------------------------*/

void *_bsp_get_c90_address (uint32_t base_addr)
{
    return (void *)MPXN20_PF_CONFIG_BASE;
}


/* EOF */
