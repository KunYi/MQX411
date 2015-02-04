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
*   This file contains FTP server subsidiary functions.
*
*
*END************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "ftpsrv_prv.h"
#include "ftpsrv_msg.h"

#if MQX_USE_IO_OLD

static uint32_t ftpsrv_accept_passive_conn(FTPSRV_SESSION_STRUCT* session);
static uint32_t ftpsrv_open_active_conn(FTPSRV_SESSION_STRUCT* session);
static void ftpsrv_normalize_path(char* path);

extern const FTPSRV_COMMAND_STRUCT ftpsrv_commands[];

void ftpsrv_convert_percents(char* path)
{
    char* src = path;
    char* dst = path;

    while(*src != '\0')
    {
        if ((*src == '%') && (isxdigit(*(src+1))) && (isxdigit(*(src+2))))
        {
            *src = *(src+1);
            *(src+1) = *(src+2);
            *(src+2) = '\0';
            *dst++ = strtol(src, NULL, 16);
            src += 3;
        }
        else
        {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

char* ftpsrv_strip_path_delimiters(char* arg_ptr)
{
   char*   ptr;
   uint32_t len;
    
   ptr = arg_ptr;
   len = strlen(ptr);

   if (len > 2)
   {
      if ((ptr[0] == '/') &&  (ptr[1] == '\\') && (ptr[2] == '/'))
      {
         ptr +=2;
         len -=2;
      }
   }

   if (len > 2)
   {
      if (ptr[len-1] == '/')
      {
         ptr[len-1] = '\0';   
      }
   }  
   
   return ptr;
}

uint32_t ftpsrv_max_cmd_length (void)
{
    const FTPSRV_COMMAND_STRUCT* cmd_ptr = ftpsrv_commands;
    uint32_t                     length = 0;
    uint32_t                     max = 0;

    while(cmd_ptr->command != NULL)
    {
        length = strlen(cmd_ptr->command);
        
        if (length > max)
        {
            max = length;
        }
        cmd_ptr++;
    }

    return(max);
}

char* ftpsrv_get_relative_path(FTPSRV_SESSION_STRUCT* session, char* abs_path)
{
    uint32_t root_length = strlen(session->root_dir);
    uint32_t new_length = strlen(abs_path) - root_length;
    char*    rel_path;

    rel_path = RTCS_mem_alloc_zero(new_length+1);
    
    if (rel_path != NULL)
    {
        _mem_copy(abs_path+root_length, rel_path, new_length);
    }
    
    return(rel_path);
}

void ftpsrv_get_prots_str(char** str, uint16_t* family)
{
    #if RTCSCFG_ENABLE_IP4 && RTCSCFG_ENABLE_IP6
    *str = "(1,2)";
    #elif RTCSCFG_ENABLE_IP4
    *str = "(1)";
    if (*family == 2)
    {
        *family = 99;
    }
    #elif RTCSCFG_ENABLE_IP6
    *str = "(2)";
    if (*family == 1)
    {
        *family = 99;
    }
    #endif
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name  : ftpsrv_get_full_path
* Returned Value : char* full path of file (root directory + server root + path from parameter)
* Comments       : This function is used for translating relative path to absolute path 
*                  which is required for file operations in MFS. Returned string is allocated dynamically.
*               
* Usage:
*       
*END*---------------------------------------------------------------------*/
char* ftpsrv_get_full_path(FTPSRV_SESSION_STRUCT* session, char* path, uint32_t *wrong_path)
{
    uint32_t path_length = strlen(path);
    uint32_t root_length = strlen(session->root_dir);
    uint32_t cur_dir_length;
    char*    new_path;
    uint32_t index = 0;

    /* 
    ** If first character in the path is backslash, then the path is absolute in FTP root
    ** and current directory is not to be added to full path.
    */
    cur_dir_length = (path[0] != '\\') ? strlen(session->cur_dir) : 0;

    new_path = RTCS_mem_alloc_zero(path_length+root_length+cur_dir_length+2); /* +1 because of backslash and +1 for terminator */
    if (new_path == NULL)
    {
        return(new_path);
    }

    memmove(new_path, session->root_dir, root_length);
    index += root_length;

    if (cur_dir_length)
    {
        memmove(new_path+index, session->cur_dir, cur_dir_length);
        index += cur_dir_length;
        if (new_path[index-1] != '\\')
        {
            new_path[index++] = '\\';
        }
    }
    memmove(new_path+index, path, path_length);
    
    /* Resolve directory changing sequences. */
    ftpsrv_normalize_path(new_path);
    path_length = strlen(new_path);
    *wrong_path = FALSE;

    /* If there is not root directory in result, signalize error. */
    if ((path_length < root_length) || 
        (new_path[root_length] != '\\') || 
        strncmp(new_path, session->root_dir, root_length) != 0)
    {
        _mem_free(new_path);
        new_path = NULL;
        *wrong_path = TRUE;
    }
    return(new_path);
}

static void ftpsrv_normalize_path(char* path)
{
    char        *bs = path;
    char        *prev_bs = path;
    bool        init = true;
    uint32_t    offset = 0;

    /* Replace path segments for listing one level up */
    while((bs = strchr(prev_bs+offset, '\\')) != NULL)
    {
        if (init)
        {
            init = false;
            prev_bs = bs;
        }
        /* Encountered "\.." sequence. */
        if ((bs[1] == '.') && (bs[2] == '.'))
        {
            uint32_t index;
            
            index = (bs[3] == '\0') ? 1 : 0;
            memmove(prev_bs+index, bs+3, strlen(bs+3)+1);
            bs = prev_bs;
            offset = 0;
        }
        /* Encountered "..\" sequence. */
        else if ((bs[-1] == '.') && (bs[-2] == '.') && (bs[1] == '\0'))
        {
            memmove(prev_bs, bs, strlen(bs)+1);
            bs = prev_bs;
            offset = 0;
        }
        /* Encountered "\\" sequence. */
        else if (bs[1] == '\\')
        {
            memmove(bs, bs+1, strlen(bs)+1);
            offset = 0;
        }
        else
        {
            offset = 1;
        }
        prev_bs = bs;
    }
}

bool ftpsrv_check_authtbl(FTPSRV_SESSION_STRUCT* session)
{
    bool retval = FALSE;
    FTPSRV_AUTH_STRUCT* table = session->auth_tbl;
    FTPSRV_AUTH_STRUCT* input = &(session->auth_input);

    /* Browse table until end or entry matches to data from user */
    while(table->uid)
    {
        if (!strncmp(input->uid, table->uid, strlen(table->uid)))
        {
            if (!strncmp(input->pass, table->pass, strlen(table->pass)))
            {
                /* Valid username/password combination found */
                retval = TRUE;
                if (table->path)
                {
                    session->root_dir = table->path;
                }
                session->cur_dir[0] = '\\';
                session->cur_dir[1] = '\0';
            }
            break;
        }
        table++;
    }

    return(retval);
}

void ftpsrv_sock_shutdown(FTPSRV_SESSION_STRUCT* session)
{
    if ((session->data_sock != 0) && (session->data_sock != RTCS_SOCKET_ERROR))
    {
        shutdown(session->data_sock, FLAG_ABORT_CONNECTION);
        session->data_sock = RTCS_SOCKET_ERROR;
    }
}

uint32_t ftpsrv_open_data_connection(FTPSRV_SESSION_STRUCT* session)
{
    uint32_t socket;

    /* Use connection based on session type (active/passive). */
    if (session->passive)
    {
        socket = ftpsrv_accept_passive_conn(session);
        session->message = (char*) ftpsrvmsg_no_datacon;
    }
    else
    {
        socket = ftpsrv_open_active_conn(session);
        session->message = (char*) ftpsrvmsg_cannot_open;
    }

    if (socket != RTCS_SOCKET_ERROR)
    {
        session->message = NULL;
    }
    return(socket);
}

/*FUNCTION*--------------------------------------------------------------
*
*Function Name  :ftpsrv_open_active_conn
*Returned Value : Socket if successful or zero if failed 
*Comments       :
*   This function opens the active data connection to send the list information
*
*END*---------------------------------------------------------------------*/

static uint32_t ftpsrv_open_active_conn(FTPSRV_SESSION_STRUCT* session)
{
    uint32_t sock;
    uint32_t error;
    uint16_t family = session->data_sockaddr.sa_family;
    uint16_t size;
    uint32_t option;
    sockaddr sock_info;

    /* If there wasn't PORT command read information about remote host from socket */
    if (session->state != FTPSRV_STATE_CONNECT)
    {
        size = sizeof(session->data_sockaddr);
        error = getpeername(session->control_sock, &session->data_sockaddr, &size);
        if (error != RTCS_OK)
        {
             return(RTCS_SOCKET_ERROR);
        }
        family = session->data_sockaddr.sa_family;

        switch(family)
        {
            case AF_INET:
                ((sockaddr_in*) &session->data_sockaddr)->sin_port++;
                break;
            case AF_INET6:
                ((sockaddr_in6*) &session->data_sockaddr)->sin6_port++;
                break;
            default:
                session->state = FTPSRV_STATE_BAD_PROT;
                return(0);
                break;
        }
    }

    sock = socket(family, SOCK_STREAM, 0);
    if (sock == RTCS_SOCKET_ERROR)
    {
        return(RTCS_SOCKET_ERROR);
    }
    option = FTPSRVCFG_TX_BUFFER_SIZE;
    setsockopt(sock, SOL_TCP, OPT_TBSIZE, &option, sizeof(option));
    option = FTPSRVCFG_RX_BUFFER_SIZE;
    setsockopt(sock, SOL_TCP, OPT_RBSIZE, &option, sizeof(option));

    /* Read information about control socket so we can bind data socket */
    size = sizeof(sock_info);
    error = getsockname(session->control_sock, &sock_info, &size);
    if (error != RTCS_OK)
    {
        goto ERROR;
    }
    /* Setup outcoming port */
    switch(family)
    {
        case AF_INET:
            ((sockaddr_in*) &sock_info)->sin_port = IPPORT_FTPDATA;
            break;
        case AF_INET6:
            ((sockaddr_in6*) &sock_info)->sin6_port = IPPORT_FTPDATA;
            break;
        default:
            session->state = FTPSRV_STATE_BAD_PROT;
            goto ERROR;
            break;
    }
    /* bind socket and connect to remote host */
    error = bind(sock, &sock_info, sizeof(sock_info));
    if (error != RTCS_OK)
    {
        goto ERROR;
    }
    error = connect(sock, &session->data_sockaddr, sizeof(session->data_sockaddr));
    if (error != RTCS_OK)
    {
        goto ERROR;
    }

    return(sock);
ERROR:
    shutdown(sock, FLAG_ABORT_CONNECTION);
    return(RTCS_SOCKET_ERROR);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name : ftpsrv_accept_passive_conn
* Returned Value : uint32_t socket if successful, zero if fail.
* Comments  :  This function is used for accepting incoming connection in passive mode
*               
* Usage:
*
*END*---------------------------------------------------------------------*/
static uint32_t ftpsrv_accept_passive_conn(FTPSRV_SESSION_STRUCT* session)
{
    uint32_t conn_sock;
    uint32_t new_sock;
    unsigned short length = 0;
    struct sockaddr remote_addr;
    
    new_sock = accept(session->data_sock, (sockaddr*) &remote_addr, &length);
    return(new_sock);
}

uint16_t ftpsrv_open_passive_conn(FTPSRV_SESSION_STRUCT* session, uint32_t family)
{
    uint16_t port = 0;
    uint32_t error;
    uint16_t size;
    struct sockaddr sock_info;
    uint32_t option;

    /*
    ** Fill sockaddr structure depending on address family, 
    ** set error message and return if family is unsupported.
    */
    switch(family)
    {
        case FTPSRV_PROT_IPV4:
            session->data_sockaddr.sa_family = AF_INET;
            /* Set port to zero, so RTCS decides which port will be used */
            ((sockaddr_in*) &session->data_sockaddr)->sin_port = 0;
            break;
        case FTPSRV_PROT_IPV6:
            session->data_sockaddr.sa_family = AF_INET6;
            ((sockaddr_in6*) &session->data_sockaddr)->sin6_port = 0;
            break;
        default:
            session->state = FTPSRV_STATE_BAD_PROT;
            return(0);
            break;
    }

    /* Abort data connection if there alredy is one */
    ftpsrv_sock_shutdown(session);

    /* Create new socket for incoming connection */
    session->data_sock = socket(session->data_sockaddr.sa_family, SOCK_STREAM, 0);
    if (session->data_sock == RTCS_SOCKET_ERROR)
    {
        return(0);
    }
    option = FTPSRVCFG_TX_BUFFER_SIZE;
    setsockopt(session->data_sock, SOL_TCP, OPT_TBSIZE, &option, sizeof(option));
    option = FTPSRVCFG_RX_BUFFER_SIZE;
    setsockopt(session->data_sock, SOL_TCP, OPT_RBSIZE, &option, sizeof(option));

    /* Bind it to local address */
    error = bind(session->data_sock, &session->data_sockaddr, sizeof(session->data_sockaddr));
    if (error != RTCS_OK)
    {
        ftpsrv_sock_shutdown(session);
        return(0);
    }

    /* Listen for incoming connection */
    error = listen(session->data_sock, 0);
    if (error != RTCS_OK)
    {
        ftpsrv_sock_shutdown(session);
        return(0);
    }
    
    size = sizeof(sock_info);
    /* Get port from data socket, so we can create correct response */
    error = getsockname(session->data_sock, &sock_info, &size);
    if (error != RTCS_OK)
    {
        ftpsrv_sock_shutdown(session);
        return(0);
    }

    switch(sock_info.sa_family)
    {
        case AF_INET:
            port = ((sockaddr_in*) &sock_info)->sin_port;
            break;
        case AF_INET6:
            port = ((sockaddr_in6*) &sock_info)->sin6_port;
            break;
        default:
            break;
    }

    return(port);
}

bool ftpsrv_process_auth(FTPSRV_SESSION_STRUCT* session, uint32_t auth_req)
{
    bool retval = FALSE;

    switch(session->auth_state)
    {
        case FTPSRV_LOGGED_OUT:
            if (!strncmp(session->command, "USER", 4))
            {
                session->auth_state = FTPSRV_USER;
            }
            retval = !auth_req;
            break;
        case FTPSRV_USER:
            if (!strncmp(session->command, "PASS", 4))
            {
                retval = TRUE;
            }
            session->message = (char*) ftpsrvmsg_badseq;
            break;
        case FTPSRV_LOGGED_IN:
            retval = TRUE;
            break;
    }
    /* Reset auth status if auth failed */
    if (!retval)
    {
        session->auth_state = FTPSRV_LOGGED_OUT;
    }
    return(retval);
}

void ftpsrv_send_msg(FTPSRV_SESSION_STRUCT* session, const char* message)
{
   send(session->control_sock, (void*)message, strlen(message), 0);
}

/*
** Internal function for server parameters initialization
**
** IN:
**      FTPSRV_STRUCT* server - server structure pointer
**
**      FTPSRV_PARAM_STRUCT* params - pointer to user parameters if there are any
** OUT:
**      none
**
** Return Value: 
**      uint32_t      error code. FTPSRV_OK if everything went right, positive number otherwise
*/
uint32_t ftpsrv_set_params(FTPSRV_STRUCT *server, FTPSRV_PARAM_STRUCT *params)
{
    /* Stack size */
    uint32_t stack;

    server->params.port = IPPORT_FTP;
    #if RTCSCFG_ENABLE_IP4
    server->params.ipv4_address.s_addr = FTPSRVCFG_DEF_ADDR;
    #endif
    #if RTCSCFG_ENABLE_IP6  
    server->params.ipv6_address = in6addr_any;
    server->params.ipv6_scope_id = 0;
    #endif
    server->params.af = FTPSRVCFG_AF;
    server->params.max_ses = FTPSRVCFG_DEF_SES_CNT;
    server->params.root_dir = "mfs:";
    server->params.server_prio = FTPSRVCFG_DEF_SERVER_PRIO;
    server->params.auth_table = NULL;
    server->params.use_nagle = 0;

    /* If there is parameters structure copy nonzero values to server */
    if (params != NULL)
    {
        if (params->port)
            server->params.port = params->port;
        #if RTCSCFG_ENABLE_IP4
        if (params->ipv4_address.s_addr != 0)
            server->params.ipv4_address = params->ipv4_address;
        #endif
        #if RTCSCFG_ENABLE_IP6
        if (params->ipv6_address.s6_addr != NULL)
            server->params.ipv6_address = params->ipv6_address;
        if (params->ipv6_scope_id)
            server->params.ipv6_scope_id = params->ipv6_scope_id;
        #endif
        if (params->af)
            server->params.af = params->af;
        if (params->max_ses)
            server->params.max_ses = params->max_ses;
        if (params->root_dir)
        {
            server->params.root_dir = params->root_dir;
        }
        else
        {
            return(FTPSRV_ERROR);
        }
        if (params->server_prio)
            server->params.server_prio = params->server_prio;
        if (params->auth_table)
            server->params.auth_table = params->auth_table;
        if (params->use_nagle)
            server->params.use_nagle = params->use_nagle;
    }

    return(FTPSRV_OK);
}


/*
** Function for socket initialization (both IPv4 and IPv6)
**
** IN:
**      FTPSRV_STRUCT* server - server structure pointer
**
**      uint16_t      family - IP protocol family
** OUT:
**      none
**
** Return Value: 
**      uint32_t      error code. FTPSRV_OK if everything went right, positive number otherwise
*/

uint32_t ftpsrv_init_socket(FTPSRV_STRUCT *server, uint16_t family)
{
    uint32_t option;
    uint32_t error;
    sockaddr sin_sock;
    uint32_t sock = 0;
    uint32_t is_error = 0;

    _mem_zero(&sin_sock, sizeof(sockaddr));
    #if RTCSCFG_ENABLE_IP4
    if (family == AF_INET) /* IPv4 */
    {
       
        if ((server->sock_v4 = socket(AF_INET, SOCK_STREAM, 0)) == RTCS_ERROR)
        {
            return(FTPSRV_CREATE_FAIL);
        }
        ((sockaddr_in *)&sin_sock)->sin_port   = server->params.port;
        ((sockaddr_in *)&sin_sock)->sin_addr   = server->params.ipv4_address;
        ((sockaddr_in *)&sin_sock)->sin_family = AF_INET;
        sock = server->sock_v4;
    }
    else
    #endif    
    #if RTCSCFG_ENABLE_IP6   
    if (family == AF_INET6) /* IPv6 */
    {
        if ((server->sock_v6 = socket(AF_INET6, SOCK_STREAM, 0)) == RTCS_ERROR)
        {
            return(FTPSRV_CREATE_FAIL);
        }
        ((sockaddr_in6 *)&sin_sock)->sin6_port      = server->params.port;
        ((sockaddr_in6 *)&sin_sock)->sin6_family    = AF_INET6;
        ((sockaddr_in6 *)&sin_sock)->sin6_scope_id  = server->params.ipv6_scope_id;
        ((sockaddr_in6 *)&sin_sock)->sin6_addr      = server->params.ipv6_address;
        sock = server->sock_v6;
    }
    else
    #endif    
    {
        return(FTPSRV_BAD_FAMILY);
    }
    /* Set socket options */
    option = FTPSRVCFG_SEND_TIMEOUT;
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_SEND_TIMEOUT, &option, sizeof(option));
    option = FTPSRVCFG_CONNECT_TIMEOUT;
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_CONNECT_TIMEOUT, &option, sizeof(option));
    option = FTPSRVCFG_TIMEWAIT_TIMEOUT;
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_TIMEWAIT_TIMEOUT, &option, sizeof(option));
    option = 0;     
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_RECEIVE_TIMEOUT, &option, sizeof(option));
    option = FALSE; 
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_RECEIVE_NOWAIT, &option, sizeof(option));
    option = TRUE;  
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_RECEIVE_PUSH, &option, sizeof(option));
    option = !server->params.use_nagle;
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_NO_NAGLE_ALGORITHM, &option, sizeof(option));
    option = FTPSRV_CMD_SOCK_BUFFER_SIZE/*FTPSRVCFG_TX_BUFFER_SIZE*/;
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_TBSIZE, &option, sizeof(option));
    option = FTPSRV_CMD_SOCK_BUFFER_SIZE/*FTPSRVCFG_RX_BUFFER_SIZE*/;
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_RBSIZE, &option, sizeof(option));

    if (is_error)
    {
        return(FTPSRV_SOCKOPT_FAIL);
    }

    /* Bind socket */
    error = bind(sock, &sin_sock, sizeof(sin_sock));
    if(error != RTCS_OK)
    {
        return(FTPSRV_BIND_FAIL);
    }

    /* Listen */
    error = listen(sock, 0);
    if (error != RTCS_OK)
    {
        return(FTPSRV_LISTEN_FAIL);
    }
    return(FTPSRV_OK);
}

#endif //MQX_USE_IO_OLD
