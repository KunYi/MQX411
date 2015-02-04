/*HEADER**********************************************************************
*
* Copyright 2010 Freescale Semiconductor, Inc.
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
*   This file contains  the USB class driver mapping table
*
*
*END************************************************************************/


#include "hostapi.h"
#include "host_ch9.h"
#include "host_common.h"

/* get application-specific configuration */
#include "usb_classes.h"

#include "mqx_host.h"

#ifdef USBCLASS_INC_MASS
#include "usb_host_msd_bo.h"
#endif

#ifdef USBCLASS_INC_PRINTER
#include "usb_host_printer.h"
#endif

#ifdef USBCLASS_INC_HID
#include "usb_host_hid.h"
#endif

#ifdef USBCLASS_INC_CDC
#include "usb_host_cdc.h"
#endif

#ifdef USBCLASS_INC_AUDIO
#include "usb_host_audio.h"
#endif

#ifdef USBCLASS_INC_PHDC
#include "usb_host_phdc.h"
#endif

/* here hub is considered as device from host point of view */
#ifdef USBCLASS_INC_HUB
#include "usb_host_hub.h"
#endif

CLASS_MAP class_interface_map[] =
{
#ifdef USBCLASS_INC_MASS
   {
      usb_class_mass_init,
      sizeof(USB_MASS_CLASS_INTF_STRUCT),
      USB_CLASS_MASS_STORAGE,
      USB_SUBCLASS_MASS_UFI,
      USB_PROTOCOL_MASS_BULK,
      0xFF, 0x00, 0xFF
   },
#endif
#ifdef USBCLASS_INC_PRINTER
   {
      usb_printer_init,
      sizeof(PRINTER_INTERFACE_STRUCT),
      USB_CLASS_PRINTER,
      USB_SUBCLASS_PRINTER,
      USB_PROTOCOL_PRT_BIDIR,
      0xFF, 0xFF, 0xFF
   },
#endif
#ifdef USBCLASS_INC_HID
   {
      usb_class_hid_init,
      sizeof(USB_HID_CLASS_INTF_STRUCT),
      USB_CLASS_HID,
      0,
      0,
      0xFF, 0x00, 0x00
   },
#endif
#ifdef USBCLASS_INC_CDC
   {
      usb_class_cdc_acm_init,
      sizeof(USB_ACM_CLASS_INTF_STRUCT),
      USB_CLASS_COMMUNICATION,
      USB_SUBCLASS_COM_ABSTRACT,
      USB_PROTOCOL_COM_NOSPEC,
      0xFF, 0xFF, 0xFE
   },
   {
      usb_class_cdc_data_init,
      sizeof(USB_DATA_CLASS_INTF_STRUCT),
      USB_CLASS_DATA,
      0,
      0,
      0xFF, 0x00, 0x00
   },
#endif
#ifdef USBCLASS_INC_AUDIO
   {
      usb_class_audio_control_init,
      sizeof(AUDIO_CONTROL_INTERFACE_STRUCT),
      USB_CLASS_AUDIO,
      USB_SUBCLASS_AUD_CONTROL,
      0,
      0xFF, 0xFF, 0x00
   },
   {
      usb_class_audio_stream_init,
      sizeof(AUDIO_STREAM_INTERFACE_STRUCT),
      USB_CLASS_AUDIO,
      USB_SUBCLASS_AUD_STREAMING,
      0,
      0xFF, 0xFF, 0x00
   },
#endif
#ifdef USBCLASS_INC_PHDC
   {
      usb_class_phdc_init,
      sizeof(USB_PHDC_CLASS_INTF_STRUCT),
      USB_CLASS_PHDC,
      0,
      0,
      0xFF, 0x00, 0x00
   },
#endif   
#ifdef USBCLASS_INC_HUB
   {
      usb_class_hub_init,
      sizeof(USB_HUB_CLASS_INTF_STRUCT),
      USB_CLASS_HUB,
      USB_SUBCLASS_HUB_NONE,
      USB_PROTOCOL_HUB_FS,
      0xFF, 0x00, 0x00
   },
#endif
   {
      NULL,
      0,
      0, 0, 0,
      0, 0, 0
   }
};

/* EOF */
