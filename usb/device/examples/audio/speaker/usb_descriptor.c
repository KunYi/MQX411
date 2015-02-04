/*HEADER**********************************************************************
*
* Copyright 2010 Freescale Semiconductor, Inc.
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
 * @brief The file contains USB descriptors for Audio Application
* 
*END************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <mqx.h>
#include <usb_types.h>
#include "usb_class.h"
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
USB_EP_STRUCT ep[AUDIO_DESC_ENDPOINT_COUNT] =
{
   {
      AUDIO_ISOCHRONOUS_ENDPOINT,
      USB_ISOCHRONOUS_PIPE,
      USB_RECV,
      AUDIO_ENDPOINT_PACKET_SIZE
   }
};
USB_ENDPOINTS usb_desc_ep =
{
   AUDIO_DESC_ENDPOINT_COUNT,
   ep
};

/* *********************************************************************
* definition a struct of Input/output or Feature Unit
************************************************************************ */

 /* Struct of Terninal Input /Output or Feature Unit */

USB_UT_STRUCT ut[AUDIO_UNIT_COUNT] =
{
   {0x01,AUDIO_CONTROL_INPUT_TERMINAL},
   {0x02,AUDIO_CONTROL_FEATURE_UNIT},
   {0x03,AUDIO_CONTROL_OUTPUT_TERMINAL},
};

USB_AUDIO_UNITS usb_audio_unit =
{
   AUDIO_UNIT_COUNT,
   ut
} ;

/* ******************************* END ******************************** */

uint8_t g_device_descriptor[DEVICE_DESCRIPTOR_SIZE] =
{
   DEVICE_DESCRIPTOR_SIZE,               /* "Device Descriptor Size        */
   USB_DEVICE_DESCRIPTOR,                /* "Device" Type of descriptor    */
   0x00,0x01,                            /*  BCD USB version               */
   0x00,                                 /*  Device Class is indicated in
                                             the interface descriptors     */
   0x00,                                 /*  Device Subclass is indicated
                                             in the interface descriptors  */
   0x00,                                 /*  Device Protocol               */
   CONTROL_MAX_PACKET_SIZE,              /*  Max Packet size               */
   0x04,0x25,                            /*  Vendor ID */
   0x00, 0x02,                           /*  Product ID */
   0x00, 0x01,                           /*  BCD Device version */
   0x01,                                 /*  Manufacturer string index     */
   0x02,                                 /*  Product string index          */
   0x00,                                 /*  Serial number string index    */
   0x01                                  /*  Number of configurations      */
};

uint8_t g_config_descriptor[CONFIG_DESC_SIZE] =
{
   CONFIG_ONLY_DESC_SIZE,           /*  Configuration Descriptor Size - always 9 bytes*/
   USB_CONFIG_DESCRIPTOR,           /* "Configuration" type of descriptor */
   CONFIG_DESC_SIZE, 0x00,          /*  Total length of the Configuration descriptor */
   0x02,                            /*  NumInterfaces */
   0x01,                            /*  Configuration Value */
   0,                               /*  Configuration Description String Index*/
   BUS_POWERED,
   /* S08/CFv1 are both self powered (its compulsory to set bus powered)*/
   /*Attributes.support RemoteWakeup and self power*/
   0x32,                            /*  Current draw from bus */

   /* AUDIO CONTROL INTERFACE DISCRIPTOR */
   IFACE_ONLY_DESC_SIZE,            /* Size of this descriptor*/
   USB_IFACE_DESCRIPTOR,            /* INTERFACE descriptor */
   0x00,                            /* Index of this interface*/
   0x00,                            /* Index of this setting*/
   0x00,                            /* 0 endpoint */
   USB_DEVICE_CLASS_AUDIO,          /* AUDIO */
   USB_SUBCLASS_AUDIOCONTROL,       /* AUDIO_CONTROL */
   0x00,                            /* Unused */
   0x00,                            /* Unused */

   /* Audio class-specific interface header */
   HEADER_ONLY_DESC_SIZE,           /* bLength (9) */
   AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */
   AUDIO_CONTROL_HEADER,            /* bDescriptorSubtype (HEADER) */
   0x00,0x01,                       /* bcdADC (1.0) */
   0x27,0x00,                       /* wTotalLength (43) */
   0x01,                            /* bInCollection (1 streaming interface) */
   0x01,                            /* baInterfaceNr (interface 1 is stream) */

  /* Audio class-specific input terminal */
   INPUT_TERMINAL_ONLY_DESC_SIZE,   /* bLength (12) */
   AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */
   AUDIO_CONTROL_INPUT_TERMINAL,    /* bDescriptorSubtype (INPUT_TERMINAL) */
   0x01,                            /* bTerminalID (1) */
   0x01,0x01,                       /* wTerminalType (radio receiver) */
   0x00,                            /* bAssocTerminal (none) */
   0x01,                            /* bNrChannels (2) */
   0x00,0x00,                       /* wChannelConfig (left, right) */
   0x00,                            /* iChannelNames (none) */
   0x00,                            /* iTerminal (none) */

  /* Audio class-specific feature unit */
   FEATURE_UNIT_ONLY_DESC_SIZE,     /* bLength (9) */
   AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */
   AUDIO_CONTROL_FEATURE_UNIT,      /* bDescriptorSubtype (FEATURE_UNIT) */
   0x02,                            /* bUnitID (2) */
   0x01,                            /* bSourceID (input terminal 1) */
   0x01,                            /* bControlSize (1 bytes) */
   0x03,
   0x00,                            /* Master controls */
   0x00,                            /* Channel 0 controls */

   /* Audio class-specific output terminal   */
   OUTPUT_TERMINAL_ONLY_DESC_SIZE,  /* bLength (9) */
   AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */
   AUDIO_CONTROL_OUTPUT_TERMINAL,   /* bDescriptorSubtype (OUTPUT_TERMINAL) */
   0x03,                            /* bTerminalID (3) */
   0x01,0x03,                       /* wTerminalType (USB streaming) */
   0x00,                            /* bAssocTerminal (none) */
   0x02,                            /* bSourceID (feature unit 2) */
   0x00,                            /* iTerminal (none) */

    /* USB speaker standard AS interface descriptor - audio streaming operational
(Interface 1, Alternate Setting 0) */
   IFACE_ONLY_DESC_SIZE,            /* bLength (9) */
   USB_IFACE_DESCRIPTOR,            /* bDescriptorType (CS_INTERFACE) */
   0x01,                            /* interface Number: 1 */
   0x00,                            /* Alternate Setting: 0 */
   0x00,                            /* not used (Zero Bandwidth) */
   USB_DEVICE_CLASS_AUDIO,          /* USB DEVICE CLASS AUDIO */
   USB_SUBCLASS_AUDIOSTREAM,        /* AUDIO SUBCLASS AUDIOSTREAMING */
   0x00,                            /* AUDIO PROTOCOL UNDEFINED */
   0x00,                            /* Unused */

    /* USB speaker standard AS interface descriptor - audio streaming operational
(Interface 1, Alternate Setting 1) */
   IFACE_ONLY_DESC_SIZE,            /* bLength (9) */
   USB_IFACE_DESCRIPTOR,            /* bDescriptorType (CS_INTERFACE) */
   0x01,                            /* interface Number: 1 */
   0x01,                            /* Alternate Setting: 1 */
   0x01,                            /* One Endpoint. */
   USB_DEVICE_CLASS_AUDIO,          /* USB DEVICE CLASS AUDIO */
   USB_SUBCLASS_AUDIOSTREAM,        /* AUDIO SUBCLASS AUDIOSTREAMING */
   0x00,                            /* AUDIO PROTOCOL UNDEFINED */
   0x00,                            /* Unused */

    /* USB speaker standard General AS interface descriptor */
   AUDIO_STREAMING_IFACE_DESC_SIZE, /* bLength (7) */
   AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */
   AUDIO_STREAMING_GENERAL,         /* GENERAL subtype */
   0x01,                            /* Unit ID of output terminal */
   0x01,                            /* Interface delay */
   0x02,0x00,                       /* PCM format */

   /* USB speaker audio type I format interface descriptor */
   AUDIO_FORMAT_TYPE_I_DESC_SIZE,   /* bLength (11) */
   AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */
   AUDIO_STREAMING_FORMAT_TYPE,     /* DescriptorSubtype: AUDIO STREAMING FORMAT TYPE */
   AUDIO_FORMAT_TYPE_I,             /* Format Type: Type I */
   AUDIO_FORMAT_CHANNELS,           /* Number of Channels: one channel */
   AUDIO_FORMAT_SIZE,               /* SubFrame Size: one byte per audio subframe */
   AUDIO_FORMAT_BITS,               /* Bit Resolution: 8 bits per sample */
   0x01,                            /* One frequency supported */
//   0x40, 0x1F,0x00,                 /* 8 kHz */
   0x80, 0x3E,0x00,                 /* 16 kHz */
//   0x80,0xBB,0x00,                  /* 48 kHz */
//   0x00, 0xFA,0x00,               /* 72 kHz */

   /*Endpoint 1 - standard descriptor*/
   ENDP_ONLY_DESC_SIZE,             /* bLength (9) */
   USB_ENDPOINT_DESCRIPTOR,         /* Descriptor type (endpoint descriptor) */
   0x02,                            /* OUT endpoint address 1 */
   0x01,                            /* Isochronous endpoint */
   0x40, 0x00,                      /* size of packet: 64 Bytes */
#if (HIGH_SPEED_DEVICE)
   0x04,                            /* 1 frames (FS) or microframes (HS) */
#else
   0x01,                            /* 1 frames (FS) or microframes (HS) */
#endif
   0x00,                            /* Unused */
   0x00,                            /* Unused */

   /* Endpoint 1 - Audio streaming descriptor */
   AUDIO_STREAMING_ENDP_DESC_SIZE,  /* bLength (7) */
   USB_AUDIO_DESCRIPTOR,            /* AUDIO ENDPOINT DESCRIPTOR TYPE */
   AUDIO_ENDPOINT_GENERAL,          /* AUDIO ENDPOINT GENERAL */
   0x00,                            /* bmAttributes: 0x80 */
   0x00,                            /* unused */
   0x00,0x00,                       /* unused */
};

uint8_t USB_STR_0[USB_STR_0_SIZE+USB_STR_DESC_SIZE] =
{
   sizeof(USB_STR_0),
   USB_STRING_DESCRIPTOR,
   0x09,
   0x04 /*equiavlent to 0x0409*/
};

uint8_t USB_STR_1[USB_STR_1_SIZE+USB_STR_DESC_SIZE] =
{
   sizeof(USB_STR_1),
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

uint8_t USB_STR_2[USB_STR_2_SIZE+USB_STR_DESC_SIZE] =
{
   sizeof(USB_STR_2),
   USB_STRING_DESCRIPTOR,
   'U',0,
   'S',0,
   'B',0,
   ' ',0,
   'A',0,
   'U',0,
   'D',0,
   'I',0,
   'O',0,
   ' ',0,
   'D',0,
   'E',0,
   'M',0,
   'O',0,
};

uint8_t USB_STR_n[USB_STR_n_SIZE+USB_STR_DESC_SIZE] =
{
   sizeof(USB_STR_n),
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
   0, /* Interface */
   0, /* Endpoint */
   0, /* Device Qualifier */
   0, /* other speed config */
   0
};

uint8_t *g_std_descriptors[USB_MAX_STD_DESCRIPTORS+1] =
{
   NULL,
   g_device_descriptor,
   g_config_descriptor,
   NULL, /* string */
   NULL, /* Interface */
   NULL, /* Endpoint */
   NULL, /* Device Qualifier */
   NULL, /* other speed config*/
   NULL
};

uint8_t g_string_desc_size[USB_MAX_STRING_DESCRIPTORS] =
{
   sizeof(USB_STR_0),
   sizeof(USB_STR_1),
   sizeof(USB_STR_2),
   sizeof(USB_STR_n)
};

uint8_t *g_string_descriptors[USB_MAX_STRING_DESCRIPTORS+1] =
{
   USB_STR_0,
   USB_STR_1,
   USB_STR_2,
   USB_STR_n
};

USB_LANGUAGE usb_lang[USB_MAX_SUPPORTED_INTERFACES] =
{
   (uint16_t)0x0409,
   g_string_descriptors,
   g_string_desc_size
};

USB_ALL_LANGUAGES g_languages =
{
   USB_STR_0, sizeof(USB_STR_0),
   usb_lang
};

uint8_t g_valid_config_values[USB_MAX_CONFIG_SUPPORTED+1]={0,1};

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
 * @param controller_ID : Controller ID
 * @param type          : Type of descriptor requested
 * @param sub_type      : String index for string descriptor
 * @param index         : String descriptor language Id
 * @param descriptor    : Output descriptor pointer
 * @param size          : Size of descriptor returned
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 * This function is used to pass the pointer of the requested descriptor
 *****************************************************************************/
uint8_t USB_Desc_Get_Descriptor
(
   uint32_t handle,
   uint8_t type,
   uint8_t str_num,
   uint16_t index,
   uint8_t **descriptor,
   uint32_t *size
)
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
            {
               /* check for max descriptors */
               if(str_num < USB_MAX_STRING_DESCRIPTORS)
               {
                  /* setup index for the string to be returned */
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
 * @param controller_ID : Controller ID
 * @param interface     : Interface number
 * @param alt_interface : Output alternate interface
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 *This function is called by the framework module to get the current interface
 *****************************************************************************/
uint8_t USB_Desc_Get_Interface
(
   uint32_t handle,
   uint8_t interface,
   uint8_t *alt_interface
)
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
 * @param handle        : handle
 * @param interface     : Interface number
 * @param alt_interface : Input alternate interface
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 *This function is called by the framework module to set the interface
 *****************************************************************************/
uint8_t USB_Desc_Set_Interface
(
   uint32_t handle,
   uint8_t interface,
   uint8_t alt_interface
)
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
 * @param handle        : handle
 * @param config_val    : Configuration value
 *
 * @return TRUE           When Valid
 *         FALSE          When Error
 *****************************************************************************
 * This function checks whether the configuration is valid or not
 *****************************************************************************/
bool USB_Desc_Valid_Configation
(
   uint32_t handle,/*[IN] Controller ID */
   uint16_t config_val   /*[IN] Configuration value */
)
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
 * @name  USB_Desc_Valid_Interface
 *
 * @brief The function checks whether the interface parameter
 *        input is valid or not
 *
 * @param handle        : handle
 * @param interface     : Target interface
 *
 * @return TRUE           When Valid
 *         FALSE          When Error
 *****************************************************************************
 * This function checks whether the interface is valid or not
 *****************************************************************************/
bool USB_Desc_Valid_Interface
(
   uint32_t handle,       /*[IN] Controller ID */
   uint8_t interface      /*[IN] Target interface */
)
{
   uint8_t loop_index=0;
   UNUSED_ARGUMENT(handle)
   /* check with only supported val right now */
   while(loop_index < USB_MAX_SUPPORTED_INTERFACES)
   {
      if(interface == g_alternate_interface[loop_index])
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
 * @param handle : handle
 *
 * @return REMOTE_WAKEUP_SUPPORT (TRUE) - If remote wakeup supported
 *****************************************************************************
 * This function returns remote wakeup is supported or not
 *****************************************************************************/
bool USB_Desc_Remote_Wakeup
(
   uint32_t handle      /* [IN] Controller ID */
)
{
   UNUSED_ARGUMENT(handle)
   return REMOTE_WAKEUP_SUPPORT;
}

/* ****************************************************************************
 * Local avaiable
 * Stock Array value of Paramater controller
 ******************************************************************************/

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Endpoints
 *
 * @brief The function returns with the list of all non control endpoints used
 *
 * @param handle : handle
 *
 * @return pointer to USB_ENDPOINTS
 *****************************************************************************
 * This function returns the information about all the non control endpoints
 * implemented
 *****************************************************************************/
USB_ENDPOINTS* USB_Desc_Get_Endpoints(uint32_t handle)
{
   return &usb_desc_ep;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Entities
 *
 * @brief The function returns with the list of all non control endpoints used
 *
 * @param handle : handle
 *
 * @return pointer to USB_ENTITIES
 *****************************************************************************
 * This Function returns the infomation about all the Input or Output of
 * Interface or Feature Unit
 * *************************************************************************** */

USB_AUDIO_UNITS* USB_Desc_Get_Entities(uint32_t handle)
{
   return &usb_audio_unit;
}

uint8_t g_copy_protect=0x01;
/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Copy_Protect
 *
 * @brief  This function is called in response to Set Terminal Control Request
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Calls to Set Copy Protect Level
 *****************************************************************************/
uint8_t USB_Desc_Set_Copy_Protect
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set copy protect data*/
      g_copy_protect = **coding_data;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Copy_Protect
 *
 * @brief  This function is called in response to Get Terminal Control Request
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Return Copy Protect Level
 *****************************************************************************/
uint8_t USB_Desc_Get_Copy_Protect
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data=&g_copy_protect;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_mute = 0x01;
/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Cur_Mute
 *
 * @brief  This function is called to Set Current Mute Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets current Mute state specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Cur_Mute
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set cur mute data*/
      g_cur_mute = **coding_data;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Mute
 *
 * @brief  This function is called to Get Current Mute Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Mute state to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Cur_Mute
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data = &g_cur_mute;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_volume[2] = {0x00,0x80};
uint8_t g_min_volume[2] = {0x00,0x80};
uint8_t g_max_volume[2] = {0xFF,0x7F};
uint8_t g_res_volume[2] = {0x01,0x00};
/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Cur_Volume
 *
 * @brief  This function is called to Set Current Volume Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets current Volume value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Cur_Volume
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set current volume data*/
      g_cur_volume[0] = **coding_data;
      g_cur_volume[1] = *(*coding_data+1);
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Min_Volume
 *
 * @brief  This function is called to Set Min Volume Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Minimum Volume value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Min_Volume
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set min volume data*/
      g_min_volume[0] = **coding_data;
      g_min_volume[1] = *(*coding_data+1);
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Set_Max_Volume
 *
 * @brief  This function is called to Set Max Volume Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Maximum Volume value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Max_Volume
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set max volume data*/
      g_max_volume[0] = **coding_data;
      g_max_volume[1] = *(*coding_data+1);
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Res_Volume
 *
 * @brief  This function is called to Set Resolution Volume Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Resolution Volume value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Res_Volume
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)

   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set resolution volume data*/
      g_res_volume[0] = **coding_data;
      g_res_volume[1] = *(*coding_data+1);
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Cur_Volume
 *
 * @brief  This function is called to Get Current Volume Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Volume value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Cur_Volume
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data = g_cur_volume;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Min_Volume
 *
 * @brief  This function is called to Get Min Volume Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Min Volume value to the Host
 *****************************************************************************/
 uint8_t USB_Desc_Get_Min_Volume
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data = g_min_volume;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Max_Volume
 *
 * @brief  This function is called to Get Max Volume Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Max Volume value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Max_Volume
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data = g_max_volume;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Res_Volume
 *
 * @brief  This function is called to Get Resolution Volume Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Res Volume value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Res_Volume
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data = g_res_volume;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_bass = 0x00;
uint8_t g_min_bass = 0x80;
uint8_t g_max_bass = 0x7F;
uint8_t g_res_bass = 0x01;
/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Cur_Bass
 *
 * @brief  This function is called to Set Current Bass Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Bass value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Cur_Bass
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set current bass data*/
      g_cur_bass = **coding_data;
      printf("bass: %d\n", g_cur_bass);
      return USB_OK;
   }

   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Min_Bass
 *
 * @brief  This function is called to Set Min Bass Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Minimum Bass value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Min_Bass
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set min bass data*/
      g_min_bass = **coding_data;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Max_Bass
 *
 * @brief  This function is called to Set Max Bass Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Maximum Bass value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Max_Bass
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set max bass data*/
      g_max_bass = **coding_data;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Res_Bass
 *
 * @brief  This function is called to Set Resolution Bass Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Resolution Bass value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Res_Bass
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set resolution bass data*/
      g_res_bass = **coding_data;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Cur_Bass
 *
 * @brief  This function is called to Get Current Bass Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Bass value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Cur_Bass
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data = &g_cur_bass;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Min_Bass
 *
 * @brief  This function is called to Get Min Bass Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Minimum Bass value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Min_Bass
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data = &g_min_bass;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Max_Bass
 *
 * @brief  This function is called to Get Max Bass Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Maximum Bass value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Max_Bass
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data = &g_max_bass;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Res_Bass
 *
 * @brief  This function is called to Get Resolution Bass Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Resolution Bass value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Res_Bass
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data = &g_res_bass;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_mid =0x00;
uint8_t g_min_mid =0x80;
uint8_t g_max_mid =0x7F;
uint8_t g_res_mid =0x01;
/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Cur_Mid
 *
 * @brief  This function is called to Set Current Mid Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Mid value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Cur_Mid
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set current mid data*/
      g_cur_mid = **coding_data;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Min_Mid
 *
 * @brief  This function is called to Set Min Mid Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Minimum Mid value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Min_Mid
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set min mid data*/
      g_min_mid = **coding_data;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Max_Mid
 *
 * @brief  This function is called to Set Max Mid Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Maximum Mid value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Max_Mid
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set max mid data*/
      g_max_mid = **coding_data;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Res_Mid
 *
 * @brief  This function is called to Set Resolution Mid Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Resolution Mid value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Res_Mid
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set resolution mid data*/
      g_res_mid = **coding_data;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Cur_Mid
 *
 * @brief  This function is called to Get Current Mid Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Mid value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Cur_Mid
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* get current data data*/
      *coding_data=&g_cur_mid;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Min_Mid
 *
 * @brief  This function is called to Get Min Mid Value *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Minimum Mid value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Min_Mid
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface<USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data = &g_min_mid;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Max_Mid
 *
 * @brief  This function is called to Get Max Mid Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Maximum Mid value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Max_Mid
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface<USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data = &g_max_mid;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Res_Mid
 *
 * @brief  This function is called to Get Resolution Mid Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Resolution Mid value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Res_Mid
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface<USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data = &g_res_mid;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_treble = 0x01;
uint8_t g_min_treble = 0x80;
uint8_t g_max_treble = 0x7F;
uint8_t g_res_treble = 0x01;
 /**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Cur_Treble
 *
 * @brief  This function is called to Set Current Treble Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Treble value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Cur_Treble
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)

   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set current treble data*/
      g_cur_treble = **coding_data;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Min_Treble
 *
 * @brief  This function is called to Set Min Treble Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Minimum Treble value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Min_Treble
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set min treble data*/
      g_min_treble = **coding_data;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Max_Treble
 *
 * @brief  This function is called to Set Max Treble Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Maximum Treble value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Max_Treble
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set max treble data*/
      g_max_treble = **coding_data;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Res_Treble
 *
 * @brief  This function is called to Set Resolution Treble Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Resolution Treble value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Res_Treble
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set resolution treble data*/
      g_res_treble = **coding_data;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Cur_Treble
 *
 * @brief  This function is called to Get Current Treble Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Treble value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Cur_Treble
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data=&g_cur_treble;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Min_Treble
 *
 * @brief  This function is called to Get Min Treble Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Minimum Treble value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Min_Treble
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data=&g_min_treble;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Max_Treble
 *
 * @brief  This function is called to Get Max Treble Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Maximum Treble value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Max_Treble
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data=&g_max_treble;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Res_Treble
 *
 * @brief  This function is called to Get Resolution Treble Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Resolution Treble value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Res_Treble
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data=&g_res_treble;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_graphic_equalizer[4] = {0x01,0x01,0x01,0x01};
uint8_t g_min_graphic_equalizer[4] = {0x01,0x01,0x01,0x01};
uint8_t g_max_graphic_equalizer[4] = {0x01,0x01,0x01,0x01};
uint8_t g_res_graphic_equalizer[4] = {0x01,0x01,0x01,0x01};
/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Cur_Graphic_Equalizer
 *
 * @brief  This function is called to Set Current Graphic Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Graphic Equalizer values specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Cur_Graphic_Equalizer
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set current graphic equalizer data*/
      g_cur_graphic_equalizer[0] = **coding_data;
      g_cur_graphic_equalizer[1] = *(*coding_data+1);
      g_cur_graphic_equalizer[2] = *(*coding_data+2);
      g_cur_graphic_equalizer[3] = *(*coding_data+3);
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}
/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Min_Graphic_Equalizer
 *
 * @brief  This function is called to Set Min Graphic Equalizer Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Minimum Graphic Equalizer values specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Min_Graphic_Equalizer
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set min graphic equalizer data*/
      g_min_graphic_equalizer[0] = **coding_data;
      g_min_graphic_equalizer[1] = *(*coding_data+1);
      g_min_graphic_equalizer[2] = *(*coding_data+2);
      g_min_graphic_equalizer[3] = *(*coding_data+3);
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Max_Graphic_Equalizer
 *
 * @brief  This function is called to Set Max Graphic Equalizer Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Maximum Graphic Equalizer values specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Max_Graphic_Equalizer
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set max graphic equalizer data*/
      g_max_graphic_equalizer[0] = **coding_data;
      g_max_graphic_equalizer[1] = *(*coding_data+1);
      g_max_graphic_equalizer[2] = *(*coding_data+2);
      g_max_graphic_equalizer[3] = *(*coding_data+3);
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Res_Graphic_Equalizer
 *
 * @brief  This function is called to Set Max Graphic Equalizer Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Resolution Graphic Equalizer values specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Res_Graphic_Equalizer
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set resolution graphic equalizer data*/
      g_res_graphic_equalizer[0] = **coding_data;
      g_res_graphic_equalizer[1] = *(*coding_data+1);
      g_res_graphic_equalizer[2] = *(*coding_data+2);
      g_res_graphic_equalizer[3] = *(*coding_data+3);
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Cur_Graphic_Equalizer
 *
 * @brief  This function is called to Get Current Graphic Equalizer Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Graphic Equalizer values to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Cur_Graphic_Equalizer
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data=g_cur_graphic_equalizer;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Min_Graphic_Equalizer
 *
 * @brief  This function is called to Get Min Graphic Equalizer Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Minimum Graphic Equalizer values to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Min_Graphic_Equalizer
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data=g_min_graphic_equalizer;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Max_Graphic_Equalizer
 *
 * @brief  This function is called to Get Max Graphic Equalizer Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Maximum Graphic Equalizer values to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Max_Graphic_Equalizer
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES){
      *coding_data=g_max_graphic_equalizer;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Res_Graphic_Equalizer
 *
 * @brief  This function is called to Get Resolution Graphic Equalizer Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Resolution Graphic Equalizer values to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Res_Graphic_Equalizer
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data=g_res_graphic_equalizer;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_automatic_gain = 0x01;
/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Cur_Automatic_Gain
 *
 * @brief  This function is called to Set Current Automatic Gain Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Automatic Gain value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Cur_Automatic_Gain
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set current automatic data*/
      g_cur_automatic_gain = **coding_data;
      printf("AGC: %d\n", g_cur_automatic_gain);
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Cur_Automatic_Gain
 *
 * @brief  This function is called to Get Current Automatic Gain Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Automatic Gain value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Cur_Automatic_Gain
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data=&g_cur_automatic_gain;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_delay[2] = {0x00,0x40};
uint8_t g_min_delay[2] = {0x00,0x00};
uint8_t g_max_delay[2] = {0xFF,0xFF};
uint8_t g_res_delay[2] = {0x00,0x01};
/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Cur_Delay
 *
 * @brief  This function is called to Set Current Delay Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Delay value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Cur_Delay
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)

   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set current delay data*/
      g_cur_delay[0] = **coding_data;
      g_cur_delay[1] = *(*coding_data+1);
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Min_Delay
 *
 * @brief  This function is called to Set Min Delay Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Minimum Delay value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Min_Delay
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)

   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set min delay data*/
      g_min_delay[0] = **coding_data;
      g_min_delay[1] = *(*coding_data+1);
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Max_Delay
 *
 * @brief  This function is called to Set Max Delay Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Maximum Delay value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Max_Delay
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set max delay data*/
      g_max_delay[0] = **coding_data;
      g_max_delay[1] = *(*coding_data+1);
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Res_Delay
 *
 * @brief  This function is called to Set Resolution Delay Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Resolution Delay value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Res_Delay
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set resolution delay data*/
      g_res_delay[0] = **coding_data;
      g_res_delay[1] = *(*coding_data+1);
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Cur_Delay
 *
 * @brief  This function is called to Get Current Delay Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Delay value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Cur_Delay
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data=g_cur_delay;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Min_Delay
 *
 * @brief  This function is called to Get Min Delay Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Minimum Delay value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Min_Delay
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data=g_min_delay;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Max_Delay
 *
 * @brief  This function is called to Get Max Delay Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Maximum Delay value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Max_Delay
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data=g_max_delay;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Res_Delay
 *
 * @brief  This function is called to Get Resolution Delay Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Resolution Delay value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Res_Delay
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data=g_res_delay;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_bass_boost = 0x01;
/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Cur_Bass_Boost
 *
 * @brief  This function is called to Set Current Bass Boost Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Set Current Bass Boost value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Cur_Bass_Boost
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set current bass boost data*/
      g_cur_bass_boost = **coding_data;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Cur_Bass_Boost
 *
 * @brief  This function is called to Get Current Bass Boost Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Bass Boost value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Cur_Bass_Boost
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data=&g_cur_bass_boost;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_loudness = 0x01;
/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Cur_Loudness
 *
 * @brief  This function is called to Set Current Loudness Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Loudness value specified to the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Cur_Loudness
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      /* set current loudness data*/
      g_cur_loudness = **coding_data;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Cur_Loudness
 *
 * @brief  This function is called to Get Current Loudness Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Loudness value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Cur_Loudness
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   /* if interface valid */
   if(interface < USB_MAX_SUPPORTED_INTERFACES)
   {
      *coding_data=&g_cur_loudness;
      return USB_OK;
   }
   return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_sampling_frequency[3] = {0x00,0x00,0x01};
uint8_t g_min_sampling_frequency[3] = {0x00,0x00,0x01};
uint8_t g_max_sampling_frequency[3] = {0x00,0x00,0x01};
uint8_t g_res_sampling_frequency[3] = {0x00,0x00,0x01};
/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Cur_Sampling_Frequency
 *
 * @brief  The function sets Current Sampling Frequency value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return :
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Sampling Frequency value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Cur_Sampling_Frequency
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT (handle)
   UNUSED_ARGUMENT (interface)

   /* set current sampling fequency data*/
   g_cur_sampling_frequency[0] = **coding_data;
   g_cur_sampling_frequency[1] = *(*coding_data+1);
   g_cur_sampling_frequency[2] = *(*coding_data+2);
   return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Min_Sampling_Frequency
 *
 * @brief  The function sets Current Sampling Frequency value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return :
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Minimum Sampling Frequency value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Min_Sampling_Frequency
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT (handle)
   UNUSED_ARGUMENT (interface)

   /* set min sampling fequency data*/
   g_min_sampling_frequency[0] = **coding_data;
   g_min_sampling_frequency[1] = *(*coding_data+1);
   g_min_sampling_frequency[2] = *(*coding_data+2);
   return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Max_Sampling_Frequency
 *
 * @brief  The function sets Current Sampling Frequency value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return :
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Maximum Sampling Frequency value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Max_Sampling_Frequency
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT (handle)
   UNUSED_ARGUMENT (interface)

   /* set max sampling fequency data*/
   g_max_sampling_frequency[0] = **coding_data;
   g_max_sampling_frequency[1] = *(*coding_data+1);
   g_max_sampling_frequency[2] = *(*coding_data+2);
   return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Res_Sampling_Frequency
 *
 * @brief  The function sets Current Sampling Frequency value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return :
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Resolution Sampling Frequency value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Res_Sampling_Frequency
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT (handle)
   UNUSED_ARGUMENT (interface)

   /* set res sampling fequency data*/
   g_res_sampling_frequency[0] = **coding_data;
   g_res_sampling_frequency[1] = *(*coding_data+1);
   g_res_sampling_frequency[2] = *(*coding_data+2);
   return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Cur_Sampling_Frequency
 *
 * @brief  This function is called to Get Current sampling frequency Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current sampling frequency value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Cur_Sampling_Frequency
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   UNUSED_ARGUMENT(interface)

   *coding_data = g_cur_sampling_frequency;
   return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Min_Sampling_Frequency
 *
 * @brief  This function is called to Get Current sampling frequency Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Minimine sampling frequency value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Min_Sampling_Frequency
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   UNUSED_ARGUMENT(interface)

   *coding_data = g_min_sampling_frequency;
   return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Max_Sampling_Frequency
 *
 * @brief  This function is called to Get Current sampling frequency Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Maximine sampling frequency value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Max_Sampling_Frequency
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   UNUSED_ARGUMENT(interface)

   *coding_data = g_max_sampling_frequency;
   return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Res_Sampling_Frequency
 *
 * @brief  This function is called to Get Current sampling frequency Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Resolution sampling frequency value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Res_Sampling_Frequency
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   UNUSED_ARGUMENT(interface)

   *coding_data = g_res_sampling_frequency;
   return USB_OK;
}

uint8_t g_cur_pitch = 0x00;
/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Cur_Pitch
 *
 * @brief  This function is called to Set Current Pitch Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets current Pitch state specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Cur_Pitch
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   UNUSED_ARGUMENT(interface)

   /* set cur pitch data*/
   g_cur_pitch = **coding_data;
   return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Cur_Pitch
 *
 * @brief  This function is called to Get Current Pitch Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Pitch state to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Cur_Pitch
(
   uint32_t handle,
   uint8_t interface,
   uint8_t **coding_data
)
{
   UNUSED_ARGUMENT(handle)
   UNUSED_ARGUMENT(interface)

   *coding_data = &g_cur_pitch;
   return USB_OK;
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
uint8_t USB_Desc_Get_feature
(
   uint32_t handle,
   int32_t cmd,
   uint8_t in_data,
   uint8_t ** out_buf
)
{
   uint8_t error = USBERR_INVALID_REQ_TYPE;

   switch (cmd)
   {
      case GET_CUR_COPY_PROTECT_CONTROL:
         error = USB_Desc_Get_Copy_Protect(handle,in_data,out_buf);
         break;
      /* Get current value*/
      case GET_CUR_MUTE_CONTROL:
         error = USB_Desc_Get_Cur_Mute(handle,in_data,out_buf);
         break;
      case GET_CUR_VOLUME_CONTROL:
         error = USB_Desc_Get_Cur_Volume(handle,in_data,out_buf);
         break;
      case GET_CUR_BASS_CONTROL:
         error = USB_Desc_Get_Cur_Bass(handle,in_data,out_buf);
         break;
      case GET_CUR_MID_CONTROL:
         error = USB_Desc_Get_Cur_Mid(handle,in_data,out_buf);
         break;
      case GET_CUR_TREBLE_CONTROL:
         error = USB_Desc_Get_Cur_Treble(handle,in_data,out_buf);
         break;
      case GET_CUR_GRAPHIC_EQUALIZER_CONTROL:
         error = USB_Desc_Get_Cur_Graphic_Equalizer(handle,in_data,out_buf);
         break;
      case GET_CUR_AUTOMATIC_GAIN_CONTROL:
         error = USB_Desc_Get_Cur_Automatic_Gain(handle,in_data,out_buf);
         break;
      case GET_CUR_DELAY_CONTROL:
         error = USB_Desc_Get_Cur_Delay(handle,in_data,out_buf);
         break;
      case GET_CUR_BASS_BOOST_CONTROL:
         error = USB_Desc_Get_Cur_Bass_Boost(handle,in_data,out_buf);
         break;
      case GET_CUR_LOUDNESS_CONTROL:
         error = USB_Desc_Get_Cur_Loudness(handle,in_data,out_buf);
         break;
      /* GET_CUR endpoint */
      case GET_CUR_SAMPLING_FREQ_CONTROL:
         error = USB_Desc_Get_Cur_Sampling_Frequency(handle,in_data,out_buf);
         break;
      case GET_CUR_PITCH_CONTROL:
         error = USB_Desc_Get_Cur_Pitch(handle,in_data,out_buf);
         break;

      /* Get min value*/
      case GET_MIN_VOLUME_CONTROL:
         error = USB_Desc_Get_Min_Volume(handle,in_data,out_buf);
         break;
      case GET_MIN_BASS_CONTROL:
         error = USB_Desc_Get_Min_Bass(handle,in_data,out_buf);
         break;
      case GET_MIN_MID_CONTROL:
         error = USB_Desc_Get_Min_Mid(handle,in_data,out_buf);
         break;
      case GET_MIN_TREBLE_CONTROL:
         error = USB_Desc_Get_Min_Treble(handle,in_data,out_buf);
         break;
      case GET_MIN_GRAPHIC_EQUALIZER_CONTROL:
         error = USB_Desc_Get_Min_Graphic_Equalizer(handle,in_data,out_buf);
         break;
      case GET_MIN_DELAY_CONTROL:
         error = USB_Desc_Get_Min_Delay(handle,in_data,out_buf);
         break;
      case GET_MIN_SAMPLING_FREQ_CONTROL:
         error = USB_Desc_Get_Min_Sampling_Frequency(handle,in_data,out_buf);
         break;

      /* Get max value*/
      case GET_MAX_VOLUME_CONTROL:
         error = USB_Desc_Get_Max_Volume(handle,in_data,out_buf);
         break;
      case GET_MAX_BASS_CONTROL:
         error = USB_Desc_Get_Max_Bass(handle,in_data,out_buf);
         break;
      case GET_MAX_MID_CONTROL:
         error = USB_Desc_Get_Max_Mid(handle,in_data,out_buf);
         break;
      case GET_MAX_TREBLE_CONTROL:
         error = USB_Desc_Get_Max_Treble(handle,in_data,out_buf);
         break;
      case GET_MAX_GRAPHIC_EQUALIZER_CONTROL:
         error = USB_Desc_Get_Max_Graphic_Equalizer(handle,in_data,out_buf);
         break;
      case GET_MAX_DELAY_CONTROL:
         error = USB_Desc_Get_Max_Delay(handle,in_data,out_buf);
         break;
      case GET_MAX_SAMPLING_FREQ_CONTROL:
         error = USB_Desc_Get_Max_Sampling_Frequency(handle,in_data,out_buf);
         break;

      /* Get res value*/
      case GET_RES_VOLUME_CONTROL:
         error = USB_Desc_Get_Res_Volume(handle,in_data,out_buf);
         break;
      case GET_RES_BASS_CONTROL:
         error = USB_Desc_Get_Res_Bass(handle,in_data,out_buf);
         break;
      case GET_RES_MID_CONTROL:
         error = USB_Desc_Get_Res_Mid(handle,in_data,out_buf);
         break;
      case GET_RES_TREBLE_CONTROL:
         error = USB_Desc_Get_Res_Treble(handle,in_data,out_buf);
         break;
      case GET_RES_GRAPHIC_EQUALIZER_CONTROL:
         error = USB_Desc_Get_Res_Graphic_Equalizer(handle,in_data,out_buf);
         break;
      case GET_RES_DELAY_CONTROL:
         error = USB_Desc_Get_Res_Delay(handle,in_data,out_buf);
         break;
      case GET_RES_SAMPLING_FREQ_CONTROL:
         error = USB_Desc_Get_Res_Sampling_Frequency(handle,in_data,out_buf);
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
uint8_t USB_Desc_Set_feature
(
   uint32_t handle,
   int32_t cmd,
   uint8_t in_data,
   uint8_t ** in_buf
)
{
   uint8_t error = USBERR_INVALID_REQ_TYPE;

   switch (cmd)
   {
      case SET_CUR_COPY_PROTECT_CONTROL:
         error = USB_Desc_Set_Copy_Protect(handle,in_data,in_buf);
         break;
      /* Get current value*/
      case SET_CUR_MUTE_CONTROL:
         error = USB_Desc_Set_Cur_Mute(handle,in_data,in_buf);
         break;
      case SET_CUR_VOLUME_CONTROL:
         error = USB_Desc_Set_Cur_Volume(handle,in_data,in_buf);
         break;
      case SET_CUR_BASS_CONTROL:
         error = USB_Desc_Set_Cur_Bass(handle,in_data,in_buf);
         break;
      case SET_CUR_MID_CONTROL:
         error = USB_Desc_Set_Cur_Mid(handle,in_data,in_buf);
         break;
      case SET_CUR_TREBLE_CONTROL:
         error = USB_Desc_Set_Cur_Treble(handle,in_data,in_buf);
         break;
      case SET_CUR_GRAPHIC_EQUALIZER_CONTROL:
         error = USB_Desc_Set_Cur_Graphic_Equalizer(handle,in_data,in_buf);
         break;
      case SET_CUR_AUTOMATIC_GAIN_CONTROL:
         error = USB_Desc_Set_Cur_Automatic_Gain(handle,in_data,in_buf);
         break;
      case SET_CUR_DELAY_CONTROL:
         error = USB_Desc_Set_Cur_Delay(handle,in_data,in_buf);
         break;
      case SET_CUR_BASS_BOOST_CONTROL:
         error = USB_Desc_Set_Cur_Bass_Boost(handle,in_data,in_buf);
         break;
      case SET_CUR_LOUDNESS_CONTROL:
         error = USB_Desc_Set_Cur_Loudness(handle,in_data,in_buf);
         break;
      /* SET_CUR for endpoint */
      case SET_CUR_SAMPLING_FREQ_CONTROL:
         error = USB_Desc_Set_Cur_Sampling_Frequency(handle,in_data,in_buf);
         break;
      case SET_CUR_PITCH_CONTROL:
         error = USB_Desc_Set_Cur_Pitch(handle,in_data,in_buf);
         break;

      /* SET min value*/
      case SET_MIN_VOLUME_CONTROL:
         error = USB_Desc_Set_Min_Volume(handle,in_data,in_buf);
         break;
      case SET_MIN_BASS_CONTROL:
         error = USB_Desc_Set_Min_Bass(handle,in_data,in_buf);
         break;
      case SET_MIN_MID_CONTROL:
         error = USB_Desc_Set_Min_Mid(handle,in_data,in_buf);
         break;
      case SET_MIN_TREBLE_CONTROL:
         error = USB_Desc_Set_Min_Treble(handle,in_data,in_buf);
         break;
      case SET_MIN_GRAPHIC_EQUALIZER_CONTROL:
         error = USB_Desc_Set_Min_Graphic_Equalizer(handle,in_data,in_buf);
         break;
      case SET_MIN_DELAY_CONTROL:
         error = USB_Desc_Set_Min_Delay(handle,in_data,in_buf);
         break;
      case SET_MIN_SAMPLING_FREQ_CONTROL:
         error = USB_Desc_Set_Min_Sampling_Frequency(handle,in_data,in_buf);
         break;

      /* SET max value*/
      case SET_MAX_VOLUME_CONTROL:
         error = USB_Desc_Set_Max_Volume(handle,in_data,in_buf);
         break;
      case SET_MAX_BASS_CONTROL:
         error = USB_Desc_Set_Max_Bass(handle,in_data,in_buf);
         break;
      case SET_MAX_MID_CONTROL:
         error = USB_Desc_Set_Max_Mid(handle,in_data,in_buf);
         break;
      case SET_MAX_TREBLE_CONTROL:
         error = USB_Desc_Set_Max_Treble(handle,in_data,in_buf);
         break;
      case SET_MAX_GRAPHIC_EQUALIZER_CONTROL:
         error = USB_Desc_Set_Max_Graphic_Equalizer(handle,in_data,in_buf);
         break;
      case SET_MAX_DELAY_CONTROL:
         error = USB_Desc_Set_Max_Delay(handle,in_data,in_buf);
         break;
      case SET_MAX_SAMPLING_FREQ_CONTROL:
         error = USB_Desc_Set_Max_Sampling_Frequency(handle,in_data,in_buf);
         break;

      /* SET res value*/
      case SET_RES_VOLUME_CONTROL:
         error = USB_Desc_Set_Res_Volume(handle,in_data,in_buf);
         break;
      case SET_RES_BASS_CONTROL:
         error = USB_Desc_Set_Res_Bass(handle,in_data,in_buf);
         break;
      case SET_RES_MID_CONTROL:
         error = USB_Desc_Set_Res_Mid(handle,in_data,in_buf);
         break;
      case SET_RES_TREBLE_CONTROL:
         error = USB_Desc_Set_Res_Treble(handle,in_data,in_buf);
         break;
      case SET_RES_GRAPHIC_EQUALIZER_CONTROL:
         error = USB_Desc_Set_Res_Graphic_Equalizer(handle,in_data,in_buf);
         break;
      case SET_RES_DELAY_CONTROL:
         error = USB_Desc_Set_Res_Delay(handle,in_data,in_buf);
         break;
      case SET_RES_SAMPLING_FREQ_CONTROL:
         error = USB_Desc_Set_Res_Sampling_Frequency(handle,in_data,in_buf);
         break;
      default:
         error = USBERR_INVALID_REQ_TYPE;
         break;
   }
   return error;
}

/* EOF */
