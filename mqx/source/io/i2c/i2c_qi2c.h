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
*   This file contains the definitions of constants and structures
*   required for the I2C drivers .
*
*
*END************************************************************************/

#ifndef _i2c_qi2c_h
#define _i2c_qi2c_h 1


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** QI2C_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a serial port is initialized.
*/
typedef struct qi2c_init_struct
{
    /* The I2C channel to initialize */
    uint8_t                 CHANNEL;

    /* Default operating mode */
    uint8_t                 MODE;

    /* The I2C station address for the channel */
    uint8_t                 STATION_ADDRESS;

    /* Desired baud rate */
    uint32_t                BAUD_RATE;

    /* Interrupt level to use if interrupt driven */
    uint32_t                LEVEL;

    /* Tx buffer size (interrupt only) */
    uint32_t                TX_BUFFER_SIZE;

    /* Rx buffer size (interrupt only) */
    uint32_t                RX_BUFFER_SIZE;

} QI2C_INIT_STRUCT, * QI2C_INIT_STRUCT_PTR;
typedef const QI2C_INIT_STRUCT * QI2C_INIT_STRUCT_CPTR;

/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

uint32_t _qi2c_polled_install (char *, QI2C_INIT_STRUCT_CPTR);
uint32_t _qi2c_int_install (char *, QI2C_INIT_STRUCT_CPTR);
VQI2C_REG_STRUCT_PTR _bsp_get_i2c_base_address(uint32_t channel);
uint32_t _bsp_get_i2c_vector(uint8_t dev_num);
bool _bsp_i2c_enable_access(uint32_t device);

#ifdef __cplusplus
}
#endif

#endif
