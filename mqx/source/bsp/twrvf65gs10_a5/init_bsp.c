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
*   This file contains the source functions for functions required to
*   specifically initialize the card.
*
*
*END************************************************************************/

#include "mqx_inc.h"
#include "bsp.h"
#include "bsp_prv.h"
#include "io_rev.h"
#include "bsp_rev.h"
#include "cortexa5.h"

#include "core_mutex.h"

#if BSP_ALARM_FREQUENCY == 0
#error Wrong definition of BSP_ALARM_FREQUENCY
#endif

HWTIMER systimer;                                   //System timer handle

void _bsp_systimer_callback(void *dummy);         //callback for system timer

const char      *_mqx_bsp_revision = REAL_NUM_TO_STR(BSP_REVISION);
const char      *_mqx_io_revision  = REAL_NUM_TO_STR(IO_REVISION);
_mem_pool_id _BSP_sram_pool;

/** Pre initialization - initializing requested modules for basic run of MQX.
 */
int _bsp_pre_init(void) {
    uint32_t *L1PageTable;
    uint32_t result;
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _mqx_set_cpu_type(MQX_CPU);

#if MQX_EXIT_ENABLED
    _mqx_set_exit_handler(_bsp_exit_handler);
#endif

    _a5_initialize_support();

    result = _psp_int_init(BSP_FIRST_INTERRUPT_VECTOR_USED, BSP_LAST_INTERRUPT_VECTOR_USED);

    if (result != MQX_OK)
        return result;

    init_gic();

    /* Initialize , set and run system hwtimer */
    result = hwtimer_init(&systimer, &BSP_SYSTIMER_DEV, BSP_SYSTIMER_ID, BSP_SYSTIMER_ISR_PRIOR);
    if (MQX_OK != result) {
        return result;
    }
    result = hwtimer_set_freq(&systimer, BSP_SYSTIMER_SRC_CLK, BSP_ALARM_FREQUENCY);
    if (MQX_OK != result) {
        hwtimer_deinit(&systimer);
        return result;
    }
    result = hwtimer_callback_reg(&systimer,(HWTIMER_CALLBACK_FPTR)_bsp_systimer_callback, NULL);
    if (MQX_OK != result) {
        hwtimer_deinit(&systimer);
        return result;
    }
    result = hwtimer_start(&systimer);
    if (MQX_OK != result) {
        hwtimer_deinit(&systimer);
        return result;
    }

    /* Initialize the system ticks */
    _GET_KERNEL_DATA(kernel_data);
    kernel_data->TIMER_HW_REFERENCE = (BSP_SYSTEM_CLOCK / BSP_ALARM_FREQUENCY);
    _time_set_ticks_per_sec(BSP_ALARM_FREQUENCY);
    _time_set_hwticks_per_tick(hwtimer_get_modulo(&systimer));
    _time_set_hwtick_function(_bsp_get_hwticks, (void *)NULL);

    if (_mqx_monitor_type == MQX_MONITOR_TYPE_NONE)
    {
        /* Enable MMU and L1 cache */
        /* alloc L1 mmu table */
        L1PageTable = _mem_alloc_align(MMU_TRANSLATION_TABLE_SIZE, MMU_TRANSLATION_TABLE_ALIGN);
        /* None cacheable is comon with strongly ordered. MMU doesnt work with another init configuration */
        _mmu_vinit(PSP_PAGE_TABLE_SECTION_SIZE(PSP_PAGE_TABLE_SECTION_SIZE_1MB) | PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RW_ALL) | PSP_PAGE_TYPE(PSP_PAGE_TYPE_STRONG_ORDER), (void *)L1PageTable);
        /* add region in sram area */
        _mmu_add_vregion((void *)__INTERNAL_SRAM_BASE, (void *)__INTERNAL_SRAM_BASE, (_mem_size) 0x00100000, PSP_PAGE_TABLE_SECTION_SIZE(PSP_PAGE_TABLE_SECTION_SIZE_1MB) | PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_NON)   | PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RW_ALL));
        /* add cached region in ddr area */
        _mmu_add_vregion((void *)__EXTERNAL_DDRAM_BASE, (void *)__EXTERNAL_DDRAM_BASE, __EXTERNAL_DDRAM_SIZE, PSP_PAGE_TABLE_SECTION_SIZE(PSP_PAGE_TABLE_SECTION_SIZE_1MB) | PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WBNWA) | PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RW_ALL));
        /* add qspi0 flash region area */
        _mmu_add_vregion((void *)__EXTERNAL_QSPI0_FLASH_BASE, (void *)__EXTERNAL_QSPI0_FLASH_BASE, __EXTERNAL_QSPI0_FLASH_SIZE, PSP_PAGE_TABLE_SECTION_SIZE(PSP_PAGE_TABLE_SECTION_SIZE_1MB) | PSP_PAGE_TYPE(PSP_PAGE_TYPE_CACHE_WBNWA) | PSP_PAGE_DESCR(PSP_PAGE_DESCR_ACCESS_RW_ALL));
         _mmu_venable();

        _DCACHE_ENABLE();
        _ICACHE_ENABLE();
    }
#if BSPCFG_HAS_SRAM_POOL
    /* Check if the Kernel data are placed in the SRAM or not (distinquish between SRAMDATA and DDRDATA targets) */
    if ( (((uint32_t)__INTERNAL_SRAM_BASE) < (uint32_t)BSP_DEFAULT_START_OF_KERNEL_MEMORY) &&
            (((uint32_t)BSP_DEFAULT_START_OF_KERNEL_MEMORY) < ((uint32_t)__INTERNAL_SRAM_BASE + (uint32_t)__INTERNAL_SRAM_SIZE)))
    {
        /* SRAM Target, just reuse system pool*/
        _BSP_sram_pool  = _mem_get_system_pool_id();
    }
    else
    {
        /* DDR Target, create new pool in SRAM or use Uncached pool */
#if MQX_USE_UNCACHED_MEM && PSP_HAS_DATA_CACHE
        _GET_KERNEL_DATA(kernel_data);
        /* Uncached pool on this platform can be placed in SRAM. We can reuse uncached pool as SRAM pool */
        if ( (((uint32_t)__INTERNAL_SRAM_BASE) < (uint32_t)kernel_data->UNCACHED_POOL) &&
                (((uint32_t)kernel_data->UNCACHED_POOL) < ((uint32_t)__INTERNAL_SRAM_BASE + (uint32_t)__INTERNAL_SRAM_SIZE)))
        {
            _BSP_sram_pool = (MEMPOOL_STRUCT_PTR) kernel_data->UNCACHED_POOL;
        }
        else
        {
            _BSP_sram_pool = _mem_create_pool(__SRAM_POOL_START, ((uint32_t)__SRAM_POOL_END) - (uint32_t) __SRAM_POOL_START);
        }
#else
        /* Uncached pool does not exist - create SRAM pool in SRAM area */
        _BSP_sram_pool = _mem_create_pool(__SRAM_POOL_START, ((uint32_t)__SRAM_POOL_END) - (uint32_t) __SRAM_POOL_START);
#endif
    }
#endif

#if BSPCFG_CORE_MUTEX
    extern const CORE_MUTEX_INIT_STRUCT _core_mutex_init_info;

    result = _core_mutex_install(&_core_mutex_init_info);
    if (result != MQX_OK) {
        return result;
    }
#endif
    
    /* Install low power support */
#if MQX_ENABLE_LOW_POWER
    _lpm_install (LPM_CPU_OPERATION_MODES, LPM_OPERATION_MODE_RUN);
#endif /* MQX_ENABLE_LOW_POWER */

    return 0;
}

/** Initialization - called from init task, usually for io initialization.
 */
int _bsp_init(void) {
    uint32_t result;

    /* Initialize DMA */
    result = dma_init(_bsp_dma_devif_list);
    if (result != MQX_OK) {
        return result;
    }

#if BSPCFG_ENABLE_IO_SUBSYSTEM
    result = _io_init();
    if (result != MQX_OK)
        return result;

    /* Init clock manager */
    _bsp_clock_manager_init();
    /* Install Device Drivers */
#if BSPCFG_ENABLE_TTYA
    _kuart_polled_install("ttya:", &_bsp_sci0_init, _bsp_sci0_init.QUEUE_SIZE);
#endif

#if BSPCFG_ENABLE_ITTYA
   _kuart_int_install("ittya:", &_bsp_sci0_init, _bsp_sci0_init.QUEUE_SIZE);
#endif

#if BSPCFG_ENABLE_TTYB
    _kuart_polled_install("ttyb:", &_bsp_sci1_init, _bsp_sci1_init.QUEUE_SIZE);
#endif

#if BSPCFG_ENABLE_ITTYB
    _kuart_int_install("ittyb:", &_bsp_sci1_init, _bsp_sci1_init.QUEUE_SIZE);
#endif

#if BSPCFG_ENABLE_TTYC
    _kuart_polled_install("ttyc:", &_bsp_sci2_init, _bsp_sci2_init.QUEUE_SIZE);
#endif

#if BSPCFG_ENABLE_ITTYC
    _kuart_int_install("ittyc:", &_bsp_sci2_init, _bsp_sci2_init.QUEUE_SIZE);
#endif

    /* Initialize RTC and MQX time */
#if BSPCFG_ENABLE_RTCDEV
    if (MQX_OK == _bsp_rtc_io_init()) {
        _rtc_init (NULL);
    }
#endif

#if BSPCFG_ENABLE_DCU4
    _dcu4_install("dcu0:", &_bsp_dcu0_init);
#endif

#if BSPCFG_ENABLE_FBDEV
    _fbdev_install("fbdev:", &_bsp_fbdev_init);
#endif

#if BSPCFG_ENABLE_ESDHC
    _esdhc_install("esdhc1:", &_bsp_esdhc1_init);
#endif

#if BSPCFG_ENABLE_FLASHX_QUADSPI0
    _io_flashx_install("flashx_qspi0:", &_bsp_flashx_quadspi0_init);
#endif

#if BSPCFG_ENABLE_SPI0
    _io_spi_install("spi0:", &_bsp_spi0_init);
#endif

#if BSPCFG_ENABLE_SPI1
    _io_spi_install("spi1:", &_bsp_spi1_init);
#endif

#if BSPCFG_ENABLE_SPI2
    _io_spi_install("spi2:", &_bsp_spi2_init);
#endif

/* Flashx and QuadSPI only one is allowed to be enabled
 * Flashx interface for QuadSPI flash is preferred */
#if BSPCFG_ENABLE_QUADSPI0 && !BSPCFG_ENABLE_FLASHX_QUADSPI0
    _io_qspi_install("qspi0:", &_bsp_quadspi0_init);
#endif

    /* I2C Initialization */
#if BSPCFG_ENABLE_I2C0
    _qi2c_polled_install("i2c0:", &_bsp_i2c0_init);
#endif

#if BSPCFG_ENABLE_I2C1
    _qi2c_polled_install("i2c1:", &_bsp_i2c1_init);
#endif

#if BSPCFG_ENABLE_I2C2
    _qi2c_polled_install("i2c2:", &_bsp_i2c2_init);
#endif

#if BSPCFG_ENABLE_I2C3
    _qi2c_polled_install("i2c3:", &_bsp_i2c3_init);
#endif

#if BSPCFG_ENABLE_II2C0
    _qi2c_int_install("ii2c0:", &_bsp_i2c0_init);
#endif

#if BSPCFG_ENABLE_II2C1
    _qi2c_int_install("ii2c1:", &_bsp_i2c1_init);
#endif

#if BSPCFG_ENABLE_II2C2
    _qi2c_int_install("ii2c2:", &_bsp_i2c2_init);
#endif

#if BSPCFG_ENABLE_II2C3
    _qi2c_int_install("ii2c3:", &_bsp_i2c3_init);
#endif
#if BSPCFG_ENABLE_NANDFLASH
    result = _io_nandflash_install(&_bsp_nandflash_init);
#endif

#if BSPCFG_ENABLE_USB
#endif

#if BSPCFG_ENABLE_SAI
    result = _io_sai_int_install(&_bsp_sai_init);
#endif
    _io_serial_default_init();

#endif

    return 0;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_exit_handler
* Returned Value   : none
* Comments         :
*    This function is called when MQX exits
*
*END*----------------------------------------------------------------------*/
void _bsp_exit_handler(void)
{
    _DCACHE_DISABLE();
    _ICACHE_DISABLE();
    _mmu_vdisable();
}

/*FUNCTION*********************************************************************
 *
 * Function Name    : _bsp_systimer_callback
 * Returned Value   : void
 * Comments         :
 *    The system timer callback.
 *
 *END**********************************************************************/

void _bsp_systimer_callback(void *dummy) {
    _time_notify_kernel();
}

/*FUNCTION*--------------------------------------------------------------------
 *
 * Function Name    : _bsp_get_hwticks
 * Returned Value   : none
 * Comments         :
 *    This function returns the number of hw ticks that have elapsed
 * since the last interrupt
 *
 *END*-----------------------------------------------------------------------*/

uint32_t _bsp_get_hwticks(void *param) {
    HWTIMER_TIME_STRUCT time;      //struct for storing time
    hwtimer_get_time(&systimer, &time);
    return time.SUBTICKS;
}

void *_bsp_vtop(void *ptr)
{
    return ptr;
}

void *_bsp_ptov(void *ptr)
{
    return ptr;
}

void _bsp_aux_core_start(void *m4_vect_base)
{
    SRC_GPR(2) = ((_mqx_uint*)m4_vect_base)[1];      // reset vector
    SRC_GPR(3) = ((_mqx_uint*)m4_vect_base)[0];      // stack pointer

    // start M4 core - enable clock
    _bsp_aux_core_clock(1);
}
