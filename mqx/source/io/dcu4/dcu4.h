#ifndef __dcu4_h__
#define __dcu4_h__
/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
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
*   required for the DCU4 driver
*
*
*END************************************************************************/

#include <mqx.h>
#include <ioctl.h>

/*--------------------------------------------------------------------------*/
/*
**                    CONSTANT DEFINITIONS
*/
/*
** brief   The number of layers supported by DCU4
*/
#define DCU4_LAYERS_NUM         (64)

/* 
** IOCTL calls specific to DCU4
*/

/*
** brief   Register callback function to DCU4 driver for certain event
**
** param [in] a void* variable, point to a DCU4_EVENT_STRUCT structure
**
*/
#define IO_IOCTL_DCU4_REGISTER_EVENT        _IO(IO_TYPE_DCU,0x01)

/*
** brief   Unregister callback function from DCU4 driver for certain event
**
** param [in] a DCU4_EVENT_TYPE variable, indicate type of the event
**
*/
#define IO_IOCTL_DCU4_UNREGISTER_EVENT      _IO(IO_TYPE_DCU,0x02)

/*
** brief   Get timing parameters from DCU4 driver
**
** param [out] a void* variable, point to a DCU4_TIMING_PARAM_STRUCT structure
**
*/
#define IO_IOCTL_DCU4_GET_TIMING            _IO(IO_TYPE_DCU,0x03)

/*
** brief   Update timing parameters to DCU4 driver
**
** param [in] a void* variable, point to a DCU4_TIMING_PARAM_STRUCT structure
**
*/
#define IO_IOCTL_DCU4_SET_TIMING            _IO(IO_TYPE_DCU,0x04)

/*
** brief   Get layer region and format from DCU4 driver
**
** param [out] a void* variable, point to a DCU4_LAYER_IOCTL_STRUCT structure
**
*/
#define IO_IOCTL_DCU4_LAYER_GET_REGION      _IO(IO_TYPE_DCU,0x05)

/*
** brief   Set layer region and format to DCU4 driver
**
** param [in] a void* variable, point to a DCU4_LAYER_IOCTL_STRUCT structure
**
*/
#define IO_IOCTL_DCU4_LAYER_SET_REGION      _IO(IO_TYPE_DCU,0x06)

/*
** brief   Get layer buffer address from DCU4 driver
**
** param [out] a void* variable, point to a DCU4_LAYER_IOCTL_STRUCT structure
**
*/
#define IO_IOCTL_DCU4_LAYER_GET_ADDRESS     _IO(IO_TYPE_DCU,0x07)

/*
** brief   Set layer buffer address to DCU4 driver
**
** param [in] a void* variable, point to a DCU4_LAYER_IOCTL_STRUCT structure
**
*/
#define IO_IOCTL_DCU4_LAYER_SET_ADDRESS     _IO(IO_TYPE_DCU,0x08)

/*
** brief   Get layer blend setting from DCU4 driver
**
** param [out] a void* variable, point to a DCU4_LAYER_IOCTL_STRUCT structure
**
*/
#define IO_IOCTL_DCU4_LAYER_GET_BLEND       _IO(IO_TYPE_DCU,0x09)

/*
** brief   Set layer blend to DCU4 driver
**
** param [in] a void* variable, point to a DCU4_LAYER_IOCTL_STRUCT structure
**
*/
#define IO_IOCTL_DCU4_LAYER_SET_BLEND       _IO(IO_TYPE_DCU,0x0A)

/*
** brief   Enable or disable the layer
**
** param [in] a bool variable, TRUE for enable and FALSE for disable
**
*/
#define IO_IOCTL_DCU4_LAYER_ENABLE          _IO(IO_TYPE_DCU,0x0B)

/* DCU4 error codes */
#define DCU4_OK                             (0x00)

/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** DCU4_EVENT_TYPE
**
** This enumeration defines the DCU4 event types.
*/
typedef enum
{
    DCU_EVENT_VBLANK = 0,         /**< vertical blank */
    DCU_EVENT_LSBFVS,             /**< lines before VS */
    DCU_EVENT_VSYNC,              /**< vertical sync */
    DCU_EVENT_PROG_END,           /**< programming of all buffered register is done */
    DCU_EVENT_LYR_TRANS_FINISH,   /**< layer transfer finished */
    DCU_EVENT_DMA_TRANS_FINISH,   /**< DMA transfer finished */
    DCU_EVENT_NUM                 /**< number of the event types */
} DCU4_EVENT_TYPE;

/*
** DCU4_BLEND_TYPE
**
** This enumeration defines the DCU4 blending types.
*/
typedef enum
{
    DCU_BLEND_NO_ALPHA = 0,       /**< no alpha blending */
    DCU_BLEND_CHROMA,             /**< blend pixels by chroma keying */
    DCU_BLEND_GLOBAL              /**< blend the whole frame by global alpha */
} DCU4_BLEND_TYPE;

typedef enum
{     
    DCU_BPP_1 = 0,                /**< CLUT */ 
    DCU_BPP_2,                    /**< CLUT */ 
    DCU_BPP_4,                    /**< CLUT */ 
    DCU_BPP_8,                    /**< CLUT */ 
    DCU_BPP_16,                   /**< RGB565 */
    DCU_BPP_24,                   /**< RGB888 */
    DCU_BPP_32,                   /**< ARGB8888 */
    DCU_BPP_TRANS_4,
    DCU_BPP_TRANS_8,
    DCU_BPP_LUM_OFFS_4,
    DCU_BPP_LUM_OFFS_8,
    DCU_BPP_16_ARGB1555,
    DCU_BPP_16_ARGB4444,
    DCU_BPP_16_APAL8,
    DCU_BPP_UYVY,
    DCU_BPP_INVALID
} DCU4_BPP_TYPE;

/*
** DCU4_EVENT_STRUCT
**
** This structure defines the DCU4 event handler info.
*/
typedef struct dcu4_event_struct {
    /* Event type */
    DCU4_EVENT_TYPE    TYPE;

    /* Event handler and data */
    void   (_CODE_PTR_ HANDLER)(void *);
    void              *DATA;

} DCU4_EVENT_STRUCT, * DCU4_EVENT_STRUCT_PTR;

/*
** DCU4_TIMING_PARAM_STRUCT
**
** This structure defines the DCU4 timing parameter.
*/
typedef struct dcu4_timing_param_struct {

    uint32_t  PULSE_WIDTH;

    uint32_t  BACK_PORCH;

    uint32_t  RESOLUTION;

    uint32_t  FRONT_PORCH;

    bool      INVERT;

} DCU4_TIMING_PARAM_STRUCT, * DCU4_TIMING_PARAM_STRUCT_PTR;

/*
** DCU4_TIMING_STRUCT
**
** This structure defines the DCU4 timing info.
*/
typedef struct dcu4_timing_struct {
    /* Timing config name */
    const char                 *name;

    /* Display refresh rate */
    uint32_t                    REFRESH_RATE;

    /* Horizonal timing parameter */
    DCU4_TIMING_PARAM_STRUCT    HSYNC;

    /* vertical timing parameter */
    DCU4_TIMING_PARAM_STRUCT    VSYNC;

} DCU4_TIMING_STRUCT, * DCU4_TIMING_STRUCT_PTR;

/*
** DCU4_LAYER_IOCTL_STRUCT
**
** This structure defines the DCU4 layer control data.
*/
typedef struct dcu4_layer_control_struct {
    /* layer id */
    uint32_t    LAYER;

    /* IOCTL data */
    union {
        struct {
            uint16_t            X;
            uint16_t            Y;
            uint16_t            WIDTH;
            uint16_t            HEIGHT;
            DCU4_BPP_TYPE       FORMAT;
        } REGION;

        void                   *ADDRESS;

        struct {
            DCU4_BLEND_TYPE     TYPE;
            uint8_t             ALPHA;

            struct {
                uint8_t         MAX_R;
                uint8_t         MAX_G;
                uint8_t         MAX_B;
                uint8_t         MIN_R;
                uint8_t         MIN_G;
                uint8_t         MIN_B;
            } CHROMA;

        } BLEND;

        bool                    ENABLE;
    } DATA;

} DCU4_LAYER_IOCTL_STRUCT, * DCU4_LAYER_IOCTL_STRUCT_PTR;

/*
** DCU4_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a dcu4 driver is initialized.
*/
typedef struct dcu4_init_struct
{
    /* The device number */
    uint32_t CHANNEL;

    /* The module input clock */
    uint32_t CLOCK_SPEED;

    /* The default display width */
    uint32_t WIDTH;

    /* The default display height */
    uint32_t HEIGHT;
   
    /* The interrupt priority */
    uint32_t INT_PRIORITY;

} DCU4_INIT_STRUCT, * DCU4_INIT_STRUCT_PTR;

typedef const DCU4_INIT_STRUCT * DCU4_INIT_STRUCT_CPTR;

/*--------------------------------------------------------------------------*/
/* 
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern void     *_bsp_get_dcu4_base_address(uint32_t);
extern uint32_t  _bsp_get_dcu4_vector(uint32_t);
extern _mqx_uint _dcu4_install(char *, DCU4_INIT_STRUCT_CPTR);


#ifdef __cplusplus
}
#endif


#endif 

/* EOF */
