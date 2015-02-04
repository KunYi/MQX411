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
*   This file contains the functions used exclusively by the
*   user interface functions RTCS_gethostbyname,
*   DNS_gethostbyname, and DNS_gethostbyaddr for RTCS.
*
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"

#if RTCSCFG_ENABLE_UDP
#if RTCSCFG_ENABLE_DNS

#include <ctype.h>
#include <string.h>
#include <fio.h>
#include "dns.h"

/*

GCC compiler did not have ctype in libc.
Used these definitions to resolve the problem:

#define tolower(c) (((c) >= 'A' && (c) <= 'Z') ? (c)-'A'+'a' : (c))
#define isalpha(c) (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
#define isdigit(c) ( (c) >= '0' && (c) <= '9' )
#define isalnum(c) (isalpha(c) || isdigit(c))

*/

/* Global variable used for gethostbyaddr and gethostbyname calls */
static INTERNAL_HOSTENT_STRUCT RTCS_HOST;
static unsigned char RTCS_HOST_NAMES[DNS_MAX_NAMES][DNS_MAX_CHARS_IN_NAME];
static _ip_address RTCS_HOST_ADDRS[DNS_MAX_ADDRS];

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_is_dotted_IP_addr()
* Returned Value  : char 
** Comments        :
*
*END*-----------------------------------------------------------------*/

uint32_t DNS_is_dotted_IP_addr
   (
   unsigned char  *name,
   unsigned char  *fill_buffer_ptr
   )

{  /* Body */

   uint32_t     i;
   uint32_t     j = 0;
   uint32_t     k;
   uint32_t     size = 0;
   uint32_t     digit_count = 0;
   uint32_t     dot_count = 0;
   int32_t      byte_num;
   uint32_t     addr_number = 0;
   unsigned char       number_chars[4];
   unsigned char       actual_number;
   unsigned char       addr[4][3];
   unsigned char      *number_buffer_ptr;
   unsigned char       appended_name[12] = {"in-addr.arpa"};
   bool     leading_zero;

   /*
   ** first check to see if name is written in correct dotted decimal
   ** IP address format
   */
   for (i=0; name[i] != '\0'; ++i ) {
      if ( name[i] == '.' ) {
         dot_count++;

         if ( dot_count > 3 ) {
            /* invalid IP address */
            return( RTCSERR_DNS_INVALID_IP_ADDR );
         }/* Endif */

         if ( digit_count == 0 ) {
            /* there are no digits before the '.' */
            return( RTCSERR_DNS_INVALID_IP_ADDR );
         }/* Endif */

         byte_num = byte_num / 10; /* shift back */

         if ( (byte_num < 0 ) || (byte_num > 255) ) {
         /* if the number does fall within this range it's invalid */
            return( RTCSERR_DNS_INVALID_IP_ADDR );
         } else {
            addr_number++;
            number_chars[j] = digit_count;
            j++;
            digit_count = 0;
         } /* Endif */
      } else { /* a digit */

         if ( digit_count == 0 ) {
            byte_num = 0;
         }/* Endif */
         ++digit_count;

         if ( digit_count > 3 ) {
            /* too many digits between the '.' */
            return( RTCSERR_DNS_INVALID_IP_ADDR );
         }/* Endif */

         if ( isdigit(name[i]) ) {
            /* number is in range */
            addr[addr_number][digit_count - 1] = name[i];
            byte_num = byte_num + name[i] - '0';
            byte_num = byte_num * 10;
         } else {
            /* if the characters are not decimal digits it's invalid */
            return( RTCSERR_DNS_INVALID_IP_ADDR );
         }/* Endif */
      }/* Endif */
   } /* Endfor */

   if ( digit_count == 0 ) {
      /* there are no digits after the last '.' */
      return( RTCSERR_DNS_INVALID_IP_ADDR );
   } /* Endif */

   byte_num = byte_num / 10;
   if ( (byte_num < 0 ) || (byte_num > 255) ) {
      /* if the number does fall within this range it's invalid */
      return( RTCSERR_DNS_INVALID_IP_ADDR );
   } else {
      number_chars[j] = digit_count;
   }  /* Endif */

   if ( dot_count != 3 ) {
      /* the wrong number of dots were found */
      return( RTCSERR_DNS_INVALID_IP_ADDR );
   }/* Endif */

   /*
   ** If we got this far, it's a legal IP addr, reverse the numbers,
   ** in the fill_buffer and append "in-addr.arpa" to it.
   */
   for ( i = 0; i < 4; i++) {

      leading_zero       = FALSE;
      number_buffer_ptr  = fill_buffer_ptr;
      actual_number      = number_chars[j];
      mqx_htonc(fill_buffer_ptr, number_chars[j]);
      fill_buffer_ptr++;
      size++;

      for ( k = 0; k < number_chars[j]; k++ ) {
         if ( addr[addr_number][k] == '0' ) {
            if ( addr[addr_number][k] == '0' && k == 0 ) {
               leading_zero = TRUE;
               actual_number--;
               continue;
            } /* Endif */

            if ( addr[addr_number][k] == '0' && k == 1
                 && leading_zero == TRUE && k < number_chars[j] ) {
               actual_number--;
               continue;
            } /* Endif */

            if ( addr[addr_number][k] == '0' && k == 1
                 && leading_zero == TRUE && k == number_chars[j] ) {

               mqx_htonc(fill_buffer_ptr, addr[addr_number][k]);
               fill_buffer_ptr++;
               mqx_htonc(number_buffer_ptr, actual_number);
               number_buffer_ptr++;
               size++;
               break;
            }/* Endif */
         } /* Endif */

         mqx_htonc(fill_buffer_ptr, addr[addr_number][k]);
         fill_buffer_ptr++;
         size++;
      } /* Endfor */

      addr_number--;
      j--;
   } /* Endfor */

   mqx_htonc(fill_buffer_ptr, 7);
   fill_buffer_ptr++;

   for ( i = 0; i < 7;  i++) {
      mqx_htonc(fill_buffer_ptr, appended_name[i]);
      fill_buffer_ptr++;
   } /* Endfor */

   mqx_htonc(fill_buffer_ptr, 4);
   fill_buffer_ptr++;
   i++;

   for ( ; i < 12;  i++) {
      mqx_htonc(fill_buffer_ptr, appended_name[i]);
      fill_buffer_ptr++;
   } /* Endfor */

   /*
   ** Need to append '\0' to terminate a domain name properly.
   ** This signifies the ROOT domain
   */
   mqx_htonc(fill_buffer_ptr, '\0');

   return( DNS_OK );

}  /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_insert_IP_query()
* Returned Value  : uint32_t
* Comments        :
*
*END*-----------------------------------------------------------------*/

static uint32_t DNS_insert_IP_query
   (
   unsigned char  *name,
   unsigned char  *fill_buffer_ptr
   )
{ /* Body */
   unsigned char my_buff[DNS_MAX_IP_STRING_SIZE];
   sprintf((char *)my_buff,"%ld.%ld.%ld.%ld", (uint32_t)name[0],
      (uint32_t)name[1], (uint32_t)name[2], (uint32_t)name[3]);
   return(DNS_is_dotted_IP_addr(my_buff, fill_buffer_ptr));
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_append_local_name()
* Returned Value  : char 
** Comments        :
*
*END*-----------------------------------------------------------------*/

static uint32_t DNS_append_local_name
   (
   unsigned char    *qname_buffer,
   uint32_t       total_length
   )

{  /* Body */

   uint32_t       local_name_length = 0;
   uint32_t       error;
   unsigned char        *label_length_ptr;
   unsigned char         label_length = 0;


   if ( DNS_Local_network_name[local_name_length] == '\0' ) {
      return( RTCSERR_DNS_INVALID_LOCAL_NAME );
   }/* Endif */

   /* If the Local Network is the ROOT Domain Name */
   if ( ( DNS_Local_network_name[local_name_length] == '.' )
        && ( DNS_Local_network_name[local_name_length + 1] == '\0' )) {
      mqx_htonc((qname_buffer + total_length), '\0');
      return( DNS_OK );
   }/* Endif */

   label_length_ptr = qname_buffer + total_length;
   error            = DNS_OK;

   while ( DNS_Local_network_name[local_name_length] ) {

      /*
      ** RFC 1035 says labels must start with a letter, but in practice
      ** some aren't
      */
      if ( isalnum(DNS_Local_network_name[local_name_length]) ) {
         mqx_htonc((qname_buffer + total_length + 1),
               DNS_Local_network_name[local_name_length]);
         total_length++;
         local_name_length++;
         label_length++;
      } else {
         error = RTCSERR_DNS_INVALID_LOCAL_NAME;
         break;
      }  /* Endif */

      while ( ( DNS_Local_network_name[local_name_length] != '.' ) &&
              ( DNS_Local_network_name[local_name_length] != '\0' ) ) {

         /* Domain Name characters can only be letters, hyphens, or numbers */
         if ( (isalnum(DNS_Local_network_name[local_name_length]) ||
               DNS_Local_network_name[local_name_length] == '-'  ) &&
              total_length < DNS_MAX_CHARS_IN_NAME  &&
              label_length <= DNS_MAX_LABEL_LENGTH ) {
            mqx_htonc((qname_buffer + total_length + 1),
                  DNS_Local_network_name[local_name_length]);
            total_length++;
            local_name_length++;
            label_length++;
         } else {
            error = RTCSERR_DNS_INVALID_LOCAL_NAME;
            break;
         } /* Endif */
      } /* Endwhile */

      if ( error == RTCSERR_DNS_INVALID_LOCAL_NAME ) {
         break;
      }/* Endif */

      mqx_htonc(label_length_ptr, label_length);
      label_length = 0;
      total_length++;
      label_length_ptr = qname_buffer + total_length;
      local_name_length++;
   } /* Endwhile */

   if ( error == DNS_OK ) {
      /*
      ** Need to append '\0' to terminate domain name properly.
      ** This signifies the ROOT domain
      */
      mqx_htonc(label_length_ptr, '\0');
   }/* Endif */

   return( error );

}  /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_is_dotted_domain_name()
* Returned Value  : uint32_t
* Comments        : Checks the DNS Domain Name passed to it to see if the
*                   proper naming conventions have been followed.
*
*END*-----------------------------------------------------------------*/

uint32_t DNS_is_dotted_domain_name
   (
   unsigned char  *domain_name,
   unsigned char  *qname_buffer
   )
{  /* Body */
   uint32_t     total_length = 0;
   uint32_t     error;
   unsigned char      *label_length_ptr;
   unsigned char       label_length = 0;


   if ( domain_name[total_length] == '\0' ) {
      return( RTCSERR_DNS_INVALID_NAME );
   }/* Endif */

   /* If the request is for the ROOT Domain Name */
   if ( ( domain_name[total_length] == '.' )
        && ( domain_name[total_length + 1] == '\0' )) {
      mqx_htonc(qname_buffer, '\0');
      return( DNS_OK );
   }/* Endif */

   label_length_ptr = qname_buffer;
   error            = DNS_OK;

   while ( domain_name[total_length] != '\0' ) {

      /*
      ** RFC 1035 says labels must start with a letter, but in practice
      ** some aren't
      */
      if ( isalnum(domain_name[total_length]) ) {
         mqx_htonc((qname_buffer + total_length + 1), domain_name[total_length]);
         total_length++;
         label_length++;
      } else {
         error = RTCSERR_DNS_INVALID_NAME;
         break;
      }  /* Endif */

      while ( ( domain_name[total_length] != '.') &&
              ( domain_name[total_length] != '\0' ) ) {

         /* Domain Name characters can only be letters, hyphens, or numbers */
         if ( (isalnum(domain_name[total_length]) ||
               domain_name[total_length] == '-'  ) &&
              total_length < DNS_MAX_CHARS_IN_NAME &&
              label_length <= DNS_MAX_LABEL_LENGTH ) {
            mqx_htonc((qname_buffer + total_length + 1), domain_name[total_length]);
            total_length++;
            label_length++;
         } else {
            error = RTCSERR_DNS_INVALID_NAME;
            break;
         } /* Endif */
      } /* Endwhile */

      if ( error == RTCSERR_DNS_INVALID_NAME ) {
         break;
      }/* Endif */

      mqx_htonc(label_length_ptr, label_length);
      label_length = 0;


      if ( domain_name[total_length] != '\0' ) {
         label_length_ptr = qname_buffer + total_length + 1;
      } else {
         /* It's NULL, append the local network name */
         error = DNS_append_local_name( qname_buffer, total_length + 1 );
         return( error );
      } /* Endif */
      total_length++;
   } /* Endwhile */

   if ( error == DNS_OK ) {
      /*
      ** Need to append '\0' to terminate domain name properly.
      ** This denotes the ROOT domain
      */
      mqx_htonc(label_length_ptr, '\0');
   }/* Endif */

   return( error );

}  /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_parse_answer_name_to_dotted_form()
* Returned Value  : unsigned char 
** Comments        :
*
*END*-----------------------------------------------------------------*/

static unsigned char  *DNS_parse_answer_name_to_dotted_form
   (
   unsigned char  *buffer_ptr,
   unsigned char  *name,
   uint32_t     loc
   )

{  /* Body */

   unsigned char        *compressed_name;
   unsigned char        *fill_ptr;
   unsigned char         label_size;
   uint32_t       i;
   uint16_t       new_location;

   fill_ptr = RTCS_HOST_NAMES[loc];
   _mem_zero(fill_ptr, DNS_MAX_CHARS_IN_NAME);

   compressed_name = name;

   while ( mqx_ntohc(compressed_name) != '\0' ) {

      if ( mqx_ntohc(compressed_name) & DNS_COMPRESSED_NAME_MASK ) {
         new_location = mqx_ntohs(compressed_name)
                        & DNS_COMPRESSED_LOCATION_MASK;
         compressed_name = buffer_ptr + new_location;
      }/* Endif */

      label_size  = mqx_ntohc(compressed_name);
      compressed_name++;

      for ( i = 0; i < label_size; i++ ) {
           *fill_ptr++ = mqx_ntohc(compressed_name);
            compressed_name++;
      } /* Endfor */

      if ( mqx_ntohc(compressed_name) != '\0' ) {
         *fill_ptr++ = '.';
      }/* Endif */
   } /* Endwhile */

   *fill_ptr = '\0';

   return( RTCS_HOST_NAMES[loc] );

}  /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_free_HOSTENT_STRUCT()
* Returned Value  : void
* Comments        :
*
*END*-----------------------------------------------------------------*/

void DNS_free_HOSTENT_STRUCT
   (
   HOSTENT_STRUCT  *host_ptr
   )

{  /* Body */
   /*
   ** This function is no longer needed, but the wrapper is left for
   ** backwards compatibility.
   */
}  /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_parse_UDP_response()
* Returned Value  : HOSTENT_STRUCT_PTR
* Comments        : If answers were returned, this function will
*                   parse them into a HOSTENT_STRUCT
*
*END*-----------------------------------------------------------------*/

HOSTENT_STRUCT  *DNS_parse_UDP_response
   (
   unsigned char    *buffer_ptr,
   unsigned char    *name_ptr,
   uint16_t       query_type
   )

{  /* Body */

   DNS_MESSAGE_HEADER_STRUCT            *message_head_ptr = NULL;
   DNS_RESPONSE_RR_MIDDLE_STRUCT        *answer_middle;
   INTERNAL_HOSTENT_STRUCT              *host_ptr = &RTCS_HOST;
   unsigned char                                *answer_ptr;
   unsigned char                                *answer_tail;
   unsigned char                                *temp_ptr;
   uint16_t                               response_length, answer_type,
      name_size, number_of_answers, num_queries;
   uint32_t                               i, name_index = 0;
   uint32_t                               j = 0;
   uint32_t                               k = 0;
   uint32_t                               buffer_size;
   uint32_t                              *addr_ptr;
   bool                               unknown_answer_type = FALSE;

   message_head_ptr  = (DNS_MESSAGE_HEADER_STRUCT *)buffer_ptr;
   buffer_size       = sizeof(DNS_MESSAGE_HEADER_STRUCT);
   temp_ptr = buffer_ptr + sizeof( DNS_MESSAGE_HEADER_STRUCT );

   /* Zero the global HOSTENT_STRUCT */
   _mem_zero((char *)host_ptr, sizeof(INTERNAL_HOSTENT_STRUCT));

   /* Get the number of queries. */
   num_queries = mqx_ntohs(message_head_ptr->QDCOUNT);
   for (i = 0; i < num_queries; i++) {
      name_size = 0;
      while( (mqx_ntohc(temp_ptr) != '\0') &&
         name_size < DNS_MAX_CHARS_IN_NAME ) {
         name_size = name_size + mqx_ntohc(temp_ptr) + 1;
         temp_ptr  = temp_ptr  + mqx_ntohc(temp_ptr) + 1;
      } /* Endwhile */
      /* To include the terminating NULL char */
      name_size++;
      buffer_size += (name_size + sizeof(DNS_MESSAGE_TAIL_STRUCT));
      temp_ptr    += (1 + sizeof(DNS_MESSAGE_TAIL_STRUCT));
   } /* Endfor */

   number_of_answers = mqx_ntohs(message_head_ptr->ANCOUNT);
   if (number_of_answers > DNS_MAX_NAMES ) {
      number_of_answers = DNS_MAX_NAMES;
   } /* Endif */

   host_ptr->HOSTENT.h_aliases   = &host_ptr->ALIASES[0];
   host_ptr->HOSTENT.h_addr_list = (char **)&host_ptr->ADDRESSES[0];
   host_ptr->ADDRESSES[0]        = NULL;
   host_ptr->HOSTENT.h_name      = NULL;
   host_ptr->HOSTENT.h_length    = sizeof( _ip_address );

   for (i = 0; (i < number_of_answers) && (j < DNS_MAX_ADDRS) &&
       (k < DNS_MAX_NAMES); i++ )
   {
      answer_ptr = temp_ptr;
      name_size  = 0;

      while( (mqx_ntohc(temp_ptr) != '\0') &&
             name_size < DNS_MAX_CHARS_IN_NAME &&
             !(mqx_ntohc(temp_ptr) & DNS_COMPRESSED_NAME_MASK)) {
         name_size += mqx_ntohc(temp_ptr);
         temp_ptr += mqx_ntohc(temp_ptr) + 1;
      } /* Endwhile */

      if ( mqx_ntohc(temp_ptr) & DNS_COMPRESSED_NAME_MASK ) {
         temp_ptr++;
      }/* Endif */

      temp_ptr++;
      answer_middle   = (DNS_RESPONSE_RR_MIDDLE_STRUCT *)temp_ptr;
      response_length = mqx_ntohs(answer_middle->RDLENGTH);
      answer_type     = mqx_ntohs(answer_middle->TYPE);
      temp_ptr       += sizeof(DNS_RESPONSE_RR_MIDDLE_STRUCT);
      answer_tail     = temp_ptr;
      temp_ptr       += response_length;

      switch ( answer_type ) {

         case DNS_A:
            if ( host_ptr->HOSTENT.h_name == NULL ) {
               host_ptr->HOSTENT.h_name =
                  (char *)DNS_parse_answer_name_to_dotted_form(
                  buffer_ptr, answer_ptr, name_index );
               name_index++;
            } /* Endif */

            RTCS_HOST_ADDRS[j] = mqx_ntohl((unsigned char *)answer_tail);
            /*
            ** j is used in case BOTH CNAME and A data is received.  If CNAME
            ** answer is first, will write into wrong address space if using
            ** i.
            */
            host_ptr->ADDRESSES[j] = &RTCS_HOST_ADDRS[j];
            j++;
            /*
            ** This is to assure that the first IP address used is the first
            ** one that was given
            */
            host_ptr->IP_address = *host_ptr->ADDRESSES[0];
            break;

         case DNS_PTR:
            if (query_type == DNS_PTR) {
               if (host_ptr->HOSTENT.h_name != NULL) {
                  host_ptr->ALIASES[k] = host_ptr->HOSTENT.h_name;
                  k++;
               } /* Endif */
               host_ptr->HOSTENT.h_name =
                  (char *)DNS_parse_answer_name_to_dotted_form(
                  buffer_ptr, answer_tail, name_index );
               name_index++;
               addr_ptr = RTCS_mem_alloc_zero( sizeof( _ip_address ));
               if ( addr_ptr == NULL ) {
                  RTCS_log_error(ERROR_DNS, RTCSERR_DNS_UNABLE_TO_ALLOCATE_MEMORY,
                                 0, 0, 0);
                  return( NULL );
               }/* Endif */

               *addr_ptr = *((_ip_address *)name_ptr);
               host_ptr->ADDRESSES[j] = addr_ptr;
               j++;
               host_ptr->IP_address = *host_ptr->ADDRESSES[0];
            } else {
               host_ptr->ALIASES[k] = (char *)
                     DNS_parse_answer_name_to_dotted_form( buffer_ptr,
                     answer_tail, name_index );
               name_index++;
               k++;
            } /* Endif */
            break;

         case DNS_CNAME:
            /* the k is used for ALIASES as the j is used for ADDRESSES */
            host_ptr->ALIASES[k] = (char *)
               DNS_parse_answer_name_to_dotted_form(
               buffer_ptr, answer_tail, name_index );
            name_index++;
            k++;
            break;

         default:
            unknown_answer_type = TRUE;
      } /* Endswitch */

      if ( unknown_answer_type == TRUE ) {
         break;
      }/* Endif */

      host_ptr->ADDRESSES[j]       = NULL;
      host_ptr->ALIASES[k]         = NULL;
      host_ptr->HOSTENT.h_addrtype = mqx_ntohs(answer_middle->CLASS);

   } /* Endfor */

   if ( number_of_answers == 0 ) {
      return( NULL );
   } /* Endif */

   return( &RTCS_HOST.HOSTENT );

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_query_resolver_task()
* Returned Value  : HOSTENT_STRUCT 
** Comments        :
*
*END*-----------------------------------------------------------------*/

HOSTENT_STRUCT  *DNS_query_resolver_task
   (
   unsigned char  *name,
   uint16_t     query_type
   )

{  /* Body */

   DNS_MESSAGE_HEADER_STRUCT        *message_head_ptr;
   DNS_MESSAGE_TAIL_STRUCT          *message_tail_ptr;
   HOSTENT_STRUCT                   *host_ptr = NULL;
   sockaddr_in                       addr;
   uint32_t                           local_sock;
   uint32_t                           qname_size;
   uint32_t                           buffer_size;
   uint16_t                           rlen;
   int32_t                            temp_size;
   int32_t                            error;
   unsigned char                            *temp_ptr;
   unsigned char                            *qname_ptr;
   unsigned char                            *buffer_ptr;



   /*
   ** If the size of this buffer is changed, also change the buffer size
   ** in the recvfrom() call near the bottom of this function
   */
   buffer_ptr = RTCS_mem_alloc_zero( DNS_MAX_UDP_MESSAGE_SIZE );
   if ( buffer_ptr == NULL ) {
      RTCS_log_error(ERROR_DNS, RTCSERR_DNS_UNABLE_TO_ALLOCATE_MEMORY,
                     0, 0, 0);
      return( NULL );
   }/* Endif */
   _mem_set_type(buffer_ptr, MEM_TYPE_DNS_UDP_MESSAGE);
   qname_ptr = buffer_ptr + sizeof( DNS_MESSAGE_HEADER_STRUCT );

   if ( query_type == DNS_A ) {
       error = DNS_is_dotted_domain_name( name, qname_ptr );
       if ( error == RTCSERR_DNS_INVALID_NAME ||
            error == RTCSERR_DNS_INVALID_LOCAL_NAME ) {
          _mem_free(buffer_ptr);
          return( NULL );
       }/* Endif */
   } else {
      if ( query_type == DNS_PTR ) {
         error = DNS_insert_IP_query( name, qname_ptr );
         if ( error == RTCSERR_DNS_INVALID_IP_ADDR ) {
            _mem_free(buffer_ptr);
            return( NULL );
         }/* Endif */
      } else {
         _mem_free(buffer_ptr);
         return( NULL );
      } /* Endif */
   } /* Endif */

   local_sock = socket(AF_INET, SOCK_DGRAM, 0);

   if ( local_sock == RTCS_HANDLE_ERROR ) {
      RTCS_log_error(ERROR_DNS, RTCSERR_DNS_UNABLE_TO_OPEN_SOCKET,
                     0, 0, 0);
      _mem_free(buffer_ptr);
      return( NULL );
   }/* Endif */

   /* Local address  */
   addr.sin_family       = AF_INET;
   addr.sin_port         = 0;
   addr.sin_addr.s_addr  = INADDR_ANY;

   error =  bind(local_sock, &addr, sizeof(addr));
   if (error != RTCS_OK) {
      RTCS_log_error(ERROR_DNS, RTCSERR_DNS_UNABLE_TO_BIND_SOCKET,
                     0, 0, 0);
      _mem_free(buffer_ptr);
      return( NULL );
   } /* Endif */

   /* set up buffer for sending query.   */
   message_head_ptr = (DNS_MESSAGE_HEADER_STRUCT *)buffer_ptr;

   mqx_htons(message_head_ptr->ID, 0);
   mqx_htons(message_head_ptr->CONTROL, DNS_STANDARD_QUERY);
   mqx_htons(message_head_ptr->QDCOUNT, DNS_SINGLE_QUERY);
   mqx_htons(message_head_ptr->NSCOUNT, 0);
   mqx_htons(message_head_ptr->ARCOUNT, 0);
   mqx_htons(message_head_ptr->ANCOUNT, 0);

   qname_size = strlen((char *)qname_ptr );
   /* Need to include the last '\0' character as well */
   qname_size++;

   temp_ptr = buffer_ptr + sizeof( DNS_MESSAGE_HEADER_STRUCT )
                          + qname_size;

   message_tail_ptr = (DNS_MESSAGE_TAIL_STRUCT *)temp_ptr;

   mqx_htons(message_tail_ptr->QTYPE, query_type);
   mqx_htons(message_tail_ptr->QCLASS, DNS_IN);

   buffer_size = sizeof(DNS_MESSAGE_HEADER_STRUCT) + qname_size
                 + sizeof(DNS_MESSAGE_TAIL_STRUCT);

    /* Remote address, DNS_Resolver currently uses port 1024 */
   addr.sin_port        = DNS_RESOLVER_PORT;
   addr.sin_addr.s_addr = DNS_RESOLVER_IP_ADDR;

   rlen = sizeof(addr);

   /* Send the buffer to the resolver for making a query */
   error = sendto(local_sock, buffer_ptr, buffer_size, 0, &addr, rlen);
   if (error == RTCS_ERROR) {
      shutdown(local_sock, FLAG_ABORT_CONNECTION);
      RTCS_log_error(ERROR_DNS, RTCSERR_DNS_UNABLE_TO_SEND_QUERY,
                     0, 0, 0);
      _mem_free(buffer_ptr);
      return( NULL );
   }/* Endif */

   /* Get the response from the resolver, if none received, return NULL */
   error = (uint32_t)RTCS_selectset( &local_sock, 1, DNS_QUERY_TIMEOUT );
   if ( !error || error == RTCS_ERROR ) {
      shutdown(local_sock, FLAG_ABORT_CONNECTION);
      RTCS_log_error(ERROR_DNS, RTCSERR_DNS_NO_RESPONSE_FROM_RESOLVER,
                     0, 0, 0);
      _mem_free(buffer_ptr);
      return( NULL );
   } /* Endif */

   temp_size = recvfrom(local_sock, buffer_ptr, DNS_MAX_UDP_MESSAGE_SIZE,
                        0, &addr, &rlen);
   if ( temp_size == RTCS_ERROR ) {
     shutdown(local_sock, FLAG_ABORT_CONNECTION);
     RTCS_log_error(ERROR_DNS, RTCSERR_DNS_PACKET_RECEPTION_ERROR,
                    0, 0, 0);
     _mem_free(buffer_ptr);
     return( NULL );
   }/* Endif */

   host_ptr = DNS_parse_UDP_response(buffer_ptr, name, query_type);
   shutdown(local_sock, FLAG_ABORT_CONNECTION);
   _mem_free(buffer_ptr);
   return( host_ptr );

} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DNS_check_local_host_file()
* Returned Value  : HOSTENT_STRUCT_PTR
* Comments        : Maps a host name to an IP address
*
*END*-----------------------------------------------------------------*/

HOSTENT_STRUCT_PTR DNS_check_local_host_file
   (
      unsigned char  *name
   )
{  /* Body */
   INTERNAL_HOSTENT_STRUCT_PTR host_ptr = &RTCS_HOST;
   uint32_t                     i;
   bool                     name_found;
   uint32_t                     num_aliases = 0;
   uint32_t                     host_index = 0;
   uint32_t                     digit_count = 0;
   uint32_t                     dot_count = 0;
   uint32_t                     name_size, name_index = 0;
   int32_t                      byte_num;
   bool                     dotted_decimal;
   uint32_t                     temp = 0;

   /* zero memory for HOSTENT_STRUCT */
   _mem_zero(host_ptr, sizeof(INTERNAL_HOSTENT_STRUCT));

   host_ptr->HOSTENT.h_aliases = &host_ptr->ALIASES[0];
   host_ptr->HOSTENT.h_addr_list = (char **)&host_ptr->ADDRESSES[0];

   /* assign address type and length */
   host_ptr->HOSTENT.h_addrtype = AF_INET;
   host_ptr->HOSTENT.h_length = 4;     /* IP addresses only used - 4 bytes */

   /* check to see if name is written in dotted decimal IP format */
   dotted_decimal = TRUE;
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
            temp = temp + byte_num;
            temp = temp * 0x100;
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

         if ( isdigit(name[i]) ) {
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
         temp = temp + byte_num;
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

   _mem_zero(RTCS_HOST_NAMES[name_index], DNS_MAX_CHARS_IN_NAME);
   if ( dotted_decimal == TRUE ) {
      name_size = strlen((char *)name );
      name_size++;
      _mem_copy(name, RTCS_HOST_NAMES[name_index], name_size);
      /* assign the hostent struct from the dotted decimal IP name */
      host_ptr->HOSTENT.h_name = (char *)RTCS_HOST_NAMES[name_index];
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
         i = 0;
         while ( ((RTCS_Hosts_list[host_index]).aliases[i] != NULL) &&
                 !name_found)
         {
            if ( strcmp((char *)(RTCS_Hosts_list[host_index]).aliases[i],
                        (char *)name) == 0 )
            {
               name_found = TRUE;
            }/* Endif */
            ++i;
         } /* Endwhile */

         /* check the name field for the name */
         if ( strcmp((char *)(RTCS_Hosts_list[host_index]).host_name,
             (char *)name) == 0)
         {
            name_found = TRUE;
         } /* Endif */

         if (name_found == TRUE ) { /* host name was found */
            name_size = strlen( (RTCS_Hosts_list[host_index]).host_name );
            name_size++;
            _mem_copy((RTCS_Hosts_list[host_index]).host_name,
               RTCS_HOST_NAMES[name_index], name_size);

            host_ptr->HOSTENT.h_name = (char *)RTCS_HOST_NAMES[name_index];
            name_index++;

            /* assign alias list */
            while ( (num_aliases < MAX_HOST_ALIASES)
               && ((RTCS_Hosts_list[host_index]).aliases[num_aliases] != NULL)
               && (name_index < DNS_MAX_NAMES))
            {
               name_size =
                  strlen((RTCS_Hosts_list[host_index]).aliases[num_aliases]);
               name_size++;


               _mem_zero(RTCS_HOST_NAMES[name_index], DNS_MAX_CHARS_IN_NAME);
               _mem_copy((RTCS_Hosts_list[host_index]).aliases[num_aliases],
                  RTCS_HOST_NAMES[name_index], name_size);
               host_ptr->ALIASES[num_aliases] =
                  (char *)RTCS_HOST_NAMES[name_index];
               name_index++;
               num_aliases++;
            } /* Endwhile */
            host_ptr->ALIASES[num_aliases] = NULL;

            /* assign addresses (in our case only one IP address is allowed) */
            _mem_copy((_ip_address *)
               &((RTCS_Hosts_list[host_index]).ip_address), &RTCS_HOST_ADDRS[0],
               sizeof(_ip_address));

            host_ptr->ADDRESSES[0] = (uint32_t *)&RTCS_HOST_ADDRS[0];
            host_ptr->ADDRESSES[1] = NULL;
         } /* Endif */
         ++host_index;
      } /* Endwhile */
   }/* Endif */


   if ( !name_found ) {
      return( NULL );
   }/* Endif */

   return( &host_ptr->HOSTENT );
}/* EndBody */

#endif
#endif
/* EOF */
