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
* @brief The file contains macros and functiuon api declarations needed by 
*        phdc bridge app
*
*END************************************************************************/

#ifndef _BRIDGE_H
#define _BRIDGE_H
/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_descriptor.h"
#include "ieee11073_phd_types.h"
/******************************************************************************
 * Constants - None
 *****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define ETHERNET_BRIDGE                                 (1)

#define BRIDGE_TYPE                                     (ETHERNET_BRIDGE)

/****** BRIDGE SPECIFIC INCLUDES AND DEFINES ****************/
#if (BRIDGE_TYPE == ETHERNET_BRIDGE)
/* Configure the IP address of Agent as 172.16.0.xxx, 
   where xxx is any number from 1 to 254 except 216*/
    #include "phdc_server.h"
    #define Bridge_Interface_Init       Ethernet_Bridge_Init
    #define Bridge_Interface_Open       Ethernet_Bridge_Open
    #define Bridge_Interface_Read       Ethernet_Bridge_Recv 
    #define Bridge_Interface_Write      Ethernet_Bridge_Send 
    #define Bridge_Interface_Close      Ethernet_Close_Connection
#else
    #error "Undefined Bridge Type!!!"
#endif
/**********************************************************/

#define SEND_DATA_QOS                                   (0x88)

#define APDU_HEADER_SIZE                                (4)

#ifndef ABORT_MESSAGE_SIZE                              
    #define ABORT_MESSAGE_SIZE                              (6)
#endif  

#define RECV_TASK                       (12)
#define SEND_TASK                       (13)

#define USB_ENUM_COMPLETED              (0x00000001)
#define USB_ENUM_COMPLETED_MASK         (0x00000001)
#define BRIDGE_RECV_EVENT_MASK          (0x00000002)
#define BRIDGE_RECV_EVENT               (0x00000002)
#define BRIDGE_SEND_EVENT_MASK          (0x00000004)
#define BRIDGE_SEND_EVENT               (0x00000004)
/******************************************************************************
 * Types
 *****************************************************************************/ 
typedef struct _bridge_variable_struct
{
    uint32_t handle;
    uint8_t usb_enum_complete;
}BRIDGE_GLOBAL_VARIABLE_STRUCT, * PTR_BRIDGE_GLOBAL_VARIABLE_STRUCT;

/******************************************************************************
 * Global Functions
 *****************************************************************************/

#endif

/* EOF */
