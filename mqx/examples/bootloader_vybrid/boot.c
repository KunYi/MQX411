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
#include "boot_ivt_header.h"
#include "iniparser.h"
#include "md5.h"
#include "qspi_memory.h"

#if _DEBUG
#define bl_dbg(x) printf x
#else
#define bl_dbg(x) (void)0
#endif

#define BOOTLOADER_USING_FREAD    (1)
#define LOG_DETAIL_INFO           (0)
#define BREAK_CHECKING_ON_ERROR   (0)
#define IMAGES_FLASH_MAX_COUNT    (3)
#define FLASH_BUFFER_LENGTH       (1024)
#define DOT_PER_LENGTH            (4096)
 
#define MD5_LENGTH          (16)
#define IMAGES_MAX_COUNT    (2)
#define SDCARD_COM_CHANNEL  BSP_SDCARD_ESDHC_CHANNEL
#define BUFFER_LENGTH       (64)
#define LINE_LENGTH         (INIPARSER_LENGTH_LINE)
#define VALUE_LENGTH        (INIPARSER_LENGTH_PROPERTY_VALUE)
#define BOOT_CORETYPE_A5    (0x43413500)
#define BOOT_CORETYPE_M4    (0x434D3400)

#define BOOT_SECTION_BASE 0x3f040000
#define BOOT_SECTION_END  0x3f06fff0
 
#define BOOTLOADER_IMGINFO_TOKEN (0x4A5B6C7D)
#define MAX_IMG_COUNT       (2)
#define IMG_INFO_OFFSET     (0x0)
#define IMAGE_INFO_LOAD_FROM_FLASH 0

#if !(BSPCFG_ENABLE_QUADSPI0) && !(BSPCFG_ENABLE_QUADSPI1)
#error This application requires BSPCFG_ENABLE_QUADSPI1 or BSPCFG_ENABLE_QUADSPI0 to be not NULL. Please set corresponding BSPCFG_ENABLE_QUADSPIx to non-zero in user_config.h and recompile BSP with this option.
#endif

#if (BSPCFG_ENABLE_QUADSPI0)
#define BL_QSPI_ID          "qspi0:"
#elif (BSPCFG_ENABLE_QUADSPI1)
#define BL_QSPI_ID          "qspi1:"
#endif

#if defined(__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".boot_data")))
#elif defined(__ICCARM__)
#pragma location=".boot_data"
#endif
const BOOT_DATA_T boot_data = {
  BOOT_SECTION_BASE,                 /* boot start location */
  (BOOT_SECTION_END - BOOT_SECTION_BASE),     /* size */
  PLUGIN_FLAG,                /* Plugin flag*/
  0xFFFFFFFF                    /* empty - extra data word */
};

#if defined(__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".ivt")))
#elif defined(__ICCARM__)
#pragma location=".ivt"
#else
    #error "Unsupported compiler"
#endif
const ivt image_vector_table = {
  IVT_HEADER,                         /* IVT Header */
  (uint32_t)&__boot,     /* Image  Entry Function */
  IVT_RSVD,                           /* Reserved = 0 */
  (uint32_t)DCD_ADDRESS,              /* Address where DCD information is stored */
  (uint32_t)&boot_data,        /* Address where BOOT Data Structure is stored */
  (uint32_t)&image_vector_table,      /* Pointer to IVT Self (absolute address */
  (uint32_t)CSF_ADDRESS,              /* Address where CSF file is stored */
  IVT_RSVD                            /* Reserved = 0 */
};

#if defined(__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".dcd")))
#elif defined(__ICCARM__)
#pragma location=".dcd"
#else
    #error "Unsupported compiler"
#endif
const uint32_t device_config_data[DCD_ARRAY_SIZE] = {
  /* DCD Header */
  (uint32_t)(DCD_TAG_HEADER |                       \
    (0x2000<< 8) | \
    (DCD_VERSION << 24)),
  /* DCD Commands */
  /* Add additional DCD Commands here */
};

typedef void (*fptr_void_t)(void);
typedef uint8_t md5digest_t[MD5_LENGTH];
char partman_name[] = "pm:";
char filesystem_name[] = "c:/";
uint8_t is_flash_image = 0;
uint8_t skip_mount_fs = 0;

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
void process_image_flashing(void);

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
    FLAGS_TYPE_PERFORM_FLASH    = 0x80,
    FLAGS_TYPE_RECOGNIZED_FLASHBASE     = 0x100,
    FLAGS_TYPE_RECOGNIZED_FLASHID       = 0x200,
    FLAGS_TYPE_PERFORM_CHECK_ONLY = 0x400,
} FLAGS_TYPE;


typedef struct {
    /* memory address to flash image */
    uint32_t              flash_base;
    /* memory address to load image */
    uint32_t             base;
    /* coreid */
    uint32_t             core;
    /* image size in B */
    int32_t             size;
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
    img_info_struct     images[IMAGES_FLASH_MAX_COUNT];
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
    INI_STATE_FLASH,
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
            if (!strcmp(name, "boot"))
            {
                ini_state = INI_STATE_BOOT;
            }
            else if (!strcmp(name, "flash_image"))
            {
                ini_state = INI_STATE_FLASH;
                is_flash_image = 1;
            }
            else
            {
                printf("Error : first label is not [boot] \n");
                _task_block();
            }
        break;
        case INI_STATE_BOOT:
        case INI_STATE_FLASH:
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
    uint32_t check_only = 0;
    
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
                else
                    printf("WRN: [%s] will be skipped, since number of images has reached the maxmium count: %d\n", value, img_context.count);
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
                else
                    printf("WRN: [%s] will be skipped, since number of images has reached the maxmium count: %d\n", value, img_context.count);
            }
        break;
        case INI_STATE_FLASH:
            image = &img_context.images[img_context.count];
            /* property of [flash] label */
            if (!strcmp(name, "flash"))
            {
                if (img_context.count < IMAGES_FLASH_MAX_COUNT)
                {
                    strcpy(image->label, value);
                    image->flags |= FLAGS_TYPE_PERFORM_FLASH;
                    img_context.count += 1;
                    break;
                }
                else
                    printf("WRN: [%s] will be skipped, since number of images has reached the maxmium count: %d\n", value, img_context.count);
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
            if (!strcmp(name, "flash_id"))
            {
                memset(image->flash_id, 0, sizeof(image->flash_id));
                strcpy(image->flash_id, value);
                if (image->flash_id[strlen(image->flash_id) - 1] != ':')
                    image->flash_id[strlen(image->flash_id)] = ':';
                image->flags |= FLAGS_TYPE_RECOGNIZED_FLASHID;
                break;
            }
            if (!strcmp(name, "flash_base"))
            {
                sscanf((char *)value, "0x%x", &image->flash_base);
                image->flags |= FLAGS_TYPE_RECOGNIZED_FLASHBASE;
                break;
            }
            if (!strcmp(name, "check_only"))
            {
                sscanf((char *)value, "%d", &check_only);
                if (check_only)
                    image->flags |= FLAGS_TYPE_PERFORM_CHECK_ONLY;
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
        if ((!(image->flags & FLAGS_TYPE_RECOGNIZED_CORE)) && (!is_flash_image))
        {
            printf("Error : missing 'core' property of label [%s] \n", image->label);
            _task_block();
        }
        if ((!(image->flags & FLAGS_TYPE_RECOGNIZED_BASE)) && (!is_flash_image))
        {
            printf("Error : missing 'base' property of label [%s] \n", image->label);
            _task_block();
        }
        if ((!(image->flags & FLAGS_TYPE_RECOGNIZED_FLASHID)) && is_flash_image)
        {
            printf("Error : missing 'flash_id' property of label [%s] \n", image->label);
            _task_block();
        }
        if ((!(image->flags & FLAGS_TYPE_RECOGNIZED_FLASHBASE)) && is_flash_image)
        {
            printf("Error : missing 'flash_base' property of label [%s] \n", image->label);
            _task_block();
        }
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : get_file_size
* Returned Value : size of the given file
* Comments       :
*END*--------------------------------------------------------------------*/
int32_t get_file_size(const char * fname)
{
    MQX_FILE_PTR fp;
    uint32_t sz = 0;
    fp = fopen(fname, "r");
    if (fp == NULL)
    {
        printf("Error : failed to open %s\n", fname);
        return -1;
    }
    fseek(fp, 0L, IO_SEEK_END);
    sz = ftell(fp);
    fclose(fp);
    return sz;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : pre_flash_check
* Returned Value : if 0 on success, otherwise is error
* Comments       :
*      It will check the address range for each image, and check if every
*   two images are overlaped.
*END*--------------------------------------------------------------------*/
int32_t pre_flash_check()
{
    uint32_t i, j, qspi_flash_base_addr;
    uint32_t range_invalid_count = 0, overlap_image_count = 0;
    int32_t qspi_flash_length = 0;
    img_info_struct * img1, * img2;
    MQX_FILE_PTR flash_fp;

    /* image address range check */
    for (i = 0; i < img_context.count; i++)
    {
        img1 = &img_context.images[i];
        if ((img1->size == 0) && ((img1->size = get_file_size(img1->path)) < 0))
        {
            printf("Error : failed to get size of %s\n", img1->path);
            return -1;
        }

        flash_fp = fopen(img1->flash_id, NULL);
        if (flash_fp == NULL)
        {
            printf("Error : failed to open flash controller: %s\n", img1->flash_id);
            return -1;
        }

        qspi_flash_base_addr = memory_get_start_addr(flash_fp);
        if (qspi_flash_base_addr == 0xFFFFFFFF)
        {
            printf("Error : %s: failed to get start address of flash %s\n", img1->flash_id);
            fclose(flash_fp);
            return -1;
        }

        qspi_flash_length = memory_get_total_size(flash_fp);
        if (qspi_flash_length < 0)
        {
            printf("Error : %s: failed to get flash length of flash %s\n", img1->flash_id);
            fclose(flash_fp);
            return -1;
        }

        bl_dbg(("DBG: flash id: [%s], start: [0x%X], to: [0x%X]\n", img1->flash_id, qspi_flash_base_addr, qspi_flash_length));

        if ((qspi_flash_base_addr > img1->flash_base)
            || ((img1->flash_base + img1->size)
                > (qspi_flash_base_addr + qspi_flash_length - sizeof(imginfo))))
        {
            printf("WRN: image %s is out of range, [0x%X, 0x%X] available on %s, request [0x%X, 0x%X]\n",
                   img1->label, qspi_flash_base_addr, qspi_flash_length, img1->flash_id, img1->flash_base, img1->size);
            range_invalid_count++;
        }
        fclose(flash_fp);
    }

    /* image overlapping check */
    for (i = 0; i < img_context.count; i++)
    {
        img1 = &img_context.images[i];
        for (j = i + 1; j < img_context.count; j++)
        {
            img2 = &img_context.images[j];
            if ((img2->flash_base > (img1->flash_base + img1->size))
                    || ((img2->flash_base + img2->size)) < img1->flash_base)
                continue;
            printf("WRN: %s [%s, 0x%08X, 0x%X] overlaps with %s [%s, 0x%08X, 0x%X]\n",
                    img1->path, img1->flash_id, img1->flash_base, img1->size, img2->path, img2->flash_id, img2->flash_base, img2->size);
            overlap_image_count++;
        }
    }
    return overlap_image_count + range_invalid_count;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : check_single_image
* Returned Value : if 0 on success, otherwise is error
* Comments       :
*      It will check if a single image written to flash is same with the one
*   from file.
*END*--------------------------------------------------------------------*/
int32_t check_single_image(const char * image_path, const char * flash_id, const uint32_t base, const uint32_t size)
{
    MQX_FILE_PTR flash_fp, image_fp;
    uint8_t * flash_buffer; 
    uint8_t * image_buffer;
    uint32_t dest_addr;
    int32_t image_read_len = 0, flash_read_len = 0, total_compared = 0;
    uint32_t byte_mismatch_count = 0;
#if LOG_DETAIL_INFO
    int32_t i, j
#endif

    if (!image_path || !flash_id)
    {
        printf("Error : null point arguments given\n");
        return -1;
    }

    image_fp = fopen(image_path, "r");
    if (image_fp == NULL)
    {
        printf("Error : failed to open: %s\n", image_path);
        return -1;
    }

    flash_fp = fopen(flash_id, NULL);
    if (flash_fp == NULL)
    {
        printf("Error : failed to open flash controller: %s\n", flash_id);
        fclose(image_fp);
        return -1;
    }

    flash_buffer = (uint8_t *) _mem_alloc_zero(FLASH_BUFFER_LENGTH);
    if (flash_buffer == NULL)
    {
        printf("Error : out of memory, can not alloc buffer for flash\n");
        fclose(image_fp);
        fclose(flash_fp);
        return -1;
    }

    image_buffer = (uint8_t *) _mem_alloc_zero(FLASH_BUFFER_LENGTH);
    if (image_buffer == NULL)
    {
        printf("Error : out of memory, can not alloc buffer for image\n");
        _mem_free(flash_buffer);
        fclose(image_fp);
        fclose(flash_fp);
        return -1;
    }

    dest_addr = base;
    total_compared = 0;
#if BOOTLOADER_USING_READ
    while((image_read_len = read(image_fp, image_buffer, FLASH_BUFFER_LENGTH)) > 0)
#elif BOOTLOADER_USING_FREAD
    while((image_read_len = fread(image_buffer, 1, FLASH_BUFFER_LENGTH, image_fp)) > 0)
#endif
    {
        flash_read_len = memory_read_data(flash_fp, dest_addr, image_read_len, flash_buffer);
        if (flash_read_len < 0)
            break;
        if (flash_read_len != image_read_len)
            break;
        if (memcmp(image_buffer, flash_buffer, flash_read_len))
        {
#if LOG_DETAIL_INFO
            printf("Error : data mismacth @ 0x%X:", dest_addr);
            for (i = 0, j = 0; i < flash_read_len; i++)
            {
                if (image_buffer[i] != flash_buffer[i])
                {
                    if (!(j % 16))
                        printf("\n");
                    if (!(j % 4))
                        printf(" ");
                    printf(" 0x%03X", i);
                    j++;
                    byte_mismatch_count++;
                }
            }
            printf("\n");
#else
            printf("Error : data mismacth @ 0x%X\n", dest_addr);
#endif

#if BREAK_CHECKING_ON_ERROR
            break;
#endif
        }
        memset(image_buffer, 0, flash_read_len);
        memset(flash_buffer, 0, flash_read_len);
        total_compared += flash_read_len;
        dest_addr += flash_read_len;
        if (!(total_compared % DOT_PER_LENGTH))
            printf(".");
    }

    printf("\n");

    _mem_free(image_buffer);
    _mem_free(flash_buffer);
    fclose(image_fp);
    fclose(flash_fp);

    if ((byte_mismatch_count) || (size != total_compared))
    {
        printf("Error : Failed post check image %s\n", image_path);
        if (flash_read_len < 0)
            printf("Error :  read from flash %s error @ 0x%X\n", flash_id, dest_addr);
        else if (image_read_len < 0)
            printf("Error :  read from image %s error\n", image_path);
        else if (image_read_len && (flash_read_len != image_read_len))
            printf("Error :  read length from image [%d] diffs with from flash [%d]\n", image_read_len, flash_read_len);
        else if (byte_mismatch_count)
#if BREAK_CHECKING_ON_ERROR
            printf("Error :  read from flash %s diffs with from image %s @ 0x%X\n", flash_id, image_path, dest_addr);
#else
            printf("Error :  read from flash %s diffs with from image %s, total mismatched bytes %d\n", flash_id, image_path, byte_mismatch_count);
#endif
        else
            printf("Error :  size [%d] diffs from total_compared [%d]\n", size, total_compared);

        return 1;
    }

    return 0;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : post_flash_check
* Returned Value : if 0 on success, otherwise is error
* Comments       :
*      It will check all the image list.
*END*--------------------------------------------------------------------*/
int32_t post_flash_check()
{
    uint32_t i = 0, failed_check_count = 0;
    img_info_struct * image = NULL;

    for (i = 0; i < img_context.count; i++)
    {
        image = &img_context.images[i];
        if (check_single_image(image->path, image->flash_id, image->flash_base, image->size))
            failed_check_count++;
    }

    return failed_check_count;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : post_flash_check
* Returned Value : if 0 on success, otherwise is error
* Comments       :
*      It cleans a given flash.
*END*--------------------------------------------------------------------*/
int32_t clean_flash(const char * flash_id, const uint32_t start, const uint32_t length)
{
    MQX_FILE_PTR flash_fp;
    uint8_t index = 0;
    static int32_t cleaned[2] = {0};

    if (!strncmp(flash_id, "qspi0:", strlen("qspi0:")))
        index = 0;
    else if (!strncmp(flash_id, "qspi1:", strlen("qspi1:")))
        index = 1;
    else
    {
        printf("Error : unknown flash id %s\n", flash_id);
        return -1;
    }

    if (!cleaned[index])
    {
        flash_fp = fopen(flash_id, NULL);
        if (flash_fp == NULL)
        {
            printf("Error : failed to open flash, id %s\n", flash_id);
            return -1;
        }
        printf("Cleaning flash [%s]:\n", flash_id);
        if (!memory_chip_erase(flash_fp))
        {
            cleaned[index] = 1;
        }
        fclose(flash_fp);
    }

    if (!cleaned[index])
        return -1;

    return 0;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : flash_single_image
* Returned Value : if 0 on success, otherwise is error
* Comments       :
*      It will flash a single image to a given address on flash.
*END*--------------------------------------------------------------------*/
int32_t flash_single_image(const char * image_path, const char * flash_id, const uint32_t base)
{
    MQX_FILE_PTR flash_fp, image_fp;
    uint8_t * flash_buffer;
    uint8_t * compare_buffer;
    uint32_t dest_addr;
    int32_t length_read = 0, length_written = 0, total_written = 0;
#if LOG_DETAIL_INFO
    int32_t i, j;
#endif

    if (!image_path || !flash_id)
    {
        printf("Error : null point arguments given\n");
        return -1;
    }

    image_fp = fopen(image_path, "r");
    if (image_fp == NULL)
    {
        printf("Error : %s: failed to open file %s\n", image_path);
        return -1;
    }

    flash_fp = fopen(flash_id, NULL);
    if (flash_fp == NULL)
    {
        printf("Error : failed to open flash controller %s, ", flash_id);
        printf("perhaps there's no flash component related to %s enabled in bsp configuration.\n", flash_id);
        fclose(image_fp);
        return -1;
    }

    flash_buffer = (uint8_t *) _mem_alloc_zero(FLASH_BUFFER_LENGTH);
    if (flash_buffer == NULL)
    {
        printf("Error : out of memory, can not alloc\n");
        fclose(image_fp);
        fclose(flash_fp);
        return -1;
    }

    compare_buffer = (uint8_t *) _mem_alloc_zero(FLASH_BUFFER_LENGTH);
    if (compare_buffer == NULL)
    {
        printf("Error : out of memory, can not alloc buffer for compare\n");
        _mem_free(flash_buffer);
        fclose(image_fp);
        fclose(flash_fp);
        return -1;
    }

    dest_addr = base;
    total_written = 0;

#if BOOTLOADER_USING_READ
    while((length_read = read(image_fp, flash_buffer, FLASH_BUFFER_LENGTH)) > 0)
#elif BOOTLOADER_USING_FREAD
    while((length_read = fread(flash_buffer, 1, FLASH_BUFFER_LENGTH, image_fp)) > 0)
#endif
    {
        length_written = memory_write_data(flash_fp, dest_addr, length_read, flash_buffer);
        if (length_written < 0)
            break;
        if (length_written != length_read)
            break;

#if READ_BACK_AFTER_WRITE_TEST
        flash_read_len = memory_read_data(flash_fp, dest_addr, length_read, compare_buffer);
        if (flash_read_len != length_read)
            break;
        if (memcmp(flash_buffer, compare_buffer, flash_read_len))
        {
#if LOG_DETAIL_INFO
            printf("Error : data mismacth @ 0x%X:", dest_addr);
            for (i = 0, j = 0; i < flash_read_len; i++)
            {
                if (compare_buffer[i] != flash_buffer[i])
                {
                    if (!(j % 16))
                        printf("\n");
                    if (!(j % 4))
                        printf(" ");
                    printf(" 0x%03X", i);
                    j++;
                }
            }
            printf("\n");
#else
            printf("Error : data mismacth @ 0x%X\n", dest_addr);
#endif
        }
#endif

        memset(compare_buffer, 0, length_read);
        dest_addr += length_written;
        total_written += length_written;
        if (!(total_written % DOT_PER_LENGTH))
            printf(".");
    }

    printf("\n");
    if (compare_buffer != NULL);
        _mem_free(compare_buffer);
    if (flash_buffer != NULL);
        _mem_free(flash_buffer);
    fclose(image_fp);
    fclose(flash_fp);

    if (length_read < 0)
    {
        printf("Error : read %s failed\n", image_path);
        return -1;
    }
    else if (length_written < 0)
    {
        printf("Error : failed to flash to 0x%X\n", dest_addr);
        return -1;
    }
    else if (length_read && (length_written != length_read))
    {
        printf("Error : length of bytes written 0x%X does not match with the length read 0x%X\n", length_written, length_read);
        return -1;
    }
    bl_dbg(("DBG: %s flashed to %s @ 0x%X, length %d\n", image_path, flash_id, base, total_written));
    return 0;
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
    MQX_FILE_PTR flash_fp;
    MQX_BOOTLOADER_IMGINFO *pImgInfo = NULL;
    img_info_struct * image = NULL;
    uint32_t i, qspi_flash_base_addr, dest_addr;
    int32_t qspi_flash_length, length_read;

    if (!flash_id)
        return -1;

    pImgInfo = (MQX_BOOTLOADER_IMGINFO *) _mem_alloc_zero(sizeof(MQX_BOOTLOADER_IMGINFO));
    if (!pImgInfo)
    {
        printf("Error : out of memory\n");
        return -1;
    }

    flash_fp = fopen(flash_id, NULL);
    if (flash_fp == NULL)
    {
        printf("Error : failed to open flash controller %s, ", flash_id);
        printf("perhaps there's no flash component related to %s enabled in bsp configuration.\n", flash_id);
        return -1;
    }

    qspi_flash_base_addr = memory_get_start_addr(flash_fp);
    if (qspi_flash_base_addr == 0xFFFFFFFF)
    {
        printf("Error : %s: failed to get start address of flash %s\n", flash_id);
        fclose(flash_fp);
        return -1;
    }

    qspi_flash_length = memory_get_total_size(flash_fp);
    if (qspi_flash_length < 0)
    {
        printf("Error : %s: failed to get flash length of flash %s\n", flash_id);
        fclose(flash_fp);
        return -1;
    }

    dest_addr = qspi_flash_base_addr + qspi_flash_length - sizeof(MQX_BOOTLOADER_IMGINFO);
    length_read = memory_read_data(flash_fp, dest_addr + IMG_INFO_OFFSET, sizeof(MQX_BOOTLOADER_IMGINFO), (uint8_t *)pImgInfo);
    if ((length_read < 0) || (length_read != sizeof(MQX_BOOTLOADER_IMGINFO)))
    {
        fclose(flash_fp);
        printf("Error : failed to read data from flash.\n");
        return -1;
    }

    fclose(flash_fp);

    if (pImgInfo->token != BOOTLOADER_IMGINFO_TOKEN)
    {
        printf("Error : Token (0x%X) is not found on flash: %s\n", BOOTLOADER_IMGINFO_TOKEN, BL_QSPI_ID);
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
* Function Name  : save_image_info2flash
* Returned Value : if 0 on success, otherwise is error
* Comments       :
*      It writes image information to flash.
*END*--------------------------------------------------------------------*/
int32_t save_image_info2flash(char const *flash_id, MQX_BOOTLOADER_IMGINFO *pImgInfo)
{
    MQX_FILE_PTR flash_fp;
    uint32_t i, qspi_flash_base_addr, dest_addr;
    int32_t qspi_flash_length, length_written;

    if (!flash_id || !pImgInfo)
        return -1;

    flash_fp = fopen(flash_id, NULL);
    if (flash_fp == NULL)
    {
        printf("Error : failed to open flash controller %s, ", flash_id);
        printf("perhaps there's no flash component related to %s enabled in bsp configuration.\n", flash_id);
        return -1;
    }

    qspi_flash_base_addr = memory_get_start_addr(flash_fp);
    if (qspi_flash_base_addr == 0xFFFFFFFF)
    {
        printf("Error : %s: failed to get start address of flash %s\n", flash_id);
        fclose(flash_fp);
        return -1;
    }

    qspi_flash_length = memory_get_total_size(flash_fp);
    if (qspi_flash_length < 0)
    {
        printf("Error : %s: failed to get flash length of flash %s\n", flash_id);
        fclose(flash_fp);
        return -1;
    }

    dest_addr = qspi_flash_base_addr + qspi_flash_length - sizeof(MQX_BOOTLOADER_IMGINFO);
    length_written = memory_write_data(flash_fp, dest_addr + IMG_INFO_OFFSET, sizeof(MQX_BOOTLOADER_IMGINFO), (uint8_t *)pImgInfo);
    if ((length_written < 0) || (length_written != sizeof(MQX_BOOTLOADER_IMGINFO)))
    {
        fclose(flash_fp);
        return -1;
    }

    bl_dbg(("DBG: update info to flash %s, @ 0x%X, size %d\n", BL_QSPI_ID, dest_addr + IMG_INFO_OFFSET, length_written));
    bl_dbg(("DBG: token is 0x%X.\n", pImgInfo->token));
    bl_dbg(("DBG: image count: %d\n", pImgInfo->img_count));
    for (i = 0; i < pImgInfo->img_count; i++)
    {
        bl_dbg(("DBG: image[%d]:\n", i+1));
        bl_dbg(("DBG:  flash id: %s\n", pImgInfo->flash_id[i]));
        bl_dbg(("DBG:  flash base: 0x%X\n", pImgInfo->flash_base[i]));
        bl_dbg(("DBG:  load base: 0x%X\n", pImgInfo->base[i]));
        bl_dbg(("DBG:  core index: %d\n", pImgInfo->core[i]));
        bl_dbg(("DBG:  size: %d\n", pImgInfo->size[i]));
    }

    fclose(flash_fp);
    return 0;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : process_image_flashing
* Returned Value : none
* Comments       :
*     preflash image check, flashing and postflash image check
*END*--------------------------------------------------------------------*/

void process_image_flashing()
{
    uint32_t i, image_flashed_count = 0;
    img_info_struct * image = NULL;

    if (pre_flash_check())
    {
        printf("Error : Preflash checking failed\n");
        return;
    }

    for (i = 0; i < img_context.count; i++)
    {
        image = &img_context.images[i];
        if (!(image->flags & FLAGS_TYPE_PERFORM_FLASH))
        {
            bl_dbg(("DBG: skip %s\n", image->label));
            continue;
        }

        if (image->flags & FLAGS_TYPE_PERFORM_CHECK_ONLY)
        {
            bl_dbg(("DBG: skip flashing image %s, only read back checking\n", image->label));
            continue;
        }

        if (clean_flash(image->flash_id, image->flash_base, image->size))
        {
            printf("Error : Failed to clean flash %s from 0x%X to 0x%X\n", image->flash_id, image->flash_base, image->flash_base + image->size);
            continue;
        }

        printf("Flashing image [%s]:\n", image->label);
        printf("  path: \"%s\"\n  flashid: \"%s\"\n  base: 0x%X\n  size: 0x%X\n", image->path, image->flash_id, image->flash_base, image->size);
        if (flash_single_image(image->path, image->flash_id, image->flash_base))
            printf("Error : Flashing image [%s] failed\n", image->label);
        else
        {
            if (check_single_image(image->path, image->flash_id, image->flash_base, image->size))
                printf("Error : Postflash checking image [%s] failed\n", image->label);
            else
            {
                if (imginfo.token == 0)
                {
                    imginfo.token = BOOTLOADER_IMGINFO_TOKEN;
                }
                image_flashed_count++;
                if (image->base)
                {
                    bl_dbg(("DBG: Keep record of %s, 0x%X, 0x%X, %d\n", image->label, image->flash_base, image->base));
                    imginfo.flash_base[imginfo.img_count] = image->flash_base;
                    strcpy(imginfo.flash_id[imginfo.img_count], image->flash_id);
                    imginfo.base[imginfo.img_count] = image->base;
                    imginfo.size[imginfo.img_count] = image->size;
                    imginfo.core[imginfo.img_count] = image->core;
                    imginfo.img_count++;
                }
            }
        }
    }

    if (imginfo.img_count == 0)
        printf("WRN: No application image is flashed.\n");

    if (save_image_info2flash(BL_QSPI_ID, &imginfo))
        printf("WRN: It failed to update image info to flash, the images flashed might not work for next reset.\n");

    printf("Flashed %d %s in total, %d recorded.\n",
           image_flashed_count, image_flashed_count > 1 ? "images" : "image", imginfo.img_count);
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
        _task_block();
    }
    
    result = _io_sdcard_install("sdcard:", (void *)&_bsp_sdcard0_init, com_handle);
    if (result != MQX_OK)
    {
        printf("Error : unable to install sdcard driver \n");
        _task_block();
    }
    
    _time_delay (200);
    
    sdcard_handle = fopen("sdcard:", 0);
    if (sdcard_handle == NULL)
    {
        printf("Error : unable to open sdcard driver \n");
        _task_block();
    }
    
    result = _io_part_mgr_install(sdcard_handle, partman_name, 0);
    if (result != MQX_OK)
    {
        printf("Error : unable to install partition manager \n");
        _task_block();
    }
    
    partman_handle = fopen(partman_name, NULL);
    if (partman_handle == NULL)
    {
        printf("Error : unable to open partition manager \n");
        _task_block();
    }
    
    result = _io_mfs_install(partman_handle, filesystem_name, 1);
    if (result != MQX_OK)
    {
        printf("Error : unable to install MFS driver \n");
        _task_block();
    }
    
    filesystem_handle = fopen(filesystem_name, NULL);
    if (partman_handle == NULL)
    {
        printf("Error : unable to open MFS driver \n");
        _task_block();
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
#elif PSP_MQX_CPU_IS_VYBRID_M4
    printf("M4");
#endif
    if (cpu_count & 0x1) {
        printf(" as primary core, dual core\n");
        single_core = 0;
    } else {
        printf(" single core\n");
        single_core = 1;
    }
    printf("======================\n");
#if IMAGE_INFO_LOAD_FROM_FLASH
    printf("Load image info from flash\n");
    if (!load_image_info_from_flash(BL_QSPI_ID))
        skip_mount_fs = 1;
#endif

    if (!skip_mount_fs)
    {
        printf("Mounting filesystem \n");
        mount();
    }
    else
        bl_dbg(("Skip mount fs\n"));

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
    MQX_FILE_PTR flash_fp;
    md5digest_t md5;
    img_info_struct * image = NULL;
#if PSP_MQX_CPU_IS_VYBRID_A5
    char *fname = "c:/setup_a5.ini";
#elif PSP_MQX_CPU_IS_VYBRID_M4
    char *fname = "c:/setup_m4.ini";
#endif

    if (!skip_mount_fs)
    {
        /* call setup.ini parser */
        if (MQX_OK != iniparser_load(fname, &found_label, &found_property))
        {
            printf("Error : cannot open '%s' \n", fname);
            _task_block();
        }

        /* verify result of parser process */
        verify_parser();

        if (is_flash_image)
        {
            printf("Ready to flash images\n");
            process_image_flashing();
            _task_block();
        }
     }
    else
        bl_dbg(("Skip loading ini file from fs\n"));
    
    /* load & verify */
    for (i = 0; i < img_context.count; i++)
    {
        image = &img_context.images[i];

        /* skip image copying for secondary core, if it's a single core chip */
        if (single_core && (image->core != 0x0)) {
            image->flags &= ~(FLAGS_TYPE_PERFORM_RUN);
            printf("*** Skip image \"%s\" for secondary core.\n", image->path);
            continue;
        }


        /* copy binary images */
        if (!strncmp(image->path, "0x", 2))
        {
            flash_fp = fopen(image->flash_id, NULL);
            if (flash_fp == NULL)
            {
                bl_dbg(("failed to open flash controller: %s\n", image->flash_id));
                _task_block();
            }
            flash_base = (uint8_t *)strtol(image->path, NULL, 0);
            bl_dbg(("DBG: copy image from 0x%08X to 0x%08X\n", flash_base, image->base));
            memcpy((uint8_t *)image->base, flash_base, image->size);
            fclose(flash_fp);
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
    
#if PSP_MQX_CPU_IS_VYBRID_A5
    __asm("cpsid ifa");
#elif PSP_MQX_CPU_IS_VYBRID_M4
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
#if PSP_MQX_CPU_IS_VYBRID_A5
            // reset vector
            SRC_GPR(2) = ((_mqx_uint*)boot_context.images[i].base)[1];
            // stack pointer
            SRC_GPR(3) = ((_mqx_uint*)boot_context.images[i].base)[0];
#elif PSP_MQX_CPU_IS_VYBRID_M4
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

