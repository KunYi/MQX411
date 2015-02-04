/*HEADER**********************************************************************
*
* Copyright 2008-2013 Freescale Semiconductor, Inc.
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
#define MQX_CPU                 PSP_CPU_VF65GS10_A5

/* MGCT: <generated_code> */
#define BSPCFG_HAS_SRAM_POOL                      1
#define BSPCFG_ENET_SRAM_BUF                      1

#define BSPCFG_ENABLE_IO_SUBSYSTEM                1

#define BSPCFG_ENABLE_I2C0                        1
#define BSPCFG_ENABLE_II2C0                       1
#define BSPCFG_ENABLE_I2C1                        0
#define BSPCFG_ENABLE_II2C1                       0
#define BSPCFG_ENABLE_I2C2                        0
#define BSPCFG_ENABLE_II2C2                       0
#define BSPCFG_ENABLE_I2C3                        0
#define BSPCFG_ENABLE_II2C3                       0
#define BSPCFG_ENABLE_SPI0                        0
#define BSPCFG_ENABLE_SPI1                        0
#define BSPCFG_ENABLE_SPI2                        0
#define BSPCFG_ENABLE_QUADSPI0                    0
#define BSPCFG_ENABLE_RTCDEV                      0
#define BSPCFG_ENABLE_PCFLASH                     0
#define BSPCFG_ENABLE_NANDFLASH                   0
#define BSPCFG_ENABLE_FLASHX_QUADSPI0             0
#define BSPCFG_ENABLE_ESDHC                       1
#define BSPCFG_ENABLE_IODEBUG                     0
#define BSPCFG_ENABLE_SAI                         1
#define BSPCFG_ENABLE_USB                         0
#define BSPCFG_ENABLE_CLKTREE_MGMT                0

#define BSPCFG_ENABLE_TTYA                        0
#define BSPCFG_ENABLE_ITTYA                       0

#define BSPCFG_ENABLE_TTYB                        1
#define BSPCFG_ENABLE_ITTYB                       0

#define BSPCFG_ENABLE_TTYC                        0
#define BSPCFG_ENABLE_ITTYC                       0

#define BSPCFG_ENABLE_TTYD                        0
#define BSPCFG_ENABLE_ITTYD                       0

#define BSPCFG_ENABLE_TTYE                        0
#define BSPCFG_ENABLE_ITTYE                       0

#define BSPCFG_ENABLE_TTYF                        0
#define BSPCFG_ENABLE_ITTYF                       0

#define BSP_DEFAULT_MAX_MSGPOOLS                  4

#define MQX_HAS_TIME_SLICE                        1
#define MQX_USE_IDLE_TASK                         1

#define MQX_ENABLE_LOW_POWER                      0

#define MQX_USE_LWMEM_ALLOCATOR                   0
#define MQX_USE_UNCACHED_MEM                      1

#define MQX_SPARSE_ISR_TABLE                      0

#define MQX_TASK_DESTRUCTION                      1

#define MQXCFG_ENABLE_FP                          0
#define MQX_INCLUDE_FLOATING_POINT_IO             0

#define MQX_IS_MULTI_PROCESSOR                    1
#define MQX_USE_IPC                               1

#define MQXCFG_MEM_COPY                           1

/* DS-5 memcpy is bugged so we have to use MQX implementation */
#define MQXCFG_MEM_COPY_NEON                      1
#define MQX_USE_MEM_ZERO                          1

#define RTCSCFG_ENABLE_ICMP                       1
#define RTCSCFG_ENABLE_SNMP                       0
#define RTCSCFG_ENABLE_UDP                        1
#define RTCSCFG_ENABLE_TCP                        1
#define RTCSCFG_ENABLE_STATS                      1
#define RTCSCFG_ENABLE_GATEWAYS                   1

#define RTCSCFG_QUEUE_BASE                        2

#define FTPDCFG_USES_MFS                          1

#define TELNETDCFG_NOWAIT                         FALSE

#define MQX_USE_TIMER                             1

/* MGCT: </generated_code> */

#define ENETCFG_SUPPORT_PTP             0

/*
** include common settings
*/

/* use the rest of defaults from small-RAM-device profile */
#include "small_ram_config.h"

/* and enable verification checks in kernel */
#include "verif_enabled_config.h"

#endif /* __user_config_h__ */
