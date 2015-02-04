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
*   The file contains FBDEV driver functions.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <io_prv.h>
#include <fio_prv.h>
#include <string.h>
#include <dcu4.h>
#include <fbdev.h>
#include <fbdev_prv.h>

static DCU4_BPP_TYPE _fbdev_dcu4_format(FBDEV_COLOR_FORMAT format)
{
    switch(format)
    {
        case FBDEV_COLOR_ARGB8888:
            return DCU_BPP_32;

        case FBDEV_COLOR_RGB565:
            return DCU_BPP_16;

        case FBDEV_COLOR_RGB888:
            return DCU_BPP_24;

        case FBDEV_COLOR_ARGB1555:
            return DCU_BPP_16_ARGB1555;

        case FBDEV_COLOR_ARGB4444:
            return DCU_BPP_16_ARGB4444;

        case FBDEV_COLOR_UYVY:
            return DCU_BPP_UYVY;

        default:
            return DCU_BPP_INVALID;
    }
}

static _mqx_int _fbdev_set_display_index(FBDEV_INFO_STRUCT_PTR fbdev_info_ptr, uint32_t index)
{
    DCU4_LAYER_IOCTL_STRUCT control;
            
    control.LAYER = fbdev_info_ptr->LAYER;
    control.DATA.ADDRESS = fbdev_info_ptr->BUF_INFO.BUFFERS[index];

    return ioctl(fbdev_info_ptr->DISPLAY, IO_IOCTL_DCU4_LAYER_SET_ADDRESS, &control);
}

static _mqx_int _fbdev_set_display_buffers(FBDEV_INFO_STRUCT_PTR fbdev_info_ptr,
                                           FBDEV_BUF_INFO_STRUCT_PTR buf_info)
{
    _mqx_int result;
    DCU4_LAYER_IOCTL_STRUCT control;

    /* Update display region */
    control.LAYER = fbdev_info_ptr->LAYER;
    control.DATA.REGION.X = buf_info->X;
    control.DATA.REGION.Y = buf_info->Y;
    control.DATA.REGION.WIDTH = buf_info->WIDTH;
    control.DATA.REGION.HEIGHT = buf_info->HEIGHT;
    control.DATA.REGION.FORMAT = _fbdev_dcu4_format(buf_info->FORMAT);

    result = ioctl(fbdev_info_ptr->DISPLAY, IO_IOCTL_DCU4_LAYER_SET_REGION, &control);
    if (MQX_OK == result)
    {
        /* When display buffers are updated, the display indices are also reset */
        _int_disable();
        fbdev_info_ptr->READY_DISPLAY_INDEX = 0;
        fbdev_info_ptr->TO_DISPLAY_INDEX = 0;
        fbdev_info_ptr->DISPLAYING_INDEX = 0;
        _int_enable();

        result = _fbdev_set_display_index(fbdev_info_ptr, 0);
    }

    return result;
}

/* Handled in ISR, and immitate the VSYNC point */
static void _fbdev_lyr_trans_finish_handler(void *data)
{
    FBDEV_INFO_STRUCT_PTR fbdev_info_ptr = (FBDEV_INFO_STRUCT_PTR)data;

    if (fbdev_info_ptr->TO_DISPLAY_INDEX != fbdev_info_ptr->READY_DISPLAY_INDEX)
    {
        /* step foward to prepare next frame */
        fbdev_info_ptr->TO_DISPLAY_INDEX = (fbdev_info_ptr->TO_DISPLAY_INDEX + 1) %
                                           fbdev_info_ptr->BUF_INFO.BUF_COUNT;
        _fbdev_set_display_index(fbdev_info_ptr, fbdev_info_ptr->TO_DISPLAY_INDEX);
    }

    /* Signal VSYNC */
    _lwevent_set(&fbdev_info_ptr->VSYNC, 1);
    _lwevent_clear(&fbdev_info_ptr->VSYNC, 1);
}

/* The real point to update the active frame */
static void _fbdev_prog_end_handler(void *data)
{
    FBDEV_INFO_STRUCT_PTR fbdev_info_ptr = (FBDEV_INFO_STRUCT_PTR)data;

    if (fbdev_info_ptr->DISPLAYING_INDEX != fbdev_info_ptr->TO_DISPLAY_INDEX)
    {
        fbdev_info_ptr->DISPLAYING_INDEX = fbdev_info_ptr->TO_DISPLAY_INDEX;
    }
}

static _mqx_int _fbdev_wait_offscreen(FBDEV_INFO_STRUCT_PTR fbdev_info_ptr, uint32_t index)
{
    _mqx_int result = MQX_OK;

    _int_disable();

    if (fbdev_info_ptr->DISPLAYING_INDEX != index)
    {
        /* The buffer is already offscreen */
        goto done;
    }

    if (fbdev_info_ptr->DISPLAYING_INDEX == fbdev_info_ptr->READY_DISPLAY_INDEX)
    {
        /* We cannot wait the buffer to be offscreen since no other buffer ready for flipping */
        result = IO_ERROR_DEVICE_BUSY;
        goto done;
    }

    _int_enable();

    /* Wait for VSYNC to assure the displaying buffer is swapped */
    do {
        result = _lwevent_wait_for(&fbdev_info_ptr->VSYNC, 1, TRUE, NULL);
    } while (result == MQX_OK && fbdev_info_ptr->DISPLAYING_INDEX == index);

    return result;

done:
    _int_enable();
    return result;
}

static _mqx_int _fbdev_pan_display(FBDEV_INFO_STRUCT_PTR fbdev_info_ptr, uint32_t index)
{
    _mqx_int result = MQX_OK;
    bool index_set = FALSE;

    _int_disable();
    if (index != fbdev_info_ptr->READY_DISPLAY_INDEX)
    {
        /* If the next frame required, then set the param immediately */
        if (index == (fbdev_info_ptr->DISPLAYING_INDEX + 1) % fbdev_info_ptr->BUF_INFO.BUF_COUNT)
        {
            result = _fbdev_set_display_index(fbdev_info_ptr, index);
            index_set = TRUE;
        }
        else
            fbdev_info_ptr->READY_DISPLAY_INDEX = index;
    }
    _int_enable();

    /*
      We update INDICES later here is to avoid following sequence:

      vsync         vsync
        |       #     |
              A B C D
      A: int disable
      B: prog end event
      C: DCU address updated
      D: int enable and prog end handler update the DISPLAYING_INDEX

      The key idea here is that we prefer waiting another frame if we cannot
      ensure the new frame really affects.
    */

    if (index_set)
    {
        _int_disable();
        fbdev_info_ptr->READY_DISPLAY_INDEX = index;
        fbdev_info_ptr->TO_DISPLAY_INDEX = index;
        _int_enable();
    }

    return result;
}

static _mqx_int _fbdev_enable(FBDEV_INFO_STRUCT_PTR fbdev_info_ptr, bool enable)
{
    DCU4_LAYER_IOCTL_STRUCT control;

    control.LAYER = fbdev_info_ptr->LAYER;
    control.DATA.ENABLE = enable;

    return ioctl(fbdev_info_ptr->DISPLAY, IO_IOCTL_DCU4_LAYER_ENABLE, &control);
}

/*FUNCTION****************************************************************
*
* Function Name    : _fbdev_init
* Returned Value   : MQX error code
* Comments         :
*    FBDEV initialization.
*
*END*********************************************************************/
static _mqx_int _fbdev_init
    (
        /* [IN/OUT] Device information */
        FBDEV_DEVICE_STRUCT_PTR  fbdev_device_ptr,
        /* [IN] Device initialization data */
        FBDEV_INIT_STRUCT_CPTR   fbdev_init_ptr,
        /* [IN] Device instance data */
        FBDEV_INFO_STRUCT_PTR    fbdev_info_ptr
    )
{
    DCU4_TIMING_STRUCT timing;
    DCU4_EVENT_STRUCT callback;
    uint32_t width, height;
    _mqx_int result;

    fbdev_info_ptr->DISPLAY = _io_fopen(fbdev_init_ptr->DISPLAY_DEVICE, NULL);

    if (NULL == fbdev_info_ptr->DISPLAY)
        return IO_DEVICE_DOES_NOT_EXIST;

    result = _lwevent_create(&fbdev_info_ptr->VSYNC, 0);
    if (MQX_OK != result)
        goto fail2;

    result = ioctl(fbdev_info_ptr->DISPLAY, IO_IOCTL_DCU4_GET_TIMING, &timing);
    if (MQX_OK != result)
        goto fail1;

    width = timing.HSYNC.RESOLUTION;
    height = timing.VSYNC.RESOLUTION;

    /* Fill in the buffer info */
    fbdev_info_ptr->BUF_INFO.BUF_COUNT = fbdev_init_ptr->BUF_COUNT;
    fbdev_info_ptr->BUF_INFO.X = 0;
    fbdev_info_ptr->BUF_INFO.Y = 0;
    fbdev_info_ptr->BUF_INFO.WIDTH = width;
    fbdev_info_ptr->BUF_INFO.HEIGHT = height;
    fbdev_info_ptr->BUF_INFO.FORMAT = fbdev_init_ptr->FORMAT;

    /* Setup display with the buffer info */
    result = _fbdev_set_display_buffers(fbdev_info_ptr, &fbdev_info_ptr->BUF_INFO);
    if (MQX_OK != result)
        goto fail1;

    /* Register callback of layer transfer finish event */
    callback.TYPE = DCU_EVENT_LYR_TRANS_FINISH;
    callback.HANDLER = _fbdev_lyr_trans_finish_handler;
    callback.DATA = fbdev_info_ptr;
    result = ioctl(fbdev_info_ptr->DISPLAY, IO_IOCTL_DCU4_REGISTER_EVENT, &callback);
    if (MQX_OK != result)
        goto fail1;

    /* Register program end callback after layer transfer finish, because DCU callback works
       with LIFO policy, thus we can avoid wrong sequence if two events happen together */
    callback.TYPE = DCU_EVENT_PROG_END;
    callback.HANDLER = _fbdev_prog_end_handler;
    callback.DATA = fbdev_info_ptr;
    result = ioctl(fbdev_info_ptr->DISPLAY, IO_IOCTL_DCU4_REGISTER_EVENT, &callback);
    if (MQX_OK != result)
        goto fail1;

    return result;

fail1:
    _lwevent_destroy(&fbdev_info_ptr->VSYNC);
fail2:
    _io_fclose(fbdev_info_ptr->DISPLAY);
    fbdev_info_ptr->DISPLAY = NULL;
    return result;
}

/*FUNCTION****************************************************************
*
* Function Name    : _fbdev_deinit
* Comments         :
*    FBDEV deinitialization.
*
*END*********************************************************************/
static void _fbdev_deinit
    (
        /* [IN/OUT] Device information */
        FBDEV_DEVICE_STRUCT_PTR  fbdev_device_ptr,
        /* [IN] Device initialization data */
        FBDEV_INIT_STRUCT_CPTR   fbdev_init_ptr,
        /* [IN] Device instance data */
        FBDEV_INFO_STRUCT_PTR    fbdev_info_ptr
    )
{
    ioctl(fbdev_info_ptr->DISPLAY, IO_IOCTL_DCU4_UNREGISTER_EVENT, (void*)DCU_EVENT_PROG_END);
    ioctl(fbdev_info_ptr->DISPLAY, IO_IOCTL_DCU4_UNREGISTER_EVENT, (void*)DCU_EVENT_LYR_TRANS_FINISH);

    _io_fclose(fbdev_info_ptr->DISPLAY);
    fbdev_info_ptr->DISPLAY = NULL;

    _lwevent_destroy(&fbdev_info_ptr->VSYNC);
}

/*FUNCTION****************************************************************
*
* Function Name    : _fbdev_install
* Returned Value   : MQX error code
* Comments         :
*    Install an FBDEV device.
*
*END*********************************************************************/
_mqx_uint _fbdev_install
    (
        /* [IN] A string that identifies the device for fopen */
        char                  *identifier,

        /* [IN] The I/O init data pointer */
        FBDEV_INIT_STRUCT_CPTR fbdev_init_ptr
    )
{
    FBDEV_DEVICE_STRUCT_PTR fbdev_device_ptr;
    FBDEV_INFO_STRUCT_PTR  *fbdev_instances;
    _mqx_uint               result;

    /* Check parameters */
    if ((NULL == identifier) || (NULL == fbdev_init_ptr))
    {
        return MQX_INVALID_PARAMETER;
    }

    /* Create device context */
    fbdev_device_ptr = _mem_alloc_system_zero ((_mem_size) sizeof (FBDEV_DEVICE_STRUCT));
    if (NULL == fbdev_device_ptr)
    {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type (fbdev_device_ptr, MEM_TYPE_IO_FBDEV_DEVICE_STRUCT);

    fbdev_instances = _mem_alloc_system_zero (
                      (_mem_size) sizeof (FBDEV_INFO_STRUCT_PTR) * fbdev_init_ptr->FBDEV_COUNT);
    if (NULL == fbdev_instances)
    {
        _mem_free (fbdev_device_ptr);
        return MQX_OUT_OF_MEMORY;
    }

    result = _lwsem_create(&fbdev_device_ptr->LOCK, 1);
    if (MQX_OK != result)
    {
        _mem_free (fbdev_instances);
        _mem_free (fbdev_device_ptr);
        return result;
    }

    /* Install device */
    result = _io_dev_install_ext(identifier,
        _fbdev_open,
        _fbdev_close,
        NULL,
        NULL,
        _fbdev_ioctl,
        _fbdev_uninstall,
        (void *)fbdev_device_ptr);

    /* Context initialization or cleanup */
    if (MQX_OK == result)
    {
        fbdev_device_ptr->INIT = fbdev_init_ptr;
        fbdev_device_ptr->INSTANCE_OPENED = 0;
        fbdev_device_ptr->INSTANCES = fbdev_instances;
    }
    else
    {
        _lwsem_destroy(&fbdev_device_ptr->LOCK);
        _mem_free (fbdev_instances);
        _mem_free (fbdev_device_ptr);
    }

    return result;
}

/*FUNCTION****************************************************************
*
* Function Name    : _fbdev_uninstall
* Returned Value   : MQX error code
* Comments         :
*    Uninstall an FBDEV device.
*
*END**********************************************************************/

_mqx_int _fbdev_uninstall
    (
        /* [IN/OUT] The device to uninstall */
        IO_DEVICE_STRUCT_PTR dev_ptr
    )
{
    FBDEV_DEVICE_STRUCT_PTR  fbdev_device_ptr;

    /* Check parameters */
    if (NULL == dev_ptr)
    {
        return IO_DEVICE_DOES_NOT_EXIST;
    }
    fbdev_device_ptr = dev_ptr->DRIVER_INIT_PTR;
    if (NULL == fbdev_device_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    /* Uninstall only when not opened */
    if (fbdev_device_ptr->INSTANCE_OPENED)
    {
        return IO_ERROR_DEVICE_BUSY;
    }

    /* Cleanup */
    _lwsem_destroy(&fbdev_device_ptr->LOCK);
    _mem_free (fbdev_device_ptr->INSTANCES);
    _mem_free (fbdev_device_ptr);
    dev_ptr->DRIVER_INIT_PTR = NULL;

    return IO_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _fbdev_open
* Returned Value   : MQX error code
* Comments         :
*    This function opens the FBDEV device.
*
*END*********************************************************************/
_mqx_int _fbdev_open
    (
        /* [IN/OUT] FBDEV file descriptor */
        MQX_FILE_PTR     fbdev_fd_ptr,

        /* [IN] The remaining portion of the name of the device */
        char            *open_name_ptr,

        /* [IN] The flags to be used during operation */
        char            *open_flags_ptr
    )
{
    IO_DEVICE_STRUCT_PTR    io_dev_ptr;
    FBDEV_INFO_STRUCT_PTR   fbdev_info_ptr;
    FBDEV_DEVICE_STRUCT_PTR fbdev_device_ptr;
    FBDEV_INIT_STRUCT_CPTR  fbdev_init_ptr;
    _mqx_int                index;
    _mqx_int                result;

    /* Check parameters */
    if (NULL == fbdev_fd_ptr || NULL == open_name_ptr)
    {
        return MQX_INVALID_PARAMETER;
    }
    io_dev_ptr = fbdev_fd_ptr->DEV_PTR;
    if (NULL == io_dev_ptr)
    {
        return IO_DEVICE_DOES_NOT_EXIST;
    }
    fbdev_device_ptr = io_dev_ptr->DRIVER_INIT_PTR;
    if (NULL == fbdev_device_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }
    fbdev_init_ptr = fbdev_device_ptr->INIT;
    if (NULL == fbdev_init_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    index = _io_atoi(open_name_ptr + strlen(io_dev_ptr->IDENTIFIER));
    if (index < 0 || index >= fbdev_init_ptr->FBDEV_COUNT)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    _lwsem_wait(&fbdev_device_ptr->LOCK);

    if (NULL == fbdev_device_ptr->INSTANCES[index])
    {
        /* Create device instance */
        fbdev_info_ptr = (FBDEV_INFO_STRUCT_PTR)_mem_alloc_system_zero(
                         (_mem_size) sizeof (FBDEV_INFO_STRUCT));
        if (NULL == fbdev_info_ptr)
        {
            _lwsem_post(&fbdev_device_ptr->LOCK);
            return MQX_OUT_OF_MEMORY;
        }
        _mem_set_type (fbdev_info_ptr, MEM_TYPE_IO_FBDEV_INFO_STRUCT);

        result = _fbdev_init(fbdev_device_ptr, fbdev_init_ptr, fbdev_info_ptr);
        if (MQX_OK != result)
        {
            _mem_free(fbdev_info_ptr);
            _lwsem_post(&fbdev_device_ptr->LOCK);
            return result;
        }

        fbdev_info_ptr->LAYER = index;

        fbdev_device_ptr->INSTANCES[index] = fbdev_info_ptr;
        fbdev_device_ptr->INSTANCE_OPENED++;
    }
    else
    {
        /* reuse opened instance */
        fbdev_info_ptr = fbdev_device_ptr->INSTANCES[index];
    }

    fbdev_fd_ptr->DEV_DATA_PTR = fbdev_info_ptr;
    fbdev_info_ptr->COUNT++;

    _lwsem_post(&fbdev_device_ptr->LOCK);

    return MQX_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _fbdev_close
* Returned Value   : MQX error code
* Comments         :
*    This function closes opened FBDEV device.
*
*END*********************************************************************/
_mqx_int _fbdev_close
    (
        /* [IN/OUT] Opened file pointer for FBDEV */
        MQX_FILE_PTR        fbdev_fd_ptr
    )
{
    IO_DEVICE_STRUCT_PTR    io_dev_ptr;
    FBDEV_DEVICE_STRUCT_PTR fbdev_device_ptr;
    FBDEV_INFO_STRUCT_PTR   fbdev_info_ptr;
    FBDEV_INIT_STRUCT_CPTR  fbdev_init_ptr;

    /* Check parameters */
    if (NULL == fbdev_fd_ptr)
    {
        return MQX_INVALID_PARAMETER;
    }
    io_dev_ptr = (IO_DEVICE_STRUCT_PTR)fbdev_fd_ptr->DEV_PTR;
    if (NULL == io_dev_ptr)
    {
        return IO_DEVICE_DOES_NOT_EXIST;
    }
    fbdev_device_ptr = io_dev_ptr->DRIVER_INIT_PTR;
    if (NULL == fbdev_device_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }
    fbdev_init_ptr = fbdev_device_ptr->INIT;
    if (NULL == fbdev_init_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }
    fbdev_info_ptr = fbdev_fd_ptr->DEV_DATA_PTR;
    if (NULL == fbdev_info_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    _lwsem_wait(&fbdev_device_ptr->LOCK);
    if (--fbdev_info_ptr->COUNT == 0)
    {
        fbdev_device_ptr->INSTANCES[fbdev_info_ptr->LAYER] = NULL;
        fbdev_device_ptr->INSTANCE_OPENED--;

        _fbdev_deinit(fbdev_device_ptr, fbdev_init_ptr, fbdev_info_ptr);
        _mem_free(fbdev_info_ptr);
    }
    _lwsem_post(&fbdev_device_ptr->LOCK);

    fbdev_fd_ptr->DEV_DATA_PTR = NULL;

    return FBDEV_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _fbdev_ioctl
* Returned Value   : MQX error code
* Comments         :
*    This function performs miscellaneous services for the FBDEV I/O device.
*
*END*********************************************************************/
_mqx_int _fbdev_ioctl
    (
        /* [IN] Opened file pointer for FBDEV */
        MQX_FILE_PTR        fbdev_fd_ptr,

        /* [IN] The command to perform */
        uint32_t            cmd,

        /* [IN/OUT] Parameters for the command */
        void               *param_ptr
    )
{
    IO_DEVICE_STRUCT_PTR    io_dev_ptr;
    FBDEV_INFO_STRUCT_PTR   fbdev_info_ptr;
    FBDEV_DEVICE_STRUCT_PTR fbdev_device_ptr;
    FBDEV_INIT_STRUCT_CPTR  fbdev_init_ptr;
    _mqx_int                result = MQX_OK;

    /* Check parameters */
    if (NULL == fbdev_fd_ptr)
    {
        return MQX_INVALID_PARAMETER;
    }
    io_dev_ptr = (IO_DEVICE_STRUCT_PTR)fbdev_fd_ptr->DEV_PTR;
    if (NULL == io_dev_ptr)
    {
        return IO_DEVICE_DOES_NOT_EXIST;
    }
    fbdev_info_ptr = (FBDEV_INFO_STRUCT_PTR)fbdev_fd_ptr->DEV_DATA_PTR;
    if (NULL == fbdev_info_ptr)
    {
        return IO_DEVICE_DOES_NOT_EXIST;
    }
    fbdev_device_ptr = io_dev_ptr->DRIVER_INIT_PTR;
    if (NULL == fbdev_device_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }
    fbdev_init_ptr = fbdev_device_ptr->INIT;
    if (NULL == fbdev_init_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    switch (cmd)
    {
        case IO_IOCTL_FBDEV_GET_BUFFER_INFO:
        {
            FBDEV_BUF_INFO_STRUCT_PTR info = (FBDEV_BUF_INFO_STRUCT_PTR)param_ptr;

            if (NULL == info)
                return MQX_INVALID_PARAMETER;

            _mem_copy(&fbdev_info_ptr->BUF_INFO, info, sizeof(*info));
            break;
        }

        case IO_IOCTL_FBDEV_SET_BUFFER_INFO:
        {
            FBDEV_BUF_INFO_STRUCT_PTR info = (FBDEV_BUF_INFO_STRUCT_PTR)param_ptr;

            if (NULL == info || info->BUF_COUNT > FBDEV_MAX_BUFFERS)
                return MQX_INVALID_PARAMETER;

            /* Save buffer info */
            _mem_copy(info, &fbdev_info_ptr->BUF_INFO, sizeof(*info));
            result = _fbdev_set_display_buffers(fbdev_info_ptr, info);
            break;
        }

        case IO_IOCTL_FBDEV_WAIT_OFFSCREEN:
        {
            uint32_t index = (uint32_t)param_ptr;

            if (index >= fbdev_info_ptr->BUF_INFO.BUF_COUNT)
                return MQX_INVALID_PARAMETER;

            result = _fbdev_wait_offscreen(fbdev_info_ptr, index);
            break;
        }

        case IO_IOCTL_FBDEV_WAIT_VSYNC:
        {
            result = _lwevent_wait_for(&fbdev_info_ptr->VSYNC, 1, TRUE, NULL);
            break;
        }

        case IO_IOCTL_FBDEV_PAN_DISPLAY:
        {
            uint32_t index = (uint32_t)param_ptr;

            if (index >= fbdev_info_ptr->BUF_INFO.BUF_COUNT)
                return MQX_INVALID_PARAMETER;

            result = _fbdev_pan_display(fbdev_info_ptr, index);
            break;
        }

        case IO_IOCTL_FBDEV_ENABLE:
        {
            bool enable = (bool)param_ptr;

            result = _fbdev_enable(fbdev_info_ptr, enable);
            break;
        }

        default:
            result = IO_ERROR_INVALID_IOCTL_CMD;
    }

    return result;
}
