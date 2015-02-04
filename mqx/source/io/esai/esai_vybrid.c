/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
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
*   This include file is used to provide information needed by
*   applications using the ESAI FIFO I/O device driver functions.
*
*
*END************************************************************************/
#include "mqx_inc.h"

uint32_t _esai_get_irq_num(uint32_t dev_num)
{
    return INT_ESAI_BIFIFO;
}

uint32_t _esai_get_base_address(uint32_t dev_num)
{
    return (uint32_t)ESAI_BASE_PTR;
}


/* EOF */
