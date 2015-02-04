/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This include file is used to provide information needed by
*   applications using the SAI I/O functions.
*
*
*END************************************************************************/

#ifndef __main_h_
#define __main_h_

#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>
#include <message.h>

extern void Init_task(uint32_t);
extern void Sdcard_task(uint32_t);
extern void Shell_task(uint32_t);
extern void Sdcard_write_task(uint32_t);

#define AUDIO_SAMPLE_RATE   (44100)
#ifdef BSPCFG_ENABLE_SAI
#define CLK_MULT             (384)
#else
#define CLK_MULT             (256)
#endif

#define INIT_TASK 1
#define SDCARD_TASK 2
#define SHELL_TASK 3
#define SDCARD_WRITE_TASK 4

#define WRITE_QUEUE 8
#define REC_BLOCK_SIZE 512

/* Parameter structure required for SD write task */
typedef struct sd_write_param_struct
{
    /* Pointer to output file */
    MQX_FILE_PTR file_ptr;
    
    /* LW Semaphore pointer signaling finished writing to SD card */
    LWSEM_STRUCT sdwrite_sem;   
} SD_WRITE_PARAM_STRUCT;

/* This struct contains a data field and a message struct. */
typedef struct
{
    /* Message header */ 
    MESSAGE_HEADER_STRUCT HEADER;
    
    /* Data length */
    uint32_t LENGTH;
    
    /* Data */
    char DATA[REC_BLOCK_SIZE];   
} REC_MESSAGE, * REC_MESSAGE_PTR;

#endif /* __main_h_ */

/* EOF */
