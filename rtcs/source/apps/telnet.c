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
*   This file contains the Telnet client implementation.
*
*
*END************************************************************************/

#include <rtcs.h>

#if MQX_USE_IO_OLD

#include <fio.h>
#include "telnet.h"

/*TASK*-----------------------------------------------------------------
*
* Function Name  : TELNET_connect
* Returned Value : error code
* Comments       : A TCP/IP based Telnet Client
*
*END------------------------------------------------------------------*/

uint32_t TELNET_connect
   (
      _ip_address    ipaddress
   )
{ /* Body */
   MQX_FILE_PTR   sockfd, telnetfd;
   sockaddr_in    addr;
   uint32_t        sock;
   uint32_t        error;
   bool        work;
   int32_t         c;

   /*
   ** Install device driver for socket and telnet
   */
   _io_socket_install("socket:");
   _io_telnet_install("telnet:");

   sock = socket(PF_INET, SOCK_STREAM, 0);
   if (sock == RTCS_SOCKET_ERROR) {
      return RTCSERR_OUT_OF_SOCKETS;
   } /* Endif */

   addr.sin_family      = AF_INET;
   addr.sin_port        = 0;
   addr.sin_addr.s_addr = INADDR_ANY;
   error = bind(sock,(const sockaddr *)&addr, sizeof(addr));
   if (error != RTCS_OK) {
      return error;
   } /* Endif */

   addr.sin_port        = IPPORT_TELNET;
   addr.sin_addr.s_addr = ipaddress;

   error = connect(sock, (const sockaddr *)(&addr), sizeof(addr));
   if (error != RTCS_OK) {
      shutdown(sock, FLAG_ABORT_CONNECTION);
      return error;
   } /* Endif */

   sockfd = fopen("socket:", (char *)sock);
   if (sockfd == NULL) {
      shutdown(sock, FLAG_ABORT_CONNECTION);
      return RTCSERR_FOPEN_FAILED;
   } /* Endif */

   telnetfd = fopen("telnet:", (char *)sockfd);
   if (telnetfd == NULL) {
      fclose(sockfd);
      shutdown(sock, FLAG_ABORT_CONNECTION);
      return RTCSERR_FOPEN_FAILED;
   } /* Endif */

   /* Set the console stream to the client  */
   ioctl(telnetfd, IO_IOCTL_SET_STREAM, (uint32_t *)((void *)stdin));
   while (TRUE) {

      work = FALSE;
      if (fstatus(stdin)) {
         work = TRUE;
         c = (int32_t)fgetc(stdin);
         if (fputc(c & 0x7F, telnetfd) == IO_EOF)  {
            break;   
         }
      } /* Endif */

      if (fstatus(telnetfd)) {
         work = TRUE;
         c = (int32_t)fgetc(telnetfd);
         if (c == IO_EOF) {
            break;
         }/* Endif */
         fputc(c & 0x7F, stdout);
      } /* Endif */

      /* Let another task run if there is no I/O */
      if (!work) {
         RTCS_time_delay(1);
      } /* Endif */

   } /* Endwhile */

   fclose(telnetfd);
   fclose(sockfd);
   shutdown(sock, FLAG_CLOSE_TX);

   return RTCS_OK;

} /* Endbody */

#endif // MQX_USE_IO_OLD
