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
*   This file contains the functions for a partition manager device that is
*   is built to work under MFS1.40. These functions can also create or
*   remove partitions from a disk.
*
*
*END************************************************************************/

#include <mqx.h>
#if MQX_USE_IO_OLD
#include <fio.h>
#include <ioctl.h>
#else
#include <nio.h>
#include <fcntl.h>
#include <nio/ioctl.h>
#endif
#include "errno.h" /* Workaround for uv4 to set including */
#include "mfs.h"
#include "mfs_prv.h"
#include "part_mgr.h"

#if !MQX_USE_IO_OLD

struct pmgr_init_struct {
    int dev_fd;
    uint32_t sector_size;
};

const NIO_DEV_FN_STRUCT mgr_if = {
    .OPEN = _io_part_mgr_open,
    .READ = _io_part_mgr_read,
    .WRITE = _io_part_mgr_write,
    .LSEEK = _io_part_mgr_lseek,
    .IOCTL = _io_part_mgr_ioctl,
    .CLOSE = _io_part_mgr_close,
    .INIT = _io_part_mgr_init,
    .DEINIT = _io_part_mgr_deinit,
};

/*FUNCTION*---------------------------------------------------------------
*
* Function Name  : _io_part_mgr_install
* Returned Value : int32_t error code
* Comments       : installs the partition manager device
*
*
*END*--------------------------------------------------------------------*/

int32_t _io_part_mgr_install
    (
    int dev_fd,             /*[IN] Handle of the device on which to install the partition manager */
    char *identifier, /*[IN] The new name of the partition manager device */
    uint32_t sector_size /*[IN] The size of sectors in bytes on the lower level device */
    )
{
    struct pmgr_init_struct pmgr_init = {
        .dev_fd = dev_fd,
        .sector_size = sector_size,
    };
    
    if (NULL == _nio_dev_install(identifier, &mgr_if, (void *)&pmgr_init)) {
        return PMGR_INSUF_MEMORY;
    }
    
    return MQX_OK;
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name  : _io_part_mgr_init
* Returned Value : int error code
* Comments       : installs the partition manager device
*
*
*END*--------------------------------------------------------------------*/

int _io_part_mgr_init
    (
     void *init_data,
     
     void **dev_context
    )
{
    struct pmgr_init_struct *pmgr_init = (struct pmgr_init_struct *)init_data;
    PART_MGR_STRUCT_PTR pm_struct_ptr;

    pm_struct_ptr = MFS_mem_alloc_system_zero( sizeof(PART_MGR_STRUCT));

    if ( pm_struct_ptr == NULL )
    {
        return PMGR_INSUF_MEMORY;
    }

    _mem_set_type(pm_struct_ptr, MEM_TYPE_PART_MGR_STRUCT);

    /*
    ** Create a light weight semaphore
    */
    if (MQX_OK != _lwsem_create(&(pm_struct_ptr->SEM), 0)) {
        _mem_free(pm_struct_ptr);
        errno = PMGR_INSUF_MEMORY;
        return -1;
    }

    /* Store the handle of the device in the lower layer */
    pm_struct_ptr->DEV_FILE_PTR = pmgr_init->dev_fd;
    pm_struct_ptr->INSTANCES = 0;

    _lwsem_post(&(pm_struct_ptr->SEM));
    
    *dev_context = (void *)pm_struct_ptr;

    return 0;

}


/*FUNCTION*---------------------------------------------------------------
*
* Function Name  : _io_part_mgr_deinit
* Returned Value : int32_t error code
* Comments       : uninstalls the partition manager device
*
*
*END*--------------------------------------------------------------------*/

int _io_part_mgr_deinit
    (
    void *dev_context  /*[IN] The identifier of the device */
    )
{
    PART_MGR_STRUCT_PTR pm_struct_ptr = (PART_MGR_STRUCT_PTR) dev_context;

    _lwsem_destroy(&pm_struct_ptr->SEM);
    _mem_free(pm_struct_ptr);

    return MFS_OK;
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name  : _io_part_mgr_uninstall
* Returned Value : int32_t error code
* Comments       : uninstalls the partition manager device
*
*
*END*--------------------------------------------------------------------*/

int32_t _io_part_mgr_uninstall
    (
    char          *identifier    /*[IN] The identifier string of the device */
    )
{
    return _nio_dev_uninstall(identifier);
}

/*FUNCTION*---------------------------------------------------------------
*
* Function Name  : _io_part_mgr_open
* Returned Value : MQX_OK
* Comments       : Does nothing. All the initialization is done in
*                  the io_part_mgr_install function
*
*END*--------------------------------------------------------------------*/

int _io_part_mgr_open
    (
    void *dev_context,
    
    const char *open_name_ptr,
    
    int flags,
    
    void **fp_context
    )
{
    PART_MGR_STRUCT_PTR       pm_struct_ptr;
    PMGR_PART_INFO_STRUCT_PTR part_info_ptr;
    int                       dev_fd;

    uint32_t                   sector_size;
    int32_t                    error;
    int32_t                    part_num;

    const char                 *file_name_ptr;

    file_name_ptr = open_name_ptr;
    while (*file_name_ptr && *file_name_ptr++ != ':') {}

    if (file_name_ptr[0])
    {
        part_num = file_name_ptr[0] - '0';
        if (file_name_ptr[1] != '\0' || part_num<0 || part_num>PMGR_MAX_PARTITIONS) {
            errno = PMGR_INVALID_PARTITION;
            return -1;
        }
    }
    else
    {
        part_num = 0;
    }

    pm_struct_ptr = (PART_MGR_STRUCT_PTR)dev_context;
    dev_fd = pm_struct_ptr->DEV_FILE_PTR;

    _lwsem_wait(&(pm_struct_ptr->SEM));

    /* Validate device (check block mode and size) if no other instances exist so far */
    if (pm_struct_ptr->INSTANCES == 0)
    {
        error = _mfs_validate_device(dev_fd, &sector_size, &pm_struct_ptr->BLOCK_MODE);
        if ( error )
        {
            _lwsem_post(&(pm_struct_ptr->SEM));
            errno = error;
            return -1;
        }
        pm_struct_ptr->DEV_SECTOR_SIZE = sector_size;
        pm_struct_ptr->DEV_NUM_SECTORS = 0;
        error = _nio_ioctl(dev_fd, IO_IOCTL_GET_NUM_SECTORS, &(pm_struct_ptr->DEV_NUM_SECTORS));
        if ( error != MFS_NO_ERROR )
        {
            _lwsem_post(&(pm_struct_ptr->SEM));
            errno = error;
            return -1;
        }
    }

    part_info_ptr = MFS_mem_alloc_system_zero(sizeof(PMGR_PART_INFO_STRUCT));
    if (part_info_ptr == NULL )
    {
        _lwsem_post(&(pm_struct_ptr->SEM));
        errno = PMGR_INSUF_MEMORY;
        return -1;
    }

    /* Check whether to opening partition or operate on whole device (part_num==0) */
    if (part_num)
    {
        part_info_ptr->SLOT = part_num;
        if (_pmgr_get_part_info(pm_struct_ptr, part_info_ptr))
        {
            _mem_free(part_info_ptr);
            _lwsem_post(&(pm_struct_ptr->SEM));
            errno = PMGR_INVALID_PARTITION;
            return -1;
        }
    }
    else
    {
        /* Use size of device as length of the partition */
        part_info_ptr->LENGTH = pm_struct_ptr->DEV_NUM_SECTORS;
    }

    *fp_context = (void *)part_info_ptr;

    _lwsem_post(&(pm_struct_ptr->SEM));
    return MFS_OK;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : _io_part_mgr_close
* Returned Value : MQX_OK
* Comments       : Frees the memory allocated to hold the partition info
*
*END*--------------------------------------------------------------------*/

int _io_part_mgr_close
    (
       void *dev_context,
       
       void *fp_context
    )
{
    PART_MGR_STRUCT_PTR   pm_struct_ptr;

    pm_struct_ptr = (PART_MGR_STRUCT_PTR)dev_context;

    _lwsem_wait(&(pm_struct_ptr->SEM));

    pm_struct_ptr->INSTANCES--;
    _mem_free(fp_context);

    _lwsem_post(&(pm_struct_ptr->SEM));

    return MFS_OK;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : _io_part_mgr_read
* Returned Value : The number of bytes read from device
* Comments       : Calls the read function of the next layer
*
*END*--------------------------------------------------------------------*/

int _io_part_mgr_read
    (
       void *dev_context,
       
       void *fp_context,
       
       void *data_ptr,
       
       size_t num
    )
{
    PART_MGR_STRUCT_PTR           pm_struct_ptr;
    PMGR_PART_INFO_STRUCT_PTR     part_info_ptr;
    uint32_t    location;
    uint32_t    part_start;
    uint32_t    part_end;
    int32_t     result;

    pm_struct_ptr = (PART_MGR_STRUCT_PTR)dev_context;
    part_info_ptr = (PMGR_PART_INFO_STRUCT_PTR)fp_context;

    /* If a partition is selected, also its start has to be defined */
    if (part_info_ptr->SLOT && part_info_ptr->START_SECTOR==0)
    {
        errno = PMGR_INVALID_PARTITION;
        return -1;
    }

    part_start = part_info_ptr->START_SECTOR;
    part_end = part_info_ptr->LENGTH ? part_start + part_info_ptr->LENGTH : 0;

    /* If not operating on a block device, multiply values by sector size */
    if (!(pm_struct_ptr->BLOCK_MODE))
    {
        part_start *= pm_struct_ptr->DEV_SECTOR_SIZE;
        part_end *= pm_struct_ptr->DEV_SECTOR_SIZE;
    }

    /* Calculate seek location for undelying device */
    location = part_start + part_info_ptr->LOCATION;
    if (part_end)
    {
        /* Check whether lhe location points into the partition area */
        if (location >= part_end)
        {
            errno = MFS_ERROR_SEEK;
            return -1;
        }
        /* Check whether the transfer would overflow the partition area and shorten it if necessary */
        if (location + num > part_end)
        {
            num = part_end - location;
        }
    }

    /* Lock to prevent concurrent access to underlying device */
    _lwsem_wait(&pm_struct_ptr->SEM);

    /* Perform seek and data transfer */
    result = lseek(pm_struct_ptr->DEV_FILE_PTR, location, SEEK_SET);
    if (result >= 0)
    {
        result = read(pm_struct_ptr->DEV_FILE_PTR, data_ptr, num);
    }

    /* Unlock */
    _lwsem_post(&pm_struct_ptr->SEM);

    return result;
}

/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : _io_part_mgr_write
* Returned Value : the number of bytes writen to device
* Comments       : calls the write function of the next layer
*
*END*--------------------------------------------------------------------*/

int _io_part_mgr_write
    (
       void *dev_context,
       
       void *fp_context,
       
       const void *data_ptr,
       
       size_t num
    )
{
    PART_MGR_STRUCT_PTR           pm_struct_ptr;
    PMGR_PART_INFO_STRUCT_PTR     part_info_ptr;
    uint32_t    location;
    uint32_t    part_start;
    uint32_t    part_end;
    int32_t     result;

    pm_struct_ptr = (PART_MGR_STRUCT_PTR)dev_context;
    part_info_ptr = (PMGR_PART_INFO_STRUCT_PTR)fp_context;

    /* If a partition is selected, also its start has to be defined */
    if (part_info_ptr->SLOT && part_info_ptr->START_SECTOR==0)
    {
        errno = PMGR_INVALID_PARTITION;
        return -1;
    }

    part_start = part_info_ptr->START_SECTOR;
    part_end = part_info_ptr->LENGTH ? part_start + part_info_ptr->LENGTH : 0;

    /* If not operating on a block device, multiply values by sector size */
    if (!(pm_struct_ptr->BLOCK_MODE))
    {
        part_start *= pm_struct_ptr->DEV_SECTOR_SIZE;
        part_end *= pm_struct_ptr->DEV_SECTOR_SIZE;
    }

    /* Calculate seek location for undelying device */
    location = part_info_ptr->LOCATION + part_start;
    if (part_info_ptr->LENGTH)
    {
        /* Check whether lhe location points into the partition area */
        if (location >= part_end)
        {
            errno = MFS_ERROR_SEEK;
            return -1;
        }
        /* Check whether the transfer would overflow the partition area and shorten it if necessary */
        if (location + num > part_end)
        {
            num = part_end - location;
        }
    }

    /* Lock to prevent concurrent access to underlying device */
    _lwsem_wait(&pm_struct_ptr->SEM);

    /* Perform seek and data transfer */
    result = lseek(pm_struct_ptr->DEV_FILE_PTR, location, SEEK_SET);
    if (result >= 0)
    {
        result = write(pm_struct_ptr->DEV_FILE_PTR, data_ptr, num);
    }

    /* Unlock */
    _lwsem_post(&pm_struct_ptr->SEM);

    return result;
}

/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : _io_part_mgr_lseek
* Returned Value : int32_t or error code
* Comments       : Executes ioctl commands
*
*END*--------------------------------------------------------------------*/
off_t _io_part_mgr_lseek
    (
       void *dev_context,
       
       void *fp_context,
       
       off_t offset,
       
       int mode
    )
{
    PMGR_PART_INFO_STRUCT_PTR fi = (PMGR_PART_INFO_STRUCT_PTR)fp_context;

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
        if (offset > 0 && (fi->LENGTH + offset < fi->LENGTH)) {
            retval = (off_t)EINVAL;
        }
        else if (offset < 0 && (fi->LENGTH + offset > fi->LENGTH)) {
            retval = (off_t)EINVAL;
        }
        else {
            fi->LOCATION = fi->LENGTH + offset;
        }
        break;
      default:
        errno = EINVAL;
        retval = (off_t)-1;
        break;
   }
   
   return retval == (off_t)0 ? fi->LOCATION : retval;
}

/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : _io_part_mgr_ioctl
* Returned Value : int32_t or error code
* Comments       : Executes ioctl commands
*
*END*--------------------------------------------------------------------*/

int _io_part_mgr_ioctl
    (
       void *dev_context,
       
       void *fp_context,
       
       unsigned long int request,
       
       va_list ap
    )
{
    PART_MGR_STRUCT_PTR           pm_struct_ptr;
    PMGR_PART_INFO_STRUCT_PTR     part_info_ptr;
    uint32_t                      result = MQX_OK;
    uint32_t                      *param_ptr;

    pm_struct_ptr = (PART_MGR_STRUCT_PTR)dev_context;
    part_info_ptr = (PMGR_PART_INFO_STRUCT_PTR)fp_context;

    param_ptr = va_arg(ap, uint32_t *);

    switch ( request )
    {

        case IO_IOCTL_GET_BLOCK_SIZE:
            *param_ptr = pm_struct_ptr->DEV_SECTOR_SIZE;
            break;

        case IO_IOCTL_GET_NUM_SECTORS:
            *param_ptr = part_info_ptr->LENGTH;
            break;

        case IO_IOCTL_VAL_PART:
            if (param_ptr == NULL)
            {
                /* Validate current partition */
                if (part_info_ptr->START_SECTOR==0 || part_info_ptr->LENGTH==0)
                    result = PMGR_INVALID_PARTITION;
                else
                    result = _pmgr_check_part_type(part_info_ptr->TYPE);
            }
            else if (part_info_ptr->SLOT)
            {
                /* Available only if operating on whole device */
                result = MQX_IO_OPERATION_NOT_AVAILABLE;
                break;
            }
            else
            {
                PMGR_PART_INFO_STRUCT part_info;
                _mem_zero(&part_info, sizeof(PMGR_PART_INFO_STRUCT));
                /* In case partition number is 0 the _pmgr_get_part_info function performs just MBR validation */
                part_info.SLOT = *param_ptr;
                _lwsem_wait(&pm_struct_ptr->SEM);
                result = _pmgr_get_part_info(pm_struct_ptr, &part_info);
                _lwsem_post(&pm_struct_ptr->SEM);
                if (!result && part_info.SLOT)
                {
                    result = _pmgr_check_part_type(part_info.TYPE);
                }
            }
            break;

        case IO_IOCTL_SEL_PART:
            if (*param_ptr && part_info_ptr->SLOT)
            {
                /*
                 * Available only if operating on whole device.
                 * This prevents selecting partition multiple times through a single handle,
                 * e.g. if the handle is "recycled" for mutliple MFS instances, which is wrong.
                 * To select a different partition, partition 0 (whole device) has to be selected first
                 */
                result = MQX_IO_OPERATION_NOT_AVAILABLE;
                break;
            }
            _mem_zero(part_info_ptr, sizeof(PMGR_PART_INFO_STRUCT));
            if (*param_ptr == 0)
            {
                /* Use size of device as length of the partition */
                part_info_ptr->LENGTH = pm_struct_ptr->DEV_NUM_SECTORS;
            }
            else
            {
                part_info_ptr->SLOT = *param_ptr;
                _lwsem_wait(&pm_struct_ptr->SEM);
                result = _pmgr_get_part_info(pm_struct_ptr, part_info_ptr);
                _lwsem_post(&pm_struct_ptr->SEM);
            }
            /* Reset location to the beginning of the partition/device */
            part_info_ptr->LOCATION = 0;
            break;

        case IO_IOCTL_GET_PARTITION:
            if (((PMGR_PART_INFO_STRUCT_PTR)param_ptr)->SLOT == 0)
            {
                /* Get info about current partition */
                _mem_copy(part_info_ptr, param_ptr, sizeof(PMGR_PART_INFO_STRUCT));
                break;
            }
            if (part_info_ptr->SLOT)
            {
                /* Available only if operating on whole device */
                result = MQX_IO_OPERATION_NOT_AVAILABLE;
                break;
            }
            _lwsem_wait(&pm_struct_ptr->SEM);
            result = _pmgr_get_part_info(pm_struct_ptr, (PMGR_PART_INFO_STRUCT_PTR) param_ptr);
            _lwsem_post(&pm_struct_ptr->SEM);
            break;

        case IO_IOCTL_SET_PARTITION:
            if (part_info_ptr->SLOT)
            {
                /* Available only if operating on whole device */
                result = MQX_IO_OPERATION_NOT_AVAILABLE;
                break;
            }
            _lwsem_wait(&pm_struct_ptr->SEM);
            if (pm_struct_ptr->INSTANCES > 1)
            {
                /* Available if this is the only instance to avoid inconsistencies */
                result = MFS_ERROR_DEVICE_BUSY;
            }
            else {
                result = _pmgr_set_part_info(pm_struct_ptr, (PMGR_PART_INFO_STRUCT_PTR) param_ptr);
            }
            _lwsem_post(&pm_struct_ptr->SEM);
            break;

        case IO_IOCTL_CLEAR_PARTITION:
            if (part_info_ptr->SLOT)
            {
                /* Available only if operating on whole device */
                result = MQX_IO_OPERATION_NOT_AVAILABLE;
                break;
            }
            _lwsem_wait(&pm_struct_ptr->SEM);
            if (pm_struct_ptr->INSTANCES > 1)
            {
                /* Available if this is the only instance to avoid inconsistencies */
                result = MFS_ERROR_DEVICE_BUSY;
            }
            else {
                result = _pmgr_clear_part_info(pm_struct_ptr, *param_ptr);
            }
            _lwsem_post(&pm_struct_ptr->SEM);
            break;

        case IO_IOCTL_USE_PARTITION:
            /*
             * Force switching to area defined by submitted PMGR_PART_INFO_STRUCT,
             * without storing anything to the partition table on the device.
             * This may be useful to divide the device to areas defined elsewhere.
             */
            if (part_info_ptr->SLOT)
            {
                /* Available only if operating on whole device */
                result = MQX_IO_OPERATION_NOT_AVAILABLE;
                break;
            }
            _mem_copy(param_ptr, part_info_ptr, sizeof(PMGR_PART_INFO_STRUCT));
            /* Reset location to the beginning of the partition/device */
            part_info_ptr->LOCATION = 0;

            break;

        default:
            /* Pass IOCTL command to lower layer */
            result = ioctl(pm_struct_ptr->DEV_FILE_PTR, request, param_ptr);
            if (result == -1) {
                result = errno;
            }
            break;
    }

    if (result != MQX_OK) {
        result = errno;
        return -1;
    }

    return result;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  _pmgr_check_part_type
* Returned Value   :  int32_t error code
* Comments  :
*     Checks partition type, returns success if it matches one of the FAT types
*
*END*---------------------------------------------------------------------*/

int32_t _pmgr_check_part_type
    (
        uint32_t part_type
    )
{
    switch (part_type)
    {
        case PMGR_PARTITION_NOT_USED:
            return PMGR_INVALID_PARTITION;

        case PMGR_PARTITION_FAT_12_BIT:
        case PMGR_PARTITION_FAT_16_BIT:
        case PMGR_PARTITION_HUGE:
        case PMGR_PARTITION_HUGE_LBA:
        case PMGR_PARTITION_FAT32:
        case PMGR_PARTITION_FAT32_LBA:
            break;

        default:
            return PMGR_UNKNOWN_PARTITION;
    }
    return MQX_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  _pmgr_validate_mbr
* Returned Value   :  int32_t error code
* Comments  :
*     Validates MBR, checks signature and sanity of the partition table
*
*END*---------------------------------------------------------------------*/

int32_t _pmgr_validate_mbr
    (
    char *mbr_ptr
    )
{
    int i, j;
    PMGR_PART_ENTRY_STRUCT_PTR disk_entry;

    uint32_t part_start[PMGR_MAX_PARTITIONS];
    uint32_t part_length[PMGR_MAX_PARTITIONS];

    /* Check partition table signature bytes */
    if (((uint8_t *)mbr_ptr)[510]!=0x55 || ((uint8_t *)mbr_ptr)[511]!=0xAA)
        return PMGR_INVALID_PARTTABLE;

    disk_entry = (PMGR_PART_ENTRY_STRUCT_PTR)(((uint8_t *)mbr_ptr) + PMGR_PARTITION_TABLE_OFFSET);

    for (i=0; i<PMGR_MAX_PARTITIONS; i++)
    {
        part_start[i] = mqx_dtohl(disk_entry[i].START_SECTOR);
        part_length[i] = mqx_dtohl(disk_entry[i].LENGTH);

        /* If type is zero, the whole partition entry should be unused (zero) */
        if ((disk_entry[i].TYPE == 0) && (part_length[i] != 0 || part_start[i] != 0))
            return PMGR_INVALID_PARTTABLE;

        /* If type is non-zero, the partition start and length shall contain meaningfull values */
        if ((disk_entry[i].TYPE != 0) && (part_length[i] == 0 || part_start[i] == 0))
            return PMGR_INVALID_PARTTABLE;

        if (part_length[i] == 0)
            continue; /* No overlap check necessary, go for next record */

        /* Check for overlaps */
        for (j=0; j<i; j++)
        {
            if (part_length[j] == 0)
                continue; /* No overlap check necessary, go for next record */

            /*
             * Two partitions do NOT overlap if: start of one is behind end of the other OR vice versa.
             * The condition below is negation of this sentence.
             */
            if ((part_start[i] < part_start[j] + part_length[j]) && (part_start[i] + part_length[i] > part_start[j]))
                return PMGR_INVALID_PARTTABLE;
        }
    }

    return MQX_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  _pmgr_get_part_info
* Returned Value   :  int32_t error code
* Comments  :
*     Read the partition table entry from disk
*
*END*---------------------------------------------------------------------*/

int32_t _pmgr_get_part_info
    (
    PART_MGR_STRUCT_PTR        pm_struct_ptr,   /*[IN] the partition manager information */
    PMGR_PART_INFO_STRUCT_PTR  host_entry       /*[IN]/[OUT] the entry to read from disk */
    )
{
    int32_t                       error_code = MQX_OK;
    char                     *mbr_ptr;
    int32_t                       size;
    PMGR_PART_ENTRY_STRUCT_PTR   disk_entry;

    mbr_ptr =  MFS_mem_alloc_system(pm_struct_ptr->DEV_SECTOR_SIZE);
    if ( mbr_ptr == NULL )
    {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type(mbr_ptr, MEM_TYPE_PART_MGR_SECTOR);

    size = pm_struct_ptr->BLOCK_MODE ? 1 : pm_struct_ptr->DEV_SECTOR_SIZE;

    lseek(pm_struct_ptr->DEV_FILE_PTR, 0, SEEK_SET);
    //TODO: check errno lseek

    if (read(pm_struct_ptr->DEV_FILE_PTR, mbr_ptr, size) != size)
    {
        error_code = errno;
    }
    else
    {
        error_code = _pmgr_validate_mbr(mbr_ptr);
    }

    if (!error_code && host_entry && host_entry->SLOT)
    {
        if (host_entry->SLOT > PMGR_MAX_PARTITIONS)
        {
            error_code = PMGR_INVALID_PARTITION;
        }
        else {
            disk_entry = (PMGR_PART_ENTRY_STRUCT_PTR)(mbr_ptr + PMGR_PARTITION_TABLE_OFFSET);
            _pmgr_disk_to_host(disk_entry + host_entry->SLOT - 1, host_entry);
            if (host_entry->TYPE == 0 || host_entry->START_SECTOR == 0 || host_entry->LENGTH == 0)
            {
                error_code = PMGR_INVALID_PARTITION;
            }
        }
    }

    _mem_free(mbr_ptr);
    return error_code;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  _pmgr_set_part_info
* Returned Value   :  int32_t error code
* Comments  :
*     Will overwrite the partition table entry with the new information
*     received (if opcode == 0). If the opcode is a partition number, this
*     function will clear that partition. Will return error code upon error.
*
*END*---------------------------------------------------------------------*/

int32_t _pmgr_set_part_info
    (
    PART_MGR_STRUCT_PTR        pm_struct_ptr,   /*[IN] the partitionn manager information */
    PMGR_PART_INFO_STRUCT_PTR  host_entry       /*[IN] the entry to write to disk */
    )
{
    int32_t                       error_code = MQX_OK;
    char                          *mbr_ptr;
    int32_t                       size;
    PMGR_PART_ENTRY_STRUCT_PTR   disk_entry;

    if (!host_entry || host_entry->SLOT==0 || host_entry->SLOT>PMGR_MAX_PARTITIONS)
    {
        return PMGR_INVALID_PARTITION;
    }

    mbr_ptr =  MFS_mem_alloc_system(pm_struct_ptr->DEV_SECTOR_SIZE);
    if ( mbr_ptr == NULL )
    {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type(mbr_ptr, MEM_TYPE_PART_MGR_SECTOR);

    size = pm_struct_ptr->BLOCK_MODE ? 1 : pm_struct_ptr->DEV_SECTOR_SIZE;

    lseek(pm_struct_ptr->DEV_FILE_PTR, 0, SEEK_SET);
    //TODO: check errno lseek

    if (read(pm_struct_ptr->DEV_FILE_PTR, mbr_ptr, size) != size)
    {
        error_code = errno;
    }
    else
    {
        error_code = _pmgr_validate_mbr(mbr_ptr);
    }

    if (!error_code)
    {
        disk_entry = (PMGR_PART_ENTRY_STRUCT_PTR)(mbr_ptr + PMGR_PARTITION_TABLE_OFFSET);
        _pmgr_host_to_disk(host_entry, disk_entry + host_entry->SLOT - 1);
        error_code = _pmgr_validate_mbr(mbr_ptr); /* Check sanity of modified MBR */
        if (!error_code)
        {
            lseek(pm_struct_ptr->DEV_FILE_PTR, 0, SEEK_SET);
            //TODO: check errno lseek

            do {
                error_code = write(pm_struct_ptr->DEV_FILE_PTR, mbr_ptr, size);
                if (error_code > 0) {
                    size = size - error_code; //error_code contain written size
                }
            } while (error_code > 0);

            if (error_code < 0)
            {
                error_code = errno;
            }
        }
    }

    _mem_free(mbr_ptr);
    return error_code;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  _pmgr_clear_part_info
* Returned Value   :  int32_t error code
* Comments  :
*     Clears partition information for given partition
*
*END*---------------------------------------------------------------------*/

int32_t _pmgr_clear_part_info
    (
    PART_MGR_STRUCT_PTR        pm_struct_ptr,   /*[IN] the partitionn manager information */
    unsigned char                      part_num         /*[IN] the partition to clear */
    )
{
    int32_t                       error_code = MQX_OK;
    char                     *mbr_ptr;
    int32_t                       size;
    PMGR_PART_ENTRY_STRUCT_PTR   disk_entry;

    if (part_num>PMGR_MAX_PARTITIONS)
    {
        return PMGR_INVALID_PARTITION;
    }

    mbr_ptr =  MFS_mem_alloc_system(pm_struct_ptr->DEV_SECTOR_SIZE);
    if ( mbr_ptr == NULL )
    {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type(mbr_ptr, MEM_TYPE_PART_MGR_SECTOR);

    size = pm_struct_ptr->BLOCK_MODE ? 1 : pm_struct_ptr->DEV_SECTOR_SIZE;

    lseek(pm_struct_ptr->DEV_FILE_PTR, 0, SEEK_SET);
    //TODO: check errno lseek

    if (read(pm_struct_ptr->DEV_FILE_PTR, mbr_ptr, size) != size)
    {
        error_code = errno;
    }
    else
    {
        error_code = _pmgr_validate_mbr(mbr_ptr);
    }

    if (error_code == PMGR_INVALID_PARTITION && part_num == 0)
    {
        /* Create blank MBR */
        _mem_zero(mbr_ptr, pm_struct_ptr->DEV_SECTOR_SIZE);
        mbr_ptr[510] = 0x55;
        mbr_ptr[511] = 0xAA;
        error_code = 0;
    }
    else if (!error_code)
    {
        disk_entry = (PMGR_PART_ENTRY_STRUCT_PTR)(mbr_ptr + PMGR_PARTITION_TABLE_OFFSET);
        if (!part_num)
        {
            _mem_zero(disk_entry, PMGR_MAX_PARTITIONS*sizeof(PMGR_PART_ENTRY_STRUCT));
        }
        else
        {
            _mem_zero(disk_entry + part_num - 1, sizeof(PMGR_PART_ENTRY_STRUCT));
        }
    }

    if (!error_code)
    {
        lseek(pm_struct_ptr->DEV_FILE_PTR, 0, SEEK_SET);
        //TODO: check errno lseek

        do {
            error_code = write(pm_struct_ptr->DEV_FILE_PTR, mbr_ptr, size);
            if (error_code > 0) {
                size = size - error_code; //error_code contain written size
            }
        } while (error_code > 0);
    }

    _mem_free(mbr_ptr);
    return error_code;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  _pmgr_disk_to_host
* Returned Value   :  void
* Comments  :
*     Will copy a partition table entry from its disk format to a host format
*
*END*---------------------------------------------------------------------*/


void _pmgr_disk_to_host
    (
    PMGR_PART_ENTRY_STRUCT_PTR   disk_entry,  /*[IN] ptr to a partition table entry in disk format */
    PMGR_PART_INFO_STRUCT_PTR    part_entry   /*[OUT] ptr to a partition table entry in a more usable format */
    )
{
    part_entry->TYPE = disk_entry->TYPE;
    part_entry->ACTIVE_FLAG = disk_entry->ACTIVE_FLAG;
    part_entry->START_SECTOR = mqx_dtohl(disk_entry->START_SECTOR);
    part_entry->LENGTH = mqx_dtohl(disk_entry->LENGTH);
    part_entry->HEADS = 0;
    part_entry->SECTORS = 0;
    part_entry->CYLINDERS = 0;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  _pmgr_host_to_disk
* Returned Value   :  void
* Comments  :
*     Will copy a partition table entry from its host format to the disk format
*
*END*---------------------------------------------------------------------*/

void _pmgr_host_to_disk
    (
    PMGR_PART_INFO_STRUCT_PTR              part_entry,  /*[IN] ptr to a partition table entry in a usable format */
    PMGR_PART_ENTRY_STRUCT_PTR             disk_entry   /*[OUT] ptr to a partition table entry in disk format */
    )
{
    uint32_t     temp,cyl,head,sec,hds_cyl,sct_trk,lba;

    disk_entry->ACTIVE_FLAG = 0;
    disk_entry->TYPE = part_entry->TYPE;
    mqx_htodl(disk_entry->START_SECTOR, part_entry->START_SECTOR);
    mqx_htodl(disk_entry->LENGTH, part_entry->LENGTH);

    /* Check if CHS is present and valid */
    if ( !part_entry->CYLINDERS || !part_entry->HEADS || !part_entry->SECTORS || part_entry->CYLINDERS>1024 || part_entry->SECTORS>63 )
    {
        disk_entry->START_HEAD = 0;
        disk_entry->START_SECT_CYL = 0;
        disk_entry->START_CYLINDER = 0;
        disk_entry->ENDING_HEAD = 0;
        disk_entry->ENDING_SECT_CYL = 0;
        disk_entry->ENDING_CYLINDER = 0;
    }
    else
    {
        /* setup info used for calculations */
        sct_trk = part_entry->SECTORS;
        hds_cyl = part_entry->HEADS;

        /* Calculate starting CHS */
        lba = part_entry->START_SECTOR;
        cyl = lba / (hds_cyl * sct_trk);
        temp = lba % (hds_cyl * sct_trk);
        head = temp / sct_trk;
        sec = temp % sct_trk + 1;

        if (cyl>1023)
        {
            /* Indicate invalid CHS entry */
            disk_entry->START_HEAD = 254;
            disk_entry->START_SECT_CYL = 0xFF;
            disk_entry->START_CYLINDER = 0xFF;
        }
        else
        {
            disk_entry->START_HEAD = (unsigned char) head;
            disk_entry->START_SECT_CYL = (unsigned char) ((sec & 0x3f) + ((cyl & 0x0300) >> 2));
            disk_entry->START_CYLINDER = (unsigned char) (cyl & 0x0FF);
        }

        /* Calculate the ending CHS */
        lba = part_entry->LENGTH + part_entry->START_SECTOR;
        cyl = lba / (hds_cyl * sct_trk);
        temp = lba % (hds_cyl * sct_trk);
        head = temp / sct_trk;
        sec = temp % sct_trk + 1;

        if (cyl>1023)
        {
            /* Indicate invalid CHS entry */
            disk_entry->ENDING_HEAD = 254;
            disk_entry->ENDING_SECT_CYL = 0xFF;
            disk_entry->ENDING_CYLINDER = 0xFF;
        }
        else
        {
            disk_entry->ENDING_HEAD = (unsigned char) head;
            disk_entry->ENDING_SECT_CYL = (unsigned char) ((sec & 0x3f) + ((cyl & 0x0300) >> 2));
            disk_entry->ENDING_CYLINDER = (unsigned char) (cyl & 0x0FF);
        }
    }
}

#else //MQX_USE_NIO

static int32_t _io_part_mgr_uninstall_internal(IO_DEVICE_STRUCT_PTR dev_ptr);


/*FUNCTION*---------------------------------------------------------------
*
* Function Name  : _io_part_mgr_install
* Returned Value : int32_t error code
* Comments       : installs the partition manager device
*
*
*END*--------------------------------------------------------------------*/

int32_t _io_part_mgr_install
    (
    MQX_FILE_PTR dev_fd,     /*[IN] Handle of the device on which to install the partition manager */
    char    *identifier, /*[IN] The new name of the partition manager device */
    uint32_t     sector_size /*[IN] The size of sectors in bytes on the lower level device */
    )
{
    PART_MGR_STRUCT_PTR           pm_struct_ptr;
    int32_t                        error_code;

    pm_struct_ptr = MFS_mem_alloc_system_zero( sizeof(PART_MGR_STRUCT));
    if ( pm_struct_ptr == NULL )
    {
        return( PMGR_INSUF_MEMORY );
    }

    _mem_set_type(pm_struct_ptr, MEM_TYPE_PART_MGR_STRUCT);

    /*
    ** Create a light weight semaphore
    */
    _lwsem_create(&(pm_struct_ptr->SEM), 0);

    /* Store the handle of the device in the lower layer */
    pm_struct_ptr->DEV_FILE_PTR = dev_fd;
    pm_struct_ptr->INSTANCES = 0;

    error_code = _io_dev_install_ext( identifier,
        _io_part_mgr_open,
        _io_part_mgr_close,
        _io_part_mgr_read,
        _io_part_mgr_write,
        (_mqx_int(_CODE_PTR_)(MQX_FILE_PTR,_mqx_uint,void *))_io_part_mgr_ioctl,
        _io_part_mgr_uninstall_internal,
        (void *) pm_struct_ptr );

    if ( error_code )
    {
        _lwsem_destroy(&pm_struct_ptr->SEM);
        _mem_free(pm_struct_ptr);
    }
    else
    {
        _lwsem_post(&(pm_struct_ptr->SEM));
    }

    return error_code;

}


/*FUNCTION*---------------------------------------------------------------
*
* Function Name  : _io_part_mgr_uninstall_internal
* Returned Value : int32_t error code
* Comments       : uninstalls the partition manager device
*
*
*END*--------------------------------------------------------------------*/

static int32_t _io_part_mgr_uninstall_internal
    (
    IO_DEVICE_STRUCT_PTR        dev_ptr     /*[IN] The identifier of the device */
    )
{
    PART_MGR_STRUCT_PTR pm_struct_ptr;

    pm_struct_ptr = (PART_MGR_STRUCT_PTR) dev_ptr->DRIVER_INIT_PTR;

    _lwsem_wait(&(pm_struct_ptr->SEM));

    if (pm_struct_ptr->INSTANCES)
    {
        _lwsem_post(&(pm_struct_ptr->SEM));
        return IO_ERROR_DEVICE_BUSY;
    }

    _lwsem_destroy(&pm_struct_ptr->SEM);
    _mem_free(pm_struct_ptr);

    return IO_OK;
}

#if MFSCFG_DEPRICATED_UNINSTALL
/*FUNCTION*---------------------------------------------------------------
*
* Function Name  : _io_part_mgr_uninstall
* Returned Value : int32_t error code
* Comments       : uninstalls the partition manager device
*
*
*END*--------------------------------------------------------------------*/

int32_t _io_part_mgr_uninstall
    (
    char          *identifier    /*[IN] The identifier string of the device */
    )
{
    return _io_dev_uninstall(identifier);
}
#endif


/*FUNCTION*---------------------------------------------------------------
*
* Function Name  : _io_part_mgr_open
* Returned Value : MQX_OK
* Comments       : Does nothing. All the initialization is done in
*                  the io_part_mgr_install function
*
*END*--------------------------------------------------------------------*/

int32_t _io_part_mgr_open
    (
    MQX_FILE_PTR   fd_ptr,          /* [IN] the file pointer of the partition manager to open */
    char       *open_name_ptr,   /* [IN] the name of the device that is being opened */
    char       *open_mode_ptr    /* [IN] the flags that determine the open mode */
    )
{
    PART_MGR_STRUCT_PTR       pm_struct_ptr;
    PMGR_PART_INFO_STRUCT_PTR part_info_ptr;
    MQX_FILE_PTR              dev_fd;

    int32_t                    error;
    int32_t                    part_num;

    char                  *file_name_ptr;

    file_name_ptr = open_name_ptr;
    while (*file_name_ptr && *file_name_ptr++ != ':') {}

    if (file_name_ptr[0])
    {
        part_num = file_name_ptr[0] - '0';
        if (file_name_ptr[1] != '\0' || part_num<0 || part_num>PMGR_MAX_PARTITIONS)
            return PMGR_INVALID_PARTITION;
    }
    else
    {
        part_num = 0;
    }

    pm_struct_ptr = (PART_MGR_STRUCT_PTR)fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    dev_fd = pm_struct_ptr->DEV_FILE_PTR;

    _lwsem_wait(&(pm_struct_ptr->SEM));

    /* Validate device (check block mode and size) if no other instances exist so far */
    if (pm_struct_ptr->INSTANCES == 0)
    {
        error = _mfs_validate_device(dev_fd, &pm_struct_ptr->DEV_SECTOR_SIZE, &pm_struct_ptr->BLOCK_MODE);
        if (error)
        {
            _lwsem_post(&(pm_struct_ptr->SEM));
            return error;
        }

        pm_struct_ptr->DEV_NUM_SECTORS = 0;
        ioctl(dev_fd, IO_IOCTL_GET_NUM_SECTORS, &(pm_struct_ptr->DEV_NUM_SECTORS));

        pm_struct_ptr->ALIGNMENT = 1; /* Default alignment is one byte (which means no special requirement) */
        ioctl(dev_fd, IO_IOCTL_GET_REQ_ALIGNMENT, &pm_struct_ptr->ALIGNMENT);
    }

    part_info_ptr = MFS_mem_alloc_system_zero(sizeof(PMGR_PART_INFO_STRUCT));
    if (part_info_ptr == NULL )
    {
        _lwsem_post(&(pm_struct_ptr->SEM));
        return PMGR_INSUF_MEMORY;
    }
    _mem_set_type(part_info_ptr, MEM_TYPE_PART_INFO_STRUCT);

    /* Check whether to open partition or operate on whole device (part_num==0) */
    if (part_num)
    {
        part_info_ptr->SLOT = part_num;
        if (_pmgr_get_part_info(pm_struct_ptr, part_info_ptr))
        {
            _mem_free(part_info_ptr);
            _lwsem_post(&(pm_struct_ptr->SEM));
            return PMGR_INVALID_PARTITION;
        }
    }
    else
    {
        /* Use size of device as length of the partition */
        part_info_ptr->LENGTH = pm_struct_ptr->DEV_NUM_SECTORS;
    }

    fd_ptr->DEV_DATA_PTR = part_info_ptr;
    pm_struct_ptr->INSTANCES++;

    _lwsem_post(&(pm_struct_ptr->SEM));
    return IO_OK;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : _io_part_mgr_close
* Returned Value : MQX_OK
* Comments       : Frees the memory allocated to hold the partition info
*
*END*--------------------------------------------------------------------*/

int32_t _io_part_mgr_close
    (

    MQX_FILE_PTR   fd_ptr   /* [IN] the file pointer of the partition manager to close */
    )
{
    PART_MGR_STRUCT_PTR   pm_struct_ptr;

    pm_struct_ptr = (PART_MGR_STRUCT_PTR)fd_ptr->DEV_PTR->DRIVER_INIT_PTR;

    _lwsem_wait(&(pm_struct_ptr->SEM));

    pm_struct_ptr->INSTANCES--;
    _mem_free(fd_ptr->DEV_DATA_PTR);

    _lwsem_post(&(pm_struct_ptr->SEM));

    return IO_OK;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : _io_part_mgr_read
* Returned Value : The number of bytes read from device
* Comments       : Calls the read function of the next layer
*
*END*--------------------------------------------------------------------*/

int32_t _io_part_mgr_read
    (
    MQX_FILE_PTR   fd_ptr,      /* [IN] the file pointer of the partition manager to read from */
    char       *data_ptr,    /* [IN] the data location to read to */
    int32_t         num          /* [IN] the number of bytes to read */
    )
{
    PART_MGR_STRUCT_PTR           pm_struct_ptr;
    PMGR_PART_INFO_STRUCT_PTR     part_info_ptr;
    uint32_t    location;
    uint32_t    part_start;
    uint32_t    part_end;
    int32_t     result;

    pm_struct_ptr = (PART_MGR_STRUCT_PTR)fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    part_info_ptr = (PMGR_PART_INFO_STRUCT_PTR)fd_ptr->DEV_DATA_PTR;

    /* If a partition is selected, also its start has to be defined */
    if (part_info_ptr->SLOT && part_info_ptr->START_SECTOR==0)
    {
        fd_ptr->ERROR = PMGR_INVALID_PARTITION;
        return IO_ERROR;
    }

    part_start = part_info_ptr->START_SECTOR;
    part_end = part_info_ptr->LENGTH ? part_start + part_info_ptr->LENGTH : 0;

    /* If not operating on a block device, multiply values by sector size */
    if (!(pm_struct_ptr->BLOCK_MODE))
    {
        part_start *= pm_struct_ptr->DEV_SECTOR_SIZE;
        part_end *= pm_struct_ptr->DEV_SECTOR_SIZE;
    }

    /* Calculate seek location for undelying device */
    location = part_start + fd_ptr->LOCATION;
    if (part_end)
    {
        /* Check whether lhe location points into the partition area */
        if (location >= part_end)
        {
            fd_ptr->ERROR = IO_ERROR_SEEK;
            return IO_ERROR;
        }
        /* Check whether the transfer would overflow the partition area and shorten it if necessary */
        if (location + num > part_end)
        {
            num = part_end - location;
        }
    }

    /* Lock to prevent concurrent access to underlying device */
    _lwsem_wait(&pm_struct_ptr->SEM);

    /* Perform seek and data transfer */
    result = fseek(pm_struct_ptr->DEV_FILE_PTR, location, IO_SEEK_SET);
    if (result == MQX_OK)
    {
        result = read( ((PART_MGR_STRUCT_PTR) fd_ptr->DEV_PTR->DRIVER_INIT_PTR)->DEV_FILE_PTR, data_ptr, num);
    }
    fd_ptr->ERROR = pm_struct_ptr->DEV_FILE_PTR->ERROR;

    /* Unlock */
    _lwsem_post(&pm_struct_ptr->SEM);

    return result;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : _io_part_mgr_write
* Returned Value : the number of bytes writen to device
* Comments       : calls the write function of the next layer
*
*END*--------------------------------------------------------------------*/

int32_t _io_part_mgr_write
    (
    MQX_FILE_PTR   fd_ptr,      /* [IN] the file pointer of the partition manager to read from */
    char       *data_ptr,    /* [IN] the data location to read from */
    int32_t         num          /* [IN] the number of bytes to write */
    )
{
    PART_MGR_STRUCT_PTR           pm_struct_ptr;
    PMGR_PART_INFO_STRUCT_PTR     part_info_ptr;
    uint32_t    location;
    uint32_t    part_start;
    uint32_t    part_end;
    int32_t     result;

    pm_struct_ptr = (PART_MGR_STRUCT_PTR)fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    part_info_ptr = (PMGR_PART_INFO_STRUCT_PTR)fd_ptr->DEV_DATA_PTR;

    /* If a partition is selected, also its start has to be defined */
    if (part_info_ptr->SLOT && part_info_ptr->START_SECTOR==0)
    {
        fd_ptr->ERROR = PMGR_INVALID_PARTITION;
        return IO_ERROR;
    }

    part_start = part_info_ptr->START_SECTOR;
    part_end = part_info_ptr->LENGTH ? part_start + part_info_ptr->LENGTH : 0;

    /* If not operating on a block device, multiply values by sector size */
    if (!(pm_struct_ptr->BLOCK_MODE))
    {
        part_start *= pm_struct_ptr->DEV_SECTOR_SIZE;
        part_end *= pm_struct_ptr->DEV_SECTOR_SIZE;
    }

    /* Calculate seek location for undelying device */
    location = fd_ptr->LOCATION + part_start;
    if (part_info_ptr->LENGTH)
    {
        /* Check whether lhe location points into the partition area */
        if (location >= part_end)
        {
            fd_ptr->ERROR = IO_ERROR_SEEK;
            return IO_ERROR;
        }
        /* Check whether the transfer would overflow the partition area and shorten it if necessary */
        if (location + num > part_end)
        {
            num = part_end - location;
        }
    }

    /* Lock to prevent concurrent access to underlying device */
    _lwsem_wait(&pm_struct_ptr->SEM);

    /* Perform seek and data transfer */
    result = fseek(pm_struct_ptr->DEV_FILE_PTR, location, IO_SEEK_SET);
    if (result == MQX_OK)
    {
        result = write( ((PART_MGR_STRUCT_PTR) fd_ptr->DEV_PTR->DRIVER_INIT_PTR)->DEV_FILE_PTR, data_ptr, num);
    }
    fd_ptr->ERROR = pm_struct_ptr->DEV_FILE_PTR->ERROR;

    /* Unlock */
    _lwsem_post(&pm_struct_ptr->SEM);

    return result;
}


/*FUNCTION*--------------------------------------------------------------
*
* Function Name  : _io_part_mgr_ioctl
* Returned Value : int32_t or error code
* Comments       : Executes ioctl commands
*
*END*--------------------------------------------------------------------*/

int32_t _io_part_mgr_ioctl
    (
    MQX_FILE_PTR   fd_ptr,      /* [IN] the stream to perform the operation on */
    uint32_t        cmd,         /* [IN] the ioctl command */
    uint32_t    *param_ptr    /* [IN] the ioctl parameters */
    )
{
    PART_MGR_STRUCT_PTR           pm_struct_ptr;
    PMGR_PART_INFO_STRUCT_PTR     part_info_ptr;
    uint32_t                       result = MQX_OK;

    pm_struct_ptr = (PART_MGR_STRUCT_PTR)fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    part_info_ptr = (PMGR_PART_INFO_STRUCT_PTR)fd_ptr->DEV_DATA_PTR;

    switch ( cmd )
    {

        case IO_IOCTL_GET_BLOCK_SIZE:
            *param_ptr = pm_struct_ptr->DEV_SECTOR_SIZE;
            break;

        case IO_IOCTL_GET_NUM_SECTORS:
            *param_ptr = part_info_ptr->LENGTH;
            break;

        case IO_IOCTL_VAL_PART:
            if (param_ptr == NULL)
            {
                /* Validate current partition */
                if (part_info_ptr->START_SECTOR==0 || part_info_ptr->LENGTH==0)
                    result = PMGR_INVALID_PARTITION;
                else
                    result = _pmgr_check_part_type(part_info_ptr->TYPE);
            }
            else if (part_info_ptr->SLOT)
            {
                /* Available only if operating on whole device */
                result = MQX_IO_OPERATION_NOT_AVAILABLE;
                break;
            }
            else
            {
                PMGR_PART_INFO_STRUCT part_info;
                _mem_zero(&part_info, sizeof(PMGR_PART_INFO_STRUCT));
                /* In case partition number is 0 the _pmgr_get_part_info function performs just MBR validation */
                part_info.SLOT = *param_ptr;
                _lwsem_wait(&pm_struct_ptr->SEM);
                result = _pmgr_get_part_info(pm_struct_ptr, &part_info);
                _lwsem_post(&pm_struct_ptr->SEM);
                if (!result && part_info.SLOT)
                {
                    result = _pmgr_check_part_type(part_info.TYPE);
                }
            }
            break;

        case IO_IOCTL_SEL_PART:
            if (*param_ptr && part_info_ptr->SLOT)
            {
                /*
                 * Available only if operating on whole device.
                 * This prevents selecting partition multiple times through a single handle,
                 * e.g. if the handle is "recycled" for mutliple MFS instances, which is wrong.
                 * To select a different partition, partition 0 (whole device) has to be selected first
                 */
                result = MQX_IO_OPERATION_NOT_AVAILABLE;
                break;
            }
            _mem_zero(part_info_ptr, sizeof(PMGR_PART_INFO_STRUCT));
            if (*param_ptr == 0)
            {
                /* Use size of device as length of the partition */
                part_info_ptr->LENGTH = pm_struct_ptr->DEV_NUM_SECTORS;
            }
            else
            {
                part_info_ptr->SLOT = *param_ptr;
                _lwsem_wait(&pm_struct_ptr->SEM);
                result = _pmgr_get_part_info(pm_struct_ptr, part_info_ptr);
                _lwsem_post(&pm_struct_ptr->SEM);
            }
            /* Reset location to the beginning of the partition/device */
            fd_ptr->LOCATION = 0;
            break;

        case IO_IOCTL_GET_PARTITION:
            if (((PMGR_PART_INFO_STRUCT_PTR)param_ptr)->SLOT == 0)
            {
                /* Get info about current partition */
                _mem_copy(part_info_ptr, param_ptr, sizeof(PMGR_PART_INFO_STRUCT));
                break;
            }
            if (part_info_ptr->SLOT)
            {
                /* Available only if operating on whole device */
                result = MQX_IO_OPERATION_NOT_AVAILABLE;
                break;
            }
            _lwsem_wait(&pm_struct_ptr->SEM);
            result = _pmgr_get_part_info(pm_struct_ptr, (PMGR_PART_INFO_STRUCT_PTR) param_ptr);
            _lwsem_post(&pm_struct_ptr->SEM);
            break;

        case IO_IOCTL_SET_PARTITION:
            if (part_info_ptr->SLOT)
            {
                /* Available only if operating on whole device */
                result = MQX_IO_OPERATION_NOT_AVAILABLE;
                break;
            }
            _lwsem_wait(&pm_struct_ptr->SEM);
            if (pm_struct_ptr->INSTANCES > 1)
            {
                /* Available if this is the only instance to avoid inconsistencies */
                result = IO_ERROR_DEVICE_BUSY;
            }
            else {
                result = _pmgr_set_part_info(pm_struct_ptr, (PMGR_PART_INFO_STRUCT_PTR) param_ptr);
            }
            _lwsem_post(&pm_struct_ptr->SEM);
            break;

        case IO_IOCTL_CLEAR_PARTITION:
            if (part_info_ptr->SLOT)
            {
                /* Available only if operating on whole device */
                result = MQX_IO_OPERATION_NOT_AVAILABLE;
                break;
            }
            _lwsem_wait(&pm_struct_ptr->SEM);
            if (pm_struct_ptr->INSTANCES > 1)
            {
                /* Available if this is the only instance to avoid inconsistencies */
                result = IO_ERROR_DEVICE_BUSY;
            }
            else {
                result = _pmgr_clear_part_info(pm_struct_ptr, *param_ptr);
            }
            _lwsem_post(&pm_struct_ptr->SEM);
            break;

        case IO_IOCTL_USE_PARTITION:
            /*
             * Force switching to area defined by submitted PMGR_PART_INFO_STRUCT,
             * without storing anything to the partition table on the device.
             * This may be useful to divide the device to areas defined elsewhere.
             */
            if (part_info_ptr->SLOT)
            {
                /* Available only if operating on whole device */
                result = MQX_IO_OPERATION_NOT_AVAILABLE;
                break;
            }
            _mem_copy(param_ptr, part_info_ptr, sizeof(PMGR_PART_INFO_STRUCT));
            /* Reset location to the beginning of the partition/device */
            fd_ptr->LOCATION = 0;

            break;

        default:
            /* Pass IOCTL command to lower layer */
            result = ioctl(pm_struct_ptr->DEV_FILE_PTR, cmd, param_ptr);
            break;
    }

    fd_ptr->ERROR = pm_struct_ptr->DEV_FILE_PTR->ERROR;

    return result;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  _pmgr_check_part_type
* Returned Value   :  int32_t error code
* Comments  :
*     Checks partition type, returns success if it matches one of the FAT types
*
*END*---------------------------------------------------------------------*/

int32_t _pmgr_check_part_type
    (
        uint32_t part_type
    )
{
    switch (part_type)
    {
        case PMGR_PARTITION_NOT_USED:
            return PMGR_INVALID_PARTITION;

        case PMGR_PARTITION_FAT_12_BIT:
        case PMGR_PARTITION_FAT_16_BIT:
        case PMGR_PARTITION_HUGE:
        case PMGR_PARTITION_HUGE_LBA:
        case PMGR_PARTITION_FAT32:
        case PMGR_PARTITION_FAT32_LBA:
            break;

        default:
            return PMGR_UNKNOWN_PARTITION;
    }
    return MQX_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  _pmgr_validate_mbr
* Returned Value   :  int32_t error code
* Comments  :
*     Validates MBR, checks signature and sanity of the partition table
*
*END*---------------------------------------------------------------------*/

int32_t _pmgr_validate_mbr
    (
    char *mbr_ptr
    )
{
    int i, j;
    PMGR_PART_ENTRY_STRUCT_PTR disk_entry;

    uint32_t part_start[PMGR_MAX_PARTITIONS];
    uint32_t part_length[PMGR_MAX_PARTITIONS];

    /* Check partition table signature bytes */
    if (((uint8_t *)mbr_ptr)[510]!=0x55 || ((uint8_t *)mbr_ptr)[511]!=0xAA)
        return PMGR_INVALID_PARTTABLE;

    disk_entry = (PMGR_PART_ENTRY_STRUCT_PTR)(((uint8_t *)mbr_ptr) + PMGR_PARTITION_TABLE_OFFSET);

    for (i=0; i<PMGR_MAX_PARTITIONS; i++)
    {
        part_start[i] = mqx_dtohl(disk_entry[i].START_SECTOR);
        part_length[i] = mqx_dtohl(disk_entry[i].LENGTH);

        /* If type is zero, the whole partition entry should be unused (zero) */
        if ((disk_entry[i].TYPE == 0) && (part_length[i] != 0 || part_start[i] != 0))
            return PMGR_INVALID_PARTTABLE;

        /* If type is non-zero, the partition start and length shall contain meaningfull values */
        if ((disk_entry[i].TYPE != 0) && (part_length[i] == 0 || part_start[i] == 0))
            return PMGR_INVALID_PARTTABLE;

        if (part_length[i] == 0)
            continue; /* No overlap check necessary, go for next record */

        /* Check for overlaps */
        for (j=0; j<i; j++)
        {
            if (part_length[j] == 0)
                continue; /* No overlap check necessary, go for next record */

            /*
             * Two partitions do NOT overlap if: start of one is behind end of the other OR vice versa.
             * The condition below is negation of this sentence.
             */
            if ((part_start[i] < part_start[j] + part_length[j]) && (part_start[i] + part_length[i] > part_start[j]))
                return PMGR_INVALID_PARTTABLE;
        }
    }

    return MQX_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  _pmgr_get_part_info
* Returned Value   :  int32_t error code
* Comments  :
*     Read the partition table entry from disk
*
*END*---------------------------------------------------------------------*/

int32_t _pmgr_get_part_info
    (
    PART_MGR_STRUCT_PTR        pm_struct_ptr,   /*[IN] the partition manager information */
    PMGR_PART_INFO_STRUCT_PTR  host_entry       /*[IN]/[OUT] the entry to read from disk */
    )
{
    int32_t                       error_code = MQX_OK;
    char                     *mbr_ptr;
    int32_t                       size;
    PMGR_PART_ENTRY_STRUCT_PTR   disk_entry;

    mbr_ptr =  MFS_mem_alloc_system_align(pm_struct_ptr->DEV_SECTOR_SIZE, pm_struct_ptr->ALIGNMENT);
    if ( mbr_ptr == NULL )
    {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type(mbr_ptr, MEM_TYPE_PART_MGR_SECTOR);

    size = pm_struct_ptr->BLOCK_MODE ? 1 : pm_struct_ptr->DEV_SECTOR_SIZE;

    fseek(pm_struct_ptr->DEV_FILE_PTR, 0, IO_SEEK_SET);
    if (read(pm_struct_ptr->DEV_FILE_PTR, mbr_ptr, size) != size)
    {
        error_code = pm_struct_ptr->DEV_FILE_PTR->ERROR;
    }
    else
    {
        error_code = _pmgr_validate_mbr(mbr_ptr);
    }

    if (!error_code && host_entry && host_entry->SLOT)
    {
        if (host_entry->SLOT > PMGR_MAX_PARTITIONS)
        {
            error_code = PMGR_INVALID_PARTITION;
        }
        else {
            disk_entry = (PMGR_PART_ENTRY_STRUCT_PTR)(mbr_ptr + PMGR_PARTITION_TABLE_OFFSET);
            _pmgr_disk_to_host(disk_entry + host_entry->SLOT - 1, host_entry);
            if (host_entry->TYPE == 0 || host_entry->START_SECTOR == 0 || host_entry->LENGTH == 0)
            {
                error_code = PMGR_INVALID_PARTITION;
            }
        }
    }

    _mem_free(mbr_ptr);
    return error_code;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  _pmgr_set_part_info
* Returned Value   :  int32_t error code
* Comments  :
*     Will overwrite the partition table entry with the new information
*     received (if opcode == 0). If the opcode is a partition number, this
*     function will clear that partition. Will return error code upon error.
*
*END*---------------------------------------------------------------------*/

int32_t _pmgr_set_part_info
    (
    PART_MGR_STRUCT_PTR        pm_struct_ptr,   /*[IN] the partitionn manager information */
    PMGR_PART_INFO_STRUCT_PTR  host_entry       /*[IN] the entry to write to disk */
    )
{
    int32_t                       error_code = MQX_OK;
    char                     *mbr_ptr;
    int32_t                       size;
    PMGR_PART_ENTRY_STRUCT_PTR   disk_entry;

    if (!host_entry || host_entry->SLOT==0 || host_entry->SLOT>PMGR_MAX_PARTITIONS)
    {
        return PMGR_INVALID_PARTITION;
    }

    mbr_ptr =  MFS_mem_alloc_system_align(pm_struct_ptr->DEV_SECTOR_SIZE, pm_struct_ptr->ALIGNMENT);
    if ( mbr_ptr == NULL )
    {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type(mbr_ptr, MEM_TYPE_PART_MGR_SECTOR);

    size = pm_struct_ptr->BLOCK_MODE ? 1 : pm_struct_ptr->DEV_SECTOR_SIZE;

    fseek(pm_struct_ptr->DEV_FILE_PTR, 0, IO_SEEK_SET);
    if (read(pm_struct_ptr->DEV_FILE_PTR, mbr_ptr, size) != size)
    {
        error_code = pm_struct_ptr->DEV_FILE_PTR->ERROR;
    }
    else
    {
        error_code = _pmgr_validate_mbr(mbr_ptr);
    }

    if (!error_code)
    {
        disk_entry = (PMGR_PART_ENTRY_STRUCT_PTR)(mbr_ptr + PMGR_PARTITION_TABLE_OFFSET);
        _pmgr_host_to_disk(host_entry, disk_entry + host_entry->SLOT - 1);
        error_code = _pmgr_validate_mbr(mbr_ptr); /* Check sanity of modified MBR */
        if (!error_code)
        {
            fseek(pm_struct_ptr->DEV_FILE_PTR, 0, IO_SEEK_SET);
            if (write(pm_struct_ptr->DEV_FILE_PTR, mbr_ptr, size) != size)
            {
                error_code = pm_struct_ptr->DEV_FILE_PTR->ERROR;
            }
        }
    }

    _mem_free(mbr_ptr);
    return error_code;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  _pmgr_clear_part_info
* Returned Value   :  int32_t error code
* Comments  :
*     Clears partition information for given partition
*
*END*---------------------------------------------------------------------*/

int32_t _pmgr_clear_part_info
    (
    PART_MGR_STRUCT_PTR        pm_struct_ptr,   /*[IN] the partitionn manager information */
    unsigned char                      part_num         /*[IN] the partition to clear */
    )
{
    int32_t                       error_code = MQX_OK;
    char                     *mbr_ptr;
    int32_t                       size;
    PMGR_PART_ENTRY_STRUCT_PTR   disk_entry;

    if (part_num>PMGR_MAX_PARTITIONS)
    {
        return PMGR_INVALID_PARTITION;
    }

    mbr_ptr =  MFS_mem_alloc_system_align(pm_struct_ptr->DEV_SECTOR_SIZE, pm_struct_ptr->ALIGNMENT);
    if ( mbr_ptr == NULL )
    {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type(mbr_ptr, MEM_TYPE_PART_MGR_SECTOR);

    size = pm_struct_ptr->BLOCK_MODE ? 1 : pm_struct_ptr->DEV_SECTOR_SIZE;

    fseek(pm_struct_ptr->DEV_FILE_PTR, 0, IO_SEEK_SET);
    if (read(pm_struct_ptr->DEV_FILE_PTR, mbr_ptr, size) != size)
    {
        error_code = pm_struct_ptr->DEV_FILE_PTR->ERROR;
    }
    else
    {
        error_code = _pmgr_validate_mbr(mbr_ptr);
    }

    if (error_code == PMGR_INVALID_PARTITION && part_num == 0)
    {
        /* Create blank MBR */
        _mem_zero(mbr_ptr, pm_struct_ptr->DEV_SECTOR_SIZE);
        mbr_ptr[510] = 0x55;
        mbr_ptr[511] = 0xAA;
        error_code = 0;
    }
    else if (!error_code)
    {
        disk_entry = (PMGR_PART_ENTRY_STRUCT_PTR)(mbr_ptr + PMGR_PARTITION_TABLE_OFFSET);
        if (!part_num)
        {
            _mem_zero(disk_entry, PMGR_MAX_PARTITIONS*sizeof(PMGR_PART_ENTRY_STRUCT));
        }
        else
        {
            _mem_zero(disk_entry + part_num - 1, sizeof(PMGR_PART_ENTRY_STRUCT));
        }
    }

    if (!error_code)
    {
        fseek(pm_struct_ptr->DEV_FILE_PTR, 0, IO_SEEK_SET);
        if ( write(pm_struct_ptr->DEV_FILE_PTR, mbr_ptr, size) != size )
        {
            error_code = pm_struct_ptr->DEV_FILE_PTR->ERROR;
        }
    }

    _mem_free(mbr_ptr);
    return error_code;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  _pmgr_disk_to_host
* Returned Value   :  void
* Comments  :
*     Will copy a partition table entry from its disk format to a host format
*
*END*---------------------------------------------------------------------*/


void _pmgr_disk_to_host
    (
    PMGR_PART_ENTRY_STRUCT_PTR   disk_entry,  /*[IN] ptr to a partition table entry in disk format */
    PMGR_PART_INFO_STRUCT_PTR    part_entry   /*[OUT] ptr to a partition table entry in a more usable format */
    )
{
    part_entry->TYPE = disk_entry->TYPE;
    part_entry->ACTIVE_FLAG = disk_entry->ACTIVE_FLAG;
    part_entry->START_SECTOR = mqx_dtohl(disk_entry->START_SECTOR);
    part_entry->LENGTH = mqx_dtohl(disk_entry->LENGTH);
    part_entry->HEADS = 0;
    part_entry->SECTORS = 0;
    part_entry->CYLINDERS = 0;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  _pmgr_host_to_disk
* Returned Value   :  void
* Comments  :
*     Will copy a partition table entry from its host format to the disk format
*
*END*---------------------------------------------------------------------*/

void _pmgr_host_to_disk
    (
    PMGR_PART_INFO_STRUCT_PTR              part_entry,  /*[IN] ptr to a partition table entry in a usable format */
    PMGR_PART_ENTRY_STRUCT_PTR             disk_entry   /*[OUT] ptr to a partition table entry in disk format */
    )
{
    uint32_t     temp,cyl,head,sec,hds_cyl,sct_trk,lba;

    disk_entry->ACTIVE_FLAG = 0;
    disk_entry->TYPE = part_entry->TYPE;
    mqx_htodl(disk_entry->START_SECTOR, part_entry->START_SECTOR);
    mqx_htodl(disk_entry->LENGTH, part_entry->LENGTH);

    /* Check if CHS is present and valid */
    if ( !part_entry->CYLINDERS || !part_entry->HEADS || !part_entry->SECTORS || part_entry->CYLINDERS>1024 || part_entry->SECTORS>63 )
    {
        disk_entry->START_HEAD = 0;
        disk_entry->START_SECT_CYL = 0;
        disk_entry->START_CYLINDER = 0;
        disk_entry->ENDING_HEAD = 0;
        disk_entry->ENDING_SECT_CYL = 0;
        disk_entry->ENDING_CYLINDER = 0;
    }
    else
    {
        /* setup info used for calculations */
        sct_trk = part_entry->SECTORS;
        hds_cyl = part_entry->HEADS;

        /* Calculate starting CHS */
        lba = part_entry->START_SECTOR;
        cyl = lba / (hds_cyl * sct_trk);
        temp = lba % (hds_cyl * sct_trk);
        head = temp / sct_trk;
        sec = temp % sct_trk + 1;

        if (cyl>1023)
        {
            /* Indicate invalid CHS entry */
            disk_entry->START_HEAD = 254;
            disk_entry->START_SECT_CYL = 0xFF;
            disk_entry->START_CYLINDER = 0xFF;
        }
        else
        {
            disk_entry->START_HEAD = (unsigned char) head;
            disk_entry->START_SECT_CYL = (unsigned char) ((sec & 0x3f) + ((cyl & 0x0300) >> 2));
            disk_entry->START_CYLINDER = (unsigned char) (cyl & 0x0FF);
        }

        /* Calculate the ending CHS */
        lba = part_entry->LENGTH + part_entry->START_SECTOR;
        cyl = lba / (hds_cyl * sct_trk);
        temp = lba % (hds_cyl * sct_trk);
        head = temp / sct_trk;
        sec = temp % sct_trk + 1;

        if (cyl>1023)
        {
            /* Indicate invalid CHS entry */
            disk_entry->ENDING_HEAD = 254;
            disk_entry->ENDING_SECT_CYL = 0xFF;
            disk_entry->ENDING_CYLINDER = 0xFF;
        }
        else
        {
            disk_entry->ENDING_HEAD = (unsigned char) head;
            disk_entry->ENDING_SECT_CYL = (unsigned char) ((sec & 0x3f) + ((cyl & 0x0300) >> 2));
            disk_entry->ENDING_CYLINDER = (unsigned char) (cyl & 0x0FF);
        }
    }
}

#endif //MQX_USE_IO_OLD
