/*HEADER**********************************************************************
*
* Copyright 2014 Freescale Semiconductor, Inc.
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
*   This file contains board-specific USB initialization functions.
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "bsp_prv.h"

const struct usb_khci_dev_init_struct _khci0_dev_init_param = {
    (void *)USB0_BASE_PTR,
    INT_USB0, //vector
    3,  //priority
    16  //max. endpoints
};

const struct usb_khci_host_init_struct _khci0_host_init_param = {
    (void *)USB0_BASE_PTR,
    INT_USB0, //vector
    3  //priority
};

const void * _bsp_usb_table[] = 
{
    (void *) &_khci0_dev_init_param,
    (void *) &_khci0_host_init_param
};
