#ifndef __fbdev_prv_h__ 
#define __fbdev_prv_h__
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
*   This file contains definitions private to the FBDEV driver.
*
*
*END************************************************************************/

#include <fbdev.h>
#include <lwevent.h>
#include <lwsem.h>

/*--------------------------------------------------------------------------*/
/*
**                    CONSTANT DEFINITIONS
*/


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** FBDEV_INFO_STRUCT
** FBDEV run time state information
*/
typedef struct fbdev_info_struct
{
    /* The correspond display handle */
    MQX_FILE_PTR              DISPLAY;

    /* The correspond display layer */
    uint32_t                  LAYER;
    
    /* The number of opened file descriptors */
    uint32_t                  COUNT;

    /* The buffer info of the instance */
    FBDEV_BUF_INFO_STRUCT     BUF_INFO;

    /* The vsync event for synchronization */
    LWEVENT_STRUCT            VSYNC;

    /* The frame ready to display */
    uint32_t                  READY_DISPLAY_INDEX;

    /* The frame to display next cycle */
    uint32_t                  TO_DISPLAY_INDEX;

    /* The frame now displaying */
    uint32_t                  DISPLAYING_INDEX;

} FBDEV_INFO_STRUCT, * FBDEV_INFO_STRUCT_PTR;

/*
** FBDEV_DEVICE_STRUCT
** FBDEV install parameters
*/
typedef struct fbdev_device_struct
{
    /* The current init values for this device */
    FBDEV_INIT_STRUCT_CPTR        INIT;

    /* Lock for multi-thread access */
    LWSEM_STRUCT                  LOCK;

    /* The opened fbdev instance count */
    uint32_t                      INSTANCE_OPENED;

    /* The fbdev device instances */
    FBDEV_INFO_STRUCT_PTR        *INSTANCES;

} FBDEV_DEVICE_STRUCT, * FBDEV_DEVICE_STRUCT_PTR;
    
/*--------------------------------------------------------------------------*/
/* 
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_int _fbdev_open (MQX_FILE_PTR, char *, char *);
extern _mqx_int _fbdev_close (MQX_FILE_PTR);
extern _mqx_int _fbdev_ioctl (MQX_FILE_PTR, uint32_t, void *);
extern _mqx_int _fbdev_uninstall (IO_DEVICE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif
      
#endif
/* EOF */
