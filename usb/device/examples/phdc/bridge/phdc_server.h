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
*
*END************************************************************************/
#ifndef _PHDC_SERVER_H
#define _PHDC_SERVER_H

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <rtcs.h>
#include <enet.h>
#include <lwevent.h>

#include "usb_phdc.h"
/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Global Variables
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
/* events for callback */
#define BRDIGE_DATA_RECEIVED            (1)
#define BRDIGE_DATA_SEND                (2)
#define BRIDGE_CONNECTION_ESTABLISHED   (3)
#define BRIDGE_CONNECTION_TERMINATED    (4)

#define PHDC_TASK                       (11)

#define PHDC_SIN_PORT                   (6024)

#define SOCK_UNINITIALIZED_VAL          (0xFFFFFFFF)

#ifndef ABORT_MESSAGE_SIZE                              
    #define ABORT_MESSAGE_SIZE                              (6)
#endif  

/*
** NOTE: the device is bsp specific.  For a PowerQUICC it's the
** SCC/FCC number.  The mcads266 it's 5.
*/
#define ENET_DEVICE  0

/*
** The target's IP address and netmask
*/
#define ENET_IPADDR  0xAC1000D8     // 172.16.0.216

#define ENET_IPMASK  0xFFFFFF00

/*
** The target's Ethernet address
** The first three bytes should be 00 00 5E
** The last three bytes should match the last three bytes of
** your test IP address
*/
#undef  _IPBN       /* IP address byte number b */
#define _IPBN(b)    (((ENET_IPADDR) >> (b*8)) & 255)
#define ENET_ENETADDR   {0x00, 0x00, 0x5E, _IPBN(2), _IPBN(1), _IPBN(0)}


/*
** Uncomment this line to enable PPP
*/
//#define PPP_DEVICE      "ittya:"


/*
** Define PPP_DEVICE_DUN only when using PPP to communicate
** to Win9x Dial-Up Networking over a null-modem
** This is ignored if PPP_DEVICE is not #define'd
*/
#define PPP_DEVICE_DUN

/*
** Define the local and remote IP addresses for the PPP link
** These are ignored if PPP_DEVICE is not #define'd
*/
#define PPP_LOCADDR     0xC0A80049
#define PPP_PEERADDR    0xC0A800C9

/*
** Define a default gateway
*/
#define GATE_ADDR       0xC0A80001

/*
** Define a server to be used by the example clients
*/
//#define SERVER_ADDR  0xC0A80145   // 192.168.1.69
#define SERVER_ADDR  0xAC10008A     // 172.16.0.138




/******************************************************************************
 * Types
 *****************************************************************************/ 
typedef struct
{
    bool flag;
    uint8_t *buff_ptr;
    uint32_t size;
}RECV_PENDING_STRUCT, * PTR_RECV_PENDING_STRUCT;

/******************************************************************************
 * Global Functions
 *****************************************************************************/
extern void Ethernet_Bridge_Init(USB_CLASS_CALLBACK  callback);
extern void PhdcSrv_Task(uint32_t param);
extern void Ethernet_Bridge_Open(uint32_t param);
extern void Ethernet_Bridge_Recv(uint8_t *buff_ptr, uint32_t size);
extern void Ethernet_Bridge_Send(uint8_t *buff_ptr, uint32_t size);
extern void Ethernet_Close_Connection(void);

#endif

/* EOF */
