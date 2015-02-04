/*HEADER**********************************************************************
*
* Copyright 2010 Freescale Semiconductor, Inc.
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
#include "bsp_prv.h"
#include "flashx.h"
#include "flashxprv.h"
#include "flash_ftfl_prv.h"
#include "flash_ftfl.h"

/* local function prototypes */
static void     ftfl_ram_function(volatile uint8_t *, void (_CODE_PTR_)(volatile uint32_t));
static void     ftfl_ram_function_end(void);
static char *ftfl_init_ram_function(char *, char *);
static _mqx_int ftfl_deinit_ram_function(char *);
static uint32_t  ftfl_flash_command_sequence(FTFL_FLASH_INTERNAL_STRUCT_PTR, uint8_t *, uint8_t, bool, void*, uint32_t);

static bool  flexnvm_read_resource(IO_FLASHX_STRUCT_PTR, FLEXNVM_READ_RSRC_STRUCT_PTR);
static bool  flexnvm_prog_part(IO_FLASHX_STRUCT_PTR, FLEXNVM_PROG_PART_STRUCT_PTR);
static bool  flexnvm_set_flexram(IO_FLASHX_STRUCT_PTR, unsigned char);

uint32_t ftfl_flash_swap_command(FTFL_FLASH_INTERNAL_STRUCT_PTR, uint32_t, uint8_t);
uint32_t ftfl_flash_swap_init_update(IO_FLASHX_STRUCT_PTR, uint32_t);
uint32_t ftfl_flash_swap_complete(IO_FLASHX_STRUCT_PTR, uint32_t);
uint32_t ftfl_flash_swap_status(IO_FLASHX_STRUCT_PTR, uint32_t , uint8_t *);
uint32_t ftfl_flash_swap_and_reset(IO_FLASHX_STRUCT_PTR);

const FLASHX_BLOCK_INFO_STRUCT _flashx_kinetisN_block_map[] = {
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE / 2,(_mem_size) BSP_INTERNAL_FLASH_BASE                              , BSP_INTERNAL_FLASH_SECTOR_SIZE, 0 },
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE / 2,(_mem_size) BSP_INTERNAL_FLASH_BASE + BSP_INTERNAL_FLASH_SIZE / 2, BSP_INTERNAL_FLASH_SECTOR_SIZE, 0 },
    { 0, 0, 0 }
};

#ifdef BSP_INTERNAL_FLEXRAM_BASE
const FLASHX_BLOCK_INFO_STRUCT _flashx_kinetisX_block_map[] = {
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE,(_mem_size) BSP_INTERNAL_FLASH_BASE, BSP_INTERNAL_FLASH_SECTOR_SIZE },
    { 1, (_mem_size) BSP_INTERNAL_FLEXRAM_BASE, BSP_INTERNAL_FLEXRAM_SIZE },
    { 0, 0, 0 }
};

const FLASHX_BLOCK_INFO_STRUCT _flashx_mcf51xx_plus_block_map[] =  {
    { BSP_INTERNAL_FLASH_SIZE / BSP_INTERNAL_FLASH_SECTOR_SIZE,(_mem_size) BSP_INTERNAL_FLASH_BASE, BSP_INTERNAL_FLASH_SECTOR_SIZE },
    { 1, (_mem_size) BSP_INTERNAL_FLEXRAM_BASE, BSP_INTERNAL_FLEXRAM_SIZE },
    { 0, 0, 0 }
};
#endif

const FLASHX_DEVICE_IF_STRUCT _flashx_ftfl_if = {
    ftfl_flash_erase_sector,
#if BSPCFG_FLASHX_USE_PA_RAM
    ftfl_flash_write_sector_pa_ram,
#else
    ftfl_flash_write_sector,
#endif
    NULL,
    NULL,
    ftfl_flash_init,
    ftfl_flash_deinit,
    NULL,
    flexnvm_flash_ioctl
};

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_flash_command_sequence
* Returned Value   : uint32_t an error code
* Comments         :
*    Run command in FTFL device.
*
*END*-----------------------------------------------------------------------*/
static uint32_t ftfl_flash_command_sequence
(
    /* [IN] Flash specific structure */
    FTFL_FLASH_INTERNAL_STRUCT_PTR     dev_spec_ptr,

    /* [IN] Command byte array */
    uint8_t                         *command_array,

    /* [IN] Number of values in the array */
    uint8_t                             count,

    /* [IN] Read the result back? */
    bool                            read,

    /* [IN] The address which will be affected by command */
    void                                *affected_addr,

    /* [IN] The address which will be affected by command */
    uint32_t                            affected_size

)
{
    volatile uint8_t  fstat;
    uint32_t result;
    void (* RunInRAM)(volatile uint8_t *, void (_CODE_PTR_) (volatile uint32_t));
    void (* RunInvalidateInRAM)(volatile uint32_t);
    RunInvalidateInRAM = NULL;
    
    FTFL_MemMapPtr ftfl_ptr;
    uint8_t cnt;
#if PSP_MQX_CPU_IS_COLDFIRE
    uint32_t temp;
#endif

    ftfl_ptr = (FTFL_MemMapPtr)dev_spec_ptr->ftfl_ptr;

    /* get pointer to RunInRAM function */
    RunInRAM = (void(*)(volatile uint8_t *, void (_CODE_PTR_)(volatile uint32_t)))(dev_spec_ptr->flash_execute_code_ptr);
#if PSP_MQX_CPU_IS_KINETIS
    RunInvalidateInRAM = (void(*)(volatile uint32_t))(dev_spec_ptr->flash_invalidate_code_ptr);
#endif

    /* set the default return as FTFL_OK */
    result = FTFL_OK;

    /* check CCIF bit of the flash status register */
    while (0 == (ftfl_ptr->FSTAT & FTFL_FSTAT_CCIF_MASK))
    { /* void */ }

    /* clear RDCOLERR & ACCERR & FPVIOL error flags in flash status register */
    if (ftfl_ptr->FSTAT & FTFL_FSTAT_RDCOLERR_MASK)
    {
        ftfl_ptr->FSTAT |= FTFL_FSTAT_RDCOLERR_MASK;
    }
    if (ftfl_ptr->FSTAT & FTFL_FSTAT_ACCERR_MASK)
    {
        ftfl_ptr->FSTAT |= FTFL_FSTAT_ACCERR_MASK;
    }
    if (ftfl_ptr->FSTAT & FTFL_FSTAT_FPVIOL_MASK)
    {
        ftfl_ptr->FSTAT |= FTFL_FSTAT_FPVIOL_MASK;
    }

    cnt = count;
    switch (cnt)
    {

        case 12: ftfl_ptr->FCCOBB = command_array[--cnt];
        case 11: ftfl_ptr->FCCOBA = command_array[--cnt];
        case 10: ftfl_ptr->FCCOB9 = command_array[--cnt];
        case  9: ftfl_ptr->FCCOB8 = command_array[--cnt];
        case  8: ftfl_ptr->FCCOB7 = command_array[--cnt];
        case  7: ftfl_ptr->FCCOB6 = command_array[--cnt];
        case  6: ftfl_ptr->FCCOB5 = command_array[--cnt];
        case  5: ftfl_ptr->FCCOB4 = command_array[--cnt];
        case  4: ftfl_ptr->FCCOB3 = command_array[--cnt];
        case  3: ftfl_ptr->FCCOB2 = command_array[--cnt];
        case  2: ftfl_ptr->FCCOB1 = command_array[--cnt];
        case  1: ftfl_ptr->FCCOB0 = command_array[--cnt];
        default: break;
    }

#if PSP_MQX_CPU_IS_COLDFIRE
        temp = _psp_get_sr();
        _psp_set_sr(temp | 0x0700);
#elif PSP_MQX_CPU_IS_KINETIS
    __disable_interrupt ();
#endif /* PSP_MQX_CPU_IS_KINETIS */

    /* run command and wait for it to finish (must execute from RAM) */
    RunInRAM(&ftfl_ptr->FSTAT, (void (_CODE_PTR_)(volatile uint32_t))RunInvalidateInRAM);



    cnt = count;
    if (read) {
        switch (cnt)
        {
            case 12: command_array[--cnt] = ftfl_ptr->FCCOBB;
            case 11: command_array[--cnt] = ftfl_ptr->FCCOBA;
            case 10: command_array[--cnt] = ftfl_ptr->FCCOB9;
            case  9: command_array[--cnt] = ftfl_ptr->FCCOB8;
            case  8: command_array[--cnt] = ftfl_ptr->FCCOB7;
            case  7: command_array[--cnt] = ftfl_ptr->FCCOB6;
            case  6: command_array[--cnt] = ftfl_ptr->FCCOB5;
            case  5: command_array[--cnt] = ftfl_ptr->FCCOB4;
            case  4: command_array[--cnt] = ftfl_ptr->FCCOB3;
            case  3: command_array[--cnt] = ftfl_ptr->FCCOB2;
            case  2: command_array[--cnt] = ftfl_ptr->FCCOB1;
            case  1: command_array[--cnt] = ftfl_ptr->FCCOB0;
            default: break;
        }
    }
    /* get flash status register value */
    fstat = ftfl_ptr->FSTAT;

/* 
invalidate data cache of 'affected_addr' address and 'affected_size' size
because reading flash through code-bus may show incorrect data
*/
#if defined(_DCACHE_INVALIDATE_MLINES) || defined(_ICACHE_INVALIDATE_MLINES)
    if (affected_size)
    {
#if defined(_DCACHE_INVALIDATE_MLINES)
        _DCACHE_INVALIDATE_MLINES(affected_addr, affected_size);
#endif
#if defined(_ICACHE_INVALIDATE_MLINES)
        _ICACHE_INVALIDATE_MLINES(affected_addr, affected_size);
#endif
    }
#endif

#if PSP_MQX_CPU_IS_COLDFIRE
    _psp_set_sr(temp);
#elif PSP_MQX_CPU_IS_KINETIS
    __enable_interrupt ();
#endif /* PSP_MQX_CPU_IS_KINETIS */

    /* checking access error */
    if (0 != (fstat & FTFL_FSTAT_ACCERR_MASK))
    {
        /* return an error code FTFL_ERR_ACCERR */
        result = FTFL_ERR_ACCERR;
    }
    /* checking protection error */
    else if (0 != (fstat & FTFL_FSTAT_FPVIOL_MASK))
    {
        /* return an error code FTFL_ERR_PVIOL */
        result = FTFL_ERR_PVIOL;
    }
    /* checking MGSTAT0 non-correctable error */
    else if (0 != (fstat & FTFL_FSTAT_MGSTAT0_MASK))
    {
        /* return an error code FTFL_ERR_MGSTAT0 */
        result = FTFL_ERR_MGSTAT0;
    }

    return result;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_flash_init
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Initialize flash specific information.
*
*END*-----------------------------------------------------------------------*/
bool ftfl_flash_init
(
    /* [IN] Device instance */
    IO_FLASHX_STRUCT_PTR dev_ptr
)
{
    FTFL_FLASH_INTERNAL_STRUCT_PTR    dev_spec_ptr;
    FTFL_MemMapPtr                    ftfl_ptr;

    if (_bsp_ftfx_io_init(0)) {
        /* Cannot initialize FTF module */
        return FALSE;
    }

    /* allocate internal structure */
    dev_spec_ptr = _mem_alloc(sizeof(FTFL_FLASH_INTERNAL_STRUCT));
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (dev_spec_ptr == NULL)
    {
        return FALSE;
    }
#endif
    dev_ptr->DEVICE_SPECIFIC_DATA = dev_spec_ptr;

    /* get the pointer to ftfl registers structure */
    ftfl_ptr = (FTFL_MemMapPtr)_bsp_get_ftfl_address();
    dev_spec_ptr->ftfl_ptr = (volatile char *)ftfl_ptr;

    /* save pointer to function in ram */
    dev_spec_ptr->flash_execute_code_ptr = ftfl_init_ram_function((char *)ftfl_ram_function, (char *)ftfl_ram_function_end);
#if PSP_MQX_CPU_IS_KINETIS
    dev_spec_ptr->flash_invalidate_code_ptr = ftfl_init_ram_function((char *)kinetis_flash_invalidate_cache, (char *)kinetis_flash_invalidate_cache_end);
#endif

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (dev_spec_ptr->flash_execute_code_ptr == NULL)
    {
        return FALSE;
    }
#if PSP_MQX_CPU_IS_KINETIS
    if (dev_spec_ptr->flash_invalidate_code_ptr == NULL)
    {
        return FALSE;
    }
#endif
#endif

#if BSPCFG_FLASHX_USE_PA_RAM && BSP_INTERNAL_FLEXRAM_BASE
    /* Make sure Programing Acceleration RAM was enabled */
    flexnvm_set_flexram(dev_ptr, (unsigned char)FLEXNVM_FLEXRAM_RAM);
#endif

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_flash_deinit
* Returned Value   : none
* Comments         :
*   Release flash specific information.
*
*END*-----------------------------------------------------------------------*/
void ftfl_flash_deinit
(
    /* [IN] Device instance */
    IO_FLASHX_STRUCT_PTR dev_ptr
)
{
    FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (FTFL_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA;

    /* de-allocate the ram function buffer */
    ftfl_deinit_ram_function(dev_spec_ptr->flash_execute_code_ptr);
#if PSP_MQX_CPU_IS_KINETIS
    ftfl_deinit_ram_function(dev_spec_ptr->flash_invalidate_code_ptr);
#endif

    /* de-allocate the device specific structure */
    _mem_free(dev_spec_ptr);

    dev_ptr->DEVICE_SPECIFIC_DATA = NULL;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_init_ram_function
* Returned Value   : pointer to allocated RAM function
* Comments         :
*   Allocate and copy flash RAM function.
*
*END*-----------------------------------------------------------------------*/
char *ftfl_init_ram_function
(
    char *function_start,
    char *function_end
)
{
    char *ram_code_ptr;
    _mem_size ftfl_ram_function_start;

    /* remove thumb2 flag from the address and align to word size */
    ftfl_ram_function_start = (_mem_size)function_start & ~3;

    /* allocate space to run flash command out of RAM */
    ram_code_ptr = _mem_alloc_align((char *)function_end - (char *)ftfl_ram_function_start, 4);

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if (ram_code_ptr == NULL)
    {
        return NULL;
    }
#endif

    /* aopy code to RAM buffer */
    _mem_copy ((char *)ftfl_ram_function_start, ram_code_ptr, (char *)function_end - (char *)ftfl_ram_function_start);

    /* adjust address with respect to the original alignment */
    ram_code_ptr = (char *)((_mem_size)ram_code_ptr | ((_mem_size)function_start & 3));

    return ram_code_ptr;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_deinit_ram_function
* Returned Value   : MQX_OK or an error code 
* Comments         :
*   Free up flash RAM function, if any.
*
*END*-----------------------------------------------------------------------*/
_mqx_int ftfl_deinit_ram_function
(
    char *ram_code_ptr
)
{
    if (NULL != ram_code_ptr)
    {
        return _mem_free((void *)((_mem_size)ram_code_ptr & ~3));
    }

    return MQX_OK;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_flash_erase_sector
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Erase a flash memory block.
*
*END*-----------------------------------------------------------------------*/
bool ftfl_flash_erase_sector
(
    /* [IN] Device instance */
    IO_FLASHX_STRUCT_PTR dev_ptr,

    /* [IN] Erased sector address */
    char             *from_ptr,

    /* [IN] Erased sector size */
    _mem_size            size
)
{
    FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (FTFL_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA;
    _mqx_uint                         i;
    _mem_size                         write_addr;
    uint8_t                            command_array[4];

    for (i = 0; dev_ptr->HW_BLOCK[i].NUM_SECTORS != 0; i++)
    {
        if (((uint32_t)from_ptr >= dev_ptr->HW_BLOCK[i].START_ADDR) &&
            ((uint32_t)from_ptr <= dev_ptr->HW_BLOCK[i].START_ADDR + dev_ptr->HW_BLOCK[i].SECTOR_SIZE * dev_ptr->HW_BLOCK[i].NUM_SECTORS))
        {
             /* check if the from_ptr is sector aligned or not */
             if ((uint32_t)from_ptr % dev_ptr->HW_BLOCK[i].SECTOR_SIZE)
                return FALSE;
             /* check if the size is sector aligned or not */
             if ((uint32_t)size % dev_ptr->HW_BLOCK[i].SECTOR_SIZE)
                return FALSE;
             break;
        }
    }

    write_addr = (_mem_size) from_ptr;

    /* preparing passing parameter to erase a flash block */
    command_array[0] = FTFL_ERASE_SECTOR;
    command_array[1] = (uint8_t)(write_addr >> 16);
    command_array[2] = (uint8_t)((write_addr >> 8) & 0xFF);
    command_array[3] = (uint8_t)(write_addr & 0xFF);

    /* call flash command sequence function to execute the command */
    if (FTFL_OK != ftfl_flash_command_sequence (dev_spec_ptr, command_array, 4, FALSE, (void*)write_addr, size))
    {
        return FALSE;
    }

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_flash_write_sector
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Performs a write into flash memory.
*
*END*-----------------------------------------------------------------------*/
bool ftfl_flash_write_sector
(
    /* [IN] Device instance */
    IO_FLASHX_STRUCT_PTR dev_ptr,

    /* [IN] Source address */
    char             *from_ptr,

    /* [IN] Destination address */
    char             *to_ptr,

    /* [IN] Number of bytes to write */
    _mem_size            size
)
{
    FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (FTFL_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA;
    _mqx_uint                         i;
    _mem_size                         write_addr;
    uint8_t                            command_array[4 + FTFL_LONGWORD_SIZE];
    char                              temp_data[FTFL_LONGWORD_SIZE];
    unsigned char                             byte_data_counter = 0;
    uint32_t                           offset = ((uint32_t)to_ptr) & 0x00000003;

    dev_spec_ptr = dev_ptr->DEVICE_SPECIFIC_DATA;

    /* write to address mod 4 correction */
    if (offset)
    {
        /* align pointer to writable address */
        to_ptr -= offset;

        /* jump over old data */
        byte_data_counter = offset;
    }

    write_addr = (_mem_size) to_ptr;

    /* heading space should be 0xFF */
    for (i = 0; i < offset; i++)
        temp_data[i] = 0xFF;

    while (size)
    {
        /* move data to temporary char array */
        while ((byte_data_counter < FTFL_LONGWORD_SIZE) && size)
        {
            temp_data[byte_data_counter++] = *from_ptr++;
            size--;
        }

        /* remaining space should be 0xFF */
        while (byte_data_counter < FTFL_LONGWORD_SIZE) {
            temp_data[byte_data_counter++] = 0xFF;
        }

        /* prepare parameters to program the flash block */
        command_array[0] = FTFL_PROGRAM_LONGWORD;
        command_array[1] = (uint8_t)(write_addr >> 16);
        command_array[2] = (uint8_t)((write_addr >> 8) & 0xFF);
        command_array[3] = (uint8_t)(write_addr & 0xFF);

#if PSP_ENDIAN == MQX_BIG_ENDIAN
        command_array[4] = temp_data[0];
        command_array[5] = temp_data[1];
        command_array[6] = temp_data[2];
        command_array[7] = temp_data[3];
#else
        command_array[4] = temp_data[3];
        command_array[5] = temp_data[2];
        command_array[6] = temp_data[1];
        command_array[7] = temp_data[0];
#endif /* PSP_ENDIAN */

        /* call flash command sequence function to execute the command */
        if (FTFL_OK != ftfl_flash_command_sequence (dev_spec_ptr, command_array, 4 + FTFL_LONGWORD_SIZE, FALSE, (void*)write_addr, FTFL_LONGWORD_SIZE))
        {
            return FALSE;
        }

        /* update destination address for next iteration */
        write_addr += FTFL_LONGWORD_SIZE;
        /* init variables for next loop */
        byte_data_counter = 0;
    }

    return TRUE;
}

#if BSPCFG_FLASHX_USE_PA_RAM
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : ftfl_flash_write_sector_acceleration_ram
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Performs a write into flash memory.
*
*END*----------------------------------------------------------------------*/
bool ftfl_flash_write_sector_pa_ram
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
    FTFL_FLASH_INTERNAL_STRUCT_PTR  dev_spec_ptr = (FTFL_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA; 
    FTFL_MemMapPtr                  ftfl_ptr;
    _mem_size                       write_addr = 0;
    uint8_t                         command_array[6];
    _mem_size                       number_of_phrases = 0;
    uint32_t                        i = 0;

    dev_spec_ptr = dev_ptr->DEVICE_SPECIFIC_DATA;

    /* get the pointer to ftfe registers structure */
    ftfl_ptr = (FTFL_MemMapPtr)dev_spec_ptr->ftfl_ptr;   
    
    write_addr = (_mem_size) to_ptr;

    /* check acceleration RAM */
    if (!(ftfl_ptr->FCNFG & FTFL_FCNFG_RAMRDY_MASK))
    {
        return FALSE;
    }
    
    /* check alignment of destination */
    if (write_addr & (FTFL_PROGRAM_SECTION_ALIGNMENT - 1))
    {
        return FALSE;
    }
    
    /* check input size */
    if (size % FTFL_PROGRAM_SECTION_ALIGNMENT == 0)
    {
        number_of_phrases = size / FTFL_PROGRAM_SECTION_ALIGNMENT;
    }
    else
    {
        number_of_phrases = (size / FTFL_PROGRAM_SECTION_ALIGNMENT) + 1;
    }
    
    /* copy data to acceleration RAM */
    _mem_copy(from_ptr, (char *)BSP_INTERNAL_PA_RAM_BASE, size);
        
    /* remaining space should be 0xFF */
    for (i = size; i < (number_of_phrases * FTFL_PROGRAM_SECTION_ALIGNMENT); i++)
    {
        *((char *)(BSP_INTERNAL_PA_RAM_BASE + i)) = 0xFF;
    }
    
    /* preparing passing parameter to program section */
    command_array[0] = FTFL_PROGRAM_SECTION;
    command_array[1] = (uint8_t)((write_addr >> 16) & 0xFF);
    command_array[2] = (uint8_t)((write_addr >> 8) & 0xFF);
    command_array[3] = (uint8_t)(write_addr & 0xFF);
    command_array[4] = (uint8_t)((number_of_phrases >> 8) & 0xFF);
    command_array[5] = (uint8_t)(number_of_phrases & 0xFF);
    
    /* call flash command sequence function to execute the command */
    if (FTFL_OK != ftfl_flash_command_sequence (dev_spec_ptr, command_array, 6, FALSE, (void*)write_addr, FTFL_PROGRAM_SECTION_ALIGNMENT))
    {
        return FALSE;
    }
    
    return TRUE;
}
#endif

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_ram_function
* Returned Value   : void
* Comments         :
*   Code required to run in SRAM to perform flash commands.
*   All else can be run in flash.
*   Parameter is an address of flash status register and function to invalidate cache.
*
*END*-----------------------------------------------------------------------*/
static void ftfl_ram_function
(
    /* [IN] Flash info structure */
    volatile uint8_t *ftfl_fstat_ptr,
    /* [IN] Pointer to function of invalidate cache*/
    void (* invalidate_cache)(volatile uint32_t)
)
{
    /* start flash write */
    *ftfl_fstat_ptr |= FTFL_FSTAT_CCIF_MASK;

    /* wait until execution complete */
    while (0 == ((*ftfl_fstat_ptr) & FTFL_FSTAT_CCIF_MASK))
    { /* void */ }
    
    if(invalidate_cache != NULL)
    {
        invalidate_cache((uint32_t)FLASHX_INVALIDATE_CACHE_ALL);
    }
    /* Flush the pipeline and ensures that all previous instructions are completed
     * before executing new instructions in flash */
#ifdef ISB
    ISB();
#endif
#ifdef DSB
    DSB();
#endif
}

static void ftfl_ram_function_end(void)
{}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : flexnvm_read_resource
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Program Partition Command
*
*END*-----------------------------------------------------------------------*/
static bool flexnvm_read_resource
(
    /* [IN] Device instance */
    IO_FLASHX_STRUCT_PTR         dev_ptr,

    /* [IN] Pointer to resource to be read */
    FLEXNVM_READ_RSRC_STRUCT_PTR param_ptr
)
{
    FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (FTFL_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA;
    uint8_t command_array[9];

    /* preparing passing parameter to erase a flash block */
    command_array[0] = FTFL_READ_RESOURCE;
    command_array[1] = (uint8_t)((param_ptr->ADDR >> 16) & 0xFF);
    command_array[2] = (uint8_t)((param_ptr->ADDR >> 8) & 0xFF);
    command_array[3] = (uint8_t)(param_ptr->ADDR & 0xFF);

    command_array[8] = (uint8_t)(param_ptr->RSRC_CODE);

    /* call flash command sequence function to execute the command */
    if (FTFL_OK != ftfl_flash_command_sequence(dev_spec_ptr, command_array, 9, TRUE, NULL, 0)) {
        return FALSE;
    }

    param_ptr->RD_DATA = *(uint32_t*)(&command_array[4]);

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : flexnvm_prog_part
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Program Partition Command
*
*END*-----------------------------------------------------------------------*/
static bool flexnvm_prog_part
(
    /* [IN] Device instance */
    IO_FLASHX_STRUCT_PTR         dev_ptr,

    /* [IN] Pointer to the structure containing partitioning info */
    FLEXNVM_PROG_PART_STRUCT_PTR param_ptr
)
{
    FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (FTFL_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA;
    uint8_t command_array[6];

    /* preparing passing parameter to erase a flash block */
    command_array[0] = FTFL_PROGRAM_PARTITION;
    command_array[1] = 0;
    command_array[2] = 0;
    command_array[3] = 0;
    command_array[4] = param_ptr->EE_DATA_SIZE_CODE;
    command_array[5] = param_ptr->FLEXNVM_PART_CODE;

    /* call flash command sequence function to execute the command */
    if (FTFL_OK != ftfl_flash_command_sequence(dev_spec_ptr, command_array, 6, FALSE, NULL, 0)) {
        return FALSE;
    }

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : flexnvm_set_flexram
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Program Partition Command
*
*END*----------------------------------------------------------------------*/
static bool flexnvm_set_flexram
(
    /* [IN] Device instance */
    IO_FLASHX_STRUCT_PTR dev_ptr,

    /* [IN] Command code */
    unsigned char ctrl_code
)
{
    FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = (FTFL_FLASH_INTERNAL_STRUCT_PTR) dev_ptr->DEVICE_SPECIFIC_DATA;
    uint8_t command_array[2];

    /* prepare to pass the parameter to set FlexRAM */
    command_array[0] = FTFL_SET_EERAM;
    command_array[1] = ctrl_code;

    /* call flash command sequence function to execute the command */
    if (FTFL_OK != ftfl_flash_command_sequence (dev_spec_ptr, command_array, 2, FALSE, NULL, 0)) {
        return FALSE;
    }

    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : flexnvm_flash_ioctl
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Program Partition Command
*
*END*----------------------------------------------------------------------*/
_mqx_int flexnvm_flash_ioctl
(
    /* [IN] Device instance */
    IO_FLASHX_STRUCT_PTR dev_ptr,

    /* [IN] the ioctl command */
    _mqx_uint cmd,

    /* [IN] the ioctl command parameter */
    void   *param_ptr
)
{
    _mqx_int result = IO_OK;

    switch (cmd) {
        case FLEXNVM_IOCTL_READ_RESOURCE:
            flexnvm_read_resource(dev_ptr, (FLEXNVM_READ_RSRC_STRUCT_PTR)param_ptr);
            break;
        case FLEXNVM_IOCTL_GET_PARTITION_CODE: {
                FLEXNVM_READ_RSRC_STRUCT rsrc_param;

                rsrc_param.ADDR = 0x8000fc;
                rsrc_param.RSRC_CODE = 0;

                if (flexnvm_read_resource(dev_ptr, &rsrc_param) && NULL != param_ptr) {
                    ((FLEXNVM_PROG_PART_STRUCT_PTR)param_ptr)->EE_DATA_SIZE_CODE = (rsrc_param.RD_DATA >> 8) & (FLEXNVM_EE_SPLIT_MASK | FLEXNVM_EE_SIZE_MASK);
                    ((FLEXNVM_PROG_PART_STRUCT_PTR)param_ptr)->FLEXNVM_PART_CODE = rsrc_param.RD_DATA & FLEXNVM_PART_CODE_MASK;
                }
                else {
                    result = IO_ERROR_READ;
                }

                break;
            }
        case FLEXNVM_IOCTL_SET_PARTITION_CODE:
            flexnvm_prog_part(dev_ptr, (FLEXNVM_PROG_PART_STRUCT_PTR)param_ptr);
            break;
        case FLEXNVM_IOCTL_SET_FLEXRAM_FN:
            flexnvm_set_flexram(dev_ptr, *(unsigned char*)param_ptr);
            break;
        case FLEXNVM_IOCTL_WAIT_EERDY: {
                FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = dev_ptr->DEVICE_SPECIFIC_DATA;
                FTFL_MemMapPtr ftfl_ptr = (FTFL_MemMapPtr)dev_spec_ptr->ftfl_ptr;

                while (!(ftfl_ptr->FCNFG & FTFL_FCNFG_EEERDY_MASK)){}

                break;
            }
        case FLEXNVM_IOCTL_GET_EERDY: {
                FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr = dev_ptr->DEVICE_SPECIFIC_DATA;
                FTFL_MemMapPtr ftfl_ptr = (FTFL_MemMapPtr)dev_spec_ptr->ftfl_ptr;

                if (NULL != param_ptr)
                    *(uint32_t*)param_ptr = (ftfl_ptr->FCNFG & FTFL_FCNFG_EEERDY_MASK);

                break;
            }
#if FTFL_SWAP_SUPPORT
        case FLASH_IOCTL_SWAP_FLASH_AND_RESET:
            result = ftfl_flash_swap_and_reset(dev_ptr);
            break;
#endif
        default:
            result = IO_ERROR_INVALID_IOCTL_CMD;
            break;
    }

    return result;
}

#if FTFL_SWAP_SUPPORT

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_flash_swap_command
* Returned Value   : 0 if successful, error_code otherwise
* Comments         :
*   Implement swap command.
*
*END*----------------------------------------------------------------------*/
uint32_t ftfl_flash_swap_command
(
    /* [IN] Flash info structure */
    FTFL_FLASH_INTERNAL_STRUCT_PTR dev_spec_ptr,

    /* [IN] Flash swap indicator address */
    uint32_t addr,

    /* [IN] Flash swap command */
    uint8_t swapcmd
)
{
    uint8_t command_array[8]; /* Command sequence array */
    uint32_t result;

    /* Preparing passing parameter to execute the swap control code */
    command_array[0] = FTFL_PFLASH_SWAP;
    command_array[1] = (uint8_t)((addr & 0x00FF0000) >> 16);
    command_array[2] = (uint8_t)((addr & 0x0000FF00) >> 8);
    command_array[3] = (uint8_t)((addr & 0x000000FF));
    command_array[4] = swapcmd;
    command_array[5] = 0xFF;
    command_array[6] = 0xFF;
    command_array[7] = 0xFF;

    /* call flash command sequence function to execute the command */
    result = ftfl_flash_command_sequence (dev_spec_ptr, command_array, 8, FALSE, NULL, 0);

    return result;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_flash_swap_status
* Returned Value   : 0 if successful, error_code otherwise
* Comments         :
*   Check Flash Swap Status and pass back.
*
*END*----------------------------------------------------------------------*/
uint32_t ftfl_flash_swap_status
(
   /* [IN] Device instance */
   IO_FLASHX_STRUCT_PTR   dev_ptr,

   /* [IN] Flash swap indicator address */
   uint32_t               flash_address,

   /* [OUT] swap status */
   uint8_t                * swapmode
)
{
    FTFL_FLASH_INTERNAL_STRUCT_PTR     dev_spec_ptr;
    FTFL_MemMapPtr                     ftfl_ptr;
    uint32_t                           result;
    uint8_t                            current_swap_mode = 0xFF;

    dev_spec_ptr = (FTFL_FLASH_INTERNAL_STRUCT_PTR)(dev_ptr->DEVICE_SPECIFIC_DATA);
    ftfl_ptr = (FTFL_MemMapPtr)(dev_spec_ptr->ftfl_ptr);

    /* Set the default return code as FTFL_OK */
    result = FTFL_OK;

     /* Get current swap report */
    result = ftfl_flash_swap_command(dev_spec_ptr, flash_address, FTFL_SWAP_REPORT_STATUS);
    /* Check for the success of command execution */
    if (FTFL_OK != result)
    {
        return (result);
    }

    /* Get current swap mode from FCCOB5 register */
    current_swap_mode = ftfl_ptr->FCCOB5;

    *swapmode = current_swap_mode;
    return (result);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_flash_swap
* Returned Value   : TRUE if successful, FALSE otherwise
* Comments         :
*   Perform flash swap.
*
*END*----------------------------------------------------------------------*/
uint32_t ftfl_flash_swap
(
   /* [IN] Device instance */
   IO_FLASHX_STRUCT_PTR   dev_ptr
)
{
    FTFL_FLASH_INTERNAL_STRUCT_PTR     dev_spec_ptr;
    FTFL_MemMapPtr                     ftfl_ptr;
    uint32_t                           result;
    uint8_t                            current_swap_mode = 0xFF;

    dev_spec_ptr = (FTFL_FLASH_INTERNAL_STRUCT_PTR)(dev_ptr->DEVICE_SPECIFIC_DATA);
    ftfl_ptr = (FTFL_MemMapPtr)(dev_spec_ptr->ftfl_ptr);

    /* Set the default return code as FTFL_OK */
    result = FTFL_OK;

    /* Check if the destination is not phrase aligned */
    if (0 != ((uint32_t)BSPCFG_SWAP_INDICATOR_ADDR % FTFL_PHRASE_SIZE))
    {
        /* Return error code FTFL_ERR_ADDR */
        return FTFL_ERR_ADDR;
    }

    /* Check whether BSPCFG_SWAP_INDICATOR_ADDR is in lower flash banks */
    if ((BSPCFG_SWAP_INDICATOR_ADDR < FTFL_SWAP_VALID_RANGE_START) || 
        (BSPCFG_SWAP_INDICATOR_ADDR >= FTFL_SWAP_VALID_RANGE_END)
    )
    {
        return FTFL_ERR_ADDR;
    }

    /* Get current swap report */
    result = ftfl_flash_swap_status(dev_ptr, BSPCFG_SWAP_INDICATOR_ADDR, &current_swap_mode);
    if (FTFL_OK != result)
    {
        return (result);
    }

    /* If current swap mode is Uninitialized */
    if (FTFL_SWAP_UNINIT == current_swap_mode)
    {
        /* Initialize Swap to Initialized/READY state */
        result = ftfl_flash_swap_command(dev_spec_ptr, BSPCFG_SWAP_INDICATOR_ADDR, FTFL_SWAP_SET_INDICATOR_ADDR);

        /* Check for the success of command execution */
        if (FTFL_OK != result)
        {
            return (result);
        }

        /* Wait for state transition from Uninitialized to Ready or Update */
        while ( ((current_swap_mode = ftfl_ptr->FCCOB5) != FTFL_SWAP_READY) &&
                ((current_swap_mode = ftfl_ptr->FCCOB5) != FTFL_SWAP_UPDATE) &&
                ((current_swap_mode = ftfl_ptr->FCCOB5) != FTFL_SWAP_UPDATE_ERASED) )
        {
            /* Do nothing*/
        }
    }

    /* Get current swap report */
    result = ftfl_flash_swap_status(dev_ptr, BSPCFG_SWAP_INDICATOR_ADDR, &current_swap_mode);
    if (FTFL_OK != result)
    {
        return (result);
    }

    /* If current swap mode is Initialized/Ready */
    if (FTFL_SWAP_READY == current_swap_mode)
    {
        /* Progress Swap to UPDATE state */
        result = ftfl_flash_swap_command(dev_spec_ptr, BSPCFG_SWAP_INDICATOR_ADDR, FTFL_SWAP_SET_IN_PREPARE);

        /* Check for the success of command execution */
        if (FTFL_OK != result)
        {
            return (result);
        }

        /* Wait for state transition from Initialized/Ready to Update */
        while ( ((current_swap_mode = ftfl_ptr->FCCOB5) != FTFL_SWAP_UPDATE) &&
                ((current_swap_mode = ftfl_ptr->FCCOB5) != FTFL_SWAP_UPDATE_ERASED) )
        {
            /* Do nothing*/
        }
    }

    /* If current swap mode is Update/Update-Erased */
    if (FTFL_SWAP_UPDATE == current_swap_mode)
    {
        result = ftfl_flash_erase_sector (dev_ptr, (void *)(FTFL_SWAP_UPPER_INDICATOR_ADDR), 0);
        if (FALSE == result)
        {
           return (FTFL_ERR_ADDR);
        }
    }

    /* Get current swap report */
    result = ftfl_flash_swap_command(dev_spec_ptr, BSPCFG_SWAP_INDICATOR_ADDR, FTFL_SWAP_REPORT_STATUS);
    /* Check for the success of command execution */
    if (FTFL_OK != result)
    {
         return (result);
    }
    /* Progress Swap to COMPLETE State */
    result = ftfl_flash_swap_command(dev_spec_ptr, BSPCFG_SWAP_INDICATOR_ADDR, FTFL_SWAP_SET_IN_COMPLETE);

    /* Check for the success of command execution */
    if (FTFL_OK != result)
    {
        return (result);
    }

    /* Wait for state transition from Update-Erased to Complete */
    while ((current_swap_mode = ftfl_ptr->FCCOB5) != FTFL_SWAP_COMPLETE)
    {
        /* Do nothing*/
    }

    return (result);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ftfl_flash_swap_and_reset
* Returned Value   : FALSE if failed
* Comments         :
*   Call flash_swap and perform reset.
*   Never return if succeed.
*
*END*----------------------------------------------------------------------*/
uint32_t ftfl_flash_swap_and_reset
(
   /* [IN] Device instance */
   IO_FLASHX_STRUCT_PTR   dev_ptr
)
{
    uint32_t result = 0;
    result = ftfl_flash_swap(dev_ptr);

    /* Perform reset if swap success */
    if (result == FTFL_OK)
    {
        #if PSP_MQX_CPU_IS_ARM_CORTEX_M4
            // reset with Application Interrupt and Reset Control Register
            SCB_AIRCR = 0x05FA0004;
        #elif PSP_MQX_CPU_IS_COLDFIRE
            // reset by invalid access
            result = *(volatile uint32_t*)(0xFFFF0000);
        #else
            #error "Unsupported architecture"
        #endif
        result = ~FTFL_OK;
    }
    return result;
}

#endif


