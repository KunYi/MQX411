#ifndef __dnscln_h__
#define __dnscln_h__
/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
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
*   Definitions for use by the DNS Client.
*
*
*END************************************************************************/

/* DNS Resource Record types. */
typedef enum dnscln_type{
    DNSCLN_TYPE_A       = 0x0001,   /* IPv4 host address */
    DNSCLN_TYPE_NULL    = 0x000A,   /* Null Resource Record */
    DNSCLN_TYPE_WKS     = 0x000B,   /* Service Description */
    DNSCLN_TYPE_HINFO   = 0x000D,   /* Host information */
    DNSCLN_TYPE_MINFO   = 0x000E,   /* Mailbox or mail list information */
    DNSCLN_TYPE_MX      = 0x000F,   /* Mail Exchange */
    DNSCLN_TYPE_TXT     = 0x0010,   /* Text Strings */
    DNSCLN_TYPE_AAAA    = 0x001C    /* IPv6 host address. */
    /* Add new DNS Resource Record gere.*/ 
} DNSCLN_TYPE;

/************************************************************************
* DNS Resorce Record, returned by DNSCLN_query().
*************************************************************************/
typedef struct dnscln_record_struct
{
    struct dnscln_record_struct *next;          /* Pointer to the next DNSCLN_RECORD_STRUCT.*/
    char                        *name;          /* Pointer to a host name string (null-terminated). */
    DNSCLN_TYPE                 type;           /* Type of DNS Resource Record. */
    uint32_t                    ttl;            /* Time to Live of DNS Resource Record, in seconds.*/
    uint16_t                    data_length;    /* Length of DNS Resource Record data, in bytes.*/
    char                        *data;          /* Pointer to DNS Resource Record data in network byte order. Its size is defined by data_length.*/
} DNSCLN_RECORD_STRUCT;

/************************************************************************
* DNS Client Input Parameters 
*************************************************************************/
typedef struct dnscln_param_struct
{
    sockaddr        dns_server;         /* Socket address of the remote DNS server to connect to. */
    char            *name_to_resolve;   /* Host name to resolve (null-terminated string). */
    DNSCLN_TYPE     type;               /* DNS Resource Record Type that is queried. */
} DNSCLN_PARAM_STRUCT;

extern void DNSCLN_record_list_free(DNSCLN_RECORD_STRUCT *record_list);
extern DNSCLN_RECORD_STRUCT *DNSCLN_query(DNSCLN_PARAM_STRUCT *params);
extern bool DNSCLN_get_dns_addr(_rtcs_if_handle ihandle /*Optional.*/, uint32_t n, sockaddr *dns_server);

#endif /* __dnscln_h__ */

