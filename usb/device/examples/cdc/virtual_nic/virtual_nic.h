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
* @brief The file contains Macro's and functions needed by the Virtual Network
*        Interface Application
* 
*END************************************************************************/


#ifndef _VIRTUAL_NIC_H
#define _VIRTUAL_NIC_H  1

#include "usb_descriptor.h"
#include "virtual_nic_enet.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define DATA_BUFF_SIZE                  (DIC_BULK_OUT_ENDP_PACKET_SIZE)
/*****************************************************************************
 * Global variables
 *****************************************************************************/

/******************************************************************************
 * Types
 *****************************************************************************/
 typedef struct _rndis_packet_msg_format
 {
    /*Specifies the Remote NDIS message type. 
      This is set to REMOTE_NDIS_PACKET_MSG = 0x1.*/
    uint32_t message_type;
    /*Message length in bytes, including appended packet data, out-of-band 
      data, per-packet-info data, and both internal and external padding*/
    uint32_t message_len;
    /*Specifies the offset in bytes from the start of the DataOffset field of 
      this message to the start of the data. This is an integer multiple of 4*/
    uint32_t data_offset;
    /*Specifies the number of bytes in the data content of this message.*/
    uint32_t data_len;
    /*Specifies the offset in bytes of the first out of band data record from 
      the start of the DataOffset field of this message.  Set to 0 if there is
      no out-of-band data. Otherwise this is an integer multiple of 4*/  
    uint32_t oob_data_offset;
    /*Specifies in bytes the total length of the out of band data.*/
    uint32_t oob_data_len;
    /*Specifies the number of out of band records in this message*/
    uint32_t num_oob_data_elems;
    /*Specifies in bytes the offset from the beginning of the DataOffset field
      in the REMOTE_NDIS_PACKET_MSG data message to the start of the first per 
      packet info data record.  Set to 0 if there is no per-packet data. 
      Otherwise this is an integer multiple of 4*/
    uint32_t per_pkt_info_offset;
    /*Specifies in bytes the total length of the per packet information 
      contained in this message*/
    uint32_t per_pkt_info_len;
    /*Reserved for connection-oriented devices.  Set to 0.*/
    uint32_t vc_handle;
    /*Reserved. Set to 0.*/
    uint32_t reserved;  
 }RNDIS_PKT_MSG_STRUCT, * PTR_RNDIS_PKT_MSG_STRUCT;

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
extern void TestApp_Init(void);
extern void VNIC_USB_Send(uint8_t *nic_data_ptr, uint32_t length, PCB_PTR  enet_pcb);

#endif 


/* EOF */
