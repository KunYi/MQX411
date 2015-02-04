/*HEADER**********************************************************************
*
* Copyright 2008-2011 Freescale Semiconductor, Inc.
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
*   This file contains the definitions for configuring
*   optional features in RTCS.
*   RTCS is delivered as follows:
*   RTCSCFG_CHECK_ERRORS                   1
*   RTCSCFG_CHECK_MEMORY_ALLOCATION_ERRORS 1
*   RTCSCFG_CHECK_VALIDITY                 1
*   RTCSCFG_CHECK_ADDRSIZE                 1
*   RTCSCFG_IP_DISABLE_DIRECTED_BROADCAST  0
*   RTCSCFG_BOOTP_RETURN_YIADDR            0
*   RTCSCFG_UDP_ENABLE_LBOUND_MULTICAST    0  
*   RTCSCFG_LOG_SOCKET_API                 0
*   RTCSCFG_LOG_PCB                        0
*   RTCSCFG_LINKOPT_8023                   0
*   RTCSCFG_LINKOPT_8021Q_PRIO             0
*   RTCSCFG_DISCARD_SELF_BCASTS            1
*
*   RTCSCFG_LOGGING                        0
*
*
*END************************************************************************/

#ifndef __rtcscfg_h__
#define __rtcscfg_h__

/***************************************************************************
**
** Code and Data configuration options
** MGCT: <category name="Protocol and Features Options">
**
*/

/* 
** IPv6 Protocol
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IP6
   #define RTCSCFG_ENABLE_IP6		0
#endif

/* 
** IPv4 Protocol
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IP4
   #define RTCSCFG_ENABLE_IP4		1
#endif

/* 
** ICMPv4 Protocol
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_ICMP
    #define RTCSCFG_ENABLE_ICMP     1
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IGMP
    #define RTCSCFG_ENABLE_IGMP     0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_NAT
   #define RTCSCFG_ENABLE_NAT       0 
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_LWDNS
   #define RTCSCFG_ENABLE_LWDNS  1 
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_DNS
    #define RTCSCFG_ENABLE_DNS      0
#endif

#if RTCSCFG_ENABLE_DNS == 1
    #ifdef RTCSCFG_ENABLE_LOOPBACK
        #undef  RTCSCFG_ENABLE_LOOPBACK      
    #endif
    #define RTCSCFG_ENABLE_LOOPBACK 1
    #ifdef RTCSCFG_ENABLE_LWDNS
        #undef  RTCSCFG_ENABLE_LWDNS
    #endif
    #define RTCSCFG_ENABLE_LWDNS 0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IPSEC
   #define RTCSCFG_ENABLE_IPSEC 0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IPIP
    #define RTCSCFG_ENABLE_IPIP 0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_RIP
    #define RTCSCFG_ENABLE_RIP  0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_OSPF
   #define RTCSCFG_ENABLE_OSPF 0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_SNMP
    #define RTCSCFG_ENABLE_SNMP 0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IP_REASSEMBLY
    #define RTCSCFG_ENABLE_IP_REASSEMBLY 0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_LOOPBACK
    #define RTCSCFG_ENABLE_LOOPBACK 0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_UDP
    #define RTCSCFG_ENABLE_UDP 1
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_TCP
    #define RTCSCFG_ENABLE_TCP 1
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_STATS
    #define RTCSCFG_ENABLE_STATS 0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_GATEWAYS
    #define RTCSCFG_ENABLE_GATEWAYS 1
#endif

/*
** Note: must be true for PPP or tunneling
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_VIRTUAL_ROUTES
    #define RTCSCFG_ENABLE_VIRTUAL_ROUTES 0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_USE_KISS_RNG 
   #define RTCSCFG_USE_KISS_RNG 0
#endif

/* MGCT: </category> */

/***************************************************************************
**
** Error Checking options
** 
** MGCT: <category name="Error Checking Options">
**
****************************************************************************/

/*
** When RTCSCFG_CHECK_ERRORS is 1, RTCS API functions will perform
** error checking on its input parameters and also some other.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_CHECK_ERRORS
   #define RTCSCFG_CHECK_ERRORS 1
#endif

/*
** When RTCSCFG_CHECK_MEMORY_ALLOCATION_ERRORS is 1, RTCS API functions
** will perform error checking on memory allocation.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_CHECK_MEMORY_ALLOCATION_ERRORS
   #define RTCSCFG_CHECK_MEMORY_ALLOCATION_ERRORS 1
#endif

/*
** When RTCSCFG_CHECK_VALIDITY is 1, RTCS will check the VALID field
** in internal structures before accessing them.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_CHECK_VALIDITY
   #define RTCSCFG_CHECK_VALIDITY 1
#endif

/*
** When RTCSCFG_CHECK_ADDRSIZE is 1, RTCS will check whether addrlen
** is at least sizeof(sockaddr_in) in functions that take a struct
** sockaddr  *parameter.  If not, RTCS returns an error (for bind(),
** connect() and sendto()) or does a partial copy (for accept(),
** getsockname(), getpeername() and recvfrom()).
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_CHECK_ADDRSIZE
   #define RTCSCFG_CHECK_ADDRSIZE 1
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** PCB Configuration options
**
** MGCT: <category name="PCB Configuration Options">
**
*/

/*
** Override in application by setting _RTCSPCB_max
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_PCBS_INIT
    #define RTCSCFG_PCBS_INIT 4
#endif

/*
** Override in application by setting _RTCSPCB_max
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_PCBS_GROW
   #define RTCSCFG_PCBS_GROW 0 
#endif

/*
** Override in application by setting _RTCSPCB_max
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef RTCSCFG_PCBS_MAX
   #define RTCSCFG_PCBS_MAX RTCSCFG_PCBS_INIT 
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** Socket Configuration options
**
** MGCT: <category name="Socket Configuration Options">
**
*/

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_SOCKET_OWNERSHIP
  #define RTCSCFG_SOCKET_OWNERSHIP 0
#endif

/*
** Number of blocks allocated for sockets
** Override in application by setting _RTCS_socket_part_init
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_SOCKET_PART_INIT
  #define RTCSCFG_SOCKET_PART_INIT 6 
#endif

/*
** Override in application by setting _RTCS_socket_part_grow
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_SOCKET_PART_GROW
  #define RTCSCFG_SOCKET_PART_GROW 1
#endif

/*
** Override in application by setting _RTCS_socket_part_limit
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef RTCSCFG_SOCKET_PART_MAX
   #define RTCSCFG_SOCKET_PART_MAX RTCSCFG_SOCKET_PART_INIT 
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** RTCS Message Configuration options
**
** MGCT: <category name="RTCS Task and Message Options">
**
*/

/*
** Override in application by setting _RTCS_msgpool_init
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_MSGPOOL_INIT
    #define RTCSCFG_MSGPOOL_INIT 10
#endif

/*
** Override in application by setting _RTCS_msgpool_grow
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_MSGPOOL_GROW
    #define RTCSCFG_MSGPOOL_GROW 2 
#endif

/*
** Override in application by setting _RTCS_msgpool_limit
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef RTCSCFG_MSGPOOL_MAX
    #define RTCSCFG_MSGPOOL_MAX RTCSCFG_MSGPOOL_INIT 
#endif

/* MGCT: </category> */

/***************************************************************************
**
** Link Layer options
**
** MGCT: <category name="Link Layer Options">
**
*/

/*
** RTCSCFG_LINKOPT_8023 enables support for sending and receiving
** 802.3 frames.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_LINKOPT_8023
   #define RTCSCFG_LINKOPT_8023 0
#endif

/*
** RTCSCFG_LINKOPT_8021Q_PRIO enables support for sending and receiving
** 802.1Q priority tags.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_LINKOPT_8021Q_PRIO
   #define RTCSCFG_LINKOPT_8021Q_PRIO 0
#endif

/* 
** RTCSCFG_DISCARD_SELF_BCASTS controls whether or not to discard all 
** broadcast pkts that we sent, as they are likely echoes from older 
** hubs
** MGCT: <option type="bool"/>
*/ 
#ifndef RTCSCFG_DISCARD_SELF_BCASTS
   #define RTCSCFG_DISCARD_SELF_BCASTS 1
#endif

/* MGCT: </category> */


/***************************************************************************
** 
** ARP Configuration options
**
** MGCT: <category name="ARP Configuration Options">
**
*/

/*
** ARP cache hash table size
** MGCT: <option type="number"/>
*/
#ifndef ARPCACHE_SIZE
    #define ARPCACHE_SIZE 4
#endif

/*
** Allocation blocks for ARP cache entries
** MGCT: <option type="number"/>
*/
#ifndef ARPALLOC_SIZE
    #define ARPALLOC_SIZE 4 
#endif

/*
** ARP cache entry longevity (in milliseconds)
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ARPTIME_RESEND_MIN
    #define RTCSCFG_ARPTIME_RESEND_MIN             5000     /*  5 sec */
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ARPTIME_RESEND_MAX  
    #define RTCSCFG_ARPTIME_RESEND_MAX            30000     /* 30 sec */
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ARPTIME_EXPIRE_INCOMPLETE
    #define RTCSCFG_ARPTIME_EXPIRE_INCOMPLETE    180000L    /*  3 min */
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ARPTIME_EXPIRE_COMPLETE
    #define RTCSCFG_ARPTIME_EXPIRE_COMPLETE     1200000L    /* 20 min */
#endif

/*
** Number of PCBs queued on an outstanding ARP
** Per RFC, Minimum recommended is 1.
** When sending large UDP packets, which will result 
** in IP fragmentations, set to at least the largest number of 
** fragments
** MGCT: <option type="number"/>
*/
#ifndef ARP_ENTRY_MAX_QUEUED_PCBS
    #define ARP_ENTRY_MAX_QUEUED_PCBS 1 
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_ARP_STATS
    #if RTCSCFG_ENABLE_STATS
        #define RTCSCFG_ENABLE_ARP_STATS 1
    #else
        #define RTCSCFG_ENABLE_ARP_STATS 0
    #endif
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** IPv4 Configuration options
**
** MGCT: <category name="IPv4 Configuration Options">
**
*/

/*
** MGCT: <option type="number"/>
*/
#ifndef IPIFALLOC_SIZE
    #define IPIFALLOC_SIZE 2 
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef IPROUTEALLOC_SIZE
    #define IPROUTEALLOC_SIZE 2 
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef IPGATEALLOC_SIZE
    #define IPGATEALLOC_SIZE 1 
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef IPMCBALLOC_SIZE
    #define IPMCBALLOC_SIZE 1 
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef RADIXALLOC_SIZE
    #define RADIXALLOC_SIZE 2 
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IP_STATS
    #if RTCSCFG_ENABLE_STATS
        #define RTCSCFG_ENABLE_IP_STATS 1
    #else
        #define RTCSCFG_ENABLE_IP_STATS 0
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_ICMP_STATS
    #if (RTCSCFG_ENABLE_STATS && RTCSCFG_ENABLE_ICMP)
        #define RTCSCFG_ENABLE_ICMP_STATS 1
    #else
        #define RTCSCFG_ENABLE_ICMP_STATS 0
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IPIF_STATS
    #if RTCSCFG_ENABLE_STATS
        #define RTCSCFG_ENABLE_IPIF_STATS 1
    #else
        #define RTCSCFG_ENABLE_IPIF_STATS 0
    #endif
#endif

/* MGCT: </category> */


/***************************************************************************
** 
** IGMP Configuration options
**
** MGCT: <category name="IGMP Configuration Options">
**
*/

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IGMP_STATS
    #if (RTCSCFG_ENABLE_STATS && RTCSCFG_ENABLE_IGMP)
        #define RTCSCFG_ENABLE_IGMP_STATS 1
    #else
        #define RTCSCFG_ENABLE_IGMP_STATS 0
    #endif
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** UDP Configuration options
**
** MGCT: <category name="UDP Configuration Options">
**
*/

/*
** Override in application by setting _UDP_max_queue_size
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef RTCSCFG_UDP_MAX_QUEUE_SIZE
    #define RTCSCFG_UDP_MAX_QUEUE_SIZE 1 
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_UDP_STATS
    #if (RTCSCFG_ENABLE_STATS && RTCSCFG_ENABLE_UDP)
        #define RTCSCFG_ENABLE_UDP_STATS 1
    #else
        #define RTCSCFG_ENABLE_UDP_STATS 0
    #endif
#endif


/* MGCT: </category> */

/***************************************************************************
** 
** TCP Configuration options
**
** MGCT: <category name="TCP Configuration Options">
**
*/

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_TCP_STATS
    #if (RTCSCFG_ENABLE_STATS && RTCSCFG_ENABLE_TCP)
        #define RTCSCFG_ENABLE_TCP_STATS 1
    #else
        #define RTCSCFG_ENABLE_TCP_STATS 0
    #endif
#endif

/*
** Maximum number of simultaneous connections allowed.  Define as 0 for no limit.
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_TCP_MAX_CONNECTIONS
   #define RTCSCFG_TCP_MAX_CONNECTIONS      0  
#endif

/*
** Maximum number of simultaneoushalf open connections allowed.
** Define as 0 to disable the SYN attack recovery feature. 
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_TCP_MAX_HALF_OPEN
   #define RTCSCFG_TCP_MAX_HALF_OPEN        0  
#endif

#ifndef RTCSCFG_TCP_ACKDELAY
    #define TCP_ACKDELAY                    1 
#else
    #if RTCSCFG_TCP_ACKDELAY > 500   /* Max ack delay, as per RFC1122 */
        #define TCP_ACKDELAY                500
    #else
        #define TCP_ACKDELAY                RTCSCFG_TCP_ACKDELAY
    #endif
#endif

/* MGCT: </category> */


/***************************************************************************
** 
** RIP Configuration options
**
** MGCT: <category name="RIP Configuration options">
**
*/

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_RIP_STATS
    #if RTCSCFG_ENABLE_STATS
        #define RTCSCFG_ENABLE_RIP_STATS 1
    #else
        #define RTCSCFG_ENABLE_RIP_STATS 0
    #endif
#endif

#if RTCSCFG_ENABLE_RIP
   #if !(RTCSCFG_ENABLE_IGMP && RTCSCFG_ENABLE_IP4) 
      #error RIP uses IGMP.  Enable RTCSCFG_ENABLE_IGMP or disable RTCSCFG_ENABLE_RIP in user_config.h
   #endif
#endif

/* MGCT: </category> */

/***************************************************************************
**
** Protocol behaviour options
**
** MGCT: <category name="Protocol Behaviour Options">
**
*/

/*
** RTCSCFG_IP_DISABLE_DIRECTED_BROADCAST disables the reception and
** forwarding of directed broadcast datagrams.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_IP_DISABLE_DIRECTED_BROADCAST
   #define RTCSCFG_IP_DISABLE_DIRECTED_BROADCAST 1
#endif

/*
** When RTCSCFG_BOOTP_RETURN_YIADDR is 1, the BOOTP_DATA_STRUCT has
** an additional field, which will be filled in with the YIADDR field
** of the BOOTREPLY.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_BOOTP_RETURN_YIADDR
   #define RTCSCFG_BOOTP_RETURN_YIADDR 0
#endif

/*
** When RTCSCFG_UDP_ENABLE_LBOUND_MULTICAST is 1, locally bound sockets
** that are members of multicast groups will be able to receive messages
** sent to both their unicast and multicast addresses.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_UDP_ENABLE_LBOUND_MULTICAST
   #define RTCSCFG_UDP_ENABLE_LBOUND_MULTICAST 0
#endif

/* MGCT: </category> */


/***************************************************************************
** 
** RTCS Configuration options
**
** MGCT: <category name="RTCS Configuration Options">
**
*/

/*
** Using a lower queue base requires fewer MQX resources
** Override in application by setting _RTCSQUEUE_base
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_QUEUE_BASE
    #define RTCSCFG_QUEUE_BASE 2 
#endif

/*
** Override in application by setting _RTCSTASK_stacksize
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef RTCSCFG_STACK_SIZE
    #define RTCSCFG_STACK_SIZE (750*sizeof(uint32_t)) 
#endif

/* MGCT: </category> */

/***************************************************************************
**
** Logging options
**
** MGCT: <category name="Logging Options">
**
*/

/*
** Enable RTCS logging functionality
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_LOGGING
    #define RTCSCFG_LOGGING 0
#endif

/*
** When RTCSCFG_LOG_SOCKET_API is 1, RTCS will call RTCS_log() on
** every socket API entry and exit.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_LOG_SOCKET_API
    #define RTCSCFG_LOG_SOCKET_API 0
#endif

/*
** When RTCSCFG_LOG_PCB is 1, RTCS will call RTCS_log() every time
** a PCB is allocated, freed, or passed between two layers.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_LOG_PCB
    #define RTCSCFG_LOG_PCB 0
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** IPCFG Configuration options
**
** MGCT: <category name="IPCFG API Configuration Options">
**
*/

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_IPCFG_ENABLE_DNS
    #if (RTCSCFG_ENABLE_DNS && RTCSCFG_ENABLE_UDP) || (RTCSCFG_ENABLE_LWDNS)
        #define RTCSCFG_IPCFG_ENABLE_DNS 1
    #else
        #define RTCSCFG_IPCFG_ENABLE_DNS 0
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_IPCFG_ENABLE_DHCP
    #if RTCSCFG_ENABLE_UDP
        #define RTCSCFG_IPCFG_ENABLE_DHCP 1
    #else
        #define RTCSCFG_IPCFG_ENABLE_DHCP 0
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_IPCFG_ENABLE_BOOT
    #define RTCSCFG_IPCFG_ENABLE_BOOT 0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_USE_MQX_PARTITIONS
    #define RTCSCFG_USE_MQX_PARTITIONS 1
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_USE_INTERNAL_PARTITIONS
    #if RTCSCFG_USE_MQX_PARTITIONS
        #define RTCSCFG_USE_INTERNAL_PARTITIONS 0
    #else
        #define RTCSCFG_USE_INTERNAL_PARTITIONS 1
    #endif
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** SMTP Client Configuration options
**
** MGCT: <category name="SMTP Client Configuration options">
**
*/

/*
** Override in application by setting RTCS_SMTP_PORT
** MGCT: <option type="number"/>
*/
#ifndef RTCS_SMTP_PORT
    #define RTCS_SMTP_PORT IPPORT_SMTP
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** FTP Client Configuration options
**
** MGCT: <category name="FTP Client Configuration Options">
**
*/

/*
** Enabled means better performance for files less than 4MB
** MGCT: <option type="bool"/>
*/
#ifndef FTPCCFG_SMALL_FILE_PERFORMANCE_ENHANCEMENT
   #define FTPCCFG_SMALL_FILE_PERFORMANCE_ENHANCEMENT 1 
#endif

/*
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#define FTPCCFG_BUFFER_SIZE 256

/*
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef FTPCCFG_WINDOW_SIZE
    #define FTPCCFG_WINDOW_SIZE 536 
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef FTPCCFG_TIMEWAIT_TIMEOUT
   #define FTPCCFG_TIMEWAIT_TIMEOUT 1000
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** FTP Server Configuration options
**
** MGCT: <category name="FTP Server Configuration Options">
**
*/

/*
** must be one of FLAG_ABORT_CONNECTION or FLAG_CLOSE_TX
** MGCT: <option type="list">
** <item name="FLAG_ABORT_CONNECTION" value="FLAG_ABORT_CONNECTION"/>
** <item name="FLAG_CLOSE_TX" value="FLAG_CLOSE_TX"/>
** </option>
*/
#ifndef FTPDCFG_SHUTDOWN_OPTION
   #define FTPDCFG_SHUTDOWN_OPTION FLAG_ABORT_CONNECTION  
#endif

/*
** MGCT: <option type="list">
** <item name="FLAG_ABORT_CONNECTION" value="FLAG_ABORT_CONNECTION"/>
** <item name="FLAG_CLOSE_TX" value="FLAG_CLOSE_TX"/>
** </option>
*/
#ifndef FTPDCFG_DATA_SHUTDOWN_OPTION
   #define FTPDCFG_DATA_SHUTDOWN_OPTION FLAG_CLOSE_TX  
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef FTPDCFG_USES_MFS
    #define FTPDCFG_USES_MFS 1
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef FTPDCFG_ENABLE_MULTIPLE_CLIENTS
    #define FTPDCFG_ENABLE_MULTIPLE_CLIENTS 1
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef FTPDCFG_ENABLE_USERNAME_AND_PASSWORD
    #define FTPDCFG_ENABLE_USERNAME_AND_PASSWORD 1
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef FTPDCFG_ENABLE_RENAME
   #define FTPDCFG_ENABLE_RENAME 1
#endif

/*
** Should be a multiple of maximum packet size
** Override in application by setting FTPd_window_size
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef FTPDCFG_WINDOW_SIZE
    #define FTPDCFG_WINDOW_SIZE 536 
#endif

/*
** Should be a multiple of sector size
** Override in application by setting FTPd_buffer_size
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef FTPDCFG_BUFFER_SIZE
    #define FTPDCFG_BUFFER_SIZE 512 
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef FTPDCFG_CONNECT_TIMEOUT
   #define FTPDCFG_CONNECT_TIMEOUT 1000 
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef FTPDCFG_SEND_TIMEOUT
   #define FTPDCFG_SEND_TIMEOUT 5000 
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef FTPDCFG_TIMEWAIT_TIMEOUT
   #define FTPDCFG_TIMEWAIT_TIMEOUT 500 
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** Telnet Configuration options
**
** MGCT: <category name="Telnet Configuration Options">
**
*/

/*
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef TELNETDCFG_BUFFER_SIZE
    #define TELNETDCFG_BUFFER_SIZE 256 
#endif

/*
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
*/
#ifndef TELNETDCFG_NOWAIT
   #define TELNETDCFG_NOWAIT FALSE 
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef TELNETDCFG_ENABLE_MULTIPLE_CLIENTS
    #define TELNETDCFG_ENABLE_MULTIPLE_CLIENTS 1
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef TELENETDCFG_CONNECT_TIMEOUT
   #define TELENETDCFG_CONNECT_TIMEOUT 1000 
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef TELENETDCFG_SEND_TIMEOUT
   #define TELENETDCFG_SEND_TIMEOUT 5000 
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef TELENETDCFG_TIMEWAIT_TIMEOUT
   #define TELENETDCFG_TIMEWAIT_TIMEOUT 1000 
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** SNMP Configuration options
**
** MGCT: <category name="SNMP Configuration Options">
**
*/

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_SNMP_STATS
    #if RTCSCFG_ENABLE_STATS
        #define RTCSCFG_ENABLE_SNMP_STATS 1
    #else
        #define RTCSCFG_ENABLE_SNMP_STATS 0
    #endif
#endif

/* MGCT: </category> */

/***************************************************************************
** 
** PPPHDLC Message Configuration options
**
** MGCT: <category name="PPPHDLC Message Configuration Options">
**
*/

/*
** MGCT: <option type="number"/>
*/
#ifndef PPPHDLC_INIT
    #define PPPHDLC_INIT 6 
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef PPPHDLC_GROW
    #define PPPHDLC_GROW 1 
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef PPPHDLC_MAX
    #define PPPHDLC_MAX PPPHDLC_INIT 
#endif

/* MGCT: </category> */

/*
** MGCT: <category name="FTPSRV Configuration Options">
*/

/*
** MGCT: <option type="number"/>
*/
#ifndef FTPSRVCFG_DEF_SERVER_PRIO
#define FTPSRVCFG_DEF_SERVER_PRIO    (8)
#endif

/*
** Default listen address
** MGCT: <option type="string"/>
*/
#ifndef FTPSRVCFG_DEF_ADDR
#define FTPSRVCFG_DEF_ADDR               INADDR_ANY
#endif

/*
** Default sessions count
** MGCT: <option type="number"/>
*/
#ifndef FTPSRVCFG_DEF_SES_CNT
#define FTPSRVCFG_DEF_SES_CNT            (2)
#endif

/*
** Socket transmit buffer size
** MGCT: <option type="number"/>
*/
#ifndef FTPSRVCFG_TX_BUFFER_SIZE
  #define FTPSRVCFG_TX_BUFFER_SIZE    (1460)
#endif

/*
** Socket receive buffer size
** MGCT: <option type="number"/>
*/
#ifndef FTPSRVCFG_RX_BUFFER_SIZE
  #define FTPSRVCFG_RX_BUFFER_SIZE    (1460)
#endif

/*
** Socket OPT_TIMEWAIT_TIMEOUT option value
** MGCT: <option type="number"/>
*/
#ifndef FTPCFG_TIMEWAIT_TIMEOUT
   #define FTPSRVCFG_TIMEWAIT_TIMEOUT     (1000)
#endif

/*
** Socket OPT_SEND_TIMEOUT option value
** MGCT: <option type="number"/>
*/
#ifndef FTPSRVCFG_SEND_TIMEOUT
   #define FTPSRVCFG_SEND_TIMEOUT         (500)
#endif

/*
** Socket OPT_CONNECT_TIMEOUT option value
** MGCT: <option type="number"/>
*/
#ifndef FTPSRVCFG_CONNECT_TIMEOUT
   #define FTPSRVCFG_CONNECT_TIMEOUT      (5000)
#endif

/*
** Socket OPT_RECEIVE_TIMEOUT option value
** MGCT: <option type="number"/>
*/
#ifndef FTPSRVCFG_RECEIVE_TIMEOUT
   #define FTPSRVCFG_RECEIVE_TIMEOUT      (50)
#endif

/*
** Default inet family configuration
** MGCT: <option type="string"/>
*/
#ifndef FTPSRVCFG_AF
   #define FTPSRVCFG_AF              AF_INET
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef FTPSRVCFG_USES_MFS
  #define FTPSRVCFG_USES_MFS              (1)
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef FTPSRVCFG_IS_ANONYMOUS
  #define FTPSRVCFG_IS_ANONYMOUS             (0)
#endif

/*
** Size of buffer used for file read/write operations.
** MGCT: <option type="number"/>
*/
#ifndef FTPSRVCFG_DATA_BUFFER_SIZE
  #define FTPSRVCFG_DATA_BUFFER_SIZE      (1460)
#endif

/* MGCT: </category> */

/*
** MGCT: <category name="HTTPSRV Configuration Options">
*/

/*
** MGCT: <option type="number"/>
*/
#ifndef HTTPSRVCFG_DEF_SERVER_PRIO
#define HTTPSRVCFG_DEF_SERVER_PRIO    (8)
#endif

/*
** Default listen address
** MGCT: <option type="string"/>
*/
#ifndef HTTPSRVCFG_DEF_ADDR
#define HTTPSRVCFG_DEF_ADDR               INADDR_ANY
#endif

/*
** Number of seconds for caching
** MGCT: <option type="number"/>
*/
#ifndef HTTPSRVCFG_CACHE_MAXAGE
  #define HTTPSRVCFG_CACHE_MAXAGE          3600
#endif

/*
** Default listen port
** MGCT: <option type="number"/>
*/
#ifndef HTTPSRVCFG_DEF_PORT
#define HTTPSRVCFG_DEF_PORT               80
#endif

/*
** MGCT: <option type="string" quoted="true"/>
*/
#ifndef HTTPSRVCFG_DEF_INDEX_PAGE
#define HTTPSRVCFG_DEF_INDEX_PAGE         "index.htm"
#endif

/*
** Default sessions count
** MGCT: <option type="number"/>
*/
#ifndef HTTPSRVCFG_DEF_SES_CNT
#define HTTPSRVCFG_DEF_SES_CNT            (2) 
#endif

/*
** Session buffer size
** MGCT: <option type="number"/>
*/
#ifndef HTTPSRVCFG_SES_BUFFER_SIZE
#define HTTPSRVCFG_SES_BUFFER_SIZE        (1360)
#endif

#if HTTPSRVCFG_SES_BUFFER_SIZE < 512
#undef HTTPSRVCFG_SES_BUFFER_SIZE
#define HTTPSRVCFG_SES_BUFFER_SIZE        (512)
#endif
/*
** Maximal URL length
** MGCT: <option type="number"/>
*/
#ifndef HTTPSRVCFG_DEF_URL_LEN
#define HTTPSRVCFG_DEF_URL_LEN            (128)
#endif

/*
** Maximal length for script line
** MGCT: <option type="number"/>
*/
#ifndef HTTPSRVCFG_MAX_SCRIPT_LN
#define HTTPSRVCFG_MAX_SCRIPT_LN          (16)
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef HTTPSRVCFG_KEEPALIVE_ENABLED
#define HTTPSRVCFG_KEEPALIVE_ENABLED       (0)
#endif

/*
** Session keep-alive timeout in milliseconds
** MGCT: <option type="number"/>
*/
#ifndef HTTPSRVCFG_KEEPALIVE_TO
#define HTTPSRVCFG_KEEPALIVE_TO            (200)
#endif

/*
** Session timeout in milliseconds
** MGCT: <option type="number"/>
*/
#ifndef HTTPSRVCFG_SES_TO
#define HTTPSRVCFG_SES_TO                 (20000)
#endif

/* 
** Socket transmit buffer size
** MGCT: <option type="number"/>
*/
#ifndef HTTPSRVCFG_TX_BUFFER_SIZE
  #define HTTPSRVCFG_TX_BUFFER_SIZE    (1460) 
#endif

/* 
** Socket receive buffer size
** MGCT: <option type="number"/>
*/
#ifndef HTTPSRVCFG_RX_BUFFER_SIZE
  #define HTTPSRVCFG_RX_BUFFER_SIZE    (1460) 
#endif

/*
** Socket OPT_TIMEWAIT_TIMEOUT option value
** MGCT: <option type="number"/>
*/
#ifndef HTTPCFG_TIMEWAIT_TIMEOUT
   #define HTTPSRVCFG_TIMEWAIT_TIMEOUT     (1000)
#endif

/*
** Socket OPT_SEND_TIMEOUT option value
** MGCT: <option type="number"/>
*/
#ifndef HTTPSRVCFG_SEND_TIMEOUT
   #define HTTPSRVCFG_SEND_TIMEOUT         (500)
#endif

/*
** Socket OPT_CONNECT_TIMEOUT option value
** MGCT: <option type="number"/>
*/
#ifndef HTTPSRVCFG_CONNECT_TIMEOUT
   #define HTTPSRVCFG_CONNECT_TIMEOUT      (5000)
#endif

/*
** Socket OPT_RECEIVE_TIMEOUT option value
** MGCT: <option type="number"/>
*/
#ifndef HTTPSRVCFG_RECEIVE_TIMEOUT
   #define HTTPSRVCFG_RECEIVE_TIMEOUT      (50)
#endif

/* 
** Default inet family configuration
** MGCT: <option type="string"/>
*/
#ifndef HTTPSRVCFG_AF
   #define HTTPSRVCFG_AF        			AF_INET
#endif

/* MGCT: </category> */

/*
** MGCT: <category name="IPv6 Specific Options">
*/


/*
** Maximum number of entries in neighbor cache.
** NOTE: A small cache may result in an
** excessive number of Neighbor Discovery messages if entries are
** discarded and rebuilt in quick succession.
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ND6_NEIGHBOR_CACHE_SIZE
   #define RTCSCFG_ND6_NEIGHBOR_CACHE_SIZE      (5) 
#endif

/*
** Maximum number of entries in the Prefix list (per interface).
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ND6_PREFIX_LIST_SIZE
    #define RTCSCFG_ND6_PREFIX_LIST_SIZE        (4) 
#endif

/*
** Maximum number of entries in the Default Router list (per interface).
** RFC4861: A host MUST retain at least two router
** addresses and SHOULD retain more.
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ND6_ROUTER_LIST_SIZE
    #define RTCSCFG_ND6_ROUTER_LIST_SIZE        (2)
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IP6_REASSEMBLY
    #define RTCSCFG_ENABLE_IP6_REASSEMBLY       (1)
#endif

/*
** Maximum number of IPv6 addresses per interface.
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_IP6_IF_ADDRESSES_MAX
    #define RTCSCFG_IP6_IF_ADDRESSES_MAX        (5) 
#endif
/* Check minimum value.*/
#if RTCSCFG_IP6_IF_ADDRESSES_MAX < 2   
    #undef RTCSCFG_IP6_IF_ADDRESSES_MAX
    #define RTCSCFG_IP6_IF_ADDRESSES_MAX        (2)
#endif

/*
** Maximum number of DNSv6 Server addresses per interface.
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_IP6_IF_DNS_MAX
    #define RTCSCFG_IP6_IF_DNS_MAX              (2) 
#endif


/**  Maximum number of unique IPv6 multicast memberships that may exist at 
**  the same time in the whole system.
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_IP6_MULTICAST_MAX
    #define RTCSCFG_IP6_MULTICAST_MAX           (10)
#endif
/* Check minimum value.*/
#if RTCSCFG_IP6_MULTICAST_MAX <= RTCSCFG_IP6_IF_ADDRESSES_MAX
    #undef RTCSCFG_IP6_MULTICAST_MAX
    #define RTCSCFG_IP6_MULTICAST_MAX        (RTCSCFG_IP6_IF_ADDRESSES_MAX+1)
#endif

/** Maximum number of IPv6 multicast memberships that may exist at 
 *  the same time per one socket.
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_IP6_MULTICAST_SOCKET_MAX
    #define RTCSCFG_IP6_MULTICAST_SOCKET_MAX    (1)
#endif

/* 
** Multicast Listener Discovery (MLDv1) Protocol.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_MLD
    #define RTCSCFG_ENABLE_MLD                  (0)
#endif

/*
** Loopback of own multicast packets.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_IP6_LOOPBACK_MULTICAST
    #define RTCSCFG_IP6_LOOPBACK_MULTICAST      (0)
#endif

/*
** Path MTU Discovery for IPv6.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_IP6_PMTU_DISCOVERY
    #define RTCSCFG_IP6_PMTU_DISCOVERY          (1)
#endif


/* Check minimum values.*/
#if RTCSCFG_ND6_ROUTER_LIST_SIZE < 1 
    #error RTCSCFG_ND6_ROUTER_LIST_SIZE should be > 0
#endif
#if RTCSCFG_ND6_NEIGHBOR_CACHE_SIZE  < 1
    #error RTCSCFG_ND6_NEIGHBOR_CACHE_SIZE should be > 0
#endif
#if RTCSCFG_ND6_PREFIX_LIST_SIZE < 1 
    #error RTCSCFG_ND6_PREFIX_LIST_SIZE should be > 0
#endif

/*
** The number of consecutive Neighbor
** Solicitation messages sent while performing Duplicate Address
** Detection on a tentative address. A value of zero indicates that
** Duplicate Address Detection is not performed on tentative
** addresses. A value of one indicates a single transmission with no
** follow-up retransmissions.
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ND6_DAD_TRANSMITS 
    #define RTCSCFG_ND6_DAD_TRANSMITS           (1) /* If RTCSCFG_ND6_DAD_TRANSMITS = 0, the DAD is disabled.*/
#endif  

/*
** Recursive DNS Server Option (RFC6106) support.
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ND6_RDNSS
    #define RTCSCFG_ND6_RDNSS                   (1)
#endif

/** Maximum number of entries in the RDNSS addresses list, per networking interface.
** RFC6106 specifies a sufficient number of RDNSS addresses as three.
** MGCT: <option type="number"/>
*/
#ifndef RTCSCFG_ND6_RDNSS_LIST_SIZE
    #define RTCSCFG_ND6_RDNSS_LIST_SIZE         (3)
#endif

/* Control RTCSCFG_ND6_RDNSS_LIST_SIZE minimum value.*/
#if RTCSCFG_ND6_RDNSS_LIST_SIZE < 1
    #undef RTCSCFG_ND6_RDNSS_LIST_SIZE
    #define RTCSCFG__ND6_RDNSS_LIST_SIZE        (1)
#endif
  

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_IP6_STATS
    #if (RTCSCFG_ENABLE_STATS && RTCSCFG_ENABLE_IP6)
        #define RTCSCFG_ENABLE_IP6_STATS 1
    #else
        #define RTCSCFG_ENABLE_IP6_STATS 0
    #endif
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef RTCSCFG_ENABLE_ICMP6_STATS
    #if (RTCSCFG_ENABLE_STATS && RTCSCFG_ENABLE_IP6)
        #define RTCSCFG_ENABLE_ICMP6_STATS 1
    #else
        #define RTCSCFG_ENABLE_ICMP6_STATS 0
    #endif
#endif



/* For Debug usage only.*/
#ifndef RTCSCFG_DEBUG_TRACE_ETH
   #define RTCSCFG_DEBUG_TRACE_ETH              (0)
#endif
#ifndef RTCSCFG_DEBUG_TRACE_IP6
   #define RTCSCFG_DEBUG_TRACE_IP6              (0)
#endif
#ifndef RTCSCFG_DEBUG_TRACE_ICMP6
   #define RTCSCFG_DEBUG_TRACE_ICMP6            (0)
#endif
#ifndef RTCSCFG_DEBUG_MSG
   #define RTCSCFG_DEBUG_MSG                    (0)
#endif

/* MGCT: </category> */


/******* In case parameters are not defined by BSP ************/
#ifndef BSPCFG_ENET_HW_TX_IP_CHECKSUM
    #define BSPCFG_ENET_HW_TX_IP_CHECKSUM           0
#endif
#ifndef BSPCFG_ENET_HW_TX_PROTOCOL_CHECKSUM
    #define BSPCFG_ENET_HW_TX_PROTOCOL_CHECKSUM     0
#endif
#ifndef BSPCFG_ENET_HW_RX_IP_CHECKSUM
    #define BSPCFG_ENET_HW_RX_IP_CHECKSUM           0
#endif
#ifndef BSPCFG_ENET_HW_RX_PROTOCOL_CHECKSUM
    #define BSPCFG_ENET_HW_RX_PROTOCOL_CHECKSUM     0
#endif
#ifndef BSPCFG_ENET_HW_RX_MAC_ERR
    #define BSPCFG_ENET_HW_RX_MAC_ERR               0
#endif
#ifndef BSP_ENET_DEVICE_COUNT
    #define BSP_ENET_DEVICE_COUNT                   0
#endif

#endif
/* EOF */
