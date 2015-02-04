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
*   The file contains functions to program Intel's StrataFlash 
*   devices
*   While this driver is generic to Intel's StrataFlash family of
*   products it has only been tested on the following parts:
*   28F128J3A
*
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "flashx.h"
#include "flashxprv.h"
#include "istrata.h"
#include "istrataprv.h"

#define LOCK    1
#define UNLOCK  0
#define BYTES_IN_WORD    2
#define BYTES_IN_DWORD   4

const FLASHX_DEVICE_IF_STRUCT _intel_strata_if = {
    _intel_strata_erase,
    _intel_strata_program,
    NULL,
    NULL,
    _intel_strata_init,
    NULL,
    _intel_strata_write_protect,
    _io_intel_strata_ioctl
};

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_program
* Returned Value   : TRUE if successful
* Comments         : 
*    This function programs a sector of flash
* 
*END*----------------------------------------------------------------------*/

bool _intel_strata_program
   (  
      /* [IN] device handle */
      IO_FLASHX_STRUCT_PTR  dev_ptr,

      /* [IN] where to copy data from */
      char              *from_ptr,
      
      /* [OUT} where to copy data to */
      char              *to_ptr,

      /* [IN] the sector size to copy */
      _mem_size             sector_size
   )
{ /* Body */
   _mqx_uint   total_width;
   _mqx_uint   i = 0;
   bool     program = FALSE;
   bool     result;
   _mqx_uint   write_protect = TRUE;
   _mqx_uint_ptr write_protect_ptr = &write_protect;
   uint32_t     temp_data = 0xFFFFFFFF;
   uint8_t  *temp_data_ptr;
   uint8_t      byte_data_counter = 0;
   
   temp_data_ptr = (uint8_t *)&temp_data;
   
   total_width = dev_ptr->DEVICES * dev_ptr->WIDTH;

   /* First make sure we actually need to program the sector */
   if (total_width == 1) {
      uint8_t  *from8_ptr = (uint8_t *)((void *)from_ptr);
      uint8_t  *to8_ptr   = (uint8_t *)((void *)to_ptr);

      while( !program && (i < sector_size) ) {
         program = *from8_ptr++ != *to8_ptr++;
         i++;
      } /* Endwhile */

      i--;
   } else if (total_width == 2) {
      uint16_t  *from16_ptr = (uint16_t *)((void *)from_ptr);
      uint16_t  *to16_ptr   = (uint16_t *)((void *)to_ptr);

      while( !program && (i < sector_size) ) {
         program = *from16_ptr++ != *to16_ptr++;
         i += 2;
      } /* Endwhile */

      i -= 2;
   } else if (total_width == 4) {
      uint32_t  *from32_ptr = (uint32_t *)((void *)from_ptr);
      uint32_t  *to32_ptr   = (uint32_t *)((void *)to_ptr);

      while( !program && (i < sector_size) ) {
         program = *from32_ptr++ != *to32_ptr++;
         i += 4;
      } /* Endwhile */

      i -= 4;
   } else {
      /* Unsupported configuration */
      return FALSE;
   } /* Endif */

   if (!program) {
      return !program;
   } /* Endif */

   /* get the flash write protect status */
   _io_intel_strata_ioctl(dev_ptr, FLASH_IOCTL_GET_WRITE_PROTECT, write_protect_ptr); 
   /* if the flash is write protect, return */
   if(write_protect){
      return(FALSE);
   }

   switch (dev_ptr->WIDTH) {
      case 1:
         result = _intel_strata_program_1byte(dev_ptr, from_ptr, to_ptr, sector_size, i);
         break;
      case 2:
         if((uint32_t)to_ptr & 0x01){
            to_ptr -= 0x01;
            byte_data_counter = 0x01;
            *temp_data_ptr = *to_ptr;
         
            while(sector_size) {            
               while( byte_data_counter < BYTES_IN_WORD && sector_size ){
                  *(unsigned char *)(temp_data_ptr+byte_data_counter) = *from_ptr++;
                  byte_data_counter++;
                  sector_size--;
               }
               byte_data_counter = 0;
               
               result = _intel_strata_program_2byte(dev_ptr, (uint16_t *)temp_data_ptr,
                  (uint16_t *)to_ptr, 2, i);
               if(FALSE == result)
                  return result;
               if(i >> 1)
                  i += 2;
               temp_data = 0xFFFFFFFF;
               to_ptr += 2;
            }
         }
         else if(sector_size & 0x01){
            sector_size++;
            result = _intel_strata_program_2byte(dev_ptr, (uint16_t *)from_ptr,
                  (uint16_t *)to_ptr, sector_size, i);
         }
         else
            result = _intel_strata_program_2byte(dev_ptr, (uint16_t *)from_ptr,
               (uint16_t *)to_ptr, sector_size, i);
         break;
      case 4:
         if((uint32_t)to_ptr & 0x03){
            to_ptr -= (uint32_t)to_ptr & 0x03;
            byte_data_counter = (uint32_t)to_ptr & 0x03;
            *temp_data_ptr = *to_ptr;
         
            while(sector_size) {            
               while( byte_data_counter < BYTES_IN_DWORD && sector_size ){
                  *(unsigned char *)(temp_data_ptr+byte_data_counter) = *from_ptr++;
                  byte_data_counter++;
                  sector_size--;
               }
               byte_data_counter = 0;
               
               result = _intel_strata_program_4byte(dev_ptr, (uint32_t *)temp_data_ptr,
                  (uint32_t *)to_ptr, 4, i);
               if(FALSE == result)
                  return result;
               if(i >> 1)
                  i += 4;
               temp_data = 0xFFFFFFFF;
               to_ptr += 4;
            }
         }
         else if(sector_size & 0x01){
            sector_size++;
            result = _intel_strata_program_4byte(dev_ptr, (uint32_t *)from_ptr,
                  (uint32_t *)to_ptr, sector_size, i);
         }
         else
            result = _intel_strata_program_4byte(dev_ptr, (uint32_t *)from_ptr,
               (uint32_t *)to_ptr, sector_size, i);
         break;
   } /* Endswitch */

   return result;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_erase
* Returned Value   : TRUE if successful
* Comments         : 
*    This function erases a sector of flash
* 
*END*----------------------------------------------------------------------*/

bool _intel_strata_erase
   (  
      /* [IN] device info */
      IO_FLASHX_STRUCT_PTR  dev_ptr,

      /* [IN] the sector to erase */
      char             *input_sect_ptr,
      
      /* [IN] the number of bytes to erase */
      _mem_size            bytes
   )
{ /* Body */
   _mqx_uint  total_width;
   _mqx_uint  i = 0;
   bool    result = TRUE;
   bool    erase = FALSE;
   MQX_TICK_STRUCT  tmp_ticks;
   _mqx_uint  write_protect = TRUE;
   _mqx_uint_ptr write_protect_ptr = &write_protect;

#if 0   
   if (dev_ptr->WRITE_PROTECT) {
      (*dev_ptr->WRITE_PROTECT)(dev_ptr, FALSE);
   }/* Endif */
#endif

   /* get the flash write protect status */
   _io_intel_strata_ioctl(dev_ptr, FLASH_IOCTL_GET_WRITE_PROTECT, write_protect_ptr); 
   /* if the flash is write protect, return */
   if(write_protect){
      return(FALSE);
   }

   total_width = dev_ptr->DEVICES * dev_ptr->WIDTH;

   /* First make sure we actually need to erase the sector */
   if (total_width == 1) {
      uint8_t  *to8_ptr   = (uint8_t *)((void *)input_sect_ptr);

      while( !erase && (i < bytes) ) {
         erase = 0xff != *to8_ptr++;
         i++;
      } /* Endwhile */

   } else if (total_width == 2) {
      uint16_t  *to16_ptr   = (uint16_t *)((void *)input_sect_ptr);

      while( !erase && (i < bytes) ) {
         erase = 0xffff != *to16_ptr++;
         i += 2;
      } /* Endwhile */

   } else if (total_width == 4) {
      uint32_t  *to32_ptr   = (uint32_t *)((void *)input_sect_ptr);

      while( !erase && (i < bytes) ) {
         erase = 0xffffffff != *to32_ptr++;
         i += 4;
      } /* Endwhile */

   } else {
      /* Unsupported configuration */
      return FALSE;
   } /* Endif */

   if (!erase) {
      return !erase;
   } /* Endif */

   switch (dev_ptr->WIDTH) {
      case 1:
         result = _intel_strata_erase_1byte(dev_ptr, input_sect_ptr, 
            bytes, &tmp_ticks);
         break;
      case 2:
         result = _intel_strata_erase_2byte(dev_ptr, (uint16_t *)input_sect_ptr,
            bytes, &tmp_ticks);
         break;
      case 4:
         result = _intel_strata_erase_4byte(dev_ptr, (uint32_t *)input_sect_ptr,
            bytes, &tmp_ticks);
         break;
   } /* Endswitch */

   return result;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_check_timeout
* Returned Value   : TRUE if timeout occurs
* Comments         : 
* 
*END*----------------------------------------------------------------------*/

bool _intel_strata_check_timeout
   (  
      /* [IN] Time operation started in ticks */
      MQX_TICK_STRUCT_PTR   start_tick_ptr,

      /* [IN] The number of ticks the operation is expected to take */
      _mqx_uint             period
   )
{ /* Body */
   MQX_TICK_STRUCT  end_ticks;
   MQX_TICK_STRUCT  current_ticks;
   _mqx_int         result;

   PSP_ADD_TICKS_TO_TICK_STRUCT(start_tick_ptr, period, &end_ticks);
   _time_get_elapsed_ticks(&current_ticks);

   result = PSP_CMP_TICKS(&current_ticks, &end_ticks);

   if (result >= 0) {
      return TRUE;
   } else {
      return FALSE;
   } /* Endif */

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_test_lock_bits
* Returned Value   : TRUE if locked
* Comments         : 
*
*   This function test, if the flash is locked  
*END*----------------------------------------------------------------------*/
static bool _intel_strata_test_lock_bits
    (
        /* [IN] device handle */
      IO_FLASHX_STRUCT_PTR  dev_ptr
    ) 
{
    uint32_t block_size;
    uint32_t num_blocks;
    volatile unsigned char         *dest_ptr;
    int i = 0;
    unsigned char   status;
    
    num_blocks = dev_ptr->HW_BLOCK->NUM_SECTORS; /* sector == block */
    block_size = dev_ptr->HW_BLOCK->SECTOR_SIZE;
    /* test all sectors for lock */ 
    for(i = 0; i < num_blocks; i++) {
        /* set block address */
        dest_ptr = (volatile unsigned char *)(dev_ptr->BASE_ADDR + dev_ptr->HW_BLOCK[0].START_ADDR + i * block_size);
        /* return in status block-bit value 0- unlock, 1- locked */ 
        ISTRATA_READ_LOCK_STATUS(ISTRATA_READ_MODE_CMD_8X1, ISTRATA_READ_ID_MODE_CMD_16X1, ISTRATA_STATUS_BUSY_8X1);
        /* is some sector locked? */
        if(status & 0x01){
            /* yes, some sector is locked */
            return TRUE;
        }
    }
    return FALSE;
}
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_write_protect
* Returned Value   : TRUE if successful
* Comments         : 
*    This function sets or clears the lock bits on all sectors
*    lock = 1 - device is to be write protect, 0 - allow writing device
* 
*END*----------------------------------------------------------------------*/
bool _intel_strata_write_protect
    (
        /* [IN] the device handle */
      IO_FLASHX_STRUCT_PTR  dev_ptr,
      
      /* [IN]  */
      _mqx_uint             lock
    ) 
{
    bool result = TRUE;
    
    /* set the lock bits */
    if(lock == LOCK){
        result = _intel_strata_set_lock_bits(dev_ptr);   
    }
    /* clear lock bits */   
    else{
        /* is the flash locked? */
        if(_intel_strata_test_lock_bits(dev_ptr)){
            /* unlock */
            result = _intel_strata_clear_lock_bits(dev_ptr);
        }
    }
    return(result);
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_clear_lock_bits
* Returned Value   : TRUE if successful
* Comments         : 
*    This function clears the lock bits on all sectors
* 
*END*----------------------------------------------------------------------*/

bool _intel_strata_clear_lock_bits
   (  
      /* [IN] the device handle */
      IO_FLASHX_STRUCT_PTR  dev_ptr
   )
{ /* Body */
   bool          result;
   MQX_TICK_STRUCT  start_ticks;

   switch (dev_ptr->WIDTH) {
      case 1:
         result = _intel_strata_clearlockbits_1byte(dev_ptr, &start_ticks);
         break;
      case 2:
         result = _intel_strata_clearlockbits_2byte(dev_ptr, &start_ticks);
         break;
      case 4:
         result = _intel_strata_clearlockbits_4byte(dev_ptr, &start_ticks);
         break;
   } /* Endswitch */

   return result;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_set_lock_bits
* Returned Value   : TRUE if successful
* Comments         : 
*    This function sets the lock bits on all sectors
* 
*END*----------------------------------------------------------------------*/

bool _intel_strata_set_lock_bits
   (  
      /* [IN] the device handle */
      IO_FLASHX_STRUCT_PTR  dev_ptr
   )
{ /* Body */
   bool          result;
   MQX_TICK_STRUCT  start_ticks;

   switch (dev_ptr->WIDTH) {
      case 1:
         result = _intel_strata_setlockbits_1byte(dev_ptr, &start_ticks);
         break;
      case 2:
         result = _intel_strata_setlockbits_2byte(dev_ptr, &start_ticks);
         break;
      case 4:
         result = _intel_strata_setlockbits_4byte(dev_ptr, &start_ticks);
         break;
   } /* Endswitch */

   return result;

} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_intel_strata_ioctl
* Returned Value   : TRUE if successful
* Comments         : 
*    flash ioctl function
* 
*END*----------------------------------------------------------------------*/
_mqx_int _io_intel_strata_ioctl
   (
      /* [IN] the handle returned from _fopen */
      IO_FLASHX_STRUCT_PTR   dev_ptr,

      /* [IN] the ioctl command */
      _mqx_uint              cmd,

      /* [IN] the ioctl parameters */
      void                  *param_ptr
   )
{ /* Body */
    _mqx_uint                       result = MQX_OK;
    switch(cmd) {
        case FLASH_IOCTL_GET_WRITE_PROTECT:
            /* is the flash locked ? */
            if (_intel_strata_test_lock_bits(dev_ptr)) {
                *((bool *)param_ptr) = TRUE;
            } else {
                *((bool *)param_ptr) = FALSE;
            }
            break;
        default:
            break;
    }
    return result;
}


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_init
* Returned Value   : TRUE if successful
* Comments         : 
*    This function initializes intel strata flash
* 
*END*----------------------------------------------------------------------*/
bool _intel_strata_init
(
    /* [IN] File pointer */
    IO_FLASHX_STRUCT_PTR dev_ptr
)
{
    if (_intel_strata_test_lock_bits(dev_ptr))
        return _intel_strata_clear_lock_bits(dev_ptr);

    return TRUE;
}

/* EOF */
