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
*   The file contains functions for internal flash read, write, erase 
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "flashx.h"
#include "flashxprv.h"
#include "flash_mcf51xx.h"
#include "flash_mcf51xx_prv.h"
   
/* Internal function definitions */
static void mcf51xx_ram_function(volatile unsigned char *); 
static void mcf51xx_ram_function_end (void);

unsigned char *mcf51xx_init_ram_function(void);

const FLASHX_BLOCK_INFO_STRUCT _flashx_mcf51xx_block_map[] = {
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE, (uint32_t) BSP_INTERNAL_FLASH_BASE, BSP_INTERNAL_FLASH_SECTOR_SIZE, 0},
    { 0, 0, 0, 0 }
};

const FLASHX_DEVICE_IF_STRUCT _flashx_mcf51xx_if = {
    mcf51xx_flash_erase_sector,
    mcf51xx_flash_write_sector,
    NULL,
    NULL,
    mcf51xx_flash_init,
    mcf51xx_flash_deinit,
    NULL,
    _io_mcf51xx_flash_ioctl
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mcf51xx_set_cfmclkd
* Returned Value   : none
* Comments         :
*    Set the CFMCLKD register.
*
*END*----------------------------------------------------------------------*/
void _mcf51xx_set_cfmclkd(uint8_t mem_array)
{
#define PRDIV8_LIMIT_FREQ   12800000L // limit freq. for using PRDIV8_PRSC
#define FLASH_FREQ          200000L   // 190 kHz program frequency
#define PRDIV8_PRSC         8         // prescaller 8
    
    VMCF51XX_FTSR_STRUCT_PTR  ftsr_ptr;
    
    ftsr_ptr = _bsp_get_cfm_address(mem_array);
    
    if(BSP_BUS_CLOCK > PRDIV8_LIMIT_FREQ)
    {
        ftsr_ptr->FCDIV = MCF51XX_FTSR_FCDIV_PRDIV8 | MCF51XX_FTSR_FCDIV_FDIV((BSP_BUS_CLOCK + PRDIV8_PRSC * FLASH_FREQ - 1) / (PRDIV8_PRSC * FLASH_FREQ) - 1);
    }
    else
    {
        ftsr_ptr->FCDIV = MCF51XX_FTSR_FCDIV_FDIV((BSP_BUS_CLOCK + FLASH_FREQ - 1) / FLASH_FREQ - 1);
    }
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf51xx_flash_init
* Returned Value   : 
* Comments         :
*   Init flash - store the FSTAT register  
*
*END*----------------------------------------------------------------------*/
bool mcf51xx_flash_init
(
    /* [IN] File pointer */
    IO_FLASHX_STRUCT_PTR dev_ptr
)
{
    volatile MCF51XX_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr;
    uint8_t array;
    VMCF51XX_FTSR_STRUCT_PTR ftsr_ptr;
    
    dev_spec_ptr = _mem_alloc(sizeof(MCF51XX_FLASH_INTERNAL_STRUCT));
#if MQX_CHECK_FOR_MEMORY_ALLOCATION_ERRORS
    /* test for correct memory allocation */
    if(dev_spec_ptr == NULL){
        return FALSE;
    }
#endif //MQX_CHECK_FOR_MEMORY_ALLOCATION_ERRORS

    /* get the pointer to cfm registers structure */
#ifndef PSP_HAS_DUAL_FLASH
    array = MEMORY_ARRAY_STD;
#else
    /* RTC_CFG_DATA[CFG0] = 0 : FSTR1 => 0x00000000 - see Flash Array Base Address table */
    array = (dev_ptr->BASE_ADDR == 0) ? MEMORY_ARRAY_1 : MEMORY_ARRAY_2;
    if (_mcf51_get_rtc_cfg_state() == 1) {
        /* if flash are already swapped, then swap the pointers */
        array = (array == MEMORY_ARRAY_1) ? MEMORY_ARRAY_2 : MEMORY_ARRAY_1;
    }
#endif //PSP_HAS_DUAL_FLASH
    ftsr_ptr = _bsp_get_cfm_address(array);

    dev_spec_ptr->ftsr_ptr = (volatile unsigned char *)ftsr_ptr;
    dev_ptr->DEVICE_SPECIFIC_DATA = dev_spec_ptr;
    
    /* save pointer to function in ram - this is the default settings  */
    dev_spec_ptr->flash_execute_code_ptr = mcf51xx_init_ram_function();
    
    /* set correct clocking for CFM module */
    _mcf51xx_set_cfmclkd(array);

    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf51xx_flash_deinit
* Returned Value   : 
* Comments         :
*    release used memory      
*
*END*----------------------------------------------------------------------*/
void mcf51xx_flash_deinit
(
    /* [IN] File pointer */
    IO_FLASHX_STRUCT_PTR dev_ptr
)
{
    MCF51XX_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (MCF51XX_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA; 
    
    /* if the flash write,erase code run form RAM, free memory */
    if (dev_ptr->FLAGS & FLASHX_WR_ERASE_FROM_FLASH_ENABLED == 0) {
        /* de-allocate the ram function buffer */
        if(dev_spec_ptr->flash_execute_code_ptr) {
            _mem_free(dev_spec_ptr->flash_execute_code_ptr);
        }
    }
    dev_spec_ptr->flash_execute_code_ptr = NULL;
    /* de-allocate device specific structure */
    _mem_free(dev_ptr->DEVICE_SPECIFIC_DATA);
    dev_ptr->DEVICE_SPECIFIC_DATA = NULL; 
}   

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf51xx_init_ram_function
* Returned Value   : pointer to allocated RAM function
* Comments         :
*   allocate and copy flash RAM function  
*
*END*----------------------------------------------------------------------*/
unsigned char *mcf51xx_init_ram_function()
{
    unsigned char *ram_code_ptr;
    
    /* Allocate space on stack to run flash command out of SRAM */
    ram_code_ptr = _mem_alloc((char*)mcf51xx_ram_function_end - (char*)mcf51xx_ram_function);   
    /* copy code to RAM buffer */
    _mem_copy((void*)mcf51xx_ram_function, ram_code_ptr, (char*)mcf51xx_ram_function_end  - (char*)mcf51xx_ram_function);

    return ram_code_ptr;               
}
    
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf51xx_erase_sector_ext
* Returned Value   : true if success, or false
* Comments         :
*   Erase a flash memory block  
*
*END*----------------------------------------------------------------------*/
bool mcf51xx_flash_erase_sector
(
    /* [IN] File pointer */
    IO_FLASHX_STRUCT_PTR dev_ptr, 

    /* [IN] Erased sector address */
    char             *from_ptr,

    /* [IN] Erased sector size */
    _mem_size            size
)
{
    MCF51XX_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (MCF51XX_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA; 
    VMCF51XX_FTSR_STRUCT_PTR ftsr_ptr;
    int (*RunInRAM)(unsigned char *);
    uint32_t temp;

    /* select the proper ramcode function */
    *RunInRAM = (int(*)(unsigned char *))dev_spec_ptr->flash_execute_code_ptr;
    
    /* get the pointer to cfm registers structure */
    ftsr_ptr = (VMCF51XX_FTSR_STRUCT_PTR)dev_spec_ptr->ftsr_ptr;   
    
    _int_disable();
    /* prepare flash write operation, clear flags and wait for ready state */
    ftsr_ptr->FSTAT = (MCF51XX_FTSR_FSTAT_FPVIOL | MCF51XX_FTSR_FSTAT_FACCERR);
    while (!(ftsr_ptr->FSTAT & MCF51XX_FTSR_FSTAT_FCBEF)){
        /* wait */
    };

    /* latch address in Flash */
    (*(volatile uint32_t *)(from_ptr)) = (volatile uint32_t)-1;  
    
    /* issue erase command */
    ftsr_ptr->FCMD = MCF51XX_FTSR_FCMD_SECTOR_ERASE;
    
    /* run command and wait for it to finish (must execute from RAM) */ 
    temp = _psp_get_sr();
    _psp_set_sr(temp | 0x0700);
    RunInRAM( (volatile unsigned char *)&ftsr_ptr->FSTAT );    
    _psp_set_sr(temp);
    
    if (ftsr_ptr->FSTAT & (MCF51XX_FTSR_FSTAT_FACCERR | MCF51XX_FTSR_FSTAT_FPVIOL)) {
        _int_enable();
        return FALSE;
    }
        
    _int_enable();
    return(TRUE);   
         
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf51xx_write_sector
* Returned Value   : true if success, or false
* Comments         :
*   Performs a write into flash memory  
*
*END*----------------------------------------------------------------------*/
bool mcf51xx_flash_write_sector
(
    /* [IN] File pointer */
    IO_FLASHX_STRUCT_PTR dev_ptr, 

    /* [IN] Source address */
    char             *from_ptr,

    /* [IN] Destination address */
    char             *to_ptr,

    /* [IN] Number of bytes to write */
    _mem_size            size
) 
{
    MCF51XX_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (MCF51XX_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA; 
    VMCF51XX_FTSR_STRUCT_PTR    ftsr_ptr;
    VMCF51XX_PMC_STRUCT_PTR     pmc_ptr; 
    int (*RunInRAM)( unsigned char *);
    volatile uint32_t temp, temp_data = 0xFFFFFFFF;
    volatile char *temp_data_ptr;
    unsigned char byte_data_counter = 0;
    #define BYTES_IN_WORD   4
    /* get the offset in write word */
    uint32_t offset = (uint32_t)to_ptr & 0x00000003;

    temp_data_ptr = (char *)&temp_data;
    
    /* select the proper ramcode function */
    RunInRAM = (int(*)( unsigned char *))dev_spec_ptr->flash_execute_code_ptr;

    /* get the pointer to cfm registers structure */
    ftsr_ptr = (VMCF51XX_FTSR_STRUCT_PTR)dev_spec_ptr->ftsr_ptr;
    
    /* get the pointer to pmc registers structure */
    pmc_ptr = _bsp_get_pmc_address();    
    /* Clear any errors */
    _int_disable();
    ftsr_ptr->FSTAT = (MCF51XX_FTSR_FSTAT_FPVIOL | MCF51XX_FTSR_FSTAT_FACCERR); 
    
    /* if the start address !=  doesn't correspond with hardware, prepare
       variables for 1st word write */
    if(offset){
        /* Align pointer to writable address */
        to_ptr -= offset;
        /* jump over old data */
        byte_data_counter = offset; 
    }
    /* while are some data to write */
    while(size){
        /* move data to write word */       
        while( byte_data_counter < BYTES_IN_WORD && size ){
            *(temp_data_ptr+byte_data_counter) = *from_ptr++;
            byte_data_counter++;
            size--;
        }
        /* test the LVDF flag - if 1, we need to write data in 2 steps */
        if( pmc_ptr->SPMSC1 & MCF51XX_PMC_SPMSC1_LVDF) {
            /* write odd bytes */
            (*(volatile uint32_t *)(to_ptr)) = temp_data | 0x00FF00FF;
            /* write command to CFMCMD */
            ftsr_ptr->FCMD = MCF51XX_FTSR_FCMD_BURST_PROGRAM;
            /* run command and wait for it to finish (must execute from RAM) */     
            temp = _psp_get_sr();
            _psp_set_sr(temp | 0x0700);
            RunInRAM( (volatile unsigned char *)&ftsr_ptr->FSTAT );
            _psp_set_sr(temp);
            /* write even bytes */              
            (*(volatile uint32_t *)(to_ptr)) = temp_data | 0xFF00FF00;
            /* write command to CFMCMD */
            ftsr_ptr->FCMD = MCF51XX_FTSR_FCMD_BURST_PROGRAM;
            /* run command and wait for it to finish (must execute from RAM) */     
            temp = _psp_get_sr();
            _psp_set_sr(temp | 0x0700);
            RunInRAM( (volatile unsigned char *)&ftsr_ptr->FSTAT );
            _psp_set_sr(temp);
        } 
        else {  
            /* move write data to register */
            (*(volatile uint32_t *)(to_ptr)) = temp_data;
            
            /* write command to CFMCMD */
            ftsr_ptr->FCMD = MCF51XX_FTSR_FCMD_BURST_PROGRAM;
            /* run command and wait for it to finish (must execute from RAM) */     
            temp = _psp_get_sr();
            _psp_set_sr(temp | 0x0700);
            RunInRAM( (volatile unsigned char *)&ftsr_ptr->FSTAT );                
            _psp_set_sr(temp);
        }
        /* Check for Errors */
        if (ftsr_ptr->FSTAT & (MCF51XX_FTSR_FSTAT_FPVIOL | MCF51XX_FTSR_FSTAT_FACCERR)) {
            _int_enable();
            return (FALSE);
        }
        
        /* init variables for next loop */
        to_ptr += BYTES_IN_WORD;
        byte_data_counter = 0;
        temp_data = 0xFFFFFFFF;
    }
    _int_enable();
    return (TRUE);  
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_mcf51xx_ioctl
* Returned Value   : TRUE if successful
* Comments         : 
*    flash ioctl function
* 
*END*----------------------------------------------------------------------*/
_mqx_int _io_mcf51xx_flash_ioctl
   (
      /* [IN] the handle returned from _fopen */
      IO_FLASHX_STRUCT_PTR   dev_ptr,

      /* [IN] the ioctl command */
      _mqx_uint              cmd,

      /* [IN] the ioctl parameters */
      void                  *param_ptr
   )
{ /* Body */
    MCF51XX_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (MCF51XX_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA; 
    _mqx_uint   result = MQX_OK;
    
    switch(cmd) {
        #ifdef PSP_HAS_DUAL_FLASH
        case FLASH_IOCTL_SWAP_FLASH_AND_RESET:
            _mcf51_swap_flash_and_reset();
            break;
        #endif
            
        case FLASH_IOCTL_WRITE_ERASE_CMD_FROM_FLASH_ENABLE:
            dev_ptr->FLAGS |= FLASHX_WR_ERASE_FROM_FLASH_ENABLED; 
            if(dev_spec_ptr->flash_execute_code_ptr){
                _mem_free(dev_spec_ptr->flash_execute_code_ptr);
            }
            dev_spec_ptr->flash_execute_code_ptr = (unsigned char *)mcf51xx_ram_function;
            break;
            
        case FLASH_IOCTL_WRITE_ERASE_CMD_FROM_FLASH_DISABLE:
            dev_ptr->FLAGS &= ~FLASHX_WR_ERASE_FROM_FLASH_ENABLED;
            dev_spec_ptr->flash_execute_code_ptr = mcf51xx_init_ram_function();
            break;
            
        default:
            break;
    }
    return result;
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : mcf51xx_ram_function
* Returned Value   : 
* Comments         : 
*    Launch the program command and wait until execution complete. This code
*    is required to run in SRAM, 
*    unless FLASH_IOCTL_WRITE_ERASE_CMD_FROM_FLASH_ENABLE ioctl command is
*    applied to allow the low level flash write and erase routines being run
*    from internal flash memory (works only with the dual flash memory 
*    controllers).
*    
*END*----------------------------------------------------------------------*/
static void mcf51xx_ram_function(volatile unsigned char *fstat_ptr) 
{
    /* Clear the FCBEF flag in the FSTAT register by writing a 1 */
    /* to FCBEF to launch the program command. */
    *fstat_ptr |= MCF51XX_FTSR_FSTAT_FCBEF;   
    /* wait until execution complete */
    while (!( *fstat_ptr & MCF51XX_FTSR_FSTAT_FCCF)){
    };
}


static void mcf51xx_ram_function_end (void) {}
