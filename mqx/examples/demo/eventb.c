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
*   
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <message.h>
#include <errno.h>
#include <mutex.h>
#include <sem.h>
#include <event.h>
#include <log.h>
#include "demo.h"

/*   Task Code -  EventB     */


/*TASK---------------------------------------------------------------
*   
* Task Name   :  EventB
* Comments    : 
* 
*END*--------------------------------------------------------------*/

void EventB
   (
      uint32_t   parameter
   )
{
   _mqx_uint  event_result;
   void      *Event1_handle;

   /* open connection to event event.Event1 */
   event_result = _event_open("event.Event1",&Event1_handle);
   if (event_result != MQX_OK) { 
      /* could not open event.Event1  */
   } /* endif */

   /* 
   ** LOOP - 
   */
   while ( TRUE ) {
      /* wait on event event.Event1 */
      event_result = _event_wait_all_ticks(Event1_handle, 1, NO_TIMEOUT);
      if (event_result != MQX_OK) { 
         /* waiting on event event.Event1 failed */
      } 
         /* clear the event bits after processing event */
      event_result = _event_clear(Event1_handle, 1);
   } /* endwhile */ 
} /*end of task*/

/* End of File */
