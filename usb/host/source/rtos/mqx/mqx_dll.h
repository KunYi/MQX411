#ifndef __mqx_dll_h__
#define __mqx_dll_h__
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
*   Doubly-linked list library header file
*
*
*END************************************************************************/

#include "mqx.h"

typedef struct mqx_dll_node
{
   struct mqx_dll_node      *next_ptr;
   struct mqx_dll_node      *prev_ptr;
} MQX_DLL_NODE, * MQX_DLL_NODE_PTR;

typedef struct mqx_dll_list
{
   struct mqx_dll_node      *head_ptr;
   struct mqx_dll_node      *tail_ptr;
} MQX_DLL_LIST, * MQX_DLL_LIST_PTR;

#define _mqx_dll_get_next_node(node_ptr, next_ptr_ptr)   (*(next_ptr_ptr) = (node_ptr)->next_ptr)
#define _mqx_dll_get_prev_node(node_ptr, prev_ptr_ptr)   (*(prev_ptr_ptr) = (node_ptr)->prev_ptr)
#define _mqx_dll_get_head_node(list_ptr, head_ptr_ptr)   (*(head_ptr_ptr) = (list_ptr)->head_ptr)
#define _mqx_dll_get_tail_node(list_ptr, tail_ptr_ptr)   (*(tail_ptr_ptr) = (list_ptr)->tail_ptr)

#define _mqx_dll_is_list_empty(list_ptr)                 ((list_ptr)->head_ptr == 0)
#define _mqx_dll_is_head_node(list_ptr, node_ptr)        ((node_ptr) == (list_ptr)->head_ptr)
#define _mqx_dll_is_tail_node(list_ptr, node_ptr)        ((node_ptr) == (list_ptr)->tail_ptr)
#define _mqx_dll_is_next_node(node_ptr, next_ptr)        ((next_ptr) == (node_ptr)->next_ptr)
#define _mqx_dll_is_prev_node(node_ptr, prev_ptr)        ((prev_ptr) == (node_ptr)->prev_ptr)

#ifdef __cplusplus
extern "C" {
#endif

void _mqx_dll_node_init
(
   MQX_DLL_NODE_PTR node_ptr
);

void _mqx_dll_list_init
(
   MQX_DLL_LIST_PTR list_ptr
);

void _mqx_dll_insert_at_head
(
   MQX_DLL_LIST_PTR list_ptr,
   MQX_DLL_NODE_PTR node_ptr
);

void _mqx_dll_insert_at_tail
(
   MQX_DLL_LIST_PTR list_ptr,
   MQX_DLL_NODE_PTR node_ptr
);

void _mqx_dll_insert_after_node
(
   MQX_DLL_LIST_PTR list_ptr,
   MQX_DLL_NODE_PTR node_ptr,
   MQX_DLL_NODE_PTR next_ptr
);

void _mqx_dll_insert_before_node
(
   MQX_DLL_LIST_PTR list_ptr,
   MQX_DLL_NODE_PTR node_ptr,
   MQX_DLL_NODE_PTR prev_ptr
);

void _mqx_dll_remove_node
(
   MQX_DLL_LIST_PTR list_ptr,
   MQX_DLL_NODE_PTR node_ptr
);

void _mqx_dll_create_sublist
(
   MQX_DLL_LIST_PTR  list_ptr,
   MQX_DLL_NODE_PTR  sublist_head_ptr,
   MQX_DLL_NODE_PTR  sublist_tail_ptr,
   MQX_DLL_LIST_PTR  sublist_ptr
);

void _mqx_dll_insert_sublist_at_tail
(
   MQX_DLL_LIST_PTR  sublist_ptr,
   MQX_DLL_LIST_PTR  list_ptr
);

void _mqx_dll_insert_sublist_at_head
(
   MQX_DLL_LIST_PTR  sublist_ptr,
   MQX_DLL_LIST_PTR  list_ptr
);

#ifdef __cplusplus
}
#endif

#endif
