#ifndef _esai_fifo_h
#define _esai_fifo_h 1
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
*   required for the ESAI FIFO drivers for the vybrid.
*
*
*END************************************************************************/


/*--------------------------------------------------------------------------*/
/*
**                      CONSTANT DECLARATIONS
*/


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
**
** This structure defines the initialization parameters to be used
** when a ESAI FIFO driver is initialized.
*/
/*![ESAI driver FIFO layer init data structure]*/
typedef struct
{
   /* The ESAI controller to initialize, for Vybrid, it only can be 0 */
   uint32_t                MODULE_ID;

    /*
     * Transmitter FIFO water mark default value, unit: word(32 bts)
     * The transmit FIFO empty flag wiil be set when the number of empty slots in the
     * Transmit FIFO has met or exceeded the Transmit FIFO Watermark. In this case,
     * ESAI Transmitter DMA request line is also driven.
     * Refer to Vybrid reference manual for details.
     */
   uint8_t                 TX_FIFO_WM;

   /*
    * Receiver FIFO water mark default value, unit: word(32 bts)
    * The transmit FIFO empty flag wiil be set when the number of data words in the
    * Transmit FIFO has met or exceeded this Receiver FIFO Watermark. In this case,
    * ESAI Receiver DMA request line is also driven.
    * Refer to Vybrid reference manual for details.
    */
   uint8_t                 RX_FIFO_WM;

    /*
    * Tx default slot width , It can be 8, 12, 16, 20, 24, 32
    * Refer to ESAI TCR register TSWS field description.
    */
   uint8_t                 TX_DEFAULT_SLOT_WIDTH;

   /*
   * Rx default slot width , It can be 8, 12, 16, 20, 24, 32
   * Refer to ESAI RCR register RSWS field description.
   */
   uint8_t                 RX_DEFAULT_SLOT_WIDTH;

   /* Tx DMA packet size  */
   uint32_t                TX_DMA_PACKET_SIZE;

   /* Tx DMA packet max number, can't beyond 10  */
   uint32_t                TX_DMA_PACKET_MAX_NUM;

   /* Rx DMA packet size  */
   uint32_t                RX_DMA_PACKET_SIZE;

   /* Rx DMA packet max number, can't beyond 10   */
   uint32_t                RX_DMA_PACKET_MAX_NUM;

   /* Tx clock is master  */
   bool                TX_DEFAULT_CLOCK_MASTER;

   /* Rx clock is master  */
   bool                RX_DEFAULT_CLOCK_MASTER;

    /*
    * Synchronous operating mode
    * TRUE: ESAI synchronous mode is chosen and the transmit and receive sections
    *           all use transmitter section clock and frame sync signals.
    * FALSE: ESAI asynchronous mode is chosen, independent clock and frame
    *            sync signals are used for the transmit and receive sections.
    */
   bool                SYN_OPERATE_MODE;
} VYBRID_ESAI_FIFO_INIT_STRUCT, * VYBRID_ESAI_FIFO_INIT_STRUCT_PTR;
/*![ESAI driver FIFO layer init data structure]*/

typedef const VYBRID_ESAI_FIFO_INIT_STRUCT * VYBRID_ESAI_FIFO_INIT_STRUCT_CPTR;

typedef void * ESAI_FIFO_HANDLE;
/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern ESAI_FIFO_HANDLE _vybrid_esai_fifo_install (VYBRID_ESAI_FIFO_INIT_STRUCT_CPTR);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
