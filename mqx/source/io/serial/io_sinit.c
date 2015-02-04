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
*   This file contains the function that initializes the kernel
*   default serial I/O.
*
*
*END************************************************************************/

#include "mqx_inc.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"
#include "serial.h"

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_serial_default_init
* Returned Value   : none
* Comments         :
*   Initializes the kernel default serial I/O
*
*END*----------------------------------------------------------------------*/

void _io_serial_default_init
   (
      void
   )
{ /* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data;

   _GET_KERNEL_DATA(kernel_data);

   if (kernel_data->INIT.IO_CHANNEL) {
      kernel_data->PROCESSOR_STDIN = _io_fopen(
         (char *)kernel_data->INIT.IO_CHANNEL,
         (char *)kernel_data->INIT.IO_OPEN_MODE);
      kernel_data->PROCESSOR_STDOUT = kernel_data->PROCESSOR_STDIN;
      kernel_data->PROCESSOR_STDERR = kernel_data->PROCESSOR_STDIN;
   } /* Endif */

} /* Endbody */

/* EOF */
