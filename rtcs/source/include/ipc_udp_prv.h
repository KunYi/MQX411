#ifndef __ipc_udp_prv_h__
#define __ipc_udp_prv_h__ 1
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
*   This file contains the private definitions for the 
*   inter-processor drivers that work over Ethernet/UDP.
*
*
*END************************************************************************/


/*--------------------------------------------------------------------------*/
/*
**                          CONSTANT DECLARATIONS
*/

#define IPC_UDP_POOL_INIT   16
#define IPC_UDP_POOL_GROW   16 
#define IPC_UDP_POOL_MAX    0

/*--------------------------------------------------------------------------*/
/*
**                          DATATYPE DECLARATIONS
*/


/*
** IPC_UDP_CONTEXT
** This structure contains protocol information for the IPC over Ethernet/UDP
**
*/
typedef struct ipc_udp_context 
{
   /* Pointer to the standard inter-processor initialization structure */
   const IPC_PROTOCOL_INIT_STRUCT *    IPC_INIT_PTR;

   /* Pointer to the structure that contains initialization information 
      for Ethernet/UDP based communication */
   IPC_UDP_INIT_PTR                INIT_PTR;

   /* Message pool ID */
   _pool_id                        POOL_ID;

   /* UDP socket */
   uint32_t                         SOCKET;

   /* TaskID of the task that handles outcomming messages */
   _task_id                        OUT_TID;

   /* TaskID of the task that handles incomming messages */
   _task_id                        IN_TID;

   /* Socket Address Structure */
   sockaddr_in                     RADDR;
}  IPC_UDP_CONTEXT, * IPC_UDP_CONTEXT_PTR;

/*--------------------------------------------------------------------------*/
/*
**                          C PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__
extern bool IPC_UDP_Init_internal(IPC_UDP_CONTEXT_PTR context_ptr );
extern void IPC_UDP_In( uint32_t param );
extern void IPC_UDP_Out( uint32_t param );
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
