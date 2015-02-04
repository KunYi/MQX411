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
*   The file contains DCU4 driver functions.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <io_prv.h>
#include <fio_prv.h>
#include <string.h>
#include <dcu4.h>
#include <dcu4_prv.h>

static uint8_t _dcu4_int_status_bit[] =
{
    /* !!! Make sure it's aligned to DCU4_EVENT_TYPE */
    DCU_INT_STATUS_VS_BLANK_SHIFT,
    DCU_INT_STATUS_LS_BF_VS_SHIFT,
    DCU_INT_STATUS_VSYNC_SHIFT,
    DCU_INT_STATUS_PROG_END_SHIFT,
    DCU_INT_STATUS_LYR_TRANS_FINISH_SHIFT,
    DCU_INT_STATUS_DMA_TRANS_FINISH_SHIFT
};

static DCU4_TIMING_STRUCT _dcu4_default_timings[] =
{
    {
        "NEC480x272@75", 75,
        {
            41,    //pulse width
            2,     //back porch
            480,   //resolution
            2,     //front porch
            TRUE   //invert
        },
        {
            2,     //pulse width
            1,     //back porch
            272,   //resolution
            2,     //front porch
            TRUE   //invert
        }
    },
    {
        "800x600@75", 75,
        {
            80,    //pulse width
            160,   //back porch
            800,   //resolution
            16,    //front porch
            FALSE  //invert
        },
        {
            3,     //pulse width
            21,    //back porch
            600,   //resolution
            1,     //front porch
            FALSE  //invert
        }
    },
    {
        "1024x768@60", 60,
        {
            136,   //pulse width
            160,   //back porch
            1024,  //resolution
            24,    //front porch
            TRUE   //invert
        },
        {
            6,     //pulse width
            29,    //back porch
            768,   //resolution
            3,     //front porch
            TRUE   //invert
        }
    }
};

static DCU4_TIMING_STRUCT_PTR _dcu4_get_default_timing(DCU4_INIT_STRUCT_CPTR dcu4_init_ptr)
{
    int32_t i;

    for (i = 0; i < ELEMENTS_OF(_dcu4_default_timings); i++)
    {
        if (_dcu4_default_timings[i].HSYNC.RESOLUTION == dcu4_init_ptr->WIDTH &&
            _dcu4_default_timings[i].VSYNC.RESOLUTION == dcu4_init_ptr->HEIGHT)
        {
            return &_dcu4_default_timings[i];
        }
    }

    return NULL;
}

static void _dcu4_int_isr(void *data)
{
    DCU4_DEVICE_STRUCT_PTR dcu4_device_ptr = data;
    DCU_MemMapPtr dcu4_ptr = _bsp_get_dcu4_base_address(dcu4_device_ptr->CHANNEL);
    uint32_t int_status;
    DCU4_EVENT_HANDLER_STRUCT_PTR handler;

    int_status = dcu4_ptr->INT_STATUS;

    for (handler = dcu4_device_ptr->EVENTS_HEAD; handler; handler = handler->DEVICE_NEXT)
    {
        if (int_status & (1 << _dcu4_int_status_bit[handler->EVENT.TYPE]))
        {
            handler->EVENT.HANDLER(handler->EVENT.DATA);
            dcu4_ptr->INT_STATUS |= (1 << _dcu4_int_status_bit[handler->EVENT.TYPE]);
        }
    }
}

static uint32_t _dcu4_int_init(DCU_MemMapPtr dcu4_ptr,
                               DCU4_DEVICE_STRUCT_PTR dcu4_device_ptr,
                               DCU4_INIT_STRUCT_CPTR dcu4_init_ptr)
{
    dcu4_device_ptr->VECTOR = _bsp_get_dcu4_vector(dcu4_init_ptr->CHANNEL);
    if (dcu4_device_ptr->VECTOR == 0)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    /* mask all interrupts */
    dcu4_ptr->INT_MASK = 0xFCFF5FFF;

    /* save old isr handler and data */
    dcu4_device_ptr->OLD_ISR_DATA = _int_get_isr_data(dcu4_device_ptr->VECTOR);

    dcu4_device_ptr->OLD_ISR = _int_install_isr(dcu4_device_ptr->VECTOR,
                                                _dcu4_int_isr, dcu4_device_ptr);

    return _bsp_int_init(dcu4_device_ptr->VECTOR, dcu4_init_ptr->INT_PRIORITY, 0, TRUE);
}

static void _dcu4_int_deinit(DCU4_DEVICE_STRUCT_PTR dcu4_device_ptr)
{
    _int_install_isr(dcu4_device_ptr->VECTOR, dcu4_device_ptr->OLD_ISR, dcu4_device_ptr->OLD_ISR_DATA);
}

static void _dcu4_activate(DCU_MemMapPtr dcu4_ptr)
{
    /* enable DCU mode */
    dcu4_ptr->DCU_MODE |= DCU_DCU_MODE_RASTER_EN_MASK | DCU_DCU_MODE_DCU_MODE(1);

    /* Initiate manual refresh */
    dcu4_ptr->UPDATE_MODE = DCU_UPDATE_MODE_READREG_MASK;

    /* Wait until the register update is complete */
    while(dcu4_ptr->UPDATE_MODE & DCU_UPDATE_MODE_READREG_MASK);

    /* Initiate automatic update */
    dcu4_ptr->UPDATE_MODE = DCU_UPDATE_MODE_MODE_MASK;
}

static void _dcu4_deactivate(DCU_MemMapPtr dcu4_ptr)
{
    /* disable automatic update */
    dcu4_ptr->UPDATE_MODE = 0;

    /* disable DCU mode */
    dcu4_ptr->DCU_MODE &= ~(DCU_DCU_MODE_RASTER_EN_MASK | DCU_DCU_MODE_DCU_MODE_MASK);
}

static void _dcu4_set_timing(DCU_MemMapPtr dcu4_ptr, DCU4_DEVICE_STRUCT_PTR  dcu4_device_ptr,
                             DCU4_TIMING_STRUCT_PTR timing)
{
    uint32_t htime, vtime;
    uint32_t pixel_clock;
    uint32_t div_ratio;

    _dcu4_deactivate(dcu4_ptr);

    _mem_copy(timing, &dcu4_device_ptr->TIMING, sizeof(*timing));

    /* init size to default */
    dcu4_ptr->DISP_SIZE = DCU_DISP_SIZE_DELTA_X(timing->HSYNC.RESOLUTION / 16) |
                          DCU_DISP_SIZE_DELTA_Y(timing->VSYNC.RESOLUTION);

    /* init horizontal and vertical timing param */
    dcu4_ptr->HSYN_PARA = DCU_HSYN_PARA_FP_H(timing->HSYNC.FRONT_PORCH) |
                          DCU_HSYN_PARA_PW_H(timing->HSYNC.PULSE_WIDTH) |
                          DCU_HSYN_PARA_BP_H(timing->HSYNC.BACK_PORCH);
    dcu4_ptr->VSYN_PARA = DCU_VSYN_PARA_FP_V(timing->VSYNC.FRONT_PORCH) |
                          DCU_VSYN_PARA_PW_V(timing->VSYNC.PULSE_WIDTH) |
                          DCU_VSYN_PARA_BP_V(timing->VSYNC.BACK_PORCH);

    /* calculate the divide ratio */
    htime = timing->HSYNC.RESOLUTION + timing->HSYNC.FRONT_PORCH +
            timing->HSYNC.PULSE_WIDTH + timing->HSYNC.BACK_PORCH;
    vtime = timing->VSYNC.RESOLUTION + timing->VSYNC.FRONT_PORCH +
            timing->VSYNC.PULSE_WIDTH + timing->VSYNC.BACK_PORCH;
    pixel_clock = htime * vtime * timing->REFRESH_RATE;

    /* ceil to integer */
    div_ratio = (dcu4_device_ptr->INIT->CLOCK_SPEED + pixel_clock - 1) / pixel_clock;
    dcu4_ptr->DIV_RATIO = DCU_DIV_RATIO_DIV_RATIO(div_ratio - 1);

    /* Invert signal */
    dcu4_ptr->SYNPOL = (timing->HSYNC.INVERT ? DCU_SYNPOL_INV_HS_MASK : 0) |
                       (timing->VSYNC.INVERT ? DCU_SYNPOL_INV_VS_MASK : 0);

    _dcu4_activate(dcu4_ptr);
}

/*FUNCTION****************************************************************
*
* Function Name    : _dcu4_init
* Returned Value   : MQX error code
* Comments         :
*    DCU4 registers initialization and card detection.
*
*END*********************************************************************/
static uint32_t _dcu4_init
    (
        /* [IN/OUT] Module registry pointer */
        DCU_MemMapPtr dcu4_ptr,

        /* [IN/OUT] Device information */
        DCU4_DEVICE_STRUCT_PTR  dcu4_device_ptr,

        /* [IN] Device initialization data */
        DCU4_INIT_STRUCT_CPTR dcu4_init_ptr
    )
{
    DCU4_TIMING_STRUCT_PTR timing;
    int32_t i, j;
    volatile uint32_t *base = &dcu4_ptr->CTRLDESCL0_0;

    /* init GPIO and clock */
    if (_bsp_dcu4_io_init (dcu4_init_ptr->CHANNEL) == IO_ERROR)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    /* init interrupt */
    if (_dcu4_int_init(dcu4_ptr, dcu4_device_ptr, dcu4_init_ptr) != MQX_OK)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    /* default background black */
    dcu4_ptr->BGND = 0;

    /* clean all the layers */
    for(i = 0; i < DCU4_LAYERS_NUM; i++)
    {
        for (j = 0; j < 9; j++)
        {
            *(base + j) = 0;
        }
        base += 16;
    }

    /* init default mode */
    dcu4_ptr->DCU_MODE = DCU_DCU_MODE_BLEND_ITER(6);

    timing = _dcu4_get_default_timing(dcu4_init_ptr);

    if (timing)
    {
        _dcu4_set_timing(dcu4_ptr, dcu4_device_ptr, timing);
    }

    return MQX_OK;
}

static void _dcu4_int_mask(DCU_MemMapPtr dcu4_ptr, DCU4_EVENT_TYPE type, bool mask)
{
    if (mask)
        dcu4_ptr->INT_MASK |= (1 << _dcu4_int_status_bit[type]);
    else
        dcu4_ptr->INT_MASK &= ~(1 << _dcu4_int_status_bit[type]);
}

static _mqx_int _dcu4_register_event(DCU_MemMapPtr dcu4_ptr,
                                     DCU4_DEVICE_STRUCT_PTR dcu4_device_ptr,
                                     DCU4_INFO_STRUCT_PTR dcu4_info_ptr,
                                     DCU4_EVENT_STRUCT_PTR event)
{
    DCU4_EVENT_HANDLER_STRUCT_PTR event_handler;

    _int_disable();

    /* Find out if event already registered in this driver handle */
    event_handler = dcu4_info_ptr->EVENTS_HEAD;
    while (event_handler && event_handler->EVENT.TYPE != event->TYPE)
    {
        event_handler = event_handler->INFO_NEXT;
    }

    if (NULL == event_handler)
    {
        /* Not found, then we must create new one */
        event_handler = (DCU4_EVENT_HANDLER_STRUCT_PTR) _mem_alloc_system (
                        (_mem_size) sizeof (DCU4_EVENT_HANDLER_STRUCT));
        if (NULL == event_handler)
        {
            _int_enable();
            return MQX_OUT_OF_MEMORY;
        }

        event_handler->INFO_NEXT = dcu4_info_ptr->EVENTS_HEAD;
        dcu4_info_ptr->EVENTS_HEAD = event_handler;

        event_handler->DEVICE_NEXT = dcu4_device_ptr->EVENTS_HEAD;
        dcu4_device_ptr->EVENTS_HEAD = event_handler;
    }

    /* Update the event content */
    _mem_copy(event, &event_handler->EVENT, sizeof(*event));

    /* Clear the int mask for first registered handler */
    if (dcu4_device_ptr->EVENTS_REG_COUNT[event->TYPE]++ == 0)
    {
        _dcu4_int_mask(dcu4_ptr, event->TYPE, FALSE);
    }

    _int_enable();

    return MQX_OK;
}

static void _dcu4_unregister_event(DCU_MemMapPtr dcu4_ptr,
                                   DCU4_DEVICE_STRUCT_PTR dcu4_device_ptr,
                                   DCU4_INFO_STRUCT_PTR dcu4_info_ptr,
                                   DCU4_EVENT_TYPE type)
{
    DCU4_EVENT_HANDLER_STRUCT_PTR event_handler;

    _int_disable();

    /* Find out if event already registered in this driver handle */
    event_handler = dcu4_info_ptr->EVENTS_HEAD;
    while (event_handler && event_handler->EVENT.TYPE != type)
    {
        event_handler = event_handler->INFO_NEXT;
    }

    if (NULL != event_handler)
    {
        DCU4_EVENT_HANDLER_STRUCT_PTR last;
        DCU4_EVENT_HANDLER_STRUCT_PTR curr;

        /* Found, then we remove it from info and device struct */
        last = NULL;
        for (curr = dcu4_info_ptr->EVENTS_HEAD; curr != NULL; curr = curr->INFO_NEXT)
        {
            if (event_handler == curr)
            {
                if (NULL != last)
                    last->INFO_NEXT = curr->INFO_NEXT;
                else
                    dcu4_info_ptr->EVENTS_HEAD = curr->INFO_NEXT;
                break;
            }
            last = curr;
        }

        last = NULL;
        for (curr = dcu4_device_ptr->EVENTS_HEAD; curr != NULL; curr = curr->DEVICE_NEXT)
        {
            if (event_handler == curr)
            {
                if (NULL != last)
                    last->DEVICE_NEXT = curr->DEVICE_NEXT;
                else
                    dcu4_device_ptr->EVENTS_HEAD = curr->DEVICE_NEXT;
                break;
            }
            last = curr;
        }

        /* Mask the int mask for the last unregistered handler */
        if (--dcu4_device_ptr->EVENTS_REG_COUNT[type] == 0)
        {
            _dcu4_int_mask(dcu4_ptr, type, TRUE);
        }

        _mem_free(event_handler);
    }

    _int_enable();
}

static _mqx_int _dcu4_layer_get_region(DCU_MemMapPtr dcu4_ptr, uint32_t layer,
                                       DCU4_LAYER_IOCTL_STRUCT_PTR layer_param)
{
    volatile uint32_t *base = &dcu4_ptr->CTRLDESCL0_0 + layer * 16;
    uint32_t value;

    if (layer > DCU4_LAYERS_NUM)
        return MQX_INVALID_PARAMETER;

    /* CTRLDESCLn_1 */
    value = *base;
    layer_param->DATA.REGION.WIDTH = (value & DCU_CTRLDESCL0_0_WIDTH_MASK) >>
                                      DCU_CTRLDESCL0_0_WIDTH_SHIFT;
    layer_param->DATA.REGION.HEIGHT = (value & DCU_CTRLDESCL0_0_HEIGHT_MASK) >>
                                       DCU_CTRLDESCL0_0_HEIGHT_SHIFT;

    /* CTRLDESCLn_2 */
    value = *(base + 1);
    layer_param->DATA.REGION.X = (value & DCU_CTRLDESCL0_1_POSX_MASK) >>
                                  DCU_CTRLDESCL0_1_POSX_SHIFT;
    layer_param->DATA.REGION.Y = (value & DCU_CTRLDESCL0_1_POSY_MASK) >>
                                  DCU_CTRLDESCL0_1_POSY_SHIFT;

    /* CTRLDESCLn_4 */
    value = *(base + 3);
    layer_param->DATA.REGION.FORMAT = (DCU4_BPP_TYPE)((value & DCU_CTRLDESCL0_3_BPP_MASK) >>
                                                      DCU_CTRLDESCL0_3_BPP_SHIFT);

    return MQX_OK;
}

static _mqx_int _dcu4_layer_set_region(DCU_MemMapPtr dcu4_ptr, uint32_t layer,
                                       DCU4_LAYER_IOCTL_STRUCT_PTR layer_param)
{
    volatile uint32_t *base = &dcu4_ptr->CTRLDESCL0_0 + layer * 16;
    uint32_t value;

    if (layer > DCU4_LAYERS_NUM)
        return MQX_INVALID_PARAMETER;

    /* CTRLDESCLn_1 */
    *base = DCU_CTRLDESCL0_0_WIDTH(layer_param->DATA.REGION.WIDTH) |
            DCU_CTRLDESCL0_0_HEIGHT(layer_param->DATA.REGION.HEIGHT);

    /* CTRLDESCLn_2 */
    *(base + 1) = DCU_CTRLDESCL0_1_POSX(layer_param->DATA.REGION.X) |
                  DCU_CTRLDESCL0_1_POSY(layer_param->DATA.REGION.Y);

    /* CTRLDESCLn_4 */
    value = *(base + 3) & (~DCU_CTRLDESCL0_3_BPP_MASK);
    *(base + 3) = value | DCU_CTRLDESCL0_3_BPP(layer_param->DATA.REGION.FORMAT);

    return MQX_OK;
}

static _mqx_int _dcu4_layer_get_address(DCU_MemMapPtr dcu4_ptr, uint32_t layer,
                                        DCU4_LAYER_IOCTL_STRUCT_PTR layer_param)
{
    volatile uint32_t *base = &dcu4_ptr->CTRLDESCL0_0 + layer * 16;

    if (layer > DCU4_LAYERS_NUM)
        return MQX_INVALID_PARAMETER;

    /* CTRLDESCLn_3 */
    layer_param->DATA.ADDRESS = (void *) *(base + 2);

    return MQX_OK;
}

static _mqx_int _dcu4_layer_set_address(DCU_MemMapPtr dcu4_ptr, uint32_t layer,
                                        DCU4_LAYER_IOCTL_STRUCT_PTR layer_param)
{
    volatile uint32_t *base = &dcu4_ptr->CTRLDESCL0_0 + layer * 16;

    if (layer > DCU4_LAYERS_NUM)
        return MQX_INVALID_PARAMETER;

    /* CTRLDESCLn_3 */
    *(base + 2) = (uint32_t) layer_param->DATA.ADDRESS;

    return MQX_OK;
}

static _mqx_int _dcu4_layer_get_blend(DCU_MemMapPtr dcu4_ptr, uint32_t layer,
                                      DCU4_LAYER_IOCTL_STRUCT_PTR layer_param)
{
    volatile uint32_t *base = &dcu4_ptr->CTRLDESCL0_0 + layer * 16;
    uint32_t value;

    if (layer > DCU4_LAYERS_NUM)
        return MQX_INVALID_PARAMETER;

    /* CTRLDESCLn_4 */
    value = *(base + 3);

    switch (value & DCU_CTRLDESCL0_3_AB_MASK)
    {
        case 1:
            layer_param->DATA.BLEND.TYPE = DCU_BLEND_CHROMA;
            layer_param->DATA.BLEND.ALPHA = (value & DCU_CTRLDESCL0_3_TRANS_MASK) >>
                                             DCU_CTRLDESCL0_3_TRANS_SHIFT;
            /* CTRLDESCLn_5 */
            value = *(base + 4);
            layer_param->DATA.BLEND.CHROMA.MAX_R = (value & DCU_CTRLDESCL0_4_CKMAX_R_MASK) >>
                                                    DCU_CTRLDESCL0_4_CKMAX_R_SHIFT;
            layer_param->DATA.BLEND.CHROMA.MAX_G = (value & DCU_CTRLDESCL0_4_CKMAX_G_MASK) >>
                                                    DCU_CTRLDESCL0_4_CKMAX_G_SHIFT;
            layer_param->DATA.BLEND.CHROMA.MAX_B = (value & DCU_CTRLDESCL0_4_CKMAX_B_MASK) >>
                                                    DCU_CTRLDESCL0_4_CKMAX_B_SHIFT;
            /* CTRLDESCLn_6 */
            value = *(base + 5);
            layer_param->DATA.BLEND.CHROMA.MIN_R = (value & DCU_CTRLDESCL0_5_CKMIN_R_MASK) >>
                                                    DCU_CTRLDESCL0_5_CKMIN_R_SHIFT;
            layer_param->DATA.BLEND.CHROMA.MIN_G = (value & DCU_CTRLDESCL0_5_CKMIN_G_MASK) >>
                                                    DCU_CTRLDESCL0_5_CKMIN_G_SHIFT;
            layer_param->DATA.BLEND.CHROMA.MIN_B = (value & DCU_CTRLDESCL0_5_CKMIN_B_MASK) >>
                                                    DCU_CTRLDESCL0_5_CKMIN_B_SHIFT;
            break;
        case 2:
            layer_param->DATA.BLEND.TYPE = DCU_BLEND_GLOBAL;
            layer_param->DATA.BLEND.ALPHA = (value & DCU_CTRLDESCL0_3_TRANS_MASK) >>
                                             DCU_CTRLDESCL0_3_TRANS_SHIFT;
            break;
        default:
            layer_param->DATA.BLEND.TYPE = DCU_BLEND_NO_ALPHA;
    }

    return MQX_OK;
}

static _mqx_int _dcu4_layer_set_blend(DCU_MemMapPtr dcu4_ptr, uint32_t layer,
                                      DCU4_LAYER_IOCTL_STRUCT_PTR layer_param)
{
    volatile uint32_t *base = &dcu4_ptr->CTRLDESCL0_0 + layer * 16;
    uint32_t value;

    if (layer > DCU4_LAYERS_NUM)
        return MQX_INVALID_PARAMETER;

    switch (layer_param->DATA.BLEND.TYPE)
    {
        case DCU_BLEND_CHROMA:
            /* CTRLDESCLn_5 */
            *(base + 4) = DCU_CTRLDESCL0_4_CKMAX_R(layer_param->DATA.BLEND.CHROMA.MAX_R) |
                          DCU_CTRLDESCL0_4_CKMAX_G(layer_param->DATA.BLEND.CHROMA.MAX_G) |
                          DCU_CTRLDESCL0_4_CKMAX_B(layer_param->DATA.BLEND.CHROMA.MAX_B);
            /* CTRLDESCLn_6 */
            *(base + 5) = DCU_CTRLDESCL0_5_CKMIN_R(layer_param->DATA.BLEND.CHROMA.MIN_R) |
                          DCU_CTRLDESCL0_5_CKMIN_G(layer_param->DATA.BLEND.CHROMA.MIN_G) |
                          DCU_CTRLDESCL0_5_CKMIN_B(layer_param->DATA.BLEND.CHROMA.MIN_B);
            /* CTRLDESCLn_4 */
            value = *(base + 3) & ~(DCU_CTRLDESCL0_3_AB_MASK | DCU_CTRLDESCL0_3_BB_MASK |
                                    DCU_CTRLDESCL0_3_TRANS_MASK);
            *(base + 3) = value | DCU_CTRLDESCL0_3_AB(1) | DCU_CTRLDESCL0_3_BB_MASK |
                          DCU_CTRLDESCL0_3_TRANS(layer_param->DATA.BLEND.ALPHA);
            break;

        case DCU_BLEND_GLOBAL:
            /* CTRLDESCLn_4 */
            value = *(base + 3) & ~(DCU_CTRLDESCL0_3_AB_MASK | DCU_CTRLDESCL0_3_BB_MASK |
                                    DCU_CTRLDESCL0_3_TRANS_MASK);
            *(base + 3) = value | DCU_CTRLDESCL0_3_AB(2) |
                          DCU_CTRLDESCL0_3_TRANS(layer_param->DATA.BLEND.ALPHA);
            break;

        case DCU_BLEND_NO_ALPHA:
            *(base + 3) &= ~(DCU_CTRLDESCL0_3_AB_MASK | DCU_CTRLDESCL0_3_BB_MASK);
            break;

        default:
            return MQX_INVALID_PARAMETER;
    }

    return MQX_OK;
}

static _mqx_int _dcu4_layer_enable(DCU_MemMapPtr dcu4_ptr, uint32_t layer, bool enable)
{
    volatile uint32_t *base = &dcu4_ptr->CTRLDESCL0_0 + layer * 16;

    if (layer > DCU4_LAYERS_NUM)
        return MQX_INVALID_PARAMETER;

    /* CTRLDESCLn_4 */
    if (enable)
        *(base + 3) |= DCU_CTRLDESCL0_3_EN_MASK;
    else
        *(base + 3) &= ~DCU_CTRLDESCL0_3_EN_MASK;

    return MQX_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _dcu4_install
* Returned Value   : MQX error code
* Comments         :
*    Install an DCU4 device.
*
*END*********************************************************************/
_mqx_uint _dcu4_install
    (
        /* [IN] A string that identifies the device for fopen */
        char                 *identifier,

        /* [IN] The I/O init data pointer */
        DCU4_INIT_STRUCT_CPTR dcu4_init_ptr
    )
{
    DCU4_DEVICE_STRUCT_PTR    dcu4_device_ptr;
    _mqx_uint                 result;

    /* Check parameters */
    if ((NULL == identifier) || (NULL == dcu4_init_ptr))
    {
        return MQX_INVALID_PARAMETER;
    }

    /* Create device context */
    dcu4_device_ptr = _mem_alloc_system_zero ((_mem_size) sizeof (DCU4_DEVICE_STRUCT));
    if (NULL == dcu4_device_ptr)
    {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type (dcu4_device_ptr, MEM_TYPE_IO_DCU4_DEVICE_STRUCT);

    /* Install device */
    result = _io_dev_install_ext(identifier,
        _dcu4_open,
        _dcu4_close,
        NULL,
        NULL,
        _dcu4_ioctl,
        _dcu4_uninstall,
        (void *)dcu4_device_ptr);

    /* Context initialization or cleanup */
    if (MQX_OK == result)
    {
        dcu4_device_ptr->INIT = dcu4_init_ptr;
        dcu4_device_ptr->CHANNEL = dcu4_init_ptr->CHANNEL;
        dcu4_device_ptr->COUNT = 0;
    }
    else
    {
        _mem_free (dcu4_device_ptr);
    }

    return result;
}

/*FUNCTION****************************************************************
*
* Function Name    : _dcu4_uninstall
* Returned Value   : MQX error code
* Comments         :
*    Uninstall an DCU4 device.
*
*END**********************************************************************/

_mqx_int _dcu4_uninstall
    (
        /* [IN/OUT] The device to uninstall */
        IO_DEVICE_STRUCT_PTR dev_ptr
    )
{
    DCU4_DEVICE_STRUCT_PTR  dcu4_device_ptr;

    /* Check parameters */
    if (NULL == dev_ptr)
    {
        return IO_DEVICE_DOES_NOT_EXIST;
    }
    dcu4_device_ptr = dev_ptr->DRIVER_INIT_PTR;
    if (NULL == dcu4_device_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    /* Uninstall only when not opened */
    if (dcu4_device_ptr->COUNT)
    {
        return IO_ERROR_DEVICE_BUSY;
    }

    /* Cleanup */
    _mem_free (dcu4_device_ptr);
    dev_ptr->DRIVER_INIT_PTR = NULL;

    return IO_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _dcu4_open
* Returned Value   : MQX error code
* Comments         :
*    This function opens the DCU4 device.
*
*END*********************************************************************/
_mqx_int _dcu4_open
    (
        /* [IN/OUT] DCU4 file descriptor */
        MQX_FILE_PTR     dcu4_fd_ptr,

        /* [IN] The remaining portion of the name of the device */
        char            *open_name_ptr,

        /* [IN] The flags to be used during operation */
        char            *open_flags_ptr
    )
{
    IO_DEVICE_STRUCT_PTR    io_dev_ptr;
    DCU4_INFO_STRUCT_PTR    dcu4_info_ptr;
    DCU4_DEVICE_STRUCT_PTR  dcu4_device_ptr;
    DCU4_INIT_STRUCT_CPTR   dcu4_init_ptr;
    DCU_MemMapPtr           dcu4_ptr;
    _mqx_int                result = MQX_OK;

    /* Check parameters */
    if (NULL == dcu4_fd_ptr)
    {
        return MQX_INVALID_PARAMETER;
    }
    io_dev_ptr = dcu4_fd_ptr->DEV_PTR;
    if (NULL == io_dev_ptr)
    {
        return IO_DEVICE_DOES_NOT_EXIST;
    }
    dcu4_device_ptr = io_dev_ptr->DRIVER_INIT_PTR;
    if (NULL == dcu4_device_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }
    dcu4_init_ptr = dcu4_device_ptr->INIT;
    if (NULL == dcu4_init_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }
    /* Get register base */
    dcu4_ptr = _bsp_get_dcu4_base_address (dcu4_init_ptr->CHANNEL);
    if (NULL == dcu4_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    /* Set device info */
    dcu4_info_ptr = (DCU4_INFO_STRUCT_PTR)_mem_alloc_system_zero((_mem_size) sizeof (DCU4_INFO_STRUCT));
    if (NULL == dcu4_info_ptr)
    {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type (dcu4_info_ptr, MEM_TYPE_IO_DCU4_INFO_STRUCT);
    dcu4_fd_ptr->DEV_DATA_PTR = dcu4_info_ptr;

    _int_disable();
    if (dcu4_device_ptr->COUNT == 0)
    {
        result = _dcu4_init(dcu4_ptr, dcu4_device_ptr, dcu4_init_ptr);
        if (MQX_OK != result)
        {
            _mem_free(dcu4_info_ptr);
            dcu4_fd_ptr->DEV_DATA_PTR = NULL;
        }
        else
            dcu4_device_ptr->COUNT++;
    }
    _int_enable();

    return result;
}

/*FUNCTION****************************************************************
*
* Function Name    : _dcu4_close
* Returned Value   : MQX error code
* Comments         :
*    This function closes opened DCU4 device.
*
*END*********************************************************************/
_mqx_int _dcu4_close
    (
        /* [IN/OUT] Opened file pointer for DCU4 */
        MQX_FILE_PTR        dcu4_fd_ptr
    )
{
    IO_DEVICE_STRUCT_PTR    io_dev_ptr;
    DCU4_DEVICE_STRUCT_PTR  dcu4_device_ptr;
    DCU4_INFO_STRUCT_PTR    dcu4_info_ptr;
    DCU4_INIT_STRUCT_CPTR   dcu4_init_ptr;
    DCU_MemMapPtr           dcu4_ptr;

    /* Check parameters */
    if (NULL == dcu4_fd_ptr)
    {
        return MQX_INVALID_PARAMETER;
    }
    io_dev_ptr = (IO_DEVICE_STRUCT_PTR)dcu4_fd_ptr->DEV_PTR;
    if (NULL == io_dev_ptr)
    {
        return IO_DEVICE_DOES_NOT_EXIST;
    }
    dcu4_device_ptr = io_dev_ptr->DRIVER_INIT_PTR;
    if (NULL == dcu4_device_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }
    dcu4_init_ptr = dcu4_device_ptr->INIT;
    if (NULL == dcu4_init_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }
    dcu4_info_ptr = dcu4_fd_ptr->DEV_DATA_PTR;
    if (NULL == dcu4_info_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    /* Get register base */
    dcu4_ptr = _bsp_get_dcu4_base_address (dcu4_init_ptr->CHANNEL);
    if (NULL == dcu4_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    while (dcu4_info_ptr->EVENTS_HEAD)
    {
        _dcu4_unregister_event(dcu4_ptr, dcu4_device_ptr, dcu4_info_ptr,
                               dcu4_info_ptr->EVENTS_HEAD->EVENT.TYPE);
    }

    _mem_free (dcu4_info_ptr);
    dcu4_fd_ptr->DEV_DATA_PTR = NULL;

    /* Disable DCU4 device */
    _int_disable();
    if (--dcu4_device_ptr->COUNT == 0)
    {
        _dcu4_deactivate(dcu4_ptr);
        _dcu4_int_deinit(dcu4_device_ptr);
    }
    _int_enable();

    return DCU4_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _dcu4_ioctl
* Returned Value   : MQX error code
* Comments         :
*    This function performs miscellaneous services for the DCU4 I/O device.
*
*END*********************************************************************/
_mqx_int _dcu4_ioctl
    (
        /* [IN] Opened file pointer for DCU4 */
        MQX_FILE_PTR        dcu4_fd_ptr,

        /* [IN] The command to perform */
        uint32_t            cmd,

        /* [IN/OUT] Parameters for the command */
        void               *param_ptr
    )
{
    IO_DEVICE_STRUCT_PTR   io_dev_ptr;
    DCU4_INFO_STRUCT_PTR   dcu4_info_ptr;
    DCU4_DEVICE_STRUCT_PTR dcu4_device_ptr;
    DCU4_INIT_STRUCT_CPTR  dcu4_init_ptr;
    DCU_MemMapPtr          dcu4_ptr;
    _mqx_int               result = MQX_OK;

    /* Check parameters */
    if (NULL == dcu4_fd_ptr)
    {
        return MQX_INVALID_PARAMETER;
    }
    io_dev_ptr = (IO_DEVICE_STRUCT_PTR)dcu4_fd_ptr->DEV_PTR;
    if (NULL == io_dev_ptr)
    {
        return IO_DEVICE_DOES_NOT_EXIST;
    }
    dcu4_info_ptr = (DCU4_INFO_STRUCT_PTR)dcu4_fd_ptr->DEV_DATA_PTR;
    if (NULL == dcu4_info_ptr)
    {
        return IO_DEVICE_DOES_NOT_EXIST;
    }
    dcu4_device_ptr = io_dev_ptr->DRIVER_INIT_PTR;
    if (NULL == dcu4_device_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }
    dcu4_init_ptr = dcu4_device_ptr->INIT;
    if (NULL == dcu4_init_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    /* Get register base */
    dcu4_ptr = _bsp_get_dcu4_base_address (dcu4_init_ptr->CHANNEL);
    if (NULL == dcu4_ptr)
    {
        return IO_ERROR_DEVICE_INVALID;
    }

    switch (cmd)
    {
        case IO_IOCTL_DCU4_REGISTER_EVENT:
        {
            DCU4_EVENT_STRUCT_PTR event = (DCU4_EVENT_STRUCT_PTR)param_ptr;

            if (NULL == event)
                return MQX_INVALID_PARAMETER;

            result = _dcu4_register_event(dcu4_ptr, dcu4_device_ptr, dcu4_info_ptr, event);
            break;
        }

        case IO_IOCTL_DCU4_UNREGISTER_EVENT:
        {
            DCU4_EVENT_TYPE type = (DCU4_EVENT_TYPE)(uint32_t)param_ptr;

            if (type >= DCU_EVENT_NUM)
                return MQX_INVALID_PARAMETER;

            _dcu4_unregister_event(dcu4_ptr, dcu4_device_ptr, dcu4_info_ptr, type);
            break;
        }

        case IO_IOCTL_DCU4_GET_TIMING:
        {
            DCU4_TIMING_STRUCT_PTR timing = (DCU4_TIMING_STRUCT_PTR)param_ptr;

            if (NULL == timing)
                return MQX_INVALID_PARAMETER;

            _mem_copy(&dcu4_device_ptr->TIMING, timing, sizeof(*timing));
            break;
        }

        case IO_IOCTL_DCU4_SET_TIMING:
        {
            DCU4_TIMING_STRUCT_PTR timing = (DCU4_TIMING_STRUCT_PTR)param_ptr;

            if (NULL == timing)
                return MQX_INVALID_PARAMETER;

            _dcu4_set_timing(dcu4_ptr, dcu4_device_ptr, timing);
            break;
        }

        case IO_IOCTL_DCU4_LAYER_GET_REGION:
        {
            DCU4_LAYER_IOCTL_STRUCT_PTR layer_param = (DCU4_LAYER_IOCTL_STRUCT_PTR)param_ptr;

            if (NULL == layer_param)
                return MQX_INVALID_PARAMETER;

            result = _dcu4_layer_get_region(dcu4_ptr, layer_param->LAYER, layer_param);
            break;
        }

        case IO_IOCTL_DCU4_LAYER_SET_REGION:
        {
            DCU4_LAYER_IOCTL_STRUCT_PTR layer_param = (DCU4_LAYER_IOCTL_STRUCT_PTR)param_ptr;

            if (NULL == layer_param)
                return MQX_INVALID_PARAMETER;

            result = _dcu4_layer_set_region(dcu4_ptr, layer_param->LAYER, layer_param);
            break;
        }

        case IO_IOCTL_DCU4_LAYER_GET_ADDRESS:
        {
            DCU4_LAYER_IOCTL_STRUCT_PTR layer_param = (DCU4_LAYER_IOCTL_STRUCT_PTR)param_ptr;

            if (NULL == layer_param)
                return MQX_INVALID_PARAMETER;

            result = _dcu4_layer_get_address(dcu4_ptr, layer_param->LAYER, layer_param);
            break;
        }

        case IO_IOCTL_DCU4_LAYER_SET_ADDRESS:
        {
            DCU4_LAYER_IOCTL_STRUCT_PTR layer_param = (DCU4_LAYER_IOCTL_STRUCT_PTR)param_ptr;

            if (NULL == layer_param)
                return MQX_INVALID_PARAMETER;

            result = _dcu4_layer_set_address(dcu4_ptr, layer_param->LAYER, layer_param);
            break;
        }

        case IO_IOCTL_DCU4_LAYER_GET_BLEND:
        {
            DCU4_LAYER_IOCTL_STRUCT_PTR layer_param = (DCU4_LAYER_IOCTL_STRUCT_PTR)param_ptr;

            if (NULL == layer_param)
                return MQX_INVALID_PARAMETER;

            result = _dcu4_layer_get_blend(dcu4_ptr, layer_param->LAYER, layer_param);
            break;
        }

        case IO_IOCTL_DCU4_LAYER_SET_BLEND:
        {
            DCU4_LAYER_IOCTL_STRUCT_PTR layer_param = (DCU4_LAYER_IOCTL_STRUCT_PTR)param_ptr;

            if (NULL == layer_param)
                return MQX_INVALID_PARAMETER;

            result = _dcu4_layer_set_blend(dcu4_ptr, layer_param->LAYER, layer_param);
            break;
        }

        case IO_IOCTL_DCU4_LAYER_ENABLE:
        {
            DCU4_LAYER_IOCTL_STRUCT_PTR layer_param = (DCU4_LAYER_IOCTL_STRUCT_PTR)param_ptr;

            if (NULL == layer_param)
                return MQX_INVALID_PARAMETER;

            result = _dcu4_layer_enable(dcu4_ptr, layer_param->LAYER, layer_param->DATA.ENABLE);
            break;
        }

        default:
            result = IO_ERROR_INVALID_IOCTL_CMD;
    }

    return result;
}
