#ifndef __flash_ftfl_h__
#define __flash_ftfl_h__
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
*   The file contains function prototypes and defines for the internal 
*   flash driver.
*
*
*END************************************************************************/

/* FlexNVM IOCTL codes */
#define FLEXNVM_IOCTL_READ_RESOURCE         _IO(IO_TYPE_FLASH, 0x14)
#define FLEXNVM_IOCTL_GET_PARTITION_CODE    _IO(IO_TYPE_FLASH, 0x15)
#define FLEXNVM_IOCTL_SET_PARTITION_CODE    _IO(IO_TYPE_FLASH, 0x16)
#define FLEXNVM_IOCTL_GET_FLEXRAM_FN        _IO(IO_TYPE_FLASH, 0x17)
#define FLEXNVM_IOCTL_SET_FLEXRAM_FN        _IO(IO_TYPE_FLASH, 0x18)
#define FLEXNVM_IOCTL_WAIT_EERDY            _IO(IO_TYPE_FLASH, 0x19)
#define FLEXNVM_IOCTL_GET_EERDY             _IO(IO_TYPE_FLASH, 0x1a)

/* FlexNVM EEPROM / FlexRAM choice */
#define FLEXNVM_FLEXRAM_RAM                 (0xff)
#define FLEXNVM_FLEXRAM_EE                  (0)

/*----------------------------------------------------------------------*/
/* 
**              EXPORTED TYPES
*/

typedef struct flexnvm_prog_part_struct {
    uint8_t EE_DATA_SIZE_CODE;
    uint8_t FLEXNVM_PART_CODE;
} FLEXNVM_PROG_PART_STRUCT, * FLEXNVM_PROG_PART_STRUCT_PTR;

typedef struct flexnvm_read_rsrc_struct {
    uint32_t ADDR;
    uint8_t RSRC_CODE;
    uint32_t RD_DATA;
} FLEXNVM_READ_RSRC_STRUCT, * FLEXNVM_READ_RSRC_STRUCT_PTR;

/*----------------------------------------------------------------------*/
/* 
**              DEFINED VARIABLES
*/
extern const FLASHX_BLOCK_INFO_STRUCT _flashx_kinetisN_block_map[];
extern const FLASHX_BLOCK_INFO_STRUCT _flashx_kinetisX_block_map[];
extern const FLASHX_BLOCK_INFO_STRUCT _flashx_mcf51xx_plus_block_map[];

extern const FLASHX_DEVICE_IF_STRUCT _flashx_ftfl_if;

#endif //__flash_ftfl_h__
