#ifndef __wl_common_h__
#define __wl_common_h__
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
*   This file contains constants and macros of WL module
*
*
*END************************************************************************/
#include "user_config.h"

#ifndef NANDWL_USE_RTOS_MQX
#define NANDWL_USE_RTOS_MQX             1
#endif

#ifndef NANDWL_USE_MM_LEAK_DETECTION
#define NANDWL_USE_MM_LEAK_DETECTION    0
#endif

#define WL_DEBUG                        0

#if NANDWL_USE_RTOS_MQX

#include "mqx.h"
#include "bsp.h"
#include "nandflash_wl_ffs.h"
#include "mutex.h"
#include "timer.h"
#include "wl_types.h"
#include "errcode.h"
#endif /* Endif NANDWL_USE_RTOS_MQX */

#define MIN(a, b)           (((a) <= (b)) ? (a) : (b))
#define MAX(a, b)           (((a) >= (b)) ? (a) : (b))

#ifdef __cplusplus
extern "C" {
#endif
short countBits(short n, uint8_t bit);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __wl_common_h__ */

/* EOF */
