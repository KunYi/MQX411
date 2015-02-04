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
#ifndef __hmi_tss_provider_h__
#define __hmi_tss_provider_h__

#include "hmi_provider.h"
#include "hmi_client.h"

typedef struct hmi_tss_init_struct
{
    uint32_t     UID;                                /* usage id */
    uint8_t      FLAG;                               /* flag */
}HMI_TSS_INIT_STRUCT, * HMI_TSS_INIT_STRUCT_PTR;


typedef struct hmi_tss_context_struct
{
    HMI_TSS_INIT_STRUCT_PTR    TSS_TABLE;           /* TSS element table */
    uint8_t                     TSS_COUNT;           /* TSS count number */
    uint8_t                     CONTROL_NUMBER;      /* control type*/
    HMI_CLIENT_STRUCT_PTR      CLIENT_HANDLE;       /* client handle */
}HMI_TSS_CONTEXT_STRUCT, * HMI_TSS_CONTEXT_STRUCT_PTR;

typedef struct hmi_tss_system_control_struct
{
    uint16_t        SYSTEM_CONFIG;                   /* TSS system config */
    uint8_t         SYSTEM_TRIGGER;                  /* TSS trigger */
    uint8_t         NUMBER_OF_SAMPLES;               /* number of samples */
    const uint8_t         *SENSITIVITY_VALUES;       /* pointer to sensitivity values array */
}HMI_TSS_SYSTEM_CONTROL_STRUCT, * HMI_TSS_SYSTEM_CONTROL_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif
HMI_PROVIDER_STRUCT_PTR hmi_tss_keypad_provider_init(const HMI_TSS_INIT_STRUCT *, uint8_t);
HMI_PROVIDER_STRUCT_PTR hmi_tss_rotary_provider_init(const HMI_TSS_INIT_STRUCT *, uint8_t);
HMI_PROVIDER_STRUCT_PTR hmi_tss_slider_provider_init(const HMI_TSS_INIT_STRUCT *, uint8_t);
uint32_t hmi_tss_provider_deinit(HMI_PROVIDER_STRUCT_PTR provider);
void hmi_tss_init(const HMI_TSS_SYSTEM_CONTROL_STRUCT system_control);
void hmi_tss_on_fault(uint8_t);
void hmi_tss_empty_function(void);
#ifdef __cplusplus
}
#endif

#endif
