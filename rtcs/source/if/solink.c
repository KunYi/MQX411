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
*   and setsockopt() at the SOL_LINK level.
*
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"

#define RTCS_ENTER(f,a) RTCSLOG_API(RTCSLOG_TYPE_FNENTRY, RTCSLOG_FN_LINK_ ## f, a)

#define RTCS_EXIT(f,a)  RTCSLOG_API(RTCSLOG_TYPE_FNEXIT,  RTCSLOG_FN_LINK_ ## f, a); \
                        return a


uint32_t SOL_LINK_getsockopt (uint32_t, uint32_t, uint32_t, void *, uint32_t *);
uint32_t SOL_LINK_setsockopt (uint32_t, uint32_t, uint32_t, void *, uint32_t);

const RTCS_SOCKOPT_CALL_STRUCT SOL_LINK_CALL = {
   SOL_LINK_getsockopt,
   SOL_LINK_setsockopt
};


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_LINK_getsockopt
* Returned Value  : error code
* Comments  :  Obtain the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint32_t  SOL_LINK_getsockopt
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
   /* Start SPR P122-0266-34. remove all warnings from RTCS code. */
#if RTCSCFG_LINKOPT_8023 || RTCSCFG_LINKOPT_8021Q_PRIO
   register SOCKET_STRUCT_PTR    socket_ptr = (SOCKET_STRUCT_PTR)sock;
#endif

   RTCS_ENTER(GETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   switch (optname) {

#if RTCSCFG_LINKOPT_8023
   case RTCS_SO_LINK_RX_8023:
      *(uint32_t *)optval = socket_ptr->LINK_OPTIONS.RX.OPT_8023;
      break;
   case RTCS_SO_LINK_TX_8023:
      *(uint32_t *)optval = socket_ptr->LINK_OPTIONS.TX.OPT_8023;
      /* break; */
       *optlen = sizeof(uint32_t);
      RTCS_EXIT(GETSOCKOPT, RTCS_OK);
#endif

#if RTCSCFG_LINKOPT_8021Q_PRIO
   case RTCS_SO_LINK_RX_8021Q_PRIO:
      *(int32_t *)optval = socket_ptr->LINK_OPTIONS.RX.OPT_PRIO ? socket_ptr->LINK_OPTIONS.RX.PRIO : -1;
      break;
   case RTCS_SO_LINK_TX_8021Q_PRIO:
      *(int32_t *)optval = socket_ptr->LINK_OPTIONS.TX.OPT_PRIO ? socket_ptr->LINK_OPTIONS.TX.PRIO : -1;
      /* break; */
      *optlen = sizeof(uint32_t);
      RTCS_EXIT(GETSOCKOPT, RTCS_OK);
#endif

   default:
      RTCS_EXIT(GETSOCKOPT, RTCSERR_SOCK_INVALID_OPTION);
   } /* Endswitch */

   /* *optlen = sizeof(uint32_t); */
   /* RTCS_EXIT(GETSOCKOPT, RTCS_OK); */
   /* End SPR P122-0266-34. */

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SOL_LINK_setsockopt
* Returned Value  : error code
* Comments  :  Modify the current value for a socket option.
*
*END*-----------------------------------------------------------------*/

uint32_t  SOL_LINK_setsockopt
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
/* Start SPR P122-0266-34 remove all warnings from RTCS code. */
#if RTCSCFG_LINKOPT_8023 || RTCSCFG_LINKOPT_8021Q_PRIO
   register SOCKET_STRUCT_PTR    socket_ptr = (SOCKET_STRUCT_PTR)sock;
#endif

   RTCS_ENTER(SETSOCKOPT, sock);

#if RTCSCFG_CHECK_ERRORS
   if (sock == 0 || sock == RTCS_SOCKET_ERROR) {
      RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID);
   } /* Endif */
#endif

   switch (optname) {

#if RTCSCFG_LINKOPT_8023
   case RTCS_SO_LINK_TX_8023:
      socket_ptr->LINK_OPTIONS.TX.OPT_8023 = *(uint32_t *)optval ? TRUE : FALSE;
      /* break; */
      RTCS_EXIT(SETSOCKOPT, RTCS_OK);
#endif

#if RTCSCFG_LINKOPT_8021Q_PRIO
   case RTCS_SO_LINK_TX_8021Q_PRIO:
      if (*(int32_t *)optval == -1) {
         socket_ptr->LINK_OPTIONS.TX.OPT_PRIO = 0;
      } else {
         socket_ptr->LINK_OPTIONS.TX.OPT_PRIO = 1;
         socket_ptr->LINK_OPTIONS.TX.PRIO = *(int32_t *)optval;
      } /* Endif */
      /* break; */
      RTCS_EXIT(SETSOCKOPT, RTCS_OK);
#endif

   default:
      RTCS_EXIT(SETSOCKOPT, RTCSERR_SOCK_INVALID_OPTION);
   } /* Endswitch */

   /* RTCS_EXIT(SETSOCKOPT, RTCS_OK); */
   /* End SPR P122-0266-34. */

} /* Endbody */


/* EOF */
