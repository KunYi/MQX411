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
*   and setsockopt() at the SOL_SOCKET level.
*
*
*END************************************************************************/

#include <rtcs.h>

#define RTCS_ENTER(f,a) RTCSLOG_API(RTCSLOG_TYPE_FNENTRY, RTCSLOG_FN_SOCKET_ ## f, a)

#define RTCS_EXIT(f,a)  RTCSLOG_API(RTCSLOG_TYPE_FNEXIT,  RTCSLOG_FN_SOCKET_ ## f, a); \
                        return a


uint32_t SOL_SOCKET_getsockopt  (uint32_t, uint32_t, uint32_t, void *, uint32_t *);
uint32_t SOL_SOCKET_setsockopt  (uint32_t, uint32_t, uint32_t, void *, uint32_t);

const RTCS_SOCKOPT_CALL_STRUCT SOL_SOCKET_CALL = {
   SOL_SOCKET_getsockopt,
   SOL_SOCKET_setsockopt
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_SOCKET_getsockopt
* Returned Value  : error code
* Comments  :  Obtain the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint32_t  SOL_SOCKET_getsockopt
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
   uint32_t error;

   RTCS_ENTER(GETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   switch (optname) {

   case RTCS_SO_TYPE:
      *(uint32_t *)optval = (uint32_t)((SOCKET_STRUCT_PTR)sock)->PROTOCOL;
      break;

   case RTCS_SO_ERROR:
      *(uint32_t *)optval = (uint32_t)((SOCKET_STRUCT_PTR)sock)->ERROR_CODE;
      break;

   default:
      if ((uint32_t)((SOCKET_STRUCT_PTR)sock)->PROTOCOL == SOCK_STREAM) {
         level = SOL_TCP;
#if RTCSCFG_ENABLE_UDP
      } else if ((uint32_t)((SOCKET_STRUCT_PTR)sock)->PROTOCOL == SOCK_DGRAM) {
         level = SOL_UDP;
#endif
      } else {
         RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_INVALID_OPTION);
      } /* Endif */
      error = getsockopt(sock, level, optname, optval, optlen);
      RTCS_EXIT(GETSOCKOPT, error);
   } /* Endswitch */

   RTCS_EXIT(GETSOCKOPT, RTCS_OK);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_SOCKET_setsockopt
* Returned Value  : error code
* Comments  :  Modify the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint32_t  SOL_SOCKET_setsockopt
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
   uint32_t error;

   RTCS_ENTER(SETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   switch (optname) {

   case RTCS_SO_TYPE:
   case RTCS_SO_ERROR:
      RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID_OPTION);

   default:
      if ((uint32_t)((SOCKET_STRUCT_PTR)sock)->PROTOCOL == SOCK_STREAM) {
         level = SOL_TCP;
#if RTCSCFG_ENABLE_UDP
      } else if ((uint32_t)((SOCKET_STRUCT_PTR)sock)->PROTOCOL == SOCK_DGRAM) {
         level = SOL_UDP;
#endif
      } else {
         RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID_OPTION);
      } /* Endif */
      error = setsockopt(sock, level, optname, optval, optlen);
      RTCS_EXIT(SETSOCKOPT, error);
   } /* Endswitch */

   /* Start SPR P122-0266-35. remove all warnings from RTCS code. */
   /* RTCS_EXIT(SETSOCKOPT, RTCS_OK); */
   /* End SPR P122-0266-35. */

} /* Endbody */


/* EOF */
