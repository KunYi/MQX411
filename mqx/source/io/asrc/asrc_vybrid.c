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
*   This file contains vybrid specific implemetation for standard I/O interfaces
*
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include <io_prv.h>
#include <fio_prv.h>
#include <asrc_prv.h>
#include <asrc.h>
#include <asrc_vybrid.h>
#include <asrc_vybrid_prv.h>


//#define ASRC_VYBRID_ENABLE_DEBUG
#ifdef ASRC_VYBRID_ENABLE_DEBUG
#define ASRC_VYBRID_DEBUG  printf
#else
#define ASRC_VYBRID_DEBUG(...)
#endif

#define ASRC_VYBRID_ENABLE_ERROR
#ifdef ASRC_VYBRID_ENABLE_ERROR
#define ASRC_VYBRID_ERROR  printf
#else
#define ASRC_VYBRID_ERROR(...)
#endif


extern int32_t vybrid_asrc_init(VYBRID_ASRC_INFO_STRUCT_PTR);
extern int32_t vybrid_asrc_deinit(VYBRID_ASRC_INFO_STRUCT_PTR);
extern int32_t vybrid_asrc_pair_init(VYBRID_ASRC_INFO_STRUCT_PTR,
                                       VYBRID_ASRC_SET_NODE_STRUCT_PTR*,
                                       uint8_t, uint8_t);
extern int32_t vybrid_asrc_config(VYBRID_ASRC_SET_NODE_STRUCT_PTR,
                                     ASRC_SET_CONFIG_STRUCT_PTR);
extern int32_t vybrid_asrc_pair_deinit(VYBRID_ASRC_SET_NODE_STRUCT_PTR);
extern int32_t vybrid_asrc_start(VYBRID_ASRC_SET_NODE_STRUCT_PTR);
extern int32_t vybrid_asrc_stop(VYBRID_ASRC_SET_NODE_STRUCT_PTR);
extern int32_t vybrid_asrc_install_service_source(
                                        VYBRID_ASRC_SET_NODE_STRUCT_PTR,
                                        ASRC_SERVICE_TYPE, int32_t*);
extern int32_t vybrid_asrc_install_service_dest(
                                        VYBRID_ASRC_SET_NODE_STRUCT_PTR,
                                        ASRC_SERVICE_TYPE, uint32_t*);
extern int32_t vybrid_asrc_uninstall_source(VYBRID_ASRC_SET_NODE_STRUCT_PTR);
extern int32_t vybrid_asrc_uninstall_dest(VYBRID_ASRC_SET_NODE_STRUCT_PTR);


uint32_t _asrc_vybrid_init (VYBRID_ASRC_INIT_STRUCT_PTR,
    void * *);
uint32_t _asrc_vybrid_open (IO_ASRC_DEVICE_STRUCT_PTR,
    void *, char *);
uint32_t _asrc_vybrid_close (IO_ASRC_DEVICE_STRUCT_PTR,
    void *);
uint32_t _asrc_vybrid_deinit (IO_ASRC_DEVICE_STRUCT_PTR);
uint32_t _asrc_vybrid_ioctl (IO_ASRC_DEVICE_STRUCT_PTR,
    void *, uint32_t, uint32_t *);


#if DISCFG_OS_DRV_ENABLE_PCMMGR
extern ASRC_PCMM_FUNCS_STRUCT _asrc_vybrid_pcmm_funcs;
#endif

/*FUNCTION****************************************************************
*
* Function Name    : _asrc_vybrid_init_default_config
* Returned Value   : MQX error code
* Comments         :
*    This function initializes the default configuration for current asrc pair
*
*END*********************************************************************/
static int32_t _asrc_vybrid_init_default_config
   (
      /* [IN] A string that identifies the device for fopen */
      ASRC_SET_CONFIG_STRUCT_PTR   asrc_set_node_config,

      /* [IN] ASRC input slot width*/
      uint8_t input_slot_width,

      /* [IN] ASRC output slot width*/
      uint8_t output_slot_width

   )
{
    if (NULL == asrc_set_node_config)
        return -1;

    asrc_set_node_config->input_sample_rate = 44100;
    asrc_set_node_config->output_sample_rate = 48000;
    asrc_set_node_config->input_ref_clk = ASRC_CLK_NONE;
    asrc_set_node_config->output_ref_clk = ASRC_CLK_ESAI_TX;
    asrc_set_node_config->input_fifo_fmt = ASRC_FIFO_16BITS_LSB;
    asrc_set_node_config->output_fifo_fmt = ASRC_FIFO_16BITS_LSB;
    asrc_set_node_config->input_slot_width = input_slot_width;
    asrc_set_node_config->output_slot_width = output_slot_width;
    asrc_set_node_config->input_fifo_threshold = 0x20;
    asrc_set_node_config->output_fifo_threshold = 0x20;

    return 0;
}


/*FUNCTION****************************************************************
*
* Function Name    : _asrc_vybrid_init
* Returned Value   : MQX error code
* Comments         :
*    This function initializes asrc vybrid driver.
*
*END*********************************************************************/
uint32_t _asrc_vybrid_init
   (
      /* [IN] Initialization information for the device being opened */
      VYBRID_ASRC_INIT_STRUCT_PTR   io_init_ptr,

      /* [OUT] Address to store device specific information */
      void * *                 io_info_ptr_ptr

   )
{ /* Body */
    VYBRID_ASRC_INFO_STRUCT_PTR           io_info_ptr;
    int32_t i = 0;

    ASRC_VYBRID_DEBUG("[ASRC VYBRID] INIT\n");

    io_info_ptr = (VYBRID_ASRC_INFO_STRUCT_PTR)_mem_alloc_system_zero (
        (uint32_t)sizeof (VYBRID_ASRC_INFO_STRUCT));
    if (NULL == io_info_ptr)
    {
        ASRC_VYBRID_ERROR("[ASRC VYBRID]init alloc mem fail\n");
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type (io_info_ptr, MEM_TYPE_IO_ASRC_INFO_STRUCT);

    _bsp_asrc_io_init();

    *io_info_ptr_ptr        = io_info_ptr;
    io_info_ptr->INIT       = *io_init_ptr;

    for (i=0; i<ASRC_PAIRS_NUM; i++) {
        io_info_ptr->PAIR_DESCS[i].identity = 0xFFFFFFFF; /*invalid value*/
        io_info_ptr->PAIR_DESCS[i].asrc_set_node_ptr = NULL;
        io_info_ptr->PAIR_DESCS[i].asrc_set_node_config_ptr = NULL;
    }
    io_info_ptr->asrc_regs = ASRC_BASE_PTR;

    vybrid_asrc_init(io_info_ptr);

    ASRC_VYBRID_DEBUG("[ASRC VYBRID] INIT END\n");
    return MQX_OK;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _asrc_vybrid_open
* Returned Value   : MQX error code
* Comments         :
*    This function opens an vybrid asrc pair.
*
*END*********************************************************************/
uint32_t _asrc_vybrid_open
   (
      /* [IN] the initialization information for the device being opened */
      IO_ASRC_DEVICE_STRUCT_PTR     asrc_io_dev_ptr,

      /* [IN] The file descriptor handler for this operation */
      void *                       file_handler_ptr,

      /* [IN] The open flags for asrc */
      char *                      flags

   )
{ /* Body */
    VYBRID_ASRC_INFO_STRUCT_PTR         io_info_ptr;
    VYBRID_ASRC_PAIR_DESC_STRUCT_PTR    pair_desc_ptr = NULL;

    int32_t i = 0;
    int32_t ret = 0;

    uint8_t mode = (uint32_t)flags;

    ASRC_VYBRID_DEBUG("[ASRC VYBRID] OPEN\n");

    io_info_ptr = (VYBRID_ASRC_INFO_STRUCT_PTR)asrc_io_dev_ptr->DEV_INFO_PTR;

    if (io_info_ptr == NULL)
        return MQX_IO_OPERATION_NOT_AVAILABLE;

    /*search a free asrc pair*/
    for (i=0; i<ASRC_PAIRS_NUM; i++) {
        if (0xFFFFFFFF == io_info_ptr->PAIR_DESCS[i].identity) {
            io_info_ptr->PAIR_DESCS[i].identity = (uint32_t)file_handler_ptr;
            pair_desc_ptr = &(io_info_ptr->PAIR_DESCS[i]);
            break;
        }

    }

    if (NULL == pair_desc_ptr) {
        ASRC_VYBRID_ERROR("[ASRC VYBRID] OPEN failed, no pair available\n");
        return MQX_IO_OPERATION_NOT_AVAILABLE;
    }


    /*init the asrc set node*/
    ASRC_VYBRID_DEBUG("[ASRC VYBRID] vybrid_asrc_pair_init\n");
    ret = vybrid_asrc_pair_init(io_info_ptr,
            &pair_desc_ptr->asrc_set_node_ptr, i, mode);
    if (0 != ret) {
        ASRC_VYBRID_ERROR("[ASRC VYBRID] vybrid_asrc_pair_init failed %d\n", ret);
        return MQX_IO_OPERATION_NOT_AVAILABLE;
    }

    /*configure default value to this asrc set node*/
    ASRC_VYBRID_DEBUG("[ASRC VYBRID] _mem_alloc_system_zero\n");
    pair_desc_ptr->asrc_set_node_config_ptr =
        (ASRC_SET_CONFIG_STRUCT_PTR)_mem_alloc_system_zero(
        (uint32_t)sizeof (ASRC_SET_CONFIG_STRUCT));
    if (NULL == pair_desc_ptr->asrc_set_node_config_ptr) {
        ASRC_VYBRID_ERROR("[ASRC VYBRID] OPEN fails to init config %d\n", ret);
        return MQX_IO_OPERATION_NOT_AVAILABLE;
    }

    _mem_set_type (pair_desc_ptr->asrc_set_node_config_ptr,
        MEM_TYPE_IO_ASRC_SET_NODE_STRUCT);

    ASRC_VYBRID_DEBUG("[ASRC VYBRID] _asrc_vybrid_init_default_config\n");
    _asrc_vybrid_init_default_config(pair_desc_ptr->asrc_set_node_config_ptr,
        io_info_ptr->INIT.INPUT_SLOT_WIDTH,
        io_info_ptr->INIT.OUTPUT_SLOT_WIDTH);

    ASRC_VYBRID_DEBUG("[ASRC VYBRID] vybrid_asrc_config\n");
    ret = vybrid_asrc_config(pair_desc_ptr->asrc_set_node_ptr,
                pair_desc_ptr->asrc_set_node_config_ptr);
    if (0 != ret) {
        ASRC_VYBRID_ERROR("[ASRC VYBRID] vybrid_asrc_config default configuration "
            "failed, ret = %d\n", ret);
    }

    ASRC_VYBRID_DEBUG("[ASRC VYBRID] OPEN END\n");

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _asrc_vybrid_close
* Returned Value   : MQX error code
* Comments         :
*    This function closes an vybrid asrc pair.
*
*END*********************************************************************/
uint32_t _asrc_vybrid_close
   (
      /* [IN] the initialization information for the device being opened */
      IO_ASRC_DEVICE_STRUCT_PTR     asrc_io_dev_ptr,

      /* [IN] The file descriptor handler for this operation */
      void *                       file_handler_ptr

   )
{
    VYBRID_ASRC_INFO_STRUCT_PTR         io_info_ptr;
    VYBRID_ASRC_PAIR_DESC_STRUCT_PTR    pair_desc_ptr = NULL;

    int32_t i = 0;

    ASRC_VYBRID_DEBUG("[ASRC VYBRID] CLOSE\n");

    io_info_ptr = (VYBRID_ASRC_INFO_STRUCT_PTR)asrc_io_dev_ptr->DEV_INFO_PTR;

    if (io_info_ptr == NULL)
        return MQX_IO_OPERATION_NOT_AVAILABLE;

    for (i=0; i<ASRC_PAIRS_NUM; i++) {
        if ((uint32_t)file_handler_ptr == io_info_ptr->PAIR_DESCS[i].identity) {
            pair_desc_ptr = &(io_info_ptr->PAIR_DESCS[i]);
            break;
        }
    }

    if (NULL == pair_desc_ptr) {
        ASRC_VYBRID_ERROR("[ASRC VYBRID] Close failed, no pair seached,"
            "file ptr = 0x%x\n", (uint32_t)file_handler_ptr);
        return MQX_IO_OPERATION_NOT_AVAILABLE;
    }

    vybrid_asrc_pair_deinit(pair_desc_ptr->asrc_set_node_ptr);

    pair_desc_ptr->asrc_set_node_ptr = NULL;
    pair_desc_ptr->identity = 0xFFFFFFFF;

    _mem_free(pair_desc_ptr->asrc_set_node_config_ptr);

    ASRC_VYBRID_DEBUG("[ASRC VYBRID] CLOSE END\n");

    return MQX_OK;

}


/*FUNCTION****************************************************************
*
* Function Name    : _asrc_vybrid_deinit
* Returned Value   : MQX error code
* Comments         :
*    This function de-initializes vybird asrc driver.
*
*END*********************************************************************/

uint32_t _asrc_vybrid_deinit
   (
      /* [IN] the initialization information for the device being opened */
      IO_ASRC_DEVICE_STRUCT_PTR asrc_io_dev_ptr
   )
{ /* Body */
    VYBRID_ASRC_INFO_STRUCT_PTR         io_info_ptr;
    VYBRID_ASRC_PAIR_DESC_STRUCT_PTR    pair_desc_ptr = NULL;
    int32_t i = 0;

    ASRC_VYBRID_DEBUG("[ASRC VYBRID] DEINIT\n");

    io_info_ptr = (VYBRID_ASRC_INFO_STRUCT_PTR)asrc_io_dev_ptr->DEV_INFO_PTR;

    if (io_info_ptr == NULL)
        return MQX_IO_OPERATION_NOT_AVAILABLE;

    for (i=0; i<ASRC_PAIRS_NUM; i++) {
        if (0xFFFFFFFF != io_info_ptr->PAIR_DESCS[i].identity) {
            vybrid_asrc_pair_deinit(pair_desc_ptr->asrc_set_node_ptr);
            _mem_free(pair_desc_ptr->asrc_set_node_ptr);

            pair_desc_ptr->asrc_set_node_ptr = NULL;
            pair_desc_ptr->identity = 0xFFFFFFFF;
        }
    }

    vybrid_asrc_deinit(io_info_ptr);

    _mem_free(io_info_ptr);
    asrc_io_dev_ptr->DEV_INFO_PTR = NULL;

    ASRC_VYBRID_DEBUG("[ASRC VYBRID] DEINIT END\n");
    return MQX_OK;

} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _asrc_vybrid_ioctl
* Returned Value   : MQX error code.
* Comments         :
*    This function performs miscellaneous services for
*    the asrc vybrid I/O device.
*
*END*********************************************************************/

uint32_t _asrc_vybrid_ioctl
   (
      /* [IN] the initialization information for the device being opened */
      IO_ASRC_DEVICE_STRUCT_PTR asrc_io_dev_ptr,

      /* [IN] The file descriptor handler for this operation */
      void *                     file_handler_ptr,

      /* [IN] The command to perform */
      uint32_t                     cmd,

      /* [IN] Parameters for the command */
      uint32_t *                 param_ptr
   )
{ /* Body */
    uint32_t                        result = MQX_OK;

    VYBRID_ASRC_INFO_STRUCT_PTR         io_info_ptr;
    VYBRID_ASRC_PAIR_DESC_STRUCT_PTR    pair_desc_ptr = NULL;
    int32_t i = 0;
    int32_t ret = 0;
    ASRC_SET_CONFIG_STRUCT config;

    ASRC_VYBRID_DEBUG("[ASRC VYBRID] IOCTL, cmd %u\n", cmd);

    io_info_ptr = (VYBRID_ASRC_INFO_STRUCT_PTR)asrc_io_dev_ptr->DEV_INFO_PTR;
    if (io_info_ptr == NULL)
    {
        return MQX_IO_OPERATION_NOT_AVAILABLE;
    }

    for (i=0; i<ASRC_PAIRS_NUM; i++) {
        if ((uint32_t)file_handler_ptr == io_info_ptr->PAIR_DESCS[i].identity) {
            pair_desc_ptr = &(io_info_ptr->PAIR_DESCS[i]);
            break;
        }
    }
    if (NULL == pair_desc_ptr) {
        ASRC_VYBRID_ERROR("[ASRC VYBRID] IOCTL failed, no pair seached,"
            "file ptr = 0x%x\n", (uint32_t)file_handler_ptr);
        return MQX_IO_OPERATION_NOT_AVAILABLE;
    }

    switch (cmd)
    {
        case IO_IOCTL_ASRC_CONFIG:
        {
            ASRC_SET_CONFIG_STRUCT_PTR config_ptr =
                (ASRC_SET_CONFIG_STRUCT_PTR)param_ptr;

            ASRC_VYBRID_DEBUG("[ASRC VYBRID] IO_IOCTL_ASRC_CONFIG\n");

            ret = vybrid_asrc_config(pair_desc_ptr->asrc_set_node_ptr,
                config_ptr);
            if (ret != 0) {
                ASRC_VYBRID_ERROR("[ASRC VYBRID] IO_IOCTL_ASRC_CONFIG "
                    "ERROR, %d\n", ret);
                vybrid_asrc_config(pair_desc_ptr->asrc_set_node_ptr,
                    pair_desc_ptr->asrc_set_node_config_ptr);
                result = MQX_EINVAL;
            }

            /*update recorded config*/
            _mem_copy(config_ptr, pair_desc_ptr->asrc_set_node_config_ptr,
                sizeof(ASRC_SET_CONFIG_STRUCT));

            break;
        }
        case IO_IOCTL_ASRC_SET_SAMPLE_RATE:
        {
            ASRC_SAMPLE_RATE_PAIR_STRUCT_PTR sample_rates_ptr =
                (ASRC_SAMPLE_RATE_PAIR_STRUCT_PTR)param_ptr;

            ASRC_VYBRID_DEBUG("[ASRC VYBRID] IO_IOCTL_ASRC_SET_SAMPLE_RATE\n");

            _mem_copy(pair_desc_ptr->asrc_set_node_config_ptr, &config,
                    sizeof(ASRC_SET_CONFIG_STRUCT));

            config.input_sample_rate =
                sample_rates_ptr->ASRC_INPUT_SAMPLE_RATE;
            config.output_sample_rate =
                sample_rates_ptr->ASRC_OUTPUT_SAMPLE_RATE;

            ret = vybrid_asrc_config(pair_desc_ptr->asrc_set_node_ptr, &config);
            if (ret != 0) {
                ASRC_VYBRID_ERROR("[ASRC VYBRID] IO_IOCTL_ASRC_SET_SAMPLE_RATE "
                    "ERROR, %d\n", ret);
                vybrid_asrc_config(pair_desc_ptr->asrc_set_node_ptr,
                    pair_desc_ptr->asrc_set_node_config_ptr);
                return MQX_EINVAL;
            }

            _mem_copy(&config, pair_desc_ptr->asrc_set_node_config_ptr,
                sizeof(ASRC_SET_CONFIG_STRUCT));
            break;
        }
        case IO_IOCTL_ASRC_START:
        {
            ASRC_VYBRID_DEBUG("[ASRC VYBRID] IO_IOCTL_ASRC_START\n");
            ret = vybrid_asrc_start(pair_desc_ptr->asrc_set_node_ptr);
            if (ret != 0) {
                ASRC_VYBRID_ERROR("[ASRC VYBRID] IO_IOCTL_ASRC_START ERROR, %d\n",
                    ret);
                return MQX_EINVAL;
            }

            break;
        }
        case IO_IOCTL_ASRC_STOP:
        {
            ASRC_VYBRID_DEBUG("[ASRC VYBRID] IO_IOCTL_ASRC_STOP\n");
            ret = vybrid_asrc_stop(pair_desc_ptr->asrc_set_node_ptr);
            if (ret != 0) {
                ASRC_VYBRID_ERROR("[ASRC VYBRID] IO_IOCTL_ASRC_STOP ERROR, %d\n",
                    ret);
                return MQX_EINVAL;
            }

            break;
        }
        case IO_IOCTL_ASRC_SET_REFCLK:
        {
            ASRC_REF_CLK_PAIR_STRUCT_PTR ref_clks_ptr =
                            (ASRC_REF_CLK_PAIR_STRUCT_PTR)param_ptr;

            ASRC_VYBRID_DEBUG("[ASRC VYBRID] IO_IOCTL_ASRC_SET_REFCLK\n");

            _mem_copy(pair_desc_ptr->asrc_set_node_config_ptr, &config,
                    sizeof(ASRC_SET_CONFIG_STRUCT));

            config.input_ref_clk = ref_clks_ptr->ASRC_INPUT_CLK;
            config.output_ref_clk = ref_clks_ptr->ASRC_OUTPUT_CLK;

            ret = vybrid_asrc_config(pair_desc_ptr->asrc_set_node_ptr, &config);
            if (ret != 0) {
                ASRC_VYBRID_ERROR("[ASRC VYBRID] IO_IOCTL_ASRC_SET_REFCLK "
                    "ERROR, %d\n", ret);
                vybrid_asrc_config(pair_desc_ptr->asrc_set_node_ptr,
                    pair_desc_ptr->asrc_set_node_config_ptr);
                return MQX_EINVAL;
            }

            _mem_copy(&config, pair_desc_ptr->asrc_set_node_config_ptr,
                sizeof(ASRC_SET_CONFIG_STRUCT));
            break;
        }
        case IO_IOCTL_ASRC_SET_SLOT_WIDTH:
        {
            ASRC_CLK_DIV_PAIR_STRUCT_PTR clk_divs_ptr =
                            (ASRC_CLK_DIV_PAIR_STRUCT_PTR)param_ptr;

            ASRC_VYBRID_DEBUG("[ASRC VYBRID] IO_IOCTL_ASRC_SET_SLOT_WIDTH\n");

            _mem_copy(pair_desc_ptr->asrc_set_node_config_ptr, &config,
                    sizeof(ASRC_SET_CONFIG_STRUCT));

            config.input_slot_width = clk_divs_ptr->ASRC_INPUT_CLK_DIV;
            config.output_slot_width = clk_divs_ptr->ASRC_OUTPUT_CLK_DIV;

            ret = vybrid_asrc_config(pair_desc_ptr->asrc_set_node_ptr, &config);
            if (ret != 0) {
                ASRC_VYBRID_ERROR("[ASRC VYBRID] IO_IOCTL_ASRC_SET_SLOT_WIDTH "
                    "ERROR, %d\n", ret);
                vybrid_asrc_config(pair_desc_ptr->asrc_set_node_ptr,
                    pair_desc_ptr->asrc_set_node_config_ptr);
                return MQX_EINVAL;
            }

            _mem_copy(&config, pair_desc_ptr->asrc_set_node_config_ptr,
                sizeof(ASRC_SET_CONFIG_STRUCT));

            break;
        }
        case IO_IOCTL_ASRC_SET_IO_FORMAT:
        {
            ASRC_IO_FORMAT_PAIR_STRUCT_PTR io_formats_ptr =
                            (ASRC_IO_FORMAT_PAIR_STRUCT_PTR)param_ptr;

            ASRC_VYBRID_DEBUG("[ASRC VYBRID] IO_IOCTL_ASRC_SET_IO_FORMAT\n");

            _mem_copy(pair_desc_ptr->asrc_set_node_config_ptr, &config,
                    sizeof(ASRC_SET_CONFIG_STRUCT));

            config.input_fifo_fmt = io_formats_ptr->ASRC_INPUT_FORMAT;
            config.output_fifo_fmt = io_formats_ptr->ASRC_OUTPUT_FORMAT;

            ret = vybrid_asrc_config(pair_desc_ptr->asrc_set_node_ptr, &config);
            if (ret != 0) {
                ASRC_VYBRID_ERROR("[ASRC VYBRID] IO_IOCTL_ASRC_SET_IO_FORMAT "
                    "ERROR, %d\n", ret);
                vybrid_asrc_config(pair_desc_ptr->asrc_set_node_ptr,
                    pair_desc_ptr->asrc_set_node_config_ptr);
                return MQX_EINVAL;
            }

            _mem_copy(&config, pair_desc_ptr->asrc_set_node_config_ptr,
                sizeof(ASRC_SET_CONFIG_STRUCT));

            break;
        }
        case IO_IOCTL_ASRC_INSTALL_SERVICE_SRC:
        {
            ASRC_INSTALL_SERVICE_STRUCT_PTR services_ptr =
                            (ASRC_INSTALL_SERVICE_STRUCT_PTR)param_ptr;

            ASRC_VYBRID_DEBUG("[ASRC VYBRID] IO_IOCTL_ASRC_INSTALL_SERVICE_SRC\n");
            ret = vybrid_asrc_install_service_source(
                pair_desc_ptr->asrc_set_node_ptr,
                services_ptr->ASRC_SERVICE, &(services_ptr->ASRC_DMA_CHL));
            if (ret != 0) {
                ASRC_VYBRID_ERROR("[ASRC VYBRID] IO_IOCTL_ASRC_INSTALL_SERVICE_SRC"
                    "ERROR, %d\n", ret);
                return MQX_EINVAL;
            }

            break;
        }
        case IO_IOCTL_ASRC_INSTALL_SERVICE_DEST:
        {
            ASRC_INSTALL_SERVICE_STRUCT_PTR services_ptr =
                            (ASRC_INSTALL_SERVICE_STRUCT_PTR)param_ptr;

            ASRC_VYBRID_DEBUG("[ASRC VYBRID] IO_IOCTL_ASRC_INSTALL_SERVICE_DEST\n");
            ret = vybrid_asrc_install_service_dest(
                pair_desc_ptr->asrc_set_node_ptr,
                services_ptr->ASRC_SERVICE,
                (uint32_t*)&(services_ptr->ASRC_DMA_CHL));
            if (ret != 0) {
                ASRC_VYBRID_ERROR("[ASRC VYBRID] IO_IOCTL_ASRC_INSTALL_SERVICE_DEST"
                    "ERROR, %d\n", ret);
                return MQX_EINVAL;
            }

            break;
        }
        case IO_IOCTL_ASRC_UNINSTALL_SRC:
        {
            ASRC_VYBRID_DEBUG("[ASRC VYBRID] IO_IOCTL_ASRC_UNINSTALL_SRC\n");
            ret = vybrid_asrc_uninstall_source(pair_desc_ptr->asrc_set_node_ptr);
            if (ret != 0) {
                ASRC_VYBRID_ERROR("[ASRC VYBRID] IO_IOCTL_ASRC_UNINSTALL_SRC"
                    "ERROR, %d\n", ret);
                return MQX_EINVAL;
            }

            break;
        }
        case IO_IOCTL_ASRC_UNINSTALL_DEST:
        {
            ASRC_VYBRID_DEBUG("[ASRC VYBRID] IO_IOCTL_ASRC_UNINSTALL_DEST\n");
            ret = vybrid_asrc_uninstall_dest(pair_desc_ptr->asrc_set_node_ptr);
            if (ret != 0) {
                ASRC_VYBRID_ERROR("[ASRC VYBRID] IO_IOCTL_ASRC_UNINSTALL_DEST"
                    "ERROR, %d\n", ret);
                return MQX_EINVAL;
            }

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
* Function Name    : _asrc_vybrid_install
* Returned Value   : MQX error code
* Comments         :
*    Install an asrc vybrid device driver.
*
*END*********************************************************************/
uint32_t _asrc_vybrid_install
   (
      /* [IN] A string that identifies the device for fopen */
      char *           identifier,

      /* [IN] The I/O init data void * */
      VYBRID_ASRC_INIT_STRUCT_CPTR            init_data_ptr
   )
{ /* Body */

    return _io_asrc_install(identifier,
      (uint32_t (_CODE_PTR_)(void *, void * *))_asrc_vybrid_init,
      (uint32_t (_CODE_PTR_)(void *, void *, char *))_asrc_vybrid_open,
      (uint32_t (_CODE_PTR_)(void *, void *))_asrc_vybrid_close,
      (uint32_t (_CODE_PTR_)(void *))_asrc_vybrid_deinit,
      (int32_t (_CODE_PTR_)(void *, void *, uint32_t, uint32_t *))
        _asrc_vybrid_ioctl,
      (void *)init_data_ptr,
#if DISCFG_OS_DRV_ENABLE_PCMMGR
      &_asrc_vybrid_pcmm_funcs);
#else
      NULL);
#endif

} /* Endbody */



/* EOF */
