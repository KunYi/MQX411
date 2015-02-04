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
*   ARP server initialization.
*   Limitations:  Although ARP can be used over any multiple-access
*   medium, this implementation will work only over an
*   Ethernet link layer.
*
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_IP4

#include "rtcs_prv.h"
#include "tcpip.h"
#include "arp.h"

void   *ARP_freelist;


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ARP_init
*  Returned Value : RTCS_OK or error code
*  Comments       :
*        Initializes the ARP Server.
*
*END*-----------------------------------------------------------------*/

uint32_t ARP_init
   (
      void
   )
{ /* Body */

   ARP_freelist = NULL;
   return RTCS_OK;

} /* Endbody */

#endif /* RTCSCFG_ENABLE_IP4 */

/* EOF */
