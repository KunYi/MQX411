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

#ifndef __flash_ftfl_prv_h__
    #define __flash_ftfl_prv_h__


#include "flashx.h"

/*--------------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

/* FTFL commands */
#define FTFL_VERIFY_BLOCK              0x00
#define FTFL_VERIFY_SECTION            0x01
#define FTFL_PROGRAM_CHECK             0x02
#define FTFL_READ_RESOURCE             0x03
#define FTFL_PROGRAM_LONGWORD          0x06
#define FTFL_PROGRAM_PHRASE            0x07
#define FTFL_ERASE_BLOCK               0x08
#define FTFL_ERASE_SECTOR              0x09
#define FTFL_PROGRAM_SECTION           0x0B
#define FTFL_VERIFY_ALL_BLOCK          0x40
#define FTFL_READ_ONCE                 0x41
#define FTFL_PROGRAM_ONCE              0x43
#define FTFL_ERASE_ALL_BLOCK           0x44
#define FTFL_SECURITY_BY_PASS          0x45
#define FTFL_PFLASH_SWAP               0x46
#define FTFL_PROGRAM_PARTITION         0x80
#define FTFL_SET_EERAM                 0x81

/* FTFL margin read settings */
#define FTFL_MARGIN_NORMAL             0x0000
#define FTFL_USER_MARGIN_LEVEL1        0x0001
#define FTFL_USER_MARGIN_LEVEL0        0x0002
#define FTFL_FACTORY_MARGIN_LEVEL0     0x0003
#define FTFL_FACTORY_MARGIN_LEVEL1     0x0004

/* FTFL sizes */
#define FTFL_WORD_SIZE                 0x0002
#define FTFL_LONGWORD_SIZE             0x0004
#define FTFL_PHRASE_SIZE               0x0008
#define FTFL_DPHRASE_SIZE              0x0010

/* FTFL error codes */
#define FTFL_OK                        0x0000
#define FTFL_ERR_SIZE                  0x0001
#define FTFL_ERR_RANGE                 0x0002
#define FTFL_ERR_ACCERR                0x0004
#define FTFL_ERR_PVIOL                 0x0008
#define FTFL_ERR_MGSTAT0               0x0010
#define FTFL_ERR_CHANGEPROT            0x0020
#define FTFL_ERR_EEESIZE               0x0040
#define FTFL_ERR_EFLASHSIZE            0x0080
#define FTFL_ERR_ADDR                  0x0100
#define FTFL_ERR_NOEEE                 0x0200
#define FTFL_ERR_EFLASHONLY            0x0400
#define FTFL_ERR_DFLASHONLY            0x0800
#define FTFL_ERR_RDCOLERR              0x1000
#define FTFL_ERR_RAMRDY                0x2000

/* Start and End Address of PFlash IFR and DFlash IFR */
#define PFLASH_IFR_START_ADDRESS       0x000000
#define PFLASH_IFR_END_ADDRESS         0x0000FF
#define DFLASH_IFR_START_ADDRESS       0x800000
#define DFLASH_IFR_END_ADDRESS         0x8000FF
#define DFLASH_IFR_LONGWORD_ADDRESS    0x8000FC

#define FLASHX_INVALIDATE_CACHE_ALL    0xFFFFFFFF
#define FLASHX_INVALIDATE_CACHE_BLOCK0 0x00000001
#define FLASHX_INVALIDATE_CACHE_BLOCK1 0x00000002
#define FLASHX_INVALIDATE_CACHE_BLOCK2 0x00000004
#define FLASHX_INVALIDATE_CACHE_BLOCK3 0x00000008

/* FlexNVM partition code bit mask */
#define FLEXNVM_PART_CODE_MASK              (0x0f)

/* FlexNVM split bit mask */
#define FLEXNVM_EE_SPLIT_MASK               (0x30)

/* FlexNVM EEPROM size bit mask */
#define FLEXNVM_EE_SIZE_MASK                (0x0f)

/* Flash Swap State */
#define FTFL_SWAP_UNINIT                0x00
#define FTFL_SWAP_READY                 0x01
#define FTFL_SWAP_INIT                  0x01
#define FTFL_SWAP_UPDATE                0x02
#define FTFL_SWAP_UPDATE_ERASED         0x03
#define FTFL_SWAP_COMPLETE              0x04

/* PFlash swap control codes */
#define FTFL_SWAP_SET_INDICATOR_ADDR    0x01
#define FTFL_SWAP_SET_IN_PREPARE        0x02
#define FTFL_SWAP_SET_IN_COMPLETE       0x04
#define FTFL_SWAP_REPORT_STATUS         0x08

/* Check if flash swap feature is available */
#define FTFL_SWAP_SUPPORT \
(defined(PSP_HAS_FLASH_SWAP) && PSP_HAS_FLASH_SWAP && defined(BSPCFG_SWAP_INDICATOR_ADDR))

/* Start address of FLASH_SWAP_INDICATOR valid range*/
#define FTFL_SWAP_VALID_RANGE_START     (0x00000420)

/* End address of FLASH_SWAP_INDICATOR valid range*/
#define FTFL_SWAP_VALID_RANGE_END       (BSP_INTERNAL_FLASH_SIZE / 2)

/* Upper flash swap indicator address - symmetric to BSPCFG_SWAP_INDICATOR_ADDR */
#define FTFL_SWAP_UPPER_INDICATOR_ADDR \
(BSPCFG_SWAP_INDICATOR_ADDR + (BSP_INTERNAL_FLASH_SIZE / 2))

/*--------------------------------------------------------------------------*/
/*
**                    Structure Definitions
*/

typedef struct ftfl_flash_internal_struct 
{
    volatile char  *ftfl_ptr;
    char   *flash_execute_code_ptr;
#if PSP_MQX_CPU_IS_KINETIS
    char   *flash_invalidate_code_ptr;
#endif
        
} FTFL_FLASH_INTERNAL_STRUCT, * FTFL_FLASH_INTERNAL_STRUCT_PTR;

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

bool  ftfl_flash_init(IO_FLASHX_STRUCT_PTR);
void     ftfl_flash_deinit(IO_FLASHX_STRUCT_PTR);
bool  ftfl_flash_erase_sector(IO_FLASHX_STRUCT_PTR, char *, _mem_size);
bool  ftfl_flash_write_sector(IO_FLASHX_STRUCT_PTR, char *, char *, _mem_size);
bool  ftfl_flash_write_sector_pa_ram(IO_FLASHX_STRUCT_PTR, char *, char *, _mem_size);
_mqx_int flexnvm_flash_ioctl(IO_FLASHX_STRUCT_PTR, _mqx_uint, void *);

#ifdef __cplusplus
}
#endif

#endif /* __flash_ftfl_prv_h__ */
