/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains source for the Lightweight MQX demo test.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
//#include <message.h>
#include <errno.h>
#include <lwevent.h>
#include "lwdemo.h"

/*   Task Code -  LWEventB     */


/*TASK---------------------------------------------------------------
*   
* Task Name   :  LWEventB
* Comments    : 
* 
*END*--------------------------------------------------------------*/

void LWEventB
   (
      uint32_t   parameter
   )
{
   _mqx_uint  event_result;

   /* 
   ** LOOP - 
   */
   while ( TRUE ) {
      /* wait on event */
      event_result = _lwevent_wait_ticks(&lwevent,1,TRUE,0);
      if (event_result != MQX_OK) { 
         /* waiting on event event.Event1 failed */
      }       
         /* clear the event bits after processing event */
      event_result = _lwevent_clear(&lwevent, 1);
   } /* endwhile */ 
} /*end of task*/

/* End of File */
