/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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
*  This file contains definitions that belongs to the Natioanl Semiconductor
*  PHY chips DP83848, DP83849, DP83640.
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "bsp_prv.h"
#include "enet.h"
#include "enetprv.h"
#include "phy_lan8720.h"

static bool phy_lan8720_discover_addr(ENET_CONTEXT_STRUCT_PTR enet_ptr);
static bool phy_lan8720_init(ENET_CONTEXT_STRUCT_PTR enet_ptr);
static uint32_t phy_lan8720_get_speed(ENET_CONTEXT_STRUCT_PTR enet_ptr);
static bool phy_lan8720_get_link_status(ENET_CONTEXT_STRUCT_PTR enet_ptr);

const ENET_PHY_IF_STRUCT phy_lan8720_IF = {
  phy_lan8720_discover_addr,
  phy_lan8720_init,
  phy_lan8720_get_speed,
  phy_lan8720_get_link_status
};
  
  

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : phy_lan8720_discover_addr
*  Returned Value : none
*  Comments       :
*    Scan possible PHY addresses looking for a valid device
*
*END*-----------------------------------------------------------------*/

static bool phy_lan8720_discover_addr
   (
       ENET_CONTEXT_STRUCT_PTR     enet_ptr
   )
{ 
   uint32_t              i;
   uint32_t              id;

   for (i = 0; i < 32; i++) {
      id = 0;
      
      // Try an address - note this needs to be set in order for *PHY_READ to use it.
      enet_ptr->PHY_ADDRESS = i;
      if ((*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, LAN8720_REG_IDR1, &id, MII_TIMEOUT)) 
      {
          if ((id != 0) && (id != 0xffff)) 
          {
            return TRUE;
          }
      } 
   } 
   
   return FALSE; 
} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : phy_lan8720_init
*  Returned Value : bool
*  Comments       :
*    Wait for PHY to automatically negotiate its configuration
*
*END*-----------------------------------------------------------------*/

static bool phy_lan8720_init
   (
       ENET_CONTEXT_STRUCT_PTR     enet_ptr
   )
{ 
   uint32_t              phy_status=0;

   if (enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_PHY_LOOPBACK) {
      if ((*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, LAN8720_REG_BMCR, &phy_status, MII_TIMEOUT)) {
         phy_status |= LAN8720_REG_BMCR_LOOPBACK;
         (*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_WRITE)(enet_ptr, LAN8720_REG_BMCR, phy_status, MII_TIMEOUT);
         return TRUE;
      }
   } else {
     if ((*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, LAN8720_REG_BMSR, &phy_status, MII_TIMEOUT)) {
        if (phy_status & LAN8720_REG_BMSR_AUTO_NEG_ABILITY) { 
           // Has auto-negotiate ability
           int i;
           for (i = 0; i < 3 * BSP_ALARM_FREQUENCY; i++) {
              if ((*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, LAN8720_REG_BMSR, &phy_status, MII_TIMEOUT)) 
                 if ((phy_status & LAN8720_REG_BMSR_AUTO_NEG_COMPLETE) != 0) 
                    return TRUE;
              _time_delay(BSP_ALARM_RESOLUTION);
           }  
        }
        return TRUE;
      }
   }  
   return FALSE;
}  


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : phy_lan8720_get_speed
*  Returned Value : uint32_t - connection speed
*  Comments       :
*    Determine if connection speed is 10 or 100 Mbit
*
*END*-----------------------------------------------------------------*/

static uint32_t phy_lan8720_get_speed
   (
       ENET_CONTEXT_STRUCT_PTR     enet_ptr
   )
{ 
   uint32_t              speed_status;

   if ((*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, LAN8720_REG_PHY_SCSR, &speed_status, MII_TIMEOUT)) {
      return ((speed_status & LAN8720_REG_PHY_SCSR_SPEED_MASK) == LAN8720_REG_PHY_SCSR_SPEED_100) ? 100 : 10;
   }
   return 0;
} 
   
  
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : phy_lan8720_get_link_status
*  Returned Value : TRUE if link active, FALSE otherwise
*  Comments       : 
*    Get actual link status.
*
*END*-----------------------------------------------------------------*/

static bool phy_lan8720_get_link_status
   (
       ENET_CONTEXT_STRUCT_PTR     enet_ptr
   )
{ 
   uint32_t data;
   bool res = FALSE;

   /* Some PHY (e.g.DP8340) returns "unconnected" and than "connected" state
   *  just to show that was transition event from one state to another.
   *  As we need only curent state,  read 2 times and return 
   *  the current/latest state. */
   if ((*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, LAN8720_REG_BMSR, &data, MII_TIMEOUT)) {
      if ((*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, LAN8720_REG_BMSR, &data, MII_TIMEOUT))
      {
         res = ((data & LAN8720_REG_BMSR_LINK_STATUS) != 0) ? TRUE : FALSE;
      }
   }
   return res;
}
