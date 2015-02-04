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
*   This file contains the ESAI VPORT I/O driver functions.
*
*
*END************************************************************************/


#include <mqx_inc.h>
#include <fio.h>
#include <fio_prv.h>
#include <io.h>
#include <io_prv.h>
#include "esai_vport_prv.h"
#include "esai_vport.h"


/*FUNCTION****************************************************************
*
* Function Name    : _io_esai_vport_install
* Returned Value   : MQX error code
* Comments         :
*    Install the ESAI VPORT device.
*
*END**********************************************************************/
_mqx_uint _io_esai_vport_install
   (
      /* [IN] A string that identifies the device for fopen */
      char *               identifier,

      /* [IN] The I/O init function */
      _mqx_uint (_CODE_PTR_ init)(void *, void * *, char *, void *),

      /* [IN] The I/O de-init function */
      _mqx_uint (_CODE_PTR_ deinit)(void *, void *),

      /* [IN] The input function */
      _mqx_int  (_CODE_PTR_ recv)(void *, char *, _mqx_int),

      /* [IN] The output function */
      _mqx_int  (_CODE_PTR_ xmit)(void *, char *, _mqx_int),

      /* [IN] The I/O ioctl function */
      _mqx_int  (_CODE_PTR_ ioctl)(void *, _mqx_uint, _mqx_uint_ptr),

      /* [IN] The I/O init data void * */
      void *               init_data_ptr,

      /* [IN] functions for pcm manager*/
      ESAI_VPORT_PCMM_FUNCS_STRUCT_PTR   pcmm_funcs_ptr,

      /* [IN] pointer to ESAI VPORT context used by access ESAI VPORT*/
      ESAI_FIFO_CONTEXT_STRUCT_PTR esai_fifo_ptr

   )

{ /* Body */
   IO_ESAI_VPORT_DEVICE_STRUCT_PTR esai_fifo_io_dev_ptr;

#if (DISCFG_OS_DRV_ENABLE_PCMMGR || ESAI_VPORT_REGISTER_HANDLE_TO_TEST)
   PCMM_AUDSRC_DRV_ID drv_id;
#endif

   esai_fifo_io_dev_ptr = (IO_ESAI_VPORT_DEVICE_STRUCT_PTR)
        _mem_alloc_system_zero ((_mem_size)sizeof (IO_ESAI_VPORT_DEVICE_STRUCT));
   if (esai_fifo_io_dev_ptr == NULL)
   {
      return MQX_OUT_OF_MEMORY;
   }
   _mem_set_type (esai_fifo_io_dev_ptr, MEM_TYPE_IO_ESAI_VPORT_DEVICE_STRUCT);

   esai_fifo_io_dev_ptr->DEV_INIT          = init;
   esai_fifo_io_dev_ptr->DEV_DEINIT        = deinit;
   esai_fifo_io_dev_ptr->DEV_READ          = recv;
   esai_fifo_io_dev_ptr->DEV_WRITE         = xmit;
   esai_fifo_io_dev_ptr->DEV_IOCTL         = ioctl;
   esai_fifo_io_dev_ptr->DEV_INIT_DATA_PTR = init_data_ptr;
   esai_fifo_io_dev_ptr->PCMM_DRV_FUNCS    = pcmm_funcs_ptr;
   esai_fifo_io_dev_ptr->FIFO_CONTEXT_PTR  = esai_fifo_ptr;

   if (NULL != pcmm_funcs_ptr) {
     /*register to pcm manager*/

     /*set pcm handle to a valid value*/
     esai_fifo_io_dev_ptr->PCMM_DRV_HANDLE = PCMM_HANDLE_ESAI_VALID;
   }

   return (_io_dev_install_ext (identifier,
      _io_esai_vport_open,  _io_esai_vport_close,
      _io_esai_vport_read,  _io_esai_vport_write,
      _io_esai_vport_ioctl, _io_esai_vport_uninstall,
      (void *)esai_fifo_io_dev_ptr));

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_esai_vport_uninstall
* Returned Value   : MQX error code
* Comments         :
*    Uninstall a polled ESAI VPORT device.
*
*END**********************************************************************/

_mqx_int _io_esai_vport_uninstall
   (
      /* [IN] The IO device structure for the device */
      IO_DEVICE_STRUCT_PTR   io_dev_ptr
   )
{ /* Body */
    IO_ESAI_VPORT_DEVICE_STRUCT_PTR esai_fifo_io_dev_ptr =
        io_dev_ptr->DRIVER_INIT_PTR;

    if (esai_fifo_io_dev_ptr->COUNT == 0)
    {
      if (esai_fifo_io_dev_ptr->DEV_DEINIT)
      {
         (*esai_fifo_io_dev_ptr->DEV_DEINIT)(
            esai_fifo_io_dev_ptr->DEV_INIT_DATA_PTR,
             esai_fifo_io_dev_ptr->DEV_INFO_PTR);
      }
      _mem_free (esai_fifo_io_dev_ptr);
      io_dev_ptr->DRIVER_INIT_PTR = NULL;
      return IO_OK;
    } else {
      return IO_ERROR_DEVICE_BUSY;
    }

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_esai_vport_open
* Returned Value   : MQX error code
* Comments         :
*    This routine initializes the ESAI VPORT channel. It acquires
*    memory, then stores information into it about the channel.
*    It then calls the hardware interface function to initialize the channel.
*
*END**********************************************************************/

_mqx_int _io_esai_vport_open
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
   IO_ESAI_VPORT_DEVICE_STRUCT_PTR    esai_fifo_io_dev_ptr;
   _mqx_int                           result = MQX_OK;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   esai_fifo_io_dev_ptr = (void *)(io_dev_ptr->DRIVER_INIT_PTR);

   _int_disable ();
   if (esai_fifo_io_dev_ptr->COUNT)
   {
      /* Device is already opened */
      _int_enable ();
      return MQX_IO_OPERATION_NOT_AVAILABLE;
   }
   esai_fifo_io_dev_ptr->COUNT = 1;
   _int_enable ();

   esai_fifo_io_dev_ptr->FLAGS = (_mqx_uint)flags;
   fd_ptr->FLAGS         = (_mqx_uint)flags;

   result = (*esai_fifo_io_dev_ptr->DEV_INIT)(
        (void *)esai_fifo_io_dev_ptr->DEV_INIT_DATA_PTR,
        &esai_fifo_io_dev_ptr->DEV_INFO_PTR, open_name_ptr,
        esai_fifo_io_dev_ptr->FIFO_CONTEXT_PTR);
   if (result != MQX_OK)
   {
      esai_fifo_io_dev_ptr->COUNT = 0;
   }

   return result;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_esai_vport_close
* Returned Value   : MQX error code
* Comments         :
*    This routine closes the ESAI VPORT channel.
*
*END**********************************************************************/

_mqx_int _io_esai_vport_close
   (
      /* [IN] the file handle for the device being closed */
      FILE_DEVICE_STRUCT_PTR       fd_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_ESAI_VPORT_DEVICE_STRUCT_PTR esai_fifo_io_dev_ptr;
   _mqx_int                        result = MQX_OK;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   esai_fifo_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

   _int_disable ();
   if (esai_fifo_io_dev_ptr->COUNT == 0) {
      _int_enable ();
      return MQX_OK;
   } else {
      esai_fifo_io_dev_ptr->COUNT--;
   }
   _int_enable ();

   if (0 == esai_fifo_io_dev_ptr->COUNT) {
      if (esai_fifo_io_dev_ptr->DEV_DEINIT) {
         result = (*esai_fifo_io_dev_ptr->DEV_DEINIT)(esai_fifo_io_dev_ptr,
            esai_fifo_io_dev_ptr->DEV_INFO_PTR);
      }
   }
   return result;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_esai_vport_read
* Returned Value   : number of bytes read
* Comments         :
*    This routine calls the appropriate read routine.
*
*END*********************************************************************/

_mqx_int _io_esai_vport_read
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR       fd_ptr,

      /* [IN] where the characters are to be stored */
      char *                     data_ptr,

      /* [IN] the number of bytes to read */
      _mqx_int                     n
   )
{  /* Body */

   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_ESAI_VPORT_DEVICE_STRUCT_PTR esai_fifo_io_dev_ptr;
   uint32_t                         error;
   _mqx_int                        result, ret;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   esai_fifo_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

   ret = (*esai_fifo_io_dev_ptr->DEV_READ)(esai_fifo_io_dev_ptr, data_ptr, n);

   if (esai_fifo_io_dev_ptr->DEV_IOCTL != NULL) {
       error = 0;
       result = (*esai_fifo_io_dev_ptr->DEV_IOCTL)(
           esai_fifo_io_dev_ptr, IO_IOCTL_ESAI_VPORT_GET_ERROR, &error);

       if (result == MQX_OK)
          fd_ptr->ERROR = error;
   }

   return ret;

}  /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_esai_vport_write
* Returned Value   : number of bytes written
* Comments         :
*    This routine calls the appropriate write routine.
*
*END**********************************************************************/

_mqx_int _io_esai_vport_write
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR       fd_ptr,

      /* [IN] where the bytes are stored */
      char *                     data_ptr,

      /* [IN] the number of bytes to output */
      _mqx_int                     n
   )
{  /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_ESAI_VPORT_DEVICE_STRUCT_PTR esai_fifo_io_dev_ptr;
   uint32_t                         error;
   _mqx_int                        result, ret;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   esai_fifo_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

   ret = (*esai_fifo_io_dev_ptr->DEV_WRITE)(esai_fifo_io_dev_ptr, data_ptr, n);

   if (esai_fifo_io_dev_ptr->DEV_IOCTL != NULL) {
       error = 0;
       result = (*esai_fifo_io_dev_ptr->DEV_IOCTL)(
           esai_fifo_io_dev_ptr, IO_IOCTL_ESAI_VPORT_GET_ERROR, &error);

       if (result == MQX_OK)
          fd_ptr->ERROR = error;
   }

   return ret;
} /* Endbody */


/*FUNCTION*****************************************************************
*
* Function Name    : _io_esai_vport_ioctl
* Returned Value   : MQX error code
* Comments         :
*    Returns result of ESAI VPORT ioctl operation.
*
*END*********************************************************************/

_mqx_int _io_esai_vport_ioctl
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
   IO_ESAI_VPORT_DEVICE_STRUCT_PTR esai_fifo_io_dev_ptr;
   _mqx_int                        result = MQX_OK;
   _mqx_uint_ptr                   param_ptr = (_mqx_uint_ptr)input_param_ptr;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   esai_fifo_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

    switch (cmd) {
        case IO_IOCTL_DEVICE_IDENTIFY :
            /* return the device identify */
            param_ptr[0] = IO_DEV_TYPE_PHYS_ESAI_FIFO;
            param_ptr[1] = 0;
            param_ptr[2] = IO_DEV_ATTR_READ | IO_DEV_ATTR_WRITE;
            result = MQX_OK;
            break;
        default:
            if (esai_fifo_io_dev_ptr->DEV_IOCTL != NULL) {
                result = (*esai_fifo_io_dev_ptr->DEV_IOCTL)(
                    esai_fifo_io_dev_ptr, cmd, param_ptr);
            }
    }

   return result;
} /* Endbody */

/* EOF */
