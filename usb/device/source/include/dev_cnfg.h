#ifndef __usbdev_cnfg_h__
#define __usbdev_cnfg_h__
/*HEADER**********************************************************************
*
* Copyright 2008-2012 Freescale Semiconductor, Inc.
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*
*END************************************************************************/
#include "mqx.h"
#include "bsp.h"        // bsp.h include user_config.h, too

#ifndef USBCFG_DEFAULT_DEVICE_CONTROLLER
#define USBCFG_DEFAULT_DEVICE_CONTROLLER  (0)
#endif

#ifndef USBCFG_MAX_DRIVERS
extern const void   *_bsp_usb_table[];
#define USBCFG_MAX_DRIVERS                (sizeof(_bsp_usb_table) / sizeof(_bsp_usb_table[0]))
#endif

/*
** The aligned buffer size for IN transactions, active when KHCICFG_4BYTE_ALIGN_FIX is defined
** MGCT: <option type="number"/>
*/
#ifndef USBCFG_KHCI_SWAP_BUF_MAX
#define USBCFG_KHCI_SWAP_BUF_MAX                (1024)
#endif


#endif
