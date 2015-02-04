/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
* Copyright 2004-2011 Embedded Access Inc.
* Copyright 1989-2011 ARC International
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
*   The file contains functions prototype, defines, structure
*   definitions of HMI client
*
*
*END************************************************************************/
#ifndef __hmi_client_h__
#define __hmi_client_h__ 1

#include "psptypes.h"
#include "hmi_provider.h"
#include "mqx.h"

#define HMI_VALUE_ON     0
#define HMI_VALUE_OFF    1

#define HMI_VALUE_RELEASE      0
#define HMI_VALUE_PUSH         1
#define HMI_VALUE_MOVEMENT     2

typedef struct hmi_client_struct
{
    void ( _CODE_PTR_ ON_CHANGE)(void *, uint32_t, uint32_t);/* function to call on change */
    HMI_PROVIDER_STRUCT_PTR          *PROVIDER_TABLE;       /* array of pointers */
    uint32_t                     MAX_PROVIDER_COUNT;         /* max provider number */
    void                       *CONTEXT_PTR;                /* pointer to context */
} HMI_CLIENT_STRUCT, * HMI_CLIENT_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif
bool hmi_add_provider(HMI_CLIENT_STRUCT_PTR, HMI_PROVIDER_STRUCT_PTR);
bool hmi_remove_provider(HMI_CLIENT_STRUCT_PTR, HMI_PROVIDER_STRUCT_PTR);
HMI_CLIENT_STRUCT_PTR hmi_client_init(void);
#ifdef __cplusplus
}
#endif

#endif

