
/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
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
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the source for the Flashx/Swap example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <string.h>


#if !defined(BSPCFG_ENABLE_FLASHX) || !(BSPCFG_ENABLE_FLASHX)
#error This application requires BSPCFG_ENABLE_FLASHX defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#define BUFFER_SIZE     (64)

/* function prototypes */
void flash_task(uint32_t);


const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   /* Task Index,   Function,          Stack,  Priority,   Name,           Attributes,          Param, Time Slice */
    {          1,   flash_task,   2000,   7,          "flash_test",   MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};


typedef enum 
{
    SWAP_STATE_INIT,
    SWAP_STATE_FIRST,
    SWAP_STATE_SECOND,
} SWAP_STATE_ENUM;


LWGPIO_STRUCT button;


typedef struct 
{
    char uid[4];
    char message[28];
} swapmark_struct, * swapmark_struct_ptr;


/*TASK*-----------------------------------------------------
*
* Task Name    : verify_flashspace
* Return       : 1 if success, 0 if failed
* Comments     :
*    Check whether this application does not overlap 
*    "swap1" file.
*
*END*-----------------------------------------------------*/

uint32_t verify_flashspace(void)
{
    MQX_FILE_PTR flash_file = NULL;
    uint32_t used_size = 0, swap_size = 0;

    flash_file = fopen("flashx:code", NULL);
    if (NULL == flash_file)
    {
        return 0;
    }
    fseek(flash_file, 0, IO_SEEK_END);
    used_size = ftell(flash_file);
    fclose(flash_file);

    flash_file = fopen("flashx:swap1", NULL);
    if (NULL == flash_file)
    {
        return 0;
    }
    fseek(flash_file, 0, IO_SEEK_END);
    swap_size = ftell(flash_file);
    fclose(flash_file);

    /* return true if application space can fit into swap space */
    return (used_size > swap_size ? 0 : 1);
}


/*TASK*-----------------------------------------------------
*
* Task Name    : clone_application
* Return       : 1 if success, 0 if failed
* Comments     :
*    Clone this application to "swap1" file.
*
*END*-----------------------------------------------------*/

uint32_t clone_application(void)
{
    char * source_buffer = NULL, * target_buffer = NULL;
    MQX_FILE_PTR source_file = NULL, target_file = NULL;
    uint32_t read_size, write_size, result = 1;
    uint32_t position = 0;

/* allocate resources */
    source_buffer = (char *)_mem_alloc_zero(BUFFER_SIZE);
    if (NULL == source_buffer)
    {
        result = 0;
    }
    target_buffer = (char *)_mem_alloc_zero(BUFFER_SIZE);
    if (NULL == target_buffer)
    {
        result = 0;
    }
    source_file = fopen("flashx:code", NULL);
    if (NULL == source_file)
    {
        result = 0;
    }
    target_file = fopen("flashx:swap1", NULL);
    if (NULL == target_file)
    {
        result = 0;
    }

/* clean target file of no error occurred */
    if (result)
    {
        fseek(target_file, 0, IO_SEEK_SET);
        ioctl(target_file, FLASH_IOCTL_ERASE_FILE, NULL);
    }

/* clone source file and target file */
    while (result)
    {
        /* read source_buffer of source_file */
        fseek(source_file, position, IO_SEEK_SET);
        read_size = read(source_file, source_buffer, BUFFER_SIZE);

        /* end loop if whole file was readed */
        if (!read_size)
        {
            break;
        }

        /* write source_buffer to target_file */
        fseek(target_file, position, IO_SEEK_SET);
        write_size = write(target_file, source_buffer, read_size);
        if (write_size != read_size)
        {
            return 0;
        }

        /* read written data to target_buffer */
        fseek(target_file, position, IO_SEEK_SET);
        write_size = read(target_file, target_buffer, read_size);
        if (write_size != read_size)
        {
            return 0;
        }

        /* verify written data by buffer comparsion */
        if (strncmp(source_buffer, target_buffer, read_size))
        {
            return 0;
        }

        /* update file position */
        position += read_size;
    }

/* free resources */
    if (source_file != NULL)
    {
        fclose(source_file);
    }
    if (target_file != NULL)
    {
        fclose(target_file);
    }
    if (source_buffer != NULL)
    {
        _mem_free(source_buffer);
    }
    if (target_buffer != NULL)
    {
        _mem_free(target_buffer);
    }

/* return status */
    return result;
}


/*TASK*-----------------------------------------------------
*
* Task Name    : write_swapmark
* Return       : 1 if success, 0 if failed
* Comments     :
*    Write swapmark to "swap1" file.
*
*END*-----------------------------------------------------*/

uint32_t write_swapmark(uint32_t swapmark_size, void * swapmark_data)
{
    MQX_FILE_PTR swap_file = NULL;
    uint32_t swap_size = 0;

    swap_file = fopen("flashx:swap1", NULL);
    if (NULL != swap_file)
    {
        fseek(swap_file, -swapmark_size, IO_SEEK_END);
        swap_size = write(swap_file, swapmark_data, swapmark_size);
        if (swapmark_size != swap_size)
        {
            return 1;
        }
        fclose(swap_file);
        return 1;
    }
    return 0;
}


/*TASK*-----------------------------------------------------
*
* Task Name    : read_swapmark
* Return       : 1 if success, 0 if failed
* Comments     :
*    Read swapmark from "swap0" file.
*
*END*-----------------------------------------------------*/

uint32_t read_swapmark(uint32_t swapmark_size, void * swapmark_data)
{
    MQX_FILE_PTR swap_file = NULL;
    uint32_t swap_size = 0;

    swap_file = fopen("flashx:swap0", NULL);
    if (NULL != swap_file)
    {
        fseek(swap_file, -swapmark_size, IO_SEEK_END);
        swap_size = read(swap_file, swapmark_data, swapmark_size);
        if (swapmark_size != swap_size)
        {
            return 0;
        }
        fclose(swap_file);
        return 1;
    }
    return 0;
}


/*TASK*-----------------------------------------------------
*
* Task Name    : do_verify_and_clone
* Comments     :
*    Verify and clone application
*
*END*-----------------------------------------------------*/

void do_verify_and_clone(void)
{
    printf("log : flash space verification start \n");
    if (!verify_flashspace())
    {
        printf("error: flash space verification failed \n");
        _task_block();
    }
    else
    {
        printf("log : flash space verification done \n");
    }

    printf("log : copying application start \n");
    if (!clone_application())
    {
        printf("error: copying application failed \n");
        _task_block();
    }
    else
    {
        printf("log : copying application done \n");
    }
}


/*TASK*-----------------------------------------------------
*
* Task Name    : do_write_message
* Comments     :
*    Get custom message and write swapmark
*
*END*-----------------------------------------------------*/

void do_write_message(swapmark_struct * swapmark)
{
    printf("put your message ( max %d characters ): \n", (sizeof(swapmark->message) - 1));
    fgets(swapmark->message, sizeof(swapmark->message), stdin);
    printf("log : writing message to upper flash space - 'SWAP1' file \n");
    strncpy(swapmark->uid, "swap", 4);
    if (!write_swapmark(sizeof(*swapmark), swapmark))
    {
        printf("error: writing swapmark message \n");
        _task_block();
    }
}


/*TASK*-----------------------------------------------------
*
* Task Name    : do_flash_swap
* Comments     :
*    Swap flash spaces
*
*END*-----------------------------------------------------*/

void do_flash_swap(void)
{
    MQX_FILE_PTR swap_file = NULL;

    printf("log : swapping flash space \n");
    swap_file = fopen("flashx:swap0", NULL);
    if (NULL != swap_file)
    {
        ioctl(swap_file, FLASH_IOCTL_SWAP_FLASH_AND_RESET, NULL);
        // this code is executed only if FLASH_IOCTL_SWAP_FLASH_AND_RESET failed
        fclose(swap_file);
    }

    printf("error: flash swap failed\n");
    _task_block();
}


/*TASK*-----------------------------------------------------
*
* Task Name    : flash_task
* Comments     :
*   This task write an user message to upper
*   flashspace and then perform swap_and_reset.
*   After reset the application read the message
*   from lower flash space.
*
*END*-----------------------------------------------------*/

void flash_task
   (
      uint32_t initial_data
   )
{
    swapmark_struct swapmark;
    _int_install_unexpected_isr();

    while (1)
    {
        read_swapmark(sizeof(swapmark), &swapmark);        
        if (!strncmp(swapmark.uid, "swap", 4))
        {
            printf("-----------------------------------------------------------\n");
            printf("running application after swap (swapflash found) \n");
            printf("reading message from lower flash space - 'SWAP0' file \n");
            printf("your message is:\n\n %s \n\n", swapmark.message);
            do_verify_and_clone();
            do_write_message(&swapmark);
            do_flash_swap();
        }
        else
        {
            printf("-----------------------------------------------------------\n");
            printf("running application for the first time (no swapflash mark was found) \n");
            do_verify_and_clone();
            do_write_message(&swapmark);
            do_flash_swap();
        }
    }

   _task_block();
}

/* EOF */
