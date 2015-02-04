#ifndef __pcb_shm_h__
#define __pcb_shm_h__
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
*   This file contains the definitions for the PCB device driver that
*   sends and receives packets over a asynchrnous serial device.  The
*   packets are in MQX IPC async packet format.
*
*
*END************************************************************************/

#include "bsp.h"

/*--------------------------------------------------------------------------*/
/*
**                          CONSTANT DECLARATIONS
*/

/*
** Initialization errors
*/
#define IO_PCB_SHM_DEVICE_ALREADY_OPEN         (IO_PCB_ERROR_BASE|0x90)
#define IO_PCB_SHM_INCORRECT_SERIAL_DEVICE     (IO_PCB_ERROR_BASE|0x91)

#if 0
/*
**             PACKET STRUCTURE CONTROL FIELD BIT DEFINITIONS
*/

#define IO_PCB_SHM_HDR_LITTLE_ENDIAN           (0x40)
#define IO_PCB_SHM_DATA_LITTLE_ENDIAN          (0x20)

#define IO_PCB_SHM_HDR_BIG_ENDIAN              (0x00)
#define IO_PCB_SHM_DATA_BIG_ENDIAN             (0x00)
#endif

/*--------------------------------------------------------------------------*/
/*
**                          DATATYPE DECLARATIONS
*/

/*
** IO_PCB_SHM_INIT_STRUCT
** This structure contains the initialization information for the
** async shared memory protocol
**
*/
typedef struct io_pcb_shm_init_struct
{

   /* Shared memory base address */
   void      *TX_BD_ADDR;

   /* TX ring limit */
   void      *TX_LIMIT_ADDR;

   /* Shared memory base address */
   void      *RX_BD_ADDR;

   /* RX ring limit */
   void      *RX_LIMIT_ADDR;

   /* Maximum size of input packet */
   _mem_size  INPUT_MAX_LENGTH;

   /* interrupt vector */
   uint32_t    RX_VECTOR;
   uint32_t    TX_VECTOR;
   uint32_t    REMOTE_RX_VECTOR;
   uint32_t    REMOTE_TX_VECTOR;

   /* The address of function to trigger interrupts */
   void (_CODE_PTR_ INT_TRIGGER)(uint32_t);

} IO_PCB_SHM_INIT_STRUCT, * IO_PCB_SHM_INIT_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                          C PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern void   *_bsp_vtop(void *ptr);
extern void   *_bsp_ptov(void *ptr);

extern _mqx_uint _io_pcb_shm_install(char *, void *);
#if BSPCFG_IO_PCB_SHM_SUPPORT
extern void _bsp_io_pcb_shm_int_trigger(uint32_t vector);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
