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
* @brief The file contains Macro's and functions needed by the disk 
*        application
* 
*END************************************************************************/


#ifndef _DISK_H
#define _DISK_H  1

#include "usb_descriptor.h"
/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define RAM_DISK_APP        (1)

#if RAM_DISK_APP
    /* Length of Each Logical Address Block */
    #define LENGTH_OF_EACH_LAB              (512)    
    /* total number of logical blocks present */
    #define TOTAL_LOGICAL_ADDRESS_BLOCKS_MIN (41)
    #define TOTAL_LOGICAL_ADDRESS_BLOCKS_NORMAL (60)
    /* Net Disk Size */
    #define DISK_SIZE_MIN    (TOTAL_LOGICAL_ADDRESS_BLOCKS_MIN * LENGTH_OF_EACH_LAB)
    #define DISK_SIZE_NORMAL (TOTAL_LOGICAL_ADDRESS_BLOCKS_NORMAL * LENGTH_OF_EACH_LAB)
#endif

#define LOGICAL_UNIT_SUPPORTED          (1)

#define SUPPORT_DISK_LOCKING_MECHANISM  (0) /*1: TRUE; 0:FALSE*/
/* If Implementing Disk Drive then configure the macro below as TRUE,
   otherwise keep it False(say for Hard Disk)*/
#define IMPLEMENTING_DISK_DRIVE         (0) /*1: TRUE; 0:FALSE*/

#define MSD_RECV_BUFFER_SIZE            (BULK_OUT_ENDP_PACKET_SIZE)
#define MSD_SEND_BUFFER_SIZE            (BULK_IN_ENDP_PACKET_SIZE)

/*****************************************************************************
 * Global variables
 *****************************************************************************/
 
/******************************************************************************
 * Types
 *****************************************************************************/
typedef struct _disk_variable_struct
{
    MSD_HANDLE app_handle;
    bool start_app;
#if RAM_DISK_APP
    /* disk space reserved */
    uint8_t *storage_disk; 
#endif  
    uint8_t disk_lock;   
}DISK_STRUCT, * DISK_STRUCT_PTR; 

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
extern void TestApp_Init(void);
extern void Disk_App(void);

#endif


/* EOF */
