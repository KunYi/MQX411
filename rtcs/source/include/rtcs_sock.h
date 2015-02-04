#ifndef __rtcs_sock_h__
#define __rtcs_sock_h__
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   RTCS Socket API.
*
*
*END************************************************************************/

/*
** Socket types
*/
#define SOCK_DGRAM      ((uint32_t)&SOCK_DGRAM_CALL)
#define SOCK_STREAM     ((uint32_t)&SOCK_STREAM_CALL)

/*
** Validity check for socket structures
*/
#define SOCKET_VALID       0x52544353  /* "rtcs" */

/*
** Default number of tasks that can simultaneously own a socket
*/
#define SOCKET_NUMOWNERS   8

/*
** protocol level at which option resides
*/
#define SOL_SOCKET      ((uint32_t)&SOL_SOCKET_CALL)
#define SOL_IP          ((uint32_t)&SOL_IP_CALL)

#if RTCSCFG_ENABLE_UDP
#define SOL_UDP         ((uint32_t)&SOL_UDP_CALL)
#else
#define SOL_UDP        (0)
#endif

#if RTCSCFG_ENABLE_TCP
#define SOL_TCP         ((uint32_t)&SOL_TCP_CALL)
#else
#define SOL_TCP        (0)
#endif

#if RTCSCFG_ENABLE_IGMP && RTCSCFG_ENABLE_IP4
#define SOL_IGMP        ((uint32_t)&SOL_IGMP_CALL)
#else
#define SOL_IGMP        (0)
#endif

#define SOL_LINK        ((uint32_t)&SOL_LINK_CALL)

#if RTCSCFG_ENABLE_NAT
#define SOL_NAT         ((uint32_t)&SOL_NAT_CALL)
#else
#define SOL_NAT         (0)
#endif

#if RTCSCFG_ENABLE_IP6
#define SOL_IP6      	((uint32_t)&SOL_IP6_CALL)
#else
#define SOL_IP6        	(0)
#endif

/*
**   socket option ids
*/
#define OPT_CONNECT_TIMEOUT            (0x01)
#define OPT_RETRANSMISSION_TIMEOUT     (0x02)
#define OPT_SEND_TIMEOUT               (0x03)
#define OPT_RECEIVE_TIMEOUT            (0x04)
#define OPT_RECEIVE_PUSH               (0x05)
#define OPT_SEND_NOWAIT                (0x06)
#define OPT_SEND_WAIT_OBSOLETE         (0x07) /* obsolete */
#define OPT_SEND_PUSH                  (0x08)
#define OPT_RECEIVE_NOWAIT             (0x09)
#define OPT_CHECKSUM_BYPASS            (0x0A)
#define OPT_ACTIVE                     (0x0B)
#define OPT_TBSIZE                     (0x0C)
#define OPT_RBSIZE                     (0x0D)
#define OPT_MAXRTO                     (0x0E)
#define OPT_MAXRCV_WND                 (0x0F)
#define OPT_KEEPALIVE                  (0x10)
#define OPT_NOWAIT                     (0x11)
#define OPT_NO_NAGLE_ALGORITHM         (0x12)
#define OPT_NOSWRBUF                   (0x13)
#define OPT_CALL_BACK_OBSOLETE         (0x14) /* obsolete */
#define OPT_SOCKET_TYPE                (0x15)
#define OPT_CALLBACK                   (0x16)
#define OPT_TIMEWAIT_TIMEOUT           (0x17)
#define OPT_TCPSECUREDRAFT_0           (0x18)
#define OPT_DELAY_ACK                  (0x19)

#define OPT_SOCKET_ERROR               (0x1c)

/*
**   default option values
*/
#define DEFAULT_CONNECT_TIMEOUT        (480000L)   /* 8 mins */
#define DEFAULT_RETRANSMISSION_TIMEOUT (3000)      /* 3 sec  */
#define DEFAULT_SEND_TIMEOUT           (DEFAULT_CONNECT_TIMEOUT/2)
#define DEFAULT_RECEIVE_TIMEOUT        (0)         /* no timeout */
#define DEFAULT_NOWAIT                 FALSE
#define DEFAULT_SEND_NOWAIT            FALSE
#define DEFAULT_SEND_WAIT              FALSE
#define DEFAULT_RECEIVE_NOWAIT         FALSE
#define DEFAULT_WAIT                   FALSE       /* don't wait for ack */
#define DEFAULT_PUSH                   TRUE        /* push */
#define DEFAULT_CHECKSUM_BYPASS        FALSE       /* perform checksum */
#define DEFAULT_ACTIVE                 TRUE
#define DEFAULT_TBSIZE                 (-1)
#define DEFAULT_RBSIZE                 (-1)
#define DEFAULT_MAXRTO                 (0)
#define DEFAULT_MAXRCV_WND             (0)
#define DEFAULT_KEEPALIVE              (0)
#define DEFAULT_NO_NAGLE_ALGORITHM     TRUE
#define DEFAULT_NOSWRBUF               FALSE
#define DEFAULT_CALL_BACK              NULL
#define DEFAULT_CALLBACK               NULL
#define DEFAULT_TIMEWAIT_TIMEOUT       (2*1000) /* msec */ /* 0 = default 4 mins.*/
#define DEFAULT_TCPSECUREDRAFT_0       FALSE
#define DEFAULT_DELAY_ACK              (TCP_ACKDELAY)
#define DEFAULT_IP6_MULTICAST_HOPS     (1)

/*
** Socket options
*/
#define RTCS_SO_TYPE                   OPT_SOCKET_TYPE
#define RTCS_SO_ERROR                  OPT_SOCKET_ERROR

#define RTCS_SO_IGMP_ADD_MEMBERSHIP    0
#define RTCS_SO_IGMP_DROP_MEMBERSHIP   1
#define RTCS_SO_IGMP_GET_MEMBERSHIP    2

#define RTCS_SO_IP_RX_DEST             0
#define RTCS_SO_IP_RX_TTL              1
#define RTCS_SO_IP_RX_TOS              2
#define RTCS_SO_IP_TX_TTL              3
#define RTCS_SO_IP_LOCAL_ADDR          4
#define RTCS_SO_IP_TX_TOS              5

#define RTCS_SO_LINK_TX_8023           0
#define RTCS_SO_LINK_TX_8021Q_PRIO     1
#define RTCS_SO_LINK_RX_8023           2
#define RTCS_SO_LINK_RX_8021Q_PRIO     3

#define RTCS_SO_UDP_NONBLOCK           OPT_SEND_NOWAIT
#define RTCS_SO_UDP_NONBLOCK_TX        OPT_SEND_NOWAIT
#define RTCS_SO_UDP_NONBLOCK_RX        OPT_RECEIVE_NOWAIT
#define RTCS_SO_UDP_NOCHKSUM           OPT_CHECKSUM_BYPASS

#define RTCS_SO_NAT_TIMEOUTS           0
#define RTCS_SO_NAT_PORTS              1
/* SOL_IP6 Options.*/
#define RTCS_SO_IP6_UNICAST_HOPS       0
#define RTCS_SO_IP6_MULTICAST_HOPS     1
#define RTCS_SO_IP6_JOIN_GROUP         2
#define RTCS_SO_IP6_LEAVE_GROUP        3


/*
** Flags for send[to]/recv[from]
*/
#define RTCS_MSG_O_NONBLOCK   0x0001
#define RTCS_MSG_S_NONBLOCK   0x0002
#define RTCS_MSG_BLOCK        (RTCS_MSG_O_NONBLOCK)
#define RTCS_MSG_NONBLOCK     (RTCS_MSG_O_NONBLOCK | RTCS_MSG_S_NONBLOCK)
#define RTCS_MSG_O_NOCHKSUM   0x0004
#define RTCS_MSG_S_NOCHKSUM   0x0008
#define RTCS_MSG_CHKSUM       (RTCS_MSG_O_NOCHKSUM)
#define RTCS_MSG_NOCHKSUM     (RTCS_MSG_O_NOCHKSUM | RTCS_MSG_S_NOCHKSUM)
#define RTCS_MSG_PEEK         0x0010
#define RTCS_MSG_NOLOOP       0x0020

/*
** Close methods for shutdown()
*/
#define FLAG_ABORT_CONNECTION       (0x0010)
#define FLAG_CLOSE_TX               (0x0001)


/*
** IP address definition
*/
typedef uint32_t _ip_address;

typedef struct in_addr {
   _ip_address s_addr;
} in_addr;

/*
** IP multicast group
*/
typedef struct ip_mreq {
   in_addr  imr_multiaddr;
   in_addr  imr_interface;
} ip_mreq;

/*
** Socket Address Structure
*/
typedef struct sockaddr_in {
   uint16_t  sin_family;
   uint16_t  sin_port;
   in_addr  sin_addr;
} sockaddr_in;


#if RTCSCFG_ENABLE_IP6
    typedef struct sockaddr
    {
        uint16_t	sa_family;
        char 		sa_data[22];
    } sockaddr;
#else
    #if RTCSCFG_ENABLE_IP4
        #define sockaddr   sockaddr_in
        #define sa_family   sin_family
    #endif
#endif

/****************************************************************
* IPv6 address definition (BSD-like).
*****************************************************************/
typedef struct in6_addr
{
    union
    {
        uint8_t     __u6_addr8[16];
        uint16_t    __u6_addr16[8];
        uint32_t 	__u6_addr32[4];
    } __u6_addr;			/* 128-bit IP6 address */
}in6_addr;
#define s6_addr   __u6_addr.__u6_addr8
/* Not RFC */
#define s6_addr8  __u6_addr.__u6_addr8
#define s6_addr16 __u6_addr.__u6_addr16
#define s6_addr32 __u6_addr.__u6_addr32

/****************************************************************
* Socket address for IPv6 (BSD-like).
*****************************************************************/
typedef struct sockaddr_in6
{
    uint16_t    sin6_family;    /* AF_INET6 */
    uint16_t    sin6_port;      /* Transport layer port # */
    in6_addr	sin6_addr;      /* IP6 address */
    uint32_t    sin6_scope_id;  /* scope zone index */
}sockaddr_in6;


/*
** IPv6 multicast group
*/
typedef struct ipv6_mreq
{
    in6_addr      ipv6imr_multiaddr; /* IPv6 multicast address of group. */
    unsigned int  ipv6imr_interface; /* Interface index. It equals to the scope zone index, defining network interface.*/
} ipv6_mreq;


/****************************************************************
*
* Helpful macros.
*
*****************************************************************/
#define RTCS_SOCKADDR_PORT(a)   (((sockaddr_in*)(a))->sin_port)
#define RTCS_SOCKADDR_ADDR(a)   (((sockaddr_in*)(a))->sin_addr)


#define IN6ADDR(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16) \
    	    { (a1), (a2), (a3), (a4), (a5), (a6), (a7), (a8),       \
            (a9), (a10), (a11), (a12), (a13), (a14), (a15), (a16) }

/*
 * Construct an IPv6 address from eight 16-bit words.
*/
#define in6addr(a, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15)    \
                         {                                                                  \
                            (a)->s6_addr[0] = a0;                                           \
                            (a)->s6_addr[1] = a1;                                           \
                            (a)->s6_addr[2] = a2;                                           \
                            (a)->s6_addr[3] = a3;                                           \
                            (a)->s6_addr[4] = a4;                                           \
                            (a)->s6_addr[5] = a5;                                           \
                            (a)->s6_addr[6] = a6;                                           \
                            (a)->s6_addr[7] = a7;                                           \
                            (a)->s6_addr[8] = a8;                                           \
                            (a)->s6_addr[9] = a9;                                           \
                            (a)->s6_addr[10] = a10;                                         \
                            (a)->s6_addr[11] = a11;                                         \
                            (a)->s6_addr[12] = a12;                                         \
                            (a)->s6_addr[13] = a13;                                         \
                            (a)->s6_addr[14] = a14;                                         \
                            (a)->s6_addr[15] = a15;                                         \
                        }

/*
* Definition of some useful macros to handle IP6 addresses (BSD-like)
*/
#define IN6ADDR_ANY_INIT                                    \
    {{{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}}
#define IN6ADDR_LOOPBACK_INIT                               \
    {{{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }}}
#define IN6ADDR_NODELOCAL_ALLNODES_INIT                     \
    {{{ 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }}}
#define IN6ADDR_INTFACELOCAL_ALLNODES_INIT                  \
    {{{ 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }}}
#define IN6ADDR_LINKLOCAL_ALLNODES_INIT                     \
    {{{ 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }}}
#define IN6ADDR_LINKLOCAL_ALLROUTERS_INIT                   \
    {{{ 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 }}}
#define IN6ADDR_LINKLOCAL_ALLV2ROUTERS_INIT                 \
    {{{ 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16 }}}

extern const struct in6_addr in6addr_any;
extern const struct in6_addr in6addr_loopback;
extern const struct in6_addr in6addr_nodelocal_allnodes;
extern const struct in6_addr in6addr_linklocal_allnodes;
extern const struct in6_addr in6addr_linklocal_allrouters;
extern const struct in6_addr in6addr_linklocal_allv2routers;

/*
* Equality (BSD-like)
*/
#define IN6_ARE_ADDR_EQUAL(a, b)			\
        (memcmp(&(a)->s6_addr[0], &(b)->s6_addr[0], sizeof(struct in6_addr)) == 0)

/*
* Copying address
*/
extern void IN6_ADDR_COPY(const in6_addr *from_addr, in6_addr *to_addr);


/*
 * Unspecified (BSD-like)
 */
#define IN6_IS_ADDR_UNSPECIFIED(a)	                                \
    ((*(const uint32_t *)(const void *)(&(a)->s6_addr[0]) == 0) &&	\
    (*(const uint32_t *)(const void *)(&(a)->s6_addr[4]) == 0) &&	\
    (*(const uint32_t *)(const void *)(&(a)->s6_addr[8]) == 0) &&	\
    (*(const uint32_t *)(const void *)(&(a)->s6_addr[12]) == 0))

/*
 * Loopback (BSD-like)
 */
#define IN6_IS_ADDR_LOOPBACK(a)	                                	\
    ((*(const uint32_t *)(const void *)(&(a)->s6_addr[0]) == 0) &&	\
    (*(const uint32_t *)(const void *)(&(a)->s6_addr[4]) == 0) &&	\
    (*(const uint32_t *)(const void *)(&(a)->s6_addr[8]) == 0) &&	\
    (*(const uint32_t *)(const void *)(&(a)->s6_addr[12]) == mqx_ntohl(1)))

/*
 * Multicast (BSD-like)
 */
#define IN6_IS_ADDR_MULTICAST(a)	((a)->s6_addr[0] == 0xff)

/*
 * Unicast Scope (BSD-like)
 * Note that we must check topmost 10 bits only, not 16 bits (see RFC2373).
 */
#define IN6_IS_ADDR_LINKLOCAL(a)	\
    	(((a)->s6_addr[0] == 0xfe) && (((a)->s6_addr[1] & 0xc0) == 0x80))
#define IN6_IS_ADDR_SITELOCAL(a)	\
    	(((a)->s6_addr[0] == 0xfe) && (((a)->s6_addr[1] & 0xc0) == 0xc0))

/* IPv4 address string size */
#define RTCS_IP4_ADDR_STR_SIZE sizeof("255.255.255.255")

/* IPv6 address string size */
#define RTCS_IP6_ADDR_STR_SIZE sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")

/*
** Type definitions for socket()
*/

typedef struct rtcs_socket_call_struct {
   uint32_t (_CODE_PTR_ SOCK_SOCKET)      (uint32_t);
   uint32_t (_CODE_PTR_ SOCK_BIND)        (uint32_t, const sockaddr*, uint16_t);
   uint32_t (_CODE_PTR_ SOCK_CONNECT)     (uint32_t, const sockaddr*, uint16_t);
   uint32_t (_CODE_PTR_ SOCK_LISTEN)      (uint32_t, uint32_t);
   uint32_t (_CODE_PTR_ SOCK_ACCEPT)      (uint32_t, sockaddr*, uint16_t*);
   uint32_t (_CODE_PTR_ SOCK_GETSOCKNAME) (uint32_t, sockaddr*, uint16_t*);
   uint32_t (_CODE_PTR_ SOCK_GETPEERNAME) (uint32_t, sockaddr*, uint16_t*);
    int32_t (_CODE_PTR_ SOCK_RECV)        (uint32_t, void*, uint32_t, uint32_t);
    int32_t (_CODE_PTR_ SOCK_RECVFROM)    (uint32_t, void*, uint32_t, uint32_t, sockaddr*, uint16_t*);
    int32_t (_CODE_PTR_ SOCK_RECVMSG)     (uint32_t, void*, uint32_t);
    int32_t (_CODE_PTR_ SOCK_SEND)        (uint32_t, void*, uint32_t, uint32_t);
    int32_t (_CODE_PTR_ SOCK_SENDTO)      (uint32_t, void*, uint32_t, uint32_t, sockaddr*, uint16_t);
    int32_t (_CODE_PTR_ SOCK_SENDMSG)     (uint32_t, void*, uint32_t);
   uint32_t (_CODE_PTR_ SOCK_SOCKATMARK)  (uint32_t);
   uint32_t (_CODE_PTR_ SOCK_SHUTDOWN)    (uint32_t, uint32_t);
} RTCS_SOCKET_CALL_STRUCT, * RTCS_SOCKET_CALL_STRUCT_PTR;

extern const RTCS_SOCKET_CALL_STRUCT  SOCK_DGRAM_CALL;
extern const RTCS_SOCKET_CALL_STRUCT  SOCK_STREAM_CALL;

/*
** Type definitions for [gs]etsockopt()
*/

typedef struct rtcs_sockopt_call_struct  {
   uint32_t (_CODE_PTR_ SOCK_GETSOCKOPT)  (uint32_t, uint32_t, uint32_t, void*, uint32_t*);
   uint32_t (_CODE_PTR_ SOCK_SETSOCKOPT)  (uint32_t, uint32_t, uint32_t, void*, uint32_t);
} RTCS_SOCKOPT_CALL_STRUCT, * RTCS_SOCKOPT_CALL_STRUCT_PTR;

extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_SOCKET_CALL;
extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_IP_CALL;
extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_UDP_CALL;
extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_TCP_CALL;
extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_IGMP_CALL;
extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_LINK_CALL;

#if RTCSCFG_ENABLE_IP6
extern const RTCS_SOCKOPT_CALL_STRUCT  SOL_IP6_CALL;
#endif

/*
** The socket state structure
*/

typedef struct rtcs_linkopt_struct {
   unsigned    OPT_8023 : 1;
   unsigned    OPT_PRIO : 1;
   unsigned             : 0;
   _ip_address DEST;
   unsigned char TTL;
   unsigned char TOS;
   unsigned char RESERVED[2];
#if RTCSCFG_ENABLE_IP6 /* IPv6 Options.*/
   unsigned char HOP_LIMIT_UNICAST;    /* Unicast hop limit.*/
   unsigned char HOP_LIMIT_MULTICAST;  /* Multicast hop limit.*/
#endif
#if RTCSCFG_LINKOPT_8021Q_PRIO
   uint32_t    PRIO;
#endif
} RTCS_LINKOPT_STRUCT, * RTCS_LINKOPT_STRUCT_PTR;

typedef struct socket_owner_struct {
   struct socket_owner_struct  *NEXT;
   _rtcs_taskid                 TASK[SOCKET_NUMOWNERS];
} SOCKET_OWNER_STRUCT, * SOCKET_OWNER_STRUCT_PTR;

/* Forward declaration. */
struct mc_member;
struct ucb_struct;
struct tcb_struct;

/* Socket control structure.*/
typedef struct socket_struct {

   struct   socket_struct      * NEXT;
   struct   socket_struct      * PREV;

   uint32_t                      VALID;

   uint16_t                      LOCAL_PORT;          /* not used */
   uint16_t                      REMOTE_PORT;         /* not used */
   _ip_address                   LOCAL_IPADDRESS;     /* not used */
   _ip_address                   REMOTE_IPADDRESS;    /* not used */

   /*TBD later we need make only one variable for IPv4 adn IPv6 */
#if RTCSCFG_ENABLE_IP6
   in6_addr                      LOCAL_IPV6ADDRESS;
   in6_addr                      REMOTE_IPV6ADDRESS;
#endif
   uint16_t                      STATE;
   uint16_t                      AF;
   RTCS_SOCKET_CALL_STRUCT_PTR   PROTOCOL;

   /*
   ** Socket options.
   **
   **  The order of these options must match the order of the option ids
   **  defined above, and CONNECT_TIMEOUT must be the first one.
   */
   uint32_t                      CONNECT_TIMEOUT;
   uint32_t                      RETRANSMISSION_TIMEOUT;
   uint32_t                      SEND_TIMEOUT;
   uint32_t                      RECEIVE_TIMEOUT;
   uint32_t                      RECEIVE_PUSH;
   uint32_t                      SEND_NOWAIT;
   uint32_t                      SEND_WAIT;
   uint32_t                      SEND_PUSH;
   uint32_t                      RECEIVE_NOWAIT;
   uint32_t                      CHECKSUM_BYPASS;
   uint32_t                      ACTIVE;
   int32_t                       TBSIZE;
   int32_t                       RBSIZE;
   int32_t                       MAXRTO;
   uint32_t                      MAXRCV_WND;
   int32_t                       KEEPALIVE;
   uint32_t                      NOWAIT;
   uint32_t                      NO_NAGLE_ALGORITHM;
   uint32_t                      NOSWRBUF;
   void _CODE_PTR_               CALL_BACK; /* obsolete */
   uint32_t                      TYPE_MIRROR;
   void (_CODE_PTR_              APPLICATION_CALLBACK)(int32_t);
   uint32_t                      TIMEWAIT_TIMEOUT;
   uint32_t                      TCPSECUREDRAFT_0;
   uint32_t                      DELAY_ACK;     /* last option */

   struct tcb_struct            *TCB_PTR;
   struct ucb_struct            *UCB_PTR;

   uint32_t                      ERROR_CODE;
#if RTCSCFG_SOCKET_OWNERSHIP
   SOCKET_OWNER_STRUCT           OWNERS;
#endif

   struct mc_member            **MCB_PTR;
   uint32_t (_CODE_PTR_ * IGMP_LEAVEALL)(struct mc_member**);

   struct {
      RTCS_LINKOPT_STRUCT        RX;
      RTCS_LINKOPT_STRUCT        TX;
   }                             LINK_OPTIONS;
} SOCKET_STRUCT, * SOCKET_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t  RTCS_socket
(
   uint32_t              ,  /* [IN] protocol family */
   uint32_t              ,  /* [IN] type of communication */
   uint32_t                 /* [IN] select a specific protocol */
);
extern uint32_t  RTCS_shutdown
(
   uint32_t              ,  /* [IN] socket handle */
   uint32_t                 /* [IN] shutdown method */
);

#if RTCSCFG_CHECK_ERRORS

   #define RTCS_CHECKSOCK(s,err) \
                       ((s) == 0) || ((uint32_t)(s) == RTCS_SOCKET_ERROR) \
                     ? (err) \
                     :

   #define RTCS_CHECKAPI(f,s,err) \
                       ((SOCKET_STRUCT_PTR)(s))->PROTOCOL->SOCK_ ## f == NULL \
                     ? (err) \
                     :

   #define RTCS_CHECKLEVEL(v,err) \
                       (v) == 0 \
                     ? (err) \
                     :

#else

   #define RTCS_CHECKSOCK(s,err)
   #define RTCS_CHECKAPI(f,s,err)
   #define RTCS_CHECKLEVEL(v,err)

#endif

#if RTCSCFG_CHECK_VALIDITY

   #define RTCS_CHECKVALID(s,err) \
                       ((SOCKET_STRUCT_PTR)(s))->VALID != SOCKET_VALID \
                     ? (err) \
                     :

#else

   #define RTCS_CHECKVALID(s,err)

#endif

#define RTCS_API(f,s,p)             RTCS_CHECKSOCK(s,RTCSERR_SOCK_INVALID) \
                                    RTCS_CHECKVALID(s,RTCSERR_SOCK_INVALID) \
                                    RTCS_CHECKAPI(f,s,RTCSERR_SOCK_NOT_SUPPORTED) \
                                    ((SOCKET_STRUCT_PTR)(s))->PROTOCOL->SOCK_ ## f p

#define RTCS_API_SOCK(f,s,p)        RTCS_CHECKSOCK(s,RTCS_SOCKET_ERROR) \
                                    RTCS_CHECKVALID(s,RTCS_SOCKET_ERROR) \
                                    RTCS_CHECKAPI(f,s,RTCS_SOCKET_ERROR) \
                                    ((SOCKET_STRUCT_PTR)(s))->PROTOCOL->SOCK_ ## f p

#define RTCS_API_SENDRECV(f,s,p)    RTCS_CHECKSOCK(s,RTCS_ERROR) \
                                    RTCS_CHECKVALID(s,RTCS_ERROR) \
                                    RTCS_CHECKAPI(f,s,RTCS_ERROR) \
                                    ((SOCKET_STRUCT_PTR)(s))->PROTOCOL->SOCK_ ## f p

#define RTCS_API_SOCKOPT(f,s,v,p)   RTCS_CHECKLEVEL(v,RTCSERR_SOCK_INVALID_OPTION) \
                                    ((RTCS_SOCKOPT_CALL_STRUCT_PTR)(v))->SOCK_ ## f p

#define bind(s,ap,al)            (RTCS_API(BIND,        s, (s,(const sockaddr *)(ap),al)))
#define connect(s,ap,al)         (RTCS_API(CONNECT,     s, (s,(sockaddr *)(ap),al)))
#define listen(s,f)              (RTCS_API(LISTEN,      s, (s,f)))
#define getsockname(s,ap,al)     (RTCS_API(GETSOCKNAME, s, (s,(sockaddr *)(ap),al)))
#define getpeername(s,ap,al)     (RTCS_API(GETPEERNAME, s, (s,(sockaddr *)(ap),al)))
#define accept(s,ap,al)          (RTCS_API_SOCK(ACCEPT, s, (s,(sockaddr *)(ap),al)))
#define send(s,p,l,f)            (RTCS_API_SENDRECV(SEND,      s, (s,p,l,f)))
#define sendto(s,p,l,f,ap,al)    (RTCS_API_SENDRECV(SENDTO,    s, (s,p,l,f,(sockaddr *)(ap),al)))
#define recv(s,p,l,f)            (RTCS_API_SENDRECV(RECV,      s, (s,p,l,f)))
#define recvfrom(s,p,l,f,ap,al)  (RTCS_API_SENDRECV(RECVFROM,  s, (s,p,l,f,(sockaddr *)(ap),al)))
#define getsockopt(s,v,on,op,ol) (RTCS_API_SOCKOPT(GETSOCKOPT, s, v, (s,v,on,op,ol)))
#define setsockopt(s,v,on,op,ol) (RTCS_API_SOCKOPT(SETSOCKOPT, s, v, (s,v,on,op,ol)))

#define socket          RTCS_socket
#define shutdown        RTCS_shutdown

/*
** RTCS (non-BSD) prototypes
*/


extern uint32_t  RTCS_attachsock
(
   uint32_t                       /* [IN] socket handle */
);
extern uint32_t  RTCS_detachsock
(
   uint32_t                       /* [IN] socket handle */
);
extern uint32_t  RTCS_transfersock
(
   uint32_t     in_sock,          /*[IN] specifies the handle of the existing socket */
   _task_id    new_owner
);
extern uint32_t  RTCS_selectall
(
   uint32_t                       /* [IN] time to wait for data, in milliseconds */
);

extern uint32_t RTCS_selectset(void *sockset, uint32_t size, uint32_t timeout);

extern uint32_t  _RTCS_select
(
   void*                    ,  /* [IN] set of read sockets to block on */
   uint32_t                 ,  /* [IN] size of read socket set */
   void*                    ,  /* [IN] set of write sockets to block on */
   uint32_t                 ,  /* [IN] size of write socket set */
   uint32_t                    /* [IN] time to wait for data, in milliseconds */
);

extern uint32_t _RTCS_selectset
(
   void *sockset,
   uint32_t size,
   uint32_t timeout
);

extern uint32_t  RTCS_geterror
(
   uint32_t                       /* [IN] socket handle */
);
 
#ifdef __cplusplus
}
#endif

#endif /* __rtcs_sock_h__ */


/* EOF */
