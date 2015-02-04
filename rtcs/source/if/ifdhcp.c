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
*   This file contains the interface functions for the
*   DHCP client.
*
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "dhcp_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_bind_DHCP
* Returned Values : uint32_t (error code)
* Comments        :
*     Initialize a hardware interface using DHCP.
*
*END*-----------------------------------------------------------------*/

uint32_t RTCS_if_bind_DHCP
   (
      _rtcs_if_handle         handle,
         /* [IN] the RTCS interface state structure */
      DHCP_DATA_STRUCT_PTR    params,
         /* [IN] optional parameters */
      unsigned char               *optptr,
         /* [IN] the DHCP options the application wishes to use */
      uint32_t                 optlen
         /* [IN] the size of the options buffer passed in */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4 

   TCPIP_PARM_IF_DHCP         parms;

   if (handle==NULL) 
   {
      return RTCSERR_INVALID_PARAMETER; 
   }
   
   parms.HANDLE        = handle;
   /* Start CR 1547 */
   parms.FLAGS         = DHCP_SEND_PROBE;
   /* End CR 1547 */
   parms.OPT_BUFF      = optptr;
   parms.OPT_BUFF_SIZE = optlen;
   parms.OPTIONS       = params;
   parms.TIMEOUT       = _RTCS_dhcp_term_timeout;

   /* We block the application until DHCP completes initialization. */
   return RTCSCMD_issue(parms, DHCPCLNT_init);

#else

    return RTCSERR_IP_IS_DISABLED;

#endif /* RTCSCFG_ENABLE_IP4 */   

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_rebind_DHCP
* Returned Values : uint32_t (error code)
* Comments        :
*     Initialize a hardware interface using DHCP.
*
*END*-----------------------------------------------------------------*/

uint32_t RTCS_if_rebind_DHCP
   (
      _rtcs_if_handle         handle,
         /* [IN] the RTCS interface state structure */
      _ip_address             address,
         /* [IN] the IP address for the interface */
      _ip_address             netmask,
         /* [IN] the IP (sub)network mask for the interface */
      uint32_t                 lease,
         /* [IN] the duration of the lease in seconds */
      _ip_address             server,
         /* [IN] the IP address of the DHCP server */
      DHCP_DATA_STRUCT_PTR    params,
         /* [IN] optional parameters */
      unsigned char               *optptr,
         /* [IN] the DHCP options the application wishes to use */
      uint32_t                 optlen
         /* [IN] the size of the options buffer passed in */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4 

   TCPIP_PARM_IF_DHCP         parms;

   parms.HANDLE         = handle;
   parms.FLAGS         = 0;
   parms.OPT_BUFF       = optptr;
   parms.OPT_BUFF_SIZE  = optlen;
   parms.OPTIONS        = params;

   parms.CLNT_IP_ADDR   = address;
   parms.CLNT_IP_MASK   = netmask;
   parms.SERVER_IP_ADDR = server;
   parms.LEASE          = lease;
   /* Start CR 1066 */
   parms.TIMEOUT       = 0;
   /* End CR 1066 */

   /* We block the application until DHCP completes initialization. */
   return RTCSCMD_issue(parms, DHCPCLNT_reinit);

#else

    return RTCSERR_IP_IS_DISABLED;

#endif /* RTCSCFG_ENABLE_IP4 */     

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_request_DHCP_inform
* Returned Values : uint32_t (error code)
* Comments        :
*     Request the sending of a DHCP inform message.
*
*END*-----------------------------------------------------------------*/

uint32_t RTCS_request_DHCP_inform
   (
      _rtcs_if_handle         handle,
         /* [IN] the RTCS interface state structure */
      unsigned char               *optptr,
         /* [IN] the DHCP options the application wishes to use */
      uint32_t                 optlen,
         /* [IN] the size of the options buffer passed in */
      _ip_address             client_addr,
        /* [IN] the IP address the application is bound to */
      _ip_address             server_addr,
        /* [IN] the IP address of the server we want parameters from */
      void (_CODE_PTR_ inform_func)(unsigned char *, uint32_t, _rtcs_if_handle)
      /* The function to call when DHCP is finished */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4 

   TCPIP_PARM_IF_DHCP parms;
   DHCP_DATA_STRUCT   options;

   options.BIND_FUNC    = inform_func;
   options.CHOICE_FUNC  = NULL;
   options.UNBIND_FUNC  = NULL;
   options.REBIND_FUNC  = NULL;

   parms.HANDLE         = handle;
   parms.FLAGS          = DHCP_SEND_INFORM_MESSAGE;
   parms.OPT_BUFF       = optptr;
   parms.OPT_BUFF_SIZE  = optlen;
   parms.OPTIONS        = &options;
   parms.CLNT_IP_ADDR   = client_addr;
   parms.SERVER_IP_ADDR = server_addr;

   /* We block the application until DHCP completes initialization. */
   return RTCSCMD_issue(parms, DHCPCLNT_init);

#else

    return RTCSERR_IP_IS_DISABLED;

#endif /* RTCSCFG_ENABLE_IP4 */  
   
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_bind_DHCP_timed
* Returned Values : uint32_t (error code)
* Comments        :
*     Initialize a hardware interface using DHCP.
*
*END*-----------------------------------------------------------------*/

uint32_t RTCS_if_bind_DHCP_timed
   (
      _rtcs_if_handle         handle,
         /* [IN] the RTCS interface state structure */
      DHCP_DATA_STRUCT_PTR    params,
         /* [IN] optional parameters */
      unsigned char               *optptr,
         /* [IN] the DHCP options the application wishes to use */
      uint32_t                 optlen,
         /* [IN] the size of the options buffer passed in */
      uint32_t                 timeout
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4 

   TCPIP_PARM_IF_DHCP         parms;

   if (handle==NULL) 
   {
      return RTCSERR_INVALID_PARAMETER; 
   }
   

   parms.HANDLE        = handle;
   /* Start CR 1547 */
   parms.FLAGS         = DHCP_SEND_PROBE;
   /* End CR 1547 */
   parms.OPT_BUFF      = optptr;
   parms.OPT_BUFF_SIZE = optlen;
   parms.OPTIONS       = params;
   parms.TIMEOUT       = timeout;

   /* We block the application until DHCP completes initialization. */
   return RTCSCMD_issue(parms, DHCPCLNT_init);

#else

    return RTCSERR_IP_IS_DISABLED;

#endif /* RTCSCFG_ENABLE_IP4 */     

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_bind_DHCP_flagged
* Returned Values : uint32_t (error code)
* Comments        :
*     Initialize a hardware interface using DHCP.
*
*END*-----------------------------------------------------------------*/

uint32_t RTCS_if_bind_DHCP_flagged
   (
      _rtcs_if_handle         handle,
         /* [IN] the RTCS interface state structure */
      DHCP_DATA_STRUCT_PTR    params,
         /* [IN] optional parameters */
      unsigned char               *optptr,
         /* [IN] the DHCP options the application wishes to use */
      uint32_t                 optlen
         /* [IN] the size of the options buffer passed in */
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4 

   TCPIP_PARM_IF_DHCP         parms;

   if (handle==NULL) 
   {
      return RTCSERR_INVALID_PARAMETER; 
   }
   

   parms.HANDLE        = handle;
   parms.FLAGS         = params->FLAGS;
   parms.OPT_BUFF      = optptr;
   parms.OPT_BUFF_SIZE = optlen;
   parms.OPTIONS       = params;
   parms.TIMEOUT       = 0;

   /* We block the application until DHCP completes initialization. */
   return RTCSCMD_issue(parms, DHCPCLNT_init);


#else

    return RTCSERR_IP_IS_DISABLED;

#endif /* RTCSCFG_ENABLE_IP4 */ 
   

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : DHCPCLNT_release
* Returned Values : void
* Comments        :
*
*END*-----------------------------------------------------------------*/

void DHCPCLNT_release
   (
      _rtcs_if_handle    ihandle
         /* [IN] The pointer to the interface we no longer need */
   )
{  /* Body */

#if RTCSCFG_ENABLE_IP4 

   TCPIP_PARM_IF_DHCP_RELEASE dhcp_parms;

   dhcp_parms.HANDLE = ihandle;

   RTCSCMD_issue(dhcp_parms, DHCPCLNT_release_internal);

#endif /* RTCSCFG_ENABLE_IP4 */ 
   
   
}  /* Endbody */


/* EOF */
