/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
* @brief The file contains Macro's and structures needed by the Network 
*        Interface for Virtual NIC Application
* 
*END************************************************************************/

#ifndef _VIRTUAL_NIC_ENET_H
#define _VIRTUAL_NIC_ENET_H  1


/******************************************************************************
 * Includes
 *****************************************************************************/
#include "virtual_nic.h"
/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define ENET_DEVICE                 (BSP_DEFAULT_ENET_DEVICE)

#ifndef ENETPROT_IP
    #define ENETPROT_IP                 (0x0800)
#endif  

#ifndef ENETPROT_ARP                
    #define ENETPROT_ARP                (0x0806)
#endif
#define RNDIS_ETHER_HDR_SIZE            (14)
#define REMOTE_NDIS_PACKET_MSG          (0x00000001)
#define RNDIS_USB_OVERHEAD_SIZE         (44)
#define RNDIS_DATA_OFFSET               (36)
#define MAC_ADDR_SIZE                   (6)

// Helpful macros
#define IPBYTES(a)            ((a>>24)&0xFF),((a>>16)&0xFF),((a>> 8)&0xFF),(a&0xFF)
#define IPADDR(a,b,c,d)       (((uint32_t)(a)&0xFF)<<24)|(((uint32_t)(b)&0xFF)<<16)|(((uint32_t)(c)&0xFF)<<8)|((uint32_t)(d)&0xFF)


#ifndef ENET_IPADDR
    #define ENET_IPADDR  IPADDR(169,254,3,3)/* Default IP Address */
#endif

#ifndef ENET_IPMASK
    #define ENET_IPMASK  IPADDR(255,255,0,0) 
#endif

/*****************************************************************************
 * Global variables
 *****************************************************************************/

/******************************************************************************
 * Types
 *****************************************************************************/

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
extern uint32_t VNIC_FEC_Init(CDC_CONFIG_STRUCT_PTR cdc_config_ptr);
extern uint32_t VNIC_FEC_Send(void *arg, uint8_t *ethernet_frame_ptr, uint32_t payload_size);

#endif 


/* EOF */
