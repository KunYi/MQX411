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
*   Header for HTTPSRV.
*
*
*END************************************************************************/

#ifndef HTTPSRV_H_
#define HTTPSRV_H_

#include <rtcs.h>

/*
** HTTP server status codes
*/
#define HTTPSRV_OK 0
#define HTTPSRV_BIND_FAIL 1 
#define HTTPSRV_LISTEN_FAIL 2
#define HTTPSRV_ERR 3
#define HTTPSRV_CREATE_FAIL 4
#define HTTPSRV_BAD_FAMILY 5
#define HTTPSRV_SOCKOPT_FAIL 6
#define HTTPSRV_SES_INVALID 7

/*
** Authentication types
*/
typedef enum httpstv_auth_type
{
    HTTPSRV_AUTH_INVALID,
    HTTPSRV_AUTH_BASIC,
    HTTPSRV_AUTH_DIGEST /* Not supported yet! */
} HTTPSRV_AUTH_TYPE;

/*
* http request method type
*/
typedef enum httpsrv_req_method
{
    HTTPSRV_REQ_UNKNOWN,
    HTTPSRV_REQ_GET,
    HTTPSRV_REQ_POST,
    HTTPSRV_REQ_HEAD
} HTTPSRV_REQ_METHOD;

/*
* http content type
*/
typedef enum httpsrv_content_type
{
    HTTPSRV_CONTENT_TYPE_OCTETSTREAM = 1,
    HTTPSRV_CONTENT_TYPE_PLAIN,
    HTTPSRV_CONTENT_TYPE_HTML,
    HTTPSRV_CONTENT_TYPE_CSS,
    HTTPSRV_CONTENT_TYPE_GIF,
    HTTPSRV_CONTENT_TYPE_JPG,
    HTTPSRV_CONTENT_TYPE_PNG,
    HTTPSRV_CONTENT_TYPE_JS,
    HTTPSRV_CONTENT_TYPE_ZIP,
    HTTPSRV_CONTENT_TYPE_PDF,
} HTTPSRV_CONTENT_TYPE;

/*
** Authentication user structure
*/
typedef struct httpsrv_auth_user_struct
{
    char* user_id;   /* User ID - usually name*/
    char* password;  /* Password */
}HTTPSRV_AUTH_USER_STRUCT;

/*
** Authentication realm structure
*/
typedef struct httpsrv_auth_realm_struct
{
    char*                     name;       /* Name of realm. Send to client so user know which login/pass should be used. */
    char*                     path;       /* Path to file/directory to protect. Relative to root directory */
    HTTPSRV_AUTH_TYPE         auth_type;  /* Authentication type to use. */
    HTTPSRV_AUTH_USER_STRUCT* users;      /* Table of allowed users. */
} HTTPSRV_AUTH_REALM_STRUCT;

/*
** CGI request structure. Contains variables specified in RFC3875 (The Common Gateway Interface (CGI) Version 1.1).
** Structure is extended by session handle.
*/
typedef struct httpsrv_cgi_request_struct
{
    uint32_t              ses_handle;         /* Session handle required for various read/write operations*/
    /* 
    *  Following is subset of variables from RFC3875. 
    ** Please see http://tools.ietf.org/html/rfc3875#section-4.1 for details
    */
    HTTPSRV_REQ_METHOD   request_method;     /* Request method (GET, POST, HEAD) see HTTPSRV_REQ_METHOD enum */
    HTTPSRV_CONTENT_TYPE content_type;       /* Content type */
    uint32_t             content_length;     /* Content length */
    uint32_t             server_port;        /* Local connection port */
    char*                remote_addr;        /* Remote client address */
    char*                server_name;        /* Server hostname/IP */
    char*                script_name;        /* CGI name */
    char*                server_protocol;    /* Server protocol name and version (HTTP/1.0) */
    char*                server_software;    /* Server software identification string */
    char*                query_string;       /* Request query string */
    char*                gateway_interface;  /* Gateway interface type and version (CGI/1.1)*/
    char*                remote_user;        /* Remote user name  */
    HTTPSRV_AUTH_TYPE    auth_type;          /* Auth type */
}HTTPSRV_CGI_REQ_STRUCT;

/*
** CGI response struct. This structure is filled by CGI function.
*/
typedef struct httpsrv_cgi_response_struct
{
    uint32_t              ses_handle;              /* Session handle for reading/writing */
    HTTPSRV_CONTENT_TYPE  content_type;            /* Response content type */
    uint32_t              content_length;          /* Response content length */
    uint32_t              status_code;             /* Status code (200, 404, etc.)*/
    char*                 data;                    /* Pointer to data to write */
    uint32_t              data_length;             /* Length of data in bytes */
}HTTPSRV_CGI_RES_STRUCT;

/*
** Directory aliases
*/
typedef struct httpsrv_alias
{
    char* alias;
    char* path;
}HTTPSRV_ALIAS;

/*
** Server side include parameter structure.
** Passed to user SSI function.
*/
typedef struct httpsrv_ssi_param_struct
{
    uint32_t ses_handle;         /* Session handle for reading/writing */
    char*    com_param;          /* Server side include command parameter (separated from command by ":") */
}HTTPSRV_SSI_PARAM_STRUCT;

/*
** Server side include callback prototype
*/
typedef _mqx_int(*HTTPSRV_SSI_CALLBACK_FN)(HTTPSRV_SSI_PARAM_STRUCT* param);

/*
** SSI callback link structure
*/
typedef struct httpsrv_ssi_link_struct
{
    char*                   fn_name;            /* Function name */
    HTTPSRV_SSI_CALLBACK_FN callback;           /* Pointer to user function */
    uint32_t                stack;              /* Stack size for SSI. If set to zero, default task will be used */
} HTTPSRV_SSI_LINK_STRUCT;

/*
** CGI callback prototype
*/
typedef _mqx_int(*HTTPSRV_CGI_CALLBACK_FN)(HTTPSRV_CGI_REQ_STRUCT* param);

/*
** CGI callback link structure
*/
typedef struct httpsrv_cgi_link_struct
{
    char*                   fn_name;            /* Function name */
    HTTPSRV_CGI_CALLBACK_FN callback;           /* Pointer to user function */
    uint32_t                stack;              /* Stack size for CGI. If set to zero, default task will be used */
} HTTPSRV_CGI_LINK_STRUCT;

/*
** HTTP server parameters
*/
typedef struct httpsrv_param_struct
{
    uint16_t                   af;             /* Inet protocol family */
    unsigned short             port;           /* Listening port */
  #if RTCSCFG_ENABLE_IP4
    in_addr                    ipv4_address;   /* Listening IPv4 address */
  #endif
  #if RTCSCFG_ENABLE_IP6    
    in6_addr                   ipv6_address;   /* Listening IPv6 address */
    uint32_t                   ipv6_scope_id;  /* Scope ID for IPv6 */
  #endif
    _mqx_uint                  max_uri;        /* maximal URI length */
    _mqx_uint                  max_ses;        /* maximal sessions count */
    bool                       use_nagle;      /* enable/disable nagle algorithm for server sockets */
    HTTPSRV_CGI_LINK_STRUCT*   cgi_lnk_tbl;    /* cgi callback table */
    HTTPSRV_SSI_LINK_STRUCT*   ssi_lnk_tbl;    /* function callback table (dynamic web pages) */
    HTTPSRV_ALIAS*             alias_tbl;      /* table od directory aliases */
    uint32_t                   server_prio;    /* server main task priority */
    uint32_t                   script_prio;    /* script handler priority */
    uint32_t                   script_stack;   /* script handler stack */
    char*                      root_dir;       /* root directory */
    char*                      index_page;     /* index page full path and name */
    HTTPSRV_AUTH_REALM_STRUCT* auth_table;     /* Table of authentication realms */
    
} HTTPSRV_PARAM_STRUCT;

/*
** Initialize and run HTTP server
** Returns server handle when successful, zero otherwise.
*/
uint32_t HTTPSRV_init(HTTPSRV_PARAM_STRUCT *params);

/*
** Stop and release HTTP server
** Returns RTCS_OK when successful, RTCS_ERR otherwise.
*/
uint32_t HTTPSRV_release(uint32_t server_h);

uint32_t HTTPSRV_cgi_write(HTTPSRV_CGI_RES_STRUCT* response);
uint32_t HTTPSRV_cgi_read(uint32_t ses_handle, char* buffer, uint32_t length);
uint32_t HTTPSRV_ssi_write(uint32_t ses_handle, char* data, uint32_t length);

#endif /* HTTP_H_ */
