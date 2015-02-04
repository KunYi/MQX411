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

/* Defines and data structures for application */

#define LWSEMB            7
#define LWSEMA            8
#define LWEVENTB          9
#define LWEVENTA          10
#define SENDER           13
#define RESPONDER        14
#define MAIN_TASK        15

#define NO_TIMEOUT        0
#define SIZE_UNLIMITED    0

/* Definitions for LW Message Queue Component */
#define NUM_MESSAGES          4
#define MSG_SIZE              1

/*
**   Externs for global data
*/
/* LW Event Definitions */
extern   LWEVENT_STRUCT lwevent;

/* LW Sem Definitions */
extern   LWSEM_STRUCT lwsem;

/* LW Message Queue Definitions */
extern   uint32_t main_queue[];
extern   uint32_t sender_queue[];
extern   uint32_t responder_queue[];

/*
** Externs for Tasks and ISRs
*/
extern void LWSemB(uint32_t);
extern void LWSemA(uint32_t);
extern void LWEventB(uint32_t);
extern void LWEventA(uint32_t);
extern void Sender(uint32_t);
extern void Responder(uint32_t);
extern void main_task(uint32_t);


/* EOF */
