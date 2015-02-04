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
*   This include file is used to provide information needed by
*   applications using the I2S I/O functions.
*
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>
#include <io.h>
#include "i2s.h"
#include "i2s_int_prv.h"
#define MEM_TYPE_IO_I2S_DEVICE_STRUCT   1045
#define IO_DEV_TYPE_PHYS_I2S_INTERRUPT  (0x001E)


/*FUNCTION****************************************************************
*
* Function Name    : _io_i2s_install
* Returned Value   : MQX error code
* Comments         :
*    Install the I2S device.
*
*END**********************************************************************/

_mqx_uint _io_i2s_install
(
    /* [IN] A string that identifies the device for fopen */
    char                      *identifier,

    /* [IN] The I/O init function */
    _mqx_uint (_CODE_PTR_ init)   (void *, char *),

    /* [IN] The I/O de-init function */
    _mqx_uint (_CODE_PTR_ deinit) (void *, void *),

    /* [IN] The input function */
    _mqx_int  (_CODE_PTR_ recv)   (void *, char *, _mqx_int),

    /* [IN] The output function */
    _mqx_int  (_CODE_PTR_ xmit)   (void *, char *, _mqx_int),

    /* [IN] The I/O ioctl function */
    _mqx_int  (_CODE_PTR_ ioctl)  (void *, _mqx_uint, _mqx_uint_ptr),

    /* [IN] The I/O init data pointer */
    void                         *init_data_ptr
)
{ /* Body */
    IO_I2S_DEVICE_STRUCT_PTR io_dev_ptr;

    io_dev_ptr = (IO_I2S_DEVICE_STRUCT_PTR) _mem_alloc_system_zero(sizeof (IO_I2S_DEVICE_STRUCT));

    if (io_dev_ptr == NULL)
    {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type (io_dev_ptr, MEM_TYPE_IO_I2S_DEVICE_STRUCT);

    io_dev_ptr->DEV_INIT          = init;
    io_dev_ptr->DEV_DEINIT        = deinit;
    io_dev_ptr->DEV_READ          = recv;
    io_dev_ptr->DEV_WRITE         = xmit;
    io_dev_ptr->DEV_IOCTL         = ioctl;
    io_dev_ptr->DEV_INIT_DATA_PTR = init_data_ptr;

    return (_io_dev_install_ext(identifier,
        _io_i2s_open, _io_i2s_close,
        _io_i2s_read, _io_i2s_write,
        _io_i2s_ioctl, _io_i2s_uninstall,
        (void *)io_dev_ptr));

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_i2s_uninstall
* Returned Value   : MQX error code
* Comments         :
*    UnInstall interrupt I2S device.
*
*END**********************************************************************/

_mqx_int _io_i2s_uninstall
(
    /* [IN] The IO device structure for the device */
    IO_DEVICE_STRUCT_PTR   io_dev_ptr
)
{ /* Body */
    IO_I2S_DEVICE_STRUCT_PTR io_i2s_dev_ptr = io_dev_ptr->DRIVER_INIT_PTR;

    if (io_i2s_dev_ptr->COUNT == 0)
    {
        if (io_i2s_dev_ptr->DEV_DEINIT)
        {
            (*io_i2s_dev_ptr->DEV_DEINIT)(io_i2s_dev_ptr, io_i2s_dev_ptr->DEV_INFO_PTR);
        }
        _mem_free (io_i2s_dev_ptr);
        io_dev_ptr->DRIVER_INIT_PTR = NULL;
        return IO_OK;
    } else {
        return IO_ERROR_DEVICE_BUSY;
    }
} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_i2s_open
* Returned Value   : MQX error code
* Comments         :
*    This routine initializes the I2S I/O channel. It acquires
*    memory, then stores information into it about the channel.
*    It then calls the hardware interface function to initialize the channel.
*
*END**********************************************************************/

_mqx_int _io_i2s_open
(
    /* [IN] the file handle for the device being opened */
    MQX_FILE_PTR         fd_ptr,

    /* [IN] the remaining portion of the name of the device */
    char             *open_name_ptr,

    /* [IN] the flags to be used during operation */
    char             *flags
)
{ /* Body */
    IO_DEVICE_STRUCT_PTR io_dev_ptr;
    IO_I2S_DEVICE_STRUCT_PTR io_i2s_dev_ptr;
    _mqx_int result = MQX_OK;

    io_dev_ptr = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
    io_i2s_dev_ptr = (void *)(io_dev_ptr->DRIVER_INIT_PTR);

    _int_disable ();
    if (io_i2s_dev_ptr->COUNT)
    {
        /* Device is already opened */
        _int_enable ();
        return MQX_IO_OPERATION_NOT_AVAILABLE;
    }
    io_i2s_dev_ptr->COUNT = 1;
    _int_enable ();

    io_i2s_dev_ptr->FLAGS = (_mqx_uint)flags;
    fd_ptr->FLAGS         = (_mqx_uint)flags;

    result = (*io_i2s_dev_ptr->DEV_INIT)(io_i2s_dev_ptr, open_name_ptr);
    if (result != MQX_OK)
    {
        io_i2s_dev_ptr->COUNT = 0;
    }
    return result;
} /* Endbody */



/*FUNCTION****************************************************************
*
* Function Name    : _io_i2s_close
* Returned Value   : MQX error code
* Comments         :
*    This routine closes the I2S I/O channel.
*
*END**********************************************************************/

_mqx_int _io_i2s_close
(
    /* [IN] the file handle for the device being closed */
    MQX_FILE_PTR    fd_ptr
)
{ /* Body */
    IO_DEVICE_STRUCT_PTR io_dev_ptr;
    IO_I2S_DEVICE_STRUCT_PTR io_i2s_dev_ptr;
    _mqx_int result = MQX_OK;

    io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
    io_i2s_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

    _int_disable();
    if (io_i2s_dev_ptr->COUNT == 0)
    {
        /* Device is already closed */
        _int_enable ();
        return MQX_IO_OPERATION_NOT_AVAILABLE;
    }
    if (--io_i2s_dev_ptr->COUNT == 0)
    {
        if (io_i2s_dev_ptr->DEV_DEINIT)
        {
            result = (*io_i2s_dev_ptr->DEV_DEINIT)(io_i2s_dev_ptr, io_i2s_dev_ptr->DEV_INFO_PTR);
        }
    }
    _int_enable();

    return result;
} /* Endbody */



/*FUNCTION****************************************************************
*
* Function Name    : _io_i2s_read
* Returned Value   : number of bytes read
* Comments         :
*    Reads bytes into array from I2S input buffers.
*    Returns number of bytes received.
*
*END*********************************************************************/

_mqx_int _io_i2s_read
(
    /* [IN] the handle returned from _fopen */
    MQX_FILE_PTR       fd_ptr,

    /* [IN] where the data are to be stored */
    char           *data_ptr,

    /* [IN] the number of bytes to read */
    _mqx_int           n
)
{  /* Body */
    IO_DEVICE_STRUCT_PTR           io_dev_ptr;
    IO_I2S_DEVICE_STRUCT_PTR       io_i2s_dev_ptr;

    io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
    io_i2s_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

    return (*io_i2s_dev_ptr->DEV_READ)(io_i2s_dev_ptr, data_ptr, n);
}  /* Endbody */



/*FUNCTION****************************************************************
*
* Function Name    : _io_i2s_write
* Returned Value   : number of bytes written
* Comments         :
*    This routine calls the appropriate write routine.
*
*END**********************************************************************/

_mqx_int _io_i2s_write
(
    /* [IN] the handle returned from _fopen */
    MQX_FILE_PTR    fd_ptr,

    /* [IN] where the bytes are stored */
    char        *data_ptr,

    /* [IN] the number of bytes to output */
    _mqx_int        n
)
{  /* Body */
    IO_DEVICE_STRUCT_PTR         io_dev_ptr;
    IO_I2S_DEVICE_STRUCT_PTR io_i2s_dev_ptr;

    io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
    io_i2s_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

    return (*io_i2s_dev_ptr->DEV_WRITE)(io_i2s_dev_ptr, data_ptr, n);
} /* Endbody */



/*FUNCTION*****************************************************************
*
* Function Name    : _io_i2s_ioctl
* Returned Value   : MQX error code
* Comments         :
*    Returns result of I2S ioctl operation.
*
*END*********************************************************************/

_mqx_int _io_i2s_ioctl
(
    /* [IN] the file handle for the device */
    MQX_FILE_PTR    fd_ptr,

    /* [IN] the ioctl command */
    _mqx_uint       cmd,

    /* [IN] the ioctl parameters */
    void           *input_param_ptr
)
{ /* Body */
    IO_DEVICE_STRUCT_PTR         io_dev_ptr;
    IO_I2S_DEVICE_STRUCT_PTR     io_i2s_dev_ptr;
    _mqx_int                     result = MQX_OK;
    _mqx_uint_ptr                param_ptr = (_mqx_uint_ptr)input_param_ptr;

    io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
    io_i2s_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

    switch (cmd) {
        case IO_IOCTL_DEVICE_IDENTIFY :
            /* return the device identify */
            param_ptr[0] = IO_DEV_TYPE_PHYS_I2S_INTERRUPT;
            param_ptr[1] = 0;
            param_ptr[2] = IO_DEV_ATTR_INTERRUPT | IO_DEV_ATTR_READ | IO_DEV_ATTR_WRITE;
            result = MQX_OK;
            break;
        default:
            if (io_i2s_dev_ptr->DEV_IOCTL != NULL)
            {
                result = (*io_i2s_dev_ptr->DEV_IOCTL)(io_i2s_dev_ptr->DEV_INFO_PTR, cmd, param_ptr);
            }
    }

    return result;
} /* Endbody */

/* EOF */
