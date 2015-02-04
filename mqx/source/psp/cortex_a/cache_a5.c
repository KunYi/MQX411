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
*   This file contains functions for use with the cortex  a5 caches.
*   Funcionality of L1 cache is accessible on by special instructions mcr - for write and mrc for read.
*
*
*
*END************************************************************************/
#include "mqx_inc.h"

/*!
 * \brief This function enables data cache at any available cache level. Works only 
 *  if MMU is enabled!
 */
void _a5_dcache_enable(void)
{
    uint32_t sctlr;/* System Control Register */
    /* read sctlr */
    MRC(15, 0, sctlr, 1, 0, 0);

    /* set  C bit (data caching enable) */
    sctlr |= 0x1 << 2;

    /* write modified sctlr*/
    MCR(15, 0, sctlr, 1, 0, 0);
    /* All Cache, Branch predictor and TLB maintenance operations before followed instruction complete*/
    DSB();
}

/*!
 * \brief This function disables the data cache at any available cache level.
 */
void _a5_dcache_disable(void)
{
    uint32_t sctlr;/* System Control Register */
    /* read sctlr */
    MRC(15, 0, sctlr, 1, 0, 0);
    /* set  C bit (data caching enable) */
    sctlr &= ~(0x1 << 2);

    /* write modified sctlr*/
    MCR(15, 0, sctlr, 1, 0, 0);
    /* All Cache, Branch predictor and TLB maintenance operations before followed instruction complete*/
    DSB();
}

/*!
 * \brief This function invalidates the entire data cache
 */
void _a5_dcache_invalidate(void)
{
    uint32_t csid, wayset;  /* Cache Size ID , wayset parameter */
    int32_t num_sets, num_ways; /* number of sets  */

    MCR(15, 2, 0, 0, 0, 0);       	/* Select data cache */
    ISB();							/* sync new cssr, csidr */
    MRC(15, 1, csid, 0, 0, 0);    	/* Read Cache Size ID */
    /* Fill number of sets  and number of ways from csid register  This walues are decremented by 1*/
    num_ways = (csid >> 3) & 0x3FFu;
    /* Invalidation all lines (all Sets in all ways) */
    while (num_ways >= 0)
    {
        num_sets = (csid >> 13) & 0x7FFFu;
        while (num_sets >= 0 )
        {
            wayset = (num_sets << 5u) | (num_ways << 30u); //(num_sets << SETWAY_SET_SHIFT) | (num_sets << 3SETWAY_WAY_SHIFT)
            /* invalidate line if we know set and way */
            MCR(15, 0, wayset, 7, 6, 2);
            num_sets--;
        }
        num_ways--;
    }
    /* All Cache, Branch predictor and TLB maintenance operations before followed instruction complete*/
    DSB();
}

/*!
 * \brief This function is called to invalidate a line of data cache.
 * 
 * \param addr 
 */
void _a5_dcache_invalidate_line(void *addr)
{
    uint32_t va;
    va = (uint32_t) addr & 0xfffffff0; //addr & va_VIRTUAL_ADDRESS_MASK
    /* Invalidate data cache line by va to PoC (Point of Coherency). */
    MCR(15, 0, va, 7, 6, 1);
    /* All Cache, Branch predictor and TLB maintenance operations before followed instruction complete*/
    DSB();
}

/*!
 * \brief  This function is called to invalidate number of lines of data cache. Number 
 *  of lines depends on length parameter and size of line. Size of line for 
 *  A5 L1 cache is 32B.
 * 
 * \param addr 
 * \param length 
 */
void _a5_dcache_invalidate_mlines(void *addr, _mem_size length)
{
    uint32_t va;
    void   *end_addr = (void *)((uint32_t)addr + length);
    addr = (void *) ((uint32_t)addr & ~(PSP_CACHE_LINE_SIZE - 1));    
    do
    {
        /* Clean data cache line to PoC (Point of Coherence) by va. */
        va = (uint32_t) ((uint32_t)addr & 0xfffffff0); //addr & va_VIRTUAL_ADDRESS_MASK
        MCR(15, 0, va, 7, 6, 1);
        /* increment addres to next line and decrement lenght */
        addr = (void *) ((uint32_t)addr + PSP_CACHE_LINE_SIZE);
    } while (addr < end_addr);
    /* All Cache, Branch predictor and TLB maintenance operations before followed instruction complete*/
    DSB();
}

/*!
 * \brief This function FLUSH (CLEAN) all lines of cachce (all sets in all ways) Size 
 *  of line for A5 L1 cache is 32B.
 */
void _a5_dcache_flush(void)
{
    uint32_t csid, wayset;  /* Cache Size ID */
    int32_t num_sets, num_ways; /* number of sets  */

    MCR(15, 2, 0, 0, 0, 0);       	/* Select data cache */
    ISB();							/* sync new cssr, csidr */
    MRC(15, 1, csid, 0, 0, 0);    	/* Read Cache Size ID */
    /* Fill number of sets  and number of ways from csid register  This walues are decremented by 1*/
    num_ways = (csid >> 3) & 0x3FFu;
    while (num_ways >= 0)
    {
        num_sets = (csid >> 13) & 0x7FFFu;
        while (num_sets >= 0 )
        {
            wayset = (num_sets << 5u) | (num_ways << 30u);
            /* FLUSH (clean) line if we know set and way */
            MCR(15, 0, wayset, 7, 10, 2);
            num_sets--;
        }
        num_ways--;
    }
    /* All Cache, Branch predictor and TLB maintenance operations before followed instruction complete*/
    DSB();
}

/*!
 * \brief This function FLUSH (CLEAN) line of cachce. Size of line for A5 L1 cache is 32B.
 * 
 * \param addr 
 */
void _a5_dcache_flush_line(void *addr)
{
    uint32_t va;
    va = (uint32_t) addr & 0xfffffff0; //addr & va_VIRTUAL_ADDRESS_MASK
    /* Clean data cache line to PoC (Point of Coherence) by va. */
    MCR(15, 0, va, 7, 10, 1);
    /* All Cache, Branch predictor and TLB maintenance operations before followed instruction complete*/
    DSB();
}

/*!
 * \brief  This function is called to flush number of lines of data cache. Number 
 *  of lines depends on length parameter and size of line. Size of line for 
 *  A5 L1 cache is 32B.
 * 
 * \param addr 
 * \param length 
 */
void _a5_dcache_flush_mlines(void *addr, _mem_size length)
{
    uint32_t va;
    void   *end_addr = (void *)((uint32_t)addr + length);
    addr = (void *) ((uint32_t)addr & ~(PSP_CACHE_LINE_SIZE - 1));    
    do
    {
        /* Clean data cache line to PoC (Point of Coherence) by va. */
        va = (uint32_t) ((uint32_t)addr & 0xfffffff0); //addr & va_VIRTUAL_ADDRESS_MASK
        MCR(15, 0, va, 7, 10, 1);
        /* increment addres to next line and decrement lenght */
        addr = (void *) ((uint32_t)addr + PSP_CACHE_LINE_SIZE);
    } while (addr < end_addr);
    /* All Cache, Branch predictor and TLB maintenance operations before followed instruction complete*/
    DSB();
}

/*!
 * \brief This function enables instruction cache at any available cache level. Works 
 *  without enabled MMU too!
 */
void _a5_icache_enable(void)
{
    uint32_t sctlr  ;/*System Control Register */
    /* read sctlr */
    MRC(15, 0, sctlr, 1, 0, 0);
    /* set  I bit (instruction caching enable) */
    sctlr |= 0x1 << 12;

    /* write modified sctlr*/
    MCR(15, 0, sctlr, 1, 0, 0);
    /* synchronize context on this processor */
    ISB();
}

/*!
 * \brief This function disables the Invalidate cache at any available cache level.
 */
void _a5_icache_disable(void)
{
    uint32_t sctlr  ;/*System Control Register */
    /* read sctlr */
    MRC(15, 0, sctlr, 1, 0, 0);
    /* Clear  I bit (instruction caching enable) */
    sctlr &= ~(0x1 << 12);

    /* write modified sctlr*/
    MCR(15, 0, sctlr, 1, 0, 0);
    /* synchronize context on this processor */
    ISB();
}

/*!
 * \brief This function invalidate entyre of the data cache
 */
void _a5_icache_invalidate(void)
{
    uint32_t SBZ;
    SBZ = 0x0u;
    MCR(15, 0, SBZ, 7, 5, 0);
    /* synchronize context on this processor */
    ISB();
}

/*!
 * \brief This function is called to invalidate a line.
 * 
 * \param addr 
 */
void _a5_icache_invalidate_line(void *addr)
{
    uint32_t va;
    va = (uint32_t) addr & 0xfffffff0;
    /* Invalidate instruction cache by va to PoU (Point of unification). */
    MCR(15, 0, va, 7, 5, 1);
    /* synchronize context on this processor */
    ISB();
}

/*!
 * \brief  This function is called to invalidate number of lines of data cache. Number 
 *  of lines depends on length parameter and size of line. Size of line for 
 *  A5 L1 cache is 32B.
 * 
 * \param addr 
 * \param length 
 */
void _a5_icache_invalidate_mlines(void *addr, _mem_size length)
{
    uint32_t va;
    void   *end_addr = (void *)((uint32_t)addr + length);
    addr = (void *) ((uint32_t)addr & ~(PSP_CACHE_LINE_SIZE - 1));    
    do
    {
        /* Clean data cache line to PoC (Point of Coherence) by va. */
        va = (uint32_t) ((uint32_t)addr & 0xfffffff0);
        MCR(15, 0, va, 7, 5, 1);
        /* increment addres to next line and decrement lenght */
        addr = (void *) ((uint32_t)addr + PSP_CACHE_LINE_SIZE);
    } while (addr < end_addr);
    /* synchronize context on this processor */
    ISB();
}

