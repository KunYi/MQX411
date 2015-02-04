#ifndef __INT_FLASH_MCF52XX_PRV_H__
#define __INT_FLASH_MCF52XX_PRV_H__
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

#define CFM_IPS_FLASH_ADDR              0x44000000

/* address in memory map to write in FLASH */
#define FLASH_START_ADDRESS  CFM_IPS_FLASH_ADDR

/* End address for region of flash */
#define FLASH_END_ADDRESS   (vuint32)&__FLASH_SIZE+0x44000000

typedef struct internal_52xx_struct 
{
    volatile char  *cfm_ptr;  /* pointer to CFM structure */
    char   *flash_execute_code_ptr; /* pointer to flash write and erase code */
        
} MCF52XX_FLASH_INTERNAL_STRUCT, * MCF52XX_FLASH_INTERNAL_STRUCT_PTR;

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif

void    _mcf52xx_set_cfmclkd();

bool mcf52xx_flash_init(IO_FLASHX_STRUCT_PTR);
void mcf52xx_flash_deinit(IO_FLASHX_STRUCT_PTR);
bool mcf52xx_flash_erase_sector(IO_FLASHX_STRUCT_PTR, char *, _mem_size);
bool mcf52xx_flash_write_sector(IO_FLASHX_STRUCT_PTR, char *, char *, _mem_size);

#ifdef __cplusplus
}
#endif

#endif /* __INT_FLASH_MCF52XX_PRV_H__ */
