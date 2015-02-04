#ifndef __eeprom_h__
#define __eeprom_h__
/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains definitions for the I2C master example.
*
*
*END************************************************************************/

#if defined(BSP_MPC8308RDB)
#define I2C_EEPROM_PAGE_SIZE       0x40     /* Maximum bytes per write for M24256 */
#define I2C_EEPROM_MEMORY_WIDTH    0x02     /* Memory address width in bytes */
#elif defined(BSP_M53015EVB)
#define I2C_EEPROM_PAGE_SIZE       0x08     /* Maximum bytes per write for AT24C02B */
#define I2C_EEPROM_MEMORY_WIDTH    0x01     /* Memory address width in bytes */
#else
#define I2C_EEPROM_PAGE_SIZE       0x10     /* Maximum bytes per write for 24LC16 */
#define I2C_EEPROM_MEMORY_WIDTH    0x01     /* Memory address width in bytes */
#endif

/* The I2C EEPROM test addresses */
#define I2C_EEPROM_BUS_ADDRESS     0x50     /* I2C bus address of EEPROM */
#define I2C_EEPROM_MEMORY_ADDRESS1 0x1A     /* Memory test address 1 */
#define I2C_EEPROM_MEMORY_ADDRESS2 0x7B     /* Memory test address 2 */

/* Global variables */
extern LWSEM_STRUCT lock;

/* Function prototypes */
extern void i2c_write_eeprom (MQX_FILE_PTR, uint32_t, unsigned char *, _mqx_int);
extern void i2c_read_eeprom (MQX_FILE_PTR, uint32_t, unsigned char *, _mqx_int);

#endif
/* EOF */
