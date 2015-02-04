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
*   This file contains the type definitions for the core mutex driver.
*
*
*END************************************************************************/

#ifndef __core_mutex_h__
#define __core_mutex_h__

#define __core_mutex_h__version "$Version:3.8.2.0$"
#define __core_mutex_h__date    "$Date:Sep-24-2012$"

#include <core_mutex_sema4.h>

/* Core Mutex driver error codes */
#define COREMUTEX_OK         (0)
#define COREMUTEX_LOCKED     (COREMUTEX_ERROR_BASE | 0x01) /* CM locked */
#define COREMUTEX_UNLOCKED   (COREMUTEX_ERROR_BASE | 0x02) /* CM unlocked */


#define CORE_MUTEX_VALID  (('c'<<24)|('m'<<24)|('t'<<24)|'x')

typedef struct core_mutex_component {
   CORE_MUTEX_DEVICE  DEVICE[SEMA4_NUM_DEVICES];
} CORE_MUTEX_COMPONENT, * CORE_MUTEX_COMPONENT_PTR;


typedef struct core_mutex_init_struct {
   uint32_t     INT_PRIORITY;
} CORE_MUTEX_INIT_STRUCT, * CORE_MUTEX_INIT_STRUCT_PTR;


#ifdef __cplusplus
extern "C" {
#endif

extern CORE_MUTEX_COMPONENT_PTR _core_mutext_get_component_ptr(void);
extern void                     _core_mutext_set_component_ptr(CORE_MUTEX_COMPONENT_PTR p);
extern uint32_t                 _core_mutex_install( const CORE_MUTEX_INIT_STRUCT * init_ptr );
extern CORE_MUTEX_PTR           _core_mutex_create( uint32_t dev_num, uint32_t mutex_num, uint32_t policy );
extern uint32_t                 _core_mutex_create_at( CORE_MUTEX_PTR mutex_ptr, uint32_t dev_num, uint32_t mutex_num, uint32_t policy );
extern uint32_t                 _core_mutex_destroy( CORE_MUTEX_PTR mutex_ptr );
extern CORE_MUTEX_PTR           _core_mutex_get( uint32_t dev_num, uint32_t mutex_num );
extern uint32_t                 _core_mutex_lock( CORE_MUTEX_PTR core_mutex_ptr );
extern uint32_t                 _core_mutex_trylock( CORE_MUTEX_PTR core_mutex_ptr );
extern uint32_t                 _core_mutex_unlock( CORE_MUTEX_PTR core_mutex_ptr );
extern int32_t                  _core_mutex_owner( CORE_MUTEX_PTR core_mutex_ptr );


#ifdef __cplusplus
}
#endif


#endif
