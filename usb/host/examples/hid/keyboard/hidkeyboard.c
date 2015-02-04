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

#include <bsp.h>

#include <usb.h>
#include <hostapi.h>
#include <usb_host_hid.h>
#include <usb_host_hub_sm.h>

#include "hidkeyboard.h"

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

#define USB_EVENT_CTRL           (0x01)
#define USB_EVENT_DATA           (0x02)
#define USB_EVENT_DATA_CORRUPTED (0x04)

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

#define MAX_SUPPORTED_USAGE_ID 57

static const unsigned char HID_table[MAX_SUPPORTED_USAGE_ID][2] = {
    {0, 0},                     /* Usage ID  0 */
    {0, 0},                     /* Usage ID  1 */
    {0, 0},                     /* Usage ID  2 */
    {0, 0},                     /* Usage ID  3 */
    {0x61, 'A'},                /* Usage ID  4 */
    {'b', 'B'},                 /* Usage ID  5 */
    {'c', 'C'},                 /* Usage ID  6 */
    {'d', 'D'},                 /* Usage ID  7 */
    {'e', 'E'},                 /* Usage ID  8 */
    {'f', 'F'},                 /* Usage ID  9 */
    {'g', 'G'},                 /* Usage ID 10 */
    {'h', 'H'},                 /* Usage ID 11 */
    {'i', 'I'},                 /* Usage ID 12 */
    {'j', 'J'},                 /* Usage ID 13 */
    {'k', 'K'},                 /* Usage ID 14 */
    {'l', 'L'},                 /* Usage ID 15 */
    {'m', 'M'},                 /* Usage ID 16 */
    {'n', 'N'},                 /* Usage ID 17 */
    {'o', 'O'},                 /* Usage ID 18 */
    {'p', 'P'},                 /* Usage ID 19 */
    {'q', 'Q'},                 /* Usage ID 20 */
    {'r', 'R'},                 /* Usage ID 21 */
    {'s', 'S'},                 /* Usage ID 22 */
    {'t', 'T'},                 /* Usage ID 23 */
    {'u', 'U'},                 /* Usage ID 24 */
    {'v', 'V'},                 /* Usage ID 25 */
    {'w', 'W'},                 /* Usage ID 26 */
    {'x', 'X'},                 /* Usage ID 27 */
    {'y', 'Y'},                 /* Usage ID 28 */
    {'z', 'Z'},                 /* Usage ID 29 */
    {'1', '!'},                 /* Usage ID 30 */
    {'2', '@'},                 /* Usage ID 31 */
    {'3', '#'},                 /* Usage ID 32 */
    {'4', '$'},                 /* Usage ID 33 */
    {'5', '%'},                 /* Usage ID 34 */
    {'6', '^'},                 /* Usage ID 35 */
    {'7', '&'},                 /* Usage ID 36 */
    {'8', '*'},                 /* Usage ID 37 */
    {'9', '('},                 /* Usage ID 38 */
    {'0', ')'},                 /* Usage ID 39 */
    {'\n', '\n'},               /* Usage ID 40 */ //CARRIAGE
    {0x1B, 0x1B},               /* Usage ID 41 */ //ESC
    {'\b', '\b'},               /* Usage ID 42 */ //BACKSPACE
    {'\t', '\t'},               /* Usage ID 43 */ //TAB
    {' ', ' '},                 /* Usage ID 44 */ //SPACE
    {'-', '_'},                 /* Usage ID 45 */
    {'=', '+'},                 /* Usage ID 46 */
    {'[', '{'},                 /* Usage ID 47 */
    {']', '}'},                 /* Usage ID 48 */
    {'\\', '|'},                /* Usage ID 49 */
    {0, 0},                     /* Usage ID 50 */
    {';', ':'},                 /* Usage ID 51 */
    {'\'', '\"'},               /* Usage ID 52 */
    {'`', '~'},                 /* Usage ID 53 */
    {',', '<'},                 /* Usage ID 54 */
    {'.', '>'},                 /* Usage ID 55 */
    {'/', '?'},                 /* Usage ID 56 */
};

#define MAIN_TASK          (10)

TASK_TEMPLATE_STRUCT MQX_template_list[] = {
    {MAIN_TASK, Main_Task, 3000L, 9L, "Main", MQX_AUTO_START_TASK},
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
    USB_STATUS              status = USB_OK;
    TR_INIT_PARAM_STRUCT    tr;
    PIPE_STRUCT_PTR         pipe = NULL;
    HID_COMMAND             hid_com;
    uint32_t                e;
    unsigned char           *buffer;

    /* _usb_otg_init needs to be done with interrupts disabled */
    _int_disable();

    /* event for USB callback signaling */
    _lwevent_create(&USB_Event, LWEVENT_AUTO_CLEAR);

    _int_install_unexpected_isr();

    /* Register USB interface and its settings from BSP */
    _usb_host_driver_install(USBCFG_DEFAULT_HOST_CONTROLLER);

    /*
     ** It means that we are going to act like host, so we initialize the
     ** host stack. This call will allow USB system to allocate memory for
     ** data structures, it uses later (e.g pipes etc.).
     */
    status = _usb_host_init(USBCFG_DEFAULT_HOST_CONTROLLER, &host_handle);

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

    printf("\nMQX USB HID Keyboard Demo\nWaiting for USB Keyboard to be attached...\n");
    fflush(stdout);

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
            printf("Keyboard device attached\n");
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
            pipe = _usb_hostdev_find_pipe_handle(hid_device.DEV_HANDLE, hid_device.INTF_HANDLE, USB_INTERRUPT_PIPE, USB_RECV);
            if (pipe == NULL) {
                printf("\nError getting interrupt pipe.");
                fflush(stdout);
                _task_block();
            }
            _usb_hostdev_get_buffer(hid_device.DEV_HANDLE, pipe->MAX_PACKET_SIZE, (void **) &buffer);
            if (buffer == NULL) {
                printf("\nMemory allocation failed. STATUS: %x", status);
                fflush(stdout);
                _task_block();
            }

            printf("Keyboard device interfaced, setting protocol...\n");
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
            printf("Keyboard device ready, try some typing\n");

            while(1) {
              /******************************************************************
                Initiate a transfer request on the interrupt pipe
              ******************************************************************/
                usb_hostdev_tr_init(&tr, usb_host_hid_recv_callback, NULL);
                tr.G.RX_BUFFER = (unsigned char *) buffer;
                tr.G.RX_LENGTH = pipe->MAX_PACKET_SIZE;

                status = _usb_host_recv_data(host_handle, pipe, &tr);

                if(status != USB_STATUS_TRANSFER_QUEUED) {
                    printf("\nError in _usb_host_recv_data: %x", status);
                    fflush(stdout);
                }

                /* Wait untill we get the data from keyboard. */
                _lwevent_wait_ticks(&USB_Event, USB_EVENT_CTRL | USB_EVENT_DATA | USB_EVENT_DATA_CORRUPTED, FALSE, 0);

                e = _lwevent_get_signalled();
                if (USB_EVENT_DATA == e) {
                    if(hid_device.DEV_STATE == USB_DEVICE_INUSE) {
                        process_kbd_buffer((unsigned char *)buffer);
                    }
                }
                else if (USB_EVENT_CTRL == e) {
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
    /* [IN] status of transfer */
    USB_STATUS status
)
{
    if (status == USB_OK) {
        /* notify application that data are available */
        _lwevent_set(&USB_Event, USB_EVENT_DATA);
    }
    else {
        /* notify application that data are available */
        _lwevent_set(&USB_Event, USB_EVENT_DATA_CORRUPTED);
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
    /* a little array in which we count how long each key is pressed */
    static unsigned char special_last;

    uint32_t i, shift = 0;
    unsigned char code;

    /* The first byte in buffer gives special key status.
     ** Process only the keys which are newly pressed. */
    code = (buffer[0] ^ special_last) & buffer[0];
    special_last = buffer[0];

    shift = 0;
    /* Check Modifiers in byte 0 (see HID specification 1.11 page 56) */
    if(code & 0x01) {
        printf("LEFT CTRL ");
    }
    if(code & 0x02) {
        printf("LEFT SHIFT ");
    }
    if(code & 0x04) {
        printf("LEFT ALT ");
    }
    if(code & 0x08) {
        printf("LEFT GUI ");
    }
    if(code & 0x10) {
        printf("RIGHT CTRL ");
    }
    if(code & 0x20) {
        printf("RIGHT SHIFT ");
    }
    if(code & 0x40) {
        printf("RIGHT ALT ");
    }
    if(code & 0x80) {
        printf("RIGHT GUI ");
    }

    /* immediate state of left or right SHIFT */
    if(buffer[0] & 0x22) {
        shift = 1;
    }

    /* Byte 1 is reserved (HID specification 1.11 page 60) */

    /*
     ** Build initial press-map by checking Keybcodes in bytes 2 to 7 
     ** (HID spec 1.11 page 60)
     */
    for(i = 8 - 1; i >= 2; i--) {
        code = buffer[i];

        /* if valid keyboard code was received */
        if(code > 1) {
            if(code <= MAX_SUPPORTED_USAGE_ID && HID_table[code][shift]) {
                switch (code) {
                    case 0x29:
                      printf("ESC ");
                      break;
                    case 0x2A:
                      printf("BACKSPACE ");
                      break;
                    default:
                      putchar(HID_table[code][shift]);
                }
            }
            else {
                printf("\\x%02x", code);
            }

            /* only print the newest key */
            break;
        }
    }

    fflush(stdout);
}
