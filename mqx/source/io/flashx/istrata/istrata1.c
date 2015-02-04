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
*
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "flashx.h"
#include "flashxprv.h"
#include "istrata.h"
#include "istrataprv.h"

                                          
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_program_1byte
* Returned Value   : TRUE if successful
* Comments         : 
*    This function programs a sector of flash for 8 bit devices
* 
*END*----------------------------------------------------------------------*/

bool _intel_strata_program_1byte
   (  
      /* [IN] the base address of the device */
      IO_FLASHX_STRUCT_PTR  handle_ptr,

      /* [IN] where to copy data from */
      char              *from_ptr,
      
      /* [OUT} where to copy data to */
      char              *to_ptr,

      /* [IN] the sector size to copy */
      _mem_size             sector_size,

      /* [IN] the offset to start at */
      _mem_size             offset
   )
{ /* Body */
   bool                 erase;
   bool                 success;
   bool                 timeout;
   _mqx_uint               j;
   MQX_TICK_STRUCT         start_ticks;

   /* First make sure we actually need to program the sector */
   erase = FALSE;

   for (j = offset; !erase && (j < sector_size); j++) {
      erase = ISTRATA_ERASE_UNIT(to_ptr[j], from_ptr[j]);
   } /* Endfor */

   if (erase) {
      success = _intel_strata_erase_1byte(handle_ptr, to_ptr, 
         sector_size, &start_ticks);
      if (!success) {
         return success;
      } /* Endif */
      /* 
      ** We erased the whole sector so we must program from the beginning of
      ** the sector
      */
      offset = 0;
   } else {
      success = TRUE;
   } /* Endif */

   if (handle_ptr->DEVICES == 1) {
      volatile unsigned char       *dest_ptr = (volatile unsigned char *)to_ptr;
      unsigned char                *src_ptr  = (unsigned char *)from_ptr;
      _mem_size             size = sector_size;
      unsigned char                 tmp, status;

      dest_ptr = dest_ptr + offset;
      src_ptr  = src_ptr  + offset;

      ISTRATA_PROGRAM(ISTRATA_READ_MODE_CMD_8X1, ISTRATA_WRITE_CMD_8X1, 
         ISTRATA_STATUS_BUSY_8X1);
   } else if (handle_ptr->DEVICES == 2) {
      volatile uint16_t       *dest_ptr = (volatile uint16_t *)((void *)to_ptr);
      uint16_t                *src_ptr  = (uint16_t *)((void *)from_ptr);
      _mem_size               size;
      uint16_t                 tmp, status;

      size = sector_size >> 1;
      offset >>= 1;
      dest_ptr = dest_ptr + offset;
      src_ptr  = src_ptr  + offset;

      ISTRATA_PROGRAM(ISTRATA_READ_MODE_CMD_8X2, ISTRATA_WRITE_CMD_8X2, 
         ISTRATA_STATUS_BUSY_8X2);
   } else if (handle_ptr->DEVICES == 4) {
      volatile uint32_t       *dest_ptr = (volatile uint32_t *)((void *)to_ptr);
      uint32_t                *src_ptr  = (uint32_t *)((void *)from_ptr);
      _mem_size               size;
      uint32_t                 tmp, status;

      size = sector_size >> 2;
      offset >>= 2;
      dest_ptr = dest_ptr + offset;
      src_ptr  = src_ptr  + offset;

      ISTRATA_PROGRAM(ISTRATA_READ_MODE_CMD_8X4, ISTRATA_WRITE_CMD_8X4, 
         ISTRATA_STATUS_BUSY_8X4);
   } else {
      /* Unsupported configuration */
      return FALSE;
   } /* Endif */

   if (success) {
      /* Verify sector is written correctly */
      for( j = 0; j < sector_size; j++ ) {
         if ( from_ptr[j] != to_ptr[j] ) {
            success = FALSE;
            break;
         } /* Endif */
      } /* Endfor */
   } /* Endif */

   return success;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_erase_1byte
* Returned Value   : TRUE if successful
* Comments         : 
*    This function erases a sector
* 
*END*----------------------------------------------------------------------*/

bool _intel_strata_erase_1byte
   (
      /* [IN] the base address of the device */
      IO_FLASHX_STRUCT_PTR handle_ptr,

      /* [IN] the sector to erase */
      char             *input_sect_ptr,

      /* [IN] the size of the sector */
      _mem_size            sect_size,

      /* [IN/OUT] place to store start time */
      MQX_TICK_STRUCT_PTR  start_ticks_ptr
   )
{ /* Body */
   bool timeout = FALSE;
   bool success = TRUE;

   if (handle_ptr->DEVICES == 1) {
      volatile unsigned char         *dest_ptr = (volatile unsigned char *)input_sect_ptr;
      unsigned char                   status;

      ISTRATA_ERASE(ISTRATA_READ_MODE_CMD_8X1, ISTRATA_ERASE_CMD1_8X1, 
         ISTRATA_ERASE_CMD2_8X1, ISTRATA_STATUS_BUSY_8X1, 0xFF);

   } else if (handle_ptr->DEVICES == 2) {
      volatile uint16_t       *dest_ptr = (volatile uint16_t *)input_sect_ptr;
      uint16_t                 status;

      ISTRATA_ERASE(ISTRATA_READ_MODE_CMD_8X2, ISTRATA_ERASE_CMD1_8X2, 
         ISTRATA_ERASE_CMD2_8X2, ISTRATA_STATUS_BUSY_8X2, 0xFFFF);

   } else if (handle_ptr->DEVICES == 4) {
      volatile uint32_t       *dest_ptr = (volatile uint32_t *)input_sect_ptr;
      uint32_t                 status;

      ISTRATA_ERASE(ISTRATA_READ_MODE_CMD_8X4, ISTRATA_ERASE_CMD1_8X4, 
         ISTRATA_ERASE_CMD2_8X4, ISTRATA_STATUS_BUSY_8X4, 0xFFFFFFFF);

   } else {
      success = FALSE;
   } /* Endif */

   return success;

} /* Endbody */


/* Start CR 871 */
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_clearlockbits_1byte
* Returned Value   : TRUE if successful
* Comments         : 
*    This function unlocks all sectors
* 
*END*----------------------------------------------------------------------*/

bool _intel_strata_clearlockbits_1byte
   (
      /* [IN] the base address of the device */
      IO_FLASHX_STRUCT_PTR handle_ptr,

      /* [IN/OUT] place to store start time */
      MQX_TICK_STRUCT_PTR  start_ticks_ptr
   )
{ /* Body */
   uint32_t block_size;
   uint32_t num_blocks;
   uint32_t i;
   bool timeout = FALSE;
   bool success = TRUE;

   num_blocks = handle_ptr->HW_BLOCK->NUM_SECTORS; /* sector == block */
   block_size = handle_ptr->HW_BLOCK->SECTOR_SIZE;

   /*
   ** All blocks default to the locked state after initial power-up or reset.
   ** The blocks are unlocked individually.
   */
   if (handle_ptr->DEVICES == 1) {
      volatile unsigned char         *dest_ptr;
      unsigned char                   status;

      for(i = 0; i < num_blocks; i++) {
         dest_ptr = (volatile unsigned char *)(handle_ptr->BASE_ADDR + handle_ptr->HW_BLOCK[0].START_ADDR + i * block_size);
         
         ISTRATA_CLEAR_LOCKBITS(ISTRATA_READ_MODE_CMD_8X1, ISTRATA_CLEAR_CMD1_8X1, 
            ISTRATA_CLEAR_CMD2_8X1, ISTRATA_STATUS_BUSY_8X1);
      } /* Endfor */
      
   } else if (handle_ptr->DEVICES == 2) {
      volatile uint16_t       *dest_ptr;
      uint16_t                 status;

      for(i = 0; i < num_blocks; i++) {
         dest_ptr = (volatile uint16_t *)(handle_ptr->BASE_ADDR + handle_ptr->HW_BLOCK[0].START_ADDR + i * block_size);
         
         ISTRATA_CLEAR_LOCKBITS(ISTRATA_READ_MODE_CMD_8X2, ISTRATA_CLEAR_CMD1_8X2, 
            ISTRATA_CLEAR_CMD2_8X2, ISTRATA_STATUS_BUSY_8X2);
      } /* Endfor */
      
   } else if (handle_ptr->DEVICES == 4) {
      volatile uint32_t       *dest_ptr;
      uint32_t                 status;

      for(i = 0; i < num_blocks; i++) {
         dest_ptr = (volatile uint32_t *)(handle_ptr->BASE_ADDR + handle_ptr->HW_BLOCK[0].START_ADDR + i * block_size);
         
         ISTRATA_CLEAR_LOCKBITS(ISTRATA_READ_MODE_CMD_8X4, ISTRATA_CLEAR_CMD1_8X4, 
            ISTRATA_CLEAR_CMD2_8X4, ISTRATA_STATUS_BUSY_8X4);
      } /* Endfor */
      
   } else {
      success = FALSE;
   } /* Endif */

   return success;

} /* Endbody */
/* End CR 871 */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _intel_strata_setlockbits_1byte
* Returned Value   : TRUE if successful
* Comments         : 
*    This function locks all sectors
* 
*END*----------------------------------------------------------------------*/

bool _intel_strata_setlockbits_1byte
   (
      /* [IN] the base address of the device */
      IO_FLASHX_STRUCT_PTR handle_ptr,

      /* [IN/OUT] place to store start time */
      MQX_TICK_STRUCT_PTR  start_ticks_ptr
   )
{ /* Body */
   uint32_t block_size;
   uint32_t num_blocks;
   uint32_t i;
   bool timeout = FALSE;
   bool success = TRUE;

   num_blocks = handle_ptr->HW_BLOCK->NUM_SECTORS; /* sector == block */
   block_size = handle_ptr->HW_BLOCK->SECTOR_SIZE;

   /*
   ** The blocks are locked individually.
   */
   if (handle_ptr->DEVICES == 1) {
      volatile unsigned char         *dest_ptr;
      unsigned char                   status;

      for(i = 0; i < num_blocks; i++) {
         dest_ptr = (volatile unsigned char *)(handle_ptr->BASE_ADDR + handle_ptr->HW_BLOCK[0].START_ADDR + i * block_size);
         
         ISTRATA_SET_LOCKBITS(ISTRATA_READ_MODE_CMD_8X1, ISTRATA_SET_CMD1_8X1, 
            ISTRATA_SET_CMD2_8X1, ISTRATA_STATUS_BUSY_8X1);
      } /* Endfor */
      
   } else if (handle_ptr->DEVICES == 2) {
      volatile uint16_t       *dest_ptr;
      uint16_t                 status;

      for(i = 0; i < num_blocks; i++) {
         dest_ptr = (volatile uint16_t *)(handle_ptr->BASE_ADDR + handle_ptr->HW_BLOCK[0].START_ADDR + i * block_size);
         
         ISTRATA_SET_LOCKBITS(ISTRATA_READ_MODE_CMD_8X2, ISTRATA_SET_CMD1_8X2, 
            ISTRATA_SET_CMD2_8X2, ISTRATA_STATUS_BUSY_8X2);
      } /* Endfor */
      
   } else if (handle_ptr->DEVICES == 4) {
      volatile uint32_t       *dest_ptr;
      uint32_t                 status;

      for(i = 0; i < num_blocks; i++) {
         dest_ptr = (volatile uint32_t *)(handle_ptr->BASE_ADDR + handle_ptr->HW_BLOCK[0].START_ADDR + i * block_size);
         
         ISTRATA_SET_LOCKBITS(ISTRATA_READ_MODE_CMD_8X4, ISTRATA_SET_CMD1_8X4, 
            ISTRATA_SET_CMD2_8X4, ISTRATA_STATUS_BUSY_8X4);
      } /* Endfor */
      
   } else {
      success = FALSE;
   } /* Endif */

   return success;

} /* Endbody */

/* EOF */
