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
* @brief  The file emulates a USB PORT as RS232 PORT.
* 
*END************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "virtual_com.h"

extern void Main_Task(uint32_t param);
#define MAIN_TASK       10
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK, Main_Task, 2*3000L, 7L, "Main", MQX_AUTO_START_TASK, 0, 0},
   { 0L, 0L, 0L, 0L, 0L, 0L, 0, 0}
};

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
 
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void TestApp_Init(void);

/****************************************************************************
 * Global Variables
 ****************************************************************************/              
extern USB_ENDPOINTS usb_desc_ep;
extern DESC_CALLBACK_FUNCTIONS_STRUCT  desc_callback;

CDC_HANDLE   g_app_handle;

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val,void *arg);
void USB_Notif_Callback(uint8_t event_type,void* val,void *arg); 
void Virtual_Com_App(void);
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/
static bool start_app = FALSE;
static bool start_transactions = FALSE;
static uint8_t *g_curr_recv_buf;
static uint8_t *g_curr_send_buf;
static uint8_t g_recv_size;
static uint8_t g_send_size;
/*****************************************************************************
 * Local Functions
 *****************************************************************************/

 /*****************************************************************************
 *  
 *   @name        TestApp_Init
 * 
 *   @brief       This function is the entry for mouse (or other usuage)
 * 
 *   @param       None
 * 
 *   @return      None
 **                
 *****************************************************************************/
void TestApp_Init(void)
{       
    //uint8_t   error;
    CDC_CONFIG_STRUCT cdc_config;
    USB_CLASS_CDC_ENDPOINT * endPoint_ptr;
    
    g_curr_recv_buf = _mem_alloc_uncached(DATA_BUFF_SIZE);
    g_curr_send_buf = _mem_alloc_uncached(DATA_BUFF_SIZE);
    
    endPoint_ptr = USB_mem_alloc_zero(sizeof(USB_CLASS_CDC_ENDPOINT)*CDC_DESC_ENDPOINT_COUNT);
    cdc_config.comm_feature_data_size = COMM_FEATURE_DATA_SIZE;
    cdc_config.usb_ep_data = &usb_desc_ep;
    cdc_config.desc_endpoint_cnt = CDC_DESC_ENDPOINT_COUNT;
    cdc_config.cdc_class_cb.callback = USB_App_Callback;
    cdc_config.cdc_class_cb.arg = &g_app_handle;
    cdc_config.vendor_req_callback.callback = NULL;
    cdc_config.vendor_req_callback.arg = NULL;
    cdc_config.param_callback.callback = USB_Notif_Callback;
    cdc_config.param_callback.arg = &g_app_handle;
    cdc_config.desc_callback_ptr =  &desc_callback;
    cdc_config.ep = endPoint_ptr;
    cdc_config.cic_send_endpoint = CIC_NOTIF_ENDPOINT;
    /* Always happend in control endpoint hence hard coded in Class layer*/
    //cdc_config.cic_recv_endpoint = 
    cdc_config.dic_send_endpoint = DIC_BULK_IN_ENDPOINT;
    cdc_config.dic_recv_endpoint = DIC_BULK_OUT_ENDPOINT;
    
    if (MQX_OK != _usb_device_driver_install(USBCFG_DEFAULT_DEVICE_CONTROLLER)) {
        printf("Driver could not be installed\n");
        return;
    }

    /* Initialize the USB interface */
    g_app_handle = USB_Class_CDC_Init(&cdc_config);
    g_recv_size = 0;
    g_send_size= 0;    
  
    while (TRUE) 
    {
        /* call the periodic task function */      
        USB_CDC_Periodic_Task();           

       /*check whether enumeration is complete or not */
        if((start_app==TRUE) && (start_transactions==TRUE))
        {        
            Virtual_Com_App(); 
        }            
    }/* Endwhile */   
} 

/******************************************************************************
 * 
 *    @name       Virtual_Com_App
 *    
 *    @brief      
 *                  
 *    @param      None
 * 
 *    @return     None
 *    
 *****************************************************************************/
void Virtual_Com_App(void)
{
    /* User Code */ 
    if(g_recv_size) 
    {
        _mqx_int i;
        
        /* Copy Buffer to Send Buff */
        for (i = 0; i < g_recv_size; i++)
        {
            printf("Copied: %c\n", g_curr_recv_buf[i]);
        	g_curr_send_buf[g_send_size++] = g_curr_recv_buf[i];
        }
        g_recv_size = 0;
    }
    
    if(g_send_size) 
    {
        uint8_t error;
        uint8_t size = g_send_size;
        g_send_size = 0;

        error = USB_Class_CDC_Send_Data(g_app_handle, DIC_BULK_IN_ENDPOINT,
        	g_curr_send_buf, size);
        if (!error && !(size % DIC_BULK_IN_ENDP_PACKET_SIZE)) {
            /* If the last packet is the size of endpoint, then send also zero-ended packet,
            ** meaning that we want to inform the host that we do not have any additional
            ** data, so it can flush the output.
            */
            error = USB_Class_CDC_Send_Data(g_app_handle, DIC_BULK_IN_ENDPOINT, NULL, 0);
        }
        if(error != USB_OK) 
        {
            /* Failure to send Data Handling code here */
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
 *    @param       handle:  handle to Identify the controller
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
 
void USB_Notif_Callback(uint8_t event_type,void* val,void *arg) 
{
    uint32_t handle;
    uint8_t index;
    
    handle = *((uint32_t *)arg);
    if(start_app == TRUE) 
    {
        if(event_type == USB_APP_CDC_DTE_ACTIVATED) 
        {
           start_transactions = TRUE;        
        } 
        else if(event_type == USB_APP_CDC_DTE_DEACTIVATED) 
        {
           start_transactions = FALSE;        
        }
        else if((event_type == USB_APP_DATA_RECEIVED) && (start_transactions == TRUE))
        {
           
            uint32_t BytesToBeCopied;            
            APP_DATA_STRUCT* dp_rcv;
            dp_rcv = (APP_DATA_STRUCT*)val; 
            
            BytesToBeCopied = dp_rcv->data_size;
            for(index = 0; index < BytesToBeCopied; index++) 
            {
                g_curr_recv_buf[index] = dp_rcv->data_ptr[index];
                printf("Received: %c\n", g_curr_recv_buf[index]);
            }
            g_recv_size = index;

            /* Schedule buffer for next receive event */
            USB_Class_CDC_Recv_Data(handle, DIC_BULK_OUT_ENDPOINT, g_curr_recv_buf, DIC_BULK_OUT_ENDP_PACKET_SIZE); 

        }        
        else if((event_type == USB_APP_SEND_COMPLETE)&&
            (start_transactions == TRUE))
        { 
            /* User: add your own code for send complete event */ 
        }
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
