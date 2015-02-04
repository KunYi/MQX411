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
*   This file contains the definition for the USB DCD
*   channel
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "usb_dcd.h"
#include "usb_dcd_kn.h"

const KUSB_DCD_INIT_STRUCT _bsp_usb_dcd_init = {
      USBDCD_CLOCK_SPEED_RESET_VALUE,       
      USBDCD_TSEQ_INIT_RESET_VALUE,         
      USB_DCD_TDCD_DBNC_RESET_VALUE,  
      USB_DCD_TVDPSRC_ON_RESET_VALUE,
      USB_DCD_TVDPSRC_CON_RESET_VALUE,   
      USB_DCD_CHECK_DM_RESET_VALUE,   
      USB_DCD_LEVEL_RESET_VALUE   
};
