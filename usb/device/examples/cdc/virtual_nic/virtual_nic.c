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
* @brief  The file emulates a USB PORT as Network Card.
* 
*END************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "virtual_nic.h"


extern void Main_Task(uint32_t param);
#define MAIN_TASK       10

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK, Main_Task, 2*3000L, 7L, "Main", MQX_AUTO_START_TASK, 0, 0},
   { 0L, 0L, 0L, 0L, 0L, 0L ,0 ,0}
};

/*****************************************************************************
 * Constant and Macro's 
 *****************************************************************************/
/* Base unit for ENIT layer is 1Mbps while for RNDIS its 100bps*/
#define ENET_CONVERT_FACTOR         (10000)
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/              
extern USB_ENDPOINTS usb_desc_ep;
extern DESC_CALLBACK_FUNCTIONS_STRUCT  desc_callback;
extern _enet_handle g_enet_handle;
CDC_HANDLE g_nic_handle;

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val,void *arg);
void USB_Notif_Callback(uint8_t event_type,void* val,void *arg); 
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/
static bool start_app;
static uint8_t *g_recv_buff_ptr = NULL;
static uint8_t g_zero_send = 0x00;
/*****************************************************************************
 * Local Functions
 *****************************************************************************/
/*****************************************************************************
 *  
 *   @name        TestApp_Init
 * 
 *   @brief       Invoked from Main_Task. Calls for all types of 
 *                Initializations
 * 
 *   @param       None
 * 
 *   @return      None
 **                
 *****************************************************************************/ 
void TestApp_Init(void)
{       
    CDC_CONFIG_STRUCT cdc_config;
    USB_CLASS_CDC_ENDPOINT * endPoint_ptr = NULL;
        
    /* Initialize USB Structures */    
    endPoint_ptr = USB_mem_alloc_zero(sizeof(USB_CLASS_CDC_ENDPOINT)*CDC_DESC_ENDPOINT_COUNT);
    if(endPoint_ptr == NULL)
    {
        #if _DEBUG
            printf("memalloc failed in TestApp_Init\n");
        #endif  
        return;
    }
    cdc_config.comm_feature_data_size = COMM_FEATURE_DATA_SIZE;
    cdc_config.max_supported_interfaces = USB_MAX_SUPPORTED_INTERFACES;
    cdc_config.usb_ep_data = &usb_desc_ep;
    cdc_config.desc_endpoint_cnt = CDC_DESC_ENDPOINT_COUNT;
    cdc_config.cdc_class_cb.callback = USB_App_Callback;
    cdc_config.cdc_class_cb.arg = &g_nic_handle;
    cdc_config.vendor_req_callback.callback = NULL;
    cdc_config.vendor_req_callback.arg = NULL;
    cdc_config.param_callback.callback = USB_Notif_Callback;
    cdc_config.param_callback.arg = &g_nic_handle;
    cdc_config.desc_callback_ptr =  &desc_callback;
    cdc_config.ep = endPoint_ptr;
    cdc_config.cic_send_endpoint = CIC_NOTIF_ENDPOINT;
    /* Always happend in control endpoint hence hard coded in Class layer*/
    cdc_config.dic_send_endpoint = DIC_BULK_IN_ENDPOINT;
    cdc_config.dic_recv_endpoint = DIC_BULK_OUT_ENDPOINT;
    cdc_config.dic_recv_pkt_size = DIC_BULK_OUT_ENDP_PACKET_SIZE;
    cdc_config.dic_send_pkt_size = DIC_BULK_IN_ENDP_PACKET_SIZE;
    cdc_config.cic_send_pkt_size = CIC_NOTIF_ENDP_PACKET_SIZE;
    
    if (MQX_OK != _usb_device_driver_install(USBCFG_DEFAULT_DEVICE_CONTROLLER)) {
        printf("Driver could not be installed\n");
        return;
    }

    /* Initialize the FEC interface */
    VNIC_FEC_Init(&cdc_config);
    /* Initialize the USB interface */
    g_nic_handle = USB_Class_CDC_Init(&cdc_config);

    while (TRUE) 
    {
        /* call the periodic task function */      
        USB_CDC_Periodic_Task();           
    }/* Endwhile */   
} 

/******************************************************************************
 * 
 *    @name       VNIC_USB_Send
 *    
 *    @brief      
 *                  
 *    @param      nic_data_ptr
 *    @param      length
 * 
 *    @return     None
 *   
 *****************************************************************************/
void VNIC_USB_Send(uint8_t *nic_data_ptr, uint32_t length, PCB_PTR  enet_pcb)
{   
    /* if total data to be sent to host is greater than 
       DIC_BULK_IN_ENDP_PACKET_SIZE than break it to max endpoint
       size for bulk In -- approach used to increase performance by avoiding 
       large memory allocations and memcopies in application layer */       
    uint32_t usb_tx_len = length + RNDIS_USB_OVERHEAD_SIZE;  
    uint32_t first_packet_size = 0;
    uint8_t *first_send_buff = NULL;
    uint8_t return_status = USB_OK;
    uint8_t zero_send_flag;
        
    /* RNDIS Protocol defines 1 byte call of 0x00, if 
    transfer size is multiple of endpoint packet size */
    zero_send_flag = (uint8_t)((usb_tx_len%DIC_BULK_IN_ENDP_PACKET_SIZE) ?
         0 : 1);
    
    /* whichver is smaller but not less than RNDIS_USB_OVERHEAD_SIZE */
    first_packet_size = usb_tx_len > DIC_BULK_IN_ENDP_PACKET_SIZE ? 
        DIC_BULK_IN_ENDP_PACKET_SIZE : usb_tx_len;
    
    if(first_packet_size < RNDIS_USB_OVERHEAD_SIZE)
    {/* for DIC_BULK_IN_ENDP_PACKET_SIZE as 8, 16 or 32
        minimun  first_packet_size has to be either usb_tx_len
        (which is definitely greater than RNDIS_USB_OVERHEAD_SIZE)
        or atleast 64 which is the next allowed multiple of 
        DIC_BULK_IN_ENDP_PACKET_SIZE*/ 
        first_packet_size = usb_tx_len > 64 ? 64 : usb_tx_len;
    }
        
    /* we are reserving one DWORD before packet for distingusihing between 
       packet with USB HEADER and the one only with payload */
    first_send_buff = (uint8_t *)USB_mem_alloc_zero(first_packet_size + 4);
    /*NOTE : currently USB_mem_alloc also sets all buffers to 0x00 */   
    if(first_send_buff == NULL)
    {
        #if _DEBUG
            printf("first_send_buff malloc failed: VNIC_USB_Send\n");
        #endif  
        return;
    }

    /* Zero marks Packet with USB HEADER 
       non zero value is used to preserve enet_pcb value before packet*/
    *((uint32_t *)first_send_buff) = (uint32_t)0;       
    
    first_send_buff += 4;/* address for actual payload */
    
    /* Prepare USB Header */
    ((PTR_RNDIS_PKT_MSG_STRUCT)first_send_buff)->message_type = LONG_LE_TO_HOST_CONST(REMOTE_NDIS_PACKET_MSG);
    ((PTR_RNDIS_PKT_MSG_STRUCT)first_send_buff)->message_len = LONG_LE_TO_HOST(usb_tx_len);
    ((PTR_RNDIS_PKT_MSG_STRUCT)first_send_buff)->data_offset = LONG_LE_TO_HOST_CONST(RNDIS_DATA_OFFSET);
    ((PTR_RNDIS_PKT_MSG_STRUCT)first_send_buff)->data_len = LONG_LE_TO_HOST(length);
    
    /* fill rest of first_send_buff buffers with payload as much as possible */
    USB_mem_copy(nic_data_ptr, first_send_buff + RNDIS_USB_OVERHEAD_SIZE,
        first_packet_size - RNDIS_USB_OVERHEAD_SIZE);   

    if(!(usb_tx_len > first_packet_size))
    {
        /* free the PCB here itself as it has been copied in g_send_buff */
        PCB_free(enet_pcb);
    }
    return_status = USB_Class_CDC_Send_Data(g_nic_handle, DIC_BULK_IN_ENDPOINT,
        first_send_buff, first_packet_size);                
    if(return_status != USB_OK)
    {
        #if _DEBUG
            printf("First USB Send of VNIC_USB_Send FAILED\n");
        #endif  
        return;
    }

    if(usb_tx_len > first_packet_size)
    {       
        /* enet_pcb address has been appended before buffer pointer used for 
           Send data call on USB */
        *((uint32_t *)(nic_data_ptr + (first_packet_size - RNDIS_USB_OVERHEAD_SIZE)) - 1) = 
            (uint32_t)enet_pcb;
        
        return_status = USB_Class_CDC_Send_Data(g_nic_handle, DIC_BULK_IN_ENDPOINT,
            nic_data_ptr + (first_packet_size - RNDIS_USB_OVERHEAD_SIZE) , 
            usb_tx_len - first_packet_size);
        if(return_status != USB_OK)
        {
            #if _DEBUG
                printf("Second USB Send of VNIC_USB_Send FAILED\n");
            #endif
            return;
        }               
    }

    if(zero_send_flag == 1)
    {
        return_status = USB_Class_CDC_Send_Data(g_nic_handle, DIC_BULK_IN_ENDPOINT,
            &g_zero_send, sizeof(uint8_t));
        if(return_status != USB_OK)
        {
            #if _DEBUG
                printf("Third USB Send of VNIC_USB_Send FAILED\n");
            #endif
            return;
        }
    }
    
    return;
}

/******************************************************************************
 * 
 *    @name        USB_App_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       handle : handle to Identify the controller
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val,void *arg) 
{
    UNUSED_ARGUMENT (arg)
    UNUSED_ARGUMENT (val)    
    if(event_type == USB_APP_BUS_RESET) 
    {
        start_app=FALSE; 
    }
    else if(event_type == USB_APP_ENUM_COMPLETE) 
    {
        start_app=TRUE; 
    }
    else if(event_type == USB_APP_ERROR)
    {
        /* add user code for error handling */
    }
    
    return;
}

/******************************************************************************
 * 
 *    @name        USB_Notif_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       arg:  handle to Identify the controller
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
void USB_Notif_Callback(uint8_t event_type,void* val,void *arg) 
{    
    uint32_t handle;
    APP_DATA_STRUCT* dp_rcv;          
    
    handle = *((uint32_t *)arg);    
    
    if(start_app != TRUE)
        return;
    
    if(event_type == USB_APP_DATA_RECEIVED)
    {                   
        uint8_t *rndis_pkt_msg_data_ptr = NULL;
        uint32_t frame_remaining_len = 0;
        uint32_t message_len = 0;
        dp_rcv = (APP_DATA_STRUCT*)val;
        
        if(g_recv_buff_ptr == NULL)        
        {/* preserve address of lower layer buffer -- to use it later to save memory */
            g_recv_buff_ptr = dp_rcv->data_ptr;
        }
        
        if(dp_rcv->data_ptr == g_recv_buff_ptr)
        {   /* indicates first part of received packet */           
            message_len = LONG_LE_TO_HOST(*((uint32_t *)dp_rcv->data_ptr + 1));
            
            if(!(message_len%DIC_BULK_OUT_ENDP_PACKET_SIZE))
            {
                /* RNDIS Protocol defines 1 byte call of 0x00, if 
               transfer size is multiple of endpoint packet size */
                message_len++;
            }
            
            rndis_pkt_msg_data_ptr = (uint8_t *)USB_mem_alloc_zero(message_len);           
            if(rndis_pkt_msg_data_ptr == NULL)
            {
                #if _DEBUG
                    printf("1: memalloc failed in USB_Notif_Callback\n");
                #endif  
            }
            
            USB_mem_copy(dp_rcv->data_ptr, rndis_pkt_msg_data_ptr, dp_rcv->data_size);
            
            frame_remaining_len = message_len - dp_rcv->data_size;           
        }

        /* this is second part of OUT TRANSFER packet received from host */
        
        if(frame_remaining_len)
        {   /* Since first half of packet will be of DIC_BULK_OUT_ENDP_PACKET_SIZE,
               Call rest at its offset and return*/
            /* required when ethernet packet + usb header is larger than  
               DIC_BULK_OUT_ENDP_PACKET_SIZE */  
            (void)USB_Class_CDC_Recv_Data(handle,DIC_BULK_OUT_ENDPOINT,
                rndis_pkt_msg_data_ptr+DIC_BULK_OUT_ENDP_PACKET_SIZE,
                frame_remaining_len);   
            return; 
        }
        
        /* NOTE : if execution control reaches here with rndis_pkt_msg_data_ptr
           as not equal to NULL then the entire Packet was received in one 
           transaction */
         
        if(dp_rcv->data_ptr != g_recv_buff_ptr)
        {   /* entire ethernet packet with USB header was not received as
               one transaction */
            rndis_pkt_msg_data_ptr = 
                dp_rcv->data_ptr - DIC_BULK_OUT_ENDP_PACKET_SIZE;
            /* because rest of the packet was received at offset */ 
        }
            
        /* re-calculate message_len as it might have changed because 
           of 1 byte of zero recv */
        message_len = LONG_LE_TO_HOST(*((uint32_t *)rndis_pkt_msg_data_ptr + 1));
        
        /* Queue Another receive on Bulk Out Endpoint */
        VNIC_FEC_Send(arg, (uint8_t *)(rndis_pkt_msg_data_ptr + 
            RNDIS_USB_OVERHEAD_SIZE), message_len - RNDIS_USB_OVERHEAD_SIZE);   

        /* Queue next receive */
        (void)USB_Class_CDC_Recv_Data(g_nic_handle, DIC_BULK_OUT_ENDPOINT,
            g_recv_buff_ptr, DIC_BULK_OUT_ENDP_PACKET_SIZE);
     }
     else if(event_type == USB_APP_SEND_COMPLETE)
     {
        dp_rcv = (APP_DATA_STRUCT*)val;
        if(dp_rcv->data_ptr == &g_zero_send)
        {   /* No action is needed on 1 byte of 0x00 send after payload */
            return;
        }
        
        if(*((uint32_t *)dp_rcv->data_ptr - 1))
        {   /* non zero value before data pointer indicates enet_pcb value */
            PCB_free((PCB_PTR)(*((uint32_t *)dp_rcv->data_ptr - 1)));
            /* *((uint32_t *)dp_rcv->data_ptr - 1) has enet_pcb to be freed */
        }
        else
        {
            USB_mem_free(dp_rcv->data_ptr - 4);                 
        }
     }
     else if(event_type == USB_APP_GET_LINK_SPEED)
     {              
        *((uint32_t *)val)= ENET_get_speed(g_enet_handle);
        *((uint32_t *)val) *= ENET_CONVERT_FACTOR; 
     }
     else if(event_type == USB_APP_GET_LINK_STATUS)
     {
        *((bool*)val)= ENET_link_status(g_enet_handle);
     }
   return;
}

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : Main_Task
* Returned Value : None
* Comments       :
*     First function called.  Calls the Test_App
*     callback functions.
* 
*END*--------------------------------------------------------------------*/
void Main_Task
(
  uint32_t param
)
{   
    UNUSED_ARGUMENT (param)
    TestApp_Init();  
   
}

/* EOF */
