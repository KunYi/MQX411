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
*   application that make use of fbdev driver.
*
*
*END************************************************************************/


#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <fbdev.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#if ! BSPCFG_ENABLE_DCU4
#error This application requires BSPCFG_ENABLE_DCU4 defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#if ! BSPCFG_ENABLE_FBDEV
#error This application requires BSPCFG_ENABLE_FBDEV defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#define FBDEV_TEST_FRAMES  (750)

extern void main_task (uint32_t);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index,   Function,   Stack,  Priority,   Name,   Attributes,          Param, Time Slice */
    { 10L,          main_task,  8192L,  8L,         "Main", MQX_AUTO_START_TASK, 0,     0  },
    { 0 }
};

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
    uint32_t i;
    _mqx_int ret;
    MQX_FILE_PTR fb0;
    MQX_FILE_PTR fb1;
    FBDEV_BUF_INFO_STRUCT buf_info0;
    FBDEV_BUF_INFO_STRUCT buf_info1;
    uint8_t *buf0;
    uint8_t *buf1;
    uint32_t bytes;
    TIME_STRUCT begin, end, diff;
    float fps;

    printf("\n-------------- FBDEV driver example --------------\n\n");
    printf("This example application demonstrates usage of FBDEV driver.\n");

    /* Open the FBDEV driver */
    fb0 = fopen ("fbdev:0", NULL);
    fb1 = fopen ("fbdev:1", NULL);
    if (fb0 == NULL) {
        printf("Error opening fbdev:0 driver!\n");
        goto OnError;
    }
    if (fb1 == NULL) {
        printf("Error opening fbdev:1 driver!\n");
        goto OnError;
    }

    printf("\n\n************************************************************************\n");
    printf("Get buffer info ....\n");
    printf("************************************************************************\n");

    _mem_zero(&buf_info0, sizeof(buf_info0));
    _mem_zero(&buf_info1, sizeof(buf_info1));

    ret = ioctl(fb0, IO_IOCTL_FBDEV_GET_BUFFER_INFO, &buf_info0);
    if (MQX_OK != ret) {
        printf("Error get fb0 buffer info, returned: 0x%08x\n", ret);
        goto OnError;
    }

    ret = ioctl(fb1, IO_IOCTL_FBDEV_GET_BUFFER_INFO, &buf_info1);
    if (MQX_OK != ret) {
        printf("Error get fb1 buffer info, returned: 0x%08x\n", ret);
        goto OnError;
    }

    /* default buffer info should be same */
    if (memcmp(&buf_info0, &buf_info1, sizeof(buf_info0)))
    {
        printf("Error buffer info comparison\n");
        goto OnError;
    }

    printf("Original buffer info: buf_count %d, pos [%d, %d], size [%d, %d], format %d\n",
           buf_info0.BUF_COUNT, buf_info0.X, buf_info0.Y, buf_info0.WIDTH, buf_info0.HEIGHT, buf_info0.FORMAT);

    printf("\n\n************************************************************************\n");
    printf("Set frame buffers ....\n");
    printf("************************************************************************\n");
    /* we created the largest buffer here regardless of color format */
    buf0 = (uint8_t*)_mem_alloc_system(buf_info0.WIDTH * buf_info0.HEIGHT * 4 * 2);
    if (NULL == buf0)
    {
        printf("Error out of system memory\n");
        goto OnError;
    }
    buf1 = buf0 + buf_info0.WIDTH * buf_info0.HEIGHT * 4;

    /* Each frame buffer only occupy 1/4 of screen */
    /* fb0 is at the top left */
    buf_info0.X = 0;
    buf_info0.Y = 0;
    buf_info0.WIDTH /= 2;
    buf_info0.HEIGHT /= 2;
    bytes = buf_info0.WIDTH * buf_info0.HEIGHT * 4;
    for (i = 0; i < buf_info0.BUF_COUNT; i++)
    {
        buf_info0.BUFFERS[i] = buf0 + i * bytes;
        memset(buf_info0.BUFFERS[i], 0x20 * (i + 1), bytes);
    }
    ret = ioctl(fb0, IO_IOCTL_FBDEV_SET_BUFFER_INFO, &buf_info0);
    if (MQX_OK != ret) {
        printf("Error set fb0 buffer info, returned: 0x%08x\n", ret);
        goto OnError;
    }

    /* fb1 is at the bottom right */
    buf_info1.WIDTH /= 2;
    buf_info1.HEIGHT /= 2;
    buf_info1.X = buf_info1.WIDTH;
    buf_info1.Y = buf_info1.HEIGHT;
    bytes = buf_info1.WIDTH * buf_info1.HEIGHT * 4;
    for (i = 0; i < buf_info1.BUF_COUNT; i++)
    {
        buf_info1.BUFFERS[i] = buf1 + i * bytes;
        memset(buf_info1.BUFFERS[i], 0x30 * (i + 1), bytes);
    }
    ret = ioctl(fb1, IO_IOCTL_FBDEV_SET_BUFFER_INFO, &buf_info1);
    if (MQX_OK != ret) {
        printf("Error get fb1 buffer info, returned: 0x%08x\n", ret);
        goto OnError;
    }

    /* make sure the frame buffers are updated in DDR */
    _DCACHE_FLUSH();

    printf("\n\n************************************************************************\n");
    printf("Enable fbdev....\n");
    printf("************************************************************************\n");
    ret = ioctl(fb0, IO_IOCTL_FBDEV_ENABLE, (void*)TRUE);
    if (MQX_OK != ret) {
        printf("Error enable fb0, returned: 0x%08x\n", ret);
        goto OnError;
    }

    _time_delay (1000);

    ret = ioctl(fb1, IO_IOCTL_FBDEV_ENABLE, (void*)TRUE);
    if (MQX_OK != ret) {
        printf("Error enable fb1, returned: 0x%08x\n", ret);
        goto OnError;
    }

    printf("\n\n************************************************************************\n");
    printf("Now refresh fbdev....\n");
    printf("************************************************************************\n");

    _time_get(&begin);
    for (i = 0; i < FBDEV_TEST_FRAMES; i++)
    {
        uint32_t index = i % buf_info0.BUF_COUNT;

        ret = ioctl(fb0, IO_IOCTL_FBDEV_WAIT_OFFSCREEN, (void*)index);
        if (MQX_OK != ret && i != 0) /* first buffer is always busy */
        {
            printf("Error fb0 wait offscreen %d, returned: 0x%08x\n", i, ret);
            goto OnError;
        }
        ret = ioctl(fb0, IO_IOCTL_FBDEV_PAN_DISPLAY, (void*)index);
        if (MQX_OK != ret)
        {
            printf("Error fb0 pan display %d, returned: 0x%08x\n", i, ret);
            goto OnError;
        }
        ret = ioctl(fb1, IO_IOCTL_FBDEV_PAN_DISPLAY, (void*)index);
        if (MQX_OK != ret)
        {
            printf("Error fb1 pan display %d, returned: 0x%08x\n", i, ret);
            goto OnError;
        }
    }
    _time_get(&end);
    _time_diff(&begin, &end, &diff);
    fps = (float)FBDEV_TEST_FRAMES / ((float)diff.SECONDS + (float)diff.MILLISECONDS/1000);
    printf("fbdev refresh rate %d\n", (int)fps);

    /* delay about 1s */
    _time_get(&begin);
    for (i = 0; i < 75; i++)
        ioctl(fb1, IO_IOCTL_FBDEV_WAIT_VSYNC, NULL);
    _time_get(&end);
    _time_diff(&begin, &end, &diff);
    fps = (float)75 / ((float)diff.SECONDS + (float)diff.MILLISECONDS/1000);
    printf("fbdev vsync rate %d\n", (int)fps);

    printf("\n\n************************************************************************\n");
    printf("Disable fbdev....\n");
    printf("************************************************************************\n");
    ret = ioctl(fb0, IO_IOCTL_FBDEV_ENABLE, (void*)FALSE);
    if (MQX_OK != ret) {
        printf("Error disable fb0, returned: 0x%08x\n", ret);
        goto OnError;
    }

    _time_delay (1000);

    ret = ioctl(fb1, IO_IOCTL_FBDEV_ENABLE, (void*)FALSE);
    if (MQX_OK != ret) {
        printf("Error disable fb1, returned: 0x%08x\n", ret);
        goto OnError;
    }

    printf("\n\n************************************************************************\n");
    printf("fbdev cleanup....\n");
    printf("************************************************************************\n");

    ret = fclose(fb0);
    if (MQX_OK != ret) {
        printf("Error closing fb0, returned: 0x%08x\n", ret);
        goto OnError;
    }

    ret = fclose(fb1);
    if (MQX_OK != ret) {
        printf("Error closing fb1, returned: 0x%08x\n", ret);
        goto OnError;
    }

    _mem_free(buf0);

    printf("\n-------------- End of example --------------\n\n");

OnError:
    _time_delay (200L);
    _task_block();
}
