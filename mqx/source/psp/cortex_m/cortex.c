
/*HEADER**********************************************************************
*
* Copyright 2010 Freescale Semiconductor, Inc.
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
*   This file contains the type definitions for the ARM Cortex processors.
*
*
*END************************************************************************/

#include "mqx_inc.h"
#include "cortex.h"

#if MQX_ENABLE_USER_MODE
#include "lwevent.h"

#if MQX_USE_LWEVENTS
#include "lwevent_prv.h"
#endif /* MQX_USE_LWEVENTS */

#if MQX_USE_LWMSGQ
#include "lwmsgq.h"
#include "lwmsgq_prv.h"
#endif

#if MQX_USE_LWMEM
#include "lwmem.h"
#endif

#endif /* MQX_ENABLE_USER_MODE */

#if !MQX_USE_IDLE_TASK
#error "MQX for ARM Cortex M requires to enable MQX_USE_IDLE_TASK"
#endif

#if PSP_MQX_CPU_IS_VYBRID
/*!
 * \brief This function returns the core number
 *
 * \return int
 */
uint32_t _psp_core_num(void)
{
    return MSCM_CPxNUM & MSCM_CPxNUM_CPN_MASK;
}
#endif // PSP_MQX_CPU_IS_VYBRID

#if MQX_ENABLE_USER_MODE

/*!
 * \brief 	MQX API handler for usermode - part of wrapper around standard MQX API 
 *  which require privilege mode.
 * 
 * \param[in] api_no API number - number of wrapped function
 * \param[in] params generic parameter - direct use with called MQX API fn
 *
 * \return uint32_t return of called function
 */
uint32_t _mqx_api_call_handler
    (
        // [IN] API number - number of wrapped function
        MQX_API_NUMBER_ENUM api_no,
        // [IN] generic parameter - direct use with called MQX API fn
        MQX_API_CALL_PARAMS_PTR params
    )
{
    uint32_t res = -1;
    uint32_t param0 = params->param0;
    uint32_t param1 = params->param1;
    uint32_t param2 = params->param2;
    uint32_t param3 = params->param3;
    uint32_t param4 = params->param4;

    switch (api_no) {

    // _lwsem
    case MQX_API_LWSEM_POLL:
        if (MQX_OK == (res = _lwsem_usr_check((LWSEM_STRUCT_PTR)param0)))
            res = (uint32_t)_lwsem_poll((LWSEM_STRUCT_PTR)param0);
        break;
    case MQX_API_LWSEM_POST:
        if (MQX_OK == (res = _lwsem_usr_check((LWSEM_STRUCT_PTR)param0)))
            res = _lwsem_post((LWSEM_STRUCT_PTR)param0);
        break;
    case MQX_API_LWSEM_WAIT:
        if (MQX_OK == (res = _lwsem_usr_check((LWSEM_STRUCT_PTR)param0)))
            res = _lwsem_wait((LWSEM_STRUCT_PTR)param0);
        break;
    case MQX_API_LWSEM_CREATE:
        res = _lwsem_create_internal((LWSEM_STRUCT_PTR)param0, (_mqx_int)param1, (bool)param2, TRUE);
        break;
#if MQX_HAS_TICK
    case MQX_API_LWSEM_WAIT_FOR:
        if (MQX_OK == (res = _lwsem_usr_check((LWSEM_STRUCT_PTR)param0)) && (!param1 || _psp_mem_check_access(param1, sizeof(MQX_TICK_STRUCT), MPU_UM_RW)))
            res = _lwsem_wait_for((LWSEM_STRUCT_PTR)param0, (MQX_TICK_STRUCT_PTR)param1);
        break;
    case MQX_API_LWSEM_WAIT_TICKS:
        if (MQX_OK == (res = _lwsem_usr_check((LWSEM_STRUCT_PTR)param0)))
            res = _lwsem_wait_ticks((LWSEM_STRUCT_PTR)param0, (_mqx_uint)param1);
        break;
    case MQX_API_LWSEM_WAIT_UNTIL:
        if (MQX_OK == (res = _lwsem_usr_check((LWSEM_STRUCT_PTR)param0)) && (!param1 || _psp_mem_check_access(param1, sizeof(MQX_TICK_STRUCT), MPU_UM_RW)))
            res = _lwsem_wait_until((LWSEM_STRUCT_PTR)param0, (MQX_TICK_STRUCT_PTR)param1);
        break;
    case MQX_API_LWSEM_DESTROY:
        if (MQX_OK == (res = _lwsem_usr_check((LWSEM_STRUCT_PTR)param0))) {
            res = _lwsem_destroy_internal((LWSEM_STRUCT_PTR)param0, TRUE);
        }
        break;

#endif // MQX_HAS_TICK

    // _lwevent
#if MQX_USE_LWEVENTS
    case MQX_API_LWEVENT_CLEAR:
        if (MQX_OK == (res = _lwevent_usr_check((LWEVENT_STRUCT_PTR)param0)))
            res = _lwevent_clear((LWEVENT_STRUCT_PTR)param0, (_mqx_uint)param1);
        break;
    case MQX_API_LWEVENT_SET:
        if (MQX_OK == (res = _lwevent_usr_check((LWEVENT_STRUCT_PTR)param0)))
            res = _lwevent_set((LWEVENT_STRUCT_PTR)param0, (_mqx_uint)param1);
        break;
    case MQX_API_LWEVENT_SET_AUTO_CLEAR:
        if (MQX_OK == (res = _lwevent_usr_check((LWEVENT_STRUCT_PTR)param0)))
            res = _lwevent_set_auto_clear((LWEVENT_STRUCT_PTR)param0, (_mqx_uint)param1);
        break;
    case MQX_API_LWEVENT_WAIT_FOR:
        if (MQX_OK == (res = _lwevent_usr_check((LWEVENT_STRUCT_PTR)param0)) && \
            (!param3 || _psp_mem_check_access(param3, sizeof(MQX_TICK_STRUCT), MPU_UM_RW))) {
            res = _lwevent_wait_for((LWEVENT_STRUCT_PTR)param0, (_mqx_uint)param1, (bool)param2, (MQX_TICK_STRUCT_PTR)param3);
        }
        break;
    case MQX_API_LWEVENT_WAIT_FOR_TICKS:
        if (MQX_OK == (res = _lwevent_usr_check((LWEVENT_STRUCT_PTR)param0))) {
            res = _lwevent_wait_ticks((LWEVENT_STRUCT_PTR)param0, (_mqx_uint)param1, (bool)param2, (_mqx_uint)param3);
        }
        break;
    case MQX_API_LWEVENT_WAIT_UNTIL:
        if (MQX_OK == (res = _lwevent_usr_check((LWEVENT_STRUCT_PTR)param0)) && \
            (!param3 || _psp_mem_check_access(param3, sizeof(MQX_TICK_STRUCT), MPU_UM_RW))) {
            res = _lwevent_wait_until((LWEVENT_STRUCT_PTR)param0, (_mqx_uint)param1, (bool)param2, (MQX_TICK_STRUCT_PTR)param3);
        }
        break;
    case MQX_API_LWEVENT_GET_SIGNALLED:
        res = _lwevent_get_signalled();
        break;

    case MQX_API_LWEVENT_CREATE:
        res = _lwevent_create_internal((LWEVENT_STRUCT_PTR)param0, (_mqx_uint)param1, TRUE);
        break;

    case MQX_API_LWEVENT_DESTROY:
        if (MQX_OK == (res = _lwevent_usr_check((LWEVENT_STRUCT_PTR)param0))) {
            res = _lwevent_destroy_internal((LWEVENT_STRUCT_PTR)param0, TRUE);
        }
        break;
#endif

#if MQX_USE_LWMSGQ
    case MQX_API_LWMSGQ_INIT:
        res = _lwmsgq_init_internal((void *)param0, (_mqx_uint)param1, (_mqx_uint)param2, TRUE);
        break;
    case MQX_API_LWMSGQ_RECEIVE:
        if (MQX_OK == (res = _lwmsgq_usr_check((LWMSGQ_STRUCT_PTR)param0)) && \
            _psp_mem_check_access(param1, ((LWMSGQ_STRUCT_PTR)param0)->MSG_SIZE, MPU_UM_RW) && \
            (!param4 || _psp_mem_check_access(param4, sizeof(MQX_TICK_STRUCT), MPU_UM_RW)))
            res = _lwmsgq_receive((void *)param0, (_mqx_max_type_ptr)param1, (_mqx_uint)param2, (_mqx_uint)param3, (MQX_TICK_STRUCT_PTR)param4);

        break;
    case MQX_API_LWMSGQ_SEND:
        if (MQX_OK == (res = _lwmsgq_usr_check((LWMSGQ_STRUCT_PTR)param0)) && \
            _psp_mem_check_access(param1, ((LWMSGQ_STRUCT_PTR)param0)->MSG_SIZE, MPU_UM_RW))
            res = _lwmsgq_send((void *)param0, (_mqx_max_type_ptr)param1, (_mqx_uint)param2);
        break;
#endif // MQX_USE_LWMSGQ

    case MQX_API_TASK_CREATE:
        res = _task_create_internal((_processor_number)param0, (_mqx_uint)param1, (uint32_t)param2, TRUE);
        break;
    case MQX_API_TASK_DESTROY:
        res = _task_destroy_internal((_task_id)param0, TRUE);
        break;
    case MQX_API_TASK_ABORT:
        res = _task_abort_internal((_task_id)param0, TRUE);
        break;
    case MQX_API_TASK_READY:
        _task_ready((void *)param0);
        res = MQX_OK;       // irelevant, function is without return value
        break;
    case MQX_API_TASK_SET_ERROR:
        res = _task_set_error((_mqx_uint)param0);
        break;
    case MQX_API_TASK_GET_TD:
        res = (uint32_t)_task_get_td((_task_id)param0);
        break;

#if MQX_USE_LWMEM
    case MQX_API_LWMEM_ALLOC:
        res = (uint32_t)_usr_lwmem_alloc_internal((_mem_size)param0);
        break;

    case MQX_API_LWMEM_ALLOC_FROM:
        if (_psp_mem_check_access(param0, sizeof(LWMEM_POOL_STRUCT), MPU_UM_RW) && \
            _psp_mem_check_access((uint32_t)(((LWMEM_POOL_STRUCT_PTR)param0)->POOL_ALLOC_START_PTR), (char*)(((LWMEM_POOL_STRUCT_PTR)param0)->POOL_ALLOC_END_PTR) - (char*)(((LWMEM_POOL_STRUCT_PTR)param0)->POOL_ALLOC_START_PTR), MPU_UM_RW))
            res = (uint32_t)_lwmem_alloc_from((_lwmem_pool_id)param0, (_mem_size)param1);
        else
            res = 0; // NULL, allocation failed
        break;

    case MQX_API_LWMEM_FREE:
        if (_psp_mem_check_access(param0, 4, MPU_UM_RW))
            res = _lwmem_free((void *)param0);
        break;

    case MQX_API_LWMEM_CREATE_POOL:\
        if (_psp_mem_check_access(param0, sizeof(LWMEM_POOL_STRUCT), MPU_UM_RW) && \
            _psp_mem_check_access(param1, param2, MPU_UM_RW))
            res = (uint32_t)_lwmem_create_pool((LWMEM_POOL_STRUCT_PTR)param0, (void *)param1, (_mem_size)param2);
        break;
#endif // MQX_USE_LWMEM

    // _time
    case MQX_API_TIME_DELAY:
        _time_delay(param0);
        res = MQX_OK;       // irelevant, function is without return value
        break;

#if MQX_HAS_TICK
    case MQX_API_TIME_DELAY_TICKS:
        _time_delay_ticks(param0);
        res = MQX_OK;       // irelevant, function is without return value
        break;
    case MQX_API_TIME_GET_ELAPSED_TICKS:
        if (_psp_mem_check_access(param0, sizeof(MQX_TICK_STRUCT), MPU_UM_RW)) {
            _time_get_elapsed_ticks((MQX_TICK_STRUCT_PTR)param0);
            res = MQX_OK;       // irelevant, function is without return value
        }
        else {
            _task_set_error(MQX_ACCESS_ERROR);
        }

        break;
#endif // MQX_HAS_TICK
    default:
        while (1);
    }

    return res;
}

#endif // MQX_ENABLE_USER_MODE
