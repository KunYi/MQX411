/*HEADER**********************************************************************
*
* Copyright 2012 Freescale Semiconductor, Inc.
* Copyright 1989-2012 ARC International
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
*   This file contains the source for the hello example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>

#include <core_mutex.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

extern void test_task(uint32_t);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   /* Task Index,   Function,   Stack,  Priority, Name,     Attributes,          Param, Time Slice */
    { 1,   test_task, 1500,   8,        "test_task",  MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};


// Declare some shared data
PACKED_STRUCT_BEGIN
struct shared_data_struct {
    uint32_t count;
    uint32_t core_count[2];
} PACKED_STRUCT_END;

typedef struct shared_data_struct SHARED_DATA_STRUCT, * SHARED_DATA_STRUCT_PTR;



/*TASK*-----------------------------------------------------
*
* Task Name    : test_task
* Comments     :
*    This task tests the core mutex access
*
*END*-----------------------------------------------------*/
void test_task(uint32_t initial_data)
{
    CORE_MUTEX_PTR cm_ptr;
    uint32_t        iteration=0;
    SHARED_DATA_STRUCT_PTR shared_data_ptr;
    bool                 corenum = _psp_core_num();

    _time_delay(1000);
    _DCACHE_DISABLE();

    cm_ptr = _core_mutex_create( 0, 1, MQX_TASK_QUEUE_FIFO );

    _core_mutex_lock(cm_ptr);
    // place shared memory in core 1's shared memory area.
    // A more complex application would want a managed pool there
    //shared_data_ptr = corenum ? (SHARED_DATA_STRUCT_PTR)BSP_SHARED_RAM_START : (SHARED_DATA_STRUCT_PTR)BSP_REMOTE_SHARED_RAM_START;
    shared_data_ptr = (SHARED_DATA_STRUCT_PTR)BSP_SHARED_RAM_START;
    _mem_zero(shared_data_ptr, sizeof(*shared_data_ptr));
    _core_mutex_unlock(cm_ptr);

    while (1) {
        _core_mutex_lock(cm_ptr);
        shared_data_ptr->count++;
        shared_data_ptr->core_count[corenum]++;
        //_time_delay((uint32_t)rand() % 20 );
        _core_mutex_unlock(cm_ptr);
        //_time_delay((uint32_t)rand() % 20 );

        iteration++;
        if (((iteration % 1000) == 0) && (corenum==1)) {
            _core_mutex_lock(cm_ptr);
            printf("Count = %d, core0_count=%d, core1_count=%d\n",
                shared_data_ptr->count,
                shared_data_ptr->core_count[0],
                shared_data_ptr->core_count[1]);
            if (shared_data_ptr->count != shared_data_ptr->core_count[0] + shared_data_ptr->core_count[1]) {
                /* the total count got overwritten because of a broken lock */
                printf("Core mutex failed!\n");
                _task_block();
                break;
            }
            _core_mutex_unlock(cm_ptr);
        }
    }
}
