#ifndef __nandflash_wl_config_h__
#define __nandflash_wl_config_h__
/*HEADER**********************************************************************
*
* Copyright 2010 Freescale Semiconductor, Inc.
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
*   The file contains configuration of FFS
*
*
*END************************************************************************/

#if BSPCFG_ENABLE_NANDFLASH
#error The FFS lib requires BSPCFG_ENABLE_NANDFLASH defined zero in user_config.h. Please recompile BSP with this option.
#endif

#if !MQX_USE_TIMER
#error The FFS lib requires MQX_USE_TIMER defined non-zero in user_config.h. Please recompile PSP with this option.
#endif

#endif /* __nandflash_wl_config_h__ */
/* EOF */
