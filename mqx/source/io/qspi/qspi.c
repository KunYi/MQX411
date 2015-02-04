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
*   This file contains functions for generic QSPI layer.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include <stdlib.h>

#include "qspi.h"
#include "qspi_prv.h"


/* Forward declarations */
static _mqx_int _io_qspi_open(MQX_FILE_PTR fd_ptr, char  *open_name_ptr, char  *flags);
static _mqx_int _io_qspi_close(MQX_FILE_PTR fd_ptr);
static _mqx_int _io_qspi_read(MQX_FILE_PTR fd_ptr, char *data_ptr, _mqx_int n);
static _mqx_int _io_qspi_write(MQX_FILE_PTR fd_ptr, char *data_ptr, _mqx_int n);
static _mqx_int _io_qspi_ioctl(MQX_FILE_PTR fd_ptr, uint32_t cmd, void *param_ptr);
static _mqx_int _io_qspi_uninstall(IO_DEVICE_STRUCT_PTR io_dev_ptr);


/*FUNCTION****************************************************************
*
* Function Name    : _io_qspi_install
* Returned Value   : MQX error code
* Comments         :
*    Installs QSPI device.
*
*END**********************************************************************/
_mqx_int _io_qspi_install
    (
        /* [IN] A string that identifies the device for fopen */
        char                       *identifier,

        /* [IN] Pointer to driver initialization data */
        QSPI_INIT_STRUCT_CPTR           init_data_ptr
    )
{
    static QSPI_DRIVER_DATA_STRUCT_PTR driver_data;
    _mqx_int error_code;

    if (init_data_ptr->DEVIF->TX_RX == NULL)
    {
        /* Missing mandatory low level driver function */
        return IO_ERROR_DEVICE_INVALID;
    }

    driver_data = (QSPI_DRIVER_DATA_STRUCT_PTR)_mem_alloc_system_zero((_mem_size)sizeof(QSPI_DRIVER_DATA_STRUCT));
    if (driver_data == NULL)
    {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type(driver_data, MEM_TYPE_IO_QSPI_DEVICE_STRUCT);

    driver_data->DEVIF = init_data_ptr->DEVIF;
    driver_data->DEVIF_INIT = init_data_ptr->DEVIF_INIT;
    driver_data->INIT_FLAG = QSPI_UNINIT;

    error_code = _io_dev_install_ext(identifier,
        _io_qspi_open,
        _io_qspi_close,
        _io_qspi_read,
        _io_qspi_write,
        _io_qspi_ioctl,
        _io_qspi_uninstall,
        (void *)driver_data);

    if (error_code)
    {
        _mem_free(driver_data);
        return error_code;
    }

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_qspi_uninstall
* Returned Value   : MQX error code
* Comments         :
*    Uninstalls QSPI device.
*
*END**********************************************************************/
static _mqx_int _io_qspi_uninstall
    (
        /* [IN] The IO device structure for the device */
        IO_DEVICE_STRUCT_PTR           io_dev_ptr
    )
{
    QSPI_DRIVER_DATA_STRUCT_PTR driver_data;

    if (io_dev_ptr)
    {
        driver_data = (QSPI_DRIVER_DATA_STRUCT_PTR)(io_dev_ptr->DRIVER_INIT_PTR);
        _mem_free(driver_data);
    }

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_qspi_read_write
* Returned Value   : Number of bytes read
* Comments         :
*    Claims the bus and performs QSPI transfer by calling low level driver.
*    Returns number of bytes processed.
*
*END*********************************************************************/
static _mqx_int _io_qspi_read_write
    (
        /* [IN] The handle returned from _fopen */
        MQX_FILE_PTR                   fd_ptr,

        /* Buffer containing bytes to write */
        char                       *wrbuf,

        /* Buffer containing bytes read after transfer */
        char                       *rdbuf,

        /* Byte length of each buffer */
        uint32_t                        len
    )
{
    QSPI_DEV_DATA_STRUCT_PTR    dev_data;
    QSPI_DRIVER_DATA_STRUCT_PTR driver_data;

    _mqx_int result;
    _mqx_int error_code;

    /* parameter sanity check */
    if (len == 0 || ((wrbuf == NULL) && (rdbuf == NULL)))
    {
        return IO_ERROR;
    }

    dev_data = (QSPI_DEV_DATA_STRUCT_PTR)(fd_ptr->DEV_DATA_PTR);
    driver_data = (QSPI_DRIVER_DATA_STRUCT_PTR)(fd_ptr->DEV_PTR->DRIVER_INIT_PTR);

    if (!(dev_data->BUS_OWNER))
    {
        error_code = _lwsem_wait(&(driver_data->BUS_LOCK));
        if (error_code != MQX_OK)
        {
            _task_set_error(error_code);
            return IO_ERROR;
        }
        dev_data->BUS_OWNER = TRUE;
    }

    result = driver_data->DEVIF->TX_RX(driver_data->DEVIF_DATA, (uint8_t *)wrbuf, (uint8_t *)rdbuf, len);

    return result;
}

/*FUNCTION****************************************************************
*
* Function Name    : _io_qspi_open
* Returned Value   : MQX error code
* Comments         :
*    This routine opens the QSPI I/O channel.
*
*END**********************************************************************/
static _mqx_int _io_qspi_open
    (
        /* [IN] The file handle for the device being opened */
        MQX_FILE_PTR         fd_ptr,

        /* [IN] The remaining portion of the name of the device */
        char                  *open_name_ptr,

        /* [IN] The flags to be used during operation */
        char                  *flags
    )
{
    QSPI_DEV_DATA_STRUCT_PTR    dev_data;
    QSPI_DRIVER_DATA_STRUCT_PTR driver_data = (QSPI_DRIVER_DATA_STRUCT_PTR)(fd_ptr->DEV_PTR->DRIVER_INIT_PTR);
     _mqx_int error_code = MQX_OK;

    /* check if the same device have been opened. QSPI device don't support mult-instance. */
    if (driver_data->INIT_FLAG == QSPI_INITIALIZED) {
        printf("\nERROR: try to reopen %s!\n", open_name_ptr);
        return QSPI_ERROR_REOPEN;
    }

    dev_data = _mem_alloc_system_zero(sizeof(QSPI_DEV_DATA_STRUCT));
    if (dev_data == NULL )
    {
        return MQX_OUT_OF_MEMORY;
    }
    fd_ptr->DEV_DATA_PTR = dev_data;

    if (driver_data->DEVIF->INIT)
        error_code = driver_data->DEVIF->INIT(driver_data->DEVIF_INIT, &(driver_data->DEVIF_DATA));

    if (error_code != MQX_OK)
    {
        printf("\nERROR: qspi failed to initialize low level driver!\n");
        _mem_free(dev_data);
        return error_code;
    }

    _lwsem_create(&driver_data->BUS_LOCK, 1);
    /* low level driver initial flag */
    driver_data->INIT_FLAG = QSPI_INITIALIZED;

    /* open flags */
    dev_data->FLAGS = (uint32_t)flags;

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_qspi_close
* Returned Value   : MQX error code
* Comments         :
*    This routine closes the QSPI I/O channel.
*
*END**********************************************************************/
static _mqx_int _io_qspi_close
    (
        /* [IN] The file handle for the device being closed */
        MQX_FILE_PTR         fd_ptr
    )
{
    QSPI_DRIVER_DATA_STRUCT_PTR driver_data = (QSPI_DRIVER_DATA_STRUCT_PTR)(fd_ptr->DEV_PTR->DRIVER_INIT_PTR);
    _mqx_int error_code = MQX_OK;

    /* free device driver data */
    _lwsem_destroy(&driver_data->BUS_LOCK);
    _mem_free(fd_ptr->DEV_DATA_PTR);

    /* deinitialize low level driver */
    if (driver_data->DEVIF->DEINIT)
        error_code = driver_data->DEVIF->DEINIT(driver_data->DEVIF_DATA);

    if (error_code != MQX_OK)
    {
        printf("\nERROR: qspi failed to release low level driver!\n");
        return error_code;
    }

    /* set driver initial flag to uninitialized */
    driver_data->INIT_FLAG = QSPI_UNINIT;

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_qspi_read
* Returned Value   : Number of bytes read
* Comments         :
*    POSIX wrapper to perform QSPI transfer storing received data into the buffer.
*    Returns number of bytes received.
*
*END*********************************************************************/
static _mqx_int _io_qspi_read
    (
        /* [IN] The handle returned from _fopen */
        MQX_FILE_PTR                   fd_ptr,

        /* [OUT] Where the characters are to be stored */
        char                       *data_ptr,

        /* [IN] The number of bytes to read */
        _mqx_int                       n
    )
{
    return _io_qspi_read_write(fd_ptr, NULL, data_ptr, n);
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_qspi_write
* Returned Value   : Number of bytes to write
* Comments         :
*    POSIX wrapper to perform QSPI transfer sending data from the buffer.
*    Returns number of bytes received.
*
*END**********************************************************************/
static _mqx_int _io_qspi_write
    (
        /* [IN] The handle returned from _fopen */
        MQX_FILE_PTR                   fd_ptr,

        /* [IN] Where the bytes are stored */
        char                       *data_ptr,

        /* [IN] The number of bytes to output */
        _mqx_int                       n
    )
{
    return _io_qspi_read_write(fd_ptr, data_ptr, NULL, n);
}


/*FUNCTION*****************************************************************
*
* Function Name    : _io_qspi_ioctl
* Returned Value   : MQX error code
* Comments         :
*    Returns result of QSPI ioctl operation.
*
*END*********************************************************************/
static _mqx_int _io_qspi_ioctl
    (
        /* [IN] the file handle for the device */
        MQX_FILE_PTR                   fd_ptr,

        /* [IN] the ioctl command */
        uint32_t                        cmd,

        /* [IN] the ioctl parameters */
        void                          *param_ptr
    )
{
    QSPI_DEV_DATA_STRUCT_PTR    dev_data;
    QSPI_DRIVER_DATA_STRUCT_PTR driver_data;

    _mqx_int result = MQX_OK;
    _mqx_int lldrv_result = MQX_OK;

    dev_data = (QSPI_DEV_DATA_STRUCT_PTR)(fd_ptr->DEV_DATA_PTR);
    driver_data = (QSPI_DRIVER_DATA_STRUCT_PTR)(fd_ptr->DEV_PTR->DRIVER_INIT_PTR);

    switch (cmd) {
        default:
            /* Pass unhandled IOCTL to the low level driver */
            if (driver_data->DEVIF->IOCTL != NULL)
            {
                /* Do not assume that the low level driver is re-entrant, lock semaphore */
                if (!(dev_data->BUS_OWNER) && ((result=_lwsem_wait(&(driver_data->BUS_LOCK))) != MQX_OK))
                    break;

                lldrv_result = driver_data->DEVIF->IOCTL(driver_data->DEVIF_DATA, cmd, param_ptr);

                if (!(dev_data->BUS_OWNER))
                    result = _lwsem_post(&(driver_data->BUS_LOCK));

                /* Report low level driver error, if any */
                if (lldrv_result != MQX_OK)
                    result = lldrv_result;
            }
            else
            {
                result = IO_ERROR_INVALID_IOCTL_CMD;
            }
            break;
    }

    return result;
}

/* EOF */
