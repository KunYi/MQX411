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
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   HTTPSRV support functions header.
*
*
*END************************************************************************/

#ifndef HTTP_SUPP_H_
#define HTTP_SUPP_H_

#define ERR_PAGE_FORMAT "<HTML><HEAD><TITLE>%s</TITLE></HEAD>\n<BODY><H1>%s</H1>\n</BODY></HTML>\n"
#include "httpsrv_prv.h"
#include "httpsrv.h"

_mqx_int httpsrv_read(HTTPSRV_SESSION_STRUCT *session, char *dst, _mqx_int len);
_mqx_int httpsrv_write(HTTPSRV_SESSION_STRUCT *session, char *src, _mqx_int len);
uint32_t httpsrv_ses_flush(HTTPSRV_SESSION_STRUCT *session);

void httpsrv_sendhdr(HTTPSRV_SESSION_STRUCT *session, _mqx_int content_len, bool has_entity);
void httpsrv_sendfile(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session);
void httpsrv_send_err_page(HTTPSRV_SESSION_STRUCT *session, const char* title, const char* text);
void httpsrv_process_req_hdr_line(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT* session, char* buffer);
void httpsrv_process_req_method(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session, char* buffer);
void httpsrv_process_cgi(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session, char* cgi_name);
void httpsrv_call_ssi(HTTPSRV_SSI_CALLBACK_FN function, HTTPSRV_SCRIPT_MSG* msg_ptr);
void httpsrv_call_cgi(HTTPSRV_CGI_CALLBACK_FN function, HTTPSRV_SCRIPT_MSG* msg_ptr);
HTTPSRV_FN_CALLBACK httpsrv_find_callback(HTTPSRV_FN_LINK_STRUCT* table, char* name, uint32_t* stack);
HTTPSRV_AUTH_REALM_STRUCT* httpsrv_req_realm(HTTPSRV_STRUCT *server, char* path);
int httpsrv_check_auth(HTTPSRV_AUTH_REALM_STRUCT* realm, HTTPSRV_AUTH_USER_STRUCT* user);
uint32_t httpsrv_init_socket (HTTPSRV_STRUCT *server, uint16_t family);
uint32_t httpsrv_set_params (HTTPSRV_STRUCT *server, HTTPSRV_PARAM_STRUCT* params);
uint32_t httpsrv_check_request(HTTPSRV_SESSION_STRUCT *session);
char* httpsrv_unalias(HTTPSRV_ALIAS* table, char* path, char** new_root);
char* httpsrv_get_full_path(char* path, char* root);
void httpsrv_detach_script(HTTPSRV_DET_TASK_PARAM* task_params);
uint32_t httpsrv_wait_for_cgi(HTTPSRV_SESSION_STRUCT *session);

#endif /* HTTP_SUPP_H_ */
