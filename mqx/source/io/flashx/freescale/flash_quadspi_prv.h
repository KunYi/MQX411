
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
*   The file contains function prototypes and defines for the internal
*   flash driver.
*   
*
*END************************************************************************/

#ifndef __flash_quadspi_prv_h__
#define __flash_quadspi_prv_h__

/*----------------------------------------------------------------------*/
/*
**              DEFINED VARIABLES
*/

#define byte_swap32(word) \
  (((word&0x000000FF) << 24)| \
  ((word&0x0000FF00) << 8) | \
  ((word&0x00FF0000) >> 8) | \
  ((word&0xFF000000) >> 24))

typedef enum {
    FLASHX_QUADSPI_READ = 0,
    FLASHX_QUADSPI_DUAL_READ,
    FLASHX_QUADSPI_QUAD_READ,
    FLASHX_QUADSPI_DDR_READ,
    FLASHX_QUADSPI_DDR_DUAL_READ,
    FLASHX_QUADSPI_DDR_QUAD_READ,
    FLASHX_QUADSPI_WRITE_EN,
    FLASHX_QUADSPI_SECTOR_ERASE,
    FLASHX_QUADSPI_CHIP_ERASE,
    FLASHX_QUADSPI_PAGE_PROM,
    FLASHX_QUADSPI_WRITE_CONFIG,
    FLASHX_QUADSPI_READ_STATUS,
    FLASHX_QUADSPI_READ_CONFIG,
    FLASHX_QUADSPI_READ_ID
} FLASHX_QUADSPI_FLASH_CMD_SEQ;


/*
** QuadSPI_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a qspi port is initialized.
*/
typedef struct _flashx_quadspi_internal_struct
{
    /* The spi device register */
    VQuadSPI_REG_STRUCT_PTR QuadSPI_REG_PTR;

    /* QuadSPI port number */
    uint32_t MODULE_ID;

    /* QuadSPI clock mode */
    QuadSPI_CLK_MODE CLK_MODE;

    QuadSPI_IO_MODE IO_MODE;

    /* QuadSPI serial frequency */
    uint32_t SERIAL_CLK;

    uint32_t FLASH_TOP_ADDR[4];

    /* QuadSPI parallel mode */
    bool PARALLEL_ENABLED;

} FLASHX_QUADSPI_INTERNAL_STRUCT, * FLASHX_QUADSPI_INTERNAL_STRUCT_PTR;

typedef const FLASHX_QUADSPI_INTERNAL_STRUCT * FLASHX_QUADSPI_INTERNAL_STRUCT_CPTR;

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

bool quadspi_flash_init(IO_FLASHX_STRUCT_PTR);
void      quadspi_flash_deinit(IO_FLASHX_STRUCT_PTR);
bool quadspi_flash_erase_sector(IO_FLASHX_STRUCT_PTR, char *, _mem_size);
bool quadspi_flash_write_sector(IO_FLASHX_STRUCT_PTR, char *, char *, _mem_size);
bool quadspi_flash_chip_erase(IO_FLASHX_STRUCT_PTR);
_mqx_int quadspi_flash_ioctl(IO_FLASHX_STRUCT_PTR, _mqx_uint, void *);

#ifdef __cplusplus
}
#endif

#endif //__flash_quadspi_h__
