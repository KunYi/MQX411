/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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
*   This file contains board-specific USB initialization functions.
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "bsp_prv.h"

struct usb_dev_if_struct _bsp_usb_dev_ehci0_if = {
    &_usb_ehci_dev_callback_table,
    (void *) &_ehci0_dev_init_param,
    NULL
};

struct usb_dev_if_struct _bsp_usb_dev_ehci1_if = {
    &_usb_ehci_dev_callback_table,
    (void *) &_ehci1_dev_init_param,
    NULL
};


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_dev_init
* Returned Value   : 0 for success, -1 for failure
* Comments         :
*    This function performs BSP-specific initialization related to USB
*
*END*----------------------------------------------------------------------*/
_mqx_int _bsp_usb_dev_init(struct usb_dev_if_struct *usb_if)
{
    USB_MemMapPtr usb_ptr;
    USB_EHCI_DEV_INIT_STRUCT_PTR usb_init;
    _mqx_int result;

    if (usb_if == NULL) {
        return USBBSP_ERROR;
    }

    if (usb_if->DEV_INIT_PARAM == &_ehci0_dev_init_param) {
        usb_ptr = USB0_BASE_PTR;
    }
    else if (usb_if->DEV_INIT_PARAM == &_ehci1_dev_init_param) {
        usb_ptr = USB1_BASE_PTR;
    }
    else {
        return USBBSP_ERROR; /* unknown controller */
    }

    result = _bsp_usb_dev_io_init(usb_if);

    if (result != MQX_OK)
        return result;

    USB_USBCMD_REG(usb_ptr) &= ~USB_USBCMD_RS_MASK;    
    while (USB_USBCMD_REG(usb_ptr) & USB_USBCMD_RS_MASK);
    { /* delay while resetting USB controller */ } 
    USB_USBCMD_REG(usb_ptr) |= USB_USBCMD_RST_MASK;    
    while (USB_USBCMD_REG(usb_ptr) & USB_USBCMD_RST_MASK)
    { /* delay while resetting USB controller */ } 

    USB_USBMODE_REG(usb_ptr) = USB_USBMODE_CM_MASK;
    USB_USBCMD_REG(usb_ptr) = USB_USBCMD_ASP(3) | USB_USBCMD_ITC(0);

    /* setup interrupt */
    usb_init = (USB_EHCI_DEV_INIT_STRUCT_PTR)usb_if->DEV_INIT_PARAM;
    result = _bsp_int_init(usb_init->VECTOR, BSP_USB_INT_LEVEL, 0, TRUE);

    return result;
}



/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_usb_timer_register_base
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the address of the VUSBHS Timer Registers
*
*END*----------------------------------------------------------------------*/
void *_bsp_get_usb_timer_register_base(uint8_t dev_num)
{
    if (dev_num == 0) {
        return (void *) ((unsigned char *) USB0_BASE_PTR + 0x100);
    }
    else if (dev_num == 1) {
        return (void *) ((unsigned char *) USB1_BASE_PTR + 0x80);
    }

    return NULL;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_usb_otg_csr
* Returned Value   : Address upon success, NULL upon failure
* Comments         :
*    This function returns the address of the U0CSR register
*
*END*----------------------------------------------------------------------*/
void *_bsp_get_usb_otg_csr(uint8_t dev_num)
{
    return NULL;
}
