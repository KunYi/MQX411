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
*   Example using RTCS Library.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <rtcs.h>
#include <enet.h>
#include <ipcfg.h>
#include "config.h"

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_ITTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#define MAIN_TASK_INDEX 1
#define RX_TASK_INDEX 2
#define TX_TASK_INDEX 3

#define SOCKET_WELCOME_MESSAGE "Ethernet <-> Serial bridge\n\r"
#define SERIAL_WELCOME_MESSAGE "Serial <-> Ethernet bridge\n\r"

#define RX_BUFFER_SIZE 128
#define TX_BUFFER_SIZE 128

/* Structure with sockets used for listening */
typedef struct socket_s
{
    uint32_t sock6; // socket for IPv6
    uint32_t sock4; // socket for IPv4
}SOCKETS_STRUCT, * SOCKETS_STRUCT_PTR;

/* Paramter for tasks */
typedef struct t_param
{
    uint32_t          sock;
    LWSEM_STRUCT     lwsem;
}TASK_PARAMS, * TASK_PARAMS_PTR;

/*
** MQX initialization information
*/
void Main_task (uint32_t);
void rx_task (uint32_t param);
void tx_task (uint32_t param);

const TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    /* Task Index,  Function,  Stack,  Priority,    Name,       Attributes,             Param,  Time Slice */
    {MAIN_TASK_INDEX, Main_task,  2500,     9,        "Bridge",   MQX_AUTO_START_TASK,    0,      0           },
    {RX_TASK_INDEX,   rx_task,    2500,     10,       "rxtask",   0,                      0,      0           },
    {TX_TASK_INDEX,   tx_task,    2500,     10,       "txtask",   0,                      0,      0           },
    {0}
};

void Main_task(uint32_t temp)
{
#if RTCSCFG_ENABLE_IP4
   IPCFG_IP_ADDRESS_DATA    ip_data;
   uint32_t                  ip4_addr = ENET_IPADDR;
#endif
   uint32_t                  error;
   sockaddr                 addr;
   _enet_address            enet_addr = ENET_MAC;
   uint32_t                  retval = 0;
   SOCKETS_STRUCT           sockets;
   uint32_t                  conn_sock;
   uint32_t                  client_sock;
   _task_id                 rx_tid;
   _task_id                 tx_tid;
   FILE_PTR                 ser_device;
   uint32_t                  option;
   uint32_t                  param[3];
   TASK_PARAMS              task_p;
#if RTCSCFG_ENABLE_IP6
    uint32_t                 n = 0;
    uint32_t                 i = 0;
    IPCFG6_GET_ADDR_DATA    data[RTCSCFG_IP6_IF_ADDRESSES_MAX];
    char prn_addr6[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"];
#endif  
   
   sockets.sock4 = 0;
   sockets.sock6 = 0;
   /* Initialize RTCS */
   error = RTCS_create();
   if (error != RTCS_OK)
   {
       fputs("Fatal Error: RTCS initialization failed.", stderr);
       _task_block();
   }
#if RTCSCFG_ENABLE_IP4   
   ip_data.ip = ENET_IPADDR;
   ip_data.mask = ENET_IPMASK;
   ip_data.gateway = ENET_IPGATEWAY;
#endif
   /* Initialize ethernet */  
   error = ipcfg_init_device(BSP_DEFAULT_ENET_DEVICE, enet_addr);
   if (error != IPCFG_OK)
   {
       fprintf(stderr, "Fatal Error 0x%X: Network device initialization failed.\n", error);
       _task_block();
   }
#if RTCSCFG_ENABLE_IP4
   /* Bind static IP address */
   error = ipcfg_bind_staticip(BSP_DEFAULT_ENET_DEVICE, &ip_data);
   if (error != IPCFG_OK)
   {
       fprintf(stderr, "Fatal Error 0x%X: IP address binding failed.", error);
       _task_block();
   }
   /* Prepare IPv4 socket for incoming connections */
   sockets.sock4 = socket(PF_INET, SOCK_STREAM, 0);
   if (sockets.sock4 == RTCS_SOCKET_ERROR)
   {
       fputs("Error: Unable to create socket for IPv4 connections.", stderr);
   }
   else
   {
       ((sockaddr_in*) &addr)->sin_family = AF_INET;
       ((sockaddr_in*) &addr)->sin_port = DEMO_PORT;
       ((sockaddr_in*) &addr)->sin_addr.s_addr = INADDR_ANY;
       retval = bind(sockets.sock4, &addr, sizeof(addr));
       if (retval != RTCS_OK)
       {
           fprintf(stderr, "Error 0x%X: Unable to bind IPv4 socket.\n", retval);
       }
       else
       {
           retval = listen(sockets.sock4, 0);
           if (retval != RTCS_OK)
           {
               fprintf(stderr, "Error 0x%X: Unable to put IPv4 socket in listening state.\n", retval);
           }
       }
   }
#endif
   
#if RTCSCFG_ENABLE_IP6
   while(!ipcfg6_get_addr(BSP_DEFAULT_ENET_DEVICE, n, &data[n]))
   {
       n++;
   }
   /* Prepare IPv6 socket for incoming connections */
   sockets.sock6 = socket(PF_INET6, SOCK_STREAM, 0);
   if (sockets.sock6 == RTCS_SOCKET_ERROR)
   {
       fputs("Error: Unable to create socket for IPv6 connections.", stderr);
   }
   else
   {
       ((sockaddr_in6*) &addr)->sin6_family = AF_INET6;
       ((sockaddr_in6*) &addr)->sin6_port = DEMO_PORT;
       ((sockaddr_in6*) &addr)->sin6_addr = in6addr_any; /* Any address */
       ((sockaddr_in6*) &addr)->sin6_scope_id= 0; /* Any scope */
       retval = bind(sockets.sock6, &addr, sizeof(addr));
       if (retval != RTCS_OK)
       {
           fprintf(stderr, "Error 0x%X: Unable to bind IPv6 socket.\n", retval);
       }
       else
       {
           retval = listen(sockets.sock6, 0);
           if (retval != RTCS_OK)
           {
               fprintf(stderr, "Error 0x%X: Unable to put IPv6 socket in listening state.\n", retval);
           }
       }
   } 
#endif
   
   if (((sockets.sock6 == RTCS_SOCKET_ERROR) && (sockets.sock4 == RTCS_SOCKET_ERROR)) || (retval != RTCS_OK))
   {
       fputs("Fatal Error: No socket avaiable for incoming connections.", stderr);
       _task_block();
   }
   /* Print listening addresses */
   fputs("Application listening on following ip addresses: \n", stdout);
#if RTCSCFG_ENABLE_IP4
   fprintf(stdout, "  IPv4 Address: %d.%d.%d.%d, port:%d\n", IPBYTES(ip4_addr), DEMO_PORT);
#endif
#if RTCSCFG_ENABLE_IP6
   /* Print IP6 addresses to stdout */
   while(i < n)
   {
       if(inet_ntop(AF_INET6,&data[i].ip_addr, prn_addr6, sizeof(prn_addr6)))
       {
           fprintf(stdout, "  IPv6 Address %d: %s, port: %d\n", i++, prn_addr6, DEMO_PORT);
       }
   }
#endif
   /* Initialize semaphore for client disconnection */
   _lwsem_create(&task_p.lwsem, 0);   
   while (1)
   {
       /* Let's wait for activity on both IPv4 and IPv6 */
       fprintf(stdout, "\nWaiting for incoming connection...");
       conn_sock = RTCS_selectset(&sockets, 2, 0);
       if (conn_sock == RTCS_SOCKET_ERROR)
       {
           fprintf(stdout, "FAILED\n\n");
           fputs("Fatal Error: Unable to determine active socket.", stderr);
           _task_block();
       }
       fprintf(stdout, "OK\n\n");
       
       /* Accept incoming connection */
       client_sock = accept(conn_sock, NULL, NULL);
       if (client_sock == RTCS_SOCKET_ERROR)
       {
           uint32_t status;
           status = RTCS_geterror(client_sock);
           fputs("Fatal Error: Unable to accept incoming connection. ", stderr);
           if (status == RTCS_OK)
           {
               fputs("Connection reset by peer.", stderr);
           }
           else 
           {
               fprintf(stderr, "Accept() failed with error code 0x%X.\n", status);
           }
           _task_block();
       }
       
       /* Set socket options */
       option = TRUE;
       retval = setsockopt(client_sock, SOL_TCP, OPT_RECEIVE_NOWAIT, &option, sizeof(option));
       if (retval != RTCS_OK)
       {
           fputs("Fatal Error: Unable to set socket options.", stderr);
           _task_block();
       }
       
       /* Prepare serial device */
       ser_device = fopen(SERIAL_DEVICE, "w");
       if (ser_device == NULL)
       {
           fprintf(stderr, "Fatal Error: Unable to open device \"%s\".\n", SERIAL_DEVICE);
           _task_block();
       }
       ioctl(ser_device, IO_IOCTL_DEVICE_IDENTIFY, param);
       if (param[0] != IO_DEV_TYPE_PHYS_SERIAL_INTERRUPT)
       {
           fprintf(stderr, "Fatal Error: Device \"%s\" is not interrupt driven serial line.\n");
           fputs("This demo application requires enabled interrupt driven serial line (i.e \"ittyf:\").\n", stderr);
           fputs("Please define proper device using macro SERIAL_DEVICE and recompile this demo application.\n", stderr);
           fputs("Please add proper configuration to user_config.h\n", stderr);
           fputs("i.e #define BSPCFG_ENABLE_ITTYF 1\n #define BSP_DEFAULT_IO_CHANNEL_DEFINED\n #define BSP_DEFAULT_IO_CHANNEL \"ittyf:\"\n", stderr);
           _task_block();
       }
       
       /* Print welcome message to socket and serial */
       send(client_sock, SOCKET_WELCOME_MESSAGE, sizeof(SOCKET_WELCOME_MESSAGE), 0);
       retval = fwrite(SERIAL_WELCOME_MESSAGE, 1, strlen(SERIAL_WELCOME_MESSAGE), ser_device);
       fclose(ser_device);
      
       /* Start receive and transmit task */
       task_p.sock = client_sock;
       rx_tid = _task_create(0, RX_TASK_INDEX, (uint32_t) &task_p);
       if (rx_tid == MQX_NULL_TASK_ID)
       {
           fprintf(stderr, "Fatal Error 0x%X: Unable to create receive task.", _task_get_error());
           _task_block();
       }   
       tx_tid = _task_create(0, TX_TASK_INDEX, (uint32_t) &task_p);
       if (tx_tid == MQX_NULL_TASK_ID)
       {
           fprintf(stderr, "Fatal Error 0x%X: Unable to create transmit task.", _task_get_error());
           _task_block();
       }
       
       /* Init is done, connection accepted. Let created tasks run and process data from/to socket and serial port */
       retval = _lwsem_wait(&task_p.lwsem);
       fputs("\nClient disconnected.", stdout);
       if (retval != MQX_OK)
       {
           fputs("Fatal Error: Wait for semaphore failed.", stderr);
           _task_block();
       }
       retval = _task_destroy(rx_tid);
       if (retval != MQX_OK)
       {
           fputs("Fatal Error: Unable to destroy rxtask.", stderr);
           _task_block();
       }
       retval = _task_destroy(tx_tid);
       if (retval != MQX_OK)
       {
           fputs("Fatal Error: Unable to destroy txtask.", stderr);
           _task_block();
       }
   }
}

void rx_task (uint32_t param)
{
    FILE_PTR ser_device;
    char buff[RX_BUFFER_SIZE];
    uint32_t count = 0;
    TASK_PARAMS_PTR task_p = (TASK_PARAMS_PTR) param;
        
    /* Prepare serial port */
    ser_device = fopen(SERIAL_DEVICE, "w");    
    if (ser_device == NULL)
    {
        fprintf(stderr, "RXtask Fatal Error: Unable to open device %s.\n", SERIAL_DEVICE);
        _task_block();
    }
    /* Endless loop for reading from socket and writing to serial port */
    while(1)
    {
        count = recv(task_p->sock, buff, RX_BUFFER_SIZE, 0);
        if (count != RTCS_ERROR)
        {
            count = fwrite(buff, 1, count, ser_device);
        }
        else
        {
            shutdown(task_p->sock, 0);
            _lwsem_post(&task_p->lwsem);
        }
        /* Let the other task run */
        _time_delay(1);
    }
}

void tx_task (uint32_t param)
{ 
    FILE_PTR ser_device;
    char buff[TX_BUFFER_SIZE];
    uint32_t count = 0;
    uint32_t ser_opts = IO_SERIAL_NON_BLOCKING;
    uint32_t retval = 0;
    TASK_PARAMS_PTR task_p = (TASK_PARAMS_PTR) param;
    
    /* Prepare serial port */
    ser_device = fopen(SERIAL_DEVICE, "r");    
    if (ser_device == NULL)
    {
        fprintf(stderr, "TXtask Fatal Error: Unable to open device %s.\n", SERIAL_DEVICE);
        _task_block();
    }
    /* Set non-blocking mode */
    retval = ioctl(ser_device, IO_IOCTL_SERIAL_SET_FLAGS, &ser_opts);
    if (retval != MQX_OK)
    {
        fputs("TXtask Fatal Error: Unable to setup non-blocking mode.", stderr);
        _task_block();
    }
    /* Endless loop for reading from serial and writing to socket */
    while(1)
    {        
        count = fread(buff, 1, TX_BUFFER_SIZE, ser_device);
        count = send(task_p->sock, buff, count, 0);
        if (count == RTCS_ERROR)
        {
            shutdown(task_p->sock, 0);
            _lwsem_post(&task_p->lwsem);
        }
        /* Let the other task run */ 
        _time_delay(1);
    }
}

