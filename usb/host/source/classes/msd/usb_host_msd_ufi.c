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
*   This file creates the mass storage class UFI command headers
*
*
*END************************************************************************/

#include "usb_host_msd_bo.h"
#include "usb_host_msd_ufi.h"

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_mass_ufi_generic
* Returned Value : None
* Comments       :
*     This function initializes the mass storage class
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_mass_ufi_generic
   (
      /* [IN] command object allocated by application*/
      COMMAND_OBJECT_PTR         cmd_ptr,
      uint8_t                     opcode,
      uint8_t                     lun,
      uint32_t                    lbaddr,
      uint32_t                    blen,

      uint8_t                     cbwflags,

      unsigned char                  *buf,
      uint32_t                    buf_len
   )
{ /* Body */
   UFI_CBWCB_EXTENDED_STRUCT_PTR ufi_ptr = NULL;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_mass_ufi_generic");
   #endif

   ufi_ptr = (void *) &(cmd_ptr->CBW_PTR->CMD_BLOCK);

   /* Construct UFI command buffer */
   ufi_ptr->BUFIOPCODE = opcode;
   ufi_ptr->BUFILUN = lun;
   HOST_TO_BE_UNALIGNED_LONG(lbaddr, ufi_ptr->BUFILOGICALBLOCKADDRESS);
   HOST_TO_BE_UNALIGNED_LONG(blen, ufi_ptr->BLENGTH);

   /* Construct CBW fields (sig and tag will be filled up by class driver)*/
   //*(uint32_t *)cmd_ptr->CBW_PTR->DCBWDATATRANSFERLENGTH = HOST_TO_LE_LONG(buf_len);
   HOST_TO_LE_UNALIGNED_LONG(buf_len, cmd_ptr->CBW_PTR->DCBWDATATRANSFERLENGTH);
   cmd_ptr->CBW_PTR->BMCBWFLAGS = cbwflags;
   TRANSFER_LOW_NIBBLE(cmd_ptr->LUN, cmd_ptr->CBW_PTR->BCBWLUN);
   TRANSFER_LOW_NIBBLE(sizeof(UFI_CBWCB_STRUCT),
      cmd_ptr->CBW_PTR->BCBWCBLENGTH);

   /* Construct Command object */
   cmd_ptr->DATA_BUFFER = buf;
   cmd_ptr->BUFFER_LEN = buf_len;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_mass_ufi_generic, SUCCESSFUL");
   #endif

   /* Pass this request to class driver */
   return usb_class_mass_storage_device_command(cmd_ptr->CALL_PTR, cmd_ptr);
} /* Endbody */

bool usb_mass_ufi_cancel
   (
      COMMAND_OBJECT_PTR         cmd_ptr   )
{ /* Body */
   return usb_class_mass_storage_device_command(cmd_ptr->CALL_PTR, cmd_ptr);
} /* Endbody */
/* EOF */
