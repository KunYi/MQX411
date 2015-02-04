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
*   This file contains the type definitions for the core mutex driver for the SEMA4.
*
*
*END************************************************************************/

#ifndef __core_mutex_sema4_h__
#define __core_mutex_sema4_h__

#define __core_mutex_sema4_h__version "$Version:3.8.2.0$"
#define __core_mutex_sema4_h__date    "$Date:Sep-24-2012$"

#include <sema4.h>

#ifndef BSPCFG_CORE_MUTEX_STATS
#define BSPCFG_CORE_MUTEX_STATS 0
#endif

PACKED_STRUCT_BEGIN
struct core_mutex_struct {
    uint32_t                 VALID;
    uint32_t                 GATE_NUM;
    volatile unsigned char   *GATE_PTR;
    void                    *WAIT_Q;
#if BSPCFG_CORE_MUTEX_STATS
    uint32_t                 LOCKS;
    uint32_t                 WAITS;
    uint32_t                 UNLOCKS;
#endif
} PACKED_STRUCT_END;

typedef struct core_mutex_struct CORE_MUTEX_STRUCT, * CORE_MUTEX_PTR;


PACKED_STRUCT_BEGIN
struct core_mutex_device {
    SEMA4_MemMapPtr          SEMA4_PTR;
    volatile uint16_t        *CPNTF_PTR;

    uint32_t                 ALLOCED;
    CORE_MUTEX_PTR           MUTEX_PTR[SEMA4_NUM_GATES];
} PACKED_STRUCT_END;

typedef struct core_mutex_device CORE_MUTEX_DEVICE, * CORE_MUTEX_DEVICE_PTR;

#ifdef __cplusplus
extern "C" {
#endif


SEMA4_MemMapPtr _bsp_get_sema4_base_address(uint32_t);
uint32_t _bsp_get_sema4_vector(uint32_t device);

#ifdef __cplusplus
}
#endif

#endif
