/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
* Copyright 1989-2008 ARC International
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
*   This file contains the implementation of an MQX
*   socket I/O device.
*
*
*END************************************************************************/

#include <rtcs.h>

#if MQX_USE_IO_OLD

#include <fio.h>
#define SOCKIO_MIN_BUFFER 64
#define SOCKIO_MAX_BUFFER (64*1024)

typedef struct io_socket {
   uint32_t  SOCKET;
   uint32_t  COUNT;
   uint32_t  SIZE;
   char *BUFFER;
} IO_SOCKET, * IO_SOCKET_PTR;

_mqx_int _io_socket_open  (MQX_FILE_PTR, char *,  char *);
_mqx_int _io_socket_close (MQX_FILE_PTR);
_mqx_int _io_socket_read  (MQX_FILE_PTR, char *, _mqx_int);
_mqx_int _io_socket_write (MQX_FILE_PTR, char *, _mqx_int);
_mqx_int _io_socket_ioctl (MQX_FILE_PTR, _mqx_uint, void *);


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_install
* Returned Value :
* Comments       :
*
*END*-----------------------------------------------------------------*/

int32_t _io_socket_install
   (
      char *identifier
   )
{ /* Body */

   return _io_dev_install(identifier,
                          _io_socket_open,
                          _io_socket_close,
                          _io_socket_read,
                          _io_socket_write,
#if MQX_VERSION < 250
                          (int32_t(_CODE_PTR_)(MQX_FILE_PTR, uint32_t, uint32_t *))
#endif
                          _io_socket_ioctl,
                          NULL );
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_open
* Returned Value :
* Comments       :
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_socket_open
   (
      MQX_FILE_PTR fd_ptr,
      char   *open_name_ptr,
      char   *flags_ptr
   )
{ /* Body */
   IO_SOCKET_PTR io_ptr;

   io_ptr = RTCS_mem_alloc_zero(sizeof(*io_ptr));
   if (io_ptr == NULL) {
      return MQX_OUT_OF_MEMORY;
   } /* Endif */
   
   _mem_set_type(io_ptr, MEM_TYPE_IO_SOCKET);
   
   fd_ptr->DEV_DATA_PTR = io_ptr;

   io_ptr->SOCKET = (uint32_t)flags_ptr;

   return MQX_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_close
* Returned Value :
* Comments       :
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_socket_close
   (
      MQX_FILE_PTR fd_ptr
   )
{ /* Body */

   IO_SOCKET_PTR  io_ptr;


   if (fd_ptr->DEV_DATA_PTR != NULL) 
   {
      io_ptr = fd_ptr->DEV_DATA_PTR;
      if (io_ptr->BUFFER != NULL) 
      {
         _mem_free(io_ptr->BUFFER);
      }
      fd_ptr->DEV_DATA_PTR = NULL;
      _mem_free(io_ptr);
   }
   return MQX_OK;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_read
* Returned Value : void
* Comments       :
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_socket_read
   (
      MQX_FILE_PTR fd_ptr,
      char   *data_ptr,
      _mqx_int    num
   )
{ /* Body */
   IO_SOCKET_PTR io_ptr;

   io_ptr = fd_ptr->DEV_DATA_PTR;
   if (io_ptr != NULL) {
   
   // flush any pending data before reading
   if (io_ptr->COUNT) {
      send(io_ptr->SOCKET, io_ptr->BUFFER, io_ptr->COUNT, 0);
      io_ptr->COUNT = 0;
   }

     return (_mqx_int)recv(io_ptr->SOCKET, data_ptr, num, 0);
   } /* Endif */

   return -1;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_set_send_push
* Returned Value : void
* Comments       :
*
*END*-----------------------------------------------------------------*/
static _mqx_int _io_socket_set_send_push( uint32_t sock, bool value)
{
   return setsockopt(sock, SOL_TCP, OPT_SEND_PUSH, &value, sizeof(value));
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_write
* Returned Value : void
* Comments       :
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_socket_write
   (
      MQX_FILE_PTR fd_ptr,
      char   *data_ptr,
      _mqx_int    num
   )
{ /* Body */
   IO_SOCKET_PTR io_ptr = fd_ptr->DEV_DATA_PTR;
   _mqx_int      sent = IO_ERROR;

   if (io_ptr != NULL) {
      if ( io_ptr->SIZE ) {
         if  (io_ptr->COUNT+num > io_ptr->SIZE) {
            
            if (io_ptr->COUNT) {
               send(io_ptr->SOCKET, io_ptr->BUFFER, io_ptr->COUNT, 0);
               io_ptr->COUNT = 0;
            }
            sent = (_mqx_int)send(io_ptr->SOCKET, data_ptr, num, 0);

         } else if (num==1) {
            // most writes are 1 byte.
            io_ptr->BUFFER[io_ptr->COUNT++] = *data_ptr;
            
            sent=1;
            
            if (io_ptr->COUNT == io_ptr->SIZE) {
               // buffer is full, flush it
               send(io_ptr->SOCKET, io_ptr->BUFFER, io_ptr->COUNT, 0);
               io_ptr->COUNT=0;
            }
            
         } else {
            _mem_copy(data_ptr,&io_ptr->BUFFER[io_ptr->COUNT],num);
            io_ptr->COUNT+=num;
            sent=num;
         }
      } else {     
         sent = (_mqx_int)send(io_ptr->SOCKET, data_ptr, num, 0);
      } 
   }

   return sent;

} /* Endbody */

static char buffer[256];


/*FUNCTION*-------------------------------------------------------------
*
* Function Name  : _io_socket_ioctl
* Returned Value : void
* Comments       :
*
*END*-----------------------------------------------------------------*/

_mqx_int _io_socket_ioctl
   (
      MQX_FILE_PTR fd_ptr,
      _mqx_uint   cmd,
      void       *param_ptr
   )
{ /* Body */
   IO_SOCKET_PTR  io_ptr;
   _mqx_int       result = IO_ERROR, bytes;
   uint32_t        size;

   io_ptr = fd_ptr->DEV_DATA_PTR;
   if (io_ptr != NULL) {
      switch (cmd) {
         case IO_IOCTL_CHAR_AVAIL:
            if (RTCS_selectset(&io_ptr->SOCKET, 1, (uint32_t)-1)) {
               *(bool *)param_ptr = TRUE;
            } else {
               *(bool *)param_ptr = FALSE;
            } /* Endif */
            result=MQX_OK;
            break;         
      
         case IO_IOCTL_FLUSH_OUTPUT:
            if (io_ptr->COUNT) {
               bytes = send(io_ptr->SOCKET, io_ptr->BUFFER, io_ptr->COUNT, 0);
               if (bytes==io_ptr->COUNT) {
                  io_ptr->COUNT = 0;
                  result = MQX_OK;
               }
            } else {
               result = MQX_OK;
            }
            break;
            
         case IO_IOCTL_SET_BLOCK_MODE:
            size = *(uint32_t *)param_ptr;
            result=MQX_OK;
            if (size != io_ptr->SIZE) {
               // First, clear out the old buffer
               if (io_ptr->BUFFER) {
                  if (io_ptr->COUNT) {
                     bytes = send(io_ptr->SOCKET, io_ptr->BUFFER, io_ptr->COUNT, 0);
                     io_ptr->COUNT = 0;
                  }
                  _mem_free(io_ptr->BUFFER);
                  io_ptr->BUFFER = NULL;
               }
            
               if (size) {
                  // Size is set, entering block mode
                  if (size < SOCKIO_MIN_BUFFER) {
                     size = SOCKIO_MIN_BUFFER;
                  }
                  if (size > SOCKIO_MAX_BUFFER) {
                     size = SOCKIO_MAX_BUFFER;
                  }

                  io_ptr->BUFFER = RTCS_mem_alloc(size);
                  if (io_ptr->BUFFER==NULL) {
                     size = 0;
                     result = IO_ERROR;
                  } else {
                     _io_socket_set_send_push(io_ptr->SOCKET, TRUE);
                  }
                  
               }                     

               io_ptr->SIZE = size;
            }
            break;      
            
         case IO_IOCTL_GET_BLOCK_SIZE:
            result=MQX_OK;
            *(bool *)param_ptr = io_ptr->SIZE;
            break;
            
       }      
   }

   return result;

} /* Endbody */

#endif // MQX_USE_IO_OLD
