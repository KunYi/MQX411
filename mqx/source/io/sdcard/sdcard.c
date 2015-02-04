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
*   This file contains the sd card driver functions.
*
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>
#include <io_prv.h>
#include <sdcard_prv.h>
#include <sdcard.h>

static _mqx_int _io_sdcard_uninstall(IO_DEVICE_STRUCT_PTR);

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_install
* Returned Value   : MQX error code
* Comments         :
*    Install a SDCARD driver.
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_sdcard_install
   (
      /* [IN] A string that identifies the device for fopen */
      char               *identifier,

      /* [IN] SD card initialization parameters */
      SDCARD_INIT_STRUCT_PTR init,

      /* [IN] Already opened communication descriptor */
      MQX_FILE_PTR           com_device
   )
{
   SDCARD_STRUCT_PTR sdcard_ptr;

   if ((NULL == init) || (NULL == com_device))
   {
      return MQX_INVALID_PARAMETER;
   }

   sdcard_ptr = (SDCARD_STRUCT_PTR)
   _mem_alloc_system_zero((_mem_size)sizeof(SDCARD_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (sdcard_ptr == NULL)
   {
      return MQX_OUT_OF_MEMORY;
   }
#endif

   _mem_set_type(sdcard_ptr, MEM_TYPE_IO_SDCARD_STRUCT);

   sdcard_ptr->INIT = init;

   sdcard_ptr->COM_DEVICE = com_device;

   _lwsem_create(&sdcard_ptr->LWSEM, 1);

   return (_io_dev_install_ext(identifier,
      _io_sdcard_open,
      _io_sdcard_close,
      _io_sdcard_read_blocks,
      _io_sdcard_write_blocks,
      _io_sdcard_ioctl,
      _io_sdcard_uninstall,
      (void *)sdcard_ptr));
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_uninstall
* Returned Value   : MQX error code
* Comments         :
*    Install a SDCARD driver.
*
*END*----------------------------------------------------------------------*/

static _mqx_int _io_sdcard_uninstall
   (
      /* [IN] The devices context */
      IO_DEVICE_STRUCT_PTR io_dev_ptr
   )
{
   SDCARD_STRUCT_PTR sdcard_ptr = (SDCARD_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;

   _lwsem_destroy(&sdcard_ptr->LWSEM);

   _mem_free(sdcard_ptr);

   return MQX_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_open
* Returned Value   : MQX error code
* Comments         : Opens and initializes SDCARD driver.
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_sdcard_open
   (
      /* [IN] the file handle for the device being opened */
      MQX_FILE_PTR fd_ptr,

      /* [IN] the remaining portion of the name of the device */
      char *open_name_ptr,

      /* [IN] the flags to be used during operation */
      char *flags
   )
{
   IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
   SDCARD_STRUCT_PTR    sdcard_ptr = (SDCARD_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
   _mqx_int             result = MQX_OK;

   if (NULL != sdcard_ptr->INIT->INIT_FUNC)
   {
      _lwsem_wait(&sdcard_ptr->LWSEM);
      if (! sdcard_ptr->INIT->INIT_FUNC (fd_ptr))
      {
         result = MQX_IO_OPERATION_NOT_AVAILABLE;
      }
      _lwsem_post(&sdcard_ptr->LWSEM);
   }

   return result;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_close
* Returned Value   : MQX error code
* Comments         : Closes SDCARD driver
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_sdcard_close
   (
      /* [IN] the file handle for the device being closed */
      MQX_FILE_PTR fd_ptr
   )
{
   return MQX_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_ioctl
* Returned Value   : MQX error code
* Comments         :
*    Returns result of ioctl operation.
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_sdcard_ioctl
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] the ioctl command */
      _mqx_uint cmd,

      /* [IN] the ioctl parameters */
      void     *param_ptr
   )
{
   IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
   SDCARD_STRUCT_PTR    sdcard_ptr = (SDCARD_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
   _mem_size_ptr        mparam_ptr;
   _mqx_uint_ptr        uparam_ptr;
   uint32_t            *u32param_ptr;
   _mqx_int             result = MQX_OK;

   switch (cmd)
   {
      case IO_IOCTL_SET_BLOCK_MODE:
         break;
      case IO_IOCTL_GET_BLOCK_SIZE:
         uparam_ptr  = (_mqx_uint_ptr)param_ptr;
         *uparam_ptr = IO_SDCARD_BLOCK_SIZE;
         break;
      case IO_IOCTL_GET_NUM_SECTORS:
         mparam_ptr = (_mem_size_ptr)param_ptr;
         *mparam_ptr = (_mem_size)sdcard_ptr->NUM_BLOCKS;
         break;
      case IO_IOCTL_GET_REQ_ALIGNMENT:
         if (NULL == param_ptr)
         {
             result = MQX_INVALID_PARAMETER;
             break;
         }
         *((uint32_t *)param_ptr) = sdcard_ptr->ALIGNMENT;
         break;
      case IO_IOCTL_SDCARD_GET_CID:
         if (NULL == param_ptr)
         {
             result = MQX_INVALID_PARAMETER;
             break;
         }
         _mem_copy(sdcard_ptr->CID, param_ptr, sizeof(sdcard_ptr->CID));
         break;
      case IO_IOCTL_DEVICE_IDENTIFY:
         u32param_ptr  = (uint32_t *)param_ptr;
         u32param_ptr[IO_IOCTL_ID_PHY_ELEMENT]  = IO_DEV_TYPE_PHYS_SDCARD;
         u32param_ptr[IO_IOCTL_ID_LOG_ELEMENT]  = IO_DEV_TYPE_LOGICAL_MFS;
         u32param_ptr[IO_IOCTL_ID_ATTR_ELEMENT] = IO_SDCARD_ATTRIBS;
         if (fd_ptr->FLAGS & IO_O_RDONLY)
         {
             u32param_ptr[IO_IOCTL_ID_ATTR_ELEMENT] &= (~ IO_DEV_ATTR_WRITE);
         }
         break;
      default:
         result = IO_ERROR_INVALID_IOCTL_CMD;
         break;
   }

   return result;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_read_blocks
* Returned Value   : number of blocks read
* Comments         : Reads data from SDCARD driver
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_sdcard_read_blocks
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] where the blocks are to be stored */
      char *data_ptr,

      /* [IN] the number of blocks to input */
      _mqx_int num
   )
{
    IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
    SDCARD_STRUCT_PTR    sdcard_ptr = (SDCARD_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
    _mqx_int             res;

    fd_ptr->ERROR = IO_OK;
    if (NULL == sdcard_ptr->INIT->READ_FUNC)
    {
        fd_ptr->ERROR = IO_ERROR_READ;
        return IO_ERROR;
    }

    if ((fd_ptr->LOCATION + num) > sdcard_ptr->NUM_BLOCKS)
    {
        fd_ptr->FLAGS |= IO_FLAG_AT_EOF;
        num = sdcard_ptr->NUM_BLOCKS - fd_ptr->LOCATION;
    }

    _lwsem_wait(&sdcard_ptr->LWSEM);
    res = sdcard_ptr->INIT->READ_FUNC (fd_ptr, (unsigned char *)data_ptr, fd_ptr->LOCATION, num);

    if (res > 0)
    {
        fd_ptr->LOCATION += res;
    }

    if (res != num)
    {
        fd_ptr->ERROR = IO_ERROR_READ;
    }

    _lwsem_post(&sdcard_ptr->LWSEM);
    return res;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_write_blocks
* Returned Value   : number of blocks written
* Comments         : Writes data to SDCARD driver
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_sdcard_write_blocks
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] where the blocks are stored */
      char *data_ptr,

      /* [IN] the number of blocks to output */
      _mqx_int num
   )
{
    IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
    SDCARD_STRUCT_PTR    sdcard_ptr = (SDCARD_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
    _mqx_int             res;

    fd_ptr->ERROR = IO_OK;
    if ((NULL == sdcard_ptr->INIT->WRITE_FUNC) || (fd_ptr->FLAGS & IO_O_RDONLY))
    {
        fd_ptr->ERROR = IO_ERROR_WRITE_PROTECTED;
        return IO_ERROR;
    }

    if ((fd_ptr->LOCATION + num) > sdcard_ptr->NUM_BLOCKS)
    {
        fd_ptr->FLAGS |= IO_FLAG_AT_EOF;
        num = sdcard_ptr->NUM_BLOCKS - fd_ptr->LOCATION;
    }

    _lwsem_wait(&sdcard_ptr->LWSEM);
    res = sdcard_ptr->INIT->WRITE_FUNC (fd_ptr, (unsigned char *)data_ptr, fd_ptr->LOCATION, num);

    if (res > 0)
    {
        fd_ptr->LOCATION += res;
    }

    if (res != num)
    {
        fd_ptr->ERROR = IO_ERROR_WRITE;
    }

    _lwsem_post(&sdcard_ptr->LWSEM);
    return res;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_csd_capacity
* Returned Value   : Capacity of storage device in 512B blocks
* Comments         :
*    Extracts capacity information from CSD.
*
*END*----------------------------------------------------------------------*/
uint32_t _io_sdcard_csd_capacity
(
    /* [IN] pointer to CSD */
    uint8_t *csd
)
{
    uint32_t capacity, c_size, c_size_mult, read_bl_len;

    /* check CSD version */
    if (0 == (csd[0] & 0xC0))
    {
        read_bl_len = csd[5] & 0x0F;
        c_size = csd[6] & 0x03;
        c_size = (c_size << 8) | csd[7];
        c_size = (c_size << 2) | (csd[8] >> 6);
        c_size_mult = ((csd[9] & 0x03) << 1) | (csd[10] >> 7);
        capacity = (c_size + 1) * (1 << (c_size_mult + 2)) * (1 << (read_bl_len - 9));
    }
    else
    {
        c_size = csd[7] & 0x3F;
        c_size = (c_size << 8) | csd[8];
        c_size = (c_size << 8) | csd[9];
        capacity = (c_size + 1) << 10;
    }

    return capacity;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_sdcard_csd_baudrate
* Returned Value   : Maximum supported data transfer rate
* Comments         :
*    Extracts maximum supported data transfer rate over single line from CSD.
*
*END*----------------------------------------------------------------------*/
uint32_t _io_sdcard_csd_baudrate
(
    /* [IN] pointer to CSD */
    uint8_t *csd
)
{
    static const uint32_t unit_tab[] = { 10000, 100000, 1000000, 10000000 };
    static const uint8_t value_tab[] = { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };

    uint32_t value, unit;

    unit = (csd[3] & 0x07);
    value = (csd[3] & 0x78) >> 3;

    if (unit > 3) return 0; /* undefined */

    return value_tab[value] * unit_tab[unit];
}

/* EOF */
