#ifndef _at49bvxx_h_
#define _at49bvxx_h_
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
*   The file contains functions prototype, defines, structure 
*   definitions specific for the Atmel AT49BVxx devices
*
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/* 
**              DEFINED VARIABLES
*/

extern const FLASHX_BLOCK_INFO_STRUCT _AT49BV040_block_map_16bit[];
extern const FLASHX_BLOCK_INFO_STRUCT _at49bv1614a_block_map_16bit[];
extern const FLASHX_BLOCK_INFO_STRUCT_PTR _at49bv6416_block_map_16bit[];

extern const FLASHX_DEVICE_IF_STRUCT _flashx_at49bvxxx_if;

#endif //_at49bvxx_h_
/* EOF */
