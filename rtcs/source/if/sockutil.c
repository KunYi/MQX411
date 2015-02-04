/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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
*   This file contains utility functions for managing
*   socket structures.
*
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "tcp_prv.h"
#include "socket.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_Get_sock_struct
* Returned Value  : none
* Comments        : Get a socket structure, initialize it and return
*                     address of socket.
*
*END*-----------------------------------------------------------------*/

SOCKET_STRUCT_PTR   SOCK_Get_sock_struct
   (
      RTCS_SOCKET_CALL_STRUCT_PTR   type,
      _rtcs_taskid                  owner

   )
{ /* Body */
   RTCS_DATA_PTR              RTCS_data_ptr;
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr;
   SOCKET_STRUCT_PTR          socket_ptr;

   RTCS_data_ptr = RTCS_get_data();
   socket_cfg_ptr = RTCS_data_ptr->SOCKET_CFG;

   socket_ptr = RTCS_part_alloc_zero(RTCS_data_ptr->RTCS_socket_partition);

   if ( socket_ptr != NULL ) {

      RTCS_mutex_lock(&socket_cfg_ptr->SOCK_MUTEX);
      socket_cfg_ptr->CURRENT_SOCKETS++;

      #if RTCSCFG_SOCKET_OWNERSHIP
      SOCK_Add_owner(socket_ptr, owner);
      #endif
      
      /*
      ** link in this socket into the linked list of active sockets
      */
      if ( socket_cfg_ptr->SOCKET_HEAD != NULL ) {

         ((SOCKET_STRUCT_PTR)socket_cfg_ptr->SOCKET_TAIL)->NEXT =
            (void *)socket_ptr;
         socket_ptr->PREV = (SOCKET_STRUCT_PTR)socket_cfg_ptr->SOCKET_TAIL;

      } else {
         socket_cfg_ptr->SOCKET_HEAD = (void *)socket_ptr;
         socket_ptr->PREV = NULL;

      } /* Endif */

      socket_ptr->NEXT = NULL;
      socket_cfg_ptr->SOCKET_TAIL = (void *)socket_ptr;
      RTCS_mutex_unlock(&socket_cfg_ptr->SOCK_MUTEX);

      socket_ptr->VALID = SOCKET_VALID;

      /*
      ** set the default socket options
      */
      socket_ptr->CONNECT_TIMEOUT  = DEFAULT_CONNECT_TIMEOUT;
      socket_ptr->RETRANSMISSION_TIMEOUT = DEFAULT_RETRANSMISSION_TIMEOUT;
      socket_ptr->SEND_TIMEOUT     = DEFAULT_SEND_TIMEOUT;
      socket_ptr->RECEIVE_TIMEOUT  = DEFAULT_RECEIVE_TIMEOUT;
      socket_ptr->RECEIVE_PUSH     = DEFAULT_PUSH;
      socket_ptr->SEND_NOWAIT      = DEFAULT_SEND_NOWAIT;
      socket_ptr->SEND_WAIT        = DEFAULT_SEND_WAIT;
      socket_ptr->SEND_PUSH        = DEFAULT_PUSH;
      socket_ptr->RECEIVE_NOWAIT   = DEFAULT_RECEIVE_NOWAIT;
      socket_ptr->CHECKSUM_BYPASS  = DEFAULT_CHECKSUM_BYPASS;
      socket_ptr->ACTIVE           = DEFAULT_ACTIVE;
      socket_ptr->TBSIZE           = DEFAULT_TBSIZE;
      socket_ptr->RBSIZE           = DEFAULT_RBSIZE;
      socket_ptr->MAXRTO           = DEFAULT_MAXRTO;
      socket_ptr->MAXRCV_WND       = DEFAULT_MAXRCV_WND;
      socket_ptr->KEEPALIVE        = DEFAULT_KEEPALIVE;
      socket_ptr->NOWAIT           = DEFAULT_NOWAIT;
      socket_ptr->NO_NAGLE_ALGORITHM  = DEFAULT_NO_NAGLE_ALGORITHM;
      socket_ptr->NOSWRBUF         = DEFAULT_NOSWRBUF;
      socket_ptr->CALL_BACK        = DEFAULT_CALL_BACK;
      socket_ptr->TYPE_MIRROR      = (uint32_t)type;
      socket_ptr->APPLICATION_CALLBACK  = DEFAULT_CALLBACK;
      socket_ptr->TIMEWAIT_TIMEOUT = DEFAULT_TIMEWAIT_TIMEOUT;
      socket_ptr->DELAY_ACK = DEFAULT_DELAY_ACK;
#if RTCSCFG_ENABLE_IP6
      socket_ptr->LINK_OPTIONS.TX.HOP_LIMIT_MULTICAST = DEFAULT_IP6_MULTICAST_HOPS;
#endif

      socket_ptr->PROTOCOL     = type;

   } /* Endif */

   return(socket_ptr);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_Free_sock_struct
* Returned Value  : none
* Comments        : Delink and release a socket structure.
*
*END*-----------------------------------------------------------------*/

void SOCK_Free_sock_struct
   (
      SOCKET_STRUCT_PTR          socket_ptr
   )
{ /* Body */
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr = RTCS_getcfg(SOCKET);
   
   #if RTCSCFG_SOCKET_OWNERSHIP //FSL AB
       SOCKET_OWNER_STRUCT_PTR  owner_ptr;
       SOCKET_OWNER_STRUCT_PTR  free_ptr;
   #endif

   socket_ptr->VALID = 0;

#if RTCSCFG_SOCKET_OWNERSHIP
   owner_ptr = socket_ptr->OWNERS.NEXT;
   while (owner_ptr != NULL) {
      free_ptr = owner_ptr;
      owner_ptr = owner_ptr->NEXT;
      _mem_free(free_ptr);
   } /* Endwhile */
#endif

   RTCS_mutex_lock(&socket_cfg_ptr->SOCK_MUTEX);

   /*
   ** Fix up the head/next pointer of our predecessor.
   */
   if ( socket_ptr->PREV == NULL ) {
      socket_cfg_ptr->SOCKET_HEAD = socket_ptr->NEXT;
   } else {
      (socket_ptr->PREV)->NEXT = socket_ptr->NEXT;
   } /* Endif */

   /*
   ** Fix up the tail/prev pointer of our successor.
   */
   if ( socket_ptr->NEXT == NULL ) {
      socket_cfg_ptr->SOCKET_TAIL = socket_ptr->PREV;
   } else {
      (socket_ptr->NEXT)->PREV = socket_ptr->PREV;
   } /* Endif */

   /*
   ** Release the socket structure memory.
   */
   RTCS_part_free(socket_ptr);

   socket_cfg_ptr->CURRENT_SOCKETS--;
   RTCS_mutex_unlock(&socket_cfg_ptr->SOCK_MUTEX);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_Remove_owner
* Returned Value  : none
* Comments        :
*
*END*-----------------------------------------------------------------*/

bool SOCK_Remove_owner
   (
      SOCKET_STRUCT_PTR    socket_ptr,
      _rtcs_taskid         task_ptr
   )
{ /* Body */
#if RTCSCFG_SOCKET_OWNERSHIP
   SOCKET_OWNER_STRUCT_PTR owner_ptr;
   uint32_t                 i;

   owner_ptr = &socket_ptr->OWNERS;

   while (owner_ptr != NULL) {
      for (i=0;i<SOCKET_NUMOWNERS;i++) {
         if (owner_ptr->TASK[i] == task_ptr) {
            owner_ptr->TASK[i] = NULL;
            return TRUE;
         } /* Endif */
      } /* Endfor */
      owner_ptr = owner_ptr->NEXT;
   } /* Endwhile */
   return FALSE;
#else
   return TRUE;
#endif
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_Is_owner
* Returned Value  : none
* Comments        :
*
*END*-----------------------------------------------------------------*/

bool SOCK_Is_owner
   (
      SOCKET_STRUCT_PTR    socket_ptr,
      _rtcs_taskid         task_ptr
   )
{ /* Body */
#if RTCSCFG_SOCKET_OWNERSHIP
   SOCKET_OWNER_STRUCT_PTR owner_ptr;
   uint32_t                 i;

   owner_ptr = &socket_ptr->OWNERS;


   while (owner_ptr != NULL) {
      for (i=0;i<SOCKET_NUMOWNERS;i++) {
         if (owner_ptr->TASK[i] == task_ptr) {
            /* already here, just return */
            return TRUE;
         } /* Endif */
      } /* Endfor */
      owner_ptr = owner_ptr->NEXT;
   } /* Endwhile */
   return FALSE;
#else
   return TRUE;
#endif
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_Add_owner
* Returned Value  : none
* Comments        :
*
*END*-----------------------------------------------------------------*/

bool SOCK_Add_owner
   (
      SOCKET_STRUCT_PTR    socket_ptr,
      _rtcs_taskid         task_ptr
   )
{ /* Body */
#if RTCSCFG_SOCKET_OWNERSHIP
   SOCKET_OWNER_STRUCT_PTR owner_ptr, new_owner_ptr;
   uint32_t                 i;
   void                  **saved_ptr = NULL;

   owner_ptr = &socket_ptr->OWNERS;

   while (owner_ptr != NULL) {
      for (i=0;i<SOCKET_NUMOWNERS;i++) {
         if (owner_ptr->TASK[i] == task_ptr) {
            /* already here, just return */
            return TRUE;
         } else if ((owner_ptr->TASK[i] == 0) && (saved_ptr == NULL)) {
            saved_ptr = &owner_ptr->TASK[i];
         } /* Endif */
      } /* Endfor */
      owner_ptr = owner_ptr->NEXT;
   } /* Endwhile */

   if (saved_ptr != NULL) {
      *saved_ptr = task_ptr;
   } else {
      new_owner_ptr = RTCS_mem_alloc_zero(sizeof(SOCKET_OWNER_STRUCT));
      if (new_owner_ptr == NULL) {
         return FALSE;
      } /* Endif */

      _mem_set_type(new_owner_ptr, MEM_TYPE_SOCKET_OWNER_STRUCT);

      new_owner_ptr->TASK[0] = task_ptr;
      owner_ptr->NEXT = new_owner_ptr;
   } /* Endif */
   return TRUE;
#else
   return TRUE;
#endif
} /* Endbody */


/* EOF */
