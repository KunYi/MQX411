/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains the source for the FlexEEPROM example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>


#ifndef BSP_INTERNAL_FLEXRAM_BASE
    #error Not supported board !!!
#endif

#if ! BSPCFG_ENABLE_FLASHX
    #error This application requires BSPCFG_ENABLE_FLASHX defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

/* Task IDs */
#define MAIN_TASK 7


/* function prototypes */
void main_task(uint32_t);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{ 
   /* Task Index,   Function,   Stack,  Priority,   Name,   Attributes,          Param, Time Slice */ 
    { MAIN_TASK,    main_task,  4000,   8,          "main", MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};

/*TASK*-----------------------------------------------------
* 
* Task Name    : main_task
* Comments     :
*    This task does nothing
*
*END*-----------------------------------------------------*/
void main_task
   (
      uint32_t initial_data
   )
{
    MQX_FILE_PTR flash_file;
    uint32_t val;
    FLEXNVM_PROG_PART_STRUCT part_param;
    _mqx_int result;
    uint8_t *byte_ptr = (uint8_t*) BSP_INTERNAL_FLEXRAM_BASE;
    uint16_t *half_ptr = (uint16_t*) BSP_INTERNAL_FLEXRAM_BASE + 1;
    uint32_t *word_ptr = (uint32_t*) BSP_INTERNAL_FLEXRAM_BASE + 1;

    _int_install_unexpected_isr();

    printf("\n\nMQX FlexNVM example");
   
    /* Open the flash device */
    flash_file = fopen("flashx:flexram0", NULL);
    if (NULL == flash_file) {
        _task_block();
    }
    
    /* read partition code to check FlexNVM settings */
    result = _io_ioctl(flash_file, FLEXNVM_IOCTL_GET_PARTITION_CODE, &part_param);
    
    if (IO_OK != result) {
        printf("\nError when reading FlexNVM configuration.");
        _task_block();
    }

    /* check FlexNVM partition settings */
    if (FLEXNVM_PART_CODE_NOPART == part_param.FLEXNVM_PART_CODE) {
        printf("\n\nEnabling FlexEEPROM - partition memory.");
        
        /* set FlexNVM partition and EEPROM size */
        part_param.EE_DATA_SIZE_CODE = BSP_EE_DATA_SIZE_CODE;
        part_param.FLEXNVM_PART_CODE = BSP_FLEXNVM_PART_CODE;
        _io_ioctl(flash_file, FLEXNVM_IOCTL_SET_PARTITION_CODE, &part_param);         

        /* switch FlexRAM to EEPROM mode */
        val = FLEXNVM_FLEXRAM_EE;
        _io_ioctl(flash_file, FLEXNVM_IOCTL_SET_FLEXRAM_FN, &val); 
    }
    else
        printf("\nDevice has FlexEEPROM already enabled.");

    printf("\n\nReading actual memory values:\n");
    printf("byte: 0x%p = 0x%x\n", byte_ptr, *byte_ptr);
    printf("half: 0x%p = 0x%x\n", half_ptr, *half_ptr);
    printf("word: 0x%p = 0x%x\n", word_ptr, *word_ptr);
    
    printf("\n\nWriting new values to memory:\n");
    
    /* wait for FlexEEPROM ready */
    _io_ioctl(flash_file, FLEXNVM_IOCTL_WAIT_EERDY, NULL); 
    *byte_ptr += 1;
    
    /* wait for FlexEEPROM ready - after write */
    _io_ioctl(flash_file, FLEXNVM_IOCTL_WAIT_EERDY, NULL); 
    *half_ptr += 2;
    
    /* wait for FlexEEPROM ready - after write */
    _io_ioctl(flash_file, FLEXNVM_IOCTL_WAIT_EERDY, NULL);     
    *word_ptr += 3;
    
    /* wait for FlexEEPROM ready - after write */
    _io_ioctl(flash_file, FLEXNVM_IOCTL_WAIT_EERDY, NULL);     
    
    printf("\n\nReading new memory values:\n");
    printf("byte: 0x%p = 0x%x\n", byte_ptr, *byte_ptr);
    printf("half: 0x%p = 0x%x\n", half_ptr, *half_ptr);
    printf("word: 0x%p = 0x%x\n", word_ptr, *word_ptr);
   
    printf("\nProgram finshed, you can try restart !");

    fclose(flash_file);
    
    _task_block();
}
