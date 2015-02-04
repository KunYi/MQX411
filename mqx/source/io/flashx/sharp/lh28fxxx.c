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
*   The file contains functions to program the LH28FXXX
*   flash devices.
*
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "flashx.h"
#include "flashxprv.h"
#include "lh28fxxx_prv.h"
#include "lh28fxxx.h"

const FLASHX_DEVICE_IF_STRUCT _lh28fxxx_if = {
    _lh28fxxx_erase,
    _lh28fxxx_program,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _lh28fxxx_program
* Returned Value   : TRUE if successful
* Comments         : 
*    This function programs the prom device.
* 
*END*----------------------------------------------------------------------*/

bool _lh28fxxx_program
   (  
      /* [IN] the flash information */
      IO_FLASHX_STRUCT_PTR flashx_ptr,

      /* [IN] where to copy data from */
      char             *from_ptr,
      
      /* [OUT} where to copy data to */
      char             *to_ptr,

      /* [IN] the amount to program */
      _mem_size            bytes
   )
{ /* Body */
   bool result = TRUE;
   uint32_t count = bytes;
   uint32_t timeout;
   uint32_t tmp1;
   uint32_t tmp2;

   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, FALSE);
   }/* Endif */

   if (flashx_ptr->WIDTH == 32){
      volatile uint32_t       *base32_ptr;
      volatile uint32_t       *dest32_ptr;
      uint32_t             *src32_ptr;

      src32_ptr  = (uint32_t *)from_ptr;         
      dest32_ptr = (volatile uint32_t *)to_ptr;
      base32_ptr = (volatile uint32_t *)to_ptr;
      count = (count+3) & ~3;   /* Round count up, multiple of 4 */

      *dest32_ptr = 0xFFFFFFFF;
      /* Clear status register */
      *dest32_ptr = 0x50505050;
      *dest32_ptr = 0xFFFFFFFF;

      while (count > 0) {

         tmp1 = *src32_ptr++;
         if (tmp1 != 0xFFFFFFFF) {
            *dest32_ptr = 0x40404040;  /* Write command */
            *dest32_ptr = tmp1;    /* Write data    */
            timeout = 100;
            while (timeout) {  
              /* Read status register */
               *dest32_ptr = 0x70707070;
               tmp2 = *dest32_ptr;
               if ((tmp2 & 0x00800080) == 0x00800080) {
                 if (tmp2 & 0x3E3E3E3E) {
                     /* An write error or Vpp low detect, or write protect, or bad cmd*/
                     *base32_ptr = 0xFFFFFFFF; /* Set read mode */
                     result = FALSE;
                     goto done;
                  } /* Endif */
                  break;
               }/* Endif */
               _io_flashx_wait_us(10);
               timeout--;
            }/* Endwhile */
            if (timeout == 0) {
               result = FALSE;
               *base32_ptr = 0xFFFFFFFF; /* Set read mode */
               goto done;
            } /* Endif */
         } /* Endif */
         dest32_ptr++;
         count -= 4;
      } /* Endwhile */

      *base32_ptr = 0xFFFFFFFF; /* Set read mode */

   } else if (flashx_ptr->WIDTH == 16){
      volatile uint16_t       *base16_ptr;
      volatile uint16_t       *dest16_ptr;
      uint16_t             *src16_ptr;

      src16_ptr  = (uint16_t *)from_ptr;         
      dest16_ptr = (volatile uint16_t *)to_ptr;
      base16_ptr = (volatile uint16_t *)to_ptr;
      count = (count+1) & ~1;   /* Round count up, multiple of 4 */

      *dest16_ptr = 0xFFFF;
      /* Clear status register */
      *dest16_ptr = 0x5050;
      *dest16_ptr = 0xFFFF;

      while (count > 0) {

         tmp1 = *src16_ptr++;
         if (tmp1 != 0xFFFF) {
            *dest16_ptr = 0x4040;  /* Write command */
            *dest16_ptr = tmp1;    /* Write data    */
            timeout = 100;
            while (timeout) {  
              /* Read status register */
               *dest16_ptr = 0x7070;
               tmp2 = *dest16_ptr;
               if ((tmp2 & 0x0080) == 0x0080) {
                 if (tmp2 & 0x3E3E) {
                     /* An write error or Vpp low detect, or write protect, or bad cmd*/
                     *base16_ptr = 0xFFFF; /* Set read mode */
                     result = FALSE;
                     goto done;
                  } /* Endif */
                  break;
               }/* Endif */
               _io_flashx_wait_us(10);
               timeout--;
            }/* Endwhile */
            if (timeout == 0) {
               result = FALSE;
               *base16_ptr = 0xFFFF; /* Set read mode */
               goto done;
            } /* Endif */
         } /* Endif */
         dest16_ptr++;
         count -= 2;
      } /* Endwhile */

      *base16_ptr = 0xFFFF; /* Set read mode */

   } else if (flashx_ptr->WIDTH == 8){
      volatile uint8_t        *base8_ptr;
      volatile uint8_t        *dest8_ptr;
      uint8_t              *src8_ptr;

      src8_ptr  = (uint8_t *)from_ptr;         
      dest8_ptr = (volatile uint8_t *)to_ptr;
      base8_ptr = (volatile uint8_t *)to_ptr;

      *dest8_ptr = 0xFF;
      /* Clear status register */
      *dest8_ptr = 0x50;
      *dest8_ptr = 0xFF;

      while (count > 0) {

         tmp1 = *src8_ptr++;
         if (tmp1 != 0xFF) {
            *dest8_ptr = 0x40;  /* Write command */
            *dest8_ptr = tmp1;    /* Write data    */
            timeout = 100;
            while (timeout) {  
              /* Read status register */
               *dest8_ptr = 0x70;
               tmp2 = *dest8_ptr;
               if ((tmp2 & 0x80) == 0x80) {
                 if (tmp2 & 0x3E) {
                     /* An write error or Vpp low detect, or write protect, or bad cmd*/
                     *base8_ptr = 0xFF; /* Set read mode */
                     result = FALSE;
                     goto done;
                  } /* Endif */
                  break;
               }/* Endif */
               _io_flashx_wait_us(10);
               timeout--;
            }/* Endwhile */
            if (timeout == 0) {
               result = FALSE;
               *base8_ptr = 0xFF; /* Set read mode */
               goto done;
            } /* Endif */
         } /* Endif */
         dest8_ptr++;
         count -= 1;
      } /* Endwhile */
      
      *base8_ptr = 0xFF; /* Set read mode */

   }/* Endif */

   if (flashx_ptr->WRITE_VERIFY) {
      /* Verify sector(s) is(are) written correctly */
      uint32_t  *src32_ptr  = (uint32_t *)from_ptr;
      uint32_t  *dest32_ptr =  (uint32_t *)to_ptr;
      count = bytes/4 + 1;
      while (--count) {
         if (*src32_ptr++ != *dest32_ptr++) {
            result = FALSE;
            break;
         } /* Endif */
      } /* Endwhile */
   } /* Endif */   

done:
   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, TRUE);
   }/* Endif */
   return(result);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _lh28fxxx_erase
* Returned Value   : TRUE if successful
* Comments         : 
*    This function erases the prom device.
* 
*END*----------------------------------------------------------------------*/

bool _lh28fxxx_erase
   (
      /* [IN] the flash information */
      IO_FLASHX_STRUCT_PTR flashx_ptr,

      /* [IN] the sector to erase */
      char             *input_sect_ptr,

      /* [IN] the number of bytes to erase */
      _mem_size            bytes
   )
{ /* Body */
   bool result = TRUE;
   uint32_t timeout;
   uint32_t tmp1;

   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, FALSE);
   }/* Endif */

   if (flashx_ptr->WIDTH == 32){
      volatile uint32_t       *base32_ptr;
   
      base32_ptr = (volatile uint32_t *)input_sect_ptr;

      *base32_ptr = 0xFFFFFFFF;
      /* Clear status register */
      *base32_ptr = 0x50505050;
      *base32_ptr = 0xFFFFFFFF;

      /* Start block erase */
      *base32_ptr = 0x20202020;
      *base32_ptr = 0xD0D0D0D0;

      timeout = 7000000;
      while (timeout) {
         /* Read status register */
         *base32_ptr = 0x70707070;
         tmp1 = *base32_ptr;
         if ((tmp1 & 0x00800080) == 0x00800080) {
            if (tmp1 & 0x3E3E3E3E) {
               /* An write error or Vpp low detect, or write protect, or bad cmd */
               result = FALSE;
               break;
            }/* Endif */
            /* Done! */
            break;
         } /* Endif */
         _io_flashx_wait_us(10);
         timeout--;
      } /* Endwhile */
      if (timeout == 0) {
         result = FALSE;
      } /* Endif */
      
      /* Reset the device */
      *base32_ptr = 0xFFFFFFFF;

   } else if (flashx_ptr->WIDTH == 16){
      volatile uint16_t       *base16_ptr;
   
      base16_ptr = (volatile uint16_t *)input_sect_ptr;

      *base16_ptr = 0xFFFF;
      /* Clear status register */
      *base16_ptr = 0x5050;
      *base16_ptr = 0xFFFF;

      /* Start block erase */
      *base16_ptr = 0x2020;
      *base16_ptr = 0xD0D0;

      timeout = 7000000;
      while (timeout) {
         /* Read status register */
         *base16_ptr = 0x7070;
         tmp1 = *base16_ptr;
         if ((tmp1 & 0x0080) == 0x0080) {
            if (tmp1 & 0x3E3E) {
               /* An write error or Vpp low detect, or write protect, or bad cmd */
               result = FALSE;
               break;
            }/* Endif */
            /* Done! */
            break;
         } /* Endif */
         _io_flashx_wait_us(10);
         timeout--;
      } /* Endwhile */
      if (timeout == 0) {
         result = FALSE;
      } /* Endif */

      /* Reset the device */
      *base16_ptr = 0xFFFF;
   
   } else if (flashx_ptr->WIDTH == 8){
      volatile uint8_t       *base8_ptr;
   
      base8_ptr = (volatile uint8_t *)input_sect_ptr;

      *base8_ptr = 0xFF;
      /* Clear status register */
      *base8_ptr = 0x50;
      *base8_ptr = 0xFF;

      /* Start block erase */
      *base8_ptr = 0x20;
      *base8_ptr = 0xD0;

      timeout = 7000000;
      while (timeout) {
         /* Read status register */
         *base8_ptr = 0x70;
         tmp1 = *base8_ptr;
         if ((tmp1 & 0x80) == 0x80) {
            if (tmp1 & 0x3E) {
               /* An write error or Vpp low detect, or write protect, or bad cmd */
               result = FALSE;
               break;
            }/* Endif */
            /* Done! */
            break;
         } /* Endif */
         _io_flashx_wait_us(10);
         timeout--;
      } /* Endwhile */
      if (timeout == 0) {
         result = FALSE;
      } /* Endif */

      /* Reset the device */
      *base8_ptr = 0xFF;

   } /* Endif */

   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, TRUE);
   }/* Endif */
   return(result);

} /* Endbody */

/* EOF */
