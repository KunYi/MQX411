#ifndef _asrc_h
#define _asrc_h 1
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
*   This file is ASRC header file
*
*
*END************************************************************************/

/*
** IOCTL calls specific to ASRC
*/

/*
** The command apply configuration to the ASRC set
** Parameter: a pointer to a ASRC_SET_CONFIG_STRUCT variable
*/
#define IO_IOCTL_ASRC_CONFIG                        _IO(IO_TYPE_ASRC,0x01)

/*
** The command apply sample rate setting to the ASRC set
** Parameter: a pointer to a ASRC_SAMPLE_RATE_PAIR_STRUCT variable
*/
#define IO_IOCTL_ASRC_SET_SAMPLE_RATE               _IO(IO_TYPE_ASRC,0x02)

/*
** The command start the converstion of the ASRC set
** Parameter: NULL
*/
#define IO_IOCTL_ASRC_START                         _IO(IO_TYPE_ASRC,0x03)

/*
** The command stop the converstion of the ASRC set
** Parameter: NULL
*/
#define IO_IOCTL_ASRC_STOP                          _IO(IO_TYPE_ASRC,0x04)

/*
** The command apply reference clock setting to the ASRC set
** Parameter: a pointer to a ASRC_REF_CLK_PAIR_STRUCT variable
*/
#define IO_IOCTL_ASRC_SET_REFCLK                    _IO(IO_TYPE_ASRC,0x05)

/*
** The command apply slot width setting to the ASRC set
** Parameter: a pointer to a ASRC_CLK_DIV_PAIR_STRUCT variable
*/
#define IO_IOCTL_ASRC_SET_SLOT_WIDTH                _IO(IO_TYPE_ASRC,0x06)

/*
** The command apply FIFO format setting to the ASRC set
** Parameter: a pointer to a ASRC_IO_FORMAT_PAIR_STRUCT variable
*/
#define IO_IOCTL_ASRC_SET_IO_FORMAT                 _IO(IO_TYPE_ASRC,0x07)

/*
** The command is used to install DMA service on the input port of the ASRC set
** Parameter: a pointer to a ASRC_INSTALL_SERVICE_STRUCT variable
*/
#define IO_IOCTL_ASRC_INSTALL_SERVICE_SRC           _IO(IO_TYPE_ASRC,0x08)

/*
** The command is used to install DMA service on the output port of the ASRC set
** Parameter: a pointer to a ASRC_INSTALL_SERVICE_STRUCT variable
*/
#define IO_IOCTL_ASRC_INSTALL_SERVICE_DEST          _IO(IO_TYPE_ASRC,0x09)

/*
** The command is used to uninstall DMA service from the input port of the ASRC set
** Parameter: NULL
*/
#define IO_IOCTL_ASRC_UNINSTALL_SRC                 _IO(IO_TYPE_ASRC,0x0a)

/*
** The command is used to uninstall DMA service from the output port of the ASRC set
** Parameter: NULL
*/
#define IO_IOCTL_ASRC_UNINSTALL_DEST                _IO(IO_TYPE_ASRC,0x0b)


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** ASRC's DMA service type on each port
** Enumerate the supported service type
*/
typedef enum asrc_service_type {
    ASRC_SERVICE_MEM = 0x0,
    ASRC_SERVICE_ESAI,
    ASRC_SERVICE_SAI0,
    ASRC_SERVICE_SAI1,
    ASRC_SERVICE_SAI2,
    ASRC_SERVICE_SPDIF,
    ASRC_SERVICE_SSI1,
    ASRC_SERVICE_SSI2,
    ASRC_SERVICE_SSI3
}ASRC_SERVICE_TYPE;

/*
** ASRC's reference clock source on each port
** Enumerate the supported reference clock
*/
typedef enum asrc_clk {
    ASRC_CLK_NONE = 0x0,
    ASRC_CLK_ESAI_RX,
    ASRC_CLK_SAI0_RX,
    ASRC_CLK_SAI1_RX,
    ASRC_CLK_SAI2_RX,
    ASRC_CLK_SAI3_RX,
    ASRC_CLK_SSI1_RX,
    ASRC_CLK_SSI2_RX,
    ASRC_CLK_SSI3_RX,
    ASRC_CLK_SPDIF_RX,
    ASRC_CLK_MLB_CLK,
    ASRC_CLK_ESAI_TX,
    ASRC_CLK_SAI0_TX,
    ASRC_CLK_SAI1_TX,
    ASRC_CLK_SAI2_TX,
    ASRC_CLK_SAI3_TX,
    ASRC_CLK_SSI1_TX,
    ASRC_CLK_SSI2_TX,
    ASRC_CLK_SSI3_TX,
    ASRC_CLK_SPDIF_TX,
    ASRC_CLK_CUSTOM_CLK,
    ASRC_CLK_EXT_AUD_CLK,
    ASRC_CLK_NA
}ASRC_CLK;


/*
** ASRC's FIFO word format on each port
** Enumerate the FIFO word format
*/
typedef enum asrc_fifo_format {
    ASRC_FIFO_8BITS_LSB = 0x0,
    ASRC_FIFO_16BITS_LSB,
    ASRC_FIFO_24BITS_LSB,
    ASRC_FIFO_8BITS_MSB,
    ASRC_FIFO_16BITS_MSB,
    ASRC_FIFO_24BITS_MSB,
    ASRC_FIFO_FORMAT_NA
}ASRC_FIFO_FORMAT;


/*
** ASRC set's full configuration structure
** Holds the parameters for configuration
*/
typedef struct asrc_set_config
{
    /*The input data's sample rate, unit: HZ*/
    uint32_t input_sample_rate;

    /*The target sample rate to output, unit: HZ*/
    uint32_t output_sample_rate;

    /*The reference clock for ASRC pair's input port*/
    ASRC_CLK input_ref_clk;

    /*The reference clock for ASRC pair's output port*/
    ASRC_CLK output_ref_clk;

    /*The FIFO word format for ASRC pair's input FIFO*/
    ASRC_FIFO_FORMAT input_fifo_fmt;

    /*The FIFO word format for ASRC pair's output FIFO*/
    ASRC_FIFO_FORMAT output_fifo_fmt;

    /*The slot width for ASRC pair's input port, it is used for clock divider, 8, 16, 24, 32, 64*/
    uint8_t input_slot_width;

    /*The slot width for ASRC pair's output port, it is used for clock divider, 8, 16, 24, 32, 64*/
    uint8_t output_slot_width;

    /*The threshold for ASRC pair's input FIFO*/
    uint8_t input_fifo_threshold;

    /*The threshold for ASRC pair's output FIFO*/
    uint8_t output_fifo_threshold;
}ASRC_SET_CONFIG_STRUCT, * ASRC_SET_CONFIG_STRUCT_PTR;


/*
** ASRC set's sample rate configuration structure
** Holds the parameters for sample rate configuration
*/
typedef struct asrc_sample_rate_pair
{
    /*The input data's sample rate to input to ASRC pair, unit: HZ*/
    uint32_t ASRC_INPUT_SAMPLE_RATE;

    /*The target sample rate to output from ASRC pair, unit: HZ*/
    uint32_t ASRC_OUTPUT_SAMPLE_RATE;

} ASRC_SAMPLE_RATE_PAIR_STRUCT, * ASRC_SAMPLE_RATE_PAIR_STRUCT_PTR;


/*
** ASRC set's reference clock configuration structure
** Holds the parameters for reference clock configuration
*/
typedef struct asrc_ref_clk_pair
{
    /*The reference clock for ASRC pair's input port*/
    ASRC_CLK ASRC_INPUT_CLK;

    /*The reference clock for ASRC pair's output port*/
    ASRC_CLK ASRC_OUTPUT_CLK;

} ASRC_REF_CLK_PAIR_STRUCT, * ASRC_REF_CLK_PAIR_STRUCT_PTR;


/*
** ASRC set's slot width configuration structure, determine the clock divider
** Holds the parameters for slot width configuration
*/
typedef struct asrc_clk_div_pair
{
    /*The slot width for ASRC pair's input port, it is used for clock divider, 8, 16, 24, 32, 64*/
    uint8_t ASRC_INPUT_CLK_DIV;

    /*The slot width for ASRC pair's output port, it is used for clock divider, 8, 16, 24, 32, 64*/
    uint8_t ASRC_OUTPUT_CLK_DIV;

} ASRC_CLK_DIV_PAIR_STRUCT, * ASRC_CLK_DIV_PAIR_STRUCT_PTR;


/*
** ASRC set's FIFO word format configuration structure
** Holds the parameters for FIFO word format configuration
*/
typedef struct asrc_io_format_pair
{
    /*The FIFO word format for ASRC pair's input FIFO*/
    ASRC_FIFO_FORMAT ASRC_INPUT_FORMAT;

    /*The FIFO word format for ASRC pair's output FIFO*/
    ASRC_FIFO_FORMAT ASRC_OUTPUT_FORMAT;

} ASRC_IO_FORMAT_PAIR_STRUCT, * ASRC_IO_FORMAT_PAIR_STRUCT_PTR;


/*
** ASRC set's DMA service installation structure
** Holds the parameters for DMA service installation configuration
*/
typedef struct asrc_install_service
{
    /*installed DMA service type*/
    ASRC_SERVICE_TYPE ASRC_SERVICE;

    /*The DMA channel id, returned from ASRC driver. User can use it to access DMA driver*/
    int32_t ASRC_DMA_CHL;
} ASRC_INSTALL_SERVICE_STRUCT, * ASRC_INSTALL_SERVICE_STRUCT_PTR;


/*
** ASRC set's pcmmgr callback functions registering structure.
** Define it for the chip specific level driver. The ASRC high level driver will call them.
** Holds the pointers for pcmmgr callback functions
*/
typedef struct asrc_pcmm_funcs_struct{

    /*The function pointer for set sample rate function*/
    int32_t (_CODE_PTR_ set_sample_rate)(void * dev, void * access_hdl,
        uint32_t rate, uint32_t port);

    /*The function pointer for set fifo word format function*/
    int32_t (_CODE_PTR_ set_data_format)(void * dev, void * access_hdl,
        uint32_t data_fmt, uint32_t port);

    /*The function pointer for set reference clock function*/
    int32_t (_CODE_PTR_ set_ref_clock)(void * dev, void * access_hdl,
        uint32_t clk, uint32_t port);

    /*The function pointer for set other pcmmgr related parameters function*/
    int32_t (_CODE_PTR_ set_parameters)(void * dev, void * access_hdl,
        void * para);

    /*The function pointer for DMA service installation function*/
    int32_t (_CODE_PTR_ install_port)(void * dev, void * access_hdl,
        uint32_t port, uint32_t conn, int32_t* channel);

    /*The function pointer for DMA service uninstallation function*/
    int32_t (_CODE_PTR_ uninstall_port)(void * dev, void * access_hdl,
        uint32_t port);

    /*The function pointer for ASRC conversion start function*/
    int32_t (_CODE_PTR_ start)(void * dev, void * access_hdl);

    /*The function pointer for ASRC conversion stop function*/
    int32_t (_CODE_PTR_ stop)(void * dev, void * access_hdl);
}ASRC_PCMM_FUNCS_STRUCT, * ASRC_PCMM_FUNCS_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/


/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_uint _io_asrc_install(
      char *,
      _mqx_uint (_CODE_PTR_)(void *, void * *),
      _mqx_uint (_CODE_PTR_)(void *, void *, char *),
      _mqx_uint (_CODE_PTR_)(void *, void *),
      _mqx_uint (_CODE_PTR_)(void *),
      _mqx_int (_CODE_PTR_)(void *, void *, _mqx_uint, _mqx_uint_ptr),
      void *,
      ASRC_PCMM_FUNCS_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
