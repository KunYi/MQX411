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
*   HTTPSRV tasks and session processing.
*
*
*END************************************************************************/

#include "httpsrv.h"
#include "httpsrv_prv.h"
#include "httpsrv_supp.h"
#include "socket.h"
#include "message.h"
#if !MQX_USE_IO_OLD
#include <strings.h>
#endif

static int httpsrv_readreq(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session);
static void httpsrv_ses_process(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session);
static void httpsrv_processreq(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session);
static void httpsrv_response(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session);

static HTTPSRV_SESSION_STRUCT* httpsrv_ses_alloc(HTTPSRV_STRUCT *server);
static void httpsrv_ses_free(HTTPSRV_SESSION_STRUCT *session);
static void httpsrv_ses_close(HTTPSRV_SESSION_STRUCT *session);
static void httpsrv_ses_init(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session, const int sock);

static void httpsrv_session_task(void *init_ptr, void *creator);

/*
** HTTPSRV main task which creates new task for each new client request
*/
void httpsrv_server_task(void *init_ptr, void *creator) 
{
    HTTPSRV_STRUCT   *server = (HTTPSRV_STRUCT*)init_ptr;
    _mqx_uint res;
    
    if (server == NULL)
    {
        goto ERROR;
    }
    
    server->server_tid = _task_get_id();

    res = _lwsem_create(&server->ses_cnt, server->params.max_ses);  
    if (res != MQX_OK)
    {
        goto ERROR;
    }
    
    RTCS_task_resume_creator(creator, RTCS_OK);
    
    while (server->run) 
    {
        uint32_t          connsock = 0;
        uint32_t          new_sock;

        /* limit number of opened sessions */
        _lwsem_wait(&server->ses_cnt);

        /* Get socket with incoming connection (IPv4 or IPv6) */
        while (!connsock && server->run)
        {
            connsock = RTCS_selectset(&(server->sock_v4), 2, 250);
        }
        
        if (server->run)
        {
            struct sockaddr  remote_addr;
            unsigned short   length;

            new_sock = accept(connsock, (sockaddr *) &remote_addr, &length);
            connsock = 0;
        }
        else
        {
            break;
        }

        if (new_sock != RTCS_SOCKET_ERROR)
        {
            HTTPSRV_SES_TASK_PARAM  ses_param;
            _mqx_uint               error;

            ses_param.server = server;
            ses_param.sock = new_sock;
            /* Try to create task for session */
            error = RTCS_task_create("httpsrv session", server->params.server_prio, HTTPSRV_SESSION_STACK_SIZE, httpsrv_session_task, &ses_param);
            if (MQX_OK != error)
            {
            	shutdown(new_sock, FLAG_ABORT_CONNECTION);
                _lwsem_post(&server->ses_cnt);
            }
        }
        else
        {
            _lwsem_post(&server->ses_cnt);
            /* We probably run out of sockets. Wait some time then try again to prevent session tasks resource starvation */
            _time_delay(150);
        }
    } 
ERROR:
    _lwsem_destroy(&server->ses_cnt);
    server->server_tid = 0;
    RTCS_task_resume_creator(creator, (uint32_t)RTCS_ERROR);
}

/*
** Session task.
** This task is responsible for session creation, processing and cleanup.
*/
static void httpsrv_session_task(void *init_ptr, void *creator) 
{
    HTTPSRV_STRUCT* server = ((HTTPSRV_SES_TASK_PARAM*) init_ptr)->server;
    uint32_t sock = ((HTTPSRV_SES_TASK_PARAM*) init_ptr)->sock;
    HTTPSRV_SESSION_STRUCT *session;
    uint32_t i;
    _task_id tid = _task_get_id();

    /* Find empty session */
    _lwsem_wait(&server->tid_sem);

    for (i = 0; i < server->params.max_ses; i++)
    {
        if (server->session[i] == NULL)
        {
            break;
        }
    }
    
    if (i == server->params.max_ses)
    {
        RTCS_task_resume_creator(creator, (uint32_t) RTCS_ERROR);
        _lwsem_post(&server->tid_sem);
        _lwsem_post(&server->ses_cnt);
        return;
    }
  
    /* Save task ID - used for indication of running task */
    server->ses_tid[i] = tid;
    /* Access to array done. Unblock other tasks. */
    _lwsem_post(&server->tid_sem);
    
    /* Allocate session */
    session = httpsrv_ses_alloc(server);
        
    if (session) 
    {
        server->session[i] = session;

        RTCS_task_resume_creator(creator, RTCS_OK);       
        httpsrv_ses_init(server, session, sock);
    
        /* Disable keep-alive for last session so we have at least one session free (not blocked by keep-alive timeout) */
        if (i == server->params.max_ses - 1)
        {
            session->keep_alive_enabled = FALSE;
        }

        while (session->valid) 
        {
            /* Run state machine for session */
            httpsrv_ses_process(server, session);
            _sched_yield();
        }
        /* If session is not closed close it */
        if (session->state != HTTPSRV_SES_CLOSE)
        {
            httpsrv_ses_close(session);
        }
        httpsrv_ses_free(session);
        server->session[i] = NULL;
    }
    else 
    {
        RTCS_task_resume_creator(creator, (uint32_t) RTCS_ERROR);
    }
    /* Cleanup and end task */
    _lwsem_post(&server->ses_cnt);
    /* Null tid => task is no longer running */
    _lwsem_wait(&server->tid_sem);
    server->ses_tid[i] = 0;
    _lwsem_post(&server->tid_sem);
}

/*
** Task for CGI/SSI handling.
*/
void httpsrv_script_task(void *param, void *creator)
{
    HTTPSRV_STRUCT* server = (HTTPSRV_STRUCT*) param;

    server->script_tid = _task_get_id();
    /* Create pool */
    server->script_msg_pool = _msgpool_create(sizeof(HTTPSRV_SCRIPT_MSG), server->params.max_ses, 0, server->params.max_ses);
    if (server->script_msg_pool == 0)
    {
        goto EXIT;
    }

    /* Open queue */
    server->script_msgq = _msgq_open(MSGQ_FREE_QUEUE, 0);
    if (server->script_msgq == MSGQ_NULL_QUEUE_ID)
    {
        goto EXIT;
    }

    RTCS_task_resume_creator(creator, RTCS_OK);

    /* Read messages */
    while(1)
    {
        HTTPSRV_SCRIPT_MSG*     msg_ptr;
        HTTPSRV_FN_CALLBACK     user_function;
        HTTPSRV_FN_LINK_STRUCT* table;
        uint32_t                stack_size;
        char*                   separator;

        msg_ptr = _msgq_receive(server->script_msgq, 0);
        if (msg_ptr == NULL)
        {
            break;
        }

        /* NULL name and session => exit task */
        if ((msg_ptr->name == NULL) && (msg_ptr->session == NULL))
        {
            _msg_free(msg_ptr);
            break;
        }

        /*
        * There are two options:
        * 1. User set stack size to 0 and script callback will be run from this task. 
        * 2. User set stack size > 0 and script callback will be run in separate task.
        */
        switch (msg_ptr->type)
        {
            case HTTPSRV_CGI_CALLBACK:
                table = (HTTPSRV_FN_LINK_STRUCT*) server->params.cgi_lnk_tbl;
                user_function = httpsrv_find_callback(table, msg_ptr->name, &stack_size);
                
                /* Option No.1a - Run User CGI function here. */
                if (user_function && (stack_size == 0))
                {
                    httpsrv_call_cgi((HTTPSRV_CGI_CALLBACK_FN) user_function, msg_ptr);
                    httpsrv_ses_flush(msg_ptr->session);
                }
                break;

            case HTTPSRV_SSI_CALLBACK:
                table = (HTTPSRV_FN_LINK_STRUCT*) server->params.ssi_lnk_tbl;

                /* Set separator to null character temporarily. */
                separator = strchr(msg_ptr->name, ':');
                if (separator != NULL)
                {
                    *separator = '\0';
                }

                user_function = httpsrv_find_callback(table, msg_ptr->name, &stack_size);

                if (separator != NULL)
                {
                    *separator = ':';
                }

                /* Option No.1b - Run User SSI function here. */
                if ((user_function != NULL) && (stack_size == 0))
                {
                    httpsrv_call_ssi((HTTPSRV_SSI_CALLBACK_FN) user_function, msg_ptr);
                    httpsrv_ses_flush(msg_ptr->session);
                }
                break;

            default:
                break;
        }

        /* Option No.2 Run script in detached task. */
        if (user_function && (stack_size > 0))
        {
            HTTPSRV_DET_TASK_PARAM task_params;

            task_params.session = msg_ptr->session;
            task_params.user_function = user_function;
            task_params.stack_size = stack_size;
            task_params.type = msg_ptr->type;
            task_params.script_name = msg_ptr->name;
            httpsrv_detach_script(&task_params);
        }
        _task_ready(_task_get_td(msg_ptr->ses_tid));
        _msg_free(msg_ptr);
    }

EXIT:
    if (server->script_msgq != MSGQ_NULL_QUEUE_ID)
    {
       _msgq_close(server->script_msgq);
       server->script_msgq = 0;
    }
    
    if (server->script_msg_pool)
    {
        _msgpool_destroy(server->script_msg_pool);
        server->script_msg_pool = 0;
    }
    server->script_tid = 0;
}

/*
** Run user script separately with independent stack. Each session can create
** such task. This makes parallel script processing possible. If session
** timeout occurs durring script processing, this task will be destroyed.
*/
void httpsrv_detached_task(void *param, void *creator)
{
    HTTPSRV_SESSION_STRUCT* session;
    HTTPSRV_FN_CALLBACK     user_function;
    HTTPSRV_DET_TASK_PARAM* task_params = (HTTPSRV_DET_TASK_PARAM*) param;
    HTTPSRV_CALLBACK_TYPE   type;
    _mqx_uint               error;
    HTTPSRV_SCRIPT_MSG      call_param;

    session = task_params->session;
    session->script_tid = _task_get_id();
    user_function = task_params->user_function;
    type = task_params->type;
    call_param.session = task_params->session;
    call_param.name = task_params->script_name;

    /* Lock session, so socket is accessible as long as required by user function. */
    error = _lwsem_wait(&session->lock);
    if (error != MQX_OK)
    {
        RTCS_task_resume_creator(creator, error);
    }
    RTCS_task_resume_creator(creator, MQX_OK);

    switch (type)
    {
        case HTTPSRV_CGI_CALLBACK:
            httpsrv_call_cgi((HTTPSRV_CGI_CALLBACK_FN) user_function, &call_param);
            httpsrv_ses_flush(session);
            break;

        case HTTPSRV_SSI_CALLBACK:
            httpsrv_call_ssi((HTTPSRV_SSI_CALLBACK_FN) user_function, &call_param);
            httpsrv_ses_flush(session);
            break;

        default:
            break;
    }

    session->script_tid = 0;
    /* Callback returned, we can unlock the session. */
    _lwsem_post(&session->lock);
}

/*
** HTTP session state machine
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer.
**      HTTPSRV_STRUCT *server - pointer to server structure (needed for session parameters).
**
** OUT:
**      none
**
** Return Value: 
**      none
*/
static void httpsrv_ses_process(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session)
{
    uint32_t time_interval;
    int     result;

    time_interval = 0;  
    if (!session->valid)
    {
        session->state = HTTPSRV_SES_CLOSE;
        return;
    }
    else
    {
        uint32_t time_now;
        /* check session timeout */
        time_now = RTCS_time_get();
        time_interval = RTCS_timer_get_interval(session->time, time_now);
        if (time_interval > session->timeout)
        {
            if (session->script_tid != 0)
            {
                _task_destroy(session->script_tid);
                session->script_tid = 0;
            }
            session->state = HTTPSRV_SES_CLOSE;
        }
    }
        
    switch (session->state)
    {
        case HTTPSRV_SES_WAIT_REQ:
            result = httpsrv_readreq(server, session);
            
            if ((result < 0) && (!session->keep_alive) && (HTTPSRV_SES_CLOSE != session->state))
            {
                session->state = HTTPSRV_SES_END_REQ;
            }
            else if ((result == 0) && (session->request.process_header == 0))
            {
                session->state = HTTPSRV_SES_PROCESS_REQ;
            }
            break;
        case HTTPSRV_SES_PROCESS_REQ:
            httpsrv_processreq(server, session);
            break;

        case HTTPSRV_SES_RESP:
            httpsrv_response(server, session);
            session->time = RTCS_time_get();
            break;

        case HTTPSRV_SES_END_REQ:
            if (!session->keep_alive)
            {
                session->state = HTTPSRV_SES_CLOSE;
            }
            else
            {
                httpsrv_wait_for_cgi(session);
                /* Re-init session */
                session->state = HTTPSRV_SES_WAIT_REQ;
                if (session->response.file)
                {
                    fclose(session->response.file);
                }
                _mem_zero(&session->response, sizeof(session->response));
                session->request.process_header = 1;
                if (session->request.auth.user_id != NULL)
                {
                    _mem_free(session->request.auth.user_id);
                }
                session->request.auth.user_id = NULL;
                session->request.auth.password = NULL;
                session->time = RTCS_time_get();
                session->timeout = HTTPSRVCFG_KEEPALIVE_TO;
                session->keep_alive = 1;
            }
            break;
        case HTTPSRV_SES_CLOSE:
            /*
             * If session did not time out; wait for lock. If waiting
             * for lock timed-out, check session timeout again.
             */
            if (time_interval < session->timeout)
            {
                if (httpsrv_wait_for_cgi(session) == MQX_LWSEM_WAIT_TIMEOUT)
                {
                    break;
                }
            }
            httpsrv_ses_close(session);
            session->valid = FALSE;
            break;
        default:
            /* invalid state */
            session->valid = FALSE;
            break;
    }
}

/*
** Function for request parsing
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer.
**      HTTPSRV_STRUCT *server - pointer to server structure (needed for session parameters).
**
** OUT:
**      none
**
** Return Value: 
**      int - zero if request is valid, negative value if invalid.
*/
static int httpsrv_readreq(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session)
{
    char* line_start = session->buffer.data;
    char *query_str;
    int read;
    char* line_end = NULL;
    uint32_t unprocessed_size = 0;
    uint32_t req_lines = 0;
    
    /* Begin */
    while (session->request.process_header)
    {
        /* Read data */
        read = recv(session->sock, session->buffer.data+session->buffer.offset, HTTPSRV_SES_BUF_SIZE_PRV-session->buffer.offset, 0);
        if ((read == RTCS_ERROR) || (read == 0))
        {
            uint32_t error;
            
            error = RTCS_geterror(session->sock);
            /* Client closed connection so we close it too */
            if (error == RTCSERR_TCP_CONN_CLOSING)
            {
                session->keep_alive = 0;
                session->state = HTTPSRV_SES_CLOSE;
            }
            /*
             * If reading timed out, set number of read bytes to 0
             * to prevent session closing.
             */
            else if (error == RTCSERR_TCP_TIMED_OUT)
            {
               read = 0;
            }
            break;
        }
        unprocessed_size += read;

        /* Process buffer line by line. End of line is \n or \r\n */
        while (session->request.process_header && ((line_end = strchr(line_start, '\n')) != NULL))
        {
            /* Null terminate the line */
            *line_end = '\0';
            if ((line_end != session->buffer.data) && (*(line_end-1) == '\r'))
            {
                *(line_end-1) = '\0';
            }
            req_lines++;
            /* Substract line length from size of unprocessed data */
            unprocessed_size -= (line_end-line_start+1);

            /* Found an empty line => end of header */
            if (strlen(line_start) == 0)
            {
                session->request.process_header = FALSE;
            }
            else
            {
                if (req_lines == 1) 
                {
                    httpsrv_process_req_method(server, session, line_start);
                }
                else
                {
                    httpsrv_process_req_hdr_line(server, session, line_start);
                }
            }
            /* Set start of next line after end of current line */
            line_start = line_end+1;
            /* Check buffer boundary */
            if (line_start > (session->buffer.data+HTTPSRV_SES_BUF_SIZE_PRV))
            {
                line_start = session->buffer.data+HTTPSRV_SES_BUF_SIZE_PRV;
            }
        }
        /* There were no valid lines in buffer */
        if ((unprocessed_size >= HTTPSRV_SES_BUF_SIZE_PRV) && (req_lines == 0))
        {
            session->buffer.offset = 0;
            unprocessed_size = 0;
        }
        /* We are not at the end of buffer and there are some data after the end of last line. */
        else if (unprocessed_size != 0)
        {
            /* Copy rest of data to begining of buffer and save offset for next reading. */
            memmove(session->buffer.data, line_start, unprocessed_size);
            line_start = session->buffer.data;
            session->buffer.offset = unprocessed_size; 
        }
        /* Clear the buffer so we don't have some old data there. */
        _mem_zero(session->buffer.data+session->buffer.offset, HTTPSRV_SES_BUF_SIZE_PRV-session->buffer.offset);
    }
    /* End */

    /* Save query string */
    query_str = strchr(session->request.path, '?');
    if (NULL != query_str)
    {
        *query_str = '\0';
        query_str++;
        session->request.query = query_str;
    }

    return (0 > read) ? read : 0;
}

/*
** Function for request processing
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer.
**      HTTPSRV_STRUCT *server - pointer to server structure (needed for session parameters).
**
** OUT:
**      none
**
** Return Value: 
**      none
*/

static void httpsrv_processreq(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session)
{
    char *suffix;
    char* root_dir = NULL;
    char* full_path;
    char* new_path;

    session->state = HTTPSRV_SES_RESP;

    /* Check request validity */
    if (httpsrv_check_request(session) != HTTPSRV_OK)
    {
        return;
    }

    /* Check authentication */
    session->response.auth_realm = httpsrv_req_realm(server, session->request.path);
    if (session->response.auth_realm != NULL)
    {
        if (!httpsrv_check_auth(session->response.auth_realm, &session->request.auth))
        {
            session->response.status_code = HTTPSRV_CODE_UNAUTHORIZED;
            if (session->request.auth.user_id != NULL)
            {
                _mem_free(session->request.auth.user_id);
                session->request.auth.user_id = NULL;
                session->request.auth.password = NULL;
            }
            return;
        }
    }
    
    /* Get rid of alias if there is any */
    new_path = httpsrv_unalias(server->params.alias_tbl, session->request.path, &root_dir);
    if (new_path != session->request.path)
    {
        memmove(session->request.path, new_path, strlen(new_path)+1);
    }

    if (root_dir == NULL)
    {
        root_dir = server->params.root_dir;
    }

    /* Check if requested file is CGI script */
    if ((suffix = strrchr(session->request.path, '.')) != 0)
    {
        if ((0 == strcasecmp(suffix, ".cgi")) && server->params.script_stack)
        {
            *suffix = '\0';
            httpsrv_process_cgi(server, session, session->request.path+1); /* +1 bacause of slash */
            *suffix = '.';

            /* if cgi call was successful skip response, end session */
            if (session->response.status_code == HTTPSRV_CODE_OK)
            {
                session->state = HTTPSRV_SES_END_REQ;
            }
            return;
        }
    }

    /* If client requested root, set reqested path to index page */
    if (session->request.path[0] == '/' && session->request.path[1] == '\0')
    {
        uint32_t offset;
        uint32_t length;
        char*    index = server->params.index_page;
        uint32_t max_length = server->params.max_uri;

        length = strlen(index);
        if (length > max_length)
        {
            length = max_length;
        }

        offset = ((index[0] == '\\') || (index[0] == '/')) ? 1 : 0;
        _mem_copy(index+offset, session->request.path+1, length);
    }

    /* Get full file path */
    full_path = httpsrv_get_full_path(session->request.path, root_dir);
    if (full_path == NULL)
    {
        session->response.status_code = HTTPSRV_CODE_INTERNAL_ERROR;
        return;
    }

    session->response.file = fopen(full_path, "r");
    session->response.len = 0;
    if (!session->response.file)
    {
        session->response.status_code = HTTPSRV_CODE_NOT_FOUND;
    }
    _mem_free(full_path);
}

/*
** Function for HTTP sending response
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer.
**      HTTPSRV_STRUCT *server - pointer to server structure (needed for session parameters).
**
** OUT:
**      none
**
** Return Value: 
**      none
*/

static void httpsrv_response(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session)
{   
    switch (session->response.status_code)
    {
        case HTTPSRV_CODE_OK:
            httpsrv_sendfile(server, session);
            break;
        case HTTPSRV_CODE_UNAUTHORIZED:
            session->state = HTTPSRV_SES_END_REQ;
            httpsrv_send_err_page(session, "401 Unauthorized", "Unauthorized!");
            break;
        case HTTPSRV_CODE_FORBIDDEN:
            session->state = HTTPSRV_SES_END_REQ;
            httpsrv_send_err_page(session, "403 Forbidden", "Forbidden!");
            break;
        case HTTPSRV_CODE_URI_TOO_LONG:
            session->keep_alive = 0;
            session->state = HTTPSRV_SES_END_REQ;
            httpsrv_send_err_page(session, "414 Uri too long", "Requested URI too long!");
            break;
        case HTTPSRV_CODE_BAD_REQ:
        case HTTPSRV_CODE_NO_LENGTH:
        case HTTPSRV_CODE_INTERNAL_ERROR:
        case HTTPSRV_CODE_NOT_IMPLEMENTED:
            session->keep_alive = 0;
            session->state = HTTPSRV_SES_END_REQ;
            httpsrv_sendhdr(session, 0, 0);
            break;
        default:
            session->state = HTTPSRV_SES_END_REQ;
            httpsrv_send_err_page(session, "404 Not Found", "Requested URL not found!");
            break;
    }
}

/*
** Function for session allocation
**
** IN:
**      HTTPSRV_STRUCT *server - pointer to server structure (needed for session parameters).
**
** OUT:
**      none
**
** Return Value: 
**      HTTPSRV_SESSION_STRUCT* - pointer to allocated session. Non-zero if allocation was OK, NULL otherwise
*/
static HTTPSRV_SESSION_STRUCT* httpsrv_ses_alloc(HTTPSRV_STRUCT *server)
{
    HTTPSRV_SESSION_STRUCT *session = NULL;

    if (server)
    {
        session = _mem_alloc_zero(sizeof(HTTPSRV_SESSION_STRUCT));
        if (session)
        {
            _mem_set_type(session, MEM_TYPE_HTTPSRV_SESSION_STRUCT);
            /* Alloc URI */
            session->request.path = _mem_alloc_zero(server->params.max_uri + 1);
            if (NULL == session->request.path)
            {
                goto ERROR;
            } 
            _mem_set_type(session->request.path, MEM_TYPE_HTTPSRV_URI);
            /* Alloc session buffer */
            session->buffer.data = _mem_alloc_zero(sizeof(char)*HTTPSRV_SES_BUF_SIZE_PRV);
            if (NULL == session->buffer.data)
            {
                goto ERROR;
            }
        }
    }

    return session;

ERROR:
    if (session->request.path)
    {
        _mem_free(session->request.path);
        session->request.path = NULL;
    }
    if (session->buffer.data)
    {
        _mem_free(session->buffer.data);
        session->buffer.data = NULL;
    }
    _mem_free(session);
    return NULL;
}

/*
** Function used to free session structure
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer
**
** OUT:
**      none
**
** Return Value: 
**      none
*/
static void httpsrv_ses_free(HTTPSRV_SESSION_STRUCT *session)
{
    if (session)
    {
        if (session->request.path)
        {
            _mem_free(session->request.path);
        }
        if(session->request.auth.user_id)
        {
            _mem_free(session->request.auth.user_id);
        }
        if(session->buffer.data)
        {
            _mem_free(session->buffer.data);
        }
        _mem_zero(session, sizeof(session));
        _mem_free(session);
    }
}

/*
** Function used to init session structure
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer
**      HTTPSRV_STRUCT *server - pointer to server structure (needed for session parameters)
**      const int sock - socket handle used for communication with client
**
** OUT:
**      none
**
** Return Value: 
**      none
*/
static void httpsrv_ses_init(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session, const int sock)
{
    if (server && session)
    {
        /* init session */
        session->state = HTTPSRV_SES_WAIT_REQ;
        session->sock = sock;
        session->valid = TRUE;
        session->request.process_header = 1;
        session->timeout = HTTPSRVCFG_SES_TO;
        session->keep_alive_enabled = HTTPSRVCFG_KEEPALIVE_ENABLED;
        session->keep_alive = session->keep_alive_enabled;
        session->time = RTCS_time_get();
        _lwsem_create(&session->lock, 1);
    }
}

/*
** Function used to close session
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer
**
** OUT:
**      none
**
** Return Value: 
**      none
*/
static void httpsrv_ses_close(HTTPSRV_SESSION_STRUCT *session)
{
    if (session != NULL)
    {
        if (session->response.file)
        {
            fclose(session->response.file);
            session->response.file = NULL;
        }
        shutdown(session->sock, FLAG_CLOSE_TX);
        _lwsem_destroy(&session->lock);
    }
}
/* EOF */
