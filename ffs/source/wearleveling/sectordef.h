#ifndef __sectordef_h__
#define __sectordef_h__
/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains constants for different sector sizes and related macros.
* The definitions in this file should be used exclusively to get the
* sizes of various types of sectors. When allocating a sector buffer,
* use either the #SECTOR_BUFFER_ALLOC_SIZE macro or one of the following:
* - #NOMINAL_DATA_SECTOR_ALLOC_SIZE
* - #NOMINAL_AUXILIARY_SECTOR_ALLOC_SIZE
*
*
*END************************************************************************/

/* 
** Constants 
*/

/* 
** Macros 
*/

/* Utility macros */
/* Simple macro to round up an integer division */
#define ROUND_UP_DIV(a, b)                      (((a) + (b) - 1) / (b))

/* Macro to round an integer up to a given multiple. */
#define ROUND_UP(n, m)                          ((ROUND_UP_DIV((n), (m))) * (m))

/* Macro to compute the number of 32-bit words required to hold \a a bytes. */
#define SIZE_IN_WORDS(a)                        (ROUND_UP_DIV((a), sizeof(uint32_t)))

/* Buffer alignment and multiple size in bytes required for data cache issues. */
#define BUFFER_CACHE_LINE_MULTIPLE_IN_WORDS     (SIZE_IN_WORDS(BUFFER_CACHE_LINE_MULTIPLE))

/* Cache line definitions */
#ifdef __MQX__
#define BUFFER_CACHE_LINE_MULTIPLE      (PSP_CACHE_LINE_SIZE)
#else
/* Buffer alignment and multiple size in bytes required for data cache issues. */
#define BUFFER_CACHE_LINE_MULTIPLE      (32)
#endif

/* 512 byte page sizes */
#define MMC_SECTOR_TOTAL_SIZE           (528)
#define MMC_SECTOR_DATA_SIZE            (512)
#define MMC_SECTOR_REDUNDANT_SIZE       (16)

/* 2K page sizes */
/* Size in bytes of a 2K NAND page. */
#define LARGE_SECTOR_TOTAL_SIZE                 (2112)

/* Size in bytes of the data area of a 2K NAND page. */
#define LARGE_SECTOR_DATA_SIZE                  (2048)

/* Size in bytes of the redundant area of a 2K NAND sector. */
#define LARGE_SECTOR_REDUNDANT_SIZE             (64)

/* Size in 32-bit words of the redundant area of a 2K NAND sector. */
#define LARGE_SECTOR_REDUNDANT_SIZE_IN_WORDS    (SIZE_IN_WORDS(LARGE_SECTOR_REDUNDANT_SIZE))

/* 4K page sizes */
/* Size in bytes of a 4K NAND page. */
#define XL_SECTOR_TOTAL_SIZE                    (4314)

/* Size in bytes of the data area of a 4K NAND page. */
#define XL_SECTOR_DATA_SIZE                     (4096)

/* Size in words of the data area of a 4K NAND sector. */
#define XL_SECTOR_DATA_SIZE_IN_WORDS            (SIZE_IN_WORDS(XL_SECTOR_DATA_SIZE))

/* Size in bytes of the redundant area of a 4K NAND sector. */
#define XL_SECTOR_REDUNDANT_SIZE                (218)

/* Size in 32-bit words of the redundant area of a 4K NAND sector. */
#define XL_SECTOR_REDUNDANT_SIZE_IN_WORDS       (SIZE_IN_WORDS(XL_SECTOR_REDUNDANT_SIZE))

/* Samsung 4K page sizes */
/* Size in bytes of a Samsung 4K NAND page. */
#define SAMSUNG_XL_SECTOR_TOTAL_SIZE                (4224)

/* Size in bytes of the redundant area of a Samsung 4K NAND sector. */
#define SAMSUNG_XL_SECTOR_REDUNDANT_SIZE            (128)

/* Size in 32-bit words of the redundant area of a Samsung 4K NAND sector. */
#define SAMSUNG_XL_SECTOR_REDUNDANT_SIZE_IN_WORDS   (SIZE_IN_WORDS(SAMSUNG_XL_SECTOR_REDUNDANT_SIZE))

/* Toshiba 8K page sizes */
/* Size in bytes of a Toshiba 8K NAND page. */
#define TOSHIBA_8K_SECTOR_TOTAL_SIZE            (8568)

/* Size in bytes of the data area of an 8K NAND page. */
#define TOSHIBA_8K_SECTOR_DATA_SIZE             (8192)

/* Size in bytes of the redundant area of an 8K NAND page. */
#define TOSHIBA_8K_SECTOR_REDUNDANT_SIZE        (376)

/* Samsung 8K page sizes */
/* Size in bytes of a Samsung 8K NAND page. */
#define SAMSUNG_8K_SECTOR_TOTAL_SIZE (8628)

/* Size in bytes of the data area of an 8K NAND page. */
#define SAMSUNG_8K_SECTOR_DATA_SIZE (8192)

// Size in bytes of the redundant area of an 8K NAND page.
#define SAMSUNG_8K_SECTOR_REDUNDANT_SIZE (436)


/* 
** Maximum supported page sizes
** On some systems, the maximum size is 8K. On others, it is set to 4K. 
** The difference is mostly based on how much the system can tolerate wasted memory. 
*/
#if defined(STMP378x) || defined(STMP377x)

/* Size in bytes of the largest sector type we support. */
#define MAX_SECTOR_TOTAL_SIZE               (SAMSUNG_8K_SECTOR_TOTAL_SIZE)

/* Size in bytes of the data area of the largest sector type we support. */
#define MAX_SECTOR_DATA_SIZE                (SAMSUNG_8K_SECTOR_DATA_SIZE)

/* Size in bytes of the redundant area of the largest sector type we support. */
#define MAX_SECTOR_REDUNDANT_SIZE           (SAMSUNG_8K_SECTOR_REDUNDANT_SIZE)

#else /* defined(STMP378x) */

/* Size in bytes of the largest sector type we support. */
#define MAX_SECTOR_TOTAL_SIZE               (XL_SECTOR_TOTAL_SIZE)

/* Size in bytes of the data area of the largest sector type we support. */
#define MAX_SECTOR_DATA_SIZE                (XL_SECTOR_DATA_SIZE)

/* Size in bytes of the redundant area of the largest sector type we support. */
#define MAX_SECTOR_REDUNDANT_SIZE           (XL_SECTOR_REDUNDANT_SIZE)

#endif /* defined(STMP378x) */

/* Size in 32-bit words of the redundant area of the largest sector type we support. */
#define MAX_SECTOR_DATA_SIZE_IN_WORDS       (SIZE_IN_WORDS(MAX_SECTOR_DATA_SIZE))

/* Size in 32-bit words of the redundant area of the largest sector type we support. */
#define MAX_SECTOR_REDUNDANT_SIZE_IN_WORDS  (SIZE_IN_WORDS(MAX_SECTOR_REDUNDANT_SIZE))

/* Auxiliary buffer sizes */
/* 
** Size in bytes of the auxiliary buffer required by the ECC8 Reed-Solomon 4-bit mode,
** used for 2K NAND pages. 
*/
#define AUXILIARY_BUFFER_ECC_RS4_SIZE           (188)

/* 
** Size in bytes of the auxiliary buffer required for ECC RS8 on 4K page Nands 
** till BCH 16 on 8K Page Nands.
** The size has been rounded up to 500. 
** Tested to ensure room for Samsung 8K Page Nands the Redundant area, 
** Status Bytes and the Parity. 
*/
#define AUXILIARY_BUFFER_ECC_RS8_SIZE           (500)

/* The amount of memory allocated for RA */
#if defined(BSP_TWRMCF54418) || defined(BSP_TWR_K70F120M) || defined(BSP_TWR_VF65GS10)
#define REDUNDANT_AREA_BUFFER_ALLOCATION        (8)
#elif defined(BSP_TWRMPC5125)
/* Buffer size suitable with 4-error correction bits (8 ECC bytes) */
#define REDUNDANT_AREA_BUFFER_ALLOCATION        (56)
#else
#define REDUNDANT_AREA_BUFFER_ALLOCATION        (500)
#endif

/* 
** Nominal buffer sizes
** These defines are the sizes of buffers to use when statically allocating a buffer
** that must be large enough to support any media type. 
*/
/* 
** Compute the size in bytes of a buffer.
** Rounds up to the nearest 32-byte multiple in order to span a complete cache
** line. Note that you still need to align the buffer to the cache line size
** (i.e., #BUFFER_CACHE_LINE_MULTIPLE). 
*/
#define CACHED_BUFFER_SIZE(a)                   (ROUND_UP((a), BUFFER_CACHE_LINE_MULTIPLE))

/* 
** Compute the size in words of a buffer.
** Rounds up to the nearest 32-byte multiple in order to span a complete cache
** line. Note that you still need to align the buffer to the cache line size
** (i.e., #BUFFER_CACHE_LINE_MULTIPLE). 
*/
#define CACHED_BUFFER_SIZE_IN_WORDS(a)          (SIZE_IN_WORDS(CACHED_BUFFER_SIZE(a)))

#define SECTOR_BUFFER_ALLOC_SIZE(a)             (CACHED_BUFFER_SIZE_IN_WORDS(a))

/* Constant to use when allocating a data sector buffer by words. */
#define NOMINAL_DATA_SECTOR_ALLOC_SIZE          (SECTOR_BUFFER_ALLOC_SIZE(MAX_SECTOR_DATA_SIZE))

/* Constant to use when allocating a auxiliary sector buffer by words. */
#define NOMINAL_AUXILIARY_SECTOR_ALLOC_SIZE     (SECTOR_BUFFER_ALLOC_SIZE(REDUNDANT_AREA_BUFFER_ALLOCATION))

/* Constant to use when allocating a data sector buffer by bytes. */
#define NOMINAL_DATA_SECTOR_SIZE                (NOMINAL_DATA_SECTOR_ALLOC_SIZE*sizeof(uint32_t))

/* Constant to use when allocating a auxiliary sector buffer by bytes. */
#define NOMINAL_AUXILIARY_SECTOR_SIZE           (NOMINAL_AUXILIARY_SECTOR_ALLOC_SIZE*sizeof(uint32_t))

#endif /* __sectordef_h__ */

/* EOF */
