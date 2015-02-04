#ifndef __INT_FLASH_MCF51JM_H__
#define __INT_FLASH_MCF51JM_H__
/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
* Copyright 1989-2008 ARC International
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
*   The file contains functions prototype, defines for the internal 
*   flash driver
*
*
*END************************************************************************/


/*----------------------------------------------------------------------*/
/*
** FUNCTION PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif

void   *_bsp_get_cfm_address();
void   *_bsp_get_pmc_address();

#ifdef __cplusplus
}
#endif

#endif /* __INT_FLASH_MCF51JM_H__ */
