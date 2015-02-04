
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
*   This file contains functions for use with the cortex  vybrid caches.
*   Funcionality of L1 cache is accessible on by special instructions mcr - for write and mrc for read.
*
*
*
*END************************************************************************/
#include "mqx_inc.h"

/* vybrid platforms can be consist of L1 core cache and L2 cache (l2c310 cache controller) */
#if PSP_HAS_L1_CACHE
/* PSP A5 Cache prototypes */
void _a5_dcache_enable(void);
void _a5_dcache_disable(void);
void _a5_dcache_flush(void);
void _a5_dcache_flush_line(void *);
void _a5_dcache_flush_mlines(void *, uint32_t);
void _a5_dcache_invalidate(void);
void _a5_dcache_invalidate_line(void *);
void _a5_dcache_invalidate_mlines(void *, uint32_t);

void _a5_icache_enable(void);
void _a5_icache_disable(void);
void _a5_icache_invalidate(void);
void _a5_icache_invalidate_line(void *);
void _a5_icache_invalidate_mlines(void *, uint32_t);
#endif
#if PSP_HAS_L2_CACHE
/* PSP L2C-310 Cache prototypes */
void _l2c310_cache_enable(void);
void _l2c310_cache_disable(void);
void _l2c310_cache_flush(void);
void _l2c310_cache_flush_line(void *);
void _l2c310_cache_flush_mlines(void *, uint32_t);
void _l2c310_cache_invalidate(void);
void _l2c310_cache_invalidate_line(void *);
void _l2c310_cache_invalidate_mlines(void *, uint32_t);

void _l2c310_cache_enable(void);
void _l2c310_cache_disable(void);
void _l2c310_cache_invalidate(void);
void _l2c310_cache_invalidate_line(void *);
void _l2c310_cache_invalidate_mlines(void *, uint32_t);
#endif

/*!
 * \brief This function enables data cache at any available cache level. Works only 
 *  if MMU is enabled!
 */
void _dcache_enable(void)
{
#if PSP_HAS_L1_CACHE
    _a5_dcache_enable();
#endif

#if PSP_HAS_L2_CACHE
    if (MSCM_BASE_PTR->CPxCFG1 & (uint32_t)MSCM_CPxCFG1_L2SZ_MASK)
    {
        _l2c310_cache_enable();
    }
#endif
}

/*!
 * \brief This function disables the data cache at any available cache level.
 */
void _dcache_disable(void)
{
#if PSP_HAS_L1_CACHE
    _a5_dcache_flush();
    _a5_dcache_disable();
#endif

#if PSP_HAS_L2_CACHE
    if (MSCM_BASE_PTR->CPxCFG1 & (uint32_t)MSCM_CPxCFG1_L2SZ_MASK)
    {
        _l2c310_cache_flush();
        _l2c310_cache_disable();
    }
#endif
}

/*!
 * \brief This function invalidates the entire data cache
 */
void _dcache_invalidate(void)
{
#if PSP_HAS_L2_CACHE
    if (MSCM_BASE_PTR->CPxCFG1 & (uint32_t)MSCM_CPxCFG1_L2SZ_MASK)
    {
        _l2c310_cache_invalidate();
    }
#endif

#if PSP_HAS_L1_CACHE
    _a5_dcache_invalidate();
#endif
}

/*!
 * \brief This function is called to invalidate a line of data cache.
 * 
 * \param addr 
 */
void _dcache_invalidate_line(void *addr)
{
#if PSP_HAS_L2_CACHE
    if (MSCM_BASE_PTR->CPxCFG1 & (uint32_t)MSCM_CPxCFG1_L2SZ_MASK)
    {
        _l2c310_cache_invalidate_line(addr);
    }
#endif

#if PSP_HAS_L1_CACHE
    _a5_dcache_invalidate_line(addr);
#endif
}

/*!
 * \brief  This function is called to invalidate number of lines of data cache. Number 
 *  of lines depends on length parameter and size of line. Size of line for 
 *  vybrid L1 cache is 32B.
 * 
 * \param addr 
 * \param length 
 */
void _dcache_invalidate_mlines(void *addr, _mem_size length)
{
#if PSP_HAS_L2_CACHE
    if (MSCM_BASE_PTR->CPxCFG1 & (uint32_t)MSCM_CPxCFG1_L2SZ_MASK)
    {
        _l2c310_cache_invalidate_mlines(addr, length);
    }
#endif

#if PSP_HAS_L1_CACHE
    _a5_dcache_invalidate_mlines(addr, length);
#endif
}

/*!
 * \brief This function FLUSH (CLEAN) all lines of cachce (all sets in all ways) Size 
 *  of line for vybrid L1 cache is 32B.
 */
void _dcache_flush(void)
{
#if PSP_HAS_L1_CACHE
    _a5_dcache_flush();
#endif

#if PSP_HAS_L2_CACHE
    if (MSCM_BASE_PTR->CPxCFG1 & (uint32_t)MSCM_CPxCFG1_L2SZ_MASK)
    {
        _l2c310_cache_flush();
    }
#endif
}

/*!
 * \brief This function FLUSH (CLEAN) line of cachce. Size of line for vybrid L1 cache 
 *  is 32B.
 * 
 * \param addr 
 */
void _dcache_flush_line(void *addr)
{
#if PSP_HAS_L1_CACHE
    _a5_dcache_flush_line(addr);
#endif

#if PSP_HAS_L2_CACHE
    if (MSCM_BASE_PTR->CPxCFG1 & (uint32_t)MSCM_CPxCFG1_L2SZ_MASK)
    {
        _l2c310_cache_flush_line(addr);
    }
#endif
}

/*!
 * \brief  This function is called to flush number of lines of data cache. Number 
 *  of lines depends on length parameter and size of line. Size of line for 
 *  vybrid L1 cache is 32B.
 * 
 * \param addr 
 * \param length 
 */
void _dcache_flush_mlines(void *addr, _mem_size length)
{
#if PSP_HAS_L1_CACHE
    _a5_dcache_flush_mlines(addr, length);
#endif

#if PSP_HAS_L2_CACHE
    if (MSCM_BASE_PTR->CPxCFG1 & (uint32_t)MSCM_CPxCFG1_L2SZ_MASK)
    {
        _l2c310_cache_flush_mlines(addr, length);
    }
#endif
}

/*!
 * \brief This function enables instruction cache at any available cache level. Works 
 *  without enabled MMU too!
 */
void _icache_enable(void)
{
#if PSP_HAS_L1_CACHE
    _a5_icache_enable();
#endif

#if PSP_HAS_L2_CACHE
    if (MSCM_BASE_PTR->CPxCFG1 & (uint32_t)MSCM_CPxCFG1_L2SZ_MASK)
    {
        _l2c310_cache_enable();
    }
#endif
}

/*!
 * \brief This function disables the Invalidate cache at any available cache level.
 */
void _icache_disable(void)
{
#if PSP_HAS_L1_CACHE
    _a5_icache_disable();
#endif
}

/*!
 * \brief This function invalidate entyre of the data cache
 */
void _icache_invalidate(void)
{
#if PSP_HAS_L2_CACHE
    if (MSCM_BASE_PTR->CPxCFG1 & (uint32_t)MSCM_CPxCFG1_L2SZ_MASK)
    {
        _l2c310_cache_invalidate();
    }
#endif

#if PSP_HAS_L1_CACHE
    _a5_icache_invalidate();
#endif
}

/*!
 * \brief This function is called to invalidate a line.
 * 
 * \param addr 
 */
void _icache_invalidate_line(void *addr)
{
#if PSP_HAS_L2_CACHE
    if (MSCM_BASE_PTR->CPxCFG1 & (uint32_t)MSCM_CPxCFG1_L2SZ_MASK)
    {
        _l2c310_cache_invalidate_line(addr);
    }
#endif

#if PSP_HAS_L1_CACHE
    _a5_icache_invalidate_line(addr);
#endif
}

/*!
 * \brief  This function is called to invalidate number of lines of data cache. Number 
 *  of lines depends on length parameter and size of line. Size of line for 
 *  vybrid L1 cache is 32B.
 * 
 * \param addr 
 * \param length 
 */
void _icache_invalidate_mlines(void *addr, _mem_size length)
{
#if PSP_HAS_L2_CACHE
    if (MSCM_BASE_PTR->CPxCFG1 & (uint32_t)MSCM_CPxCFG1_L2SZ_MASK)
    {
        _l2c310_cache_invalidate_mlines(addr, length);
    }
#endif

#if PSP_HAS_L1_CACHE
    _a5_icache_invalidate_mlines(addr, length);
#endif
}
