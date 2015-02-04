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

/*   Defines and data structures for application       */

#define MUTEXB            5
#define MUTEXA            6
#define SEMB              7
#define SEMA              8
#define EVENTB            9
#define EVENTA           10
#define BTIMESLICETASK   11
#define ATIMESLICETASK   12
#define SENDER           13
#define RESPONDER        14
#define MAIN_TASK        15

#define NO_TIMEOUT        0
#define SIZE_UNLIMITED    0


/* Defines for Semaphore Component */
#define SEM_INITIAL_NUMBER    10
#define SEM_GROWTH            10
#define SEM_MAXIMUM           20

/* Defines for Event Component */
#define EVENT_INITIAL_NUMBER  10
#define EVENT_GROWTH          10
#define EVENT_MAXIMUM         20

/*
**   Externs for global data
*/
/*  Message Queue Ids */
extern   _queue_id     Sender_Queue_qid;

/*  Message Queue Ids */
extern   _queue_id     Responder_Queue_qid;

/*  Message Queue Ids */
extern   _queue_id     Main_Queue_qid;

/*  Message Pool Ids */
extern   _pool_id      MsgPool_pool_id;

/*  Mutex Definitions */
extern   MUTEX_STRUCT  Mutex1;

/*
** Externs for Tasks and ISRs
*/
extern void MutexB(uint32_t);
extern void MutexA(uint32_t);
extern void SemB(uint32_t);
extern void SemA(uint32_t);
extern void EventB(uint32_t);
extern void EventA(uint32_t);
extern void BTimeSliceTask(uint32_t);
extern void ATimeSliceTask(uint32_t);
extern void Sender(uint32_t);
extern void Responder(uint32_t);
extern void main_task(uint32_t);


/* EOF */
