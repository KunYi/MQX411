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
*   This file contains the RTCS shell.
*
*
*END************************************************************************/


#include <mqx.h>
#include <fio.h>
#include <rtcs.h>
#include <ftpc.h>
#include <shell.h>
#include <sh_rtcs.h>
#if SHELLCFG_USES_MFS
#include <sh_mfs.h>
#endif
#include "config.h"

#if ! SHELLCFG_USES_RTCS
#error This application requires SHELLCFG_USES_RTCS defined non-zero in user_config.h. Please recompile libraries with this option if any Ethernet interface is available.
#endif

const SHELL_COMMAND_STRUCT Shell_commands[] = {
    { "arpadd",    Shell_arpadd },
    { "arpdel",    Shell_arpdel },
    { "arpdisp",   Shell_arpdisp },
#if RTCSCFG_ENABLE_NAT
    { "dnat",      Shell_dnat },
#endif
    { "email",     Shell_smtp },
#if RTCSCFG_ENABLE_UDP
    { "exec",      Shell_exec },
#endif
    { "gate",      Shell_gate },
    { "gethbn",    Shell_get_host_by_name },
    { "getrt",     Shell_getroute },
    { "help",      Shell_help },
    { "ipconfig",  Shell_ipconfig },
#if DEMOCFG_USE_WIFI
    { "iwconfig",  Shell_iwconfig },
#endif
#if RTCSCFG_ENABLE_UDP
    { "load",      Shell_load },
#endif
#if RTCSCFG_ENABLE_NAT
    { "natinit",   Shell_natinit },
    { "natinfo",   Shell_natinfo },
#endif
    { "netstat",   Shell_netstat },
#if RTCSCFG_ENABLE_ICMP
    { "ping",      Shell_ping },
#endif
#if RTCSCFG_ENABLE_UDP
    { "sendto",    Shell_sendto },
#endif
#if RTCSCFG_ENABLE_SNMP
    { "snmpd",     Shell_SNMPd },
#endif
    { "telnet",    Shell_Telnet_client },
    { "telnetd",   Shell_Telnetd },
    { "ftp",       Shell_FTP_client },
    { "ftpsrv",    Shell_ftpsrv },
#if RTCSCFG_ENABLE_UDP
    { "tftp",      Shell_TFTP_client },
    { "tftpd",     Shell_TFTPd },
#endif

#if DEMOCFG_ENABLE_PPP
    { "ppp"    ,  Shell_ppp},
#endif

    { "walkrt",    Shell_walkroute },

/* Filesystem commands */
#if SHELLCFG_USES_MFS
#if DEMOCFG_ENABLE_RAMDISK
    { "cd",        Shell_cd },
    { "copy",      Shell_copy },
    { "create",    Shell_create },
    { "del",       Shell_del },
    { "dir",       Shell_dir },
    { "format",    Shell_format },
    { "mkdir",     Shell_mkdir },
    { "pwd",       Shell_pwd },
    { "read",      Shell_read },
    { "ren",       Shell_rename },
    { "rmdir",     Shell_rmdir },
    { "write",     Shell_write },
#endif
#endif
    { "?",         Shell_command_list },
    { NULL,        NULL }
};

const SHELL_COMMAND_STRUCT Telnetd_shell_commands[] = {
    { "exit",      Shell_exit },
    { "gethbn",    Shell_get_host_by_name },
    { "getrt",     Shell_getroute },
    { "help",      Shell_help },
    { "ipconfig",  Shell_ipconfig },
    { "netstat",   Shell_netstat },
    { "pause",     Shell_pause },
#if RTCSCFG_ENABLE_ICMP
    { "ping",      Shell_ping },
#endif
    { "telnet",    Shell_Telnet_client },
#if RTCSCFG_ENABLE_UDP
    { "tftp",      Shell_TFTP_client },
#endif
    { "walkrt",    Shell_walkroute },
    { "?",         Shell_command_list },
    { NULL,        NULL }
};

const FTPc_COMMAND_STRUCT FTPc_commands[] = {
   { "ascii",        FTPc_ascii       },
   { "binary",       FTPc_binary      },
   { "bye",          FTPc_bye         },
   { "cd",           FTPc_cd          },
   { "close",        FTPc_close       },
   { "delete",       FTPc_delete      },
   { "get",          FTPc_get         },
   { "help",         FTPc_help        },
   { "ls",           FTPc_ls          },
   { "mkdir",        FTPc_mkdir       },
   { "open",         FTPc_open        },
   { "pass",         FTPc_pass        },
   { "put",          FTPc_put         },
   { "pwd",          FTPc_pwd         },
   { "remotehelp",   FTPc_remotehelp  },
   { "rename",       FTPc_rename      },
   { "rmdir",        FTPc_rmdir       },
   { "user",         FTPc_user        },
   { NULL,   NULL }
};


/* EOF */
