#ifndef _esai_vport_h_
#define _esai_vport_h_ 1
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
*   applications using the ESAI VPORT I/O device driver functions.
*
*
*END************************************************************************/

#include <ioctl.h>
#include "esai_vport_config.h"
#include "esai_fifo_prv.h"

/*--------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/

/*
** IOCTL calls specific to ESAI VPORT
*/

/*
** The command set the RX configuration to ESAI VPORT module.
** Parameter: a pointer to ESAI_VPORT_CONFIG_STRUCT structure
*/
#define IO_IOCTL_ESAI_VPORT_CONFIG              _IO(IO_TYPE_ESAI_VPORT,0x01)

/*
** The command start the ESAI VPORT node
** Parameter: AUD_IO_FW_DIRECTION value for direction
*/
#define IO_IOCTL_ESAI_VPORT_START               _IO(IO_TYPE_ESAI_VPORT,0x02)

/*
** The command stop the ESAI VPORT node
** Parameter: AUD_IO_FW_DIRECTION value for direction
*/
#define IO_IOCTL_ESAI_VPORT_STOP                _IO(IO_TYPE_ESAI_VPORT,0x03)

/*
** The command reset the ESAI VPORT node
** Parameter: NULL
*/
#define IO_IOCTL_ESAI_VPORT_RESET               _IO(IO_TYPE_ESAI_VPORT,0x04)

/*
** The command set the ASRC to the ESAI VPORT node
** Parameter: NULL
*/
#define IO_IOCTL_ESAI_VPORT_SET_ASRC            _IO(IO_TYPE_ESAI_VPORT,0x05)

/*
** The command clear the ESAI VPORT node buffer
** Parameter: NULL
*/
#define IO_IOCTL_ESAI_VPORT_CLEAR_BUF           _IO(IO_TYPE_ESAI_VPORT,0x06)

/*
** The command set the sample rate to the ESAI VPORT node
** Parameter: sample rate value in uint32_t
*/
#define IO_IOCTL_ESAI_VPORT_SET_SAMPLE_RATE     _IO(IO_TYPE_ESAI_VPORT,0x07)

/*
** The command set the hw interface to the ESAI VPORT node
** Parameter: hardware interface ESAI_VPORT_HW_INTERFACE
*/
#define IO_IOCTL_ESAI_VPORT_SET_HW_INTERFACE    _IO(IO_TYPE_ESAI_VPORT,0x08)

/*
** The command enable the ASRC plugin in the ESAI VPORT node
** Parameter: ASRC input sample rate in uint32_t
*/
#define IO_IOCTL_ESAI_VPORT_ENABLE_ASRC_PLUGIN  _IO(IO_TYPE_ESAI_VPORT,0x09)

/*
** The command disable the ASRC plugin in the ESAI VPORT node
** Parameter: NULL
*/
#define IO_IOCTL_ESAI_VPORT_DISABLE_ASRC_PLUGIN _IO(IO_TYPE_ESAI_VPORT,0x0A)

/*
** The command gets the error code in the ESAI VPORT node
** Parameter: NULL
*/
#define IO_IOCTL_ESAI_VPORT_GET_ERROR           _IO(IO_TYPE_ESAI_VPORT,0x0B)

/*
** The command sets the tx timeout value to the ESAI VPORT node
** Parameter: NULL
*/
#define IO_IOCTL_ESAI_VPORT_SET_TX_TIMEOUT      _IO(IO_TYPE_ESAI_VPORT,0x0C)

/*
** The command sets the tx timeout value to the ESAI VPORT node
** Parameter: NULL
*/
#define IO_IOCTL_ESAI_VPORT_SET_RX_TIMEOUT      _IO(IO_TYPE_ESAI_VPORT,0x0D)



/*
** ESAI VPORT Error Codes
*/

/*
** Error code means the return OK.
*/
#define ESAI_VPORT_OK                            (MQX_OK)

/*
** Error code means the ESAI VPORT device busy
*/
#define ESAI_VPORT_ERROR_DEVICE_BUSY             (ESAI_VPORT_ERROR_BASE | 0x01)

/*
** Error code means the ESAI VPORT Transceiver number invalid
*/
#define ESAI_VPORT_ERROR_TR_INVALID              (ESAI_VPORT_ERROR_BASE | 0x02)

/*
** Error code means the parameter setting to ESAI VPORT is invalid
*/
#define ESAI_VPORT_ERROR_INVALID_PARAMETER       (ESAI_VPORT_ERROR_BASE | 0x03)

/*
** Error code means some internal errors occur
*/
#define ESAI_VPORT_ERROR_INTERNAL_ERROR          (ESAI_VPORT_ERROR_BASE | 0x04)

/*
** Error code means a NULL pointer passed to internal function
*/
#define ESAI_VPORT_ERROR_INVALID_POINTER         (ESAI_VPORT_ERROR_BASE | 0x05)

/*
** Error code means acess to asrc function fails
*/
#define ESAI_VPORT_ERROR_ASRC_FAIL               (ESAI_VPORT_ERROR_BASE | 0x06)

/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** ESAI VPORT pcmmgr interfaces registration structure
** Holds the function pointer registered for further pcmmgr operations
*/
typedef struct esai_vport_pcmm_funcs_struct{
    int32_t (_CODE_PTR_ open)(void * dev, uint32_t mode);
    int32_t (_CODE_PTR_ close)(void * dev);
    int32_t (_CODE_PTR_ start_record)(void * dev);
    int32_t (_CODE_PTR_ start_playback)(void * dev);
    int32_t (_CODE_PTR_ stop_record)(void * dev);
    int32_t (_CODE_PTR_ stop_playback)(void * dev);
    int32_t (_CODE_PTR_ set_volume)(void * dev, uint32_t volume);
    int32_t (_CODE_PTR_ get_volume)(void * dev, uint32_t *volume);
    int32_t (_CODE_PTR_ reset)(void * dev);
    int32_t (_CODE_PTR_ read)(void * dev, char *buf, int32_t size);
    int32_t (_CODE_PTR_ write)(void * dev, char *buf, int32_t size);
    int32_t (_CODE_PTR_ set_tx_format)(void * dev, uint32_t width);
    int32_t (_CODE_PTR_ set_rx_format)(void * dev, uint32_t width);
    int32_t (_CODE_PTR_ set_tx_rate)(void * dev, uint32_t rate);
    int32_t (_CODE_PTR_ get_tx_rate)(void * dev, uint32_t *rate);
    int32_t (_CODE_PTR_ set_rx_rate)(void * dev, uint32_t rate);
    int32_t (_CODE_PTR_ get_rx_rate)(void * dev, uint32_t *rate);
    int32_t (_CODE_PTR_ get_opt_mode)(void * dev, int32_t *mode);
    int32_t (_CODE_PTR_ set_parameters)(void * dev, void * para);
    int32_t (_CODE_PTR_ set_tx_asrc)(void * dev, void * funcs);
    int32_t (_CODE_PTR_ set_rx_asrc)(void * dev, void * funcs);
    int32_t (_CODE_PTR_ clear_tx_buf)(void * dev);
    int32_t (_CODE_PTR_ clear_rx_buf)(void * dev);
    int32_t (_CODE_PTR_ set_chnl_type)(void * dev, uint32_t type);
    int32_t (_CODE_PTR_ set_tx_timeout)(void * dev, uint32_t timeout);
    int32_t (_CODE_PTR_ set_rx_timeout)(void * dev, uint32_t timeout);
    int32_t (_CODE_PTR_ get_error)(void * dev, uint32_t *error);
}ESAI_VPORT_PCMM_FUNCS_STRUCT, * ESAI_VPORT_PCMM_FUNCS_STRUCT_PTR;


/*--------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_uint _io_esai_vport_install(
      char *,
      _mqx_uint (_CODE_PTR_)(void *, void * *, char *, void *),
      _mqx_uint (_CODE_PTR_)(void *, void *),
      _mqx_int (_CODE_PTR_)(void *, char *, _mqx_int),
      _mqx_int (_CODE_PTR_)(void *, char *, _mqx_int),
      _mqx_int (_CODE_PTR_)(void *, _mqx_uint, _mqx_uint_ptr),
      void *,
      ESAI_VPORT_PCMM_FUNCS_STRUCT_PTR,
      ESAI_FIFO_CONTEXT_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
