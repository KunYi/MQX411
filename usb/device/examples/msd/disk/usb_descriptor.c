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
* @brief The file contains USB descriptors and functions
* 
*END************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_descriptor.h"

/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/ 
USB_EP_STRUCT ep[MSC_DESC_ENDPOINT_COUNT] = 
{  
    {
        BULK_IN_ENDPOINT,
        USB_BULK_PIPE, 
        USB_SEND,
        BULK_IN_ENDP_PACKET_SIZE
    }, 
    {
        BULK_OUT_ENDPOINT,
        USB_BULK_PIPE, 
        USB_RECV,
        BULK_OUT_ENDP_PACKET_SIZE
    }        
};

/* structure containing details of all the endpoints used by this device */ 
USB_ENDPOINTS usb_desc_ep =
{
    MSC_DESC_ENDPOINT_COUNT,
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
    0x00,0x02,  
    /* BCD Device version */
    0x02,0x00,
    /* Manufacturer string index */
    0x01,     
    /* Product string index */  
    0x02,                        
    /*  Serial number string index */
    0x03,                  
    /*  Number of configurations */
    DEVICE_DESC_NUM_CONFIG_SUPPOTED                           
};

uint8_t g_config_descriptor[CONFIG_DESC_SIZE] =         
{   
    /* Configuration Descriptor Size - always 9 bytes*/   
    CONFIG_ONLY_DESC_SIZE,  
    /* "Configuration" type of descriptor */   
    USB_CONFIG_DESCRIPTOR,  
    /*  Total length of the Configuration descriptor */   
    USB_uint_16_low(CONFIG_DESC_SIZE),USB_uint_16_high(CONFIG_DESC_SIZE),
    /*  NumInterfaces */   
    CONFIG_DESC_NUM_INTERFACES_SUPPOTED,
    /*  Configuration Value */      
    1,
    /* Configuration Description String Index */   
    0,
    /*  Attributes.support RemoteWakeup and self power */
    BUS_POWERED|SELF_POWERED|(REMOTE_WAKEUP_SUPPORT<<REMOTE_WAKEUP_SHIFT),
    /*  Current draw from bus */
    CONFIG_DESC_CURRENT_DRAWN, 
    
    /* INTERFACE DESCRIPTOR */  
    IFACE_ONLY_DESC_SIZE,
    USB_IFACE_DESCRIPTOR,
    /* bInterfaceNumber */
    0x00,
    /* bAlternateSetting */
    0x00,
    (uint8_t)MSC_DESC_ENDPOINT_COUNT,     
    /* Interface Class */     
    MASS_STORAGE_CLASS,
      /* Interface Subclass*/     
    SCSI_TRANSPARENT_COMMAND_SET,
    /* Interface Protocol*/
    BULK_ONLY_PROTOCOL, 
    /* Interface Description String Index*/
    0x00, 

    /*Endpoint descriptor */
    ENDP_ONLY_DESC_SIZE,
    USB_ENDPOINT_DESCRIPTOR,
    BULK_IN_ENDPOINT|(USB_SEND << 7),
    USB_BULK_PIPE, 
    USB_uint_16_low(BULK_IN_ENDP_PACKET_SIZE),
    USB_uint_16_high(BULK_IN_ENDP_PACKET_SIZE),
    0x00,/* This value is ignored for Bulk ENDPOINT */
    
    /*Endpoint descriptor */
    ENDP_ONLY_DESC_SIZE,
    USB_ENDPOINT_DESCRIPTOR,
    BULK_OUT_ENDPOINT|(USB_RECV << 7),
    USB_BULK_PIPE, 
    USB_uint_16_low(BULK_OUT_ENDP_PACKET_SIZE),
    USB_uint_16_high(BULK_OUT_ENDP_PACKET_SIZE),
    0x00 /* This value is ignored for Bulk ENDPOINT */
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
       
        /* INTERFACE DESCRIPTOR */  
        IFACE_ONLY_DESC_SIZE,
        USB_IFACE_DESCRIPTOR,
        /* bInterfaceNumber */
        0x00,
        /* bAlternateSetting */
        0x00,
        (uint8_t)MSC_DESC_ENDPOINT_COUNT,     
        /* Interface Class */     
        MASS_STORAGE_CLASS,
          /* Interface Subclass*/     
        SCSI_TRANSPARENT_COMMAND_SET,
        /* Interface Protocol*/
        BULK_ONLY_PROTOCOL, 
        /* Interface Description String Index*/
        0x00, 

        /*Endpoint descriptor */
        ENDP_ONLY_DESC_SIZE,
        USB_ENDPOINT_DESCRIPTOR,
        BULK_IN_ENDPOINT|(USB_SEND << 7),
        USB_BULK_PIPE, 
        USB_uint_16_low(OTHER_SPEED_BULK_IN_ENDP_PACKET_SIZE), 
        USB_uint_16_high(OTHER_SPEED_BULK_IN_ENDP_PACKET_SIZE),
        0x00,/* This value is ignored for Bulk ENDPOINT */
        
        /*Endpoint descriptor */
        ENDP_ONLY_DESC_SIZE,
        USB_ENDPOINT_DESCRIPTOR,
        BULK_OUT_ENDPOINT|(USB_RECV << 7),
        USB_BULK_PIPE,      
        USB_uint_16_low(OTHER_SPEED_BULK_OUT_ENDP_PACKET_SIZE), 
        USB_uint_16_high(OTHER_SPEED_BULK_OUT_ENDP_PACKET_SIZE),
        0x00 /* This value is ignored for Bulk ENDPOINT */
    };
#endif

uint8_t USB_STR_0[USB_STR_0_SIZE+USB_STR_DESC_SIZE] =     
    { sizeof(USB_STR_0),    
      USB_STRING_DESCRIPTOR, 
      0x09,
      0x04/*equiavlent to 0x0409*/ 
    };
                                    
/*  Manufacturer string */                                    
uint8_t USB_STR_1[USB_STR_1_SIZE+USB_STR_DESC_SIZE] = 
    {  sizeof(USB_STR_1),          
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

/*  Product string */
uint8_t USB_STR_2[USB_STR_2_SIZE+USB_STR_DESC_SIZE] =        
    {  sizeof(USB_STR_2),          
       USB_STRING_DESCRIPTOR,                                
       'M',0,
       'Q',0,
       'X',0,
       ' ',0,
       'M',0,
       'A',0,
       'S',0,
       'S',0,
       ' ',0,
       'S',0,
       'T',0,
       'O',0,
       'R',0,
       'A',0,
       'G',0,
       'E',0
    };

/*  Serial number string */
uint8_t USB_STR_3[USB_STR_3_SIZE+USB_STR_DESC_SIZE] = 
    {  sizeof(USB_STR_3),          
       USB_STRING_DESCRIPTOR,                                
       '0',0,
       '1',0,
       '2',0,
       '3',0,
       '4',0,
       '5',0,
       '6',0,
       '7',0,
       '8',0,
       '9',0,
       'A',0,
       'B',0,
       'C',0,
       'D',0,
       'E',0,
       'F',0
    };                 

uint8_t USB_STR_n[USB_STR_n_SIZE+USB_STR_DESC_SIZE] =
    {  sizeof(USB_STR_n),         
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
    { 0,
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
    { sizeof(USB_STR_0),
      sizeof(USB_STR_1),
      sizeof(USB_STR_2),
      sizeof(USB_STR_3),
      sizeof(USB_STR_n)
    };   
                                             
uint8_t *g_string_descriptors[USB_MAX_STRING_DESCRIPTORS+1] = 
    { USB_STR_0,
      USB_STR_1,
      USB_STR_2,
      USB_STR_3,
      USB_STR_n
    };    

USB_LANGUAGE usb_language[USB_MAX_SUPPORTED_INTERFACES] = 
{ (uint16_t)0x0409,g_string_descriptors, g_string_desc_size};
                                                                                                   
USB_ALL_LANGUAGES g_languages = 
    { USB_STR_0, sizeof(USB_STR_0), 
      usb_language 
    };

uint8_t g_valid_config_values[USB_MAX_CONFIG_SUPPORTED+1] = {0,1};

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
 * Local Variables
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
   NULL,
   NULL 
};
 
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
uint8_t USB_Desc_Get_Descriptor
(
    uint32_t handle, // this is not of type _usb_device_handle because of some bug in CW 7.1 RegABI
    uint8_t type,
    uint8_t str_num, 
    uint16_t index,
    uint8_t **descriptor,
    uint32_t *size
) 
{
    UNUSED_ARGUMENT (handle)
    
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
            *descriptor = (uint8_t *)
                g_languages.usb_language[lang_id].lang_desc[lang_index];
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
uint8_t USB_Desc_Get_Interface
(
    uint32_t handle, 
    uint8_t interface, 
    uint8_t *alt_interface
)
{   
    UNUSED_ARGUMENT (handle)
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
uint8_t USB_Desc_Set_Interface
(
    uint32_t handle, 
    uint8_t interface, 
    uint8_t alt_interface
)
{
    UNUSED_ARGUMENT (handle)
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
bool USB_Desc_Valid_Configation(uint32_t handle,uint16_t config_val)
{
    uint8_t loop_index=0;
    UNUSED_ARGUMENT (handle)
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
    UNUSED_ARGUMENT (handle)
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
    UNUSED_ARGUMENT (handle) 
    return &usb_desc_ep;
}         
/* EOF */
