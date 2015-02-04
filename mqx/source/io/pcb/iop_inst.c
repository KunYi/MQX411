/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
* Copyright 1989-2008 ARC International
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
*   This file contains the function for installing a dynamic device
*   driver.
*
*
*END************************************************************************/

/* suppress warning if partition component is not enabled in MQX */
#define MQX_DISABLE_CONFIG_CHECK 1

#include <string.h>
#include "mqx_inc.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"
#include "partition.h"
#include "io_pcb.h"
#include "iopcbprv.h"

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_pcb_dev_install
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Install a device dynamically, so tasks can fopen to it.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _io_pcb_dev_install
   (
      /* [IN] A string that identifies the device for fopen */
      char             *identifier,
  
      /* [IN] The I/O open function */
      _mqx_int (_CODE_PTR_ io_open)(FILE_DEVICE_STRUCT_PTR,
         char *, char *),

      /* [IN] The I/O close function */
      _mqx_int (_CODE_PTR_ io_close)(FILE_DEVICE_STRUCT_PTR),

      /* [IN] The I/O read function */
      _mqx_int (_CODE_PTR_ io_read)(FILE_DEVICE_STRUCT_PTR,
         IO_PCB_STRUCT  **),

      /* [IN] The I/O write function */
      _mqx_int (_CODE_PTR_ io_write)(FILE_DEVICE_STRUCT_PTR,
         IO_PCB_STRUCT *),

      /* [IN] The I/O ioctl function */
      _mqx_int (_CODE_PTR_ io_ioctl)(FILE_DEVICE_STRUCT_PTR, _mqx_uint,
         void *),

      /* [IN] The I/O uninstall function */
      _mqx_int (_CODE_PTR_ io_uninstall)(IO_PCB_DEVICE_STRUCT_PTR),

      /* [IN] The I/O initialization data */
      void                *io_init_data_ptr
   )
{ /* Body */
   KERNEL_DATA_STRUCT_PTR   kernel_data;
   IO_PCB_DEVICE_STRUCT_PTR dev_ptr;

   kernel_data = _mqx_get_kernel_data();

#if MQX_CHECK_ERRORS
   if ((io_open == NULL) || (io_close == NULL)){
      return(MQX_INVALID_PARAMETER);
   }/* Endif */

   /* Check to see if device already installed */
   _lwsem_wait((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
   /* Start CR 460 */
   if (kernel_data->IO_DEVICES.NEXT == NULL) {
      /* Set up the device driver queue */
      _QUEUE_INIT(&kernel_data->IO_DEVICES, 0);
   } /* Endif */
   /* End CR 460 */
   dev_ptr = (IO_PCB_DEVICE_STRUCT_PTR)((void *)kernel_data->IO_DEVICES.NEXT);
   while (dev_ptr != (IO_PCB_DEVICE_STRUCT_PTR)
      ((void *)&kernel_data->IO_DEVICES.NEXT))
   {
      if (!strncmp(identifier, dev_ptr->IDENTIFIER, IO_MAXIMUM_NAME_LENGTH)) {
         _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
         return(IO_DEVICE_EXISTS);
      } /* Endif */
      dev_ptr = (IO_PCB_DEVICE_STRUCT_PTR)((void *)dev_ptr->QUEUE_ELEMENT.NEXT);
   } /* Endwhile */
   _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
#endif
   
   dev_ptr     = (void *)_mem_alloc_system_zero(
      (_mem_size)sizeof(IO_PCB_DEVICE_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (dev_ptr == NULL) {
      return(MQX_OUT_OF_MEMORY);
   }/* Endif */
#endif
   
   dev_ptr->IDENTIFIER      = identifier;
   dev_ptr->IO_OPEN         = io_open;
   dev_ptr->IO_CLOSE        = io_close;
   dev_ptr->IO_READ         = io_read;
   dev_ptr->IO_WRITE        = io_write;
   dev_ptr->IO_IOCTL        = io_ioctl;
   dev_ptr->IO_UNINSTALL    = io_uninstall;
   dev_ptr->DRIVER_INIT_PTR = io_init_data_ptr;

   _lwsem_wait((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
   _QUEUE_ENQUEUE(&kernel_data->IO_DEVICES, dev_ptr);
   _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);

   return MQX_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_pcb_dev_uninstall
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    UnInstall a device dynamically.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _io_pcb_dev_uninstall
   (
      /* [IN] A string that identifies the device */
      char           *identifier
   )
{ /* Body */
   KERNEL_DATA_STRUCT_PTR   kernel_data;
   IO_PCB_DEVICE_STRUCT_PTR dev_ptr;

   kernel_data = _mqx_get_kernel_data();

   _lwsem_wait((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
   /* Check to see if device already installed */
   dev_ptr = (IO_PCB_DEVICE_STRUCT_PTR)((void *)kernel_data->IO_DEVICES.NEXT);
   while (dev_ptr != (IO_PCB_DEVICE_STRUCT_PTR)
      ((void *)&kernel_data->IO_DEVICES.NEXT))
   {
      if (!strncmp(identifier, dev_ptr->IDENTIFIER, IO_MAXIMUM_NAME_LENGTH)) {
         break;
      } /* Endif */
      dev_ptr = (IO_PCB_DEVICE_STRUCT_PTR)((void *)dev_ptr->QUEUE_ELEMENT.NEXT);
   } /* Endwhile */
   if (dev_ptr == (IO_PCB_DEVICE_STRUCT_PTR)
      ((void *)&kernel_data->IO_DEVICES.NEXT))
   {
      dev_ptr = NULL;
   } /* Endif */
   if (dev_ptr)  {
      if (dev_ptr->IO_UNINSTALL)  {
         (*dev_ptr->IO_UNINSTALL)(dev_ptr);
      } /* Endif */
      _QUEUE_REMOVE(&kernel_data->IO_DEVICES, dev_ptr);
   } /* Endif */
   _lwsem_post((LWSEM_STRUCT_PTR)&kernel_data->IO_LWSEM);
   if (dev_ptr) {
      _mem_free(dev_ptr);
      return MQX_OK;
   }/* Endif */
   return(IO_PCB_DEVICE_DOES_NOT_EXIST);
   
} /* Endbody */

/* EOF */
