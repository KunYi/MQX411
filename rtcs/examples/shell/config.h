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
*   The Common Configurable information for the RTCS 
*   Shell examples.
*
*
*END************************************************************************/


 
/* RAMDISK can not be used from MRAM targets !!!  Whole MRAM is used for RamDisk. */ 
#define DEMOCFG_ENABLE_RAMDISK     1
/* Enable iwconfig commad */
#define DEMOCFG_USE_WIFI           0
/* Enable PPP */
#define DEMOCFG_ENABLE_PPP         0

#define DEMOCFG_ENABLE_MRAM_RAMDISK 0

#define ENET_DEVICE BSP_DEFAULT_ENET_DEVICE

/* PPP device must be set manually and 
** must be different from the default IO channel (BSP_DEFAULT_IO_CHANNEL) 
*/

#if DEMOCFG_ENABLE_PPP

#if   defined BSP_TWR_K60N512
  #define PPP_DEVICE        "ittyd:" 
#elif defined BSP_M52259EVB
  #define PPP_DEVICE        "ittyb:" 
#else
   #error PPP_DEVICE must be defined! Define it in config.h.
#endif

#endif


#ifdef ENET_DEVICE
/*
** The target's IP address and netmask
*/

#ifndef ENET_IPADDR
    #define ENET_IPADDR  IPADDR(192,168,1,202) 
#endif

#ifndef ENET_IPMASK
    #define ENET_IPMASK  IPADDR(255,255,255,0) 
#endif

#endif  /* ENET_DEVICE */


#if DEMOCFG_USE_WIFI
   #include "iwcfg.h"
   
   #define DEMOCFG_SSID            "NGZG"
   //Possible Values ENET_MEDIACTL_MODE_INFRA or ENET_MEDIACTL_MODE_ADHOC
   #define DEMOCFG_NW_MODE         ENET_MEDIACTL_MODE_INFRA 
   /* Posible values.
    * 1. ENET_MEDIACTL_SECURITY_TYPE_WPA
    * 2.ENET_MEDIACTL_SECURITY_TYPE_WPA2
    * 3.ENET_MEDIACTL_SECURITY_TYPE_NONE
    * 4.ENET_MEDIACTL_SECURITY_TYPE_WEP
    */   
   #define DEMOCFG_SECURITY        ENET_MEDIACTL_SECURITY_TYPE_NONE 
   #define DEMOCFG_PASSPHRASE      NULL
   #define DEMOCFG_WEP_KEY         "ABCDE"
   #define DEMOCFG_WEP_KEY_INDEX   1 //can be 1,2,3, or 4
   #define DEMOCFG_DEFAULT_DEVICE  1
#endif

#ifdef PPP_DEVICE
/*
** Define the local and remote IP addresses for the PPP link
** These are ignored if PPP_DEVICE is not #define'd
*/
#define PPP_LOCADDR     IPADDR(192,168,1,203)
#define PPP_PEERADDR    IPADDR(192,168,1,204)

/*
** Define a default gateway
*/
#define GATE_ADDR       IPADDR(0,0,0,0)

#endif /* PPP_DEVICE */

extern int32_t shell_ppp_start(int32_t argc, char *argv[] );
extern int32_t shell_ppp_stop(int32_t argc, char *argv[] );




/* EOF */
