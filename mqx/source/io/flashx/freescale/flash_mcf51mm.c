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
#include "flash_mcf51mm.h"

static void mcf51mm_swap_flash_ram_end(void);
static void mcf51mm_swap_flash_ram(VMCF51MM_SIM_STRUCT_PTR sim_ptr);

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_cfm_address
* Returned Value   : Address upon success
* Comments         :
*    This function returns the base register address of the CFM
*
*END*----------------------------------------------------------------------*/
void *_bsp_get_cfm_address(unsigned char module)
{
   VMCF51MM_STRUCT_PTR reg_ptr = _PSP_GET_MBAR();
   VMCF51XX_FTSR_STRUCT_PTR ftsr_ptr;
   switch (module)
   {
      case MEMORY_ARRAY_STD: 
      case MEMORY_ARRAY_1: 
               ftsr_ptr = (VMCF51XX_FTSR_STRUCT_PTR)&reg_ptr->FTSR1;
               break;
      case MEMORY_ARRAY_2: 
               ftsr_ptr = (VMCF51XX_FTSR_STRUCT_PTR)&reg_ptr->FTSR2;
               break;
      default: ftsr_ptr = NULL;
               break;          
   }
   return (void *)ftsr_ptr;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_get_pmc_address
* Returned Value   : Address upon success
* Comments         :
*    This function returns the base register address of the PMC
*
*END*----------------------------------------------------------------------*/
void *_bsp_get_pmc_address()
{
   VMCF51MM_STRUCT_PTR reg_ptr = _PSP_GET_MBAR(); 
   uint8_t *pmc_ptr = (uint8_t *)&reg_ptr->PMC;
   return (void *)pmc_ptr;   
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mcf51mm_get_rtc_cfg_state
* Returned Value   : Value of bit ARRAYSEL
* Comments         :
*    This function returns the value of ARRAYSEL.
*    There is stored information which flash bank is addressed from 0
*
*END*----------------------------------------------------------------------*/
uint32_t _mcf51_get_rtc_cfg_state()
{
    uint32_t result;
    
    if ( ( ((VMCF51MM_STRUCT_PTR)BSP_IPSBAR)->SIM.SOPT3 ) & MCF51XX_SOPT3_ARRAYSEL_MASK ){    
        result = 1;
    } else {
        result = 0;
    }
    return result;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _mcf51_swap_flash_and_reset
* Returned Value   : none
* Comments         :
*    Swap the flash adresses and restart device.
*
*END*----------------------------------------------------------------------*/
void _mcf51_swap_flash_and_reset()
{
    
    unsigned char *ramcode_ptr = NULL;
    int32_t (*RunInRAM)( VMCF51MM_RTC_STRUCT_PTR );
    VMCF51MM_SIM_STRUCT_PTR sim_ptr = &(((VMCF51MM_STRUCT_PTR)BSP_IPSBAR)->SIM);
    
    /* Allocate space on stack to run flash command out of SRAM */
    ramcode_ptr = _mem_alloc((char*)mcf51mm_swap_flash_ram_end - (char*)mcf51mm_swap_flash_ram); 
    /* copy code to RAM buffer */
    _mem_copy((void*)mcf51mm_swap_flash_ram, ramcode_ptr, (char*)mcf51mm_swap_flash_ram_end  - (char*)mcf51mm_swap_flash_ram);
    *RunInRAM = (int32_t(*)( VMCF51MM_SIM_STRUCT_PTR ))ramcode_ptr;   

    /* Run the code from RAM */
    RunInRAM( sim_ptr );
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : mcf51mm_swap_flash_ram
* Returned Value   : none
* Comments         :
*    Finish the swap of flash adresses and restart device. This function is
*    copied to RAM
*
*    WARNING! This code doesn't run correctly from debuger. If debuger is used,
*    instead reset jump to exception occured.
*
*END*----------------------------------------------------------------------*/
static void mcf51mm_swap_flash_ram(VMCF51MM_SIM_STRUCT_PTR sim_ptr) 
{
    void (_CODE_PTR_ jump_to_address)(void);
    uint32_t temp;

    /* disable flash speculation */
    _psp_set_cpucr(0x02000000);
    
    /* disable all interrupts */
    temp = _psp_get_sr();
    _psp_set_sr(temp | 0x0700);

    /* swap banks */
    sim_ptr->SOPT3 ^= MCF51XX_SOPT3_ARRAYSEL_MASK;
    
    /* wait after write to ARRAYSEL */
    _ASM_NOP();
    _ASM_NOP();
    _ASM_NOP();

    /* The processor generates a reset if CPUCR[ARD] = 0 */
    _psp_set_cpucr(0);
    
    /* reset using illegal address jump */
    jump_to_address = (void (_CODE_PTR_)(void))0x00804001;
    jump_to_address();

}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : mcf51mm_swap_flash_ram_end
* Returned Value   : none
* Comments         :
*    End-mark function
*
*END*----------------------------------------------------------------------*/
static void mcf51mm_swap_flash_ram_end(void) {}

/* EOF */
