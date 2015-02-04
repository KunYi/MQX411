#ifndef _max7310_prv_h
#define _max7310_prv_h 1
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

#include "io_exp_max7310.h"

#define MAX7310_MAGIC_WORD 0x4D415837
#define MAX7310_PINS_NUM   8
/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** MAX7310_ACCESS_DESC_STRUCT
** The descriptor structure used for record one valid access to the driver
*/
typedef struct max7310_access_descriptor
{
    /*The identity for distinguish different accesses*/
    uint32_t                            identity;

    /*The pin number that is using by this access*/
    uint8_t                             pin;

    /*The direction for this pin*/
    uint8_t                             direction;

    /*The value of the pin's logic level*/
    uint8_t                             val;

    /*Mutex to synchronize the tasks sharing this access descriptor*/
    MUTEX_STRUCT                       access_mutex;

}MAX7310_ACCESS_DESC_STRUCT, * MAX7310_ACCESS_DESC_STRUCT_PTR;

/*
** MAX7310_INFO_STRUCT
** The private info structure for control MAX7310
*/
typedef struct max7310_info_struct
{
   /*Initialization parameters for MAX7310 */
   IOEXP_MAX7310_INIT_STRUCT               INIT;

   /* This mutex is used to synchronize the MAX7310 device operations*/
   MUTEX_STRUCT                            DEV_MUTEX;

   /*The file handle for I2C device*/
   FILE_PTR                                I2C_FILE;

   /*Pin bitmap, indicating the usage of the pins*/
   uint8_t                                 PIN_BITMAP;

} MAX7310_INFO_STRUCT, * MAX7310_INFO_STRUCT_PTR;


#endif
/* EOF */
