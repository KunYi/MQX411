/*HEADER**********************************************************************
*
* Copyright 2012 Freescale Semiconductor, Inc.
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   Simple time operations
*
*END************************************************************************/

#include "wl_common.h"
#include "simple_timer.h"
#include "timer.h"

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : nandwl_hw_get_microsecond
* Returned Value   : hw microsecond in uint64_t 
* Comments         : 
*   This function retrieves the number of microseconds since
*   the processor started.  (without any time offset information)
*
*END*----------------------------------------------------------------------*/
uint64_t wl_hw_get_microsecond()
{ /* Body */
    MQX_TICK_STRUCT    tick;
    TIME_STRUCT_PTR time_ptr;
    
    _time_get_elapsed_ticks(&tick);
    return PSP_TICKS_TO_MICROSECONDS(&tick, &time_ptr );
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : wl_msecs_to_ticks
* Returned Value   : ticks in uint64_t 
* Comments         : 
*   Convert from given msecs to OS ticks 
*
*END*----------------------------------------------------------------------*/
uint64_t wl_msecs_to_ticks(uint32_t msecs)
{ /* Body */
    PSP_TICK_STRUCT tick;
    _psp_usecs_to_ticks(msecs, &tick);
    return tick.TICKS[0];
} /* Endbody */

/* EOF */
