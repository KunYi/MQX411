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
*   This file contains the binary file decoder for the
*   Exec function of the RTCS Communication Library.
*
*
*END************************************************************************/

#include <rtcs.h>


/* The binary file decoder information to pass to RTCS_load() */
uint32_t BIN_start  (void *ff_data);
uint32_t BIN_decode (uint32_t size, unsigned char *data);
uint32_t BIN_eod    (void);
void    BIN_exec   (void);

static FF_CALL_STRUCT ff_bin = {
   BIN_start,
   BIN_decode,
   BIN_eod,
   BIN_exec
};

const FF_CALL_STRUCT_PTR FF_BIN = &ff_bin;


/* Binary file decoder state information */
static unsigned char *download_address;
static unsigned char *execute_address;


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : BIN_start
* Returned Value : error code
* Comments       : Routine to begin loading a binary file.
*
*END*-----------------------------------------------------------------*/

uint32_t BIN_start
   (
      void    *ff_data
   )
{ /* Body */

   download_address = ((BINFILE_DATA_STRUCT_PTR)ff_data)->DOWNLOAD_ADDR;
   execute_address  = ((BINFILE_DATA_STRUCT_PTR)ff_data)->EXEC_ADDR;
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : BIN_decode
* Returned Value : status
* Comments       : Copy the transferred bytes to the target.
*
*END*-----------------------------------------------------------------*/

uint32_t BIN_decode
   (
      uint32_t   size,
         /* [IN] number of bytes to decode */
      unsigned char *data
         /* [IN] bytes to decode */
   )
{ /* Body */

   _mem_copy(data, download_address, size);
   download_address += size;
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : BIN_eod
* Returned Value : error code
* Comments       : Routine to end decoding.
*
*END*-----------------------------------------------------------------*/

uint32_t BIN_eod
   (
      void
   )
{ /* Body */

   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : BIN_exec
* Returned Value : none
* Comments       : Routine to execute a decoded binary file.
*
*END*-----------------------------------------------------------------*/

void BIN_exec
   (
      void
   )
{ /* Body */

   (*(void(_CODE_PTR_)(void))(uint32_t)execute_address)();

} /* Endbody */


/* EOF */
