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
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the source for the bootloader_vybrid_qspixip example program.
*
*
*END************************************************************************/
#include <quadspi_boot.h>
#include <mqx.h>
#include <bsp.h>



#if defined(__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".conf")))
#elif defined(__ICCARM__)
#pragma location=".conf"
#endif

#if defined(BSP_VYBRID_AUTOEVB_A5) || defined(BSP_VYBRID_AUTOEVB_M4)
#define A1_FLASH_SIZE           0x2000000
#define A2_FLASH_SIZE           0
#define B1_FLASH_SIZE           0x2000000
#define B2_FLASH_SIZE           0
#define SET_RD_CMD              0x0413
#define SET_ADDR_CMD            0x0820
#elif defined(BSP_TWR_VF65GS10_A5) || defined(BSP_TWR_VF65GS10_M4)
#define A1_FLASH_SIZE           0x1000000
#define A2_FLASH_SIZE           0
#define B1_FLASH_SIZE           0x1000000
#define B2_FLASH_SIZE           0
#define SET_RD_CMD              0x0403
#define SET_ADDR_CMD            0x0818
#endif

/*
#define IMAGE_ENTRY_FUNCTION  0x3f040800
  #define FLASH_BASE            0x3f040000
  #define FLASH_END             0x3f080000
*/
const SFLASH_CONFIGURATION_PARAM quadspi_conf = {
  0,                      /* DQS LoopBack */
  0,                      /* Reserved 1*/
  0,                      /* Reserved 2*/
  0,                      /* Reserved 3*/
  0,                      /* Reserved 4*/
  0,                      /* cs_hold_time */
  0,                      /* cs_setup_time */
  A1_FLASH_SIZE,              /* A1 flash size */
  A2_FLASH_SIZE,              /* A2 flash size */
  B1_FLASH_SIZE,              /* B1 flash size */
  B2_FLASH_SIZE,              /* B2 flash size */
 SCLK,                      /* SCLK Freq */
  0,                      /* Reserved 5*/
  1,                      /* Single Mode Flash */
  1,                      /* Port - Only A1 */
  0,                      /* DDR Mode Disable */
  0,                      /* DQS Disable */
  0,                      /* Parallel Mode Disable */
  0,                      /* Port A CS1 */
  0,                      /* Port B CS1 */
  0,                      /* FS Phase */
  0,                      /* FS Delay */
  0,                      /* DDR Sampling */
  /* LUT Programming */
  SET_RD_CMD,
  SET_ADDR_CMD,
  0x1c08,
  0x2400,
};
#if defined(__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".boot_data")))
#elif defined(__ICCARM__)
#pragma location=".boot_data"
#endif

const BOOT_DATA_T boot_data = {
  FLASH_BASE,                 /* boot start location */
  (FLASH_END-FLASH_BASE),     /* size */
  PLUGIN_FLAG,                /* Plugin flag*/
  0xFFFFFFFF  				  /* empty - extra data word */
};

#if defined(__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".ivt")))
#elif defined(__ICCARM__)
#pragma location=".ivt"
#endif


const ivt image_vector_table = {
  IVT_HEADER,                         /* IVT Header */
  (uint32_t)&__boot,     /* Image  Entry Function */
  IVT_RSVD,                           /* Reserved = 0 */
  (uint32_t)DCD_ADDRESS,              /* Address where DCD information is stored */
  (uint32_t)BOOT_DATA_ADDRESS,        /* Address where BOOT Data Structure is stored */
  (uint32_t)&image_vector_table,      /* Pointer to IVT Self (absolute address */
  (uint32_t)CSF_ADDRESS,              /* Address where CSF file is stored */
  IVT_RSVD                            /* Reserved = 0 */
};

#if defined(__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".dcd")))
#elif defined(__ICCARM__)
#pragma location=".dcd"
#endif

const uint32_t device_config_data[DCD_ARRAY_SIZE] = {
  /* DCD Header */
  (uint32_t)(DCD_TAG_HEADER |                       \
    (0x2000<< 8) | \
    (DCD_VERSION << 24)),
  /* DCD Commands */
  /* Add additional DCD Commands here */
};
const uint8_t fill_data[0x3CC]; //dc - fill data so ds-5 parser aligns correctly Fill 0x434-0x800.
