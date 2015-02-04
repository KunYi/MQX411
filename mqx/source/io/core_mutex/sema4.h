/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*  This file contains the type definitions for the MPXxxx SEMA4 module.
*
*
*END************************************************************************/

#ifndef __sema4_h__
#define __sema4_h__

#define __sema4_h__version "$Version:3.8.2.0$"
#define __sema4_h__date    "$Date:Sep-24-2012$"


#define SEMA4_NUM_GATES 16

#define SEMA4_UNLOCK    0

/* PPC dont have SEMA4 register structure */
#if PSP_MQX_CPU_IS_PPC
typedef volatile struct sema4_reg_struct
{
   unsigned char    GATE[SEMA4_NUM_GATES];
   RESERVED_REGISTER(0x10,0x40);
   uint16_t  CP0INE;
   RESERVED_REGISTER(0x42,0x48);
   uint16_t  CP1INE;
   RESERVED_REGISTER(0x4a,0x80);
   uint16_t  CP0NTF;
   RESERVED_REGISTER(0x82,0x88);
   uint16_t  CP1NTF;
   RESERVED_REGISTER(0x8a,0x100);
   uint16_t  RSTGT;
   RESERVED_REGISTER(0x102,0x104);
   uint16_t  RSTNTF;
   RESERVED_REGISTER(0x106,0x400);
} SEMA4_REG_STRUCT;

typedef volatile struct sema4_reg_struct *SEMA4_MemMapPtr;
#endif

#endif
