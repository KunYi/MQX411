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
*   Example HTTP server using RTCS Library.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <rtcs.h>
#include <ipcfg.h>

#include "httpsrv.h"
#include "tfs.h"
#include "config.h"

#include <shell.h>
#include <sh_rtcs.h>

#define SHELL_TASK              2

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#ifndef BSP_ENET_DEVICE_COUNT
#error Ethernet devices not defined!
#endif

#if BSP_ENET_DEVICE_COUNT == 0
#error No ethernet devices defined!
#endif
/* add SHELL */

const SHELL_COMMAND_STRUCT Shell_commands[] = {
   /* RTCS commands */ 

   { "help",      Shell_help }, 
   { "ipconfig",  Shell_ipconfig },   
#if RTCSCFG_ENABLE_ICMP      
   { "ping",      Shell_ping },      
#endif
   { "?",         Shell_command_list },     
   { NULL,        NULL } 
};

HTTPSRV_AUTH_USER_STRUCT users[] = {
    {"developer", "freescale"},
    { NULL, NULL} /* Array terminator */
};

HTTPSRV_AUTH_REALM_STRUCT auth_realms[] = {
    { "Please use uid:developer pass:freescale to login", "rtc.html", HTTPSRV_AUTH_BASIC, users},
    { NULL, NULL, HTTPSRV_AUTH_INVALID, NULL} /* Array terminator */
};

void main_task(uint32_t);
void shell_task(uint32_t);

/*
** MQX initialization information
*/

const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   /* Task Index,   Function,   Stack,  Priority,   Name,    Attributes,          Param, Time Slice */
    { 1,            main_task,  2000,    8,          "Main",         MQX_AUTO_START_TASK, 0,     0 },
    { SHELL_TASK,   shell_task, 2000,    9,          "Shell_task",   0, 0,     0 },
    { 0 }
};

void shell_task(uint32_t data)
{
    for (;;)  
    {
        /* Run the shell */
        Shell(Shell_commands, NULL);
        printf("Shell exited, restarting...\n");
    }
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : main_task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void main_task(uint32_t temp)
{
    int32_t                                 error;
    uint32_t                                server[BSP_ENET_DEVICE_COUNT];
    extern const HTTPSRV_CGI_LINK_STRUCT    cgi_lnk_tbl[];
    extern const TFS_DIR_ENTRY              tfs_data[];
    HTTPSRV_PARAM_STRUCT                    params[BSP_ENET_DEVICE_COUNT];
    uint32_t                                ip_addr[BSP_ENET_DEVICE_COUNT];    
    uint32_t                                i = 0;
    char*                                   indexes[BSP_ENET_DEVICE_COUNT];
    uint8_t                                 n_devices = BSP_ENET_DEVICE_COUNT;
    #if !HTTP_USE_ONE_SERVER
    uint8_t n_servers = BSP_ENET_DEVICE_COUNT;
    #else
    uint8_t n_servers = 1;
    #endif

    _int_install_unexpected_isr();

    /* Init RTCS */
    _RTCSPCB_init = 4;
    _RTCSPCB_grow = 2;
    _RTCSPCB_max = 8;
    
    _RTCSTASK_stacksize = 4500;
    
    error = RTCS_create();
    if (error != RTCS_OK) 
    {
        printf("RTCS failed to initialize, error = 0x%X\n", error);
        _task_block();
    }

    _IP_forward = TRUE;

    /* Bind IP address for each interface */
    for (i = 0; (i < n_devices) && (n_devices != 0); i++)
    {
        _enet_address           address;
        uint32_t                 phy_addr;
        #if RTCSCFG_ENABLE_IP4
        IPCFG_IP_ADDRESS_DATA	ip_data; 
        #endif
        phy_addr = i;
        ip_addr[i] = IPADDR(A,B,C+i,D);

        ENET_get_mac_address(phy_addr, ip_addr[i], address);
        
        /* Init ENET device */
        error = ipcfg_init_device (phy_addr, address);
        if (error != RTCS_OK) 
        {
            printf("IPCFG: Device n.%d init failed. Error = 0x%X\n", i, error);
            _task_set_error(MQX_OK);
            n_devices--;
            i--;
            continue;
        }

        #if RTCSCFG_ENABLE_IP4
        ip_data.ip      = ip_addr[i];
        ip_data.mask    = ENET_IPMASK;
        ip_data.gateway = 0;
        /* Bind IPv4 address */
        error = ipcfg_bind_staticip (phy_addr, &ip_data);
        if (error != RTCS_OK) 
        {
            printf("\nIPCFG: Failed to bind IP address. Error = 0x%X", error);
            _task_block();
        }
        #endif
       
        indexes[i] = (char*) _mem_alloc_zero(sizeof("\\index_x.html"));
        if (indexes[i] == NULL)
        {
            printf("\n Failed to allocate memory.");
            _task_block();
        }
    }  

    /* Install trivial file system. HTTP server pages are stored there. */
    error = _io_tfs_install("tfs:", tfs_data);
    
    /* Start HTTP server on each interface */
    for (i = 0; i < n_servers; i++)
    {
        _mem_zero(&params[i], sizeof(HTTPSRV_PARAM_STRUCT));
        params[i].af = HTTP_INET_AF;  //IPv4, IPv6 or from config.h

        #if RTCSCFG_ENABLE_IP6
        /* Set interface number here. Zero is any. */
        params[i].ipv6_scope_id = HTTP_SCOPE_ID;
        #endif

        sprintf(indexes[i], "\\index.html");
        params[i].root_dir = "tfs:";
        params[i].index_page = indexes[i];
        params[i].auth_table = auth_realms;
        printf("Starting http server No.%d on IP", i);
        /*
        ** If there is only one server listen on any IP address 
        ** so address can change in runtime (DHCP etc.).
        ** Otherwise we will use static IP for server.
        */
        if (HTTP_INET_AF & AF_INET)
        {
            #if RTCSCFG_ENABLE_IP4
            if ((i == 0) && (n_servers == 1))
            {
                params[i].ipv4_address.s_addr = INADDR_ANY;
            }
            else
            {
                params[i].ipv4_address.s_addr = ip_addr[i];
            }
            /* Print active IPv4 address */
            printf(" %d.%d.%d.%d", IPBYTES(ip_addr[i]));
            #endif
        }
        if (HTTP_INET_AF & AF_INET6)
        {
            #if RTCSCFG_ENABLE_IP6 
            IPCFG6_GET_ADDR_DATA data;
            char prn_addr6[RTCS_IP_ADDR_STR_SIZE];
            
            ipcfg6_get_addr(0, 0, &data);

            if(inet_ntop(AF_INET6, &(data.ip_addr), prn_addr6, sizeof(prn_addr6)))
            {
                /* Print active IPv6 address */
                printf("%s %s", (HTTP_INET_AF & AF_INET) ? " and" : "", prn_addr6);
            }
            #endif
        }
        /*
        ** Maximum default number of available sockets in RTCS is six.
        ** Maximum of two are used for HTTP server as listening sockets (one for IPv4 and one for IPv6).
        ** So we take two of sockets and use them for sessions.
        */
        params[i].max_ses = 2;
        params[i].cgi_lnk_tbl = (HTTPSRV_CGI_LINK_STRUCT*) cgi_lnk_tbl;

        /* There are a lot of static data in CGIs so we have to set large enough stack for script handler */
        params[i].script_stack = 3000;
        server[i] = HTTPSRV_init(&params[i]);
        
        printf("...");
        if (server[i] == (uint32_t) RTCS_ERROR)
        {
            printf("[FAIL]\n");
        }
        else
        {
            printf("[OK]\n");
        }
    }
    /* HTTP server initialization is done so we can start shell */
    _task_create(0, SHELL_TASK, 0);
    _task_block();
}
