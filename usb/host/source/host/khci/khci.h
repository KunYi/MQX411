/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains KHCI-specific implementations of USB interfaces
*
*
*END************************************************************************/

#ifndef __khci_h__
#define __khci_h__

/* Basic scheduling packets algorithm, schedules next packet to the beginning of next frame
*/
//#define KHCICFG_BASIC_SCHEDULING

/* Scheduling based on internal "deadtime" register THSLD; you can choose additional value passed to the register
** Note that the register value depends on delay on line from host to device and is experimentally set to 0x65,
** which should be enough for the absolute majority of cases.
*/
#define KHCICFG_THSLD_DELAY 0x65

/* Allow workaround for bug in the peripheral when unaligned buffer @4B address is used */
#define KHCICFG_4BYTE_ALIGN_FIX

#endif
