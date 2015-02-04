
/*HEADER**********************************************************************
*
* Copyright 2008-2014 Freescale Semiconductor, Inc.
* Copyright 2004-2010 Embedded Access Inc.
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains functions for use with the cortex caches.
*
*
*END************************************************************************/
#include "mqx_inc.h"

/*!
 * \brief This function enables the data cache
 */
void _dcache_enable()
{
    /* set command to invalidate all ways, enable write buffer */
    LMEM_PSCCR = (LMEM_PSCCR_INVW1_MASK | LMEM_PSCCR_INVW0_MASK | LMEM_PSCCR_ENWRBUF_MASK);
    /* Write GO bit to initiate command */
    LMEM_PSCCR = (LMEM_PSCCR_GO_MASK | LMEM_PSCCR_INVW1_MASK | LMEM_PSCCR_INVW0_MASK | LMEM_PSCCR_ENWRBUF_MASK);
    /* wait until the command completes */
    while (LMEM_PSCCR & LMEM_PSCCR_GO_MASK)
    {}
    /* Enable cache, enable write buffer */
    LMEM_PSCCR = (LMEM_PSCCR_ENWRBUF_MASK | LMEM_PSCCR_ENCACHE_MASK);
}

/*!
 * \brief This function disables the data cache
 */
void _dcache_disable()
{
    LMEM_PSCCR = 0x0;
}

/*!
 * \brief This function invalidates the entire data cache
 */
void _dcache_invalidate()
{
    LMEM_PSCCR |= LMEM_PSCCR_INVW0_MASK | LMEM_PSCCR_INVW1_MASK;
    LMEM_PSCCR |= LMEM_PSCCR_GO_MASK;
    /* wait until the command completes */
    while (LMEM_PSCCR & LMEM_PSCCR_GO_MASK)
    {}
}

/*!
 * \brief This function is called to invalidate a line out of the data cache.
 * 
 * \param void *addr 
 */
void _dcache_invalidate_line(void *addr)
{
    /* Invalidate by physical address */
    LMEM_PSCLCR = LMEM_PSCLCR_LADSEL_MASK | (0x1u << LMEM_PSCLCR_LCMD_SHIFT);
    /* Set physical address and activate command */
    LMEM_PSCSAR = ((uint32_t)addr & ~0x03u) | LMEM_PSCSAR_LGO_MASK;
    /* wait until the command completes */
    while (LMEM_PSCSAR & LMEM_PSCSAR_LGO_MASK)
    {}
}

/*!
 * \brief  This function is called to Invalidate the data cache.
 * 
 * \param addr 
 * \param length 
 */
void _dcache_invalidate_mlines(void *addr, _mem_size length)
{
    void   *end_addr = (void *)((uint32_t)addr + length);
    addr = (void *) ((uint32_t)addr & ~(PSP_CACHE_LINE_SIZE - 1));
    do
    {
        _dcache_invalidate_line(addr);
        addr = (void *) ((uint32_t)addr + PSP_CACHE_LINE_SIZE);
    } while (addr < end_addr);
}

/*!
 * \brief This function enables the data cache
 */
void _dcache_flush()
{
    LMEM_PSCCR |= LMEM_PSCCR_PUSHW0_MASK | LMEM_PSCCR_PUSHW1_MASK ;
    LMEM_PSCCR |= LMEM_PSCCR_GO_MASK;
    /* wait until the command completes */
    while (LMEM_PSCCR & LMEM_PSCCR_GO_MASK)
    {}
}

/*!
 * \brief This function is called to push (flush and invalidate) a line out of the 
 *  data cache.
 * 
 * \param addr 
 */
void _dcache_flush_line(void *addr)
{
    /* Invalidate by physical address */
    LMEM_PSCLCR = LMEM_PSCLCR_LADSEL_MASK | (0x2u << LMEM_PSCLCR_LCMD_SHIFT);
    /* Set physical address and activate command */
    LMEM_PSCSAR = ((uint32_t)addr & ~0x03u) | LMEM_PSCSAR_LGO_MASK;
    /* wait until the command completes */
    while (LMEM_PSCSAR & LMEM_PSCSAR_LGO_MASK)
    {}
}

/*!
 * \brief  This function is called to flush the data cache by performing cache copybacks. 
 *  It must determine how many cache lines need to be copied back and then 
 *  perform the copybacks.
 * 
 * \param addr 
 * \param length 
 */
void _dcache_flush_mlines(void *addr, _mem_size length)
{
    void   *end_addr = (void *)((uint32_t)addr + length);
    addr = (void *) ((uint32_t)addr & ~(PSP_CACHE_LINE_SIZE - 1));
    do
    {
        _dcache_flush_line(addr);
        addr = (void *) ((uint32_t)addr + PSP_CACHE_LINE_SIZE);
    } while (addr < end_addr);
}

/*!
 * \brief This function enables the data cache
 */
void _icache_enable()
{
    /* set command to invalidate all ways, enable write buffer */
    LMEM_PCCCR = (LMEM_PCCCR_INVW1_MASK | LMEM_PCCCR_INVW0_MASK | LMEM_PCCCR_ENWRBUF_MASK);
    /* Write GO bit to initiate command */
    LMEM_PCCCR = (LMEM_PCCCR_GO_MASK | LMEM_PCCCR_INVW1_MASK | LMEM_PCCCR_INVW0_MASK | LMEM_PCCCR_ENWRBUF_MASK);
    /* wait until the command completes */
    while (LMEM_PCCCR & LMEM_PCCCR_GO_MASK)
    {}
    /* Enable cache, enable write buffer */
    LMEM_PCCCR = (LMEM_PCCCR_ENWRBUF_MASK | LMEM_PCCCR_ENCACHE_MASK);
}

/*!
 * \brief This function disables the data cache
 */
void _icache_disable()
{
    LMEM_PCCCR = 0x0;
}

/*!
 * \brief This function enables the data cache
 */
void _icache_invalidate()
{
    LMEM_PCCCR |= LMEM_PCCCR_INVW0_MASK | LMEM_PCCCR_INVW1_MASK;
    LMEM_PCCCR |= LMEM_PCCCR_GO_MASK;
    /* wait until the command completes */
    while (LMEM_PCCCR & LMEM_PCCCR_GO_MASK)
    {}
}

/*!
 * \brief This function is called to invalidate a line out of the code cache.
 * 
 * \param addr 
 */
void _icache_invalidate_line(void *addr)
{
    /* Invalidate by physical address */
    LMEM_PCCLCR = LMEM_PCCLCR_LADSEL_MASK | (0x1u << LMEM_PCCLCR_LCMD_SHIFT);
    /* Set physical address and activate command */
    LMEM_PCCSAR = ((uint32_t)addr & ~0x03u) | LMEM_PCCSAR_LGO_MASK;
    /* wait until the command completes */
    while (LMEM_PCCSAR & LMEM_PCCSAR_LGO_MASK)
    {}
}

/*!
 * \brief This function is responsible for performing an instruction cache invalidate. 
 *  It must determine how many cache lines need to be invalidated and then 
 *  perform the invalidation.
 * 
 * \param addr 
 * \param length 
 */
void _icache_invalidate_mlines(void *addr, _mem_size length)
{
    void   *end_addr = (void *)((uint32_t)addr + length);
    addr = (void *) ((uint32_t)addr & ~(PSP_CACHE_LINE_SIZE - 1));
    do
    {
        _icache_invalidate_line(addr);
        addr = (void *) ((uint32_t)addr + PSP_CACHE_LINE_SIZE);
    } while (addr < end_addr);
}

/*!
 * \brief This function flushes the data cache
 */
void _icache_flush()
{
    LMEM_PCCCR |= LMEM_PCCCR_PUSHW0_MASK | LMEM_PCCCR_PUSHW1_MASK;
    LMEM_PCCCR |= LMEM_PCCCR_GO_MASK;
    /* wait until the command completes */
    while (LMEM_PCCCR & LMEM_PCCCR_GO_MASK)
    {}
}

/*!
 * \brief This function is called to push (flush and invalidate) a line out of the 
 *  instruction cache.
 * 
 * \param addr 
 */
void _icache_flush_line(void *addr)
{
    /* Invalidate by physical address */
    LMEM_PCCLCR = LMEM_PCCLCR_LADSEL_MASK | (0x2u << LMEM_PCCLCR_LCMD_SHIFT);
    /* Set physical address and activate command */
    LMEM_PCCSAR = ((uint32_t)addr & ~0x03u) | LMEM_PCCSAR_LGO_MASK;
    /* wait until the command completes */
    while (LMEM_PCCSAR & LMEM_PCCSAR_LGO_MASK)
    {}
}

/*!
 * \brief This function is called to flush the data cache by performing cache copybacks. 
 *  It must determine how many cache lines need to be copied back and then 
 *  perform the copybacks.
 * 
 * \param addr 
 * \param length 
 */
void _icache_flush_mlines(void *addr, _mem_size length)
{
    void   *end_addr = (void *)((uint32_t)addr + length);
    addr = (void *) ((uint32_t)addr & ~(PSP_CACHE_LINE_SIZE - 1));
    do
    {
        _icache_flush_line(addr);
        addr = (void *) ((uint32_t)addr + PSP_CACHE_LINE_SIZE);
    } while (addr < end_addr);
}
