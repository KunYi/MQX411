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
*   This file contains board-specific QuadSPI initialization functions.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "qspi_quadspi.h"

#define QuadSPI0_MEMMAP_BASE 0x20000000
#define QuadSPI1_MEMMAP_BASE 0x50000000

static const void *quadspi_reg_address[] =
{
    (void *)QuadSPI0_BASE_PTR,
    (void *)QuadSPI1_BASE_PTR,
};

static const uint32_t quadspi_base_address[] =
{
    (uint32_t)QuadSPI0_MEMMAP_BASE,
    (uint32_t)QuadSPI1_MEMMAP_BASE,
};

static const uint32_t /*PSP_INTERRUPT_TABLE_INDEX*/ quadspi_vectors[][1] =
{
    { NVIC_QuadSPI0 },
    { NVIC_QuadSPI1 },
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_qspi_reg_address
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the base register address of the corresponding QuadSPI
*    module.
*
*END*----------------------------------------------------------------------*/

void *_bsp_get_quadspi_reg_address(uint32_t dev_num)
{
    if (dev_num < ELEMENTS_OF(quadspi_reg_address)) {
        return (void *)quadspi_reg_address[dev_num];
    }
    return NULL;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_qspi_base_address
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the base register address of the corresponding QuadSPI
*    module.
*
*END*----------------------------------------------------------------------*/

uint32_t _bsp_get_quadspi_base_address(uint32_t dev_num)
{
    if (dev_num < ELEMENTS_OF(quadspi_base_address)) {
        return quadspi_base_address[dev_num];
    }
    return (uint32_t) NULL;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_qspi_vectors
* Returned Value   : Number of vectors associated with the peripheral
* Comments         :
*    This function returns desired interrupt vector table indices for specified
*    QSPI module.
*
*END*----------------------------------------------------------------------*/

uint32_t _bsp_get_quadspi_vectors(uint32_t dev_num, const uint32_t  **vectors_ptr)
{
    if (dev_num < ELEMENTS_OF(quadspi_vectors)) {
        *vectors_ptr = quadspi_vectors[dev_num];
        return ELEMENTS_OF(quadspi_vectors[0]);
    }
    return 0;
}
