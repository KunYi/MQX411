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
*   
*
*
*END************************************************************************/

#ifndef _cgi_h_
#define _cgi_h_


_mqx_int cgi_index(HTTPSRV_CGI_REQ_STRUCT* param);
_mqx_int cgi_hvac_data(HTTPSRV_CGI_REQ_STRUCT* param);
_mqx_int cgi_hvac_input(HTTPSRV_CGI_REQ_STRUCT* param);
_mqx_int cgi_hvac_output(HTTPSRV_CGI_REQ_STRUCT* param);
static _mqx_int cgi_usbstat(HTTPSRV_CGI_REQ_STRUCT* param);
static _mqx_int cgi_rtc_data(HTTPSRV_CGI_REQ_STRUCT* param);

#endif
