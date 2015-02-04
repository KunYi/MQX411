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
*   This file contains the Telnet server implementation.
*
*
*END************************************************************************/

#include <rtcs.h>

#if MQX_USE_IO_OLD

#include <fio.h>
#include <serial.h>
#include "telnet.h"

struct telnetsrv_init {
   RTCS_TASK_PTR  shell;
   uint32_t        sock;
};

typedef struct  telnetsrv_context {
   uint32_t     UDPSOCK;
   uint32_t     LISTENSOCK;
} TELNETSRV_CONTEXT, * TELNETSRV_CONTEXT_PTR;

_rtcs_taskid TELNETSRV_task_id = 0;

#ifdef __MQX__ 
void TELNETSRV_Exit_handler(void);
#endif

void TELNETSRV_task  (void *, void *);
void TELNETSRV_child (void *, void *);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : TELNETSRV_test_run
* Returned Value   : updated TELNETSRV_task_id
* Comments  :  Test if Telnet server is running and update TELNETSRV_task_id.
*
*END*-----------------------------------------------------------------*/

static _rtcs_taskid TELNETSRV_test_run
   (
       void
   )
{
   if (TELNETSRV_task_id != 0) {
     if (_task_get_td (RTCS_task_id_from_td (TELNETSRV_task_id)) == NULL) {
       TELNETSRV_task_id = 0;
     }
   }
   return TELNETSRV_task_id;
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : TELNETSRV_run
* Returned Value   : error code
* Comments  :  Become the Telnet server.
*
*END*-----------------------------------------------------------------*/

uint32_t TELNETSRV_run
   (
       RTCS_TASK_PTR  shell
   )
{ 
   struct telnetsrv_init init;
   if (TELNETSRV_test_run () != 0)  {
     return RTCSERR_SERVER_ALREADY_RUNNING;
   }
   init.shell = shell;
   init.sock  = RTCS_SOCKET_ERROR;
   TELNETSRV_task_id = RTCS_task_getid();
   TELNETSRV_task(&init,0);
   TELNETSRV_task_id = 0;
   return RTCSERR_SERVER_NOT_RUNNING;
} 


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : TELNETSRV_init
* Returned Value   : error code
* Comments  :  Start the Telnet server.
*
*END*-----------------------------------------------------------------*/

uint32_t TELNETSRV_init
   (
      char       *name,
      uint32_t        priority,
      uint32_t        stacksize,
      RTCS_TASK_PTR  shell
   )
{ /* Body */
   struct telnetsrv_init init;

   if (TELNETSRV_test_run () == 0)  {
      init.shell = shell;
      init.sock  = RTCS_SOCKET_ERROR;
      return RTCS_task_create(name, priority, stacksize, TELNETSRV_task, &init);
   } else  {
      return RTCSERR_SERVER_ALREADY_RUNNING;
   }
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : TELNETSRV_sockinit
* Returned Value   : error code
* Comments  :  Start the Telnet server with an existing socket.
*
*END*-----------------------------------------------------------------*/

uint32_t TELNETSRV_sockinit
   (
      char       *name,
      uint32_t        priority,
      uint32_t        stacksize,
      RTCS_TASK_PTR  shell,
      uint32_t        sock
   )
{ /* Body */
   struct telnetsrv_init init;

   if (TELNETSRV_test_run () == 0)  {
      init.shell = shell;
      init.sock  = sock;
      return RTCS_task_create(name, priority, stacksize, TELNETSRV_task, &init);
   } else  {
      return RTCSERR_SERVER_ALREADY_RUNNING;
   }

} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : TELNET_server
* Returned Value   : none
* Comments  :  A TCP/IP based Telnet Server
*
*END*-----------------------------------------------------------------*/

void TELNET_server
   (
      uint32_t template_index
   )
{ /* Body */
   struct telnetsrv_init      init;
   RTCS_TASK                  shell;
   TASK_TEMPLATE_STRUCT_PTR   template_ptr;

   template_ptr = _mqx_get_initialization()->TASK_TEMPLATE_LIST;
   while (template_ptr->TASK_TEMPLATE_INDEX &&
          (template_ptr->TASK_TEMPLATE_INDEX != template_index)) {
      template_ptr++;
   } /* Endwhile */

   shell.NAME      = template_ptr->TASK_NAME;
   shell.PRIORITY  = template_ptr->TASK_PRIORITY;
   shell.STACKSIZE = template_ptr->TASK_STACKSIZE;
   shell.START     = (void(_CODE_PTR_)(void *))template_ptr->TASK_ADDRESS;
   shell.ARG       = (void *)template_ptr->CREATION_PARAMETER;

   init.shell = &shell;
   init.sock  = RTCS_SOCKET_ERROR;

   TELNETSRV_task(&init, NULL);

} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Function Name    : TELNETSRV_task
* Returned Value   : none
* Comments  :  A TCP/IP based Telnet Server
*
* If sock<0, *sockp is set to a newly created TCP socket, and it
* is connected to raddr.
*
*END*-----------------------------------------------------------------*/

void TELNETSRV_task
   (
      void    *init_ptr,
      void    *creator
   )
{ /* Body */
   struct telnetsrv_init init;
   RTCS_TASK             shell = *((struct telnetsrv_init *)init_ptr)->shell;
   uint32_t               sock = ((struct telnetsrv_init *)init_ptr)->sock;
   sockaddr_in           addr;
   uint32_t               error,option;
   TELNETSRV_CONTEXT     telnetsrv_context =  { 0 };

   TELNETSRV_task_id = RTCS_task_getid();
#ifdef __MQX__ 
   /* Set up exit handler and context so that we can clean up if the Echo Server is terminated */
   _task_set_environment( _task_get_id(), (void *) &telnetsrv_context );
   _task_set_exit_handler( _task_get_id(), TELNETSRV_Exit_handler );
#endif 

   /* Install device drivers for socket and telnet I/O */
   _io_socket_install("socket:");
   _io_telnet_install("telnet:");

   /* Allocate a socket if the caller didn't provide one */
   if (sock == RTCS_SOCKET_ERROR) {
      sock = socket(PF_INET, SOCK_STREAM, 0);
      if (sock == RTCS_SOCKET_ERROR) {
         RTCS_task_exit(creator, RTCSERR_OUT_OF_SOCKETS);
      }
   }
  
   telnetsrv_context.LISTENSOCK = sock;
  
   option = TELNETDCFG_BUFFER_SIZE;   
   error = setsockopt(telnetsrv_context.LISTENSOCK, SOL_TCP, OPT_TBSIZE, &option, sizeof(option));
   if (error) {
      RTCS_task_exit(creator, error);
   }      

   option = TELNETDCFG_BUFFER_SIZE;   
   error = setsockopt(telnetsrv_context.LISTENSOCK, SOL_TCP, OPT_RBSIZE, &option, sizeof(option));
   if (error) {
      RTCS_task_exit(creator, error);
   }   

   option = TELNETDCFG_NOWAIT;   
   error = setsockopt(telnetsrv_context.LISTENSOCK, SOL_TCP, OPT_NOWAIT, &option, sizeof(option));
   if (error) {
      RTCS_task_exit(creator, error);
   }      

   option = TELENETDCFG_SEND_TIMEOUT;   
   error = setsockopt(telnetsrv_context.LISTENSOCK, SOL_TCP, OPT_SEND_TIMEOUT, &option, sizeof(option));
   if (error) {
      RTCS_task_exit(creator, error);
   }      

   option = TELENETDCFG_CONNECT_TIMEOUT;   
   error = setsockopt(telnetsrv_context.LISTENSOCK, SOL_TCP, OPT_CONNECT_TIMEOUT, &option, sizeof(option));
   if (error) {
      RTCS_task_exit(creator, error);
   }      

   option = TELENETDCFG_TIMEWAIT_TIMEOUT;   
   error = setsockopt(telnetsrv_context.LISTENSOCK, SOL_TCP, OPT_TIMEWAIT_TIMEOUT, &option, sizeof(option));
   if (error) {
      RTCS_task_exit(creator, error);
   }      

   /* Bind the socket to the Telnet port */
   addr.sin_family      = AF_INET;
   addr.sin_port        = IPPORT_TELNET;
   addr.sin_addr.s_addr = INADDR_ANY;

   error = bind(sock,(const sockaddr *)&addr, sizeof(addr));
   if (error != RTCS_OK) {
      RTCS_task_exit(creator, error);
   } /* Endif */

   /* Put the socket into listening mode */
   error = listen(sock, 0);
   if (error != RTCS_OK) {
      RTCS_task_exit(creator, error);
   } /* Endif */

   RTCS_task_resume_creator(creator, RTCS_OK);

   init.shell = &shell;
   while (TRUE) {

      /* Wait for a connection */
      init.sock = accept(sock, NULL, NULL);
      if (init.sock == RTCS_SOCKET_ERROR) {
         continue;
      } /*Endif*/
      
#if TELNETDCFG_ENABLE_MULTIPLE_CLIENTS

      /* Create a task to look after it */
      RTCS_detachsock(init.sock);
      if (RTCS_task_create(shell.NAME, shell.PRIORITY, shell.STACKSIZE, TELNETSRV_child, &init)!=RTCS_OK) {
         RTCS_attachsock(init.sock);
         shutdown(init.sock, FLAG_ABORT_CONNECTION);
      }
      #else
         TELNETSRV_child((void *) &init,0);
      #endif

   } /* Endwhile */

} /* Endbody */


/*TASK*-----------------------------------------------------------------
*
* Task Name      : TELNETSRV_child
* Returned Value : void
* Comments       : Telnet child task.
*
*END*-----------------------------------------------------------------*/

void TELNETSRV_child
   (
      void    *init_ptr,
      void    *creator
   )
{ /* Body */
   struct telnetsrv_init      *init = init_ptr;
   uint32_t                     sock = init->sock;
   _mqx_uint                   param;
   void (_CODE_PTR_            start)(void *) = init->shell->START;
   void                       *arg = init->shell->ARG;

   MQX_FILE_PTR sockfd, telnetfd;

   sockfd = fopen("socket:", (char *)sock);
   if (sockfd == NULL) {
      RTCS_task_exit(creator, RTCSERR_FOPEN_FAILED);
   } /* Endif */

#ifndef TELNETDCFG_SOCKIO_BUFFER_SIZE
#define TELNETDCFG_SOCKIO_BUFFER_SIZE  100 // 1460
#endif 
   param =  TELNETDCFG_SOCKIO_BUFFER_SIZE;
   ioctl(sockfd, IO_IOCTL_SET_BLOCK_MODE, &param);

   telnetfd = fopen("telnet:", (char *)sockfd);
   if (telnetfd == NULL) {
      RTCS_task_exit(creator, RTCSERR_FOPEN_FAILED);
   } /* Endif */


   param = IO_SERIAL_ECHO;
   ioctl(telnetfd, IO_IOCTL_SERIAL_SET_FLAGS, &param);

   _io_set_handle(IO_STDIN, telnetfd);
   _io_set_handle(IO_STDOUT, telnetfd);

   RTCS_task_resume_creator(creator, RTCS_OK);

   /*
   ** Send a greeting message to the user
   */
   printf("\nRTCS v%d.%02d.%02d Telnet server\n",
          RTCS_VERSION_MAJOR, RTCS_VERSION_MINOR, RTCS_VERSION_REV);

   /*
   ** Call user application function
   */
   start(arg);

   printf("\nGoodbye\n");

   /*
   ** Allow some time for queued data to go out.
   */
   RTCS_time_delay(100);

   /*
   ** Child task is done
   */
   fclose(telnetfd);
   fclose(sockfd);
   shutdown(sock, FLAG_CLOSE_TX);
} /* Endbody */

#ifdef __MQX__ 
/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : TELNETSRV_stop
* Returned Value   : error code
* Comments  :  Start the TELNETSRV task.
*
*END*-----------------------------------------------------------------*/

uint32_t TELNETSRV_stop( void )
{ /* Body */
   if (TELNETSRV_test_run () == 0)  {
      return RTCSERR_SERVER_NOT_RUNNING;
   }
   RTCS_task_abort(TELNETSRV_task_id);
   TELNETSRV_task_id = 0;
   return RTCS_OK;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : TELNETSRV_Exit_handler
* Returned Value   : error code
* Comments  :  Start the TELNETSRV task.
*
*END*-----------------------------------------------------------------*/

void TELNETSRV_Exit_handler( void )
{ /* Body */
   TELNETSRV_CONTEXT_PTR    telnetsrv_context_ptr;
   
   telnetsrv_context_ptr = (TELNETSRV_CONTEXT_PTR) _task_get_environment( _task_get_id() );
   if (telnetsrv_context_ptr != NULL)  {
      if (telnetsrv_context_ptr->UDPSOCK)  {
         shutdown(telnetsrv_context_ptr->UDPSOCK, 0);
         telnetsrv_context_ptr->UDPSOCK = 0;   
      }   
      if (telnetsrv_context_ptr->LISTENSOCK)  {
         shutdown(telnetsrv_context_ptr->LISTENSOCK, FLAG_ABORT_CONNECTION);   
         telnetsrv_context_ptr->LISTENSOCK = 0;   
      }   
   }
   TELNETSRV_task_id = 0;
} /* Endbody */
#endif

#endif //MQX_USE_IO_OLD
