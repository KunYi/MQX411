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
*   This file contains the initialization definition for DSPI driver
*
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>


/*
** Parameters for SPI initialization
*/

#if BSPCFG_DSPI0_USE_DMA

static const DSPI_DMA_INIT_STRUCT _bsp_dspi0_init = {
    0,                            /* SPI channel */
    CM_CLOCK_SOURCE_BUS,          /* Relevant module clock source */
    BSP_DSPI0_DMA_RX_CHANNEL,     /* DMA channel for RX */
    BSP_DSPI0_DMA_TX_CHANNEL,     /* DMA channel for TX */
    BSP_DSPI0_DMA_RX_SOURCE,      /* Source to be used with RX channel */
    BSP_DSPI0_DMA_TX_SOURCE       /* Source to be used with TX channel */
};

const SPI_INIT_STRUCT _bsp_spi0_init = {
    &_spi_dspi_dma_devif,         /* Low level driver interface */
    &_bsp_dspi0_init,             /* Low level driver init data */
    {                             /* Default parameters: */
        10000000,                     /* Baudrate */
        SPI_CLK_POL_PHA_MODE0,        /* Mode */
        8,                            /* Frame size */
        1,                            /* Chip select */
        0,                            /* Attributes */
        0xFFFFFFFF                    /* Dummy pattern */
    }
};

#else /* BSPCFG_DSPI0_USE_DMA */

static const DSPI_INIT_STRUCT _bsp_dspi0_init = {
    0,                            /* SPI channel */
    CM_CLOCK_SOURCE_BUS           /* Relevant module clock source */
};

const SPI_INIT_STRUCT _bsp_spi0_init = {
    &_spi_dspi_devif,             /* Low level driver interface */
    &_bsp_dspi0_init,             /* Low level driver init data */
    {                             /* Default parameters: */
        10000000,                     /* Baudrate */
        SPI_CLK_POL_PHA_MODE0,        /* Mode */
        8,                            /* Frame size */
        1,                            /* Chip select */
        DSPI_ATTR_USE_ISR,            /* Attributes */
        0xFFFFFFFF                    /* Dummy pattern */
    }
};

#endif /* DSPI0_USE_DMA */


#if BSPCFG_DSPI1_USE_DMA

static const DSPI_DMA_INIT_STRUCT _bsp_dspi1_init = {
    1,                            /* SPI channel */
    CM_CLOCK_SOURCE_BUS,          /* Relevant module clock source */
    BSP_DSPI1_DMA_RX_CHANNEL,     /* DMA channel for RX */
    BSP_DSPI1_DMA_TX_CHANNEL,     /* DMA channel for TX */
    BSP_DSPI1_DMA_RX_SOURCE,      /* Source to be used with RX channel */
    BSP_DSPI1_DMA_TX_SOURCE       /* Source to be used with TX channel */
};

const SPI_INIT_STRUCT _bsp_spi1_init = {
    &_spi_dspi_dma_devif,         /* Low level driver interface */
    &_bsp_dspi1_init,             /* Low level driver init data */
    {                             /* Default parameters: */
        10000000,                     /* Baudrate */
        SPI_CLK_POL_PHA_MODE0,        /* Mode */
        8,                            /* Frame size */
        1,                            /* Chip select */
        0,                            /* Attributes */
        0xFFFFFFFF                    /* Dummy pattern */
    }
};

#else /* BSPCFG_DSPI1_USE_DMA */

static const DSPI_INIT_STRUCT _bsp_dspi1_init = {
    1,                            /* SPI channel */
    CM_CLOCK_SOURCE_BUS           /* Relevant module clock source */
};

const SPI_INIT_STRUCT _bsp_spi1_init = {
    &_spi_dspi_devif,             /* Low level driver interface */
    &_bsp_dspi1_init,             /* Low level driver init data */
    {                             /* Default parameters: */
        10000000,                     /* Baudrate */
        SPI_CLK_POL_PHA_MODE0,        /* Mode */
        8,                            /* Frame size */
        1,                            /* Chip select */
        DSPI_ATTR_USE_ISR,            /* Attributes */
        0xFFFFFFFF                    /* Dummy pattern */
    }
};

#endif /* BSPCFG_DSPI1_USE_DMA */


#if BSPCFG_DSPI2_USE_DMA

static const DSPI_DMA_INIT_STRUCT _bsp_dspi2_init = {
    2,                            /* SPI channel */
    CM_CLOCK_SOURCE_BUS,          /* Relevant module clock source */
    BSP_DSPI2_DMA_RX_CHANNEL,     /* DMA channel for RX */
    BSP_DSPI2_DMA_TX_CHANNEL,     /* DMA channel for TX */
    BSP_DSPI2_DMA_RX_SOURCE,      /* Source to be used with RX channel */
    BSP_DSPI2_DMA_TX_SOURCE       /* Source to be used with TX channel */
};

const SPI_INIT_STRUCT _bsp_spi2_init = {
    &_spi_dspi_dma_devif,         /* Low level driver interface */
    &_bsp_dspi2_init,             /* Low level driver init data */
    {                             /* Default parameters: */
        10000000,                     /* Baudrate */
        SPI_CLK_POL_PHA_MODE0,        /* Mode */
        8,                            /* Frame size */
        1,                            /* Chip select */
        0,                            /* Attributes */
        0xFFFFFFFF                    /* Dummy pattern */
    }
};

#else /* BSPCFG_DSPI2_USE_DMA */

static const DSPI_INIT_STRUCT _bsp_dspi2_init = {
    2,                            /* SPI channel */
    CM_CLOCK_SOURCE_BUS           /* Relevant module clock source */
};

const SPI_INIT_STRUCT _bsp_spi2_init = {
    &_spi_dspi_devif,             /* Low level driver interface */
    &_bsp_dspi2_init,             /* Low level driver init data */
    {                             /* Default parameters: */
        10000000,                     /* Baudrate */
        SPI_CLK_POL_PHA_MODE0,        /* Mode */
        8,                            /* Frame size */
        1,                            /* Chip select */
        DSPI_ATTR_USE_ISR,            /* Attributes */
        0xFFFFFFFF                    /* Dummy pattern */
    }
};

#endif /* BSPCFG_DSPI2_USE_DMA */
