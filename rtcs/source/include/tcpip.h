#ifndef __tcpip_h__
#define __tcpip_h__
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
*   Definitions for the TCP/IP layer error codes.
*
*
*END************************************************************************/

#include "rtcstime.h"
#include "ipradix.h"
#include "checksum.h"
#include "ip.h"
#include "icmp.h"
#include "igmp.h"
#include "udp.h"
#include "tcp.h"
#include "icmp6.h"

/***************************************
**
** Type definitions
**
*/

typedef struct tcpip_message {
   MESSAGE_HEADER_STRUCT   HEAD;
   void (_CODE_PTR_        COMMAND)(void *);
   void                   *DATA;
} TCPIP_MESSAGE, * TCPIP_MESSAGE_PTR;

/*  TCP Configuration
**    This information is persistent for the TCP layer.
*/
typedef struct tcpip_cfg_struct {

   uint32_t     status;           /* task status - RTCS_OK => running */

} TCPIP_CFG_STRUCT, * TCPIP_CFG_STRUCT_PTR;


/***************************************
**
** Function-specific type definitions
**
*/


/* IPIF_*() */
typedef struct ipif_parm {
   TCPIP_PARM           COMMON;
   void                *mhandle;
   _rtcs_if_handle      ihandle;
   RTCS_IF_STRUCT_PTR   if_ptr;
   _ip_address          address;
   _ip_address          locmask;
   _ip_address          network;
   _ip_address          netmask;
   bool              probe;
} IPIF_PARM, * IPIF_PARM_PTR;


/* SOCK_selectall() and SOCK_select() */
typedef struct sockselect_parm {
   TCPIP_PARM                          COMMON;
   struct sockselect_parm             *NEXT;
   struct sockselect_parm            **PREV;
   TCPIP_EVENT                         EXPIRE;
   _rtcs_taskid                        owner;
   uint32_t                            *sock_ptr;
   uint32_t                             sock_count;
   uint32_t                             timeout;
   uint32_t                             sock;
} SOCKSELECT_PARM, * SOCKSELECT_PARM_PTR;


/***************************************
**
** Globals
**
*/

/* The TCP tick server */
extern uint32_t (_CODE_PTR_ TCP_tick)(void);


/***************************************
**
** Prototypes
**
*/

void TCPIP_task (void *, void *);

/* Select functions */
extern void    SOCK_selectall       (SOCKSELECT_PARM_PTR);
extern void    SOCK_select          (SOCKSELECT_PARM_PTR);
extern void    SOCK_selectset       (SOCKSELECT_PARM_PTR  parms);
extern bool SOCK_select_activity (SOCKET_STRUCT_PTR, int);
extern void    SOCK_select_block    (SOCKSELECT_PARM_PTR);
extern bool SOCK_select_expire   (TCPIP_EVENT_PTR);
extern void    SOCK_select_unblock  (SOCKSELECT_PARM_PTR, uint16_t);

#define SOCK_select_signal(a, b)    _SOCK_select_signal(a, b, 0)

extern void    _SOCK_select_signal   (uint32_t, uint16_t, uint32_t);

/* BOOTP and DHCP common functions */

uint32_t BOOT_init
(
   void
);

uint32_t BOOT_open
(
   void (_CODE_PTR_)(RTCSPCB_PTR, UCB_STRUCT_PTR)
);

uint32_t BOOT_close
(
   void
);

uint32_t BOOT_send
(
   RTCSPCB_PTR       ,        /* [IN]     outgoing packet  */
   void                      * /* [IN]     target interface */
);

void BOOT_service
(
   RTCSPCB_PTR       ,        /* [IN/OUT] incoming packet  */
   UCB_STRUCT_PTR             /* [IN]     target UCB       */
);


#endif
/* EOF */
