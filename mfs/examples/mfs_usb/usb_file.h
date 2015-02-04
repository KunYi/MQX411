#ifndef __usb_file_h__
#define __usb_file_h__
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
*   This file contains the Test specific data structures and defines
*
*
*END************************************************************************/

typedef struct msd_dev_data_struct {
   CLASS_CALL_STRUCT    ccs;
   char                *DEV_NAME;
   char                *PM_NAME;
   char                 FS_NAME[5][3];
   MQX_FILE_PTR         DEV_FD_PTR;
   MQX_FILE_PTR         PM_FD_PTR[5];
   MQX_FILE_PTR         FS_FD_PTR[5];
} MSD_DEV_DATA_STRUCT, *MSD_DEV_DATA_PTR;


#ifdef __cplusplus
extern "C" {
#endif

extern int usb_msd_install(void *usb_handle, uint32_t *mountp);
extern int usb_msd_uninstall(void *usb_handle, uint32_t *mountp);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
