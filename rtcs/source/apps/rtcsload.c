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
*   This file contains the interface functions to the
*   Exec function of the RTCS Communication Library.
*
*
*END************************************************************************/

#include <rtcs.h>


/*FUNCTION*-------------------------------------------------------------
*
* Function Name    : RTCS_load
* Returned Value   : RTCS_OK or error code
* Comments  :  Routine to load and execute a bootimage from a server.
*
*END*-----------------------------------------------------------------*/

uint32_t RTCS_load
   (
      FT_CALL_STRUCT_PTR   ft_ptr,
         /* [IN] file transfer protocol to use */
      void                *ft_data,
         /* [IN] file transfer data */
      FF_CALL_STRUCT_PTR   ff_ptr,
         /* [IN] file format to decode */
      void                *ff_data,
         /* [IN] file format data */
      uint32_t              flags
         /* [IN] optional flags */
   )
{ /* Body */
   uint32_t   error;  /* also used as packet size */
   unsigned char *data;

   /*
   ** Initialize
   */
   error = (*ft_ptr->OPEN)(ft_data);
   if (error) {
      return error;
   } /* Endif */
   error = (*ff_ptr->START)(ff_data);
   if (error) {
      (*ft_ptr->CLOSE)();
      return error;
   } /* Endif */

   /*
   ** Repeat until end of file
   */
   while (!(*ft_ptr->EOFT)()) {

      /*
      ** Retrieve some data...
      */
      data = (*ft_ptr->READ)(&error);
      if (!data) {
         (*ff_ptr->EOFD)();
         return error;
      } /* Endif */

      /*
      ** ...and decode it
      **
      ** Note:  If data is non-NULL, then error contains the
      **        number of bytes read.
      */
      error = (*ff_ptr->DECODE)(error, data);
      if (error) {
         (*ft_ptr->CLOSE)();
         return error;
      } /* Endif */
   } /* Endwhile */

   /*
   ** Check if the decoding succeeded
   */
   error = (*ff_ptr->EOFD)();
   if (error) {
      (*ft_ptr->CLOSE)();
      return error;
   } /* Endif */

   /*
   ** End the file transfer session
   */
   error = (*ft_ptr->CLOSE)();
   if (error) {
      return error;
   } /* Endif */

   /*
   ** If we get here, the file was retrieved and decoded successfully.
   ** Run the image.
   */
   if (flags & RTCSLOADOPT_EXEC) {
      (*ff_ptr->EXEC)();
   } /* Endif */

   return RTCS_OK;

} /* Endbody */


/* EOF */
