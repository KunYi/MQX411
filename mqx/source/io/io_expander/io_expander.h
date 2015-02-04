#ifndef _io_expander_h_
#define _io_expander_h_ 1
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
*   This include file is used to provide information needed by
*   applications using the IO expander functions.
*
*
*END************************************************************************/
#include <ioctl.h>


/*
** IOCTL calls specific to IO EXPANDER
*/
#define IO_IOCTL_IOEXP_SET_PIN_NO                         _IO(IO_TYPE_IOEXP,0x01)
#define IO_IOCTL_IOEXP_SET_PIN_DIR_IN                     _IO(IO_TYPE_IOEXP,0x02)
#define IO_IOCTL_IOEXP_SET_PIN_DIR_OUT                    _IO(IO_TYPE_IOEXP,0x03)
#define IO_IOCTL_IOEXP_SET_PIN_VAL_HIGH                   _IO(IO_TYPE_IOEXP,0x04)
#define IO_IOCTL_IOEXP_SET_PIN_VAL_LOW                    _IO(IO_TYPE_IOEXP,0x05)
#define IO_IOCTL_IOEXP_GET_INPUT_REG                      _IO(IO_TYPE_IOEXP,0x06)
#define IO_IOCTL_IOEXP_CLEAR_PIN_NO                       _IO(IO_TYPE_IOEXP,0x07)

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

extern uint32_t _io_expander_install(
      char *,
      uint32_t (*)(void *, void **),
      uint32_t (*)(void *, void **, char *),
      uint32_t (*)(void *, void *),
      uint32_t (*)(void *),
      int32_t (*)(void *, void *, char *, int32_t),
      int32_t (*)(void *, void *, char *, int32_t),
      int32_t (*)(void *, void *, uint32_t, uint32_t *),
      void *);


#ifdef __cplusplus
}
#endif

#endif
/* EOF */
