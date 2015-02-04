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
*   Example of shell using RTCS.
*
*
*END************************************************************************/

#include "httpsrv.h"
//#include <stdio.h>

static _mqx_int cgi_ipstat(HTTPSRV_CGI_REQ_STRUCT* param);
static _mqx_int cgi_icmpstat(HTTPSRV_CGI_REQ_STRUCT* param);
static _mqx_int cgi_udpstat(HTTPSRV_CGI_REQ_STRUCT* param);
static _mqx_int cgi_tcpstat(HTTPSRV_CGI_REQ_STRUCT* param);
static _mqx_int cgi_rtc_data(HTTPSRV_CGI_REQ_STRUCT* param);


const HTTPSRV_CGI_LINK_STRUCT cgi_lnk_tbl[] = {
    { "ipstat",         cgi_ipstat,   0},
    { "icmpstat",       cgi_icmpstat, 0},
    { "udpstat",        cgi_udpstat,  0},
    { "tcpstat",        cgi_tcpstat,  0},
    { "rtcdata",        cgi_rtc_data, 1500},
    { 0, 0 }    // DO NOT REMOVE - last item - end of table
};

static _mqx_int cgi_rtc_data(HTTPSRV_CGI_REQ_STRUCT* param)
{
    #define BUFF_SIZE sizeof("00\n00\n00\n")
    HTTPSRV_CGI_RES_STRUCT response;
    TIME_STRUCT time;
    uint32_t min;
    uint32_t hour;
    uint32_t sec;
    
    char str[BUFF_SIZE];
    uint32_t length = 0;
    
    if (param->request_method != HTTPSRV_REQ_GET)
    {
        return(0);
    }

    _time_get(&time);
    
    sec = time.SECONDS % 60;
    min = time.SECONDS / 60;
    hour = min / 60;
    min %= 60;

    response.ses_handle = param->ses_handle;
    response.content_type = HTTPSRV_CONTENT_TYPE_PLAIN;
    response.status_code = 200;
    /* 
    ** When the keep-alive is used we have to calculate a correct content length
    ** so the receiver knows when to ACK the data and continue with a next request.
    ** Please see RFC2616 section 4.4 for further details.
    */
    
    /* Calculate content length while saving it to buffer */                                  
    length = snprintf(str, BUFF_SIZE, "%ld\n%ld\n%ld\n", hour, min, sec);          
    response.data = str;
    response.data_length = length;
    response.content_length = response.data_length;
    /* Send response */
    HTTPSRV_cgi_write(&response);
    return (response.content_length);
}

_mqx_int cgi_ipstat(HTTPSRV_CGI_REQ_STRUCT* param)
{
    HTTPSRV_CGI_RES_STRUCT response;

    if (param->request_method != HTTPSRV_REQ_GET)
    {
        return(0);
    }

    response.ses_handle = param->ses_handle;
    response.content_type = HTTPSRV_CONTENT_TYPE_PLAIN;
    response.status_code = 200;

#if RTCSCFG_ENABLE_IP_STATS && RTCSCFG_ENABLE_IP4
    {
        uint32_t length = 0;
        /* We need to store 10 literals + '\n' (in worst case) for 19 unsigned integers */
        char str[sizeof(char)*19*(10+1)+1];

        /* Read IP statistics */
        IP_STATS_PTR ip = IP_stats();

        /* Print data to buffer */
        length = snprintf(str, sizeof(str),
            "%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n",
            ip->COMMON.ST_RX_TOTAL,
            ip->ST_RX_DELIVERED,
            ip->ST_RX_FORWARDED,
            ip->COMMON.ST_RX_MISSED, 
            ip->COMMON.ST_RX_ERRORS,
            ip->COMMON.ST_RX_DISCARDED,
            ip->ST_RX_HDR_ERRORS,
            ip->ST_RX_ADDR_ERRORS,
            ip->ST_RX_NO_PROTO,
            ip->ST_RX_FRAG_RECVD,
            ip->ST_RX_FRAG_REASMD,
            ip->ST_RX_FRAG_DISCARDED,
            ip->COMMON.ST_TX_TOTAL,
            ip->COMMON.ST_TX_MISSED,
            ip->COMMON.ST_TX_ERRORS,
            ip->COMMON.ST_TX_DISCARDED,
            ip->ST_TX_FRAG_FRAGD,
            ip->ST_TX_FRAG_SENT,
            ip->ST_TX_FRAG_DISCARDED);

        /* Write header and buffer with data */
        
        response.data = str;
        response.data_length = length;
        response.content_length = response.data_length;
        HTTPSRV_cgi_write(&response);
    }
#else
    {
        int i;

        response.content_length = strlen("unk\n")*19;
        response.data = "unk\n";
        response.data_length = 4;
        for (i = 0; i < 19; i++)
        {
            HTTPSRV_cgi_write(&response);
        }
    }
#endif
    return (response.content_length);
}

_mqx_int cgi_icmpstat(HTTPSRV_CGI_REQ_STRUCT* param)
{
    HTTPSRV_CGI_RES_STRUCT response;
    
    if (param->request_method != HTTPSRV_REQ_GET)
    {
        return(0);
    }

    response.ses_handle = param->ses_handle;
    response.content_type = HTTPSRV_CONTENT_TYPE_PLAIN;
    response.status_code = 200;
	
#if RTCSCFG_ENABLE_ICMP_STATS && RTCSCFG_ENABLE_IP4
    {
        uint32_t length = 0;
        char str[sizeof(char)*35*(10+1)];

        ICMP_STATS_PTR icmp = ICMP_stats();

        length = snprintf(str, sizeof(str),
        "%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n",
        icmp->COMMON.ST_RX_TOTAL,
        icmp->COMMON.ST_RX_MISSED,
        icmp->COMMON.ST_RX_ERRORS,
        icmp->COMMON.ST_RX_DISCARDED,
        icmp->ST_RX_BAD_CHECKSUM + icmp->ST_RX_SMALL_DGRAM,
        icmp->ST_RX_BAD_CODE,
        icmp->ST_RX_RD_NOTGATE,
        icmp->COMMON.ST_TX_TOTAL,
        icmp->COMMON.ST_TX_MISSED,
        icmp->COMMON.ST_TX_ERRORS,
        icmp->COMMON.ST_TX_DISCARDED,
        icmp->ST_RX_DESTUNREACH,
        icmp->ST_RX_TIMEEXCEED,
        icmp->ST_RX_PARMPROB,
        icmp->ST_RX_SRCQUENCH,
        icmp->ST_RX_REDIRECT,
        icmp->ST_RX_ECHO_REQ,
        icmp->ST_RX_ECHO_REPLY,
        icmp->ST_RX_TIME_REQ,
        icmp->ST_RX_TIME_REPLY,
        icmp->ST_RX_INFO_REQ,
        icmp->ST_RX_INFO_REPLY,
        icmp->ST_RX_OTHER,
        icmp->ST_TX_DESTUNREACH,
        icmp->ST_TX_TIMEEXCEED,
        icmp->ST_TX_PARMPROB,
        icmp->ST_TX_SRCQUENCH,
        icmp->ST_TX_REDIRECT,
        icmp->ST_TX_ECHO_REQ,
        icmp->ST_TX_ECHO_REPLY,
        icmp->ST_TX_TIME_REQ,
        icmp->ST_TX_TIME_REPLY,
        icmp->ST_TX_INFO_REQ,
        icmp->ST_TX_INFO_REPLY,
        icmp->ST_TX_OTHER);

        /* Write header and buffer with data */
        response.data = str;
        response.data_length = length;
        response.content_length = response.data_length;
        HTTPSRV_cgi_write(&response);
    }
    
#else
    {
        int i;

        response.content_length = strlen("unk\n")*35;
        response.data = "unk\n";
        response.data_length = 4;
        for (i = 0; i < 35; i++)
        {
            HTTPSRV_cgi_write(&response);
        }
    }

#endif
    return (response.content_length);
}
   
_mqx_int cgi_udpstat(HTTPSRV_CGI_REQ_STRUCT* param)
{
    HTTPSRV_CGI_RES_STRUCT response;

    if (param->request_method != HTTPSRV_REQ_GET)
    {
        return(0);
    }

    response.ses_handle = param->ses_handle;
    response.content_type = HTTPSRV_CONTENT_TYPE_PLAIN;
    response.status_code = 200;

#if RTCSCFG_ENABLE_UDP_STATS
    {
        uint32_t length = 0;
        char str[sizeof(char)*10*(10+1)];

        UDP_STATS_PTR udp = UDP_stats();

        length = snprintf(str, sizeof(str),
            "%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n", 
            udp->COMMON.ST_RX_TOTAL,
            udp->COMMON.ST_RX_MISSED,
            udp->COMMON.ST_RX_ERRORS,
            udp->COMMON.ST_RX_DISCARDED,
            udp->ST_RX_BAD_PORT + udp->ST_RX_BAD_CHECKSUM + udp->ST_RX_SMALL_DGRAM + udp->ST_RX_SMALL_PKT,                             
            udp->ST_RX_NO_PORT,
            udp->COMMON.ST_TX_TOTAL,
            udp->COMMON.ST_TX_MISSED,
            udp->COMMON.ST_TX_ERRORS,
            udp->COMMON.ST_TX_DISCARDED);

        /* Write header and buffer with data */
        response.data = str;
        response.data_length = length;
        response.content_length = response.data_length;
        HTTPSRV_cgi_write(&response);  
    }
    
#else
    {
        int i;

        response.content_length = strlen("unk\n")*10;
        response.data = "unk\n";
        response.data_length = 4;
        for (i = 0; i < 10; i++)
        {
            HTTPSRV_cgi_write(&response);
        }
    }

#endif
    return (response.content_length);
}
   
_mqx_int cgi_tcpstat(HTTPSRV_CGI_REQ_STRUCT* param)
{
    HTTPSRV_CGI_RES_STRUCT response;

    if (param->request_method != HTTPSRV_REQ_GET)
    {
        return(0);
    }

    response.ses_handle = param->ses_handle;
    response.content_type = HTTPSRV_CONTENT_TYPE_PLAIN;
    response.status_code = 200;
	
#if RTCSCFG_ENABLE_TCP_STATS
    {
        uint32_t length = 0;
        char str[sizeof(char)*32*(10+1)];
        TCP_STATS_PTR tcp  = TCP_stats();

        length = snprintf(str, sizeof(str), 
            "%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n", 
            tcp->COMMON.ST_RX_TOTAL,
            tcp->COMMON.ST_RX_MISSED,
            tcp->COMMON.ST_RX_ERRORS,
            tcp->COMMON.ST_RX_DISCARDED,
            tcp->ST_RX_BAD_PORT + tcp->ST_RX_BAD_CHECKSUM + tcp->ST_RX_SMALL_HDR + tcp->ST_RX_SMALL_DGRAM + tcp->ST_RX_SMALL_PKT,
            tcp->ST_RX_BAD_ACK,
            tcp->ST_RX_BAD_DATA,
            tcp->ST_RX_LATE_DATA,
            tcp->ST_RX_DATA,
            tcp->ST_RX_DATA_DUP,
            tcp->ST_RX_ACK,
            tcp->ST_RX_ACK_DUP,
            tcp->ST_RX_RESET,
            tcp->ST_RX_PROBE,
            tcp->ST_RX_WINDOW,
            tcp->COMMON.ST_TX_TOTAL,
            tcp->COMMON.ST_TX_MISSED,
            tcp->COMMON.ST_TX_ERRORS,
            tcp->COMMON.ST_TX_DISCARDED,
            tcp->ST_TX_DATA,
            tcp->ST_TX_DATA_DUP,
            tcp->ST_TX_ACK,
            tcp->ST_TX_ACK_DELAYED,
            tcp->ST_TX_RESET,
            tcp->ST_TX_PROBE,
            tcp->ST_TX_WINDOW,
            tcp->ST_CONN_ACTIVE,
            tcp->ST_CONN_PASSIVE,
            tcp->ST_CONN_OPEN,
            tcp->ST_CONN_CLOSED,
            tcp->ST_CONN_RESET,
            tcp->ST_CONN_FAILED);

        /* Write header and buffer with data */
        response.data = str;
        response.data_length = length;
        response.content_length = response.data_length;
        HTTPSRV_cgi_write(&response);
    }

#else
    {
        int i;
        
        response.content_length = strlen("unk\n")*32;
        response.data = "unk\n";
        response.data_length = 4;
        for (i = 0; i < 32; i++)
        {
            HTTPSRV_cgi_write(&response);
        }
    }
#endif
    return (response.content_length);
}
