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
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the initialization definition for QSPI driver
*
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>


/*
** Parameters for QSPI initialization
*/

/* definiton for QSPI FLash */

static QuadSPI_FLASH_INFO_STRUCT _bsp_quadspi0_flash_device[] = {
    {
        0,
        64,
        0x40000
    }, {
        0x1000000,
        0,
        0
    }, {
        0x1000000,
        64,
        0x40000
    }, {
        0x2000000,
        0,
        0
   }
};

static QuadSPI_FLASH_CMD_STRUCT _bsp_quadspi0_flash_cmd =
{
    .READ_CMD = {0x0403, 0x0818, 0x1c80},
    .DUAL_READ_CMD = {0x04BB, 0x0918, 0x0C04, 0x1D80},
    .QUAD_READ_CMD = {0x04EB, 0x0A18, 0x12FF, 0x0C04, 0x1E80},
    .DDR_READ_CMD = {0x040D, 0x2818, 0x2CFF, 0x0C02, 0x3880},
    .DDR_DUAL_READ_CMD = {0x04BD, 0x2918, 0x2DFF, 0x0C04, 0x3980},
    .DDR_QUAD_READ_CMD = {0x04ED, 0x2A18, 0x2EFF, 0x0C06, 0x3A80},
};

static const QuadSPI_INIT_STRUCT _bsp_quadspi0_init_data = {
    0,                                  /* QSPI controller id */
    QuadSPI_CLK_DDR_MODE,               /* Clock Mode */
    QuadSPI_SINGLE_MODE,                /* IO Mode */
    33000000,                           /* Serial Clock: Read */
    33000000,                           /* Serial Clock: Write */
    QuadSPI_PAGE_256,                   /* Page size */
    _bsp_quadspi0_flash_device,         /* flash device information */
    &_bsp_quadspi0_flash_cmd,           /* flash command */
    TRUE,                               /* Parallel mode */
};

const QSPI_INIT_STRUCT _bsp_quadspi0_init = {
    &_qspi_quadspi_devif,               /* Low level driver interface */
    &_bsp_quadspi0_init_data,           /* Low level driver init data */
};

