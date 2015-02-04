#ifndef __fbdev_h__
#define __fbdev_h__
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
*   This file contains the definitions of constants and structures
*   required for the FBDEV driver
*
*
*END************************************************************************/

#include <mqx.h>
#include <ioctl.h>

/*--------------------------------------------------------------------------*/
/*
**                    CONSTANT DEFINITIONS
*/
#define FBDEV_MAX_BUFFERS                    (3)

/* 
** IOCTL calls specific to FBDEV
*/

/*
** brief   Get buffer info from FBDEV driver
**
** param [out] a void* variable, point to a FBDEV_BUF_INFO_STRUCT structure
**
*/
#define IO_IOCTL_FBDEV_GET_BUFFER_INFO       _IO(IO_TYPE_FBDEV,0x01)

/*
** brief   Set buffer info to FBDEV driver
**
** param [in] a void* variable, point to a FBDEV_BUF_INFO_STRUCT structure
**
*/
#define IO_IOCTL_FBDEV_SET_BUFFER_INFO       _IO(IO_TYPE_FBDEV,0x02)

/*
** brief  Wait for the specified buffer to be offscreen 
**
** param [in] a uint32_t variable, indicate which buffer to wait.
**
** note   A FBDEV device could hold several buffers and only one of them is displaying,
**        the rest buffers are offscreen and capable for application to fill their
**        contents.
*/
#define IO_IOCTL_FBDEV_WAIT_OFFSCREEN        _IO(IO_TYPE_FBDEV,0x03)

/*
** brief  Wait until the next frame to be displayed
*/
#define IO_IOCTL_FBDEV_WAIT_VSYNC            _IO(IO_TYPE_FBDEV,0x04)

/*
** brief   Set specified buffer to be displayed by FBDEV driver
**
** param [in] a uint32_t variable, indicate which buffer to display.
**
*/
#define IO_IOCTL_FBDEV_PAN_DISPLAY           _IO(IO_TYPE_FBDEV,0x05)

/*
** brief   Enable or disable displaying of the frame buffer
**
** param [in] a bool variable, TRUE for enable and FALSE for disable
**
*/
#define IO_IOCTL_FBDEV_ENABLE                _IO(IO_TYPE_FBDEV,0x06)

/* FBDEV error codes */
#define FBDEV_OK                             (0x00)

/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** FBDEV_COLOR_FORMAT
**
** This enumeration defines the FBDEV color format.
*/
typedef enum
{
    /* RGB format */
    FBDEV_COLOR_ARGB8888 = 0,
    FBDEV_COLOR_RGB565,
    FBDEV_COLOR_RGB888,
    FBDEV_COLOR_ARGB1555,
    FBDEV_COLOR_ARGB4444,

    /* YUV format */
    FBDEV_COLOR_UYVY = 0x100
} FBDEV_COLOR_FORMAT;

/*
** FBDEV_BUF_INFO_STRUCT
**
** This structure defines the fbdev buffer info struct.
*/
typedef struct fbdev_buf_info_struct {
    /* Buffer count of each fbdev for flipping */
    uint32_t             BUF_COUNT;

    /* Display position and size on the screen */
    uint32_t             X;
    uint32_t             Y;
    uint32_t             WIDTH;
    uint32_t             HEIGHT;

    /* Buffer format */
    FBDEV_COLOR_FORMAT   FORMAT;

    /* Buffers */
    void                *BUFFERS[FBDEV_MAX_BUFFERS];

} FBDEV_BUF_INFO_STRUCT, * FBDEV_BUF_INFO_STRUCT_PTR;

/*
** FBDEV_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a fbdev driver is initialized.
*/
typedef struct fbdev_init_struct
{
    /* The underlying display device name */
    const char         *DISPLAY_DEVICE;

    /* The fbdev device count */
    uint32_t            FBDEV_COUNT;

    /* Default buffer count of each fbdev for flipping */
    uint32_t            BUF_COUNT;

    /* Default format of the frame buffers */
    FBDEV_COLOR_FORMAT  FORMAT;

} FBDEV_INIT_STRUCT, * FBDEV_INIT_STRUCT_PTR;

typedef const FBDEV_INIT_STRUCT * FBDEV_INIT_STRUCT_CPTR;

/*--------------------------------------------------------------------------*/
/* 
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_uint _fbdev_install(char *, FBDEV_INIT_STRUCT_CPTR);

#ifdef __cplusplus
}
#endif


#endif 

/* EOF */
