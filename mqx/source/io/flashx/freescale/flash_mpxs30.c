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

#include "mqx.h"
#include "bsp.h"
#include "flash_c90_prv.h"
#include "flash_c90i_prv.h"

#define K1      (1024)
#define K16     (16*K1)
#define K32     (32*K1)
#define K64     (64*K1)
#define K128    (128*K1)
#define K256    (256*K1)


const FLASHX_BLOCK_INFO_STRUCT _mpxs30_data_flash_block_map[] =
{
    /* count, offset, size */

    /* LAS */
    { 4, 0,                                                 K16,  C90_ASPACE_LAS|C90_CONTROLLER_0|C90_WIDTH_32|C90_ALIGN_32 }, /* D0 L0..L3 4*16 KB */

    { 0, 0, 0 }
};

const FLASHX_BLOCK_INFO_STRUCT _mpxs30_code_flash_block_map[] =
{
    /* count, offset, size */
    /* LAS */
    { 2,  0,                                                K16,  C90_ASPACE_LAS|C90_CONTROLLER_0|C90_WIDTH_64 }, /* C0 L0..L1 2*16 KB */
    { 2,  2*K16,                                            K32,  C90_ASPACE_LAS|C90_CONTROLLER_0|C90_WIDTH_64 }, /* C0 L2..L3 2*32 KB */
    { 2,  2*K16+2*K32,                                      K16,  C90_ASPACE_LAS|C90_CONTROLLER_0|C90_WIDTH_64 }, /* C0 L4..L5 2*16 KB */
    { 2,  4*K16+2*K32,                                      K64,  C90_ASPACE_LAS|C90_CONTROLLER_0|C90_WIDTH_64 }, /* C0 L6..L7 2*64 KB */
    { 2,  K256+0,                                           K16,  C90_ASPACE_LAS|C90_CONTROLLER_1|C90_WIDTH_64 }, /* C1 L0..L1 2*16 KB */
    { 2,  K256+2*K16,                                       K32,  C90_ASPACE_LAS|C90_CONTROLLER_1|C90_WIDTH_64 }, /* C1 L2..L3 2*32 KB */
    { 2,  K256+2*K16+2*K32,                                 K16,  C90_ASPACE_LAS|C90_CONTROLLER_1|C90_WIDTH_64 }, /* C1 L4..L5 2*16 KB */
    { 2,  K256+4*K16+2*K32,                                 K64,  C90_ASPACE_LAS|C90_CONTROLLER_1|C90_WIDTH_64 }, /* C1 L6..L7 2*64 KB */

    /* MAS */
    { 2,   MPXS30_CFLASH_MAS_ADDR-MPXS30_CFLASH0_MEM_BASE,  K256, C90_ASPACE_MAS|C90_INTERLEAVED|C90_WIDTH_64 }, /* C0/C1 M0..M1 2*128 KB */

    /* HAS */
    { 4,   MPXS30_CFLASH_HAS_ADDR-MPXS30_CFLASH0_MEM_BASE,  K256, C90_ASPACE_HAS|C90_INTERLEAVED|C90_WIDTH_64 }, /* C0/C1 H0..H3 4*128 KB */

    { 0, 0, 0 }
};


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_c90_address
* Returned Value   : Address upon success
* Comments         :
*    This function returns the base register address of the default C90 flash
*    controller. It is for use with the single controller version of the c90
*    driver.
*
*END*----------------------------------------------------------------------*/

void *_bsp_get_c90_address (uint32_t base_addr)
{
    return (void *)MPXS30_DFLASH0_BASE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_c90i_address
* Returned Value   : Address upon success
* Comments         :
*    This function returns the base register address of the specified C90 flash
*    controller. It is for use with the multiple controller version of the c90
*    driver.
*
*END*----------------------------------------------------------------------*/


void *_bsp_get_c90i_address (uint32_t base_addr, uint32_t controller)
{
    /* Asking for base address of the main controller associated with the specified address */
    if (controller == 0) {
        switch (base_addr) {
            case  MPXS30_CFLASH0_MEM_BASE: return (void *)MPXS30_CFLASH0_BASE;
            case  MPXS30_DFLASH_MEM_BASE:  return (void *)MPXS30_DFLASH0_BASE;
        }
    } else if (controller == 1){
        switch (base_addr) {
            case  MPXS30_CFLASH0_MEM_BASE: return (void *)MPXS30_CFLASH1_BASE;
            case  MPXS30_CFLASH1_MEM_BASE: return (void *)MPXS30_CFLASH1_BASE;
        }
    }
    return NULL;
}
