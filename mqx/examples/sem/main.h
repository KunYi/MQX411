#ifndef __main_h__
#define __main_h__
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
*   This file contains definitions for the semaphore example.
*
*
*END************************************************************************/

#define MAIN_TASK     5
#define WRITE_TASK    6
#define READ_TASK     7
#define ARRAY_SIZE    5
#define NUM_WRITERS   2

/* 
** Global data structure that is accessible by read and write tasks.
** Contains a data array that simulates a fifo. The read_index
** and write_index mark the location in the array that the read
** and write tasks are accessing. All data is protected by
** semaphores.
*/ 

typedef struct sw_fifo
{
   _task_id  DATA[ARRAY_SIZE];
   uint32_t   READ_INDEX;
   uint32_t   WRITE_INDEX; 
} SW_FIFO, * SW_FIFO_PTR;

/* Funtion prototypes */
extern void main_task(uint32_t initial_data);
extern void write_task(uint32_t initial_data);
extern void read_task(uint32_t initial_data);

extern    SW_FIFO fifo;

#endif
/* EOF */
