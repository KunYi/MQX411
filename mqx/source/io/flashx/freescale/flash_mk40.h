/*HEADER**********************************************************************
*
* Copyright 2010-2011 Freescale Semiconductor, Inc.
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
*   The file contains functions prototype, defines for the internal 
*   flash driver.
*
*
*END************************************************************************/

#ifndef __flash_mk40_h__
#define __flash_mk40_h__

/* FlexNVM partition code values */
#define FLEXNVM_PART_CODE_DATA256_EE0       (0)
#define FLEXNVM_PART_CODE_DATA224_EE32      (3)
#define FLEXNVM_PART_CODE_DATA192_EE64      (4)
#define FLEXNVM_PART_CODE_DATA128_EE128     (5)
#define FLEXNVM_PART_CODE_DATA32_EE224      (0x0b)
#define FLEXNVM_PART_CODE_DATA64_EE192      (0x0c)
#define FLEXNVM_PART_CODE_DATA0_EE256       (8)
#define FLEXNVM_PART_CODE_NOPART            (0x0f)

/* FlexNVM EEPROM size values */
#define FLEXNVM_EE_SIZE_4096                (2)
#define FLEXNVM_EE_SIZE_2048                (3)
#define FLEXNVM_EE_SIZE_1024                (4)
#define FLEXNVM_EE_SIZE_512                 (5)
#define FLEXNVM_EE_SIZE_256                 (6)
#define FLEXNVM_EE_SIZE_128                 (7)
#define FLEXNVM_EE_SIZE_64                  (8)
#define FLEXNVM_EE_SIZE_32                  (9)
#define FLEXNVM_EE_SIZE_0                   (0x0f)

/* FlexNVM split values */
#define FLEXNVM_EE_SPLIT_1_7                (0)
#define FLEXNVM_EE_SPLIT_1_3                (0x10)
#define FLEXNVM_EE_SPLIT_1_1                (0x30)

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif

void   *_bsp_get_ftfl_address(void);
void kinetis_flash_invalidate_cache(volatile uint32_t);
void kinetis_flash_invalidate_cache_end(void);

#ifdef __cplusplus
}
#endif

#endif
