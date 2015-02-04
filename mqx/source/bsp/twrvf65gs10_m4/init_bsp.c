/*HEADER**********************************************************************
*
* Copyright 2012-2013 Freescale Semiconductor, Inc.
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

#include "core_mutex.h"

#if BSP_ALARM_FREQUENCY == 0
#error Wrong definition of BSP_ALARM_FREQUENCY
#endif

HWTIMER systimer;                                   //System timer handle

void _bsp_systimer_callback(void *dummy);         //callback for system timer

const char      *_mqx_bsp_revision = REAL_NUM_TO_STR(BSP_REVISION);
const char      *_mqx_io_revision  = REAL_NUM_TO_STR(IO_REVISION);

/** Pre initialization - initializing requested modules for basic run of MQX.
 */
int _bsp_pre_init(void) {
    uint32_t result;
    KERNEL_DATA_STRUCT_PTR         kernel_data;

    /* Set the CPU type */
    _mqx_set_cpu_type(MQX_CPU);

#if MQX_EXIT_ENABLED
    /* Set the bsp exit handler, called by _mqx_exit */
    _mqx_set_exit_handler(_bsp_exit_handler);
#endif

    /* Memory splitter - prevent accessing both ram banks in one instruction */
    //_mem_alloc_at(0, (void*)0x20000000);

    result = _psp_int_init(BSP_FIRST_INTERRUPT_VECTOR_USED, BSP_LAST_INTERRUPT_VECTOR_USED);
    if (result != MQX_OK) {
        return result;
    }

    /* set possible new interrupt vector table - if MQX_ROM_VECTORS = 0 switch to
    ram interrupt table which was initialized in _psp_int_init) */
    _int_set_vector_table(BSP_INTERRUPT_VECTOR_TABLE);

#if PE_LDD_VERSION
    /** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
    PE_low_level_init();
#endif

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

#if 0
    /* MCG initialization and internal oscillators trimming */
    if (CM_ERR_OK != _bsp_set_clock_configuration(BSP_CLOCK_CONFIGURATION_AUTOTRIM))
    {
        return MQX_TIMER_ISR_INSTALL_FAIL;
    }

    if (CM_ERR_OK != _bsp_osc_autotrim())
    {
        return MQX_TIMER_ISR_INSTALL_FAIL;
    }

    /* Switch to startup clock configuration */
    if (CM_ERR_OK != _bsp_set_clock_configuration(BSP_CLOCK_CONFIGURATION_STARTUP))
    {
        return MQX_TIMER_ISR_INSTALL_FAIL;
    }
#endif

    /* Initialize the system ticks */
    _GET_KERNEL_DATA(kernel_data);
    kernel_data->TIMER_HW_REFERENCE = (BSP_CORE_CLOCK / BSP_ALARM_FREQUENCY);
    _time_set_ticks_per_sec(BSP_ALARM_FREQUENCY);
    _time_set_hwticks_per_tick(hwtimer_get_modulo(&systimer));
    _time_set_hwtick_function(_bsp_get_hwticks, (void *)NULL);

#if MQX_ENABLE_USER_MODE
#warning usermode is not supported on this platform yet
    _kinetis_mpu_init();

    // supervisor full access, user no access for whole memory
    _kinetis_mpu_add_region(0, ((unsigned char *)kernel_data->INIT.START_OF_USER_NO_MEMORY) - 1, \
        MPU_WORD_M3SM(MPU_SM_RWX) | MPU_WORD_M3UM(MPU_UM_R | MPU_UM_X) | \
        MPU_WORD_M2SM(MPU_SM_RWX) | MPU_WORD_M2UM(MPU_UM_R | MPU_UM_X) | \
        MPU_WORD_M1SM(MPU_SM_RWX) | MPU_WORD_M1UM(MPU_UM_R | MPU_UM_X) | \
        MPU_WORD_M0SM(MPU_SM_RWX) | MPU_WORD_M0UM(MPU_UM_R | MPU_UM_X));

    _kinetis_mpu_add_region(((unsigned char *)kernel_data->INIT.END_OF_USER_NO_MEMORY), (unsigned char *)0xffffffff, \
        MPU_WORD_M3SM(MPU_SM_RWX) | MPU_WORD_M3UM(MPU_UM_R | MPU_UM_X) | \
        MPU_WORD_M2SM(MPU_SM_RWX) | MPU_WORD_M2UM(MPU_UM_R | MPU_UM_X) | \
        MPU_WORD_M1SM(MPU_SM_RWX) | MPU_WORD_M1UM(MPU_UM_R | MPU_UM_X) | \
        MPU_WORD_M0SM(MPU_SM_RWX) | MPU_WORD_M0UM(MPU_UM_R | MPU_UM_X));

    // set access for user memory area
#if MQX_DEFAULT_USER_ACCESS_RW
    // user .data RW
    _kinetis_mpu_add_region(kernel_data->INIT.START_OF_USER_DEFAULT_MEMORY, ((unsigned char *)kernel_data->INIT.END_OF_USER_DEFAULT_MEMORY) - 1, \
        MPU_WORD_M1SM(MPU_SM_RWX) | MPU_WORD_M1UM(MPU_UM_R | MPU_UM_W) | \
        MPU_WORD_M0SM(MPU_SM_RWX) | MPU_WORD_M0UM(MPU_UM_R | MPU_UM_W));


#else
    // user RO - this is by default
    // user .data RO

    /*_kinetis_mpu_add_region(kernel_data->INIT.START_OF_KERNEL_AREA, kernel_data->INIT.END_OF_KERNEL_AREA, \
        MPU_WORD_M1SM(MPU_SM_RWX) | MPU_WORD_M1UM(MPU_UM_R) | \
        MPU_WORD_M0SM(MPU_SM_RWX) | MPU_WORD_M0UM(MPU_UM_R));
    */

#endif // MQX_DEFAULT_USER_ACCESS_RW

    // set access for user memory area
    if (0 == kernel_data->INIT.END_OF_USER_HEAP) {
        // create user heap automaticaly, we have specified only size of heap (end of heap is zero, start of heap mean size)
        LWMEM_POOL_STRUCT_PTR   lwmem_pool_ptr;
        unsigned char               *start;

        //start = _lwmem_alloc((char*)kernel_data->INIT.END_OF_USER_HEAP - (char*)kernel_data->INIT.START_OF_USER_HEAP + sizeof(LWMEM_POOL_STRUCT));
        start = _lwmem_alloc((uint32_t)kernel_data->INIT.START_OF_USER_HEAP + sizeof(LWMEM_POOL_STRUCT));
        lwmem_pool_ptr = (LWMEM_POOL_STRUCT_PTR)start;
        start = (void *)((unsigned char *)start + sizeof(LWMEM_POOL_STRUCT));
        _lwmem_create_pool(lwmem_pool_ptr, start, (uint32_t)kernel_data->INIT.START_OF_USER_HEAP);
        _mem_set_pool_access((_lwmem_pool_id) lwmem_pool_ptr, POOL_USER_RW_ACCESS);

        kernel_data->KD_USER_POOL = (void *) lwmem_pool_ptr;
    }
    else {
        // manual user heap definition
        _kinetis_mpu_add_region(kernel_data->INIT.START_OF_USER_HEAP, ((unsigned char *)kernel_data->INIT.END_OF_USER_HEAP) - 1, \
            MPU_WORD_M1SM(MPU_SM_RWX) | MPU_WORD_M1UM(MPU_UM_R | MPU_UM_W) | \
            MPU_WORD_M0SM(MPU_SM_RWX) | MPU_WORD_M0UM(MPU_UM_R | MPU_UM_W));
    }

    // set access for user read-write memory area
    if (kernel_data->INIT.START_OF_USER_RW_MEMORY < kernel_data->INIT.END_OF_USER_RW_MEMORY) {
        _kinetis_mpu_add_region(kernel_data->INIT.START_OF_USER_RW_MEMORY, ((unsigned char *)kernel_data->INIT.END_OF_USER_RW_MEMORY) - 1, \
            MPU_WORD_M1SM(MPU_SM_RWX) | MPU_WORD_M1UM(MPU_UM_R | MPU_UM_W) | \
            MPU_WORD_M0SM(MPU_SM_RWX) | MPU_WORD_M0UM(MPU_UM_R | MPU_UM_W));
    }

    // set access for user read-only memory area
    if (kernel_data->INIT.START_OF_USER_RO_MEMORY < kernel_data->INIT.END_OF_USER_RO_MEMORY) {
        _kinetis_mpu_add_region(kernel_data->INIT.START_OF_USER_RO_MEMORY, ((unsigned char *)kernel_data->INIT.END_OF_USER_RO_MEMORY) - 1, \
            MPU_WORD_M1SM(MPU_SM_RWX) | MPU_WORD_M1UM(MPU_UM_R) | \
            MPU_WORD_M0SM(MPU_SM_RWX) | MPU_WORD_M0UM(MPU_UM_R));
    }

    // set access for user no access memory area
    if (kernel_data->INIT.START_OF_USER_NO_MEMORY < kernel_data->INIT.END_OF_USER_NO_MEMORY) {
        _kinetis_mpu_add_region(kernel_data->INIT.START_OF_USER_NO_MEMORY, ((unsigned char *)kernel_data->INIT.END_OF_USER_NO_MEMORY) - 1, \
            MPU_WORD_M1SM(MPU_SM_RWX) | MPU_WORD_M1UM(0) | \
            MPU_WORD_M0SM(MPU_SM_RWX) | MPU_WORD_M0UM(0));
    }

    _kinetis_mpu_enable();
#else
    //_kinetis_mpu_disable();
#endif // MQX_ENABLE_USER_MODE

    /* Install low power support */
#if MQX_ENABLE_LOW_POWER
    _lpm_install (LPM_CPU_OPERATION_MODES, LPM_OPERATION_MODE_RUN);
#endif

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
    /* Initialize the I/O Sub-system */
    result = _io_init();
    if (result != MQX_OK) {
        return result;
    }

    /* Init clock manager */
    _bsp_clock_manager_init();

    /* Initialize RTC and MQX time */
#if BSPCFG_ENABLE_RTCDEV
    if (MQX_OK == _bsp_rtc_io_init()) {
        _rtc_init (NULL);
    }
#endif
    /* Extend default memory pool with lower part of tightly coupled memory */
#if MQX_USE_MEM
    result = _mem_extend(BSP_CORE_M4_TCML_START, BSP_CORE_M4_TCML_SIZE);
    if (result != MQX_OK)
    {
        return result;
    }
#endif

/** Cache settings **/
    /*folowing 2 functions enable caches (instruction and data cache) and invalidate caches*/
    _ICACHE_ENABLE(0);
    _DCACHE_ENABLE(0);

#if BSPCFG_CORE_MUTEX
    extern const CORE_MUTEX_INIT_STRUCT _core_mutex_init_info;

    result = _core_mutex_install(&_core_mutex_init_info);
    if (result != MQX_OK) {
        return result;
    }
#endif

    /* Install device drivers */
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

#if BSPCFG_ENABLE_TTYD
    _kuart_polled_install("ttyd:", &_bsp_sci3_init, _bsp_sci3_init.QUEUE_SIZE);
#endif

#if BSPCFG_ENABLE_ITTYD
   _kuart_int_install("ittyd:", &_bsp_sci3_init, _bsp_sci3_init.QUEUE_SIZE);
#endif

#if BSPCFG_ENABLE_TTYE
    _kuart_polled_install("ttye:", &_bsp_sci4_init, _bsp_sci4_init.QUEUE_SIZE);
#endif

#if BSPCFG_ENABLE_ITTYE
   _kuart_int_install("ittye:", &_bsp_sci4_init, _bsp_sci4_init.QUEUE_SIZE);
#endif

#if BSPCFG_ENABLE_TTYF
    _kuart_polled_install("ttyf:", &_bsp_sci5_init, _bsp_sci5_init.QUEUE_SIZE);
#endif

#if BSPCFG_ENABLE_ITTYF
   _kuart_int_install("ittyf:", &_bsp_sci5_init, _bsp_sci5_init.QUEUE_SIZE);
#endif

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

#if BSPCFG_ENABLE_ESDHC
    _esdhc_install("esdhc1:", &_bsp_esdhc1_init);
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
#if BSPCFG_ENABLE_QUADSPI0 && !BSPCFG_ENABLE_FLASHX
    _io_qspi_install("qspi0:", &_bsp_quadspi0_init);
#endif

#if BSPCFG_ENABLE_ESDHC
    _esdhc_install ("esdhc:", &_bsp_esdhc0_init);
#endif

#if BSPCFG_ENABLE_IODEBUG
    _io_debug_install("iodebug:", &_bsp_iodebug_init);
#endif

#if BSPCFG_ENABLE_NANDFLASH
    result = _io_nandflash_install(&_bsp_nandflash_init);
#endif
#if BSPCFG_ENABLE_II2S0
   _ki2s_int_install("ii2s0:", &_bsp_i2s0_init);
#endif

#if BSPCFG_ENABLE_SAI
    result = _io_sai_int_install(&_bsp_sai_init);
#endif
   /* Initialize the default serial I/O */
   _io_serial_default_init();

#endif // BSPCFG_ENABLE_IO_SUBSYSTEM

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
void _bsp_exit_handler(void) {
   _DCACHE_DISABLE();
   _ICACHE_DISABLE();
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


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_hwticks
* Returned Value   : none
* Comments         :
*    This function returns the number of hw ticks that have elapsed
* since the last interrupt
*
*END*----------------------------------------------------------------------*/
uint32_t _bsp_get_hwticks(void *param) {
    HWTIMER_TIME_STRUCT time;      //struct for storing time
    hwtimer_get_time(&systimer, &time);
    return time.SUBTICKS;
}


void *_bsp_vtop(void *ptr)
{
    uint32_t addr = (uint32_t)ptr;
    return (void *) addr;
}


void *_bsp_ptov(void *ptr)
{
    uint32_t addr = (uint32_t) ptr;
    return (void *) addr;
}
