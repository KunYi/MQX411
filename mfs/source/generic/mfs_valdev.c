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
*   This file contains the function for making sure the device on which
*   MFS is being run on is suitable
*
*
*END************************************************************************/

#include <mqx.h>
#include <fio.h>
#include <io_prv.h>
#include <errno.h>
#include "mfs.h"
#include "mfs_prv.h"
#include "part_mgr.h"
#if !MQX_USE_IO_OLD
#include <nio/ioctl.h>
#endif

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mfs_validate_device
* Returned Value   : MFS_NO_ERROR or an error
* Comments         : Checks the lower layer device for compatibility
*
*END*----------------------------------------------------------------------*/

_mfs_error _mfs_validate_device
    (
#if MQX_USE_IO_OLD
    MQX_FILE_PTR        dev_fd,            /* [IN] the file handle for the lower layer device */
#else
    int                 dev_fd,            /* [IN] the file handle for the lower layer device */
#endif

    uint32_t            *sector_size_ptr,   /* [OUT] Pointer to where the sector size is to be stored  */
    bool                *block_mode_ptr     /* [OUT] Pointer to where the block mode is to be stored  */
    )
{
    _mfs_error      error_code;
#ifdef IO_DEV_ATTR_BLOCK_MODE
    uint32_t         id_array[3];

    id_array[0] = id_array[1] = id_array[2] = 0;

    /*
    ** Issue the id command. Block mode drivers must support this command but
    ** other drivers can support it also
    */
#if MQX_USE_IO_OLD
    error_code = _io_ioctl(dev_fd, IO_IOCTL_DEVICE_IDENTIFY, id_array);
    if ( error_code == IO_OK )
    {
        /*
        ** The identify command is supported. Check to see if it is a block mode
        ** driver
        */
        if ( id_array[MFS_IOCTL_ID_ATTR_ELEMENT] & IO_DEV_ATTR_BLOCK_MODE )
        {
            *block_mode_ptr = TRUE;
        }
        else
        {
            *block_mode_ptr = FALSE;
        }
    }
    else if ( error_code == IO_ERROR_INVALID_IOCTL_CMD )
    {
        /*
        ** The ID command is not supported by the lower layer. It is not a block
        ** mode driver
        */
        *block_mode_ptr = FALSE;
    }
    else
    {
        /* Something bad happened at the lower layer */
        return( error_code );
    }
#else //MQX_USE_IO_OLD
    error_code = ioctl(dev_fd, IO_IOCTL_DEVICE_IDENTIFY, id_array);
    if ( 0 >= error_code )
    {
        /*
        ** The identify command is supported. Check to see if it is a block mode
        ** driver
        */
        if ( id_array[MFS_IOCTL_ID_ATTR_ELEMENT] & IO_DEV_ATTR_BLOCK_MODE )
        {
            *block_mode_ptr = TRUE;
        }
        else
        {
            *block_mode_ptr = FALSE;
        }
    }
    else
    {
        *block_mode_ptr = FALSE;
    }
#endif //MQX_USE_IO_OLD
#else
    *block_mode_ptr = FALSE;
#endif

    *sector_size_ptr = MFS_DEFAULT_SECTOR_SIZE;
    error_code = ioctl(dev_fd, IO_IOCTL_GET_BLOCK_SIZE, sector_size_ptr);
#if MQX_USE_IO_OLD
    if ( error_code == IO_ERROR_INVALID_IOCTL_CMD )
#else
    if ( error_code == -1 && errno == EINVAL )
#endif
    {
        /*
        ** The command is not supported. This is OK as long as it isn't a block
        ** mode driver.
        */
        if ( *block_mode_ptr )
        {
            /* MFS needs to know the block size for block mode drivers */
            error_code = MFS_INVALID_DEVICE;
        }
        else
        {
            /*
            ** It doesn't matter that we can't tell the actual block size. MFS
            ** will be doing byte accesses anyway since it isn't a block mode
            ** driver.
            */
            *sector_size_ptr = MFS_DEFAULT_SECTOR_SIZE;
            error_code = MFS_NO_ERROR;
        }
    }
    else if ( error_code == MFS_OK )
    {
        /* Check to see if the sector or block size is suitable for MFS */
        if ( !((*sector_size_ptr == 512) || (*sector_size_ptr == 1024) ||
            (*sector_size_ptr == 2048) || (*sector_size_ptr == 4096)) )
        {
            /*
            ** The block size isn't compatible. This may still be OK as long as we
            ** are not dealing with a block mode driver otherwise MFS won't be
            ** able to work with it
            */
            if ( *block_mode_ptr )
            {
                error_code = MFS_INVALID_DEVICE;
            }

        }
    }

    return(error_code);
}
