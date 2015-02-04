/*HEADER**********************************************************************
*
* Copyright 2009 Freescale Semiconductor, Inc.
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
*   User configuration for USB Stack components
*

*
*END************************************************************************/

#ifndef __usb_stack_config_h__
#define __usb_stack_config_h__

#ifndef HID_CONFIG
#define HID_CONFIG                   1
#endif

#ifndef CDC_CONFIG
#define CDC_CONFIG                   1
#endif


#ifndef AUDIO_CONFIG
#define AUDIO_CONFIG                 1
#endif

#ifndef MSD_CONFIG
#define MSD_CONFIG                   1
#endif

#ifndef PHDC_CONFIG
#define PHDC_CONFIG                  1
#endif

#if CDC_CONFIG
    #define PSTN_SUBCLASS_NOTIF_SUPPORT          (1)/* 1: TRUE; 0: FALSE*/ 
#endif

#if PHDC_CONFIG
    #define USB_METADATA_SUPPORTED              (0)/* 1: TRUE; 0: FALSE*/ 
#endif

#define IMPLEMENT_QUEUING                    (1)/* 1: TRUE; 0: FALSE*/ 

#endif
/* EOF */
