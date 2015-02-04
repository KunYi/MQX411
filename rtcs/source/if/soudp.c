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
*   This file contains the implementation of getsockopt()
*   and setsockopt() at the SOL_UDP level.
*
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "udp.h"

#if RTCSCFG_ENABLE_UDP

#define RTCS_ENTER(f,a) RTCSLOG_API(RTCSLOG_TYPE_FNENTRY, RTCSLOG_FN_UDP_ ## f, a)

#define RTCS_EXIT(f,a)  RTCSLOG_API(RTCSLOG_TYPE_FNEXIT,  RTCSLOG_FN_UDP_ ## f, a); \
                        return a


uint32_t SOL_UDP_getsockopt  (uint32_t, uint32_t, uint32_t, void *, uint32_t *);
uint32_t SOL_UDP_setsockopt  (uint32_t, uint32_t, uint32_t, void *, uint32_t);

const RTCS_SOCKOPT_CALL_STRUCT SOL_UDP_CALL = {
   SOL_UDP_getsockopt,
   SOL_UDP_setsockopt
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_UDP_getsockopt
* Returned Value  : error code
* Comments  :  Obtain the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint32_t  SOL_UDP_getsockopt
   (
      uint32_t        sock,
         /* [IN] socket handle */
      uint32_t        level,
         /* [IN] protocol level for the option */
      uint32_t        optname,
         /* [IN] name of the option */
      void          *optval,
         /* [IN] buffer for the current value of the option */
      uint32_t   *optlen
         /* [IN/OUT] length of the option value, in bytes */
   )
{ /* Body */
   UDP_PARM    parms;
   uint32_t     error;

   RTCS_ENTER(GETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   parms.ucb      = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR;
   parms.udpflags = optname;
   error = RTCSCMD_issue(parms, UDP_getopt);
   if (error) {
      RTCS_EXIT(GETSOCKOPT, error);
   } /* Endif */

   *(uint32_t *)optval = parms.udpword;
   RTCS_EXIT(GETSOCKOPT, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_UDP_setsockopt
* Returned Value  : error code
* Comments  :  Modify the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint32_t  SOL_UDP_setsockopt
   (
      uint32_t        sock,
         /* [IN] socket handle */
      uint32_t        level,
         /* [IN] protocol level for the option */
      uint32_t        optname,
         /* [IN] name of the option */
      void          *optval,
         /* [IN] new value for the option */
      uint32_t        optlen
         /* [IN] length of the option value, in bytes */
   )
{ /* Body */
   UDP_PARM    parms;
   uint32_t     error;

   RTCS_ENTER(SETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   parms.ucb      = ((SOCKET_STRUCT_PTR)sock)->UCB_PTR;
   parms.udpflags = optname;
   parms.udpword  = *(uint32_t *)optval;
   error = RTCSCMD_issue(parms, UDP_setopt);
   if (error) {
      RTCS_EXIT(SETSOCKOPT, error);
   } /* Endif */

   RTCS_EXIT(SETSOCKOPT, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_getopt
* Parameters      :
*
*     UCB_STRUCT_PTR    ucb         [IN] UCB
*     _ip_address       ipaddress   not used
*     uint16_t           udpport     not used
*     uint16_t           udpflags    [IN] option
*     uint32_t           udpptr      not used
*     uint32_t           udpword     [OUT] option value
*
* Comments        :
*     Retrieves the value of a UDP option.
*
*END*-----------------------------------------------------------------*/

void UDP_getopt
   (
      UDP_PARM_PTR      parms
   )
{ /* Body */
   uint32_t     error = RTCS_OK;

   switch (parms->udpflags) {

   /* Start CR 1145 */
   case RTCS_SO_UDP_NONBLOCK_TX:
      parms->udpword = parms->ucb->NONBLOCK_TX;
      break;

   case RTCS_SO_UDP_NONBLOCK_RX:
      parms->udpword = parms->ucb->NONBLOCK_RX;
      break;
   /* End CR 1145 */

   case RTCS_SO_UDP_NOCHKSUM:
      parms->udpword = parms->ucb->BYPASS_TX;
      break;

   default:
      error = RTCSERR_SOCK_INVALID_OPTION;
   } /* Endswitch */

   RTCSCMD_complete(parms, error);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : UDP_setopt
* Parameters      :
*
*     UCB_STRUCT_PTR    ucb         [IN] UCB
*     _ip_address       ipaddress   not used
*     uint16_t           udpport     not used
*     uint16_t           udpflags    [IN] option
*     uint32_t           udpptr      not used
*     uint32_t           udpword     [IN] option value
*
* Comments        :
*     Sets the value of a UDP option.
*
*END*-----------------------------------------------------------------*/

void UDP_setopt
   (
      UDP_PARM_PTR      parms
   )
{ /* Body */
   uint32_t     error = RTCS_OK;

   switch (parms->udpflags) {

   /* Start CR 1145 */
   case RTCS_SO_UDP_NONBLOCK_TX:
      parms->ucb->NONBLOCK_TX = parms->udpword ? TRUE : FALSE;
      break;

   case RTCS_SO_UDP_NONBLOCK_RX:
      parms->ucb->NONBLOCK_RX = parms->udpword ? TRUE : FALSE;
      break;
   /* End CR 1145 */

   case RTCS_SO_UDP_NOCHKSUM:
      parms->ucb->BYPASS_TX = parms->udpword ? TRUE : FALSE;
      break;

   default:
      error = RTCSERR_SOCK_INVALID_OPTION;
   } /* Endswitch */

   RTCSCMD_complete(parms, error);

} /* Endbody */
#else

const RTCS_SOCKOPT_CALL_STRUCT SOL_UDP_CALL; /* for SOL_UDP, TBD fix other way.*/

#endif

/* EOF */
