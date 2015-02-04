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
*   This file contains the ASRC ANSI standard driver functions.
*
*
*END************************************************************************/


#include <mqx_inc.h>
#include <fio.h>
#include <fio_prv.h>
#include <io.h>
#include <io_prv.h>
#include <asrc_prv.h>
#include <asrc.h>



/*FUNCTION****************************************************************
*
* Function Name    : _io_asrc_uninstall
* Returned Value   : MQX error code
* Comments         :
*    Uninstall a asrc device.
*
*END**********************************************************************/

_mqx_int _io_asrc_uninstall
   (
      /* [IN] The IO device structure for the device */
      IO_DEVICE_STRUCT_PTR   io_dev_ptr
   )
{ /* Body */
    IO_ASRC_DEVICE_STRUCT_PTR asrc_io_dev_ptr =
            io_dev_ptr->DRIVER_INIT_PTR;

    if (asrc_io_dev_ptr->COUNT == 0)
    {
        if (asrc_io_dev_ptr->DEV_DEINIT)
        {
            (*asrc_io_dev_ptr->DEV_DEINIT)(asrc_io_dev_ptr);
        }
        _mutex_destroy(&asrc_io_dev_ptr->DEV_MUTEX);
        _mem_free (asrc_io_dev_ptr);
        io_dev_ptr->DRIVER_INIT_PTR = NULL;
        return IO_OK;
    } else {
        return IO_ERROR_DEVICE_BUSY;
    }

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_asrc_open
* Returned Value   : MQX error code
* Comments         :
*    This routine opens the asrc device file. It acquires
*    memory, then stores information into it about the device.
*    It then calls the hardware interface function to open the deivce.
*
*END**********************************************************************/

_mqx_int _io_asrc_open
   (
      /* [IN] the file handle for the device being opened */
      FILE_DEVICE_STRUCT_PTR          fd_ptr,

      /* [IN] the remaining portion of the name of the device */
      char *                        open_name_ptr,

      /* [IN] the flags to be used during operation */
      char *                        flags
   )
{ /* Body */
    IO_DEVICE_STRUCT_PTR               io_dev_ptr;
    IO_ASRC_DEVICE_STRUCT_PTR    asrc_io_dev_ptr;
    _mqx_int                           result = MQX_OK;

    io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
    asrc_io_dev_ptr = (void *)(io_dev_ptr->DRIVER_INIT_PTR);

    _mutex_lock(&asrc_io_dev_ptr->DEV_MUTEX);

    if (asrc_io_dev_ptr->DEV_INFO_PTR == NULL) {
        if (asrc_io_dev_ptr->DEV_INIT) {
            result = (*asrc_io_dev_ptr->DEV_INIT)(
                (void *)asrc_io_dev_ptr->DEV_INIT_DATA_PTR,
                &asrc_io_dev_ptr->DEV_INFO_PTR);

            if (result != MQX_OK) {
                _mutex_unlock(&asrc_io_dev_ptr->DEV_MUTEX);
                return result;
            }
        }
    }

    asrc_io_dev_ptr->FLAGS = (_mqx_uint)flags;
    fd_ptr->FLAGS         = (_mqx_uint)flags;

    if (asrc_io_dev_ptr->DEV_OPEN) {
        result = (*asrc_io_dev_ptr->DEV_OPEN)((void *)asrc_io_dev_ptr,
            (void *)fd_ptr, flags);
        if (result == MQX_OK) {
          /*allow many accesses to this device at same time*/
          asrc_io_dev_ptr->COUNT++;
        }
    }

    _mutex_unlock(&asrc_io_dev_ptr->DEV_MUTEX);

    return result;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_asrc_close
* Returned Value   : MQX error code
* Comments         :
*    This routine closes the asrc device
*
*END**********************************************************************/

_mqx_int _io_asrc_close
   (
      /* [IN] the file handle for the device being closed */
      FILE_DEVICE_STRUCT_PTR       fd_ptr
   )
{ /* Body */
    IO_DEVICE_STRUCT_PTR            io_dev_ptr;
    IO_ASRC_DEVICE_STRUCT_PTR asrc_io_dev_ptr;
    _mqx_int                        result = MQX_OK;

    io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
    asrc_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

    _mutex_lock(&asrc_io_dev_ptr->DEV_MUTEX);

    if (asrc_io_dev_ptr->COUNT > 0) {
        if (asrc_io_dev_ptr->DEV_CLOSE) {
            result = (*asrc_io_dev_ptr->DEV_CLOSE)(asrc_io_dev_ptr,
                (void *)fd_ptr);
        }
        asrc_io_dev_ptr->COUNT--;

        if (0 == asrc_io_dev_ptr->COUNT) {
            if (asrc_io_dev_ptr->DEV_DEINIT) {
                (*asrc_io_dev_ptr->DEV_DEINIT)(asrc_io_dev_ptr);
            }
        }
    }

    _mutex_unlock(&asrc_io_dev_ptr->DEV_MUTEX);

    return result;

} /* Endbody */



/*FUNCTION*****************************************************************
*
* Function Name    : _io_asrc_ioctl
* Returned Value   : MQX error code
* Comments         :
*    Returns result of asrc operation.
*
*END*********************************************************************/

_mqx_int _io_asrc_ioctl
   (
      /* [IN] the file handle for the device */
      FILE_DEVICE_STRUCT_PTR       fd_ptr,

      /* [IN] the ioctl command */
      _mqx_uint                    cmd,

      /* [IN] the ioctl parameters */
      void *                      input_param_ptr
   )
{ /* Body */
    IO_DEVICE_STRUCT_PTR            io_dev_ptr;
    IO_ASRC_DEVICE_STRUCT_PTR asrc_io_dev_ptr;
    _mqx_int                        result = MQX_OK;
    _mqx_uint_ptr                   param_ptr = (_mqx_uint_ptr)input_param_ptr;

    io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
    asrc_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

    _mutex_lock(&asrc_io_dev_ptr->DEV_MUTEX);

    switch (cmd) {
        case IO_IOCTL_DEVICE_IDENTIFY :
            /* return the device identify */
            param_ptr[0] = IO_DEV_TYPE_PHYS_ASRC;
            param_ptr[1] = 0;
            param_ptr[2] = 0;
            result = MQX_OK;
            break;
        default:
            if (asrc_io_dev_ptr->DEV_IOCTL != NULL) {
                result = (*asrc_io_dev_ptr->DEV_IOCTL)(asrc_io_dev_ptr,
                    (void *)fd_ptr, cmd, param_ptr);
            }
    }

    _mutex_unlock(&asrc_io_dev_ptr->DEV_MUTEX);

    return result;
} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _io_asrc_install
* Returned Value   : MQX error code
* Comments         :
*    Install the IO asrc device.
*
*END**********************************************************************/

_mqx_uint _io_asrc_install
   (
      /* [IN] A string that identifies the device for fopen */
      char *               identifier,

      /* [IN] The I/O init function */
      _mqx_uint (_CODE_PTR_ init)(void *, void * *),

      /* The I/O open function */
      _mqx_uint (_CODE_PTR_ open)(void *, void *, char *),

      /* The I/O open function */
      _mqx_uint (_CODE_PTR_ close)(void *, void *),

      /* [IN] The I/O de-init function */
      _mqx_uint (_CODE_PTR_ deinit)(void *),

      /* [IN] The I/O ioctl function */
      _mqx_int  (_CODE_PTR_ ioctl)(void *, void *, _mqx_uint, _mqx_uint_ptr),

      /* [IN] The I/O init data pointer */
      void *               init_data_ptr,

      /* [IN] functions for pcm manager*/
      ASRC_PCMM_FUNCS_STRUCT_PTR   pcmm_funcs_ptr

   )

{ /* Body */
   IO_ASRC_DEVICE_STRUCT_PTR asrc_io_dev_ptr;
   MUTEX_ATTR_STRUCT mutex_attr;

   asrc_io_dev_ptr = (IO_ASRC_DEVICE_STRUCT_PTR)_mem_alloc_system_zero(
        (_mem_size)sizeof (IO_ASRC_DEVICE_STRUCT));
   if (asrc_io_dev_ptr == NULL)
   {
      return MQX_OUT_OF_MEMORY;
   }
   _mem_set_type (asrc_io_dev_ptr, MEM_TYPE_IO_ASRC_DEVICE_STRUCT);

   asrc_io_dev_ptr->DEV_INIT          = init;
   asrc_io_dev_ptr->DEV_OPEN          = open;
   asrc_io_dev_ptr->DEV_CLOSE         = close;
   asrc_io_dev_ptr->DEV_DEINIT        = deinit;
   asrc_io_dev_ptr->DEV_IOCTL         = ioctl;
   asrc_io_dev_ptr->DEV_INIT_DATA_PTR = init_data_ptr;
   asrc_io_dev_ptr->DEV_INFO_PTR      = NULL;
   asrc_io_dev_ptr->COUNT             = 0;

   asrc_io_dev_ptr->PCMM_DRV_FUNCS_PTR = pcmm_funcs_ptr;

   _mutatr_init(&mutex_attr);
   _mutatr_set_sched_protocol(&mutex_attr, MUTEX_PRIO_INHERIT);
   _mutex_init(&asrc_io_dev_ptr->DEV_MUTEX, &mutex_attr);

   if (NULL != pcmm_funcs_ptr) {
     /*register to pcm manager*/

     /*set pcm handle to a valid value*/
     asrc_io_dev_ptr->PCMM_DRV_HANDLE = PCMM_HANDLE_ASRC_VALID;

   }

   return (_io_dev_install_ext (identifier,
      _io_asrc_open,  _io_asrc_close,
      NULL,  NULL,
      _io_asrc_ioctl, _io_asrc_uninstall,
      (void *)asrc_io_dev_ptr));

} /* Endbody */

/* EOF */
