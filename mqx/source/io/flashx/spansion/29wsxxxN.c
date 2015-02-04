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
*   S29ws128N flash devices.
*
*
*END************************************************************************/

#include "mqx.h" 
#include "bsp.h"
#include "flashx.h"
#include "flashxprv.h"
#include "29wsxxxN.h"
#include "29wsxxxN_prv.h"
                                                      
#define FLASH_READY 1
#define FLASH_ERASE_SUSPENDED 2
#define FLASH_TIMEOUT 3
#define FLASH_BUSY 4
#define FLASH_ERROR 5
#define FLASH_UNSUPPORTED_BUS_SIZE 6
#define FLASH_WRITE_PROTECT 7

#define FLASH_RESET_CMD 0xF0

#define DQ0 0x01
#define DQ1 0x02
#define DQ2 0x04
#define DQ3 0x08
#define DQ4 0x10
#define DQ5 0x20
#define DQ6 0x40
#define DQ7 0x80

#define  UNLOCKED   0
#define  LOCKED     1

#define WS_BYTES_IN_WORD 2

uint32_t _29wsxxxN_test_erase_cmd( void *, uint16_t );
uint32_t _29wsxxxN_test_program_cmd( void *, uint16_t );

const FLASHX_DEVICE_IF_STRUCT _flashx_29wsxxxN_if = {
    _29wsxxxN_erase,
    _29wsxxxN_program,
    _29wsxxxN_chip_erase,
    NULL,
    NULL,
    NULL,
    _29wsxxxN_write_protect,
    _29wsxxxN_ioctl
};

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29wsxxxN_program
* Returned Value   : TRUE if successful
* Comments         : 
*    This function programs the prom devices listed below:
*       29ws128N, 29ws256N and 29ws512N
* 
*END*----------------------------------------------------------------------*/

bool _29wsxxxN_program
   (  
      /* [IN] the flash information structure */
      IO_FLASHX_STRUCT_PTR dev_ptr,

      /* [IN] where to copy data from */
      char             *from_ptr,
      
      /* [OUT] where to copy data to */
      char             *to_ptr,

      /* [IN] the size to copy in bytes */
      _mem_size            size
   )
{ /* Body */
   volatile uint16_t       *base16_ptr;
   uint32_t                 result = FLASH_READY;
   
   uint16_t                 temp_data = 0xFFFF;
   unsigned char               *temp_data_ptr;
   unsigned char                   byte_data_counter = 0;
   /* get the offset in write word */
   uint16_t offset = (uint16_t)to_ptr & 0x0001;

    /* test for flash write protect */
    if (_29wsxxxN_test_lock_bits(dev_ptr) == LOCKED) {
        return  FLASH_WRITE_PROTECT;     
    }/* Endif */

   if (dev_ptr->WIDTH == 16) {

      (uint16_t *)temp_data_ptr =  &temp_data;
            
      /* if the start address !=  doesn't correspond with hardware, prepare
           variables for 1st word write */
      if(offset){
          /* Align pointer to writable address */
          to_ptr -= offset;
          /* jump over old data */
          byte_data_counter = offset;
      }
       
      /* set the flash base address */
      base16_ptr = (uint16_t *)dev_ptr->BASE_ADDR; 

      /* we have data to write */
      while (size) {
         /* read old data to write word */
          temp_data = *(uint16_t *)to_ptr;
         /* move data to write word byte by byte */
         while( byte_data_counter < WS_BYTES_IN_WORD && size ){
            *(unsigned char *)(temp_data_ptr+byte_data_counter) = *from_ptr++;
            byte_data_counter++;
            size--;
         }
        /* flash program */
        _int_disable();
        /* unlock flash */
        *( base16_ptr + 0x555 ) = 0x00AA;
        *( base16_ptr + 0x2AA ) = 0x0055;
        /* Write command */            
        *( base16_ptr + 0x555 ) = 0x00A0;
        /* write value */        
        *(uint16_t *)to_ptr = temp_data;     
        _int_enable();

        /* test if the write operation complete */
        result = _29wsxxxN_test_program_cmd(to_ptr, temp_data);
        /* if write command failed, exit write command */ 
        if(result != FLASH_READY){
            break;
        }
         byte_data_counter = 0;
         temp_data = 0xFFFF;
         to_ptr += WS_BYTES_IN_WORD;
      }/* Endwhile */

   }
   else 
       result = FLASH_UNSUPPORTED_BUS_SIZE;

   return result;

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29wsxxxN_erase
* Returned Value   : TRUE if successful
* Comments         : 
*    This function erases (sector) the prom devices listed below:
*       29ws128N, 29ws256N and 29ws512N
* 
*END*----------------------------------------------------------------------*/

bool _29wsxxxN_erase
   (
      /* [IN] the flash information structure */
      IO_FLASHX_STRUCT_PTR dev_ptr,

      /* [IN] the sector to erase */
      char            *input_sect_ptr,

      /* [IN] the sector size */
      _mem_size            sector_size
   )
{ /* Body */
   volatile uint16_t      *sect16_ptr;
   uint32_t                result = FLASH_READY;
   
   uint32_t tmp1;
   uint32_t time0;

   /* test for flash write protect */
    if (_29wsxxxN_test_lock_bits(dev_ptr) == LOCKED) {
        return  FLASH_WRITE_PROTECT;     
    }/* Endif */

   if (dev_ptr->WIDTH == 16) {
      sect16_ptr = (volatile uint16_t *)input_sect_ptr;
      _int_disable();
      /* unlock flash */     
      *( sect16_ptr + 0x555 ) = 0x00AA;
      *( sect16_ptr + 0x2AA ) = 0x0055;
      /* setup command */
      *( sect16_ptr + 0x555 ) = 0x0080;
      /* unlock */
      *( sect16_ptr + 0x555 ) = 0x00AA;
      *( sect16_ptr +0x2AA ) = 0x0055;
      /* Sector erase */
      *( sect16_ptr )    = 0x0030;    
      _int_enable();

      /* erase check */ 
      tmp1 = *sect16_ptr; 
      /* wait, until DQ3 = 0 or tSEA=50us */  
      time0 = _time_get_microseconds(); 
      while(!(tmp1 & DQ3) && (_time_get_microseconds()-time0)<50){      
          tmp1 = *sect16_ptr;   
      }
      _io_flashx_wait_us(4);
      result = _29wsxxxN_test_erase_cmd(input_sect_ptr, (uint16_t)0xFFFF);  
   } 
   else {
      result = FLASH_UNSUPPORTED_BUS_SIZE;
   }

   return result == FLASH_READY;
   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29wsxxxN_chip_erase
* Returned Value   : TRUE if successful
* Comments         : 
*    This function erases all the prom devices. listed below:
*       29ws128N, 29ws256N and 29ws512N
* 
*END*----------------------------------------------------------------------*/

bool _29wsxxxN_chip_erase
   (
      /* [IN] the flash information structure */
      IO_FLASHX_STRUCT_PTR dev_ptr
   )
{ /* Body */
   volatile uint16_t      *sect16_ptr;
   uint32_t                result  = FLASH_READY;

   /* test for flash write protect */
    if (_29wsxxxN_test_lock_bits(dev_ptr) == LOCKED) {
        return  FLASH_WRITE_PROTECT;     
    }/* Endif */

   if (dev_ptr->WIDTH == 16) {
      sect16_ptr = (volatile uint16_t *)dev_ptr->BASE_ADDR;
      _int_disable();
      /* unlock */
      *( sect16_ptr + 0x555 ) = 0x00AA;
      *( sect16_ptr + 0x2AA ) = 0x0055;
      /* setup command */
      *( sect16_ptr + 0x555 ) = 0x0080;
      /* unlock */
      *( sect16_ptr + 0x555 ) = 0x00AA;
      *( sect16_ptr + 0x2AA ) = 0x0055;
      /* chip erase */
      *( sect16_ptr + 0x555 ) = 0x0010;    
      _int_enable();
     
      /* chip erase test */
      result = _29wsxxxN_test_erase_cmd((unsigned char *)sect16_ptr, (uint16_t)0xFFFF);     
   }
   else {
      result = FLASH_UNSUPPORTED_BUS_SIZE; 
   }

   return result == FLASH_READY;
   
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : 29wsxxxN_test_erase_cmd
* Returned Value   : uint32_t current status of operation
* Comments         : test, if the write flash command operation finished
*
*END*----------------------------------------------------------------------*/
uint32_t _29wsxxxN_test_erase_cmd
   (
      /* [IN] address */
      void                *to_ptr,
      /* written data */
      uint16_t              write_data
   ) 
{
    uint16_t     test_data, test_data1;
    unsigned char       error_count = 0;
    uint32_t     result =  FLASH_BUSY;   

    while(result == FLASH_BUSY){
        /* Read 1 */
        test_data = *(uint16_t *)to_ptr;
        /* DQ7 contain valid data? */
        if((test_data & DQ7) == (write_data & DQ7)){
            /* Read 2 */
            test_data1 = *(uint16_t *)to_ptr;
            /* Read 3 */
            test_data = *(uint16_t *)to_ptr;
            /* DQ6 toggling, device error */
            if((test_data & DQ6) != (test_data1 & DQ6) ){
                /* software flash reset */
                *(uint16_t *)to_ptr = FLASH_RESET_CMD;
                result = FLASH_ERROR;
            }
            /* Erase operation complete */
            if((test_data & DQ2) == (test_data1 & DQ2)){
                result = FLASH_READY;
            }
        }
        /* test for operation internal timeout */
        else {
            /* read 1 DQ5 = 1? */
            if(test_data & DQ5){
                /* read 2 */
                test_data1 = *(uint16_t *)to_ptr;
                /* read 3 */
                test_data = *(uint16_t *)to_ptr;
                /* DQ6 togling? Timeout occured */  
                if(test_data1 & DQ6 != test_data & DQ6){
                    result = FLASH_TIMEOUT;
                    /* software flash reset */
                    *(uint16_t *)to_ptr = FLASH_RESET_CMD;
                }
            }
        }
    }
    
    return result == FLASH_READY;       
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : 29wsxxxN_test_program_cmd
* Returned Value   : uint32_t current status of operation
* Comments         : test, if the write flash command operation finished
*
*END*----------------------------------------------------------------------*/
uint32_t _29wsxxxN_test_program_cmd
   (
      /* [IN] address */
      void                *to_ptr,
      /* written data */
      uint16_t              write_data
   ) 
{
    uint16_t     test_data, test_data1;
    uint16_t     cycle_counter = 5000;
    unsigned char       error_count = 0;
    uint32_t     result =  FLASH_BUSY;   

    while(result == FLASH_BUSY){
        /* Read 1 */
        test_data = *(uint16_t *)to_ptr;
        /* DQ7 contain valid data? */
        if((test_data & DQ7) == (write_data & DQ7)){
            /* Read 2 */
            test_data1 = *(uint16_t *)to_ptr;
            /* Read 3 */
            test_data = *(uint16_t *)to_ptr;
            /* Read 3 valid data => Write operation complete */
            if(test_data == write_data){
                result = FLASH_READY;
            }
            /* Read 3 doesn't contain valid data, operation failed */
            else {                  
                if(error_count++>2){
                    result = FLASH_ERROR;
                    /* software flash reset */
                    *(uint16_t *)to_ptr = FLASH_RESET_CMD;
                }
            }
        }
        /* test for operation internal timeout */
        else {
            if(!(test_data & DQ5)){
                test_data1 = *(uint16_t *)to_ptr;
                test_data = *(uint16_t *)to_ptr;
                /* DQ6 togling? Timeout occured */  
                if(test_data1 & DQ6 != test_data & DQ6){
                    result = FLASH_TIMEOUT;
                    /* software flash reset */
                    *(uint16_t *)to_ptr = FLASH_RESET_CMD;
                }
            }
        }
        /* timeout test */
        if(!cycle_counter--){
            result = FLASH_TIMEOUT;
        }
    }
    
    return result;      
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29wsxxxN_test_lock_bits
* Returned Value   : uint32_t current status of operation
* Comments         : test, if flash is write protect
*
*END*----------------------------------------------------------------------*/
static bool _29wsxxxN_test_lock_bits
    (
      /* [IN] the flash information structure */
      IO_FLASHX_STRUCT_PTR dev_ptr
    ) 
{
    uint16_t *base_address_ptr = (uint16_t *)dev_ptr->BASE_ADDR;
    uint16_t *sector_address_ptr; 
    int i;
    int sector_count;
    uint16_t data = 0;
        
    /* go over all blocks */
    for ( i = 0; dev_ptr->HW_BLOCK[i].NUM_SECTORS != 0 ; i++ ) {
        /* set the start block address */
        sector_address_ptr = (uint16_t *)dev_ptr->HW_BLOCK[i].START_ADDR; 
        /* go over all sectors in block */
        for ( sector_count = 0; sector_count < dev_ptr->HW_BLOCK[i].NUM_SECTORS; sector_count ++){
            /* enter DYB mode */
            _int_disable();
            *( base_address_ptr + 0x555 ) = 0x00AA; /* write unlock cycle 1 */
            *( base_address_ptr + 0x2AA ) = 0x0055; /* write unlock cycle 2 */
            *( sector_address_ptr + 0x555 ) = 0x00E0; /* write command entry */
            /* read sector lock bit */
            data = *sector_address_ptr;             
            /* exit DYB mode */
            *( base_address_ptr ) = 0x0090; /* command set exit */
            *( base_address_ptr ) = 0x0000; 
            _int_enable();
            /* is sector locked? 0 == locked */
            if( !(data & DQ0) ){    
                /* yes - exit locked */
                return LOCKED;
            }               
            /* no - incrememnt sector address */
            (unsigned char *)sector_address_ptr += (dev_ptr->HW_BLOCK[i].SECTOR_SIZE); 
        }       
    }
    /* exit flash unlocked */
    return UNLOCKED;    
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29wsxxxN_write_protect
* Returned Value   : uint32_t current status of operation
* Comments         : lock/unlock flash for write
*
*END*----------------------------------------------------------------------*/
bool _29wsxxxN_write_protect
    (
        /* [IN] the base address of the device */
      IO_FLASHX_STRUCT_PTR  dev_ptr,
      
      /* [IN]  */
      _mqx_uint             lock
    ) 
{
    volatile uint16_t *source_address_ptr;
    uint16_t *sector_address_ptr; 
    uint16_t *base_address_ptr = (uint16_t *)dev_ptr->BASE_ADDR;
    int i;
    int sector_count;
    uint16_t command = 0;

    /* set the lock/unlock command */
    if (lock == 1 ){
        /* lock sector command */
        command = 0;
    }
    else {
        /* unlock sector command */
        command = 1;
    }
    /* go over all blocks */
    for ( i = 0; dev_ptr->HW_BLOCK[i].NUM_SECTORS != 0; i++ ) {
        /* set the start block address */
        sector_address_ptr = (uint16_t *)dev_ptr->HW_BLOCK[i].START_ADDR; 
        /* go over all sectors in block */
        for ( sector_count = 0; sector_count < dev_ptr->HW_BLOCK[i].NUM_SECTORS; sector_count ++){
            /* enter DYB mode */
            _int_disable(); 
            *( base_address_ptr + 0x555 ) = 0x00AA; /* write unlock cycle 1 */
            *( base_address_ptr + 0x2AA ) = 0x0055; /* write unlock cycle 2 */
            *( sector_address_ptr + 0x555 ) = 0x00E0; /* write command */
            *( base_address_ptr ) = 0x00A0; 
            *( sector_address_ptr ) = command; 
            *( base_address_ptr ) = 0x0090; /* command set exit */
            *( base_address_ptr ) = 0x0000; 
            _int_enable();                      
            /* increment sector */
            (unsigned char *)sector_address_ptr += (dev_ptr->HW_BLOCK[i].SECTOR_SIZE); 
        }       
    }
    /* reset device */
    * base_address_ptr = FLASH_RESET_CMD;
    
    return MQX_OK;  
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _29wsxxxN_ioctl
* Returned Value   : TRUE if successful
* Comments         : 
*    flash ioctl function
* 
*END*----------------------------------------------------------------------*/
_mqx_int _29wsxxxN_ioctl
   (
      /* [IN] the handle returned from _fopen */
      IO_FLASHX_STRUCT_PTR handle_ptr,

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
            if (_29wsxxxN_test_lock_bits(handle_ptr) ) {
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


/* EOF */
