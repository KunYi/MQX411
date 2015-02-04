#ifndef _esai_fifo_prv_h
#define _esai_fifo_prv_h
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
*   required for the ESAI FIFO drivers for iMX53.
*
*
*END************************************************************************/

#include "esai_fifo.h"
#include "esai_fifo_config.h"
#include "esai_fifo_node.h"
//#include "esai_dim_define.h"
#include "vybrid_esai_regs.h"

/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/
typedef enum
{
    ESAI_FIFO_EVENT_TX_DATA_TRANSFERED = 1,
    ESAI_FIFO_EVENT_TX_DATA_DEMAND,
    ESAI_FIFO_EVENT_RX_DATA_READY,
    ESAI_FIFO_EVENT_RX_DATA_OVERFLOW
} ESAI_FIFO_EVENT_ID;

/*
** Run time state information for each ESAI FIFO module
*/
typedef struct
{
   VYBRID_ESAI_FIFO_INIT_STRUCT      INIT;

   ESAI_FIFO_AUD_IO_NODE_STRUCT      ESAI_NODE;

   ESAI_FIFO_CONFIG_STRUCT           TX_CONFIG;

   ESAI_FIFO_CONFIG_STRUCT           RX_CONFIG;

   uint8_t TX_CONNECT_COUNT;
   uint8_t RX_CONNECT_COUNT;

   uint8_t TX_DEVS_COUNT;
   uint8_t RX_DEVS_COUNT;

   uint8_t TX_BITMAP;
   uint8_t RX_BITMAP;

   uint8_t TX_ENABLE_BITMAP;
   uint8_t RX_ENABLE_BITMAP;

//   dim_hdl  DIM_TX_PTR;
//   dim_hdl  DIM_RX_PTR;

   MUTEX_STRUCT                      CONTEXT_MUTEX;

} ESAI_FIFO_CONTEXT_STRUCT, * ESAI_FIFO_CONTEXT_STRUCT_PTR;


#endif
/* EOF */
