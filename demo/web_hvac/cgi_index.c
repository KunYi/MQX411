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
#if DEMOCFG_ENABLE_WEBSERVER
#include "cgi.h"

#include <string.h>
#include <stdlib.h>


extern LWSEM_STRUCT      USB_Stick;

static _mqx_int usb_status_fn(HTTPSRV_SSI_PARAM_STRUCT* param);

const HTTPSRV_SSI_LINK_STRUCT fn_lnk_tbl[] = { { "usb_status_fn", usb_status_fn }, { 0, 0 } };

const HTTPSRV_CGI_LINK_STRUCT cgi_lnk_tbl[] = {
    { "hvacdata",       cgi_hvac_data, 0},
    { "rtcdata",        cgi_rtc_data, 0},
    { "hvacoutput",     cgi_hvac_output, 0},
    { "usbstat", 	cgi_usbstat, 0},
    { 0, 0 }    // DO NOT REMOVE - last item - end of table
};

static bool usbstick_attached()
{
#if DEMOCFG_ENABLE_USB_FILESYSTEM
    if (_lwsem_poll(&USB_Stick))
    {
        _lwsem_post(&USB_Stick);
        return TRUE;
    } 
    else
#endif      
        return FALSE;
}

static _mqx_int usb_status_fn(HTTPSRV_SSI_PARAM_STRUCT* param)
{
    char* str;
    
    if (usbstick_attached())
    {
        str = "visible";
    } 
    else
    {
        str = "hidden";
    }
    HTTPSRV_ssi_write(param->ses_handle, str, strlen(str));
    return 0;
}

static _mqx_int cgi_usbstat(HTTPSRV_CGI_REQ_STRUCT* param)
{
    HTTPSRV_CGI_RES_STRUCT response;
    
    if (param->request_method != HTTPSRV_REQ_GET)
    {
        return(0);
    }
    
    response.ses_handle = param->ses_handle;
    response.content_type = HTTPSRV_CONTENT_TYPE_PLAIN;
    response.status_code = 200;
    
    if (usbstick_attached())
    {
        response.data = "visible\n";
    }
    else
    {
        response.data = "hidden\n";
    }
    response.data_length = strlen(response.data);
    response.content_length = response.data_length;
    HTTPSRV_cgi_write(&response);
    return (response.content_length);
}

static _mqx_int cgi_rtc_data(HTTPSRV_CGI_REQ_STRUCT* param)
{
    #define BUFF_SIZE sizeof("00\n00\n00\n")
    HTTPSRV_CGI_RES_STRUCT response;
    TIME_STRUCT time;
    uint32_t min;
    uint32_t hour;
    uint32_t sec;
    
    char str[BUFF_SIZE];
    uint32_t length = 0;
    
    if (param->request_method != HTTPSRV_REQ_GET)
    {
        return(0);
    }

    _time_get(&time);
    
    sec = time.SECONDS % 60;
    min = time.SECONDS / 60;
    hour = min / 60;
    min %= 60;

    response.ses_handle = param->ses_handle;
    response.content_type = HTTPSRV_CONTENT_TYPE_PLAIN;
    response.status_code = 200;
    /* 
    ** When the keep-alive is used we have to calculate a correct content length
    ** so the receiver knows when to ACK the data and continue with a next request.
    ** Please see RFC2616 section 4.4 for further details.
    */
    
    /* Calculate content length while saving it to buffer */                                  
    length = snprintf(str, BUFF_SIZE, "%ld\n%ld\n%ld\n", hour, min, sec);          
    response.data = str;
    response.data_length = length;
    response.content_length = response.data_length;
    /* Send response */
    HTTPSRV_cgi_write(&response);
    return (response.content_length);
}

#endif
