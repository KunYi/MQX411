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
*   This file contains the Domain Name System resolver
*   initialization functions for RTCS.
*
*
*END************************************************************************/


#include <rtcs.h>
#include "rtcs_prv.h"

#if RTCSCFG_ENABLE_UDP
#if RTCSCFG_ENABLE_DNS 

#include "dns.h"


/* Start CR 2170 */
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_init_unwind
* Returned Value  : void
* Comments        : cleans up after a DNS_init error
*
*END*-----------------------------------------------------------------*/

static void DNS_init_unwind
   (
      DNS_CONTROL_STRUCT  *control_ptr,
      uint32_t                  sock

   )
{ /* Body */
   RTCS_setcfg(DNS, NULL);
   if (sock)
      shutdown(sock, 0);
   if (control_ptr) {
      if (control_ptr->CNAME_PARTID)
         RTCS_part_destroy(control_ptr->CNAME_PARTID);
      if (control_ptr->RR_LIST_PARTID)
         RTCS_part_destroy(control_ptr->RR_LIST_PARTID);
      if (control_ptr->NAME_RR_PARTID)
         RTCS_part_destroy(control_ptr->NAME_RR_PARTID);
      if (control_ptr->INT_QUERY_PARTID)
         RTCS_part_destroy(control_ptr->INT_QUERY_PARTID);
      if (control_ptr->ADDR_RR_PARTID)
         RTCS_part_destroy(control_ptr->ADDR_RR_PARTID);
      _mem_free(control_ptr);
   }
   return;
} /* Endbody */
/* End CR 2170 */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_init
* Returned Value  : DNS_OK or error code
* Comments        : Initializes the DNS task
*
*END*-----------------------------------------------------------------*/

uint32_t DNS_init
   (
      void
   )

{ /* Body */
   DNS_CONTROL_STRUCT      *control_ptr = RTCS_getcfg(DNS);
   uint32_t                  temp_socket = 0;
   int32_t                   error;

   if (control_ptr != NULL) {
      return RTCS_OK;
   }

   /*
   ** Please note, if this is modified to use a value other than
   ** DNS_MAX_UDP_MESSAGE_SIZE, the recvfrom() in the DNS_Resolver_task
   ** will need to be modified as well, as they rely on this constant.
   */
   control_ptr = RTCS_mem_alloc_system_zero(sizeof(DNS_CONTROL_STRUCT)+DNS_MAX_UDP_MESSAGE_SIZE);
   if (control_ptr == NULL) {
      return RTCSERR_DNS_UNABLE_TO_ALLOCATE_MEMORY;
   } /* Endif */
   _mem_set_type(control_ptr, MEM_TYPE_DNS_CONTROL_STRUCT);

   RTCS_setcfg(DNS, control_ptr);

   temp_socket = socket(PF_INET, SOCK_DGRAM, 0);
   if (temp_socket == RTCS_HANDLE_ERROR) {
      DNS_init_unwind(control_ptr, temp_socket);
      return RTCSERR_DNS_UNABLE_TO_OPEN_SOCKET;
   } /* Endif */

   error = RTCS_detachsock(temp_socket);
   if (error != RTCS_OK) {
      DNS_init_unwind(control_ptr, temp_socket);
      return error;
   } /* Endif */

   control_ptr->INT_QUERY_PARTID  = RTCS_part_create(
      sizeof(DNS_INTERNAL_QUERY_STRUCT), DNSALLOC_SIZE, DNSALLOC_SIZE, 0,
      NULL, NULL);
   if (control_ptr->INT_QUERY_PARTID == 0) {
      DNS_init_unwind(control_ptr, temp_socket);
      return RTCSERR_DNS_UNABLE_TO_CREATE_PARTITION;
   } /* Endif */

   control_ptr->ADDR_RR_PARTID  = RTCS_part_create(
      sizeof(DNS_ADDR_RECORD_STRUCT), DNSALLOC_SIZE, DNSALLOC_SIZE, 0,
      NULL, NULL);
   if (control_ptr->ADDR_RR_PARTID == 0) {
      DNS_init_unwind(control_ptr, temp_socket);
      return RTCSERR_DNS_UNABLE_TO_CREATE_PARTITION;
   } /* Endif */

   control_ptr->NAME_RR_PARTID  = RTCS_part_create(
      sizeof(DNS_NAME_RECORD_STRUCT), DNSALLOC_SIZE, DNSALLOC_SIZE, 0,
      NULL, NULL);
   if (control_ptr->NAME_RR_PARTID == 0) {
      DNS_init_unwind(control_ptr, temp_socket);
      return RTCSERR_DNS_UNABLE_TO_CREATE_PARTITION;
   } /* Endif */

   control_ptr->RR_LIST_PARTID  = RTCS_part_create(
      sizeof(DNS_RR_LIST_STRUCT), DNSALLOC_SIZE, DNSALLOC_SIZE, 0,
      NULL, NULL);
   if (control_ptr->RR_LIST_PARTID == 0) {
      DNS_init_unwind(control_ptr, temp_socket);
      return RTCSERR_DNS_UNABLE_TO_CREATE_PARTITION;
   } /* Endif */

   control_ptr->CNAME_PARTID  = RTCS_part_create(
      sizeof(DNS_CNAME_STRUCT), DNSALLOC_SIZE, DNSALLOC_SIZE, 0,
      NULL, NULL);
   if (control_ptr->CNAME_PARTID == 0) {
      DNS_init_unwind(control_ptr, temp_socket);
      return RTCSERR_DNS_UNABLE_TO_CREATE_PARTITION;
   } /* Endif */

   control_ptr->ROOT_SLIST_PTR  = DNS_A_Root_server;
   control_ptr->LOCAL_SLIST_PTR = DNS_First_Local_server;
   control_ptr->NS_BUFFER       = NULL;
   control_ptr->RES_BUFFER      = (unsigned char *)(control_ptr + 1);
   control_ptr->RES_SOCKET      = temp_socket;
   control_ptr->UPDATING        = FALSE;

   error = RTCS_mutex_init(&control_ptr->CACHE_MUTEX);
   if (error != RTCS_OK) {
      DNS_init_unwind(control_ptr, temp_socket);
      return error;
   } /* Endif */

   error = RTCS_mutex_init(&control_ptr->SLIST_MUTEX);
   if (error != RTCS_OK) {
      DNS_init_unwind(control_ptr, temp_socket);
      return error;
   } /* Endif */

   error = RTCS_task_create("DNS Resolver", _RTCSTASK_priority,
      _RTCSTASK_stacksize + 500 * sizeof(uint32_t),
      DNS_Resolver_task, control_ptr);
   if (error) {
      DNS_init_unwind(control_ptr, temp_socket);
      return error;
   } /* Endif */

   return RTCS_OK;

} /* Endbody */

#endif
#endif
/* EOF */
