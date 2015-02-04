/*HEADER**********************************************************************
*
* Copyright 2010 Freescale Semiconductor, Inc.
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
*   This file contains the definitions of constants and structures
*   required for initialization of the card.
*
*
*END************************************************************************/
#ifndef _bsp_prv_h
#define _bsp_prv_h 1

#ifdef __cplusplus
extern "C" {
#endif


/*
**  FUNCTION PROTOTYPES
*/

void    __init(void);

uint32_t _bsp_get_hwticks(void *);
void    _bsp_exit_handler(void);
void    _bsp_timer_isr(void *);

void init_gic(void);


/*
**  STRUCTURE DEFINITIONS
*/

extern       HWTIMER systimer;
/* I/O initialization controlled by initialization structures for each channel */
extern const KUART_INIT_STRUCT _bsp_sci0_init;
extern const KUART_INIT_STRUCT _bsp_sci2_init;

extern const DCU4_INIT_STRUCT _bsp_dcu0_init;
extern const FBDEV_INIT_STRUCT _bsp_fbdev_init;

extern const ESDHC_INIT_STRUCT _bsp_esdhc0_init;
extern const ESDHC_INIT_STRUCT _bsp_esdhc1_init;
extern const LWADC_INIT_STRUCT lwadc0_init;
extern const LWADC_INIT_STRUCT lwadc1_init;
extern const SPI_INIT_STRUCT _bsp_spi0_init;
extern const SPI_INIT_STRUCT _bsp_spi1_init;
extern const SPI_INIT_STRUCT _bsp_spi2_init;
extern const QSPI_INIT_STRUCT _bsp_quadspi0_init;
extern const QSPI_INIT_STRUCT _bsp_quadspi1_init;
extern const FLASHX_INIT_STRUCT _bsp_flashx_quadspi0_init;
extern const FLASHX_INIT_STRUCT _bsp_flashx_quadspi1_init;

extern const QI2C_INIT_STRUCT _bsp_i2c0_init;
extern const QI2C_INIT_STRUCT _bsp_i2c1_init;
extern const QI2C_INIT_STRUCT _bsp_i2c2_init;
extern const QI2C_INIT_STRUCT _bsp_i2c3_init;

extern const USB_EHCI_HOST_INIT_STRUCT _ehci0_host_init_param;
extern const USB_EHCI_HOST_INIT_STRUCT _ehci1_host_init_param;

extern const IOEXP_MAX7310_INIT_STRUCT _bsp_max7310_init;

extern const QUADDEC_INIT_INFO_STRUCT _bsp_ftm1_quaddec_init;
extern const QUADDEC_INIT_INFO_STRUCT _bsp_ftm2_quaddec_init;

extern const DMA_DEVIF_LIST _bsp_dma_devif_list[];

extern const VYBRID_ESAI_FIFO_INIT_STRUCT _bsp_esai_init;
extern const VYBRID_ESAI_VPORT_INIT_STRUCT _bsp_esai_vport0_tx_init;
extern const VYBRID_ESAI_VPORT_INIT_STRUCT _bsp_esai_vport5_rx_init;
extern const VYBRID_ASRC_INIT_STRUCT _bsp_asrc_init;

#ifdef __cplusplus
}
#endif

#endif
/* EOF */

