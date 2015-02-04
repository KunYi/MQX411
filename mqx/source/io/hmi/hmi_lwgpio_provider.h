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
*   This header file is for HMI lwgpio provider. It contains  defines,
*   structures.
*
*
*END************************************************************************/
#ifndef __hmi_lwgpio_provider_h__
#define __hmi_lwgpio_provider_h__ 1

#include "hmi_provider.h"
#include "hmi_client.h"
#include "psptypes.h"
#include "mqx.h"
#include "bsp.h"

/* FLAG DEFINES */
#define HMI_LWGPIO_FLAG_DIRECTION_IN       (0x00000000)
#define HMI_LWGPIO_FLAG_DIRECTION_OUT      (0x00000001)
#define HMI_LWGPIO_FLAG_ACTIVE_STATE_0     (0x00000000)
#define HMI_LWGPIO_FLAG_ACTIVE_STATE_1     (0x00000002)
#define HMI_LWGPIO_FLAG_PULL_UP_OFF        (0x00000000)
#define HMI_LWGPIO_FLAG_PULL_UP_ON         (0x00000004)
#define HMI_LWGPIO_FLAG_PULL_DOWN_OFF      (0x00000000)
#define HMI_LWGPIO_FLAG_PULL_DOWN_ON       (0x00000008)
#define HMI_LWGPIO_FLAG_INT_OFF            (0x00000000)
#define HMI_LWGPIO_FLAG_INT_ON             (0x00000010)

/* GET FLAGS */
#define HMI_LWGPIO_GET_DIRECTION(x)        ((x)&0x1)
#define HMI_LWGPIO_GET_ACTIVE_STATE(x)     (((x)>>1)&0x1)
#define HMI_LWGPIO_GET_PULL_UP(x)          (((x)>>2)&0x1)
#define HMI_LWGPIO_GET_PULL_DOWN(x)        (((x)>>3)&0x1)
#define HMI_LWGPIO_GET_INT(x)              (((x)>>4)&0x1)

#define LWGPIO_PIN_FROM_ID(id)  (((id) & LWGPIO_PIN_MASK) >> LWGPIO_PIN_SHIFT)
#define LWGPIO_PORT_FROM_ID(id) (((id) & LWGPIO_PORT_MASK) >> LWGPIO_PORT_SHIFT)

/* LWGPIO INITIALIZATION STRUCTURE */
typedef struct hmi_lwgpio_init_struct
{
    uint32_t UID;                /* usage id */
    uint32_t PID;                /* pin id */
    uint32_t FUNC;               /* functionality of the pin */
    uint32_t FLAG;               /* polarity of the pin */
}HMI_LWGPIO_INIT_STRUCT, * HMI_LWGPIO_INIT_STRUCT_PTR;

/* LWGPIO STRUCTURE */
typedef struct hmi_lwgpio_struct
{
    LWGPIO_VALUE            LAST_STATE;         /* last state */
    LWGPIO_STRUCT           HANDLE;             /* handle of LWGPIO */
    HMI_LWGPIO_INIT_STRUCT  INIT_STRUCT;        /* initialization struct */
    INT_ISR_FPTR            INIT_ISR;           /* pointer to ISR */
}HMI_LWGPIO_STRUCT, * HMI_LWGPIO_STRUCT_PTR;

/* LWGPIO CONTEXT STRUCTURE */
typedef struct hmi_lwgpio_context_struct
{
    HMI_LWGPIO_STRUCT_PTR   BTNLED_TABLE;       /* btn led table */
    uint32_t                 BTNLED_COUNT;       /* number of LWGPIO */
    HMI_CLIENT_STRUCT_PTR   CLIENT_HANDLE;
}HMI_LWGPIO_CONTEXT_STRUCT, * HMI_LWGPIO_CONTEXT_STRUCT_PTR;


#ifdef __cplusplus
extern "C" {
#endif
HMI_PROVIDER_STRUCT_PTR hmi_lwgpio_provider_init(const HMI_LWGPIO_INIT_STRUCT *);
uint32_t hmi_lwgpio_provider_deinit(HMI_PROVIDER_STRUCT_PTR);
#ifdef __cplusplus
}
#endif

#endif
