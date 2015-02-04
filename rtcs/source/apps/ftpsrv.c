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
*   This file contains an implementation of FTP server.
*
*
*END************************************************************************/

#include <rtcs.h>
#include <string.h>

#if MQX_USE_IO_OLD

#include "ftpsrv_prv.h"
/*
** Function for starting the HTTP server 
**
** IN:
**      FTPSRV_PARAM_STRUCT*   params - server parameters (port, ip, root directory etc.)
**
** OUT:
**      none
**
** Return Value: 
**      uint32_t      server handle if successful, NULL otherwise
*/
uint32_t FTPSRV_init(FTPSRV_PARAM_STRUCT *params)
{
    FTPSRV_STRUCT *server = NULL;
    uint32_t error;
    uint32_t error4 = FTPSRV_OK;
    uint32_t error6 = FTPSRV_OK;

    if ((server = RTCS_mem_alloc_zero(sizeof(FTPSRV_STRUCT))) == NULL)
    {
        return((uint32_t) NULL);
    }
    _mem_set_type(server, MEM_TYPE_FTPSRV_STRUCT);

    error = _lwsem_create(&server->tid_sem, 1);
    if (error != MQX_OK)
    {
        FTPSRV_release((uint32_t) server);
        return((uint32_t) NULL);
    }

    error = ftpsrv_set_params(server, params);
    if (error != FTPSRV_OK)
    {
        FTPSRV_release((uint32_t) server);
        return((uint32_t) NULL);
    }
    
    /* Allocate space for session pointers */
    server->session = RTCS_mem_alloc_zero(sizeof(FTPSRV_SESSION_STRUCT*) * server->params.max_ses);
    if (server->session == NULL)
    {
        FTPSRV_release((uint32_t) server);
        return((uint32_t) NULL);
    }

    /* Allocate space for session task IDs */
    server->ses_tid = RTCS_mem_alloc_zero(sizeof(_rtcs_taskid) * server->params.max_ses);
    if (server->ses_tid == NULL)
    {
        FTPSRV_release((uint32_t) server);
        return((uint32_t) NULL);
    }

    /* Init sockets. */
    if (params->af & AF_INET)
    {
        /* Setup IPv4 server socket */
        error4 = ftpsrv_init_socket(server, AF_INET);
    }
    if (params->af & AF_INET6)
    {
        /* Setup IPv6 server socket */
        error6 = ftpsrv_init_socket(server, AF_INET6);
    }

    if ((error4 != FTPSRV_OK) || (error6 != FTPSRV_OK))
    {
        FTPSRV_release((uint32_t) server);
        return((uint32_t) NULL);
    }
    
    server->run = 1;
    /* Run server */
    if (RTCS_task_create("ftpsrv server", server->params.server_prio, FTPSRV_SERVER_STACK_SIZE, ftpsrv_server_task, server) != RTCS_OK)
    {
        FTPSRV_release((uint32_t) server);
        return((uint32_t) NULL);
    }

    return((uint32_t) server);
}

/*
** Function for releasing/stopping FTP server 
**
** IN:
**      uint32_t       server_h - server handle
**
** OUT:
**      none
**
** Return Value: 
**      uint32_t      error code. FTPSRV_OK if everything went right, positive number otherwise
*/
uint32_t FTPSRV_release(uint32_t server_h)
{
    uint32_t i;
    uint32_t n = 0;
    FTPSRV_STRUCT* server = (void*) server_h;
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
            server->session[n]->connected = FALSE;
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
    
    /* Shutdown server task */
    server->run = 0;
    
    while(server->server_tid)
    {
        _sched_yield();
    }

    _lwsem_destroy(&server->tid_sem);
    
    /* Free memory */
    _mem_free(server->ses_tid);
    server->ses_tid = NULL;
    _mem_free(server->session);
    server->session = NULL;
    _mem_free(server);
    return(RTCS_OK);
}

#endif // MQX_USE_IO_OLD
