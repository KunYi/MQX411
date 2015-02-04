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
#define MQX_CPU                 PSP_CPU_MK20D50M

/* BSPCFG_ENABLE_LEGACY_II2C_SLAVE is the flag to enable interrupt driven i2c
   slave driver.
   If it's enabled, the driver will register interrupt handler to corresponding
   I2C SDA pin muxing port vector, and please make sure your application would not
   register handler on same interrupt vector.
 */
#define BSPCFG_ENABLE_LEGACY_II2C_SLAVE     0

/* MGCT: <generated_code> */
#define MQX_USE_IDLE_TASK        1

#define MQX_ENABLE_LOW_POWER     0

#define BSPCFG_ENABLE_I2C0       0
#define BSPCFG_ENABLE_II2C0      0
#define BSPCFG_ENABLE_RTCDEV     0
#define BSPCFG_ENABLE_SPI0       0
#define BSPCFG_ENABLE_ADC0       1
#define BSPCFG_ENABLE_FLASHX     0
#define BSPCFG_ENABLE_IODEBUG    0

#define BSPCFG_ENABLE_SAI        1

#define BSPCFG_ENABLE_TTYA       0
#define BSPCFG_ENABLE_ITTYA      0

#define BSPCFG_ENABLE_TTYB       1
#define BSPCFG_ENABLE_ITTYB      0

#define BSPCFG_ENABLE_TTYC       0
#define BSPCFG_ENABLE_ITTYC      0

/* MGCT: </generated_code> */

/* use the rest of defaults from small-RAM-device profile */
#include "small_ram_config.h"

/* and enable verification checks in kernel */
#include "verif_enabled_config.h"

#endif /* __user_config_h__ */
