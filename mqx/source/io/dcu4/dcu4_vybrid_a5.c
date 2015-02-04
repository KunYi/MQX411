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
*   This file contains board-specific DCU4 initialization functions.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include <dcu4.h>

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_dcu4_base_address
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the base register address of the corresponding DCU4
*    module.
*
*END*----------------------------------------------------------------------*/
void *_bsp_get_dcu4_base_address(uint32_t dev_num)
{
    void   *addr = NULL;

    switch (dev_num)
    {
        case 0:
            addr = (void *)DCU0_BASE_PTR;
            break;
        case 1:
            addr = (void *)DCU1_BASE_PTR;
            break;
        default:
            break;
    }

    return addr;
}

/*FUNCTION****************************************************************
*
* Function Name    : _bsp_get_dcu4_vector
* Returned Value   : vector number if successful, 0 otherwise
* Comments         :
*    This function returns desired interrupt vector number for specified DCU4 device.
*
*END*********************************************************************/

uint32_t _bsp_get_dcu4_vector(uint32_t dev_num)
{
    switch (dev_num)
    {
        case 0:
            return INT_DCU0;
        case 1:
            return INT_DCU1;
        default:
            break;
    }

    return 0;
}

/* EOF */
