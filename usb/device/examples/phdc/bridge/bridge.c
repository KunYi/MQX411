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
/******************************************************************************
 * Includes
 *****************************************************************************/
#include "bridge.h"

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
#define MAIN_TASK               (10)
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/
extern USB_ENDPOINTS usb_desc_ep;
extern DESC_CALLBACK_FUNCTIONS_STRUCT  desc_callback;
uint8_t abort_msg[ABORT_MESSAGE_SIZE] = {0xE6, 0x00, 0x00, 0x02, 0x00, 0x00};
#if (BRIDGE_TYPE == ETHERNET_BRIDGE)
    extern char Phdc_Task_Name;
#endif
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
static void Main_Task(uint32_t param);
static void Recv_Task(uint32_t param);
static void Send_Task(uint32_t param);

static void USB_App_Callback(uint8_t event_type,void* val,void *arg);
static void Bridge_Callback(uint8_t event_type,void* val,void *arg);
/*****************************************************************************
 * Local Variables
 *****************************************************************************/
/* Add all the variables needed for phd_com_model.c to this structure */
BRIDGE_GLOBAL_VARIABLE_STRUCT g_bridge;
static LWEVENT_STRUCT lwevent;
static uint8_t *g_usb_rx_buff_ptr = NULL;
static uint8_t *g_bridge_rx_buff_ptr = NULL;
static uint8_t *g_tx_buff_ptr = NULL;
static uint32_t g_bridge_tx_size = 0;
static bool g_hdr_flag = TRUE;
static bool g_connection_present = FALSE;
static uint32_t g_bridge_rx_size = 0;
static char Main_Task_Name[] = "Main";
static char Recv_Task_Name[] = "Bridge_Recv";
static char Send_Task_Name[] = "Bridge_Send";

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*Task number, Entry point,  Stack,   Pri, String,         Auto? */
{MAIN_TASK,    Main_Task,    2*3000L, 8L, Main_Task_Name, MQX_AUTO_START_TASK, 0, 0},
{RECV_TASK,    Recv_Task,    1000L,   9L, Recv_Task_Name, MQX_AUTO_START_TASK, 0, 0},
{SEND_TASK,    Send_Task,    1000L,   9L, Send_Task_Name, MQX_AUTO_START_TASK, 0, 0},
#if (BRIDGE_TYPE == ETHERNET_BRIDGE)
/* PhdcSrv_Task has lower priority than Main_Task */
{PHDC_TASK,    PhdcSrv_Task, 1000L,   7L, &Phdc_Task_Name, MQX_AUTO_START_TASK, 0, 0},
#endif
{0,            0,            0,       0,   0,             0,                   0, 0}
};
/*****************************************************************************
 * Local Functions
 *****************************************************************************/
/******************************************************************************
 *
 *    @name        USB_App_Callback
 *
 *    @brief       This function handles the callback
 *
 *    @param       handle
 *    @param       event_type    : type of the event
 *    @param       val           : val
 *
 *    @return      None
 *
 *****************************************************************************
 * This function is called from the lower layer whenever an event occurs.
 * This sets a variable according to the event_type
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type,void* val,void *arg)
{

    switch(event_type)
    {
        case USB_APP_BUS_RESET:
            g_bridge.usb_enum_complete = FALSE;
            if(g_connection_present == TRUE)
            {   /* close the previous connection if present */
                Bridge_Interface_Close();
            }
            break;
        case USB_APP_ENUM_COMPLETE:
            g_bridge.usb_enum_complete = TRUE;
            if(_lwevent_set(&lwevent,USB_ENUM_COMPLETED) != MQX_OK)
            {
                #if _DEBUG
                    printf("\n 1 Set Event failed : USB_App_Callback");
                #endif
                _task_block();
            }
            break;
        case USB_APP_GET_TRANSFER_SIZE:
            {
                volatile PTR_USB_CLASS_PHDC_RX_BUFF usb_rx_buff =
                    (PTR_USB_CLASS_PHDC_RX_BUFF)val;
                #if USB_METADATA_SUPPORTED
                    if(usb_rx_buff->meta_data_packet)
                    {
                        PTR_USB_META_DATA_MSG_PREAMBLE metadata_preamble_ptr =
                            (PTR_USB_META_DATA_MSG_PREAMBLE)usb_rx_buff->in_buff;
                        usb_rx_buff->transfer_size = (uint32_t)
                            (metadata_preamble_ptr->opaque_data_size +
                            METADATA_HEADER_SIZE);
                    }
                    else
                #endif
                    {
                        APDU *papdu = (APDU*)usb_rx_buff->in_buff;
                        usb_rx_buff->transfer_size = (uint32_t)
                            (papdu->length + APDU_HEADER_SIZE);

                    }
            }
            break;
        case USB_APP_GET_DATA_BUFF:
            {
                /* called by lower layer to get recv buffer */
                volatile PTR_USB_CLASS_PHDC_RX_BUFF usb_rx_buff =
                    (PTR_USB_CLASS_PHDC_RX_BUFF)val;
                usb_rx_buff->out_size = *((uint16_t *)usb_rx_buff->in_buff + 1) +
                    APDU_HEADER_SIZE;
                usb_rx_buff->out_buff = USB_mem_alloc_zero(usb_rx_buff->out_size);
                if(usb_rx_buff->out_buff == NULL)
                {
                    #if _DEBUG
                        printf("\n usb_rx_buff->out_buff malloc failed in USB_App_Callback");
                    #endif
                }
            }
            break;
        case USB_APP_DATA_RECEIVED:
            {
                volatile PTR_USB_APP_EVENT_DATA_RECIEVED usb_rx_buff =
                    (PTR_USB_APP_EVENT_DATA_RECIEVED)val;
                g_tx_buff_ptr = usb_rx_buff->buffer_ptr;
                g_bridge_tx_size = usb_rx_buff->size;
                /* Trigger send on Bridge Interface */
                if(_lwevent_set(&lwevent,BRIDGE_SEND_EVENT) != MQX_OK)
                {
                    #if _DEBUG
                        printf("\n 2 Set Event failed : USB_App_Callback");
                    #endif
                    _task_block();
                }

                if(!(usb_rx_buff->size%PHDC_BULK_OUT_EP_SIZE))
                {/* need to call zero byte of USB recv because total transfer
                    size is multiple of PHDC_BULK_OUT_EP_SIZE */
                    (void)USB_Class_PHDC_Recv_Data(g_bridge.handle, PHDC_BULK_OUT_QOS,
                        g_usb_rx_buff_ptr, 0);
                }
            }
            break;
        case USB_APP_SEND_COMPLETE:
            {
                volatile PTR_USB_APP_EVENT_SEND_COMPLETE send_buff =
                    (PTR_USB_APP_EVENT_SEND_COMPLETE)val;
                if((g_bridge_rx_size != 0)||(send_buff->size == 0))
                {/* break if g_bridge_rx_size is non-zero meaning entire data has not yet
                    been received from Bridge interface, meaning that this callback is for
                    partial send on USB bus.
                    Also break if send_buff->size is zero, because this callback this
                    corresponding to zero byte send data on USB bus. Already a REcv has
                    been queued on Bridge Interface. So there's no point in doing that again */
                    break;
                }
                /* Queue another recv on Bridge Interface */
                if(_lwevent_set(&lwevent,BRIDGE_RECV_EVENT) != MQX_OK)
                {
                    #if _DEBUG
                        printf("\n 3 Set Event failed : USB_App_Callback");
                    #endif
                    _task_block();
                }
            }
            break;
        case USB_APP_META_DATA_PARAMS_CHANGED:
            break;
        case USB_APP_ERROR:
            break;
    }
    return;
}


/******************************************************************************
 *
 *    @name        Bridge_Callback
 *
 *    @brief       This function handles the callback
 *
 *    @param       None
 *
 *    @return      None
 *
 *****************************************************************************/
 void Bridge_Callback(uint8_t event_type,void* val,void *arg)
 {
    switch(event_type)
    {
        case BRDIGE_DATA_RECEIVED:
            {
                PTR_USB_APP_EVENT_DATA_RECIEVED data_recv_ptr =
                    (PTR_USB_APP_EVENT_DATA_RECIEVED)val;
                (void)USB_Class_PHDC_Send_Data (g_bridge.handle, FALSE, 0,
                    SEND_DATA_QOS, data_recv_ptr->buffer_ptr,
                    data_recv_ptr->size);

                if(g_hdr_flag == TRUE)
                {
                    g_bridge_rx_size = (uint32_t)(*((uint16_t *)data_recv_ptr->buffer_ptr + 1)) +
                        APDU_HEADER_SIZE - data_recv_ptr->size; /*data yet to be received */
                    if(g_bridge_rx_size)
                    {   /* call for more data to recv on BRidge */
                        g_hdr_flag = FALSE;
                        if(_lwevent_set(&lwevent,BRIDGE_RECV_EVENT) != MQX_OK)
                        {
                            #if _DEBUG
                                printf("BRDIGE_DATA_RECEIVED: Set Event failed : Bridge_Callback\n");
                            #endif
                            _task_block();
                        }
                    }
                    else if(!(data_recv_ptr->size % PHDC_BULK_IN_EP_SIZE))
                    {/* Entire data was received in one transaction but its multiple of
                        PHDC_BULK_IN_EP_SIZE, therefore, need to send zero byte data */
                        (void)USB_Class_PHDC_Send_Data (g_bridge.handle, FALSE, 0,
                            SEND_DATA_QOS, data_recv_ptr->buffer_ptr, 0);
                        /* buffer pointer is immaterial in this case */
                    }
                }
                else
                {
                    g_bridge_rx_size -= data_recv_ptr->size;

                    if((!g_bridge_rx_size)&&((!(data_recv_ptr->size % PHDC_BULK_IN_EP_SIZE))))
                    {/* entire data from bridge has been received and last transaction was
                        multiple of PHDC_BULK_IN_EP_SIZE, therefore need to send zero byte data */
                        (void)USB_Class_PHDC_Send_Data (g_bridge.handle, FALSE, 0,
                            SEND_DATA_QOS, data_recv_ptr->buffer_ptr, 0);
                        /* buffer pointer is immaterial in this case */
                        g_hdr_flag = TRUE;
                    }
                    else if(data_recv_ptr->size < PHDC_BULK_IN_EP_SIZE)
                    {   /* Short packet */
                        g_hdr_flag = TRUE;
                    }
                    else
                    {/* there is more data to recv from Bridge Interface */
                        if(_lwevent_set(&lwevent,BRIDGE_RECV_EVENT) != MQX_OK)
                        {
                            #if _DEBUG
                                printf("BRDIGE_DATA_RECEIVED: Set Event failed : Bridge_Callback\n");
                            #endif
                            _task_block();
                        }
                    }
                }
            }
            break;
        case BRDIGE_DATA_SEND :
            {
                PTR_USB_APP_EVENT_DATA_RECIEVED data_send_ptr =
                    (PTR_USB_APP_EVENT_DATA_RECIEVED)val;

                USB_mem_free(data_send_ptr->buffer_ptr);

                /* Queue another recv on USB BUS */
                (void)USB_Class_PHDC_Recv_Data(g_bridge.handle, PHDC_BULK_OUT_QOS,
                    g_usb_rx_buff_ptr, PHDC_BULK_OUT_EP_SIZE);
            }
            break;
        case BRIDGE_CONNECTION_ESTABLISHED :
            g_connection_present = TRUE;
            if(_lwevent_set(&lwevent,BRIDGE_RECV_EVENT) != MQX_OK)
            {
                #if _DEBUG
                    printf("BRIDGE_CONNECTION_ESTABLISHED: Set Event failed : Bridge_Callback\n");
                #endif
                _task_block();
            }
            break;
        case BRIDGE_CONNECTION_TERMINATED :
            g_connection_present = FALSE;
            if((bool)g_bridge.usb_enum_complete == TRUE)
            {/* this if condition prevents call of send data when
                USB INTERFACE is absent */
                (void)USB_Class_PHDC_Send_Data (g_bridge.handle, FALSE, 0,
                SEND_DATA_QOS, abort_msg, ABORT_MESSAGE_SIZE);
            }
            break;
    }
    return;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : Main_Task
* Returned Value : None
* Comments       :
*     First function called.  This function is the entry for
*     the PHDC Application
*
*END*--------------------------------------------------------------------*/
void Main_Task(uint32_t param)
{
    /* Initialize Global Variable Structure */
    PHDC_CONFIG_STRUCT phdc_config;
    phdc_config.phdc_callback.callback =  USB_App_Callback;
    phdc_config.phdc_callback.arg = (void*)&g_bridge.handle;
    phdc_config.vendor_callback.callback = NULL;
    phdc_config.vendor_callback.arg = NULL;
    phdc_config.desc_callback_ptr = &desc_callback;
    phdc_config.info = &usb_desc_ep;

    USB_mem_zero(&g_bridge, sizeof(BRIDGE_GLOBAL_VARIABLE_STRUCT));

    if (_lwevent_create(&lwevent,0) != MQX_OK)
    {
        #if _DEBUG
            printf("\nMake event failed : Main_Task");
        #endif
        _task_block();
    }

    g_usb_rx_buff_ptr = USB_mem_alloc_zero(PHDC_BULK_OUT_EP_SIZE);
    if(g_usb_rx_buff_ptr == NULL)
    {
        #if _DEBUG
            printf("g_usb_rx_buff_ptr malloc failed\n");
        #endif
    }

    g_bridge_rx_buff_ptr = USB_mem_alloc_zero(PHDC_BULK_IN_EP_SIZE);
    if(g_bridge_rx_buff_ptr == NULL)
    {
        #if _DEBUG
            printf("g_bridge_rx_buff_ptr  malloc failed\n");
        #endif
    }

    if (MQX_OK != _usb_device_driver_install(USBCFG_DEFAULT_DEVICE_CONTROLLER)) {
        printf("Driver could not be installed\n");
        return;
    }

    g_bridge.handle = USB_Class_PHDC_Init(&phdc_config);
    Bridge_Interface_Init(Bridge_Callback);

    while(TRUE)
    {
        /* Block the task until USB Enumeration is completed */
        if(_lwevent_wait_for(&lwevent,USB_ENUM_COMPLETED,FALSE,NULL) !=
            MQX_OK)
        {
            #if _DEBUG
                printf("USB_ENUM_COMPLETEDEvent Wait failed\n");
            #endif
            _task_block();
        }

        if(_lwevent_clear(&lwevent,USB_ENUM_COMPLETED) != MQX_OK)
        {
            #if _DEBUG
                printf("Enum Event Clear failed\n");
            #endif
            _task_block();
        }

        Bridge_Interface_Open(param);
    }
}

/******************************************************************************
 *
 *   @name        Recv_Task
 *
 *   @brief
 *
 *   @param       None
 *
 *   @return      None
 *****************************************************************************/
void Recv_Task(uint32_t param)
{
    while(TRUE)
    {
        if((bool)g_bridge.usb_enum_complete == FALSE)
        {
            continue;
        }

        if(_lwevent_wait_for(&lwevent,BRIDGE_RECV_EVENT,FALSE,NULL) !=
            MQX_OK)
        {/* since third argument is false, the execution continues when any
            of the event is generated */
            #if _DEBUG
                printf("Event Wait failed : Recv_Task\n");
            #endif
            _task_block();
        }

        /* clear the event and proceed*/
        if(_lwevent_clear(&lwevent,BRIDGE_RECV_EVENT) != MQX_OK)
        {
            #if _DEBUG
                printf("\nRecv Event Clear failed");
            #endif
            _task_block();
        }

        if(g_connection_present == TRUE)
        {
            /* Queue recv on Bridge Interface of max buffer size*/
            Bridge_Interface_Read(g_bridge_rx_buff_ptr, PHDC_BULK_IN_EP_SIZE);
        }
    }
}

/******************************************************************************
 *
 *   @name        Send_Task
 *
 *   @brief
 *
 *   @param       None
 *
 *   @return      None
 *****************************************************************************/
void Send_Task(uint32_t param)
{
    while(TRUE)
    {
        if((bool)g_bridge.usb_enum_complete == FALSE)
        {
            continue;
        }

        if(_lwevent_wait_for(&lwevent,BRIDGE_SEND_EVENT,FALSE,NULL) !=
            MQX_OK)
        {/* since third argument is false, the execution continues when any
            of the event is generated */
            #if _DEBUG
                printf("Event Wait failed : Send_Task\n");
            #endif
            _task_block();
        }

        /* clear the event and proceed*/
        if(_lwevent_clear(&lwevent,BRIDGE_SEND_EVENT) != MQX_OK)
        {
            #if _DEBUG
                printf("\nSend Event Clear failed");
            #endif
            _task_block();
        }

        if(g_connection_present == TRUE)
        {
            Bridge_Interface_Write(g_tx_buff_ptr, g_bridge_tx_size);
        }
    }
}

/* EOF */
