#ifndef io_exp_max7310_h
#define io_exp_max7310_h 1
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
*   This file contains the definitions of constants and structures
*   required for the max7310 drivers.
*
*
*END************************************************************************/


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** IOEXP_MAX7310_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a serial port is initialized.
*/
typedef struct io_exp_max7310_init_struct
{
   /* The device ID to identify different MAX7310 device*/
   uint8_t                 DEV_ID;

   /* The I2C address for this MAX7310 I/O expander */
   uint8_t                 I2C_address;

   /* The I2C bus name the MAX7310 on*/
   const char*            I2C_control;

} IOEXP_MAX7310_INIT_STRUCT, * IOEXP_MAX7310_INIT_STRUCT_PTR;

typedef const IOEXP_MAX7310_INIT_STRUCT * IOEXP_MAX7310_INIT_STRUCT_CPTR;

/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t _max7310_install (char *, IOEXP_MAX7310_INIT_STRUCT_CPTR);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
