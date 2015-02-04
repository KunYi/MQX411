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
*   required for the QuadSPI driver
*
*
*END************************************************************************/


#ifndef __qspi_h__
#define __qspi_h__


/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

/* QSPI Error Codes */
#define QSPI_OK                             (0x00)
#define QSPI_ERROR_REOPEN                   (QSPI_ERROR_BASE | 0x1)


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** Functions implemented by lower layer SPI driver (device interface).
*/
typedef _mqx_int (_CODE_PTR_ QSPI_DEVIF_INIT_FPTR)(const void * init_data_ptr, void **io_info_ptr_ptr);
typedef _mqx_int (_CODE_PTR_ QSPI_DEVIF_DEINIT_FPTR)(void *io_info_ptr);
typedef _mqx_int (_CODE_PTR_ QSPI_DEVIF_TX_RX_FPTR)(void *io_info_ptr, uint8_t *txbuf, uint8_t *rxbuf, uint32_t len);
typedef _mqx_int (_CODE_PTR_ QSPI_DEVIF_IOCTL_FPTR)(void *io_info_ptr, uint32_t cmd, uint32_t *cmd_param_ptr);


typedef struct qspi_devif_struct
{
    /* Initialization if low level SPI driver */
    QSPI_DEVIF_INIT_FPTR     INIT;

    /* Deinitialization of low level SPI driver */
    QSPI_DEVIF_DEINIT_FPTR   DEINIT;

    /* Data trasfer function */
    QSPI_DEVIF_TX_RX_FPTR    TX_RX;

    /* IOCTL commands not fully handled by upper layer are passed to this function */
    QSPI_DEVIF_IOCTL_FPTR    IOCTL;

} QSPI_DEVIF_STRUCT, * QSPI_DEVIF_STRUCT_PTR;

typedef const QSPI_DEVIF_STRUCT * QSPI_DEVIF_STRUCT_CPTR;


/*
** QSPI_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a qspi port is initialized.
*/
typedef struct qspi_init_struct
{
    /* Pointer to low level driver */
    QSPI_DEVIF_STRUCT_CPTR DEVIF;

    /* Pointer to init data specific for low level driver */
    const void           *DEVIF_INIT;

} QSPI_INIT_STRUCT, * QSPI_INIT_STRUCT_PTR;

typedef const QSPI_INIT_STRUCT * QSPI_INIT_STRUCT_CPTR;



/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif


extern _mqx_int _io_qspi_install(char *identifier, QSPI_INIT_STRUCT_CPTR init_data_ptr);


#ifdef __cplusplus
}
#endif


#endif

/* EOF */
