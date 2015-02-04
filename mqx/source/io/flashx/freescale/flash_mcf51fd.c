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
*   The file contains device specific flash functions.
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "flashx.h"
#include "flashxprv.h"
#include "flash_ftfl.h"
#include "flash_ftfl_prv.h"
#include "flash_mcf51fd.h"

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_ftfl_address
* Returned Value   : Address upon success
* Comments         :
*    This function returns the base register address of the FTFL.
*
*END*----------------------------------------------------------------------*/
void *_bsp_get_ftfl_address
(
    void
)
{
  
    return (void *)(FTFL_BASE_PTR);   

}
