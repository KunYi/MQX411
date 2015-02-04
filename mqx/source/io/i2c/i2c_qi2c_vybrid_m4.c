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
*   This file contains board-specific I2C initialization functions.
*
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include "i2c_qi2c.h"

static const void   *i2c_address[] = { (void *) I2C0_BASE_PTR, (void *) I2C1_BASE_PTR, (void *) I2C2_BASE_PTR, (void *) I2C3_BASE_PTR };
     
/*FUNCTION****************************************************************
*
* Function Name    : _bsp_get_i2c_base_address
* Returned Value   : address if successful, NULL otherwise
* Comments         :
*    This function returns the base register address of the corresponding I2C device.
*
*END*********************************************************************/


VQI2C_REG_STRUCT_PTR _bsp_get_i2c_base_address(uint32_t channel)
{
    if (channel < ELEMENTS_OF(i2c_address)) {
        return (VQI2C_REG_STRUCT_PTR) i2c_address[channel];
    }
    return NULL;
}



/*FUNCTION****************************************************************
*
* Function Name    : _bsp_get_i2c_vector
* Returned Value   : vector number if successful, 0 otherwise
* Comments         :
*    This function returns desired interrupt vector number for specified I2C device.
*
*END*********************************************************************/

uint32_t _bsp_get_i2c_vector(uint8_t dev_num)
{
    switch (dev_num)
    {
      case 0: return (NVIC_I2C0);
      case 1: return (NVIC_I2C1);
      case 2: return (NVIC_I2C2);
      case 3: return (NVIC_I2C3);
    }
    return 0;
}


/* EOF */
