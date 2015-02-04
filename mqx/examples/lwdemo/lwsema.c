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

/*   Task Code -  LWSemA     */


/*TASK---------------------------------------------------------------
*   
* Task Name   :  LWSemA
* Comments    : 
* 
*END*--------------------------------------------------------------*/

void LWSemA
   (
      uint32_t   parameter
   )
{
   _mqx_uint sem_result;

   /* 
   ** LOOP - 
   */
   while ( TRUE ) {
      /* wait for lw semaphore until it is available */      
      sem_result = _lwsem_wait_ticks(&lwsem, NO_TIMEOUT);
      if (sem_result != MQX_OK) { 
         /* waiting on semaphore sem.Sem1 failed */
      }  
      /* semaphore obtained, perform work */
      _time_delay_ticks(1);
      /* semaphore protected work done, release semaphore */
      sem_result = _lwsem_post(&lwsem);
   } /* endwhile */ 
} /*end of task*/

/* End of File */
