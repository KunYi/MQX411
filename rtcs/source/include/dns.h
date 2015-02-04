#ifndef __dns_h__
#define __dns_h__
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
*   Definitions for use by either the Domain Name Server
*   or Resolver.
*
*
*END************************************************************************/

extern void DNS_Resolver_task      (void *, void *);
extern void DNS_Name_Server_task   (uint32_t);
extern void DNS_Stub_Resolver_task (uint32_t);

/***************************************
**
** Constants
**
*/

#define DNSALLOC_SIZE                       4


#define DNS_NAME_SERVER_MODULE              1
#define DNS_FULL_RESOLVER_MODULE            2
#define DNS_STUB_RESOLVER_MODULE            3

/* maximum UDP size is 512 bytes, not counting headers */
#define DNS_MAX_PACKET_SIZE                 1024
#define DNS_MAX_UDP_MESSAGE_SIZE            512
#define DNS_MAX_IP_STRING_SIZE              16

/* maximum number of query tries for one DNS server address */
#define DNS_MAX_NUMBER_OF_QUERY_TRIES       5

/* picked 400 for now, need to find exact size that won't overflow a UDP send
   with headers, ect... SMR June 19, 1998 */
#define DNS_NULL_RECORD_SIZE                400

#define DNS_MAX_LABEL_LENGTH_ARRAY_SIZE     130
#define DNS_LAST_LABEL_ALLOWED              127
#define DNS_NAME_CONTROL_BUFFER             DNS_LAST_LABEL_ALLOWED + 1
#define DNS_NAME_LENGTH_BUFFER              DNS_MAX_LABEL_LENGTH_ARRAY_SIZE - 1
#define DNS_MAX_LABEL_LENGTH                63
#define DNS_MAX_CHARS_IN_NAME               0xFF
#define DNS_MAX_ID_CHECK                    0xFFFE
#define DNS_NAME_ADDITION_ERROR             0x00BADADD
#define DNS_MAX_ALLOWED_NAMES               30
/* one week, as required by RFC 1035 */
#define DNS_MAX_ALLOWED_TTL                 604800L
/* ten minutes as required by RFC 2308 */
#define DNS_NAME_ERROR_TTL                  600L
/*
** 4 seconds is BIND standard, but any reasonable value will do.
** This value is kept in seconds so that TTL for information
** coming in from the net can be kept in a consistant format and
** all functions can be kept common for all TTL's
*/
#define DNS_QUERY_RETRY_TIME                4L /* in seconds */
#define DNS_SERVER_RETRY_TIME               4000
#define DNS_RESOLVER_RETRY_TIME             4000
/* timeout for the response from the DNS server */
#define DNS_QUERY_TIMEOUT                   40000 

/* Name server and resolver ports and IP addresses */

#define DNS_NAMESERVER_PORT                 53
/* Start CR 2176 */
#define DNS_RESOLVER_PORT                   1024
/* End CR 2176 */

#define DNS_RESOLVER_IP_ADDR                INADDR_LOOPBACK


/* these are used to check and add to cached data */

#define DNS_NS_AUTH_DATA              1
#define DNS_NS_CACHE_DATA             2
#define DNS_RES_CACHE_DATA            3

/* RFC 1035 suggests 8 as a maximum, but it is a SHOULD item, not a MUST. */
#define DNS_MAX_NUM_CNAME_RETRIES     8

/* This is used to assign the size of a global buffer. */
#define DNS_MAX_NAMES  MAX_HOST_ALIASES
#define DNS_MAX_ADDRS  MAX_HOST_ADDRESSES


/* Error codes specific to DNS - backward compatibility */
#define DNS_QUERY_OK                        RTCSERR_DNS_QUERY_OK
#define DNS_NO_NAME_SERVER_RESPONSE         RTCSERR_DNS_NO_NAME_SERVER_RESPONSE
#define DNS_UNABLE_TO_OPEN_SOCKET           RTCSERR_DNS_UNABLE_TO_OPEN_SOCKET
#define DNS_UNABLE_TO_BIND_SOCKET           RTCSERR_DNS_UNABLE_TO_BIND_SOCKET
#define DNS_UNABLE_TO_UPDATE_ROOT_SERVERS   RTCSERR_DNS_UNABLE_TO_UPDATE_ROOT_SERVERS
#define DNS_SOCKET_RECV_ERROR               RTCSERR_DNS_SOCKET_RECV_ERROR
#define DNS_UNABLE_TO_SEND_QUERY            RTCSERR_DNS_UNABLE_TO_SEND_QUERY
#define DNS_NO_RESPONSE_FROM_RESOLVER       RTCSERR_DNS_NO_RESPONSE_FROM_RESOLVER
#define DNS_PACKET_RECEPTION_ERROR          RTCSERR_DNS_PACKET_RECEPTION_ERROR
#define DNS_INVALID_NAME                    RTCSERR_DNS_INVALID_NAME
#define DNS_INVALID_IP_ADDR                 RTCSERR_DNS_INVALID_IP_ADDR
#define DNS_ALL_SERVERS_QUERIED             RTCSERR_DNS_ALL_SERVERS_QUERIED
#define DNS_INVALID_LOCAL_NAME              RTCSERR_DNS_INVALID_LOCAL_NAME
#define DNS_UNABLE_TO_ALLOCATE_MEMORY       RTCSERR_DNS_UNABLE_TO_ALLOCATE_MEMORY
#define DNS_UNABLE_TO_FREE_SOCKET           RTCSERR_DNS_UNABLE_TO_FREE_SOCKET
#define DNS_VALID_IP_ADDR                   RTCSERR_DNS_VALID_IP_ADDR
#define DNS_INITIALIZATION_FAILURE          RTCSERR_DNS_INITIALIZATION_FAILURE
#define DNS_NOT_IMPLEMENTED                 RTCSERR_DNS_NOT_IMPLEMENTED
#define DNS_UNABLE_TO_CREATE_PARTITION      RTCSERR_DNS_UNABLE_TO_CREATE_PARTITION
#define DNS_SHORT_MESSAGE_ERROR             RTCSERR_DNS_SHORT_MESSAGE_ERROR


/* Any value from 64 to 255 will do if there is a conflict with these */
#define DNS_APPEND_LOCAL_NAME               0xFF
#define DNS_NOT_DOTTED_NAME_FORMAT          0xFE


/* Query Codes */

#define DNS_STANDARD_QUERY                  0x0000
#define DNS_INVERSE_QUERY                   0x0800
#define DNS_STATUS_REQUEST                  0x1000
#define DNS_RECURSION_DESIRED               0x0100

/* QDCOUNT Codes */
#define DNS_SINGLE_QUERY                    0x0001
#define DNS_DOUBLE_QUERY                    0x0002
#define DNS_TRIPLE_QUERY                    0x0003

/* ANCOUNT Codes */
#define DNS_NO_ANSWER                       0x0000
#define DNS_SINGLE_ANSWER                   0x0001
#define DNS_DOUBLE_ANSWER                   0x0002
#define DNS_TRIPLE_ANSWER                   0x0003
#define DNS_QADRUPLE_ANSWER                 0x0004

/* Response Codes  */

#define DNS_RESPONSE                        0x8000
#define DNS_RECURSION_DESIRED               0x0100
#define DNS_RECURSION_AVAILABLE             0x0080
#define DNS_AUTHORITATIVE_ANSWER            0x0400
#define DNS_TRUNCATION                      0x0200
#define DNS_NO_ERROR                        0x0000
#define DNS_FORMAT_ERROR                    0x0001
#define DNS_SERVER_FAILURE                  0x0002
#define DNS_NAME_ERROR                      0x0003
#define DNS_AUTHORITATIVE_NAME_ERROR        0x0403
#define DNS_QUERY_NOT_IMPLEMENTED           0x0004
#define DNS_QUERY_REFUSED                   0x0005

#define DNS_RCODE_MASK                      0x000F
#define DNS_OPCODE_MASK                     0x7800
#define DNS_COMPRESSED_NAME_MASK            0xC0
#define DNS_COMPRESSED_LOCATION_MASK        0x3FFF

/* These are needed for the TI board */
#define DNS_UINT_16_MASK                    0xFFFF
#define DNS_CHAR_MASK                       0xFF

/* TYPE and QTYPE definitions [RFC 1035] -- all TYPES are valid QTYPES
   Those marked EXPERIMENTAL or OBSOLETE should never be sent,
   however provisions should be made in case they are received  */


/* Class specific TYPES */

#define DNS_A          0x0001   /* IPv4 host address */
#define DNS_MD         0x0003   /* Mail Destination (OBSOLETE) */
#define DNS_MF         0x0004   /* Mail Forward     (OBSOLETE) */
#define DNS_MB         0x0007   /* Mailbox Domain Name (EXPERIMENTAL) */
#define DNS_MG         0x0008   /* Mail Group Member (EXPERIMENTAL) */
#define DNS_MR         0x0009   /* Mail Rename Domain Name (EXPERIMENTAL) */
#define DNS_NULL       0x000A   /* Null Resource Record */
#define DNS_WKS        0x000B   /* Service Description */
#define DNS_HINFO      0x000D   /* Host information */
#define DNS_MINFO      0x000E   /* Mailbox or mail list information */
#define DNS_MX         0x000F   /* Mail Exchange */
#define DNS_TXT        0x0010   /* Text Strings */
#define DNS_AAAA       0x001C   /* IPv6 host address. */

/* Types to be used by all classes */
#define DNS_NS         0x0002   /* Authoritative name server */
#define DNS_SOA        0x0006   /* Start of Zone of Authority */
#define DNS_CNAME      0x0005   /* Canonical Name for an alias */
#define DNS_PTR        0x000C   /* Domain name pointer */

/* QTYPES */

#define DNS_AXFR       0x00FC   /* Request for transfer of entire zone */
#define DNS_MAILB      0x00FD   /* Request for mailbox related records */
#define DNS_MAILA      0x00FE   /* Request for mail agent RR's (OBSOLETE) */

/* This is used for both QTYPE and QCLASS definitions */
#define DNS_ALL        0x00FF   /* Request for all records or for all classes */


/* CLASS values */

#define DNS_IN         0x0001   /* The Internet */
#define DNS_CS         0x0002   /* CSNET Class   (OBSOLETE) */
#define DNS_CH         0x0003   /* CHAOS Class  */
#define DNS_HS         0x0004   /* Hessoid [Dyer 87] */


#define DNS_RESERVED_CLASS 0xFFFF

 /***************************************
**
** Structure Definitions
**
*/


/*
**  Resource Record Header Structure
*/

typedef struct dns_rr_header {
   uint32_t                       LAST_TTL_CHECK;
   struct dns_rr_header         *NEXT;
   unsigned char                        *DOMAIN_NAME_PTR;
   int32_t                        TTL;
   uint16_t                       CLASS;
   uint16_t                       TYPE;

}DNS_RR_HEADER_STRUCT, * DNS_RR_HEADER_STRUCT_PTR;


/*
** Record-specific data structures, traditional record types.
*/

typedef struct dns_addr_record_struct {
   DNS_RR_HEADER_STRUCT             RR;
   _ip_address                      IPADDR;

}DNS_ADDR_RECORD_STRUCT, * DNS_ADDR_RECORD_STRUCT_PTR;

typedef struct dns_name_record_struct {
   DNS_RR_HEADER_STRUCT             RR;
   unsigned char                           *ANS_NAME_PTR;

}DNS_NAME_RECORD_STRUCT, * DNS_NAME_RECORD_STRUCT_PTR;


typedef struct dns_null_record_struct {
   DNS_RR_HEADER_STRUCT             RR;
   unsigned char                            NULL_BUFFER[DNS_NULL_RECORD_SIZE];

}DNS_NULL_RECORD_STRUCT, * DNS_NULL_RECORD_STRUCT_PTR;


/*
** Structure used to maintain the linked list of resource records.
** NAME_SIZE is used for parsing name into response buffers of
** correct size.
*/

typedef struct dns_rr_list_struct {

   unsigned char                             *NAME_PTR;
   DNS_RR_HEADER_STRUCT              *RR_HEAD_PTR;
   struct dns_rr_list_struct         *NEXT;

}DNS_RR_LIST_STRUCT, * DNS_RR_LIST_STRUCT_PTR;


/*
** Domain Name Server Response Resource Record Structure
** This is defined to make writing a response easier.  This section of
** the response buffer should have a name first, this information second,
** and then response information of RDLENGTH after the structure.
*/

typedef struct dns_response_rr_middle_struct {

   unsigned char         TYPE[2];
   unsigned char         CLASS[2];
   unsigned char         TTL[4];
   unsigned char         RDLENGTH[2];

}DNS_RESPONSE_RR_MIDDLE_STRUCT, * DNS_RESPONSE_RR_MIDDLE_STRUCT_PTR;

/*
** Domain Name Server Message Header Structure
*/

typedef struct dns_message_header_struct {

  unsigned char          ID[2];          /* Identifier used to match replies to
                                    outstanding queries */
  unsigned char          CONTROL[2];     /* Bits set to define query or response */
  unsigned char          QDCOUNT[2];     /* Number of entries in question section */
  unsigned char          ANCOUNT[2];     /* Number of RR's in answer section */
  unsigned char          NSCOUNT[2];     /* Number of Name Server RR's in the
                                    authority records   */
  unsigned char          ARCOUNT[2];     /* Number of RR's in the additional records */

}DNS_MESSAGE_HEADER_STRUCT, * DNS_MESSAGE_HEADER_STRUCT_PTR;

/*
** Domain Name Server Message Tail Structure
*/

typedef struct dns_message_tail_struct {

  unsigned char       QTYPE[2];
  unsigned char       QCLASS[2];

}DNS_MESSAGE_TAIL_STRUCT, * DNS_MESSAGE_TAIL_STRUCT_PTR;


/*
** Domain Name Server Server List Structure.  This structure is used
** exclusively by the resolver to keep a list of Name Servers to connect
** to when a query is made. Used only in the Root Server Structure
*/

typedef struct dns_slist_struct {

   char                  *NAME_PTR;
   struct dns_slist_struct      *NEXT;
   _ip_address            IPADDR;
   uint32_t                LAST_TTL_CHECK;
   int32_t                 TTL;
   uint32_t                NUMBER_OF_HITS;
   uint32_t                NUMBER_OF_TRIES;
   uint32_t                AVG_RESPONSE_TIME;
   uint16_t                TYPE;
   uint16_t                CLASS;

}DNS_SLIST_STRUCT, * DNS_SLIST_STRUCT_PTR;

/*
** Canonical Name Structure
*/

typedef struct dns_cname_struct {
   unsigned char                          *CNAME_PTR;
   struct dns_cname_struct      *NEXT;
} DNS_CNAME_STRUCT, * DNS_CNAME_STRUCT_PTR;

/*
**  Domain Name Server Query Id Structure. This structure is used in a
**  linked list of all outstanding queries.
**  The entries are deleted when expired or answered.
*/

typedef struct dns_internal_query_struct {


   _ip_address                     SERVER_IPADDR;
   uint16_t                         QUERY_ID;
   uint16_t                         CONTROL;
   uint16_t                         QTYPE;
   uint16_t                         QCLASS;
   /*
   ** This field is to keep track of which query was the reason for an
   ** internal query.
   */
   uint16_t                         ORIGINAL_QID;
   uint16_t                         RESERVED;
   int32_t                          TTL;
   uint32_t                         QUERY_RETRIES;
   uint32_t                         LAST_TTL_CHECK;
   unsigned char                          *QNAME_PTR;
   DNS_CNAME_STRUCT               *CNAME;
   struct dns_internal_query_struct      *NEXT;
   sockaddr_in                     CLIENT_SIN;
   bool                         INTERNAL_QUERY;

} DNS_INTERNAL_QUERY_STRUCT, * DNS_INTERNAL_QUERY_STRUCT_PTR;

/*
** DNS Configuration Structure.  Used so DNS server and resolver functions
** only need to have one parameter passed to them and keep all necessary
** structures private.
*/

typedef struct dns_control_struct {

   DNS_INTERNAL_QUERY_STRUCT      *NS_QUERY_HEAD_PTR;
   DNS_INTERNAL_QUERY_STRUCT      *RES_QUERY_HEAD_PTR;
   DNS_RR_LIST_STRUCT             *NS_ZONE_AUTH_LIST;
   DNS_RR_LIST_STRUCT             *CACHE_PTR;
   DNS_SLIST_STRUCT               *ROOT_SLIST_PTR;
   DNS_SLIST_STRUCT               *LOCAL_SLIST_PTR;
   unsigned char                          *NS_BUFFER;
   unsigned char                          *RES_BUFFER;
   int32_t                          NS_BUFFER_SIZE;
   int32_t                          RES_BUFFER_SIZE;
   uint32_t                         NS_SOCKET;
   uint32_t                         RES_SOCKET;
   bool                         UPDATING;
   _rtcs_mutex                     CACHE_MUTEX;
   uint32_t                         NUM_NAMES_CACHED;
   _rtcs_part                      INT_QUERY_PARTID;
   _rtcs_part                      ADDR_RR_PARTID;
   _rtcs_part                      NAME_RR_PARTID;
   _rtcs_part                      RR_LIST_PARTID;
   _rtcs_part                      CNAME_PARTID;
   _rtcs_mutex                     SLIST_MUTEX;
} DNS_CONTROL_STRUCT, * DNS_CONTROL_STRUCT_PTR;

/* Used to keep from blowing the stack in a recursive call */
typedef struct dns_recursive_cache_struct {
   DNS_INTERNAL_QUERY_STRUCT      *NEW_QUERY_PTR;
   DNS_CONTROL_STRUCT             *CONTROL_PTR;
   DNS_RR_LIST_STRUCT             *CACHE_HEAD;
   unsigned char                          *NAME_PTR;
   unsigned char                          *BUFFER_PTR;
   int32_t                         *BUFFER_SIZE_PTR;
   uint32_t                         NUM_TRIES;
} DNS_RECURSIVE_CACHE_STRUCT, * DNS_RECURSIVE_CACHE_STRUCT_PTR;


/* Function definitions required by DNS_gethostbyname and DNS_gethostbyaddr */

DNS_RR_LIST_STRUCT  *DNS_dynamically_allocate_zone_list
(
     DNS_CONTROL_STRUCT      *control_ptr
);

uint32_t DNS_is_dotted_domain_name
(
     unsigned char  *domain_name,
     unsigned char      *qname_buffer
);

uint32_t DNS_is_dotted_IP_addr
(
     unsigned char  *addr_ptr,
     unsigned char      *qname_buffer
);

HOSTENT_STRUCT  *DNS_parse_UDP_response
(
     unsigned char  *buffer_ptr,
     unsigned char  *name_ptr,
     uint16_t     query_type
);

HOSTENT_STRUCT  *DNS_query_resolver_task
(
     unsigned char  *name,
     uint16_t    query_type
);

HOSTENT_STRUCT_PTR DNS_check_local_host_file
(
     unsigned char      *name
);

uint32_t DNS_is_valid_domain_name
(
     unsigned char             *buffer_ptr,
     unsigned char             *domain_name,
     unsigned char               **local_name_buffer
);


/*
* extern struct of initial root server information
*/

extern DNS_SLIST_STRUCT   DNS_A_Root_server[];
extern char         DNS_Local_network_name[];
extern uint32_t DNS_CACHE_NAME_LIMIT;
extern DNS_RR_LIST_STRUCT DNS_Zone_of_Authority_list[];

/* Start CR 2257 */
extern DNS_SLIST_STRUCT      *DNS_First_Local_server;
extern char         DNS_Local_server_name[];
extern void DNS_init_slist_entry   (DNS_SLIST_STRUCT *);
extern uint32_t DNS_append_slist_entry (DNS_SLIST_STRUCT *);
extern uint32_t DNS_prepend_slist_entry (DNS_SLIST_STRUCT *);
extern uint32_t DNS_remove_slist_entry (_ip_address);
extern DNS_SLIST_STRUCT      *DNS_find_slist_entry (_ip_address);
#define DNS_add_slist_entry(x) DNS_prepend_slist_entry(x)
/* End CR 2257 */

#endif

/* EOF */
