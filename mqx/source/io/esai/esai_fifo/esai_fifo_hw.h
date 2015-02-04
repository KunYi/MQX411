#ifndef _esai_fifo_hw_h_
#define _esai_fifo_hw_h_ 1
/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This include file defines some structures used in ESAI registers level operations
*
*
*END************************************************************************/

/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/


/*
** esai control register setting structure
*/
typedef struct
{
    uint32_t slots_num;
    uint32_t slots_mask;
    bool  fsync_slot_length;
    bool  fsync_early;
    bool  left_align;
    bool  zero_pad;
    bool  msb_shift;
    uint32_t  mode;
    uint32_t  slot_data_width;

} ESAI_HW_CR_SETTING_STRUCT, * ESAI_HW_CR_SETTING_STRUCT_PTR;


/*
** esai clock register setting structure
*/
typedef struct
{
    uint32_t dc_val;
    uint8_t  pm_val;
    uint8_t  fp_val;
    bool sck_dir;
    bool fst_dir;
    bool hck_dir;
    bool psr_val;
    bool sync_polar;
    bool sclk_polar;

    bool source_extal;

} ESAI_HW_CLK_SETTING_STRUCT, * ESAI_HW_CLK_SETTING_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern void esai_hw_init(ESAI_MemMapPtr esai_hw);

extern void esai_enable_syn(ESAI_MemMapPtr esai_hw);

extern void esai_hw_tx_reset(ESAI_MemMapPtr esai_hw);

extern void esai_hw_tx_out_of_reset(ESAI_MemMapPtr esai_hw);

extern void esai_hw_tx_fifo_enable(ESAI_MemMapPtr esai_hw,
                                        uint8_t enabled_tx,
                                        uint8_t watermark,
                                        uint32_t fifo_wa);

extern void esai_hw_tx_enable(ESAI_MemMapPtr esai_hw,
                                  uint8_t enabled_tx);

extern void esai_hw_tx_control_setting(ESAI_MemMapPtr esai_hw,
                                    ESAI_HW_CR_SETTING_STRUCT_PTR hw_setting);

extern void esai_hw_tx_clock_setting(ESAI_MemMapPtr esai_hw,
                                    ESAI_HW_CLK_SETTING_STRUCT_PTR hw_setting);

extern void esai_hw_tx_fifo_write(ESAI_MemMapPtr esai_hw,
                                     uint32_t data);

extern bool esai_hw_tx_fifo_empty(ESAI_MemMapPtr esai_hw);

extern bool esai_hw_tx_fifo_underrun(ESAI_MemMapPtr esai_hw);

extern void esai_hw_tx_enable_interrupts(ESAI_MemMapPtr esai_hw,
                                              bool enable);

extern void esai_hw_rx_reset(ESAI_MemMapPtr esai_hw);

extern void esai_hw_rx_out_of_reset(ESAI_MemMapPtr esai_hw);

extern void esai_hw_rx_fifo_enable(ESAI_MemMapPtr esai_hw,
                                       uint8_t enabled_rx,
                                       uint8_t watermark,
                                       uint32_t fifo_wa);

extern void esai_hw_rx_enable(ESAI_MemMapPtr esai_hw, uint8_t enabled_rx);

extern void esai_hw_rx_control_setting(ESAI_MemMapPtr esai_hw,
                                    ESAI_HW_CR_SETTING_STRUCT_PTR hw_setting);

extern void esai_hw_rx_clock_setting(ESAI_MemMapPtr esai_hw,
                                    ESAI_HW_CLK_SETTING_STRUCT_PTR hw_setting);

extern void esai_hw_rx_fifo_read(ESAI_MemMapPtr esai_hw, uint32_t *data);

extern bool esai_hw_rx_fifo_full(ESAI_MemMapPtr esai_hw);

extern bool esai_hw_rx_fifo_overrun(ESAI_MemMapPtr esai_hw);

extern void esai_hw_rx_enable_interrupts(ESAI_MemMapPtr esai_hw,
                                              bool enable);


#ifdef __cplusplus
}
#endif

#endif
/* EOF */
