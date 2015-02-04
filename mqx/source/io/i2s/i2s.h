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
*   This include file is used to provide information needed by
*   applications using the I2S I/O functions.
*
*
*END************************************************************************/


#ifndef _i2s_h_
#define _i2s_h_ 1

#include <ioctl.h>


/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/
#define I2S_CHAR_BIT    (0x08)

/*
** I2S Clock sources
*/
#define I2S_CLK_INT     (0x00)
#define I2S_CLK_EXT     (0x01)

/*
** I2S Bus Modes
*/
#define I2S_MODE_MASTER (0x01)
#define I2S_MODE_SLAVE  (0x02)
/*
** I2S I/O Mode
*/
#define I2S_IO_READ     (0x10)
#define I2S_IO_WRITE    (0x20)
/*
** I2S Error Codes
*/
#define I2S_OK                              (0x00)
#define I2S_ERROR_INVALID_PARAMETER         (I2S_ERROR_BASE | 0x01)
#define I2S_ERROR_CHANNEL_INVALID           (I2S_ERROR_BASE | 0x02)
#define I2S_ERROR_MODE_INVALID              (I2S_ERROR_BASE | 0x03)
#define I2S_ERROR_WORD_LENGTH_UNSUPPORTED   (I2S_ERROR_BASE | 0x04)
#define I2S_ERROR_CLK_INVALID               (I2S_ERROR_BASE | 0x05)
#define I2S_ERROR_DIVIDER_VALUE_INVALID     (I2S_ERROR_BASE | 0x06)
#define I2S_ERROR_FREQUENCY_INVALID         (I2S_ERROR_BASE | 0x07)
#define I2S_ERROR_BUFFER_SMALL              (I2S_ERROR_BASE | 0x08)
#define I2S_ERROR_DEVICE_BUSY               (I2S_ERROR_BASE | 0x09)
#define I2S_ERROR_PARAM_OUT_OF_RANGE        (I2S_ERROR_BASE | 0x0A)
#define I2S_ERROR_IO_INIT_FAILED            (I2S_ERROR_BASE | 0x0B)

/*
** IOCTL calls specific to I2S
*/
#define IO_TYPE_I2S                                 0x15
#define IO_IOCTL_I2S_SET_MODE_MASTER                _IO(IO_TYPE_I2S, 0x01)
#define IO_IOCTL_I2S_SET_MODE_SLAVE                 _IO(IO_TYPE_I2S, 0x02)
#define IO_IOCTL_I2S_SET_CLOCK_SOURCE_INT           _IO(IO_TYPE_I2S, 0x03)
#define IO_IOCTL_I2S_SET_CLOCK_SOURCE_EXT           _IO(IO_TYPE_I2S, 0x04)
#define IO_IOCTL_I2S_SET_DATA_BITS                  _IO(IO_TYPE_I2S, 0x05)
#define IO_IOCTL_I2S_DISABLE_DEVICE                 _IO(IO_TYPE_I2S, 0x06)
#define IO_IOCTL_I2S_ENABLE_DEVICE                  _IO(IO_TYPE_I2S, 0x07)
#define IO_IOCTL_I2S_SET_MCLK_FREQ                  _IO(IO_TYPE_I2S, 0x08)
#define IO_IOCTL_I2S_SET_FS_FREQ                    _IO(IO_TYPE_I2S, 0x09)
#define IO_IOCTL_I2S_TX_DUMMY_ON                    _IO(IO_TYPE_I2S, 0x0A)
#define IO_IOCTL_I2S_TX_DUMMY_OFF                   _IO(IO_TYPE_I2S, 0x0B)
#define IO_IOCTL_I2S_GET_MODE                       _IO(IO_TYPE_I2S, 0x0C)
#define IO_IOCTL_I2S_GET_CLOCK_SOURCE               _IO(IO_TYPE_I2S, 0x0D)
#define IO_IOCTL_I2S_GET_DATA_BITS                  _IO(IO_TYPE_I2S, 0x0E)
#define IO_IOCTL_I2S_GET_MCLK_FREQ                  _IO(IO_TYPE_I2S, 0x0F)
#define IO_IOCTL_I2S_GET_BCLK_FREQ                  _IO(IO_TYPE_I2S, 0x10)
#define IO_IOCTL_I2S_GET_TX_DUMMY                   _IO(IO_TYPE_I2S, 0x11)
#define IO_IOCTL_I2S_GET_FS_FREQ                    _IO(IO_TYPE_I2S, 0x12)
#define IO_IOCTL_I2S_GET_STATISTICS                 _IO(IO_TYPE_I2S, 0x13)
#define IO_IOCTL_I2S_SET_TXFIFO_WATERMARK           _IO(IO_TYPE_I2S, 0x14)
#define IO_IOCTL_I2S_SET_RXFIFO_WATERMARK           _IO(IO_TYPE_I2S, 0x15)
#define IO_IOCTL_I2S_GET_TXFIFO_WATERMARK           _IO(IO_TYPE_I2S, 0x16)
#define IO_IOCTL_I2S_GET_RXFIFO_WATERMARK           _IO(IO_TYPE_I2S, 0x17)
#define IO_IOCTL_I2S_SET_CLK_ALWAYS_ENABLED_ON      _IO(IO_TYPE_I2S, 0x18)
#define IO_IOCTL_I2S_SET_CLK_ALWAYS_ENABLED_OFF     _IO(IO_TYPE_I2S, 0x19)
#define IO_IOCTL_I2S_GET_CLK_ALWAYS_ENABLED         _IO(IO_TYPE_I2S, 0x1A)
#define IO_IOCTL_I2S_CLEAR_STATISTICS               _IO(IO_TYPE_I2S, 0x1B)

/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** I2S_STATISTICS_STRUCT
** I2S run time statistics structure
*/

typedef struct i2s_statistics_struct
{
    /* Number of SAI interrupts so far */
    uint32_t INTERRUPTS;

    /* FIFO error */
    uint32_t FIFO_ERROR;

    /* FIFO sync error */
    uint32_t BUFFER_ERROR;

    /* Number of valid bytes received (not dummy receives) */
    uint32_t RX_PACKETS;

    /* Number of valid bytes transmitted (not dummy transmits) */
    uint32_t TX_PACKETS;

    /* Number of bytes currently read or wrote */
    uint32_t PACKETS_PROCESSED;

    /* Number of bytes currently buffered */
    uint32_t PACKETS_QUEUED;

    /* Number of bytes requested for reading or writing */
    uint32_t PACKETS_REQUESTED;

} I2S_STATISTICS_STRUCT, * I2S_STATISTICS_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

   extern _mqx_uint _io_i2s_install(
    char *,
    _mqx_uint (_CODE_PTR_)(void *, char *),
    _mqx_uint (_CODE_PTR_)(void *, void *),
    _mqx_int (_CODE_PTR_)(void *, char *, _mqx_int),
    _mqx_int (_CODE_PTR_)(void *, char *, _mqx_int),
    _mqx_int (_CODE_PTR_)(void *, _mqx_uint, _mqx_uint_ptr),
    void *);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
