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

/*   Task Code -  MutexA     */


/*TASK---------------------------------------------------------------
*   
* Task Name   :  MutexA
* Comments    : 
* 
*END*--------------------------------------------------------------*/

void MutexA
   (
      uint32_t   parameter
   )
{
   MUTEX_ATTR_STRUCT      mutex_init;

   /* initialize a mutex Mutex1 */
   if (_mutatr_init(&mutex_init) == MQX_EOK) {
      _mutatr_set_wait_protocol(&mutex_init,MUTEX_QUEUEING);
      _mutatr_set_sched_protocol(&mutex_init,MUTEX_NO_PRIO_INHERIT);
      _mutex_init(&Mutex1,&mutex_init);
   }

   /* 
   ** LOOP - 
   */
   while ( TRUE ) {
      if (_mutex_lock(&Mutex1) != MQX_EOK) { 
         /* an error occurred */
      }

      /* access shared resource */

      _time_delay_ticks(1);
      _mutex_unlock(&Mutex1);
   } /* endwhile */ 
} /*end of task*/

/* End of File */
