#ifndef __dcu4_prv_h__ 
#define __dcu4_prv_h__
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
*   This file contains definitions private to the DCU4 driver.
*
*
*END************************************************************************/

#include "dcu4.h"

/*--------------------------------------------------------------------------*/
/*
**                    CONSTANT DEFINITIONS
*/


/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** DCU4_EVENT_HANDLER_STRUCT
** DCU4 run time event callback structure
*/
typedef struct dcu4_event_handler_struct DCU4_EVENT_HANDLER_STRUCT, * DCU4_EVENT_HANDLER_STRUCT_PTR;

struct dcu4_event_handler_struct
{
    /* The event handler and data */
    DCU4_EVENT_STRUCT             EVENT;

    /* Event handler list in DCU4_INFO_STRUCT */
    DCU4_EVENT_HANDLER_STRUCT_PTR INFO_NEXT;

    /* Event handler list in DCU4_DEVICE_STRUCT */
    DCU4_EVENT_HANDLER_STRUCT_PTR DEVICE_NEXT;
};

/*
** DCU4_INFO_STRUCT
** DCU4 run time state information
*/
typedef struct dcu4_info_struct
{
    /* The event handlers */
    DCU4_EVENT_HANDLER_STRUCT_PTR EVENTS_HEAD;
    
} DCU4_INFO_STRUCT, * DCU4_INFO_STRUCT_PTR;

/*
** DCU4_DEVICE_STRUCT
** DCU4 install parameters
*/
typedef struct dcu4_device_struct
{
    /* The current init values for this device */
    DCU4_INIT_STRUCT_CPTR         INIT;
   
    /* The device number */
    uint32_t                      CHANNEL;

    /* The number of opened file descriptors */
    uint32_t                      COUNT;

    /* The previous interrupt handler and data */
    void              (_CODE_PTR_ OLD_ISR)(void *);
    void                         *OLD_ISR_DATA;

    /* The vector number of interrupt controller */
    uint32_t                      VECTOR;

    /* Current timing configuration */
    DCU4_TIMING_STRUCT            TIMING;

    /* Event handlers that registered */
    DCU4_EVENT_HANDLER_STRUCT_PTR EVENTS_HEAD;

    /* Event registered count */
    uint32_t                      EVENTS_REG_COUNT[DCU_EVENT_NUM];

} DCU4_DEVICE_STRUCT, * DCU4_DEVICE_STRUCT_PTR;
    
/*--------------------------------------------------------------------------*/
/* 
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_int _dcu4_open (MQX_FILE_PTR, char *, char *);
extern _mqx_int _dcu4_close (MQX_FILE_PTR);
extern _mqx_int _dcu4_ioctl (MQX_FILE_PTR, uint32_t, void *);
extern _mqx_int _dcu4_uninstall (IO_DEVICE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif
      
#endif
/* EOF */
