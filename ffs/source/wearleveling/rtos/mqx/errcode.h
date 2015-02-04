#ifndef _WL_ERRCODE_H_
#define _WL_ERRCODE_H_
/*HEADER**********************************************************************
*
* Copyright 2012 Freescale Semiconductor, Inc.
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   Error code & types convert between WL and RTOS
*
*END************************************************************************/

enum NANDWLContext
{
    kNandDontCare = 0,
    kNandReadPage,
    kNandWritePage,
    kNandReadMetadata,
    kNandEraseBlock,
    kNandEraseMultiBlock,
    kNandMarkBlockBad,
    kNandIsBlockBad
};

#ifdef __cplusplus

extern "C"
{
#endif /* __cplusplus*/

RtStatus_t os_err_code_to_wl(_mqx_uint status, uint32_t context);
_mqx_uint wl_err_code_to_os(RtStatus_t status, uint32_t context);
char * wl_err_to_str(RtStatus_t wl_status, uint32_t context);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _WL_ERRCODE_H_*/

/* EOF */
