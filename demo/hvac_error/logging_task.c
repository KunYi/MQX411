/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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

#include <string.h>
#include "hvac.h"
#include "logging_public.h"

#if DEMOCFG_ENABLE_AUTO_LOGGING

#include "logging_private.h"
#include "usb_file.h"

extern LWSEM_STRUCT  USB_Stick;
extern void         *USB_handle;

_pool_id  log_pool=NULL;
LWSEM_STRUCT Logging_init_sem;


void LogInit(void)
{ 
   _lwsem_create(&Logging_init_sem,0);
   _task_create(0, LOGGING_TASK, 0); 
   _lwsem_wait(&Logging_init_sem);
}


void Log(char *msg) 
{
   LOG_MESSAGE_PTR msg_ptr;

   if (log_pool) {
      msg_ptr = (LOG_MESSAGE_PTR)_msg_alloc(log_pool);

      if (msg_ptr != NULL) {
         msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, LOG_QUEUE);
         strncpy(msg_ptr->MESSAGE,msg,LOG_MESSAGE_SIZE);
         _msgq_send(msg_ptr);
      }
   }
}


void Logging_task(uint32_t param)
{
   _queue_id         log_qid;
   LOG_MESSAGE_PTR   msg_ptr;
#if DEMOCFG_ENABLE_USB_FILESYSTEM
  MQX_FILE_PTR      log_fp;
#endif

   /* create a pool of logging messages */   
   log_pool = _msgpool_create(sizeof(LOG_MESSAGE), LOG_POOL_SIZE, 0, 0);
   if (MSGPOOL_NULL_POOL_ID == log_pool) {
      printf("\nCould not create a message pool\n");
      _task_block();
   }

   /* open a message queue to receive log message on */
   log_qid = _msgq_open(LOG_QUEUE, 0);
   if (MSGQ_NULL_QUEUE_ID == log_qid) {
      printf("\nCould not open the message queue\n");
      _task_block();
   }

   /* signal that initialization is complete */
   _lwsem_post(&Logging_init_sem);

   while (TRUE) {
      /* wait for a message */
      msg_ptr = _msgq_receive(log_qid, 0);

      if (msg_ptr) {
         #if DEMOCFG_ENABLE_USB_FILESYSTEM
            /* check to see if a filesystem is available */
            if (_lwsem_poll(&USB_Stick)) {

               /* Open the log file and position to the end */
               log_fp = fopen(LOG_FILE,"a");

               if (log_fp) {
                  fseek(log_fp,0,IO_SEEK_END);
         
                  do {
                     /* Write the message to the log file */
                     write(log_fp,msg_ptr->MESSAGE, strlen(msg_ptr->MESSAGE));

                     /* Return the message back to the message pool */
                     _msg_free(msg_ptr);

                     /* check for another message */
                     msg_ptr = _msgq_poll(log_qid);
                  } while (msg_ptr != NULL);

                  /* close the file */
                  fclose(log_fp);
               }

               /* Indicate that the USB stick is no longer in use */
               _lwsem_post(&USB_Stick);
            } else {
               _msg_free(msg_ptr);
            }
         #else
            printf(msg_ptr->MESSAGE);
            _msg_free(msg_ptr);
         #endif
      }
   }
}

#else
void Log(char *msg) 
{
}
#endif

/* EOF */
