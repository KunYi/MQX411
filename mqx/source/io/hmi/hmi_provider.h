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
*   definitions private to hmi provider.
*
*
*END************************************************************************/
#ifndef __hmi_provider_h__
#define __hmi_provider_h__ 1

#include "psptypes.h"

typedef struct hmi_provider_struct
{
    void        (_CODE_PTR_ POLL_PTR)           (void *);                          /* poll */
    bool     (_CODE_PTR_ GET_VALUE_PTR)      (void *, uint32_t, uint32_t *);    /* get value */
    void        (_CODE_PTR_ INSTALL_INTERRUPT_PTR)  (void *);                      /* install INT */
    void        (_CODE_PTR_ UNINSTALL_INTERRUPT_PTR)  (void *);                    /* uninstall INT */
    void         (_CODE_PTR_ ATTACH_CLIENT) (void *,void *);                      /* attach client */
    bool     (_CODE_PTR_ SET_VALUE_PTR)      (void *, uint32_t, uint32_t);        /* set value */
    void       *CONTEXT_PTR;                                                        /* pointer to context data */
} HMI_PROVIDER_STRUCT, * HMI_PROVIDER_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
