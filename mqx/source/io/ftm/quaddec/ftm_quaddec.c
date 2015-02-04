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
*   This source file is used to provide the I/O functions of FTM
*   Quadrature Decoder driver to application.
*
*
*END************************************************************************/


#include <mqx_inc.h>
#include <fio.h>
#include <fio_prv.h>
#include <io.h>
#include <io_prv.h>
#include "ftm.h"
#include "ftm_quaddec.h"

extern _mqx_int _io_ftm_quaddec_uninstall(IO_DEVICE_STRUCT_PTR);
extern _mqx_int _io_ftm_quaddec_open
    (FILE_DEVICE_STRUCT_PTR, char*, char*);
extern _mqx_int _io_ftm_quaddec_close(FILE_DEVICE_STRUCT_PTR);
extern _mqx_int _io_ftm_quaddec_read
    (FILE_DEVICE_STRUCT_PTR, char*, _mqx_int);
extern _mqx_int _io_ftm_quaddec_write
    (FILE_DEVICE_STRUCT_PTR, char*, _mqx_int);
extern _mqx_int _io_ftm_quaddec_ioctl
    (FILE_DEVICE_STRUCT_PTR, _mqx_uint, void*);

/*FUNCTION****************************************************************
*
* Function Name    : _io_ftm_quaddec_install
* Returned Value   : MQX error code
* Comments         :
*    Install the ftm quaddecoder device.
*
*END**********************************************************************/
_mqx_uint _io_ftm_quaddec_install
   (
      /* [IN] A string that identifies the device for fopen */
      char*                 identifier,

      /* [IN] The I/O init function */
      _mqx_uint (_CODE_PTR_ init)(void*, char*),

      /* [IN] The I/O de-init function */
      _mqx_uint (_CODE_PTR_ deinit)(void*),

      /* [IN] The input function */
      _mqx_int  (_CODE_PTR_ recv)(void*, char*, _mqx_int),

      /* [IN] The output function */
      _mqx_int  (_CODE_PTR_ xmit)(void*, char*, _mqx_int),

      /* [IN] The I/O ioctl function */
      _mqx_int  (_CODE_PTR_ ioctl)(void*, _mqx_uint, _mqx_uint_ptr),

      /* [IN] The I/O init data pointer */
      void*     init_data_ptr
   )

{ /* Body */
   IO_FTM_QUADDEC_STRUCT_PTR io_ftm_quaddec_ptr;

   io_ftm_quaddec_ptr =
    (IO_FTM_QUADDEC_STRUCT_PTR)_mem_alloc_system_zero
    ((_mem_size)sizeof (IO_FTM_QUADDEC_STRUCT));
   if (io_ftm_quaddec_ptr == NULL)
   {
      return MQX_OUT_OF_MEMORY;
   }
   _mem_set_type (io_ftm_quaddec_ptr, MEM_TYPE_IO_FTM_QUADDEC_STRUCT);

   io_ftm_quaddec_ptr->DEV_INIT          = init;
   io_ftm_quaddec_ptr->DEV_DEINIT        = deinit;
   io_ftm_quaddec_ptr->DEV_READ          = recv;
   io_ftm_quaddec_ptr->DEV_WRITE         = xmit;
   io_ftm_quaddec_ptr->DEV_IOCTL         = ioctl;
   io_ftm_quaddec_ptr->DEV_INIT_DATA_PTR = init_data_ptr;

   return (_io_dev_install_ext (identifier,
      _io_ftm_quaddec_open,  _io_ftm_quaddec_close,
      _io_ftm_quaddec_read,  _io_ftm_quaddec_write,
      _io_ftm_quaddec_ioctl, _io_ftm_quaddec_uninstall,
      (void *)io_ftm_quaddec_ptr));

} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _io_ftm_quaddec_uninstall
* Returned Value   : MQX error code
* Comments         :
*    Uninstall a ftm quaddec device.
*
*END**********************************************************************/
_mqx_int _io_ftm_quaddec_uninstall
   (
      /* [IN] The IO device structure for the device */
      IO_DEVICE_STRUCT_PTR   io_dev_ptr
   )
{ /* Body */
   IO_FTM_QUADDEC_STRUCT_PTR io_ftm_quaddec_ptr = io_dev_ptr->DRIVER_INIT_PTR;

   if (io_ftm_quaddec_ptr->COUNT == 0)
   {
      if (io_ftm_quaddec_ptr->DEV_DEINIT)
      {
         (*io_ftm_quaddec_ptr->DEV_DEINIT)(io_ftm_quaddec_ptr);
      }
      _mem_free (io_ftm_quaddec_ptr);
      io_dev_ptr->DRIVER_INIT_PTR = NULL;
      return IO_OK;
   } else {
      return IO_ERROR_DEVICE_BUSY;
   }

} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _io_ftm_quaddec_open
* Returned Value   : MQX error code
* Comments         :
*    This routine initializes the ftm quaddec channel. It alloc
*    memory to stores context information about the channel.
*    It then calls the hardware interface function to initialize the channel.
*
*END**********************************************************************/
_mqx_int _io_ftm_quaddec_open
   (
      /* [IN] the file handle for the device being opened */
      FILE_DEVICE_STRUCT_PTR          fd_ptr,

      /* [IN] the remaining portion of the name of the device */
      char*                           open_name_ptr,

      /* [IN] the flags to be used during operation */
      char*                           flags
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR               io_dev_ptr;
   IO_FTM_QUADDEC_STRUCT_PTR          io_ftm_quaddec_ptr;
   _mqx_int                           result = MQX_OK;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   io_ftm_quaddec_ptr = (void *)(io_dev_ptr->DRIVER_INIT_PTR);

   _int_disable ();
   if (io_ftm_quaddec_ptr->COUNT)
   {
      /* Device is already opened */
      _int_enable ();
      return MQX_IO_OPERATION_NOT_AVAILABLE;
   }
   io_ftm_quaddec_ptr->COUNT = 1;
   _int_enable ();

   io_ftm_quaddec_ptr->FLAGS = (_mqx_uint)flags;
   fd_ptr->FLAGS         = (_mqx_uint)flags;

   result = (*io_ftm_quaddec_ptr->DEV_INIT)
    ((void *)io_ftm_quaddec_ptr, open_name_ptr);
   if (result != MQX_OK)
   {
      io_ftm_quaddec_ptr->COUNT = 0;
   }

   return result;

} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _io_ftm_quaddec_close
* Returned Value   : MQX error code
* Comments         :
*    This routine closes the ftm quaddec channel.
*
*END**********************************************************************/
_mqx_int _io_ftm_quaddec_close
   (
      /* [IN] the file handle for the device being closed */
      FILE_DEVICE_STRUCT_PTR       fd_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_FTM_QUADDEC_STRUCT_PTR       io_ftm_quaddec_ptr;
   _mqx_int                        result = MQX_OK;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   io_ftm_quaddec_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

   _int_disable ();
   if (--io_ftm_quaddec_ptr->COUNT == 0)
   {
      if (io_ftm_quaddec_ptr->DEV_DEINIT)
      {
         result = (*io_ftm_quaddec_ptr->DEV_DEINIT)(io_ftm_quaddec_ptr);
      }
   }
   _int_enable ();
   return result;

} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _io_ftm_quaddec_read
* Returned Value   : number of bytes read
* Comments         :
*    This routine calls the appropriate read routine.
*    This driver does not provide this interface. It should not be used.
*
*END*********************************************************************/
_mqx_int _io_ftm_quaddec_read
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR       fd_ptr,

      /* [IN] where the characters are to be stored */
      char*                        data_ptr,

      /* [IN] the number of bytes to read */
      _mqx_int                     n
   )
{  /* Body */

   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_FTM_QUADDEC_STRUCT_PTR       io_ftm_quaddec_ptr;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   io_ftm_quaddec_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

   return (*io_ftm_quaddec_ptr->DEV_READ)(io_ftm_quaddec_ptr, data_ptr, n);

}  /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _io_ftm_quaddec_write
* Returned Value   : number of bytes written
* Comments         :
*    This routine calls the appropriate write routine.
*    This driver does not provide this interface. It should not be used.
*
*END**********************************************************************/
_mqx_int _io_ftm_quaddec_write
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR       fd_ptr,

      /* [IN] where the bytes are stored */
      char*                        data_ptr,

      /* [IN] the number of bytes to output */
      _mqx_int                     n
   )
{  /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_FTM_QUADDEC_STRUCT_PTR       io_ftm_quaddec_ptr;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   io_ftm_quaddec_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

   return (*io_ftm_quaddec_ptr->DEV_WRITE)(io_ftm_quaddec_ptr, data_ptr, n);

} /* Endbody */

/*FUNCTION*****************************************************************
*
* Function Name    : _io_ftm_quaddec_ioctl
* Returned Value   : MQX error code
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/
_mqx_int _io_ftm_quaddec_ioctl
   (
      /* [IN] the file handle for the device */
      FILE_DEVICE_STRUCT_PTR       fd_ptr,

      /* [IN] the ioctl command */
      _mqx_uint                    cmd,

      /* [IN] the ioctl parameters */
      void*                        input_param_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_FTM_QUADDEC_STRUCT_PTR       io_ftm_quaddec_ptr;
   _mqx_int                        result = MQX_OK;
   _mqx_uint_ptr                   param_ptr = (_mqx_uint_ptr)input_param_ptr;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   io_ftm_quaddec_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

   if (io_ftm_quaddec_ptr->DEV_IOCTL != NULL) {
      result = (*io_ftm_quaddec_ptr->DEV_IOCTL)(io_ftm_quaddec_ptr,
                                                cmd,
                                                param_ptr);
   }

   return result;
} /* Endbody */

/* EOF */

