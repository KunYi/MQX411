/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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
*   This file contains board-specific USB initialization functions.
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "bsp_prv.h"

const struct usb_ehci_dev_init_struct _ehci0_dev_init_param = {
    (void *)USB0_BASE_PTR,
    (unsigned char *)USB0_BASE_PTR + 0x100,
    (unsigned char *)USB0_BASE_PTR + 0x80,
    NULL,
    INT_USB0 - 32, //vector for M4 core is not setup in the processor header file
    3           //priority
};

const struct usb_ehci_host_init_struct _ehci0_host_init_param = {
    (void *)USB0_BASE_PTR,
    (unsigned char *)USB0_BASE_PTR + 0x100,
    (unsigned char *)USB0_BASE_PTR + 0x80,
    /* The speed of the EHCI host controller is set to HS */
    BSP_EHCI_HS, //speed: HS
    INT_USB0 - 32, //vector for M4 core is not setup in the processor header file
    3,           //priority
    1024           //size of periodic list [items]
};

const struct usb_ehci_dev_init_struct _ehci1_dev_init_param = {
    (void *)USB1_BASE_PTR,
    (unsigned char *)USB1_BASE_PTR + 0x100,
    (unsigned char *)USB1_BASE_PTR + 0x80,
    NULL,
    INT_USB1 - 32, //vector for M4 core is not setup in the processor header file
    3           //priority
};

const struct usb_ehci_host_init_struct _ehci1_host_init_param = {
    (void *)USB1_BASE_PTR,
    (unsigned char *)USB1_BASE_PTR + 0x100,
    (unsigned char *)USB1_BASE_PTR + 0x80,
    /* The speed of the EHCI host controller is set to HS */
    BSP_EHCI_HS, //speed: HS
    INT_USB1 - 32, //vector for M4 core is not setup in the processor header file
    3,           //priority
    1024           //size of periodic list [items]
};

const void *_bsp_usb_table[] = 
{
    (void *) &_ehci0_dev_init_param,
    (void *) &_ehci0_dev_init_param,
    (void *) &_ehci0_host_init_param,
    (void *) &_ehci0_host_init_param
};
