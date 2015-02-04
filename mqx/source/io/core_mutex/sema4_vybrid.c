/*HEADER**********************************************************************
*
* Copyright 2008-2011 Freescale Semiconductor, Inc.
* Copyright 2004-2011 Embedded Access Inc.
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
*   This file contains the MPXS30 specific driver functions for the SEMA4.
*
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>

static const void   *sema4_address[] = { (void *)SEMA4_BASE_PTR };

#if PSP_MQX_CPU_IS_VYBRID_A5
static const uint32_t sema4_vector[] = { INT_Directed_CA5_int0 };
#else
static const uint32_t sema4_vector[] = { 20 };
#endif

/*!
 * \brief Get SEMA4 base address.
 * 
 * This function returns base address of the specified SEMA4 device.
 * 
 * \param[in] device   SEMA4 device number.
 *
 * \return NULL (Failure.)
 * \return SEMA4 base address as SEMA4_MemMapPtr.
 */ 
SEMA4_MemMapPtr _bsp_get_sema4_base_address(uint32_t device)
{
   if (device < ELEMENTS_OF(sema4_address)) {
      return (SEMA4_MemMapPtr) sema4_address[device];
   }
   return NULL;
}

/*!
 * \brief Get SEMA4 vector.
 * 
 * This function returns ISR number for the specified SEMA4 device.
 * 
 * \param[in] device   SEMA4 device number.
 *
 * \return 0 (Failure.)
 * \return SEMA4 ISR number as uint32_t.
 */ 
uint32_t _bsp_get_sema4_vector(uint32_t device)
{
   if (device < ELEMENTS_OF(sema4_vector)) {
      return  sema4_vector[device];
   }
   return 0;
}

