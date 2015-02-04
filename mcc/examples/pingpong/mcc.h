#ifndef __mcc_h__
#define __mcc_h__
/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
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
*   This file contains the source for the common definitions for the
*   MCC pingpong example
*
*
*END************************************************************************/

#define MAIN_TTN               (10)
#define RESPONDER_TTN          (11)

#define MCC_MQX_NODE_A5        (0)
#define MCC_MQX_NODE_M4        (0)

#define MCC_MQX_SENDER_PORT    (1)
#define MCC_MQX_RESPONDER_PORT (2)

typedef struct the_message
{
   uint32_t  DATA;
} THE_MESSAGE, * THE_MESSAGE_PTR;

#endif
