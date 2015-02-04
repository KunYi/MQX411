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
#include "route.h"
#include <arp.h>

IP_IF_PTR         IF_FREE =  NULL;        /* linked list of free IP_IFs */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_add
*  Parameters     :
*
*     pointer              mhandle     [IN] the packet driver handle
*     _rtcs_if_handle      ihandle     [OUT] the IP interface handle
*     RTCS_IF_STRUCT_PTR   if_ptr      [IN] the call table
*     _ip_address          address     not used
*     _ip_address          locmask     not used
*     _ip_address          network     not used
*     _ip_address          netmask     not used
*
*  Comments       :
*        Registers a hardware interface with RTCS.
*
*END*-----------------------------------------------------------------*/
void IPIF_add(IPIF_PARM_PTR  parms)
{ 
    IP_CFG_STRUCT_PTR IP_cfg_ptr;
    IP_IF_PTR         ipif;
    uint32_t           i, error;

    IP_cfg_ptr = RTCS_getcfg(IP);

    /* Allocate a block of entries if none are left */
    if (!IF_FREE)
    {
        ipif = RTCS_mem_alloc_zero(IPIFALLOC_SIZE * sizeof(IP_IF));
        if (ipif)
        {
            _mem_set_type(ipif, MEM_TYPE_IP_IF);

            for (i = 0; i < IPIFALLOC_SIZE; i++)
            {
                ipif->HANDLE = IF_FREE;
                IF_FREE = ipif;
                ipif++;
            }
        }
    }

    /* Dequeue a free entry */
    ipif = IF_FREE;
    if (!ipif)
    {
        RTCSCMD_complete(parms, RTCSERR_IP_IF_ALLOC);
        return;
    } 

    /* Initialize the interface */
    IF_FREE = ipif->HANDLE;
    ipif->HANDLE = parms->mhandle;
    ipif->DEVICE = *parms->if_ptr;
    ipif->ARP    = NULL;
    ipif->BOOTFN = NULL;
    ipif->MTU_FN = NULL;
    ipif->MTU_FN = IP_IF_FEATURE_NONE;
    error = ipif->DEVICE.OPEN(ipif);
    if (error)
    {
        ipif->HANDLE = IF_FREE;
        IF_FREE = ipif;
        RTCSCMD_complete(parms, error);
        return;
    }

#if RTCSCFG_ENABLE_IGMP && RTCSCFG_ENABLE_IP4
    /* If IGMP_ipif_add fails, it isn't sufficient reason to abort IPIF_add */
    IGMP_ipif_add(ipif);
#endif

    parms->ihandle = (_rtcs_if_handle)ipif;
    RTCSCMD_complete(parms, RTCS_OK);
} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_bind
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     [IN] the IP interface handle
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     [IN] the local address
*     _ip_address          locmask     not used
*     _ip_address          network     not used
*     _ip_address          netmask     [IN] the network address mask
*
*  Comments       :
*        Starts bind of an IP address and network to a hardware interface.
*
*END*-----------------------------------------------------------------*/

void IPIF_bind
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   _ip_address       address, netmask;
   uint32_t           error = RTCS_OK;

   /* Start CR 2303 */
   if (parms->ihandle==NULL) {
      RTCSCMD_complete(parms, RTCSERR_INVALID_PARAMETER);
      return;
   } /* Endif */
   /* End CR 2303 */

   address = parms->address;
   netmask = parms->netmask;

   /*
   ** Make sure the netmask is valid.  We use the fact that
   ** (x & x+1) == 0  <=>  x = 2^n-1.
   */
   if (~netmask & (~netmask + 1)) {
      RTCSCMD_complete(parms, RTCSERR_IP_BIND_MASK);
      return;
   } /* Endif */

   /*
   ** Make sure the address is valid.
   */
   if (((address & ~netmask) == 0)
    || ((address & ~netmask) == ~netmask)) {
      RTCSCMD_complete(parms, RTCSERR_IP_BIND_ADDR);
      return;
   } /* Endif */

   if (parms->probe) {
      // probe IP address here, set error if in use
      if (ARP_request(parms->ihandle, address, address ) != RTCS_OK) {
         error = RTCSERR_SEND_FAILED;
      }
      RTCSCMD_complete(parms, error);
   }
   else
   {
      IPIF_bind_finish (parms);
   }
#else

    RTCSCMD_complete(parms, RTCSERR_IP_IS_DISABLED);

#endif /* RTCSCFG_ENABLE_IP4 */
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_bind_finish
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     [IN] the IP interface handle
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     [IN] the local address
*     _ip_address          locmask     not used
*     _ip_address          network     not used
*     _ip_address          netmask     [IN] the network address mask
*
*  Comments       :
*        Finishes bind of an IP address and network to a hardware interface.
*
*END*-----------------------------------------------------------------*/

void IPIF_bind_finish
   (
      IPIF_PARM_PTR  parms
   )
{
#if RTCSCFG_ENABLE_IP4   
    IP_CFG_STRUCT_PTR IP_cfg_ptr;
   uint32_t           error = RTCS_OK;
   
   IP_cfg_ptr = RTCS_getcfg(IP);
   
   if (parms->probe)
   {
      if (ARP_is_complete (parms->ihandle, parms->address)) {
         error = RTCSERR_TCP_ADDR_IN_USE;
      }
   }
   if (!error) {
      /* Create the route entry for the directly connected network */
      error = IP_route_add_direct(parms->address, parms->netmask, (IP_IF_PTR)parms->ihandle,
         (IP_IF_PTR)parms->ihandle);
   }
   /* Add the IP and the LOCALHOST interface into the routing table */
   if (!error) {
      error = IP_route_add_direct(parms->address, 0xFFFFFFFF, (IP_IF_PTR)parms->ihandle,
         RTCS_IF_LOCALHOST_PRV);
   } /* Endif */

   if (error) {
      RTCSCMD_complete(parms, error);
      return;
   } /* Endif */

#if RTCSCFG_ENABLE_IGMP && RTCSCFG_ENABLE_IP4
   /* IGMP_ipif_bind failure isn't sufficient reason to abort IPIF_bind */
   IGMP_ipif_bind((IP_IF_PTR)parms->ihandle, parms->address);
#endif

   RTCSCMD_complete(parms, error);

#else

    RTCSCMD_complete(parms, RTCSERR_IP_IS_DISABLED);

#endif /* RTCSCFG_ENABLE_IP4 */
   

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_bind_ppp
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
*        Binds an IP address and network to a hardware interface.
*
*END*-----------------------------------------------------------------*/

void IPIF_bind_ppp
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   IP_CFG_STRUCT_PTR IP_cfg_ptr;
   _ip_address       address, peer;
   uint32_t           error;

   IP_cfg_ptr = RTCS_getcfg(IP);

   address = parms->address;
   peer    = parms->network;
   
   /* Create the route entry for the directly connected peer */
   error = IP_route_add_virtual(peer, 0xFFFFFFFF, address,
      INADDR_ANY, INADDR_ANY, parms->ihandle, NULL);

   /* Add the IP and the LOCALHOST interface into the routing table */
   if (!error) {
      error = IP_route_add_direct(address, 0xFFFFFFFF, parms->ihandle, RTCS_IF_LOCALHOST_PRV);
   } /* Endif */

   if (error) {
      RTCSCMD_complete(parms, error);
      return;
   } /* Endif */

#if RTCSCFG_ENABLE_IGMP
   /* IGMP_ipif_bind failure isn't sufficient reason to abort IPIF_bind */
   IGMP_ipif_bind((IP_IF_PTR)parms->ihandle, address);
#endif

   RTCSCMD_complete(parms, RTCS_OK);

#else

    RTCSCMD_complete(parms, RTCSERR_IP_IS_DISABLED);

#endif /* RTCSCFG_ENABLE_IP4 */
   

} /* Endbody */

#if RTCSCFG_ENABLE_GATEWAYS
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_gate_add
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     not used
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     [IN] the gateway address
*     _ip_address          locmask     [IN] the gateway metric
*     _ip_address          network     [IN] the network address
*     _ip_address          netmask     [IN] the network address mask
*
*  Comments       :
*        Adds a gateway to the routing table.
*
*END*-----------------------------------------------------------------*/

void IPIF_gate_add
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   _ip_address          netmask = parms->netmask;
   uint32_t              error;

   /*
   ** Make sure the netmask is valid.  We use the fact that
   ** (x & x+1) == 0  <=>  x = 2^n-1.
   */
   if (~netmask & (~netmask + 1)) {
      RTCSCMD_complete(parms, RTCSERR_IP_BIND_MASK);
      return;
   } /* Endif */

   /* Start CR 1133 */
   error = IP_route_add_indirect(parms->address, netmask, parms->network, RTF_STATIC, parms->locmask);
   /* End CR 1133 */

   RTCSCMD_complete(parms, error);

#else

    RTCSCMD_complete(parms, RTCSERR_IP_IS_DISABLED);

#endif /* RTCSCFG_ENABLE_IP4 */   

} /* Endbody */

/* Start CR 1016 */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : IPIF_gate_add_redirect
*  Parameters     :
*
*     pointer              mhandle     not used
*     _rtcs_if_handle      ihandle     not used
*     RTCS_IF_STRUCT_PTR   if_ptr      not used
*     _ip_address          address     [IN] the gateway address
*     _ip_address          locmask     [IN] the gateway metric
*     _ip_address          network     [IN] the network address
*     _ip_address          netmask     [IN] the network address mask
*
*  Comments       :
*        Adds a gateway to the routing table with the REDIRECT flag set.
*
*END*-----------------------------------------------------------------*/

void IPIF_gate_add_redirect
   (
      IPIF_PARM_PTR  parms
   )
{ /* Body */

#if RTCSCFG_ENABLE_IP4

   _ip_address          netmask = parms->netmask;
   uint32_t              error;

   /*
   ** Make sure the netmask is valid.  We use the fact that
   ** (x & x+1) == 0  <=>  x = 2^n-1.
   */
   if (~netmask & (~netmask + 1)) {
      RTCSCMD_complete(parms, RTCSERR_IP_BIND_MASK);
      return;
   } /* Endif */

   /* Start CR 1133 */
   error = IP_route_add_indirect(parms->address, netmask, parms->network, RTF_REDIRECT, parms->locmask);
   /* End CR 1133 */

   RTCSCMD_complete(parms, error);

#else

    RTCSCMD_complete(parms, RTCSERR_IP_IS_DISABLED);

#endif /* RTCSCFG_ENABLE_IP4 */    

} /* Endbody */

/* End CR 1016 */
#endif

/* EOF */
