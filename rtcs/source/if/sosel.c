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
*   This file contains the RTCS_select() implementation.
*
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "socket.h"
#include "tcpip.h"
#include "tcp_prv.h"    /* for TCP internal definitions */
#include "udp_prv.h"    /* for UDP internal definitions */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_activity
* Returned Value  : TRUE or FALSE
* Comments        : Determine whether there is any activity on a socket.
*
*END*-----------------------------------------------------------------*/

bool SOCK_select_activity
   (
      SOCKET_STRUCT_PTR    socket_ptr,
      int flag                          // direction flag - 0 recv, non zero send
   )
{
   TCB_STRUCT_PTR tcb_ptr;
   bool        activity = FALSE;

   /* Check closing and connected stream sockets for data */
   if (socket_ptr->TCB_PTR)
   {
      tcb_ptr = socket_ptr->TCB_PTR;
   
      if (!flag)
      {
         /*
         ** Check that there is data in the receive ring or
         ** that the socket has been closed by the peer
         */
         if ((tcb_ptr->conn_pending) || (tcb_ptr->state == CLOSED) || (GT32(tcb_ptr->rcvnxt, tcb_ptr->rcvbufseq)))
         {
            activity = TRUE;
         }
      }
      else
      {
         if (!tcb_ptr->sndlen) // TODO - remake for partialy empty send buffers
         {    
            activity = TRUE;
         }
      }

   /* Check datagram sockets for data */
   } 
   else if (socket_ptr->UCB_PTR)
   {
      /*
      ** Check that there is queued data
      */
      if (socket_ptr->UCB_PTR->PHEAD)
      {
         activity = TRUE;
      } /* Endif */

   /* TCB=UCB=NULL is a TCP connection reset by peer */
   }
   else
   {
      activity = TRUE;
   }

   return activity;

}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_block
* Returned Values :
* Comments  :
*     Enqueues a select() call on the select queue.
*
*END*-----------------------------------------------------------------*/

void SOCK_select_block
   (
      SOCKSELECT_PARM_PTR  parms
   )
{ /* Body */
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr = RTCS_getcfg(SOCKET);

   parms->NEXT = socket_cfg_ptr->SELECT_HEAD;
   if (parms->NEXT) {
      parms->NEXT->PREV = &parms->NEXT;
   } /* Endif */
   socket_cfg_ptr->SELECT_HEAD = parms;
   parms->PREV = &socket_cfg_ptr->SELECT_HEAD;

   if (parms->timeout) {
      parms->EXPIRE.TIME    = parms->timeout;
      parms->EXPIRE.EVENT   = SOCK_select_expire;
      parms->EXPIRE.PRIVATE = parms;
      TCPIP_Event_add(&parms->EXPIRE);
   } /* Endif */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_expire
* Returned Values :
* Comments  :
*     Called by the Timer.  Expire a select call.
*
*END*-----------------------------------------------------------------*/

bool SOCK_select_expire
   (
      TCPIP_EVENT_PTR   event
   )
{ /* Body */
   SOCKSELECT_PARM_PTR  parms = event->PRIVATE;

   if (parms->NEXT) {
      parms->NEXT->PREV = parms->PREV;
   } /* Endif */
   *parms->PREV = parms->NEXT;

   parms->sock = 0;
   RTCSCMD_complete(parms, RTCS_OK);

   return FALSE;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_unblock
* Returned Values :
* Comments  :
*     Dequeues a select() call from the select queue.
*
*END*-----------------------------------------------------------------*/

void SOCK_select_unblock
   (
      SOCKSELECT_PARM_PTR  parms,
      uint16_t  state      // TCP state or UDP socket flag
   )
{

   if (parms->NEXT) {
      parms->NEXT->PREV = parms->PREV;
   } 

   *parms->PREV = parms->NEXT;

   if (parms->timeout)
   {
      TCPIP_Event_cancel(&parms->EXPIRE);
   } 

   /* Check TCP state and UDP socket flag */
   if ( (state == CLOSED) || (state == UDP_SOCKET_CLOSE) )
   {
      RTCSCMD_complete(parms, RTCSERR_SOCK_CLOSED);
   } 
   else
   {
      RTCSCMD_complete(parms, RTCS_OK);
   }
    

}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select_signal
* Returned Values :
* Comments  :
*     Unblocks all select() calls blocked on a specific socket.
*
*END*-----------------------------------------------------------------*/

void _SOCK_select_signal
   (
      uint32_t  sock,
      uint16_t  state,      // TCP state or UDP socket flag
      uint32_t  flag        // direction flag - 0 recv, non zero send
   )
{ /* Body */
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr = RTCS_getcfg(SOCKET);
   SOCKSELECT_PARM_PTR        parms;
   SOCKSELECT_PARM_PTR        nextparms;
   uint32_t                    i;

   if (!sock)
   {
      return;
   } /* Endif */

   for (parms = socket_cfg_ptr->SELECT_HEAD; parms; parms = nextparms)
   {
      nextparms = parms->NEXT;
      #if RTCSCFG_SOCKET_OWNERSHIP
      if (parms->owner)
      {
         if (SOCK_Is_owner((SOCKET_STRUCT_PTR)sock, parms->owner))
         {
            parms->sock = sock;
            SOCK_select_unblock(parms, state);
         } /* Endif */
      }
      else 
      #endif
      {
         /* check if look on selectset or selectall */
         /* parms->sock_count equals 0xFFFFFFFF only when called */
         /* from RTCS_selectall() */
         /* for this case we just unblock upper layer */
         /*  */
         /* from RTCS_selectset, parms->sock_count has number of socket */
         /* handles in the parms->sock_ptr[] array */
         /* check if sock is on the select waiting list (array) */
        
         if(0xFFFFFFFF!=parms->sock_count)
         {
            for (i = 0; i < parms->sock_count; i++)
            {
                if (parms->sock_ptr[i] == sock)
                {
                    break;
                }  
            }
            if(i >= parms->sock_count)
            {
                continue;
            }
         }
         /* now, it is sure the select will be awaken. 
            Just zero out the others in both arrays */
         parms->sock = sock;
         
         SOCK_select_unblock(parms, state);
      } /* Endif */

   } /* Endfor */

   /*
   ** Finally, call the application callback, if one exists.
   */
   if (((SOCKET_STRUCT_PTR)sock)->APPLICATION_CALLBACK)
   {
      ((SOCKET_STRUCT_PTR)sock)->APPLICATION_CALLBACK(sock);
   } /* Endif */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : _RTCS_selectset
* Returned Value  : socket handle
* Comments  :  Wait for data or connection requests on any socket
*      in a specified set.
*
*END*-----------------------------------------------------------------*/
uint32_t RTCS_selectset(void *sockset, uint32_t size, uint32_t timeout)
{
   SOCKSELECT_PARM   parms;
   uint32_t           error;

   parms.sock_ptr   = sockset;
   parms.sock_count = size;
   parms.timeout    = timeout;

   error = RTCSCMD_issue(parms, SOCK_selectset);
   if (error)
   {
      return RTCS_SOCKET_ERROR;
   }
   return parms.sock;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_select
* Parameters      :
*
*     _rtcs_taskid      owner            not used
*     uint32_t          *sock_ptr        [IN] array of socket handles
*     uint32_t          sock_count       [IN] size of socket array
*     uint32_t          timeout          [IN] timeout, 0=forever, -1=none
*     uint32_t          sock             [OUT] socket with activity
*
* Comments  :  Wait for data or connection requests on any socket
*      in array.
*
*END*-----------------------------------------------------------------*/
void SOCK_selectset(SOCKSELECT_PARM_PTR  parms)
{ 
   uint32_t                   size = parms->sock_count;
   SOCKET_STRUCT_PTR          socket_ptr = NULL;
   int                        i = 0;
   
   for (i = 0; i < size; i++) 
   {
      if ((parms->sock_ptr+i) && ((uint32_t)(parms->sock_ptr+i) != RTCS_SOCKET_ERROR) && (*(parms->sock_ptr+i) != 0))
      {
         if (SOCK_select_activity((SOCKET_STRUCT_PTR) *(parms->sock_ptr+i), 0))
         {  
            socket_ptr = (SOCKET_STRUCT_PTR) *(parms->sock_ptr+i);
            break;
         }
      }
   }

   if (socket_ptr)
   {
      parms->sock = (uint32_t) socket_ptr;
      RTCSCMD_complete(parms, RTCS_OK);
      return;
   }

   if (parms->timeout == (uint32_t)-1) 
   {
      parms->sock = 0;
      RTCSCMD_complete(parms, RTCS_OK);
      return;
   }

   SOCK_select_block(parms);
} 

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_selectall
* Returned Value  : socket handle
* Comments  :  Wait for data or connection requests on any socket
*      owned by this task.
*
*END*-----------------------------------------------------------------*/

uint32_t RTCS_selectall
   (
      uint32_t     timeout
         /* [IN] specifies the maximum amount of time to wait for data */
   )
{ /* Body */
   SOCKSELECT_PARM   parms;
   uint32_t           error;

   parms.owner   = RTCS_task_getid();
   parms.timeout = timeout;
   /* unblock by activity on any socket */
   parms.sock_count = 0xFFFFFFFF;
   error = RTCSCMD_issue(parms, SOCK_selectall);
   if (error) return RTCS_SOCKET_ERROR;

   return parms.sock;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOCK_selectall
* Parameters      :
*
*     _rtcs_taskid      owner       [IN] socket owner
*     uint32_t      *sock_ptr    not used
*     uint32_t           sock_count  not used
*     uint32_t           timeout     [IN] timeout, 0=forever, -1=none
*     uint32_t           sock        [OUT] socket with activity
*
* Comments  :  Wait for data or connection requests on any socket
*      owned by this task.
*
*END*-----------------------------------------------------------------*/

void SOCK_selectall
   (
      SOCKSELECT_PARM_PTR  parms
   )
{
   SOCKET_CONFIG_STRUCT_PTR   socket_cfg_ptr = RTCS_getcfg(SOCKET);
   SOCKET_STRUCT_PTR          socket_ptr;
   
   /* cycle through sockets looking for one owned by this task */
   for (socket_ptr = socket_cfg_ptr->SOCKET_HEAD;
        socket_ptr;
        socket_ptr = socket_ptr->NEXT) 
   {
      if (SOCK_Is_owner(socket_ptr, parms->owner)
         && SOCK_select_activity(socket_ptr, 0)) 
      {   
         break;
      } /* Endif */
   } /* Endfor */

   if (socket_ptr) 
   {
      parms->sock = (uint32_t)socket_ptr;
      RTCSCMD_complete(parms, RTCS_OK);
      return;
   } /* Endif */

   if (parms->timeout == (uint32_t)-1) 
   {
      parms->sock = 0;
      RTCSCMD_complete(parms, RTCS_OK);
      return;
   } /* Endif */

   SOCK_select_block(parms);
}

/* EOF */
