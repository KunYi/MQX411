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
*   The file contains the definitions for the MT29F2G16 flash device.
*
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "nandflash.h"


/* MT29F2G16AABWP organization */
#define MT29F2G16_PHYSICAL_PAGE_SIZE      2048
#define MT29F2G16_SPARE_AREA_SIZE         64
#define MT29F2G16_BLOCK_SIZE              131072 /* 128kB */
#define MT29F2G16_NUM_BLOCKS              2048
#define MT29F2G16_WIDTH                   16


NANDFLASH_INFO_STRUCT _MT29F2G16_organization_16bit[] =  {
  MT29F2G16_PHYSICAL_PAGE_SIZE,
  MT29F2G16_SPARE_AREA_SIZE, 
  MT29F2G16_BLOCK_SIZE,
  MT29F2G16_NUM_BLOCKS,
  MT29F2G16_WIDTH
};

/* EOF*/
