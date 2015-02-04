#ifndef __ftm_vybrid_h__
#define __ftm_vybrid_h__ 1
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
*   This file is used to provide the board-specific Flextimer initialization APIs and defines.
*
*
*END************************************************************************/


#define INT_FTM0 74
#define INT_FTM1 75
#define INT_FTM2 76
#define INT_FTM3 77

/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern void * _bsp_get_ftm_base_address (uint8_t);
extern uint32_t _bsp_get_ftm_vector (uint8_t);
extern void _bsp_dump_ftm_register(FTM_MemMapPtr);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */

