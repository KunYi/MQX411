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
*   and setsockopt() at the SOL_TCP level.
*
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"

#if RTCSCFG_ENABLE_TCP

#define RTCS_ENTER(f,a) RTCSLOG_API(RTCSLOG_TYPE_FNENTRY, RTCSLOG_FN_TCP_ ## f, a)

#define RTCS_EXIT(f,a)  RTCSLOG_API(RTCSLOG_TYPE_FNEXIT,  RTCSLOG_FN_TCP_ ## f, a); \
                        return a


uint32_t SOL_TCP_getsockopt (uint32_t, uint32_t, uint32_t, void *, uint32_t *);
uint32_t SOL_TCP_setsockopt (uint32_t, uint32_t, uint32_t, void *, uint32_t);

const RTCS_SOCKOPT_CALL_STRUCT SOL_TCP_CALL = {
   SOL_TCP_getsockopt,
   SOL_TCP_setsockopt
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_TCP_getsockopt
* Returned Value  : error code
* Comments  :  Obtain the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint32_t  SOL_TCP_getsockopt
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
   register SOCKET_STRUCT_PTR    socket_ptr = (SOCKET_STRUCT_PTR)sock;

   RTCS_ENTER(GETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   *(uint32_t *)optval = ((uint32_t *)&socket_ptr->CONNECT_TIMEOUT)[optname-1];
   *optlen = sizeof(uint32_t);

   RTCS_EXIT(GETSOCKOPT, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_TCP_setsockopt
* Returned Value  : error code
* Comments  :  Routine is used to modify the current socket options.
*
*END*-----------------------------------------------------------------*/

uint32_t  SOL_TCP_setsockopt
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
   register SOCKET_STRUCT_PTR    socket_ptr = (SOCKET_STRUCT_PTR)sock;

   RTCS_ENTER(SETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   ((uint32_t *)&socket_ptr->CONNECT_TIMEOUT)[optname-1] = *(uint32_t *)optval;

   RTCS_EXIT(SETSOCKOPT, RTCS_OK);

} /* Endbody */

#else

const RTCS_SOCKOPT_CALL_STRUCT SOL_TCP_CALL; /* for SOL_TCP, TBD fix other way.*/

#endif

/* EOF */
