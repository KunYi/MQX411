#ifndef __ftm_h__
#define __ftm_h__ 1
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
*   Flextimer driver public header file.
*
*
*END************************************************************************/


#include <ioctl.h>


/*-------------------------------------------------------------------------*/
/*
**                            CONSTANT DEFINITIONS
*/


/*-------------------------------------------------------------------------*/
/*
**                            MACRO DECLARATIONS
*/

/*
** brief   Get the FTM channel number.
**
** param [out] a void* variable, return channel number
*/
#define IO_IOCTL_FTM_GET_CHANNEL                     _IO(IO_TYPE_FTM,0x01)

/*
** brief   Dump FTM registers to console
*/
#define IO_IOCTL_FTM_REG_DUMP                        _IO(IO_TYPE_FTM,0x02)

/*
** brief   Register callback function to FTM Quaddec driver
**
** param [in] a void* variable, point to a FTM_QUADDEC_CALLBACK_STRUCT variable
**
** note    According the FTM Quaddec driver init data configuration, if driver works on polling
**           mode, the callback will never be called even if application has register callback
**           function.
*/
#define IO_IOCTL_FTM_QUADDEC_SET_CB                  _IO(IO_TYPE_FTM,0x03)

/*
** brief   Get FTM Quaddec driver evnet
**
** param [out] a void* variable, return event. Refer to FTM_QUADDEC_EVENT for
**                  event type definitions
**
*/
#define IO_IOCTL_FTM_QUADDEC_GET_EVENT               _IO(IO_TYPE_FTM,0x04)

/*
** brief   Get FTM Quaddec inquiring driver mode
**
** param [out] a void* variable, return mode. It may be synchronous mode
**                   or asynchronous mode. Refer to QUADDEC_INQUIRE_MODE for
**                   definitions
**
*/
#define IO_IOCTL_FTM_QUADDEC_GET_MODE                _IO(IO_TYPE_FTM,0x05)


/*-------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/

/*
**
** FTM_QUADDEC_EVENT ENUM
**
** Defines the FTM Quaddec driver event type
*/

typedef enum {
    QUADDEC_NO_EVENT,

    QUADDEC_CNT_UP,

    QUADDEC_CNT_DOWN
} FTM_QUADDEC_EVENT;

/*
**
** QUADDEC_INQUIRE_MODE ENUM
**
** Defines Quaddec event inquiring mode
*/
typedef enum {
    QUADDEC_SYNC_MODE,         //Quaddec driver returns the event by sync mode
    QUADDEC_ASYNC_MODE         //Quaddec driver returns the event by async mode
} QUADDEC_INQUIRE_MODE;

/* FTM Quaddec driver callback prototype */
typedef void (*FTM_QUADDEC_CALLBACK_FUNCTION_PTR)(void *, void *);

/*
**
** FTM_QUADDEC_CALLBACK_STRUCT STRUCTURE
**
** Callback function void* and arg, the arg filed will be returned by the fisrt argument
** of callback function when it is called.
*/
typedef struct ftm_quaddec_callback_struct{
  FTM_QUADDEC_CALLBACK_FUNCTION_PTR cb_fn;
  void *arg;
} FTM_QUADDEC_CALLBACK_STRUCT, * FTM_QUADDEC_CALLBACK_STRUCT_PTR;

/*-------------------------------------------------------------------------*/
/*
**                            FUNCTION PROTOTYPES
*/


#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_uint _io_ftm_quaddec_install(
      char*,
      _mqx_uint (_CODE_PTR_)(void*, char*),
      _mqx_uint (_CODE_PTR_)(void*),
      _mqx_int (_CODE_PTR_)(void*, char*, _mqx_int),
      _mqx_int (_CODE_PTR_)(void*, char*, _mqx_int),
      _mqx_int (_CODE_PTR_)(void*, _mqx_uint, _mqx_uint_ptr),
      void*);


#ifdef __cplusplus
}
#endif


#endif
