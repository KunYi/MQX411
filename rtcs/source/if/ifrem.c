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
*   This file contains the interface functions to the
*   packet driver interface.
*
*
*END************************************************************************/

#include <rtcs.h>
#include "rtcs_prv.h"
#include "tcpip.h"
#include "ip_prv.h"


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_remove
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Unregister a hardware interface with RTCS.
*
*END*-----------------------------------------------------------------*/

uint32_t RTCS_if_remove
   (
      _rtcs_if_handle   handle
         /* [IN] the RTCS interface state structure */
   )
{ /* Body */
   IPIF_PARM   parms;

   parms.ihandle = handle;

   return RTCSCMD_issue(parms, IPIF_remove);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_if_unbind
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Unbind an IP address from a registered hardware interface.
*
*END*-----------------------------------------------------------------*/

uint32_t RTCS_if_unbind
   (
      _rtcs_if_handle   handle,
         /* [IN] the RTCS interface state structure */
      _ip_address       address
         /* [IN] the IP address for the interface */
   )
{ /* Body */
   IPIF_PARM   parms;

   parms.ihandle = handle;
   parms.address = address;

   return RTCSCMD_issue(parms, IPIF_unbind);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_gate_remove
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Register a gateway with RTCS.
*
*END*-----------------------------------------------------------------*/

uint32_t RTCS_gate_remove
   (
      _ip_address       gateway,
         /* [IN] the IP address of the gateway */
      _ip_address       network,
         /* [IN] the IP (sub)network to route */
      _ip_address       netmask
         /* [IN] the IP (sub)network mask to route */
   )
{ /* Body */
#if RTCSCFG_ENABLE_GATEWAYS
   IPIF_PARM   parms;

   parms.address = gateway;
   parms.network = network;
   parms.netmask = netmask;
   /* Start CR 1133 */
   parms.locmask = 0;
   /* End CR 1133 */

   return RTCSCMD_issue(parms, IPIF_gate_remove);
#else
   return RTCSERR_FEATURE_NOT_ENABLED;
#endif
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_gate_remove_metric
* Returned Value  : RTCS_OK or error code
* Comments        :
*        Removes a gateway from RTCS if the metric matches the route.
*
*END*-----------------------------------------------------------------*/

uint32_t RTCS_gate_remove_metric
   (
      _ip_address       gateway,
         /* [IN] the IP address of the gateway */
      _ip_address       network,
         /* [IN] the IP (sub)network to route */
      _ip_address       netmask,
         /* [IN] the IP (sub)network mask to route */
      uint16_t           metric
         /* [IN] the metric of the gateway */
   )
{ /* Body */
#if RTCSCFG_ENABLE_GATEWAYS
   IPIF_PARM   parms;

   parms.address = gateway;
   parms.network = network;
   parms.netmask = netmask;
   parms.locmask = metric;

   return RTCSCMD_issue(parms, IPIF_gate_remove);
#else
   return RTCSERR_FEATURE_NOT_ENABLED;
#endif
} 
