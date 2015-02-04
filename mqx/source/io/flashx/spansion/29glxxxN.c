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
*   The file contains functions to program the Spansion
*   S29GL128N flash devices.
*
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "flashx.h"
#include "flashxprv.h"
#include "29glxxxN.h"
#include "29glxxxN_prv.h"
                                                      
#define FLASH_READY 1
#define FLASH_ERASE_SUSPENDED 2
#define FLASH_TIMEOUT 3
#define FLASH_BUSY 4
#define FLASH_ERROR 5

extern uint32_t _29glxxxN_status(IO_FLASHX_STRUCT_PTR, void *);


const FLASHX_DEVICE_IF_STRUCT _flashx_29glxxxN_if = {
    _29glxxxN_erase,
    _29glxxxN_program,
    _29glxxxN_chip_erase,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29glxxxN_program
* Returned Value   : TRUE if successful
* Comments         : 
*    This function programs the prom devices listed below:
*       29GL128N, 29GL256N and 29GL512N
* 
*END*----------------------------------------------------------------------*/

bool _29glxxxN_program
   (  
      /* [IN] the flash information structure */
      IO_FLASHX_STRUCT_PTR flashx_ptr,

      /* [IN] where to copy data from */
      char             *from_ptr,
      
      /* [OUT] where to copy data to */
      char             *to_ptr,

      /* [IN] the size to copy in bytes */
      _mem_size            size
   )
{ /* Body */
   volatile uint16_t       *base16_ptr;
   volatile uint16_t       *dest16_ptr;
   uint16_t             *src16_ptr;
   volatile unsigned char         *base8_ptr;
   volatile unsigned char         *dest8_ptr;
   unsigned char               *src8_ptr;
   uint32_t                 tmp1;
   uint32_t                 result = FLASH_READY;
   _mem_size               sector_addr;

   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, FALSE);
   }/* Endif */

   _io_flashx_phys_to_sector(flashx_ptr, (_mem_size)to_ptr, NULL, &sector_addr, NULL);

   if (flashx_ptr->WIDTH == 16) {
      src16_ptr  = (uint16_t *)from_ptr;         
      dest16_ptr = (volatile uint16_t *)to_ptr;
      base16_ptr = (volatile uint16_t *)sector_addr;
      while (size) {
         tmp1 = (uint32_t)*src16_ptr++;
         if (tmp1 != 0xFFFF) {
            _int_disable();
            base16_ptr[0x555] = 0xAAAA;
            base16_ptr[0x2AA] = 0x5555;
            base16_ptr[0x555] = 0xA0A0;        /* Write command */            
            *dest16_ptr       = (uint16_t)tmp1; /* write value */
           _int_enable();
            while ((result = _29glxxxN_status(flashx_ptr,
               (void *)dest16_ptr)) == FLASH_BUSY)
            {
            }/* Endwhile */
            if (result != FLASH_READY) {
               break;
            } /* Endif */
         }/* Endif */
         size -= 2;
         dest16_ptr++;
      }/* Endwhile */
      base16_ptr[0] = 0xF0F0;

   } else if (flashx_ptr->WIDTH == 8) {
      src8_ptr  = (unsigned char *)from_ptr;         
      dest8_ptr = (volatile unsigned char *)to_ptr;
      base8_ptr = (volatile unsigned char *)sector_addr;
      while (size) {
         tmp1 = (uint32_t)*src8_ptr++;
         if (tmp1 != 0xFF) {
            _int_disable();
            base8_ptr[0xAAA] = 0xAA;
            base8_ptr[0x555] = 0x55;
            base8_ptr[0xAAA] = 0xA0;        /* Write command */           
            *dest8_ptr       = (unsigned char)tmp1; /* write value */
           _int_enable();
            while ((result = _29glxxxN_status(flashx_ptr,
               (void *)dest8_ptr)) == FLASH_BUSY)
            {
            }/* Endwhile */
            if (result != FLASH_READY) {
               break;
            } /* Endif */
         }/* Endif */
         size -= 1;
         dest8_ptr++;
      }/* Endwhile */
      base8_ptr[0] = 0xF0;

   } /* Endif */
   
   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, TRUE);
   }/* Endif */

   return result == FLASH_READY;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29glxxxN_erase
* Returned Value   : TRUE if successful
* Comments         : 
*    This function erases (sector) the prom devices listed below:
*       29GL128N, 29GL256N and 29GL512N
* 
*END*----------------------------------------------------------------------*/

bool _29glxxxN_erase
   (
      /* [IN] the flash information structure */
      IO_FLASHX_STRUCT_PTR flashx_ptr,

      /* [IN] the sector to erase */
      char            *input_sect_ptr,

      /* [IN] the sector size */
      _mem_size            sector_size
   )
{ /* Body */
   volatile uint16_t      *sect16_ptr;
   volatile unsigned char        *sect8_ptr;
   uint32_t                result;

   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, FALSE);
   }/* Endif */

   if (flashx_ptr->WIDTH == 16) {
      sect16_ptr = (volatile uint16_t *)input_sect_ptr;
      _int_disable();     
      sect16_ptr[0x555] = 0xAAAA;
      sect16_ptr[0x2AA] = 0x5555;
      sect16_ptr[0x555] = 0x8080;
      sect16_ptr[0x555] = 0xAAAA;
      sect16_ptr[0x2AA] = 0x5555;
      sect16_ptr[0]     = 0x3030;    /* Sector erase */
      _int_enable();
      while ((result = _29glxxxN_status(flashx_ptr, (void *)sect16_ptr))
         == FLASH_BUSY)
      {
      }/* Endwhile */
      sect16_ptr[0] = 0xF0F0;

   } else if (flashx_ptr->WIDTH == 8) {
      sect8_ptr = (volatile unsigned char *)input_sect_ptr;
      _int_disable();
      sect8_ptr[0xAAA] = 0xAA;
      sect8_ptr[0x555] = 0x55;
      sect8_ptr[0xAAA] = 0x80;
      sect8_ptr[0xAAA] = 0xAA;
      sect8_ptr[0x555] = 0x55;
      sect8_ptr[0]     = 0x30;    /* Sector erase */
      _int_enable();
      while ((result = _29glxxxN_status(flashx_ptr, (void *)sect8_ptr))
         == FLASH_BUSY)
      {
      }/* Endwhile */
      sect8_ptr[0] = 0xF0;

   }/* Endif */

   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, TRUE);
   }/* Endif */

   return result == FLASH_READY;
   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29glxxxN_chip_erase
* Returned Value   : TRUE if successful
* Comments         : 
*    This function erases all the prom devices. listed below:
*       29GL128N, 29GL256N and 29GL512N
* 
*END*----------------------------------------------------------------------*/

bool _29glxxxN_chip_erase
   (
      /* [IN] the flash information structure */
      IO_FLASHX_STRUCT_PTR flashx_ptr
   )
{ /* Body */
   volatile uint16_t      *sect16_ptr;
   volatile unsigned char        *sect8_ptr;
   uint32_t                result;

   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, FALSE);
   }/* Endif */

   if (flashx_ptr->WIDTH == 16) {
      sect16_ptr = (volatile uint16_t *)flashx_ptr->BASE_ADDR;
      _int_disable();
      sect16_ptr[0xAAA] = 0xAAAA;
      sect16_ptr[0x555] = 0x5555;
      sect16_ptr[0xAAA] = 0x8080;
      sect16_ptr[0xAAA] = 0xAAAA;
      sect16_ptr[0x555] = 0x5555;
      sect16_ptr[0xAAA] = 0x1010;    /* chip erase */
      _int_enable();
      while ((result = _29glxxxN_status(flashx_ptr, (void *)sect16_ptr))
         == FLASH_BUSY)
      {
      }/* Endwhile */
      sect16_ptr[0] = 0xF0F0;

   } else if (flashx_ptr->WIDTH == 8) {
      sect8_ptr = (volatile unsigned char *)flashx_ptr->BASE_ADDR;
      _int_disable();
      sect8_ptr[0xAAA] = 0xAA;
      sect8_ptr[0x555] = 0x55;
      sect8_ptr[0xAAA] = 0x80;
      sect8_ptr[0xAAA] = 0xAA;
      sect8_ptr[0x555] = 0x55;
      sect8_ptr[0xAAA] = 0x10;    /* chip erase */
      _int_enable();
      while ((result = _29glxxxN_status(flashx_ptr, (void *)sect8_ptr))
         == FLASH_BUSY)
      {
      }/* Endwhile */
      sect8_ptr[0] = 0xF0;

   }/* Endif */

   if (flashx_ptr->WRITE_PROTECT) {
      (*flashx_ptr->WRITE_PROTECT)(flashx_ptr, TRUE);
   }/* Endif */

   return result == FLASH_READY;
   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29glxxxN_status
* Returned Value   : uint32_t current status of operation
* Comments         : 
*
*END*----------------------------------------------------------------------*/

uint32_t _29glxxxN_status
   (
      /* [IN] the flash information structure */
      IO_FLASHX_STRUCT_PTR flashx_ptr,
      
      /* [IN] address */
      void                *addr
   )
{ /* Body */
   volatile uint16_t      *sect16_ptr;
   volatile unsigned char        *sect8_ptr;
   uint32_t                tmp1;
   uint32_t                tmp2;
   uint32_t                toggle;
   uint32_t                retry = 2;
   
   sect16_ptr = (volatile uint16_t *)addr;
   sect8_ptr  = (volatile unsigned char *)addr;

   while (retry--) {
      if (flashx_ptr->WIDTH == 16) {
         tmp1 = *sect16_ptr;
         tmp2 = *sect16_ptr;
         toggle = tmp1 ^ tmp2;   /* See what bits changed */
      } else if (flashx_ptr->WIDTH == 8) {
         tmp1 = *sect8_ptr;
         tmp2 = *sect8_ptr;
         toggle = tmp1 ^ tmp2;   /* See what bits changed */
      }/* Endif */
   
      if (toggle == 0) {
        return FLASH_READY;
      } /* Endif */

      if (toggle & 0x40) {
         if (tmp1 & 0x20) {
            if (retry) {
               continue;
            }/* Endif */
            return FLASH_TIMEOUT;
         }/* Endif */
         return FLASH_BUSY;
      } else {
         if ((toggle & 0x04) && (tmp1 & 0x80)) {
            if (retry) {
               continue;
            }/* Endif */
            return FLASH_ERASE_SUSPENDED;
         } /* Endif */
      }/* Endif */

   } /* Endwhile */

   return FLASH_ERROR;
   
} /* Endbody */
      
/* EOF */
