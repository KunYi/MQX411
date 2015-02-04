#ifndef _i2s_int_prv_h_
#define _i2s_int_prv_h_ 1
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

/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/


/*---------------------------------------------------------------------
**
** IO I2S DEVICE STRUCT
**
** This structure used to store information about an I2S interrupt io device
** for the IO device table
*/
typedef struct io_i2s_device_struct
{

    /* The I/O init function */
    _mqx_uint (_CODE_PTR_ DEV_INIT)(void *, char *);

    /* The I/O deinit function */
    _mqx_uint (_CODE_PTR_ DEV_DEINIT)(void *, void *);

    /* [IN] The READ function */
    _mqx_int  (_CODE_PTR_ DEV_READ)(void *, char *, _mqx_int);

    /* [IN] The WRITE function */
    _mqx_int  (_CODE_PTR_ DEV_WRITE)(void *, char *, _mqx_int);

    /* The ioctl function, (change bauds etc) */
    _mqx_int  (_CODE_PTR_ DEV_IOCTL)(void *, _mqx_uint, _mqx_uint_ptr);

    /* The I/O channel initialization data */
    void      *DEV_INIT_DATA_PTR;

    /* Device specific information */
    void      *DEV_INFO_PTR;

    /* Open count for number of accessing file descriptors */
    _mqx_uint  COUNT;

    /* Open flags for this channel */
    _mqx_uint  FLAGS;

} IO_I2S_DEVICE_STRUCT, * IO_I2S_DEVICE_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

    /* I/O prototypes */
    extern _mqx_int _io_i2s_open(MQX_FILE_PTR fd_ptr, char *open_name_ptr, char *flags);
    extern _mqx_int _io_i2s_close(MQX_FILE_PTR fd_ptr);
    extern _mqx_int _io_i2s_read(MQX_FILE_PTR fd_ptr, char *data_ptr, _mqx_int n);
    extern _mqx_int _io_i2s_write(MQX_FILE_PTR fd_ptr, char *data_ptr, _mqx_int n);
    extern _mqx_int _io_i2s_ioctl(MQX_FILE_PTR fd_ptr, _mqx_uint cmd, void *input_param_ptr);
    extern _mqx_int _io_i2s_uninstall(IO_DEVICE_STRUCT_PTR io_dev_ptr);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
