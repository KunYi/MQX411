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
*   for S-Record files using TFTP.
*
*
*END************************************************************************/

#include <rtcs.h>

#if RTCSCFG_ENABLE_UDP

/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_load_TFTP_SREC
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load and execute a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint32_t RTCS_load_TFTP_SREC
   (
      _ip_address          server,
         /* [IN] address of the TFTP server */
      char             *filename
         /* [IN] file to retrieve */
   )
{ /* Body */
   TFTP_DATA_STRUCT  tftp_data;

   tftp_data.SERVER   = server;
   tftp_data.FILENAME = filename;
   tftp_data.FILEMODE = "netascii";

#if (PSP_MEMORY_ADDRESSING_CAPABILITY == 32)
   return RTCS_load(FT_TFTP, &tftp_data, FF_SREC32, NULL, 0);
#else
   return RTCS_load(FT_TFTP, &tftp_data, FF_SREC,   NULL, 0);
#endif

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_exec_TFTP_SREC
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load and execute a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint32_t RTCS_exec_TFTP_SREC
   (
      _ip_address          server,
         /* [IN] address of the TFTP server */
      char             *filename
         /* [IN] file to retrieve */
   )
{ /* Body */
   TFTP_DATA_STRUCT  tftp_data;

   tftp_data.SERVER   = server;
   tftp_data.FILENAME = filename;
   tftp_data.FILEMODE = "netascii";

#if (PSP_MEMORY_ADDRESSING_CAPABILITY == 32)
   return RTCS_load(FT_TFTP, &tftp_data, FF_SREC32, NULL, RTCSLOADOPT_EXEC);
#else
   return RTCS_load(FT_TFTP, &tftp_data, FF_SREC,   NULL, RTCSLOADOPT_EXEC);
#endif

} /* Endbody */

#endif
/* EOF */
