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
*   This file contains an implementation of an
*   RFC 862 Echo server.
*
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_UDP

#define BUFFER_SIZE  1500
static char buffer[BUFFER_SIZE];

/* Start CR 1658C */
typedef struct echo_context  {
   uint32_t     UDPSOCK;
   uint32_t     LISTENSOCK;
} ECHOSRV_CONTEXT, * ECHOSRV_CONTEXT_PTR;

_rtcs_taskid ECHOSRV_task_id = 0;

#ifdef __MQX__ 
void ECHOSRV_Exit_handler(void);
#endif
/* End CR 1658C */

void ECHOSRV_task(void *, void *);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : ECHOSRV_init
* Returned Value   : error code
* Comments  :  Start the port 7 echo server.
*
*END*-----------------------------------------------------------------*/

uint32_t ECHOSRV_init
   (
      char *name,
      uint32_t  priority,
      uint32_t  stacksize
   )
{ /* Body */
   /* Start CR 1658C */
   if (ECHOSRV_task_id == 0)  {
      return RTCS_task_create(name, priority, stacksize, ECHOSRV_task, NULL);
   } else  {
      return RTCSERR_SERVER_ALREADY_RUNNING;
   }
   /* End CR 1658C */
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : ECHO_task
* Returned Value   : none
* Comments  :  Simple Echo server.
*
*END*-----------------------------------------------------------------*/

void ECHO_task
   (
      uint32_t dummy
   )
{ /* Body */
   ECHOSRV_task(NULL, NULL);
} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : ECHOSRV_task
* Returned Value   : none
* Comments  :  Simple Echo server.
*
*END*-----------------------------------------------------------------*/

void ECHOSRV_task
   (
      void   *dummy,
      void   *creator
   )
{ /* Body */
   sockaddr_in    laddr, raddr;
   uint32_t        sock, listensock;
   uint32_t        error;
   uint32_t        optval, optlen;
   int32_t         length;
   uint16_t        rlen;
   /* Start CR 1658C */
   ECHOSRV_CONTEXT   echo_context =  { 0 };

   ECHOSRV_task_id = RTCS_task_getid();
#ifdef __MQX__ 
   /* Set up exit handler and context so that we can clean up if the Echo Server is terminated */
   _task_set_environment( _task_get_id(), (void *) &echo_context );
   _task_set_exit_handler( _task_get_id(), ECHOSRV_Exit_handler );
#endif 
   
   /* Echo server services port 7 */
   laddr.sin_family      = AF_INET;
   laddr.sin_port        = IPPORT_ECHO;
   laddr.sin_addr.s_addr = INADDR_ANY;

   /* Bind to UDP port */
   echo_context.UDPSOCK = socket(PF_INET, SOCK_DGRAM, 0);
   if (echo_context.UDPSOCK == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(echo_context.UDPSOCK, (const sockaddr *)&laddr, sizeof(laddr));
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */

   /* Listen on TCP port */
   echo_context.LISTENSOCK = socket(PF_INET, SOCK_STREAM, 0);
   listensock = echo_context.LISTENSOCK;
   if (listensock == RTCS_SOCKET_ERROR) {
      RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
   } /* Endif */
   error = bind(listensock, (const sockaddr *)&laddr, sizeof(laddr));
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */
   error = listen(listensock, 0);
   if (error) {
      RTCS_task_exit(creator, error);
   } /* Endif */
   /* End CR 1658C */

   RTCS_task_resume_creator(creator, RTCS_OK);

   for (;;) {

      sock = RTCS_selectall(0);

      if (sock == listensock) {

         /* Connection requested; accept it */
         rlen = sizeof(raddr);
         accept(listensock,(sockaddr *)&raddr, &rlen);

      } else {

         /* Get the socket type */
         getsockopt(sock, SOL_SOCKET, RTCS_SO_TYPE, (char *)&optval, &optlen);

         if (optval == SOCK_STREAM) {

            length = recv(sock, buffer, BUFFER_SIZE, 0);
            if (length == RTCS_ERROR) {
               shutdown(sock, FLAG_CLOSE_TX);
            } else {
               send(sock, buffer, length, 0);
            } /* Endif */

         } else { /* optval == SOCK_DGRAM */

            rlen = sizeof(raddr);
            length = recvfrom(sock, buffer, BUFFER_SIZE, 0, (sockaddr *)&raddr, &rlen);
            sendto(sock, buffer, length, 0, (sockaddr *)&raddr, rlen);

         } /* Endif */

      } /* Endif */

   } /* Endfor */

} /* Endbody */

/* Start CR 1658C */
#ifdef __MQX__ 
/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : ECHOSRV_stop
* Returned Value   : error code
* Comments  :  Start the ECHO task.
*
*END*-----------------------------------------------------------------*/

uint32_t ECHOSRV_stop( void )
{ /* Body */
   if (ECHOSRV_task_id == 0)  {
      return RTCSERR_SERVER_NOT_RUNNING;
   }
   RTCS_task_abort(ECHOSRV_task_id);
   ECHOSRV_task_id = 0;
   return RTCS_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : ECHOSRV_Exit_handler
* Returned Value   : error code
* Comments  :  Start the ECHO task.
*
*END*-----------------------------------------------------------------*/

void ECHOSRV_Exit_handler( void )
{ /* Body */
   ECHOSRV_CONTEXT_PTR    echo_context_ptr;
   
   echo_context_ptr = (ECHOSRV_CONTEXT_PTR) _task_get_environment( _task_get_id() );
   if (echo_context_ptr != NULL)  {
      if (echo_context_ptr->UDPSOCK)  {
         shutdown(echo_context_ptr->UDPSOCK, 0);
         echo_context_ptr->UDPSOCK = 0;   
      }   
      if (echo_context_ptr->LISTENSOCK)  {
         shutdown(echo_context_ptr->LISTENSOCK, FLAG_ABORT_CONNECTION);   
         echo_context_ptr->LISTENSOCK = 0;   
      }   
   }
   ECHOSRV_task_id = 0;
} /* Endbody */
#endif
/* End CR 1658 */

#endif
/* EOF */
