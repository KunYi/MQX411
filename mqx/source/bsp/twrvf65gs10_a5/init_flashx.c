
/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the global generic settings for FLASHX driver.
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"

const FLASHX_FILE_BLOCK _bsp_flashx_quadspi0_file_blocks[] = {
    {"bank0", QuadSPI0_MEMMAP_BASE, QuadSPI0_MEMMAP_END},
    {NULL, 0, 0 }
};

/* Comment: NUM_SECTORS == 0 is the end flag for flashx to check the end of flashx blocks */
const FLASHX_BLOCK_INFO_STRUCT _flashx_quadspi0_block_map[] = {
    {64, 0,         0x40000, FLASHX_QUADSPI_A1_TAG},
    {64, 0x1000000, 0x40000, FLASHX_QUADSPI_B1_TAG},
    {0,  0,         0,       0xFFFFFFFF},
};

const FLASHX_QUADSPI_INIT_STRUCT _bsp_flashx_quadspi0_dev_init_data = {
    0,                                  /* QSPI controller id */
    QuadSPI_CLK_DDR_MODE,               /* Clock Mode */
    QuadSPI_SINGLE_MODE,                /* IO Mode */
    33000000,                           /* Serial Clock */
    FALSE,                              /* Parallel Mode */
};


const FLASHX_INIT_STRUCT _bsp_flashx_quadspi0_init = {
    QuadSPI0_MEMMAP_BASE, /* BASE_ADDR should be 0 for internal flashes */
    _flashx_quadspi0_block_map, /* HW block map for KinetisN devices */
    _bsp_flashx_quadspi0_file_blocks, /* Files on the device defined by the BSP */
    &_flashx_quadspi_if, /* Interface for low level driver */
    4, /* For external devices, data lines for the flash. Not used for internal flash devices. */
    0,  /* Number of parallel external flash devices. Not used for internal flash devices. */
    0,  /* 0 if the write verify is requested, non-zero otherwise */
    (void *) &_bsp_flashx_quadspi0_dev_init_data /* low level driver specific data */
};

/* EOF */
