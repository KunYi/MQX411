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
*   This file is an example of device drivers for the HID class. This example
*   demonstrates the keyboard functionality. Note that a real keyboard driver also
*   needs to distinguish between intentionally repeated and unrepeated key presses.
*   This example simply demonstrates how to receive data from a USB Keyboard. 
*   Interpretation of data is upto the application and customers can add the code
*   for it.
*
*
*END************************************************************************/

#include "hidkeyboard.h"
#include <usb_host_hid.h>
#include <usb_host_hub_sm.h>
#include <mqx_host.h>
#include <lwevent.h>

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

/***************************************
**
** Globals
*/
extern LWEVENT_STRUCT USB_Keyboard_Event;
_usb_pipe_handle HID_pipe;

volatile USB_KEYBOARD_DEVICE_STRUCT kbd_hid_device = { 0 };
extern void Keyboard_Task(uint32_t param);
void process_kbd_buffer(unsigned char *buffer);

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : main (Main_Task if using MQX)
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/
void Keyboard_Task(uint32_t param)
{
    USB_STATUS status = USB_OK;
    TR_INIT_PARAM_STRUCT tr;
    unsigned char *buffer;
    _usb_host_handle kbd_host_handle = (_usb_host_handle) param;
    /* Allocate buffer to receive data from interrupt. It must be created on uncached heap,
    ** since some USB host controllers use DMA to access those buffers.
    */
    buffer = USB_mem_alloc_uncached(HID_KEYBOARD_BUFFER_SIZE);
    if (buffer == NULL) {
        printf("\nMemory allocation failed. STATUS: %x", status);
        fflush(stdout);
        _task_block();
    }
    /* event for USB callback signaling */
    /*
    ** Infinite loop, waiting for events requiring action
    */
    for(;;) 
    {
        // Wait for insertion or removal event
        _lwevent_wait_ticks(&USB_Keyboard_Event, USB_Keyboard_Event_CTRL, FALSE, 0);
        
        switch (kbd_hid_device.DEV_STATE) {
        case USB_DEVICE_IDLE:
            break;
        case USB_DEVICE_INUSE:
            if(HID_pipe) {
                /******************************************************************
                    Initiate a transfer request on the interrupt pipe
                ******************************************************************/
                usb_hostdev_tr_init(&tr, usb_host_hid_keyboard_recv_callback, NULL);
                tr.G.RX_BUFFER = (unsigned char *) buffer;
                tr.G.RX_LENGTH = HID_KEYBOARD_BUFFER_SIZE;
                while(kbd_hid_device.DEV_STATE == USB_DEVICE_INUSE) 
                {
                    status = _usb_host_recv_data(kbd_host_handle, HID_pipe, &tr);
                    if(status != USB_STATUS_TRANSFER_QUEUED) {
                        printf("\nError in _usb_host_recv_data: %x", status);
                        fflush(stdout);
                    }

                    /* Wait untill we get the data from keyboard. */
                    _lwevent_wait_ticks(&USB_Keyboard_Event, USB_Keyboard_Event_CTRL | USB_Keyboard_Event_DATA, FALSE, 0);
                    if (kbd_hid_device.DEV_STATE == USB_DEVICE_INUSE) 
                    {
                        process_kbd_buffer((unsigned char *)buffer);
                        _lwevent_clear(&USB_Keyboard_Event, USB_Keyboard_Event_DATA);
                    }
                    _time_delay(1);
                }
            }
            break;
        }
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hid_keyboard_event
* Returned Value : None
* Comments       :
*     Called when HID device has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
void usb_host_hid_keyboard_event(
/* [IN] pointer to device instance */
_usb_device_instance_handle dev_handle,
/* [IN] pointer to interface descriptor */
_usb_interface_descriptor_handle intf_handle,
/* [IN] code number for event causing callback */
uint32_t event_code)
{
    USB_STATUS           status = USB_OK;
    INTERFACE_DESCRIPTOR_PTR intf_ptr = (INTERFACE_DESCRIPTOR_PTR) intf_handle;

    fflush(stdout);
    switch (event_code) {

    case USB_ATTACH_EVENT:
        /* Drop through config event for the same processing */
    case USB_CONFIG_EVENT:
        printf("State = %d", kbd_hid_device.DEV_STATE);
        printf("  Class = %d", intf_ptr->bInterfaceClass);
        printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
        fflush(stdout);

        if(kbd_hid_device.DEV_STATE == USB_DEVICE_IDLE) {
            kbd_hid_device.DEV_HANDLE = dev_handle;
            kbd_hid_device.INTF_HANDLE = intf_handle;
            kbd_hid_device.DEV_STATE = USB_DEVICE_ATTACHED;         
            status = _usb_hostdev_select_interface(kbd_hid_device.DEV_HANDLE, kbd_hid_device.INTF_HANDLE, (void *) & kbd_hid_device.CLASS_INTF);
            if(status != USB_OK) {
                printf("\nError in _usb_hostdev_select_interface: %x", status);
                fflush(stdout);
                _task_block();
            }
            HID_pipe = _usb_hostdev_find_pipe_handle(kbd_hid_device.DEV_HANDLE, kbd_hid_device.INTF_HANDLE, USB_INTERRUPT_PIPE, USB_RECV);         
        }
        else {
            printf("HID device already attached\n");
            fflush(stdout);
        }
        break;

    case USB_DETACH_EVENT:
        /* Use only the interface with desired protocol */
        printf("\n----- Detach Event -----\n");
        printf("State = %d", kbd_hid_device.DEV_STATE);
        printf("  Class = %d", intf_ptr->bInterfaceClass);
        printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
        fflush(stdout);

        kbd_hid_device.DEV_HANDLE = NULL;
        kbd_hid_device.INTF_HANDLE = NULL;
        kbd_hid_device.DEV_STATE = USB_DEVICE_IDLE;
        break;
    }

    /* notify application that status has changed */
    _lwevent_set(&USB_Keyboard_Event, USB_Keyboard_Event_CTRL);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hid_ctrl_callback
* Returned Value : None
* Comments       :
*     Called when a control pipe command is completed. 
*
*END*--------------------------------------------------------------------*/
void usb_host_hid_keyboard_ctrl_callback(
/* [IN] pointer to pipe */
_usb_pipe_handle pipe_handle,
/* [IN] user-defined parameter */
void *user_parm,
/* [IN] buffer address */
unsigned char *buffer,
/* [IN] length of data transferred */
uint32_t buflen,
/* [IN] status, hopefully USB_OK or USB_DONE */
uint32_t status)
{
    if(status == USBERR_ENDPOINT_STALLED) {
        printf("\nHID Set_Protocol Request BOOT is not supported!\n");
        fflush(stdout);
    }
    else if(status) {
        printf("\nHID Set_Protocol Request BOOT failed!: 0x%x ... END!\n", status);
        fflush(stdout);
    }

    if(kbd_hid_device.DEV_STATE == USB_DEVICE_SETTING_PROTOCOL)
    kbd_hid_device.DEV_STATE = USB_DEVICE_INUSE;

    /* notify application that status has changed */
    _lwevent_set(&USB_Keyboard_Event, USB_Keyboard_Event_CTRL);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_kbd_host_hid_recv_callback
* Returned Value : None
* Comments       :
*     Called when a interrupt pipe transfer is completed.
*
*END*--------------------------------------------------------------------*/
void usb_host_hid_keyboard_recv_callback(
/* [IN] pointer to pipe */
_usb_pipe_handle pipe_handle,
/* [IN] user-defined parameter */
void *user_parm,
/* [IN] buffer address */
unsigned char *buffer,
/* [IN] length of data transferred */
uint32_t buflen,
/* [IN] status, hopefully USB_OK or USB_DONE */
uint32_t status)
{
    /* notify application that data are available */
    if (USB_Keyboard_Event_DATA != USB_Keyboard_Event.VALUE)
    {
        _lwevent_set(&USB_Keyboard_Event, USB_Keyboard_Event_DATA);
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : process_kbd_buffer
* Returned Value : None
* Comments       :
*      The way keyboard works is that it sends reports of 8 bytes of data
*      every time keys are pressed. However, it reports all the keys
*      that are pressed in a single report. The following code should
*      really be implemented by a user in the way he would like it to be.
*END*--------------------------------------------------------------------*/
void process_kbd_buffer(unsigned char *buffer)
{
    if(1==buffer[0]) audio_mute_command();
    if(2==buffer[0]) audio_increase_volume_command();
    if(4==buffer[0]) audio_decrease_volume_command();
    fflush(stdout);
}
