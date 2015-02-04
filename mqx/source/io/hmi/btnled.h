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
*   The file contains functions prototype, defines, structure
*   definitions for HMI btn_led driver
*
*
*END************************************************************************/
#ifndef __btnled_h__
#define __btnled_h__ 1

#include "psptypes.h"
#include "mqx.h"
#include "bsp.h"
#include "hmi_client.h"


typedef struct btnled_clbreg
{
    uint32_t STATE;                              /* callback register state */
    uint32_t UID;                                /* UID */
    void   *CALLBACK_PARAMETER;                 /* callback parameter */
    void(_CODE_PTR_ CLBFUNCTION) (void *);     /* callback function */
    struct btnled_clbreg      *NEXT_CALLBACK;   /* next callback */
}BTNLED_CLBREG_STRUCT, * BTNLED_CLBREG_STRUCT_PTR;

typedef struct btnled_context
{
    BTNLED_CLBREG_STRUCT_PTR    CLBREG;             /* pointer to callback register structure */
    LWSEM_STRUCT                LOCK_BTNLED_SEM;    /* semaphore structure */
}BTNLED_CONTEXT_STRUCT, * BTNLED_CONTEXT_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif
HMI_CLIENT_STRUCT_PTR btnled_init(void);
void btnled_poll(HMI_CLIENT_STRUCT_PTR);
bool btnled_get_value(HMI_CLIENT_STRUCT_PTR, uint32_t, uint32_t *);
bool btnled_set_value(HMI_CLIENT_STRUCT_PTR, uint32_t, uint32_t);
bool btnled_toogle(HMI_CLIENT_STRUCT_PTR, uint32_t);
BTNLED_CLBREG_STRUCT_PTR btnled_add_clb(HMI_CLIENT_STRUCT_PTR, uint32_t, uint32_t, void(CODE_PTR)(void *), void *);
bool btnled_remove_clb(HMI_CLIENT_STRUCT_PTR, BTNLED_CLBREG_STRUCT_PTR);
uint32_t btnled_deinit(HMI_CLIENT_STRUCT_PTR);
#ifdef __cplusplus
}
#endif
#endif

