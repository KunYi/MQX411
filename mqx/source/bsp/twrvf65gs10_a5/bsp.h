/*HEADER**********************************************************************
*
* Copyright 2010-2013 Freescale Semiconductor, Inc.
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
*   This file includes all include files specific to this target system.
*
*
*END************************************************************************/

#ifndef _bsp_h_
#define _bsp_h_ 1

#include <bsp_rev.h>
#include <psp.h>
#include <twrvf65gs10_a5.h>

#include <fio.h>
#include <io.h>

#include <io_mem.h>

//#include <clock.h>

#include <bsp_cm.h>
#include <cm_vybrid.h>
#include <cm.h>
#include <vybrid_wkpu.h>

#include <serial.h>
#include <serl_kuart.h>

#include <enet.h>
#include <macnet_vybrid.h>

/* Low power manager*/
#include <init_lpm.h>
#include <lpm_vybrid.h>
#include <lpm.h>

//#include <timer_gt.h>

#include <hwtimer.h>
#include <hwtimer_gtim.h>
#include <hwtimer_pit.h>

#include <qi2c.h>
#include <i2c.h>
#include <i2c_qi2c.h>

#include <dcu4.h>
#include <fbdev.h>

#include <esdhc.h>
#include <sdcard.h>
#include <sdcard_esdhc.h>

#include <srtc.h>
#include <rtc.h>

#include <sai.h>
#include <sai_ksai.h>

#include <lwgpio_vgpio.h>
#include <lwgpio.h>
#include <lwadc_vadc.h>

#include <spi.h>
#include <spi_dspi.h>
#include <spi_dspi_dma.h>

#include <qspi.h>
#include <qspi_quadspi.h>

#include <nandflash.h>

#include <flashx.h>
#include <flash_quadspi.h>
#include <flash_vybrid.h>

#include <usb_bsp.h>

#include <dma.h>

#include <bsp_clk_api.h>

_mqx_int _bsp_adc_io_init(_mqx_uint adc_num);
_mqx_int _bsp_adc_channel_io_init(_mqx_uint input);
_mqx_int _bsp_enet_io_init(_mqx_uint);
_mqx_int _bsp_dcu4_io_init(_mqx_uint);
_mqx_int _bsp_esdhc_io_init(_mqx_uint, _mqx_uint);
_mqx_int _bsp_gpio_io_init(void);
_mqx_int _bsp_flexcan_io_init(_mqx_uint);
_mqx_int _bsp_i2c_io_init(_mqx_uint);
_mqx_int _bsp_sai_io_init(_mqx_uint);
_mqx_int _bsp_serial_io_init(_mqx_uint, _mqx_uint);
_mqx_int _bsp_serial_rts_init(_mqx_uint);
_mqx_int _bsp_dspi_io_init(_mqx_uint dev_num);
_mqx_int _bsp_quadspi_io_init(_mqx_uint dev_num);
_mqx_int _bsp_usb_host_io_init(struct usb_host_if_struct *usb_if);
_mqx_int _bsp_usb_dev_io_init(struct usb_dev_if_struct *usb_if);
_mqx_int _bsp_nandflash_io_init(void);
void _bsp_usb_hs_disconnect_detection_enable(void *host_handle);
void _bsp_usb_hs_disconnect_detection_disable(void *host_handle);
_mqx_int _bsp_rtc_io_init( void );

bool _bsp_get_mac_address(uint32_t, uint32_t, _enet_address);

extern const LPM_CPU_OPERATION_MODE LPM_CPU_OPERATION_MODES[];

static inline uint32_t _bsp_int_init(_mqx_uint num, _mqx_uint prior, _mqx_uint subprior, bool enable) {
    uint32_t res;

    if (MQX_OK == (res = _gic_int_init(num, prior, enable))) {
        _irouter_routing(num, 1 << _psp_core_num(), enable);
    }

    return res;
}

#define _bsp_int_enable(num)                            _gic_int_enable(num)
#define _bsp_int_disable(num)                           _gic_int_disable(num)

void _bsp_aux_core_clock(_mqx_uint enable);
void _bsp_aux_core_start(void *m4_vect_base);

extern const SAI_INIT_STRUCT _bsp_sai_init;
extern const SDCARD_INIT_STRUCT _bsp_sdcard0_init;

#endif  /* _bsp_h_ */
