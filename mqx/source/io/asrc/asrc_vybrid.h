#ifndef _asrc_vybrid_h
#define _asrc_vybrid_h 1
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
*   This file is ASRC header file for vybrid family
*
*
*END************************************************************************/

/*
** ASRC_VYBRID_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a ASRC is initialized.
*/
typedef struct vybrid_asrc_init_struct
{
   /* The device ID to identify different device*/
   uint8_t                 DEV_ID;

   /* The channel's number for ASRC pair A, this value can't be changed dynamclly*/
   uint8_t                 PAIR_A_CHANNELS;

   /* The channel's number for ASRC pair B, this value can't be changed dynamclly*/
   uint8_t                 PAIR_B_CHANNELS;

   /* The channel's number for ASRC pair C, this value can't be changed dynamclly*/
   uint8_t                 PAIR_C_CHANNELS;

   /* Default slot width for input port*/
   uint8_t                 INPUT_SLOT_WIDTH;

   /* Default slot width for output port*/
   uint8_t                 OUTPUT_SLOT_WIDTH;

} VYBRID_ASRC_INIT_STRUCT, * VYBRID_ASRC_INIT_STRUCT_PTR;

typedef const VYBRID_ASRC_INIT_STRUCT * VYBRID_ASRC_INIT_STRUCT_CPTR;

/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t _asrc_vybrid_install(char * identifier,
                VYBRID_ASRC_INIT_STRUCT_CPTR init_data_ptr);

#ifdef __cplusplus
}
#endif

#endif
