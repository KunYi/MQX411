#ifndef _MFS_USB_h_
#define _MFS_USB_h_
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
*   
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>
#include <message.h>
#include <mfs.h>
#include <part_mgr.h>
#include <usbmfs.h>

#define DEMOCFG_USE_POOLS             0   /* enable external memory pools for USB */

#if BSP_M52259EVB
    #define DEMOCFG_MFS_POOL_ADDR  (uint32_t)(BSP_EXTERNAL_MRAM_RAM_BASE)
    #define DEMOCFG_MFS_POOL_SIZE  0x00010000
#elif DEMOCFG_USE_POOLS
    #warning External pools will not be used on this board.
#endif


#include <shell.h>

extern void Shell_Task(uint32_t);
extern void USB_task(uint32_t);

#endif
