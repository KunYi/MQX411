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
*   This file contains the TFTP access control function.
*
*
*END************************************************************************/

#include <rtcs.h>

#if MQX_USE_IO_OLD

#include <fio.h>
#include "tftp.h"

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TFTPSRV_service_request
* Returned Value  : TRUE if the requested transaction is to be
*                   allowed, FALSE if it is to be rejected
* Comments        : The default behaviour for this function is to allow
*                   read requests on all devices and reject all write
*                   requests.
*
*END*-----------------------------------------------------------------*/

bool TFTPSRV_access
   (
      char    *string_ptr,
         /* [IN] The requested device */
      uint16_t     request_type
         /* [IN] The type of access requested */
   )
{ /* Body */
   return (TRUE);
} /* Endbody */

#endif // MQX_USE_IO_OLD
