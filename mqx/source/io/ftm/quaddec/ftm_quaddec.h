#ifndef __ftm_quaddec_h__
#define __ftm_quaddec_h__ 1
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
*   This file provides private definitions and APIs of constants and structures
*   of FTM Quadrature Decoder driver.
*
*
*END************************************************************************/



/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/

/* defines the FTM driver clock source */
typedef enum {
    NO_CLK,
    SYSTEM_CLK,         //System clock (IPG clock)
    FIXED_FREQ_CLK,     //Fixed frequency clock, depend on the clock controller settings
    EXTERNAL_CLK        //external clock
} FTM_CLOCK_TYPE;

/*
  * defines Quaddec phase A and phase B polarity type,
  * refer to PHAPOL and PHBPOL  bits of QDCTRL register
  */
typedef enum {
    NORMAL_POL,     /*Normal polarity. Phase A or Phase B input signal is not
                                      inverted before identifying the rising and falling
                                      edges of this signal*/
    INVERTED_POL    /*Inverted polarity. Phase A or Phase B input signal is
                                       inverted before identifying the rising and falling edges
                                       of this signal*/
} QUADDEC_POL_TYPE;

/* defines Quaddec work mode, refer to QUADMODE field of QDCTRL register */
typedef enum {
    PHASE_MODE,         //Phase A and Phase B encoding mode
    CNT_DIR_MODE        //Count and direction encoding mode
} QUADDEC_MODE;

/* FTM Quaddec driver init data structure */
typedef struct quaddec_init_info {
    /*
         * FTM channel number
         * 0: FTM0
         * 1: FTM1
         * 2: FTM2
         * 3: FTM3
         */
    uint8_t          CHANNEL;

    /*
         * Indicates which clock source is selected, the clock source types are defined by
         * FTM_CLOCK_TYPE enum.
         * 1: System clock (IPG clock)
         * 2: Fixed frequency clock, depned on the clock controller settings
         * 3: external clock
         */
    uint8_t          CLK_TYPE;

    /*
         * Quadrature Decoder mode, refer to QUADDEC_MODE defines:
         */
    uint8_t          QUADDEC_MODE;

    /* Phase A input polarity, refer to QUADDEC_POL_TYPE defines */
    uint8_t          PHA_POL;

    /* Phase B input polarity, refer to QUADDEC_POL_TYPE defines  */
    uint8_t          PHB_POL;

    /* Filter value, this filed is useful only when filter is enabled */
    uint8_t          FILTER_VAL;

    /* the value of modulo register*/
    uint16_t         MODULO;

    /* the initial value counter, this value is used to reset the COUNTER register */
    uint16_t         CNT_INIT;

    /* Flag indicates enabling or disabling FTM hardware filter */
    bool             FILTER_EN;

    /*
         * Enable FTM Quaddec interrupt or not, if enabled,
         * driver works on interrupt mode, if not, works on
         * pooling mode */
    bool             INT_EN;
} QUADDEC_INIT_INFO_STRUCT, * QUADDEC_INIT_INFO_STRUCT_PTR;

/* FTM Quaddec driver io info structure */
typedef struct quaddec_io_info {
    /* the base address of the FTM device */
    FTM_MemMapPtr                       FTM_PTR;

    /* Quadrature Decoder event, refer to defines in FTM_QUADDEC_EVENT */
    uint8_t                              QUADDEC_EVENT;

    /* Time stamp for event, it is useful for software filter */
    uint32_t                             EVENT_TS;

    /* call back function */
    FTM_QUADDEC_CALLBACK_STRUCT_PTR     CB_PTR;

} QUADDEC_IO_INFO_STRUCT, * QUADDEC_IO_INFO_STRUCT_PTR;

typedef struct io_ftm_quaddec_struct
{

   /* The I/O init function */
   _mqx_uint (_CODE_PTR_ DEV_INIT)(void*, char*);

   /* The I/O deinit function */
   _mqx_uint (_CODE_PTR_ DEV_DEINIT)(void*);

   /* The read function */
   _mqx_int  (_CODE_PTR_ DEV_READ)(void*, char*, _mqx_int);

   /* The write function */
   _mqx_int  (_CODE_PTR_ DEV_WRITE)(void*, char*, _mqx_int);

   /* The ioctl function, (change bauds etc) */
   _mqx_int  (_CODE_PTR_ DEV_IOCTL)(void*, _mqx_uint, _mqx_uint_ptr);

   /* The I/O channel initialization data */
   void*                 DEV_INIT_DATA_PTR;

   /* Device specific information */
   void*                 DEV_INFO_PTR;

   /* it is used to backup previous ISR data*/
   void*                 OLD_ISR_DATA;

   /* it is used to backup previous ISR*/
   void*                 OLD_ISR;

   /* Open count for number of accessing file descriptors */
   _mqx_uint             COUNT;

   /* Open flags for this channel */
   _mqx_uint             FLAGS;

} IO_FTM_QUADDEC_STRUCT, * IO_FTM_QUADDEC_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t _frd_ftm_quaddec_install(
      char*,
      QUADDEC_INIT_INFO_STRUCT_PTR);


#ifdef __cplusplus
}
#endif

#endif
/* EOF */

