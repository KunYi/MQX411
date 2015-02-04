
/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
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
*   This file contains prototypes for MQX-specific MCC library functions
*
*
*END************************************************************************/

#ifndef __MCC_MQX__
#define __MCC_MQX__

#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>
#include "core_mutex.h"
#include "vybrid.h"

#define MCC_MQX_LWEVENT_COMPONENTS_COUNT   (8)
#define MCC_MQX_LWEVENT_GROUP_SIZE         (32)

#define MCC_MQX_NODE_NUMBER                (0)

#define MCC_DCACHE_ENABLE(n)               _DCACHE_ENABLE(n)
#define MCC_DCACHE_DISABLE()               _DCACHE_DISABLE()
#define MCC_DCACHE_FLUSH()                 _DCACHE_FLUSH()
#define MCC_DCACHE_FLUSH_LINE(p)           _DCACHE_FLUSH_LINE(p)
#define MCC_DCACHE_FLUSH_MLINES(p, m)      _DCACHE_FLUSH_MLINES(p, m)
#define MCC_DCACHE_INVALIDATE()            _DCACHE_INVALIDATE()
#define MCC_DCACHE_INVALIDATE_LINE(p)      _DCACHE_INVALIDATE_LINE(p)
#define MCC_DCACHE_INVALIDATE_MLINES(p, m) _DCACHE_INVALIDATE_MLINES(p, m)

/* Semaphore-related functions */
int mcc_init_semaphore(unsigned int);
int mcc_deinit_semaphore(unsigned int);
int mcc_get_semaphore(void);
int mcc_release_semaphore(void);

/* CPU-to-CPU interrupt-related functions */
int mcc_register_cpu_to_cpu_isr(void);
int mcc_generate_cpu_to_cpu_interrupt(void);

/* Memory management-related functions */
void mcc_memcpy(void*, void*, unsigned int);

#endif /* __MCC_MQX__ */
