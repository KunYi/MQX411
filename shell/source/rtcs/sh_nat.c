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
*   This file contains the RTCS shell.
*
*
*END************************************************************************/
#include <ctype.h>
#include <string.h>
#include <mqx.h>
#include "shell.h"
#if SHELLCFG_USES_RTCS

#include <rtcs.h>
#include "sh_rtcs.h"
#include <fio.h>


#include <nat.h>
#include <rtcstime.h>
#include <nat_session.h>

#if RTCSCFG_ENABLE_NAT

static char *IP_Prot_str(uint32_t prot)
{
   switch (prot) {
      case IPPROTO_ICMP:   return "ICMP";
      case IPPROTO_IGMP:   return "IGMP";
      case IPPROTO_IP:     return "IP";
      case IPPROTO_TCP:    return "TCP";
      case IPPROTO_UDP:    return "UDP";
      default:             return "???";
   }
}


static char *NAT_err_str(uint32_t err)
{
   switch (err) {
      case  RTCSERR_NAT_NO_SESSION:                return "No session";
      case  RTCSERR_NAT_UNSUPPORTED_PROTOCOL:      return "Unsupported protocol";
      case  RTCSERR_NAT_INITIALIZED:               return "NAT initialized";
      case  RTCSERR_NAT_UNEXPECTED:                return "Unexpected error";
      case  RTCSERR_NAT_NOT_INITIALIZED:           return "NAT not initialized";
      case  RTCSERR_NAT_INVALID_RULE:              return "Invalid rule";
      case  RTCSERR_OUT_OF_MEMORY:                 return "No memory";
      case  RTCSERR_NAT_INVALID_PRIVATE_NETWORK:   return "Invalid private network";
      case  RTCSERR_NAT_MAX_PRIVATE_NETWORKS:      return "Max private networks";
      case  RTCSERR_NAT_DUPLICATE_PRIORITY:        return "Duplicate priority";
      case  RTCSERR_NAT_END_OF_RULES:              return "End of rules";
      case  RTCSERR_NAT_INVALID_PRIVATE_ADDRESS:   return "Invalid private address";
      default:                                     return "Unknown error";
   }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_natinit
*  Returned Value:  none
*  Comments  :  SHELL utility to initialize NAT
*END*-----------------------------------------------------------------*/

int32_t  Shell_natinit(int32_t argc, char *argv[] )
{
   uint32_t              error;
   bool              print_usage, shorthelp = FALSE;
   int32_t               return_code = SHELL_EXIT_SUCCESS;
   _ip_address          addr, mask;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc == 3)  {
         if (! Shell_parse_ip_address( argv[1], &addr  ))  {
            printf("Error, invalid ip address\n");
            return_code = SHELL_EXIT_ERROR;
         } else if (! Shell_parse_netmask( argv[2], &mask  ))  {
            printf("Error, invalid ip address\n");
            return_code = SHELL_EXIT_ERROR;
         } else {
            error = NAT_init( addr, mask );
            if (error != RTCS_OK) {
               printf("Error initializing NAT: %s\n",NAT_err_str(error));
               return_code = SHELL_EXIT_ERROR;
            } else {
               printf("NAT Initialized\n");
            }
         }
      } else {
         print_usage = TRUE;
         return_code = SHELL_EXIT_ERROR;
      }
   }

   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <ipaddress> <netmask>\n", argv[0]);
      } else  {
         printf("Usage: %s <ipaddress> <netmask>\n",argv[0]);
      }
   }
   return return_code;
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_natinfo
*  Returned Value:  none
*  Comments  :  SHELL utility to enable/disable NAT
*END*-----------------------------------------------------------------*/

#define NATINFO_STATS      0x01
#define NATINFO_RULES      0x02
#define NATINFO_NETWORKS   0x04
#define NATINFO_SESSIONS   0x08
#define NATINFO_ALL (NATINFO_STATS|NATINFO_RULES|NATINFO_NETWORKS|NATINFO_SESSIONS)

int32_t  Shell_natinfo(int32_t argc, char *argv[] )
{
   bool                 print_usage, shorthelp = FALSE;
   int32_t                  i  , return_code = SHELL_EXIT_SUCCESS;
   uint32_t                 natinfo = 0, num_nets;
   NAT_STATS_PTR           nat_stats_ptr = NULL;
   NAT_NETWORK_STRUCT_PTR  nat_network_ptr = NULL;
   bool                 continuous = FALSE;
   uint32_t                 priority;        
   uint16_t                 start_port;  
   uint16_t                 end_port;        
   _ip_address             target_ip;     
   uint16_t                 target_port; 
   uint16_t                 protocol;
   uint32_t                 result;

   NAT_SESSION_STRUCT_PTR  session_ptr;

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );
      
   if (!print_usage)  {

      if (argc==1) {
         natinfo = NATINFO_ALL;   
      } else {
         for (i=1;i<argc;i++) {
            if (strcmp(argv[i],"stats")==0) {
               natinfo |= NATINFO_STATS;
            } else if (strcmp(argv[i],"rules")==0) {
               natinfo |= NATINFO_RULES;
            } else if (strcmp(argv[i],"networks")==0) {
               natinfo |= NATINFO_NETWORKS;
            } else if (strcmp(argv[i],"all")==0) {
               natinfo |= NATINFO_ALL;
            } else if (strcmp(argv[i],"loop")==0) {
               continuous = TRUE;
            } else {
                print_usage = TRUE;
                return_code = SHELL_EXIT_ERROR;
            }
         }
      }   
      if (return_code == SHELL_EXIT_SUCCESS) {
         do {
            if (natinfo & NATINFO_STATS) {
               nat_stats_ptr= NAT_stats();
               if (nat_stats_ptr == NULL) {
                  printf("NAT statistics not available.\n");
                  return_code = SHELL_EXIT_ERROR;
               } else {
                  printf("NAT Statistics:\n");
                  printf("   ST_SESSIONS:                0x%08x\n", nat_stats_ptr->ST_SESSIONS );   
                  printf("   ST_SESSIONS_SNAT:           0x%08x\n", nat_stats_ptr->ST_SESSIONS_SNAT );   
                  printf("   ST_SESSIONS_DNAT:           0x%08x\n", nat_stats_ptr->ST_SESSIONS_DNAT );   
                  printf("   ST_SESSIONS_OPEN:           0x%08x\n", nat_stats_ptr->ST_SESSIONS_OPEN );  
                  printf("   ST_SESSIONS_OPEN_MAX:       0x%08x\n", nat_stats_ptr->ST_SESSIONS_OPEN_MAX ); 
                  printf("   ST_PACKETS_TOTAL:           0x%08x\n", nat_stats_ptr->ST_PACKETS_TOTAL ); 
                  printf("   ST_PACKETS_BYPASS:          0x%08x\n", nat_stats_ptr->ST_PACKETS_BYPASS ); 
                  printf("   ST_PACKETS_PUB_PRV:         0x%08x\n", nat_stats_ptr->ST_PACKETS_PUB_PRV ); 
                  printf("   ST_PACKETS_PUB_PRV_ERR:     0x%08x\n", nat_stats_ptr->ST_PACKETS_PUB_PRV_ERR ); 
                  printf("   ST_PACKETS_PRV_PUB:         0x%08x\n", nat_stats_ptr->ST_PACKETS_PRV_PUB );
                  printf("   ST_PACKETS_PRV_PUB_ERR:     0x%08x\n", nat_stats_ptr->ST_PACKETS_PRV_PUB_ERR );
                  printf("   ST_PACKETS_FRAGMENTS:       0x%08x\n", nat_stats_ptr->ST_PACKETS_FRAGMENTS );
                  printf("   ST_PACKETS_REASSEMBLED:     0x%08x\n", nat_stats_ptr->ST_PACKETS_REASSEMBLED );
                  printf("   ST_PACKETS_REASSSEMBLY_ERR: 0x%08x\n", nat_stats_ptr->ST_PACKETS_REASSSEMBLY_ERR );
              }
            }
            if (natinfo & NATINFO_RULES) {
               priority=0;
               result = DNAT_get_next_rule(&priority,&protocol,&start_port,&end_port,
                  &target_ip,&target_port);
               if (result==RTCS_OK) {
                  printf("NAT Rules:\n");
                  while (result==RTCS_OK) {
                     printf("%d: %s (%d-%d) -> %03d.%03d.%03d.%03d:%d\n",
                        priority, protocol==IPPROTO_TCP?"TCP":"UDP",start_port,end_port, 
                        IPBYTES(target_ip), target_port);
                     result = DNAT_get_next_rule(&priority,&protocol,&start_port,&end_port,
                        &target_ip,&target_port);
                  }
               } else {
                  printf("NAT Rule data not available.\n");
                  return_code = SHELL_EXIT_ERROR;
               }
            }
            if (natinfo & NATINFO_NETWORKS) {
               nat_network_ptr= NAT_networks();
               if (nat_network_ptr == NULL) {
                  printf("NAT Network data not available.\n");
                  return_code = SHELL_EXIT_ERROR;
               } else {
                  printf("NAT Networks:\n");
                  for (num_nets=0;num_nets<nat_network_ptr->NUM_PRV_NETS;num_nets++) {
                     printf("   %2d: %03d.%03d.%03d.%03d  %03d.%03d.%03d.%03d \n", 
                        num_nets,
                        IPBYTES(nat_network_ptr->PRV_NET[num_nets]),
                        IPBYTES(nat_network_ptr->PRV_MASK[num_nets]));
                  }
                  for (num_nets=nat_network_ptr->NUM_PRV_NETS;num_nets<NAT_MAX_PRIVATE_NETWORKS;num_nets++) {
                     if ((nat_network_ptr->PRV_NET[num_nets]!=0) || (nat_network_ptr->PRV_MASK[num_nets])) {
                        printf("   Invalid Entry:\n   %2d: %03d.%03d.%03d.%03d  %03d.%03d.%03d.%03d \n", 
                           num_nets,
                           IPBYTES(nat_network_ptr->PRV_NET[num_nets]),
                           IPBYTES(nat_network_ptr->PRV_MASK[num_nets]));
                     }
                  }
               }
            }
            if (natinfo & NATINFO_SESSIONS) {
               printf("NAT Inbound sessions:\n");
               session_ptr = (NAT_SESSION_STRUCT_PTR) NAT_find_next_session(NULL, 0);
               while (session_ptr!=NULL) {
                  printf("%s-%s: %d.%d.%d.%d:%d -> %d.%d.%d.%d:%d -> %d.%d.%d.%d:%d  [%d]\n",
                     session_ptr->SNAT_OR_DNAT==SNAT?"SNAT":"DNAT",
                     IP_Prot_str(session_ptr->IP_PROT), 
                     IPBYTES(session_ptr->PUB_HST), session_ptr->PUB_PORT,
                     IPBYTES(session_ptr->NAT_HST), session_ptr->NAT_PORT,
                     IPBYTES(session_ptr->PRV_HST), session_ptr->PRV_PORT,
                     session_ptr->TIMEOUT.TIME);

                     session_ptr = (NAT_SESSION_STRUCT_PTR) NAT_find_next_session((void *)session_ptr, 0);
               }
               printf("NAT Outbound sessions:\n");
               session_ptr = (NAT_SESSION_STRUCT_PTR) NAT_find_next_session(NULL, 1);
               while (session_ptr!=NULL) {
                  printf("%s-%s: %d.%d.%d.%d:%d -> %d.%d.%d.%d:%d -> %d.%d.%d.%d:%d  [%d]\n",
                     session_ptr->SNAT_OR_DNAT==SNAT?"SNAT":"DNAT",
                     IP_Prot_str(session_ptr->IP_PROT), 
                     IPBYTES(session_ptr->PRV_HST), session_ptr->PRV_PORT,
                     IPBYTES(session_ptr->NAT_HST), session_ptr->NAT_PORT,
                     IPBYTES(session_ptr->PUB_HST), session_ptr->PUB_PORT,
                     session_ptr->TIMEOUT.TIME);
                     session_ptr = (NAT_SESSION_STRUCT_PTR) NAT_find_next_session((void *)session_ptr, 1);
               }
            }
            if (continuous) {
               if (status()) {
                  continuous = FALSE;
               } else {
                  _time_delay(1000);
               }
            }
         } while (continuous);
      }        
   }

   if (print_usage)  {
      if (shorthelp)  {
         printf("%s [stats|rules|networks|all|loop]\n", argv[0]);
      } else  {
         printf("Usage: %s [stats|rules|networks|all|loop]\n",argv[0]);
      }
   }
   return return_code;
} /* Endbody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_dnat
*  Returned Value:  none
*  Comments  :  SHELL utility to add a DNAT rule
*END*-----------------------------------------------------------------*/

int32_t  Shell_dnat(int32_t argc, char *argv[] )
{
   uint32_t              error;
   bool              print_usage, shorthelp = FALSE;
   int32_t               return_code = SHELL_EXIT_SUCCESS;
   uint32_t              priority=0;     
   uint16_t              protocol=0;
   uint16_t              start_port=0;   
   uint16_t              end_port=0;     
   _ip_address          target_ip=0;      
   uint16_t              target_port=0;  

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc == 7)  {
         if (!Shell_parse_uint_32(argv[1], &priority  )) {
            printf("Error, invalid rule priority\n");
            return_code = SHELL_EXIT_ERROR;
         } else if (!Shell_parse_uint_16(argv[3], &start_port  )) {
            printf("Error, invalid start port\n");
            return_code = SHELL_EXIT_ERROR;
         } else if (!Shell_parse_uint_16(argv[4], &end_port  )) {
            printf("Error, invalid end port\n");
            return_code = SHELL_EXIT_ERROR;
         } else if (! Shell_parse_ip_address( argv[5], &target_ip  ))  {
            printf("Error, invalid ip address\n");
            return_code = SHELL_EXIT_ERROR;
         } else if (!Shell_parse_uint_16(argv[6], &target_port  )) {
            printf("Error, invalid target port\n");
            return_code = SHELL_EXIT_ERROR;
         } else {
            if (strcmp(argv[2],"TCP")==0) {
               protocol = IPPROTO_TCP;
            } else if (strcmp(argv[2],"UDP")==0) {
               protocol = IPPROTO_UDP;
            } else {
               printf("Error, invalid protocol\n");
               return_code = SHELL_EXIT_ERROR;
            }
         }
         if (return_code == SHELL_EXIT_SUCCESS) {
            printf("Adding rule:\n");
            printf("%d: %s (%d-%d) -> %03d.%03d.%03d.%03d:%d",
               priority, protocol==IPPROTO_TCP?"TCP":"UDP",start_port,end_port, 
               IPBYTES(target_ip), target_port);

            error = DNAT_add_rule(priority, protocol, start_port, end_port, target_ip, target_port );
            if (error != RTCS_OK) {
               printf("   Error: %s\n",NAT_err_str(error));
               return_code = SHELL_EXIT_ERROR;
            } else {
               printf("   OK\n");
            }
         }
      } else {
         print_usage = TRUE;
         return_code = SHELL_EXIT_ERROR;
      }
   }

   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <priority> TCP|UDP <start_port> <end_port> <target_ip> <target_port>\n", argv[0]);
      } else  {
         printf("Usage: %s <priority> TCP|UDP <start_port> <end_port> <target_ip> <target_port>\n",argv[0]);
      }
   }
   return return_code;
} /* Endbody */






/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_dnat
*  Returned Value:  none
*  Comments  :  SHELL utility to add a DNAT rule
*END*-----------------------------------------------------------------*/

int32_t  Shell_dnat_delet_rule(int32_t argc, char *argv[] )
{
   uint32_t              error;
   bool              print_usage, shorthelp = FALSE;
   int32_t               return_code = SHELL_EXIT_SUCCESS;
   uint32_t              priority=0;     
   uint16_t              protocol=0;
   uint16_t              start_port=0;   
   uint16_t              end_port=0;     
   _ip_address          target_ip=0;      
   uint16_t              target_port=0;  

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  
   {
      if (argc == 2)  
      {
         if (!Shell_parse_uint_32(argv[1], &priority  )) 
         {
            printf("Error, invalid rule priority\n");
            return_code = SHELL_EXIT_ERROR;
         } 
         if (return_code == SHELL_EXIT_SUCCESS) 
         {
            error = DNAT_delete_rule(priority);
            
            if (error != RTCS_OK) 
            {
               printf("   Error: %s\n",NAT_err_str(error));
               return_code = SHELL_EXIT_ERROR;
            } 
            else 
            {
               printf("   OK\n");
            }
         }
      } 
      else 
      {
         print_usage = TRUE;
         return_code = SHELL_EXIT_ERROR;
      }
   }

   if (print_usage)  {
      if (shorthelp)  {
         printf("%s <priority> TCP|UDP <start_port> <end_port> <target_ip> <target_port>\n", argv[0]);
      } else  {
         printf("Usage: %s <priority> TCP|UDP <start_port> <end_port> <target_ip> <target_port>\n",argv[0]);
      }
   }
   return return_code;
} /* Endbody */




























#endif
#endif /* SHELLCFG_USES_RTCS */
/* EOF */
