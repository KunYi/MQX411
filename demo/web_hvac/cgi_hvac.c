/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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
*   Example of shell using RTCS.
*
*
*END************************************************************************/

#include "hvac.h"
#include "hvac_public.h"

#if DEMOCFG_ENABLE_WEBSERVER
#include "httpsrv.h"
#include "cgi.h"

#include <string.h>

static int hvac_get_varval(char *var_str, char *var_name, char *var_val, _mqx_int var_val_len);

_mqx_int cgi_hvac_data(HTTPSRV_CGI_REQ_STRUCT* param)
{
    HTTPSRV_CGI_RES_STRUCT response;
    uint32_t Td = HVAC_GetDesiredTemperature();
    uint32_t Ta = HVAC_GetActualTemperature();
    uint32_t length = 0;
    uint32_t size = 0;
    char* str = NULL;
    
    if (param->request_method != HTTPSRV_REQ_GET)
    {
        return(0);
    }
    
    response.ses_handle = param->ses_handle;
    response.content_type = HTTPSRV_CONTENT_TYPE_PLAIN;
    response.status_code = 200;
    
    size = snprintf(NULL, 0, "%d.%d &deg;%c\n%d.%d &deg;%c\n%s\n%s\n%s\n%s\n%s\n",
                     Ta/10,
                     Ta%10,
                     HVAC_GetTemperatureSymbol(),
                     Td/10,
                     Td%10,
                     HVAC_GetTemperatureSymbol(), 
                     HVAC_GetFanMode() == Fan_Automatic ? "auto" : "on", 
                     HVAC_GetOutput(HVAC_FAN_OUTPUT) ? "on" : "off",
                     HVAC_GetOutput(HVAC_COOL_OUTPUT) ? "on" : "off",
                     HVAC_GetOutput(HVAC_HEAT_OUTPUT) ? "on" : "off",
                     HVAC_HVACModeName(HVAC_GetHVACMode()));
    size += 1;

    str = _mem_alloc(sizeof(char)*size);
    if (str != NULL)
    {
        length = snprintf(str, size, "%d.%d &deg;%c\n%d.%d &deg;%c\n%s\n%s\n%s\n%s\n%s\n",
                     Ta/10,
                     Ta%10,
                     HVAC_GetTemperatureSymbol(),
                     Td/10,
                     Td%10,
                     HVAC_GetTemperatureSymbol(), 
                     HVAC_GetFanMode() == Fan_Automatic ? "auto" : "on", 
                     HVAC_GetOutput(HVAC_FAN_OUTPUT) ? "on" : "off",
                     HVAC_GetOutput(HVAC_COOL_OUTPUT) ? "on" : "off",
                     HVAC_GetOutput(HVAC_HEAT_OUTPUT) ? "on" : "off",
                     HVAC_HVACModeName(HVAC_GetHVACMode()));

        response.data = str;
        response.data_length = length;
        response.content_length = response.data_length;
        
    }
    else
    {
        response.data_length = strlen("n/a\n")*11;
        response.data = "n/a\nn/a\nn/a\nn/a\nn/a\nn/a\nn/a\nn/a\nn/a\nn/a\nn/a\n";
        response.content_length = response.data_length;
    }
    HTTPSRV_cgi_write(&response);
    if (str)
    {
        _mem_free(str);
    }
    return (response.content_length);
}

_mqx_int cgi_hvac_output(HTTPSRV_CGI_REQ_STRUCT* param)
{
    
    uint32_t  len = 0;
    char     hvac[10];
    char     unit[10];
    char     fan[10];
    char     t[40];
    uint32_t  temp = 0;
    uint32_t  temp_fract = 0;
    bool  bParams = FALSE; 
    char     buffer[100];
    HTTPSRV_CGI_RES_STRUCT response;
    
    if (param->request_method != HTTPSRV_REQ_POST)
    {
        return(0);
    }
    
    len = param->content_length;
    len = HTTPSRV_cgi_read(param->ses_handle, buffer, (len > sizeof(buffer)) ? sizeof(buffer) : len);
    
    if (param->content_length)
    {
        buffer[len] = 0;
        param->content_length -= len;
        len = 0;
        
        if (hvac_get_varval(buffer, "hvac", hvac, sizeof(hvac)) &&
            hvac_get_varval(buffer, "unit", unit, sizeof(unit)) &&
            hvac_get_varval(buffer, "temp", t, sizeof(t)) &&
            hvac_get_varval(buffer, "fan", fan, sizeof(fan))) {
            
            bParams =  TRUE;
         
            if (strcmp(hvac,"heat") == 0)
            {
                HVAC_SetHVACMode(HVAC_Heat);
            }
            else if (strcmp(hvac,"cool") == 0)
            {
                HVAC_SetHVACMode(HVAC_Cool);
            }
            else
            {
                HVAC_SetHVACMode(HVAC_Off);
            }
            
            if (strcmp(unit,"f") == 0)
            {
                HVAC_SetTemperatureScale(Fahrenheit);
            }
            else
            {
                HVAC_SetTemperatureScale(Celsius);
            }
            if (strcmp(fan,"auto") == 0)
            {
                HVAC_SetFanMode(Fan_Automatic);
            }
            else
            {
                HVAC_SetFanMode(Fan_On);
            }
        
            if (sscanf(t, "%d.%d", &temp, &temp_fract) >= 1)
            {
                if (temp_fract<10)
                {
                    HVAC_SetDesiredTemperature(temp * 10 + temp_fract);
                }
            }
        }
    }
    response.ses_handle = param->ses_handle;
    response.content_type = HTTPSRV_CONTENT_TYPE_HTML;
    response.status_code = 200;
    response.data = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">"
        "<html><head><title>HVAC Settings response</title>"
        "<meta http-equiv=\"REFRESH\" content=\"0;url=hvac.shtml\"></head>\n<body>\n";
    response.data_length = strlen(response.data);
    response.content_length = 0;
    HTTPSRV_cgi_write(&response);
    
    if (!bParams)
    {
        response.data = "No parameters received.<br>\n";
        response.data_length = strlen(response.data);
        HTTPSRV_cgi_write(&response);
    }
    response.data = "<br><br>\n</body></html>";
    response.data_length = strlen(response.data);
    HTTPSRV_cgi_write(&response);    
    return (response.content_length);
}

static int hvac_get_varval(char *var_str, char *var_name, char *var_val, _mqx_int var_val_len)
{
    char *var = var_str;
    int res = 0;
    int idx;

    var_val[0] = 0;

    while ((var = strstr(var, var_name)) != 0)
    {
        if (*(var + strlen(var_name)) == '=')
        {
            var += strlen(var_name) + 1;    // +1 because '='

            idx = (int)strcspn(var, "&");
            strncpy(var_val, var, (unsigned long)idx);
            var_val[idx] = 0;
            res = 1;
            break;
        }
        else
        {
            var = strchr(var, '&');
        }
    }

    return res;
}

#endif
