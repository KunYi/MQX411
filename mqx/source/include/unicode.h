
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   This include file is used to define constants and function prototypes
*   for manipulation with Unicode characters/strings
*
*
*END************************************************************************/
#ifndef __unicode_h__
#define __unicode_h__ 1


/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

#define UNICODE_MAX 0x10FFFF

#define UNICODE_REPLACEMENT_CHAR 0xFFFD

#define UNICODE_LEAD_SURROGATE_MIN 0xD800
#define UNICODE_LEAD_SURROGATE_MAX 0xDBFF
#define UNICODE_TRAIL_SURROGATE_MIN 0xDC00
#define UNICODE_TRAIL_SURROGATE_MAX 0xDFFF

#define UNICODE_SURROGATE_VALUE_MASK 0x03FF

#define UNICODE_SURROGATE_MASK UNICODE_TRAIL_SURROGATE_MIN
#define UNICODE_LEAD_SURROGATE_MASK UNICODE_LEAD_SURROGATE_MIN
#define UNICODE_TRAIL_SURROGATE_MASK UNICODE_TRAIL_SURROGATE_MIN

#define UNICODE_SURROGATE_MIN UNICODE_LEAD_SURROGATE_MIN
#define UNICODE_SURROGATE_MAX UNICODE_TRAIL_SURROGATE_MAX


/*--------------------------------------------------------------------------*/
/*                       PROTOTYPE DEFINITIONS                              */

#ifdef __cplusplus
extern "C" {
#endif

extern int utf8_encode(uint32_t codepoint, char *output, int output_len);

#ifdef __cplusplus
}
#endif


#endif /* __unicode_h__ */
