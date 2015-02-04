
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains functions for use with the l2c-310 cache controller.
*   This controller can work only with physical adresses.
*
*
*
*END************************************************************************/
#include "mqx_inc.h"
/* Function _mmu_vtop is used for translation from virtual to physical address */
 _mqx_int _mmu_vtop(void *va, void  **pa);

/*!
 * \brief This function enables data cache at any available cache level. Works only 
 *  if MMU is enabled!
 */
void _l2c310_cache_enable(void)
{
    /* this cache is common for Data and Instruction we should avoid invalidating if cache is already turned on*/
    if (!(CA5L2C_reg1_control & L2C_reg1_control_L2_Cache_enable_MASK))
    {
      /* we should invalidate entire cache before enabling :Initializaion sequence in L2C-310 r3p2 Technical Reference Manual*/
        CA5L2C_reg7_inv_way |= L2C_reg7_inv_way_Way_MASK;
        while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);

        CA5L2C_reg1_control |= L2C_reg1_control_L2_Cache_enable_MASK;
        /* Drain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are emptyDrain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are empty */
        while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);
    }
}
/*!
 * \brief This function disables the data cache at any available cache level.
 */
void _l2c310_cache_disable(void)
{
    CA5L2C_reg1_control &= ~L2C_reg1_control_L2_Cache_enable_MASK;
    /* Drain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are emptyDrain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are empty */
    while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);
}

/*!
 * \brief This function invalidates the entire data cache
 */
void _l2c310_cache_invalidate(void)
{
    CA5L2C_reg7_inv_way |= L2C_reg7_inv_way_Way_MASK;
    while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);
}

/*!
 * \brief This function is called to invalidate a line of data cache.
 * 
 * \param addr 
 */
void _l2c310_cache_invalidate_line(void *addr)
{
    /* addr parameter can be virtual address and this cache work only with physical address */
    void   *pa = NULL; /* physical address pointer */
    if (_mmu_vtop(addr, &pa) == MQX_OK)
    {
        CA5L2C_reg7_inv_pa = (uint32_t)pa & (L2C_reg7_clean_pa_Index_MASK | L2C_reg7_clean_pa_Tag_MASK);
        /* Drain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are emptyDrain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are empty */
        while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);
    }
}

/*!
 * \brief  This function is called to invalidate number of lines of data cache. Number 
 *  of lines depends on length parameter and size of line. Size of line for 
 *  A5 L2 cache is 32B.
 * 
 * \param addr 
 * \param length 
 */
void _l2c310_cache_invalidate_mlines(void *addr, _mem_size length)
{
    /* addr parameter can be virtual address and this cache work only with physical address */
    void   *pa = NULL; /* physical address pointer */
    void   *end_addr = (void *)((uint32_t)addr + length);
    addr = (void *) ((uint32_t)addr & ~(PSP_CACHE_LINE_SIZE - 1));    
    do
    {
        if (_mmu_vtop(addr, &pa) == MQX_OK)
        {
            CA5L2C_reg7_inv_pa = (uint32_t)pa & (L2C_reg7_clean_pa_Index_MASK | L2C_reg7_clean_pa_Tag_MASK);
            addr = (void *) ((uint32_t)addr + PSP_CACHE_LINE_SIZE);
            /* Drain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are emptyDrain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are empty */
            while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);
        }
        else
        {
            addr = (void *) ((uint32_t)addr + PSP_CACHE_LINE_SIZE);
        }
    } while (addr < end_addr);
}

/*!
 * \brief This function FLUSH (CLEAN) all lines of cachce (all sets in all ways) Size 
 *  of line for A5 L2 cache is 32B.
 */
void _l2c310_cache_flush(void)
{
    //read count of ways and clean only them
    CA5L2C_reg7_clean_way |= L2C_reg7_clean_way_Way_MASK;
    /* Drain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are emptyDrain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are empty */
    while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);
}

/*!
 * \brief This function FLUSH (CLEAN) line of cachce. Size of line for A5 L2 cache is 32B.
 * 
 * \param addr 
 */
void _l2c310_cache_flush_line(void *addr)
{
    /* addr parameter can be virtual address and this cache work only with physical address */
    void   *pa = NULL; /* physical address pointer */
    if (_mmu_vtop(addr, &pa) == MQX_OK)
    {
        CA5L2C_reg7_clean_pa = (uint32_t)pa & (L2C_reg7_clean_pa_Index_MASK | L2C_reg7_clean_pa_Tag_MASK);
        /* Drain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are emptyDrain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are empty */
        while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);
    }
}

/*!
 * \brief  This function is called to flush number of lines of data cache. Number 
 *  of lines depends on length parameter and size of line. Size of line for 
 *  A5 L2 cache is 32B.
 * 
 * \param addr 
 * \param length 
 */
void _l2c310_cache_flush_mlines(void *addr, _mem_size length)
{
    /* addr parameter can be virtual address and this cache work only with physical address */
    void   *pa = NULL; /* physical address pointer */
    void   *end_addr = (void *)((uint32_t)addr + length);
    addr = (void *) ((uint32_t)addr & ~(PSP_CACHE_LINE_SIZE - 1));    
    do
    {
        if (_mmu_vtop(addr, &pa) == MQX_OK)
        {
            CA5L2C_reg7_clean_pa = (uint32_t)pa & (L2C_reg7_clean_pa_Index_MASK | L2C_reg7_clean_pa_Tag_MASK);
            addr = (void *) ((uint32_t)addr + PSP_CACHE_LINE_SIZE);
            /* Drain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are emptyDrain the STB. Operation complete when all buffers, LRB, LFB, STB, and EB, are empty */
            while (CA5L2C_reg7_cache_sync & L2C_reg7_cache_sync_C_MASK);
        }
        else
        {
            addr = (void *) ((uint32_t)addr + PSP_CACHE_LINE_SIZE);
        }
    } while (addr < end_addr);
}
