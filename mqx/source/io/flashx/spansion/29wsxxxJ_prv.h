#ifndef _29wsxxxJ_prv_h_
#define _29wsxxxJ_prv_h_
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
*   definitions specific for the Spansion S29ws128N,256N and 512N
*   flash devices.
*   Revision History:
*   Date          Version        Changes
*
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/*
**                     FUNCTION PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif

extern bool _29wsxxxJ_program(IO_FLASHX_STRUCT_PTR, char *, char *, _mem_size);
extern bool _29wsxxxJ_erase(IO_FLASHX_STRUCT_PTR, char *, _mem_size);
extern bool _29wsxxxJ_chip_erase(IO_FLASHX_STRUCT_PTR);
extern bool _29wsxxxJ_test_lock_bits(IO_FLASHX_STRUCT_PTR);
extern bool _29wsxxxJ_write_protect(IO_FLASHX_STRUCT_PTR, bool);
extern _mqx_int _29wsxxxJ_ioctl(IO_FLASHX_STRUCT_PTR, _mqx_uint, void *); 
#ifdef __cplusplus
}
#endif

#endif //_29wsxxxJ_prv_h_
/* EOF */
