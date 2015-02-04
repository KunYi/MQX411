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
*   This file contains the implementation of the
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
*  Function Name  : IPIF_remove
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     [IN] the IP interface handle
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     not used
*     _ip_address          locmask     not used
*     _ip_address          network     not used
*     _ip_address          netmask     not used
*
*  Comments       :
*        Unregisters a hardware interface with RTCS.
*
*END*-----------------------------------------------------------------*/

void IPIF_remove
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */
   IP_CFG_STRUCT_PTR IP_cfg_ptr;
   IP_IF_PTR         ipif;
   uint32_t           error;

   IP_cfg_ptr = RTCS_getcfg(IP);

   ipif = (IP_IF_PTR)parms->ihandle;
   error = ipif->DEVICE.CLOSE(ipif);

   /* Enqueue onto the free list */
   ipif->HANDLE = IF_FREE;
   IF_FREE = ipif;

   RTCSCMD_complete(parms, error);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_unbind
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     [IN] the IP interface handle
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     [IN] the local address
*     _ip_address          locmask     not used
*     _ip_address          network     not used
*     _ip_address          netmask     not used
*
*  Comments       :
*        Unbinds an IP address and network from a hardware interface.
*
*END*-----------------------------------------------------------------*/

void IPIF_unbind
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   _ip_address    mask;

   IP_route_remove_direct(parms->address, 0xFFFFFFFF, parms->ihandle);
   
#if RTCSCFG_ENABLE_IGMP 
   if (parms->ihandle) {
      ((IP_IF_PTR)(parms->ihandle))->IGMP_UNBIND((IP_IF_PTR)parms->ihandle,
         parms->address);
   } /* Endif */
#endif
   IP_get_netmask(parms->ihandle, parms->address, &mask);
   
   /* If mask is invalid do not try to remove direct route */
   if (mask != 1)
   {
        IP_route_remove_direct(parms->address, mask, parms->ihandle);
   }
   RTCSCMD_complete(parms, RTCS_OK);

#else

    RTCSCMD_complete(parms, RTCSERR_IP_IS_DISABLED);

#endif /* RTCSCFG_ENABLE_IP4 */
   

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_unbind_ppp
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     [IN] the IP interface handle
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     [IN] the local address
*     _ip_address          locmask     not used
*     _ip_address          network     [IN] the peer address
*     _ip_address          netmask     not used
*
*  Comments       :
*        Unbinds an IP address and network from a hardware interface.
*
*END*-----------------------------------------------------------------*/

void IPIF_unbind_ppp
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   IP_route_remove_virtual(parms->network, 0xFFFFFFFF, parms->address,
      INADDR_ANY, INADDR_ANY, parms->ihandle);
      
#if RTCSCFG_ENABLE_IGMP
   if (parms->ihandle) {
      ((IP_IF_PTR)(parms->ihandle))->IGMP_UNBIND((IP_IF_PTR)parms->ihandle,
         parms->address);
   } /* Endif */
#endif

   IP_route_remove_direct(parms->address, 0xFFFFFFFF, parms->ihandle);

   RTCSCMD_complete(parms, RTCS_OK);

#else

    RTCSCMD_complete(parms, RTCSERR_IP_IS_DISABLED);

#endif /* RTCSCFG_ENABLE_IP4 */
   

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_gate_remove
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     not used
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     [IN] the gateway address
*     _ip_address          locmask     not used
*     _ip_address          network     [IN] the network address
*     _ip_address          netmask     [IN] the network address mask
*
*  Comments       :
*        Removes a gateway from the routing table.
*
*END*-----------------------------------------------------------------*/

void IPIF_gate_remove
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

    /* Start CR 1133 */
    IP_route_remove_indirect(parms->address, parms->netmask, parms->network, RTF_STATIC, parms->locmask);
    /* End CR 1133 */

    RTCSCMD_complete(parms, RTCS_OK);

#else

    RTCSCMD_complete(parms, RTCSERR_IP_IS_DISABLED);

#endif /* RTCSCFG_ENABLE_IP4 */
   

} /* Endbody */


/* EOF */
