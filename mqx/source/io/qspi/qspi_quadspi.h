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
*   This file contains the definitions of constants and structures
*   required for the QuadSPI driver
*
*
*END************************************************************************/

#ifndef __qspi_quadspi_h__
#define __qspi_quadspi_h__

#include <bsp.h>


/*--------------------------------------------------------------------------*/
/*
**                    CONSTANT DEFINITIONS
*/

#define QUADSPI_REGS_POLLING_MAX     0xFFFFFFFF

/* IOCTL defintion */
#define QuadSPI_IOCTL_SET_SDR           _IO(IO_TYPE_QSPI, 0x50)
#define QuadSPI_IOCTL_SET_DDR           _IO(IO_TYPE_QSPI, 0x51)
#define QuadSPI_IOCTL_SET_SINGLE_IO     _IO(IO_TYPE_QSPI, 0x52)
#define QuadSPI_IOCTL_SET_QUAD_IO       _IO(IO_TYPE_QSPI, 0x53)
#define QuadSPI_IOCTL_SET_DUAL_IO       _IO(IO_TYPE_QSPI, 0x54)
#define QuadSPI_IOCTL_SET_READ_SPEED    _IO(IO_TYPE_QSPI, 0x55)
#define QuadSPI_IOCTL_SET_WRITE_SPEED   _IO(IO_TYPE_QSPI, 0x56)
#define QuadSPI_IOCTL_SET_FLASH_ADDR    _IO(IO_TYPE_QSPI, 0x57)
#define QuadSPI_IOCTL_GET_MEM_BASE_ADDR _IO(IO_TYPE_QSPI, 0x58)
#define QuadSPI_IOCTL_INVALID_BUFFER    _IO(IO_TYPE_QSPI, 0x59)
#define QuadSPI_IOCTL_GET_IO_MODE       _IO(IO_TYPE_QSPI, 0x5a)
#define QuadSPI_IOCTL_GET_CLK_MODE      _IO(IO_TYPE_QSPI, 0x5b)
#define QuadSPI_IOCTL_GET_FLASH_INFO    _IO(IO_TYPE_QSPI, 0x5c)
#define QuadSPI_IOCTL_GET_MEM_TOTAL_LEN _IO(IO_TYPE_QSPI, 0x5d)
#define QuadSPI_IOCTL_GET_PARA_MODE     _IO(IO_TYPE_QSPI, 0x5e)
#define QuadSPI_IOCTL_GET_IP_PARA_MODE  _IO(IO_TYPE_QSPI, 0x5f)
#define QuadSPI_IOCTL_SET_IP_PARA_MODE  _IO(IO_TYPE_QSPI, 0x60)


/* QuadSPI Error Codes: Begin from SPI_ERROR_BASE + 0x50 */
#define QuadSPI_INVAID_PARAMETER        (QSPI_ERROR_BASE | 0x50)
#define QuadSPI_ERROR_DEVICE_INVALID    (QSPI_ERROR_BASE | 0x51)
#define QuadSPI_ERROR_NOT_SUPPORT       (QSPI_ERROR_BASE | 0x52)

/* QuadSPI Lookup table command definiton */
#define QuadSPI_INST_STOP          0x0
#define QuadSPI_INST_CMD           0x1
#define QuadSPI_INST_ADDR          0x2
#define QuadSPI_INST_DUMMY         0x3
#define QuadSPI_INST_MODE          0x4
#define QuadSPI_INST_MODE2         0x5
#define QuadSPI_INST_MODE4         0x6
#define QuadSPI_INST_READ          0x7
#define QuadSPI_INST_WRITE         0x8
#define QuadSPI_INST_JMP_ON_CS     0x9
#define QuadSPI_INST_ADDR_DDR      0xa
#define QuadSPI_INST_MODE_DDR      0xb
#define QuadSPI_INST_MODE2_DDR     0xc
#define QuadSPI_INST_MODE4_DDR     0xd
#define QuadSPI_INST_READ_DDR      0xe
#define QuadSPI_INST_WRITE_DDR     0xf
#define QuadSPI_INST_DATA_LEARN    0x10

#define QuadSPI_SINGLE_PAD         0x0
#define QuadSPI_DUAL_PAD           0x1
#define QuadSPI_QUAD_PAD           0x2

#define QuadSPI_INST(x)                ((x & 0x3F) << 10)
#define QuadSPI_PAD(x)                 ((x & 0x3) << 8)
#define QuadSPI_OPR(x)                 (x & 0xFF)
#define QuadSPI_SEQ(inst, pad, opr)    (QuadSPI_INST(inst) | QuadSPI_PAD(pad) | QuadSPI_OPR(opr))

#define QuadSPI_GET_INST(x)            ((x >> 10) & 0x3F)
#define QuadSPI_GET_PAD(x)             ((x >> 8 ) & 0x3)
#define QuadSPI_GET_OPR(x)             (x & 0xFF)

#define QuadSPI_LOOKUP_STOP            QuadSPI_SEQ(0, 0, 0)

#define QuadSPI_READ_BUF_SIZE     128

#define QuadSPI_CMD_SEQ_LEN        8

/* Define QuadSPI clock mode*/
typedef enum {
    QuadSPI_CLK_SDR_MODE = 0,
    QuadSPI_CLK_DDR_MODE
} QuadSPI_CLK_MODE;

typedef enum {
    QuadSPI_SINGLE_MODE = 0,
    QuadSPI_DUAL_MODE = 0x1,
    QuadSPI_QUAD_MODE = 0x2
} QuadSPI_IO_MODE;

typedef enum {
    QuadSPI_PAGE_128 = 128,
    QuadSPI_PAGE_256 = 256,
    QuadSPI_PAGE_512 = 512
} QuadSPI_PAGE_SIZE;

/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/
/*--------------------------------------------------------------------------*/
/* Peripheral register access */
typedef struct quadspi_reg_struct {
    uint32_t MCR;               /*! Module Configuration Register, offset: 0x0 */
    uint8_t RESERVED_0[4];
    uint32_t IPCR;              /*! IP Configuration Register, offset: 0x8 */
    uint32_t FLSHCR;            /*! Flash Configuration Register, offset: 0xC */
    uint32_t BUF0CR;            /*! Buffer0 Configuration Register, offset: 0x10 */
    uint32_t BUF1CR;            /*! Buffer1 Configuration Register, offset: 0x14 */
    uint32_t BUF2CR;            /*! Buffer2 Configuration Register, offset: 0x18 */
    uint32_t BUF3CR;            /*! Buffer3 Configuration Register, offset: 0x1C */
    uint32_t BFGENCR;           /*! Buffer Generic Configuration Register, offset: 0x20 */
    uint32_t SOCCR;             /*! SOC Configuration Register, offset: 0x24 */
    uint8_t RESERVED_1[8];
    uint32_t BUF0IND;           /*! Buffer0 Top Index Register, offset: 0x30 */
    uint32_t BUF1IND;           /*! Buffer1 Top Index Register, offset: 0x34 */
    uint32_t BUF2IND;           /*! Buffer2 Top Index Register, offset: 0x38 */
    uint8_t RESERVED_2[196];
    uint32_t SFAR;              /*! Serial Flash Address Register, offset: 0x100 */
    uint8_t RESERVED_3[4];
    uint32_t SMPR;              /*! Sampling Register, offset: 0x108 */
    uint32_t RBSR;              /*! RX Buffer Status Register, offset: 0x10C */
    uint32_t RBCT;              /*! RX Buffer Control Register, offset: 0x110 */
    uint8_t RESERVED_4[60];
    uint32_t TBSR;              /*! TX Buffer Status Register, offset: 0x150 */
    uint32_t TBDR;              /*! TX Buffer Data Register, offset: 0x154 */
    uint8_t RESERVED_5[4];
    uint32_t SR;                /*! Status Register, offset: 0x15C */
    uint32_t FR;                /*! Flag Register, offset: 0x160 */
    uint32_t RSER;              /*! Interrupt and DMA Request Select and Enable Register, offset: 0x164 */
    uint32_t SPNDST;            /*! Sequence Suspend Status Register, offset: 0x168 */
    uint32_t SPTRCLR;           /*! Sequence Pointer Clear Register, offset: 0x16C */
    uint8_t RESERVED_6[16];
    uint32_t SFA1AD;            /*! Serial Flash A1 Top Address, offset: 0x180 */
    uint32_t SFA2AD;            /*! Serial Flash A2 Top Address, offset: 0x184 */
    uint32_t SFB1AD;            /*! Serial Flash B1Top Address, offset: 0x188 */
    uint32_t SFB2AD;            /*! Serial Flash B2Top Address, offset: 0x18C */
    uint8_t RESERVED_7[112];
    uint32_t RBDR[32];              /*! RX Buffer Data Register, offset: 0x200 */
    uint8_t RESERVED_8[128];
    uint32_t LUTKEY;            /*! LUT Key Register, offset: 0x300 */
    uint32_t LCKCR;             /*! LUT Lock Configuration Register, offset: 0x304 */
    uint8_t RESERVED_9[8];
    uint32_t LUT[64];           /*! Look-up Table register, array offset: 0x310, array step: 0x4 */
} QuadSPI_REG_STRUCT, * QuadSPI_REG_STRUCT_PTR;

typedef volatile struct quadspi_reg_struct * VQuadSPI_REG_STRUCT_PTR;

/*
** QuadSPI_FLASH_INFO_STRUCT
**
** This structure defines the flash blocks beloning to specific flash device
*/
typedef struct quadspi_flash_info_struct
{
    /* Physical start address of the block of same size sectors */
    _mqx_uint START_ADDR;

    /* The number of sectors with the same size in this block */
    _mqx_uint NUM_SECTOR;

    /* The size of the sectors in this block */
    _mem_size SECTOR_SIZE;
} QuadSPI_FLASH_INFO_STRUCT, * QuadSPI_FLASH_INFO_STRUCT_PTR;

typedef const QuadSPI_FLASH_INFO_STRUCT * QuadSPI_FLASH_INFO_STRUCT_CPTR;

/*
** QuadSPI_FLASH_CMD_STRUCT
**
** This structure defines the flash command belonging to specific flash device
*/
typedef struct quadspi_flash_cmd_struct
{
    /* Set of flash command: read */
    uint16_t  READ_CMD[QuadSPI_CMD_SEQ_LEN];

    /* Set of flash command: dual read */
    uint16_t  DUAL_READ_CMD[QuadSPI_CMD_SEQ_LEN];

    /* Set of flash command: quad read */
    uint16_t  QUAD_READ_CMD[QuadSPI_CMD_SEQ_LEN];

        /* Set of flash command: ddr read */
    uint16_t  DDR_READ_CMD[QuadSPI_CMD_SEQ_LEN];

    /* Set of flash command: ddr dual read */
    uint16_t  DDR_DUAL_READ_CMD[QuadSPI_CMD_SEQ_LEN];

    /* Set of flash command: ddr quad read */
    uint16_t  DDR_QUAD_READ_CMD[QuadSPI_CMD_SEQ_LEN];

} QuadSPI_FLASH_CMD_STRUCT, * QuadSPI_FLASH_CMD_STRUCT_PTR;

typedef const QuadSPI_FLASH_CMD_STRUCT * QuadSPI_FLASH_CMD_STRUCT_CPTR;
/*
** QuadSPI_INIT_STRUCT
**
** This structure defines the initialization parameters to be used
** when a qspi port is initialized.
*/
typedef struct quadspi_init_struct
{
    /* QuadSPI module number */
    uint32_t MODULE_ID;

     /* QuadSPI clock mode */
    QuadSPI_CLK_MODE CLK_MODE;
    QuadSPI_IO_MODE IO_MODE;

    uint32_t READ_CLK;
    uint32_t WRITE_CLK;

    QuadSPI_PAGE_SIZE PAGE_SIZE;

    /* External QuadSPI Flash Device */
    QuadSPI_FLASH_INFO_STRUCT *FLASH_DEVICE_INFO;

    /* External QuadSPI Flash Command*/
    QuadSPI_FLASH_CMD_STRUCT *FLASH_CMD;

    /* QuadSPI Parallel Mode */
    bool PARALLEL_ENABLED;

} QuadSPI_INIT_STRUCT, * QuadSPI_INIT_STRUCT_PTR;

typedef const QuadSPI_INIT_STRUCT * QuadSPI_INIT_STRUCT_CPTR;

/*
** QuadSPI_MEM_BLOCK_STRUCT
**
** This structure defines the memory blocks which is identified by start address and block size.
*/
typedef struct quadspi_mem_block
{
    /* start address of the memory block */
    void * ADDR;

    /* The size of this memory block */
    _mem_size SIZE;
} QuadSPI_MEM_BLOCK_STRUCT, * QuadSPI_MEM_BLOCK_STRUCT_PTR;

typedef const QuadSPI_MEM_BLOCK_STRUCT * QuadSPI_MEM_BLOCK_STRUCT_CPTR;

/*----------------------------------------------------------------------*/
/*
**              DEFINED VARIABLES
*/

extern const QSPI_DEVIF_STRUCT _qspi_quadspi_devif;


/*--------------------------------------------------------------------------*/
/*
**                        FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern void   *_bsp_get_quadspi_reg_address(uint32_t dev_num);
extern uint32_t _bsp_get_quadspi_base_address(uint32_t dev_num);
extern uint32_t _bsp_get_quadspi_vectors(uint32_t dev_num, const uint32_t  **vectors_ptr);

extern _mqx_int _quadspi_flash_enable_serial_clk (uint32_t quadspi_id);
extern _mqx_int _quadspi_flash_disable_serial_clk (uint32_t quadspi_id);

extern uint32_t _quadspi_flash_set_serial_clk (VQuadSPI_REG_STRUCT_PTR quadspi_regs_ptr,
                                                                uint32_t quadspi_id, uint32_t serial_clk);
extern void _quadspi_lockLookupTable(VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr);
extern void _quadspi_unlockLookupTable(VQuadSPI_REG_STRUCT_PTR quadspi_reg_ptr);
extern void _quadspi_invalid_buffer(VQuadSPI_REG_STRUCT_PTR qspi_reg_ptr, void *addr, _mem_size size);

#ifdef __cplusplus
}
#endif

#endif
