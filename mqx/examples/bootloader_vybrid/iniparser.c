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

#include <mqx.h>
#include <fio.h>
#include "iniparser.h"

uint32_t iniparser_load(
    /* [IN] '\0' terminated line */
    const char *filename,
    /* [IN], 'label' expression callback */
    iniparser_label_fptr label_call,
    /* [IN], 'property' expression callback */
    iniparser_property_fptr property_call
)
{
    MQX_FILE_PTR file_handle = NULL;
    char line[INIPARSER_LENGTH_LINE];
    char label_name[INIPARSER_LENGTH_LABEL_NAME];
    char property_value[INIPARSER_LENGTH_PROPERTY_VALUE];
    char property_name[INIPARSER_LENGTH_PROPERTY_NAME];
    uint32_t recognized_count;
    
    file_handle = fopen (filename, "r+");
    if (NULL == file_handle)
    { 
        return ~(MQX_OK);
    }
    
    /* read file by lines */
    while (NULL != fgets(line, INIPARSER_LENGTH_LINE, file_handle))
    {
        /* recognize comment */
        recognized_count = iniparser_recognize_comment(line);
        if (recognized_count)
        {
            //if (NULL != comment_call) (*comment_call)();
            continue;
        }
        
        /* recognize label */
        recognized_count = iniparser_recognize_label(
            line, label_name, INIPARSER_LENGTH_LABEL_NAME
        );
        if (recognized_count)
        {
            if (NULL != label_call)
            {
                (*label_call)(label_name);
            }
            continue;
        }
        
        /* recognize property */
        recognized_count = iniparser_recognize_property(
            line, 
            property_name, INIPARSER_LENGTH_PROPERTY_NAME, 
            property_value, INIPARSER_LENGTH_PROPERTY_VALUE
        );
        if (recognized_count)
        {
            if (NULL != property_call)
            {
                (*property_call)(property_name, property_value);
            }
            continue;
        }
    }
    fclose(file_handle);
    return MQX_OK;
}


uint32_t iniparser_recognize_comment
(
    /* [IN] parsed line */ 
    const char *line
)
{
    uint32_t i = 0;
    
    /* check leading comment */
    if (!(INIPARSER_CHAR_IS_COMMENT(line[i]))) return 0;
    
    /* skip rest of line */
    for (i = 1; !(INIPARSER_CHAR_IS_EOS(line[i])); i++);
    return i;
}


uint32_t iniparser_recognize_label
(
    /* [IN] parsed line */ 
    const char *line,
    /* [OUT] label name */ 
    char *label,
    /* [IN] label length*/
    uint32_t label_length
)
{
    uint32_t i = 0, j = 0;
    
    /* check leading '[' */
    if (line[i] != '[') return 0;
    
    /* compare label name unil 'label' reach EOS */
    for (i = 1, j = 0; line[i] != ']'; i++, j++)
    {
        /* end of parsed line */
        if (INIPARSER_CHAR_IS_EOS(line[i])) return 0;
        
        /* 'value' length too small to fit parsed value */
        if ((j + 1) >= label_length) return 0;
        
        /* copy char */
        label[j] = line[i];
    }
    
    /* terminate label */
    label[j] = '\0';
    
    /* check trailing ']' */
    if (line[i] != ']') return 0;
    
    /* all passed, update readed number */
    return (i + 1);
}


// return number of recognized chars
uint32_t iniparser_recognize_property
(
    /* [IN] parsed line */
    const char *line,
    /* [OUT] parsed line */
    char *name,
    /* [IN] value length */
    uint32_t name_length,
    /* [OUT] value length */
    char *value,
    /* [IN] value length */
    uint32_t value_length
)
{
    uint32_t i = 0, j = 0;
    
    /* take property name */
    for (i = 0, j = 0; line[i] != '='; i++, j++)
    {
        /* line ends */
        if (INIPARSER_CHAR_IS_EOS(line[i])) return 0;
        /* not enought space to store name */
        if ((j + 1) >= name_length) return 0;
        /* copy char */
        name[j] = line[i];
    }
    
    /* terminate name */
    name[j] = '\0';
    i += 1;
    
    /* take value (rest of the line after property) */
    for (j = 0; ((j + 1) < value_length); i++, j++)
    {
        /* value end by EOS or EOL, break loop */
        if (INIPARSER_CHAR_IS_EOS(line[i]) || INIPARSER_CHAR_IS_EOL(line[i])) break;
        /* copy char */
        value[j] = line[i];
    }
    
    /* terminate value */
    value[j] = '\0';
    return i;
}
