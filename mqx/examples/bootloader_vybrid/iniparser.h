/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*
*END************************************************************************/

#ifndef __iniparser_h__
#define __iniparser_h__

#include <psptypes.h>

#define INIPARSER_CHAR_IS_EOS(x) \
(x == '\0')

#define INIPARSER_CHAR_IS_COMMENT(x) \
(x == ';')

#define INIPARSER_CHAR_IS_EOL(x) \
(((x) == 0xa) || ((x) == 0xd))

#define INIPARSER_LENGTH_PROPERTY_NAME  (64)
#define INIPARSER_LENGTH_PROPERTY_VALUE (128)
#define INIPARSER_LENGTH_LABEL_NAME     (64)
#define INIPARSER_LENGTH_LINE           (256)

#if (INIPARSER_LENGTH_LINE <= INIPARSER_LENGTH_PROPERTY_NAME) || \
    (INIPARSER_LENGTH_LINE <= INIPARSER_LENGTH_PROPERTY_VALUE) || \
    (INIPARSER_LENGTH_LINE <= INIPARSER_LENGTH_LABEL_NAME)
#error "INIPARSER_LENGTH_LINE value is too short "
#endif


typedef void (*iniparser_label_fptr)(const char *);
typedef void (*iniparser_property_fptr)(const char *, const char *);


uint32_t iniparser_load(const char *, iniparser_label_fptr, iniparser_property_fptr);
uint32_t iniparser_recognize_comment(const char *);
uint32_t iniparser_recognize_label_name(const char *, const char *);
uint32_t iniparser_recognize_label(const char *, char *, uint32_t);
uint32_t iniparser_recognize_property(const char *, char *, uint32_t, char *, uint32_t);


#endif
