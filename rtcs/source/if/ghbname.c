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
*   This file contains the gethostbyname() function.
*
*
*END************************************************************************/

#include <string.h>
#include <rtcs.h>

static bool RTCS_get_dotted_address (char *name, uint32_t *address)
{
   uint32_t                     i;
   uint32_t                     digit_count = 0;
   uint32_t                     dot_count = 0;
   int32_t                      byte_num;
   bool                     dotted_decimal = TRUE;
   
   if ((name == NULL) || (address == NULL)) return FALSE;
   
   *address = 0;
   
   for (i=0; name[i] != '\0'; ++i ) {
      if ( name[i] == '.' ) {
         dot_count++;
         if ( dot_count > 3 ) {
            /* invalid IP address */
            dotted_decimal = FALSE;
            break;
         }/* Endif */

         if ( digit_count == 0 ) {
            /* there are no digits before the '.' */
            dotted_decimal = FALSE;
            break;
         }/* Endif */
         digit_count = 0;
         byte_num = byte_num / 10; /* shift back */

         if ( (byte_num < 0 ) || (byte_num > 255) ) {
         /* if the number does fall within this range it's invalid */
            dotted_decimal = FALSE;
            break;
         } else  {
            *address = (*address) + byte_num;
            *address = (*address) * 0x100;
         }/* Endif */
      } else { /* a digit */

         if ( digit_count == 0 ) {
            byte_num = 0;
         }/* Endif */
         ++digit_count;

         if ( digit_count > 3 ) {
            /* too many digits between the '.' */
            dotted_decimal = FALSE;
            break;
         }/* Endif */

         if ( (name[i] >= '0') && (name[i] <= '9') ) {
            /* number is in range */
            byte_num = byte_num + name[i] - '0';
            byte_num = byte_num * 10;
         } else {
            /* if the characters are not decimal digits it's invalid */
            dotted_decimal = FALSE;
            break;
         }/* Endif */
      }/* Endif */
   } /* Endfor */

   if ( dotted_decimal ) { /* check last number */
      if ( digit_count == 0 ) {
         /* there are no digits before the '.' */
         dotted_decimal = FALSE;
      }/* Endif */

      byte_num = byte_num / 10;
      if ( (byte_num < 0 ) || (byte_num > 255) ) {
         /* if the number does fall within this range it's invalid */
         dotted_decimal = FALSE;
      } else {
         *address = (*address) + byte_num;
      }/* Endif */

      if ( dot_count != 3 ) {
         /* the wrong number of dots were found */
         dotted_decimal = FALSE;
      }/* Endif */

   }/* Endif */


   if ( i == 0 ) {
      /* no name string of first char was not a number */
      dotted_decimal = FALSE;
   }/* Endif */
    
   return dotted_decimal;
}






#if RTCSCFG_ENABLE_LWDNS && RTCSCFG_ENABLE_UDP
bool RTCS_resolve_ip_address( char *arg, _ip_address  *ipaddr_ptr, char *ipname, uint32_t ipnamesize)
{
    bool result = FALSE;

    if(arg && ipaddr_ptr)
    {
        /* first check to see if name is written in dotted decimal IP format */
        if(RTCS_get_dotted_address (arg, ipaddr_ptr))
        {
            /* Name is in dotted decimal format */
            result = TRUE;
        }
        else
        {
            struct addrinfo     addrinfo_hints;
            struct addrinfo     *addrinfo_result;
                
            _mem_zero(&addrinfo_hints, sizeof(addrinfo_hints));
            addrinfo_hints.ai_flags = AI_CANONNAME;
            addrinfo_hints.ai_family = AF_INET;

            if(getaddrinfo(arg, NULL, &addrinfo_hints, &addrinfo_result) == 0) 
            {
                *ipaddr_ptr = ((struct sockaddr_in *)((*addrinfo_result).ai_addr))->sin_addr.s_addr;
                if(ipname && addrinfo_result->ai_canonname)    /* Canonical name.*/
                {
                    strncpy(ipname, addrinfo_result->ai_canonname, ipnamesize);
                }
                freeaddrinfo(addrinfo_result);
                result = TRUE; /* Resolved.*/
            }
        }
    }

    return result;
}



#elif RTCSCFG_ENABLE_DNS && RTCSCFG_ENABLE_UDP



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_gethostbyname()
* Returned Value  : HOSTENT_STRUCT_PTR
* Comments        : Maps a host name to an IP address
*
*END*-----------------------------------------------------------------*/

HOSTENT_STRUCT_PTR RTCS_gethostbyname
   (
      char  *name
   )
{  /* Body */
   INTERNAL_HOSTENT_STRUCT_PTR host_ptr;
   uint32_t                     i;
   bool                     name_found;
   bool                     dotted_decimal;
   uint32_t                     num_aliases = 0;
   uint32_t                     host_index = 0;
   uint32_t                     temp;

   /* allocate memory for HOSTENT_STRUCT */
   host_ptr = RTCS_mem_alloc_zero(sizeof(INTERNAL_HOSTENT_STRUCT));
   if ( host_ptr == NULL ) {
      return(NULL);
   }/* Endif */
   _mem_set_type(host_ptr, MEM_TYPE_HOSTENT_STRUCT);
   
   host_ptr->HOSTENT.h_aliases = &host_ptr->ALIASES[0];
   host_ptr->HOSTENT.h_addr_list = (char **)&host_ptr->ADDRESSES[0];

   /* assign address type and length */
   host_ptr->HOSTENT.h_addrtype = AF_INET;
   host_ptr->HOSTENT.h_length = 4;     /* IP addresses only used - 4 bytes */

   /* check to see if name is written in dotted decimal IP format */
   dotted_decimal = RTCS_get_dotted_address (name, &temp);

   if ( dotted_decimal == TRUE ) {
      /* assign the hostent struct from the dotted decimal IP name */
      host_ptr->HOSTENT.h_name = name;
      host_ptr->ALIASES[num_aliases] = NULL;
      host_ptr->IP_address = temp;
      host_ptr->ADDRESSES[0] = &host_ptr->IP_address;
      host_ptr->ADDRESSES[1] = NULL;
      name_found = TRUE;
   } else  {

      name_found = FALSE;

     /* search for the name in the hosts structure */
      while ( ((RTCS_Hosts_list[host_index]).ip_address != 0) && !name_found) {
      /*end of list hasn't been reached*/
         /* check the aliases for the name */
         i=0;
         while ( ((RTCS_Hosts_list[host_index]).aliases[i] != NULL) && !name_found) {
            if ( strcmp((RTCS_Hosts_list[host_index]).aliases[i],name) == 0 ) {
               name_found = TRUE;
            }/* Endif */
            ++i;
         } /* Endwhile */

         /* check the name field for the name */
         if ( strcmp((RTCS_Hosts_list[host_index]).host_name, name) == 0)  {
            name_found = TRUE;
         } /* Endif */

         if (name_found == TRUE ) { /* host name was found */

            host_ptr->HOSTENT.h_name = (RTCS_Hosts_list[host_index]).host_name;

            /* assign alias list */
            while ( (num_aliases < MAX_HOST_ALIASES)
                     && (RTCS_Hosts_list[host_index]).aliases[num_aliases] != NULL) {

               host_ptr->ALIASES[num_aliases] = (RTCS_Hosts_list[host_index]).aliases[num_aliases];
               ++num_aliases;
            } /* Endwhile */
            host_ptr->ALIASES[num_aliases] = NULL;

            /* assign addresses (in our case only one IP address is allowed) */

            host_ptr->ADDRESSES[0] = (uint32_t *)&((RTCS_Hosts_list[host_index]).ip_address);
            host_ptr->ADDRESSES[1] = NULL;
         } /* Endif */
         ++host_index;
      } /* Endwhile */
   }/* Endif */


   if ( !name_found ) {
      _mem_free(host_ptr);
      return( NULL );
   }/* Endif */

   return( &host_ptr->HOSTENT );

}/* EndBody */


bool RTCS_resolve_ip_address( char *arg, _ip_address  *ipaddr_ptr, char *ipname, uint32_t ipnamesize)
{
    HOSTENT_STRUCT_PTR   host_ptr;
    bool              result = FALSE;

   if (ipaddr_ptr == NULL) return FALSE;
   
   *ipaddr_ptr = 0;
   if (*arg != '\0') {
      host_ptr = gethostbyname(arg);
      if (host_ptr != NULL)  {
         if (*host_ptr->h_addr_list != NULL) {
            *ipaddr_ptr = *(_ip_address *)*host_ptr->h_addr_list;
            strncpy(ipname, host_ptr->h_name, ipnamesize);
            result = TRUE;
         } 
         DNS_free_HOSTENT_STRUCT(host_ptr);
      } 
   } 
   
   return result;
}



#else



bool RTCS_resolve_ip_address( char *arg, _ip_address  *ipaddr_ptr, char *ipname, uint32_t ipnamesize)
{
    return RTCS_get_dotted_address (arg, ipaddr_ptr);
}



#endif

/* EOF */
