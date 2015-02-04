
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains MMU support functions.
*
*
*END************************************************************************/

#include "mqx_inc.h"
#include "mmu_cortexa5.h"

#define MMU_L1_GET_INDEX(addr)              ((((uint32_t)addr) & MMU_L1_INDEX_MASK) >> MMU_L1_INDEX_SHIFT)
#define MMU_IS_ENTRY_L2_DESCRIPTOR(entry)   ((0x01 << MMU_SECTION_ENTRY_TYPE_SHIFT) == (entry & MMU_SECTION_ENTRY_TYPE_MASK))
#define MMU_L2_GET_INDEX(addr)              ((((uint32_t)addr) & MMU_L2_INDEX_MASK) >> MMU_L2_INDEX_SHIFT)


/*!
 * \brief
 */
static void _mmu_lock()
{
    _int_disable();
}

/*!
 * \brief
 */
static void _mmu_unlock()
{
    _int_enable();
}

/*!
 * \brief Fill line for table entry 16M
 * 
 * \param addr
 * \param flags
 *
 * \return one line from L1 table
 */
uint32_t super_section_line_get(uint32_t addr, uint32_t flags)
{
    uint32_t pageline;
    uint32_t temp;

    if ((flags & PSP_PAGE_TYPE_MASK) == PSP_PAGE_TYPE(PSP_PAGE_TYPE_ENTRY_FAULT))
    {
        /* When  1. and 2. bit is 0 rest bits of entry are ignored */
        return 0;
    }

    pageline = addr  & MMU_SUPER_SECTION_SUPER_SECTION_BASE_ADDR_MASK;
    /* set page type for section base addres 1 mega */
    pageline |= (( 2    << MMU_SUPER_SECTION_ENTRY_TYPE_SHIFT               ) & MMU_SUPER_SECTION_ENTRY_TYPE_MASK               ) |
                (( 1    << MMU_SUPER_SECTION_TABLE_ENTRY_TYPE_A_SHIFT       ) & MMU_SUPER_SECTION_TABLE_ENTRY_TYPE_A_MASK       ) |
                /* Page description is Global */
                (( 0    << MMU_SUPER_SECTION_NG_SHIFT                       ) & MMU_SUPER_SECTION_NG_MASK                       ) |
                /*Page is Not-shared */
                (( 0    << MMU_SUPER_SECTION_S_SHIFT                        ) & MMU_SUPER_SECTION_S_MASK                        ) |
                /* Page Domain is deprecated */
                (( 0    << MMU_SUPER_SECTION_DOMAIN_SHIFT                   ) & MMU_SUPER_SECTION_DOMAIN_MASK                   ) |
                /* Execute Never disabled - this would prevent erroneous execution of data if set */
                (( 0    << MMU_SUPER_SECTION_XN_SHIFT                       ) & MMU_SUPER_SECTION_XN_MASK                       );

    /** Cache type **/
    temp = flags & PSP_PAGE_TYPE_MASK;
    /* write-throuth, no write-allocate */
    if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WTNWA))
    {
        pageline |= (( 0  <<  MMU_SUPER_SECTION_TEX_SHIFT               ) & MMU_SUPER_SECTION_TEX_MASK  ) |
                    (( 1  <<  MMU_SUPER_SECTION_C_SHIFT                 ) & MMU_SUPER_SECTION_C_MASK    ) |
                    (( 0  <<  MMU_SUPER_SECTION_B_SHIFT                 ) & MMU_SUPER_SECTION_B_MASK    );
    }
    /* write-back, no write-allocate */
    else if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WBNWA))
    {
        pageline |= (( 0  <<  MMU_SUPER_SECTION_TEX_SHIFT               ) & MMU_SUPER_SECTION_TEX_MASK  ) |
                    (( 1  <<  MMU_SUPER_SECTION_C_SHIFT                 ) & MMU_SUPER_SECTION_C_MASK    ) |
                    (( 1  <<  MMU_SUPER_SECTION_B_SHIFT                 ) & MMU_SUPER_SECTION_B_MASK    );
    }
    /* non cacheable */
    else if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_NON))
    {
        pageline |= (( 1  <<  MMU_SUPER_SECTION_TEX_SHIFT               ) & MMU_SUPER_SECTION_TEX_MASK  ) |
                    (( 0  <<  MMU_SUPER_SECTION_C_SHIFT                 ) & MMU_SUPER_SECTION_C_MASK    ) |
                    (( 0  <<  MMU_SUPER_SECTION_B_SHIFT                 ) & MMU_SUPER_SECTION_B_MASK    );
    }
    else if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_STRONG_ORDER))
    {
        /* we decided to use strongly ordered memory rule, because we experience that this the only working rule */
        pageline |= (( 0  <<  MMU_SUPER_SECTION_TEX_SHIFT               ) & MMU_SUPER_SECTION_TEX_MASK  ) |
                    (( 0  <<  MMU_SUPER_SECTION_C_SHIFT                 ) & MMU_SUPER_SECTION_C_MASK    ) |
                    (( 0  <<  MMU_SUPER_SECTION_B_SHIFT                 ) & MMU_SUPER_SECTION_B_MASK    );
    }
    /* write-back, write-allocate */
    else if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WBWA))
    {
        pageline |= (( 1  <<  MMU_SUPER_SECTION_TEX_SHIFT               ) & MMU_SUPER_SECTION_TEX_MASK  ) |
                    (( 1  <<  MMU_SUPER_SECTION_C_SHIFT                 ) & MMU_SUPER_SECTION_C_MASK    ) |
                    (( 1  <<  MMU_SUPER_SECTION_B_SHIFT                 ) & MMU_SUPER_SECTION_B_MASK    );
    }

    /** Shareable bit **/
    temp = flags & PSP_PAGE_SHARE_MASK;
    if      (temp == PSP_PAGE_SHARE(PSP_PAGE_SHARE_SHAREABLE))
    {
        pageline |= ((1 << MMU_SECTION_S_SHIFT) & MMU_SECTION_S_MASK);
    }
    else if (temp == PSP_PAGE_SHARE(PSP_PAGE_SHARE_NON_SHAREABLE))
    {
        pageline |= ((0 << MMU_SECTION_S_SHIFT) & MMU_SECTION_S_MASK);
    }

    /** Access controll **/
    temp = flags & PSP_PAGE_DESCR_MASK;
    if (temp == PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RW_ALL))
    {
        pageline |= (( 0  <<  MMU_SUPER_SECTION_APX_SHIFT               ) &  MMU_SUPER_SECTION_APX_MASK ) |
                    (( 3  <<  MMU_SUPER_SECTION_AP_SHIFT                ) &  MMU_SUPER_SECTION_AP_MASK  );
    }
    else if (temp == PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RO_ALL))
    {
        pageline |= (( 1  <<  MMU_SUPER_SECTION_APX_SHIFT               ) &  MMU_SUPER_SECTION_APX_MASK ) |
                    (( 2  <<  MMU_SUPER_SECTION_AP_SHIFT                ) &  MMU_SUPER_SECTION_AP_MASK  );
    }
    else if (temp == PSP_PAGE_DESCR(PSP_PAGE_DESCR_NOACCESS_ALL))
    {
        pageline |= (( 0  <<  MMU_SUPER_SECTION_APX_SHIFT               ) &  MMU_SUPER_SECTION_APX_MASK ) |
                    (( 0  <<  MMU_SUPER_SECTION_AP_SHIFT                ) &  MMU_SUPER_SECTION_AP_MASK  );
    }
    return pageline;
}

/*!
 * \brief Fill line for table entry 1M
 * 
 * \param addr
 * \param flags
 *
 * \return one line from L1 table
 */
static uint32_t section_line_get(uint32_t addr, uint32_t flags)
{
    uint32_t pageline;
    uint32_t temp;

    if ((flags & PSP_PAGE_TYPE_MASK) == PSP_PAGE_TYPE(PSP_PAGE_TYPE_ENTRY_FAULT))
    {
        /* When  1. and 2. bit is 0 rest bits of entry are ignored */
        return 0;
    }

    pageline = addr  & MMU_SECTION_SECTION_BASE_ADDR_MASK;
    /* set page type for section base addres 1 mega */
    pageline |= (( 2    << MMU_SECTION_ENTRY_TYPE_SHIFT         ) & MMU_SECTION_ENTRY_TYPE_MASK         ) |
                (( 0    << MMU_SECTION_TABLE_ENTRY_TYPE_A_SHIFT ) & MMU_SECTION_TABLE_ENTRY_TYPE_A_MASK ) |
                /* Page description is Global */
                (( 0    << MMU_SECTION_NG_SHIFT                 ) & MMU_SECTION_NG_MASK                 ) |
                /*Page is Not-shared */
                (( 0    << MMU_SECTION_S_SHIFT                  ) &  MMU_SECTION_S_MASK                 ) |
                /* Page Domain is deprecated */
                (( 0    << MMU_SECTION_DOMAIN_SHIFT             ) & MMU_SECTION_DOMAIN_MASK             ) |
                /* Execute Never disabled - this would prevent erroneous execution of data if set */
                (( 0    << MMU_SECTION_XN_SHIFT                 ) & MMU_SECTION_XN_MASK                 );

    /** Cache type **/
    temp = flags & PSP_PAGE_TYPE_MASK;
    /* write-throuth, no write-allocate */
    if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WTNWA))
    {
        pageline |= (( 0  <<  MMU_SECTION_TEX_SHIFT               ) & MMU_SECTION_TEX_MASK  ) |
                    (( 1  <<  MMU_SECTION_C_SHIFT                 ) & MMU_SECTION_C_MASK    ) |
                    (( 0  <<  MMU_SECTION_B_SHIFT                 ) & MMU_SECTION_B_MASK    );
    }
    /* write-back, no write-allocate */
    else if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WBNWA))
    {
        pageline |= (( 0  <<  MMU_SECTION_TEX_SHIFT               ) & MMU_SECTION_TEX_MASK  ) |
                    (( 1  <<  MMU_SECTION_C_SHIFT                 ) & MMU_SECTION_C_MASK    ) |
                    (( 1  <<  MMU_SECTION_B_SHIFT                 ) & MMU_SECTION_B_MASK    );
    }
    /* non cacheable */
    /* we decided to use strongly ordered memory rule, because we experience that this the only working rule */
    else if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_NON))
    {
        pageline |= (( 1  <<  MMU_SECTION_TEX_SHIFT               ) & MMU_SECTION_TEX_MASK  ) |
                    (( 0  <<  MMU_SECTION_C_SHIFT                 ) & MMU_SECTION_C_MASK    ) |
                    (( 0  <<  MMU_SECTION_B_SHIFT                 ) & MMU_SECTION_B_MASK    );
    }
    /* strong order */
    else if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_STRONG_ORDER))
    {
        pageline |= (( 0  <<  MMU_SECTION_TEX_SHIFT               ) & MMU_SECTION_TEX_MASK  ) |
                    (( 0  <<  MMU_SECTION_C_SHIFT                 ) & MMU_SECTION_C_MASK    ) |
                    (( 0  <<  MMU_SECTION_B_SHIFT                 ) & MMU_SECTION_B_MASK    );
    }
    /* write-back, write-allocate */
    else if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WBWA))
    {
        pageline |= (( 1  <<  MMU_SECTION_TEX_SHIFT               ) & MMU_SECTION_TEX_MASK  ) |
                    (( 1  <<  MMU_SECTION_C_SHIFT                 ) & MMU_SECTION_C_MASK    ) |
                    (( 1  <<  MMU_SECTION_B_SHIFT                 ) & MMU_SECTION_B_MASK    );
    }

    /** Shareable bit **/
    temp = flags & PSP_PAGE_SHARE_MASK;
    if (temp == PSP_PAGE_SHARE(PSP_PAGE_SHARE_SHAREABLE))
    {
        pageline |= (( 1  <<  MMU_SECTION_S_SHIFT                 ) & MMU_SECTION_S_MASK    );
    }
    else if (temp == PSP_PAGE_SHARE(PSP_PAGE_SHARE_NON_SHAREABLE))
    {
        pageline |= (( 0  <<  MMU_SECTION_S_SHIFT                 ) & MMU_SECTION_S_MASK    );
    }

    /** Access controll **/
    temp = flags & PSP_PAGE_DESCR_MASK;
    if (temp == PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RW_ALL))
    {
        pageline |= (( 0  <<  MMU_SECTION_APX_SHIFT               ) &  MMU_SECTION_APX_MASK ) |
                    (( 3  <<  MMU_SECTION_AP_SHIFT                ) &  MMU_SECTION_AP_MASK  );
    }
    else if (temp == PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RO_ALL))
    {
        pageline |= (( 1  <<  MMU_SECTION_APX_SHIFT               ) &  MMU_SECTION_APX_MASK ) |
                    (( 2  <<  MMU_SECTION_AP_SHIFT                ) &  MMU_SECTION_AP_MASK  );
    }
    else if (temp == PSP_PAGE_DESCR(PSP_PAGE_DESCR_NOACCESS_ALL))
    {
        pageline |= (( 0  <<  MMU_SECTION_APX_SHIFT               ) &  MMU_SECTION_APX_MASK ) |
                    (( 0  <<  MMU_SECTION_AP_SHIFT                ) &  MMU_SECTION_AP_MASK  );
    }
    return pageline;
}

#if MMU_L2_SUPPORT
/*!
 * \brief Fill line for table entry with L2 Descriptor Base Address
 * 
 * \param addr
 *
 * \return one line from L1 table
 */
static uint32_t level2_descriptor_line_get(uint32_t addr)
{
    uint32_t pageline;
    pageline = addr  & MMU_PAGE_TABLE_LEVEL_2_DESC_BASE_ADDR_MASK;
    pageline |= (( 1    << MMU_PAGE_TABLE_ENTRY_TYPE_SHIFT      ) & MMU_PAGE_TABLE_ENTRY_TYPE_MASK      ) |
                /* Page Domain is deprecated */
                (( 0    << MMU_PAGE_TABLE_DOMAIN_SHIFT          ) & MMU_PAGE_TABLE_DOMAIN_MASK          );
    return pageline;
}

/*!
 * \brief Fill line for table entry 64kB
 * 
 * \param addr
 * \param flags
 *
 * \return one line from L2 table
 */
uint32_t large_page_line_get(uint32_t addr, uint32_t flags)
{
    uint32_t pageline;
    uint32_t temp;

    if ((flags & PSP_PAGE_TYPE_MASK) == PSP_PAGE_TYPE(PSP_PAGE_TYPE_ENTRY_FAULT))
    {
        /* When  1. and 2. bit is 0 rest bits of entry are ignored */
        return 0;
    }

    pageline = addr  & MMU_LARGE_PAGE_LARGE_PAGE_BASE_ADDR_MASK;
    /* set page type for section base addres 1 mega */
    pageline |= (( 1    << MMU_LARGE_PAGE_ENTRY_TYPE_SHIFT) & MMU_LARGE_PAGE_ENTRY_TYPE_MASK) |
                /* Page description is Global */
                (( 0    << MMU_LARGE_PAGE_NG_SHIFT        ) & MMU_LARGE_PAGE_NG_MASK        ) |
                /* Page is Not-shared */
                (( 0    << MMU_LARGE_PAGE_S_SHIFT         ) &  MMU_LARGE_PAGE_S_MASK        ) |
                /* Execute Never disabled - this would prevent erroneous execution of data if set */
                (( 0    << MMU_LARGE_PAGE_XN_SHIFT        ) & MMU_LARGE_PAGE_XN_MASK        );

    /** Cache type **/
    temp = flags & PSP_PAGE_TYPE_MASK;
    /* write-throuth, no write-allocate */
    if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WTNWA))
    {
        pageline |= (( 0  <<  MMU_LARGE_PAGE_TEX_SHIFT ) & MMU_LARGE_PAGE_TEX_MASK ) |
                    (( 1  <<  MMU_LARGE_PAGE_C_SHIFT   ) & MMU_LARGE_PAGE_C_MASK   ) |
                    (( 0  <<  MMU_LARGE_PAGE_B_SHIFT   ) & MMU_LARGE_PAGE_B_MASK   );
    }
    /* write-back, no write-allocate */
    else if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WBNWA))
    {
        pageline |= (( 0  <<  MMU_LARGE_PAGE_TEX_SHIFT ) & MMU_LARGE_PAGE_TEX_MASK ) |
                    (( 1  <<  MMU_LARGE_PAGE_C_SHIFT   ) & MMU_LARGE_PAGE_C_MASK   ) |
                    (( 1  <<  MMU_LARGE_PAGE_B_SHIFT   ) & MMU_LARGE_PAGE_B_MASK   );
    }
    /* non cacheable */
    /* we decided to use strongly ordered memory rule, because we experience that this the only working rule */
    else if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_NON))
    {
        pageline |= (( 1  <<  MMU_LARGE_PAGE_TEX_SHIFT ) & MMU_LARGE_PAGE_TEX_MASK ) |
                    (( 0  <<  MMU_LARGE_PAGE_C_SHIFT   ) & MMU_LARGE_PAGE_C_MASK   ) |
                    (( 0  <<  MMU_LARGE_PAGE_B_SHIFT   ) & MMU_LARGE_PAGE_B_MASK   );
    }
    /* strong order */
    else if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_STRONG_ORDER))
    {
        pageline |= (( 0  <<  MMU_LARGE_PAGE_TEX_SHIFT ) & MMU_LARGE_PAGE_TEX_MASK ) |
                    (( 0  <<  MMU_LARGE_PAGE_C_SHIFT   ) & MMU_LARGE_PAGE_C_MASK   ) |
                    (( 0  <<  MMU_LARGE_PAGE_B_SHIFT   ) & MMU_LARGE_PAGE_B_MASK   );
    }
    /* write-back, write-allocate */
    else if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WBWA))
    {
        pageline |= (( 1  <<  MMU_LARGE_PAGE_TEX_SHIFT ) & MMU_LARGE_PAGE_TEX_MASK ) |
                    (( 1  <<  MMU_LARGE_PAGE_C_SHIFT   ) & MMU_LARGE_PAGE_C_MASK   ) |
                    (( 1  <<  MMU_LARGE_PAGE_B_SHIFT   ) & MMU_LARGE_PAGE_B_MASK   );
    }


    /** Shareable bit **/
    temp = flags & PSP_PAGE_SHARE_MASK;
    if (temp == PSP_PAGE_SHARE(PSP_PAGE_SHARE_SHAREABLE))
    {
        pageline |= (( 1  <<  MMU_LARGE_PAGE_S_SHIFT   ) & MMU_LARGE_PAGE_S_MASK   );
    }
    else if (temp == PSP_PAGE_SHARE(PSP_PAGE_SHARE_NON_SHAREABLE))
    {
        pageline |= (( 0  <<  MMU_LARGE_PAGE_S_SHIFT   ) & MMU_LARGE_PAGE_S_MASK    );
    }
    /** Access controll **/
    temp = flags & PSP_PAGE_DESCR_MASK;
    if (temp == PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RW_ALL))
    {
        pageline |= (( 0  <<  MMU_LARGE_PAGE_APX_SHIFT ) &  MMU_LARGE_PAGE_APX_MASK ) |
                    (( 3  <<  MMU_LARGE_PAGE_AP_SHIFT  ) &  MMU_LARGE_PAGE_AP_MASK  );
    }
    else if (temp == PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RO_ALL))
    {
        pageline |= (( 1  <<  MMU_LARGE_PAGE_APX_SHIFT ) &  MMU_LARGE_PAGE_APX_MASK ) |
                    (( 2  <<  MMU_LARGE_PAGE_AP_SHIFT  ) &  MMU_LARGE_PAGE_AP_MASK  );
    }
    else if (temp == PSP_PAGE_DESCR(PSP_PAGE_DESCR_NOACCESS_ALL))
    {
        pageline |= (( 0  <<  MMU_LARGE_PAGE_APX_SHIFT ) &  MMU_LARGE_PAGE_APX_MASK ) |
                    (( 0  <<  MMU_LARGE_PAGE_AP_SHIFT  ) &  MMU_LARGE_PAGE_AP_MASK  );
    }
    return pageline;
}

/*!
 * \brief Fill line for table entry 4kB
 * 
 * \param addr
 * \param flags
 *
 * \return one line from L2 table
 */
static uint32_t small_page_line_get(uint32_t addr, uint32_t flags)
{
     uint32_t pageline;
     uint32_t temp;

    if ((flags & PSP_PAGE_TYPE_MASK) == PSP_PAGE_TYPE(PSP_PAGE_TYPE_ENTRY_FAULT))
    {
        /* When  1. and 2. bit is 0 rest bits of entry are ignored */
        return 0;
    }

    pageline = addr  & MMU_SMALL_PAGE_SMALL_PAGE_BASE_ADDR_MASK;
    /* set page type for section base addres 1 mega */
    pageline |= (( 1    << MMU_SMALL_PAGE_ENTRY_TYPE_SHIFT) & MMU_SMALL_PAGE_ENTRY_TYPE_MASK) |
                /* Page description is Global */
                (( 0    << MMU_SMALL_PAGE_NG_SHIFT        ) & MMU_SMALL_PAGE_NG_MASK        ) |
                /* Page is Not-shared */
                (( 0    << MMU_SMALL_PAGE_S_SHIFT         ) &  MMU_SMALL_PAGE_S_MASK        ) |
                /* Execute Never disabled - this would prevent erroneous execution of data if set */
                (( 0    << MMU_SMALL_PAGE_XN_SHIFT        ) & MMU_SMALL_PAGE_XN_MASK        );

    /** Cache type **/
    temp = flags & PSP_PAGE_TYPE_MASK;
    /* write-throuth, no write-allocate */
    if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WTNWA))
    {
        pageline |= (( 0  <<  MMU_SMALL_PAGE_TEX_SHIFT ) & MMU_SMALL_PAGE_TEX_MASK ) |
                    (( 1  <<  MMU_SMALL_PAGE_C_SHIFT   ) & MMU_SMALL_PAGE_C_MASK   ) |
                    (( 0  <<  MMU_SMALL_PAGE_B_SHIFT   ) & MMU_SMALL_PAGE_B_MASK   );
    }
    /* write-back, no write-allocate */
    else if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WBNWA))
    {
        pageline |= (( 0  <<  MMU_SMALL_PAGE_TEX_SHIFT ) & MMU_SMALL_PAGE_TEX_MASK ) |
                    (( 1  <<  MMU_SMALL_PAGE_C_SHIFT   ) & MMU_SMALL_PAGE_C_MASK   ) |
                    (( 1  <<  MMU_SMALL_PAGE_B_SHIFT   ) & MMU_SMALL_PAGE_B_MASK   );
    }
    /* non cacheable */
    /* we decided to use strongly ordered memory rule, because we experience that this the only working rule */
    else if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_NON))
    {
        pageline |= (( 1  <<  MMU_SMALL_PAGE_TEX_SHIFT ) & MMU_SMALL_PAGE_TEX_MASK ) |
                    (( 0  <<  MMU_SMALL_PAGE_C_SHIFT   ) & MMU_SMALL_PAGE_C_MASK   ) |
                    (( 0  <<  MMU_SMALL_PAGE_B_SHIFT   ) & MMU_SMALL_PAGE_B_MASK   );
    }
    /* strong order */
    else if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_STRONG_ORDER))
    {
        pageline |= (( 0  <<  MMU_SMALL_PAGE_TEX_SHIFT ) & MMU_SMALL_PAGE_TEX_MASK ) |
                    (( 0  <<  MMU_SMALL_PAGE_C_SHIFT   ) & MMU_SMALL_PAGE_C_MASK   ) |
                    (( 0  <<  MMU_SMALL_PAGE_B_SHIFT   ) & MMU_SMALL_PAGE_B_MASK   );
    }
    /* write-back, write-allocate */
    else if (temp == PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WBWA))
    {
        pageline |= (( 1  <<  MMU_SMALL_PAGE_TEX_SHIFT ) & MMU_SMALL_PAGE_TEX_MASK ) |
                    (( 1  <<  MMU_SMALL_PAGE_C_SHIFT   ) & MMU_SMALL_PAGE_C_MASK   ) |
                    (( 1  <<  MMU_SMALL_PAGE_B_SHIFT   ) & MMU_SMALL_PAGE_B_MASK   );
    }

    /** Shareable bit **/
    temp = flags & PSP_PAGE_SHARE_MASK;
    if (temp == PSP_PAGE_SHARE(PSP_PAGE_SHARE_SHAREABLE))
    {
        pageline |= (( 1  <<  MMU_SMALL_PAGE_S_SHIFT   ) & MMU_SMALL_PAGE_S_MASK   );
    }
    else if (temp == PSP_PAGE_SHARE(PSP_PAGE_SHARE_NON_SHAREABLE))
    {
        pageline |= (( 0  <<  MMU_SMALL_PAGE_S_SHIFT   ) & MMU_SMALL_PAGE_S_MASK    );
    }

    /** Access controll **/
    temp = (flags & PSP_PAGE_DESCR_MASK);
    if (temp == PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RW_ALL))
    {
        pageline |= (( 0  <<  MMU_SMALL_PAGE_APX_SHIFT ) &  MMU_SMALL_PAGE_APX_MASK ) |
                    (( 3  <<  MMU_SMALL_PAGE_AP_SHIFT  ) &  MMU_SMALL_PAGE_AP_MASK  );
    }
    else if (temp == PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RO_ALL))
    {
        pageline |= (( 1  <<  MMU_SMALL_PAGE_APX_SHIFT ) &  MMU_SMALL_PAGE_APX_MASK ) |
                    (( 2  <<  MMU_SMALL_PAGE_AP_SHIFT  ) &  MMU_SMALL_PAGE_AP_MASK  );
    }
    else if (temp == PSP_PAGE_DESCR(PSP_PAGE_DESCR_NOACCESS_ALL))
    {
        pageline |= (( 0  <<  MMU_SMALL_PAGE_APX_SHIFT ) &  MMU_SMALL_PAGE_APX_MASK ) |
                    (( 0  <<  MMU_SMALL_PAGE_AP_SHIFT  ) &  MMU_SMALL_PAGE_AP_MASK  );
    }
    return pageline;
}
#endif
/*!
 * \brief This function return flags from L1 page entry
 * 
 * \param entry
 *
 * \return flags from l1 page entry
 */
static uint32_t get_flags_from_l1(uint32_t entry)
{
    uint32_t result = 0;
    /** Fault page entry **/
    if (( 0 << MMU_SECTION_ENTRY_TYPE_SHIFT) == (entry & MMU_SECTION_ENTRY_TYPE_MASK )) //if 1. and 2. bit is zero
    {
        /* If flag PSP_PAGE_TYPE_ENTRY_FAULT is rest of entry is ignored */
        return PSP_PAGE_TYPE(PSP_PAGE_TYPE_ENTRY_FAULT);
    }
    /** Cache type **/
    /* write-throuth, no write-allocate */
    if ((( 0 << MMU_SECTION_TEX_SHIFT) == (entry & MMU_SECTION_TEX_MASK )) &&
        (( 1 << MMU_SECTION_C_SHIFT  ) == (entry & MMU_SECTION_C_MASK   )) &&
        (( 0 << MMU_SECTION_B_SHIFT  ) == (entry & MMU_SECTION_B_MASK   )))
    {
        result |= PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WTNWA);
    }
    /* write-back, no write-allocate */
    else if ((( 0  <<  MMU_SECTION_TEX_SHIFT) == (entry & MMU_SECTION_TEX_MASK )) &&
             (( 1  <<  MMU_SECTION_C_SHIFT  ) == (entry & MMU_SECTION_C_MASK   )) &&
             (( 1  <<  MMU_SECTION_B_SHIFT  ) == (entry & MMU_SECTION_B_MASK   )))
    {
        result |= PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WBNWA);
    }
    /* non cacheable */
    else if ((( 1  <<  MMU_SECTION_TEX_SHIFT ) == (entry & MMU_SECTION_TEX_MASK )) &&
             (( 0  <<  MMU_SECTION_C_SHIFT   ) == (entry & MMU_SECTION_C_MASK   )) &&
             (( 0  <<  MMU_SECTION_B_SHIFT   ) == (entry & MMU_SECTION_B_MASK   )))
    {
        result |= PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_NON);
    }
    /* strong order */
    else if ((( 0  <<  MMU_SECTION_TEX_SHIFT ) == (entry & MMU_SECTION_TEX_MASK )) &&
             (( 0  <<  MMU_SECTION_C_SHIFT   ) == (entry & MMU_SECTION_C_MASK   )) &&
             (( 0  <<  MMU_SECTION_B_SHIFT   ) == (entry & MMU_SECTION_B_MASK   )))
    {
        result |= PSP_PAGE_TYPE(PSP_PAGE_TYPE_STRONG_ORDER);
    }
    /* write-back, write-allocate */
    else if ((( 1  <<  MMU_SECTION_TEX_SHIFT ) == (entry & MMU_SECTION_TEX_MASK )) &&
             (( 1  <<  MMU_SECTION_C_SHIFT   ) == (entry & MMU_SECTION_C_MASK   )) &&
             (( 1  <<  MMU_SECTION_B_SHIFT   ) == (entry & MMU_SECTION_B_MASK   )))
    {
        result |= PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WBWA);
    }

    /** Shareable bit **/
    if (( 1  <<  MMU_SECTION_S_SHIFT) == (entry & MMU_SECTION_S_MASK ))
    {
        result |= PSP_PAGE_SHARE(PSP_PAGE_SHARE_SHAREABLE);
    }
    else// if (( 0  <<  MMU_SECTION_S_SHIFT) == (entry & MMU_SECTION_S_MASK ))
    {
        result |= PSP_PAGE_SHARE(PSP_PAGE_SHARE_NON_SHAREABLE);
    }

    /** Access controll **/
    if ((( 0  <<  MMU_SECTION_APX_SHIFT) == (entry & MMU_SECTION_APX_MASK )) &&
        (( 3  <<  MMU_SECTION_AP_SHIFT ) == (entry & MMU_SECTION_AP_MASK  )))
    {
        result |= PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RW_ALL);
    }
    else if((( 0  <<  MMU_SECTION_APX_SHIFT) == (entry & MMU_SECTION_APX_MASK )) &&
            (( 0  <<  MMU_SECTION_AP_SHIFT ) == (entry & MMU_SECTION_AP_MASK  )))
    {
        result |= PSP_PAGE_DESCR(PSP_PAGE_DESCR_NOACCESS_ALL);
    }
    else if((( 1  <<  MMU_SECTION_APX_SHIFT) == (entry & MMU_SECTION_APX_MASK )) &&
            (( 2  <<  MMU_SECTION_AP_SHIFT ) == (entry & MMU_SECTION_AP_MASK  )))
    {
        result |= PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RO_ALL);
    }

    return result;
}
/*!
 * \brief Initialize the mmu
 * 
 * \param[in] default_attribute initialization flags
 * \param[in] base_ptr initialization info
 *
 * \return MQX_OK or an error code
 */
_mqx_uint _mmu_vinit
   (
      /* [IN] initialization flags */
      _mqx_uint default_attribute,

      /* [IN] initialization info */
      void     *base_ptr
   )
{
    uint32_t addr = (uint32_t)base_ptr;
    PSP_SUPPORT_STRUCT_PTR psp_support_ptr = _psp_get_support_ptr();

    _mmu_lock();
    #if MQX_CHECK_ERRORS
    if (NULL == base_ptr)
    {
        _mmu_unlock();
        _task_set_error(MQX_MMU_ERROR);
        return MQX_INVALID_PARAMETER;
    }

    if (psp_support_ptr == NULL)
    {
        _mmu_unlock();
        _task_set_error(MQX_MMU_ERROR);
        return(MQX_COMPONENT_DOES_NOT_EXIST);
    }
    /*
    ** We need to set the translation table base register. This register
    ** must be aligned on a 16 Kbyte boundary.
    */
    if (addr & ~MMU_TRANSLATION_TABLE_ALIGN_MASK)
    {
        _mmu_unlock();
        _task_set_error(MQX_MMU_ERROR);
        return MQX_INVALID_PARAMETER;
    }
    #endif
    psp_support_ptr->PAGE_TABLE_BASE = addr;
    psp_support_ptr->PAGE_TABLE_USED = MMU_TRANSLATION_TABLE_SIZE;
    psp_support_ptr->MMU_DEFAULT_ATTRIBUTE = default_attribute;

#if MMU_L2_SUPPORT
    /* If L2 support is enabled we have to clear l1 table, because _mmu_add_vregion try to free L2 table if L1 table entry point to L2 */
    _mem_zero(base_ptr, MMU_TRANSLATION_TABLE_SIZE);
#endif
    /* A 1MB section translation entry */
    if ((default_attribute & PSP_PAGE_TABLE_SECTION_SIZE_MASK) == PSP_PAGE_TABLE_SECTION_SIZE(PSP_PAGE_TABLE_SECTION_SIZE_1MB))
    {
        /* _mmu_add_vregion() is called twice because entire address space is 64b number (0x100000000)*/
        _mmu_add_vregion((void *)0, (void *)0, 0x80000000, default_attribute);
        _mmu_add_vregion((void *)0x80000000, (void *)0x80000000, 0x80000000, default_attribute);
    }
    else
    {
        /* unsupported init section size */
        _mmu_unlock();
        _task_set_error(MQX_MMU_ERROR);
        return MQX_INVALID_PARAMETER;
    }
    _mmu_unlock();
    return MQX_OK;
}

/*!
 * \brief Enable the operation of the mmu
 *
 * \return MQX_OK or an error code
 */
_mqx_uint _mmu_venable(void)
{
    uint32_t SCTLR;/*System Control Register */
    uint32_t DACR;
    PSP_SUPPORT_STRUCT_PTR psp_support_ptr = _psp_get_support_ptr();

#if MQX_CHECK_ERRORS
    if ((void *)psp_support_ptr->PAGE_TABLE_BASE == NULL)
    {
        _task_set_error(MQX_MMU_ERROR);
        return MQX_COMPONENT_DOES_NOT_EXIST;
    }
#endif
    _mmu_lock();
    /* write table address to TTBR0*/
    MCR(15, 0, (uint32_t)psp_support_ptr->PAGE_TABLE_BASE, 2, 0, 0);

    //DACR = 0xFFFFFFFF; //All inclusive acces
    DACR = 0x55555555; /* set Client mode for all Domains */

    /* write modified DACR*/
    MCR(15, 0, DACR, 3, 0, 0);
    // MCR p15, 0, <Rd>, c3, c0, 0 ; Write DACR

    /* invalidate all tlb */
    MCR(15, 0, 0, 8, 7, 0);
    /*   MCR p15, 0, <Rd>, c8, c7, 0 -- Invalidate entire Unified TLB : TLBIALL*/

    /* read SCTLR */
    MRC(15, 0, SCTLR, 1, 0, 0);
    /* set MMU enable bit */
    SCTLR = SCTLR | 0x1u;

    /* write modified SCTLR*/
    MCR(15, 0, SCTLR, 1, 0, 0);
    _mmu_unlock();
    return MQX_OK;
}

/*!
 * \brief Disable the operation of the mmu
 *
 * \return MQX_OK or an error code
 */
_mqx_uint _mmu_vdisable(void)
{
    uint32_t SCTLR;/*System Control Register */

#if MMU_L2_SUPPORT
    PSP_SUPPORT_STRUCT_PTR psp_support_ptr = _psp_get_support_ptr();
    uint32_t *l1_table = (uint32_t *)psp_support_ptr ->PAGE_TABLE_BASE;
    uint32_t i;
 #if MQX_CHECK_ERRORS
    if ((void *)psp_support_ptr->PAGE_TABLE_BASE == NULL)
    {
        _task_set_error(MQX_MMU_ERROR);
        return MQX_COMPONENT_DOES_NOT_EXIST;
    }
#endif
#endif
    _mmu_lock();
    /* read SCTLR */
    MRC(15, 0, SCTLR, 1, 0, 0);
    /* clear MMU enable bit */
    SCTLR &=~ 0x01u;

    /* write modified SCTLR*/
    MCR(15, 0, SCTLR, 1, 0, 0);

#if MMU_L2_SUPPORT
    /* un-allocate all L2 pages */
    for (i = 0; i < MMU_TRANSLATION_TABLE_ENTRIES; i++)
    {
        if (MMU_IS_ENTRY_L2_DESCRIPTOR(l1_table[i])) // If entry in L1 table is L2 descriptor
        {
            /* Change entry */
            _mem_free((void *)(l1_table[i] & MMU_PAGE_TABLE_LEVEL_2_DESC_BASE_ADDR_MASK));
        }
    }
#endif
    psp_support_ptr->PAGE_TABLE_BASE = (uint32_t)NULL;
    _mmu_unlock();
    return MQX_OK;
}

/*!
 * \brief Initialize the mmu
 * 
 * \param[in] paddr physical address of region
 * \param[in] vaddr virtual address of region
 * \param[in] size size of region
 * \param[in] flags flags for region
 *
 * \return MQX_OK or an error code
 */
_mqx_uint _mmu_add_vregion
   (
        /* [IN] physical address of region */
        void   *paddr,
        /* [IN] virtual address of region */
        void   *vaddr,
        /* [IN] size of region */
        _mem_size size,
        /* [IN] flags for region */
        _mqx_uint flags
   )
{
    _mqx_int i;
    PSP_SUPPORT_STRUCT_PTR   psp_support_ptr = _psp_get_support_ptr();
    /* Index with in 1MB for L1 table */
    uint32_t pindex = ((uint32_t)paddr >> MMU_L1_INDEX_SHIFT);
    uint32_t vindex = ((uint32_t)vaddr >> MMU_L1_INDEX_SHIFT);
    uint32_t *l1_table = (uint32_t*)psp_support_ptr ->PAGE_TABLE_BASE;

#if MMU_L2_SUPPORT
    uint32_t vindex_l2;
    uint32_t pindex_l2;
    uint32_t *l2_table;
    uint32_t temp_flag, temp_paddr, temp_entry;
    uint32_t temp;
#endif

 #if MQX_CHECK_ERRORS
    if ((void *)psp_support_ptr->PAGE_TABLE_BASE == NULL)
    {
        _task_set_error(MQX_MMU_ERROR);
        return MQX_COMPONENT_DOES_NOT_EXIST;
    }
#endif
    _mmu_lock();
    /* A 1MB section translation entry */
    if ((flags & PSP_PAGE_TABLE_SECTION_SIZE_MASK) == PSP_PAGE_TABLE_SECTION_SIZE(PSP_PAGE_TABLE_SECTION_SIZE_1MB))
    {
        for (i = (size >> MMU_SECTION_SECTION_BASE_ADDR_SHIFT); ((i > 0) && (vindex < MMU_TRANSLATION_TABLE_ENTRIES)); pindex++, vindex++, i-- )
        {
#if MMU_L2_SUPPORT
            /* store old entry */
            temp_entry = l1_table[vindex];
#endif
            /* Add L1 entry */
            l1_table[vindex] = section_line_get(pindex << MMU_SECTION_SECTION_BASE_ADDR_SHIFT, flags);
#if MMU_L2_SUPPORT
            /* If entry is L2 desriptor than need to be de-alocated */
            if (MMU_IS_ENTRY_L2_DESCRIPTOR(temp_entry))
            {
                /* Flush data cache */
                _DCACHE_FLUSH_LINE(&l1_table[vindex]);
                /* Invalidate all TLB entries in case when tlb still point to L2 table. Invalidate TLB entries by VA All ASID */
               /// MCR(15, 0, (vindex << MMU_L1_INDEX_SHIFT), 8, 7, 3);///
                MCR(15, 0, 0, 8, 7, 0);
                /* Free L2 table */
                DSB(); // Ensure completion of TLB invalidation
                ISB(); //synchronise context on this processor
                _mem_free((void *)(temp_entry & MMU_PAGE_TABLE_LEVEL_2_DESC_BASE_ADDR_MASK));
            }
#endif
        }
    }

#if MMU_L2_SUPPORT
    /* A 4kB section in L2 table */
    else if ((flags & PSP_PAGE_TABLE_SECTION_SIZE_MASK) == PSP_PAGE_TABLE_SECTION_SIZE(PSP_PAGE_TABLE_SECTION_SIZE_4KB))
    {
        vindex_l2 = (((uint32_t)vaddr & ~(MMU_L1_INDEX_MASK)) >> MMU_L2_INDEX_SHIFT);
        pindex_l2 = (((uint32_t)paddr & ~(MMU_L1_INDEX_MASK)) >> MMU_L2_INDEX_SHIFT);
        temp = ((size >> MMU_L2_INDEX_SHIFT) + vindex_l2);

        for ( ; ((vindex_l2 < temp) && (vindex < MMU_TRANSLATION_TABLE_ENTRIES)); vindex_l2++, pindex_l2++)
        {
            /* update vindex */
            vindex = (((uint32_t)vaddr & (MMU_L1_INDEX_MASK)) + (vindex_l2 << MMU_L2_INDEX_SHIFT)) >> MMU_L1_INDEX_SHIFT;
            /* Check if l2 table is already created */
            if (MMU_IS_ENTRY_L2_DESCRIPTOR(l1_table[vindex])) //vindex is aligned to 1MB
            {
                /* Get L2 table base addr from entry */
                l2_table = (uint32_t*) (l1_table[vindex] & MMU_PAGE_TABLE_LEVEL_2_DESC_BASE_ADDR_MASK);
                /* Fill entry in L2 table*/
                l2_table[vindex_l2 % MMU_L2_TABLE_ENTRIES] = small_page_line_get((((uint32_t)paddr & (MMU_L1_INDEX_MASK)) + (pindex_l2 << MMU_SMALL_PAGE_SMALL_PAGE_BASE_ADDR_SHIFT)), flags);
                /* Flush data cache line with changed L2 table entry */
                _DCACHE_FLUSH_LINE(&l2_table[vindex_l2 % MMU_L2_TABLE_ENTRIES]);
            }
            else
            {
                /* Store old flags for 1MB entries */
                temp_flag = get_flags_from_l1(l1_table[vindex]);
                /* Store old paddr */
                temp_paddr = l1_table[vindex] & MMU_L1_INDEX_MASK;

                /* Alloc aligned L2 page*/
                l2_table = _mem_alloc_system_align(MMU_L2_TABLE_SIZE, MMU_L2_TABLE_ALIGN);
                if (NULL == l2_table)
                {
                    _mmu_unlock();
                    _task_set_error(MQX_MMU_ERROR);
                    return MQX_OUT_OF_MEMORY;
                }
                /* Set all entries in L2 table with flags from previous L1 entry */
                for (i = 0; i < MMU_L2_TABLE_ENTRIES; i++)
                {
                    l2_table[i] = small_page_line_get(temp_paddr | (i << MMU_L2_INDEX_SHIFT), temp_flag);
                }
                /* Set required L2 entry */
                l2_table[vindex_l2 % MMU_L2_TABLE_ENTRIES] = small_page_line_get((((uint32_t)paddr & (MMU_L1_INDEX_MASK)) + (pindex_l2 << MMU_SMALL_PAGE_SMALL_PAGE_BASE_ADDR_SHIFT)), flags);

                /* Flush data cache in case L2 is in cached area */
                _DCACHE_FLUSH_MLINES(l2_table, MMU_L2_TABLE_SIZE);

                /* Change entry in L1 table */
                l1_table[vindex] = level2_descriptor_line_get((uint32_t)l2_table);
            }
        }
    }
#endif
    else
    {
        _mmu_unlock();
        _task_set_error(MQX_MMU_ERROR);
         return MQX_INVALID_PARAMETER;
    }
    _DCACHE_FLUSH_MLINES(&l1_table[((uint32_t)vaddr >> MMU_L1_INDEX_SHIFT)], (size >> MMU_L1_INDEX_SHIFT) * (sizeof(uint32_t)));
    /* invalidate all tlb */
    MCR(15, 0, 0, 8, 7, 0);
    /*   MCR p15, 0, <Rd>, c8, c7, 0 -- Invalidate entire Unified TLB : TLBIALL*/
    DSB(); // Ensure completion of TLB invalidation
    ISB(); //synchronise context on this processor
    _mmu_unlock();
    return(MQX_OK);
}


/*!
 * \brief translate virtual address to physical address
 *
 * \param va
 * \param pa
 *
 * \return MQX_OK or an error code
 */
 _mqx_int _mmu_vtop(void *va, void  **pa)
{
    /* VA to PA translation with privileged read permission check  */
    MCR(15, 0, (uint32_t)va & 0xfffffc00, 7, 8, 0);
    /* Read PA register */
    MRC(15, 0, *(uint32_t*)pa, 7, 4, 0);
    /* First bit of returned value is Result of conversion(0 is successful translation) */
    if ((uint32_t)*pa & 0x01)
    {
        /* We can try write permission also */
        /* VA to PA translation with privileged write permission check  */
        MCR(15, 0, (uint32_t)va & 0xfffffc00, 7, 8, 1);
        /* Read PA register */
        MRC(15, 0, *(uint32_t*)pa, 7, 4, 0);
        /* First bit of returned value is Result of conversion(0 is successful translation) */
        if ((uint32_t)*pa & 0x01)
        {
            return MQX_INVALID_POINTER;
        }
    }
    /* complete address returning base + offset*/
    *pa = (void *) (((uint32_t)*pa & 0xfffff000) | ((uint32_t)va & 0x00000fff));
    return MQX_OK;
}
