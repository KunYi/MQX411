/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
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
*   This file contains the definitions of constants and structures
*   required for playing and recording audio.
*
*
*END************************************************************************/
#ifndef __sh_audio_h_
#define __sh_audio_h_

#include <mqx.h>
#include <wav.h>
#include <fio.h>
#include <lwevent.h>
#include <message.h>

/* the write message queue */
#define WRITE_QUEUE 8
/* size of each record block */
#define REC_BLOCK_SIZE 512
/* number of initial messages in the pool */
#define MSG_NUM_INIT    10
/* block write ready event */
#define EV_BLOCK0_W_READY        0x01
#define EV_BLOCK1_W_READY        0x02

#if (defined(__CWCC__)||defined(__GNUC__))
   #define ALIGN  __attribute__ ((packed))
#elif defined(__IAR_SYSTEMS_ICC__)||defined(__CC_ARM)
   #pragma pack(push)
   #pragma pack(1)
   #define ALIGN
#endif
/* Parameter structure required for SD write task */
typedef struct sd_write_param_struct
{
   /* Pointer to output file */
   MQX_FILE_PTR file_ptr;
   
   /* Flag indicating finished reading from device */
   bool dev_read_done;
   
   /* Event pointer signaling finished writing to SD card */
   LWEVENT_STRUCT_PTR sdwrite_event_ptr;
   
}ALIGN SD_WRITE_PARAM_STRUCT;

/* This struct contains a data field and a message struct. */
typedef struct {
   /* Message header */ 
   MESSAGE_HEADER_STRUCT HEADER;
   
   /* Data */
   char DATA[REC_BLOCK_SIZE];
   
} REC_MESSAGE, * REC_MESSAGE_PTR;

#if defined(__IAR_SYSTEMS_ICC__)
#pragma pack(pop)   /* Restore memory alignment configuration */
#endif
extern int32_t Shell_mute(int32_t argc, char *argv[]);
extern int32_t Shell_record(int32_t argc, char *argv[]);
extern void Sdcard_write_task(uint32_t temp);
#endif

/* EOF */
