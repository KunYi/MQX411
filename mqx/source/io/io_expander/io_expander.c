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
*   This file contains the IO expander I/O driver functions.
*
*
*END************************************************************************/


#include <mqx_inc.h>
#include <bsp.h>
#include <fio.h>
#include <fio_prv.h>
#include <io.h>
#include <io_prv.h>
#include <io_expander_prv.h>
#include <io_expander.h>


/*FUNCTION****************************************************************
*
* Function Name    : _io_expander_install
* Returned Value   : MQX error code
* Comments         :
*    Install the IO expander device.
*
*END**********************************************************************/

uint32_t _io_expander_install
   (
      /* [IN] A string that identifies the device for fopen */
      char *               identifier,

      /* [IN] The I/O init function */
      uint32_t (*init)(void *, void **),

      /* The I/O open function */
      uint32_t (*open)(void *, void **, char *),

      /* The I/O open function */
      uint32_t (*close)(void *, void *),

      /* [IN] The I/O de-init function */
      uint32_t (*deinit)(void *),

      /* [IN] The input function */
      int32_t  (*recv)(void *, void *, char *, int32_t),

      /* [IN] The output function */
      int32_t  (*xmit)(void *, void *, char *, int32_t),

      /* [IN] The I/O ioctl function */
      int32_t  (*ioctl)(void *, void *, uint32_t, uint32_t *),

      /* [IN] The I/O init data void * */
      void *               init_data_ptr

   )

{ /* Body */
   IO_EXPANDER_DEVICE_STRUCT_PTR expander_io_dev_ptr;

   expander_io_dev_ptr = (IO_EXPANDER_DEVICE_STRUCT_PTR)_mem_alloc_system_zero(
        (_mem_size)sizeof (IO_EXPANDER_DEVICE_STRUCT));
   if (expander_io_dev_ptr == NULL)
   {
      return MQX_OUT_OF_MEMORY;
   }
   _mem_set_type (expander_io_dev_ptr, MEM_TYPE_IO_EXPANDER_DEVICE_STRUCT);

   expander_io_dev_ptr->DEV_INIT          = init;
   expander_io_dev_ptr->DEV_OPEN          = open;
   expander_io_dev_ptr->DEV_CLOSE         = close;
   expander_io_dev_ptr->DEV_DEINIT        = deinit;
   expander_io_dev_ptr->DEV_READ          = recv;
   expander_io_dev_ptr->DEV_WRITE         = xmit;
   expander_io_dev_ptr->DEV_IOCTL         = ioctl;
   expander_io_dev_ptr->DEV_INIT_DATA_PTR = init_data_ptr;
   expander_io_dev_ptr->DEV_INFO_PTR      = NULL;
   expander_io_dev_ptr->COUNT             = 0;

   _bsp_ioexp_io_init();
   return (_io_dev_install_ext (identifier,
      _io_expander_open,  _io_expander_close,
      _io_expander_read,  _io_expander_write,
      _io_expander_ioctl, _io_expander_uninstall,
      (void *)expander_io_dev_ptr));

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_expander_uninstall
* Returned Value   : MQX error code
* Comments         :
*    Uninstall a IO expander device.
*
*END**********************************************************************/

int32_t _io_expander_uninstall
   (
      /* [IN] The IO device structure for the device */
      IO_DEVICE_STRUCT_PTR   io_dev_ptr
   )
{ /* Body */
   IO_EXPANDER_DEVICE_STRUCT_PTR expander_io_dev_ptr =
            io_dev_ptr->DRIVER_INIT_PTR;

   if (expander_io_dev_ptr->COUNT == 0)
   {
      if (expander_io_dev_ptr->DEV_DEINIT)
      {
         (*expander_io_dev_ptr->DEV_DEINIT)(expander_io_dev_ptr);
      }
      _mem_free (expander_io_dev_ptr);
      io_dev_ptr->DRIVER_INIT_PTR = NULL;
      return IO_OK;
   } else {
      return IO_ERROR_DEVICE_BUSY;
   }

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_expander_open
* Returned Value   : MQX error code
* Comments         :
*    This routine opens the IO expander I/O channel. It acquires
*    memory, then stores information into it about the device.
*    It then calls the hardware interface function to open the deivce.
*
*END**********************************************************************/

int32_t _io_expander_open
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
   IO_EXPANDER_DEVICE_STRUCT_PTR    expander_io_dev_ptr;
   int32_t                           result = MQX_OK;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   expander_io_dev_ptr = (void *)(io_dev_ptr->DRIVER_INIT_PTR);

   if (expander_io_dev_ptr->DEV_INFO_PTR == NULL) {
       if (expander_io_dev_ptr->DEV_INIT) {
         result = (*expander_io_dev_ptr->DEV_INIT)(
            (void *)expander_io_dev_ptr->DEV_INIT_DATA_PTR,
            &expander_io_dev_ptr->DEV_INFO_PTR);

         if (result != MQX_OK)
            return result;
       }
   }

   _int_disable ();
   /*allow many accesses to this device at same time*/
   expander_io_dev_ptr->COUNT++;
   _int_enable ();

   expander_io_dev_ptr->FLAGS = (uint32_t)flags;
   fd_ptr->FLAGS         = (uint32_t)flags;

   if (expander_io_dev_ptr->DEV_OPEN) {
       result = (*expander_io_dev_ptr->DEV_OPEN)((void *)expander_io_dev_ptr,
            &(fd_ptr->DEV_DATA_PTR), open_name_ptr);
       if (result != MQX_OK)
       {
          expander_io_dev_ptr->COUNT--;
       }
   }

   return result;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_expander_close
* Returned Value   : MQX error code
* Comments         :
*    This routine closes the IO expander device
*
*END**********************************************************************/

int32_t _io_expander_close
   (
      /* [IN] the file handle for the device being closed */
      FILE_DEVICE_STRUCT_PTR       fd_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_EXPANDER_DEVICE_STRUCT_PTR expander_io_dev_ptr;
   int32_t                        result = MQX_OK;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   expander_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

   _int_disable ();
   if (expander_io_dev_ptr->COUNT > 0)
   {
      if (expander_io_dev_ptr->DEV_CLOSE)
      {
         result = (*expander_io_dev_ptr->DEV_CLOSE)(expander_io_dev_ptr,
            fd_ptr->DEV_DATA_PTR);
      }
      expander_io_dev_ptr->COUNT--;
   }

   if (0 == expander_io_dev_ptr->COUNT) {
      if (expander_io_dev_ptr->DEV_DEINIT) {
         result = (*expander_io_dev_ptr->DEV_DEINIT)(expander_io_dev_ptr);
      }
   }
   _int_enable ();
   return result;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_expander_read
* Returned Value   : number of bytes read
* Comments         :
*    This routine calls the appropriate read routine.
*
*END*********************************************************************/

int32_t _io_expander_read
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR       fd_ptr,

      /* [IN] where the characters are to be stored */
      char *                     data_ptr,

      /* [IN] the number of bytes to read */
      int32_t                     n
   )
{  /* Body */

   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_EXPANDER_DEVICE_STRUCT_PTR expander_io_dev_ptr;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   expander_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

   if (expander_io_dev_ptr->DEV_READ != NULL) {
       return (*expander_io_dev_ptr->DEV_READ)(expander_io_dev_ptr,
                        fd_ptr->DEV_DATA_PTR, data_ptr, n);
   } else {
       return -1;
   }

}  /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_expander_write
* Returned Value   : number of bytes written
* Comments         :
*    This routine calls the appropriate write routine.
*
*END**********************************************************************/

int32_t _io_expander_write
   (
      /* [IN] the handle returned from _fopen */
      FILE_DEVICE_STRUCT_PTR       fd_ptr,

      /* [IN] where the bytes are stored */
      char *                     data_ptr,

      /* [IN] the number of bytes to output */
      int32_t                     n
   )
{  /* Body */
   IO_DEVICE_STRUCT_PTR            io_dev_ptr;
   IO_EXPANDER_DEVICE_STRUCT_PTR expander_io_dev_ptr;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   expander_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

   if (expander_io_dev_ptr->DEV_WRITE != NULL) {
       return (*expander_io_dev_ptr->DEV_WRITE)(expander_io_dev_ptr,
                         fd_ptr->DEV_DATA_PTR, data_ptr, n);
   } else {
       return -1;
   }

} /* Endbody */


/*FUNCTION*****************************************************************
*
* Function Name    : _io_expander_ioctl
* Returned Value   : MQX error code
* Comments         :
*    Returns result of IO expander ioctl operation.
*
*END*********************************************************************/

int32_t _io_expander_ioctl
   (
      /* [IN] the file handle for the device */
      FILE_DEVICE_STRUCT_PTR       fd_ptr,

      /* [IN] the ioctl command */
      uint32_t                    cmd,

      /* [IN] the ioctl parameters */
      void *                      input_param_ptr
   )
{ /* Body */
   IO_DEVICE_STRUCT_PTR          io_dev_ptr;
   IO_EXPANDER_DEVICE_STRUCT_PTR expander_io_dev_ptr;
   int32_t                       result = MQX_OK;
   uint32_t *                    param_ptr = (uint32_t *)input_param_ptr;

   io_dev_ptr     = (IO_DEVICE_STRUCT_PTR)fd_ptr->DEV_PTR;
   expander_io_dev_ptr = (void *)io_dev_ptr->DRIVER_INIT_PTR;

    switch (cmd) {
        case IO_IOCTL_DEVICE_IDENTIFY :
            /* return the device identify */
            param_ptr[0] = IO_DEV_TYPE_PHYS_IO_EXPANDER;
            param_ptr[1] = 0;
            param_ptr[2] = IO_DEV_ATTR_READ | IO_DEV_ATTR_WRITE;
            result = MQX_OK;
            break;
        default:
            if (expander_io_dev_ptr->DEV_IOCTL != NULL) {
                result = (*expander_io_dev_ptr->DEV_IOCTL)(expander_io_dev_ptr,
                    fd_ptr->DEV_DATA_PTR, cmd, param_ptr);
            }
    }

   return result;
} /* Endbody */

/* EOF */
