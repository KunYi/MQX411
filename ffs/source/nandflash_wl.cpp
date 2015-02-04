/*HEADER**********************************************************************
*
* Copyright 2010 Freescale Semiconductor, Inc.
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
*   This file contains NAND flash driver functions.
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"
#include "nandflash.h"
#include "nandflash_wl_ffs.h"
#include "nandflash_wl_prv.h"
#include "media_buffer_manager.h"
#include "ddi_media.h"

/* Static variable for NAND WL Driver */
static bool nandwl_ignore_init = FALSE;


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_wl_install
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Install a NAND flash driver.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _io_nandflash_wl_install
(
    /* [IN] The initialization structure for the device */
    NANDFLASH_WL_INIT_STRUCT_CPTR  init_ptr,

    /* Device signature */
    char * device_id
)
{ /* Body */
    IO_NANDFLASH_WL_STRUCT_PTR dev_ptr;

    dev_ptr = (IO_NANDFLASH_WL_STRUCT_PTR)_mem_alloc_system_zero(
    (_mem_size)sizeof(IO_NANDFLASH_WL_STRUCT));

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (dev_ptr == NULL) 
    {
        return MQX_OUT_OF_MEMORY;
    } /* Endif */
#endif

    dev_ptr->CORE_NANDFLASH.INIT                    = init_ptr->CORE_NANDFLASH_INIT_PTR->INIT;
    dev_ptr->CORE_NANDFLASH.DEINIT                  = init_ptr->CORE_NANDFLASH_INIT_PTR->DEINIT;
    dev_ptr->CORE_NANDFLASH.CHIP_ERASE              = init_ptr->CORE_NANDFLASH_INIT_PTR->CHIP_ERASE;
    dev_ptr->CORE_NANDFLASH.BLOCK_ERASE             = init_ptr->CORE_NANDFLASH_INIT_PTR->BLOCK_ERASE;
    dev_ptr->CORE_NANDFLASH.PAGE_READ               = init_ptr->CORE_NANDFLASH_INIT_PTR->PAGE_READ;
    dev_ptr->CORE_NANDFLASH.PAGE_PROGRAM            = init_ptr->CORE_NANDFLASH_INIT_PTR->PAGE_PROGRAM;
    dev_ptr->CORE_NANDFLASH.WRITE_PROTECT           = init_ptr->CORE_NANDFLASH_INIT_PTR->WRITE_PROTECT;
    dev_ptr->CORE_NANDFLASH.IS_BLOCK_BAD            = init_ptr->CORE_NANDFLASH_INIT_PTR->IS_BLOCK_BAD;
    dev_ptr->CORE_NANDFLASH.MARK_BLOCK_AS_BAD       = init_ptr->CORE_NANDFLASH_INIT_PTR->MARK_BLOCK_AS_BAD;
    dev_ptr->CORE_NANDFLASH.IOCTL                   = init_ptr->CORE_NANDFLASH_INIT_PTR->IOCTL;
    dev_ptr->CORE_NANDFLASH.NANDFLASH_INFO_PTR      = init_ptr->CORE_NANDFLASH_INIT_PTR->NANDFLASH_INFO_PTR;
    dev_ptr->CORE_NANDFLASH.VIRTUAL_PAGE_SIZE       = BSP_VIRTUAL_PAGE_SIZE;
    dev_ptr->CORE_NANDFLASH.NUM_VIRTUAL_PAGES       = (init_ptr->CORE_NANDFLASH_INIT_PTR->NANDFLASH_INFO_PTR->BLOCK_SIZE / BSP_VIRTUAL_PAGE_SIZE) * init_ptr->CORE_NANDFLASH_INIT_PTR->NANDFLASH_INFO_PTR->NUM_BLOCKS;
    dev_ptr->CORE_NANDFLASH.PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO    
        = (init_ptr->CORE_NANDFLASH_INIT_PTR->NANDFLASH_INFO_PTR->PHY_PAGE_SIZE / BSP_VIRTUAL_PAGE_SIZE);
    dev_ptr->CORE_NANDFLASH.ECC_SIZE                = BSP_ECC_SIZE;
    dev_ptr->CORE_NANDFLASH.WRITE_VERIFY            = init_ptr->CORE_NANDFLASH_INIT_PTR->WRITE_VERIFY;
    dev_ptr->CORE_NANDFLASH.DEVICE_SPECIFIC_DATA    = init_ptr->CORE_NANDFLASH_INIT_PTR->DEVICE_SPECIFIC_DATA;
    dev_ptr->PHY_PAGE_READ_RAW                      = init_ptr->PHY_PAGE_READ_RAW;
    dev_ptr->PHY_PAGE_WRITE_RAW                     = init_ptr->PHY_PAGE_WRITE_RAW;
    dev_ptr->PAGE_READ_METADATA                     = init_ptr->PAGE_READ_METADATA;
    dev_ptr->PAGE_READ_WITH_METADATA                = init_ptr->PAGE_READ_WITH_METADATA;
    dev_ptr->PAGE_WRITE_WITH_METADATA               = init_ptr->PAGE_WRITE_WITH_METADATA;

    _lwsem_create(&dev_ptr->CORE_NANDFLASH.LWSEM, 1);
    
    if (device_id)
    {
        return (_io_dev_install_ext(
        device_id,
        _io_nandflash_wl_open,
        _io_nandflash_wl_close,
        _io_nandflash_wl_read,
        _io_nandflash_wl_write,
        _io_nandflash_wl_ioctl,
        _io_nandflash_wl_uninstall, 
        (void *)dev_ptr)); 
    }
    else
    {
        return (_io_dev_install_ext(
        init_ptr->CORE_NANDFLASH_INIT_PTR->ID_PTR,
        _io_nandflash_wl_open,
        _io_nandflash_wl_close,
        _io_nandflash_wl_read,
        _io_nandflash_wl_write,
        _io_nandflash_wl_ioctl,
        _io_nandflash_wl_uninstall, 
        (void *)dev_ptr));
    }
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_wl_uninstall
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Uninstalls a NAND flash driver.
*
*END*----------------------------------------------------------------------*/

_mqx_int _io_nandflash_wl_uninstall
(
    /* [IN] The IO device structure for the device */
    IO_DEVICE_STRUCT_PTR   io_dev_ptr
)
{ /* Body */
    IO_NANDFLASH_WL_STRUCT_PTR handle_ptr = 
        (IO_NANDFLASH_WL_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;

    if (handle_ptr->CORE_NANDFLASH.COUNT == 0) 
    {
        _mem_free((void *)handle_ptr);
        io_dev_ptr->DRIVER_INIT_PTR = NULL;
        return IO_OK;
    } 
    else 
    {
        return IO_ERROR_DEVICE_BUSY;
    } /* Endif */  

}  /* Endbody */    

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_wl_open
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         : Opens and initializes NAND flash driver.
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_nandflash_wl_open
(
    /* [IN] the file handle for the device being opened */
    MQX_FILE_PTR fd_ptr,

    /* [IN] the remaining portion of the name of the device */
    char * open_name_ptr,

    /* [IN] the flags to be used during operation: */
    char * flags
)
{ /* Body */
    IO_DEVICE_STRUCT_PTR      io_dev_ptr = fd_ptr->DEV_PTR;
    IO_NANDFLASH_WL_STRUCT_PTR   handle_ptr = 
        (IO_NANDFLASH_WL_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
    uint32_t                   chip_initialized;
    uint32_t                   num_ecc_bytes;
    _mqx_int status;
    bool force_repair = FALSE;

    switch (*flags) 
    {
        case 'I':
            nandwl_ignore_init = TRUE;
            force_repair = FALSE;
            break;
        case 'R':
            nandwl_ignore_init = FALSE;
            force_repair = TRUE;
            break;
        default:
            nandwl_ignore_init = FALSE;
            force_repair = FALSE;
    }   

    if (handle_ptr->CORE_NANDFLASH.COUNT) 
    {
        /* Device is already opened */
        fd_ptr->FLAGS = handle_ptr->CORE_NANDFLASH.FLAGS;
        handle_ptr->CORE_NANDFLASH.COUNT++;
        return MQX_OK;
    } /* Endif */
    
    if (handle_ptr->CORE_NANDFLASH.INIT) 
    {
        chip_initialized = (*handle_ptr->CORE_NANDFLASH.INIT)(&handle_ptr->CORE_NANDFLASH);
    }/* Endif */

#if MQX_CHECK_ERRORS
    if (chip_initialized != NANDFLASHERR_NO_ERROR) 
    {
        return(IO_ERROR);
    } /* Endif */
#endif   

    /* Check if the sum of virtual page size (incl. ECC bytes) per one physical page 
    is not greater than the physical page size plus the number of physical spare bytes. */
    num_ecc_bytes = NANDFLASH_ECC_SIZE_TO_NUM_BYTES_CONV(handle_ptr->CORE_NANDFLASH.ECC_SIZE);
#if !(BSP_M5329EVB || BSP_TWRMPC5125)
    if(((handle_ptr->CORE_NANDFLASH.VIRTUAL_PAGE_SIZE + num_ecc_bytes) * handle_ptr->CORE_NANDFLASH.PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO) > 
        (handle_ptr->CORE_NANDFLASH.NANDFLASH_INFO_PTR->PHY_PAGE_SIZE + handle_ptr->CORE_NANDFLASH.NANDFLASH_INFO_PTR->SPARE_AREA_SIZE))
    {
        return ((_mqx_int)NANDFLASHERR_IMPROPER_ECC_SIZE);
    }
#endif

    handle_ptr->CORE_NANDFLASH.COUNT++;

    // TODO: use nandwl_ignore_init variable to ignore creating WL module
    if (nandwl_ignore_init == FALSE)
    {
        // TODO: Communicate with WL 
        NandHalSetMqxPtr(0, fd_ptr);
        
        media_buffer_init();
        
        status = MediaInit(kInternalNandMedia);
        if (status != SUCCESS) 
        { 
            WL_LOG(WL_MODULE_NANDWL, WL_LOG_ERROR, "\n Media init failed err: %s\n", wl_err_to_str(status, kNandDontCare));
            return status;
            // return ERROR_DDI_LDL_LDRIVE_REPAIR_REQUIRED;
        } 

        status = MediaBuildFromPreDefine();
        if (status != SUCCESS) 
        {
            MediaShutdown(kInternalNandMedia);
            media_buffer_deinit();
            WL_LOG(WL_MODULE_NANDWL, WL_LOG_ERROR, "\n Media allocate err: %s\n", wl_err_to_str(status, kNandDontCare));
            return status;
        }

        status = DriveInit(DRIVE_TAG_DATA); // DRIVE_TAG_DATA
        if (status != SUCCESS) 
        {
            if (force_repair == FALSE) 
            {
                MediaShutdown(kInternalNandMedia);
                media_buffer_deinit();
                WL_LOG(WL_MODULE_NANDWL, WL_LOG_ERROR, "\n Drive init err: %s\n", wl_err_to_str(ERROR_DDI_LDL_LDRIVE_REPAIR_REQUIRED, kNandDontCare));
                return ERROR_DDI_LDL_LDRIVE_REPAIR_REQUIRED;
            }
            else if (force_repair == TRUE) 
            {
                return DriveRepair(DRIVE_TAG_DATA, 0 , FALSE);
            }
        }
    }

    return MQX_OK;  

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_wl_close
* Returned Value   : MQX_OK
* Comments         : Closes NAND flash driver
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_nandflash_wl_close
(
    /* [IN] the file handle for the device being closed */
    MQX_FILE_PTR fd_ptr
)
{ /* Body */
    IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
    IO_NANDFLASH_WL_STRUCT_PTR handle_ptr = 
    (IO_NANDFLASH_WL_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
    _mqx_int status;

    if (--handle_ptr->CORE_NANDFLASH.COUNT == 0) 
    {
        if (handle_ptr->CORE_NANDFLASH.DEINIT) 
        {
            (*handle_ptr->CORE_NANDFLASH.DEINIT)((IO_NANDFLASH_STRUCT_PTR)&handle_ptr->CORE_NANDFLASH);
        } /* Endif */
    } /* Endif */

    if (nandwl_ignore_init == FALSE)
    {
        status = MediaShutdown(kInternalNandMedia);
        if (status != SUCCESS) 
        {
            WL_LOG(WL_MODULE_NANDWL, WL_LOG_ERROR, "\n Media shutdown err: %s\n", wl_err_to_str(status, kNandDontCare));
            return status;
        }
        media_buffer_deinit();
    }

    return MQX_OK;  

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_wl_read
* Returned Value   : number of pages read
* Comments         : Reads data from NAND flash memory device
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_nandflash_wl_read
(
    /* [IN] the file handle for the device */
    MQX_FILE_PTR fd_ptr,

    /* [IN] where the data is to be stored */
    char *   data_ptr,

    /* [IN] the number of pages to input */
    _mqx_int   num
)
{ /* Body */
    IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
    IO_NANDFLASH_WL_STRUCT_PTR handle_ptr  = 
        (IO_NANDFLASH_WL_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
    _mqx_int status;
    _mqx_int count1, lastSector;
    uint32_t i = 0;

    if ((num + fd_ptr->LOCATION) > handle_ptr->CORE_NANDFLASH.NUM_VIRTUAL_PAGES) 
    {
        num = (_mqx_int) (handle_ptr->CORE_NANDFLASH.NUM_VIRTUAL_PAGES - fd_ptr->LOCATION);
    } /* Endif */

    lastSector = fd_ptr->LOCATION + num;

    for (count1 = fd_ptr->LOCATION; count1 < lastSector; count1++ )
    {
        /* The fd-ptr->LOCATION is updated internally  */
        status = DriveReadSector(DRIVE_TAG_DATA, count1, (SECTOR_BUFFER *)data_ptr + i * handle_ptr->CORE_NANDFLASH.VIRTUAL_PAGE_SIZE);
        i++;
        if (SUCCESS != status)
        {
            /* Error occured */
            WL_LOG(WL_MODULE_NANDWL, WL_LOG_ERROR, "Read sector %d failed, retcode  0X%0X, error %s", 
            count1, status, wl_err_to_str(status, kNandDontCare));
            
            fd_ptr->ERROR = IO_ERROR_READ;
            /*fd_ptr->ERROR = status; */
            if (lastSector - count1 == num )
            {
                /* Read first sector err -> must return IO_ERROR instead of num read sector (0) */
                num = IO_ERROR;
            }
            else
            {
                num = count1 - num;
                
            }
            
            break;
        }
    }

    return num;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_wl_read_raw
* Returned Value   : number of PHYSICAL pages read
* Comments         : Reads data from NAND flash memory device
*                       FOR DEBUG PURPOSE ONLY
*END*----------------------------------------------------------------------*/

_mqx_int _io_nandflash_wl_internal_read_raw
(
    /* [IN] the file handle for the device */
    MQX_FILE_PTR fd_ptr,

    /* [IN] where the data is to be stored */
    char *   data_ptr,

    /* [OUT] the error is returned */
    uint32_t * ret_err
)
{ /* Body */
    IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
    IO_NANDFLASH_WL_STRUCT_PTR handle_ptr  = 
        (IO_NANDFLASH_WL_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
    uint32_t error;
    _mqx_int num = 1; /* Read physical data on ONE page */
    uint32_t curr_physical_page_location;

    fd_ptr->ERROR = IO_OK;
    if (NULL == handle_ptr->PHY_PAGE_READ_RAW)
    {
        fd_ptr->ERROR = IO_ERROR_READ;
        return IO_ERROR;
    } /* Endif */

    /* Convert to PHYSICAL page location */
    curr_physical_page_location = fd_ptr->LOCATION ;

    if ( (num + curr_physical_page_location) > handle_ptr->CORE_NANDFLASH.NUM_VIRTUAL_PAGES/handle_ptr->CORE_NANDFLASH.PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO  ) 
    {
        fd_ptr->FLAGS |= IO_FLAG_AT_EOF;
        num = (_mqx_int)(handle_ptr->CORE_NANDFLASH.NUM_VIRTUAL_PAGES/handle_ptr->CORE_NANDFLASH.PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO  - fd_ptr->LOCATION);
    } /* Endif */


    _lwsem_wait(&handle_ptr->CORE_NANDFLASH.LWSEM);

    error = (*handle_ptr->PHY_PAGE_READ_RAW)(handle_ptr, (unsigned char *)data_ptr, curr_physical_page_location, num);

    if (NULL != ret_err)
    {
        *ret_err = error;
    }
    if (error == NANDFLASHERR_NO_ERROR)
    {
        fd_ptr->LOCATION += num;
    }

    else
    {
        fd_ptr->ERROR = IO_ERROR_READ;
        num = IO_ERROR;
    } /* Endif */

    _lwsem_post(&handle_ptr->CORE_NANDFLASH.LWSEM);

    return num;
    
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_wl_write
* Returned Value   : number of pages written
* Comments         : Writes data to the NAND flash memory device
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_nandflash_wl_write
(
    /* [IN] the file handle for the device */
    MQX_FILE_PTR fd_ptr,

    /* [IN] where the data is stored */
    char *   data_ptr,

    /* [IN] the number of pages to output */
    _mqx_int   num
)
{ /* Body */
    IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
    IO_NANDFLASH_WL_STRUCT_PTR handle_ptr  = 
    (IO_NANDFLASH_WL_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;   

    _mqx_int status;
    _mqx_int count1,  lastSector;
    uint32_t i = 0;

    if ((num + fd_ptr->LOCATION) > handle_ptr->CORE_NANDFLASH.NUM_VIRTUAL_PAGES) 
    {
        num = (_mqx_int) (handle_ptr->CORE_NANDFLASH.NUM_VIRTUAL_PAGES - fd_ptr->LOCATION);
    } /* Endif */
    
    lastSector = fd_ptr->LOCATION + num;

    for (count1 = fd_ptr->LOCATION; count1 < lastSector ; count1++) 
    {
        /* The fd-ptr->LOCATION is updated internally  */
        status = DriveWriteSector(DRIVE_TAG_DATA, count1, (SECTOR_BUFFER *)data_ptr + i * handle_ptr->CORE_NANDFLASH.VIRTUAL_PAGE_SIZE);
        i++;
        if (SUCCESS != status)
        {
            /* Error occured */
            WL_LOG(WL_MODULE_NANDWL, WL_LOG_ERROR, "Write sector %d failed, retcode  0X%0X, error %s", 
            count1, status, wl_err_to_str(status, kNandDontCare));
            fd_ptr->ERROR = IO_ERROR_WRITE;
            if (lastSector - count1 == num )
            {
                /* Read first sector err -> must return IO_ERROR instead of num read sector (0) */
                num = IO_ERROR;
            }
            else
            {
                num = count1 - num;
            }
            break;
        }
    }

    return num;
    
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_wl_write_raw
* Returned Value   : number of PHYSICAL pages written
* Comments         : Writes data to the NAND flash memory device
*                       for DEBUG purpose only
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_nandflash_wl_internal_write_raw
(
    /* [IN] the file handle for the device */
    MQX_FILE_PTR fd_ptr,

    /* [IN] where the data is stored */
    char *   data_ptr,

    /* [IN] the number of pages to output */
    uint32_t * ret_err
)
{ /* Body */
    IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
    IO_NANDFLASH_WL_STRUCT_PTR handle_ptr  = 
    (IO_NANDFLASH_WL_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
    _mqx_int num = 1; /* Write data on ONE page */
    uint32_t curr_physical_page_location;
    uint32_t error;

    fd_ptr->ERROR = IO_OK;
    if (NULL == handle_ptr->PHY_PAGE_WRITE_RAW)
    {
        fd_ptr->ERROR = IO_ERROR_WRITE;
        return IO_ERROR;
    } /* Endif */

    if (fd_ptr->FLAGS & IO_O_RDONLY)
    {
        fd_ptr->ERROR = IO_ERROR_WRITE_PROTECTED;
        return IO_ERROR;
    } /* Endif */

    curr_physical_page_location = fd_ptr->LOCATION ;

    if ( (num + curr_physical_page_location) > handle_ptr->CORE_NANDFLASH.NUM_VIRTUAL_PAGES/handle_ptr->CORE_NANDFLASH.PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO ) 
    {
        fd_ptr->FLAGS |= IO_FLAG_AT_EOF;
        num = (_mqx_int)(handle_ptr->CORE_NANDFLASH.NUM_VIRTUAL_PAGES/handle_ptr->CORE_NANDFLASH.PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO - fd_ptr->LOCATION);
    } /* Endif */


    _lwsem_wait(&handle_ptr->CORE_NANDFLASH.LWSEM);

    error = ((*handle_ptr->PHY_PAGE_WRITE_RAW)(handle_ptr, (unsigned char *)data_ptr, curr_physical_page_location, num));
    if (NULL != ret_err)
    {
        *ret_err = error;
    }
    if (error == NANDFLASHERR_NO_ERROR)
    {
        fd_ptr->LOCATION += num;
    }
    else
    {
        fd_ptr->ERROR = IO_ERROR_WRITE;
        num = IO_ERROR;
    } /* Endif */

    _lwsem_post(&handle_ptr->CORE_NANDFLASH.LWSEM);

    return num;
    
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_wl_write
* Returned Value   : number of pages written
* Comments         : Writes data to the NAND flash memory device
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_nandflash_wl_internal_write_with_metadata
(
    /* [IN] the file handle for the device */
    MQX_FILE_PTR fd_ptr,

    /* [IN] where the data is stored */
    char *   page_buff_struct,

    /* [IN] the number of pages to output */
    _mqx_int   num,
    
    /* [OUT] Error is returned */
    uint32_t * ret_err
)
{ /* Body */
    IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
    IO_NANDFLASH_WL_STRUCT_PTR handle_ptr  = 
        (IO_NANDFLASH_WL_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
    unsigned char * data_ptr = ((NANDFLASH_PAGE_BUFF_STRUCT_PTR)page_buff_struct)->data;
    unsigned char * metadata_ptr = ((NANDFLASH_PAGE_BUFF_STRUCT_PTR)page_buff_struct)->metadata;   
    uint32_t error;

    fd_ptr->ERROR = IO_OK;
    if (NULL == handle_ptr->PAGE_WRITE_WITH_METADATA)
    {
        fd_ptr->ERROR = IO_ERROR_WRITE;
        return IO_ERROR;
    } /* Endif */

    if (fd_ptr->FLAGS & IO_O_RDONLY)
    {
        fd_ptr->ERROR = IO_ERROR_WRITE_PROTECTED;
        return IO_ERROR;
    } /* Endif */

    if ( (num + fd_ptr->LOCATION) > handle_ptr->CORE_NANDFLASH.NUM_VIRTUAL_PAGES ) 
    {
        fd_ptr->FLAGS |= IO_FLAG_AT_EOF;
        num = (_mqx_int)(handle_ptr->CORE_NANDFLASH.NUM_VIRTUAL_PAGES - fd_ptr->LOCATION);
    } /* Endif */


    _lwsem_wait(&handle_ptr->CORE_NANDFLASH.LWSEM);
    error =  ((*handle_ptr->PAGE_WRITE_WITH_METADATA)(handle_ptr, 
    (unsigned char *)data_ptr, (unsigned char *)metadata_ptr, 
    fd_ptr->LOCATION, num)) ;
    if (NULL != ret_err )
    {
        *ret_err = error;
    }

    if ( error == NANDFLASHERR_NO_ERROR)
    {
        fd_ptr->LOCATION += num;
    }
    else
    {
        /* In case this flash is set to EOF before, read failed leads to NOT EOF yet */
        fd_ptr->FLAGS &= ~IO_FLAG_AT_EOF;
        fd_ptr->ERROR = IO_ERROR_WRITE;
        num = IO_ERROR;
    } /* Endif */

    _lwsem_post(&handle_ptr->CORE_NANDFLASH.LWSEM);

    return num;
    
} /* Endbody */


/*FUNCTION*****************************************************************
* 
* Function Name    : _io_nandflash_wl_ioctl
* Returned Value   : int32_t
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/

_mqx_int _io_nandflash_wl_ioctl
(
    /* [IN] the file handle for the device */
    MQX_FILE_PTR fd_ptr,

    /* [IN] the ioctl command */
    _mqx_uint  cmd,

    /* [IN/OUT] the ioctl parameters */
    void    *param_ptr
)
{ /* Body */
    IO_DEVICE_STRUCT_PTR    io_dev_ptr = fd_ptr->DEV_PTR;
    IO_NANDFLASH_WL_STRUCT_PTR handle_ptr = 
    (IO_NANDFLASH_WL_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
    _mqx_int                result = MQX_OK;
    _mqx_uint               i;
    _mqx_uint_ptr           uparam_ptr;
    bool                 isMediaInit = FALSE;
    uint64_t                 num_sectors = 0;
    RtStatus_t              status;
    uint32_t                 ret_err;
    uint8_t                  *temp;

    switch (cmd) 
    {
        
        /* In order to compatible with MFS, nandflash_wl should return number 
        * sector in *Data region* only 
        */
        case IO_IOCTL_GET_NUM_SECTORS:
            /* Check whether the Media and DataDrive are initialed already? */
            
            status = MediaGetInfo(kInternalNandMedia, kMediaInfoIsInitialized,  &isMediaInit);
            if ((SUCCESS != status) || !isMediaInit)
            {
                return IO_ERROR;
            }
            
            status = DriveGetInfo(DRIVE_TAG_DATA, kDriveInfoSizeInSectors, &num_sectors);
            if (SUCCESS != status) 
            {
                return IO_ERROR;
            }

            uparam_ptr  = (_mqx_uint_ptr)param_ptr;
            
            /* DriveGetInfo return number of sector in uint64_t, therefore we 
            ** should convert to uint32_t
            */
            *uparam_ptr = (uint32_t)(num_sectors & 0xFFFFFFFF);
            
            break;
            
        case NANDFLASH_IOCTL_GET_PHY_PAGE_SIZE:
            uparam_ptr = (_mqx_uint_ptr)param_ptr;
            *uparam_ptr = handle_ptr->CORE_NANDFLASH.NANDFLASH_INFO_PTR->PHY_PAGE_SIZE;
            break;

        case NANDFLASH_IOCTL_GET_SPARE_AREA_SIZE:
            uparam_ptr  = (_mqx_uint_ptr)param_ptr;
            *uparam_ptr = handle_ptr->CORE_NANDFLASH.NANDFLASH_INFO_PTR->SPARE_AREA_SIZE ;
            break;
            
        case NANDFLASH_IOCTL_GET_PAGE_METADATA_AREA_SIZE:
            uparam_ptr  = (_mqx_uint_ptr)param_ptr;
            *uparam_ptr = handle_ptr->CORE_NANDFLASH.NANDFLASH_INFO_PTR->SPARE_AREA_SIZE / 
                handle_ptr->CORE_NANDFLASH.PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO
            - NANDFLASH_ECC_SIZE_TO_NUM_BYTES_CONV(handle_ptr->CORE_NANDFLASH.ECC_SIZE);
            break;

        case NANDFLASH_IOCTL_GET_BLOCK_SIZE:
            uparam_ptr  = (_mqx_uint_ptr)param_ptr;
            *uparam_ptr = handle_ptr->CORE_NANDFLASH.NANDFLASH_INFO_PTR->BLOCK_SIZE;
            break;

        case NANDFLASH_IOCTL_GET_NUM_BLOCKS:
            uparam_ptr  = (_mqx_uint_ptr)param_ptr;
            *uparam_ptr = handle_ptr->CORE_NANDFLASH.NANDFLASH_INFO_PTR->NUM_BLOCKS;
            break;

        case NANDFLASH_IOCTL_GET_WIDTH:
            uparam_ptr  = (_mqx_uint_ptr)param_ptr;
            *uparam_ptr = handle_ptr->CORE_NANDFLASH.NANDFLASH_INFO_PTR->WIDTH;
            break;

        case NANDFLASH_IOCTL_GET_NUM_VIRT_PAGES:
            uparam_ptr  = (_mqx_uint_ptr)param_ptr;
            *uparam_ptr = handle_ptr->CORE_NANDFLASH.NUM_VIRTUAL_PAGES;
            break;

        /* IO_IOCTL_GET_BLOCK_SIZE uses for MFS compatible */
        case IO_IOCTL_GET_BLOCK_SIZE:
        case NANDFLASH_IOCTL_GET_VIRT_PAGE_SIZE:
            uparam_ptr  = (_mqx_uint_ptr)param_ptr;
            *uparam_ptr = handle_ptr->CORE_NANDFLASH.VIRTUAL_PAGE_SIZE;
            break;

        case NANDFLASH_IOCTL_ERASE_BLOCK:
            if (handle_ptr->CORE_NANDFLASH.BLOCK_ERASE) 
            {
                if (NANDFLASHERR_NO_ERROR == (*handle_ptr->CORE_NANDFLASH.BLOCK_ERASE)((IO_NANDFLASH_STRUCT_PTR)&handle_ptr->CORE_NANDFLASH, (uint32_t)param_ptr,FALSE)) 
                {
                    result = MQX_OK;
                } 
                else 
                {
                    result = IO_ERROR;
                } /* Endif */
            } 
            else 
            {
                result = IO_ERROR_INVALID_IOCTL_CMD;
            } /* Endif */   
            break;

        case NANDFLASH_IOCTL_ERASE_BLOCK_FORCE:
            if (handle_ptr->CORE_NANDFLASH.BLOCK_ERASE) 
            {
                if (NANDFLASHERR_NO_ERROR == (*handle_ptr->CORE_NANDFLASH.BLOCK_ERASE)((IO_NANDFLASH_STRUCT_PTR)&handle_ptr->CORE_NANDFLASH, (uint32_t)param_ptr,TRUE)) 
                {
                    result = MQX_OK;
                } 
                else 
                {
                    result = IO_ERROR;
                } /* Endif */
            } 
            else 
            {
                result = IO_ERROR_INVALID_IOCTL_CMD;
            } /* Endif */   
            break;

        case NANDFLASH_IOCTL_ERASE_CHIP:
            if (handle_ptr->CORE_NANDFLASH.CHIP_ERASE) 
            {
                if (NANDFLASHERR_NO_ERROR == (*handle_ptr->CORE_NANDFLASH.CHIP_ERASE)((IO_NANDFLASH_STRUCT_PTR)&handle_ptr->CORE_NANDFLASH)) 
                {
                    result = MQX_OK;
                } 
                else 
                {
                    result = IO_ERROR;
                } /* Endif */
            } 
            else 
            {
                result = IO_ERROR_INVALID_IOCTL_CMD;
            } /* Endif */   
            break;

        case NANDFLASH_IOCTL_WRITE_PROTECT:
            if (handle_ptr->CORE_NANDFLASH.WRITE_PROTECT) {
                if (NANDFLASHERR_NO_ERROR == (*handle_ptr->CORE_NANDFLASH.WRITE_PROTECT)((IO_NANDFLASH_STRUCT_PTR)&handle_ptr->CORE_NANDFLASH, *(_mqx_uint_ptr)param_ptr)) 
                {
                    result = MQX_OK;
                } 
                else 
                {
                    result = IO_ERROR;
                } /* Endif */
            } 
            else 
            {
                result = IO_ERROR_INVALID_IOCTL_CMD;
            } /* Endif */   
            break;

        case NANDFLASH_IOCTL_CHECK_BLOCK:
            if (handle_ptr->CORE_NANDFLASH.IS_BLOCK_BAD) 
            {
                result = (*handle_ptr->CORE_NANDFLASH.IS_BLOCK_BAD)((IO_NANDFLASH_STRUCT_PTR)&handle_ptr->CORE_NANDFLASH, (uint32_t)param_ptr);
            } 
            else 
            {
                result = IO_ERROR_INVALID_IOCTL_CMD;
            } /* Endif */   
            break;

        case NANDFLASH_IOCTL_MARK_BLOCK_AS_BAD:
            if (handle_ptr->CORE_NANDFLASH.MARK_BLOCK_AS_BAD) 
            {
                if (NANDFLASHERR_NO_ERROR == (*handle_ptr->CORE_NANDFLASH.MARK_BLOCK_AS_BAD)((IO_NANDFLASH_STRUCT_PTR)&handle_ptr->CORE_NANDFLASH, (uint32_t)param_ptr)) 
                {
                    result = MQX_OK;
                } 
                else 
                {
                    result = IO_ERROR;
                } /* Endif */
            } 
            else 
            {
                result = IO_ERROR_INVALID_IOCTL_CMD;
            } /* Endif */   
            break;

        case NANDFLASH_IOCTL_GET_BAD_BLOCK_TABLE:
            for (i=0; i<handle_ptr->CORE_NANDFLASH.NANDFLASH_INFO_PTR->NUM_BLOCKS; i++)
            {
                if(NANDFLASHERR_BLOCK_NOT_BAD == (*handle_ptr->CORE_NANDFLASH.IS_BLOCK_BAD)((IO_NANDFLASH_STRUCT_PTR)&handle_ptr->CORE_NANDFLASH, (uint32_t)i))
                {
                    *(uint8_t*)param_ptr = 1;    /* block is good */
                }
                else
                {
                    *(uint8_t*)param_ptr = 0;    /* block is bad */
                }
                // TODO: IAR Error -> follow IAR instruction
                // (uint8_t*)param_ptr += sizeof(uint8_t);
                temp = (uint8_t *)param_ptr;
                temp += sizeof(uint8_t);
                param_ptr = temp;
            }
            break;

        case NANDFLASH_IOCTL_GET_ID:
            if (handle_ptr->CORE_NANDFLASH.IOCTL) 
            {
                if (NANDFLASHERR_NO_ERROR == (*handle_ptr->CORE_NANDFLASH.IOCTL)((IO_NANDFLASH_STRUCT_PTR)&handle_ptr->CORE_NANDFLASH,NANDFLASH_IOCTL_GET_ID,param_ptr)) 
                {
                    result = MQX_OK;
                } 
                else 
                {
                    result = IO_ERROR;
                } /* Endif */
            } 
            else 
            {
                result = IO_ERROR_INVALID_IOCTL_CMD;
            } /* Endif */   
            break;

        case IO_IOCTL_DEVICE_IDENTIFY:
            uparam_ptr  = (uint32_t *)param_ptr;
            uparam_ptr[IO_IOCTL_ID_PHY_ELEMENT]  = IO_DEV_TYPE_PHYS_NANDFLASH;
            uparam_ptr[IO_IOCTL_ID_LOG_ELEMENT]  = IO_DEV_TYPE_LOGICAL_MFS;
            uparam_ptr[IO_IOCTL_ID_ATTR_ELEMENT] = IO_NANDFLASH_ATTRIBS;
            break;
            
        case NANDFLASH_IOCTL_READ_METADATA:
            _io_nandflash_wl_internal_read_metadata(fd_ptr, 
            (char *)param_ptr, &ret_err);
            result = ret_err;
            break;
            
        case NANDFLASH_IOCTL_READ_WITH_METADATA:
            _io_nandflash_wl_internal_read_with_metadata(fd_ptr, 
            (char *)param_ptr, 1, &ret_err);
            result = ret_err;
            break;
            
        case NANDFLASH_IOCTL_WRITE_WITH_METADATA:
            _io_nandflash_wl_internal_write_with_metadata(fd_ptr, 
            (char *)param_ptr, 1, &ret_err) ;
            result = ret_err;
            break;
            
        case NANDFLASH_IOCTL_WRITE_RAW:
            _io_nandflash_wl_internal_write_raw(fd_ptr, 
            (char *)param_ptr, &ret_err);
            result = ret_err;
            break;
            
        case NANDFLASH_IOCTL_READ_RAW:
            _io_nandflash_wl_internal_read_raw(fd_ptr, 
            (char *)param_ptr, &ret_err) ;
            result = ret_err;
            break;
    
        case NANDFLASH_IOCTL_REPAIR:
            result = _io_nandflash_wl_internal_repair(fd_ptr, FALSE);
            break;
            
        case NANDFLASH_IOCTL_REPAIR_WITH_BAD_SCAN:
            result = _io_nandflash_wl_internal_repair(fd_ptr, TRUE);
            break;
            
        case NANDFLASH_IOCTL_ERASE:
            result = _io_nandflash_wl_internal_erase(fd_ptr);
            break;
            
        case IO_IOCTL_FLUSH_OUTPUT:
            result = MediaFlushDrives(kInternalNandMedia);
            if (result != SUCCESS) 
            { 
                WL_LOG(WL_MODULE_NANDWL, WL_LOG_ERROR, "\n Media flush failed err: %s\n", wl_err_to_str(status, kNandDontCare));
            }
            break;
            
        default:
            if(handle_ptr->CORE_NANDFLASH.IOCTL != NULL)
            {
                result = (*handle_ptr->CORE_NANDFLASH.IOCTL)((IO_NANDFLASH_STRUCT_PTR)&handle_ptr->CORE_NANDFLASH, cmd, param_ptr);
            } 
            else 
            {
                result = IO_ERROR_INVALID_IOCTL_CMD;    
            }         
            break;
    } /* Endswitch */
    return result;

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_wl_internal_read_with_metadata
* Returned Value   : number of pages read
* Comments         : Reads data from NAND flash memory device
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_nandflash_wl_internal_read_with_metadata
(
    /* [IN] the file handle for the device */
    MQX_FILE_PTR fd_ptr,

    /* [IN] where the data is to be stored */
    char *   page_struct_ptr,

    /* [IN] the number of pages to input */
    _mqx_int   num,
    
    /* [OUT] error is returned */
    uint32_t * ret_err
)
{ /* Body */
    IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
    IO_NANDFLASH_WL_STRUCT_PTR handle_ptr  = 
    (IO_NANDFLASH_WL_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
    unsigned char * data_ptr = ((NANDFLASH_PAGE_BUFF_STRUCT_PTR)page_struct_ptr)->data;
    unsigned char * metadata_ptr = ((NANDFLASH_PAGE_BUFF_STRUCT_PTR)page_struct_ptr)->metadata;

    uint32_t error;

    fd_ptr->ERROR = IO_OK;
    if (NULL == handle_ptr->PAGE_READ_WITH_METADATA)
    {
        fd_ptr->ERROR = IO_ERROR_READ;
        return IO_ERROR;
    } /* Endif */

    if ( (num + fd_ptr->LOCATION) > handle_ptr->CORE_NANDFLASH.NUM_VIRTUAL_PAGES ) 
    {
        fd_ptr->FLAGS |= IO_FLAG_AT_EOF;
        num = (_mqx_int)(handle_ptr->CORE_NANDFLASH.NUM_VIRTUAL_PAGES - fd_ptr->LOCATION);
    } /* Endif */


    _lwsem_wait(&handle_ptr->CORE_NANDFLASH.LWSEM);

    error = (*handle_ptr->PAGE_READ_WITH_METADATA)(handle_ptr, 
    (unsigned char *)data_ptr, (unsigned char *)metadata_ptr, 
    fd_ptr->LOCATION, num);
    
    if (ret_err) 
    {
        *ret_err = error;
    }

    if (error == NANDFLASHERR_NO_ERROR)
    {
        fd_ptr->LOCATION += num;
        
    }
    else if (error == NANDFLASHERR_ECC_CORRECTED)
    {
        fd_ptr->LOCATION += num;
        
    }
    else
    {
        /* In case this flash is set to EOF before, read failed leads to NOT EOF yet */
        fd_ptr->FLAGS &= ~IO_FLAG_AT_EOF;
        fd_ptr->ERROR = IO_ERROR_READ;
        num = IO_ERROR;
        
    } /* Endif */

    _lwsem_post(&handle_ptr->CORE_NANDFLASH.LWSEM);

    return num;
    
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_wl_internal_read_metadata
* Returned Value   : number of PHYSICAL pages read
* Comments         : Reads data from NAND flash memory device
*                       FOR DEBUG PURPOSE ONLY
*END*----------------------------------------------------------------------*/

_mqx_int _io_nandflash_wl_internal_read_metadata
(
    /* [IN] the file handle for the device */
    MQX_FILE_PTR fd_ptr,

    /* [IN] where the data is to be stored */
    char *   data_ptr,

    /* [OUT] error is returned */
    uint32_t * ret_err
)
{ /* Body */
    IO_DEVICE_STRUCT_PTR io_dev_ptr = fd_ptr->DEV_PTR;
    IO_NANDFLASH_WL_STRUCT_PTR handle_ptr  = 
    (IO_NANDFLASH_WL_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
    uint32_t error;
    _mqx_int num = 1; /* Read metadata on ONE page */

    fd_ptr->ERROR = IO_OK;
    if (NULL == handle_ptr->PAGE_READ_METADATA)
    {
        printf("handle_ptr->PAGE_READ_METADATA == NULL");
        fd_ptr->ERROR = IO_ERROR_READ;
        return IO_ERROR;
    } /* Endif */

    if ( (num + fd_ptr->LOCATION) > handle_ptr->CORE_NANDFLASH.NUM_VIRTUAL_PAGES  ) 
    {
        fd_ptr->FLAGS |= IO_FLAG_AT_EOF;
        num = (_mqx_int)(handle_ptr->CORE_NANDFLASH.NUM_VIRTUAL_PAGES - fd_ptr->LOCATION);
    } /* Endif */


    _lwsem_wait(&handle_ptr->CORE_NANDFLASH.LWSEM);

    error = (*handle_ptr->PAGE_READ_METADATA)(handle_ptr, (unsigned char *)data_ptr, fd_ptr->LOCATION, num);
    if (NULL != ret_err )
    {
        *ret_err = error;
    }

    if (error == NANDFLASHERR_NO_ERROR)
    {
        
        fd_ptr->LOCATION += num;
    }

    else
    {
        /* In case this flash is set to EOF before, read failed leads to NOT EOF yet */
        fd_ptr->FLAGS &= ~IO_FLAG_AT_EOF; 
        fd_ptr->ERROR = IO_ERROR_READ;
        num = IO_ERROR;
    } /* Endif */

    _lwsem_post(&handle_ptr->CORE_NANDFLASH.LWSEM);

    return num;
    
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_wl_repair
* Returned Value   : 
* Comments         : repair the WL data when WL module initialized failed
*END*----------------------------------------------------------------------*/
_mqx_int _io_nandflash_wl_internal_repair(MQX_FILE_PTR fd_ptr, bool isBadScan) {
    _mqx_int status;

    status = DriveRepair(DRIVE_TAG_DATA, 0 /* Currently unsused */, isBadScan);
    
    return status;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name     : _io_nandflash_wl_erase
* Returned Value    : 
* Comments          : erase all data region, this function must be called when
                        wl initialized successfully 
*END*----------------------------------------------------------------------*/
_mqx_int _io_nandflash_wl_internal_erase(MQX_FILE_PTR fd_ptr) {
    _mqx_int status;
    
    status = DriveErase(DRIVE_TAG_DATA, 0 /* Currently unsused */);
    
    return status;
} /* Endbody */

/* EOF */
