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
*   
*
*
*END************************************************************************/

#include "hvac.h"
#include "hvac_shell_commands.h"

const SHELL_COMMAND_STRUCT Shell_commands[] = {
#if DEMOCFG_ENABLE_USB_FILESYSTEM 
   { "cd",        Shell_cd },      
   { "copy",      Shell_copy },     
   { "del",       Shell_del },       
   { "dir",       Shell_dir },      
   { "log",       Shell_log },
   { "mkdir",     Shell_mkdir },     
   { "pwd",       Shell_pwd },       
   { "read",      Shell_read },      
   { "ren",       Shell_rename },    
   { "rmdir",     Shell_rmdir },
   { "type",      Shell_type },
   { "write",     Shell_write }, 
#endif
   { "exit",      Shell_exit },      
   { "fan",       Shell_fan },
   { "help",      Shell_help }, 
   { "hvac",      Shell_hvac },
   { "info",      Shell_info },
   { "scale",     Shell_scale },
   { "temp",      Shell_temp },       

#if DEMOCFG_ENABLE_RTCS
   { "netstat",   Shell_netstat },  
   { "ipconfig",  Shell_ipconfig },
#if DEMOCFG_USE_WIFI   
   { "iwconfig",  Shell_iwconfig },
#endif      
#if RTCSCFG_ENABLE_ICMP
   { "ping",      Shell_ping },      
#endif   
#endif
   { "?",         Shell_command_list },     
   { NULL,        NULL } 
};

const SHELL_COMMAND_STRUCT Telnet_commands[] = {
   { "exit",      Shell_exit },      
   { "fan",       Shell_fan },
   { "help",      Shell_help }, 
   { "hvac",      Shell_hvac },
   { "info",      Shell_info },
#if DEMOCFG_ENABLE_USB_FILESYSTEM
   { "log",       Shell_log },
#endif

#if DEMOCFG_ENABLE_RTCS
#if RTCSCFG_ENABLE_ICMP
   { "ping",      Shell_ping },      
#endif   
#endif   

   { "scale",     Shell_scale },
   { "temp",      Shell_temp },
   { "?",         Shell_command_list },     
   
   { NULL,        NULL } 
};

/* EOF */
