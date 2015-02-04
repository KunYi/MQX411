#ifndef _esai_config_h_
#define _esai_config_h_ 1
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
*   This include file is used to provide information needed by
*   applications using the ESAI FIFO I/O device driver functions.
*
*
*END************************************************************************/

#include <ioctl.h>


/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** ESAI module's clock's direction
** Enumerate the ESAI clock direction
*/
typedef enum
{
    /*ESAI master clock*/
    ESAI_CLOCK_MASTER,

    /*ESAI slave clock*/
    ESAI_CLOCK_SLAVE

} ESAI_CLOCK_DIRECTION;

/*
** ESAI module's serial clock and frame sync polarity enumeration
** Enumerate the ESAI clock polarity
*/
typedef enum
{
    /* normal bit clock + frame sync*/
    ESAI_CLOCK_POLAR_NB_NF,

    /* normal bit clock + inv frame sync*/
    ESAI_CLOCK_POLAR_NB_IF,

    /* inv bit clock + normal frame sync*/
    ESAI_CLOCK_POLAR_IB_NF,

    /* inv bit clock + inv frame sync*/
    ESAI_CLOCK_POLAR_IB_IF

} ESAI_CLOCK_POLARITY;

/*
** ESAI module's clock source enumeration
** Enumerate the ESAI clock source
*/
typedef enum
{
    /*133Mhz from ahb clock*/
    ESAI_CLOCK_FSYS,

    /*esai root clk*/
    ESAI_CLOCK_EXTAL

} ESAI_CLOCK_SOURCE;

/*
** ESAI module's slot length enumeration
** Enumerate the ESAI supported slot length
*/
typedef enum
{
    /*12 bits width slot*/
    ESAI_MODULE_SW_12BIT,

    /*16 bits width slot*/
    ESAI_MODULE_SW_16BIT,

    /*20 bits width slot*/
    ESAI_MODULE_SW_20BIT,

    /*24 bits width slot*/
    ESAI_MODULE_SW_24BIT,

    /*32 bits width slot*/
    ESAI_MODULE_SW_32BIT

} ESAI_MODULE_SLOT_WIDTH;

/*
** ESAI module's Mode enumeration
** Enumerate the ESAI modes
*/
typedef enum
{
    /*normal mode*/
    ESAI_MODULE_MODE_NORMAL,

    /*on-demand mode*/
    ESAI_MODULE_MODE_ON_DEMAND,

    /*network mode*/
    ESAI_MODULE_MODE_NETWORK,

    /*ac97 mode*/
    ESAI_MODULE_MODE_AC97

} ESAI_MODULE_MODE;

/*
** ESAI interface format enumeration for normal mode
** Enumerate the ESAI interface formats at NORMAL mode
*/
typedef enum
{
    /*I2S format*/
    ESAI_MODULE_FMT_I2S,

    /*Left-J format*/
    ESAI_MODULE_FMT_LEFT_J,

    /*Right-J format*/
    ESAI_MODULE_FMT_RIGHT_J,

    /*TDM format*/
    ESAI_MODULE_FMT_TDM,

    /*Custom format for network mode*/
    ESAI_MODULE_FMT_CUSTOM_NETWORK,

    /*Custom format for normal mode*/
    ESAI_MODULE_FMT_CUSTOM_NORMAL

} ESAI_MODULE_INF_FORMAT;


/*
** ESAI module's Fsync length enumeration
** Enumerate the ESAI Fsync length
*/
typedef enum
{
    /*one slot length*/
    ESAI_MODULE_ONE_SLOT = 0,

    /*one serial clock length*/
    ESAI_MODULE_ONE_SCLK

} ESAI_MODULE_FSYNC_LENGTH;


/*
** ESAI module's Fsys clock parameters structure
** Holds the parameters for Fsys clock source
*/
typedef struct
{
    /*a fixed divide-by-eight prescaler, 1 bypass, 0 operational*/
    uint8_t   psr;

    /*set the divide value to TPM register, 0x0-0xff*/
    uint8_t   pm;

    /*set the divide value to TFP register,
       **a divide ratio of high freq clock to serial bit clock
       */
    uint8_t   fp;

} ESAI_MODULE_FSYS_CLK_PARA,  * ESAI_MODULE_FSYS_CLK_PARA_PTR;


/*
** ESAI module's EXTAL clock parameters structure
** Holds the parameters for EXTAL clock source
*/
typedef struct
{
    /*The frame rate for output clock, this is same with sample rate for I2S, Left_J, Right_J*/
    uint32_t  frame_rate;

} ESAI_MODULE_EXTAL_CLK_PARA,  * ESAI_MODULE_EXTAL_CLK_PARA_PTR;


/*
   ** ESAI FIFO clock timing parameters enumeration
   ** User can set the sample rate or the detail dividers
   */
typedef union
{
    ESAI_MODULE_FSYS_CLK_PARA fsys_para;
    ESAI_MODULE_EXTAL_CLK_PARA extal_para;
} ESAI_MODULE_TIMING;

/*
** ESAI module's clock configuration structure
** Define the parameters for clock configuration to ESAI FIFO device driver
*/
typedef struct esai_module_clk_config
{
    /*the clock direction, master or slave*/
    ESAI_CLOCK_DIRECTION   clock_direction;

    /*the polarity of this clock*/
    ESAI_CLOCK_POLARITY   clock_polarity;

    /*the reference clock source Fsys or EXTAL, if the clock is master*/
    ESAI_CLOCK_SOURCE   output_refer_clk;

    /*the clock parameters for Fsys or EXTAL*/
    ESAI_MODULE_TIMING  timing;

}ESAI_MODULE_CLK_CONFIG_STRUCT, * ESAI_MODULE_CLK_CONFIG_STRUCT_PTR;




/*
** ESAI NORMAL mode parameters structure
** Holds the parameters for ESAI Normal mode
*/
typedef struct
{
    /*The slots width*/
    ESAI_MODULE_SLOT_WIDTH slot_width;

    /*sync length*/
    ESAI_MODULE_FSYNC_LENGTH fsync_length;

    /*sync early*/
    bool fsync_early;

    /*left align or right align*/
    bool left_align;

    /*pad with zero to meet the gap betwwen slot size and avalid data size*/
    bool zero_pad;

    /*msb shift or lsb shift*/
    bool msb_shift;


} ESAI_MODULE_COMMON_PARA, * ESAI_MODULE_COMMON_PARA_PTR;

/*
** ESAI normal mode parameters structure
** Holds the parameters for ESAI normal mode
*/
typedef struct
{
    /*The slots width*/
    ESAI_MODULE_COMMON_PARA common_para;

    /*how much serial clock to produce one sync*/
    uint32_t sync_period;

} ESAI_MODULE_NORMAL_MODE_PARA,  * ESAI_MODULE_NORMAL_MODE_PARA_PTR;



/*
** ESAI TDM parameters structure
** Holds the parameters for ESAI TDM format
*/
typedef struct
{
    /*The slots number for TDM*/
    uint32_t slots;

    /*The slots mask for TDM*/
    uint32_t mask;

} ESAI_MODULE_NETWORK_TDM_PARA, * ESAI_MODULE_NETWORK_TDM_PARA_PTR;


/*
** ESAI customize network format parameters structure
** Holds the parameters for customize network format
*/
typedef struct
{
    /*The slots number for network*/
    uint32_t slots;

    /*The slots mask for network*/
    uint32_t mask;

    /*common parameters*/
    ESAI_MODULE_COMMON_PARA common_para;

} ESAI_MODULE_NETWORK_CUSTOM_PARA, * ESAI_MODULE_NETWORK_CUSTOM_PARA_PTR;


/*
** ESAI Stereo format parameters structure
** Holds the parameters for network stereo format
*/
typedef struct
{
    /*The slots width*/
    ESAI_MODULE_SLOT_WIDTH slot_width;

} ESAI_MODULE_NETWORK_STEREO_PARA, * ESAI_MODULE_NETWORK_STEREO_PARA_PTR;



/*
   ** ESAI FIFO mode related parameters union
   ** Enumerate the parameters structure for network and normal mode
   */
typedef union {
    /*parameters for I2S, Left-J and Right-J*/
    ESAI_MODULE_NETWORK_STEREO_PARA stereo_para;

    /*parameters for TDM*/
    ESAI_MODULE_NETWORK_TDM_PARA tdm_para;

    /*parameters for customed network format*/
    ESAI_MODULE_NETWORK_CUSTOM_PARA custom_network_para;

    /*parameters for customed normal format*/
    ESAI_MODULE_NORMAL_MODE_PARA custom_normal_para;
}ESAI_FMT_PARA;


/*
** ESAI interface configuration structure
** Holds the parameters for ESAI interface setting
*/
typedef struct
{
    /*setting current interface format*/
    ESAI_MODULE_INF_FORMAT fmt;

    /*parameters for every formt*/
    ESAI_FMT_PARA fmt_para;

} ESAI_MODULE_INF_CONFIG_STRUCT, * ESAI_MODULE_INF_CONFIG_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif
/* EOF */
