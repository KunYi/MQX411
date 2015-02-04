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
*   This file contains definitions private to the QSPI driver.
*
*
*END************************************************************************/

#ifndef __qspi_quadspi_prv_h__
#define __qspi_quadspi_prv_h__

#include "qspi.h"
#include "qspi_quadspi.h"


/*--------------------------------------------------------------------------*/
/*
**                    CONSTANT DEFINITIONS
*/
#define QuadSPI_RD_BUF          ((unsigned int *) 0x7C000000)
#define ARDB                    (*(volatile unsigned int *) 0x7C000000)
#define QuadSPI_WR_BUF_SIZE     64

#define byte_swap32(word) \
  (((word&0x000000FF) << 24)| \
  ((word&0x0000FF00) << 8) | \
  ((word&0x00FF0000) >> 8) | \
  ((word&0xFF000000) >> 24))

#define byte_swap16(word) \
  (((word&0x00FF) << 8) | \
  ((word&0xFF00) >> 8))

typedef enum {
    QuadSPI_FLASH_READ,
    QuadSPI_FLASH_REG_READ
} QuadSPI_FLASH_READ_FLAG;

typedef enum {
    QUADSPI_READ = 0,
    QUADSPI_DUAL_READ,
    QUADSPI_QUAD_READ,
    QUADSPI_DDR_READ = 3,
    QUADSPI_DDR_DUAL_READ,
    QUADSPI_DDR_QUAD_READ,
} QuadSPI_FLASH_CMD_SEQ;


/* Clock defintion need by QuadSPI */
/* CCSR Bit Fields */
#define  CCM_CCSR_PLL1_PFD4_EN(x)   (((uint32_t)(((uint32_t)(x))<< CCM_CCSR_PLL1_PFD4_EN_SHIFT))&CCM_CCSR_PLL1_PFD4_EN_MASK)
#define  CCM_CCSR_PLL2_PFD4_EN(x)   (((uint32_t)(((uint32_t)(x))<< CCM_CCSR_PLL2_PFD4_EN_SHIFT))&CCM_CCSR_PLL2_PFD4_EN_MASK)
#define  CCM_CCSR_PLL3_PFD4_EN(x)   (((uint32_t)(((uint32_t)(x))<< CCM_CCSR_PLL3_PFD4_EN_SHIFT))&CCM_CCSR_PLL3_PFD4_EN_MASK)

/* CCOSR Bit Fields */
#define  CCM_CCOSR_CKO1_EN(x)       (((uint32_t)(((uint32_t)(x))<< CCM_CCOSR_CKO1_EN_SHIFT))& CCM_CCOSR_CKO1_EN_MASK)

/* CSCDR3 Bit Fields */
#define CCM_CSCDR3_QSPI0_X2_DIV(x)  (((uint32_t)(((uint32_t)(x))<<CCM_CSCDR3_QSPI0_X2_DIV_SHIFT))&CCM_CSCDR3_QSPI0_X2_DIV_MASK)
#define  CCM_CSCDR3_QSPI0_DIV(x)    (((uint32_t)(((uint32_t)(x))<< CCM_CSCDR3_QSPI0_DIV_SHIFT))& CCM_CSCDR3_QSPI0_DIV_MASK)
#define  CCM_CSCDR3_QSPI0_EN(x)     (((uint32_t)(((uint32_t)(x))<< CCM_CSCDR3_QSPI0_EN_SHIFT))& CCM_CSCDR3_QSPI0_EN_MASK)
#define CCM_CSCDR3_QSPI1_X2_DIV(x)  (((uint32_t)(((uint32_t)(x))<<CCM_CSCDR3_QSPI1_X2_DIV_SHIFT))&CCM_CSCDR3_QSPI1_X2_DIV_MASK)
#define CCM_CSCDR3_QSPI1_DIV(x)     (((uint32_t)(((uint32_t)(x))<<CCM_CSCDR3_QSPI1_DIV_SHIFT))&CCM_CSCDR3_QSPI1_DIV_MASK)
#define CCM_CSCDR3_QSPI1_EN(x)      (((uint32_t)(((uint32_t)(x))<<CCM_CSCDR3_QSPI1_EN_SHIFT))&CCM_CSCDR3_QSPI1_EN_MASK)

/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

/*
** QuadSPI_INFO_STRUCT
** Run time state information for each spi channel
*/
typedef struct quadspi_info_struct
{
    /* The spi device register */
    VQuadSPI_REG_STRUCT_PTR QuadSPI_REG_PTR;

        /* The spi device register */
    uint32_t QuadSPI_MEM_ADDR;

    /* Record the current flash address */
    uint32_t FLASH_CUR_ADDR;

    /* QuadSPI port number */
    uint32_t MODULE_ID;

    /* QuadSPI clock mode */
    QuadSPI_CLK_MODE CLK_MODE;

    QuadSPI_IO_MODE IO_MODE;

    /* QuadSPI serial frequency */
    uint32_t READ_CLK;
    uint32_t WRITE_CLK;

    /* Page Size */
    QuadSPI_PAGE_SIZE PAGE_SIZE;

    uint32_t FLASH_TOP_ADDR[4];

    /* External QuadSPI Flash Device */
    QuadSPI_FLASH_INFO_STRUCT      *HW_BLOCK;

    QuadSPI_FLASH_READ_FLAG FLASH_READ_FLAG;

    /* QuadSPI parallel mode */
    bool PARALLEL_ENABLED;

    /* QuadSPI parallel mode for IP read */
    bool IP_READ_IN_PARALLEL;
} QuadSPI_INFO_STRUCT, * QuadSPI_INFO_STRUCT_PTR;

typedef volatile struct quadspi_info_struct * VQuadSPI_INFO_STRUCT_PTR;

#endif
