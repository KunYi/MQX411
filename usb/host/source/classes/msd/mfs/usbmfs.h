#ifndef _usbmfs_h_
#define _usbmfs_h_
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
*   The file contains functions prototype, defines, structure
*   definitions specific for USB MFS mass storage link driver.
*
*
*END************************************************************************/

#if MQX_USE_IO_OLD
#include <io.h>
#else
#include <nio.h>
#endif
#include <host_main.h>

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

/*
** USBMFS IOCTL calls
*/
#define USB_MFS_VERY_LARGE_DRIVE           (3)
#define IO_IOCTL_GET_VENDOR_INFO       (0x3000) //TODO: rewrite
#define IO_IOCTL_GET_PRODUCT_ID        (0x3001) //TODO: rewrite
#define IO_IOCTL_GET_PRODUCT_REV       (0x3002) //TODO: rewrite
#define IO_IOCTL_DEVICE_STOP           (0x3003) //TODO: rewrite

/*
** USBMFS error codes
*/
#define USBMFS_ERROR_TIMEOUT               (0x3000) //TODO: rewrite
#define USBMFS_ERROR_INQUIRY               (0x3001) //TODO: rewrite

#ifdef PSP_CACHE_LINE_SIZE
   #define CACHE_LINE_FILLER(x)   (PSP_CACHE_LINE_SIZE-(x%PSP_CACHE_LINE_SIZE))
#else
   #define CACHE_LINE_FILLER(x)
#endif

/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

typedef struct usb_mfs_drive_info_struct
{
   uint32_t  NUMBER_OF_HEADS;
   uint32_t  NUMBER_OF_TRACKS;
   uint32_t  SECTORS_PER_TRACK;
} USB_MFS_DRIVE_INFO_STRUCT, * USB_MFS_DRIVE_INFO_STRUCT_PTR;

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

#if MQX_USE_IO_OLD
uint32_t _io_usb_mfs_install(char *identifier, uint8_t logical_unit, CLASS_CALL_STRUCT_PTR ccs_ptr);
#else
NIO_DEV_STRUCT* _io_usb_mfs_install(char *identifier, uint8_t logical_unit, CLASS_CALL_STRUCT_PTR ccs_ptr);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
