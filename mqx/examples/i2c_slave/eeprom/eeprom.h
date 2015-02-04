#ifndef __eeprom_h__
#define __eeprom_h__
/*HEADER**********************************************************************
*
* Copyright 2014 Freescale Semiconductor, Inc.
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
*   This file contains definitions for the I2C slave EEPROM example.
*
*
*END************************************************************************/

/* The I2C EEPROM parameters */
#define I2C_EEPROM_ADDRESS   0x50     /* I2C bus address of EEPROM */
#define I2C_EEPROM_SIZE      0x0100   /* Overall memory byte size */
#define I2C_EEPROM_PAGE_SIZE 0x10     /* Maximum bytes per write */

#endif
/* EOF */
