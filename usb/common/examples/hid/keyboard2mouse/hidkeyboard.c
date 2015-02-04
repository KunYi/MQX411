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
#include <mqx.h>
#include <lwevent.h>

#include <usb.h>
#include <host_main.h>
#include <host_rcv.h>
#include <usb_host_hid.h>
#include <usb_host_hub_sm.h>

#include "hidkeyboard.h"
#include "mouse.h"

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
LWEVENT_STRUCT USB_Event;

#define USB_EVENT_CTRL 0x01
#define USB_EVENT_DATA 0x02

/* Table of driver capabilities this application wants to use */
static USB_HOST_DRIVER_INFO DriverInfoTable[] = {
    {
            {0x00, 0x00},       /* Vendor ID per USB-IF             */
            {0x00, 0x00},       /* Product ID per manufacturer      */
            USB_CLASS_HID,      /* Class code                       */
            USB_SUBCLASS_HID_BOOT, /* Sub-Class code                   */
            USB_PROTOCOL_HID_KEYBOARD, /* Protocol                         */
            0,                  /* Reserved                         */
            usb_host_hid_keyboard_event /* Application call back function   */
    },
    /* USB 1.1 hub */
    {
            {0x00, 0x00},       /* Vendor ID per USB-IF             */
            {0x00, 0x00},       /* Product ID per manufacturer      */
            USB_CLASS_HUB,      /* Class code                       */
            USB_SUBCLASS_HUB_NONE, /* Sub-Class code                   */
            USB_PROTOCOL_HUB_ALL, /* Protocol                         */
            0,                  /* Reserved                         */
            usb_host_hub_device_event /* Application call back function   */
    },
    {
            {0x00, 0x00},       /* All-zero entry terminates        */
            {0x00, 0x00},       /* driver info list.                */
            0,
            0,
            0,
            0,
            NULL
    },
};

volatile DEVICE_STRUCT hid_device = { 0 };

_usb_host_handle host_handle;

void Main_Task(uint32_t param);
void process_kbd_buffer(unsigned char *buffer);

TASK_TEMPLATE_STRUCT MQX_template_list[] = {
    {10L, Main_Task, 3000L, 9L, "Main", MQX_AUTO_START_TASK},
    {0L, 0L, 0L, 0L, 0L, 0L}
};

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : main (Main_Task if using MQX)
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/

void Main_Task(uint32_t param)
{
    USB_STATUS status = USB_OK;
    _usb_pipe_handle pipe;
    TR_INIT_PARAM_STRUCT tr;
    HID_COMMAND hid_com;
    unsigned char *buffer;

    printf("\n** MQX USB Keyboard To Mouse Demo **\n");
    
    printf("\nDemo will allow you control mouse cursor on your PC using keyboard attached to TWR-SER2 board.");
    printf("\nThis demo requires TWR-SER2 card and correct BSP setting.");
    printf("\nSee MQX_Getting_Started.pdf chapter 7 for details.");
    fflush(stdout);

    MouseDev_Init();

    /* Allocate buffer to receive data from interrupt. It must be created on uncached heap,
    ** since some USB host controllers use DMA to access those buffers.
    */
    buffer = USB_mem_alloc_uncached(HID_BUFFER_SIZE);
    if (buffer == NULL) {
        printf("\nMemory allocation failed. STATUS: %x", status);
        fflush(stdout);
        _task_block();
    }
    
    /* _usb_otg_init needs to be done with interrupts disabled */
    _int_disable();

    /* event for USB callback signaling */
    _lwevent_create(&USB_Event, LWEVENT_AUTO_CLEAR);

    _int_install_unexpected_isr();

    /* Register USB interface and its settings from BSP */
    if (MQX_OK != _usb_host_driver_install(&_bsp_usb_host_khci0_if)) {
        printf("\n\nUSB Host Installation failed.");
        fflush(stdout);
        _task_block();
    }

    /*
     ** It means that we are going to act like host, so we initialize the
     ** host stack. This call will allow USB system to allocate memory for
     ** data structures, it uses later (e.g pipes etc.).
     */
    status = _usb_host_init(&_bsp_usb_host_khci0_if, &host_handle);

    if(status != USB_OK) {
        printf("\nUSB Host Initialization failed. STATUS: %x", status);
        fflush(stdout);
        _task_block();
    }

    /*
     ** since we are going to act as the host driver, register the driver
     ** information for wanted class/subclass/protocols
     */
    status = _usb_host_driver_info_register(host_handle, DriverInfoTable);
    if(status != USB_OK) {
        printf("\nDriver Registration failed. STATUS: %x", status);
        fflush(stdout);
        _task_block();
    }

    _int_enable();
    printf("\nHost initialization finished. Attach USB Keyboard to the board.");

    /*
     ** Infinite loop, waiting for events requiring action
     */
    for(;;) {

        // Wait for insertion or removal event
        _lwevent_wait_ticks(&USB_Event, USB_EVENT_CTRL, FALSE, 0);

        switch (hid_device.DEV_STATE) {

        case USB_DEVICE_IDLE:
            break;

        case USB_DEVICE_ATTACHED:
            hid_device.DEV_STATE = USB_DEVICE_SET_INTERFACE_STARTED;

            status = _usb_hostdev_select_interface(hid_device.DEV_HANDLE, hid_device.INTF_HANDLE, (void *) & hid_device.CLASS_INTF);
            if(status != USB_OK) {
                printf("\nError in _usb_hostdev_select_interface: %x", status);
                fflush(stdout);
                _task_block();
            }
            break;

        case USB_DEVICE_SET_INTERFACE_STARTED:
            break;

        case USB_DEVICE_INTERFACED:
            printf("\nKeyboard device interfaced, setting protocol.");
            /* now we will set the USB Hid standard boot protocol */
            hid_device.DEV_STATE = USB_DEVICE_SETTING_PROTOCOL;

            hid_com.CLASS_PTR = (CLASS_CALL_STRUCT_PTR) & hid_device.CLASS_INTF;
            hid_com.CALLBACK_FN = usb_host_hid_ctrl_callback;
            hid_com.CALLBACK_PARAM = 0;

            /* Force the keyboard to behave as in USB Hid class standard boot protocol */
            status = usb_class_hid_set_protocol(&hid_com, USB_PROTOCOL_HID_KEYBOARD);

            if(status != USB_STATUS_TRANSFER_QUEUED) {
                printf("\nError in usb_class_hid_set_protocol: %x", status);
                fflush(stdout);
            }

            break;

        case USB_DEVICE_INUSE:
            pipe = _usb_hostdev_find_pipe_handle(hid_device.DEV_HANDLE, hid_device.INTF_HANDLE, USB_INTERRUPT_PIPE, USB_RECV);

            if(pipe) {
                printf("\n\nUse W, A, S or D to move the cursor.\nW = UP\nS = DOWN\nA = LEFT\nD = RIGHT\n");

                while(1) {
                  /******************************************************************
                    Initiate a transfer request on the interrupt pipe
                  ******************************************************************/
                    usb_hostdev_tr_init(&tr, usb_host_hid_recv_callback, NULL);
                    tr.G.RX_BUFFER = (unsigned char *) buffer;
                    tr.G.RX_LENGTH = HID_BUFFER_SIZE;

                    status = _usb_host_recv_data(host_handle, pipe, &tr);

                    if(status != USB_STATUS_TRANSFER_QUEUED) {
                        printf("\nError in _usb_host_recv_data: %x", status);
                        fflush(stdout);
                    }

                    /* Wait untill we get the data from keyboard. */
                    _lwevent_wait_ticks(&USB_Event, USB_EVENT_CTRL | USB_EVENT_DATA, FALSE, 0);

                    /* if not detached in the meanwhile */
                    if(hid_device.DEV_STATE == USB_DEVICE_INUSE) {
                        process_kbd_buffer((unsigned char *)buffer);
                    }
                    else {
                        /* kick the outer loop again to handle the CTRL event */
                        _lwevent_set(&USB_Event, USB_EVENT_CTRL);
                        break;
                    }

                    /* Slight delay to be nice to other processes. (Note that keyboards have its 
                       own autorepeat delay typically much longer than one would expect. In case 
                       the user holds the key (without any other keys changed), the USB response 
                       from keyboard is delayed (typ by 500ms). This is why the USB host should 
                       handle autorepeat feature by itself (not done in this example) */
                    _time_delay(1);

                    USB_HID_Periodic_Task(); //feed the USB device with the correct setup answers
                }
            }
            break;

        case USB_DEVICE_DETACHED:
            printf("Going to idle state\n");
            hid_device.DEV_STATE = USB_DEVICE_IDLE;
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
    INTERFACE_DESCRIPTOR_PTR intf_ptr = (INTERFACE_DESCRIPTOR_PTR) intf_handle;

    fflush(stdout);
    switch (event_code) {

    case USB_ATTACH_EVENT:
        printf("----- Attach Event -----\n");
        /* Drop through config event for the same processing */
    case USB_CONFIG_EVENT:
        printf("State = %d", hid_device.DEV_STATE);
        printf("  Class = %d", intf_ptr->bInterfaceClass);
        printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
        fflush(stdout);

        if(hid_device.DEV_STATE == USB_DEVICE_IDLE) {
            hid_device.DEV_HANDLE = dev_handle;
            hid_device.INTF_HANDLE = intf_handle;
            hid_device.DEV_STATE = USB_DEVICE_ATTACHED;
        }
        else {
            printf("HID device already attached\n");
            fflush(stdout);
        }
        break;

    case USB_INTF_EVENT:
        printf("----- Interfaced Event -----\n");
        hid_device.DEV_STATE = USB_DEVICE_INTERFACED;
        break;

    case USB_DETACH_EVENT:
        /* Use only the interface with desired protocol */
        printf("\n----- Detach Event -----\n");
        printf("State = %d", hid_device.DEV_STATE);
        printf("  Class = %d", intf_ptr->bInterfaceClass);
        printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
        fflush(stdout);

        hid_device.DEV_HANDLE = NULL;
        hid_device.INTF_HANDLE = NULL;
        hid_device.DEV_STATE = USB_DEVICE_DETACHED;
        break;
    }

    /* notify application that status has changed */
    _lwevent_set(&USB_Event, USB_EVENT_CTRL);
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hid_ctrl_callback
* Returned Value : None
* Comments       :
*     Called when a control pipe command is completed. 
*
*END*--------------------------------------------------------------------*/

void usb_host_hid_ctrl_callback(
    /* [IN] pointer to pipe */
    _usb_pipe_handle pipe_handle,
    /* [IN] user-defined parameter */
    void   *user_parm,
    /* [IN] buffer address */
    unsigned char *buffer,
    /* [IN] length of data transferred */
    uint32_t buflen,
    /* [IN] status, hopefully USB_OK or USB_DONE */
    USB_STATUS status
)
{
    if(status == USBERR_ENDPOINT_STALLED) {
        printf("\nHID Set_Protocol Request BOOT is not supported!\n");
        fflush(stdout);
    }
    else if(status) {
        printf("\nHID Set_Protocol Request BOOT failed!: 0x%x ... END!\n", status);
        fflush(stdout);
        _task_block();
    }

    if(hid_device.DEV_STATE == USB_DEVICE_SETTING_PROTOCOL)
        hid_device.DEV_STATE = USB_DEVICE_INUSE;

    /* notify application that status has changed */
    _lwevent_set(&USB_Event, USB_EVENT_CTRL);
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hid_recv_callback
* Returned Value : None
* Comments       :
*     Called when a interrupt pipe transfer is completed.
*
*END*--------------------------------------------------------------------*/

void usb_host_hid_recv_callback(
    /* [IN] pointer to pipe */
    _usb_pipe_handle pipe_handle,
    /* [IN] user-defined parameter */
    void   *user_parm,
    /* [IN] buffer address */
    unsigned char *buffer,
    /* [IN] length of data transferred */
    uint32_t buflen,
    /* [IN] status, hopefully USB_OK or USB_DONE */
    USB_STATUS status
)
{
    /* notify application that data are available */
    _lwevent_set(&USB_Event, USB_EVENT_DATA);
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
    _mqx_uint i;

    for(i = HID_BUFFER_SIZE - 1; i >= 2; i--) {
        switch (buffer[i]) {
            case 4: //'a'
                printf("LEFT\n");
                move_mouse(LEFT);
                break;
            case 22: //'s'
                printf("DOWN\n");
                move_mouse(DOWN);
                break;
            case 7: //'d'
                printf("RIGHT\n");
                move_mouse(RIGHT);
                break;
            case 26: //'w'
                printf("UP\n");
                move_mouse(UP);
                break;
            default:
                break;
        }
    }
}
