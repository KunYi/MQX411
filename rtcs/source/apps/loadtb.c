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
*   This file contains the interface functions to RTCS_load()
*   for binary files using TFTP.
*
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_UDP

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_load_TFTP_BIN
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint32_t RTCS_load_TFTP_BIN
   (
      _ip_address          server,
         /* [IN] address of the TFTP server */
      char             *filename,
         /* [IN] file to retrieve */
      unsigned char            *dl_addr
         /* [IN] download start address */
   )
{ /* Body */
   TFTP_DATA_STRUCT     tftp_data;
   BINFILE_DATA_STRUCT  binfile_data;

   tftp_data.SERVER   = server;
   tftp_data.FILENAME = filename;
   tftp_data.FILEMODE = "octet";

   binfile_data.DOWNLOAD_ADDR = dl_addr;

   return RTCS_load(FT_TFTP, &tftp_data, FF_BIN, &binfile_data, 0);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_exec_TFTP_BIN
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load and execute a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint32_t RTCS_exec_TFTP_BIN
   (
      _ip_address          server,
         /* [IN] address of the TFTP server */
      char             *filename,
         /* [IN] file to retrieve */
      unsigned char            *dl_addr,
         /* [IN] download start address */
      unsigned char            *exec_addr
         /* [IN] execution start address */
   )
{ /* Body */
   TFTP_DATA_STRUCT     tftp_data;
   BINFILE_DATA_STRUCT  binfile_data;

   tftp_data.SERVER   = server;
   tftp_data.FILENAME = filename;
   tftp_data.FILEMODE = "octet";

   binfile_data.DOWNLOAD_ADDR = dl_addr;
   binfile_data.EXEC_ADDR     = exec_addr;

   return RTCS_load(FT_TFTP, &tftp_data, FF_BIN, &binfile_data, RTCSLOADOPT_EXEC);

} /* Endbody */

#endif
/* EOF */
