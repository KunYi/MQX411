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
*   This file contains the source for a simple example of an
*   application that make use of DCU4 driver.
*
*
*END************************************************************************/


#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <dcu4.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#if ! BSPCFG_ENABLE_DCU4
#error This application requires BSPCFG_ENABLE_DCU4 defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#define LAYER_NUM          (6)

#if BSP_VYBRID_AUTOEVB_A5 /* AutoEVB */
#define LAYER_WIDTH        (480)
#define LAYER_HEIGHT       (272)
#define LAYER_OFFSET       (64)
#else /* Tower board */
#define LAYER_WIDTH        (200)
#define LAYER_HEIGHT       (128)
#define LAYER_OFFSET       (32)
#endif

extern void main_task (uint32_t);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index,   Function,   Stack,  Priority,   Name,   Attributes,          Param, Time Slice */
    { 10L,          main_task,  8192L,  8L,         "Main", MQX_AUTO_START_TASK, 0,     0  },
    { 0 }
};

static void vsync_handler(void *data)
{
    static uint32_t count = 0;
    static TIME_STRUCT prev;
    TIME_STRUCT now, diff;
    float fps; 

    if ((count++ & 0xFF) == 0)
    {
        if (count == 1)
            _time_get(&prev); /* initialize prev time */
        else
        {
            _time_get(&now);
            _time_diff(&prev, &now, &diff);
            fps = (float)0xFF / ((float)diff.SECONDS + (float)diff.MILLISECONDS/1000);
            printf("display refresh rate %d\n", (int)fps);
            _mem_copy(&now, &prev, sizeof(now));
        }
    }
}

static void setup_layer(MQX_FILE_PTR fd, int layer, DCU4_BPP_TYPE format, int bpp)
{
    DCU4_LAYER_IOCTL_STRUCT layer_control;
    DCU4_LAYER_IOCTL_STRUCT copy;
    void *frame_buffer;
    _mqx_int ret;

    _mem_zero(&layer_control, sizeof(layer_control));
    _mem_zero(&copy, sizeof(copy));

    layer_control.LAYER = layer;
    layer_control.DATA.REGION.X = layer * LAYER_OFFSET;
    layer_control.DATA.REGION.Y = layer * LAYER_OFFSET;
    layer_control.DATA.REGION.WIDTH = LAYER_WIDTH;
    layer_control.DATA.REGION.HEIGHT = LAYER_HEIGHT;
    layer_control.DATA.REGION.FORMAT = format;
    ret = ioctl(fd, IO_IOCTL_DCU4_LAYER_SET_REGION, &layer_control);
    if (MQX_OK != ret) {
        printf ("Error set layer %d region, returned: 0x%08x\n", layer, ret);
        return;
    }

    copy.LAYER = layer;
    ret = ioctl(fd, IO_IOCTL_DCU4_LAYER_GET_REGION, &copy);
    if (MQX_OK != ret) {
        printf ("Error get layer %d region, returned: 0x%08x\n", layer, ret);
        return;
    }

    if (memcmp(&layer_control, &copy, sizeof(layer_control)))
    {
        printf ("Error layer %d region comparison\n", layer);
        return;
    }

    frame_buffer = _mem_alloc_system_align(LAYER_WIDTH * LAYER_HEIGHT * bpp, 32);
    if (frame_buffer == NULL) {
        printf ("Allocate frame buffer %d failed: %d\n", layer, LAYER_WIDTH * LAYER_HEIGHT * bpp);
        return;
    }
    memset(frame_buffer, 0x22 * (layer + 1), LAYER_WIDTH * LAYER_HEIGHT * bpp);
    _DCACHE_FLUSH();

    layer_control.DATA.ADDRESS = frame_buffer;
    ret = ioctl(fd, IO_IOCTL_DCU4_LAYER_SET_ADDRESS, &layer_control);
    if (MQX_OK != ret) {
        printf ("Error set layer %d address, returned: 0x%08x\n", layer, ret);
        return;
    }

    ret = ioctl(fd, IO_IOCTL_DCU4_LAYER_GET_ADDRESS, &copy);
    if (MQX_OK != ret) {
        printf ("Error get layer %d address, returned: 0x%08x\n", layer, ret);
        return;
    }

    if (layer_control.DATA.ADDRESS != copy.DATA.ADDRESS)
    {
        printf ("Error layer %d address comparison\n", layer);
        return;
    }

    layer_control.DATA.ENABLE = TRUE;
    ret = ioctl(fd, IO_IOCTL_DCU4_LAYER_ENABLE, &layer_control);
    if (MQX_OK != ret) {
        printf ("Error layer %d enable, returned: 0x%08x\n", layer, ret);
        return;
    }
}

/*TASK*-------------------------------------------------------------------
*
* Task Name : main_task
* Comments  :
*
*END*----------------------------------------------------------------------*/
void main_task
   (
      uint32_t dummy
   )
{
    int i;
    _mqx_int ret;
    MQX_FILE_PTR fd;
    DCU4_EVENT_STRUCT event;
    DCU4_TIMING_STRUCT default_timing;
#if BSP_VYBRID_AUTOEVB_A5
    DCU4_TIMING_STRUCT timing = {
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
    };
#endif
    DCU4_LAYER_IOCTL_STRUCT layer_control;
    DCU4_LAYER_IOCTL_STRUCT copy;

    printf ("\n-------------- DCU4 driver example --------------\n\n");
    printf ("This example application demonstrates usage of DCU4 driver.\n");

    /* Open the DCU4 driver */
    fd = fopen ("dcu0:", NULL);
    if (fd == NULL) {
        printf ("Error opening DCU4 driver!\n");
        goto OnError;
    }

    /* enable layer 0 */
    
    printf("\n\n************************************************************************\n");
    printf("Calculate default fps ....\n");
    printf("************************************************************************\n");

    event.TYPE = DCU_EVENT_VSYNC;
    event.HANDLER = vsync_handler;
    event.DATA = NULL;
    ret = ioctl(fd, IO_IOCTL_DCU4_REGISTER_EVENT, &event);

    if (MQX_OK != ret) {
        printf ("Error register event, returned: 0x%08x\n", ret);
        goto OnError;
    }

    setup_layer(fd, 0, DCU_BPP_32, 4);

    _time_delay(10000);

    printf("\n\n************************************************************************\n");
    printf("Update resolution and timing ....\n");
    printf("************************************************************************\n");

    ret = ioctl(fd, IO_IOCTL_DCU4_GET_TIMING, &default_timing);
    if (MQX_OK != ret) {
        printf ("Error get timing, returned: 0x%08x\n", ret);
        goto OnError;
    }
    printf("Original timing %s: rate %d, HSYNC: %d, %d, %d, %d, %d. VSYNC: %d, %d, %d, %d, %d\n",
           default_timing.name, default_timing.REFRESH_RATE, default_timing.HSYNC.PULSE_WIDTH,
           default_timing.HSYNC.BACK_PORCH, default_timing.HSYNC.RESOLUTION,
           default_timing.HSYNC.FRONT_PORCH, default_timing.HSYNC.INVERT,
           default_timing.VSYNC.PULSE_WIDTH, default_timing.VSYNC.BACK_PORCH,
           default_timing.VSYNC.RESOLUTION, default_timing.VSYNC.FRONT_PORCH,
           default_timing.VSYNC.INVERT);

#if BSP_VYBRID_AUTOEVB_A5
    ret = ioctl(fd, IO_IOCTL_DCU4_SET_TIMING, &timing);
    if (MQX_OK != ret) {
        printf ("Error set timing, returned: 0x%08x\n", ret);
        goto OnError;
    }

    _time_delay(10000);
#endif

    printf("\n\n************************************************************************\n");
    printf("Multiple layers test (chroma key)....\n");
    printf("************************************************************************\n");

    setup_layer(fd, 1, DCU_BPP_16, 2);
    setup_layer(fd, 2, DCU_BPP_24, 3);
    setup_layer(fd, 3, DCU_BPP_16_ARGB1555, 2);
    setup_layer(fd, 4, DCU_BPP_16_ARGB4444, 2);
    setup_layer(fd, 5, DCU_BPP_UYVY, 2);

    for (i = 0; i < LAYER_NUM; i++)
    {
        _mem_zero(&layer_control, sizeof(layer_control));
        _mem_zero(&copy, sizeof(copy));

        layer_control.LAYER = i;
        layer_control.DATA.BLEND.TYPE = DCU_BLEND_CHROMA;
        layer_control.DATA.BLEND.ALPHA = 0;
        layer_control.DATA.BLEND.CHROMA.MAX_R = 255;
        layer_control.DATA.BLEND.CHROMA.MAX_G = 255;
        layer_control.DATA.BLEND.CHROMA.MAX_B = 255;
        layer_control.DATA.BLEND.CHROMA.MIN_R = 0;
        layer_control.DATA.BLEND.CHROMA.MIN_G = 0;
        layer_control.DATA.BLEND.CHROMA.MIN_B = 0;
        ret = ioctl(fd, IO_IOCTL_DCU4_LAYER_SET_BLEND, &layer_control);
        if (MQX_OK != ret) {
            printf ("Error layer %d set blend chroma, returned: 0x%08x\n", i, ret);
            goto OnError;
        }

        copy.LAYER = i;
        ret = ioctl(fd, IO_IOCTL_DCU4_LAYER_GET_BLEND, &copy);
        if (MQX_OK != ret) {
            printf ("Error layer %d get blend chroma, returned: 0x%08x\n", i, ret);
            goto OnError;
        }

        if (memcmp(&layer_control, &copy, sizeof(layer_control)))
        {
            printf ("Error layer %d blend chroma comparison\n", i);
            goto OnError;
        }
    }

    _time_delay(10000);

    printf("\n\n************************************************************************\n");
    printf("Multiple layers test (global alpha)....\n");
    printf("************************************************************************\n");

    for (i = 0; i < LAYER_NUM; i++)
    {
        _mem_zero(&layer_control, sizeof(layer_control));
        _mem_zero(&copy, sizeof(copy));

        layer_control.LAYER = i;
        layer_control.DATA.BLEND.TYPE = DCU_BLEND_GLOBAL;
        layer_control.DATA.BLEND.ALPHA = 128;
        ret = ioctl(fd, IO_IOCTL_DCU4_LAYER_SET_BLEND, &layer_control);
        if (MQX_OK != ret) {
            printf ("Error layer %d set blend global, returned: 0x%08x\n", i, ret);
            goto OnError;
        }

        copy.LAYER = i;
        ret = ioctl(fd, IO_IOCTL_DCU4_LAYER_GET_BLEND, &copy);
        if (MQX_OK != ret) {
            printf ("Error layer %d get blend global, returned: 0x%08x\n", i, ret);
            goto OnError;
        }

        if (memcmp(&layer_control, &copy, sizeof(layer_control)))
        {
            printf ("Error layer %d blend global comparison\n", i);
            goto OnError;
        }
    }

    _time_delay(10000);

    printf("\n\n************************************************************************\n");
    printf("Multiple layers test (no alpha)....\n");
    printf("************************************************************************\n");

    for (i = 0; i < LAYER_NUM; i++)
    {
        _mem_zero(&layer_control, sizeof(layer_control));
        _mem_zero(&copy, sizeof(copy));

        layer_control.LAYER = i;
        layer_control.DATA.BLEND.TYPE = DCU_BLEND_NO_ALPHA;
        ret = ioctl(fd, IO_IOCTL_DCU4_LAYER_SET_BLEND, &layer_control);
        if (MQX_OK != ret) {
            printf ("Error layer %d set blend no alpha, returned: 0x%08x\n", i, ret);
            goto OnError;
        }

        copy.LAYER = i;
        ret = ioctl(fd, IO_IOCTL_DCU4_LAYER_GET_BLEND, &copy);
        if (MQX_OK != ret) {
            printf ("Error layer %d get blend no alpha, returned: 0x%08x\n", i, ret);
            goto OnError;
        }

        if (memcmp(&layer_control, &copy, sizeof(layer_control)))
        {
            printf ("Error layer %d blend no alpha comparison\n", i);
            goto OnError;
        }
    }

    _time_delay(10000);

    printf("\n\n************************************************************************\n");
    printf("DCU4 disable layers....\n");
    printf("************************************************************************\n");

    for (i = 0; i < LAYER_NUM - 1; i++)
    {
        layer_control.LAYER = i;
        layer_control.DATA.ENABLE = FALSE;
        ret = ioctl(fd, IO_IOCTL_DCU4_LAYER_ENABLE, &layer_control);
        if (MQX_OK != ret) {
            printf ("Error disable layer %d, returned: 0x%08x\n", i, ret);
            goto OnError;
        }
    }

    _time_delay(10000);

    printf("\n\n************************************************************************\n");
    printf("DCU4 cleanup....\n");
    printf("************************************************************************\n");

    ret = ioctl(fd, IO_IOCTL_DCU4_UNREGISTER_EVENT, (void*)DCU_EVENT_VSYNC);
    if (MQX_OK != ret) {
        printf ("Error unreigster event, returned: 0x%08x\n", ret);
        goto OnError;
    }

    for (i = 0; i < LAYER_NUM; i++)
    {
        layer_control.LAYER = i;
        ret = ioctl(fd, IO_IOCTL_DCU4_LAYER_GET_ADDRESS, &layer_control);
        if (MQX_OK != ret) {
            printf ("Error get layer %d address, returned: 0x%08x\n", i, ret);
            return;
        }

        _mem_free(layer_control.DATA.ADDRESS);
    }

    ret = fclose (fd);
    if (MQX_OK != ret) {
        printf ("Error closing DCU4, returned: 0x%08x\n", ret);
        goto OnError;
    }

    printf ("\n-------------- End of example --------------\n\n");

OnError:
    _time_delay (200L);
    _task_block();
}
