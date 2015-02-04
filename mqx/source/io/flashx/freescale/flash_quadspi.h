
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

#ifndef __flash_quadspi_h__
#define __flash_quadspi_h__


/*--------------------------------------------------------------------------*/
/*
**             Flashx QuadSPI IOCTL Defintion
*/

#define FLASH_QUADSPI_IOCTL_SET_CLOCK           _IO(IO_TYPE_FLASH,0x50)
#define FLASH_QUADSPI_IOCTL_GET_CLOCK           _IO(IO_TYPE_FLASH,0x51)
#define FLASH_QUADSPI_IOCTL_SET_IO_MODE         _IO(IO_TYPE_FLASH,0x52)
#define FLASH_QUADSPI_IOCTL_GET_IO_MODE         _IO(IO_TYPE_FLASH,0x53)
#define FLASH_QUADSPI_IOCTL_SET_CLK_MODE        _IO(IO_TYPE_FLASH,0x54)
#define FLASH_QUADSPI_IOCTL_GET_CLK_MODE        _IO(IO_TYPE_FLASH,0x55)

/*----------------------------------------------------------------------*/
/*
**              ERROR CODE
*/
#define FLASHXERR_QUADSPI_INVALID_PARM  (FLASHX_ERROR_BASE | 0x50)
#define FLASHXERR_QUADSPI_PROGRAM       (FLASHX_ERROR_BASE | 0x51)
#define FLASHXERR_QUADSPI_SET_QUAD_MODE (FLASHX_ERROR_BASE | 0x52)
/*----------------------------------------------------------------------*/
/*
**              DEFINED VARIABLES
*/
typedef enum {
    FLASHX_QUADSPI_A1_TAG,
    FLASHX_QUADSPI_A2_TAG,
    FLASHX_QUADSPI_B1_TAG,
    FLASHX_QUADSPI_B2_TAG
} FLASHX_QUADSPI_FLASH_TAG;

/*
** QuadSPI_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a qspi port is initialized.
*/
typedef struct _flashx_quadspi_init_struct
{
    /* QuadSPI module number */
    uint32_t MODULE_ID;

     /* QuadSPI clock mode */
    QuadSPI_CLK_MODE CLK_MODE;

    /* QuadSPI IO mode: single/dual/quad pad*/
   QuadSPI_IO_MODE IO_MODE;

    /* Serial clock output to the serial flash device */
    uint32_t SERIAL_CLK;

    /* Parallel Mode */
    bool PARALLEL_ENABLED;

} FLASHX_QUADSPI_INIT_STRUCT, * FLASHX_QUADSPI_INIT_STRUCT_PTR;

typedef const FLASHX_QUADSPI_INIT_STRUCT * FLASHX_QUADSPI_INIT_STRUCT_CPTR;


extern const FLASHX_DEVICE_IF_STRUCT _flashx_quadspi_if;

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif

_mqx_int _quadspi_send_command(VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,
                                        char *dest_ptr, uint8_t seqid, _mem_size data_size);
int32_t _quadspi_program_64bytes (VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,
                                        char *src_ptr, char *dest_ptr, _mem_size size);
_mqx_int _quadspi_flashx_setup_LUT(VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr);
_mqx_int _quadspi_memory_erase_sector(VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr, char *dest_ptr);
_mqx_int _quadspi_memory_chip_erase(VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr, char *dest_ptr);
int32_t _quadspi_memory_program (VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr,
                                        char *src_ptr, char *dest_ptr, _mem_size size);
uint32_t _quadspi_memory_get_sector_size(VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr, char *dest_ptr);
uint32_t _quadspi_memory_enable_quad_mode (VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr, char *dest_ptr, bool enable);

#ifdef __cplusplus
}
#endif


#endif //__flash_quadspi_h__
