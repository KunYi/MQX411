
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
*   This file contains implementation of functions
*   for manipulation with Unicode characters/strings
*
*
*END************************************************************************/

#include <stdint.h>
#include "unicode.h"

/*!
 * \brief Encodes given  codepoint as UTF-8 sequence.
 *
 * \param[in]  codepoint  The codepoint to encode.
 * \param[out] output     Pointer to output buffer.
 * \param[in]  output_len Length of the output buffer.
 *
 * \return The number of characters written to the output buffer.
 *
 */
int utf8_encode(uint32_t codepoint, char *output, int output_len)
{
    int code_len;
    uint8_t leading_byte;

    /* Surrogate range is not checked to allow for CESU-8 encoding */

    if (codepoint > UNICODE_MAX) {
        codepoint = UNICODE_REPLACEMENT_CHAR;
    }

    if (codepoint <= 0x7F) {
        code_len = 1;
        leading_byte = codepoint;
    }
    else if (codepoint <= 0x7FF) {
        code_len = 2;
        leading_byte = (codepoint >> 6) | 0xC0;
    }
    else if (codepoint <= 0xFFFF) {
        code_len = 3;
        leading_byte = (codepoint >> 12) | 0xE0;
    }
    else if (codepoint <= 0x1FFFFF) {
        code_len = 4;
        leading_byte = (codepoint >> 18) | 0xF0;
    }
    else if (codepoint <= 0x3FFFFFF) {
        code_len = 5;
        leading_byte = (codepoint >> 24) | 0xF8;
    }
    else if (codepoint <= 0x7FFFFFFF) {
        code_len = 6;
        leading_byte = (codepoint >> 30) | 0xFC;
    }
    else {
        /* Just for sure, this should never happen */
        return 0;
    }

    if (code_len > output_len) {
        return 0;
    }

    *(output++) = leading_byte;

    switch (code_len) {
        case 6:
            *(output++) = ((codepoint >> 24) & 0x3F) | 0x80;
        case 5:
            *(output++) = ((codepoint >> 18) & 0x3F) | 0x80;
        case 4:
            *(output++) = ((codepoint >> 12) & 0x3F) | 0x80;
        case 3:
            *(output++) = ((codepoint >> 6) & 0x3F) | 0x80;
        case 2:
            *(output++) = (codepoint & 0x3F) | 0x80;
        default:
            break;
    }

    return code_len;
}

/* EOF */
