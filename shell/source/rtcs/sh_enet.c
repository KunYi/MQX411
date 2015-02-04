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
*   Example using RTCS Library.
*
*
*END************************************************************************/

#include <ctype.h>
#include <string.h>
#include <mqx.h>
#include "shell.h"
#if SHELLCFG_USES_RTCS

#include <rtcs.h>
#include <enet.h>
#include "sh_rtcs.h" 
#include "sh_enet.h" 

 

bool Shell_parse_enet_address( char *arg, _enet_address enet_address)
{
   int i,j=0;
   
   if (strlen(arg) != 17)  {
      return FALSE;
   }
   
   for (i=0;i<16;i+=3)  {
      if ( isxdigit(arg[i]) && isxdigit(arg[i+1]) && ((arg[i+2]==':') || (arg[i+2]=='\0')) )  {
         enet_address[j++] = hexnum(arg[i]) * 16 + hexnum(arg[i+1]);
      } else  {
         return FALSE;
      }
   } 
   return TRUE;   
}

#endif /* SHELLCFG_USES_RTCS */
/* EOF*/
