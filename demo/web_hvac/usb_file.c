/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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
*   This file contains the initialization and menu source code for
*   the USB mass storage MFS test example program using USB mass storage
*   link driver.
*
*
*END************************************************************************/

#include "hvac.h"

#include "usb_file.h"


#if DEMOCFG_ENABLE_USB_FILESYSTEM

void *usb_filesystem_install( 
    void       *usb_handle,
    char    *block_device_name,
    char    *partition_manager_name,
    char    *file_system_name )
{
    uint32_t                    partition_number;
    unsigned char                  *dev_info;
    int32_t                     error_code;
    USB_FILESYSTEM_STRUCT_PTR  usb_fs_ptr;


    usb_fs_ptr = _mem_alloc_system_zero(sizeof(USB_FILESYSTEM_STRUCT));
    if (usb_fs_ptr==NULL)
    {
        return NULL;
    }

    /* Install USB device */
    error_code = _io_usb_mfs_install(block_device_name, 0, (void *)usb_handle);
    if (error_code != MQX_OK)
    {
        printf("Error while installing USB device (0x%X)\n", error_code);
        return NULL;
    }
    usb_fs_ptr->DEV_NAME = block_device_name;

    /* Open the USB mass storage  device */
    _time_delay(500);
    usb_fs_ptr->DEV_FD_PTR = fopen(block_device_name, 0);

    if (usb_fs_ptr->DEV_FD_PTR == NULL)
    {
        printf("Unable to open USB device\n");
        usb_filesystem_uninstall(usb_fs_ptr);
        return NULL;
    } 

    _io_ioctl(usb_fs_ptr->DEV_FD_PTR, IO_IOCTL_SET_BLOCK_MODE, NULL);

    /* Get the vendor information and display it */
    printf("\n************************************************************************\n");
    _io_ioctl(usb_fs_ptr->DEV_FD_PTR, IO_IOCTL_GET_VENDOR_INFO, &dev_info);
    printf("Vendor Information:     %-1.8s Mass Storage Device\n",dev_info);
    _io_ioctl(usb_fs_ptr->DEV_FD_PTR, IO_IOCTL_GET_PRODUCT_ID, &dev_info);
    printf("Product Identification: %-1.16s\n",dev_info);
    _io_ioctl(usb_fs_ptr->DEV_FD_PTR, IO_IOCTL_GET_PRODUCT_REV, &dev_info);
    printf("Product Revision Level: %-1.4s\n",dev_info);
    printf("************************************************************************\n");

    /* Try to install the partition manager */
    error_code = _io_part_mgr_install(usb_fs_ptr->DEV_FD_PTR, partition_manager_name, 0);
    if (error_code != MFS_NO_ERROR)
    {
        printf("Error while initializing partition manager: %s\n", MFS_Error_text((uint32_t)error_code));
        usb_filesystem_uninstall(usb_fs_ptr);
        return NULL;
    } 
    usb_fs_ptr->PM_NAME = partition_manager_name;

    /* Open partition manager */
    usb_fs_ptr->PM_FD_PTR = fopen(partition_manager_name, NULL);
    if (usb_fs_ptr->PM_FD_PTR == NULL)
    {
        error_code = ferror(usb_fs_ptr->PM_FD_PTR);
        printf("Error while opening partition manager: %s\n", MFS_Error_text((uint32_t)error_code));
        usb_filesystem_uninstall(usb_fs_ptr);
        return NULL;
    } 

    /* Select partition */
    partition_number = 1;
    error_code = _io_ioctl(usb_fs_ptr->PM_FD_PTR, IO_IOCTL_SEL_PART, &partition_number);

    if (error_code == MFS_NO_ERROR)
    {
        printf("Installing MFS over partition...\n");

        /* Validate partition */
        error_code = _io_ioctl(usb_fs_ptr->PM_FD_PTR, IO_IOCTL_VAL_PART, NULL);
        if (error_code != MFS_NO_ERROR)
        {
            printf("Error while validating partition: %s\n", MFS_Error_text((uint32_t)error_code));
            printf("Not installing MFS.\n");
            usb_filesystem_uninstall(usb_fs_ptr);
            return NULL;
        }

        /* Install MFS over partition */
        error_code = _io_mfs_install(usb_fs_ptr->PM_FD_PTR, file_system_name, 0);
        if (error_code != MFS_NO_ERROR)
        {
            printf("Error initializing MFS over partition: %s\n", MFS_Error_text((uint32_t)error_code));
        }
    }
    else {

        printf("Installing MFS over USB device...\n");

        /* Install MFS over USB device driver */
        error_code = _io_mfs_install(usb_fs_ptr->DEV_FD_PTR, file_system_name, 0);
        if (error_code != MFS_NO_ERROR)
        {
            printf("Error initializing MFS: %s\n", MFS_Error_text((uint32_t)error_code));
        }
    }

    /* Open file system */
    if (error_code == MFS_NO_ERROR)
    {
        usb_fs_ptr->FS_NAME = file_system_name;
        usb_fs_ptr->FS_FD_PTR = fopen(file_system_name, NULL);
        error_code = ferror(usb_fs_ptr->FS_FD_PTR);
        if (error_code == MFS_NOT_A_DOS_DISK)
        {
            printf("NOT A DOS DISK! You must format to continue.\n");
        }
        else if (error_code != MFS_NO_ERROR)
        {
            printf("Error opening filesystem: %s\n", MFS_Error_text((uint32_t)error_code));
            usb_filesystem_uninstall(usb_fs_ptr);
            return NULL;
        }

        printf("USB device installed to %s\n", file_system_name);  
    }
    else {
        
        usb_filesystem_uninstall(usb_fs_ptr);
        return NULL;
    }

    return (void *)usb_fs_ptr;
}

MQX_FILE_PTR usb_filesystem_handle( USB_FILESYSTEM_STRUCT_PTR  usb_fs_ptr)
{
    return usb_fs_ptr->FS_FD_PTR ;
}

void usb_filesystem_uninstall( USB_FILESYSTEM_STRUCT_PTR  usb_fs_ptr)
{
    int32_t  error_code;

    if (usb_fs_ptr == NULL) {
        return;
    }
    _int_disable();

    /* Close the filesystem */
    if ((usb_fs_ptr->FS_FD_PTR != NULL) && (MQX_OK != fclose(usb_fs_ptr->FS_FD_PTR)))
    {
        printf("Error while closing filesystem.\n");
    }

    /* Uninstall MFS */
    error_code = _io_dev_uninstall(usb_fs_ptr->FS_NAME);
    if (error_code != MFS_NO_ERROR)
    {
        printf("Error while uninstalling filesystem.\n");
    }

    /* Close partition */
    if ((usb_fs_ptr->PM_FD_PTR != NULL) && (MQX_OK != fclose(usb_fs_ptr->PM_FD_PTR)))
    {
        printf("Error while closing partition.\n");
    }

    /* Uninstall partition manager */
    error_code = _io_dev_uninstall(usb_fs_ptr->PM_NAME);
    if (error_code != MFS_NO_ERROR)
    {
        printf("Error while uninstalling partition manager.\n");
    }
    
    /* Close USB device */
    if ((usb_fs_ptr->DEV_FD_PTR != NULL) && (MQX_OK != fclose(usb_fs_ptr->DEV_FD_PTR)))
    {
        printf("Unable to close USB device.\n");
    }

    _int_enable();

    /* Uninstall USB device */
    error_code = _io_dev_uninstall(usb_fs_ptr->DEV_NAME);
    if (error_code != MQX_OK)
    {
        printf("Error while uninstalling USB device.\n");
    }

    _mem_free(usb_fs_ptr);

    printf("USB device removed.\n");
}


#endif


/* EOF */
