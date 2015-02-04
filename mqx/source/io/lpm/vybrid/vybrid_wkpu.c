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
*   WKPU Vybrid BSP specific function implementation
*
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <bsp_prv.h>

void wkpu_wakeup_enable(uint32_t source)
{
    WKPU_IRER |= (1<<source);
    WKPU_WRER |= (1<<source);

    WKPU_WIFEER |= (1<<source);

    GPC_IMR3 &= ~GPC_IMR3_WKPU0_MASK;
}

void wkpu_clear_wakeup(uint32_t source)
{
    WKPU_WISR |= (1<<source);
}

uint32_t wkpu_int_get_vector(void)
{
#if PSP_MQX_CPU_IS_VYBRID_A5
	/* return CA5 VECTOR */
    return INT_WKPU0;
#elif PSP_MQX_CPU_IS_VYBRID_M4
	/* return CM4 VECTOR */
	return NVIC_WKPU0;
#else
	#error "Unsupported core"
#endif
}
