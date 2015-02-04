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
*   This file contains the default hosts file structure.
*
*
*END************************************************************************/

#include <rtcs.h>


const HOST_ENTRY_STRUCT RTCS_Hosts_list[] =
{
 /* IP Address   Host Name     Aliases          */
  { 0x7F000001, "localhost",    0       },
  { 0xC0589C0A, "www.freescale.com", {"www"}  },
  { 0xFFFFFFFF, "broadcast",    0       },
  { 0, 0, 0 },
};


/* EOF */
