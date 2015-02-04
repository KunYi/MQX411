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
*   This file contains the functions that are used to initialize MFS
*   It also contains the MFS driver functions.
*
*
*END************************************************************************/

#include <mqx.h>
#include "errno.h" /* Workaround for uv4 to set including */
#if MQX_USE_IO_OLD
#include <ioctl.h>
#include <fio.h>
#include <io_prv.h>
#else
#include <nio.h>
#include <fcntl.h>
#include <nio/ioctl.h>
#endif
#include "mfs.h"
#include "mfs_prv.h"
#include "part_mgr.h"

_mem_pool_id _MFS_pool_id;
uint32_t _MFS_handle_pool_init = MFSCFG_HANDLE_INITIAL;
uint32_t _MFS_handle_pool_grow = MFSCFG_HANDLE_GROW;
uint32_t _MFS_handle_pool_max  = MFSCFG_HANDLE_MAX;

static int32_t MFS_Uninstall(MFS_DRIVE_STRUCT_PTR drive_ptr);

struct init_t {
    uint32_t               part_num;
#if MQX_USE_IO_OLD
    MQX_FILE_PTR dev_fd;   /*[IN] the device on which to install MFS */
#else
    int          dev_fd;   /*[IN] the device on which to install MFS */
#endif
};

#if MQX_USE_IO_OLD
static _mqx_int _io_mfs_deinit(struct io_device_struct *dev_context);
static _mqx_int _io_mfs_open(MQX_FILE_PTR fd_ptr,char *, char *flags_str);
static _mqx_int _io_mfs_close(MQX_FILE_PTR fd_ptr);
static _mqx_int _io_mfs_read(MQX_FILE_PTR file_ptr,char *data_ptr,int32_t num);
static _mqx_int _io_mfs_write(MQX_FILE_PTR file_ptr,char *data_ptr,int32_t num);
static _mqx_int _io_mfs_ioctl(MQX_FILE_PTR file_ptr,uint32_t cmd,uint32_t *param_ptr);
#else
static int _io_mfs_deinit(void *dev_context);
static int _io_mfs_open(void *dev_context, const char *open_name_ptr, int flags, void **fp_context);
static int _io_mfs_close(void *dev_context, void *fp_context);
static int _io_mfs_read(void *dev_context, void *fp_context, void *buf, size_t nbytes);
static int _io_mfs_write(void *dev_context, void *fp_context, const void *data_ptr, size_t num);
static off_t _io_mfs_lseek(void *dev_context, void *fp_context, off_t offset, int whence);
static int _io_mfs_ioctl(void *dev_context, void *fp_context, unsigned long int request, va_list ap);
#endif
static int _io_mfs_init(void *init_data, void **dev_context);

#if !MQX_USE_IO_OLD
static const NIO_DEV_FN_STRUCT _nio_mfs = {
    .OPEN = _io_mfs_open,
    .READ = _io_mfs_read,
    .WRITE = _io_mfs_write,
    .LSEEK = _io_mfs_lseek,
    .IOCTL = _io_mfs_ioctl,
    .CLOSE = _io_mfs_close,
    .INIT = _io_mfs_init,
    .DEINIT = _io_mfs_deinit,
};
#endif


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_mfs_install
* Returned Value   : error code
* Comments  :   Initialize the MSDOS File System.
*
*END*---------------------------------------------------------------------*/

uint32_t _io_mfs_install
    (
#if MQX_USE_IO_OLD
    MQX_FILE_PTR dev_fd,     /*[IN] the device on which to install MFS */
#else
    int      dev_fd,         /*[IN] the device on which to install MFS */
#endif
    char     *identifier,    /*[IN] the name that should be given to mfs (ex: "C:", "MFS1:", etc..) */
    uint32_t      partition_num  /*[IN] the partition number to install MFS on. 0 for no partitions */
    )
{
    uint32_t                     error_code;
    void                         *handle;

    struct init_t mfs_init = {
        .dev_fd = dev_fd,
        .part_num = partition_num,
    };

#if MQX_USE_IO_OLD
    error_code = _io_mfs_init(&mfs_init, (void **)&handle);
    if ( error_code != MFS_NO_ERROR )
    {
        return error_code;
    }
    error_code = _io_dev_install_ext( identifier,
        _io_mfs_open,
        _io_mfs_close,
        _io_mfs_read,
        _io_mfs_write,
        (_mqx_int(_CODE_PTR_)(MQX_FILE_PTR,_mqx_uint,void *))_io_mfs_ioctl,
        _io_mfs_deinit,
        (void *)handle);
#else
    if (NULL == _nio_dev_install( identifier,
        &_nio_mfs,
        (void *)&mfs_init))
    {
        error_code = MFS_INSUFFICIENT_MEMORY;
    }
    else
    {
        error_code = MFS_NO_ERROR;
    }
#endif

    return error_code;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_mfs_init
* Returned Value   : error code
* Comments  :   Initialize the MSDOS File System.
*
*END*---------------------------------------------------------------------*/

static int _io_mfs_init
    (
        void *init_data,

        void **dev_context
    )
{
    MFS_DRIVE_STRUCT_PTR         drive_ptr;
    struct init_t                *mfs_init = (struct init_t *)init_data;

    drive_ptr = MFS_mem_alloc_system_zero( sizeof(MFS_DRIVE_STRUCT) );
    if ( drive_ptr == NULL )
    {
        return( MFS_INSUFFICIENT_MEMORY );
    }
    _mem_set_type(drive_ptr, MEM_TYPE_MFS_DRIVE_STRUCT);

    /*
    ** create a light weight semaphore
    */
    _lwsem_create(&drive_ptr->SEM,1);

    drive_ptr->DEV_FILE_PTR = mfs_init->dev_fd;
    drive_ptr->DRV_NUM = mfs_init->part_num;
    drive_ptr->HANDLE_PARTITION = _partition_create(sizeof(MFS_HANDLE), _MFS_handle_pool_init, _MFS_handle_pool_grow, _MFS_handle_pool_max );

    // Inform Kernel that MFS is installed
    _mqx_set_io_component_handle(IO_MFS_COMPONENT,(void *)MFS_VERSION);

    *dev_context = (void *) drive_ptr;

    return 0;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_mfs_uninstall
* Returned Value   : error code
* Comments  :   Initialize the MSDOS File System.
*
*END*---------------------------------------------------------------------*/

int _io_mfs_uninstall
    (
    char     *identifier    /*[IN] the name that should be given to mfs (ex: "C:", "MFS1:", etc..) */
    )
{
    uint32_t retval;

#if MQX_USE_IO_OLD
    retval = (int)_io_dev_uninstall(identifier);
#else
    if (-1 == _nio_dev_uninstall(identifier))
    {
        retval = MFS_ACCESS_DENIED;
    }
    else
    {
        retval = MFS_NO_ERROR;
    }
#endif

    return retval;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Uninstall
* Returned Value   : uint32_t error code
* Comments  :   Uninstalls the MSDOS File System and frees all memory allocated
*               to it.
*
*END*---------------------------------------------------------------------*/

static int32_t MFS_Uninstall
    (
        MFS_DRIVE_STRUCT_PTR   drive_ptr
    )
{
    int32_t                error;

    if ( !drive_ptr->MFS_FILE_PTR )
    {
#if MQX_USE_IO_OLD
        error = IO_OK;
#else
        error = 0;
#endif
        _lwsem_destroy(&drive_ptr->SEM);
        _mfs_partition_destroy(drive_ptr->HANDLE_PARTITION);
        _mem_free(drive_ptr);
    }
    else
    {
        error = MFS_SHARING_VIOLATION;
    }

    return error;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_mfs_deinit
* Returned Value   : uint32_t error code
* Comments  :   Uninstalls the MSDOS File System and frees all memory allocated
*               to it.
*
*END*---------------------------------------------------------------------*/
#if MQX_USE_IO_OLD
static _mqx_int _io_mfs_deinit
    (

        struct io_device_struct *dev_context  /*[IN] The identifier of the device */
    )
#else
static int _io_mfs_deinit
    (
        void *dev_context  /*[IN] The identifier of the device */
    )
#endif
{
#if MQX_USE_IO_OLD
    MFS_DRIVE_STRUCT_PTR drive_ptr = (MFS_DRIVE_STRUCT_PTR)dev_context->DRIVER_INIT_PTR;
#else
    MFS_DRIVE_STRUCT_PTR drive_ptr = (MFS_DRIVE_STRUCT_PTR)dev_context;
#endif
    return MFS_Uninstall(drive_ptr);
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_mfs_open
* Returned Value   : MQX_OK or an error
* Comments         : Opens and initializes MFS driver.
*
*END*----------------------------------------------------------------------*/
#if MQX_USE_IO_OLD
static _mqx_int _io_mfs_open
    (
    MQX_FILE_PTR         fd_ptr,        /* [IN] the file handle for the device being opened */
    char                 *open_name_ptr, /* [IN] the remaining portion of the name of the device */
    char                 *flags_str      /* [IN] the flags to specify file type:
                                            ** w  write
                                            ** r  read
                                            ** a append
                                            ** x for temp file
                                            ** n for new file
                                            ** etc...
                                            */
    )
#else
static int _io_mfs_open
    (
    void *dev_context,

    const char *open_name_ptr,

    int fflags,

    void **fp_context
    )
#endif
{
    MFS_DRIVE_STRUCT_PTR        drive_ptr;
    unsigned char               flags = 0;
    uint32_t                    error_code = MFS_NO_ERROR;
    uint32_t                    error_code_bkp = MFS_NO_ERROR;

#if MQX_USE_IO_OLD
    drive_ptr = (MFS_DRIVE_STRUCT_PTR)fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
#else
    drive_ptr = (MFS_DRIVE_STRUCT_PTR)dev_context;
    *fp_context = NULL;
#endif

    if ( drive_ptr->MFS_FILE_PTR == 0 )
    {
        /* This means we are opening MFS and not a file . */
        error_code = MFS_Open_Device(drive_ptr);
        if (error_code == MFS_NOT_A_DOS_DISK)
        {
            /*
            ** This error is OK. It just means the disk probably has to be
            ** formatted
            */
            error_code_bkp = error_code;
            error_code = MFS_NO_ERROR;
        }
#if MQX_USE_IO_OLD
        if (error_code == MFS_NO_ERROR) {
            _io_register_file_system(fd_ptr, (char *)open_name_ptr);
        }
#endif
    }
    else
    {
        /* We are opening a file */
        if ( !drive_ptr->DOS_DISK )
        {
            error_code = MFS_NOT_A_DOS_DISK;
            goto error_cleanup;
        }

        open_name_ptr = MFS_Parse_Out_Device_Name((char *)open_name_ptr);

        /* Check for a filename, if none, then check for the temp flag */
        if ( *(open_name_ptr-1) != ':' )
        {
            error_code = MFS_PATH_NOT_FOUND;
        }
        else if ( *open_name_ptr == '\0' )
        {
            error_code = MFS_PATH_NOT_FOUND;
        }
        else
        {
            /*
            ** When opening a file, only read-only attributes are set. The other
            ** attributes (such as hidden, system, etc..) must be set afterwards
            ** with an ioctl call.
            */
#if MQX_USE_IO_OLD
            /* Reset file struct members */
            fd_ptr->LOCATION = 0;
            fd_ptr->SIZE = 0;
            switch ( *flags_str )
            {
                case 'r':
                    if ( *(flags_str + 1) == '+' )
                    {
                        flags |= MFS_ACCESS_READ_WRITE;
                    }
                    else
                    {
                        flags |= MFS_ACCESS_READ_ONLY;
                    }
                    fd_ptr->DEV_DATA_PTR = (void *) MFS_Open_file(drive_ptr, (char *)open_name_ptr, flags, &error_code);
                    break;

#if !MFSCFG_READ_ONLY
                case 'w':
#if MFSCFG_READ_ONLY_CHECK
                    if (MFS_is_read_only (drive_ptr))
                    {
                        error_code = MFS_DISK_IS_WRITE_PROTECTED;
                    }
#endif
                    if (error_code == MFS_NO_ERROR)
                    {
                        if ( *(flags_str + 1) == '+' )
                        {
                            flags |= MFS_ACCESS_READ_WRITE;
                        }
                        else
                        {
                            flags |= MFS_ACCESS_WRITE_ONLY;
                        }
                        fd_ptr->DEV_DATA_PTR = (void *) MFS_Create_file(drive_ptr, 0, (char *)open_name_ptr, &error_code);
                        if ( error_code == MFS_NO_ERROR )
                        {
                            MFS_HANDLE_PTR h = (MFS_HANDLE_PTR)fd_ptr->DEV_DATA_PTR;
                            h->ACCESS = flags;
                        }
                    }
                    break;
#endif //!MFSCFG_READ_ONLY

#if !MFSCFG_READ_ONLY
                case 'a':
#if MFSCFG_READ_ONLY_CHECK
                    if (MFS_is_read_only (drive_ptr))
                    {
                        error_code = MFS_DISK_IS_WRITE_PROTECTED;
                    }
#endif
                    if (error_code == MFS_NO_ERROR)
                    {
                        if ( *(flags_str + 1) == '+' )
                        {
                            flags |= MFS_ACCESS_READ_WRITE;
                        }
                        else
                        {
                            flags |= MFS_ACCESS_WRITE_ONLY;
                        }
                        fd_ptr->DEV_DATA_PTR = (void *) MFS_Open_file(drive_ptr, (char *)open_name_ptr, flags, &error_code);
                        if ( error_code == MFS_FILE_NOT_FOUND )
                        {
                            error_code = MFS_NO_ERROR;
                            fd_ptr->DEV_DATA_PTR = (void *) MFS_Create_file(drive_ptr, 0, (char *)open_name_ptr, &error_code);
                            if ( error_code == MFS_NO_ERROR )
                            {
                                MFS_HANDLE_PTR h = (MFS_HANDLE_PTR)fd_ptr->DEV_DATA_PTR;
                                h->ACCESS = flags;
                            }
                        }
                        if (error_code == MFS_NO_ERROR) {
                            MFS_HANDLE_PTR h = (MFS_HANDLE_PTR)fd_ptr->DEV_DATA_PTR;
                            fd_ptr->SIZE = h->SIZE = mqx_dtohl(h->DIR_ENTRY.FILE_SIZE); //we need the size only because of location, but in fallthrough catch we set the size once again
                            h->SAVED_POSITION = 0;
                            h->LOCATION = h->SIZE;
                            MFS_Move_file_pointer(h, drive_ptr, &error_code);
                            fd_ptr->LOCATION = h->LOCATION;
                            fd_ptr->FLAGS |= IO_FLAG_AT_EOF;
                        }
                    }
                    break;
#endif //!MFSCFG_READ_ONLY

                case 'x':
#if MFSCFG_READ_ONLY_CHECK
                    if (MFS_is_read_only (drive_ptr))
                    {
                        error_code = MFS_DISK_IS_WRITE_PROTECTED;
                    }
#endif
                    if (error_code == MFS_NO_ERROR)
                    {
                        fd_ptr->DEV_DATA_PTR = (void *) MFS_Create_temp_file(drive_ptr, flags, (char *)open_name_ptr, &error_code);
                    }
                    break;

#if !MFSCFG_READ_ONLY
                case 'n':
#if MFSCFG_READ_ONLY_CHECK
                    if (MFS_is_read_only (drive_ptr))
                    {
                        error_code = MFS_DISK_IS_WRITE_PROTECTED;
                    }
#endif
                    if (error_code == MFS_NO_ERROR)
                    {
                        if ( *(flags_str + 1) == '+' )
                        {
                            flags |= MFS_ACCESS_READ_WRITE;
                        }
                        else
                        {
                            flags |= MFS_ACCESS_WRITE_ONLY;
                        }

                        fd_ptr->DEV_DATA_PTR = (void *) MFS_Create_new_file(drive_ptr, 0, (char *)open_name_ptr, &error_code);
                        if ( error_code == MFS_NO_ERROR )
                        {
                            MFS_HANDLE_PTR h = (MFS_HANDLE_PTR) fd_ptr->DEV_DATA_PTR;
                            h->ACCESS = flags;
                        }
                    }
                    break;
#endif //MFSCFG_READ_ONLY
                default:
                    error_code = MFS_INVALID_PARAMETER;
                    break;
            }
#else
            if (fflags & O_RDWR) {
#if MFSCFG_READ_ONLY_CHECK
                if (MFS_is_read_only (drive_ptr))
                {
                    error_code = MFS_DISK_IS_WRITE_PROTECTED;
                    goto error_cleanup;
                }
#endif
                flags |= MFS_ACCESS_READ_WRITE;
                if (fflags & O_CREAT) {
                    *fp_context = (void *) MFS_Create_file(drive_ptr, 0, (char *)open_name_ptr, &error_code);
                }
                else {
                    *fp_context = (void *) MFS_Open_file(drive_ptr, (char *)open_name_ptr, flags, &error_code);
                }
            }
            else if (fflags & O_WRONLY) {
#if MFSCFG_READ_ONLY_CHECK
                if (MFS_is_read_only (drive_ptr))
                {
                    error_code = MFS_DISK_IS_WRITE_PROTECTED;
                    goto error_cleanup;
                }
#endif
                flags |= MFS_ACCESS_WRITE_ONLY;
                if (fflags & O_CREAT) {
                    *fp_context = (void *) MFS_Create_file(drive_ptr, 0, (char *)open_name_ptr, &error_code);
                }
                else {
                    *fp_context = (void *) MFS_Open_file(drive_ptr, (char *)open_name_ptr, flags, &error_code);
                }
            }
            else //since RD_ONLY, RDWR and WRONLY is mutual exclusive, we use else here. This is also because O_RDONLY can be defined as zero
            {
                flags |= MFS_ACCESS_READ_ONLY;
                if (fflags & O_CREAT) {
                    *fp_context = (void *) MFS_Create_file(drive_ptr, MFS_ATTR_READ_ONLY, (char *)open_name_ptr, &error_code);
                }
                else {
                    *fp_context = (void *) MFS_Open_file(drive_ptr, (char *)open_name_ptr, flags, &error_code);
                }
            }
            if ( error_code == MFS_NO_ERROR )
            {
                MFS_HANDLE_PTR h = (MFS_HANDLE_PTR)*fp_context;
                h->SIZE = mqx_dtohl(h->DIR_ENTRY.FILE_SIZE); //TODO: check if this is necessary, the fallthrough catch after error_cleanup should set up the value
                h->SAVED_POSITION = 0;
                if (fflags & O_APPEND) {
                    h->LOCATION = h->SIZE;
                    MFS_Move_file_pointer(h, drive_ptr, &error_code);
                }
                else {
                    h->LOCATION = 0;
                }
                h->ACCESS = flags;
            }
#endif //MQX_USE_IO_OLD
        }
    }
error_cleanup:

    /* Check to make sure the error code is OK */
    if ( error_code == MFS_NO_ERROR )
    {
        /* Initialise the file information fields */
#if MQX_USE_IO_OLD
        MFS_HANDLE_PTR h = (MFS_HANDLE_PTR) fd_ptr->DEV_DATA_PTR;
        fd_ptr->ERROR = error_code_bkp;
        if (h) {
            fd_ptr->SIZE = mqx_dtohl(h->DIR_ENTRY.FILE_SIZE);
#else
        MFS_HANDLE_PTR h = (MFS_HANDLE_PTR) *fp_context;
        if (h) {
#endif
            h->SIZE = mqx_dtohl(h->DIR_ENTRY.FILE_SIZE);
        }
    }
    else
    {
        // An error occured, so the calling function (_io_fopen) will
        // free the MQX_FILE_PTR. Need to release the handle
#if MQX_USE_IO_OLD
        if ( fd_ptr->DEV_DATA_PTR ) {
            MFS_Close_file((MFS_HANDLE_PTR) fd_ptr->DEV_DATA_PTR, drive_ptr);
        }
#else
        if ( *fp_context ) {
            MFS_Close_file((MFS_HANDLE_PTR) *fp_context, drive_ptr);
        }

        if (error_code != MFS_NO_ERROR) {
            errno = error_code;
            return -1;
        }
#endif
    }

    return(error_code);
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_mfs_close
* Returned Value   : ERROR CODE
* Comments         : Closes MFS driver
*
*END*----------------------------------------------------------------------*/
#if MQX_USE_IO_OLD
static _mqx_int _io_mfs_close
    (
       MQX_FILE_PTR                   fd_ptr   /* [IN] the file handle for the device being closed */
    )
#else
static int _io_mfs_close
    (
       void *dev_context,

       void *fp_context
    )
#endif
{
#if MQX_USE_IO_OLD
    MFS_HANDLE_PTR handle = (MFS_HANDLE_PTR) fd_ptr->DEV_DATA_PTR;
    MFS_DRIVE_STRUCT_PTR drive_ptr = (MFS_DRIVE_STRUCT_PTR) fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
#else
    MFS_HANDLE_PTR handle = (MFS_HANDLE_PTR) fp_context;
    MFS_DRIVE_STRUCT_PTR drive_ptr = (MFS_DRIVE_STRUCT_PTR) dev_context;
#endif
    uint32_t                 result = MQX_OK;
   // unregister file system from handle table

#if MQX_USE_IO_OLD
   _io_unregister_file_system(fd_ptr);
#endif

    if ( handle == NULL )
    {
        /* We are closing the mfs_fd_ptr, and not a normal file */
        result = MFS_Close_Device(drive_ptr);
    }
    else
    {
        /* We are closing a normal file */
        result = MFS_Close_file(handle, drive_ptr);
    }

#if !MQX_USE_IO_OLD
    if (result != MFS_NO_ERROR) {
        errno = result;
        return -1;
    }
#endif
    return(result);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_mfs_read
* Returned Value   : number of characters read
* Comments         : Reads data from MFS driver
*
*END*----------------------------------------------------------------------*/
#if MQX_USE_IO_OLD
static _mqx_int _io_mfs_read
    (
    MQX_FILE_PTR file_ptr,   /* [IN] the stream to perform the operation on */
    char         *data_ptr,   /* [IN] the data location to read to */
    int32_t      num         /* [IN] the number of bytes to read */
    )
#else
static int _io_mfs_read
    (
       void *dev_context,

       void *fp_context,

       void *data_ptr,

       size_t num
    )
#endif
{
#if MQX_USE_IO_OLD
    MFS_HANDLE_PTR handle = (MFS_HANDLE_PTR) file_ptr->DEV_DATA_PTR;
    MFS_DRIVE_STRUCT_PTR drive_ptr = (MFS_DRIVE_STRUCT_PTR) file_ptr->DEV_PTR->DRIVER_INIT_PTR;
#else
    MFS_HANDLE_PTR handle = (MFS_HANDLE_PTR) fp_context;
    MFS_DRIVE_STRUCT_PTR drive_ptr = (MFS_DRIVE_STRUCT_PTR) dev_context;
#endif
    int32_t result;
    _mfs_error errcode;

    result = MFS_Read(handle, drive_ptr, num, data_ptr, &errcode);

#if MQX_USE_IO_OLD
    file_ptr->ERROR = errcode;
    file_ptr->LOCATION = handle->LOCATION;
    /* Check for EOF. The MFS EOF must be translated to the standard EOF */
    if (errcode == MFS_EOF)
    {
        file_ptr->FLAGS |= IO_FLAG_AT_EOF;
    }

    return (errcode == 0 || errcode == MFS_EOF) ? result : IO_ERROR;
#else
    if (errcode != 0 && errcode != MFS_EOF)
    {
        errno = errcode;
        return -1;
    }

    return result;
#endif //MQX_USE_IO_OLD

}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_mfs_write
* Returned Value   : number of characters written
* Comments         : Writes data to the fdv_ram device
*
*END*----------------------------------------------------------------------*/
#if MQX_USE_IO_OLD
static _mqx_int _io_mfs_write
    (
    MQX_FILE_PTR file_ptr,   /* [IN] the stream to perform the operation on */
    char    *data_ptr,   /* [IN] the data location to read to */
    int32_t      num         /* [IN] the number of bytes to read */
    )
#else
static int _io_mfs_write
    (
       void *dev_context,

       void *fp_context,

       const void *data_ptr,

       size_t num
    )
#endif
{
#if MQX_USE_IO_OLD
    MFS_HANDLE_PTR handle = (MFS_HANDLE_PTR) file_ptr->DEV_DATA_PTR;
    MFS_DRIVE_STRUCT_PTR drive_ptr = (MFS_DRIVE_STRUCT_PTR) file_ptr->DEV_PTR->DRIVER_INIT_PTR;
#else
    MFS_HANDLE_PTR handle = (MFS_HANDLE_PTR) fp_context;
    MFS_DRIVE_STRUCT_PTR drive_ptr = (MFS_DRIVE_STRUCT_PTR) dev_context;
#endif
    int32_t result;
    _mfs_error errcode;

#if MFSCFG_READ_ONLY

#if MQX_USE_IO_OLD
    file_ptr->ERROR = MFS_OPERATION_NOT_ALLOWED;
    return IO_ERROR;
#else
    errno = MFS_OPERATION_NOT_ALLOWED;
    return -1;
#endif //MQX_USE_IO_OLD


#else //MFSCFG_READ_ONLY


#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (drive_ptr))
    {
#if MQX_USE_IO_OLD
        file_ptr->ERROR = MFS_OPERATION_NOT_ALLOWED;
        return IO_ERROR;
#else
        errno = MFS_OPERATION_NOT_ALLOWED;
        return -1;
#endif //MQX_USE_IO_OLD
    }
#endif //MFSCFG_READ_ONLY_CHECK

    if (data_ptr == NULL)
    {
       if (num == 0)
       {
           MFS_Flush_Device(drive_ptr, handle);
       }
       else
       {
            return MFS_ERROR;
       }
    }
    else
    {
        result = MFS_Write(handle, drive_ptr, num, (char *)data_ptr, &errcode);
    }

#if MQX_USE_IO_OLD
    file_ptr->ERROR = errcode;
    file_ptr->LOCATION = handle->LOCATION;
    /* Check for EOF. The MFS EOF must be translated to the standard EOF */
    if (result == MFS_NO_ERROR && errcode == MFS_EOF)
    {
        file_ptr->FLAGS |= IO_FLAG_AT_EOF;
        /* Update the file size in the old IO structure */
        file_ptr->SIZE = handle->SIZE;
    }

    return (errcode == 0 || errcode == MFS_EOF) ? result : IO_ERROR;
#else
    if (result == MFS_NO_ERROR && errcode != 0 && errcode != MFS_EOF) {
        errno = errcode;
        return -1;
    }

    return result;
#endif //MQX_USE_IO_OLD

#endif //MFSCFG_READ_ONLY
}

#if !MQX_USE_IO_OLD
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_mfs_lseek
* Returned Value   : off_t
* Comments         :
*    The returned value is IO_EOF or a MQX error code.
*
*END*----------------------------------------------------------------------*/
static off_t _io_mfs_lseek
    (
       void *dev_context,

       void *fp_context,

       off_t offset,

       int mode
    )
{
   MFS_HANDLE_PTR handle = (MFS_HANDLE_PTR) fp_context;
   MFS_DRIVE_STRUCT_PTR drive_ptr = (MFS_DRIVE_STRUCT_PTR) dev_context;

   off_t retval = 0;

   switch (mode) {
      case SEEK_SET:
        //set the absolute position
        if (offset < 0) {
            retval = (off_t)EINVAL;
        }
        else {
            handle->LOCATION = offset;
        }
        break;
      case SEEK_CUR:
        //set the relative position from current
        //check for overflow
        if (offset > 0 && (handle->LOCATION + offset < handle->LOCATION)) {
            retval = (off_t)EINVAL;
        }
        else if (offset < 0 && (handle->LOCATION + offset > handle->LOCATION)) {
            retval = (off_t)EINVAL;
        }
        else {
            handle->LOCATION = handle->LOCATION + offset;
        }
        break;
      case SEEK_END:
        //set the relative position from end
        //check for overflow
        if (offset > 0 && (handle->SIZE + offset < handle->SIZE)) {
            retval = (off_t)EINVAL;
        }
        else if (offset < 0 && (handle->SIZE + offset > handle->SIZE)) {
            retval = (off_t)EINVAL;
        }
        else {
            handle->LOCATION = handle->SIZE + offset;
        }
        break;
      default:
        errno = EINVAL;
        retval = (off_t)-1;
        break;
   }

   return retval == (off_t)0 ? handle->SIZE : retval;
}
#endif //!MQX_USE_IO_OLD

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_mfs_ioctl
* Returned Value   : int32_t
* Comments         :
*    The returned value is IO_EOF or a MQX error code.
*
*END*----------------------------------------------------------------------*/
#if MQX_USE_IO_OLD
static _mqx_int _io_mfs_ioctl
    (
    MQX_FILE_PTR file_ptr,   /* [IN] the stream to perform the operation on */
    uint32_t     cmd,        /* [IN] the ioctl command */
    uint32_t     *param_ptr   /* [IN] the ioctl parameters */
    )
#else
static int _io_mfs_ioctl
    (
       void *dev_context,

       void *fp_context,

       unsigned long int cmd,

       va_list ap
    )
#endif
{
#if MQX_USE_IO_OLD
    MFS_HANDLE_PTR handle = (MFS_HANDLE_PTR) file_ptr->DEV_DATA_PTR;
    MFS_DRIVE_STRUCT_PTR drive_ptr = (MFS_DRIVE_STRUCT_PTR) file_ptr->DEV_PTR->DRIVER_INIT_PTR;
    uint64_t                *param64_ptr = (uint64_t *) param_ptr;
#else
    MFS_HANDLE_PTR handle = (MFS_HANDLE_PTR) fp_context;
    MFS_DRIVE_STRUCT_PTR drive_ptr = (MFS_DRIVE_STRUCT_PTR) dev_context;
    uint32_t                *param_ptr;
    uint64_t                *param64_ptr;
#endif
    uint32_t                result = MFS_NO_ERROR;

    switch ( cmd )
    {
#if MQX_USE_IO_OLD
        case IO_IOCTL_SEEK:
            handle->LOCATION = file_ptr->LOCATION;
            MFS_Move_file_pointer(handle, drive_ptr, &result);
            if ( result == MFS_EOF )
            {
                file_ptr->FLAGS |= IO_FLAG_AT_EOF;
            }
            break;

        case IO_IOCTL_CHAR_AVAIL:
            /*
            ** Location is one smaller than file size. We want to know if
            ** we can read one extra character
            */
#if !MQX_USE_IO_OLD //never true in this context
            param_ptr = va_arg(ap, uint32_t *);
#endif
            *param_ptr = handle->LOCATION >= mqx_dtohl(handle->DIR_ENTRY.FILE_SIZE) ? FALSE : TRUE;
            break;
#endif //MQX_USE_IO_OLD

        case IO_IOCTL_BAD_CLUSTERS:
            result = MFS_Bad_clusters(drive_ptr);
            break;
        case IO_IOCTL_LAST_CLUSTER:
            result = MFS_Last_cluster(drive_ptr);
            break;
        case IO_IOCTL_FREE_SPACE:
            {
#if !MQX_USE_IO_OLD
                param64_ptr = va_arg(ap, uint64_t *);
#else
                param64_ptr = (uint64_t *) param_ptr;
#endif
                uint64_t bytes_free = MFS_Get_disk_free_space(drive_ptr);
                if (param64_ptr) {
                    *param64_ptr = bytes_free;
                }
            }
            break;
        case IO_IOCTL_FREE_CLUSTERS:
            MFS_lock(drive_ptr);
            {
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#else
                param_ptr = (uint32_t *) param_ptr;
#endif
                uint32_t clusters_free = MFS_Get_disk_free_space_internal(drive_ptr, NULL);
                if (param_ptr) {
                    *param_ptr = clusters_free;
                }
            }
            MFS_unlock(drive_ptr,FALSE);
            break;
        case IO_IOCTL_GET_CLUSTER_SIZE:
            MFS_lock(drive_ptr);
#if !MQX_USE_IO_OLD
            param_ptr = va_arg(ap, uint32_t *);
#endif
            *param_ptr = (uint32_t)drive_ptr->CLUSTER_SIZE_BYTES;
            MFS_unlock(drive_ptr,FALSE);
            break;
        case IO_IOCTL_GET_CURRENT_DIR:
#if !MQX_USE_IO_OLD
            param_ptr = va_arg(ap, uint32_t *);
#endif
            result = MFS_Get_current_dir(drive_ptr, (char *) param_ptr);
            break;
        case IO_IOCTL_CHANGE_CURRENT_DIR:
#if !MQX_USE_IO_OLD
            param_ptr = va_arg(ap, uint32_t *);
#endif
            param_ptr = (uint32_t *) MFS_Parse_Out_Device_Name((char *) param_ptr);
            result = MFS_Change_current_dir(drive_ptr, (char *) param_ptr);
            break;
        case IO_IOCTL_CHECK_DIR_EXIST:
#if !MQX_USE_IO_OLD
            param_ptr = va_arg(ap, uint32_t *);
#endif
            param_ptr = (uint32_t *) MFS_Parse_Out_Device_Name((char *) param_ptr);
            result = MFS_Check_dir_exist(drive_ptr, (char *) param_ptr);
            break;

        case IO_IOCTL_FIND_FIRST_FILE:
#if !MQX_USE_IO_OLD
            param_ptr = va_arg(ap, uint32_t *);
#endif
            result = MFS_Find_first_file(drive_ptr, ((MFS_SEARCH_PARAM_PTR) param_ptr)->ATTRIBUTE,
                MFS_Parse_Out_Device_Name(((MFS_SEARCH_PARAM_PTR) param_ptr)->WILDCARD),
                ((MFS_SEARCH_PARAM_PTR) param_ptr)->SEARCH_DATA_PTR);
            break;
        case IO_IOCTL_FIND_NEXT_FILE:
#if !MQX_USE_IO_OLD
            param_ptr = va_arg(ap, uint32_t *);
#endif
            result = MFS_Find_next_file(drive_ptr, (MFS_SEARCH_DATA_PTR) param_ptr);
            break;
        case IO_IOCTL_GET_FILE_ATTR:
#if !MQX_USE_IO_OLD
            param_ptr = va_arg(ap, uint32_t *);
#endif
            result = MFS_Get_file_attributes(drive_ptr, handle,
                MFS_Parse_Out_Device_Name(
                ((MFS_FILE_ATTR_PARAM_PTR) param_ptr)->PATHNAME),
                ((MFS_FILE_ATTR_PARAM_PTR) param_ptr)->ATTRIBUTE_PTR);
            break;
        case IO_IOCTL_GET_DATE_TIME:
#if !MQX_USE_IO_OLD
            param_ptr = va_arg(ap, uint32_t *);
#endif
            result = MFS_Get_date_time(drive_ptr, handle,
                ((MFS_DATE_TIME_PARAM_PTR) param_ptr)->DATE_PTR,
                ((MFS_DATE_TIME_PARAM_PTR) param_ptr)->TIME_PTR);
            break;

        case IO_IOCTL_GET_LFN:
#if !MQX_USE_IO_OLD
            param_ptr = va_arg(ap, uint32_t *);
#endif
            MFS_lock(drive_ptr);
            {
                MFS_GET_LFN_STRUCT_PTR  lfn_ptr = ((MFS_GET_LFN_STRUCT_PTR) param_ptr);
                char *pathname = MFS_Parse_Out_Device_Name(lfn_ptr->PATHNAME);
                result = MFS_get_lfn(drive_ptr, pathname, lfn_ptr->LONG_FILENAME);
#if 0
                //            _mfs_error result;
                MFS_DIR_ENTRY_PTR       entry_ptr;

                if ( lfn_ptr->SEARCH_DATA_PTR )
                {
                    entry_ptr =
                        MFS_Read_directory_sector(drive_ptr,lfn_ptr->SEARCH_DATA_PTR->INTERNAL_SEARCH_DATA.CURRENT_CLUSTER,
                        INDEX_TO_SECTOR(lfn_ptr->SEARCH_DATA_PTR->INTERNAL_SEARCH_DATA.DIR_ENTRY_INDEX), &result);
                    if ( (result== MFS_NO_ERROR && entry_ptr!=NULL) )
                    {
                        entry_ptr += INDEX_TO_OFFSET (lfn_ptr->SEARCH_DATA_PTR->INTERNAL_SEARCH_DATA.DIR_ENTRY_INDEX);

                        result = MFS_get_lfn_of_entry(drive_ptr,entry_ptr,
                            lfn_ptr->SEARCH_DATA_PTR->INTERNAL_SEARCH_DATA.CURRENT_CLUSTER,
                            lfn_ptr->SEARCH_DATA_PTR->INTERNAL_SEARCH_DATA.DIR_ENTRY_INDEX,
                            lfn_ptr->SEARCH_DATA_PTR->INTERNAL_SEARCH_DATA.PREV_CLUSTER,
                            lfn_ptr->LONG_FILENAME);
                    }

                }
                else
                {
                    result = MFS_get_lfn(drive_ptr, pathname, lfn_ptr->LONG_FILENAME);
                }
#endif
            }
            MFS_unlock(drive_ptr,FALSE);
            break;
        case IO_IOCTL_GET_VOLUME:
#if !MQX_USE_IO_OLD
            param_ptr = va_arg(ap, uint32_t *);
#endif
            result = MFS_Get_volume(drive_ptr, (char *) param_ptr);
            break;

#if MFSCFG_READ_ONLY
        case MFS_IOCTL_DELETE_FILE:
        case MFS_IOCTL_CREATE_SUBDIR:
        case MFS_IOCTL_REMOVE_SUBDIR:
        case MFS_IOCTL_RENAME_FILE:
        case MFS_IOCTL_SET_FILE_ATTR:
        case MFS_IOCTL_FLUSH_OUTPUT:
        case MFS_IOCTL_SET_DATE_TIME:
        case MFS_IOCTL_SET_VOLUME:
        case MFS_IOCTL_FORMAT:
        case MFS_IOCTL_DEFAULT_FORMAT:
        case MFS_IOCTL_FORMAT_TEST:
        case MFS_IOCTL_WRITE_CACHE_ON:
        case MFS_IOCTL_WRITE_CACHE_OFF:
        case MFS_IOCTL_FAT_CACHE_ON:
        case MFS_IOCTL_FAT_CACHE_OFF:
        case MFS_IOCTL_FLUSH_FAT:
        case MFS_IOCTL_TEST_UNUSED_CLUSTERS:
        case MFS_IOCTL_SET_FAT_CACHE_MODE:
        case MFS_IOCTL_SET_WRITE_CACHE_MODE:
        case MFS_IOCTL_VERIFY_WRITES:
            result = MFS_OPERATION_NOT_ALLOWED;
            break;


        case MFS_IOCTL_GET_FAT_CACHE_MODE:
        case MFS_IOCTL_GET_WRITE_CACHE_MODE:
#if !MQX_USE_IO_OLD
            param_ptr = va_arg(ap, uint32_t *);
#endif
            if ( param_ptr )
            {
                *(_mfs_cache_policy *)param_ptr = MFS_WRITE_THROUGH_CACHE;
            }
            else
            {
                result = MQX_INVALID_POINTER;
            }
            break;
#else //MFSCFG_READ_ONLY

        case IO_IOCTL_DELETE_FILE:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#endif
                param_ptr = (uint32_t *) MFS_Parse_Out_Device_Name((char *) param_ptr);
                result = MFS_Delete_file(drive_ptr, handle, (char *) param_ptr);
            }
            break;
        case IO_IOCTL_CREATE_SUBDIR:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#endif
                param_ptr = (uint32_t *) MFS_Parse_Out_Device_Name((char *) param_ptr);
                result = MFS_Create_subdir(drive_ptr, (char *) param_ptr);
            }
            break;
        case IO_IOCTL_REMOVE_SUBDIR:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#endif
                param_ptr = (uint32_t *) MFS_Parse_Out_Device_Name((char *) param_ptr);
                result = MFS_Remove_subdir(drive_ptr, (char *) param_ptr);
            }
            break;

        case IO_IOCTL_RENAME_FILE:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#endif
                result = MFS_Rename_file(drive_ptr,
                    MFS_Parse_Out_Device_Name( ((MFS_RENAME_PARAM_PTR) param_ptr)->OLD_PATHNAME),
                    MFS_Parse_Out_Device_Name( ((MFS_RENAME_PARAM_PTR) param_ptr)->NEW_PATHNAME));
            }
            break;
        case IO_IOCTL_SET_FILE_ATTR:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#endif
                result = MFS_Set_file_attributes(drive_ptr, handle,
                    MFS_Parse_Out_Device_Name( ((MFS_FILE_ATTR_PARAM_PTR) param_ptr)->PATHNAME),
                    ((MFS_FILE_ATTR_PARAM_PTR) param_ptr)->ATTRIBUTE_PTR);
            }
            break;

        case IO_IOCTL_SET_DATE_TIME:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#endif
                result = MFS_Set_date_time(drive_ptr, handle,
                    ((MFS_DATE_TIME_PARAM_PTR) param_ptr)->DATE_PTR,
                    ((MFS_DATE_TIME_PARAM_PTR) param_ptr)->TIME_PTR);
            }
            break;

        case IO_IOCTL_SET_VOLUME:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#endif
                result = MFS_Set_volume(drive_ptr, (char *) param_ptr);
            }
            break;

        case IO_IOCTL_WRITE_CACHE_ON:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#endif
                MFS_lock(drive_ptr);
                drive_ptr->WRITE_CACHE_POLICY = MFS_WRITE_BACK_CACHE;
                MFS_unlock(drive_ptr,FALSE);
            }
            break;

        case IO_IOCTL_WRITE_CACHE_OFF:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                MFS_lock(drive_ptr);
                drive_ptr->WRITE_CACHE_POLICY = MFS_WRITE_THROUGH_CACHE;
                MFS_unlock(drive_ptr,FALSE);
            }
            break;

#if MQX_USE_IO_OLD
        case IO_IOCTL_FLUSH_OUTPUT:
            result = MFS_Flush_Device(drive_ptr, handle);
            break;
#endif

        case IO_IOCTL_SET_WRITE_CACHE_MODE:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#endif
                if ( param_ptr )
                {
                    _mfs_cache_policy    policy = *((_mfs_cache_policy *)param_ptr);
                    if ( (policy==MFS_WRITE_THROUGH_CACHE) || (policy==MFS_MIXED_MODE_CACHE) || (policy==MFS_WRITE_BACK_CACHE) )
                    {
                        MFS_lock(drive_ptr);
                        drive_ptr->WRITE_CACHE_POLICY = policy;
                        MFS_unlock(drive_ptr, TRUE);
                    }
                    else
                    {
                        result = MFS_INVALID_PARAMETER;
                    }
                }
                else
                {
                    result = MQX_INVALID_POINTER;
                }
            }
            break;


        case IO_IOCTL_GET_WRITE_CACHE_MODE:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#endif
                if ( param_ptr )
                {
                    *((_mfs_cache_policy *)param_ptr) = drive_ptr->WRITE_CACHE_POLICY ;
                }
                else
                {
                    result = MQX_INVALID_POINTER;
                }
            }
            break;

        case IO_IOCTL_FAT_CACHE_ON:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                MFS_lock(drive_ptr);
                drive_ptr->FAT_CACHE_POLICY = MFS_WRITE_BACK_CACHE;
                MFS_unlock(drive_ptr, FALSE);
            }
            break;

        case IO_IOCTL_FAT_CACHE_OFF:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                MFS_lock(drive_ptr);
                drive_ptr->FAT_CACHE_POLICY = MFS_WRITE_THROUGH_CACHE;
                MFS_unlock(drive_ptr,FALSE);
            }
            break;

        case IO_IOCTL_FLUSH_FAT:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                MFS_lock(drive_ptr);
                MFS_Flush_fat_cache(drive_ptr);
                MFS_unlock(drive_ptr,FALSE);
            }
            break;

        case IO_IOCTL_SET_FAT_CACHE_MODE:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#endif
                if ( param_ptr )
                {
                    _mfs_cache_policy    policy = *((_mfs_cache_policy *)param_ptr);
                    if ( (policy==MFS_WRITE_THROUGH_CACHE) || (policy==MFS_MIXED_MODE_CACHE) || (policy==MFS_WRITE_BACK_CACHE) )
                    {
                        MFS_lock(drive_ptr);
                        drive_ptr->FAT_CACHE_POLICY = policy;
                        MFS_unlock(drive_ptr,TRUE);
                    }
                    else
                    {
                        result = MFS_INVALID_PARAMETER;
                    }
                }
                else
                {
                    result = MQX_INVALID_POINTER;
                }
            }
            break;

        case IO_IOCTL_GET_FAT_CACHE_MODE:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#endif
                if ( param_ptr )
                {
                    *((_mfs_cache_policy *)param_ptr) = drive_ptr->FAT_CACHE_POLICY;
                }
                else
                {
                    result = MQX_INVALID_POINTER;
                }
            }
            break;

        case IO_IOCTL_TEST_UNUSED_CLUSTERS:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#endif
                result = MFS_Test_unused_clusters(drive_ptr, param_ptr);
            }
            break;

        case IO_IOCTL_VERIFY_WRITES:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                MFS_lock(drive_ptr);
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#endif
                if (param_ptr) {
                   if (!drive_ptr->READBACK_SECTOR_PTR && drive_ptr->BPB.SECTOR_SIZE ) {
                      drive_ptr->READBACK_SECTOR_PTR = MFS_mem_alloc_system_align(drive_ptr->BPB.SECTOR_SIZE, drive_ptr->ALIGNMENT);
                      _mem_set_type(drive_ptr->READBACK_SECTOR_PTR, MEM_TYPE_MFS_DATA_SECTOR);
                   }
                   result =  (drive_ptr->READBACK_SECTOR_PTR != NULL)?MQX_OK:MQX_OUT_OF_MEMORY;
                } else {
                   if (drive_ptr->READBACK_SECTOR_PTR) {
                      _mem_free(drive_ptr->READBACK_SECTOR_PTR);
                      drive_ptr->READBACK_SECTOR_PTR = NULL;
                   }
                   result = MQX_OK;
                }
                MFS_unlock(drive_ptr,FALSE);
            }
            break;

#ifdef MFSCFG_ENABLE_FORMAT

        case IO_IOCTL_FORMAT:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#endif
                result = MFS_Format(drive_ptr, ((MFS_IOCTL_FORMAT_PARAM_PTR) param_ptr)->FORMAT_PTR);
            }
            break;

        case IO_IOCTL_DEFAULT_FORMAT:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
                result = MFS_Default_Format(drive_ptr);
            }
            break;

        case IO_IOCTL_FORMAT_TEST:
#if MFSCFG_READ_ONLY_CHECK
            if (MFS_is_read_only (drive_ptr))
            {
                result = MFS_DISK_IS_WRITE_PROTECTED;
            }
#endif
            if (result == MQX_OK)
            {
#if !MQX_USE_IO_OLD
                param_ptr = va_arg(ap, uint32_t *);
#endif
                result = MFS_Format_and_test(drive_ptr,
                    ((MFS_IOCTL_FORMAT_PARAM_PTR) param_ptr)->FORMAT_PTR,
                    ((MFS_IOCTL_FORMAT_PARAM_PTR) param_ptr)->COUNT_PTR);
            }
            break;
#else //MFSCFG_ENABLE_FORMAT
        case MFS_IOCTL_FORMAT:
        case MFS_IOCTL_DEFAULT_FORMAT:
        case MFS_IOCTL_FORMAT_TEST:
            result = MFS_OPERATION_NOT_ALLOWED;
            break;

#endif //MFSCFG_ENABLE_FORMAT

#endif //MFSCFG_READ_ONLY

        case IO_IOCTL_GET_DEVICE_HANDLE:
#if !MQX_USE_IO_OLD
            param_ptr = va_arg(ap, uint32_t *);
#endif
            *param_ptr = (uint32_t)drive_ptr->DEV_FILE_PTR;
            result = MQX_OK;
            break;

        default:
#if MQX_USE_IO_OLD
            result = IO_ERROR_INVALID_IOCTL_CMD;
#else
            result = EINVAL;
#endif
            break;
    }

#if !MQX_USE_IO_OLD
    if (result != MFS_NO_ERROR)
    {
        errno = result;
        return -1;
    }
#endif

    return( (int32_t) result);
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_free_drive_data
* Returned Value   :
* Comments  :
*
*END*---------------------------------------------------------------------*/

static void MFS_free_drive_data( MFS_DRIVE_STRUCT_PTR drive_ptr, bool all)
{
    if ( (drive_ptr->DIR_SECTOR_PTR != NULL) && all )
    {
        _mem_free(drive_ptr->DIR_SECTOR_PTR);
        drive_ptr->DIR_SECTOR_PTR = NULL;
    }

    if ( drive_ptr->DATA_SECTOR_PTR != NULL )
    {
        _mem_free(drive_ptr->DATA_SECTOR_PTR);
        drive_ptr->DATA_SECTOR_PTR = NULL;
    }

    if ( drive_ptr->FAT_CACHE_PTR != NULL )
    {
        _mem_free(drive_ptr->FAT_CACHE_PTR);
        drive_ptr->FAT_CACHE_PTR = NULL;
    }
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Mount_drive_internal
* Returned Value   : error code
* Comments  :   Used to set the MFS drive parameters for a unit.
*   This function assumes that the boot sector of the drive is stored in
*   the drive's sector buffer.  This function is called after MFS is
*   initialized, or after the drive has been formatted.
*
*   NOTE: It is assumed that the drive is locked by the
*   calling function.
*
*END*---------------------------------------------------------------------*/

uint32_t MFS_Mount_drive_internal
    (
    MFS_DRIVE_STRUCT_PTR         drive_ptr
    )
{
    BIOS_PARAM_STRUCT_DISK_PTR   bpb_ptr;
    BIOS_PARAM32_STRUCT_DISK_PTR bpb32_ptr;
    FILESYSTEM_INFO_DISK_PTR     fsinfo_ptr;
    uint32_t                      k,fat_size, root_dir_sectors,
        data_sectors, cluster_count,
        error_code = MFS_NO_ERROR;
    uint32_t                      temp[3];

    /*
    ** First, deallocate the FAT and CLUSTER buffers previously allocated.
    ** If the unit has changed or been reformated, these may not be the same
    ** size.
    */
    MFS_free_drive_data(drive_ptr, FALSE);

    /*
    ** Next, check  to see that the BOOT record is that of a DOS disk.  If  not,
    ** the drive will have to be formatted by the upper layer before the drive
    ** can be 'mounted'.
    */
    if ( (*drive_ptr->DIR_SECTOR_PTR != (char) MFS_DOS30_JMP) &&
        (*drive_ptr->DIR_SECTOR_PTR != (char)MFS_DOS30_B) )
    {
        drive_ptr->DOS_DISK = FALSE;
        return MFS_NOT_A_DOS_DISK;
    }


    /*
    ** Extract the drive parameters (BIOS Parameter Block) from the BOOT Record.
    */
    bpb_ptr = (BIOS_PARAM_STRUCT_DISK_PTR) drive_ptr->DIR_SECTOR_PTR;

    drive_ptr->BPB.SECTOR_SIZE =         mqx_dtohs(bpb_ptr->SECTOR_SIZE);
    drive_ptr->BPB.SECTORS_PER_CLUSTER = mqx_dtohc(bpb_ptr->SECTORS_PER_CLUSTER);
    drive_ptr->BPB.RESERVED_SECTORS =    mqx_dtohs(bpb_ptr->RESERVED_SECTORS);
    drive_ptr->BPB.NUMBER_OF_FAT =       mqx_dtohc(bpb_ptr->NUMBER_OF_FAT);
    drive_ptr->BPB.ROOT_ENTRIES =        mqx_dtohs(bpb_ptr->ROOT_ENTRIES);
    drive_ptr->BPB.NUMBER_SECTORS =      mqx_dtohs(bpb_ptr->NUMBER_SECTORS);
    drive_ptr->BPB.MEDIA_TYPE =          mqx_dtohc(bpb_ptr->MEDIA_TYPE);
    drive_ptr->BPB.SECTORS_PER_FAT =     mqx_dtohs(bpb_ptr->SECTORS_PER_FAT);
    drive_ptr->BPB.SECTORS_PER_TRACK =   mqx_dtohs(bpb_ptr->SECTORS_PER_TRACK);
    drive_ptr->BPB.NUM_HEADS =           mqx_dtohs(bpb_ptr->NUM_HEADS);
    drive_ptr->BPB.HIDDEN_SECTORS =      mqx_dtohl(bpb_ptr->HIDDEN_SECTORS);
    drive_ptr->BPB.MEGA_SECTORS =        mqx_dtohl(bpb_ptr->MEGA_SECTORS);

    if ( drive_ptr->BPB.NUMBER_SECTORS != 0 )
    {
        drive_ptr->BPB.MEGA_SECTORS = drive_ptr->BPB.NUMBER_SECTORS;
    }

    /* Determine FAT type by calculating the count of clusters on disk */
    root_dir_sectors = ((drive_ptr->BPB.ROOT_ENTRIES * sizeof(MFS_DIR_ENTRY))
        + (drive_ptr->BPB.SECTOR_SIZE - 1)) / drive_ptr->BPB.SECTOR_SIZE;

    bpb32_ptr = (BIOS_PARAM32_STRUCT_DISK_PTR) (drive_ptr->DIR_SECTOR_PTR + sizeof(BIOS_PARAM_STRUCT_DISK));
    if ( drive_ptr->BPB.SECTORS_PER_FAT == 0 )
    {
        fat_size = mqx_dtohl(bpb32_ptr->FAT_SIZE);
    }
    else
    {
        fat_size = drive_ptr->BPB.SECTORS_PER_FAT;
    }

    data_sectors = drive_ptr->BPB.MEGA_SECTORS - (drive_ptr->BPB.RESERVED_SECTORS +
        (drive_ptr->BPB.NUMBER_OF_FAT * fat_size) + root_dir_sectors);

    cluster_count = data_sectors / drive_ptr->BPB.SECTORS_PER_CLUSTER;

    /* Now we have cluster count, so we can determine FAT type */
    if ( cluster_count < 4085 )
    {
        drive_ptr->FAT_TYPE = MFS_FAT12;
    }
    else if ( cluster_count < 65525 )
    {
        drive_ptr->FAT_TYPE = MFS_FAT16;
    }
    else
    {
        drive_ptr->FAT_TYPE = MFS_FAT32;
    }

    /*
    ** Calculate the rest of the drive data.
    */
    k = drive_ptr->BPB.SECTORS_PER_CLUSTER;
    for ( drive_ptr->CLUSTER_POWER_SECTORS = 0; !(k & 1); drive_ptr->CLUSTER_POWER_SECTORS++ )
    {
        k>>=1;
    }

    drive_ptr->CLUSTER_SIZE_BYTES = drive_ptr->BPB.SECTORS_PER_CLUSTER * drive_ptr->BPB.SECTOR_SIZE;

    k = drive_ptr->BPB.SECTOR_SIZE;
    for ( drive_ptr->SECTOR_POWER = 0; !(k & 1); drive_ptr->SECTOR_POWER++ )
    {
        k>>=1;
    }

    drive_ptr->CLUSTER_POWER_BYTES = drive_ptr->SECTOR_POWER + drive_ptr->CLUSTER_POWER_SECTORS;

    drive_ptr->CUR_DIR_CLUSTER = 0;
    drive_ptr->FREE_COUNT = FSI_UNKNOWN; /* This is the unknown value */
    drive_ptr->NEXT_FREE_CLUSTER = FSI_UNKNOWN; /* MFS will calculate it later */
    drive_ptr->CURRENT_DIR[0] = '\\'; /* Root dir */
    drive_ptr->CURRENT_DIR[1] = '\0';
    drive_ptr->ENTRIES_PER_SECTOR = drive_ptr->BPB.SECTOR_SIZE / sizeof(MFS_DIR_ENTRY);

    drive_ptr->FAT_START_SECTOR =  drive_ptr->BPB.RESERVED_SECTORS;

    drive_ptr->DATA_SECTOR_PTR = MFS_mem_alloc_system_align(drive_ptr->BPB.SECTOR_SIZE, drive_ptr->ALIGNMENT);
    if ( drive_ptr->DATA_SECTOR_PTR == NULL )
    {
        error_code = MFS_INSUFFICIENT_MEMORY;
    }
    else
    {
        MFS_Invalidate_data_sector(drive_ptr);
        _mem_set_type(drive_ptr->DATA_SECTOR_PTR, MEM_TYPE_MFS_DATA_SECTOR);

        /* Determine if we should enable the FAT and write caches */
        if ( (ioctl(drive_ptr->DEV_FILE_PTR, IO_IOCTL_DEVICE_IDENTIFY, temp) != MQX_OK) || !(temp[IO_IOCTL_ID_ATTR_ELEMENT] & IO_DEV_ATTR_REMOVE) )
        {
            /*
            ** Device either doesn't support the identify command or is not removable.
            ** Assume we can enable both caches
            */
            drive_ptr->WRITE_CACHE_POLICY = MFS_WRITE_BACK_CACHE;
            drive_ptr->FAT_CACHE_POLICY   = MFS_WRITE_BACK_CACHE;
            drive_ptr->READ_ONLY = FALSE;
        }
        else
        {
            // Device is removable. Disable FAT cache to ensure device is not removed without
            // fat cache flush. Enable Mixed mode write cache (only cache during consecitive file writes)
            drive_ptr->WRITE_CACHE_POLICY = MFS_MIXED_MODE_CACHE;
            drive_ptr->FAT_CACHE_POLICY   = MFS_WRITE_THROUGH_CACHE;   //
            drive_ptr->READ_ONLY = (temp[IO_IOCTL_ID_ATTR_ELEMENT] & IO_DEV_ATTR_WRITE) == 0;
        }

        if ( drive_ptr->FAT_TYPE != MFS_FAT32 )
        {
            drive_ptr->ROOT_START_SECTOR = drive_ptr->FAT_START_SECTOR + (drive_ptr->BPB.SECTORS_PER_FAT * drive_ptr->BPB.NUMBER_OF_FAT);
            drive_ptr->DATA_START_SECTOR = drive_ptr->ROOT_START_SECTOR + drive_ptr->BPB.ROOT_ENTRIES/drive_ptr->ENTRIES_PER_SECTOR;
        }
        else
        {
            if ( !error_code )
            {
                drive_ptr->BPB32.FAT_SIZE = mqx_dtohl(bpb32_ptr->FAT_SIZE);
                drive_ptr->BPB32.EXT_FLAGS = mqx_dtohs(bpb32_ptr->EXT_FLAGS);
                drive_ptr->BPB32.FS_VER = mqx_dtohs(bpb32_ptr->FS_VER);
                drive_ptr->BPB32.ROOT_CLUSTER = mqx_dtohl(bpb32_ptr->ROOT_CLUSTER);
                drive_ptr->BPB32.FS_INFO = mqx_dtohs(bpb32_ptr->FS_INFO);
                drive_ptr->BPB32.BK_BOOT_SEC = mqx_dtohs(bpb32_ptr->BK_BOOT_SEC);

                drive_ptr->ROOT_START_SECTOR = 0;
                drive_ptr->DATA_START_SECTOR = drive_ptr->FAT_START_SECTOR + (drive_ptr->BPB32.FAT_SIZE * drive_ptr->BPB.NUMBER_OF_FAT);
                drive_ptr->CUR_DIR_CLUSTER = drive_ptr->BPB32.ROOT_CLUSTER;

                /*
                ** Reset the FSInfo->Free_Count and the FSInfo->Next_Free to
                ** unknown (0xFFFFFFFF). MFS uses it's own internal version of these
                ** fields. If Windows uses the same disk, it will recalculate the
                ** correct fields the first time it mounts the drive.
                */

                /* Check filesystem version for FAT32 */
                if ( drive_ptr->BPB32.FS_VER > MFS_FAT32_VER )
                {
                    error_code = MFS_ERROR_UNKNOWN_FS_VERSION;
                }

                if ( !error_code )
                {
                    fsinfo_ptr = (FILESYSTEM_INFO_DISK_PTR) drive_ptr->DATA_SECTOR_PTR;
                    error_code = MFS_Read_device_sector(drive_ptr, FSINFO_SECTOR, (char *)fsinfo_ptr);

                    if ( (mqx_dtohl(fsinfo_ptr->LEAD_SIG) == FSI_LEADSIG) &&  (mqx_dtohl(fsinfo_ptr->STRUCT_SIG) == FSI_STRUCTSIG) &&
                         (mqx_dtohl(fsinfo_ptr->TRAIL_SIG) == FSI_TRAILSIG) )
                    {
                        drive_ptr->FREE_COUNT = mqx_dtohl(fsinfo_ptr->FREE_COUNT);
                        drive_ptr->NEXT_FREE_CLUSTER = mqx_dtohl(fsinfo_ptr->NEXT_FREE);
                    }

                    mqx_htodl(fsinfo_ptr->LEAD_SIG, FSI_LEADSIG);
                    mqx_htodl(fsinfo_ptr->STRUCT_SIG, FSI_STRUCTSIG);
                    mqx_htodl(fsinfo_ptr->FREE_COUNT, FSI_UNKNOWN);   /* compute it */
                    mqx_htodl(fsinfo_ptr->NEXT_FREE, FSI_UNKNOWN);  /* compute it */
                    mqx_htodl(fsinfo_ptr->TRAIL_SIG, FSI_TRAILSIG);
#if !MFSCFG_READ_ONLY
#if MFSCFG_READ_ONLY_CHECK
                    if (!MFS_is_read_only (drive_ptr))
                    {
#endif
                        if (error_code == MFS_NO_ERROR)
                        {
                            error_code = MFS_Write_device_sector(drive_ptr, FSINFO_SECTOR, (char *)fsinfo_ptr);
                        }
#if MFSCFG_READ_ONLY_CHECK
                    }
#endif
#endif
                }
            }
        }

        drive_ptr->LAST_CLUSTER   = (drive_ptr->BPB.MEGA_SECTORS - drive_ptr->DATA_START_SECTOR) / drive_ptr->BPB.SECTORS_PER_CLUSTER + 1;

        if ( !error_code )
        {
            /* Set the fat buffer size */
            if ( drive_ptr->FAT_TYPE == MFS_FAT32 )
                drive_ptr->FAT_CACHE_SIZE = drive_ptr->BPB32.FAT_SIZE;
            else
                drive_ptr->FAT_CACHE_SIZE = drive_ptr->BPB.SECTORS_PER_FAT;

            if ( drive_ptr->FAT_CACHE_SIZE > MFSCFG_FAT_CACHE_SIZE )
            {
                drive_ptr->FAT_CACHE_SIZE = (MFSCFG_FAT_CACHE_SIZE < 2 ? 2 : MFSCFG_FAT_CACHE_SIZE);
            }

            drive_ptr->FAT_CACHE_PTR = MFS_mem_alloc_system_align(drive_ptr->FAT_CACHE_SIZE * drive_ptr->BPB.SECTOR_SIZE, drive_ptr->ALIGNMENT);

            if ( drive_ptr->FAT_CACHE_PTR == NULL )
            {
                error_code = MFS_INSUFFICIENT_MEMORY;
            }
            else
            {
                _mem_set_type(drive_ptr->FAT_CACHE_PTR, MEM_TYPE_MFS_FAT_BUFFER);

                drive_ptr->FAT_CACHE_DIRTY = FALSE;
                drive_ptr->FAT_CACHE_START = MAX_UINT_32;

                drive_ptr->DOS_DISK = TRUE;

            }
        }
    }

    if ( error_code )
    {
        MFS_free_drive_data(drive_ptr, TRUE);
    }

    return error_code;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Parse_Out_Device_name
* Returned Value   : A pointer to character following the semi-colon (':')
* Comments  :   Isolates the pathname and filename
*
*END*---------------------------------------------------------------------*/

char *MFS_Parse_Out_Device_Name
    (
    char          *name_ptr
    )
{
    char             *start_path_ptr;

    start_path_ptr = name_ptr;

    /* Parse out the device name */
    while ( (*start_path_ptr  != ':') && (*start_path_ptr != '\0') )
    {
        start_path_ptr = start_path_ptr + 1;
    }

    if ( *start_path_ptr == '\0' )
    {
        start_path_ptr = name_ptr;
    }
    else
    {
        start_path_ptr++;
    }

    return(start_path_ptr );
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Open_Device
* Returned Value   : MQX_OK or an error
* Comments         : Opens and initializes MFS driver.
*
*END*----------------------------------------------------------------------*/

int32_t MFS_Open_Device
    (
        MFS_DRIVE_STRUCT_PTR     drive_ptr
    )
{
#if MQX_USE_IO_OLD
    MQX_FILE_PTR dev_fd;
#else
    int          dev_fd;
#endif
    uint32_t     sector_size, k;
    int32_t      error_code;

    dev_fd = drive_ptr->DEV_FILE_PTR;

    drive_ptr->MFS_FILE_PTR = 1;

    /* Select partition, if desired */
    if (drive_ptr->DRV_NUM)
    {
        error_code = ioctl(dev_fd, IO_IOCTL_SEL_PART, &drive_ptr->DRV_NUM);
        if (error_code != MFS_NO_ERROR)
        {
            return error_code;
        }
    }

    drive_ptr->ALIGNMENT = 1; /* Default alignment is one byte (which means no special requirement) */
    ioctl(dev_fd, IO_IOCTL_GET_REQ_ALIGNMENT, &drive_ptr->ALIGNMENT);
    /* Check if alignment is power of 2 */
    if (drive_ptr->ALIGNMENT & (drive_ptr->ALIGNMENT-1))
    {
        return FS_INVALID_DEVICE;
    }
    /* Calculate alignment mask */
    drive_ptr->ALIGNMENT_MASK = drive_ptr->ALIGNMENT-1;

    /*
    ** obtain the buffer for configuration data and for storing general
    ** sector reads
    */
    error_code = _mfs_validate_device(dev_fd, &sector_size, &drive_ptr->BLOCK_MODE);

    if ( error_code )
    {
        /* Device isn't valid */
        drive_ptr->MFS_FILE_PTR = 0;
        return error_code;
    }

    _lwsem_wait(&drive_ptr->SEM);

    drive_ptr->BPB.SECTOR_SIZE = (uint16_t) sector_size;
    drive_ptr->DIR_SECTOR_PTR = MFS_mem_alloc_system_align(sector_size, drive_ptr->ALIGNMENT);
    MFS_Invalidate_directory_sector(drive_ptr);

    if ( drive_ptr->DIR_SECTOR_PTR == NULL )
    {
        _lwsem_post(&drive_ptr->SEM);
        drive_ptr->MFS_FILE_PTR = 0;
        return MFS_INSUFFICIENT_MEMORY;
    }

    _mem_set_type(drive_ptr->DIR_SECTOR_PTR, MEM_TYPE_MFS_DIRECTORY_SECTOR);
    _queue_init(&drive_ptr->HANDLE_LIST, 0);

    k = drive_ptr->BPB.SECTOR_SIZE;
    for ( drive_ptr->SECTOR_POWER = 0; !(k & 1);
        drive_ptr->SECTOR_POWER++ )
    {
        k>>=1;
    }

    /*
    ** read boot sector and get the BIOS Parameter Block
    */
    error_code = MFS_Read_device_sector(drive_ptr, BOOT_SECTOR, drive_ptr->DIR_SECTOR_PTR);

    if ( error_code == MFS_NO_ERROR )
    {
        drive_ptr->DIR_SECTOR_NUMBER = BOOT_SECTOR;
        error_code = MFS_Mount_drive_internal(drive_ptr);
    }

    if ( !error_code )
    {
        /* Determine the real sector size */
        if ( sector_size != drive_ptr->BPB.SECTOR_SIZE )
        {
            _mem_free(drive_ptr->DIR_SECTOR_PTR);
            drive_ptr->DIR_SECTOR_PTR = MFS_mem_alloc_system_align(drive_ptr->BPB.SECTOR_SIZE, drive_ptr->ALIGNMENT);
            MFS_Invalidate_directory_sector(drive_ptr);
            if ( drive_ptr->DIR_SECTOR_PTR == NULL )
            {
                error_code = MFS_INSUFFICIENT_MEMORY;
                drive_ptr->MFS_FILE_PTR = 0;
            }
            else
            {
                _mem_set_type(drive_ptr->DIR_SECTOR_PTR, MEM_TYPE_MFS_DIRECTORY_SECTOR);
            }
        }

        /* Calculate the free space on disk */
#if MFSCFG_CALCULATE_FREE_SPACE_ON_OPEN
        if ( !error_code )
        {
            MFS_Get_disk_free_space_internal(drive_ptr,(uint32_t *)&error_code);
        }
#endif
    }

    _lwsem_post(&drive_ptr->SEM);

    return(error_code);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Flush_Device
* Returned Value   : MQX_OK or an error
* Comments         : Closes MFS driver.
*
*END*----------------------------------------------------------------------*/
int32_t MFS_Flush_Device
    (
        MFS_DRIVE_STRUCT_PTR      drive_ptr,

        MFS_HANDLE_PTR            handle
    )
{
    int32_t                   result = MFS_NO_ERROR;
#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (drive_ptr))
    {
        result = MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif
    if (result == MQX_OK)
    {
        /* lock the filesystem and obtain file handle */
        result = MFS_lock(drive_ptr);
        if (result == MFS_ERROR_INVALID_FILE_HANDLE)
        {
            /* file_ptr is not associated with a particular file, at least try to lock the filesystem */
            handle = NULL;
            result = MFS_lock_dos_disk(drive_ptr);
        }

        if ( result == MFS_NO_ERROR )
        {
            if (handle != NULL && handle->TOUCHED)
            {
                TIME_STRUCT time;
                DATE_STRUCT clk_time;

                _time_get(&time);
                _time_to_date(&time, &clk_time);
                NORMALIZE_DATE(&clk_time);
                mqx_htods(handle->DIR_ENTRY.TIME, PACK_TIME(clk_time));
                mqx_htods(handle->DIR_ENTRY.DATE, PACK_DATE(clk_time));
                result = MFS_Update_entry(drive_ptr, handle);
            }

            MFS_Flush_caches(drive_ptr);
            MFS_unlock(drive_ptr, FALSE);
        }
    }

    return result;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : MFS_Close_Device
* Returned Value   : MQX_OK or an error
* Comments         : Closes MFS driver.
*
*END*----------------------------------------------------------------------*/

int32_t MFS_Close_Device
    (
        MFS_DRIVE_STRUCT_PTR      drive_ptr
    )
{
    FILESYSTEM_INFO_DISK_PTR  fsinfo_ptr;
    int32_t                   result = MFS_NO_ERROR;

#if !MFSCFG_READ_ONLY
    result = MFS_Flush_Device(drive_ptr, NULL);
    if (result != MFS_NO_ERROR)
    {
        return result;
    }
#endif

    MFS_lock(drive_ptr);

#if !MFSCFG_READ_ONLY
    if (result != MFS_DISK_IS_WRITE_PROTECTED)
    {
        MFS_Flush_caches(drive_ptr);
    }
#endif

    if ( _queue_is_empty(&drive_ptr->HANDLE_LIST) )
    {
        if ( drive_ptr->FAT_TYPE == MFS_FAT32 )
        {
#if !MFSCFG_READ_ONLY
            if (result != MFS_DISK_IS_WRITE_PROTECTED)
            {
                fsinfo_ptr = (FILESYSTEM_INFO_DISK_PTR)drive_ptr->DATA_SECTOR_PTR;
                if ( fsinfo_ptr != NULL )
                {
                    mqx_htodl(fsinfo_ptr->LEAD_SIG,   FSI_LEADSIG);
                    mqx_htodl(fsinfo_ptr->STRUCT_SIG, FSI_STRUCTSIG);
                    mqx_htodl(fsinfo_ptr->FREE_COUNT, drive_ptr->FREE_COUNT);
                    mqx_htodl(fsinfo_ptr->NEXT_FREE,  drive_ptr->NEXT_FREE_CLUSTER);
                    mqx_htodl(fsinfo_ptr->TRAIL_SIG,  FSI_TRAILSIG);

                    MFS_Write_device_sector(drive_ptr, FSINFO_SECTOR, (char *)fsinfo_ptr);
                }
            }
#endif
        }

        MFS_free_drive_data(drive_ptr, TRUE);

        drive_ptr->MFS_FILE_PTR = 0;
        result = MFS_NO_ERROR;
    }
    else
    {
        result = MFS_SHARING_VIOLATION;
    }

    MFS_unlock(drive_ptr,FALSE);

    return result;

}

/*FUNCTION*--------------------------------------------------------------
*
* Function Name    : _mfs_partition_destroy
* Returned Value   : MQX_OK or an error code
* Comments         :
*    Destroys a partition allocated in kernel memory
*
*END*-----------------------------------------------------------------*/

uint32_t _mfs_partition_destroy
    (
    _partition_id partition /* [IN] the partition to destory */
    )
{
    return _partition_destroy(partition);
}
