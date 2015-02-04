/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
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
#ifndef __flash_c90_prv_h__
#define __flash_c90_prv_h__


#include "flashx.h"
#include "flashxprv.h"


/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

/* SPECIAL_TAG definitions */
#define C90_ASPACE_MASK     0x0000000f
#define C90_CONTROLLER_MASK 0x000000f0
#define C90_WIDTH_MASK      0x0000ff00
#define C90_ALIGN_MASK      0x000f0000

#define C90_ASPACE_SHIFT        (0)
#define C90_CONTROLLER_SHIFT    (4)
#define C90_WIDTH_SHIFT         (8)
#define C90_ALIGN_SHIFT         (16)

#define C90_CONTROLLER_0    0x00000010
#define C90_CONTROLLER_1    0x00000020
#define C90_INTERLEAVED     (C90_CONTROLLER_0|C90_CONTROLLER_1)

#define C90_WIDTH_32        (32<<C90_WIDTH_SHIFT)
#define C90_WIDTH_64        (64<<C90_WIDTH_SHIFT)
#define C90_WIDTH_128       (128<<C90_WIDTH_SHIFT)

#define C90_ALIGN_32        (4<<C90_ALIGN_SHIFT)
#define C90_ALIGN_64        (8<<C90_ALIGN_SHIFT)

#define C90_ASPACE(x)       ((x&C90_ASPACE_MASK)>>C90_ASPACE_SHIFT)
#define C90_WIDTH(x)        ((x&C90_WIDTH_MASK)>>C90_WIDTH_SHIFT)
#define C90_ALIGN(x)        ((x&C90_ALIGN_MASK)>>C90_ALIGN_SHIFT)


/* module control register */
#define C90_MCR_EHV  0x01
#define C90_MCR_ESUS 0x02
#define C90_MCR_ERS  0x04
#define C90_MCR_PSUS 0x08
#define C90_MCR_PGM  0x10
#define C90_MCR_PEG  0x0200
#define C90_MCR_DONE 0x0400
#define C90_MCR_PEAS 0x0800
#define C90_MCR_SBC  0x2000
#define C90_MCR_RWE  0x4000
#define C90_MCR_ERR  0x8000
#define C90_MCR_MAS  0x10000

#define C90_MCR_LAS_MASK   0x700000
#define C90_MCR_LAS_SHIFT  20
#define C90_MCR_LAS(x)     (((x) << C90_MCR_LAS_SHIFT) & C90_MCR_LAS_MASK)

#define C90_MCR_SIZE_MASK  0x07000000
#define C90_MCR_SIZE_SHIFT 24
#define C90_MCR_SIZE(x)    (((x) << C90_MCR_SIZE_SHIFT) & C90_MCR_SIZE_MASK)


/* LAS/MAS block locking register */
#define C90_LML_LME 0x80000000
#define C90_LML_SLOCK 0x100000

#define C90_LML_MLOCK_MASK  0x30000
#define C90_LML_MLOCK_SHIFT 16
#define C90_LML_MLOCK(x)    (((x) << C90_LML_MLOCK_SHIFT) & C90_LML_MLOCK_MASK)

#define C90_LML_LLOCK_MASK  0x3ff
#define C90_LML_LLOCK_SHIFT 0
#define C90_LML_LLOCK(x)    (((x) << C90_LML_LLOCK_SHIFT) & C90_LML_LLOCK_MASK)

#define C90_LML_PASSWORD    0xA1A11111


/* HAS block locking register */
#define C90_HBL_HBE 0x80000000

#define C90_HBL_HLOCK_MASK  0x3f
#define C90_HBL_HLOCK_SHIFT 0
#define C90_HBL_HLOCK(x)    (((x) << C90_HBL_HLOCK_SHIFT) & C90_HBL_HLOCK_MASK)

#define C90_HBL_PASSWORD    0xB2B22222


/* secondary LAS/MAS block locking register */
#define C90_SLL_SLE 0x80000000
#define C90_SLL_SSLOC 0x100000

#define C90_SLL_SMLOCK_MASK  0x30000
#define C90_SLL_SMLOCK_SHIFT 16
#define C90_SLL_SMLOCK(x)    (((x) << C90_SLL_SMLOCK_SHIFT) & C90_SLL_SMLOCK_MASK)

#define C90_SLL_SLLOCK_MASK  0x3ff
#define C90_SLL_SLLOCK_SHIFT 0
#define C90_SLL_SLLOCK(x)    (((x) << C90_SLL_SLLOCK_SHIFT) & C90_SLL_SLLOCK_MASK)

#define C90_SLL_PASSWORD    0xC3C33333


/* LAS/MAS block select register */
#define C90_LMS_MSEL_MASK  0x30000
#define C90_LMS_MSEL_SHIFT 16
#define C90_LMS_MSEL(x)    (((x) << C90_LMS_MSEL_SHIFT) & C90_LMS_MSEL_MASK)

#define C90_LMS_LSEL_MASK  0x3ff
#define C90_LMS_LSEL_SHIFT 0
#define C90_LMS_LSEL(x)    (((x) << C90_LMS_LSEL_SHIFT) & C90_LMS_LSEL_MASK)


/* HAS block select register */
#define C90_HBS_HSEL_MASK  0x3f
#define C90_HBS_HSEL_SHIFT 0
#define C90_HBS_HSEL(x)    (((x) << C90_HBS_HSEL_SHIFT) & C90_HBS_HSEL_MASK)


#define C90_ASPACE_LAS 0
#define C90_ASPACE_MAS 1
#define C90_ASPACE_HAS 2
#define C90_ASPACE_SHADOW 3
#define C90_ASPACE_COUNT 4

/*----------------------------------------------------------------------*/
/*
**                    Structure Definitions
*/

/* Peripheral register access */
typedef struct c90_reg_struct {
    uint32_t MCR;
    uint32_t LML;
    uint32_t HBL;
    uint32_t SLL;
    uint32_t LMS;
    uint32_t HBS;
    uint32_t ADR;
    uint32_t PFCR0;
    uint32_t padding_20;
    uint32_t PFAPR;
    uint32_t padding_28;    
    uint32_t BIU4;
    uint32_t padding_30[3];    
    uint32_t UT0;
    uint32_t UT1;
    uint32_t UT2;
    uint32_t UM0;
    uint32_t UM1;
    uint32_t UM2;
    uint32_t UM3;
    uint32_t UM4;
} C90_REG_STRUCT, * C90_REG_STRUCT_PTR;

typedef volatile struct c90_reg_struct * VC90_REG_STRUCT_PTR;


typedef struct c90_flash_internal_struct 
{
   VC90_REG_STRUCT_PTR reg_ptr;
   void (_CODE_PTR_ flash_exec)(VC90_REG_STRUCT_PTR);
   uint32_t aspace_sectors[C90_ASPACE_COUNT];
} C90_FLASH_INTERNAL_STRUCT, * C90_FLASH_INTERNAL_STRUCT_PTR;


/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

bool c90_flash_init (IO_FLASHX_STRUCT_PTR);
void    c90_flash_deinit (IO_FLASHX_STRUCT_PTR);
bool c90_flash_sector_erase (IO_FLASHX_STRUCT_PTR, char *, _mem_size);
bool c90_flash_sector_program (IO_FLASHX_STRUCT_PTR, char *, char *, _mem_size);
bool c90_flash_chip_erase (IO_FLASHX_STRUCT_PTR);
bool c90_flash_write_protect(IO_FLASHX_STRUCT_PTR, _mqx_uint);

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
}
#endif

#endif
