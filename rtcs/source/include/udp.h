#ifndef __udp_h__
#define __udp_h__
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
*   This file contains the User Datagram Protocol definitions.
*   For more details, refer to RFC768.
*
*
*END************************************************************************/


/***************************************
**
** Constants
**
*/

/*
** UDP Open Options
*/
#define UDPOPT_CHKSUM_RX   0x0001      /* Option to bypass UDP checksum on Rx */
#define UDPOPT_CHKSUM_TX   0x0002      /* Option to bypass UDP checksum on Tx */
#define UDPOPT_NOBLOCK     0x0004      /* Option to send non-blocking         */


/***************************************
**
** Type definitions
**
*/


/*
** UDP Channel Block for listeners
*/
struct ucb_parm;
struct MCB;
struct ip6_multicast_member;
typedef struct ucb_struct {

   struct ucb_struct        *NEXT;
   uint32_t                 SOCKET;

   /* local UDP port */
   uint16_t                 PORT;
   uint16_t                 PCOUNT;

   void (_CODE_PTR_ SERVICE)(RTCSPCB_PTR, struct ucb_struct *);

   /*
   ** Queue of incoming packets
   */
   RTCSPCB_PTR    PHEAD;
   RTCSPCB_PTR    PTAIL;

   /*
   ** Queue of waiting receive requests
   */
   struct ucb_parm        *RHEAD;
   struct ucb_parm        *RTAIL;

   /* Checksum bypass on reception */
   bool        BYPASS_RX;

   /* Checksum bypass on transmission */
   bool        BYPASS_TX;

   /* Non-blocking mode */
   bool        NONBLOCK_RX;
   bool        NONBLOCK_TX;
#if RTCSCFG_ENABLE_IP4
   /* local IP address, usually INADDR_ANY */
   _ip_address    IPADDR;
   /* Remote IP address and port when connected. (0 when not connected) */
   _ip_address    REMOTE_HOST;

#endif
   uint16_t       AF;
   /* list of joined multicast groups */
   struct mc_member         *MCB_PTR;
   uint32_t (_CODE_PTR_     IGMP_LEAVEALL)(struct mc_member  **);

#if RTCSCFG_ENABLE_IP6
   struct ip6_multicast_member   *IP6_MULTICAST_PTR[RTCSCFG_IP6_MULTICAST_SOCKET_MAX];
   in6_addr        REMOTE_HOST6;
   in6_addr        IPV6ADDR;  
   uint32_t        IF_SCOPE_ID;
#endif
   
   /*
   ** Determines if a connection failure keeps the ucb's local IP,
   ** or if it resets it to INADDR_ANY
   */
   bool            KEEP_IPADDR;
   uint16_t        REMOTE_PORT;
   //uint16_t        RESERVED0;
} UCB_STRUCT, * UCB_STRUCT_PTR;

/*
** UDP parameters
*/
typedef struct ucb_parm {
   TCPIP_PARM              COMMON;
   struct ucb_parm        *NEXT;
   UCB_STRUCT_PTR          ucb;
   void (_CODE_PTR_        udpservice)(RTCSPCB_PTR, UCB_STRUCT_PTR);
   _ip_address             ipaddress;
#if RTCSCFG_ENABLE_IP6    
   in6_addr                ipv6address; 
   uint32_t                 if_scope_id; 
#endif
   uint16_t                 af;
   uint16_t                 udpport;
   uint16_t                 udpflags;
   void                   *udpptr;
   uint32_t                 udpword;
} UDP_PARM, * UDP_PARM_PTR;


/***************************************
**
** Prototypes
**
*/

extern uint32_t UDP_init
(
   void
);

extern void UDP_open     (UDP_PARM_PTR);
extern void UDP_bind     (UDP_PARM_PTR);

#if RTCSCFG_ENABLE_IP6
    extern void UDP_bind6    (UDP_PARM_PTR);
#endif

extern void UDP_connect  (UDP_PARM_PTR);

#if RTCSCFG_ENABLE_IP6
    extern void UDP_connect6 (UDP_PARM_PTR);
#endif

extern void UDP_close    (UDP_PARM_PTR);
extern void UDP_send     (UDP_PARM_PTR);
extern void UDP_receive  (UDP_PARM_PTR);

#if RTCSCFG_ENABLE_IP6
extern void UDP_receive6 (UDP_PARM_PTR);
#endif

extern void UDP_getopt   (UDP_PARM_PTR);
extern void UDP_setopt   (UDP_PARM_PTR);

extern uint32_t UDP_openbind_internal
(
   uint16_t              localport,
   void (_CODE_PTR_     service)(RTCSPCB_PTR, UCB_STRUCT_PTR),
   UCB_STRUCT_PTR      *ucb
);

extern uint32_t UDP_close_internal
(
   UCB_STRUCT_PTR       ucb
);

#if RTCSCFG_ENABLE_IP6
    extern uint32_t UDP_send_internal6
    (
        UCB_STRUCT_PTR       ucb,        /* [IN] UDP layer context      */
        in6_addr             srcaddr,    /* [IN] source IPv6 address      */
        in6_addr             destaddr,   /* [IN] destination IPv6 address */
        uint16_t              destport,   /* [IN] destination UDP port   */
        RTCSPCB_PTR          pcb_ptr,    /* [IN] packet to send         */
        uint32_t              flags       /* [IN] optional flags         */
    );
#endif


extern uint32_t UDP_send_internal
(
   UCB_STRUCT_PTR       ucb,        /* [IN] UDP layer context      */
   _ip_address          srcaddr,    /* [IN] source IP address      */
   _ip_address          destaddr,   /* [IN] destination IP address */
   uint16_t              destport,   /* [IN] destination UDP port   */
   RTCSPCB_PTR          pcb_ptr,    /* [IN] packet to send         */
   uint32_t              flags       /* [IN] optional flags         */
);

extern uint32_t UDP_send_IF
(
   UCB_STRUCT_PTR,
   void *,
   uint16_t,
   RTCSPCB_PTR
);


#if RTCSCFG_ENABLE_IP6
    extern void UDP_service6        /* Use function like service for UDP v6*/
    (
        RTCSPCB_PTR,
        void *
    );
#endif    

#if RTCSCFG_ENABLE_IP4
    extern void UDP_service         /* Use function like service for UDP v4*/
    (
        RTCSPCB_PTR,
        void *
    );
#endif

    extern void UDP_service_common     /* This function called from UDP_service or UDP_service6 */ 
    (                               /* with parameter of INET FAMILY */
        RTCSPCB_PTR,
        void *,
        uint16_t                     /* INET FAMILY parameter */
    );

extern void UDP_process
(
   RTCSPCB_PTR,
   UCB_STRUCT_PTR
);

#if RTCSCFG_ENABLE_IP6
    extern void UDP_process6
    (
        RTCSPCB_PTR,
        UCB_STRUCT_PTR
    );
#endif    

#endif
/* EOF */
