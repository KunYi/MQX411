
/*HEADER**********************************************************************
*
* Copyright 2012 Freescale Semiconductor, Inc.
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains utiltity functions for use with an A5.
*
*
*END************************************************************************/

#include "mqx_inc.h"


#if !PSP_HAS_SUPPORT_STRUCT
#error ARM PSP requires PSP_HAS_SUPPORT_STRUCT to be set
#endif

/*!
 * \brief Set PSP_SUPPORT_STRUCT to kernel data
 * 
 * \param psp_sup_ptr 
 */
void _psp_set_support_ptr(PSP_SUPPORT_STRUCT_PTR psp_sup_ptr)
{
   KERNEL_DATA_STRUCT_PTR kernel_data;

   if (psp_sup_ptr != NULL)
   {
      _GET_KERNEL_DATA(kernel_data);

      _mem_set_type(psp_sup_ptr, MEM_TYPE_PSP_SUPPORT_STRUCT);

      kernel_data->PSP_SUPPORT_PTR = psp_sup_ptr;
   }
}

/*!
 * \brief Return PSP_SUPPORT_STRUCT from kernel data
 *
 * \return pointer to PSP_SUPPORT_STRUCT
 */
PSP_SUPPORT_STRUCT_PTR _psp_get_support_ptr(void)
{
   KERNEL_DATA_STRUCT_PTR kernel_data;

   _GET_KERNEL_DATA(kernel_data);

   return kernel_data->PSP_SUPPORT_PTR;
}

/*!
 * \brief Initilize the support functions for the A5 platforms
 */
void _a5_initialize_support(void)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    PSP_SUPPORT_STRUCT_PTR psp_sup_ptr;

    _GET_KERNEL_DATA(kernel_data);

    psp_sup_ptr = _mem_alloc_system_zero((uint32_t)sizeof(PSP_SUPPORT_STRUCT));

    if (psp_sup_ptr)
    {
       _mem_set_type(psp_sup_ptr, MEM_TYPE_PSP_SUPPORT_STRUCT);
       kernel_data->PSP_SUPPORT_PTR = psp_sup_ptr;
    }
}

/*!
 * \brief This function returns the core number
 *
 * \return int
 */
uint32_t _psp_core_num(void)
{
    return MSCM_CPxNUM & MSCM_CPxNUM_CPN_MASK;
}
