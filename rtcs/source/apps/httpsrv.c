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
*   This file contains the HTTPSRV implementation.
*
*
*END************************************************************************/

#include "httpsrv.h"
#include "httpsrv_supp.h"
#include "httpsrv_prv.h"
#include <string.h>
#include <stdlib.h>

/*
** Function for starting the HTTP server 
**
** IN:
**      HTTPSRV_PARAM_STRUCT*   params - server parameters (port, ip, index page etc.)
**
** OUT:
**      none
**
** Return Value: 
**      uint32_t      server handle if successful, NULL otherwise
*/
uint32_t HTTPSRV_init(HTTPSRV_PARAM_STRUCT *params)
{
    HTTPSRV_STRUCT *server = NULL;
    uint32_t error;
    uint32_t error4 = HTTPSRV_OK;
    uint32_t error6 = HTTPSRV_OK;

    
    if ((server = _mem_alloc_zero(sizeof(HTTPSRV_STRUCT))) == NULL)
    {
        return(0);
    }
    _mem_set_type(server, MEM_TYPE_HTTPSRV_STRUCT);

    error = _lwsem_create(&server->tid_sem, 1);
    if (error != MQX_OK)
    {
        HTTPSRV_release((uint32_t) server);
        return(0);
    }

    error = httpsrv_set_params(server, params);
    if (error != HTTPSRV_OK)
    {
        HTTPSRV_release((uint32_t) server);
        return(0);
    }
    
    /* Allocate space for session pointers */
    server->session = _mem_alloc_zero(sizeof(HTTPSRV_SESSION_STRUCT*) * server->params.max_ses);
    if (server->session == NULL)
    {
        HTTPSRV_release((uint32_t) server);
        return(0);
    }

    /* Allocate space for session task IDs */
    server->ses_tid = _mem_alloc_zero(sizeof(_rtcs_taskid) * server->params.max_ses);
    if (server->ses_tid == NULL)
    {
        HTTPSRV_release((uint32_t) server);
        return(0);
    }

    /* Init sockets. */
    if (params->af & AF_INET)
    {
        /* Setup IPv4 server socket */
        error4 = httpsrv_init_socket(server, AF_INET);
    }
    if (params->af & AF_INET6)
    {
        /* Setup IPv6 server socket */
        error6 = httpsrv_init_socket(server, AF_INET6);
    }

    if ((error4 != HTTPSRV_OK) || (error6 != HTTPSRV_OK))
    {
        HTTPSRV_release((uint32_t) server);
        return(0);
    }

    /* Create script handler if required */
    if ((server->params.cgi_lnk_tbl != NULL) || (server->params.ssi_lnk_tbl != NULL))
    {
        error = RTCS_task_create("httpsrv callback handler", server->params.script_prio, server->params.script_stack, httpsrv_script_task, server);
        if (error != RTCS_OK)
        {
            HTTPSRV_release((uint32_t) server);
            return((uint32_t) NULL);
        }
    }
    
    server->run = 1;
    /* Run server */
    if (RTCS_task_create("httpsrv server", server->params.server_prio, HTTPSRV_SERVER_STACK_SIZE, httpsrv_server_task, server) != RTCS_OK)
    {
        HTTPSRV_release((uint32_t) server);
        return((uint32_t) NULL);
    }

    return((uint32_t) server);
}

/*
** Function for releasing/stopping HTTP server 
**
** IN:
**      uint32_t       server_h - server handle
**
** OUT:
**      none
**
** Return Value: 
**      uint32_t      error code. HTTPSRV_OK if everything went right, positive number otherwise
*/
uint32_t HTTPSRV_release(uint32_t server_h)
{
    uint32_t n = 0;
    HTTPSRV_STRUCT* server = (void *) server_h;
    bool wait = FALSE;

    /* Shutdown server listen sockets, stop server task */
    if (server != NULL)
    {
        if (server->sock_v4)
        {
            shutdown(server->sock_v4, FLAG_ABORT_CONNECTION);
        }
            
        if (server->sock_v6)
        {
            shutdown(server->sock_v6, FLAG_ABORT_CONNECTION);
        }
    }
    else
    {
        return(RTCS_ERROR);
    }

    /* Invalidate sessions (this is signal for session tasks to end them) */
    while(n < server->params.max_ses)
    {
        if (server->session[n])
        {
            server->session[n]->valid = FALSE;
        }
        n++;
    }
    /* Wait until all session tasks end */
    do
    {
        wait = FALSE;
        for (n = 0; n < server->params.max_ses; n++)
        {
            if (server->ses_tid[n])
            {
                wait = TRUE;
            }
        }
        _sched_yield();
    }while(wait);

    /* Shutdown script handler if there is any */
    if (server->script_tid)
    {
        HTTPSRV_SCRIPT_MSG* msg_ptr;
        msg_ptr = _msg_alloc(server->script_msg_pool);
        if (msg_ptr != NULL)
        {
            msg_ptr->header.TARGET_QID = server->script_msgq;
            msg_ptr->header.SOURCE_QID = server->script_msgq;
            msg_ptr->header.SIZE = sizeof(HTTPSRV_SCRIPT_MSG);
            msg_ptr->session = NULL;
            msg_ptr->name = NULL;
            _msgq_send(msg_ptr);
        }
        else
        {
            return(RTCS_ERROR);
        }
    }
    while(server->script_tid)
    {
        _sched_yield();
    }
    
    /* Shutdown server task */
    server->run = 0;
    
    while(server->server_tid)
    {
        _sched_yield();
    }

    _lwsem_destroy(&server->tid_sem);
    
    /* Free memory */
    _mem_free((void*) server->ses_tid);
    server->ses_tid = NULL;
    _mem_free(server->session);
    server->session = NULL;
    _mem_free(server);
    return(RTCS_OK);
}

/*
** Write data to client from CGI script
**
** IN:
**      HTTPSRV_CGI_RES_STRUCT* response - CGI response structure used for forming response
**
** OUT:
**      none
**
** Return Value:
**      uint_32 - Number of bytes written
*/
uint32_t HTTPSRV_cgi_write(HTTPSRV_CGI_RES_STRUCT* response)
{
    HTTPSRV_SESSION_STRUCT* session = (HTTPSRV_SESSION_STRUCT*) response->ses_handle;
    uint32_t retval = 0;

    if (session == NULL)
    {
        return(0);
    }

    if (session->response.hdrsent == 0)
    {
        session->response.status_code = response->status_code;
        session->response.content_type = response->content_type;
        session->response.len = response->content_length;
        /* 
        ** If there is no content length we have to disable keep alive.
        ** Otherwise we would have to wait for session timeout.
        */
        if (session->response.len == 0)
        {
            session->keep_alive = 0;
        }
        httpsrv_sendhdr(session, response->content_length, 1);
    }
    if ((response->data != NULL) && (response->data_length))
    {
        retval = httpsrv_write(session, response->data, response->data_length);
    }
    session->time = RTCS_time_get();
    return(retval);
}

/*
** Read data from client to CGI script
**
** IN:
**      uint32_t ses_handle - handle to session used for reading
**      char*   buffer - user buffer to read data to
**      uint32_t length - size of buffer in bytes
**
** OUT:
**      none
**
** Return Value:
**      uint32_t - Number of bytes read
*/
uint32_t HTTPSRV_cgi_read(uint32_t ses_handle, char* buffer, uint32_t length)
{
    HTTPSRV_SESSION_STRUCT* session = (HTTPSRV_SESSION_STRUCT*) ses_handle;
    uint32_t retval;

    retval = httpsrv_read(session, buffer, length);

    if (retval > 0)
    {
        session->request.content_length -= retval;
    }
    session->time = RTCS_time_get();
    return(retval);
}

/*
** Write data to client from server side include
**
** IN:
**      uint32_t ses_handle - session foe writing
**      char*   data - user data to write
**      uint32_t length - size of data in bytes
**
** OUT:
**      none
**
** Return Value:
**      uint32_t - Number of bytes written
*/
uint32_t HTTPSRV_ssi_write(uint32_t ses_handle, char* data, uint32_t length)
{
    HTTPSRV_SESSION_STRUCT* session = (HTTPSRV_SESSION_STRUCT*) ses_handle;
    uint32_t retval = 0;

    if ((session != NULL) && (data != NULL) && (length))
    {
        retval = httpsrv_write(session, data, length);
    }
    
    return(retval);
}
/* EOF */
