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
*   This file contains the symbols used to set addresses based on their
* location in the linker file.  For example, we put the KERNEL_DATA_START
* section after everything else in ram, and then its address tells us 
* where the balance of the free RAM starts for the kernel data.
*
*
*END************************************************************************/

  AREA KERNEL_DATA_START, DATA, ALIGN=4, READWRITE
  EXPORT __KERNEL_DATA_START
__KERNEL_DATA_START
  DCD 0

  AREA KERNEL_DATA_END, DATA, ALIGN=4, READWRITE
  EXPORT __KERNEL_DATA_END
__KERNEL_DATA_END
  DCD 0

  AREA BOOT_STACK, DATA, ALIGN=4, READWRITE
  EXPORT __BOOT_STACK_ADDRESS
__BOOT_STACK_ADDRESS
  DCD 0

  AREA UNCACHED_DATA_START, DATA, ALIGN=4, READWRITE
  EXPORT __UNCACHED_DATA_START
__UNCACHED_DATA_START
  DCD 0
  
  AREA UNCACHED_DATA_END, DATA, ALIGN=4, READWRITE
  EXPORT __UNCACHED_DATA_END
__UNCACHED_DATA_END
  DCD 0

  AREA SRAM_POOL_START, DATA, ALIGN=4, READWRITE
  EXPORT __SRAM_POOL_START
__SRAM_POOL_START
  DCD 0
  
  AREA SRAM_POOL_END, DATA, ALIGN=4, READWRITE
  EXPORT __SRAM_POOL_END
__SRAM_POOL_END
  DCD 0

  END

; EOF
