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
* @brief The file contains USB Mass Storage SCSI command set.
* 
*END************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <mqx.h>
#include "usb_stack_config.h"

#if !MQX_USE_IO_OLD
#include <stdio.h>
#endif

#if MSD_CONFIG
#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "usb_msc.h"
#include "usb_msc_scsi.h"

/*****************************************************************************
* Constant and Macro's
*****************************************************************************/
INQUIRY_DATA_STRUCT inquiry_info = /* constant */
{
   (PERIPHERAL_QUALIFIER << PERIPHERAL_QUALIFIER_SHIFT)|PERIPHERAL_DEVICE_TYPE,
   (uint8_t)REMOVABLE_MEDIUM_BIT << REMOVABLE_MEDIUM_BIT_SHIFT,
   SPC_VERSION, 0x02, ADDITIONAL_LENGTH, 0x00,0x00,0x00,
   {'F', 'S', 'L', ' ', 'S', 'E', 'M', 'I'},
   {'F','S','L',' ','M','A','S','S',' ','S','T','O','R','A','G','E'},
   {'0', '0', '0', '1'}
};

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
/****************************************************************************
 * Global Variables
 ****************************************************************************/
/* Add all the variables needed for usb_msc.c to this structure */
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
 uint8_t msc_thirteen_cases_check(MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,PTR_MSC_THIRTEEN_CASE_STRUCT msc_check_event);
/*****************************************************************************
 * Local Variables
 *****************************************************************************/

 /*****************************************************************************
 * Local Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  msc_thirteen_cases_check
 *
 * @brief The function checks for thirteen error case of MSC and takes action
 *        appropriately
 *
 * @param scsi_ptr->thirteen_case_ptr: structure containing all necessary parameter to
 *        evaluate error scenarios
 *
 * @return error
 *
 *****************************************************************************/
uint8_t msc_thirteen_cases_check(MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
PTR_MSC_THIRTEEN_CASE_STRUCT msc_check_event
)
{
    uint8_t             error = USBERR_ERROR;
    MSC_SCSI_STRUCT_PTR scsi_ptr;

    scsi_ptr = (MSC_SCSI_STRUCT_PTR)msc_obj_ptr->scsi_object_ptr;

    if(!msc_check_event->host_expected_data_len)
    {   /* host expects no data transfer */
        *(msc_check_event->csw_residue_ptr) = 0;

        if(!msc_check_event->device_expected_data_len)
        {   /* CASE 1: Device intends no data transfer : Thin Diagonal Case*/
            *(msc_check_event->csw_status_ptr) = COMMAND_PASSED;
        }
        else
        {   /* if(msc_check_event->device_expected_direction) :
                CASE 2: Device intends to send data to host
               else
                CASE 3: Device intends to receive data from host */
            *(msc_check_event->csw_status_ptr) = PHASE_ERROR;
        }
    }
    else if(msc_check_event->host_expected_direction)
    {   /* host expects to receive data from device (USB_SEND direction)*/
        if(!msc_check_event->device_expected_data_len)
        {   /* CASE 4:  Device intends no data transfer */
            *(msc_check_event->csw_residue_ptr) =
                msc_check_event->host_expected_data_len -
                msc_check_event->device_expected_data_len;
            /* sending zero bytes of data */
            error = USB_MSC_Bulk_Send_Data(msc_obj_ptr->msc_handle,
                msc_check_event->buffer_ptr,
                msc_check_event->device_expected_data_len);

            if(error == USB_OK)
            {
                *(msc_check_event->csw_status_ptr) = COMMAND_PASSED;
            }
            else
            {
                *(msc_check_event->csw_status_ptr) = COMMAND_FAILED;
                scsi_ptr->request_sense.sense_key = MEDIUM_ERROR;
                scsi_ptr->request_sense.add_sense_code = UNRECOVERED_READ_ERROR;
            }

            /* BULK IN PIPE TO BE STALLED for status phase */
            error = USBERR_ENDPOINT_STALLED;
        }
        else if(msc_check_event->device_expected_direction)
        {   /* device intends to send data to host */
            if(msc_check_event->host_expected_data_len >
                msc_check_event->device_expected_data_len)
            {   /* CASE 5: Host intends more data to receive than device
                           intends to send*/
                *(msc_check_event->csw_residue_ptr) =
                msc_check_event->host_expected_data_len -
                msc_check_event->device_expected_data_len;

                if(scsi_ptr->thirteen_case_ptr->lba_txrx_select == TRUE)
                {
                    error = USB_MSC_LBA_Transfer(msc_obj_ptr,USB_SEND,
                        &msc_check_event->lba_info);
                }
                else
                {
                    error = USB_MSC_Bulk_Send_Data(msc_obj_ptr->msc_handle,
                        msc_check_event->buffer_ptr,
                        msc_check_event->device_expected_data_len);
                }

                if(msc_check_event->device_expected_data_len%
                    msc_obj_ptr->bulk_in_endpoint_packet_size == 0)
                {   /* need to send zero bytes of data to tell host that device
                       does not have any more data. This is needed only if the
                       bytes send to host are integral multiple of max packet
                       size of Bulk In endpoint */
                    error |= USB_MSC_Bulk_Send_Data(msc_obj_ptr->msc_handle,
                        msc_check_event->buffer_ptr,0);
                }

                if(error == USB_OK)
                {
                    *(msc_check_event->csw_status_ptr) = COMMAND_PASSED;
                }
                else
                {
                    *(msc_check_event->csw_status_ptr) = COMMAND_FAILED;
                    scsi_ptr->request_sense.sense_key = MEDIUM_ERROR;
                    scsi_ptr->request_sense.add_sense_code = UNRECOVERED_READ_ERROR;
                }
            }
            else if(msc_check_event->host_expected_data_len ==
                msc_check_event->device_expected_data_len)
            {   /* CASE 6: Host intends exact amount of data to receive
                           as device intends to send : Thin Diagonal Case*/
                *(msc_check_event->csw_residue_ptr) = 0;

                if(scsi_ptr->thirteen_case_ptr->lba_txrx_select == TRUE)
                {
                    error = USB_MSC_LBA_Transfer(msc_obj_ptr,USB_SEND,
                        &msc_check_event->lba_info);
                }
                else
                {
                    error = USB_MSC_Bulk_Send_Data(msc_obj_ptr->msc_handle,
                        msc_check_event->buffer_ptr,
                        msc_check_event->device_expected_data_len);
                }

                if(error == USB_OK)
                {
                    *(msc_check_event->csw_status_ptr) = COMMAND_PASSED;
                }
                else
                {
                    *(msc_check_event->csw_status_ptr) = COMMAND_FAILED;
                    scsi_ptr->request_sense.sense_key = MEDIUM_ERROR;
                    scsi_ptr->request_sense.add_sense_code = UNRECOVERED_READ_ERROR;
                }
            }
            else
            {
                /* CASE 7: Host intends less data to receive than device
                           intends to send*/
                *(msc_check_event->csw_residue_ptr) = 0;

                if(scsi_ptr->thirteen_case_ptr->lba_txrx_select == TRUE)
                {
                    error = USB_MSC_LBA_Transfer(msc_obj_ptr,USB_SEND,
                        &msc_check_event->lba_info);
                }
                else
                {
                    error = USB_MSC_Bulk_Send_Data(msc_obj_ptr->msc_handle,
                        msc_check_event->buffer_ptr,
                        msc_check_event->host_expected_data_len);
                }

                if(error == USB_OK)
                {
                    *(msc_check_event->csw_status_ptr) = COMMAND_PASSED;
                }
                else
                {
                    *(msc_check_event->csw_status_ptr) = COMMAND_FAILED;
                    scsi_ptr->request_sense.sense_key = MEDIUM_ERROR;
                    scsi_ptr->request_sense.add_sense_code = UNRECOVERED_READ_ERROR;
                }
            }
        }
        else
        {   /* CASE 8: Device intends to receive data from host */
            *(msc_check_event->csw_residue_ptr) =
                msc_check_event->host_expected_data_len;
            /* device has no data to send */
            error = USB_MSC_Bulk_Send_Data(msc_obj_ptr->msc_handle,
                msc_check_event->buffer_ptr,0);
            *(msc_check_event->csw_status_ptr) = PHASE_ERROR;
            /* BULK IN PIPE TO BE STALLED for status phase */
            error = USBERR_ENDPOINT_STALLED;
        }
    }
    else
    {   /* host expects to send data to device (USB_RECV direction)*/
        if(!msc_check_event->device_expected_data_len)
        {   /* CASE 9:  Device intends no data transfer */
            uint8_t component = (uint8_t)(msc_obj_ptr->bulk_out_endpoint |
                (USB_RECV<<COMPONENT_PREPARE_SHIFT));
            (void)_usb_device_set_status(msc_obj_ptr->controller_handle,
                (uint8_t)(component|USB_STATUS_ENDPOINT),
                (uint16_t)USB_STATUS_STALLED);
            *(msc_check_event->csw_residue_ptr) =
                msc_check_event->host_expected_data_len;
            *(msc_check_event->csw_status_ptr) = COMMAND_FAILED;
            /* BULK OUT PIPE STALLED */
            error = USBERR_ENDPOINT_STALLED;
        }
        else if(msc_check_event->device_expected_direction)
        {   /*CASE10: device intends to send data to host */
            uint8_t component = (uint8_t)(msc_obj_ptr->bulk_out_endpoint |
                (uint8_t)(USB_RECV<<COMPONENT_PREPARE_SHIFT));
            /* now, stalling the status phase - CASE 5th of THIRTEEN CASES*/
            (void)_usb_device_set_status(msc_obj_ptr->controller_handle,
                (uint8_t)(component|USB_STATUS_ENDPOINT),
                (uint16_t)USB_STATUS_STALLED);
            *(msc_check_event->csw_residue_ptr) =
                msc_check_event->host_expected_data_len;
            *(msc_check_event->csw_status_ptr) = PHASE_ERROR;
            /* BULK OUT PIPE STALLED */
            error = USBERR_ENDPOINT_STALLED;
        }
        else
        {   /*Device intends to receive data from host */
            if(msc_check_event->host_expected_data_len >
                msc_check_event->device_expected_data_len)
            {   /* CASE 11: Host intends more data to send than device
                            intends to receive*/
                *(msc_check_event->csw_residue_ptr) =
                    msc_check_event->host_expected_data_len -
                    msc_check_event->device_expected_data_len;

                if(scsi_ptr->thirteen_case_ptr->lba_txrx_select == TRUE)
                {
                    error = USB_MSC_LBA_Transfer(msc_obj_ptr,USB_RECV,
                        &msc_check_event->lba_info);
                }
                else
                {
                    error = USB_MSC_Bulk_Recv_Data(msc_obj_ptr->controller_handle,
                        msc_check_event->buffer_ptr,
                        msc_check_event->device_expected_data_len);
                }

                if(error == USB_OK)
                {
                    *(msc_check_event->csw_status_ptr) = COMMAND_PASSED;
                }
                else
                {
                    *(msc_check_event->csw_status_ptr) = COMMAND_FAILED;
                    scsi_ptr->request_sense.sense_key = MEDIUM_ERROR;
                    scsi_ptr->request_sense.add_sense_code = WRITE_FAULT;
                }
                /* BULK OUT PIPE TO BE STALLED for status phase */
                error = USBERR_ENDPOINT_STALLED;
            }
            else if(msc_check_event->host_expected_data_len ==
                msc_check_event->device_expected_data_len)
            {   /* CASE 12: Host intends exact amount of data to send
                            as device intends to receive : Thin Diagonal Case*/
                *(msc_check_event->csw_residue_ptr) = 0;

                if(scsi_ptr->thirteen_case_ptr->lba_txrx_select == TRUE)
                {
                    error = USB_MSC_LBA_Transfer(msc_obj_ptr,USB_RECV,
                        &msc_check_event->lba_info);
                }
                else
                {
                    error = USB_MSC_Bulk_Recv_Data(msc_obj_ptr->controller_handle,
                        msc_check_event->buffer_ptr,
                        msc_check_event->device_expected_data_len);
                }

                if(error == USB_OK)
                {
                    *(msc_check_event->csw_status_ptr) = COMMAND_PASSED;
                }
                else
                {
                    *(msc_check_event->csw_status_ptr) = COMMAND_FAILED;
                    scsi_ptr->request_sense.sense_key = MEDIUM_ERROR;
                    scsi_ptr->request_sense.add_sense_code = WRITE_FAULT;
                }
            }
            else
            {
                /* CASE 13: Host intends less data to send than device
                            intends to receive*/
                *(msc_check_event->csw_residue_ptr) = 0;

                if(scsi_ptr->thirteen_case_ptr->lba_txrx_select == TRUE)
                {
                    msc_check_event->lba_info.lba_transfer_num =
                      msc_check_event->host_expected_data_len /
                        scsi_ptr->length_of_each_lab;
                    error = USB_MSC_LBA_Transfer(msc_obj_ptr,USB_RECV,
                        &msc_check_event->lba_info);
                }
                else
                {
                    error = USB_MSC_Bulk_Recv_Data(msc_obj_ptr->controller_handle,
                        msc_check_event->buffer_ptr,
                        msc_check_event->host_expected_data_len);
                }

                if(error != USB_OK)
                {
                    scsi_ptr->request_sense.sense_key = MEDIUM_ERROR;
                    scsi_ptr->request_sense.add_sense_code = WRITE_FAULT;
                }
                *(msc_check_event->csw_status_ptr) = PHASE_ERROR;
            }
        }
    }
    return error;
}

/*****************************************************************************
 * Global Functions
 *****************************************************************************/

 /**************************************************************************//*!
 *
 * @name  USB_MSC_SCSI_Init
 *
 * @brief The function initializes the SCSI parameters and callbacks
 *
 * @param msc_obj_ptr         MSD class object pointer.
 * @param cb:                 event callback
 * @param device_info_ptr   : Contains MSD device info like number of LBA, etc
 * @return status
 *         USB_OK           : When Successful
 *         Others           : Errors
 *
 *****************************************************************************/
uint8_t USB_MSC_SCSI_Init
(
     MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
     USB_CLASS_CALLBACK_STRUCT_PTR cb,
     PTR_DEVICE_LBA_INFO_STRUCT device_info_ptr,
     uint32_t implementing_disk_drive
)
{
    MSC_SCSI_STRUCT_PTR scsi_ptr = NULL;
    /* initialize the Global Variable Structure */
    scsi_ptr  = (MSC_SCSI_STRUCT_PTR)USB_mem_alloc_zero(sizeof(MSC_SCSI_STRUCT));
    if (NULL == scsi_ptr)
    {
        #if _DEBUG
            printf("1: USB_MSC_SCSI_Init: Memalloc failed\n");
        #endif
        return USBERR_ALLOC;
    }

    /* save input parameters */
    scsi_ptr->scsi_callback.callback = cb->callback;
    scsi_ptr->scsi_callback.arg = cb->arg;
    scsi_ptr->total_logical_add_block =
        device_info_ptr->total_lba_device_supports;
    scsi_ptr->length_of_each_lab =
        device_info_ptr->length_of_each_lab_of_device;
    scsi_ptr->implementing_disk_drive = implementing_disk_drive;

    /* no need to initialize other structure fields as g_resquest_sense has
       been declared as static */
    scsi_ptr->request_sense.valid_error_code = REQ_SENSE_VALID_ERROR_CODE;
    scsi_ptr->request_sense.add_sense_len = REQ_SENSE_ADDITIONAL_SENSE_LEN;
    scsi_ptr->request_sense.sense_key = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code_qual = NO_SENSE;
    scsi_ptr->thirteen_case_ptr = NULL; /* Initialize */

    scsi_ptr->thirteen_case_ptr = (PTR_MSC_THIRTEEN_CASE_STRUCT)USB_mem_alloc_zero(
        sizeof(MSC_THIRTEEN_CASE_STRUCT));
    if (NULL == scsi_ptr->thirteen_case_ptr)
    {
        #if _DEBUG
            printf("2: USB_MSC_SCSI_Init: Memalloc failed\n");
        #endif
        return USBERR_ALLOC;
    }

    msc_obj_ptr->scsi_object_ptr = scsi_ptr;
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  msc_request_sense_command
 *
 * @brief command instructs the Device to transfer sense data to host computer
 *
 * @param handle
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return NONE
 *
 *****************************************************************************/
uint8_t msc_request_sense_command
(
    MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
    PTR_CBW cbw_ptr,
    uint32_t* csw_residue_ptr,
    uint8_t* csw_status_ptr
)
{
    uint8_t error;
    MSC_SCSI_STRUCT_PTR scsi_ptr;

    scsi_ptr = (MSC_SCSI_STRUCT_PTR)msc_obj_ptr->scsi_object_ptr;

    scsi_ptr->thirteen_case_ptr->handle = msc_obj_ptr->controller_handle;
    scsi_ptr->thirteen_case_ptr->host_expected_data_len = cbw_ptr->data_length;
    scsi_ptr->thirteen_case_ptr->host_expected_direction =
        (uint8_t)(cbw_ptr->flag >> USB_CBW_DIRECTION_SHIFT);
    scsi_ptr->thirteen_case_ptr->device_expected_data_len = REQ_SENSE_DATA_LENGTH;
    scsi_ptr->thirteen_case_ptr->device_expected_direction = USB_SEND;
    scsi_ptr->thirteen_case_ptr->csw_status_ptr = csw_status_ptr;
    scsi_ptr->thirteen_case_ptr->csw_residue_ptr = csw_residue_ptr;
    scsi_ptr->thirteen_case_ptr->buffer_ptr = (uint8_t *)&scsi_ptr->request_sense;
    scsi_ptr->thirteen_case_ptr->lba_txrx_select = FALSE;

    error = msc_thirteen_cases_check(msc_obj_ptr, scsi_ptr->thirteen_case_ptr);

    return error;
}

/**************************************************************************//*!
 *
 * @name  msc_inquiry_command
 *
 * @brief It requests that information regarding parameters of the Device be
 *        sent to the Host Computer
 *
 * @param handle
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
uint8_t msc_inquiry_command
(
    MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
    PTR_CBW cbw_ptr,
    uint32_t* csw_residue_ptr,
    uint8_t* csw_status_ptr
)
{
    MSC_SCSI_STRUCT_PTR scsi_ptr;
    uint8_t error = USBERR_TX_FAILED;

    scsi_ptr = (MSC_SCSI_STRUCT_PTR)msc_obj_ptr->scsi_object_ptr;

    /* initialize sense code values */
    scsi_ptr->request_sense.sense_key = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code_qual = NO_SENSE;

    scsi_ptr->thirteen_case_ptr->handle = msc_obj_ptr->controller_handle;
    scsi_ptr->thirteen_case_ptr->host_expected_data_len = cbw_ptr->data_length;
    scsi_ptr->thirteen_case_ptr->host_expected_direction =
        (uint8_t)(cbw_ptr->flag >> USB_CBW_DIRECTION_SHIFT);
    scsi_ptr->thirteen_case_ptr->device_expected_data_len = INQUIRY_ALLOCATION_LENGTH;
    scsi_ptr->thirteen_case_ptr->device_expected_direction = USB_SEND;
    scsi_ptr->thirteen_case_ptr->csw_status_ptr = csw_status_ptr;
    scsi_ptr->thirteen_case_ptr->csw_residue_ptr = csw_residue_ptr;
    scsi_ptr->thirteen_case_ptr->buffer_ptr = (uint8_t *)&inquiry_info;
    scsi_ptr->thirteen_case_ptr->lba_txrx_select = FALSE;

    error = msc_thirteen_cases_check(msc_obj_ptr,scsi_ptr->thirteen_case_ptr);

    return error;
}

/**************************************************************************//*!
 *
 * @name  msc_read_command
 *
 * @brief command requests that device transfer data to the host computer
 *        (read (10) command)
 * @param handle
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
uint8_t msc_read_command
(
    MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
    PTR_CBW cbw_ptr,
    uint32_t* csw_residue_ptr,
    uint8_t* csw_status_ptr
)
{
    MSC_SCSI_STRUCT_PTR scsi_ptr;
    uint32_t lba = 0;
    uint32_t num_lba_tx = 0;
    uint8_t error = USBERR_TX_FAILED;

    scsi_ptr = (MSC_SCSI_STRUCT_PTR)msc_obj_ptr->scsi_object_ptr;

    /* initialize sense code values */
    scsi_ptr->request_sense.sense_key = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code_qual = NO_SENSE;

    lba  = ((uint32_t)cbw_ptr->command_block[2] << 24);
    lba |= ((uint32_t)cbw_ptr->command_block[3] << 16);
    lba |= ((uint32_t)cbw_ptr->command_block[4] << 8);
    lba |= ((uint32_t)cbw_ptr->command_block[5]);

    if(cbw_ptr->command_block[0] == READ_10_COMMAND)
    {
        num_lba_tx = (uint16_t)((uint16_t)cbw_ptr->command_block[7] << 8);
        num_lba_tx |= (uint16_t)cbw_ptr->command_block[8];
    }
    else if(cbw_ptr->command_block[0] == READ_12_COMMAND)
    {
        num_lba_tx  = ((uint32_t)cbw_ptr->command_block[6] << 24);
        num_lba_tx |= ((uint32_t)cbw_ptr->command_block[7] << 16);
        num_lba_tx |= ((uint32_t)cbw_ptr->command_block[8] << 8);
        num_lba_tx |= ((uint32_t)cbw_ptr->command_block[9]);
    }

    scsi_ptr->thirteen_case_ptr->handle = msc_obj_ptr->controller_handle;
    scsi_ptr->thirteen_case_ptr->host_expected_data_len = cbw_ptr->data_length;
    scsi_ptr->thirteen_case_ptr->host_expected_direction =
        (uint8_t)(cbw_ptr->flag >> USB_CBW_DIRECTION_SHIFT);
    scsi_ptr->thirteen_case_ptr->device_expected_direction = USB_SEND;
    scsi_ptr->thirteen_case_ptr->csw_status_ptr = csw_status_ptr;
    scsi_ptr->thirteen_case_ptr->csw_residue_ptr = csw_residue_ptr;

    scsi_ptr->thirteen_case_ptr->device_expected_data_len =
        scsi_ptr->length_of_each_lab * num_lba_tx;
    scsi_ptr->thirteen_case_ptr->buffer_ptr = NULL;

    scsi_ptr->thirteen_case_ptr->lba_txrx_select = TRUE;
    scsi_ptr->thirteen_case_ptr->lba_info.starting_lba = lba;
    scsi_ptr->thirteen_case_ptr->lba_info.lba_transfer_num = num_lba_tx;

    error = msc_thirteen_cases_check(msc_obj_ptr,scsi_ptr->thirteen_case_ptr);
    return error;
}

/**************************************************************************//*!
 *
 * @name  msc_write_command
 *
 * @brief command requests that the Device write the data transferred by the
 *        Host Computer to the medium.
 *
 * @param handle
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
uint8_t msc_write_command
(
    MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
    PTR_CBW cbw_ptr,
    uint32_t* csw_residue_ptr,
    uint8_t* csw_status_ptr
)
{
    MSC_SCSI_STRUCT_PTR scsi_ptr;
    uint32_t lba = 0;
    uint32_t num_lba_rx = 0;
    uint8_t error = USBERR_RX_FAILED;

    scsi_ptr = (MSC_SCSI_STRUCT_PTR)msc_obj_ptr->scsi_object_ptr;

    /* initialize sense code values */
    scsi_ptr->request_sense.sense_key = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code_qual = NO_SENSE;

    lba  = ((uint32_t)cbw_ptr->command_block[2] << 24);
    lba |= ((uint32_t)cbw_ptr->command_block[3] << 16);
    lba |= ((uint32_t)cbw_ptr->command_block[4] << 8);
    lba |= ((uint32_t)cbw_ptr->command_block[5]);


    if(cbw_ptr->command_block[0] == WRITE_10_COMMAND)
    {
        num_lba_rx = (uint16_t)((uint16_t)cbw_ptr->command_block[7] << 8);
        num_lba_rx |= (uint16_t)cbw_ptr->command_block[8];
    }
    else if(cbw_ptr->command_block[0] == WRITE_12_COMMAND)
    {
        num_lba_rx  = ((uint32_t)cbw_ptr->command_block[6] << 24);
        num_lba_rx |= ((uint32_t)cbw_ptr->command_block[7] << 16);
        num_lba_rx |= ((uint32_t)cbw_ptr->command_block[8] << 8);
        num_lba_rx |= ((uint32_t)cbw_ptr->command_block[9]);
    }

    scsi_ptr->thirteen_case_ptr->handle = msc_obj_ptr->controller_handle;
    scsi_ptr->thirteen_case_ptr->host_expected_data_len = cbw_ptr->data_length;
    scsi_ptr->thirteen_case_ptr->host_expected_direction =
        (uint8_t)(cbw_ptr->flag >> USB_CBW_DIRECTION_SHIFT);
    scsi_ptr->thirteen_case_ptr->device_expected_direction = USB_RECV;
    scsi_ptr->thirteen_case_ptr->csw_status_ptr = csw_status_ptr;
    scsi_ptr->thirteen_case_ptr->csw_residue_ptr = csw_residue_ptr;

    scsi_ptr->thirteen_case_ptr->buffer_ptr = NULL;

    if(_usb_device_get_transfer_status(msc_obj_ptr->controller_handle,
        BULK_OUT_ENDPOINT, USB_RECV) != USB_STATUS_IDLE)
    {
        (void)_usb_device_cancel_transfer(msc_obj_ptr->controller_handle,BULK_OUT_ENDPOINT,
            USB_RECV);
    }

    scsi_ptr->thirteen_case_ptr->device_expected_data_len =
        scsi_ptr->length_of_each_lab * num_lba_rx;

    scsi_ptr->thirteen_case_ptr->lba_txrx_select = TRUE;
    scsi_ptr->thirteen_case_ptr->lba_info.starting_lba = lba;
    scsi_ptr->thirteen_case_ptr->lba_info.lba_transfer_num = num_lba_rx;

    error = msc_thirteen_cases_check(msc_obj_ptr,scsi_ptr->thirteen_case_ptr);

    return error;
}

/**************************************************************************//*!
 *
 * @name  msc_test_unit_ready_command
 *
 * @brief It provides a means to check if the device is ready
 *
 * @param handle
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
uint8_t msc_test_unit_ready_command
(
    MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
    PTR_CBW cbw_ptr,
    uint32_t* csw_residue_ptr,
    uint8_t* csw_status_ptr
)
{
    MSC_SCSI_STRUCT_PTR scsi_ptr;
    uint8_t error;

    scsi_ptr = (MSC_SCSI_STRUCT_PTR)msc_obj_ptr->scsi_object_ptr;
    /* initialize sense code values */
    scsi_ptr->request_sense.sense_key = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code_qual = NO_SENSE;

    scsi_ptr->thirteen_case_ptr->handle = msc_obj_ptr->controller_handle;
    scsi_ptr->thirteen_case_ptr->host_expected_data_len = cbw_ptr->data_length;
    scsi_ptr->thirteen_case_ptr->host_expected_direction =
        (uint8_t)(cbw_ptr->flag >> USB_CBW_DIRECTION_SHIFT);

    scsi_ptr->thirteen_case_ptr->device_expected_data_len = 0;
    scsi_ptr->thirteen_case_ptr->device_expected_direction = USB_SEND;
    scsi_ptr->thirteen_case_ptr->csw_status_ptr = csw_status_ptr;
    scsi_ptr->thirteen_case_ptr->csw_residue_ptr = csw_residue_ptr;
    scsi_ptr->thirteen_case_ptr->buffer_ptr = NULL;
    scsi_ptr->thirteen_case_ptr->lba_txrx_select = FALSE;

    error = msc_thirteen_cases_check(msc_obj_ptr,scsi_ptr->thirteen_case_ptr);

    return error;
}

/**************************************************************************//*!
 *
 * @name  msc_verify_command
 *
 * @brief requests that device verifies the data on medium
 *
 * @param handle
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
uint8_t msc_verify_command
(
    MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
    PTR_CBW cbw_ptr,
    uint32_t* csw_residue_ptr,
    uint8_t* csw_status_ptr
)
{
    MSC_SCSI_STRUCT_PTR scsi_ptr;
    uint8_t error;
    /* Our Device has no mechanism to verify the blocks,
       so just returning success status to host*/

    scsi_ptr = (MSC_SCSI_STRUCT_PTR)msc_obj_ptr->scsi_object_ptr;
    /* initialize sense code values */
    scsi_ptr->request_sense.sense_key = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code_qual = NO_SENSE;

    scsi_ptr->thirteen_case_ptr->handle = msc_obj_ptr->controller_handle;
    scsi_ptr->thirteen_case_ptr->host_expected_data_len = cbw_ptr->data_length;
    scsi_ptr->thirteen_case_ptr->host_expected_direction =
        (uint8_t)(cbw_ptr->flag >> USB_CBW_DIRECTION_SHIFT);

    scsi_ptr->thirteen_case_ptr->device_expected_data_len = 0;
    scsi_ptr->thirteen_case_ptr->device_expected_direction = USB_SEND;
    scsi_ptr->thirteen_case_ptr->csw_status_ptr = csw_status_ptr;
    scsi_ptr->thirteen_case_ptr->csw_residue_ptr = csw_residue_ptr;
    scsi_ptr->thirteen_case_ptr->buffer_ptr = NULL;
    scsi_ptr->thirteen_case_ptr->lba_txrx_select = FALSE;

    error = msc_thirteen_cases_check(msc_obj_ptr,scsi_ptr->thirteen_case_ptr);

    return error;
}

/**************************************************************************//*!
 *
 * @name  msc_mode_sense_command
 *
 * @brief command provides a means for a Device to report parameters to Host
 *        Computer.It is a complementary command to the MODE SELECT command.
 *
 * @param handle
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
uint8_t msc_mode_sense_command
(
    MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
    PTR_CBW cbw_ptr,
    uint32_t* csw_residue_ptr,
    uint8_t* csw_status_ptr
)
{
    MSC_SCSI_STRUCT_PTR scsi_ptr;
    uint8_t error = USBERR_TX_FAILED;

    MODE_PARAMETER_HEADER_STRUCT mode_param_header =
    {
      0x0000,/*no following data available because of reason given below*/
      0x00,/* 0x00 indicates current/default medium */
      0x00,/* for write_protect and DPOFUA - no write protection available*/
      0x00,0x00,0x00,0x00 /* reserved bytes are always to be set to zero */
    };

    scsi_ptr = (MSC_SCSI_STRUCT_PTR)msc_obj_ptr->scsi_object_ptr;
    /* initialize sense code values */
    scsi_ptr->request_sense.sense_key = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code_qual = NO_SENSE;

    /* irrespective of DBD(Disable block descriptor) bit in Command Block
       Wrapper we are not returning any block descriptors in the returned mode
       sense data.Presently, we are just returning Mode Parameter Header */

    scsi_ptr->thirteen_case_ptr->handle = msc_obj_ptr->controller_handle;
    scsi_ptr->thirteen_case_ptr->host_expected_data_len = cbw_ptr->data_length;
    scsi_ptr->thirteen_case_ptr->host_expected_direction =
        (uint8_t)(cbw_ptr->flag >> USB_CBW_DIRECTION_SHIFT);
    scsi_ptr->thirteen_case_ptr->device_expected_data_len = sizeof(mode_param_header);
    scsi_ptr->thirteen_case_ptr->device_expected_direction = USB_SEND;
    scsi_ptr->thirteen_case_ptr->csw_status_ptr = csw_status_ptr;
    scsi_ptr->thirteen_case_ptr->csw_residue_ptr = csw_residue_ptr;
    scsi_ptr->thirteen_case_ptr->buffer_ptr = (uint8_t *)&mode_param_header;
    scsi_ptr->thirteen_case_ptr->lba_txrx_select = FALSE;

    error = msc_thirteen_cases_check(msc_obj_ptr,scsi_ptr->thirteen_case_ptr);

    return error;
}

/**************************************************************************//*!
 *
 * @name  msc_mode_select_command
 *
 * @brief command provides a means for a Device to report parameters to Host
 *        Computer.It is a complementary command to the MODE SENSE command.
 *
 * @param handle
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
uint8_t msc_mode_select_command
(
    MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
    PTR_CBW cbw_ptr,
    uint32_t* csw_residue_ptr,
    uint8_t* csw_status_ptr
)
{
    MSC_SCSI_STRUCT_PTR scsi_ptr;
    uint8_t error = USBERR_TX_FAILED;



    MODE_PARAMETER_HEADER_STRUCT mode_param_header =
    {
      0x0000,/*mode data length for mode select command is always zero*/
      0x00,/* 0x00 indicates current/default medium */
      0x00,/* for write_protect and DPOFUA*/
      0x00,0x00,0x00,0x00 /* reserved bytes are always to be set to zero */
    };

    scsi_ptr = (MSC_SCSI_STRUCT_PTR)msc_obj_ptr->scsi_object_ptr;
    /* initialize sense code values */
    scsi_ptr->request_sense.sense_key = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code_qual = NO_SENSE;

    scsi_ptr->thirteen_case_ptr->handle = msc_obj_ptr->controller_handle;
    scsi_ptr->thirteen_case_ptr->host_expected_data_len = cbw_ptr->data_length;
    scsi_ptr->thirteen_case_ptr->host_expected_direction =
        (uint8_t)(cbw_ptr->flag >> USB_CBW_DIRECTION_SHIFT);

    scsi_ptr->thirteen_case_ptr->device_expected_data_len = sizeof(mode_param_header);
    scsi_ptr->thirteen_case_ptr->device_expected_direction = USB_SEND;
    scsi_ptr->thirteen_case_ptr->csw_status_ptr = csw_status_ptr;
    scsi_ptr->thirteen_case_ptr->csw_residue_ptr = csw_residue_ptr;
    scsi_ptr->thirteen_case_ptr->buffer_ptr = (uint8_t *)&mode_param_header;
    scsi_ptr->thirteen_case_ptr->lba_txrx_select = FALSE;

    error = msc_thirteen_cases_check(msc_obj_ptr,scsi_ptr->thirteen_case_ptr);

    if(cbw_ptr->command_block[1] & 0x01)/* checking Save Pages Bit in command*/
    {
        scsi_ptr->request_sense.sense_key = ILLEGAL_REQUEST;
        scsi_ptr->request_sense.add_sense_code = INVALID_FIELD_IN_COMMAND_PKT;
        /*logical unit does not implement save mode pages in our case*/
    }
    return error;
}

/**************************************************************************//*!
 *
 * @name  msc_read_capacity_command
 *
 * @brief command provides a means for the host computer to request information
 *        regarding the capacity of the installed medium of the device.
 *
 * @param handle
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
uint8_t msc_read_capacity_command
(
    MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
    PTR_CBW cbw_ptr,
    uint32_t* csw_residue_ptr,
    uint8_t* csw_status_ptr
)
{
    MSC_SCSI_STRUCT_PTR scsi_ptr;
    READ_CAPACITY_DATA_STRUCT read_capacity;
    uint8_t error = USBERR_TX_FAILED;

    scsi_ptr = (MSC_SCSI_STRUCT_PTR)msc_obj_ptr->scsi_object_ptr;

    read_capacity.last_logical_block_address = LONG_BE_TO_HOST(scsi_ptr->total_logical_add_block -1);
    read_capacity.block_size = LONG_BE_TO_HOST((uint32_t) scsi_ptr->length_of_each_lab);

    /* initialize sense code values */
    scsi_ptr->request_sense.sense_key = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code_qual = NO_SENSE;

    scsi_ptr->thirteen_case_ptr->handle = msc_obj_ptr->controller_handle;
    scsi_ptr->thirteen_case_ptr->host_expected_data_len = cbw_ptr->data_length;
    scsi_ptr->thirteen_case_ptr->host_expected_direction =
        (uint8_t)(cbw_ptr->flag >> USB_CBW_DIRECTION_SHIFT);

    scsi_ptr->thirteen_case_ptr->device_expected_data_len = READ_CAPACITY_DATA_LENGTH;
    scsi_ptr->thirteen_case_ptr->device_expected_direction = USB_SEND;
    scsi_ptr->thirteen_case_ptr->csw_status_ptr = csw_status_ptr;
    scsi_ptr->thirteen_case_ptr->csw_residue_ptr = csw_residue_ptr;
    scsi_ptr->thirteen_case_ptr->buffer_ptr = (uint8_t *)&read_capacity;
    scsi_ptr->thirteen_case_ptr->lba_txrx_select = FALSE;

    error = msc_thirteen_cases_check(msc_obj_ptr,scsi_ptr->thirteen_case_ptr);

    return error;
}

/**************************************************************************//*!
 *
 * @name  msc_read_format_capacity_command
 *
 * @brief allows the host to request a list of the possible capacities that
 *        can be formatted on the currently installed medium
 *
 * @param handle
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
uint8_t msc_read_format_capacity_command
(
    MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
    PTR_CBW cbw_ptr,
    uint32_t* csw_residue_ptr,
    uint8_t* csw_status_ptr
)
{
    MSC_SCSI_STRUCT_PTR scsi_ptr ;
    uint8_t num_formattable_cap_desc ;
    uint16_t allocation_length;
    /* pointer to data to be sent in data phase for this command*/
    uint8_t *response_data_ptr = NULL;
     /* size of data to be sent in data phase for this command*/
    uint32_t response_size;
    /* general variable for counting in loop */
    uint8_t i;
    CURR_MAX_CAPACITY_DESC_STRUCT curr_max_cap_header;
    uint8_t desc_code;
    FORMATTABLE_CAP_DESC formattable_cap_descriptor;
    CAPACITY_LIST_HEADER_STRUCT capacity_list_header= {0x00,0x00,0x00,0x00};
    uint8_t error = USBERR_TX_FAILED;

    scsi_ptr = (MSC_SCSI_STRUCT_PTR)msc_obj_ptr->scsi_object_ptr;
    allocation_length = (uint16_t)((cbw_ptr->command_block[7] << 8) |
        cbw_ptr->command_block[8]);

    num_formattable_cap_desc = (uint8_t)(scsi_ptr->formatted_disk ?
        (scsi_ptr->implementing_disk_drive?0x02:0x03):0x00);
    /*
     * gives the number of Formattable Capacity Descriptor to be sent by device
     * in response to read format capacities command
     */

     /*
      * 0x03 is number of formattable capacity desc for HD while for DD its 2 if
      * formatted drive is there
      */

    formattable_cap_descriptor.num_blocks = scsi_ptr->total_logical_add_block;
    formattable_cap_descriptor.block_len = scsi_ptr->length_of_each_lab;
    //{scsi_ptr->total_logical_add_block,scsi_ptr->length_of_each_lab};

    desc_code = (uint8_t)(scsi_ptr->formatted_disk?FORMATTED_MEDIA:UNFORMATTED_MEDIA);
    /* initialize sense code values */
    scsi_ptr->request_sense.sense_key = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code_qual = NO_SENSE;

    capacity_list_header.capacity_list_len = (uint8_t)(num_formattable_cap_desc * 8);
    curr_max_cap_header.num_blocks = scsi_ptr->total_logical_add_block;
    curr_max_cap_header.desc_code_and_block_len = (desc_code << 24)|
                                                          scsi_ptr->length_of_each_lab;
    response_size = sizeof(capacity_list_header) + sizeof(curr_max_cap_header)+
        sizeof(formattable_cap_descriptor) * num_formattable_cap_desc;

    if(response_size > allocation_length)
    {   /* comparing the length of data available with allocation length value
          sent in CBW which indicates the length of buffer host has reserved
          for data phase of this command */
        response_size = allocation_length;
    }

    /* reserving memory for response data */
    response_data_ptr = (uint8_t *)USB_mem_alloc_uncached(response_size);
    if (NULL == response_data_ptr)
    {
        #if _DEBUG
            printf("msc_read_format_capacity_command: Memalloc failed\n");
        #endif
        return USBERR_ALLOC;
    }
    _mem_zero(response_data_ptr, response_size);

    USB_mem_copy(&capacity_list_header, response_data_ptr,
        sizeof(capacity_list_header));
    USB_mem_copy(&curr_max_cap_header, response_data_ptr +
        sizeof(capacity_list_header),sizeof(curr_max_cap_header));

    if(scsi_ptr->formatted_disk)
    {
        for(i = 0; i < num_formattable_cap_desc; i++)
        {
            USB_mem_copy(&formattable_cap_descriptor, response_data_ptr +
                sizeof(capacity_list_header) + sizeof(curr_max_cap_header)+
                sizeof(formattable_cap_descriptor) * i,
                sizeof(formattable_cap_descriptor));
        }
    }

    scsi_ptr->thirteen_case_ptr->handle = msc_obj_ptr->controller_handle;
    scsi_ptr->thirteen_case_ptr->host_expected_data_len = cbw_ptr->data_length;
    scsi_ptr->thirteen_case_ptr->host_expected_direction =
        (uint8_t)(cbw_ptr->flag >> USB_CBW_DIRECTION_SHIFT);
    scsi_ptr->thirteen_case_ptr->device_expected_data_len = response_size;
    scsi_ptr->thirteen_case_ptr->device_expected_direction = USB_SEND;
    scsi_ptr->thirteen_case_ptr->csw_status_ptr = csw_status_ptr;
    scsi_ptr->thirteen_case_ptr->csw_residue_ptr = csw_residue_ptr;
    scsi_ptr->thirteen_case_ptr->buffer_ptr = response_data_ptr;
    scsi_ptr->thirteen_case_ptr->lba_txrx_select = FALSE;

    error = msc_thirteen_cases_check(msc_obj_ptr,scsi_ptr->thirteen_case_ptr);

    /* free the reserved memory */
    if(response_data_ptr != NULL)
    {
        USB_mem_free(response_data_ptr);
        response_data_ptr = NULL;
    }

    return error;
}

/**************************************************************************//*!
 *
 * @name  msc_format_unit_command
 *
 * @brief host sends the FORMAT UNIT command to physically format a diskette
 *        according to selected options
 *
 * @param handle
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
uint8_t msc_format_unit_command
(
    MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
    PTR_CBW cbw_ptr,
    uint32_t* csw_residue_ptr,
    uint8_t* csw_status_ptr
)
{
    MSC_SCSI_STRUCT_PTR scsi_ptr;
    uint8_t error;
    *csw_residue_ptr = 0;

    scsi_ptr = (MSC_SCSI_STRUCT_PTR)msc_obj_ptr->scsi_object_ptr;

    /* initialize sense code values */
    scsi_ptr->request_sense.sense_key = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code_qual = NO_SENSE;

    scsi_ptr->thirteen_case_ptr->handle = msc_obj_ptr->controller_handle;
    scsi_ptr->thirteen_case_ptr->host_expected_data_len = cbw_ptr->data_length;
    scsi_ptr->thirteen_case_ptr->host_expected_direction =
        (uint8_t)(cbw_ptr->flag >> USB_CBW_DIRECTION_SHIFT);
    scsi_ptr->thirteen_case_ptr->device_expected_data_len = 0;
    scsi_ptr->thirteen_case_ptr->device_expected_direction = USB_SEND;
    scsi_ptr->thirteen_case_ptr->csw_status_ptr = csw_status_ptr;
    scsi_ptr->thirteen_case_ptr->csw_residue_ptr = csw_residue_ptr;
    scsi_ptr->thirteen_case_ptr->buffer_ptr = NULL;
    scsi_ptr->thirteen_case_ptr->lba_txrx_select = FALSE;

    error = msc_thirteen_cases_check(msc_obj_ptr,scsi_ptr->thirteen_case_ptr);

    if(*csw_status_ptr != PHASE_ERROR)
    {
        /* FmtData = 1, CmpList = 0, Defect List Format = 7 */
        if((cbw_ptr->command_block[1] & 0x1F) == 0x17)
        {
            *csw_status_ptr = COMMAND_PASSED;
        }
        else
        {
            *csw_status_ptr = COMMAND_FAILED;
            scsi_ptr->request_sense.sense_key = ILLEGAL_REQUEST;
            scsi_ptr->request_sense.add_sense_code = INVALID_FIELD_IN_COMMAND_PKT;
        }
    }
    return error;
}

/**************************************************************************//*!
 *
 * @name  msc_prevent_allow_medium_removal
 *
 * @brief command tells the UFI device to enable or disable the removal of the
 *        medium in the logical unit.
 *
 * @param handle
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
uint8_t msc_prevent_allow_medium_removal
(
    MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
    PTR_CBW cbw_ptr,
    uint32_t* csw_residue_ptr,
    uint8_t* csw_status_ptr
)
{
    MSC_SCSI_STRUCT_PTR scsi_ptr;
    uint8_t error;
    uint8_t prevent_removal = 0;
    /* masking to obtain value of last bit */
    prevent_removal = (uint8_t)(cbw_ptr->command_block[4] &
        PREVENT_ALLOW_REMOVAL_MASK);

    scsi_ptr = (MSC_SCSI_STRUCT_PTR)msc_obj_ptr->scsi_object_ptr;
    /* initialize sense code values */
    scsi_ptr->request_sense.sense_key = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code_qual = NO_SENSE;

    scsi_ptr->thirteen_case_ptr->handle = msc_obj_ptr->controller_handle;
    scsi_ptr->thirteen_case_ptr->host_expected_data_len = cbw_ptr->data_length;
    scsi_ptr->thirteen_case_ptr->host_expected_direction =
        (uint8_t)(cbw_ptr->flag >> USB_CBW_DIRECTION_SHIFT);
    scsi_ptr->thirteen_case_ptr->device_expected_data_len = 0;
    scsi_ptr->thirteen_case_ptr->device_expected_direction = USB_SEND;
    scsi_ptr->thirteen_case_ptr->csw_status_ptr = csw_status_ptr;
    scsi_ptr->thirteen_case_ptr->csw_residue_ptr = csw_residue_ptr;
    scsi_ptr->thirteen_case_ptr->buffer_ptr = NULL;
    scsi_ptr->thirteen_case_ptr->lba_txrx_select = FALSE;

    error = msc_thirteen_cases_check(msc_obj_ptr,scsi_ptr->thirteen_case_ptr);

    if(*csw_status_ptr != PHASE_ERROR)
    {
        if((!SUPPORT_DISK_LOCKING_MECHANISM)&&(prevent_removal))
        {/*there is no support for disk locking and removal of medium is disabled*/
            scsi_ptr->request_sense.sense_key = ILLEGAL_REQUEST;
            scsi_ptr->request_sense.add_sense_code = INVALID_FIELD_IN_COMMAND_PKT;
            *csw_status_ptr = COMMAND_FAILED;
        }
    }

    if(scsi_ptr->scsi_callback.callback != NULL)
    {
        scsi_ptr->scsi_callback.callback(USB_MSC_DEVICE_REMOVAL_REQUEST,
            (void*)&prevent_removal, scsi_ptr->scsi_callback.arg);
    }

    return error;
}

/**************************************************************************//*!
 *
 * @name  msc_send_diagnostic_command
 *
 * @brief requests the device to perform self test
 *
 * @param msc_obj_ptr
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
uint8_t msc_send_diagnostic_command
(
    MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
    PTR_CBW cbw_ptr,
    uint32_t* csw_residue_ptr,
    uint8_t* csw_status_ptr
)
{
    MSC_SCSI_STRUCT_PTR scsi_ptr;
    /* No Self Test Procedure available */
    uint8_t error;

    scsi_ptr = (MSC_SCSI_STRUCT_PTR)msc_obj_ptr->scsi_object_ptr;

    /* initialize sense code values */
    scsi_ptr->request_sense.sense_key = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code_qual = NO_SENSE;

    scsi_ptr->thirteen_case_ptr->handle = msc_obj_ptr->controller_handle;
    scsi_ptr->thirteen_case_ptr->host_expected_data_len = cbw_ptr->data_length;
    scsi_ptr->thirteen_case_ptr->host_expected_direction =
        (uint8_t)(cbw_ptr->flag >> USB_CBW_DIRECTION_SHIFT);
    scsi_ptr->thirteen_case_ptr->device_expected_data_len = 0;
    scsi_ptr->thirteen_case_ptr->device_expected_direction = USB_SEND;
    scsi_ptr->thirteen_case_ptr->csw_status_ptr = csw_status_ptr;
    scsi_ptr->thirteen_case_ptr->csw_residue_ptr = csw_residue_ptr;
    scsi_ptr->thirteen_case_ptr->buffer_ptr = NULL;
    scsi_ptr->thirteen_case_ptr->lba_txrx_select = FALSE;

    error = msc_thirteen_cases_check(msc_obj_ptr,scsi_ptr->thirteen_case_ptr);

    return error;
}

/**************************************************************************//*!
 *
 * @name  msc_start_stop_unit_command
 *
 * @brief command instructs device to enable or disable media access operations
 *
 * @param msc_obj_ptr:
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
uint8_t msc_start_stop_unit_command
(
    MSC_DEVICE_STRUCT_PTR msc_obj_ptr,
    PTR_CBW cbw_ptr,
    uint32_t* csw_residue_ptr,
    uint8_t* csw_status_ptr
)
{
    MSC_SCSI_STRUCT_PTR scsi_ptr;
    uint8_t error;
    uint8_t load_eject_start;

    scsi_ptr = (MSC_SCSI_STRUCT_PTR)msc_obj_ptr->scsi_object_ptr;

    /* masking to obtain value of last bit */
    load_eject_start = (uint8_t)(cbw_ptr->command_block[4] &
        LOAD_EJECT_START_MASK);
    /* initialize sense code values */
    scsi_ptr->request_sense.sense_key = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code = NO_SENSE;
    scsi_ptr->request_sense.add_sense_code_qual = NO_SENSE;

    scsi_ptr->thirteen_case_ptr->handle = msc_obj_ptr->controller_handle;
    scsi_ptr->thirteen_case_ptr->host_expected_data_len = cbw_ptr->data_length;
    scsi_ptr->thirteen_case_ptr->host_expected_direction =
        (uint8_t)(cbw_ptr->flag >> USB_CBW_DIRECTION_SHIFT);
    scsi_ptr->thirteen_case_ptr->device_expected_data_len = 0;
    scsi_ptr->thirteen_case_ptr->device_expected_direction = USB_SEND;
    scsi_ptr->thirteen_case_ptr->csw_status_ptr = csw_status_ptr;
    scsi_ptr->thirteen_case_ptr->csw_residue_ptr = csw_residue_ptr;
    scsi_ptr->thirteen_case_ptr->buffer_ptr = NULL;
    scsi_ptr->thirteen_case_ptr->lba_txrx_select = FALSE;

    error = msc_thirteen_cases_check(msc_obj_ptr,scsi_ptr->thirteen_case_ptr);

    if(*csw_status_ptr != PHASE_ERROR)
    {
        if(scsi_ptr->scsi_callback.callback != NULL)
        {
            scsi_ptr->scsi_callback.callback(USB_MSC_START_STOP_EJECT_MEDIA,
                (void*)&load_eject_start, scsi_ptr->scsi_callback.arg);
        }
    }
    return error;
}

/**************************************************************************//*!
 *
 * @name  msc_unsupported_command
 *
 * @brief Responds appropriately to unsupported commands
 *
 * @param msc_obj_ptr
 * @param cbw_ptr        : pointer to Command Block Wrapper sent by host
 * @param csw_residue_ptr: pointer to dCSWDataResidue of Command Status Wrapper
 * @param csw_status_ptr : pointer to bCSWStatus  of Command Status Wrapper
 *
 * @return error
 *
 *****************************************************************************/
uint8_t msc_unsupported_command
(
    MSC_DEVICE_STRUCT_PTR  msc_obj_ptr,
    PTR_CBW cbw_ptr,
    uint32_t* csw_residue_ptr,
    uint8_t* csw_status_ptr
)
{
    MSC_SCSI_STRUCT_PTR scsi_ptr;

    UNUSED_ARGUMENT(cbw_ptr)

    scsi_ptr = (MSC_SCSI_STRUCT_PTR)msc_obj_ptr->scsi_object_ptr;

    *csw_residue_ptr = 0;
    *csw_status_ptr = COMMAND_FAILED;

    scsi_ptr->request_sense.sense_key = ILLEGAL_REQUEST;
    scsi_ptr->request_sense.add_sense_code = INVALID_COMMAND_OPCODE;
    scsi_ptr->request_sense.add_sense_code_qual = NO_SENSE;

    return USB_OK;
}
#endif /*MSD_CONFIG*/
/* EOF */
