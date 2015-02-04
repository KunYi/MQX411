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
*   This file Contains the implementation of class driver for Audio devices.
*
*
*END************************************************************************/
#include "usb_host_audio.h"

static void                         *audio_control_anchor;
static void                           *audio_stream_anchor;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_control_init
* Returned Value : None
* Comments       :
*     This function is called by common class to initialize the class driver
*     for audio control interface. It is called in response to a select
*     interface call by application.
*
*END*--------------------------------------------------------------------*/

void usb_class_audio_control_init
(
    /* [IN]  structure with USB pipe information on the interface */
    PIPE_BUNDLE_STRUCT_PTR      pbs_ptr,

    /* [IN] audio call struct pointer */
    CLASS_CALL_STRUCT_PTR       ccs_ptr
)
{ /* Body */
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR if_ptr = ccs_ptr->class_intf_handle;
    USB_STATUS                    status;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_control_init");
#endif

    /* Make sure the device is still attached */
    USB_lock(); //not needed as init is called from "already locked" function
    status = usb_host_class_intf_init(pbs_ptr, if_ptr, &audio_control_anchor, NULL);
    if (status == USB_OK) {
        /*
        ** We generate a code_key based on the attached device. This is used to
        ** verify that the device has not been detached and replaced with another.
        */
        ccs_ptr->code_key = 0;
        ccs_ptr->code_key = usb_host_class_intf_validate(ccs_ptr);

        if_ptr->AUDIO_G.IFNUM = ((INTERFACE_DESCRIPTOR_PTR)if_ptr->AUDIO_G.G.intf_handle)->bInterfaceNumber;
        if_ptr->interrupt_pipe = usb_hostdev_get_pipe_handle(pbs_ptr, USB_INTERRUPT_PIPE, 0);
  
        if (MQX_OK != (status = _lwevent_create(&if_ptr->control_event, 0))) {
           status = USBERR_INIT_FAILED;
        }
        else {
        /* prepare events to be auto or manual */
            _lwevent_set_auto_clear(&if_ptr->control_event, USB_AUDIO_CTRL_PIPE_FREE | USB_AUDIO_CTRL_INT_PIPE_FREE);
            /* pre-set events */
            _lwevent_set(&if_ptr->control_event, USB_AUDIO_CTRL_PIPE_FREE | USB_AUDIO_CTRL_INT_PIPE_FREE);
        }
    } /* Endif */

    /* Signal that an error has occured by setting the "code_key" */
    if (status) {
        ccs_ptr->code_key = 0;
    } /* Endif */

    USB_unlock();

#ifdef _HOST_DEBUG_
    if (status) {
        DEBUG_LOG_TRACE("usb_class_audio_control_init, SUCCESSFUL");
    }
    else {
        DEBUG_LOG_TRACE("usb_class_audio_control_init, FAILED");
    }
#endif

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_stream_init
* Returned Value : None
* Comments       :
*     This function is called by common class to initialize the class driver
*     for audio stream interface. It is called in response to a select
*     interface call by application.
*
*END*--------------------------------------------------------------------*/
void usb_class_audio_stream_init
(
    /* [IN]  structure with USB pipe information on the interface */
    PIPE_BUNDLE_STRUCT_PTR      pbs_ptr,

    /* [IN] stream call struct pointer */
    CLASS_CALL_STRUCT_PTR       ccs_ptr
)
{ /* Body */
    AUDIO_STREAM_INTERFACE_STRUCT_PTR if_ptr = ccs_ptr->class_intf_handle;
    USB_STATUS                     status;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_stream_init");
#endif


    /* Make sure the device is still attached */
    USB_lock(); //not needed as init is called from "already locked" function
    status = usb_host_class_intf_init(pbs_ptr, if_ptr, &audio_stream_anchor, NULL);
    if (status == USB_OK) {

        /* Check the audio stream interface alternating 1 */
        if (0 != (((INTERFACE_DESCRIPTOR_PTR)if_ptr->AUDIO_G.G.intf_handle)->bNumEndpoints)) {
            /*
            ** We generate a code_key based on the attached device. This is used to
            ** verify that the device has not been detached and replaced with another.
            */
            ccs_ptr->code_key = 0;
            ccs_ptr->code_key = usb_host_class_intf_validate(ccs_ptr);

            if_ptr->AUDIO_G.IFNUM = ((INTERFACE_DESCRIPTOR_PTR)if_ptr->AUDIO_G.G.intf_handle)->bInterfaceNumber;

            if_ptr->iso_in_pipe = usb_hostdev_get_pipe_handle(pbs_ptr, USB_ISOCHRONOUS_PIPE, USB_RECV);
            if_ptr->iso_out_pipe = usb_hostdev_get_pipe_handle(pbs_ptr, USB_ISOCHRONOUS_PIPE, USB_SEND);

            if ((if_ptr->iso_in_pipe == NULL) && (if_ptr->iso_out_pipe == NULL)) {
                status = USBERR_OPEN_PIPE_FAILED;
            }
        } else {
            status = USBERR_INIT_FAILED;
        }
    } /* Endif */

    /* Signal that an error has occured by setting the "code_key" */
    if (status) {
        ccs_ptr->code_key = 0;
    } /* Endif */

    USB_unlock();

#ifdef _HOST_DEBUG_
    if (status) {
        DEBUG_LOG_TRACE("usb_class_audio_stream_init, SUCCESSFUL");
    }
    else {
        DEBUG_LOG_TRACE("usb_class_audio_stream_init, FAILED");
    }
#endif

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_get_ctrl_interface
* Returned Value : control interface instance
* Comments       :
*     This function is used to find registered control interface in the chain.
*     It must be run with interrupts disabled to have interfaces validated.
*END*--------------------------------------------------------------------*/

CLASS_CALL_STRUCT_PTR usb_class_audio_get_ctrl_interface
(
    /* [IN] pointer to interface handle */
    void             *intf_handle
)
{
    CLASS_CALL_STRUCT_PTR   control_parser;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_get_ctrl_interface");
#endif

    /* find this control interface in the list with interface descriptor */
    for (control_parser = audio_control_anchor; control_parser != NULL; control_parser = control_parser->next)
        if (usb_host_class_intf_validate(control_parser))
            if (((AUDIO_CONTROL_INTERFACE_STRUCT_PTR) control_parser->class_intf_handle)->AUDIO_G.G.intf_handle == intf_handle)
                break;

#ifdef _HOST_DEBUG_
    if (control_parser != NULL) {
        DEBUG_LOG_TRACE("usb_class_audio_get_ctrl_interface, SUCCESSFULL");
    }
    else {
        DEBUG_LOG_TRACE("usb_class_audio_get_ctrl_interface, FAILED");
    }
#endif

    return control_parser;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_control_get_descriptors
* Returned Value : USB_OK
* Comments       :
*     This function is hunting for descriptors in the device configuration
*     and fills back fields if the descriptor was found.
*     Must be run in locked state and validated USB device or
*     directly from attach event.
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_control_get_descriptors
(
    /* [IN] pointer to device instance */
    _usb_device_instance_handle      			dev_handle,

    /* [IN] pointer to interface descriptor */
    _usb_interface_descriptor_handle 			intf_handle,

    /* [OUT] pointer to header descriptor */
    USB_AUDIO_CTRL_DESC_HEADER_PTR         *header_desc,

    /* [OUT] pointer to input terminal descriptor */
    USB_AUDIO_CTRL_DESC_IT_PTR         	*it_desc,

    /* [OUT] pointer to output terminal descriptor */
    USB_AUDIO_CTRL_DESC_OT_PTR         	*ot_desc,

    /* [OUT] pointer to feature unit descriptor */
    USB_AUDIO_CTRL_DESC_FU_PTR      		*fu_desc
)
{
    INTERFACE_DESCRIPTOR_PTR   intf_ptr = (INTERFACE_DESCRIPTOR_PTR)intf_handle;
    USB_STATUS                 status;
    int32_t i;
    USB_AUDIO_CTRL_FUNC_DESC_PTR      fd;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_control_get_descriptors");
#endif

    status = USB_OK;
    /* collect all interface functional descriptors */
    for (i = 0; ; i++) {
        if (USB_OK != _usb_hostdev_get_descriptor(
                    dev_handle,
                    intf_handle,
                    USB_DESC_TYPE_CS_INTERFACE,  /* Functional descriptor for interface */
                    i,                           /* Index of descriptor */
                    intf_ptr->bAlternateSetting, /* Index of interface alternate */
                    (void **) &fd))
        {
            /* Here we can check capabilities from functional descriptors */
            /* But for now, nothing is checked */
            if (fu_desc == NULL)
                status = USBERR_INIT_FAILED;
            break;
        }
        /* Check if this union descriptor describes master for this interface */
        switch (fd->header.bDescriptorSubtype) {
        case USB_DESC_SUBTYPE_AUDIO_CS_HEADER:
            *header_desc = &fd->header;
            if (SHORT_UNALIGNED_LE_TO_HOST(((*header_desc)->bcdCDC)) > 0x0110)
                status = USBERR_INIT_FAILED;
            break;
        case USB_DESC_SUBTYPE_AUDIO_CS_IT:
            *it_desc = &fd->it;
            break;
        case USB_DESC_SUBTYPE_AUDIO_CS_OT:
            *ot_desc = &fd->ot;
            break;
        case USB_DESC_SUBTYPE_AUDIO_CS_FU:
            *fu_desc = &fd->fu;
            break;
        }
        if (status != USB_OK)
            break;
    }

#ifdef _HOST_DEBUG_
    if (!status) {
        DEBUG_LOG_TRACE("usb_class_audio_control_get_descriptors, SUCCESSFULL");
    }
    else {
        DEBUG_LOG_TRACE("usb_class_audio_control_get_descriptors, FAILED");
    }
#endif

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_stream_get_descriptors
* Returned Value : USB_OK
* Comments       :
*     This function is hunting for descriptors in the device configuration
*     and fills back fields if the descriptor was found.
*     Must be run in locked state and validated USB device or
*     directly from attach event.
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_audio_stream_get_descriptors
(
    /* [IN] pointer to device instance */
    _usb_device_instance_handle      dev_handle,

    /* [IN] pointer to interface descriptor */
    _usb_interface_descriptor_handle intf_handle,

    /* [OUT] pointer to specific audio stream interface descriptor */
    USB_AUDIO_STREAM_DESC_SPEPIFIC_AS_IF_PTR        *as_itf_desc,

    /* [OUT] pointer to format type descriptor */
    USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR         	 *frm_type_desc,

    /* [OUT] pointer to specific isochronous endpoint descriptor */
    USB_AUDIO_STREAM_DESC_SPECIFIC_ISO_ENDP_PTR 		*iso_endp_spec_desc
)
{
    INTERFACE_DESCRIPTOR_PTR   intf_ptr =
        (INTERFACE_DESCRIPTOR_PTR)intf_handle;
    USB_STATUS                 status;
    _mqx_int i;
    USB_AUDIO_STREAM_FUNC_DESC_PTR      fd;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_stream_get_descriptors");
#endif

    status = USB_OK;
    /* collect all interface functional descriptors */
    for (i = 0; ; i++) {
        if (USB_OK != _usb_hostdev_get_descriptor(
                    dev_handle,
                    intf_handle,
                    USB_DESC_TYPE_CS_INTERFACE,  /* Functional descriptor for interface */
                    i,                           /* Index of descriptor */
                    intf_ptr->bAlternateSetting, /* Index of interface alternate */
                    (void **) &fd))
        {
            /* Here we can check capabilities from functional descriptors */
            /* But for now, nothing is checked */
            if (as_itf_desc == NULL)
                status = USBERR_INIT_FAILED;
            break;
        }
        /* Check if this union descriptor describes master for this interface */
        switch (fd->as_general.bDescriptorSubtype) {
        case USB_DESC_SUBTYPE_AS_CS_GENERAL:
            *as_itf_desc = &fd->as_general;
            break;
        case USB_DESC_SUBTYPE_AS_CS_FORMAT_TYPE:
            *frm_type_desc = &fd->frm_type;
            break;
        }
        if (status != USB_OK)
            break;
    }

    /* collect all interface functional descriptors */
    for (i = 0; ; i++) {
        if (USB_OK != _usb_hostdev_get_descriptor(
                    dev_handle,
                    intf_handle,
                    USB_DESC_TYPE_CS_ENDPOINT,  /* Functional descriptor for interface */
                    i,                           /* Index of descriptor */
                    intf_ptr->bAlternateSetting, /* Index of interface alternate */
                    (void **) &fd))
        {
            /* Here we can check capabilities from functional descriptors */
            /* But for now, nothing is checked */
            if (as_itf_desc == NULL)
                status = USBERR_INIT_FAILED;
            break;
        }
        /* Check if this union descriptor describes master for this interface */
        switch (fd->as_general.bDescriptorSubtype) {
        case USB_DESC_CLASS_ENDPOINT_GENERAL:
            *iso_endp_spec_desc = &fd->iso_endp_specific;
            break;
        default:
            break;
        }
        if (status != USB_OK)
            break;
    }

#ifdef _HOST_DEBUG_
    if (!status) {
        DEBUG_LOG_TRACE("usb_class_audio_stream_get_descriptors, SUCCESSFULL");
    }
    else {
        DEBUG_LOG_TRACE("usb_class_audio_stream_get_descriptors, FAILED");
    }
#endif

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_control_set_descriptors
* Returned Value : USB_OK if validation passed
* Comments       :
*     This function is used to set descriptors for control interface
*     Descriptors can be used afterwards by application or by driver
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_audio_control_set_descriptors
(
    /* [IN] class call struct pointer */
    CLASS_CALL_STRUCT_PTR            ccs_ptr,

    /* [IN] header descriptor pointer */
    USB_AUDIO_CTRL_DESC_HEADER_PTR   header_desc,

    /* [IN] input terminal descriptor pointer */
    USB_AUDIO_CTRL_DESC_IT_PTR       it_desc,

    /* [IN] output terminal descriptor pointer */
    USB_AUDIO_CTRL_DESC_OT_PTR       ot_desc,

    /* [IN] feature unit descriptor pointer */
    USB_AUDIO_CTRL_DESC_FU_PTR       fu_desc
)
{ /* Body */
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR if_ptr;
    USB_STATUS                    status = USBERR_NO_INTERFACE;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_control_set_descriptors");
#endif

    /* Make sure the device is still attached */
    USB_lock();

    /* Validity checking */
    if (usb_host_class_intf_validate(ccs_ptr)) {
        if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR)ccs_ptr->class_intf_handle;

        if_ptr->header_desc = header_desc;
        if_ptr->it_desc = it_desc;
        if_ptr->ot_desc = ot_desc;
        if_ptr->fu_desc = fu_desc;

    } /* Endif */

    USB_unlock();

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_control_set_descriptors, SUCCESSFUL");
#endif

    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_stream_set_descriptors
* Returned Value : USB_OK if validation passed
* Comments       :
*     This function is used to set descriptors for stream interface
*     Descriptors can be used afterwards by application or by driver
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_audio_stream_set_descriptors
(
    /* [IN] class call struct pointer */
    CLASS_CALL_STRUCT_PTR            			ccs_ptr,

    /* [IN] audio stream interface descriptor pointer */
    USB_AUDIO_STREAM_DESC_SPEPIFIC_AS_IF_PTR    as_itf_desc,

    /* [IN] format type descriptor pointer */
    USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR       frm_type_desc,

    /* [IN] isochonous endpoint specific descriptor pointer */
    USB_AUDIO_STREAM_DESC_SPECIFIC_ISO_ENDP_PTR iso_endp_spec_desc
)
{ /* Body */
    AUDIO_STREAM_INTERFACE_STRUCT_PTR if_ptr;
    USB_STATUS                    status = USBERR_NO_INTERFACE;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_stream_set_descriptors");
#endif

    /* Make sure the device is still attached */
    USB_lock();

    /* Validity checking */
    if (usb_host_class_intf_validate(ccs_ptr)) {
        if_ptr = (AUDIO_STREAM_INTERFACE_STRUCT_PTR)ccs_ptr->class_intf_handle;

        if_ptr->as_itf_desc 		= as_itf_desc;
        if_ptr->frm_type_desc 		= frm_type_desc;
        if_ptr->iso_endp_spec_desc 	= iso_endp_spec_desc;
    } /* Endif */

    USB_unlock();

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_audio_control_set_descriptors, SUCCESSFUL");
#endif

    return status;
} /* Endbody */
/* EOF */
