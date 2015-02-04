#ifndef __ipc_udp_h__
#define __ipc_udp_h__ 1
/*HEADER**********************************************************************
*
* Copyright 2010 Freescale Semiconductor, Inc.
* Copyright 2004-2010 Embedded Access Inc.
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
*   This file contains the definitions for the 
*   interprocessor drivers that work over Ethernet/UDP.
*
*
*END************************************************************************/

#include <rtcs.h>

/*--------------------------------------------------------------------------*/
/*
**                          DATATYPE DECLARATIONS
*/

/*
** IPC_UDP_INIT
** This structure contains the initialization information for Ethernet/UDP
** based communication
**
*/
typedef struct ipc_udp_init
{
   /* 
   ** The following are parameters used to create two tasks of the
   ** IPC/UDP driver for handling incomming and outcomming messages
   */
   char    *NAME;
   uint32_t     STACK_SIZE;
   uint32_t     PRIORITY;

   /* Maximum size of the message queue created by the IPC/UDP driver */
   uint32_t     MAX_MSG_SIZE;

   /* UDP port number for receiving */
   uint32_t     SOURCE_PORT;

   /* UDP port number for sending */
   uint32_t     DEST_PORT;

   /* IP address of the device the IPC/UDP driver sends messages to */
   _ip_address DEST;
} IPC_UDP_INIT, * IPC_UDP_INIT_PTR;

/*--------------------------------------------------------------------------*/
/*
**                          C PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern _mqx_uint IPC_UDP_Init( const IPC_PROTOCOL_INIT_STRUCT * ipc_init_ptr, void *ipc_info_ptr);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
