#ifndef __read_h__
#define __read_h__
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
*   This file contains the definitions for this example.
*
*
*END************************************************************************/


/* Number of Writer Tasks */
#define NUM_WRITERS  3

/* Task IDs */
#define WRITE_TASK   5
#define READ_TASK    6

/* 
** Global data structure accessible by read and write tasks. 
** It contains two lightweight semaphores that govern access to 
** the data variable.
*/  
typedef struct sw_fifo
{
   LWSEM_STRUCT   READ_SEM;
   LWSEM_STRUCT   WRITE_SEM;
   unsigned char          DATA;
} SW_FIFO, * SW_FIFO_PTR;


/* Function prototypes */
extern void write_task(uint32_t initial_data);
extern void read_task(uint32_t initial_data);

extern SW_FIFO fifo;

#endif
/* EOF */
