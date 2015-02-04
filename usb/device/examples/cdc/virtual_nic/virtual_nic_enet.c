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
* @brief The file contains functions needed by the Network 
*        Interface for Virtual NIC Application
* 
*END************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
 #include "virtual_nic_enet.h"
/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/
_enet_address nic_mac_addr; 
_ip_address nic_ip_addr = (_ip_address)ENET_IPADDR;

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/              
extern CDC_HANDLE g_nic_handle;

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
static void Ethernet_Callback(PCB_PTR  enet_pcb, void  *handle);
static void VNIC_Send_PCB_Free(PCB_PTR nic_pcb);

/*****************************************************************************
 * Local Variables 
 *****************************************************************************/
_enet_handle g_enet_handle;
static ENET_HEADER g_enet_hdr;

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
/*****************************************************************************
 * Global Functions
 *****************************************************************************/
/******************************************************************************
 * 
 *    @name       VNIC_FEC_Init
 *    
 *    @brief      
 *                  
 *    @param      arg : handle to Identify the controller
 * 
 *    @return     None
 *   
 *****************************************************************************/
uint32_t VNIC_FEC_Init(CDC_CONFIG_STRUCT_PTR cdc_config_ptr)
{
    uint32_t  error;
    bool flag = FALSE;

    ENET_get_mac_address ((uint32_t)ENET_DEVICE, nic_ip_addr, nic_mac_addr);

    error = ENET_initialize (ENET_DEVICE, nic_mac_addr, flag, &g_enet_handle);
    if(error != ENET_OK)
    {
        #if _DEBUG
            printf ("ENET_Initialization Failed\n");
        #endif  
        return ENETERR_INIT_FAILED; 
    }

    USB_mem_copy(nic_mac_addr, cdc_config_ptr->mac_address, MAC_ADDR_SIZE);
    cdc_config_ptr->ip_address =  nic_ip_addr;
    
    error = ENET_open(g_enet_handle, ENETPROT_IP, Ethernet_Callback, NULL);
    error |= ENET_open(g_enet_handle, ENETPROT_ARP, Ethernet_Callback, NULL);
    if(error != ENET_OK)
    {
        #if _DEBUG
            printf("ENET_open Failed.\n");
        #endif  
        return ENETERR_OPEN_PROT;
    }
    
    cdc_config_ptr->rndis_max_frame_size = ENET_FRAMESIZE_MAXDATA + ENET_FRAMESIZE_HEAD; 
    
    return error;
}

/******************************************************************************
 * 
 *    @name       VNIC_FEC_Send
 *    
 *    @brief      
 *                  
 *    @param      arg : 
 *    @param      enet_hdr_ptr: 
 * 
 *    @return     None
 *   
 *****************************************************************************/     
uint32_t VNIC_FEC_Send(void *arg, uint8_t *ethernet_frame_ptr, uint32_t payload_size)
{
    uint16_t protocol;
    uint8_t *nic_pcb = NULL;
    uint32_t error;
    PCB_FRAGMENT_PTR frag = NULL;
    UNUSED_ARGUMENT(arg)
    
    USB_mem_copy(ethernet_frame_ptr, 
        g_enet_hdr.DEST , MAC_ADDR_SIZE);
    USB_mem_copy(ethernet_frame_ptr + MAC_ADDR_SIZE, 
        g_enet_hdr.SOURCE, MAC_ADDR_SIZE);
    USB_mem_copy(ethernet_frame_ptr + 2 * MAC_ADDR_SIZE,
         g_enet_hdr.TYPE, 2);
        
    protocol = *((uint16_t *)(((ENET_HEADER_PTR)ethernet_frame_ptr)->TYPE)); 

    nic_pcb = _mem_alloc_system_zero(sizeof(PCB) + sizeof(PCB_FRAGMENT));
    if(nic_pcb == NULL) 
    {
      #if _DEBUG
        printf("Memalloc Failed in VNIC_FEC_Send\n");
      #endif    
      return ENETERR_ALLOC_PCB;
    } 

    frag = ((PCB_PTR)nic_pcb)->FRAG;
    ((PCB_PTR)nic_pcb)->FREE = (void(_CODE_PTR_)(PCB_PTR))VNIC_Send_PCB_Free;
    ((PCB_PTR)nic_pcb)->PRIVATE = NULL;
    frag[0].LENGTH = payload_size;

    frag[0].FRAGMENT = ethernet_frame_ptr;

    frag[1].LENGTH = 0;
    frag[1].FRAGMENT = NULL;
        
     error = ENET_send(g_enet_handle, (PCB_PTR)nic_pcb, protocol, 
        ((ENET_HEADER_PTR)ethernet_frame_ptr)->DEST, 0); //flag passed as Zero
    
    return error;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : VNIC_Send_PCB_Free
*  Returned Value : None
*  Comments       :
*        Free nic_pcb
*
*END*-----------------------------------------------------------------*/
void VNIC_Send_PCB_Free(PCB_PTR nic_pcb)
{      
    if(nic_pcb != NULL)
    {
        /* ethernet_frame_ptr is ((PCB_PTR)nic_pcb)->FRAG[0].FRAGMENT used in VNIC_FEC_Send
           rndis_pkt_msg_data_ptr is ethernet_frame_ptr - RNDIS_USB_OVERHEAD_SIZE 
           used in USB_Notif_Callback*/
        USB_mem_free(((PCB_PTR)nic_pcb)->FRAG[0].FRAGMENT - RNDIS_USB_OVERHEAD_SIZE); 
        USB_mem_free((void*)nic_pcb);
    }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : Ethernet_Callback
*  Returned Value : ENET or error code
*  Comments       :
*        Services an IP packet.
*
*END*-----------------------------------------------------------------*/
void Ethernet_Callback
   (
      PCB_PTR  enet_pcb,
         /* [IN] the received packet */
      void    *handle
         /* [IN] the IP interface structure */
   )
{ /* Body */
    uint32_t length = enet_pcb->FRAG[0].LENGTH;
    uint8_t *nic_data_ptr = enet_pcb->FRAG[0].FRAGMENT; 
    UNUSED_ARGUMENT(handle)
    
    VNIC_USB_Send(nic_data_ptr, length, enet_pcb);
    return;
} /* Endbody */


/* EOF */
