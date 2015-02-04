#ifndef _mem_management_internal_h_
#define _mem_management_internal_h_
/*HEADER**********************************************************************
*
* Copyright 2012 Freescale Semiconductor, Inc.
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
*   Memory management
*
*END************************************************************************/

typedef struct _mem_entry_head_struct {
    // uint8_t file_name[15];
    const char* file_name;
    int         file_loc;
    uint32_t     requested_size;
    bool     is_from_operator;
    // TODO: 64bits signature does not support on IAR
    // uint64_t   buf_head_sign; 
    uint32_t     buf_head_sign;
} MEM_ENTRY_HEAD, * MEM_ENTRY_HEAD_PTR;

typedef struct _mem_entry_tail {
    // TODO: 64bits signature does not support on IAR
    // uint64_t buf_tail_sign;
    uint32_t buf_tail_sign;
} MEM_ENTRY_TAIL, * MEM_ENTRY_TAIL_PTR;

#define MEM_MAX_SAVE_BUF_PTR    200
typedef struct _mem_allocation_table_struct {
    uint32_t totalAllocSize;
    uint32_t totalFreedSize;
    uint32_t totalNewSize;
    uint32_t totalDeleteSize;
    MEM_ENTRY_HEAD_PTR buf_ptr[MEM_MAX_SAVE_BUF_PTR];
    uint32_t nextSaveIdx;
// TODO: Fix race-condition with a semaphore */
} MEM_ALLOCATION_TABLE, * MEM_ALLOCATION_TABLE_PTR;

extern MEM_ALLOCATION_TABLE g_mem_allocation_table;

#endif //_mem_management_internal_h_

/* EOF */


