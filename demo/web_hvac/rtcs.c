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
*   Example of HVAC using RTCS.
*
*
*END************************************************************************/

#include "hvac.h"
#include "tfs.h"

#if DEMOCFG_USE_WIFI
#include "string.h"
#endif

#if DEMOCFG_ENABLE_RTCS

#include  <ipcfg.h>
#include "httpsrv.h"

#if DEMOCFG_ENABLE_WEBSERVER
HTTPSRV_ALIAS http_aliases[] = {
    {"/usb/", "c:\\"},
    {NULL, NULL}
    };
#endif

#if DEMOCFG_ENABLE_FTP_SERVER
static const FTPSRV_AUTH_STRUCT ftpsrv_users[] =
{
    {"developer", "freescale", NULL},
    {NULL, NULL, NULL}
};
#endif

#if (DEMOCFG_ENABLE_FTP_SERVER + DEMOCFG_ENABLE_TELNET_SERVER + DEMOCFG_ENABLE_WEBSERVER) < 1
	#warning Please enable one of the network services. The restriction is only for RAM size limited devices.
#endif

extern const SHELL_COMMAND_STRUCT Telnet_commands[];

static void Telnetd_shell_fn (void *dummy) 
{  
	Shell(Telnet_commands,NULL);
}

const RTCS_TASK Telnetd_shell_template = {"Telnet_shell", 8, 2000, Telnetd_shell_fn, NULL};

void HVAC_initialize_networking(void) 
{
	int32_t                  error;
	IPCFG_IP_ADDRESS_DATA   ip_data;
	_enet_address           enet_address;

#if DEMOCFG_USE_POOLS && defined(DEMOCFG_RTCS_POOL_ADDR) && defined(DEMOCFG_RTCS_POOL_SIZE)
    /* use external RAM for RTCS buffers */
    _RTCS_mem_pool = _mem_create_pool((void *)DEMOCFG_RTCS_POOL_ADDR, DEMOCFG_RTCS_POOL_SIZE);
#endif

    /* runtime RTCS configuration for devices with small RAM, for others the default BSP setting is used */
    _RTCSPCB_init = 4;
    _RTCSPCB_grow = 2;
    _RTCSPCB_max = 20;
    _RTCS_msgpool_init = 4;
    _RTCS_msgpool_grow = 2;
    _RTCS_msgpool_max  = 20;
    _RTCS_socket_part_init = 4;
    _RTCS_socket_part_grow = 2;
    _RTCS_socket_part_max  = 20;

    error = RTCS_create();
    
#if RTCSCFG_ENABLE_LWDNS
    LWDNS_server_ipaddr = ENET_IPGATEWAY;
#endif
    
    ip_data.ip = ENET_IPADDR;
    ip_data.mask = ENET_IPMASK;
    ip_data.gateway = ENET_IPGATEWAY;
    
    ENET_get_mac_address (DEMOCFG_DEFAULT_DEVICE, ENET_IPADDR, enet_address);
    error = ipcfg_init_device (DEMOCFG_DEFAULT_DEVICE, enet_address);

#if DEMOCFG_USE_WIFI
    iwcfg_set_essid (DEMOCFG_DEFAULT_DEVICE,DEMOCFG_SSID);
    
    if ((strcmp(DEMOCFG_SECURITY,"wpa") == 0) || strcmp(DEMOCFG_SECURITY,"wpa2") == 0)
    {
    	iwcfg_set_passphrase (DEMOCFG_DEFAULT_DEVICE,DEMOCFG_PASSPHRASE);
    }
    
    if (strcmp(DEMOCFG_SECURITY,"wep") == 0)
    {
    	iwcfg_set_wep_key (DEMOCFG_DEFAULT_DEVICE,DEMOCFG_WEP_KEY,strlen(DEMOCFG_WEP_KEY),DEMOCFG_WEP_KEY_INDEX);
    }
    
    iwcfg_set_sec_type (DEMOCFG_DEFAULT_DEVICE,DEMOCFG_SECURITY);
    iwcfg_set_mode (DEMOCFG_DEFAULT_DEVICE,DEMOCFG_NW_MODE);
#endif  

    error = ipcfg_bind_staticip (DEMOCFG_DEFAULT_DEVICE, &ip_data);

#if DEMOCFG_ENABLE_WEBSERVER
    {
        uint32_t server;
        extern const HTTPSRV_CGI_LINK_STRUCT cgi_lnk_tbl[];
        extern const HTTPSRV_SSI_LINK_STRUCT fn_lnk_tbl[];
        extern const TFS_DIR_ENTRY tfs_data[];
        HTTPSRV_PARAM_STRUCT params;
        
        error = _io_tfs_install("tfs:", tfs_data);
        if (error) printf("\nTFS install returned: %08x\n", error);

        /* Setup webserver parameters */
        _mem_zero(&params, sizeof(HTTPSRV_PARAM_STRUCT));
    #if RTCSCFG_ENABLE_IP4 
        params.af |= AF_INET;
    #endif
    #if RTCSCFG_ENABLE_IP6 
        params.af |= AF_INET6;
    #endif
        params.root_dir = "tfs:";
        params.alias_tbl = (HTTPSRV_ALIAS*)http_aliases;
        params.index_page = "\\mqx.shtml";
        params.cgi_lnk_tbl = (HTTPSRV_CGI_LINK_STRUCT*)cgi_lnk_tbl;
        params.ssi_lnk_tbl = (HTTPSRV_SSI_LINK_STRUCT*)fn_lnk_tbl;
        params.script_stack = 2500;

        server = HTTPSRV_init(&params);
        if(!server)
        {
            printf("Error: HTTP server init error.\n");
        }
    }
#endif
#if DEMOCFG_ENABLE_FTP_SERVER
    {
        FTPSRV_PARAM_STRUCT  params = {0};
        uint32_t             ftpsrv_handle;
        #if RTCSCFG_ENABLE_IP4
        params.af |= AF_INET;
        #endif
        #if RTCSCFG_ENABLE_IP6
        params.af |= AF_INET6;
        #endif
        params.auth_table = (FTPSRV_AUTH_STRUCT*) ftpsrv_users;
        params.root_dir = "c:";

        ftpsrv_handle = FTPSRV_init(&params);
        if (ftpsrv_handle != 0)
        {
            printf("FTP Server Started. Root directory is set to \"%s\", login: \"%s\", password: \"%s\".\n", 
                params.root_dir,
                ftpsrv_users[0].uid, 
                ftpsrv_users[0].pass);
        }
        else
        {
            printf("Failed to start FTP server.\n");
        }
    }
#endif

#if DEMOCFG_ENABLE_TELNET_SERVER
	TELNETSRV_init("Telnet_server", 7, 2000, (RTCS_TASK_PTR) &Telnetd_shell_template );
#endif

#if DEMOCFG_ENABLE_KLOG && MQX_KERNEL_LOGGING
	RTCSLOG_enable(RTCSLOG_TYPE_FNENTRY);
	RTCSLOG_enable(RTCSLOG_TYPE_PCB);
#endif

}

#endif /* DEMOCFG_ENABLE_RTCS */
