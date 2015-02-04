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
*   The file contains functions prototype, defines for the internal 
*   flash driver
*
*
*END************************************************************************/
#ifndef __INT_FLASH_MCF51XX_PRV_H__
#define __INT_FLASH_MCF51XX_PRV_H__


#define CFM_IPS_FLASH_ADDR              0x44000000

/* address in memory map to write in FLASH */
#define FLASH_START_ADDRESS  CFM_IPS_FLASH_ADDR

/* End address for region of flash */
#define FLASH_END_ADDRESS   (vuint32)&__FLASH_SIZE

#define FSTR_SET_STD    0
#define FSTR_SET_1      1
#define FSTR_SET_2      2

#define MEMORY_ARRAY_STD    0
#define MEMORY_ARRAY_1      1
#define MEMORY_ARRAY_2      2

typedef struct mcf51xx_flash_internal_struct
{
    volatile unsigned char   *ftsr_ptr;  /* pointer to proper FSTAT register */
    unsigned char   *flash_execute_code_ptr; /* pointer to flash write and erase code */
        
} MCF51XX_FLASH_INTERNAL_STRUCT, * MCF51XX_FLASH_INTERNAL_STRUCT_PTR;

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif

void _mcf51xx_set_cfmclkd(uint8_t);

bool mcf51xx_erase_flash(IO_FLASHX_STRUCT_PTR);
bool mcf51xx_flash_init(IO_FLASHX_STRUCT_PTR);
void    mcf51xx_flash_deinit(IO_FLASHX_STRUCT_PTR);
bool mcf51xx_flash_erase_sector(IO_FLASHX_STRUCT_PTR, char *, _mem_size);
bool mcf51xx_flash_write_sector(IO_FLASHX_STRUCT_PTR, char *, char *, _mem_size);
_mqx_int _io_mcf51xx_flash_ioctl(IO_FLASHX_STRUCT_PTR, _mqx_uint, void *);
void _mcf51_swap_flash_and_reset();

#ifdef __cplusplus
}
#endif

#endif /* __INT_FLASH_MCF51XX_H__ */
