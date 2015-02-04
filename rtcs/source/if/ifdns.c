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
*   This file contains the Domain Name System user interface
*   functions for RTCS.
*
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"

#if RTCSCFG_ENABLE_UDP
#if RTCSCFG_ENABLE_DNS

#include <string.h>
#include "dns.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_gethostbyname()
* Returned Value  : DNS_RESOURCE_RECORD_STRUCT_PTR
* Comments        : Maps a host name to an IP address
*
*END*-----------------------------------------------------------------*/

HOSTENT_STRUCT  *DNS_gethostbyname
   (
   char   *name_ptr
   )
{   /* Body */
   HOSTENT_STRUCT         *host_ptr = NULL;
   DNS_SLIST_STRUCT       *slist_ptr;
   
   char      *full_name_ptr;
   /*
   ** First check to see if the information exists in the RTCS_host.c
   ** file.  This check is included for backwards compatibility
   ** with previous versions of RTCS
   */

   host_ptr = DNS_check_local_host_file((unsigned char *)name_ptr);

   /* Try to query all DNS servers from the DNS list */
   slist_ptr = DNS_First_Local_server;
   while (( slist_ptr != NULL ) && ( host_ptr == NULL )) {
      host_ptr = DNS_query_resolver_task((unsigned char *)name_ptr, DNS_A);
      slist_ptr = slist_ptr->NEXT;
   }

   /*
    * In case this is a fully qualified dotted domain name without the 
    * trailing "." Root domain, append the trailing "." and try again.
    */
   if ( host_ptr == NULL ) {
      /* allocate string + dot + null */    
      full_name_ptr = (char *)RTCS_mem_alloc(strlen(name_ptr) +1 +1); 
      if (full_name_ptr == NULL){
          return NULL;  
      }          
      strcpy(full_name_ptr, name_ptr);
      strcat(full_name_ptr, ".");
      
      slist_ptr = DNS_First_Local_server;
      while (( slist_ptr != NULL ) && ( host_ptr == NULL )) {
         host_ptr = DNS_query_resolver_task((unsigned char *)full_name_ptr, DNS_A);
         slist_ptr = slist_ptr->NEXT;
      }
      
      _mem_free(full_name_ptr);

   }/* Endif */

   return( host_ptr );

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_gethostbyaddr()
* Returned Value  : DNS_RESOURCE_RECORD_STRUCT_PTR
* Comments        : Maps a host name to an IP address
*
*END*-----------------------------------------------------------------*/

HOSTENT_STRUCT  *DNS_gethostbyaddr
   (
   char  *addr_ptr,
   uint32_t    len,
   uint32_t    type
   )
{   /* Body */
   HOSTENT_STRUCT        *host_ptr = NULL;
   DNS_SLIST_STRUCT      *slist_ptr;

   /*
   ** This is only included to conform to how UNIX handles gethostbyaddr,
   ** internally, RTCS DNS can handle other types.
   */
   if ((type != AF_INET) || (len != sizeof(_ip_address))) {
      return NULL;
   } /* Endif */

   slist_ptr = DNS_First_Local_server;
   while (( slist_ptr != NULL ) && ( host_ptr == NULL )) {
      host_ptr = DNS_query_resolver_task((unsigned char *)addr_ptr, DNS_PTR );
      slist_ptr = slist_ptr->NEXT;
   }

   return( host_ptr );

} /* Endbody */


/* Start CR 2257 */
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_find_slist_entry()
* Returned Value  : DNS_SLIST_STRUCT 
** Comments        : 
*
*END*-----------------------------------------------------------------*/

DNS_SLIST_STRUCT  *DNS_find_slist_entry
   (
   _ip_address ipaddr
   )
{ /* Body */

   DNS_SLIST_STRUCT      *temp_ptr;

   if (ipaddr == 0) 
   {
      return( NULL );
   }

   temp_ptr = DNS_First_Local_server;
   while ( temp_ptr != NULL ) 
   {
      if (temp_ptr->IPADDR == ipaddr)
      {
         return(temp_ptr);
      }
      temp_ptr = temp_ptr->NEXT;
   }
   return (NULL);
   
} /* End Body */
 
 
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_init_slist_entry()
* Returned Value  : void
* Comments        : Init slist record to default values
*
*END*-----------------------------------------------------------------*/

void DNS_init_slist_entry
   (
   DNS_SLIST_STRUCT    *slist_entry_ptr
   )
{ /* Body */

   slist_entry_ptr->NAME_PTR = DNS_Local_server_name;
   slist_entry_ptr->NEXT = NULL;
   slist_entry_ptr->IPADDR = INADDR_LOOPBACK;
   slist_entry_ptr->LAST_TTL_CHECK = 0;
   slist_entry_ptr->TTL = 0;
   slist_entry_ptr->NUMBER_OF_HITS = 0;
   slist_entry_ptr->NUMBER_OF_TRIES = 0;
   slist_entry_ptr->AVG_RESPONSE_TIME = 0;
   slist_entry_ptr->TYPE = DNS_A;
   slist_entry_ptr->CLASS = DNS_IN;
   
} /* Endbody */


/* Flags for second parameter of DNS_insert_slist_entry() */
#define DNS_APPEND_SLIST_ENTRY              0x01
#define DNS_PREPEND_SLIST_ENTRY             0x02

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_insert_slist_entry()
* Returned Value  : uint32_t
* Comments        : This is called by the publically visible
*                   DNS_add_slist_entry() and
*                   DNS_append_slist_entry().
*
*END*-----------------------------------------------------------------*/

static uint32_t DNS_insert_slist_entry
   (
   DNS_SLIST_STRUCT  *usr_slist_entry_ptr,
   uint32_t add_flag
   )

{ /* Body */

   DNS_SLIST_STRUCT      *new_slist_entry_ptr;
   DNS_SLIST_STRUCT      *tmp_slist_entry_ptr;

   if (usr_slist_entry_ptr == NULL) 
   {
      return RTCSERR_DNS_INVALID_IP_ADDR;
   }
   
   if (usr_slist_entry_ptr->IPADDR == 0)
   {
      return RTCSERR_DNS_INVALID_IP_ADDR;
   }

   /*
    * If an entry with the same ipaddr already exists, update it
    */
   tmp_slist_entry_ptr = DNS_find_slist_entry(usr_slist_entry_ptr->IPADDR);
   if (tmp_slist_entry_ptr) 
   {
      DNS_SLIST_STRUCT      *tmp_ptr;
      char * tmp_str = NULL;
   
      if (usr_slist_entry_ptr->NAME_PTR)
      {
         tmp_str = RTCS_mem_alloc(strlen(usr_slist_entry_ptr->NAME_PTR));
         if (tmp_str == NULL)
            return RTCSERR_DNS_UNABLE_TO_ALLOCATE_MEMORY;
         strcpy(tmp_str, usr_slist_entry_ptr->NAME_PTR);
      }
      
      if (tmp_slist_entry_ptr->NAME_PTR)
         _mem_free(tmp_slist_entry_ptr->NAME_PTR);
   
      tmp_ptr = tmp_slist_entry_ptr->NEXT;
      
      memcpy(tmp_slist_entry_ptr, usr_slist_entry_ptr, sizeof(DNS_SLIST_STRUCT));
      
      tmp_slist_entry_ptr->NEXT = tmp_ptr;
      tmp_slist_entry_ptr->NAME_PTR = tmp_str;
      
      return DNS_OK;
   }
   
   new_slist_entry_ptr = RTCS_mem_alloc(sizeof(DNS_SLIST_STRUCT));
   if (new_slist_entry_ptr == NULL) 
   {
      return RTCSERR_DNS_UNABLE_TO_ALLOCATE_MEMORY;
   }
   
   memcpy(new_slist_entry_ptr, usr_slist_entry_ptr, sizeof(DNS_SLIST_STRUCT));
   
   new_slist_entry_ptr->NAME_PTR = RTCS_mem_alloc(strlen(usr_slist_entry_ptr->NAME_PTR) + 1);
   if (new_slist_entry_ptr->NAME_PTR == NULL)
   {
      _mem_free(new_slist_entry_ptr);
      return RTCSERR_DNS_UNABLE_TO_ALLOCATE_MEMORY;
   }
   strcpy(new_slist_entry_ptr->NAME_PTR, usr_slist_entry_ptr->NAME_PTR);
 
   /* If the list is empty, make the new entry the first entry */
   if (DNS_First_Local_server == NULL)
   {
      DNS_First_Local_server = new_slist_entry_ptr;
   }
   else
   {
      /* prepend or append? */
      switch (add_flag)
      {
         default:
         case DNS_PREPEND_SLIST_ENTRY:
            new_slist_entry_ptr->NEXT = DNS_First_Local_server;
            DNS_First_Local_server = new_slist_entry_ptr;
            break;
         case DNS_APPEND_SLIST_ENTRY:
            tmp_slist_entry_ptr = DNS_First_Local_server;
            while (tmp_slist_entry_ptr->NEXT)
            {
               tmp_slist_entry_ptr = tmp_slist_entry_ptr->NEXT;
            }
            tmp_slist_entry_ptr->NEXT = new_slist_entry_ptr;
            break;
      }
   }
   
   return( DNS_OK );

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_append_slist_entry()
* Returned Value  : uint32_t
* Comments        :
*
*END*-----------------------------------------------------------------*/

uint32_t DNS_append_slist_entry
   (
   DNS_SLIST_STRUCT  *usr_slist_entry_ptr
   )
{
   uint32_t ret;
   DNS_CONTROL_STRUCT      *control_ptr = RTCS_getcfg(DNS);
   
   RTCS_mutex_lock(&control_ptr->SLIST_MUTEX);
   ret = (DNS_insert_slist_entry(usr_slist_entry_ptr, DNS_APPEND_SLIST_ENTRY));
   RTCS_mutex_unlock(&control_ptr->SLIST_MUTEX);

   if (ret == DNS_OK)
   {
      control_ptr->LOCAL_SLIST_PTR = DNS_First_Local_server;
   }

   return ret;
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_add_slist_entry()
* Returned Value  : uint32_t
* Comments        :
*
*END*-----------------------------------------------------------------*/

uint32_t DNS_prepend_slist_entry
   (
   DNS_SLIST_STRUCT  *usr_slist_entry_ptr
   )
{
   uint32_t ret;
   DNS_CONTROL_STRUCT      *control_ptr = RTCS_getcfg(DNS);
   
   RTCS_mutex_lock(&control_ptr->SLIST_MUTEX);
   ret = (DNS_insert_slist_entry(usr_slist_entry_ptr, DNS_PREPEND_SLIST_ENTRY));
   RTCS_mutex_unlock(&control_ptr->SLIST_MUTEX);
   
   if (ret == DNS_OK)
   {
      control_ptr->LOCAL_SLIST_PTR = DNS_First_Local_server;
   }
   
   return ret;
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_remove_slist_entry()
* Returned Value  : uint32_t
* Comments        : Remove slist record from list
*
*END*-----------------------------------------------------------------*/

uint32_t DNS_remove_slist_entry
   (
   _ip_address ipaddr
   )

{ /* Body */

   DNS_SLIST_STRUCT      *prev_ptr = NULL;
   DNS_SLIST_STRUCT      *temp_ptr = NULL;
   DNS_CONTROL_STRUCT      *control_ptr = RTCS_getcfg(DNS);

   /* 
   ** If ipaddr is 0, return error
   */
   if (ipaddr == 0) 
   {
      return( RTCSERR_DNS_INVALID_IP_ADDR );
   }

   temp_ptr = DNS_First_Local_server;

   /*
   ** find entry in list
   */
   while ( temp_ptr != NULL ) 
   {
      if (temp_ptr->IPADDR == ipaddr) 
      {
         /*
         ** Is it the first added entry?
         */
         if (prev_ptr == NULL) 
         {
            DNS_First_Local_server = temp_ptr->NEXT;
            control_ptr->LOCAL_SLIST_PTR = DNS_First_Local_server;
         }
         else 
         {
            prev_ptr->NEXT = temp_ptr->NEXT;
         }
         _mem_free(temp_ptr->NAME_PTR);
         _mem_free(temp_ptr);
         break;
      }
      prev_ptr = temp_ptr;
      temp_ptr = temp_ptr->NEXT;
   }

   /*
    * Never leave the list empty
    */
   if (DNS_First_Local_server == NULL)
   {
      DNS_SLIST_STRUCT tmp_slist_entry;
      DNS_init_slist_entry(&tmp_slist_entry);
      return( DNS_add_slist_entry(&tmp_slist_entry));
   }
   
   return( DNS_OK );

} /* Endbody */

/* Cleanup local defines */
#undef DNS_APPEND_SLIST_ENTRY
#undef DNS_PREPEND_SLIST_ENTRY
/* End CR 2257 */

#endif
#endif
/* EOF */
