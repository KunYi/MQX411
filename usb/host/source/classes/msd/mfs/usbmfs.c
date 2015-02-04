/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains the USB MFS mass storage link driver.
*
*
*END************************************************************************/

#include <mqx.h>
#if MQX_USE_IO_OLD
#include <fio.h>
#else
#include "errno.h" /* Workaround for uv4 to set including */
#include <nio.h>
#include <nio/ioctl.h>
#include <fcntl.h>
#endif
#include <mfs.h>
#include "usb.h"
#include "usb_prv.h"
#include "host_common.h"

#include "host_dev_list.h" //see below: workaround

#ifdef __USB_OTG__
#include "otgapi.h"
#include "devapi.h"
#else
#include "hostapi.h"
#endif

#include "../usb_host_msd_bo.h"
#include "../usb_host_msd_ufi.h"
#include "usbmfs.h"
#include "usbmfspr.h"

/* Function declaration */
static void _io_usb_mfs_callback
(
    USB_STATUS,  /*status of this command*/
    void *,     /*pointer to USB_MASS_BULK_ONLY_REQUEST_STRUCT*/
    void *,     /*pointer to the command object*/
    uint32_t      /* length of the data transfered if any */
);

struct init_data {
    CLASS_CALL_STRUCT_PTR ccs_ptr;
    uint8_t lun;
};

#if !MQX_USE_IO_OLD
struct file_data {
    long size;
    uint32_t ERROR; //in capitals for better code compatibility with FIO
    long LOCATION; //in capitals for better code compatibility with FIO
};

const NIO_DEV_FN_STRUCT _usb_mfs_dev = {
    .OPEN = _io_usb_mfs_open,
    .READ = _io_usb_mfs_read,
    .WRITE = _io_usb_mfs_write,
    .LSEEK = _io_usb_mfs_seek,
    .IOCTL = _io_usb_mfs_ioctl,
    .CLOSE = _io_usb_mfs_close,
    .INIT = _io_usb_mfs_init,
    .DEINIT = _io_usb_mfs_deinit,
};
#endif //MQX_USE_IO_OLD


static int32_t _io_usb_mfs_open_internal(IO_USB_MFS_STRUCT_PTR info_ptr, CLASS_CALL_STRUCT_PTR ccs_ptr);
static _mqx_int _io_usb_mfs_ioctl_stop(IO_USB_MFS_STRUCT_PTR info_ptr);
static _mqx_int _io_usb_mfs_read_sector_internal(IO_USB_MFS_STRUCT_PTR info_ptr, uint32_t sector, uint16_t how_many_sectors, unsigned char *data_ptr);
static _mqx_int _io_usb_mfs_read_sector_long_internal(IO_USB_MFS_STRUCT_PTR info_ptr, uint32_t sector, uint32_t how_many_sectors, unsigned char *data_ptr);
static _mqx_int _io_usb_mfs_write_sector_internal(IO_USB_MFS_STRUCT_PTR info_ptr, uint32_t sector, uint16_t how_many_sectors, unsigned char *data_ptr);
static _mqx_int _io_usb_mfs_write_sector_long_internal(IO_USB_MFS_STRUCT_PTR info_ptr, uint32_t sector, uint32_t how_many_sectors, unsigned char *data_ptr);
#if MQX_USE_IO_OLD
static _mqx_int _io_usb_mfs_read_write_sectors_internal(IO_USB_MFS_STRUCT_PTR info_ptr, char *data_ptr, uint32_t num, bool write, MQX_FILE_PTR fd_ptr);
#else
static _mqx_int _io_usb_mfs_read_write_sectors_internal(IO_USB_MFS_STRUCT_PTR info_ptr, char *data_ptr, uint32_t num, bool write, void *fp_context);
#endif //MQX_USE_IO_OLD

static void _io_usb_mfs_callback(USB_STATUS command_status, void *p1, void *p2, uint32_t len);
static void _io_usb_ctrl_callback(void *, void *, unsigned char *, uint32_t, USB_STATUS);

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_install
* Returned Value   : uint32_t - a task error code or MQX_OK
* Comments         :
*    Install a  USB-MFS mass storage device driver.
*
*END*----------------------------------------------------------------------*/
#if MQX_USE_IO_OLD
uint32_t _io_usb_mfs_install
#else
NIO_DEV_STRUCT* _io_usb_mfs_install
#endif
   (
      /* [IN] A string that identifies the device for fopen */
      char                          *identifier,

      /* [IN] Logical unit number which driver need to install */
      uint8_t                        logical_unit,

      /* [IN] Interface Handle */
      CLASS_CALL_STRUCT_PTR         ccs_ptr

   )
{ /* Body */
   struct init_data id = {
      ccs_ptr,
      logical_unit
   };

#if MQX_USE_IO_OLD
   /* call the init function explicitly */
   return _io_usb_mfs_init(identifier, (void *)&id, NULL);
#else
   /* we rely that the init_data will be passed to init function in this task */
   return _nio_dev_install(identifier, &_usb_mfs_dev, (void *)&id);
#endif
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_init
* Returned Value   : uint_32 error code
* Comments  :   Uninstalls the MSDOS File System and frees all memory allocated
*               to it.
*
*END*---------------------------------------------------------------------*/
int _io_usb_mfs_init
   (
#if MQX_USE_IO_OLD
      char *identifier,
#endif
      void *init_data,

      void **dev_context
   )
{
   USB_MASS_CLASS_INTF_STRUCT_PTR intf_ptr = NULL;
   uint8_t *block_ptr, offset;
   struct init_data *id = (struct init_data *)init_data;
   CLASS_CALL_STRUCT_PTR ccs_ptr;
   USB_STATUS error = USBERR_ERROR;
   IO_USB_MFS_STRUCT_PTR info_ptr;


   ccs_ptr = id->ccs_ptr;
   USB_lock();
   if (usb_host_class_intf_validate(ccs_ptr)) {
      intf_ptr = (USB_MASS_CLASS_INTF_STRUCT_PTR) ccs_ptr->class_intf_handle;
      if(intf_ptr != NULL) {
         error = usb_hostdev_validate (intf_ptr->G.dev_handle);
      }
   } /* Endif */

   if (USB_OK != error) { /* Device was already detached or intf_ptr is NULL */
      USB_unlock();
#if MQX_USE_IO_OLD
      return MQX_OUT_OF_MEMORY;
#else
      return -MQX_OUT_OF_MEMORY;
#endif
   }

   if (USB_OK != _usb_hostdev_get_buffer(intf_ptr->G.dev_handle, sizeof(IO_USB_MFS_STRUCT), (void **) &info_ptr)) {
      USB_unlock();
#if MQX_USE_IO_OLD
      return MQX_OUT_OF_MEMORY;
#else
      return -MQX_OUT_OF_MEMORY;
#endif
   }

   USB_unlock();
   _mem_zero(info_ptr, sizeof(IO_USB_MFS_STRUCT));

   /* Fill in the state structure with the info we know */
   info_ptr->LUN         = id->lun;
   info_ptr->BLENGTH     = USB_MFS_DEFAULT_SECTOR_SIZE;
   info_ptr->ERROR_CODE  = 0;
   info_ptr->BLOCK_MODE  = TRUE;

   info_ptr->COMMAND.CBW_PTR = (CBW_STRUCT_PTR) &info_ptr->CBW;
   info_ptr->COMMAND.CSW_PTR = (CSW_STRUCT_PTR) &info_ptr->CSW;
   info_ptr->COMMAND.CALL_PTR = ccs_ptr;
   info_ptr->COMMAND.LUN = id->lun;
   info_ptr->COMMAND.CALLBACK = _io_usb_mfs_callback;
   info_ptr->COMMAND_STATUS = MQX_OK;
   _lwsem_create(&info_ptr->LWSEM, 1);
   _lwsem_create(&info_ptr->COMMAND_DONE, 0);

   /* Workaround for the issue with device file specific structure, which is later on accessed by the MFS:
   ** We will return pointer to allocated block rather than USB-host buffer pointer (see USB _usb_hostdev_get_buffer)
   */
   block_ptr = (uint8_t *)info_ptr;
   offset = *(block_ptr - 1);
   block_ptr = block_ptr - offset - MEM_HEADER_LEN;

   _mem_set_type(block_ptr, MEM_TYPE_USB_MFS_STRUCT);

#if MQX_USE_IO_OLD
   return (_io_dev_install_ext(identifier,
      (_mqx_int (_CODE_PTR_)(MQX_FILE_PTR, char *,  char *))_io_usb_mfs_open,
      (_mqx_int (_CODE_PTR_)(MQX_FILE_PTR)                     )_io_usb_mfs_close,
      (_mqx_int (_CODE_PTR_)(MQX_FILE_PTR, char *,  int32_t)  )_io_usb_mfs_read,
      (_mqx_int (_CODE_PTR_)(MQX_FILE_PTR, char *,  int32_t)  )_io_usb_mfs_write,
      (_mqx_int (_CODE_PTR_)(MQX_FILE_PTR, _mqx_uint, void *) ) _io_usb_mfs_ioctl,
      _io_usb_mfs_uninstall,
      (void *)block_ptr /* instead of info_ptr */
      ));
#else
   *dev_context = block_ptr;
   return MQX_OK;
#endif //MQX_USE_IO_OLD
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_uninstall
* Returned Value   : uint32_t error code
* Comments  :   Uninstalls the MSDOS File System and frees all memory allocated
*               to it.
*
*END*---------------------------------------------------------------------*/
#if MQX_USE_IO_OLD
int32_t _io_usb_mfs_uninstall
#else
int _io_usb_mfs_deinit
#endif
   (
       /* [IN] The device to uninstall */
#if MQX_USE_IO_OLD
      IO_DEVICE_STRUCT_PTR   io_dev_ptr
#else
      void *dev_context
#endif
   )
{
   DEV_MEMORY_PTR block_ptr;
   IO_USB_MFS_STRUCT_PTR info_ptr;

   /* Workaround for the issue with device file specific structure, which is later on accessed by the MFS:
   ** We will return pointer to allocated block rather than USB-host buffer pointer (see USB _usb_hostdev_get_buffer)
   */
#if MQX_USE_IO_OLD
   block_ptr = (DEV_MEMORY_PTR)io_dev_ptr->DRIVER_INIT_PTR;
#else
   block_ptr = (DEV_MEMORY_PTR)dev_context;
#endif
   info_ptr = (IO_USB_MFS_STRUCT_PTR)(block_ptr->payload.data + block_ptr->offset);

   _lwsem_destroy(&info_ptr->COMMAND_DONE);
   _lwsem_destroy(&info_ptr->LWSEM);
   /* The block_ptr will be disposed after device detach */
   //USB_mem_free(block_ptr);
   return MQX_OK;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_open_internal
* Returned Value   : An error code or MQX_OK
* Comments         : Opens and initializes a USB-MFS mass storage device driver.
*                    Mass storage device should be initialized to prior to this
*                    this call.
*END*----------------------------------------------------------------------*/
static _mqx_int _io_usb_mfs_open_internal
   (
   IO_USB_MFS_STRUCT_PTR            info_ptr,
   CLASS_CALL_STRUCT_PTR            ccs_ptr
   )
{ /* Body */
   uint32_t     i;
   USB_STATUS error;

   error = usb_class_mass_getmaxlun_bulkonly(info_ptr->COMMAND.CALL_PTR, (unsigned char *) &info_ptr->INQUIRY_DATA, _io_usb_ctrl_callback, info_ptr);
   if ((error != MQX_OK) && (error != USB_STATUS_TRANSFER_QUEUED)) {
      return -1;
   } /* Endif */
   if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) != MQX_OK) {
      /* Cancel the command */
      usb_mass_ufi_cancel(&info_ptr->COMMAND);
#if MQX_USE_IO_OLD
      return USBMFS_ERROR_TIMEOUT;
#else
      errno = EACCES;
      return -1;
#endif
   } /* Endif */

   error = usb_mass_ufi_inquiry(&info_ptr->COMMAND, (unsigned char *) &info_ptr->INQUIRY_DATA, sizeof(INQUIRY_DATA_FORMAT));
   if ((error != MQX_OK) && (error != USB_STATUS_TRANSFER_QUEUED)) {
#if MQX_USE_IO_OLD
      return USBMFS_ERROR_TIMEOUT;
#else
      errno = EACCES;
      return -1;
#endif
   } /* Endif */
   if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) != MQX_OK) {
      /* Cancel the command */
      usb_mass_ufi_cancel(&info_ptr->COMMAND);
#if MQX_USE_IO_OLD
      return USBMFS_ERROR_TIMEOUT;
#else
      errno = EACCES;
      return -1;
#endif
   } /* Endif */


   if (info_ptr->COMMAND_STATUS != MQX_OK) {
#if MQX_USE_IO_OLD
      return USBMFS_ERROR_INQUIRY;
#else
      errno = EACCES;
      return -1;
#endif
   } /* Endif */

   for (i = 0; i < USBCFG_MFS_MAX_RETRIES; i++) {
      if (i) {
         /* Delay for a user-specified amount of time */
         _time_delay_ticks(USBCFG_MFS_OPEN_READ_CAPACITY_RETRY_DELAY);
      }

      /* Send the call now */
      error = usb_mass_ufi_read_capacity(&info_ptr->COMMAND, (unsigned char *) &info_ptr->CAPACITY,
         sizeof(MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO));

      if ((error != MQX_OK) && (error != USB_STATUS_TRANSFER_QUEUED)) {
#if MQX_USE_IO_OLD
         return MFS_READ_FAULT;
#else
         errno = EACCES;
         return -1;
#endif
      } /* Endif */

      if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) != MQX_OK) {
         usb_mass_ufi_cancel(&info_ptr->COMMAND);
#if MQX_USE_IO_OLD
         return USBMFS_ERROR_TIMEOUT;
#else
         errno = EACCES;
         return -1;
#endif
      } /* Endif */

      if (info_ptr->COMMAND_STATUS == MQX_OK)
         break;


      error = usb_mass_ufi_request_sense(&info_ptr->COMMAND, &info_ptr->SENSE, sizeof(REQ_SENSE_DATA_FORMAT));

      if ((error != MQX_OK) && (error != USB_STATUS_TRANSFER_QUEUED)) {
#if MQX_USE_IO_OLD
         return MFS_READ_FAULT;
#else
         errno = EACCES;
         return -1;
#endif
      }

      if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) != MQX_OK) {
         usb_mass_ufi_cancel(&info_ptr->COMMAND);
#if MQX_USE_IO_OLD
         return USBMFS_ERROR_TIMEOUT;
#else
         errno = EACCES;
         return -1;
#endif
      }
   }

   if (info_ptr->COMMAND_STATUS == MQX_OK) {
      info_ptr->BCOUNT = LONG_BE_TO_HOST(*(uint32_t*)&info_ptr->CAPACITY.BLLBA);
      info_ptr->BLENGTH = LONG_BE_TO_HOST(*(uint32_t*)&info_ptr->CAPACITY.BLENGTH);

      /* should read this to support  low level format */
      info_ptr->NUMBER_OF_HEADS   = 0;
      info_ptr->NUMBER_OF_TRACKS  = 0;
      info_ptr->SECTORS_PER_TRACK = 0;

      info_ptr->SIZE_BYTES = info_ptr->BLENGTH * info_ptr->BCOUNT;
   } /* Endif */

   return (_mqx_int)info_ptr->COMMAND_STATUS;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_open
* Returned Value   : An error code or MQX_OK
* Comments         : Opens and initializes a USB-MFS mass storage device driver.
*                    Mass storage device should be initialized to prior to this
*                    this call.
*END*----------------------------------------------------------------------*/
#if MQX_USE_IO_OLD
_mqx_int _io_usb_mfs_open
   (
      /* [IN] the file handle for the device being opened */
      MQX_FILE_PTR       fd_ptr,

      /* [IN] the remaining portion of the name of the device */
      char           *open_name_ptr,

      /* [IN] USB mass storage class handle */
      char           *flags
   )
#else
int _io_usb_mfs_open
   (
      void *dev_context,
      const char *dev_name,
      int flags,

      void **fp_context
   )
#endif
{ /* Body */
#if MQX_USE_IO_OLD
   IO_DEVICE_STRUCT_PTR             io_dev_ptr = fd_ptr->DEV_PTR;
#endif
   CLASS_CALL_STRUCT_PTR            ccs_ptr = (CLASS_CALL_STRUCT_PTR)flags;
   _mqx_int                         io_error;
   DEV_MEMORY_PTR                   block_ptr;
   IO_USB_MFS_STRUCT_PTR            info_ptr;

#if MQX_USE_IO_OLD
   block_ptr = (DEV_MEMORY_PTR)io_dev_ptr->DRIVER_INIT_PTR;
#else
   *fp_context = _mem_alloc_zero(sizeof(struct file_data));
   if (NULL == *fp_context) {
      return -MQX_OUT_OF_MEMORY;
   }
   block_ptr = (DEV_MEMORY_PTR)dev_context;
#endif
   /* Workaround for the issue with device file specific structure, which is later on accessed by the MFS:
   ** We will return pointer to allocated block rather than USB-host buffer pointer (see USB _usb_hostdev_get_buffer)
   */
   info_ptr = (IO_USB_MFS_STRUCT_PTR)(block_ptr->payload.data + block_ptr->offset);

   /* Save the mass storage class handle. */
   info_ptr->MSC_STREAM = (void *) ccs_ptr;

   /* Send the call now */
   _lwsem_wait(&info_ptr->LWSEM);
   io_error = _io_usb_mfs_open_internal(info_ptr, ccs_ptr);
   _lwsem_post(&info_ptr->LWSEM);

   return io_error;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_close
* Returned Value   : ERROR CODE
* Comments         : Closes the USB mass storage link driver
*
*END*----------------------------------------------------------------------*/
#if MQX_USE_IO_OLD
_mqx_int _io_usb_mfs_close
   (
      /* [IN] the file handle for the device being closed */
      MQX_FILE_PTR fd_ptr
   )
#else
int _io_usb_mfs_close
   (
      void *dev_context,

      void *fp_context
   )
#endif
{ /* Body */
#if MQX_USE_IO_OLD
   _mem_free(fd_ptr->DEV_DATA_PTR);
#else
   _mem_free(fp_context);
#endif

   return MQX_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_read
* Returned Value   : number of bytes read
* Comments         : Reads data from the USB mass storage device
*
*END*----------------------------------------------------------------------*/
#if MQX_USE_IO_OLD
_mqx_int _io_usb_mfs_read
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR         fd_ptr,
      /* [IN] where the outgoing data is store */
      char             *data_ptr,
      /* [IN] the number of bytes to output */
      int32_t               num
   )
#else
int _io_usb_mfs_read
   (
      void *dev_context,

      void *fp_context,

      void *data_ptr,

      size_t num
   )
#endif
{ /* Body */
   DEV_MEMORY_PTR         block_ptr;
   IO_USB_MFS_STRUCT_PTR  info_ptr;
   _mqx_int               io_result;
#if MQX_USE_IO_OLD
   IO_DEVICE_STRUCT_PTR   io_dev_ptr = fd_ptr->DEV_PTR;
#endif

   /* Workaround for the issue with device file specific structure, which is later on accessed by the MFS:
   ** We will return pointer to allocated block rather than USB-host buffer pointer (see USB _usb_hostdev_get_buffer)
   */
#if MQX_USE_IO_OLD
   block_ptr = (DEV_MEMORY_PTR)io_dev_ptr->DRIVER_INIT_PTR;
#else
   block_ptr = (DEV_MEMORY_PTR)dev_context;
#endif
   info_ptr = (IO_USB_MFS_STRUCT_PTR)(block_ptr->payload.data + block_ptr->offset);

   _lwsem_wait(&info_ptr->LWSEM);
#if MQX_USE_IO_OLD
   io_result = _io_usb_mfs_read_write_sectors_internal(info_ptr, data_ptr, (uint32_t)num, FALSE, fd_ptr);
#else
   io_result = _io_usb_mfs_read_write_sectors_internal(info_ptr, data_ptr, (uint32_t)num, FALSE, fp_context);
#endif
   _lwsem_post(&info_ptr->LWSEM);

#if !MQX_USE_IO_OLD
   if (io_result < 0) {
      errno = EIO;
      return -1;
   }
#endif

   return io_result;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_write
* Returned Value   : number of bytes written
* Comments         : Writes data to the USB mass storage device
*
*END*----------------------------------------------------------------------*/
#if MQX_USE_IO_OLD
int32_t _io_usb_mfs_write
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR     fd_ptr,
      /* [IN] where the outgoing data is store */
      char             *data_ptr,
      /* [IN] the number of bytes to output */
      int32_t          num
   )
#else
int _io_usb_mfs_write
   (
      void *dev_context,

      void *fp_context,

      const void *data_ptr,

      size_t num
   )
#endif
{ /* Body */
   DEV_MEMORY_PTR         block_ptr;
   IO_USB_MFS_STRUCT_PTR  info_ptr;
   _mqx_int               io_result;
#if MQX_USE_IO_OLD
   IO_DEVICE_STRUCT_PTR   io_dev_ptr = fd_ptr->DEV_PTR;
#endif

   /* Workaround for the issue with device file specific structure, which is later on accessed by the MFS:
   ** We will return pointer to allocated block rather than USB-host buffer pointer (see USB _usb_hostdev_get_buffer)
   */
#if MQX_USE_IO_OLD
   block_ptr = (DEV_MEMORY_PTR)io_dev_ptr->DRIVER_INIT_PTR;
#else
   block_ptr = (DEV_MEMORY_PTR)dev_context;
#endif
   info_ptr = (IO_USB_MFS_STRUCT_PTR)(block_ptr->payload.data + block_ptr->offset);

   _lwsem_wait(&info_ptr->LWSEM);

#if MQX_USE_IO_OLD
   io_result = _io_usb_mfs_read_write_sectors_internal(info_ptr, data_ptr, (uint32_t)num, TRUE, fd_ptr);
#else
   io_result = _io_usb_mfs_read_write_sectors_internal(info_ptr, (char *)data_ptr, (uint32_t)num, TRUE, fp_context);
#endif

   _lwsem_post(&info_ptr->LWSEM);

#if !MQX_USE_IO_OLD
   if (io_result < 0) {
      errno = EIO;
      return -1;
   }
#endif

   return io_result;
} /* Endbody */


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_ioctl_stop
* Returned Value   : int32_t
* Comments         :
*    Cancels ioctl operation
*
*END*-------------------------------------------------------------------------*/
static _mqx_int _io_usb_mfs_ioctl_stop
   (
      IO_USB_MFS_STRUCT_PTR       info_ptr
   )
{ /* Body */
   MASS_STORAGE_START_STOP_UNIT_STRUCT_INFO  start_stop = {0};
   USB_STATUS  error;
   _mqx_int    io_result = -1;

   start_stop.START = 0;
   start_stop.LOEJ  = 0;
   error = usb_mass_ufi_start_stop(&info_ptr->COMMAND, start_stop.LOEJ, start_stop.START);
   if ((error==MQX_OK) || (error==USB_STATUS_TRANSFER_QUEUED)) {
      if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) == MQX_OK) {
         if (info_ptr->COMMAND_STATUS == MQX_OK) {
            io_result =  MQX_OK;
         } /* Endif */
      } else {
         usb_mass_ufi_cancel(&info_ptr->COMMAND);
      } /* Endif */
   } /* Endif */
   return(io_result);

} /* Endbody */


/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_ioctl
* Returned Value   : int32_t
* Comments         :
*    Returns result of ioctl operation.
*
*END*-------------------------------------------------------------------------*/
#if MQX_USE_IO_OLD
int32_t _io_usb_mfs_ioctl
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] the ioctl command */
      int32_t       command,

      /* [IN] the ioctl parameters */
      void        *input_param_ptr
   )
#else
int _io_usb_mfs_ioctl
   (
      void *dev_context,

      void *fp_context,

      unsigned long int command,

      va_list ap
   )
#endif
{ /* Body */
   DEV_MEMORY_PTR                block_ptr;
   IO_USB_MFS_STRUCT_PTR         info_ptr;
   USB_MFS_DRIVE_INFO_STRUCT_PTR drive_info_ptr;
#if MQX_USE_IO_OLD
   IO_DEVICE_STRUCT_PTR          io_dev_ptr = fd_ptr->DEV_PTR;
   uint32_t                      *param_ptr = input_param_ptr;
   int32_t                       result = MQX_OK;
#else
   uint32_t                      *param_ptr;
   int                           result = MQX_OK;
#endif

   /* Workaround for the issue with device file specific structure, which is later on accessed by the MFS:
   ** We will return pointer to allocated block rather than USB-host buffer pointer (see USB _usb_hostdev_get_buffer)
   */
#if MQX_USE_IO_OLD
   block_ptr = (DEV_MEMORY_PTR)io_dev_ptr->DRIVER_INIT_PTR;
#else
   block_ptr = (DEV_MEMORY_PTR)dev_context;
#endif
   info_ptr = (IO_USB_MFS_STRUCT_PTR)(block_ptr->payload.data + block_ptr->offset);

   switch (command) {
      case IO_IOCTL_GET_NUM_SECTORS:
#if !MQX_USE_IO_OLD
         param_ptr = va_arg(ap, uint32_t *);
#endif
         *param_ptr = info_ptr->BCOUNT;
         break;

      case IO_IOCTL_GET_BLOCK_SIZE:
#if !MQX_USE_IO_OLD
         param_ptr = va_arg(ap, uint32_t *);
#endif
         *param_ptr = info_ptr->BLENGTH;
         break;

      case IO_IOCTL_DEVICE_IDENTIFY:
#if !MQX_USE_IO_OLD
         param_ptr = va_arg(ap, uint32_t *);
#endif
         param_ptr[0] = IO_DEV_TYPE_PHYS_USB_MFS;
         param_ptr[1] = IO_DEV_TYPE_LOGICAL_MFS;
         param_ptr[2] = IO_DEV_ATTR_ERASE | IO_DEV_ATTR_POLL
                         | IO_DEV_ATTR_READ | IO_DEV_ATTR_REMOVE
                         | IO_DEV_ATTR_SEEK | IO_DEV_ATTR_WRITE;
         if (info_ptr->BLOCK_MODE) {
            param_ptr[2] |= IO_DEV_ATTR_BLOCK_MODE;
         } /* Endif */
         break;

      case IO_IOCTL_GET_DRIVE_PARAMS:
#if MQX_USE_IO_OLD
         drive_info_ptr = (USB_MFS_DRIVE_INFO_STRUCT_PTR)((void *)param_ptr);
#else
         drive_info_ptr = va_arg(ap, USB_MFS_DRIVE_INFO_STRUCT_PTR);
#endif
         drive_info_ptr->NUMBER_OF_HEADS   = info_ptr->NUMBER_OF_HEADS;
         drive_info_ptr->NUMBER_OF_TRACKS  = info_ptr->NUMBER_OF_TRACKS;
         drive_info_ptr->SECTORS_PER_TRACK = info_ptr->SECTORS_PER_TRACK;
         break;

      case IO_IOCTL_GET_VENDOR_INFO:
#if !MQX_USE_IO_OLD
         param_ptr = va_arg(ap, uint32_t *);
#endif
         *param_ptr = (uint32_t)&(info_ptr->INQUIRY_DATA.BVID);
         break;

      case IO_IOCTL_GET_PRODUCT_ID:
#if !MQX_USE_IO_OLD
         param_ptr = va_arg(ap, uint32_t *);
#endif
         *param_ptr = (uint32_t)&(info_ptr->INQUIRY_DATA.BPID);
         break;

      case IO_IOCTL_GET_PRODUCT_REV:
#if !MQX_USE_IO_OLD
         param_ptr = va_arg(ap, uint32_t *);
#endif
         *param_ptr = (uint32_t)&(info_ptr->INQUIRY_DATA.BPRODUCT_REV);
         break;

      case IO_IOCTL_DEVICE_STOP:
         /* Send the call now */
         _lwsem_wait(&info_ptr->LWSEM);

         result = _io_usb_mfs_ioctl_stop(info_ptr);

            /* Wait for the completion*/
         _lwsem_post(&info_ptr->LWSEM);


         break;

         /* Start CR 812 */
      case IO_IOCTL_SET_BLOCK_MODE:
         info_ptr->BLOCK_MODE = TRUE;
         break;
         /* End   CR 812 */

      default:
#if MQX_USE_IO_OLD
         result = IO_ERROR_INVALID_IOCTL_CMD;
#else
         result = EINVAL;
#endif
         break;
   } /* Endswitch */

#if !MQX_USE_IO_OLD
   if (result != MQX_OK) {
       errno = result;
       return -1;
   }
#endif

   return result;
} /* Endbody */


#if !MQX_USE_IO_OLD
/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_lseek
* Returned Value   : int_32
* Comments         :
*    Returns result of ioctl operation.
*
*END*-------------------------------------------------------------------------*/
off_t _io_usb_mfs_seek
   (
       void *dev_context,

       void *fp_context,

       off_t offset,

       int mode
   )
{ /* Body */
   struct file_data *fi = (struct file_data *)fp_context;
   off_t retval = 0;

   switch (mode) {
      case SEEK_SET:
        //set the absolute position
        if (offset < 0) {
            retval = (off_t)EINVAL;
        }
        else {
            fi->LOCATION = offset;
        }
        break;
      case SEEK_CUR:
        //set the relative position from current
        //check for overflow
        if (offset > 0 && (fi->LOCATION + offset < fi->LOCATION)) {
            retval = (off_t)EINVAL;
        }
        else if (offset < 0 && (fi->LOCATION + offset > fi->LOCATION)) {
            retval = (off_t)EINVAL;
        }
        else {
            fi->LOCATION = fi->LOCATION + offset;
        }
        break;
      case SEEK_END:
        //set the relative position from end
        //check for overflow
        if (offset > 0 && (fi->size + offset < fi->size)) {
            retval = (off_t)EINVAL;
        }
        else if (offset < 0 && (fi->size + offset > fi->size)) {
            retval = (off_t)EINVAL;
        }
        else {
            fi->LOCATION = fi->size + offset;
        }
        break;
      default:
        errno = EINVAL;
        retval = (off_t)-1;
        break;
   }

   return retval == (off_t)0 ? fi->LOCATION : retval;
} /* Endbody */
#endif //MQX_USE_IO_OLD

/* Private functions */
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_read_sector
* Returned Value   : error code
* Comments         :
*
*
*END*----------------------------------------------------------------------*/
static _mqx_int _io_usb_mfs_read_sector_internal
   (
      /* [IN] USB MFS state structure */
      IO_USB_MFS_STRUCT_PTR  info_ptr,

      /* [IN] The sector number to read */
      uint32_t                sector,

      /* [IN] How many sectors should read */
      uint16_t                how_many_sectors,

      /* [IN] location to read data into */
      unsigned char              *data_ptr
   )
{ /* Body */
   uint32_t        read_size;
   USB_STATUS      error;
#if MQX_USE_IO_OLD
   _mqx_int        io_result = IO_ERROR;
#else
   _mqx_int        io_result = -1;
#endif
   uint32_t        tries;

   /* UFI_READ10 command  */
   read_size = info_ptr->BLENGTH * how_many_sectors;

   /* Send the call now */
   for (tries=0;tries<USBCFG_MFS_MAX_RETRIES;tries++) {
      error = usb_mass_ufi_read_10(
         &info_ptr->COMMAND, sector, data_ptr, read_size, how_many_sectors);
      if ((error==MQX_OK) || (error==USB_STATUS_TRANSFER_QUEUED)) {
         if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) == MQX_OK) {
            if (info_ptr->COMMAND_STATUS == MQX_OK) {
               io_result = (_mqx_int)how_many_sectors;
               break;
            } /* Endif */
         } else {
            /* Wait for the completion*/
            usb_mass_ufi_cancel(&info_ptr->COMMAND);
         } /* Endif */
      }
      else
      {
#ifdef _HOST_DEBUG_
          printf("\nUSBMFS retrying read sector");
#endif
      } /* Endif */

   } /* Endif */

   return(io_result);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_read_sector_long
* Returned Value   : error code
* Comments         :
*
*
*END*----------------------------------------------------------------------*/
static _mqx_int _io_usb_mfs_read_sector_long_internal
   (
      /* [IN] USB MFS state structure */
      IO_USB_MFS_STRUCT_PTR  info_ptr,

      /* [IN] The sector number to read */
      uint32_t                sector,

      /* [IN] How many sectors should read */
      uint32_t                how_many_sectors,

      /* [IN] location to read data into */
      unsigned char              *data_ptr
   )
{ /* Body */
   uint32_t        read_size;
   USB_STATUS      error;
#if MQX_USE_IO_OLD
   _mqx_int        io_result = IO_ERROR;
#else
   _mqx_int        io_result = -1;
#endif
   uint32_t        tries;

   read_size = info_ptr->BLENGTH * how_many_sectors;

   /* Send the call now */
   for (tries=0;tries<USBCFG_MFS_MAX_RETRIES;tries++) {
      error = usb_mass_ufi_read_12(
         &info_ptr->COMMAND, sector, data_ptr, read_size, how_many_sectors);

      if ((error==MQX_OK) || (error==USB_STATUS_TRANSFER_QUEUED)) {
         if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) == MQX_OK) {
            if (info_ptr->COMMAND_STATUS == MQX_OK) {
               io_result = (_mqx_int) how_many_sectors;
               break;
   } /* Endif */
         } else {
            usb_mass_ufi_cancel(&info_ptr->COMMAND);
         } /* Endif */
      } /* Endif */
      //printf("\nUSBMFS retrying read sector long");
   } /* Endif */

   return (io_result);
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_write_sector
* Returned Value   : error code
* Comments         :
*
*
*END*----------------------------------------------------------------------*/
static _mqx_int _io_usb_mfs_write_sector_internal
   (
      /* [IN] USB MFS state structure */
      IO_USB_MFS_STRUCT_PTR  info_ptr,

      /* [IN] The sector number to write */
      uint32_t                sector,

      /* [IN] How many sectors should read */
      uint16_t                how_many_sectors,

      /* [IN] Source data location */
      unsigned char              *data_ptr
   )
{ /* Body */
   uint32_t        write_size;
   uint32_t        tries;
   USB_STATUS     error;
#if MQX_USE_IO_OLD
   _mqx_int        io_result = IO_ERROR;
#else
   _mqx_int        io_result = -1;
#endif

   if (sector <= info_ptr->BCOUNT) {

      write_size = info_ptr->BLENGTH * how_many_sectors;
      for (tries=0;tries<USBCFG_MFS_MAX_RETRIES;tries++) {
         error = usb_mass_ufi_write_10(&info_ptr->COMMAND,
               sector, data_ptr, write_size, how_many_sectors);
         if ((error==MQX_OK) || (error==USB_STATUS_TRANSFER_QUEUED)) {
            if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) == MQX_OK) {
               if (info_ptr->COMMAND_STATUS == MQX_OK) {
                  io_result = (_mqx_int) how_many_sectors;
                  break;
               } /* Endif */
            } else {

               /* Send the call now */

               /* Wait for the completion*/
               usb_mass_ufi_cancel(&info_ptr->COMMAND);
            } /* Endif */
         } /* Endif */
         //printf("\nUSBMFS retrying write sector");
      }
   } /* Endif */

   return (io_result);
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_write_sector_long
* Returned Value   : error code
* Comments         :
*
*
*END*----------------------------------------------------------------------*/
static _mqx_int _io_usb_mfs_write_sector_long_internal
   (
      /* [IN] USB MFS state structure */
      IO_USB_MFS_STRUCT_PTR  info_ptr,

      /* [IN] The sector number to write */
      uint32_t                sector,

      /* [IN] How many sectors should read */
      uint32_t                how_many_sectors,

      /* [IN] Source data location */
      unsigned char              *data_ptr
   )
{ /* Body */
   uint32_t        write_size;
   USB_STATUS      error;
#if MQX_USE_IO_OLD
   _mqx_int        io_result = IO_ERROR;
#else
   _mqx_int        io_result = -1;
#endif
   uint32_t        tries;

   if (sector <= info_ptr->BCOUNT) {

      write_size = info_ptr->BLENGTH * how_many_sectors;
      for (tries=0;tries<USBCFG_MFS_MAX_RETRIES;tries++) {
         error = usb_mass_ufi_write_12(&info_ptr->COMMAND,
            sector, data_ptr, write_size, how_many_sectors);
         if ((error==MQX_OK) || (error==USB_STATUS_TRANSFER_QUEUED)) {
            if (_lwsem_wait_ticks(&info_ptr->COMMAND_DONE, USBCFG_MFS_LWSEM_TIMEOUT) == MQX_OK) {
               if (info_ptr->COMMAND_STATUS == MQX_OK) {
                  io_result = (_mqx_int) how_many_sectors;
                  break;
               } /* Endif */
            } else {

               /* Send the call now */
               usb_mass_ufi_cancel(&info_ptr->COMMAND);
            }
            /* Wait for the completion*/
         } /* Endif */
      }
   } /* Endif */

   return (io_result);
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_read_write_blocks
* Returned Value   : error or number of blocks read/written
* Comments         : This function is used to read/write blocks when driver
*   is in block mode
*
*END*----------------------------------------------------------------------*/
static _mqx_int _io_usb_mfs_read_write_sectors_internal
   (
      /* [IN] USB MFS state structure */
      IO_USB_MFS_STRUCT_PTR    info_ptr,

      /* [IN] where the newly-read bytes are to be stored */
      char                     *data_ptr,

      /* [IN] the number of sectors to read */
      uint32_t                 num,

      /* [IN] Read/write mode */
      bool                     write,

      /* [IN] File pointer for checking file location and setting ERROR */
#if MQX_USE_IO_OLD
      MQX_FILE_PTR             fd_ptr
#else
      void                     *fp_context
#endif
   )
{ /* Body */
   _mqx_int (_CODE_PTR_ rw_func_long)(IO_USB_MFS_STRUCT_PTR, uint32_t, uint32_t, unsigned char *);
   _mqx_int (_CODE_PTR_ rw_func)(IO_USB_MFS_STRUCT_PTR, uint32_t, uint16_t, unsigned char *);
#if MQX_USE_IO_OLD
   _mqx_int io_result;
#else
   _mqx_int io_result = EIO;
#endif
#if !MQX_USE_IO_OLD
   struct file_data *fd_ptr = (struct file_data *)fp_context;
#endif

   if (write) {
#if MQX_USE_IO_OLD
      io_result = IO_ERROR_WRITE_ACCESS;
#endif
      rw_func        = _io_usb_mfs_write_sector_internal;
      rw_func_long   = _io_usb_mfs_write_sector_long_internal;
   } else {
#if MQX_USE_IO_OLD
      io_result = IO_ERROR_READ_ACCESS;
#endif
      rw_func        = _io_usb_mfs_read_sector_internal;
      rw_func_long   = _io_usb_mfs_read_sector_long_internal;
   } /* Endif */

   if (fd_ptr->LOCATION >= info_ptr->BCOUNT) {
      fd_ptr->ERROR = io_result;
   } else {
      if ((num + fd_ptr->LOCATION) >= info_ptr->BCOUNT) {
         fd_ptr->ERROR = io_result;
         num = (uint32_t)(info_ptr->BCOUNT - fd_ptr->LOCATION + 1);
      } /* Endif */

      /* Read all the sectors, one at a time */
      if (num > 0xFFFE) {
         /* Read all the sectors at once */
          io_result = rw_func_long(info_ptr, fd_ptr->LOCATION,
            num, (unsigned char *)data_ptr);
      } else {
          io_result = rw_func(info_ptr, fd_ptr->LOCATION,
            (uint16_t)num, (unsigned char *)data_ptr);
      } /* Endif */
#if MQX_USE_IO_OLD
      if (io_result == IO_ERROR ) {
#else
      if (io_result == EIO ) {
#endif
         fd_ptr->ERROR = io_result;
      } else {
         fd_ptr->LOCATION += num;
      } /* Endif */
   } /* Endif */
   return io_result;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_callback
* Returned Value   : None
* Comments         : This function is called by lowlevel drivers
when a command call back is set
*
*END*----------------------------------------------------------------------*/
static void _io_usb_mfs_callback
   (
      USB_STATUS command_status,      /*status of this command*/
      void   *p1,     /* pointer to USB_MASS_BULK_ONLY_REQUEST_STRUCT*/
      void   *p2,     /* pointer to the command object*/
      uint32_t len     /* length of the data transfered if any */
   )
{ /* Body */
   IO_USB_MFS_STRUCT_PTR info_ptr = (IO_USB_MFS_STRUCT_PTR)p2;

   if (command_status == (USB_STATUS)USB_MASS_FAILED_IN_DATA)
      info_ptr->COMMAND.CSW_PTR->BCSWSTATUS = CSW_STATUS_FAILED;
   if (info_ptr->COMMAND.CSW_PTR->BCSWSTATUS == CSW_STATUS_GOOD) {
      info_ptr->COMMAND_STATUS = command_status;   /* set the status */
   } else {
      info_ptr->COMMAND_STATUS = (uint32_t)-1;
   }

   _lwsem_post(&info_ptr->COMMAND_DONE);

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_ctrl_callback
* Returned Value   : None
* Comments         : This function is called by lowlevel drivers
* when a control pipe replies
*
*END*----------------------------------------------------------------------*/
static void _io_usb_ctrl_callback(void *pipe_handle, void *user_param, unsigned char *buffer, uint32_t size, USB_STATUS status)
{
   IO_USB_MFS_STRUCT_PTR info_ptr = (IO_USB_MFS_STRUCT_PTR)user_param;

   _lwsem_post(&info_ptr->COMMAND_DONE);
}

/* EOF */
