/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
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
*   This file contains low level functions for the ESAI VPORT device driver
*   for vybrid family.
*
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include <io_prv.h>
#include <fio_prv.h>
#include <mutex.h>
#include "esai_vport_prv.h"
#include "esai_vport.h"
#include "esai_vport_config.h"
#include "esai_vport_vybrid_prv.h"
#include "esai_fifo.h"
#include "esai_fifo_config.h"
#include "esai_fifo_prv.h"
#include "esai_vybrid.h"

#if DISCFG_OS_DRV_ENABLE_PCMMGR
extern ESAI_VPORT_PCMM_FUNCS_STRUCT vybrid_esai_vport_pcmm_funcs;
#endif

extern void _bsp_esai_io_init(void);
extern uint32_t _bsp_esai_clocks_get_fsys(void);
extern uint32_t _bsp_esai_clocks_get_extal(void);

extern uint32_t esai_fifo_node_deinit(ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR);
extern uint32_t esai_fifo_node_tx_config(ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR,
                                            ESAI_FIFO_CONFIG_STRUCT_PTR);
extern uint32_t esai_fifo_node_rx_config(ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR,
                                            ESAI_FIFO_CONFIG_STRUCT_PTR);
extern uint32_t esai_fifo_enable_tr(ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR,
                                     uint32_t, bool);
extern uint32_t esai_fifo_set_dir(ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR,
                                  uint32_t, AUD_IO_FW_DIRECTION);
extern uint32_t esai_fifo_reset(ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR);

extern uint32_t esai_fifo_node_enable_asrc(ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR,
                                               bool, int32_t);
extern uint32_t esai_fifo_reg_callback(ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR,
                            void (_CODE_PTR_ cb_ptr)(void *, uint32_t, void *),
                            void *);

extern int32_t esai_fifo_node_clear_tx_buf(ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR);

extern int32_t esai_fifo_node_clear_rx_buf(ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR);

extern uint32_t esai_fifo_node_tx_error(ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR);

extern uint32_t esai_fifo_node_rx_error(ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR);

extern uint32_t esai_fifo_node_set_tx_timeout(ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR,
                                                  uint32_t);

extern uint32_t esai_fifo_node_set_rx_timeout(ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR,
                                                 uint32_t);

extern uint32_t _vybrid_esai_vport_init (VYBRID_ESAI_VPORT_INIT_STRUCT_PTR,
                         void * *, char *, ESAI_FIFO_CONTEXT_STRUCT_PTR);

extern uint32_t _vybrid_esai_vport_deinit (IO_ESAI_VPORT_DEVICE_STRUCT_PTR,
                                        VYBRID_ESAI_VPORT_INFO_STRUCT_PTR);


/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_esai_vport_init_default_config
* Returned Value   : MQX error code
* Comments         :
*    This function initializes the default configurations for esai fifo driver.
*
*END*********************************************************************/
static uint32_t _vybrid_esai_vport_init_default_config
    (
        /* [IN] the address of the fifo node context */
        ESAI_FIFO_CONTEXT_STRUCT_PTR fifo_context_ptr,

        /* [IN] esai tx default slot width*/
        uint8_t tx_default_slot_width,

        /* [IN] esai rx default slot width*/
        uint8_t rx_default_slot_width,

        /* [IN] esai tx clock is master*/
        bool tx_clk_master,

        /* [IN] esai rx clock is master*/
        bool rx_clk_master
    )
{
    if (NULL == fifo_context_ptr)
        return ESAI_VPORT_ERROR_INVALID_PARAMETER;


    /*init the default tx config*/
    fifo_context_ptr->TX_CONFIG.pcm_fmt = ESAI_FIFO_PCM_S24_PACK_LE;/*32 bit pcm*/
    fifo_context_ptr->TX_CONFIG.inf_fmt.fmt = ESAI_MODULE_FMT_I2S; /*I2S */
    fifo_context_ptr->TX_CONFIG.clock.clock_polarity = ESAI_CLOCK_POLAR_NB_IF;

    switch (tx_default_slot_width) {
        case 16:
            fifo_context_ptr->TX_CONFIG.inf_fmt.fmt_para.stereo_para.slot_width
                = ESAI_MODULE_SW_16BIT;
            break;
        case 24:
            fifo_context_ptr->TX_CONFIG.inf_fmt.fmt_para.stereo_para.slot_width
                = ESAI_MODULE_SW_24BIT;
            break;
        case 32:
            fifo_context_ptr->TX_CONFIG.inf_fmt.fmt_para.stereo_para.slot_width
                = ESAI_MODULE_SW_32BIT;
            break;
        default:
            fifo_context_ptr->TX_CONFIG.inf_fmt.fmt_para.stereo_para.slot_width
                = ESAI_MODULE_SW_32BIT;
            break;
    }

    if (tx_clk_master) {
        fifo_context_ptr->TX_CONFIG.clock.clock_direction = ESAI_CLOCK_MASTER;
        fifo_context_ptr->TX_CONFIG.clock.output_refer_clk = ESAI_CLOCK_EXTAL;
//        fifo_context_ptr->TX_CONFIG.clock.output_refer_clk = ESAI_CLOCK_FSYS;
    } else {
        fifo_context_ptr->TX_CONFIG.clock.clock_direction = ESAI_CLOCK_SLAVE;
    }
    fifo_context_ptr->TX_CONFIG.clock.timing.extal_para.frame_rate
        = 48000; /*48k sample rate*/

    fifo_context_ptr->TX_CONFIG.asrc_8bit_covert = FALSE;

    /*init the default rx config*/
    fifo_context_ptr->RX_CONFIG.pcm_fmt = ESAI_FIFO_PCM_S24_PACK_LE;/*32 bit pcm*/
    fifo_context_ptr->RX_CONFIG.inf_fmt.fmt = ESAI_MODULE_FMT_I2S; /*I2S */
    fifo_context_ptr->RX_CONFIG.clock.clock_polarity = ESAI_CLOCK_POLAR_NB_IF;

    switch  (rx_default_slot_width) {
    case    16:
            fifo_context_ptr->RX_CONFIG.inf_fmt.fmt_para.stereo_para.slot_width
                = ESAI_MODULE_SW_16BIT;
            break;

    case    24:
            fifo_context_ptr->RX_CONFIG.inf_fmt.fmt_para.stereo_para.slot_width
                = ESAI_MODULE_SW_24BIT;
            break;

    case    32:
            fifo_context_ptr->RX_CONFIG.inf_fmt.fmt_para.stereo_para.slot_width
                = ESAI_MODULE_SW_32BIT;
            break;

    default:
            fifo_context_ptr->RX_CONFIG.inf_fmt.fmt_para.stereo_para.slot_width
                = ESAI_MODULE_SW_32BIT;
            break;
    }

    if (rx_clk_master) {
        fifo_context_ptr->RX_CONFIG.clock.clock_direction = ESAI_CLOCK_MASTER;
        fifo_context_ptr->RX_CONFIG.clock.output_refer_clk = ESAI_CLOCK_EXTAL;
    } else {
        fifo_context_ptr->RX_CONFIG.clock.clock_direction = ESAI_CLOCK_SLAVE;
    }

    /*48k sample rate*/
    fifo_context_ptr->RX_CONFIG.clock.timing.extal_para.frame_rate = 48000;

    fifo_context_ptr->RX_CONFIG.asrc_8bit_covert = FALSE;

    return MQX_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_esai_vport_setup_fifo_ports
* Returned Value   : MQX error code
* Comments         :
*    This function sets up the transceivers port configuration to ESAI fifo node and setup DIM
*
*END*********************************************************************/
static uint32_t _vybrid_esai_vport_setup_fifo_ports
(
    /* [IN] the address of the fifo node context */
    ESAI_FIFO_CONTEXT_STRUCT_PTR    esai_fifo_ptr
)
{
    uint8_t i = 0;
    bool enable = FALSE;

//    DIM_INI_DATA_STRUCT dim_init;

    /*enable transceivers*/
    for (i=0; i<8; i++) {

        enable = FALSE;
        if ((1<<i) & esai_fifo_ptr->TX_BITMAP) {
            esai_fifo_enable_tr(&(esai_fifo_ptr->ESAI_NODE), i, TRUE);
            esai_fifo_set_dir(&(esai_fifo_ptr->ESAI_NODE), i, AUD_IO_FW_DIR_TX);

            enable = TRUE;
        }

        if ((1<<i) & esai_fifo_ptr->RX_BITMAP) {
            esai_fifo_enable_tr(&(esai_fifo_ptr->ESAI_NODE), i, TRUE);
            esai_fifo_set_dir(&(esai_fifo_ptr->ESAI_NODE), i, AUD_IO_FW_DIR_RX);

            enable = TRUE;
        }

        if (!enable) {
            esai_fifo_enable_tr(&(esai_fifo_ptr->ESAI_NODE), i, FALSE);
        }
    }

    /*install DIM RX for multiple ESAI VPORT devices case*/
    if (esai_fifo_ptr->RX_DEVS_COUNT > 1) {
    }

    return MQX_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_esai_vport_connect_fifo
* Returned Value   : MQX error code
* Comments         :
*    This function initializes and configures the fifo node,
*    connects current vport to fifo node and DIM
*
*END*********************************************************************/
static uint32_t _vybrid_esai_vport_connect_fifo
    (
        /* [IN] the initialization information for the device being opened */
        VYBRID_ESAI_VPORT_INFO_STRUCT_PTR   io_info_ptr,

        /* [IN] the address of the fifo node context */
        ESAI_FIFO_CONTEXT_STRUCT_PTR    esai_fifo_ptr
    )
{
    ESAI_FIFO_NODE_INIT_STRUCT      fifo_node_init;

    VYBRID_ESAI_FIFO_INIT_STRUCT_PTR fifo_init_ptr;

    uint32_t result;

    if (esai_fifo_ptr->TX_CONNECT_COUNT == 0 &&
        esai_fifo_ptr->RX_CONNECT_COUNT == 0) {

        /*call bsp clock and IO setting to config and ungate the clock to ESAI*/
        _bsp_esai_io_init();

        fifo_init_ptr = &(esai_fifo_ptr->INIT);

        fifo_node_init.FSYS_CLK     = _bsp_esai_clocks_get_fsys();
        fifo_node_init.EXTAL_CLK    = _bsp_esai_clocks_get_extal();
        fifo_node_init.MODULE_ID    = fifo_init_ptr->MODULE_ID;
        fifo_node_init.IRQ_NUM      = _esai_get_irq_num(
            fifo_node_init.MODULE_ID);
        fifo_node_init.BASE_ADDRESS = _esai_get_base_address(
                    fifo_node_init.MODULE_ID);
        fifo_node_init.RX_DMA_PACKET_MAX_NUM  =
                    fifo_init_ptr->RX_DMA_PACKET_MAX_NUM;
        fifo_node_init.RX_DMA_PACKET_SIZE     =
                    fifo_init_ptr->RX_DMA_PACKET_SIZE;
        fifo_node_init.RX_FIFO_WM   = fifo_init_ptr->RX_FIFO_WM;
        fifo_node_init.TX_DMA_PACKET_MAX_NUM  =
                    fifo_init_ptr->TX_DMA_PACKET_MAX_NUM;
        fifo_node_init.TX_DMA_PACKET_SIZE     =
                    fifo_init_ptr->TX_DMA_PACKET_SIZE;
        fifo_node_init.TX_FIFO_WM   = fifo_init_ptr->TX_FIFO_WM;
        fifo_node_init.SYN_MODE = fifo_init_ptr->SYN_OPERATE_MODE;

        ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] init esai fifo node\n");
        result = esai_fifo_node_init(&(esai_fifo_ptr->ESAI_NODE),
            &fifo_node_init);

        if (result != 0) {
            ESAI_FIFO_VYBRID_ERROR("[ESAI VPORT] fail to init esai fifo node\n");
            return MQX_IO_OPERATION_NOT_AVAILABLE;
        }

        result = _vybrid_esai_vport_init_default_config(esai_fifo_ptr,
            fifo_init_ptr->TX_DEFAULT_SLOT_WIDTH,
            fifo_init_ptr->RX_DEFAULT_SLOT_WIDTH,
            fifo_init_ptr->TX_DEFAULT_CLOCK_MASTER,
            fifo_init_ptr->RX_DEFAULT_CLOCK_MASTER);

        if (result != MQX_OK) {
            ESAI_FIFO_VYBRID_ERROR("[ESAI VPORT] fail to init default config\n");
            return MQX_IO_OPERATION_NOT_AVAILABLE;
        }

        result = esai_fifo_node_tx_config(&(esai_fifo_ptr->ESAI_NODE),
            &(esai_fifo_ptr->TX_CONFIG));
        if (result != 0) {
            ESAI_FIFO_VYBRID_ERROR("[ESAI VPORT] fail to set tx config\n");
            return MQX_IO_OPERATION_NOT_AVAILABLE;
        }

        result = esai_fifo_node_rx_config(&(esai_fifo_ptr->ESAI_NODE),
            &(esai_fifo_ptr->RX_CONFIG));
        if (result != 0) {
            ESAI_FIFO_VYBRID_ERROR("[ESAI VPORT] fail to set rx config\n");
            return MQX_IO_OPERATION_NOT_AVAILABLE;
        }

        ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] Setup ESAI FIFO ports\n");
        _vybrid_esai_vport_setup_fifo_ports(esai_fifo_ptr);
    }

    return MQX_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_esai_vport_mono_to_stereo
* Returned Value   : MQX error codes
* Comments         : convert chnl type from mono to stereo
*
*END*********************************************************************/
static int32_t _vybrid_esai_vport_mono_to_stereo
(
    /* [IN] src buffer */
    void *     src,

    /* [IN] src buffer size */
    uint32_t     src_size,

    /* [OUT] dst buffer */
    void *     dst,

    /* [OUT] dst buffer size */
    uint32_t     dst_size,

    /*[IN] word length*/
    uint32_t     wl
)
{
    uint32_t i = 0, n = 0;
    void * dptr = NULL, *sptr = NULL;

    if (dst_size < (src_size * 2)) {
        ESAI_FIFO_VYBRID_ERROR("%s : dst buf too small, ds[%d], ss[%d]\n",
                 __FUNCTION__, dst_size, src_size);
        return MQX_INVALID_SIZE;
    }

    n = src_size / wl;

    dptr = dst;
    sptr = src;

    for (i = 0; i < n; i++) {
        _mem_copy(sptr, dptr, wl);
        dptr = (char*) dptr + wl;
        _mem_copy(sptr, dptr, wl);
        dptr = (char*) dptr + wl;
        sptr = (char*) sptr + wl;
    }

    return MQX_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_esai_vport_stereo_to_mono
* Returned Value   : MQX error codes
* Comments         : convert chnl type from stereo to mono
*
*END*********************************************************************/
static int32_t _vybrid_esai_vport_stereo_to_mono
(
    /* [IN] src buffer */
    void *     src,

    /* [IN] src buffer size */
    uint32_t     src_size,

    /* [OUT] dst buffer */
    void *     dst,

    /* [OUT] dst buffer size */
    uint32_t     dst_size,

    /*[IN] word length*/
    uint32_t     wl
)
{
    uint32_t i = 0, n = 0;
    void * dptr = NULL, *sptr = NULL;

    if (dst_size < (src_size / 2)) {
        ESAI_FIFO_VYBRID_ERROR("%s : dst buf too small, ds[%d], ss[%d]\n",
            __FUNCTION__, dst_size, src_size);
        return MQX_OUT_OF_MEMORY;
    }

    n = src_size / wl;

    dptr = dst;
    sptr = src;

    for (i = 0; i < n; i++) {
        _mem_copy(sptr,dptr,wl);
        dptr = (char*) dptr + wl;
        dptr = (char*) dptr + wl;
        sptr = (char*) sptr + wl;
    }

    return MQX_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_esai_vport_init
* Returned Value   : MQX error code
* Comments         :
*    This function initializes an ESAI device.
*
*END*********************************************************************/

uint32_t _vybrid_esai_vport_init
   (
      /* [IN] Initialization information for the device being opened */
      VYBRID_ESAI_VPORT_INIT_STRUCT_PTR   io_init_ptr,

      /* [OUT] Address to store device specific information */
      void * *                 io_info_ptr_ptr,

      /* [IN] The rest of the name of the device opened */
      char *                      open_name_ptr,

      /* [IN] the address of the fifo node context */
      ESAI_FIFO_CONTEXT_STRUCT_PTR         esai_fifo_ptr
   )
{ /* Body */
    VYBRID_ESAI_VPORT_INFO_STRUCT_PTR      io_info_ptr;

    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] _vybrid_esai_vport_init, TR_ID %d\n",
        io_init_ptr->TRANSCEIVER_ID);

    io_info_ptr = (VYBRID_ESAI_VPORT_INFO_STRUCT_PTR)_mem_alloc_system_zero (
                (uint32_t)sizeof (VYBRID_ESAI_VPORT_INFO_STRUCT));
    if (NULL == io_info_ptr) {
      return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type (io_info_ptr, MEM_TYPE_IO_ESAI_VPORT_INFO_STRUCT);

    *io_info_ptr_ptr = io_info_ptr;

    io_info_ptr->INIT = *io_init_ptr;
    io_info_ptr->CHL_NUM = ESAI_VPORT_CHNL_STEREO;
    io_info_ptr->WORD_LENGTH = ESAI_VPORT_DW_16BITS;
    io_info_ptr->ASRC_8BITS_CONV = FALSE;
    io_info_ptr->ERR_CODE = 0;
    _mutex_init(&io_info_ptr->ESAI_MUTEX, NULL);

    _mutex_lock(&esai_fifo_ptr->CONTEXT_MUTEX);
    _vybrid_esai_vport_connect_fifo(io_info_ptr, esai_fifo_ptr);

    if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_TX)
        esai_fifo_ptr->TX_CONNECT_COUNT++;
    else
        esai_fifo_ptr->RX_CONNECT_COUNT++;
    _mutex_unlock(&esai_fifo_ptr->CONTEXT_MUTEX);

    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] FIFO CONTEXT: \n");
    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] TX_CONNECT_COUNT: %d\n",
        esai_fifo_ptr->TX_CONNECT_COUNT);
    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] RX_CONNECT_COUNT: %d\n",
        esai_fifo_ptr->RX_CONNECT_COUNT);
    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] TX_BITMAP: 0x%x\n",
        esai_fifo_ptr->TX_BITMAP);
    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] RX_BITMAP: 0x%x\n",
        esai_fifo_ptr->RX_BITMAP);
    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] TX_EN_BITMAP: 0x%x\n",
        esai_fifo_ptr->TX_ENABLE_BITMAP);
    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] RX_EN_BITMAP: 0x%x\n",
        esai_fifo_ptr->RX_ENABLE_BITMAP);
    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] TX_DEVS_COUNT: %d\n",
        esai_fifo_ptr->TX_DEVS_COUNT);
    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] RX_DEVS_COUNT: %d\n",
        esai_fifo_ptr->RX_DEVS_COUNT);


    return MQX_OK;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_esai_vport_deinit
* Returned Value   : MQX error code
* Comments         :
*    This function de-initializes an ESAI device.
*
*END*********************************************************************/

uint32_t _vybrid_esai_vport_deinit
   (
      /* [IN] the initialization information for the device being opened */
      IO_ESAI_VPORT_DEVICE_STRUCT_PTR pol_io_dev_ptr,

      /* [IN] the address of the device specific information */
      VYBRID_ESAI_VPORT_INFO_STRUCT_PTR     io_info_ptr
   )
{ /* Body */

    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] _vybrid_esai_vport_deinit TR_ID %d\n",
        io_info_ptr->INIT.TRANSCEIVER_ID);

    if ((NULL == io_info_ptr) || (NULL == pol_io_dev_ptr))
    {
      return ESAI_VPORT_ERROR_INVALID_PARAMETER;
    }

    /*disable corresponding DIM port*/
    _vybrid_esai_vport_ioctl(pol_io_dev_ptr,
        IO_IOCTL_ESAI_VPORT_STOP, NULL);

    /* Free info struct */
    _mutex_destroy(&io_info_ptr->ESAI_MUTEX);
    _mem_free (pol_io_dev_ptr->DEV_INFO_PTR);
    pol_io_dev_ptr->DEV_INFO_PTR = NULL;

    _mutex_lock(&(pol_io_dev_ptr->FIFO_CONTEXT_PTR->CONTEXT_MUTEX));
    if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_TX)
        pol_io_dev_ptr->FIFO_CONTEXT_PTR->TX_CONNECT_COUNT--;
    else
        pol_io_dev_ptr->FIFO_CONTEXT_PTR->RX_CONNECT_COUNT--;

    if (pol_io_dev_ptr->FIFO_CONTEXT_PTR->TX_CONNECT_COUNT == 0 &&
        pol_io_dev_ptr->FIFO_CONTEXT_PTR->RX_CONNECT_COUNT == 0) {

        ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] deinit esai fifo node\n");
        esai_fifo_node_deinit(GET_ESAI_NODE_PTR(
            pol_io_dev_ptr->FIFO_CONTEXT_PTR));
    }

    _mutex_unlock(&(pol_io_dev_ptr->FIFO_CONTEXT_PTR->CONTEXT_MUTEX));

    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] FIFO CONTEXT: \n");
    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] TX_CONNECT_COUNT: %d\n",
        pol_io_dev_ptr->FIFO_CONTEXT_PTR->TX_CONNECT_COUNT);
    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] RX_CONNECT_COUNT: %d\n",
        pol_io_dev_ptr->FIFO_CONTEXT_PTR->RX_CONNECT_COUNT);
    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] TX_BITMAP: 0x%x\n",
        pol_io_dev_ptr->FIFO_CONTEXT_PTR->TX_BITMAP);
    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] RX_BITMAP: 0x%x\n",
        pol_io_dev_ptr->FIFO_CONTEXT_PTR->RX_BITMAP);
    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] TX_EN_BITMAP: 0x%x\n",
        pol_io_dev_ptr->FIFO_CONTEXT_PTR->TX_ENABLE_BITMAP);
    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] RX_EN_BITMAP: 0x%x\n",
        pol_io_dev_ptr->FIFO_CONTEXT_PTR->RX_ENABLE_BITMAP);
    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] TX_DEVS_COUNT: %d\n",
        pol_io_dev_ptr->FIFO_CONTEXT_PTR->TX_DEVS_COUNT);
    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] RX_DEVS_COUNT: %d\n",
        pol_io_dev_ptr->FIFO_CONTEXT_PTR->RX_DEVS_COUNT);

    return MQX_OK;

} /* Endbody */


static uint32_t _vybrid_esai_vport_start
   (
      /* [IN] the initialization information for the device being opened */
      IO_ESAI_VPORT_DEVICE_STRUCT_PTR pol_io_dev_ptr
   )
{
    uint32_t result = MQX_OK;
    uint32_t ret = 0;

    ESAI_NODE_STATE fifo_node_state;
    uint32_t start_param;

    VYBRID_ESAI_VPORT_INFO_STRUCT_PTR io_info_ptr = NULL;
    ESAI_FIFO_CONTEXT_STRUCT_PTR esai_fifo_context_ptr;
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node_ptr = NULL;

    io_info_ptr = pol_io_dev_ptr->DEV_INFO_PTR;
    esai_fifo_context_ptr = pol_io_dev_ptr->FIFO_CONTEXT_PTR;
    esai_fifo_node_ptr = GET_ESAI_NODE_PTR(esai_fifo_context_ptr);

    _mutex_lock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
    if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_TX) {

        if (esai_fifo_context_ptr->TX_ENABLE_BITMAP
            & (1 << io_info_ptr->INIT.TRANSCEIVER_ID)){
            _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            return MQX_OK;
        }

        fifo_node_state =
            esai_fifo_node_get_tx_state(esai_fifo_node_ptr);
        if (0 == esai_fifo_context_ptr->TX_ENABLE_BITMAP
            && ESAI_STATE_RUN != fifo_node_state) {

            ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] fifo node start TX\n");
            ret = esai_fifo_node_start(esai_fifo_node_ptr,
                (void *)AUD_IO_FW_DIR_TX);

            if (ret != 0) {
                result = MQX_EINVAL;
            }
        }

        if (MQX_OK == result)
            esai_fifo_context_ptr->TX_ENABLE_BITMAP |=
                        (1 << io_info_ptr->INIT.TRANSCEIVER_ID);

    } else {

        if (esai_fifo_context_ptr->RX_ENABLE_BITMAP
            & (1 << io_info_ptr->INIT.TRANSCEIVER_ID)){
            _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            return MQX_OK;
        }

        if (esai_fifo_context_ptr->RX_ENABLE_BITMAP == 0) {
            start_param = AUD_IO_FW_DIR_RX;
            if (esai_fifo_context_ptr->INIT.SYN_OPERATE_MODE) {
                fifo_node_state =
                    esai_fifo_node_get_tx_state(esai_fifo_node_ptr);
                if (ESAI_STATE_RUN != fifo_node_state) {
                    start_param = AUD_IO_FW_DIR_FULL_DUPLEX;
                    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] SYNC, TX on\n");
                }
            }

            ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] fifo node start RX\n");
            ret = esai_fifo_node_start(esai_fifo_node_ptr,
                (void *)start_param);

            if (ret != 0) {
                result = MQX_EINVAL;
            }

            if (MQX_OK == result)
                esai_fifo_context_ptr->RX_ENABLE_BITMAP |=
                        (1 << io_info_ptr->INIT.TRANSCEIVER_ID);
        }
    }
    _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);

    return result;
}

static uint32_t _vybrid_esai_vport_stop
   (
      /* [IN] the initialization information for the device being opened */
      IO_ESAI_VPORT_DEVICE_STRUCT_PTR pol_io_dev_ptr
   )
{
    uint32_t result = MQX_OK;
    uint32_t ret = 0;

    uint8_t enable_bitmap;
    uint32_t start_param;

    VYBRID_ESAI_VPORT_INFO_STRUCT_PTR io_info_ptr = NULL;
    ESAI_FIFO_CONTEXT_STRUCT_PTR esai_fifo_context_ptr;
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node_ptr = NULL;

    io_info_ptr = pol_io_dev_ptr->DEV_INFO_PTR;
    esai_fifo_context_ptr = pol_io_dev_ptr->FIFO_CONTEXT_PTR;
    esai_fifo_node_ptr = GET_ESAI_NODE_PTR(esai_fifo_context_ptr);

    _mutex_lock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
    if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_TX) {

        if (!(esai_fifo_context_ptr->TX_ENABLE_BITMAP
                & (1 << io_info_ptr->INIT.TRANSCEIVER_ID))){
            _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            return MQX_OK;
        }

        enable_bitmap = esai_fifo_context_ptr->TX_ENABLE_BITMAP;
        enable_bitmap &= ~(1 << io_info_ptr->INIT.TRANSCEIVER_ID);

        if (0 == enable_bitmap) {
            if (!(esai_fifo_context_ptr->INIT.SYN_OPERATE_MODE
                && esai_fifo_context_ptr->RX_ENABLE_BITMAP > 0)) {

                ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] node stop TX\n");
                ret = esai_fifo_node_stop(esai_fifo_node_ptr,
                    (void *)AUD_IO_FW_DIR_TX);
                if (ret != 0) {
                    result = MQX_EINVAL;
                }
            }
        }

        if (MQX_OK == result)
            esai_fifo_context_ptr->TX_ENABLE_BITMAP = enable_bitmap;
    } else {

        if (!(esai_fifo_context_ptr->RX_ENABLE_BITMAP
                & (1 << io_info_ptr->INIT.TRANSCEIVER_ID))){
            _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            return MQX_OK;
        }

        enable_bitmap = esai_fifo_context_ptr->RX_ENABLE_BITMAP;
        enable_bitmap &= ~(1 << io_info_ptr->INIT.TRANSCEIVER_ID);

        if (0 == enable_bitmap) {
            start_param = AUD_IO_FW_DIR_RX;
            if (esai_fifo_context_ptr->INIT.SYN_OPERATE_MODE
                && esai_fifo_context_ptr->TX_ENABLE_BITMAP == 0) {
                start_param = AUD_IO_FW_DIR_FULL_DUPLEX;
                ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] SYNC, TX off\n");
            }

            ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] fifo node stop RX\n");
            ret = esai_fifo_node_stop(esai_fifo_node_ptr,
                (void *)start_param);
            if (ret != 0) {
                result = MQX_EINVAL;
            }
        }

        if (MQX_OK == result)
            esai_fifo_context_ptr->RX_ENABLE_BITMAP = enable_bitmap;
    }
    _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);

    return result;
}


static uint32_t _vybrid_esai_vport_config
   (
      /* [IN] the initialization information for the device being opened */
      IO_ESAI_VPORT_DEVICE_STRUCT_PTR pol_io_dev_ptr,

      /* [IN] the pointer to configuration structure containg all settings to ESAI */
      ESAI_VPORT_CONFIG_STRUCT_PTR vport_config_ptr
   )
{
    uint32_t result = MQX_OK;
    uint32_t ret = 0;

    ESAI_FIFO_CONFIG_STRUCT fifo_config;

    VYBRID_ESAI_VPORT_INFO_STRUCT_PTR io_info_ptr = NULL;
    ESAI_FIFO_CONTEXT_STRUCT_PTR esai_fifo_context_ptr;
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node_ptr = NULL;

    io_info_ptr = pol_io_dev_ptr->DEV_INFO_PTR;
    esai_fifo_context_ptr = pol_io_dev_ptr->FIFO_CONTEXT_PTR;
    esai_fifo_node_ptr = GET_ESAI_NODE_PTR(esai_fifo_context_ptr);

    _mutex_lock(&(pol_io_dev_ptr->FIFO_CONTEXT_PTR->CONTEXT_MUTEX));
    if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_TX) {

        ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] chnl_type %d, "
            "data_width = %d, transceiver id %d\n",
            vport_config_ptr->chnl_type, vport_config_ptr->data_width,
            io_info_ptr->INIT.TRANSCEIVER_ID);

        /*configure the word length and channel num to ESAI FIFO*/
        _mem_copy(&(esai_fifo_context_ptr->TX_CONFIG), &fifo_config,
                sizeof(ESAI_FIFO_CONFIG_STRUCT));

        switch (vport_config_ptr->data_width) {
            case ESAI_VPORT_DW_8BITS:
                fifo_config.pcm_fmt = ESAI_FIFO_PCM_S8_LE;
                break;
            case ESAI_VPORT_DW_16BITS:
                fifo_config.pcm_fmt = ESAI_FIFO_PCM_S16_LE;
                break;
            case ESAI_VPORT_DW_24BITS:
                fifo_config.pcm_fmt = ESAI_FIFO_PCM_S24_LE;
                break;
            case ESAI_VPORT_DW_32BITS:
                fifo_config.pcm_fmt = ESAI_FIFO_PCM_S24_PACK_LE;
                break;
            default:
                _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
                return ESAI_VPORT_ERROR_INVALID_PARAMETER;
        }
        fifo_config.asrc_8bit_covert =
            vport_config_ptr->asrc_8bit_covert;

        ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] Config fifo node tx\n");
        ret = esai_fifo_node_tx_config(esai_fifo_node_ptr,
            &fifo_config);
        if (ret != 0) {
            result = MQX_EINVAL;
        } else {
            /*update recorded config*/
            _mem_copy(&fifo_config,
                &(esai_fifo_context_ptr->TX_CONFIG),
                sizeof(ESAI_FIFO_CONFIG_STRUCT));
        }

        io_info_ptr->CHL_NUM = vport_config_ptr->chnl_type;
        io_info_ptr->WORD_LENGTH= vport_config_ptr->data_width;
        io_info_ptr->ASRC_8BITS_CONV =
            vport_config_ptr->asrc_8bit_covert;

    } else {
        /*configure the word length and channel num to ESAI FIFO*/
        _mem_copy(&(esai_fifo_context_ptr->RX_CONFIG), &fifo_config,
                sizeof(ESAI_FIFO_CONFIG_STRUCT));

        switch (vport_config_ptr->data_width) {
            case ESAI_VPORT_DW_8BITS:
                fifo_config.pcm_fmt = ESAI_FIFO_PCM_S8_LE;
                break;
            case ESAI_VPORT_DW_16BITS:
                fifo_config.pcm_fmt = ESAI_FIFO_PCM_S16_LE;
                break;
            case ESAI_VPORT_DW_24BITS:
                fifo_config.pcm_fmt = ESAI_FIFO_PCM_S24_LE;
                break;
            case ESAI_VPORT_DW_32BITS:
                fifo_config.pcm_fmt = ESAI_FIFO_PCM_S24_PACK_LE;
                break;
            default:
                _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
                return ESAI_VPORT_ERROR_INVALID_PARAMETER;
        }
        fifo_config.asrc_8bit_covert =
            vport_config_ptr->asrc_8bit_covert;

        ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] Config fifo node rx\n");
        ret = esai_fifo_node_rx_config(esai_fifo_node_ptr,
            &fifo_config);
        if (ret != 0) {
            result = MQX_EINVAL;
        } else {
            /*update recorded config*/
            _mem_copy(&fifo_config,
                &(esai_fifo_context_ptr->RX_CONFIG),
                sizeof(ESAI_FIFO_CONFIG_STRUCT));
        }

        io_info_ptr->CHL_NUM = vport_config_ptr->chnl_type;
        io_info_ptr->WORD_LENGTH= vport_config_ptr->data_width;
        io_info_ptr->ASRC_8BITS_CONV =
            vport_config_ptr->asrc_8bit_covert;
    }
    _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);

    return result;

}

static uint32_t _vybrid_esai_vport_set_sample_rate
   (
      /* [IN] the initialization information for the device being opened */
      IO_ESAI_VPORT_DEVICE_STRUCT_PTR pol_io_dev_ptr,

      /* [IN] the sample rate value applied to ESAI VPORT*/
      uint32_t sample_rate
   )
{
    uint32_t result = MQX_OK;
    uint32_t ret = 0;

    ESAI_FIFO_CONFIG_STRUCT fifo_config;
    ESAI_NODE_STATE fifo_node_state;

    VYBRID_ESAI_VPORT_INFO_STRUCT_PTR io_info_ptr = NULL;
    ESAI_FIFO_CONTEXT_STRUCT_PTR esai_fifo_context_ptr;
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node_ptr = NULL;

    io_info_ptr = pol_io_dev_ptr->DEV_INFO_PTR;
    esai_fifo_context_ptr = pol_io_dev_ptr->FIFO_CONTEXT_PTR;
    esai_fifo_node_ptr = GET_ESAI_NODE_PTR(esai_fifo_context_ptr);

    _mutex_lock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
    if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_TX ||
        esai_fifo_context_ptr->INIT.SYN_OPERATE_MODE) {

        _mem_copy(&(esai_fifo_context_ptr->TX_CONFIG), &fifo_config,
                        sizeof(ESAI_FIFO_CONFIG_STRUCT));

        if (fifo_config.clock.clock_direction != ESAI_CLOCK_MASTER ||
            fifo_config.clock.output_refer_clk != ESAI_CLOCK_EXTAL) {
            _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            return ESAI_VPORT_ERROR_INVALID_PARAMETER;
        }

        fifo_node_state =
            esai_fifo_node_get_tx_state(esai_fifo_node_ptr);

        if (ESAI_STATE_RUN != fifo_node_state) {
            fifo_config.clock.timing.extal_para.frame_rate =
                sample_rate;

            ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] Set sample rate %d"
                "to fifo node\n",
                fifo_config.clock.timing.extal_para.frame_rate);
            ret = esai_fifo_node_tx_config(esai_fifo_node_ptr,
                &fifo_config);
            if (ESAI_NODE_OK != ret) {
                ESAI_FIFO_VYBRID_ERROR("[ESAI VPORT]Set tx sample rate"
                    "ERROR\n");
                result = ESAI_VPORT_ERROR_INTERNAL_ERROR;
            } else {
                _mem_copy(&fifo_config,
                    &(esai_fifo_context_ptr->TX_CONFIG),
                    sizeof(ESAI_FIFO_CONFIG_STRUCT));
            }
        } else {
            ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] current sample rate %d"
                ", new sample rate %d\n",
                fifo_config.clock.timing.extal_para.frame_rate,
                sample_rate);

            if (fifo_config.clock.timing.extal_para.frame_rate
                == sample_rate)
                result = ESAI_VPORT_OK;
            else
                result = MQX_EINVAL;
        }

    }

    if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_RX &&
        MQX_OK == result) {

        _mem_copy(&(esai_fifo_context_ptr->RX_CONFIG), &fifo_config,
                        sizeof(ESAI_FIFO_CONFIG_STRUCT));

        if (fifo_config.clock.clock_direction != ESAI_CLOCK_MASTER ||
            fifo_config.clock.output_refer_clk != ESAI_CLOCK_EXTAL) {
            _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            return ESAI_VPORT_ERROR_INVALID_PARAMETER;
        }

        fifo_node_state =
            esai_fifo_node_get_rx_state(esai_fifo_node_ptr);
        if (ESAI_STATE_RUN != fifo_node_state) {

            fifo_config.clock.timing.extal_para.frame_rate =
                sample_rate;

            ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] Set sample rate %d"
                "to fifo node\n",
                fifo_config.clock.timing.extal_para.frame_rate);

            ret = esai_fifo_node_rx_config(esai_fifo_node_ptr,
                &fifo_config);
            if (ESAI_NODE_OK != ret) {
                ESAI_FIFO_VYBRID_ERROR("[ESAI VPORT]set rx sample rate"
                    "ERROR\n");
                result = ESAI_VPORT_ERROR_INTERNAL_ERROR;
            } else {
                _mem_copy(&fifo_config,
                    &(esai_fifo_context_ptr->RX_CONFIG),
                    sizeof(ESAI_FIFO_CONFIG_STRUCT));
            }
        } else {
            if (fifo_config.clock.timing.extal_para.frame_rate
                == sample_rate)
                result = ESAI_VPORT_OK;
            else
                result = MQX_EINVAL;
        }

    }
    _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);

    return result;
}


static uint32_t _vybrid_esai_vport_set_aud_interface
   (
      /* [IN] the initialization information for the device being opened */
      IO_ESAI_VPORT_DEVICE_STRUCT_PTR pol_io_dev_ptr,

      /* [IN] the audio HW interface enum applied to ESAI VPORT*/
      ESAI_VPORT_HW_INTERFACE vport_hw_inf
   )
{

    uint32_t result = MQX_OK;
    uint32_t ret = 0;

    ESAI_FIFO_CONFIG_STRUCT fifo_config;
    ESAI_NODE_STATE fifo_node_state;

    VYBRID_ESAI_VPORT_INFO_STRUCT_PTR io_info_ptr = NULL;
    ESAI_FIFO_CONTEXT_STRUCT_PTR esai_fifo_context_ptr;
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node_ptr = NULL;

    io_info_ptr = pol_io_dev_ptr->DEV_INFO_PTR;
    esai_fifo_context_ptr = pol_io_dev_ptr->FIFO_CONTEXT_PTR;
    esai_fifo_node_ptr = GET_ESAI_NODE_PTR(esai_fifo_context_ptr);

    _mutex_lock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
    if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_TX ||
        esai_fifo_context_ptr->INIT.SYN_OPERATE_MODE) {

        fifo_node_state =
            esai_fifo_node_get_tx_state(esai_fifo_node_ptr);

        if (ESAI_STATE_RUN != fifo_node_state) {
            _mem_copy(&(esai_fifo_context_ptr->TX_CONFIG), &fifo_config,
                sizeof(ESAI_FIFO_CONFIG_STRUCT));

            switch (vport_hw_inf) {
                case ESAI_VPORT_HW_INF_I2S:
                    fifo_config.inf_fmt.fmt = ESAI_MODULE_FMT_I2S;
                    fifo_config.clock.clock_polarity =
                        ESAI_CLOCK_POLAR_NB_IF;
                    break;
                case ESAI_VPORT_HW_INF_LEFTJ:
                    fifo_config.inf_fmt.fmt = ESAI_MODULE_FMT_LEFT_J;
                    fifo_config.clock.clock_polarity =
                        ESAI_CLOCK_POLAR_NB_NF;
                    break;
                case ESAI_VPORT_HW_INF_TDM:
                    fifo_config.inf_fmt.fmt = ESAI_MODULE_FMT_TDM;
                    fifo_config.clock.clock_polarity =
                        ESAI_CLOCK_POLAR_NB_NF;
                    fifo_config.inf_fmt.fmt_para.tdm_para.slots = 8;
                    fifo_config.inf_fmt.fmt_para.tdm_para.mask =
                        esai_fifo_context_ptr->TX_BITMAP;
                    break;
                default:
                    _mutex_unlock(
                        &esai_fifo_context_ptr->CONTEXT_MUTEX);
                    ESAI_FIFO_VYBRID_ERROR("Unsupported protocol %d!\n",
                        vport_hw_inf);
                    return ESAI_VPORT_ERROR_INVALID_PARAMETER;
            }

            ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] set hw inf to node\n");
            ret = esai_fifo_node_tx_config(
                &(esai_fifo_context_ptr->ESAI_NODE), &fifo_config);
            if (ret != 0) {
                ESAI_FIFO_VYBRID_ERROR("[ESAI VPORT]config fifo node"
                    "tx ERROR\n");
                result = ESAI_VPORT_ERROR_INTERNAL_ERROR;
            }

            _mem_copy(&fifo_config, &(esai_fifo_context_ptr->TX_CONFIG),
                      sizeof(ESAI_FIFO_CONFIG_STRUCT));

        }else {

            if ((vport_hw_inf == ESAI_VPORT_HW_INF_I2S &&
                esai_fifo_context_ptr->TX_CONFIG.inf_fmt.fmt !=
                    ESAI_MODULE_FMT_I2S) ||
                (vport_hw_inf == ESAI_VPORT_HW_INF_LEFTJ &&
                esai_fifo_context_ptr->TX_CONFIG.inf_fmt.fmt !=
                    ESAI_MODULE_FMT_LEFT_J) ||
                (vport_hw_inf == ESAI_VPORT_HW_INF_TDM &&
                esai_fifo_context_ptr->TX_CONFIG.inf_fmt.fmt !=
                    ESAI_MODULE_FMT_TDM)) {
                result = ESAI_VPORT_ERROR_INTERNAL_ERROR;
            }
        }
    }

    if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_RX &&
        MQX_OK == result) {

        fifo_node_state =
            esai_fifo_node_get_rx_state(esai_fifo_node_ptr);

        if (ESAI_STATE_RUN != fifo_node_state) {

            /*setup hw interfaces protocol for RX*/
            _mem_copy(&(esai_fifo_context_ptr->RX_CONFIG), &fifo_config,
                      sizeof(ESAI_FIFO_CONFIG_STRUCT));

            switch (vport_hw_inf)    {

            case    ESAI_VPORT_HW_INF_I2S:
                    fifo_config.inf_fmt.fmt = ESAI_MODULE_FMT_I2S;
                    fifo_config.clock.clock_polarity =
                        ESAI_CLOCK_POLAR_NB_IF;
                    break;

            case    ESAI_VPORT_HW_INF_LEFTJ:
                    fifo_config.inf_fmt.fmt = ESAI_MODULE_FMT_LEFT_J;
                    fifo_config.clock.clock_polarity =
                        ESAI_CLOCK_POLAR_NB_NF;
                    break;

            case    ESAI_VPORT_HW_INF_TDM:
                    fifo_config.inf_fmt.fmt = ESAI_MODULE_FMT_TDM;
                    fifo_config.clock.clock_polarity =
                        ESAI_CLOCK_POLAR_NB_NF;
                    fifo_config.inf_fmt.fmt_para.tdm_para.slots = 8;
                    fifo_config.inf_fmt.fmt_para.tdm_para.mask =
                        esai_fifo_context_ptr->RX_BITMAP;
                    break;

            default:
                    _mutex_unlock(
                        &esai_fifo_context_ptr->CONTEXT_MUTEX);
                    ESAI_FIFO_VYBRID_ERROR("Unsupported protocol %d!\n",
                        vport_hw_inf);
                    return ESAI_VPORT_ERROR_INVALID_PARAMETER;
            }

            ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] set hw inf to node\n");
            ret = esai_fifo_node_rx_config(
                &(esai_fifo_context_ptr->ESAI_NODE), &fifo_config);
            if (ret != 0) {
                ESAI_FIFO_VYBRID_ERROR("[ESAI VPORT]config fifo node"
                    "RX ERROR\n");
                result = ESAI_VPORT_ERROR_INTERNAL_ERROR;
            }

            _mem_copy(&fifo_config, &(esai_fifo_context_ptr->RX_CONFIG),
                      sizeof(ESAI_FIFO_CONFIG_STRUCT));

        } else {

            if ((vport_hw_inf == ESAI_VPORT_HW_INF_I2S &&
                esai_fifo_context_ptr->RX_CONFIG.inf_fmt.fmt !=
                    ESAI_MODULE_FMT_I2S) ||
                (vport_hw_inf == ESAI_VPORT_HW_INF_LEFTJ &&
                esai_fifo_context_ptr->RX_CONFIG.inf_fmt.fmt !=
                    ESAI_MODULE_FMT_LEFT_J) ||
                (vport_hw_inf == ESAI_VPORT_HW_INF_TDM &&
                esai_fifo_context_ptr->RX_CONFIG.inf_fmt.fmt !=
                    ESAI_MODULE_FMT_TDM)) {
                result = ESAI_VPORT_ERROR_INTERNAL_ERROR;
            }
        }
    }
    _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);

    return result;
}

/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_esai_vport_ioctl
* Returned Value   : MQX error code.
* Comments         :
*    This function performs miscellaneous services for
*    the ESAI I/O device.
*
*END*********************************************************************/

uint32_t _vybrid_esai_vport_ioctl
   (
      /* [IN] the initialization information for the device being opened */
      IO_ESAI_VPORT_DEVICE_STRUCT_PTR pol_io_dev_ptr,

      /* [IN] The command to perform */
      uint32_t                     cmd,

      /* [IN] Parameters for the command */
      uint32_t *                 param_ptr
   )
{ /* Body */

    uint32_t result = MQX_OK;
    uint32_t ret = 0;
    VYBRID_ESAI_VPORT_INFO_STRUCT_PTR io_info_ptr = NULL;

    ESAI_FIFO_CONTEXT_STRUCT_PTR esai_fifo_context_ptr;
    ESAI_FIFO_AUD_IO_NODE_STRUCT_PTR esai_fifo_node_ptr = NULL;

    io_info_ptr = pol_io_dev_ptr->DEV_INFO_PTR;
    esai_fifo_context_ptr = pol_io_dev_ptr->FIFO_CONTEXT_PTR;
    esai_fifo_node_ptr = GET_ESAI_NODE_PTR(esai_fifo_context_ptr);

    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] _vybrid_esai_vport_ioctl, TR_ID %d\n",
        io_info_ptr->INIT.TRANSCEIVER_ID);

    switch (cmd)
    {
        case IO_IOCTL_ESAI_VPORT_START:
        {
            ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] IO_IOCTL_ESAI_VPORT_START\n");
            result = _vybrid_esai_vport_start(pol_io_dev_ptr);
            break;
        }

        case IO_IOCTL_ESAI_VPORT_STOP:
        {
            ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] IO_IOCTL_ESAI_FIFO_STOP\n");
            result = _vybrid_esai_vport_stop(pol_io_dev_ptr);
            break;
        }
        case IO_IOCTL_ESAI_VPORT_CONFIG:
        {
            ESAI_VPORT_CONFIG_STRUCT_PTR vport_config_ptr =
                (ESAI_VPORT_CONFIG_STRUCT_PTR)param_ptr;

            ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] "
                "IO_IOCTL_ESAI_VPORT_CONFIG\n");

            result = _vybrid_esai_vport_config(pol_io_dev_ptr,
                vport_config_ptr);
            break;
        }

        case IO_IOCTL_ESAI_VPORT_SET_SAMPLE_RATE:
        {
            uint32_t sample_rate = (uint32_t)param_ptr;
            ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] "
                "IO_IOCTL_ESAI_VPORT_SET_SAMPLE_RATE\n");

            result = _vybrid_esai_vport_set_sample_rate(pol_io_dev_ptr,
                sample_rate);
            break;
        }

        case IO_IOCTL_ESAI_VPORT_SET_HW_INTERFACE:
        {
            uint32_t temp = (uint32_t)param_ptr;
            ESAI_VPORT_HW_INTERFACE vport_hw_inf =
                (ESAI_VPORT_HW_INTERFACE)temp;

            ESAI_FIFO_VYBRID_DEBUG(
                "[ESAI VPORT] IO_IOCTL_ESAI_VPORT_SET_HW_INTERFACE\n");
            result = _vybrid_esai_vport_set_aud_interface(pol_io_dev_ptr,
                vport_hw_inf);

            break;
        }

        case IO_IOCTL_ESAI_VPORT_RESET:
        {
            ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] IO_IOCTL_ESAI_FIFO_RESET\n");

            _mutex_lock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_TX) {
                if (esai_fifo_context_ptr->TX_ENABLE_BITMAP
                        & (1<<io_info_ptr->INIT.TRANSCEIVER_ID)) {
                    result = MQX_EINVAL;
                }

                if (esai_fifo_context_ptr->TX_ENABLE_BITMAP == 0 &&
                    esai_fifo_context_ptr->RX_ENABLE_BITMAP == 0) {
                    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] fifo node reset\n");
                    ret = esai_fifo_reset(esai_fifo_node_ptr);
                    if (ret != 0) {
                        result = MQX_EINVAL;
                    }

                    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] TX DIM reset\n");
                }
            }else {
                if (esai_fifo_context_ptr->RX_ENABLE_BITMAP
                        & (1<<io_info_ptr->INIT.TRANSCEIVER_ID)) {
                    result = MQX_EINVAL;
                }

                if (esai_fifo_context_ptr->TX_ENABLE_BITMAP == 0 &&
                    esai_fifo_context_ptr->RX_ENABLE_BITMAP == 0) {
                    ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] fifo node reset\n");
                    ret = esai_fifo_reset(esai_fifo_node_ptr);
                    if (ret != 0) {
                        result = MQX_EINVAL;
                    }

                }
            }
            _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            break;
        }
        case IO_IOCTL_ESAI_VPORT_SET_ASRC:
        {
            ESAI_ASRC_DMA_STRUCT_PTR dma_cfg =
                (ESAI_ASRC_DMA_STRUCT_PTR)param_ptr;

            ESAI_FIFO_VYBRID_DEBUG("[ESAI VPORT] "
                "IO_IOCTL_ESAI_FIFO_SET_ASRC\n");

            _mutex_lock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_TX)
            {
                ret = easi_fifo_set_asrc_dma(esai_fifo_node_ptr,
                    dma_cfg->input_dma_channel,
                    dma_cfg->output_dma_channel);
                if (ret != 0) {
                    result = MQX_EINVAL;
                }
            } else {
                result = MQX_EINVAL;
            }
            _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            break;
        }

        case IO_IOCTL_ESAI_VPORT_CLEAR_BUF:
        {
            ESAI_FIFO_VYBRID_DEBUG(
                "[ESAI VPORT] IO_IOCTL_ESAI_VPORT_CLEAR_BUF\n");

            _mutex_lock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            /*Clear buffer only works for single port case*/
            if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_TX) 
            {
                ret = esai_fifo_node_clear_tx_buf(esai_fifo_node_ptr);
                if (ret != 0) {
                    result = MQX_EINVAL;
                }
            } else if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_RX)
            {
                ret = esai_fifo_node_clear_rx_buf(esai_fifo_node_ptr);
                if (ret != 0) {
                    result = MQX_EINVAL;
                }
            } else {
                result = MQX_EINVAL;
            }
            _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            break;
        }
        case IO_IOCTL_ESAI_VPORT_ENABLE_ASRC_PLUGIN:
        {
            int32_t source_sample_rate = (int32_t)param_ptr;

            ESAI_FIFO_VYBRID_DEBUG(
                "[ESAI VPORT] IO_IOCTL_ESAI_VPORT_ENABLE_ASRC_PLUGIN\n");

            _mutex_lock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_TX)
            {
                ret = esai_fifo_node_enable_asrc(esai_fifo_node_ptr, TRUE,
                    source_sample_rate);
                if (ret != 0) {
                    result = MQX_EINVAL;
                }
            } else {
                result = MQX_EINVAL;
            }
            _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            break;
        }
        case IO_IOCTL_ESAI_VPORT_DISABLE_ASRC_PLUGIN:
        {
            ESAI_FIFO_VYBRID_DEBUG(
                "[ESAI VPORT] IO_IOCTL_ESAI_VPORT_DISABLE_ASRC_PLUGIN\n");
            _mutex_lock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_TX)
            {
                ret = esai_fifo_node_enable_asrc(esai_fifo_node_ptr, FALSE,
                    0);
                if (ret != 0) {
                    result = MQX_EINVAL;
                }
            } else {
                result = MQX_EINVAL;
            }
            _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            break;
        }
        case IO_IOCTL_ESAI_VPORT_GET_ERROR:
        {
            ESAI_FIFO_VYBRID_DEBUG(
                "[ESAI VPORT] IO_IOCTL_ESAI_VPORT_GET_ERROR\n");
            _mutex_lock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            *param_ptr = io_info_ptr->ERR_CODE;
            _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            break;
        }
        case IO_IOCTL_ESAI_VPORT_SET_TX_TIMEOUT:
        {
            uint32_t timeout = (uint32_t)param_ptr;
            ESAI_FIFO_VYBRID_DEBUG(
                "[ESAI VPORT] IO_IOCTL_ESAI_VPORT_SET_TX_TIMEOUT\n");
            _mutex_lock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
             esai_fifo_node_set_tx_timeout(esai_fifo_node_ptr, timeout);
            _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            break;
        }
        case IO_IOCTL_ESAI_VPORT_SET_RX_TIMEOUT:
        {
            uint32_t timeout = (uint32_t)param_ptr;
            ESAI_FIFO_VYBRID_DEBUG(
                "[ESAI VPORT] IO_IOCTL_ESAI_VPORT_SET_RX_TIMEOUT\n");
            _mutex_lock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            esai_fifo_node_set_rx_timeout(esai_fifo_node_ptr, timeout);
            _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
            break;
        }
        default:
            result = IO_ERROR_INVALID_IOCTL_CMD;
            break;
    }

    return result;
} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_esai_vport_rx
* Returned Value   : number of bytes read
* Comments         :
*   Returns the number of bytes received.
*   Reads the data into provided array when data is available.
*
*END*********************************************************************/

int32_t _vybrid_esai_vport_rx
   (
      /* [IN] the address of the device specific information */
      IO_ESAI_VPORT_DEVICE_STRUCT_PTR pol_io_dev_ptr,

      /* [IN] The array to copy data into */
      unsigned char *                       buffer,

      /* [IN] number of bytes to read */
      int32_t                         length
   )
{ /* Body */
    VYBRID_ESAI_VPORT_INFO_STRUCT_PTR       io_info_ptr;
    unsigned char * convert_buf = NULL;
    unsigned char * temp_buf = NULL;
    int32_t ret;
    uint32_t error;
    ESAI_FIFO_CONTEXT_STRUCT_PTR esai_fifo_context_ptr;

    esai_fifo_context_ptr = pol_io_dev_ptr->FIFO_CONTEXT_PTR;
    io_info_ptr = pol_io_dev_ptr->DEV_INFO_PTR;
    io_info_ptr->ERR_CODE = IO_OK;

    _mutex_lock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
    if (!(esai_fifo_context_ptr->RX_ENABLE_BITMAP
                & (1 << io_info_ptr->INIT.TRANSCEIVER_ID))){
        _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
        return IO_ERROR;
    }
    _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);

    if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_TX) {
        io_info_ptr->ERR_CODE = IO_ERROR_DEVICE_INVALID;
        return -1;
    }

    _mutex_lock(&io_info_ptr->ESAI_MUTEX);
    if (io_info_ptr->CHL_NUM == 1) {
        convert_buf = _mem_alloc_system_zero(length<<1);
        if (NULL == convert_buf) {
            _mutex_unlock(&io_info_ptr->ESAI_MUTEX);
            ESAI_FIFO_VYBRID_ERROR("%s : failed to alloc\n",
                __FUNCTION__);
            io_info_ptr->ERR_CODE = IO_ERROR_READ;
            return IO_ERROR;
        }

        _mem_set_type(convert_buf, MEM_TYPE_IO_ESAI_VPORT_CONVERT_BUF);

        temp_buf = buffer;
        buffer = convert_buf;
        length = length<<1;
    }

    ret = esai_fifo_node_read(
        GET_ESAI_NODE_PTR(pol_io_dev_ptr->FIFO_CONTEXT_PTR),
        buffer, length);
    if (ret != length) {
        error = esai_fifo_node_rx_error(
            GET_ESAI_NODE_PTR(pol_io_dev_ptr->FIFO_CONTEXT_PTR));
        io_info_ptr->ERR_CODE = error;
    }

    if (ret > 0 && convert_buf) {

       buffer = temp_buf;
        _vybrid_esai_vport_stereo_to_mono(convert_buf, ret, buffer,
            (ret>>1), io_info_ptr->WORD_LENGTH);

        _mem_free(convert_buf);
        ret = (ret>>1);
    }

    _mutex_unlock(&io_info_ptr->ESAI_MUTEX);

    return ret;
} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_esai_vport_tx
* Returned Value   : number of bytes transmitted
* Comments         :
*   Writes the provided data buffer and loops until transmission complete.
*
*END*********************************************************************/

int32_t _vybrid_esai_vport_tx
   (
      /* [IN] the address of the device specific information */
      IO_ESAI_VPORT_DEVICE_STRUCT_PTR pol_io_dev_ptr,

      /* [IN] The array characters are to be read from */
      unsigned char *                       buffer,

      /* [IN] number of bytes to output */
      int32_t                         length
   )
{ /* Body */
    VYBRID_ESAI_VPORT_INFO_STRUCT_PTR       io_info_ptr;
    unsigned char * convert_buf = NULL;
    int32_t ret = 0;
    uint32_t error;
    ESAI_FIFO_CONTEXT_STRUCT_PTR esai_fifo_context_ptr;

    esai_fifo_context_ptr = pol_io_dev_ptr->FIFO_CONTEXT_PTR;
    io_info_ptr  = pol_io_dev_ptr->DEV_INFO_PTR;
    io_info_ptr->ERR_CODE = IO_OK;

    _mutex_lock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
    if (!(esai_fifo_context_ptr->TX_ENABLE_BITMAP
                & (1 << io_info_ptr->INIT.TRANSCEIVER_ID))){
        _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);
        return IO_ERROR;
    }
    _mutex_unlock(&esai_fifo_context_ptr->CONTEXT_MUTEX);

    if (io_info_ptr->INIT.PORT_DIRECTION == VPORT_DIR_RX) {
        io_info_ptr->ERR_CODE = IO_ERROR_DEVICE_INVALID;
        return IO_ERROR;
    }

    _mutex_lock(&io_info_ptr->ESAI_MUTEX);
    if (io_info_ptr->CHL_NUM == 1) {
        convert_buf = _mem_alloc_system_zero(length<<1);
        if (NULL == convert_buf) {
            _mutex_unlock(&io_info_ptr->ESAI_MUTEX);
            ESAI_FIFO_VYBRID_ERROR("%s : failed to alloc\n",
                __FUNCTION__);
            io_info_ptr->ERR_CODE = IO_ERROR_WRITE;
            return IO_ERROR;
        }

        _mem_set_type(convert_buf, MEM_TYPE_IO_ESAI_VPORT_CONVERT_BUF);

        _vybrid_esai_vport_mono_to_stereo(buffer, length, convert_buf,
            (length<<1), io_info_ptr->WORD_LENGTH);

        buffer = convert_buf;
        length = (length << 1);
    }

    ret = esai_fifo_node_write(
        GET_ESAI_NODE_PTR(pol_io_dev_ptr->FIFO_CONTEXT_PTR),
        buffer, length);
    if (ret != length) {
        error = esai_fifo_node_tx_error(
            GET_ESAI_NODE_PTR(pol_io_dev_ptr->FIFO_CONTEXT_PTR));
        io_info_ptr->ERR_CODE = error;
    }

    if (io_info_ptr->CHL_NUM == 1 && ret > 0)
        ret = ret / 2;

    if (convert_buf)
        _mem_free(convert_buf);

    _mutex_unlock(&io_info_ptr->ESAI_MUTEX);
    return ret;
} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_esai_vport_install
* Returned Value   : MQX error code
* Comments         :
*    Install an ESAI VPORT device.
*
*END*********************************************************************/

uint32_t _vybrid_esai_vport_install
   (
      /* [IN] A string that identifies the device for fopen */
      char *           identifier,

      /* [IN] The I/O init data pointer */
      VYBRID_ESAI_VPORT_INIT_STRUCT_CPTR            init_data_ptr,

      /* [IN] ESAI fifo context*/
      ESAI_FIFO_HANDLE                  esai_fifo_handle
   )
{ /* Body */
    uint32_t ret;
    ESAI_FIFO_CONTEXT_STRUCT_PTR esai_fifo_ptr =
        (ESAI_FIFO_CONTEXT_STRUCT_PTR)esai_fifo_handle;

    if (NULL == esai_fifo_ptr)
        return MQX_IO_OPERATION_NOT_AVAILABLE;

    ret = _io_esai_vport_install(identifier,
      (uint32_t (_CODE_PTR_)(void *, void * *, char *, void *))
      _vybrid_esai_vport_init,
      (uint32_t (_CODE_PTR_)(void *, void *))_vybrid_esai_vport_deinit,
      (int32_t (_CODE_PTR_)(void *, char *, int32_t))_vybrid_esai_vport_rx,
      (int32_t (_CODE_PTR_)(void *, char *, int32_t))_vybrid_esai_vport_tx,
      (int32_t (_CODE_PTR_)(void *, uint32_t, uint32_t *))
      _vybrid_esai_vport_ioctl,
      (void *)init_data_ptr,
#if DISCFG_OS_DRV_ENABLE_PCMMGR
      &vybrid_esai_vport_pcmm_funcs,
#else
      NULL,
#endif
      esai_fifo_ptr);

    if (ret != MQX_OK)
        return ret;

    /*update esai fifo context*/
    if (init_data_ptr->PORT_DIRECTION == VPORT_DIR_TX) {
        if ((esai_fifo_ptr->TX_BITMAP & (1 << init_data_ptr->TRANSCEIVER_ID))
            || (esai_fifo_ptr->RX_BITMAP
                & (1 << init_data_ptr->TRANSCEIVER_ID))) {
            /*someone has installed a ESAI VPORT device with same device id*/
            return MQX_IO_OPERATION_NOT_AVAILABLE;
        } else {
            esai_fifo_ptr->TX_BITMAP |= (1 << init_data_ptr->TRANSCEIVER_ID);
            esai_fifo_ptr->TX_DEVS_COUNT++;
        }
    } else {

        /*transceiver 0 and transceiver 1 only support TX*/
        if (init_data_ptr->TRANSCEIVER_ID == 0
            || init_data_ptr->TRANSCEIVER_ID == 1){
            return MQX_IO_OPERATION_NOT_AVAILABLE;
        }

        if ((esai_fifo_ptr->RX_BITMAP & (1 << init_data_ptr->TRANSCEIVER_ID))
            || (esai_fifo_ptr->TX_BITMAP
                & (1 << init_data_ptr->TRANSCEIVER_ID))) {
            /*someone has installed a ESAI VPORT device with same device id*/
            return MQX_IO_OPERATION_NOT_AVAILABLE;
        } else {
            esai_fifo_ptr->RX_BITMAP |= (1 << init_data_ptr->TRANSCEIVER_ID);
            esai_fifo_ptr->RX_DEVS_COUNT++;
        }
    }

    return MQX_OK;
} /* Endbody */

/*FUNCTION****************************************************************
*
* Function Name    : _vybrid_esai_fifo_install
* Returned Value   : MQX error code
* Comments         :
*    Create a esai fifo context which is used to associates all vports to ESAI FIFO node.
*    A Context handle will return from this function for following ESAI VPORTs installlation.
*
*END*********************************************************************/
ESAI_FIFO_HANDLE _vybrid_esai_fifo_install
(
    VYBRID_ESAI_FIFO_INIT_STRUCT_CPTR esai_fifo_init_ptr
)
{
    ESAI_FIFO_CONTEXT_STRUCT_PTR esai_fifo_context_ptr = NULL;

    esai_fifo_context_ptr = (ESAI_FIFO_CONTEXT_STRUCT_PTR)
        _mem_alloc_system_zero ((_mem_size)sizeof (ESAI_FIFO_CONTEXT_STRUCT));
   if (esai_fifo_context_ptr == NULL)
   {
      return NULL;
   }
   _mem_set_type (esai_fifo_context_ptr, MEM_TYPE_IO_ESAI_FIFO_CONTEXT);

   esai_fifo_context_ptr->INIT = *esai_fifo_init_ptr;

   _mutex_init(&esai_fifo_context_ptr->CONTEXT_MUTEX, NULL);

   return (ESAI_FIFO_HANDLE)esai_fifo_context_ptr;
}

/* EOF */
