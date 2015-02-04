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

/*   Task Code -  SemA     */


/*TASK---------------------------------------------------------------
*   
* Task Name   :  SemA
* Comments    : 
* 
*END*--------------------------------------------------------------*/

void SemA
   (
      uint32_t   parameter
   )
{
   _mqx_uint  sem_result;
   void      *Sem1_handle;

   /* create semaphore - sem.Sem1 */
   sem_result = _sem_create("sem.Sem1", 1, 0);
   if (sem_result != MQX_OK) { 
      /* semaphore sem.Sem1 not be created */
   } /* endif */
   /* open connection to semaphore sem.Sem1 */
   sem_result = _sem_open("sem.Sem1",&Sem1_handle);
   if (sem_result != MQX_OK) { 
      /* could not open sem.Sem1  */
   } /* endif */
   
   /* 
   ** LOOP - 
   */
   while ( TRUE ) {
      /* wait for semaphore sem.Sem1 */
      sem_result = _sem_wait_ticks(Sem1_handle, NO_TIMEOUT);
      if (sem_result != MQX_OK) { 
         /* waiting on semaphore sem.Sem1 failed */
      }  
      /* semaphore obtained, perform work */
      _time_delay_ticks(1);
      /* semaphore protected work done, release semaphore */
      sem_result = _sem_post(Sem1_handle);
   } /* endwhile */ 
} /*end of task*/

/* End of File */
