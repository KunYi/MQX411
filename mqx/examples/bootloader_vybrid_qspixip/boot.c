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
*   This file contains the source for the bootloader_vybrid example program.
*
*
*END************************************************************************/

#include <stdlib.h>
#include <mqx.h>
#include <bsp.h>
#include <mfs.h>
#include <fio.h>
#include <sdcard.h>
#include <string.h>
#include <part_mgr.h>
#include "quadspi_boot.h"
#include "iniparser.h"
#include "md5.h"
#include "qspi_memory.h"

#if _DEBUG
#define BOOTLOADER_VYBRID_DEBUG
#endif

#if defined(BOOTLOADER_VYBRID_DEBUG)
#define bl_dbg(x) printf x
#else
#define bl_dbg(x) (void)0
#endif

#define MD5_LENGTH          (16)
#define IMAGES_MAX_COUNT    (2)
#define SDCARD_COM_CHANNEL  BSP_SDCARD_ESDHC_CHANNEL
#define BUFFER_LENGTH       (64)
#define LINE_LENGTH         (INIPARSER_LENGTH_LINE)
#define VALUE_LENGTH        (INIPARSER_LENGTH_PROPERTY_VALUE)
#define IMAGES_MAX_COUNT    (2)
#define BOOT_CORETYPE_A5    (0x43413500)
#define BOOT_CORETYPE_M4    (0x434D3400)

#define BOOTLOADER_IMGINFO_TOKEN (0x4A5B6C7D)
#define MAX_IMG_COUNT       (2)
#define IMG_INFO_OFFSET     (0x0)

#if BSPCFG_ENABLE_QUADSPI0
#define IMG_INFO_STORED_QSPI_MODULE_ID    0
#define IMG_INFO_STORED_FLASH_ID          "qspi0:"
#else
#error This application requires BSPCFG_ENABLE_QUADSPI0. Please set corresponding BSPCFG_ENABLE_QUADSPI0 to non-zero in user_config.h and recompile BSP with this option.
#endif

typedef void (*fptr_void_t)(void);
typedef uint8_t md5digest_t[MD5_LENGTH];
char partman_name[] = "pm:";
char filesystem_name[] = "c:/";
uint8_t info_from_flash = 0;

uint32_t single_core;

MQX_FILE_PTR
    com_handle = NULL,
    sdcard_handle = NULL, mfs_handle = NULL,
    partman_handle = NULL,
    file_handle = NULL,
    filesystem_handle = NULL;


/* Function prototypes */
void found_label(const char *);
void found_property(const char *, const char *);
uint32_t copy_image(uint32_t, const char *);
void md5_of_memory(uint32_t, uint32_t, md5digest_t *);
void verify_parser(void);
uint32_t mount(void);
void main_task(uint32_t);
void boot_process(void);
void boot_from_reserved(void);


const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index, Function, Stack, Priority,   Name,          Attributes, Param, Time Slice */
    { 1,          main_task,  2000,        8, "main", MQX_AUTO_START_TASK,     0,          0 },
    { 0 }
};


typedef enum {
    PERFORM_TYPE_RUN    = 1,
    PERFORM_TYPE_LOAD   = 2,
} PERFORM_TYPE;


typedef enum {
    FLAGS_TYPE_RECOGNIZED_LABEL = 0x01,
    FLAGS_TYPE_RECOGNIZED_PATH  = 0x02,
    FLAGS_TYPE_RECOGNIZED_BASE  = 0x04,
    FLAGS_TYPE_RECOGNIZED_CORE  = 0x08,
    FLAGS_TYPE_RECOGNIZED_MD5   = 0x10,
    FLAGS_TYPE_PERFORM_RUN      = 0x20,
    FLAGS_TYPE_PERFORM_LOAD     = 0x40,
} FLAGS_TYPE;


typedef struct {
    /* memory address to load image */
    uint32_t             base;
    /* coreid */
    uint32_t             core;
    /* image size in B */
    uint32_t             size;
    /* md5 hash */
    md5digest_t         md5;
    /* flags */
    FLAGS_TYPE          flags;
    /* image label of setup.ini */
    char                label[VALUE_LENGTH];
    /* path to .bin file */
    char                path[VALUE_LENGTH];
    /* flash id */
    char                flash_id[VALUE_LENGTH];
} img_info_struct;


typedef struct {
    /* number of boot_info records */
    uint32_t            count;
    /* primary coreid */
    uint32_t            index;
    /* list of boot_info records */
    img_info_struct     images[IMAGES_MAX_COUNT];
} img_context_struct;


typedef struct
{
    /* image base address */
    uint32_t             base;
    /* coreid */
    uint32_t             core;
    /* run flags */
    FLAGS_TYPE          flags;
} boot_info_struct;


typedef struct {
    /* number of boot_info records */
    uint32_t            count;
    /* primary coreid */
    uint32_t            primary;
    /* list of boot_info records */
    boot_info_struct    images[IMAGES_MAX_COUNT];
} boot_context_struct;

img_context_struct img_context;
extern boot_context_struct boot_context;


typedef enum {
    INI_STATE_START = 0,
    INI_STATE_BOOT,
    INI_STATE_IMAGE,
    INI_STATE_IMAGEPROP,
} INI_STATE;

typedef struct
{
    uint32_t token;
    uint32_t img_count;
    char     flash_id[MAX_IMG_COUNT][VALUE_LENGTH];
    uint32_t flash_base[MAX_IMG_COUNT];
    uint32_t size[MAX_IMG_COUNT];
    uint32_t base[MAX_IMG_COUNT];
    uint32_t core[MAX_IMG_COUNT];
} MQX_BOOTLOADER_IMGINFO;
 
INI_STATE ini_state = INI_STATE_START;
MQX_BOOTLOADER_IMGINFO imginfo;


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : found_label
* Returned Value : None
* Comments       :
*     "label" callback, initialization of img_context structure
*END*--------------------------------------------------------------------*/

void found_label
(
    /* [IN] property name */
    const char *name
)
{
    switch (ini_state)
    {
        case INI_STATE_START:
            if (strcmp(name, "boot"))
            {
                printf("Error : first label is not [boot] \n");
                _task_block();
            }
            ini_state = INI_STATE_BOOT;
        break;
        case INI_STATE_BOOT:
        case INI_STATE_IMAGEPROP:
            ini_state = INI_STATE_IMAGE;
        case INI_STATE_IMAGE:
            for (img_context.index = 0; img_context.index < img_context.count; img_context.index++)
            {
                if (!strcmp(img_context.images[img_context.index].label, name))
                {
                    img_context.images[img_context.index].flags |= FLAGS_TYPE_RECOGNIZED_LABEL;
                    ini_state = INI_STATE_IMAGEPROP;
                    break;
                }
            }
        break;
    }
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : found_property
* Returned Value : None
* Comments       :
*     "property" callback, initialization of img_context structure
*END*--------------------------------------------------------------------*/

void found_property
(
    /* [IN] property name */
    const char *name,
    /* [IN] value */
    const char *value
)
{
    img_info_struct * image = NULL;
    uint32_t convert[4] = {0};
    uint32_t i = 0;

    switch (ini_state)
    {
        case INI_STATE_BOOT:
            image = &img_context.images[img_context.count];
            /* property of [boot] label */
            if (!strcmp(name, "run"))
            {
                if (img_context.count < IMAGES_MAX_COUNT)
                {
                    strcpy(image->label, value);
                    image->flags |= FLAGS_TYPE_PERFORM_RUN;
                    img_context.count += 1;
                    break;
                }
            }
            if (!strcmp(name, "load"))
            {
                if (img_context.count < IMAGES_MAX_COUNT)
                {
                    strcpy(image->label, value);
                    image->flags |= FLAGS_TYPE_PERFORM_LOAD;
                    img_context.count += 1;
                    break;
                }
            }
        break;
        case INI_STATE_IMAGEPROP:
            image = &img_context.images[img_context.index];
            if (!strcmp(name, "base"))
            {
                sscanf((char *)value, "0x%x", &image->base);
                image->flags |= FLAGS_TYPE_RECOGNIZED_BASE;
                break;
            }
            if (!strcmp(name, "core"))
            {
                sscanf((char *)value, "%d", &image->core);
                image->flags |= FLAGS_TYPE_RECOGNIZED_CORE;
                break;
            }
            if (!strcmp(name, "path"))
            {
                strcpy(image->path, value);
                image->flags |= FLAGS_TYPE_RECOGNIZED_PATH;
                break;
            }
            if (!strcmp(name, "md5"))
            {
                sscanf((char *)value, "%8x%8x%8x%8x", &convert[0], &convert[1], &convert[2], &convert[3]);
                for (i = 0; i < 4; i++)
                {
                    image->md5[(4 * i) + 0] = (uint8_t)((convert[i] >> 24) & 0xff);
                    image->md5[(4 * i) + 1] = (uint8_t)((convert[i] >> 16) & 0xff);
                    image->md5[(4 * i) + 2] = (uint8_t)((convert[i] >> 8) & 0xff);
                    image->md5[(4 * i) + 3] = (uint8_t)(convert[i] & 0xff);
                }
                image->flags |= FLAGS_TYPE_RECOGNIZED_MD5;
                break;
            }
        break;
    }
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : copy_image
* Returned Value : number of copied bytes
* Comments       :
*     copy file content to memory, starting at "base_addr"
*END*--------------------------------------------------------------------*/

uint32_t copy_image
(
    /* [IN] memory base address */
    uint32_t base_addr,
    /* [IN] image filename */
    const char *filename
)
{
    MQX_FILE_PTR file_handle = NULL;
    uint8_t buffer[BUFFER_LENGTH], * target_addr = NULL;
    uint32_t buffer_length;
    uint32_t copied = 0;

    /* open binary file, return 0 if failed */
    file_handle = fopen(filename, "r+");
    if (NULL == file_handle)
    {
        return 0;
    }

    /* copy content to memory */
    target_addr = (uint8_t *)base_addr;
    while ((buffer_length = read(file_handle, buffer, BUFFER_LENGTH)) > 0)
    {
        _mem_copy(buffer, target_addr, buffer_length);
        target_addr += buffer_length;
        copied += buffer_length;
    }

    _DCACHE_FLUSH_MLINES((uint8_t *)base_addr, copied);
    fclose(file_handle);
    return copied;
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : md5_of_memory
* Returned Value : none
* Comments       :
*     calculate memory MD5 hash, start at "base_address" and "size" bytes
*END*--------------------------------------------------------------------*/

void md5_of_memory
(
    /* [IN] memory base address */
    uint32_t base_addr,
    /* [IN] size in bytes */
    uint32_t size,
    /* [OUT] ptr to md5 hash */
    md5digest_t * md5
)
{
    md5_state_t state;
    uint8_t *text = (uint8_t *)base_addr;
    md5_init(&state);

    // append memory blocks
    for (; size > BUFFER_LENGTH; size -= BUFFER_LENGTH)
    {
        md5_append(&state, (const md5_byte_t *)text, BUFFER_LENGTH);
        text += BUFFER_LENGTH;
    }

    // append remains bytes
    if (size)
    {
        md5_append(&state, (const md5_byte_t *)text, size);
    }
    md5_finish(&state, *md5);
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : verify_parser
* Returned Value : none
* Comments       :
*     verify & stop program if img_context doesn't contain all
*     necessary informations
*END*--------------------------------------------------------------------*/

void verify_parser(void)
{
    uint32_t i;
    img_info_struct * image = NULL;

    /* check whether all required parts were parsed */
    for (i = 0; i < img_context.count; i++)
    {
        image = &img_context.images[i];
        if (!(image->flags & FLAGS_TYPE_RECOGNIZED_LABEL))
        {
            printf("Error : missing label [%s] \n", image->label);
            _task_block();
        }
        if (!(image->flags & FLAGS_TYPE_RECOGNIZED_PATH))
        {
            printf("Error : missing 'path' property of label [%s] \n", image->label);
            _task_block();
        }
        if (!(image->flags & FLAGS_TYPE_RECOGNIZED_CORE))
        {
            printf("Error : missing 'core' property of label [%s] \n", image->label);
            _task_block();
        }
        if (!(image->flags & FLAGS_TYPE_RECOGNIZED_BASE))
        {
            printf("Error : missing 'base' property of label [%s] \n", image->label);
            _task_block();
        }
    }
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : load_image_info_from_flash
* Returned Value : if 0 on success, otherwise is error
* Comments       :
*      It reads back image information from flash.
*END*--------------------------------------------------------------------*/
int32_t load_image_info_from_flash(char const *flash_id)
{
    MQX_BOOTLOADER_IMGINFO *pImgInfo = NULL;
    img_info_struct * image = NULL;
    uint32_t i, dest_addr;
    int32_t qspi_flash_length;

    if (!flash_id)
        return -1; 

#if NOT_XIP_IN_AREA
    pImgInfo = (MQX_BOOTLOADER_IMGINFO *) _mem_alloc_zero(sizeof(MQX_BOOTLOADER_IMGINFO));
    if (!pImgInfo)
    {
        printf("Error : out of memory\n");
        return -1;
    }

    flash_fp = fopen(flash_id, NULL);
    if (flash_fp == NULL)
    {
        printf("Error : failed to open flash controller %s,", flash_id);
        printf("perhaps there's no flash component related to %s enabled in bsp\n", flash_id);
        return -1;
    }

    qspi_flash_base_addr = memory_get_start_addr(flash_fp);
    if (qspi_flash_base_addr == 0xFFFFFFFF)
    {
        printf("Error : failed to get start address of flash %s\n", flash_id);
        fclose(flash_fp);
        return -1;
    }

    qspi_flash_length = memory_get_total_size(flash_fp);
    if (qspi_flash_length < 0)
    {
        printf("Error : failed to get flash length of flash %s\n", flash_id);
        fclose(flash_fp);
        return -1;
    }

    dest_addr = qspi_flash_base_addr + qspi_flash_length - sizeof(MQX_BOOTLOADER_IMGINFO);
    length_read = memory_read_data(flash_fp, dest_addr + IMG_INFO_OFFSET, sizeof(MQX_BOOTLOADER_IMGINFO), (uint8_t *)pImgInfo);
    if ((length_read < 0) || (length_read != sizeof(MQX_BOOTLOADER_IMGINFO)))
    {
        fclose(flash_fp);
        return -1;
    }

    fclose(flash_fp);
#else
    qspi_flash_length = quadspi_conf.sflash_A1_size + quadspi_conf.sflash_A2_size + quadspi_conf.sflash_B1_size + quadspi_conf.sflash_B2_size;
    bl_dbg(("DBG: Base addr: 0x%X, size: 0x%X\n", FLASH_BASE, qspi_flash_length));
    dest_addr = FLASH_BASE + qspi_flash_length - sizeof(MQX_BOOTLOADER_IMGINFO);
    pImgInfo = (MQX_BOOTLOADER_IMGINFO *)(dest_addr + IMG_INFO_OFFSET);
#endif

    if (pImgInfo->token != BOOTLOADER_IMGINFO_TOKEN)
    {
        printf("Error : Token (0x%X) is not found on flash: %s\n", pImgInfo->token, IMG_INFO_STORED_FLASH_ID);
        return -1;
    }

    bl_dbg(("DBG: image count: %d\n", pImgInfo->img_count));
    img_context.count = pImgInfo->img_count;
    for (i = 0; i < pImgInfo->img_count; i++)
    {
        bl_dbg(("DBG: image[%d]:\n", i+1));
        bl_dbg(("DBG:  flash base: 0x%X\n", pImgInfo->flash_base[i]));
        bl_dbg(("DBG:  flash id: %s\n", pImgInfo->flash_id[i]));
        bl_dbg(("DBG:  load base: 0x%X\n", pImgInfo->base[i]));
        bl_dbg(("DBG:  core index: %d\n", pImgInfo->core[i]));
        bl_dbg(("DBG:  size: %d\n", pImgInfo->size[i]));
        image = &img_context.images[i];
        image->core = pImgInfo->core[i];
        image->base = pImgInfo->base[i];
        image->size = pImgInfo->size[i];
        strcpy(image->flash_id, pImgInfo->flash_id[i]);
        image->flags |= FLAGS_TYPE_PERFORM_RUN;
        sprintf(image->path, "0x%08X", pImgInfo->flash_base[i]);
    }
    return 0;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : mount
* Returned Value : none
* Comments       :
*     mount sdcard, partition, filesystem, .. to 'c:/' drive
*END*--------------------------------------------------------------------*/

uint32_t mount(void)
{
    uint32_t result = MQX_OK;

    com_handle = fopen(SDCARD_COM_CHANNEL, NULL);
    if (com_handle == NULL)
    {
        printf("Error : unable to open sdhc driver \n");
        return MQX_ERROR;
    }

    result = _io_sdcard_install("sdcard:", (void *)&_bsp_sdcard0_init, com_handle);
    if (result != MQX_OK)
    {
        printf("Error : unable to install sdcard driver \n");
        fclose(com_handle);
        return MQX_ERROR;
    }

    _time_delay (200);

    sdcard_handle = fopen("sdcard:", 0);
    if (sdcard_handle == NULL)
    {
        printf("Error : unable to open sdcard driver \n");
        fclose(com_handle);
        return MQX_ERROR;
    }

    result = _io_part_mgr_install(sdcard_handle, partman_name, 0);
    if (result != MQX_OK)
    {
        printf("Error : unable to install partition manager \n");
        fclose(sdcard_handle);
        fclose(com_handle);
        return MQX_ERROR;
    }

    partman_handle = fopen(partman_name, NULL);
    if (partman_handle == NULL)
    {
        printf("Error : unable to open partition manager \n");
        fclose(sdcard_handle);
        fclose(com_handle);
        return MQX_ERROR;
    }

    result = _io_mfs_install(partman_handle, filesystem_name, 1);
    if (result != MQX_OK)
    {
        printf("Error : unable to install MFS driver \n");
        fclose(partman_handle);
        fclose(sdcard_handle);
        fclose(com_handle);
        return MQX_ERROR;
    }

    filesystem_handle = fopen(filesystem_name, NULL);
    if (partman_handle == NULL)
    {
        printf("Error : unable to open MFS driver \n");
        fclose(partman_handle);
        fclose(sdcard_handle);
        fclose(com_handle);
        return MQX_ERROR;
    }

    return MQX_OK;
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : main_task
* Returned Value : none
* Comments       :
*     main_task function
*END*--------------------------------------------------------------------*/

void main_task
(
    uint32_t initial_data
)
{   
    volatile uint32_t cpu_count = (*(uint32_t*)MSCM_CPxCOUNT);
    printf("Bootloader from ");
#if PSP_MQX_CPU_IS_VYBRID_A5
    printf("A5");
#elif  PSP_MQX_CPU_IS_VYBRID_M4
    printf("M4");
#endif
  //  printf(" as primary core\n");
  
    if (cpu_count & 0x1){
      printf(" as primary core, dual core\n");
      single_core = 0;
  } else {
      printf(" single core\n");
      single_core = 1;
  }
    printf("======================\n");
    printf("Trying to mount filesystem \n");
    if (MQX_OK != mount())
    {
        printf("Failed to mount filesystem, try to load image info from flash\n");
        if (!load_image_info_from_flash(IMG_INFO_STORED_FLASH_ID))
            info_from_flash = 1;
        else
        {
            printf("Error : failed to load image information from flash\n");
            _task_block();
        }
    }

    printf("Starting bootloader \n");
    boot_process();
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : boot_process
* Returned Value : none
* Comments       :
*     prepare img_context and call boot function
*END*--------------------------------------------------------------------*/

void boot_process(void)
{
    uint32_t i = 0;
    uint8_t * flash_base = 0;
    md5digest_t md5;
    img_info_struct * image = NULL;
#if  PSP_MQX_CPU_IS_VYBRID_A5
    char *fname = "c:/setup_a5.ini";
#elif  PSP_MQX_CPU_IS_VYBRID_M4
    char *fname = "c:/setup_m4.ini";
#endif

    if (!info_from_flash)
    {
        /* call setup.ini parser */
        if (MQX_OK != iniparser_load(fname, &found_label, &found_property))
        {
            printf("Error : cannot open '%s' \n", fname);
            _task_block();
        }
        /* verify result of parser process */
        verify_parser();

    }
    else
        bl_dbg(("Skip loading ini file from fs\n"));

    /* load & verify */
    for (i = 0; i < img_context.count; i++)
    {
        image = &img_context.images[i];

        if (single_core && (image->core != 0x0)) {
            image->flags &= ~(FLAGS_TYPE_PERFORM_RUN);
            continue;
        }

        /* copy binary images */
        if (!strncmp(image->path, "0x", 2))
        {
            flash_base = (uint8_t *)strtol(image->path, NULL, 0);
            bl_dbg(("DBG: copy image from 0x%08X to 0x%08X\n", flash_base, image->base));
            memcpy((uint8_t *)image->base, flash_base, image->size);
            _DCACHE_FLUSH_MLINES((uint8_t *)image->base, image->size);
        }
        else
        {
            image->size = copy_image(image->base, image->path);
        }
        if (0 == image->size)
        {
            printf("Error : cannot load image %s \n", image->path);
            _task_block();
        }

        /* verify image */
        if (image->flags & FLAGS_TYPE_RECOGNIZED_MD5)
        {
            md5_of_memory(image->base, image->size, &md5);
            if (memcmp(image->md5, md5, MD5_LENGTH))
            {
                printf("Error : image verify fail %s \n", image->path);
                _task_block();
            }
        }
    }

    /* set primary core number */
    boot_context.primary = (*(uint32_t *)MSCM_CPxNUM);
    boot_context.primary &= 0x1;
    boot_context.count = img_context.count;

    /* copy necessary data from img_context to boot_context */
    for (i = 0; i < img_context.count; i++)
    {
        boot_context.images[i].base = img_context.images[i].base;
        boot_context.images[i].core = img_context.images[i].core;
        boot_context.images[i].flags = img_context.images[i].flags;
    }

#if PSP_MQX_CPU_IS_VYBRID_A5
    /* disable cache and MMU before overwriting MMU table */
    _ICACHE_DISABLE();
    _DCACHE_DISABLE();
    _mmu_vdisable();
#endif

    /* try to boot from .boot_reserved segment */
    boot_from_reserved();
}


#if defined(__ICCARM__)
    #pragma language = extended
    #pragma location = ".boot_reserved_data"
    #pragma segment  = ".boot_reserved_data"
#elif defined(__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".boot_reserved_data")))
#else
    #error "Unsupported compiler"
#endif
boot_context_struct boot_context = {0};


#if defined(__ICCARM__)
    #pragma language = extended
    #pragma location = ".boot_reserved_data"
    #pragma segment  = ".boot_reserved_data"
#elif defined(__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".boot_reserved_data")))
#else
    #error "Unsupported compiler"
#endif
static uint32_t i = 0;



#if   defined(__ICCARM__)
    #pragma language = extended
    #pragma location = ".boot_reserved_code"
    #pragma segment  = ".boot_reserved_code"
#elif defined(__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".boot_reserved_code")))
#else
    #error "Unsupported compiler"
#endif
void boot_from_reserved (void)
{
    /* from this point stack cannot be used */
    /* images cannot use reserved memmory .boot_reserved ! */

#if  PSP_MQX_CPU_IS_VYBRID_A5
    __asm("cpsid ifa");
#elif  PSP_MQX_CPU_IS_VYBRID_M4
    __asm("cpsid i");
#endif

    /* try to find and boot auxiliary core */
    for (i = 0; i < boot_context.count; i++)
    {
        if (
            (boot_context.images[i].core != boot_context.primary) &&
            (boot_context.images[i].flags & FLAGS_TYPE_PERFORM_RUN)
        )
        {
#if  PSP_MQX_CPU_IS_VYBRID_A5
            // reset vector
            SRC_GPR(2) = ((_mqx_uint*)boot_context.images[i].base)[1];
            // stack pointer
            SRC_GPR(3) = ((_mqx_uint*)boot_context.images[i].base)[0];
#elif  PSP_MQX_CPU_IS_VYBRID_M4
            SRC_GPR(2) = boot_context.images[i].base;
#endif
            CCM_CCOWR = 0x15a5a;
            break;
        }
    }

    /* try to find and boot primary core */
    for (i = 0; i < boot_context.count; i++)
    {
        if (
            (boot_context.images[i].core == boot_context.primary) &&
            (boot_context.images[i].flags & FLAGS_TYPE_PERFORM_RUN)
        )
        {
#if PSP_MQX_CPU_IS_VYBRID_A5
            (*((fptr_void_t)(boot_context.images[i].base)))();
#elif PSP_MQX_CPU_IS_VYBRID_M4
           #if defined(__CC_ARM)
            __asm
            {
                msr     MSP, ((_mqx_uint*)boot_context.images[i].base)[0];
            }
#else
            __asm("msr     MSP, %0" : : "r" (((_mqx_uint*)boot_context.images[i].base)[0]));
#endif
            (*((fptr_void_t)(((_mqx_uint*)boot_context.images[i].base)[1])))();
#endif
            break;
        }
    }
           
  

    /* primary core is not used, go to infinite loop */
    while (1);
}

