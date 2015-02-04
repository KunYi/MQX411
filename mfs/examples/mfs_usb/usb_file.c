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
*   This file contains the initialization and menu source code for
*   the USB mass storage MFS test example program using USB mass storage
*   link driver.
*
*
*END************************************************************************/

#include "mfs_usb.h"

#include "usb_file.h"


static char *dev_name[4] = {"USB1:", "USB2:", "USB3:", "USB4:"};
static char *pm_name[4] = {"PM1:", "PM2:", "PM3:", "PM4:"};


/* Retrieves mask indicating valid partitions found on the device. */
int get_valid_partitions(MQX_FILE_PTR pm_fd_ptr, uint32_t *valid_partitions);


MQX_FILE_PTR mfs_mount(MQX_FILE_PTR block_device, char *fs_name)
{
    MQX_FILE_PTR fs_fd_ptr;
    int error_code;

    /* Install MFS */
    error_code = _io_mfs_install(block_device, fs_name, 0);
    if (error_code != MFS_NO_ERROR)
    {
        printf("Error initializing MFS: %s\n", MFS_Error_text((uint32_t)error_code));
        return NULL;
    }

    /* Open filesytem */
    fs_fd_ptr = fopen(fs_name, NULL);

    /* Handle error states */
    error_code = ferror(fs_fd_ptr);
    if (error_code == MFS_NOT_A_DOS_DISK)
    {
        printf("NOT A DOS DISK! You must format to continue.\n");
    }
    else if (error_code != MFS_NO_ERROR)
    {
        printf("Error opening filesystem: %s\n", MFS_Error_text((uint32_t)error_code));

        error_code = _io_dev_uninstall(fs_name);
        if (error_code != MFS_NO_ERROR)
        {
            printf("Error while uninstalling filesystem.\n");
        }
        return NULL;
    }

    return fs_fd_ptr;
}


int mfs_unmount(MQX_FILE_PTR fs_fd_ptr, char *fs_name)
{
    int error_code = MQX_OK;

    if (fs_name == NULL) {
        fs_name = "";
    }

    if (fs_fd_ptr) {
        error_code = fclose(fs_fd_ptr);
        if (error_code == MFS_SHARING_VIOLATION) {
            /*
             * Filesystem could not be closed. There are probably still some files opened preventing it.
             * Wait for a while (give other tasks a chance to finish their job) and try again.
             */
            printf("\nWaiting for tasks to close all files on %s\n", fs_name);
        }
        while (error_code == MFS_SHARING_VIOLATION)
        {
            _time_delay(200);
            putchar('.');
            error_code = fclose(fs_fd_ptr);
        }
        if (error_code != MQX_OK)
        {
            printf("Error while closing filesystem %s\n", fs_name);
        }
    }

    if (*fs_name) {
        /* Uninstall MFS */
        error_code = _io_dev_uninstall(fs_name);
        if (error_code != MFS_NO_ERROR)
        {
            printf("Error while uninstalling filesystem %s\n", fs_name);
        }
    }

    return error_code;
}


MQX_FILE_PTR msd_assign_drive_letter(MQX_FILE_PTR block_device, char *fs_name, uint32_t *mountp)
{
    MQX_FILE_PTR fs_fd_ptr;
    uint32_t mask;
    char drive_letter;

    /* Find free mountpint (drive letter) */
    mask = 1;
    for (drive_letter = 'a'; drive_letter <= 'z'; drive_letter++) {
        if ((*mountp & mask) == 0)
            break;
        mask <<= 1;
    }

    if (drive_letter > 'z') {
        printf("No drive letter available\n");
        return NULL;
    }

    fs_name[0] = drive_letter;
    fs_name[1] = ':';
    fs_name[2] = '\0';

    fs_fd_ptr = mfs_mount(block_device, fs_name);
    if (fs_fd_ptr != NULL) {
        *mountp |= mask;
    }
    else {
        *fs_name = '\0';
    }

    return fs_fd_ptr;
}


int usb_msd_install
    (
        void        *usb_handle,
        uint32_t    *mountp
    )
{
    unsigned char *dev_info;
    uint32_t error_code;
    uint32_t part = 0;
    uint32_t valid_parts;
    uint32_t dev_id = -1;
    uint32_t pm_id = -1;

    MSD_DEV_DATA_PTR msd_dev_data = (MSD_DEV_DATA_PTR)usb_handle;

    _mem_zero(((uint8_t *)msd_dev_data)+sizeof(CLASS_CALL_STRUCT), sizeof(MSD_DEV_DATA_STRUCT)-sizeof(CLASS_CALL_STRUCT));

    /* Install USB device */
    do {
        dev_id++;
        error_code = _io_usb_mfs_install(dev_name[dev_id], 0, &msd_dev_data->ccs);
    } while (error_code == IO_DEVICE_EXISTS && dev_id < ELEMENTS_OF(dev_name) - 1);

    if (error_code != MQX_OK)
    {
        printf("Error while installing USB device (0x%X)\n", error_code);
        return error_code;
    }

    _time_delay(500);

    /* Open the USB mass storage device */
    msd_dev_data->DEV_NAME = dev_name[dev_id];
    msd_dev_data->DEV_FD_PTR = fopen(msd_dev_data->DEV_NAME, 0);

    if (msd_dev_data->DEV_FD_PTR == NULL)
    {
        printf("Unable to open USB device\n");
        return _task_get_error();
    }

    /* Get the vendor information and display it */
    printf("\n************************************************************************\n");
    _io_ioctl(msd_dev_data->DEV_FD_PTR, IO_IOCTL_GET_VENDOR_INFO, &dev_info);
    printf("Vendor Information:     %-1.8s Mass Storage Device\n",dev_info);
    _io_ioctl(msd_dev_data->DEV_FD_PTR, IO_IOCTL_GET_PRODUCT_ID, &dev_info);
    printf("Product Identification: %-1.16s\n", dev_info);
    _io_ioctl(msd_dev_data->DEV_FD_PTR, IO_IOCTL_GET_PRODUCT_REV, &dev_info);
    printf("Product Revision Level: %-1.4s\n", dev_info);
    printf("************************************************************************\n");

    _io_ioctl(msd_dev_data->DEV_FD_PTR, IO_IOCTL_SET_BLOCK_MODE, NULL);

    /* Try to install the partition manager */
    do {
        pm_id++;
        error_code = _io_part_mgr_install(msd_dev_data->DEV_FD_PTR, pm_name[pm_id], 0);
    } while (error_code == IO_DEVICE_EXISTS && pm_id < ELEMENTS_OF(pm_name) - 1);

    if (error_code != MFS_NO_ERROR)
    {
        printf("Error while installing partition manager: %s\n", MFS_Error_text((uint32_t)error_code));
        return error_code;
    }

    /* Open partition manager */
    msd_dev_data->PM_NAME = pm_name[pm_id];
    msd_dev_data->PM_FD_PTR[0] = fopen(msd_dev_data->PM_NAME, NULL);

    if (msd_dev_data->PM_FD_PTR[0] == NULL)
    {
        error_code = ferror(msd_dev_data->PM_FD_PTR[0]);
        printf("Error while opening partition manager: %s\n", MFS_Error_text((uint32_t)error_code));
        return error_code;
    }

    /* Validate MBR */
    error_code = get_valid_partitions(msd_dev_data->PM_FD_PTR[0], &valid_parts);

    if ((error_code != MQX_OK) || (valid_parts == 0))
    {
        /* Install MFS on whole storage device */
        printf("Installing MFS over USB device...\n");

        /* Close partition manager */
        fclose(msd_dev_data->PM_FD_PTR[0]);
        msd_dev_data->PM_FD_PTR[0] = NULL;

        /* Uninstall partition manager */
        error_code = _io_dev_uninstall(msd_dev_data->PM_NAME);
        if (error_code != MFS_NO_ERROR)
        {
            printf("Error while uninstalling partition manager.\n");
        }
        msd_dev_data->PM_NAME = NULL;

        /* Assign drive letter to whole device */
        msd_dev_data->FS_FD_PTR[0] = msd_assign_drive_letter(msd_dev_data->DEV_FD_PTR, msd_dev_data->FS_NAME[0], mountp);
        if (msd_dev_data->FS_FD_PTR[0] != NULL) {
            printf("Storage device installed as %s\n", msd_dev_data->FS_NAME[0]);
        }
    }

    else {
        /* Install MFS over partitions */
        printf("Installing MFS over partitions...\n");

        for (part=1; part<=4 ;part++) {

            /* Open partition manager */
            msd_dev_data->PM_FD_PTR[part] = fopen(msd_dev_data->PM_NAME, NULL);

            if (msd_dev_data->PM_FD_PTR[part] == NULL)
            {
                error_code = ferror(msd_dev_data->PM_FD_PTR[part]);
                printf("Error while opening partition manager: %s\n", MFS_Error_text((uint32_t)error_code));
                return error_code; /* It probably does not make a sense to try other partitions at this point */
            }

            /* Select partition */
            error_code = _io_ioctl(msd_dev_data->PM_FD_PTR[part], IO_IOCTL_SEL_PART, &part);

            if (error_code != MFS_NO_ERROR)
            {
                /* This particular partition cannot be selected but this is not a fatal error */
                fclose(msd_dev_data->PM_FD_PTR[part]);
                msd_dev_data->PM_FD_PTR[part] = NULL;
                continue;
            }

            /* Validate partition */
            error_code = _io_ioctl(msd_dev_data->PM_FD_PTR[part], IO_IOCTL_VAL_PART, NULL);
            if (error_code != MFS_NO_ERROR)
            {
                printf("Error while validating partition: %s\n", MFS_Error_text((uint32_t)error_code));
                printf("Not installing MFS.\n");
                continue;
            }

            /* Assign drive letter for the partition */
            msd_dev_data->FS_FD_PTR[part] = msd_assign_drive_letter(msd_dev_data->PM_FD_PTR[part], msd_dev_data->FS_NAME[part], mountp);
            if (msd_dev_data->FS_FD_PTR[part] != NULL) {
                printf("Partition %d installed as %s\n", part, msd_dev_data->FS_NAME[part]);
            }
        }
    }

    return MQX_OK;
}


int usb_msd_uninstall
    (
        void        *usb_handle,
        uint32_t    *mountp
    )
{
    MSD_DEV_DATA_PTR msd_dev_data = (MSD_DEV_DATA_PTR) usb_handle;

    uint32_t part;
    int drive_letter_index;


    if (msd_dev_data == NULL) {
        return MQX_INVALID_PARAMETER;
    }

    for (part=0; part<=4 ; part++) {
        /* Check for invalid parameters is done internally in mfs_unmount */
        mfs_unmount(msd_dev_data->FS_FD_PTR[part], msd_dev_data->FS_NAME[part]);

        /* Mark drive letter as free */
        if (msd_dev_data->FS_NAME[part][0]) {
            drive_letter_index = msd_dev_data->FS_NAME[part][0] - 'a';
            *mountp &= ~(1 << drive_letter_index);
        }

        /* Close partition manager, if any */
        if ((msd_dev_data->PM_FD_PTR[part] != NULL) && (MQX_OK != fclose(msd_dev_data->PM_FD_PTR[part])))
        {
            printf("Error while closing partition manager.\n");
        }
    }

    /* All partition manager instances are closed for sure at this point, uninstall partition manager (if any) */
    if (msd_dev_data->PM_NAME && *(msd_dev_data->PM_NAME) && (MQX_OK != _io_dev_uninstall(msd_dev_data->PM_NAME))) {
        printf("Error while uninstalling partition manager.\n");
    }

    /* Close USB device */
    if ((msd_dev_data->DEV_FD_PTR != NULL) && (MQX_OK != fclose(msd_dev_data->DEV_FD_PTR)))
    {
        printf("Unable to close USB device.\n");
    }

    /* Uninstall USB device */
    if (msd_dev_data->DEV_NAME && *(msd_dev_data->DEV_NAME) && (MQX_OK != _io_dev_uninstall(msd_dev_data->DEV_NAME)))
    {
        printf("Error while uninstalling USB device.\n");
    }

    printf("USB device removed.\n");

    return MQX_OK;
}


int get_valid_partitions(
        MQX_FILE_PTR pm_fd_ptr,
        uint32_t *valid_partitions
    )
{
    int error_code;
    uint32_t part;
    uint32_t nullpart = 0;

    if (NULL == valid_partitions) {
        return MQX_INVALID_PARAMETER;
    }

    *valid_partitions = 0;

    error_code = _io_ioctl(pm_fd_ptr, IO_IOCTL_SEL_PART, &nullpart);
    if (error_code != MQX_OK) {
        return error_code;
    }

    error_code = _io_ioctl(pm_fd_ptr, IO_IOCTL_VAL_PART, &nullpart);
    if (error_code != MQX_OK) {
        return error_code;
    }

    /* Select partition */
    for (part=1; part<=4; part++) {
        error_code = _io_ioctl(pm_fd_ptr, IO_IOCTL_SEL_PART, &part);
        if (error_code == MFS_NO_ERROR) {
            *valid_partitions |= (1 << part);
        }

        error_code = _io_ioctl(pm_fd_ptr, IO_IOCTL_SEL_PART, &nullpart);
        if (error_code != MQX_OK) {
            break;
        }
    }

    return error_code;
}

/* EOF */
