#ifndef __usb_file_h__
#define __usb_file_h__
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
*   This file contains the Test specific data structures and defines
*
*
*END************************************************************************/



typedef struct {
   MQX_FILE_PTR DEV_FD_PTR;
   MQX_FILE_PTR PM_FD_PTR;
   MQX_FILE_PTR FS_FD_PTR;
   char    *DEV_NAME;
   char    *PM_NAME;
   char    *FS_NAME;
} USB_FILESYSTEM_STRUCT, * USB_FILESYSTEM_STRUCT_PTR;



#ifdef __cplusplus
extern "C" {
#endif
extern void *usb_filesystem_install( 
   void       *usb_handle,
   char    *block_device_name,
   char    *partition_manager_name,
   char    *file_system_name );
extern void usb_filesystem_uninstall( USB_FILESYSTEM_STRUCT_PTR  usb_fs_ptr);
extern MQX_FILE_PTR usb_filesystem_handle( USB_FILESYSTEM_STRUCT_PTR  usb_fs_ptr);

   
#ifdef __cplusplus
}
#endif

#endif
/* EOF */
