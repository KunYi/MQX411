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
*   This file contains the interface to the RTCS
*   message queue object.
*
*
*END************************************************************************/

#include <rtcsrtos.h>
#include <rtcs.h>
#if 0
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_msgqueue_create
* Returned Values : error code
* Comments        :
*     Initialize a message queue.
*
*END*-----------------------------------------------------------------*/

uint32_t RTCS_msgqueue_create
   (
      _rtcs_msgqueue  *msgq
   )
{ /* Body */

   _lwsem_create_hidden(&msgq->EMPTY, RTCSMQ_SIZE);
   _lwsem_create_hidden(&msgq->FULL, 0);
   msgq->HEAD = 0;
   msgq->TAIL = 0;
   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_msgqueue_send
* Returned Values : void
* Comments        :
*     Send a message, block if the message queue is full.
*
*END*-----------------------------------------------------------------*/

void RTCS_msgqueue_send
   (
      _rtcs_msgqueue  *msgq,
      void                *msg
   )
{ /* Body */

   /* Wait for an empty slot in the queue */
   _lwsem_wait(&msgq->EMPTY);
   _int_disable();

   /* Dequeue the message */
   msgq->QUEUE[msgq->TAIL++] = msg;
   if (msgq->TAIL == RTCSMQ_SIZE) {
      msgq->TAIL = 0;
   } /* Endif */

   /* Unblock the receiver */
   _int_enable();
   _lwsem_post(&msgq->FULL);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_msgqueue_trysend
* Returned Values : RTCS_OK if successful, or RTCS_ERROR if the message
*                   queue is full.
* Comments        :
*     Send a message, fail if the message queue is full.
*
*END*-----------------------------------------------------------------*/

int32_t RTCS_msgqueue_trysend
   (
      _rtcs_msgqueue  *msgq,
      void                *msg
   )
{ /* Body */

   /* Check for an empty slot in the queue */
   _int_disable();
   if (msgq->EMPTY.VALUE <= 0) {
      _int_enable();
      return RTCS_ERROR;
   } else {
      --msgq->EMPTY.VALUE;
   } /* Endif */

   /* Dequeue the message */
   msgq->QUEUE[msgq->TAIL++] = msg;
   if (msgq->TAIL == RTCSMQ_SIZE) {
      msgq->TAIL = 0;
   } /* Endif */

   /* Unblock the receiver */
   _int_enable();
   _lwsem_post(&msgq->FULL);

   return RTCS_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_msgqueue_receive
* Returned Values : message
* Comments        :
*     Receive a message, block if the message queue is empty.
*
*END*-----------------------------------------------------------------*/

void *RTCS_msgqueue_receive
   (
      _rtcs_msgqueue  *msgq,
      uint32_t              timeout
   )
{ /* Body */
   void    *msg;
   TIME_STRUCT     time;
   MQX_TICK_STRUCT ticks;

   if (!timeout) {
      _lwsem_wait(&msgq->FULL);
   } else {
      time.SECONDS      = timeout/1000;
      time.MILLISECONDS = timeout%1000;
      if (!_time_to_ticks(&time, &ticks)) {
         return NULL;
      } /* Endif */
      if (_lwsem_wait_for(&msgq->FULL, &ticks) != MQX_OK) {
         /* If we have a timeout, return */
         return NULL;
      } /* Endif */
   } /* Endif */

   _int_disable();
   msg = msgq->QUEUE[msgq->HEAD++];
   if (msgq->HEAD == RTCSMQ_SIZE) {
      msgq->HEAD = 0;
   } /* Endif */

   _int_enable();
   _lwsem_post(&msgq->EMPTY);

   return msg;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : RTCS_msgqueue_close
* Returned Values : void
* Comments        :
*     Uninitialize a message queue.
*
*END*-----------------------------------------------------------------*/

void RTCS_msgqueue_close
   (
      _rtcs_msgqueue  *msgq
   )
{ /* Body */

   _lwsem_destroy(&msgq->FULL);
   _lwsem_destroy(&msgq->EMPTY);

} /* Endbody */

#endif
/* EOF */
