#ifndef _esai_vport_vybrid_h
#define _esai_vport_vybrid_h 1
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
*   required for the ESAI VPORT driver for vybrid.
*
*
*END************************************************************************/
#include "esai_fifo.h"

/*--------------------------------------------------------------------------*/
/*
**                      CONSTANT DECLARATIONS
*/


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/
/*
** This enumeration defines the VPORT directions
*/
typedef enum
{
   VPORT_DIR_TX,
   VPORT_DIR_RX
} ESAI_VPORT_DIRECTION;

/*
**
** This structure defines the initialization parameters to be used
** when a ESAI FIFO driver is initialized.
*/
/*![ESAI VPORT init data structure]*/
typedef struct
{
   /* The ESAI transceiver to be initialized, index from 0 */
   uint32_t                TRANSCEIVER_ID;

   /* The direction configured for this transceiver: tx or rx*/
   ESAI_VPORT_DIRECTION   PORT_DIRECTION;

} VYBRID_ESAI_VPORT_INIT_STRUCT, * VYBRID_ESAI_VPORT_INIT_STRUCT_PTR;
/*![ESAI VPORT init data structure]*/

typedef const VYBRID_ESAI_VPORT_INIT_STRUCT * VYBRID_ESAI_VPORT_INIT_STRUCT_CPTR;

/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t _vybrid_esai_vport_install (char * identifier,
    VYBRID_ESAI_VPORT_INIT_STRUCT_CPTR init_data_ptr, ESAI_FIFO_HANDLE esai_fifo_handle);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
