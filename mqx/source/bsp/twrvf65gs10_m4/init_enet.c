/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains the function that reads the timer and returns
*   the number of nanoseconds elapsed since the last interrupt.
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "bsp_prv.h"
#include "enet.h"
#include "enetprv.h"
#include <macnet_prv.h>          
#include "phy_dp83xxx.h"



const MACNET_INIT_STRUCT MACNET_device[MACNET_DEVICE_COUNT] =  {
    { BSP_SYSTEM_CLOCK, BSP_MACNET0_INT_RX_LEVEL, 0, BSP_MACNET0_INT_TX_LEVEL, 0 },
    { BSP_SYSTEM_CLOCK, BSP_MACNET1_INT_RX_LEVEL, 0, BSP_MACNET1_INT_TX_LEVEL, 0 }
};


const ENET_IF_STRUCT ENET_0 = {
    &MACNET_IF,
    &phy_dp83xxx_IF,
    MACNET_DEVICE_0,
    MACNET_DEVICE_0,
    BSP_ENET0_PHY_ADDR,
    BSP_ENET0_PHY_MII_SPEED
};

const ENET_IF_STRUCT ENET_1 = {
    &MACNET_IF,
    &phy_dp83xxx_IF,
    MACNET_DEVICE_1,
    MACNET_DEVICE_0,
    BSP_ENET1_PHY_ADDR,
    BSP_ENET1_PHY_MII_SPEED
};

const ENET_PARAM_STRUCT ENET_default_params[BSP_ENET_DEVICE_COUNT] = {
    {
        &ENET_0,
        Auto_Negotiate,
        ENET_OPTION_RMII | ENET_OPTION_PTP_MASTER_CLK
    #if BSPCFG_ENET_HW_TX_IP_CHECKSUM 
        | ENET_OPTION_HW_TX_IP_CHECKSUM
    #endif
    #if BSPCFG_ENET_HW_TX_PROTOCOL_CHECKSUM
        | ENET_OPTION_HW_TX_PROTOCOL_CHECKSUM
    #endif
    #if BSPCFG_ENET_HW_RX_IP_CHECKSUM 
        | ENET_OPTION_HW_RX_IP_CHECKSUM
    #endif
    #if BSPCFG_ENET_HW_RX_PROTOCOL_CHECKSUM
        | ENET_OPTION_HW_RX_PROTOCOL_CHECKSUM
    #endif
    #if BSPCFG_ENET_HW_RX_MAC_ERR 
        | ENET_OPTION_HW_RX_MAC_ERR
    #endif
        , 
        BSPCFG_TX_RING_LEN,   // # tx ring entries
        BSPCFG_TX_RING_LEN,   // # large tx packets
        ENET_FRAMESIZE,       // tx packet size
     
        BSPCFG_RX_RING_LEN,   // # rx ring entries
        BSPCFG_RX_RING_LEN,   // # normal rx packets - must be >= rx ring entries
        ENET_FRAMESIZE,       // ENET_FRAMESIZE,   // rx packet size
        BSPCFG_RX_RING_LEN,   // # rx PCBs - should be >= large rx packets.
     
        0,     
        0     
    }/*,
    {
        &ENET_1,
        Auto_Negotiate,
        ENET_OPTION_RMII
    #if BSPCFG_ENET_HW_TX_IP_CHECKSUM 
        | ENET_OPTION_HW_TX_IP_CHECKSUM
    #endif
    #if BSPCFG_ENET_HW_TX_PROTOCOL_CHECKSUM
        | ENET_OPTION_HW_TX_PROTOCOL_CHECKSUM
    #endif
    #if BSPCFG_ENET_HW_RX_IP_CHECKSUM 
        | ENET_OPTION_HW_RX_IP_CHECKSUM
    #endif
    #if BSPCFG_ENET_HW_RX_PROTOCOL_CHECKSUM
        | ENET_OPTION_HW_RX_PROTOCOL_CHECKSUM
    #endif
    #if BSPCFG_ENET_HW_RX_MAC_ERR 
        | ENET_OPTION_HW_RX_MAC_ERR
    #endif
        , 
        BSPCFG_TX_RING_LEN,   // # tx ring entries
        BSPCFG_TX_RING_LEN,   // # large tx packets
        ENET_FRAMESIZE,       // tx packet size
     
        BSPCFG_RX_RING_LEN,   // # rx ring entries
        BSPCFG_RX_RING_LEN,   // # normal rx packets - must be >= rx ring entries
        ENET_FRAMESIZE,       // ENET_FRAMESIZE,   // rx packet size
        BSPCFG_RX_RING_LEN,   // # rx PCBs - should be >= large rx packets.
     
        0,     
        0     
    }*/
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_mac_address
* Returned Value   : uint32_t
* Comments         :
*    This function returns the mac address associated with the specified device
*
* If the MAC address is stored in NV-storage, this fuction should read the
* MAC address from NV-storage and set it.
*
* If the MAC address is generated from a device serial number and an OUI, the
* serial number should be passed in, and the MAC address should be constructed
*
*END*----------------------------------------------------------------------*/

const _enet_address _enet_oui = BSP_DEFAULT_ENET_OUI;

bool _bsp_get_mac_address
   (
      uint32_t        device,
      uint32_t        value,
      _enet_address  address
   )
{ /* Body */
   if (device >= BSP_ENET_DEVICE_COUNT) 
      return FALSE;

   address[0] = _enet_oui[0];
   address[1] = _enet_oui[1];
   address[2] = _enet_oui[2];
   address[3] = (value & 0xFF0000) >> 16;
   address[4] = (value & 0xFF00) >> 8;
   address[5] = (value & 0xFF);

   return TRUE;

} /* Endbody */


/* EOF */
