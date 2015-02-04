/*HEADER**********************************************************************
*
* Copyright 2012 Freescale Semiconductor, Inc.
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
*   This file contains parameters for PCB over shared memory initialization
*
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <pcb_shm.h>

/*
** parameters for PCB over shared memory initialization
*/
const IO_PCB_SHM_INIT_STRUCT pcb_shm_init =
{
    /* TX_BD_ADDR       */ (unsigned char *)(BSP_SHARED_RAM_START)+1024,
    /* TX_LIMIT_ADDR    */ (unsigned char *)(BSP_SHARED_RAM_START)+2048,
    /* RX_BD_ADDR       */ (void *)(BSP_SHARED_RAM_START),
    /* RX_LIMIT_ADDR    */ (unsigned char *)(BSP_SHARED_RAM_START)+1024,
    /* INPUT_MAX_LENGTH */ 128,
    /* RX_VECTOR        */ NVIC_CPU_to_CPU_int0,
    /* TX_VECTOR        */ NVIC_CPU_to_CPU_int1,
    /* REMOTE_RX_VECTOR */ INT_CPU_to_CPU_int0,
    /* REMOTE_TX_VECTOR */ INT_CPU_to_CPU_int1,
    /* INT_TRIGGER      */ _bsp_io_pcb_shm_int_trigger
};
