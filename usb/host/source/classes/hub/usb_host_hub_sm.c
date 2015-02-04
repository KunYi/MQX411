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
*   This file contains the implementation of hub state machine on host.
*
*
*END************************************************************************/
#include "usb_host_hub.h"
#include "usb_host_hub_prv.h"
#include "usb_host_hub_sm.h"

static void usb_host_hub_device_sm(PIPE_STRUCT_PTR, void *, void *, uint32_t, USB_STATUS);
static void usb_host_hub_int_callback(PIPE_STRUCT_PTR, void *, void *, uint32_t, USB_STATUS);

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hub_device_event
* Returned Value : None
* Comments       :
*     called when a hub has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
void usb_host_hub_device_event
   (
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint32_t                          event_code
   )
{
    HUB_DEVICE_STRUCT_PTR hub_instance;

    switch (event_code) {
        case USB_CONFIG_EVENT:
            /* Drop through into attach, same processing */
        case USB_ATTACH_EVENT:
            /* Create 'unsafe' application struct */
            if (NULL != (hub_instance = USB_mem_alloc_zero(sizeof(HUB_DEVICE_STRUCT)))) {
                _usb_hostdev_select_interface(dev_handle, intf_handle, (void *)&hub_instance->CCS);
            }
            break;

        case USB_INTF_EVENT:
            if (USB_OK != usb_class_hub_get_app(dev_handle, intf_handle, (CLASS_CALL_STRUCT_PTR *) &hub_instance))
                break;

            /* set we are in process of getting hub descriptor */
            hub_instance->STATE = HUB_GET_DESCRIPTOR_TINY_PROCESS;

            /* here, we should retrieve information from the hub */
            usb_class_hub_get_descriptor(&hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, 7);
            break;

        case USB_DETACH_EVENT: {
            DEV_INSTANCE_PTR dev_ptr;
            _mqx_int  i;

            if (USB_OK != usb_class_hub_get_app(dev_handle, intf_handle, (CLASS_CALL_STRUCT_PTR *) &hub_instance))
                break;

            dev_ptr = (DEV_INSTANCE_PTR) dev_handle;
            /* detach all the devices connected to all the ports of found hub */
            for (i = 0; i < hub_instance->HUB_PORT_NR; i++) {
                if ((hub_instance->HUB_PORTS + i)->APP_STATUS & HUB_PORT_ATTACHED)
                    /* We will access dev_ptr without validation process (usb_hostdev_validate), because
                    ** USB_DETACH_EVENT is synced call, so we can do that. dev_ptr is still valid pointer to device.
                    */
                    usb_dev_list_detach_device(dev_ptr->host, dev_ptr->address, i + 1);
            }
            if (hub_instance->HUB_PORTS != NULL)
                USB_mem_free(hub_instance->HUB_PORTS);
            if (hub_instance != NULL) {
                USB_mem_free(hub_instance);
                hub_instance = NULL;
            }
            }
            break;

        default:
            break;
   } /* EndSwitch */

} /* Enbbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hub_device_sm
* Returned Value : None
* Comments       :
*     called when a hub changes state; sm = state machine
*END*--------------------------------------------------------------------*/
static void usb_host_hub_device_sm
   (
         /* [IN] structure with USB pipe information on the interface */
         PIPE_STRUCT_PTR   pipe,

         /* [IN] parameters */
         void             *param,

         /* [IN] buffer of data from IN stage */
         void             *buffer,

         /* [IN] length of data from IN stage */
         uint32_t           len,

         /* [IN] status of transaction */
         USB_STATUS        status
   )
{ /* Body */
    register HUB_DEVICE_STRUCT_PTR hub_instance = (HUB_DEVICE_STRUCT_PTR) param;
    _mqx_int                       i;

    switch (hub_instance->STATE) {
        case HUB_GET_DESCRIPTOR_TINY_PROCESS:

            /* here we get number of ports from USB data */
            hub_instance->HUB_PORT_NR = ((HUB_DESCRIPTOR_STRUCT_PTR) buffer)->BNRPORTS;
            hub_instance->STATE = HUB_GET_DESCRIPTOR_PROCESS;
            usb_class_hub_get_descriptor((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, 7 + hub_instance->HUB_PORT_NR / 8 + 1);
            break;

        case HUB_GET_DESCRIPTOR_PROCESS:
            {
                /* here, we get information from the hub and fill info in hub_instance */
                HUB_DESCRIPTOR_STRUCT_PTR hub_desc = (HUB_DESCRIPTOR_STRUCT_PTR) buffer;

                hub_instance->HUB_PORTS = USB_mem_alloc_zero(hub_instance->HUB_PORT_NR * sizeof(HUB_PORT_STRUCT));

                for (i = 0; i < hub_instance->HUB_PORT_NR; i++)
                {
                    /* get REMOVABLE bit from the desriptor for appropriate installed port */
                    (hub_instance->HUB_PORTS + i)->APP_STATUS = hub_desc->DEVICEREMOVABLE[(i + 1) / 8] & (0x01 << ((i + 1) % 8)) ? HUB_PORT_REMOVABLE : 0;
                }
            }

            /* pass fluently to HUB_SET_PORT_FEATURE_PROCESS */
            hub_instance->STATE = HUB_SET_PORT_FEATURE_PROCESS;
            hub_instance->port_iterator = 0;

        case HUB_SET_PORT_FEATURE_PROCESS:
//              if (hub_instance->port_iterator)
                /* register, that the port is now powered */
//                hub_instance->HUB_PORTS[hub_instance->port_iterator - 1].POWERED = 1;
            if (hub_instance->port_iterator < hub_instance->HUB_PORT_NR)
            {
                usb_class_hub_set_port_feature((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, ++hub_instance->port_iterator, PORT_POWER);
                break;
            } /* EndIf */
            /* else */

            /* pass fluently to HUB_CLEAR_PORT_FEATURE_PROCESS */
            hub_instance->STATE = HUB_CLEAR_PORT_FEATURE_PROCESS;
            hub_instance->port_iterator = 0;
        case HUB_CLEAR_PORT_FEATURE_PROCESS:
            if (hub_instance->port_iterator < hub_instance->HUB_PORT_NR)
            {
                usb_class_hub_clear_port_feature((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, ++hub_instance->port_iterator, C_PORT_CONNECTION);
                break;
            } /* EndIf */
            /* else */

            /* pass fluently to HUB_GET_PORT_STATUS_PROCESS */
            hub_instance->STATE = HUB_GET_PORT_STATUS_PROCESS;
            hub_instance->port_iterator = 0;
        case HUB_GET_PORT_STATUS_PROCESS:
            if (hub_instance->port_iterator)
                /* register the current status of the port, do the conversion LSB -> MSB */
                (hub_instance->HUB_PORTS + hub_instance->port_iterator - 1)->STATUS = SHORT_LE_TO_HOST(*(uint16_t*)buffer);
            if (hub_instance->port_iterator < hub_instance->HUB_PORT_NR)
            {
                usb_class_hub_get_port_status((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, ++hub_instance->port_iterator, 4);
                break;
            } /* EndIf */
            /* else */

            /* test if device is attached since startup */
            for (i = 0; i < hub_instance->HUB_PORT_NR; i++)
                if ((hub_instance->HUB_PORTS + i)->STATUS & (1 << PORT_CONNECTION))
                {
                    /* if some device is attached since startup, then start session */
                    hub_instance->port_iterator = i;
                    hub_instance->STATE = HUB_NONE;
                    (hub_instance->HUB_PORTS + i)->APP_STATUS |= HUB_PORT_ATTACHED;
                    usb_class_hub_set_port_feature((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, hub_instance->port_iterator + 1, PORT_RESET);
                    break;
                }

            usb_class_hub_wait_for_interrupt((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_int_callback, (void *) hub_instance, hub_instance->HUB_PORT_NR / 8 + 1);
            break;

        case HUB_RESET_DEVICE_PORT_PROCESS:
            hub_instance->STATE = HUB_NONE;

            usb_class_hub_set_port_feature((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, hub_instance->port_iterator + 1, PORT_RESET);

            usb_class_hub_wait_for_interrupt((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_int_callback, (void *) hub_instance, hub_instance->HUB_PORT_NR / 8 + 1);
            break;

        case HUB_ADDRESS_DEVICE_PORT_PROCESS:
            /* compute speed */
            if ((hub_instance->HUB_PORTS + hub_instance->port_iterator)->STATUS & (1 << PORT_HIGH_SPEED))
                i = USB_SPEED_HIGH;
            else if ((hub_instance->HUB_PORTS + hub_instance->port_iterator)->STATUS & (1 << PORT_LOW_SPEED))
                i = USB_SPEED_LOW;
            else
                i = USB_SPEED_FULL;

            /* FIXME: accessing intf_handle directly without validation to get its host handle is not good method */
            usb_dev_list_attach_device(
                ((USB_HUB_CLASS_INTF_STRUCT_PTR) hub_instance->CCS.class_intf_handle)->G.host_handle,
                i, /* port speed */
                pipe->DEVICE_ADDRESS, /* hub address */
                hub_instance->port_iterator + 1 /* hub port */
                );

            /* test if there is still device which was not reset */
            for (i = 0; i < hub_instance->HUB_PORT_NR; i++)
                if (((hub_instance->HUB_PORTS + i)->STATUS & (1 << PORT_CONNECTION)) && !((hub_instance->HUB_PORTS + i)->APP_STATUS & HUB_PORT_ATTACHED))
                {
                    /* if some device is attached since startup, then start session */
                    hub_instance->port_iterator = i;
                    hub_instance->STATE = HUB_NONE;
                    (hub_instance->HUB_PORTS + i)->APP_STATUS |= HUB_PORT_ATTACHED;
                    usb_class_hub_set_port_feature((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, hub_instance->port_iterator + 1, PORT_RESET);
                    break;
                }

            usb_class_hub_wait_for_interrupt((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_int_callback, (void *) hub_instance, hub_instance->HUB_PORT_NR / 8 + 1);
            break;

        case HUB_DETACH_DEVICE_PORT_PROCESS:
            /* FIXME: accessing intf_handle directly without validation to get its host handle is not good method */
            usb_dev_list_detach_device(
                ((USB_HUB_CLASS_INTF_STRUCT_PTR) hub_instance->CCS.class_intf_handle)->G.host_handle,
                pipe->DEVICE_ADDRESS, /* hub address */
                hub_instance->port_iterator + 1 /* hub port */
                );

            /* reset the app status */
            (hub_instance->HUB_PORTS + hub_instance->port_iterator)->APP_STATUS = 0x00;

            /* test if there is still device which was not reset */
            for (i = 0; i < hub_instance->HUB_PORT_NR; i++)
                if (((hub_instance->HUB_PORTS + i)->STATUS & (1 << PORT_CONNECTION)) && !((hub_instance->HUB_PORTS + i)->APP_STATUS & HUB_PORT_ATTACHED))
                {
                    /* if some device is attached since startup, then start session */
                    hub_instance->port_iterator = i;
                    hub_instance->STATE = HUB_NONE;
                    (hub_instance->HUB_PORTS + i)->APP_STATUS |= HUB_PORT_ATTACHED;
                    usb_class_hub_set_port_feature((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, hub_instance->port_iterator + 1, PORT_RESET);
                    break;
                }

            usb_class_hub_wait_for_interrupt((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_int_callback, (void *) hub_instance, hub_instance->HUB_PORT_NR / 8 + 1);
            break;

        case HUB_GET_PORT_STATUS_ASYNC:
        {
            uint32_t stat = LONG_LE_TO_HOST(*(uint32_t*)buffer);

            /* register the current status of the port */
            (hub_instance->HUB_PORTS + hub_instance->port_iterator)->STATUS = stat;

            /* let's see what happened */
            if ((1 << C_PORT_CONNECTION) & stat)
            {
                /* get if a device on port was attached or detached */
                if ((hub_instance->HUB_PORTS + hub_instance->port_iterator)->APP_STATUS & HUB_PORT_ATTACHED)
                {
                    hub_instance->STATE = HUB_DETACH_DEVICE_PORT_PROCESS;
                    usb_class_hub_clear_port_feature((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, hub_instance->port_iterator + 1, C_PORT_CONNECTION);
                }
                else
                {
                    hub_instance->STATE = HUB_RESET_DEVICE_PORT_PROCESS;
                    usb_class_hub_clear_port_feature((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, hub_instance->port_iterator + 1, C_PORT_CONNECTION);
                }
                break;
            }
            else if ((1 << C_PORT_RESET) & stat)
            {
                hub_instance->STATE = HUB_ADDRESS_DEVICE_PORT_PROCESS;
                (hub_instance->HUB_PORTS + hub_instance->port_iterator)->APP_STATUS |= HUB_PORT_ATTACHED;
                usb_class_hub_clear_port_feature((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, hub_instance->port_iterator + 1, C_PORT_RESET);
                break;
            }
            else if ((1 << C_PORT_ENABLE) & stat)
            {
                /* unexpected event (error), just ignore the port */
                hub_instance->STATE = HUB_DETACH_DEVICE_PORT_PROCESS;
                usb_class_hub_clear_port_feature((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, hub_instance->port_iterator + 1, C_PORT_ENABLE);
                break;
            }
            else if ((1 << C_PORT_OVER_CURRENT) & stat)
            {
                /* unexpected event (error), just ignore the port */
                hub_instance->STATE = HUB_NONE;
                usb_class_hub_clear_port_feature((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, hub_instance->port_iterator + 1, C_PORT_OVER_CURRENT);
                usb_class_hub_wait_for_interrupt((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_int_callback, (void *) hub_instance, hub_instance->HUB_PORT_NR / 8 + 1);
                break;
            }
            else if ((1 << C_PORT_POWER) & stat)
            {
                /* unexpected event (error), just ignore the port */
                hub_instance->STATE = HUB_NONE;
                usb_class_hub_clear_port_feature((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, hub_instance->port_iterator + 1, C_PORT_POWER);
                usb_class_hub_wait_for_interrupt((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_int_callback, (void *) hub_instance, hub_instance->HUB_PORT_NR / 8 + 1);
                break;
            }
            /* FIXME: handle more events */

            break;
        }

        case HUB_GET_STATUS_ASYNC:
        {
            HUB_STATUS_STRUCT_PTR hub_stat = (HUB_STATUS_STRUCT_PTR) buffer;
            uint32_t change = SHORT_LE_TO_HOST(hub_stat->WHUBCHANGE);

            if ((1 << C_HUB_LOCAL_POWER) & change)
            {
                hub_instance->STATE = HUB_NONE;
                usb_class_hub_clear_feature((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, C_HUB_LOCAL_POWER);
                usb_class_hub_wait_for_interrupt((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_int_callback, (void *) hub_instance, hub_instance->HUB_PORT_NR / 8 + 1);
            }
            else if ((1 << C_HUB_OVER_CURRENT) & change)
            {
                hub_instance->STATE = HUB_NONE;
                usb_class_hub_clear_feature((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, C_HUB_OVER_CURRENT);
                usb_class_hub_wait_for_interrupt((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_int_callback, (void *) hub_instance, hub_instance->HUB_PORT_NR / 8 + 1);
            }
            break;
        }
        default:
            break;
    } /* EndSwitch */
    return;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hub_int_callback
* Returned Value : None
* Comments       :
*     Called on interrupt endpoint data reception
*END*--------------------------------------------------------------------*/

static void usb_host_hub_int_callback
   (
      /* [IN] pointer to pipe */
      PIPE_STRUCT_PTR   pipe,

      /* [IN] user-defined parameter */
      void             *param,

      /* [IN] buffer address */
      void             *buffer,

      /* [IN] length of data transferred */
      uint32_t           len,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      USB_STATUS        status
   )
{ /* Body */
    HUB_DEVICE_STRUCT_PTR          hub_instance = (HUB_DEVICE_STRUCT_PTR) param;
    _mqx_int                       i, j, port = 0;
    uint8_t                        *port_pattern= (uint8_t *) buffer;

    /* find which port changed its state */
    for (i = 0; i < len; i++)
    {
        if (!port_pattern[i])
            continue;
        port = i * 8;
        for (j = 0; j < 8; j++, port++)
        {
            if (port_pattern[i] & 0x01)
                break;
            port_pattern[i] >>= 1;
        }
        break;
    }

    /* The port number, which changed status, is written in variable "port"
    ** Note, that if there are more ports, which changed its status, these will
    ** be invoked later (in next interrupt)
    */

    if (port == 0) {
        hub_instance->STATE = HUB_GET_STATUS_ASYNC;
        usb_class_hub_get_status((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, 4);
    }
    else {
        hub_instance->STATE = HUB_GET_PORT_STATUS_ASYNC;
        hub_instance->port_iterator = port - 1;
        usb_class_hub_get_port_status((CLASS_CALL_STRUCT_PTR) &hub_instance->CCS, (tr_callback) usb_host_hub_device_sm, (void *) hub_instance, port, 4);
    }
} /*EndBody */
