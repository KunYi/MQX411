#ifndef _i2c_qi2c_prv_h
#define _i2c_qi2c_prv_h
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
*   This file contains the definitions of constants and structures
*   required for the I2C drivers for PPC family.
*
*
*END************************************************************************/



/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** QI2C_INFO_STRUCT
** Run time state information for each serial channel
*/
typedef struct qi2c_info_struct
{
    /* Current initialized values */
    QI2C_INIT_STRUCT                  INIT;

    VQI2C_REG_STRUCT_PTR              I2C_PTR;

    /* The previous interrupt handler and data */
    void                  (_CODE_PTR_ OLD_ISR)(void *);
    void                             *OLD_ISR_DATA;

    /* Interrupt vector */
    uint32_t                           VECTOR;
   
    /* Actual baudrate table index */
    uint8_t                            BAUD_INDEX;

    /* Actual mode */
    uint8_t                            MODE;

    /* Actual state */
    uint8_t                            STATE;

    /* Destination address */
    uint8_t                            ADDRESSEE;
   
   /* Operation flags */
    uint8_t                            OPERATION;

    /* Interrupt disabling workaround flag */
    uint8_t                            INT_REQUEST;
    /* Number of bytes requested for receive */
    uint32_t                           RX_REQUEST;

    /* Pointer to the buffer to use for Tx/Rx data */
    unsigned char                         *RX_BUFFER;

    /* Rx write index */
    uint32_t                           RX_IN;

    /* Rx read index */
    uint32_t                           RX_OUT;

    /* Pointer to the buffer to use for current Tx data */
    unsigned char                         *TX_BUFFER;

    /* Tx write index */
    uint32_t                           TX_IN;

    /* Tx read index */
    uint32_t                           TX_OUT;

    /* Statistical information */
    I2C_STATISTICS_STRUCT             STATISTICS;

} QI2C_INFO_STRUCT, * QI2C_INFO_STRUCT_PTR;

typedef volatile struct qi2c_info_struct * VQI2C_INFO_STRUCT_PTR; 

/*
** QI2C_BAUDRATE_STRUCT
*/
typedef struct qi2c_baudrate_struct
{
    /* Baudrate */
    uint32_t                           BAUD_RATE;

    /* Divider */
    uint8_t                            IC;

} QI2C_BAUDRATE_STRUCT, * QI2C_BAUDRATE_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern void   *_bsp_get_qi2c_base_address (uint8_t);
extern uint32_t _bsp_get_qi2c_vector (uint8_t);

#ifdef __cplusplus
}
#endif


#endif
