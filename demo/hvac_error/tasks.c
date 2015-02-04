/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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
*   
*
*
*END************************************************************************/


#include "hvac.h"


/*
** MQX initialization information
*/

const TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
   /* Task Index,   Function,         Stack,  Priority, Name,       Attributes,             Param,  Time Slice */
    { HVAC_TASK,    HVAC_Task,         1400,    9,      "HVAC",     MQX_AUTO_START_TASK,    0,      0           },
#if DEMOCFG_ENABLE_SWITCH_TASK
    { SWITCH_TASK,  Switch_Task,        800,   10,      "Switch",   MQX_AUTO_START_TASK,    0,      0           },
#endif
#if DEMOCFG_ENABLE_SERIAL_SHELL
    { SHELL_TASK,   Shell_Task,        2500,   12,      "Shell",    MQX_AUTO_START_TASK,    0,      0           },
#endif
#if DEMOCFG_ENABLE_AUTO_LOGGING
    { LOGGING_TASK, Logging_task,      2500,   11,      "Logging",                    0,    0,      0           },
#endif
#if DEMOCFG_ENABLE_USB_FILESYSTEM
    { USB_TASK,     USB_task,         2200L,   8L,      "USB",      MQX_AUTO_START_TASK,    0,      0           },
#endif
    { ALIVE_TASK,   HeartBeat_Task,    1500,   10,      "HeartBeat",                  0,    0,      0           },
    {0}
};

 
/* EOF */
