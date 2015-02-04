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
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the type definitions for the ARM Cortex processors.
*
*
*END************************************************************************/
#ifndef __cortexa5_h__
#define __cortexa5_h__

/* MMU constant definition of L1 table */
#define MMU_TRANSLATION_TABLE_ALIGN         (0x00004000)
#define MMU_TRANSLATION_TABLE_ALIGN_MASK    (~(MMU_TRANSLATION_TABLE_ALIGN - 1))
#define MMU_TRANSLATION_TABLE_ENTRIES       (0x00001000)
#define MMU_TRANSLATION_LINE_SIZE           (sizeof(uint32_t))
#define MMU_TRANSLATION_TABLE_SIZE          (MMU_TRANSLATION_TABLE_ENTRIES * MMU_TRANSLATION_LINE_SIZE)


/*-----------------------Memory Configuration-------------------------*/

#define PSP_IRAMBLK1_START  0xf8000000
#define PSP_IRAMBLK1_SIZE   0x20000
#define PSP_IRAMBLK1_END    (PSP_IRAMBLK1_START + PSP_IRAMBLK1_SIZE)

#define PSP_IRAMBLK2_START  0xf8020000
#define PSP_IRAMBLK2_SIZE   0x40000
#define PSP_IRAMBLK2_END    (PSP_IRAMBLK2_START + PSP_IRAMBLK2_SIZE)

#define PSP_FLTT_SIZE       (16*1024)



#ifndef GIC_PRIOR_IMPL
#define GIC_PRIOR_IMPL          (3)   /* minimal implemented priority required by cortex core */
#endif

#define GIC_PRIOR_SHIFT         (8 - GIC_PRIOR_IMPL)
#define GIC_PRIOR_MASK          ((0xff << GIC_PRIOR_SHIFT) & 0xff)
#define GIC_PRIOR(x)            (((x) << GIC_PRIOR_SHIFT) & GIC_PRIOR_MASK)

#define PSP_INT_FIRST_INTERNAL      0
#define PSP_INT_LAST_INTERNAL       0x3ff

#define PSP_INT_FIRST_INT_ROUTER    48        /* A5 core vector 48 == GIC vector 32 -> first vector in interrupt router */

#ifndef __ASM__

/*
** Standard cache macros
*/
#define PSP_PAGE_TYPE_MASK                  0x000000ff
#define PSP_PAGE_TYPE_SHIFT                 0
#define PSP_PAGE_TYPE(x)                    ((uint32_t)((x << PSP_PAGE_TYPE_SHIFT) & PSP_PAGE_TYPE_MASK))
#define PSP_PAGE_DESCR_MASK                 0x00000f00
#define PSP_PAGE_DESCR_SHIFT                8
#define PSP_PAGE_DESCR(x)                   ((uint32_t)((x << PSP_PAGE_DESCR_SHIFT) & PSP_PAGE_DESCR_MASK))
#define PSP_PAGE_SHARE_MASK                 0x0000f000
#define PSP_PAGE_SHARE_SHIFT                12
#define PSP_PAGE_SHARE(x)                   ((uint32_t)((x << PSP_PAGE_SHARE_SHIFT) & PSP_PAGE_SHARE_MASK))
#define PSP_PAGE_TABLE_SECTION_SIZE_MASK    0x000f0000
#define PSP_PAGE_TABLE_SECTION_SIZE_SHIFT   16
#define PSP_PAGE_TABLE_SECTION_SIZE(x)      ((uint32_t)((x << PSP_PAGE_TABLE_SECTION_SIZE_SHIFT) & PSP_PAGE_TABLE_SECTION_SIZE_MASK))

/* PSP_PAGE_TYPE macros */
#define PSP_PAGE_TYPE_CACHE_WTNWA           1
#define PSP_PAGE_TYPE_CACHE_WBNWA           2
#define PSP_PAGE_TYPE_CACHE_WBWA            3   /* This option is not valid for al cortex A procesors */
#define PSP_PAGE_TYPE_CACHE_NON             4
#define PSP_PAGE_TYPE_STRONG_ORDER          5
#define PSP_PAGE_TYPE_SHAREABLE             6
#define PSP_PAGE_TYPE_ENTRY_FAULT           7

/* PSP_PAGE_DESCR macros */
#define PSP_PAGE_DESCR_ACCESS_RW_ALL        1
#define PSP_PAGE_DESCR_ACCESS_RO_ALL        2
#define PSP_PAGE_DESCR_NOACCESS_ALL         3

/* PSP_PAGE_SHARE macros */
#define PSP_PAGE_SHARE_NON_SHAREABLE        1
#define PSP_PAGE_SHARE_SHAREABLE            2

/* PSP_PAGE_TABLE_SECTION_SIZE macros */
#define PSP_PAGE_TABLE_SECTION_SIZE_4KB     1
#define PSP_PAGE_TABLE_SECTION_SIZE_64KB    2
#define PSP_PAGE_TABLE_SECTION_SIZE_1MB     3
#define PSP_PAGE_TABLE_SECTION_SIZE_16MB    4



typedef struct psp_mmu_region_table
   /* This structure is used to hold the MMU page table setup information */
{
   /* Start of the memory region */
   uint32_t START_OF_REGION;

   /* End of the memory region */
   uint32_t REGION_SIZE;

   /* Region page size */
   uint32_t PAGE_SIZE;

   /* Properties of the region */
   uint32_t FLAGS;

} PSP_MMU_REGION_TABLE, * PSP_MMU_REGION_TABLE_PTR;


#ifdef __cplusplus
extern "C" {
#endif

/* Generic PSP prototypes */
_mqx_uint _psp_int_init(_mqx_uint, _mqx_uint);
void _psp_int_install(void);

/* Prototypes of assembler functions */
void _psp_push_fp_context(void);
void _psp_pop_fp_context(void);
extern void __set_mode_SP(uint32_t, uint32_t);

#ifndef __IAR_SYSTEMS_ICC__

#ifndef __get_SP
extern uint32_t         __get_SP(void);
#endif

#ifndef __set_SP
extern void             __set_SP(uint32_t);
#endif

#ifndef __get_CPSR
extern uint32_t         __get_CPSR(void);
#endif

#endif /* __IAR_SYSTEMS_ICC__ */

void _a5_initialize_support(void);
/* MMU functions */
_mqx_uint _mmu_vdisable(void);
_mqx_uint _mmu_venable(void);
_mqx_uint _mmu_add_vregion(void *, void *, _mem_size, _mqx_uint);
_mqx_uint _mmu_vinit(_mqx_uint, void *);

uint32_t _psp_core_num(void);


#ifdef __cplusplus
}
#endif

#endif /* __ASM__ */

#endif /* __cortexa5_h__ */
