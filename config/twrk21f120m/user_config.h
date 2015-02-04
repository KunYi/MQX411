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
*   User configuration for MQX components
*
*
*END************************************************************************/

#ifndef __user_config_h__
#define __user_config_h__

/* mandatory CPU identification */
#define MQX_CPU                 PSP_CPU_MK21FN1M

/*
** BSP settings - for defaults see mqx\source\bsp\<board_name>\<board_name>.h
*/
/* MGCT: <generated_code> */

/* BSP settings - for defaults see mqx\source\bsp\<board_name>\<board_name>.h */
#define BSPCFG_ENABLE_I2C0                        0
#define BSPCFG_ENABLE_II2C0                       0

#define BSPCFG_ENABLE_I2C1                        1
#define BSPCFG_ENABLE_II2C1                       1

#define BSPCFG_ENABLE_I2C2                        0
#define BSPCFG_ENABLE_II2C2                       0

#define BSPCFG_ENABLE_RTCDEV                      1

/* pccard shares CS signal with MRAM - opening pccard in user application disables MRAM */
#define BSPCFG_ENABLE_PCFLASH                     1

#define BSPCFG_ENABLE_SPI0                        0

#define BSPCFG_ENABLE_SPI1                        0

#define BSPCFG_ENABLE_SPI2                        0

#define BSPCFG_ENABLE_ADC0                        0
#define BSPCFG_ENABLE_ADC1                        0
#define BSPCFG_ENABLE_FLASHX                      1
#define BSPCFG_ENABLE_ESDHC                       1
#define BSPCFG_ENABLE_SAI                         0
#define BSPCFG_ENABLE_IODEBUG                     0

/* BSP settings - for defaults see mqx\source\bsp\<board_name>\<board_name>.h */
#define BSPCFG_ENABLE_TTYA                        0
#define BSPCFG_ENABLE_ITTYA                       0

#define BSPCFG_ENABLE_TTYB                        0
#define BSPCFG_ENABLE_ITTYB                       0

#define BSPCFG_ENABLE_TTYC                        0
#define BSPCFG_ENABLE_ITTYC                       0

#define BSPCFG_ENABLE_TTYD                        0
#define BSPCFG_ENABLE_ITTYD                       0

#define BSPCFG_ENABLE_TTYE                        0
#define BSPCFG_ENABLE_ITTYE                       0

#define BSPCFG_ENABLE_TTYF                        1
#define BSPCFG_ENABLE_ITTYF                       0

/* board-specific MQX settings - see for defaults mqx\source\include\mqx_cnfg.h */
#define MQX_USE_IDLE_TASK                         1

#define MQX_ENABLE_LOW_POWER                      0
#define MQX_USE_TIMER                             1
#define MQXCFG_ENABLE_FP                          1
#define MQX_INCLUDE_FLOATING_POINT_IO             1

/* board-specific Shell settings */
#define RTCSCFG_ENABLE_TCP                        1

#define SHELLCFG_USES_MFS                         1
#define SHELLCFG_USES_RTCS                        1
/* MGCT: </generated_code> */

/*
** include common settings
*/

/* use the rest of defaults from small-RAM-device profile */
#include "small_ram_config.h"

/* and enable verification checks in kernel */
#include "verif_enabled_config.h"

#endif /* __user_config_h__ */

