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
* @brief The file contains USB descriptors 
* 
*END************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_descriptor.h"

/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/
DESC_CALLBACK_FUNCTIONS_STRUCT  desc_callback = 
{
   0xFF,
   USB_Desc_Get_Descriptor,
   USB_Desc_Get_Endpoints,
   USB_Desc_Get_Interface,
   USB_Desc_Set_Interface,
   USB_Desc_Valid_Configation,
   USB_Desc_Remote_Wakeup,
   USB_Desc_Set_feature,
   USB_Desc_Get_feature
};
/* structure containing details of all the endpoints used by this device */
USB_EP_STRUCT ep[CDC_DESC_ENDPOINT_COUNT] =  {
        #if CIC_NOTIF_ELEM_SUPPORT
        {
            CIC_NOTIF_ENDPOINT, 
            USB_INTERRUPT_PIPE, 
            USB_SEND,
            CIC_NOTIF_ENDP_PACKET_SIZE
        }
        #endif
      
        #if DATA_CLASS_SUPPORT
            #if CIC_NOTIF_ELEM_SUPPORT
            ,
            #endif
            {
                DIC_BULK_IN_ENDPOINT,
                USB_BULK_PIPE, 
                USB_SEND,
                DIC_BULK_IN_ENDP_PACKET_SIZE
            }, 
            {
                DIC_BULK_OUT_ENDPOINT,
                USB_BULK_PIPE, 
                USB_RECV,
                DIC_BULK_OUT_ENDP_PACKET_SIZE
            } 
        #endif
};
 
USB_ENDPOINTS usb_desc_ep = 
{
    CDC_DESC_ENDPOINT_COUNT, 
    ep
};

uint8_t g_device_descriptor[DEVICE_DESCRIPTOR_SIZE] =
{
    /* "Device Dexcriptor Size */
    DEVICE_DESCRIPTOR_SIZE,               
    /* "Device" Type of descriptor */   
    USB_DEVICE_DESCRIPTOR,                
    /*  BCD USB version  */  
    USB_uint_16_low(BCD_USB_VERSION), USB_uint_16_high(BCD_USB_VERSION),
    /* Device Class is indicated in the interface descriptors */   
    DEVICE_DESC_DEVICE_CLASS,
    /*  Device Subclass is indicated in the interface descriptors  */      
    DEVICE_DESC_DEVICE_SUBCLASS,
    /*  Device Protocol  */     
    DEVICE_DESC_DEVICE_PROTOCOL,
    /* Max Packet size */
    CONTROL_MAX_PACKET_SIZE,
    /* Vendor ID */
    0x04,0x25,
    /* Product ID */
    0x00,0x03,  
    /* BCD Device version */
    0x02,0x00,
    /* Manufacturer string index */
    0x01,     
    /* Product string index */  
    0x02,                        
    /*  Serial number string index */
    0x00,                  
    /*  Number of configurations */
    DEVICE_DESC_NUM_CONFIG_SUPPOTED                           
};

uint8_t g_config_descriptor[CONFIG_DESC_SIZE] =
{   
    CONFIG_ONLY_DESC_SIZE, /*  Configuration Descriptor Size - always 9 bytes*/   
    USB_CONFIG_DESCRIPTOR, /* "Configuration" type of descriptor */   
    USB_uint_16_low(CONFIG_DESC_SIZE),
    USB_uint_16_high(CONFIG_DESC_SIZE), /*  Total length of the Configuration descriptor */   
    /*  NumInterfaces */   
    CONFIG_DESC_NUM_INTERFACES_SUPPOTED,
    0x01,                      /*  Configuration Value */      
    0x00,                      /*  Configuration Description String Index*/   
    /*  Attributes.support RemoteWakeup and self power */
    BUS_POWERED|SELF_POWERED|(REMOTE_WAKEUP_SUPPORT<<REMOTE_WAKEUP_SHIFT),
    /*  Current draw from bus */
    CONFIG_DESC_CURRENT_DRAWN, 
    
    /* CIC INTERFACE DESCRIPTOR */  
    IFACE_ONLY_DESC_SIZE,
    USB_IFACE_DESCRIPTOR,
    0x00, /* bInterfaceNumber */
    0x00, /* bAlternateSetting */
    CIC_ENDP_COUNT, /* management and notification(optional)element present */
    CDC_CLASS, /* Communication Interface Class */
    CIC_SUBCLASS_CODE, 
    CIC_PROTOCOL_CODE,
    0x00, /* Interface Description String Index*/
        
    /* CDC Class-Specific descriptor */
    CDC_HEADER_FUNC_DESC_SIZE,/* size of Functional Desc in bytes */
    USB_CS_INTERFACE,  /* descriptor type*/
    HEADER_FUNC_DESC,
    0x10, 0x01,/* USB Class Definitions for CDC spec release number in BCD */
    
    CDC_CALL_MANAG_DESC_SIZE,/* Size of this descriptor */
    USB_CS_INTERFACE, /* descriptor type*/
    CALL_MANAGEMENT_FUNC_DESC,   
    0x01,/*D0(if set): device handales call management itself
           D1(if set): process commands multiplexed over the data interface*/
    0x01,/* Indicates multiplexed commands are handled via data interface */
   
    CDC_ABSTRACT_DESC_SIZE,             /* Size of this descriptor */
    USB_CS_INTERFACE, /* descriptor type*/
    ABSTRACT_CONTROL_FUNC_DESC, 
    0x06,/* Device supports request send break, device supports reuest 
            combination o set_line_coding, set_control_line_state, 
            get_line_coding and the notification serial state */
   
    CDC_UNION_FUNC_DESC_SIZE,             /* size of Functional Desc in bytes */
    USB_CS_INTERFACE,  /* descriptor type*/
    UNION_FUNC_DESC, 
    0x00,   /* Interface Number of Control */
    0x01    /* Interface Number of Subordinate (Data Class) Interface */   
       
#if CIC_NOTIF_ELEM_SUPPORT    /*Endpoint descriptor */
    , /* Comma Added if NOTIF ELEM IS TO BE ADDED */
    ENDP_ONLY_DESC_SIZE,
    USB_ENDPOINT_DESCRIPTOR,
    CIC_NOTIF_ENDPOINT|(USB_SEND << 7),
    USB_INTERRUPT_PIPE, 
    USB_uint_16_low(CIC_NOTIF_ENDP_PACKET_SIZE),
    USB_uint_16_high(CIC_NOTIF_ENDP_PACKET_SIZE),
    0x0A
#endif   
   
#if DATA_CLASS_SUPPORT    
        , /* Comma Added if DATA_CLASS_DESC IS TO BE ADDED */
        IFACE_ONLY_DESC_SIZE,
        USB_IFACE_DESCRIPTOR,
        (uint8_t)(0x00+DATA_CLASS_SUPPORT), /* bInterfaceNumber */
        0x00, /* bAlternateSetting */
        DIC_ENDP_COUNT, /* notification element included */
        0x0A, /* DATA Interface Class */
        0x00, /* Data Interface SubClass Code */
        DIC_PROTOCOL_CODE,
        0x00, /* Interface Description String Index*/
     
        /*Endpoint descriptor */
        ENDP_ONLY_DESC_SIZE,
        USB_ENDPOINT_DESCRIPTOR,
        DIC_BULK_IN_ENDPOINT|(USB_SEND << 7),
        USB_BULK_PIPE, 
        USB_uint_16_low(DIC_BULK_IN_ENDP_PACKET_SIZE),
        USB_uint_16_high(DIC_BULK_IN_ENDP_PACKET_SIZE),
        0x00,/* This value is ignored for Bulk ENDPOINT */
        
        /*Endpoint descriptor */
        ENDP_ONLY_DESC_SIZE,
        USB_ENDPOINT_DESCRIPTOR,
        DIC_BULK_OUT_ENDPOINT|(USB_RECV << 7),
        USB_BULK_PIPE, 
        USB_uint_16_low(DIC_BULK_OUT_ENDP_PACKET_SIZE),
        USB_uint_16_high(DIC_BULK_OUT_ENDP_PACKET_SIZE),
        0x00 /* This value is ignored for Bulk ENDPOINT */
#endif
};

#if HIGH_SPEED_DEVICE
    uint8_t  g_device_qualifier_descriptor[DEVICE_QUALIFIER_DESCRIPTOR_SIZE] =
    {
       /* Device Qualifier Descriptor Size */
       DEVICE_QUALIFIER_DESCRIPTOR_SIZE, 
       /* Type of Descriptor */
       USB_DEVQUAL_DESCRIPTOR,           
       /*  BCD USB version  */  
       USB_uint_16_low(BCD_USB_VERSION), USB_uint_16_high(BCD_USB_VERSION),
       /* bDeviceClass */
       DEVICE_DESC_DEVICE_CLASS,
       /* bDeviceSubClass */
       DEVICE_DESC_DEVICE_SUBCLASS,      
       /* bDeviceProtocol */
       DEVICE_DESC_DEVICE_PROTOCOL,      
       /* bMaxPacketSize0 */
       CONTROL_MAX_PACKET_SIZE,          
       /* bNumConfigurations */
       DEVICE_OTHER_DESC_NUM_CONFIG_SUPPOTED,  
       /* Reserved : must be zero */ 
       0x00                              
    };

    uint8_t  g_other_speed_config_descriptor[OTHER_SPEED_CONFIG_DESCRIPTOR_SIZE] =
    {
        CONFIG_ONLY_DESC_SIZE,     
        /* This is a Other speed config descr */
        USB_OTHER_SPEED_DESCRIPTOR,
        /*  Total length of the Configuration descriptor */   
        USB_uint_16_low(CONFIG_DESC_SIZE), USB_uint_16_high(CONFIG_DESC_SIZE),
        CONFIG_DESC_NUM_INTERFACES_SUPPOTED,
        /*value used to selct this configuration : Configuration Value */      
        1, 
        /*  Configuration Description String Index*/   
        0, 
        /*Attributes.support RemoteWakeup and self power*/
        BUS_POWERED|SELF_POWERED|(REMOTE_WAKEUP_SUPPORT<<REMOTE_WAKEUP_SHIFT),     
        /*  Current draw from bus */
        CONFIG_DESC_CURRENT_DRAWN, 
       
    /* CIC INTERFACE DESCRIPTOR */  
    IFACE_ONLY_DESC_SIZE,
    USB_IFACE_DESCRIPTOR,
    0x00, /* bInterfaceNumber */
    0x00, /* bAlternateSetting */
    CIC_ENDP_COUNT, /* management and notification(optional)element present */
    CDC_CLASS, /* Communication Interface Class */
    CIC_SUBCLASS_CODE, 
    CIC_PROTOCOL_CODE,
    0x00, /* Interface Description String Index*/
        
    /* CDC Class-Specific descriptor */
    CDC_HEADER_FUNC_DESC_SIZE,/* size of Functional Desc in bytes */
    USB_CS_INTERFACE,  /* descriptor type*/
    HEADER_FUNC_DESC,
    0x10, 0x01,/* USB Class Definitions for CDC spec release number in BCD */
    
    CDC_CALL_MANAG_DESC_SIZE,/* Size of this descriptor */
    USB_CS_INTERFACE, /* descriptor type*/
    CALL_MANAGEMENT_FUNC_DESC,   
    0x01,/*D0(if set): device handales call management itself
           D1(if set): process commands multiplexed over the data interface*/
    0x01,/* Indicates multiplexed commands are handled via data interface */
   
    CDC_ABSTRACT_DESC_SIZE,             /* Size of this descriptor */
    USB_CS_INTERFACE, /* descriptor type*/
    ABSTRACT_CONTROL_FUNC_DESC, 
    0x06,/* Device supports request send break, device supports reuest 
            combination o set_line_coding, set_control_line_state, 
            get_line_coding and the notification serial state */
   
    CDC_UNION_FUNC_DESC_SIZE,             /* size of Functional Desc in bytes */
    USB_CS_INTERFACE,  /* descriptor type*/
    UNION_FUNC_DESC, 
    0x00,   /* Interface Number of Control */
    0x01    /* Interface Number of Subordinate (Data Class) Interface */   
       
    #if CIC_NOTIF_ELEM_SUPPORT    /*Endpoint descriptor */
        , /* Comma Added if NOTIF ELEM IS TO BE ADDED */
        ENDP_ONLY_DESC_SIZE,
        USB_ENDPOINT_DESCRIPTOR,
        CIC_NOTIF_ENDPOINT|(USB_SEND << 7),
        USB_INTERRUPT_PIPE, 
        USB_uint_16_low(CIC_NOTIF_ENDP_PACKET_SIZE),
        USB_uint_16_high(CIC_NOTIF_ENDP_PACKET_SIZE),
        0x0A
    #endif   
   
    #if DATA_CLASS_SUPPORT    
        , /* Comma Added if DATA_CLASS_DESC IS TO BE ADDED */
        IFACE_ONLY_DESC_SIZE,
        USB_IFACE_DESCRIPTOR,
        (uint8_t)(0x00+DATA_CLASS_SUPPORT), /* bInterfaceNumber */
        0x00, /* bAlternateSetting */
        DIC_ENDP_COUNT, /* notification element included */
        0x0A, /* DATA Interface Class */
        0x00, /* Data Interface SubClass Code */
        DIC_PROTOCOL_CODE,
        0x00, /* Interface Description String Index*/
     
        /*Endpoint descriptor */
        ENDP_ONLY_DESC_SIZE,
        USB_ENDPOINT_DESCRIPTOR,
        DIC_BULK_IN_ENDPOINT|(USB_SEND << 7),
        USB_BULK_PIPE, 
        USB_uint_16_low(OTHER_SPEED_DIC_BULK_IN_ENDP_PACKET_SIZE),
        USB_uint_16_high(OTHER_SPEED_DIC_BULK_IN_ENDP_PACKET_SIZE),
        0x00,/* This value is ignored for Bulk ENDPOINT */
        
        /*Endpoint descriptor */
        ENDP_ONLY_DESC_SIZE,
        USB_ENDPOINT_DESCRIPTOR,
        DIC_BULK_OUT_ENDPOINT|(USB_RECV << 7),
        USB_BULK_PIPE, 
        USB_uint_16_low(OTHER_SPEED_DIC_BULK_OUT_ENDP_PACKET_SIZE),
        USB_uint_16_high(OTHER_SPEED_DIC_BULK_OUT_ENDP_PACKET_SIZE),
        0x00 /* This value is ignored for Bulk ENDPOINT */
    #endif
    };
#endif

uint8_t USB_STR_0[USB_STR_0_SIZE+USB_STR_DESC_SIZE] = 
                                    {sizeof(USB_STR_0),    
                                     USB_STRING_DESCRIPTOR, 
                                      0x09,
                                      0x04/*equiavlent to 0x0409*/ 
                                    };
                                    
uint8_t USB_STR_1[USB_STR_1_SIZE+USB_STR_DESC_SIZE] 
                          = {  sizeof(USB_STR_1),          
                               USB_STRING_DESCRIPTOR,
                               'F',0,
                               'R',0,
                               'E',0,
                               'E',0,
                               'S',0,
                               'C',0,
                               'A',0,
                               'L',0,
                               'E',0,
                               ' ',0,
                               'S',0,
                               'E',0,
                               'M',0,
                               'I',0,
                               'C',0,
                               'O',0,
                               'N',0,
                               'D',0,
                               'U',0,
                               'C',0,
                               'T',0,
                               'O',0,
                               'R',0,
                               ' ',0,
                               'I',0,
                               'N',0,
                               'C',0,
                               '.',0                             
                          };


uint8_t USB_STR_2[USB_STR_2_SIZE+USB_STR_DESC_SIZE] 
                          = {  sizeof(USB_STR_2),          
                               USB_STRING_DESCRIPTOR,                                                                                           
                               'M',0,
                               'Q',0,
                               'X',0,
                               ' ',0,
                               'V',0,
                               'I',0,
                               'R',0,
                               'T',0,
                               'U',0,
                               'A',0,
                               'L',0,
                               ' ',0,
                               'C',0,
                               'O',0,
                               'M',0,
                               ' ',0,
                               'D',0,
                               'E',0,
                               'M',0,
                               'O',0                               
                          };

uint8_t USB_STR_n[USB_STR_n_SIZE+USB_STR_DESC_SIZE] 
                          = {  sizeof(USB_STR_n),         
                               USB_STRING_DESCRIPTOR,                                
                               'B',0,
                               'A',0,
                               'D',0,
                               ' ',0,
                               'S',0,
                               'T',0,
                               'R',0,
                               'I',0,
                               'N',0,
                               'G',0,
                               ' ',0,
                               'I',0,
                               'N',0,
                               'D',0,
                               'E',0,
                               'X',0                               
                          };


uint32_t g_std_desc_size[USB_MAX_STD_DESCRIPTORS+1] = 
{
    0,
    DEVICE_DESCRIPTOR_SIZE,
    CONFIG_DESC_SIZE,
    0, /* string */
    0, /* Interfdace */
    0, /* Endpoint */
    #if HIGH_SPEED_DEVICE
        DEVICE_QUALIFIER_DESCRIPTOR_SIZE,
        OTHER_SPEED_CONFIG_DESCRIPTOR_SIZE
    #else                                       
         0, /* Device Qualifier */
         0 /* other spped config */
    #endif
};   
                                             
uint8_t *g_std_descriptors[USB_MAX_STD_DESCRIPTORS+1] = 
 {
      NULL,
      g_device_descriptor,
      g_config_descriptor,
      NULL, /* string */
      NULL, /* Interfdace */
      NULL, /* Endpoint */
      #if HIGH_SPEED_DEVICE
          g_device_qualifier_descriptor,
          g_other_speed_config_descriptor
      #else
          NULL, /* Device Qualifier */
          NULL /* other spped config*/
      #endif
 }; 
   
uint8_t g_string_desc_size[USB_MAX_STRING_DESCRIPTORS+1] = 
                                    {sizeof(USB_STR_0),
                                     sizeof(USB_STR_1),
                                     sizeof(USB_STR_2),
                                     sizeof(USB_STR_n)
                                    };   
                                             
uint8_t *g_string_descriptors[USB_MAX_STRING_DESCRIPTORS+1] = 
                                                          {USB_STR_0,
                                                           USB_STR_1,
                                                           USB_STR_2,
                                                           USB_STR_n
                                                          };    
USB_LANGUAGE    usb_lang[USB_MAX_SUPPORTED_INTERFACES] = 
                                   { 
                                    (uint16_t)0x0409,
                                    g_string_descriptors,
                                    g_string_desc_size
                                  };                                                                                              
USB_ALL_LANGUAGES g_languages = { USB_STR_0, sizeof(USB_STR_0),
                                  usb_lang
                                };
                                
uint8_t g_valid_config_values[USB_MAX_CONFIG_SUPPORTED+1]={0,1};

uint8_t g_line_coding[LINE_CODING_SIZE] = 
{
 /*e.g. 0x00,0x10,0x0E,0x00 : 0x000E1000 is 921600 bits per second */
    (LINE_CODE_DTERATE_IFACE>> 0) & 0x000000FF,
    (LINE_CODE_DTERATE_IFACE>> 8) & 0x000000FF,
    (LINE_CODE_DTERATE_IFACE>>16) & 0x000000FF,          
    (LINE_CODE_DTERATE_IFACE>>24) & 0x000000FF,
     LINE_CODE_CHARFORMAT_IFACE,
     LINE_CODE_PARITYTYPE_IFACE,
     LINE_CODE_DATABITS_IFACE
};

uint8_t g_abstract_state[COMM_FEATURE_DATA_SIZE] = 
{
     (STATUS_ABSTRACT_STATE_IFACE>>0) & 0x00FF,
     (STATUS_ABSTRACT_STATE_IFACE>>8) & 0x00FF                                                                      
};

uint8_t g_country_code[COMM_FEATURE_DATA_SIZE] = 
{
 (COUNTRY_SETTING_IFACE>>0) & 0x00FF,
 (COUNTRY_SETTING_IFACE>>8) & 0x00FF                                                              
};

/****************************************************************************
 * Global Variables
 ****************************************************************************/
static uint8_t g_alternate_interface[USB_MAX_SUPPORTED_INTERFACES];
                                                        
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes 
 *****************************************************************************/

/*****************************************************************************
 * Local Variables - None
 *****************************************************************************/
 
 
 /*****************************************************************************
 * Local Functions - None
 *****************************************************************************/

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Descriptor
 *
 * @brief The function returns the correponding descriptor
 *
 * @param handle:        handle     
 * @param type:          type of descriptor requested     
 * @param str_num:       string index for string descriptor     
 * @param index:         string descriptor language Id     
 * @param descriptor:    output descriptor pointer
 * @param size:          size of descriptor returned
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Desc_Get_Descriptor(
     uint32_t handle, 
     uint8_t type,
     uint8_t str_num, 
     uint16_t index,
     uint8_t **descriptor,
     uint32_t *size) 
{
     UNUSED_ARGUMENT(handle)

    /* string descriptors are handled saperately */
    if (type == USB_STRING_DESCRIPTOR)
    { 
        if(index == 0) 
        {  
            /* return the string and size of all languages */      
            *descriptor = (uint8_t *)g_languages.languages_supported_string;
            *size = g_languages.languages_supported_size;            
        } 
        else 
        {
            uint8_t lang_id=0;
            uint8_t lang_index=USB_MAX_LANGUAGES_SUPPORTED;
            
            for(;lang_id< USB_MAX_LANGUAGES_SUPPORTED;lang_id++) 
            {
                /* check whether we have a string for this language */
                if(index == g_languages.usb_language[lang_id].language_id) 
                {   /* check for max descriptors */
                    if(str_num < USB_MAX_STRING_DESCRIPTORS) 
                    {   /* setup index for the string to be returned */
                        lang_index=str_num;                 
                    }                    
                    break;                    
                }                
            }

            /* set return val for descriptor and size */
            *descriptor = 
              (uint8_t *)g_languages.usb_language[lang_id].lang_desc[str_num];
            *size = 
                  g_languages.usb_language[lang_id].lang_desc_size[lang_index];
        }        
    }
    else if (type < USB_MAX_STD_DESCRIPTORS+1)
    {
        /* set return val for descriptor and size*/
        *descriptor = (uint8_t *)g_std_descriptors [type];
       
        /* if there is no descriptor then return error */
        if(*descriptor == NULL) 
        {
            return USBERR_INVALID_REQ_TYPE;
        }        
        *size = g_std_desc_size[type];                
    }
    else /* invalid descriptor */
    {
        return USBERR_INVALID_REQ_TYPE;
    }
    return USB_OK;  
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Interface
 *
 * @brief The function returns the alternate interface
 *
 * @param handle:         handle     
 * @param interface:      interface number     
 * @param alt_interface:  output alternate interface     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Desc_Get_Interface(uint32_t handle, 
                              uint8_t interface, 
                              uint8_t *alt_interface)
{   
    UNUSED_ARGUMENT(handle)
    
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get alternate interface*/
        *alt_interface = g_alternate_interface[interface];
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Interface
 *
 * @brief The function sets the alternate interface
 *
 * @param handle:         handle     
 * @param interface:      interface number     
 * @param alt_interface:  input alternate interface     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Desc_Set_Interface(uint32_t handle, 
                              uint8_t interface, 
                              uint8_t alt_interface)
{
    UNUSED_ARGUMENT(handle)
    
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set alternate interface*/
        g_alternate_interface[interface]=alt_interface;
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Valid_Configation
 *
 * @brief The function checks whether the configuration parameter 
 *        input is valid or not
 *
 * @param handle          handle    
 * @param config_val      configuration value     
 *
 * @return TRUE           When Valid
 *         FALSE          When Error
 *****************************************************************************/
bool USB_Desc_Valid_Configation(uint32_t handle,
                                   uint16_t config_val)
{
    uint8_t loop_index=0;
 
    UNUSED_ARGUMENT(handle)
 
    /* check with only supported val right now */
    while(loop_index < (USB_MAX_CONFIG_SUPPORTED+1)) 
    {
        if(config_val == g_valid_config_values[loop_index]) 
        {          
            return TRUE;
        }
        loop_index++;
    }    
    return FALSE;    
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Remote_Wakeup
 *
 * @brief The function checks whether the remote wakeup is supported or not
 *
 * @param handle:        handle     
 *
 * @return REMOTE_WAKEUP_SUPPORT (TRUE) - if remote wakeup supported
 *****************************************************************************/
bool USB_Desc_Remote_Wakeup(uint32_t handle) 
{
    UNUSED_ARGUMENT(handle)
    return REMOTE_WAKEUP_SUPPORT;   
}           

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Endpoints
 *
 * @brief The function returns with the list of all non control endpoints used
 *
 * @param handle:          handle     
 *
 * @return pointer to USB_ENDPOINTS
 *****************************************************************************/
USB_ENDPOINTS *USB_Desc_Get_Endpoints(uint32_t handle) 
{
    UNUSED_ARGUMENT(handle) 
    return &usb_desc_ep;
}         

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Line_Coding
 *
 * @brief The function returns the Line Coding/Configuraion
 *
 * @param handle:        handle     
 * @param interface:     interface number     
 * @param coding_data:   output line coding data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Desc_Get_Line_Coding(uint32_t handle, 
                                uint8_t interface, 
                                uint8_t **coding_data)
{   
    UNUSED_ARGUMENT(handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get line coding data*/
        *coding_data = g_line_coding;
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Line_Coding
 *
 * @brief The function sets the Line Coding/Configuraion
 *
 * @param handle: handle     
 * @param interface:     interface number     
 * @param coding_data:   output line coding data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Desc_Set_Line_Coding(uint32_t handle, 
                                uint8_t interface, 
                                uint8_t **coding_data)
{   
    uint8_t count;

    UNUSED_ARGUMENT(handle)
    
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set line coding data*/
        for (count = 0; count < LINE_CODING_SIZE; count++) 
        {          
            g_line_coding[count] = *((*coding_data+USB_SETUP_PKT_SIZE) + count);
        }
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Abstract_State
 *
 * @brief The function gets the current setting for communication feature
 *                                                  (ABSTRACT_STATE)
 * @param handle:        handle
 * @param interface:     interface number     
 * @param feature_data:   output comm feature data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Desc_Get_Abstract_State(uint32_t handle, 
                                uint8_t interface, 
                                uint8_t **feature_data)
{   
    UNUSED_ARGUMENT(handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get line coding data*/
        *feature_data = g_abstract_state;
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Country_Setting
 *
 * @brief The function gets the current setting for communication feature
 *                                                  (COUNTRY_CODE)
 * @param handle:        handle     
 * @param interface:     interface number     
 * @param feature_data:   output comm feature data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Desc_Get_Country_Setting(uint32_t handle, 
                                    uint8_t interface, 
                                    uint8_t **feature_data)
{   
    UNUSED_ARGUMENT(handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get line coding data*/
        *feature_data = g_country_code;
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Abstract_State
 *
 * @brief The function gets the current setting for communication feature
 *                                                  (ABSTRACT_STATE)
 * @param handle:        handle     
 * @param interface:     interface number     
 * @param feature_data:   output comm feature data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Desc_Set_Abstract_State(uint32_t handle, 
                                uint8_t interface, 
                                uint8_t **feature_data)
{   
    uint8_t count;
    UNUSED_ARGUMENT(handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set Abstract State Feature*/
        for (count = 0; count < COMM_FEATURE_DATA_SIZE; count++) 
        {          
            g_abstract_state[count] = *(*feature_data + count);
        }
        return USB_OK; 
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Country_Setting
 *
 * @brief The function gets the current setting for communication feature
 *                                                  (COUNTRY_CODE)
 * @param handle: handle     
 * @param interface:     interface number     
 * @param feature_data:   output comm feature data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Desc_Set_Country_Setting(uint32_t handle, 
                                    uint8_t interface, 
                                    uint8_t **feature_data)
{   
    uint8_t count;
    UNUSED_ARGUMENT (handle)
    
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        for (count = 0; count < COMM_FEATURE_DATA_SIZE; count++) 
        {          
            g_country_code[count] = *(*feature_data + count);
        }
        return USB_OK; 
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_feature
 *
 * @brief  .
 *
 * @param handle:           
 *                                
 * @param cmd:              
 * @param in_data:          
 * @param out_buf           
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint8_t USB_Desc_Get_feature(uint32_t handle,int32_t cmd, uint8_t in_data,
                            uint8_t ** out_buf)
{
    uint8_t error = USBERR_INVALID_REQ_TYPE;
    
    switch (cmd)
    {
        case GET_LINE_CODING:
          error = USB_Desc_Get_Line_Coding(handle,in_data,out_buf);
          break;
        case GET_ABSTRACT_STATE:
         error = USB_Desc_Get_Abstract_State(handle,in_data,out_buf);
         break;
        case GET_COUNTRY_SETTING:
         error = USB_Desc_Get_Country_Setting(handle,in_data,out_buf);
         break;
        default:
        error = USBERR_INVALID_REQ_TYPE;
        break;
    }
    
    return error;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_feature
 *
 * @brief  .
 *
 * @param handle:           
 *                                
 * @param cmd:              
 * @param in_data:          
 * @param in_buf           
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint8_t USB_Desc_Set_feature(uint32_t handle,int32_t cmd, uint8_t in_data,uint8_t ** in_buf)
{
    uint8_t error = USBERR_INVALID_REQ_TYPE;
    
    switch (cmd)
    {
        case SET_LINE_CODING:
          error = USB_Desc_Set_Line_Coding(handle,in_data,in_buf);
          break;
        case SET_ABSTRACT_STATE:
         error = USB_Desc_Set_Abstract_State(handle,in_data,in_buf);
         break;
        case SET_COUNTRY_SETTING:
         error = USB_Desc_Set_Country_Setting(handle,in_data,in_buf);
         break;
        default:
        error = USBERR_INVALID_REQ_TYPE;
        break;
    }
    
    return error;
}

/* EOF */
