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
*   This file contains CPU specific eDMA functions.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "edma.h"


static const uint32_t /*PSP_INTERRUPT_TABLE_INDEX*/ edma_done_vectors[][1] =
{
    { 
        INT_DMA0_Transfer_Complete
    },
    { 
        INT_DMA1_Transfer_Complete
    }
};



static const uint32_t /*PSP_INTERRUPT_TABLE_INDEX*/ edma_error_vectors[][1] =
{
    { 
        INT_DMA0_Error
    },
    { 
        INT_DMA1_Error
    }
};



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_edma_done_vectors
* Returned Value   : Number of vectors associated with the peripheral
* Comments         :
*    This function returns desired interrupt vector table indices for specified module.
*
*END*----------------------------------------------------------------------*/

int _bsp_get_edma_done_vectors(uint32_t dev_num, const uint32_t  **vectors_ptr)
{
    if (dev_num < ELEMENTS_OF(edma_done_vectors)) {
        *vectors_ptr = edma_done_vectors[dev_num];
        return ELEMENTS_OF(edma_done_vectors[0]);
    }
    return 0;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_edma_error_vector
* Returned Value   : Number of vectors associated with the peripheral
* Comments         :
*    This function returns desired interrupt vector table indices for specified module.
*
*END*----------------------------------------------------------------------*/

int _bsp_get_edma_error_vectors(uint32_t dev_num, const uint32_t  **vectors_ptr)
{
    if (dev_num < ELEMENTS_OF(edma_error_vectors)) {
        *vectors_ptr = edma_error_vectors[dev_num];
        return ELEMENTS_OF(edma_error_vectors[0]);
    }
    return 0;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_edma_enable
* Returned Value   :
* Comments         :
*    The function performs necessary operations to enable eDMA module
*
*END*----------------------------------------------------------------------*/

int _bsp_edma_enable(uint32_t dev_num)
{
    switch (dev_num) {
        case 0:
            CCM_CCGR0 |= CCM_CCGR0_CG4_MASK; /* DMAMUX0 */
            CCM_CCGR0 |= CCM_CCGR0_CG5_MASK; /* DMAMUX1 */
            CCM_CCPGR1 |= CCM_CCPGR1_PPCG8_MASK; /* DMA0 */
            CCM_CCPGR1 |= CCM_CCPGR1_PPCG9_MASK; /* DMA0 TCD */
            break;
        case 1:
            CCM_CCGR6 |= CCM_CCGR6_CG1_MASK; /* DMAMUX2 */
            CCM_CCGR6 |= CCM_CCGR6_CG2_MASK; /* DMAMUX3 */
            CCM_CCPGR3 |= CCM_CCPGR3_PPCG8_MASK; /* DMA1 */
            CCM_CCPGR3 |= CCM_CCPGR3_PPCG9_MASK; /* DMA1 TCD */
            break;
        default:
            return -1;
    }
    
    return 0;
}
