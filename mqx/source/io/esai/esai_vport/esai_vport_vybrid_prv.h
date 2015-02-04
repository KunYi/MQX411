#ifndef _esai_vport_vybrid_prv_h
#define _esai_vport_vybrid_prv_h
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
*   required for the ESAI VPORT drivers for vybrid.
*
*
*END************************************************************************/

#include "esai_vport_vybrid.h"

/*--------------------------------------------------------------------------*/
/*
**                      CONSTANT DECLARATIONS
*/

#define GET_ESAI_NODE_PTR(x) (&(x->ESAI_NODE))

//#define ESAI_FIFO_VYBRID_ENABLE_DEBUG
#ifdef ESAI_FIFO_VYBRID_ENABLE_DEBUG
#define ESAI_FIFO_VYBRID_DEBUG  printf
#else
#define ESAI_FIFO_VYBRID_DEBUG(...)
#endif

#define ESAI_FIFO_VYBRID_ENABLE_ERROR
#ifdef ESAI_FIFO_VYBRID_ENABLE_ERROR
#define ESAI_FIFO_VYBRID_ERROR  printf
#else
#define ESAI_FIFO_VYBRID_ERROR(...)
#endif


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** Run time state information for each ESAI VPORT module
*/
typedef struct vybrid_esai_vport_info_struct
{
   /* Current initialized values */
   VYBRID_ESAI_VPORT_INIT_STRUCT    INIT;

   /*channel num: mono or stereo*/
   uint8_t                            CHL_NUM;

   /*sample word length: 1,2,3,4 bytes*/
   uint8_t                            WORD_LENGTH;

   /*indicate whether the 8bits conversion for ASRC must enable*/
   bool                           ASRC_8BITS_CONV;

   /*mutex to protect on ESAI VPORT access*/
   MUTEX_STRUCT                      ESAI_MUTEX;

   /* error code of ESAI VPORT device*/
   uint32_t                           ERR_CODE;

} VYBRID_ESAI_VPORT_INFO_STRUCT, * VYBRID_ESAI_VPORT_INFO_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C"
{
#endif
extern uint32_t easi_fifo_set_asrc_dma(ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR,
                                          int32_t, int32_t);

extern uint32_t _vybrid_esai_vport_ioctl (IO_ESAI_VPORT_DEVICE_STRUCT_PTR,
                                        uint32_t, uint32_t *);

extern int32_t _vybrid_esai_vport_rx (IO_ESAI_VPORT_DEVICE_STRUCT_PTR,
                                        unsigned char *, int32_t);

extern int32_t _vybrid_esai_vport_tx (IO_ESAI_VPORT_DEVICE_STRUCT_PTR,
                                        unsigned char *, int32_t);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
