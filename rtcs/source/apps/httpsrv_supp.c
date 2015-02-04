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
*   HTTPSRV support functions.
*
*
*END************************************************************************/

#include "httpsrv.h"
#include "httpsrv_prv.h"
#include "httpsrv_supp.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <rtcs_base64.h>
#include <ctype.h>
#include <limits.h>

#if MQX_USE_IO_OLD
#include <fio.h>
#include <posix.h>
#else
#include "nio.h"
#include "errno.h"
#include <strings.h>
#endif

/*
* string table item
*/
typedef struct
{
    int  id;
    char *str;
}httpsrv_table_row;

/* Structure defining MIME types table row */
typedef struct
{
    uint32_t length;         /* extension string length (strlen) */
    char*    ext;            /* extension string */
    int      content_type;   /* extension MIME type string */
    bool     use_cache;      /* cache use */
}httpsrv_content_table_row;

/*
* content type
*/
static const httpsrv_table_row content_type[] = {
        { HTTPSRV_CONTENT_TYPE_PLAIN,       "text/plain" },
        { HTTPSRV_CONTENT_TYPE_HTML,        "text/html" },
        { HTTPSRV_CONTENT_TYPE_CSS,         "text/css" },
        { HTTPSRV_CONTENT_TYPE_GIF,         "image/gif" },
        { HTTPSRV_CONTENT_TYPE_JPG,         "image/jpeg" },
        { HTTPSRV_CONTENT_TYPE_PNG,         "image/png" },
        { HTTPSRV_CONTENT_TYPE_JS,          "application/javascript" },
        { HTTPSRV_CONTENT_TYPE_ZIP,         "application/zip" },
        { HTTPSRV_CONTENT_TYPE_PDF,         "application/pdf" },
        { HTTPSRV_CONTENT_TYPE_OCTETSTREAM, "application/octet-stream" },
        { 0,    0 }
};

/*
* Response status to reason conversion table
*/
static const httpsrv_table_row reason_phrase[] = {
        { 200, "OK" },
        { 400, "Bad Request"},
        { 401, "Unauthorized" },
        { 403, "Forbidden" },
        { 404, "Not Found" },
        { 411, "Length Required"},
        { 414, "Request-URI Too Long"},
        { 500, "Internal Server Error"},
        { 501, "Not Implemented" },
        { 0,   "" }
};

/*
** Extension -> content type conversion table.
** This table rows MUST be ordered by size and alphabetically
** so we can list througth it quicly
*/
static httpsrv_content_table_row content_tbl[] = {
    /* Size,          extension, MIME type,                        Cache? */
    {sizeof("js")-1 ,   "js",    HTTPSRV_CONTENT_TYPE_JS,          TRUE},
    {sizeof("css")-1,   "css",   HTTPSRV_CONTENT_TYPE_CSS,         TRUE},
    {sizeof("gif")-1,   "gif",   HTTPSRV_CONTENT_TYPE_GIF,         TRUE}, 
    {sizeof("htm")-1,   "htm",   HTTPSRV_CONTENT_TYPE_HTML,        TRUE},
    {sizeof("jpg")-1,   "jpg",   HTTPSRV_CONTENT_TYPE_JPG,         TRUE},
    {sizeof("pdf")-1,   "pdf",   HTTPSRV_CONTENT_TYPE_PDF,         FALSE}, 
    {sizeof("png")-1,   "png",   HTTPSRV_CONTENT_TYPE_PNG,         TRUE},
    {sizeof("txt")-1,   "txt",   HTTPSRV_CONTENT_TYPE_PLAIN,       FALSE},
    {sizeof("zip")-1,   "zip",   HTTPSRV_CONTENT_TYPE_ZIP,         FALSE},
    {sizeof("html")-1,  "html",  HTTPSRV_CONTENT_TYPE_HTML,        TRUE},
    {sizeof("shtm")-1,  "shtm",  HTTPSRV_CONTENT_TYPE_HTML,        FALSE},
    {sizeof("shtml")-1, "shtml", HTTPSRV_CONTENT_TYPE_HTML,        FALSE},
    /* Following row MUST have length set to zero so we have proper array termination */
    {0,                      "", HTTPSRV_CONTENT_TYPE_OCTETSTREAM, FALSE}
};

static uint32_t httpsrv_sendextstr(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session, uint32_t length);
static inline void httpsrv_print(HTTPSRV_SESSION_STRUCT *session, char* format, ...);
static void httpsrv_urldecode(char* url);
static void httpsrv_urlcleanup(char* url);
static char* httpsrv_get_table_str(httpsrv_table_row *table, const _mqx_int id);
static int httpsrv_get_table_int(httpsrv_table_row *table, char* str, uint32_t len);
static void httpsrv_process_file_type(char* extension, HTTPSRV_SESSION_STRUCT* session);

#if MQX_USE_IO_OLD == 0
size_t httpsrv_fsize(FILE *file)
{
    fseek (file, 0, SEEK_END);
    return ftell(file);
}
#endif

/*
** Call function by name. Used for dynamic web pages.
**
** IN:
**      HTTPSRV_CALL_PARAM* param - parameters structure:
**          HTTPSRV_FN_LINK_STRUCT *table   - table containing function names and poiters to callbacks
**          pointer                param    - parameter for callback
**          char                   *fn_name - name of called function
**          HTTPSRV_CALLBACK_TYPE  type     - callback type (CGI/SSI)
**
** OUT:
**      none
**
** Return Value:
**      none
*/

/*
** Send extended string to socket (dynamic webpages).
**
** IN:
**      HTTPSRV_STRUCT         *server - server structure.
**      HTTPSRV_SESSION_STRUCT *session - session for sending.
**      char                   *str - string to send.
**      uint32_t                length - length of source string.
**
** OUT:
**      none
**
** Return Value:
**      int - number of bytes processed.
*/
static uint32_t httpsrv_sendextstr(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session, uint32_t length)
{
    char     *src;
    char*    t_buffer;
    uint32_t n;
    uint32_t retval;

    t_buffer = session->response.script_buffer;
    src = session->buffer.data + session->buffer.offset;
    
    n = strlen(t_buffer);

    if (n == 0) /* Start searching for script token. */
    {
        uint32_t n_send;
        uint32_t max;
        uint32_t i;
        char     token[] = "<%";

        i = 0;
        max = length;
        
        for (n_send = 0; (n_send < max) && (token[i]); n_send++)
        {
            if (src[n_send] == token[i])
            {
                t_buffer[i] = token[i];
                i++;
                max = ((n_send+2) < length) ? n_send+2 : length;
            }
            else
            {
                i = 0;
                max = length;
                session->response.script_buffer[0] = 0;
            }
        }
        session->buffer.offset = n_send - i;
        retval = i;
    }
    else if (n == 1) /* There was already the less-than sign.*/
    {
        if (*src == '%')
        {
            /* There is script token spanning over two buffers. */
            t_buffer[n] = *src;
            retval = 1;
        }
        else
        {
            /* There was no script token, send missing less-than sign. */
            send(session->sock, session->response.script_buffer, n, 0);
            *((uint16_t *) session->response.script_buffer) = 0;
            retval = 0;
        }
    }
    else /* Script token beginning found, find end. */
    {
        uint32_t name_length;
        char     fname[HTTPSRVCFG_MAX_SCRIPT_LN] = {0};
        uint32_t term_length;

        name_length = strcspn(src, " ;%<>\r\n\t\f");

        if ((src[name_length] == '%') && (src[name_length + 1] == '>'))
        {
            term_length = 2;
        }
        else
        {
            term_length = 1;
        }

        if ((name_length > 1) && (name_length < HTTPSRVCFG_MAX_SCRIPT_LN))
        {
            HTTPSRV_SCRIPT_MSG* msg_ptr;

            snprintf(fname, name_length+1, "%s", src);
            /* Form up message for handler task and send it. */
            msg_ptr = _msg_alloc(server->script_msg_pool);
            if (msg_ptr != NULL)
            {
                msg_ptr->header.TARGET_QID = server->script_msgq;
                msg_ptr->header.SOURCE_QID = server->script_msgq;
                msg_ptr->header.SIZE = sizeof(HTTPSRV_SCRIPT_MSG);
                msg_ptr->session = session;
                msg_ptr->type = HTTPSRV_SSI_CALLBACK;
                msg_ptr->name = fname;
                msg_ptr->ses_tid = _task_get_id();
                _msgq_send(msg_ptr);
                /* Wait until SSI is processed. */
                _task_block();
            }
            *((uint16_t *) session->response.script_buffer) = 0;
        }
        retval = name_length + term_length;
    }
    return(retval);
}

/*
** Read data from HTTP server.
**
** First copy data from session buffer if there are any and then read rest from socket if required.
**
** IN:
**      HTTPSRV_SESSION_STRUCT *session - session to use for reading.
**      char                   *dst - user buffer to read to.
**      _mqx_int               len - size of user buffer.
**
** OUT:
**      none
**
** Return Value:
**      int - number of bytes read.
*/
_mqx_int httpsrv_read(HTTPSRV_SESSION_STRUCT *session, char *dst, _mqx_int len)
{
    int read = 0;
    uint32_t data_size = session->buffer.offset;

    /* If there are any data in buffer copy them to user buffer */
    if (data_size > 0)
    {
        uint32_t length = (data_size < len) ? data_size : len;
        uint32_t tail = HTTPSRV_SES_BUF_SIZE_PRV-length;
        
        _mem_copy(session->buffer.data, dst, length);
        memmove(session->buffer.data, session->buffer.data+length, tail);
        _mem_zero(session->buffer.data+tail, length);
        session->buffer.offset -= length;
        read = length;
    }

    /* If there is some space remaining in user buffer try to read from socket */
    while (read < len)
    {
        uint32_t received;
        
        received = recv(session->sock, dst+read, len-read, 0);
        if (RTCS_ERROR != received)
        {
            read += received;
        }
        else
        {
            break;
        }
    }
    
    return read;
}

/*
** Write data to buffer. If buffer is full during write flush it to client.
**
** IN:
**      HTTPSRV_SESSION_STRUCT *session - session used for write.
**      char*                   src     - pointer to data to send.
**      _mqx_int                len     - length of data in bytes.
**
** OUT:
**      none
**
** Return Value:
**      _mqx_int - number of bytes written.
*/
_mqx_int httpsrv_write(HTTPSRV_SESSION_STRUCT *session, char *src, _mqx_int len)
{
    uint32_t space = HTTPSRV_SES_BUF_SIZE_PRV - session->buffer.offset;
    uint32_t retval = len;

    /* User buffer is bigger than session buffer - send user data directly */
    if (len > HTTPSRV_SES_BUF_SIZE_PRV)
    {
        /* If there are some data already buffered send them to client first */
        httpsrv_ses_flush(session);
        return(send(session->sock, src, len, 0));
    }

    /* No space in buffer - make some */
    if (space < len)
    {
        httpsrv_ses_flush(session);
    }

    /* Now we can save user data to buffer and eventually send them to client */
    _mem_copy(src, session->buffer.data+session->buffer.offset, len);
    session->buffer.offset += len;
    
    if (session->buffer.offset >= HTTPSRV_SES_BUF_SIZE_PRV)
    {
        httpsrv_ses_flush(session);
    }

    return(retval);
}

/*
** Send data from session buffer to client.
**
** IN:
**      HTTPSRV_SESSION_STRUCT *session - session to use.
**
** OUT:
**      none
**
** Return Value:
**      int - number of bytes send.
*/
uint32_t httpsrv_ses_flush(HTTPSRV_SESSION_STRUCT *session)
{
    uint32_t length = 0;
    
    while (length != session->buffer.offset)
    {
        uint32_t remaining;

        length = send(session->sock, session->buffer.data, session->buffer.offset, 0);
    
        if (length == RTCS_ERROR)
        {
            session->state = HTTPSRV_SES_CLOSE;
            length = 0;
            break;
        }
        remaining = session->buffer.offset - length;

        if (remaining > 0)
        {
            _mem_zero(session->buffer.data, length);
            memmove(session->buffer.data, session->buffer.data+length, remaining);
            session->buffer.offset = remaining;
        }
        else
        {
            _mem_zero(session->buffer.data, session->buffer.offset);
            session->buffer.offset = 0;
            break;
        }
    }

    return(length);
}

/*
** Get string for ID from table.
**
** IN:
**      httpsrv_table_row  *table - table to be searched
**      _mqx_int          id - search ID
**
** OUT:
**      none
**
** Return Value:
**      char* - pointer to result. NULL if not found.
*/
static char* httpsrv_get_table_str(httpsrv_table_row *table, const _mqx_int id)
{
    httpsrv_table_row *ptr = table;

    while ((ptr->str) && (id != ptr->id))
    {
        ptr++;
    }
    return ptr->str;
}

/*
** Get ID for string from table
**
** IN:
**      httpsrv_table_row     *tbl - table to be searched
**      char*             str - search string
**      uint32_t           len - length of string
**
** OUT:
**      none
**
** Return Value:
**      ID corresponding to searched string. Zero if not found.
*/
static int httpsrv_get_table_int(httpsrv_table_row *table, char* str, uint32_t len)
{
    httpsrv_table_row *ptr = table;

    while ((ptr->id) && (strncmp(str, ptr->str, len)))
    {
        ptr++;
    }
    return (ptr->id);
}

/*
** Send http header according to the session response structure.
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session used for transmission
**      _mqx_int                content_len - content length
**      bool                 has_entity - flag indicating if HTTP entity is going to be send following header.
**
** OUT:
**      none
**
** Return Value:
**      none
*/
void httpsrv_sendhdr(HTTPSRV_SESSION_STRUCT *session, _mqx_int content_len, bool has_entity)
{ 
    if (session->response.hdrsent)
    {
         return;
    }
    
    httpsrv_print(session, "%s %d %s\r\n", HTTPSRV_PROTOCOL_STRING, session->response.status_code,
            httpsrv_get_table_str((httpsrv_table_row*)reason_phrase, session->response.status_code));
    
    httpsrv_print(session, "Server: %s\r\n", HTTPSRV_PRODUCT_STRING);
    
    /* Check authorization */
    if (session->response.status_code == HTTPSRV_CODE_UNAUTHORIZED)
    {
        httpsrv_print(session, "WWW-Authenticate: Basic realm=\"%s\"\r\n", session->response.auth_realm->name);
    }
    httpsrv_print(session, "Connection: %s\r\n", session->keep_alive ? "Keep-Alive":"close");
    
    /* If there will be entity body send content type */
    if (has_entity)
    {
        httpsrv_print(session, "Content-Type: %s\r\n", httpsrv_get_table_str((httpsrv_table_row*)content_type, session->response.content_type));
    }

    httpsrv_print(session, "Cache-Control: ");
    if (session->response.cacheable)
    {
        httpsrv_print(session, "max-age=%d\r\n", HTTPSRVCFG_CACHE_MAXAGE);
    }
    else
    {
        if (session->response.auth_realm != NULL)
        {
            httpsrv_print(session, "no-store\r\n");
        }
        else
        {
            httpsrv_print(session, "no-cache\r\n");
        }
    }

    /* Only non zero length cause sending Content-Length header field */
    if (content_len > 0)
    {
        httpsrv_print(session, "Content-Length: %d\r\n", content_len);
    }
    /* End of header */
    httpsrv_print(session, "\r\n");
    
    /* Commented out to prevent problems with filesystem on KHCI USB */
    //if ((content_len == 0) && (!has_entity))
    {
        httpsrv_ses_flush(session);
    }
    session->response.hdrsent = 1;
}

/*
** Print data to session
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session used for transmission
**      char*                   format - format for snprintf function
**      void                    ...    - parameters to print
**
** OUT:
**      none
**
** Return Value:
**      none
*/
static void httpsrv_print(HTTPSRV_SESSION_STRUCT *session, char* format, ...)
{
    va_list ap;
    uint32_t req_space = 0;
    char* buffer = session->buffer.data;
    int buffer_space = HTTPSRV_SES_BUF_SIZE_PRV - session->buffer.offset;

    va_start(ap, format);
    /*
    ** First we always test if there is enough space in buffer. If there is not, 
    ** we flush it first and then write.
    */
    req_space = vsnprintf(NULL, 0, format, ap);
    va_end(ap);
 
    va_start(ap, format);
    if (req_space > buffer_space)
    {
        httpsrv_ses_flush(session);
        buffer_space = HTTPSRV_SES_BUF_SIZE_PRV;
    }
    session->buffer.offset += vsnprintf(buffer+session->buffer.offset, buffer_space, format, ap);
    va_end(ap);
}

/*
** Convert file extension to content type and determine what kind of cache control should be used.
**
** IN:
**      char* extension - extension to convert
**
** IN/OUT:
**      HTTPSRV_SESSION_STRUCT* session - session pointer
**
** Return Value:
**      none
*/
static void httpsrv_process_file_type(char* extension, HTTPSRV_SESSION_STRUCT* session)
{
    httpsrv_content_table_row* row = content_tbl;
    uint32_t length = 0;

    if (extension != NULL)
    {
        length = strlen(extension)-1;
        /* Move pointer after the dot. */
        extension++;
    }
    else
    {
        goto NO_EXT;
    }

    /* List throught table rows until length match */
    while ((row->length) && (row->length < length))
    {
        row++;
    }

    /* Do a search in valid rows */
    while (row->length == length)
    {
        if (strcasecmp(extension, row->ext) == 0)
        {
            session->response.content_type = row->content_type;
            session->response.cacheable = row->use_cache;

            if (session->response.auth_realm != NULL)
            {
                /* If authentication is required, then response MUST NOT be cached */
                session->response.cacheable = FALSE;
            }
            return;
        }
        row++;
    }

NO_EXT:
    session->response.content_type = HTTPSRV_CONTENT_TYPE_OCTETSTREAM;
    session->response.cacheable = FALSE;
}

/*
** Send file to client
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session used for transmission
**      HTTPSRV_STRUCT*         server - server structure
**
** OUT:
**      none
**
** Return Value:
**      none
*/
void httpsrv_sendfile(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session)
{
    char *ext;
    int  length;
    char *buffer;

    buffer =  session->buffer.data;

    ext = strrchr(session->request.path, '.');
    httpsrv_process_file_type(ext, session);

    /* Check if file has server side includes */
    if ((0 == strcasecmp(ext, ".shtml")) || (0 == strcasecmp(ext, ".shtm")))
    {
        /* 
         * Disable keep-alive for this session otherwise we would have to 
         * wait for session timeout.
         */
        session->keep_alive = 0;
        httpsrv_sendhdr(session, 0, 1);
        #if MQX_USE_IO_OLD
        fseek(session->response.file, session->response.len, IO_SEEK_SET);
        #else
        fseek(session->response.file, session->response.len, SEEK_SET);
        #endif
        length = fread(buffer+session->buffer.offset, 1, HTTPSRV_SES_BUF_SIZE_PRV-session->buffer.offset, session->response.file);
        if (length > 0)
        {
            uint32_t offset;

            offset = httpsrv_sendextstr(server, session, length);
            session->response.len += session->buffer.offset;
            httpsrv_ses_flush(session);
            session->response.len += offset;
        }
    }
    else
    {
        #if MQX_USE_IO_OLD
        httpsrv_sendhdr(session, session->response.file->SIZE, 1);
        fseek(session->response.file, session->response.len, IO_SEEK_SET);
        #else
        httpsrv_sendhdr(session, httpsrv_fsize(session->response.file), 1);
        fseek(session->response.file, session->response.len, SEEK_SET);
        #endif    
        length = fread(buffer+session->buffer.offset, 1, HTTPSRV_SES_BUF_SIZE_PRV-session->buffer.offset, session->response.file);
        if (length > 0)
        {
            session->buffer.offset += length;
            httpsrv_ses_flush(session);
            session->response.len += length;
        }
    }

    if (length <= 0)
    {
        session->state = HTTPSRV_SES_END_REQ;
    }
}

/*
** Send error page to client
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session used for transmission
**      const char*             title - title of error page
**      const char*             text - text displayed on error page
**
** OUT:
**      none
**
** Return Value:
**      none
*/
void httpsrv_send_err_page(HTTPSRV_SESSION_STRUCT *session, const char* title, const char* text)
{
    uint32_t length;
    char* page;
    
    length = snprintf(NULL, 0, ERR_PAGE_FORMAT, title, text);
    length++;
    page = _mem_alloc(length*sizeof(char));

    session->response.content_type = HTTPSRV_CONTENT_TYPE_HTML;

    if (page != NULL)
    {
        snprintf(page, length, ERR_PAGE_FORMAT, title, text);
        httpsrv_sendhdr(session, strlen(page), 1);
        httpsrv_write(session, page, strlen(page));
        httpsrv_ses_flush(session);
        _mem_free(page);
    }
    else
    {
        httpsrv_sendhdr(session, 0, 0);
    }
}

/*
** Process one line of http request header
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer.
**      HTTPSRV_STRUCT *server - pointer to server structure (needed for session parameters).
**      char* buffer - pointer to begining of line with request.
**
** OUT:
**      none
**
** Return Value:
**      none
*/
void httpsrv_process_req_hdr_line(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT* session, char* buffer)
{
    char* param_ptr = NULL;

    if (strncmp(buffer, "Connection: ", 12) == 0)
    {
        param_ptr = buffer+12;

        if ((session->keep_alive_enabled) &&
           ((strncmp(param_ptr, "Keep-Alive", 10) == 0) || 
            (strncmp(param_ptr, "keep-alive", 10) == 0)))
        {
            session->keep_alive = 1;
        }
        else
        {
            session->keep_alive = 0;
        }
    }
    else if (strncmp(buffer, "Content-Length: ", 16) == 0)
    {
        param_ptr = buffer+16;
        session->request.content_length = strtoul(param_ptr, NULL, 10);
    }
    else if (strncmp(buffer, "Content-Type: ", 14) == 0)
    {
        param_ptr = buffer+14;
        session->request.content_type = httpsrv_get_table_int((httpsrv_table_row*) content_type, param_ptr, strlen(param_ptr)-2);
        if (session->request.content_type == 0)
        {
            session->request.content_type = HTTPSRV_CONTENT_TYPE_OCTETSTREAM;
        }
    }
    else if (strncmp(buffer, "Authorization: ", 15) == 0)
    {
        param_ptr = buffer+15;
        if (strncmp(param_ptr, "Basic ", 6) == 0)
        {
            uint32_t decoded_length = 0;
            char* user = NULL;
            char* pass = NULL;
            param_ptr += 6;

            /* evaluate number of bytes required for worst case (no padding) */
            decoded_length = (strlen(param_ptr)/4) * 3 + 1;
            user = _mem_alloc_zero(sizeof(char)*decoded_length);
            if (user != NULL)
            {
                _mem_set_type(user, MEM_TYPE_HTTPSRV_AUTH);
                base64_decode(user, param_ptr, decoded_length);
                session->request.auth.user_id = user;
            }
            else
            {
                return;
            }

            pass = strchr(user, ':');
            if (pass)
            {
                *pass = '\0';
                pass = pass + 1;
                session->request.auth.password = pass;
            }
        }
    }
}

/*
** Read http method 
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer.
**      HTTPSRV_STRUCT *server - pointer to server structure (needed for session parameters).
**      char* buffer - pointer to begining of line with request.
**
** OUT:
**      none
**
** Return Value:
**      none
*/
void httpsrv_process_req_method(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session, char* buffer)
{
    char* uri_begin  = NULL;
    char* uri_end = NULL;
    uint32_t written;
    uint32_t i;
    
    if (strncmp(buffer, "GET ", 4) == 0)
    {
        session->request.method = HTTPSRV_REQ_GET;
    }
    else if (strncmp(buffer, "POST ", 5) == 0)
    {
        session->request.method = HTTPSRV_REQ_POST;
    }
    else /* Unknown method - not implemented response */
    {  
        session->request.method = HTTPSRV_REQ_UNKNOWN;
        return;
    }

    /* Parse remaining part of line */
    uri_begin = strchr(buffer, ' ');

    if (uri_begin != NULL)
    {
        uri_begin++;
        uri_end = strchr(uri_begin, ' ');
        if (uri_end != NULL)
        {
            *uri_end = '\0';
        }
    }
    else
    {
        session->request.path[0] = '\0';
        session->response.status_code = HTTPSRV_CODE_BAD_REQ;
        return;
    }

    /* Preprocess URI */
    httpsrv_urldecode(uri_begin);
    httpsrv_urlcleanup(uri_begin);

    written = snprintf(session->request.path, server->params.max_uri, "%s", uri_begin);
    /* Check if whole URI is saved in buffer */
    if (written > server->params.max_uri-1)
    {
        session->request.path[0] = '\0';
        /* URI is too long so we set proper status code for response */
        session->response.status_code = HTTPSRV_CODE_URI_TOO_LONG;
    }
}

/*
** Decode percent encoded string (inplace)
**
** IN:
**      char* url - string to decode
**
** OUT:
**      none
**
** Return Value:
**      none
*/
static void httpsrv_urldecode(char* url)
{
    char* src = url;
    char* dst = url;

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

/*
** URL cleanup (remove invalid path segments - /./ and /../)
**
** IN:
**      char* url - string to decode
**
** OUT:
**      none
**
** Return Value:
**      none
*/
static void httpsrv_urlcleanup(char* url)
{
    char* src = url;
    char* dst = url;

    while(*src != '\0')
    {
        if ((src[0] == '/') && (src[1] == '.'))
        {
            if (src[2] ==  '/')
            {
                src += 2;
            }
            else if ((src[2] == '.') && (src[3] == '/'))
            {
                src += 3;
            }
        }
        *dst++ = *src++;
    }
    *dst = '\0';
}

/*
** Get realm for requested path
**
** IN:
**      char*            path - search path.
**      HTTPSRV_STRUCT*  server - pointer to server structure (needed for session parameters).
**
** OUT:
**      none
**
** Return Value: 
**      HTTPSRV_AUTH_REALM_STRUCT* - authentication realm for requested path. Null if not found.
*/
HTTPSRV_AUTH_REALM_STRUCT* httpsrv_req_realm(HTTPSRV_STRUCT *server, char* path)
{
    HTTPSRV_AUTH_REALM_STRUCT* table = server->params.auth_table;
    
    if (table == NULL)
    {
        return(NULL);
    }

    while((table->path != NULL) && (strstr(path, table->path) == NULL))
    {
        table++;
    }

    return(table->path ? table : NULL);
}

/*
** Check user authentication credentials
**
** IN:
**      HTTPSRV_AUTH_REALM_STRUCT* realm - search realm.
**      HTTPSRV_AUTH_USER_STRUCT*  user - user to authenticate.
**
** OUT:
**      none
**
** Return Value: 
**      int - 1 if user is sucessfully authenticated, zero otherwise.
*/
int httpsrv_check_auth(HTTPSRV_AUTH_REALM_STRUCT* realm, HTTPSRV_AUTH_USER_STRUCT* user)
{
    HTTPSRV_AUTH_USER_STRUCT* users = NULL;
    
    if ((realm == NULL) || (user == NULL))
    {
        return(0);
    }

    users = realm->users;

    while (users->user_id != NULL)
    {
        if (!strcmp(users->user_id, user->user_id) && !strcmp(users->password, user->password))
        {
            return(1);
        }
        users++;
    }
    return(0);
}

/*
** Function for CGI request processing
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer.
**      HTTPSRV_STRUCT*         server - pointer to server structure (needed for session parameters).
**      char*                   cgi_name - name of cgi function.
**
** OUT:
**      none
**
** Return Value: 
**      none
*/
void httpsrv_process_cgi(HTTPSRV_STRUCT *server, HTTPSRV_SESSION_STRUCT *session, char* cgi_name)
{
    HTTPSRV_SCRIPT_MSG* msg_ptr;

    msg_ptr = _msg_alloc(server->script_msg_pool);

    if (msg_ptr == NULL)
    {
        /* Out of memory - report server error */
        session->response.status_code = HTTPSRV_CODE_INTERNAL_ERROR;
        return;
    }

    msg_ptr->header.TARGET_QID = server->script_msgq;
    msg_ptr->header.SOURCE_QID = server->script_msgq;
    msg_ptr->header.SIZE = sizeof(HTTPSRV_SCRIPT_MSG);
    msg_ptr->session = session;
    msg_ptr->type = HTTPSRV_CGI_CALLBACK;
    msg_ptr->name = cgi_name;
    msg_ptr->ses_tid = _task_get_id();
    _msgq_send(msg_ptr);
    /* wait until CGI is processed */
    _task_block();

    /*
    ** There is some unread content from client after CGI finished. 
    ** It must be read and discarded if we have keep-alive enabled
    ** so it does not affect next request.
    */
    if (session->request.content_length)
    {
        char *tmp = NULL;

        tmp = _mem_alloc(HTTPSRV_TMP_BUFFER_SIZE);
        if (tmp != NULL)
        {
            uint32_t length = session->request.content_length;

            while(length)
            {
                uint32_t retval;

                retval = httpsrv_read(session, tmp, HTTPSRV_TMP_BUFFER_SIZE);
                if (!retval)
                {
                    break;
                }
                length -= retval;
            }
            _mem_free(tmp);
            session->request.content_length = 0;
        } 
    }
    return;
}

/*
** Internal function for server parameters initialization
**
** IN:
**      HTTPSRV_STRUCT* server - server structure pointer
**
**      HTTPSRV_PARAM_STRUCT* params - pointer to user parameters if there are any
** OUT:
**      none
**
** Return Value: 
**      uint32_t      error code. HTTPSRV_OK if everything went right, positive number otherwise
*/
uint32_t httpsrv_set_params(HTTPSRV_STRUCT *server, HTTPSRV_PARAM_STRUCT *params)
{
    server->params.port = HTTPSRVCFG_DEF_PORT;
    #if RTCSCFG_ENABLE_IP4
    server->params.ipv4_address.s_addr = HTTPSRVCFG_DEF_ADDR;
    #endif
    #if RTCSCFG_ENABLE_IP6  
    server->params.ipv6_address = in6addr_any;
    server->params.ipv6_scope_id = 0;
    #endif
    server->params.af = HTTPSRVCFG_AF;
    server->params.max_uri = HTTPSRVCFG_DEF_URL_LEN;
    server->params.max_ses = HTTPSRVCFG_DEF_SES_CNT;
    server->params.root_dir = "tfs:";
    server->params.index_page = HTTPSRVCFG_DEF_INDEX_PAGE;
    server->params.cgi_lnk_tbl = NULL;
    server->params.ssi_lnk_tbl = NULL;
    server->params.server_prio = HTTPSRVCFG_DEF_SERVER_PRIO;
    server->params.script_prio = HTTPSRVCFG_DEF_SERVER_PRIO;
    server->params.script_stack = 0;
    server->params.auth_table = NULL;
    server->params.use_nagle = 0;
    server->params.alias_tbl = NULL;

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
        if (params->max_uri)
            server->params.max_uri = params->max_uri;
        if (params->max_ses)
            server->params.max_ses = params->max_ses;
        if (params->root_dir)
            server->params.root_dir = params->root_dir;
        if (params->index_page)
            server->params.index_page = params->index_page;
        if (params->cgi_lnk_tbl)
            server->params.cgi_lnk_tbl = params->cgi_lnk_tbl;
        if (params->ssi_lnk_tbl)
            server->params.ssi_lnk_tbl = params->ssi_lnk_tbl;
        if (params->server_prio)
            server->params.server_prio = params->server_prio;
        if (params->script_prio)
            server->params.script_prio = params->script_prio;
        if (params->script_stack)
            server->params.script_stack = (params->script_stack < HTTPSRV_CGI_HANDLER_STACK) ? HTTPSRV_CGI_HANDLER_STACK : params->script_stack;  
        if (params->auth_table)
            server->params.auth_table = params->auth_table;
        if (params->use_nagle)
            server->params.use_nagle = params->use_nagle;
        if (params->alias_tbl)
            server->params.alias_tbl = params->alias_tbl;
    }

    /* If there is some callback table and no handler stack is set, use default stack size */
    if ((server->params.cgi_lnk_tbl || server->params.ssi_lnk_tbl) && !server->params.script_stack)
    {
        server->params.script_stack = HTTPSRV_CGI_HANDLER_STACK;
    }

    return(HTTPSRV_OK);
}

/*
** Function for socket initialization (both IPv4 and IPv6)
**
** IN:
**      HTTPSRV_STRUCT* server - server structure pointer
**
**      uint16_t      family - IP protocol family
** OUT:
**      none
**
** Return Value: 
**      uint32_t      error code. HTTPSRV_OK if everything went right, positive number otherwise
*/
uint32_t httpsrv_init_socket(HTTPSRV_STRUCT *server, uint16_t family)
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
            return(HTTPSRV_CREATE_FAIL);
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
            return(HTTPSRV_CREATE_FAIL);
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
        return(HTTPSRV_BAD_FAMILY);
    }
    /* Set socket options */
    option = HTTPSRVCFG_SEND_TIMEOUT;
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_SEND_TIMEOUT, &option, sizeof(option));
    option = HTTPSRVCFG_CONNECT_TIMEOUT;
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_CONNECT_TIMEOUT, &option, sizeof(option));
    option = HTTPSRVCFG_TIMEWAIT_TIMEOUT;
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_TIMEWAIT_TIMEOUT, &option, sizeof(option));
    option = HTTPSRVCFG_RECEIVE_TIMEOUT;     
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_RECEIVE_TIMEOUT, &option, sizeof(option));
    option = FALSE; 
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_RECEIVE_NOWAIT, &option, sizeof(option));
    option = TRUE;  
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_RECEIVE_PUSH, &option, sizeof(option));
    option = !server->params.use_nagle;
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_NO_NAGLE_ALGORITHM, &option, sizeof(option));
    option = HTTPSRVCFG_TX_BUFFER_SIZE;
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_TBSIZE, &option, sizeof(option));
    option = HTTPSRVCFG_RX_BUFFER_SIZE;
    is_error = is_error || setsockopt(sock, SOL_TCP, OPT_RBSIZE, &option, sizeof(option));

    if (is_error)
    {
        return(HTTPSRV_SOCKOPT_FAIL);
    }

    /* Bind socket */
    error = bind(sock, &sin_sock, sizeof(sin_sock));
    if(error != RTCS_OK)
    {
        return(HTTPSRV_BIND_FAIL);
    }

    /* Listen */
    error = listen(sock, 0);
    if (error != RTCS_OK)
    {
        return(HTTPSRV_LISTEN_FAIL);
    }
    return(HTTPSRV_OK);
}

/*
** Function for CGI calling
**
** IN:
**      HTTPSRV_CGI_CALLBACK_FN function - pointer to user function to be called as CGI
**
**      HTTPSRV_SCRIPT_MSG* msg_ptr - pointer to message containing data required for CGI parameter
** OUT:
**      none
**
** Return Value: 
**      none
*/
void httpsrv_call_cgi(HTTPSRV_CGI_CALLBACK_FN function, HTTPSRV_SCRIPT_MSG* msg_ptr)
{
    HTTPSRV_SESSION_STRUCT *session = msg_ptr->session;
    HTTPSRV_CGI_REQ_STRUCT cgi_param;
    char server_ip[RTCS_IP_ADDR_STR_SIZE];
    char remote_ip[RTCS_IP_ADDR_STR_SIZE];
    struct sockaddr l_address;
    struct sockaddr r_address;
    uint16_t length = sizeof(sockaddr);
    uint32_t retval;

    /* Fill callback parameter */
    cgi_param.ses_handle = (uint32_t) session;
    cgi_param.request_method = session->request.method;
    cgi_param.content_type = (HTTPSRV_CONTENT_TYPE) session->request.content_type;
    cgi_param.content_length = session->request.content_length;
    
    getsockname(session->sock, &l_address, &length);
    getpeername(session->sock, &r_address, &length);

    if (l_address.sa_family == AF_INET)
    {
        inet_ntop(l_address.sa_family, &((struct sockaddr_in*) &l_address)->sin_addr.s_addr, server_ip, sizeof(server_ip));
        inet_ntop(r_address.sa_family, &((struct sockaddr_in*) &r_address)->sin_addr.s_addr, remote_ip, sizeof(remote_ip));
        cgi_param.server_port = ((struct sockaddr_in*) &l_address)->sin_port;
    }
    else if (l_address.sa_family == AF_INET6)
    {
        inet_ntop(l_address.sa_family, ((struct sockaddr_in6*) &l_address)->sin6_addr.s6_addr, server_ip, sizeof(server_ip));
        inet_ntop(r_address.sa_family, ((struct sockaddr_in6*) &r_address)->sin6_addr.s6_addr, remote_ip, sizeof(remote_ip));
        cgi_param.server_port = ((struct sockaddr_in6*) &l_address)->sin6_port;
    }
    
    cgi_param.auth_type = HTTPSRV_AUTH_BASIC;
    cgi_param.remote_user = session->request.auth.user_id;
    cgi_param.remote_addr = remote_ip;
    cgi_param.server_name = server_ip;
    cgi_param.script_name = msg_ptr->name;
    cgi_param.server_protocol = HTTPSRV_PROTOCOL_STRING;
    cgi_param.server_software = HTTPSRV_PRODUCT_STRING;
    cgi_param.query_string = session->request.query;
    cgi_param.gateway_interface = HTTPSRV_CGI_VERSION_STRING;

    /* Call the function */
    function(&cgi_param);
}

/*
** Function for SSI calling
**
** IN:
**      HTTPSRV_SSI_CALLBACK_FN function - pointer to user function to be called as SSI
**
**      HTTPSRV_SCRIPT_MSG* msg_ptr - pointer to message containing data required for SSI parameter
** OUT:
**      none
**
** Return Value: 
**      none
*/
void httpsrv_call_ssi(HTTPSRV_SSI_CALLBACK_FN function, HTTPSRV_SCRIPT_MSG* msg_ptr)
{
    char* tmp;
    HTTPSRV_SSI_PARAM_STRUCT ssi_param;
    _mqx_int retval;

    ssi_param.ses_handle = (uint32_t) msg_ptr->session;
    tmp = strchr(msg_ptr->name, ':');
    if (tmp != NULL)
    {
        *tmp++ = '\0';
    }
    else
    {
        tmp = "";
    }
    ssi_param.com_param = tmp;

    function(&ssi_param);
}

/*
** Function for searching callback for name in function table (SSI/CGI)
**
** IN:
**      HTTPSRV_FN_LINK_STRUCT* table - table to search in.
**
**       char* name - name to search.
** OUT:
**      stack_size - pointer to variable to store callback stack size to.
**
** Return Value: 
**      HTTPSRV_FN_CALLBACK - function callback if successfull, NULL if not found
*/
HTTPSRV_FN_CALLBACK httpsrv_find_callback(HTTPSRV_FN_LINK_STRUCT* table, char* name, uint32_t* stack_size)
{
    HTTPSRV_FN_CALLBACK retval = NULL;

    if ((table == NULL) || (name == NULL))
    {
        goto EXIT;
    }

    while (table->callback && *(table->callback))
    {
        if (0 == strcmp(name, table->fn_name))
        {
            retval = (HTTPSRV_FN_CALLBACK) table->callback;
            if (stack_size != NULL)
            {
                *stack_size = table->stack_size;
            }
            break;
        }
        table++;
    }
EXIT:
    return(retval);
}

/*
** Function for checking of HTTP request validity
**
** IN:
**       HTTPSRV_SESSION_STRUCT *session - session to check
** OUT:
**      none
**
** Return Value: 
**      uint32_t - HTTPSRV_OK if request is valid, HTTPSRV_ERR if invalid.
*/
uint32_t httpsrv_check_request(HTTPSRV_SESSION_STRUCT *session)
{
    char* cp;

    /* If method is not implemented return without request processing */
    if (session->request.method == HTTPSRV_REQ_UNKNOWN)
    {
        session->response.status_code = HTTPSRV_CODE_NOT_IMPLEMENTED;
        return(HTTPSRV_ERR);
    }
    /* We need content length for post requests */
    else if ((session->request.method == HTTPSRV_REQ_POST) && (session->request.content_length == 0))
    {
        session->response.status_code = HTTPSRV_CODE_NO_LENGTH;
        return(HTTPSRV_ERR);
    }
    /* Check request path */
    cp = strrchr(session->request.path, '/');
    if (cp == NULL)
    {
        /* We have invalid request */
        session->response.status_code = HTTPSRV_CODE_BAD_REQ;
        return(HTTPSRV_ERR);
    }

    session->response.status_code = HTTPSRV_CODE_OK;
    return(HTTPSRV_OK);
}

/*
** Function for resolving aliases
**
** IN:
**      HTTPSRV_ALIAS* table - table of aliases
**
**       char* path - path to unalias
** OUT:
**      char** new_root - new root directory for request, NULL if alias is not present
**
** Return Value: 
**      char* - unaliased request path
*/
char* httpsrv_unalias(HTTPSRV_ALIAS* table, char* path, char** new_root)
{
    char* retval = path;
    *new_root = NULL;

    while (table && (table->path != NULL) && (table->alias != NULL))
    {
        if (!strncmp(table->alias, path, strlen(table->alias)))
        {
            *new_root = table->path;
            retval = path + strlen(table->alias);
            break;
        }
        table++;
    }
    return(retval);
}

/*
** Function resolving requested file path in filesystem
**
** IN:
**       char *path - path to resolve.
**
**       char* root - root directory of requested file.
** OUT:
**      none
**
** Return Value: 
**      char* - full path in filesystem with correct slashes.
*/
char* httpsrv_get_full_path(char *path, char *root)
{
    char* buffer;
    char* tmp;

    /* Session buffer is too small */
    buffer = _mem_alloc_zero(strlen(path) + strlen(root) + 1);
    if (buffer == NULL)
    {
        return(NULL);
    }
    /* Create full path from root directory and path */
    _mem_copy(root, buffer, strlen(root)+1);
    strcat(buffer, path);

    tmp = buffer;
    /* Correct path slashes */
    while (*tmp)
    {
        if (*tmp == '/')
        {
            *tmp = '\\';
        }
        tmp++;
    }
    return(buffer);
}

/*
** Detach script processing to separate task
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
void httpsrv_detach_script(HTTPSRV_DET_TASK_PARAM* task_params)
{
    _mqx_uint  error;
    _mqx_uint  priority;

    error = _task_get_priority(MQX_NULL_TASK_ID, &priority);
    if (error != MQX_OK)
    {
        return;
    }

    error = RTCS_task_create("httpsrv detached script", priority, task_params->stack_size, httpsrv_detached_task, (void *)task_params);
    if (error != MQX_OK)
    {
        return;
    }
}

/*
** Wait for session CGI unlock. Wait until session timeout.
**
** IN:
**      HTTPSRV_SESSION_STRUCT* session - session structure pointer.
**
** OUT:
**      none
**
** Return Value: 
**      MQX_OK if CGI processign finished during wait, error code otherwise.
*/
uint32_t httpsrv_wait_for_cgi(HTTPSRV_SESSION_STRUCT *session)
{
    MQX_TICK_STRUCT time_ticks;
    uint32_t        time_msec;
    TIME_STRUCT     time;

    if (session->script_tid == 0)
    {
        return(MQX_OK);
    }

    time_msec = session->time + session->timeout;
    /* Prepare information about time in appropriate format */
    time.SECONDS = time_msec/1000;
    time.MILLISECONDS = time_msec%1000;
    _time_to_ticks(&time, &time_ticks);

    return(_lwsem_wait_until(&session->lock, &time_ticks));
}
