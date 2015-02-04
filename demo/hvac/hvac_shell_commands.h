#ifndef __sh_hvac_h__
#define __sh_hvac_h__
/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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
*   
*
*
*END************************************************************************/


extern int32_t Shell_fan(int32_t argc, char *argv[] ); 
extern int32_t Shell_hvac(int32_t argc, char *argv[] ); 
extern int32_t Shell_scale(int32_t argc, char *argv[] ); 
extern int32_t Shell_temp(int32_t argc, char *argv[] ); 
extern int32_t Shell_info(int32_t argc, char *argv[] ); 
extern int32_t Shell_log(int32_t argc, char *argv[] ); 

#endif

/* EOF*/
