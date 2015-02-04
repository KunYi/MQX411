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
*   This file contains definitions private to the SPI driver.
*
*
*END************************************************************************/

#ifndef __qspi_prv_h__
#define __qspi_prv_h__

#include "qspi.h"

 /* Define QSPI initialization flag */
typedef enum {
    QSPI_UNINIT,
    QSPI_INITIALIZED,
} QSPI_INIT_FLAGS;


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** Run time state information for SPI driver (shared for all fds)
*/
typedef struct qspi_driver_data_struct
{
    /* LWSEM for locking for concurrent access from several tasks */
    LWSEM_STRUCT          BUS_LOCK;

    /* Pointer to low level driver */
    QSPI_DEVIF_STRUCT_CPTR DEVIF;

    /* Pointer to init data specific for low level driver */
    const void           *DEVIF_INIT;

    /* Pointer to runtime data specific for low level driver */
    void                 *DEVIF_DATA;

    /* Initialization flag for this channel */
    QSPI_INIT_FLAGS               INIT_FLAG;

} QSPI_DRIVER_DATA_STRUCT, * QSPI_DRIVER_DATA_STRUCT_PTR;


/*
** Context information for open fd
*/
typedef struct qspi_dev_data_struct
{
    /* Inidicates that BUS_LOCK is being held by this fd */
    bool               BUS_OWNER;

    /* Open flags for this channel */
    uint32_t               FLAGS;

} QSPI_DEV_DATA_STRUCT, * QSPI_DEV_DATA_STRUCT_PTR;


#endif
