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
*   Configurable information for the RTCS examples.
*
*
*END************************************************************************/
#ifndef _config_h_
#define _config_h_

/* IP address macros */
#define A 192
#define B 168
#define C 1
#define D 202

/* Use this define to tell example if only one server should be used for all interfaces */
#define HTTP_USE_ONE_SERVER 1

#ifndef ENET_IPMASK
    #define ENET_IPMASK  IPADDR(255,255,255,0) 
#endif

#if RTCSCFG_ENABLE_IP6   
    #if RTCSCFG_ENABLE_IP4   
        /* 
        **  HTTP_INET_AF is AF_INET+AF_INET for http support IPv4+IPv6.
        **  HTTP_INET_AF is AF_INET  for http support IPv4 only.
        **  HTTP_INET_AF is AF_INET6 for http support IPv6 only.
        */
        #define HTTP_INET_AF    (AF_INET | AF_INET6)
    #else
        #define HTTP_INET_AF    AF_INET6        
    #endif
#else
        #define HTTP_INET_AF    AF_INET         
#endif
    
    //#define HTTP_INET_AF           AF_INET
    #define HTTP_SCOPE_ID          0 /* For any IF. */


#endif // _config_h_
