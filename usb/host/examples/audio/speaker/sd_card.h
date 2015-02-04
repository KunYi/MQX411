/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
* Copyright 2010 ARC International
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
*   This file contains audio types and definitions.
*
*
*END************************************************************************/

#ifndef __sd_card_h__
#define __sd_card_h__

#ifdef __USB_OTG__
#include "otgapi.h"
#include "devapi.h"
#else
#include "hostapi.h"
#endif
#include "usb_host_audio.h"


#define SD_CARD_NOT_INSERTED     0x00
#define SD_CARD_INSERTED         0x01
#define SD_CARD_READY            0x02

#define SD_Card_Event_CLOSE      0x01
/* Alphabetical list of Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
