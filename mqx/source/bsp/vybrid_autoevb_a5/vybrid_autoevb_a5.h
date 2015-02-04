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
*   This include file is used to provide information needed by
*   an application program using the kernel running on the
*   Freescale TWR-VF65GS10 Evaluation board.
*
*
*END************************************************************************/

#ifndef _vybrid_autoevb_a5_
#define _vybrid_autoevb_a5_  1

/*----------------------------------------------------------------------
**                  HARDWARE INITIALIZATION DEFINITIONS
*/

/*
** Define the board type
*/
#define BSP_VYBRID_AUTOEVB_A5               1
#define BSP_NAME                            "AUTOEVB_A5"


#define __DEFAULT_PROCESSOR_NUMBER          1
#define __DEFAULT_INTERRUPT_STACK_SIZE      1024

/*----------------------------------------------------------------------
**                  CLOCK
*/

/* Init startup clock configuration is CPU_CLOCK_CONFIG_0 */
#define BSP_CLOCK_SRC                       (24000000)
#define BSP_CORE_CLOCK                      (396000000)
#define BSP_BUS_CLOCK                       (132000000)
#define BSP_IPG_CLOCK                       (66000000)
#define BSP_SYSTEM_CLOCK                    (BSP_CORE_CLOCK)

#define BSP_CORE_CLOCK_FIRC                 (24000000u)
#define BSP_BUS_CLOCK_FIRC                  (8000000u)
#define BSP_IPG_CLOCK_FIRC                  (4000000u)

/** MGCT: <category name="BSP Hardware Options"> */

/*
** The clock tick rate in miliseconds - be cautious to keep this value such
** that it divides 1000 well
**
** MGCT: <option type="number" min="1" max="1000"/>
*/
#ifndef BSP_ALARM_FREQUENCY
    #define BSP_ALARM_FREQUENCY             (200)
#endif

/*
** System timer definitions
*/
#define BSP_SYSTIMER_DEV          gtim_devif
#define BSP_SYSTIMER_ID           0
#define BSP_SYSTIMER_SRC_CLK      CM_CLOCK_SOURCE_BUS
#define BSP_SYSTIMER_ISR_PRIOR    2
/* We need to keep BSP_TIMER_INTERRUPT_VECTOR macro for tests and watchdog.
 * Will be removed after hwtimer expand to all platforms */
#define BSP_TIMER_INTERRUPT_VECTOR INT_GlobalTimer

/** MGCT: </category> */

/*
** Old clock rate definition in MS per tick, kept for compatibility
*/
#define BSP_ALARM_RESOLUTION                (1000 / BSP_ALARM_FREQUENCY)

/*
** Define the location of the hardware interrupt vector table
*/

//#define BSP_INTERRUPT_VECTOR_TABLE ...

#ifndef BSP_FIRST_INTERRUPT_VECTOR_USED
   #define BSP_FIRST_INTERRUPT_VECTOR_USED  16
#endif

#ifndef BSP_LAST_INTERRUPT_VECTOR_USED
   #define BSP_LAST_INTERRUPT_VECTOR_USED   159
#endif

#define BSP_TIMER_INTERRUPT_VECTOR           INT_GlobalTimer


/*
** EDMA interrupt level
*/
#define BSP_EDMA_INT_LEVEL                   (3)

/*
** RTC interrupt level
*/
#define BSP_RTC_INT_LEVEL                    (4)
#define BSP_TIMER                            (0)
#define BSP_TIMER_PRIORITY                   (2)
#define BSP_TIMER_CLOCK_SOURCE               (EPIT_CLK_SRC_PER)
#define BSP_TIMER_FREQUENCY                  (BSP_BUS_CLOCK)
#define BSP_TIMER_PRESCALE                   (1)

/*
** LPM related
*/
#define BSP_LPM_DEPENDENCY_LEVEL_SERIAL_POLLED (30)
#define BSP_LPM_DEPENDENCY_LEVEL_SERIAL_INT    (31)

/*----------------------------------------------------------------------
**            Core Mutex
*/

/** MGCT: <category name="Core Mutex Options"> */

/*
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_CORE_MUTEX
    #define BSPCFG_CORE_MUTEX               1
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_CORE_MUTEX_STATS
    #define BSPCFG_CORE_MUTEX_STATS         0
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef BSPCFG_CORE_MUTEX_PRIORITY
    #define BSPCFG_CORE_MUTEX_PRIORITY          3
#endif

/** MGCT: </category> */

/*-----------------------------------------------------------------------------
**                      Ethernet Info
*/
#define BSP_ENET_DEVICE_COUNT               (1)

/*
** MACNET interrupt levels and vectors
*/
#define BSP_MACNET0_INT_TX_LEVEL            (4)
#define BSP_MACNET0_INT_RX_LEVEL            (4)
#define BSP_MACNET1_INT_TX_LEVEL            (4)
#define BSP_MACNET1_INT_RX_LEVEL            (4)

#define BSP_DEFAULT_ENET_DEVICE             0
#define BSP_DEFAULT_ENET_OUI                { 0x00, 0x00, 0x5E, 0, 0, 0 }

/*
** The Ethernet PHY device number 0..31
*/
#ifndef BSP_ENET0_PHY_ADDR
    #define BSP_ENET0_PHY_ADDR              0
#endif

#ifndef BSP_ENET1_PHY_ADDR
    #define BSP_ENET1_PHY_ADDR              1
#endif

// ** PHY MII Speed (MDC - Management Data Clock)

#define BSP_ENET0_PHY_MII_SPEED             (2500000L)
#define BSP_ENET1_PHY_MII_SPEED             (2500000L)

/** MGCT: <category name="BSP Ethernet Options"> */

/*
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_HAS_SRAM_POOL
    #define BSPCFG_HAS_SRAM_POOL            1
#endif

/*
** Number of receive BD's.
** MGCT: <option type="number"/>
*/
#ifndef BSPCFG_RX_RING_LEN
    #define BSPCFG_RX_RING_LEN              3
#endif

/*
** Number of transmit BD's.
** MGCT: <option type="number"/>
*/
#ifndef BSPCFG_TX_RING_LEN
    #define BSPCFG_TX_RING_LEN              3
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_ENET_STATS
    #define BSPCFG_ENABLE_ENET_STATS        1
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENET_RESTORE
    #define BSPCFG_ENET_RESTORE             1
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_CPP
    #define BSPCFG_ENABLE_CPP               0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENET_SRAM_BUF
    #define BSPCFG_ENET_SRAM_BUF                    1
#endif

/** MGCT: </category> */

/*-----------------------------------------------------------------------------
**                      SPI
*/
#define BSP_DSPI_INT_LEVEL                  (12)
#define BSP_SPI_MEMORY_CHANNEL              (0)

#define BSP_DSPI0_DMA_RX_CHANNEL  0
#define BSP_DSPI0_DMA_TX_CHANNEL  1
#define BSP_DSPI0_DMA_RX_SOURCE  12
#define BSP_DSPI0_DMA_TX_SOURCE  13

#define BSP_DSPI1_DMA_RX_CHANNEL  2
#define BSP_DSPI1_DMA_TX_CHANNEL  3
#define BSP_DSPI1_DMA_RX_SOURCE  14
#define BSP_DSPI1_DMA_TX_SOURCE  15

#define BSP_DSPI2_DMA_RX_CHANNEL 32
#define BSP_DSPI2_DMA_TX_CHANNEL 33
#define BSP_DSPI2_DMA_RX_SOURCE  10
#define BSP_DSPI2_DMA_TX_SOURCE  11

/*-----------------------------------------------------------------------------
**                      QuadSPI
*/
#define BSP_QUADSPI_MEMORY_CHANNEL      (0)
#define BSP_QUADSPI_ENABLE_32BIT

/*-----------------------------------------------------------------------------
**                      I2C
*/
#define BSP_I2C_CLOCK                       (BSP_BUS_CLOCK) // NEED TO DIG DEEPER INTO CLOCKS. THIS IS BASED ON UBOOT AND WORKS FOR NOW
#define BSP_I2C0_ADDRESS                    (0x6E)
#define BSP_I2C1_ADDRESS                    (0x6F)
#define BSP_I2C2_ADDRESS                    (0x70)
#define BSP_I2C3_ADDRESS                    (0x71)

#define BSP_I2C0_BAUD_RATE                  (100000)
#define BSP_I2C1_BAUD_RATE                  (100000)
#define BSP_I2C2_BAUD_RATE                  (100000)
#define BSP_I2C3_BAUD_RATE                  (100000)

#define BSP_I2C0_MODE                       (I2C_MODE_MASTER)
#define BSP_I2C1_MODE                       (I2C_MODE_MASTER)
#define BSP_I2C2_MODE                       (I2C_MODE_MASTER)
#define BSP_I2C3_MODE                       (I2C_MODE_MASTER)

#define BSP_I2C0_INT_LEVEL                  (4)
#define BSP_I2C0_INT_SUBLEVEL               (4)
#define BSP_I2C1_INT_LEVEL                  (4)
#define BSP_I2C1_INT_SUBLEVEL               (4)
#define BSP_I2C2_INT_LEVEL                  (4)
#define BSP_I2C2_INT_SUBLEVEL               (4)
#define BSP_I2C3_INT_LEVEL                  (4)
#define BSP_I2C3_INT_SUBLEVEL               (4)

#define BSP_I2C0_RX_BUFFER_SIZE             (64)
#define BSP_I2C0_TX_BUFFER_SIZE             (64)
#define BSP_I2C1_RX_BUFFER_SIZE             (64)
#define BSP_I2C1_TX_BUFFER_SIZE             (64)
#define BSP_I2C2_RX_BUFFER_SIZE             (64)
#define BSP_I2C2_TX_BUFFER_SIZE             (64)
#define BSP_I2C3_RX_BUFFER_SIZE             (64)
#define BSP_I2C3_TX_BUFFER_SIZE             (64)
/*-----------------------------------------------------------------------------
**                      ADC
*/
#define ADC_MAX_MODULES                     (2)
#define BSP_ADC_POTENTIOMETER               (ADC_INPUT_ADC0_AN8)
#define BSP_ADC0_VECTOR_PRIORITY            (3)
#define BSP_ADC1_VECTOR_PRIORITY            (3)
#define BSP_PDB_VECTOR_PRIORITY             (3)
#define BSP_ADC_VREF_DEFAULT                (3300)
#define BSP_ADC_POTENTIOMETER_MAX           (3300)

#define BSP_DEFAULT_LWADC_MODULE            lwadc0_init

/*-----------------------------------------------------------------------------
**                      SSI
*/
// #define BSP_SSI1_AUDMUX_OUT_PORT             (6)
// #define BSP_SSI2_AUDMUX_OUT_PORT             (5)
// #define BSP_SSI3_AUDMUX_OUT_PORT             (7)

// #define BSP_SSI1_MODE                        (SSI_MODE_SLAVE)
// #define BSP_SSI2_MODE                        (SSI_MODE_SLAVE)
// #define BSP_SSI3_MODE                        (SSI_MODE_SLAVE)

// #define BSP_SSI1_DIR                         (SSI_DIR_TX)
// #define BSP_SSI2_DIR                         (SSI_DIR_TX)
// #define BSP_SSI3_DIR                         (SSI_DIR_TX)

// #define BSP_SSI1_FIFO                        (SSI_FIFO_FIFO0)
// #define BSP_SSI2_FIFO                        (SSI_FIFO_FIFO0)
// #define BSP_SSI3_FIFO                        (SSI_FIFO_FIFO0)

// #define BSP_SSI1_WORD_LENGTH                 (SSI_WORD_LENGTH_16BIT)
// #define BSP_SSI2_WORD_LENGTH                 (SSI_WORD_LENGTH_16BIT)
// #define BSP_SSI3_WORD_LENGTH                 (SSI_WORD_LENGTH_16BIT)

// #define BSP_SSI1_ALIGN                       (SSI_ALIGN_LSB)
// #define BSP_SSI2_ALIGN                       (SSI_ALIGN_LSB)
// #define BSP_SSI3_ALIGN                       (SSI_ALIGN_LSB)

// #define BSP_SSI1_RX_BUFFER_SIZE              (4096)
// #define BSP_SSI2_RX_BUFFER_SIZE              (4096)
// #define BSP_SSI3_RX_BUFFER_SIZE              (4096)

// #define BSP_SSI1_TX_BUFFER_SIZE              (16384)
// #define BSP_SSI2_TX_BUFFER_SIZE              (8192)
// #define BSP_SSI3_TX_BUFFER_SIZE              (16384)

/*-----------------------------------------------------------------------------
**                      GPIO
** NOTE:
** User leds require external connection to P20 header. This settings match
** connection between P20 [1, 2, 3, 4] header and P17 [1, 2, 3, 4] header.
** Button4 (SW9) is shared with SAI0_RX_SYNC. Uncomment BUTTON4 in case
** you don't want use sai0.
*/

#define BSP_LED1                            (LWGPIO_PIN_PTD16)
#define BSP_LED1_MUX_GPIO                   (LWGPIO_MUX_PTD16_GPIO)

#define BSP_LED2                            (LWGPIO_PIN_PTD17)
#define BSP_LED2_MUX_GPIO                   (LWGPIO_MUX_PTD17_GPIO)

#define BSP_LED3                            (LWGPIO_PIN_PTD24)
#define BSP_LED3_MUX_GPIO                   (LWGPIO_MUX_PTD24_GPIO)

#define BSP_LED4                            (LWGPIO_PIN_PTD25)
#define BSP_LED4_MUX_GPIO                   (LWGPIO_MUX_PTD25_GPIO)


#define BSP_BUTTONS_ACTIVE_HIGH             (1)

#define BSP_SW6                             (LWGPIO_PIN_PTD31)
#define BSP_SW6_MUX_GPIO                    (LWGPIO_MUX_PTD31_GPIO)

#define BSP_SW7                             (LWGPIO_PIN_PTD30)
#define BSP_SW7_MUX_GPIO                    (LWGPIO_MUX_PTD30_GPIO)

#define BSP_SW8                             (LWGPIO_PIN_PTD29)
#define BSP_SW8_MUX_GPIO                    (LWGPIO_MUX_PTD29_GPIO)

#define BSP_SW9                             (LWGPIO_PIN_PTB27)
#define BSP_SW9_MUX_GPIO                    (LWGPIO_MUX_PTB27_GPIO)

#define BSP_BUTTON1                         BSP_SW6
#define BSP_BUTTON1_MUX_GPIO                BSP_SW6_MUX_GPIO
#define BSP_BUTTON1_MUX_IRQ                 BSP_SW6_MUX_GPIO

#define BSP_BUTTON2                         BSP_SW7
#define BSP_BUTTON2_MUX_GPIO                BSP_SW7_MUX_GPIO
#define BSP_BUTTON2_MUX_IRQ                 BSP_SW7_MUX_GPIO

#define BSP_BUTTON3                         BSP_SW8
#define BSP_BUTTON3_MUX_GPIO                BSP_SW8_MUX_GPIO
#define BSP_BUTTON3_MUX_IRQ                 BSP_SW8_MUX_GPIO

/*
 * Button4 will be used as wakeup interrupt
 */
#define BSP_BUTTON4                         BSP_SW9
#define BSP_BUTTON4_MUX_GPIO                BSP_SW9_MUX_GPIO
#define BSP_BUTTON4_MUX_IRQ                 BSP_SW9_MUX_GPIO


/*-----------------------------------------------------------------------------
**                      HWTIMER
*/
#define BSP_HWTIMER1_DEV                    pit_devif
#define BSP_HWTIMER1_SOURCE_CLK             (CM_CLOCK_SOURCE_IPG)
#define BSP_HWTIMER1_ID                     (0)

#define BSP_HWTIMER2_DEV                    pit_devif
#define BSP_HWTIMER2_SOURCE_CLK             (CM_CLOCK_SOURCE_IPG)
#define BSP_HWTIMER2_ID                     (1)


/*-----------------------------------------------------------------------------
**                  IO DEVICE DRIVERS CONFIGURATION
*/

/** MGCT: <category name="I/O Subsystem"> */

/*
** Enable I/O subsystem features in MQX. No I/O drivers and file I/O will be
** possible without this feature.
**
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_IO_SUBSYSTEM
    #define BSPCFG_ENABLE_IO_SUBSYSTEM      1
#endif

/** MGCT: </category> */

/** MGCT: <category name="Default Driver Installation in BSP startup"> */

/** MGCT: <category name="UART0 Settings"> */

/*
** Polled TTY device (UART0)
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_TTYA
    #define BSPCFG_ENABLE_TTYA              1
#endif

/*
** Interrupt-driven TTY device (UART0)
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_ITTYA
    #define BSPCFG_ENABLE_ITTYA             0
#endif

/*
** TTYA and ITTYA baud rate
** MGCT: <option type="number" min="0" max="115200"/>
*/
#ifndef BSPCFG_SCI0_BAUD_RATE
    #define BSPCFG_SCI0_BAUD_RATE             115200
#endif

/*
** TTYA and ITTYA buffer size
** MGCT: <option type="number" min="0" max="256"/>
*/
#ifndef BSPCFG_SCI0_QUEUE_SIZE
    #define BSPCFG_SCI0_QUEUE_SIZE             64
#endif

/** MGCT: </category> */

/** MGCT: <category name="UART1 Settings"> */

/*
** Polled TTY device (UART1)
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_TTYB
    #define BSPCFG_ENABLE_TTYB              0
#endif

/*
** Interrupt-driven TTY device (UART1)
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_ITTYB
    #define BSPCFG_ENABLE_ITTYB             0
#endif

/*
** TTYB and ITTYB baud rate
** MGCT: <option type="number" min="0" max="115200"/>
*/
#ifndef BSPCFG_SCI1_BAUD_RATE
    #define BSPCFG_SCI1_BAUD_RATE             115200
#endif

/*
** TTYB and ITTYB buffer size
** MGCT: <option type="number" min="0" max="256"/>
*/
#ifndef BSPCFG_SCI1_QUEUE_SIZE
    #define BSPCFG_SCI1_QUEUE_SIZE             64
#endif

/** MGCT: </category> */

/** MGCT: <category name="UART2 Settings"> */

/*
** Polled TTY device (UART2)
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_TTYC
    #define BSPCFG_ENABLE_TTYC              0
#endif

/*
** Interrupt-driven TTY device (UART2)
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_ITTYC
    #define BSPCFG_ENABLE_ITTYC             0
#endif

/*
** TTYC and ITTYC baud rate
** MGCT: <option type="number" min="0" max="115200"/>
*/
#ifndef BSPCFG_SCI2_BAUD_RATE
    #define BSPCFG_SCI2_BAUD_RATE             115200
#endif

/*
** TTYC and ITTYC buffer size
** MGCT: <option type="number" min="0" max="256"/>
*/
#ifndef BSPCFG_SCI2_QUEUE_SIZE
    #define BSPCFG_SCI2_QUEUE_SIZE             64
#endif

/** MGCT: </category> */

/** MGCT: <category name="UART3 Settings"> */

/*
** Polled TTY device (UART3)
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_TTYD
    #define BSPCFG_ENABLE_TTYD              0
#endif

/*
** Interrupt-driven TTY device (UART3)
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_ITTYD
    #define BSPCFG_ENABLE_ITTYD             0
#endif

/*
** TTYD and ITTYD baud rate
** MGCT: <option type="number" min="0" max="115200"/>
*/
#ifndef BSPCFG_SCI3_BAUD_RATE
    #define BSPCFG_SCI3_BAUD_RATE             115200
#endif

/*
** TTYD and ITTYD buffer size
** MGCT: <option type="number" min="0" max="256"/>
*/
#ifndef BSPCFG_SCI3_QUEUE_SIZE
    #define BSPCFG_SCI3_QUEUE_SIZE             64
#endif

/** MGCT: </category> */

/** MGCT: <category name="UART4 Settings"> */

/*
** Polled TTY device (UART4)
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_TTYE
    #define BSPCFG_ENABLE_TTYE              0
#endif

/*
** Interrupt-driven TTY device (UART4)
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_ITTYE
    #define BSPCFG_ENABLE_ITTYE             0
#endif

/*
** TTYE and ITTYE baud rate
** MGCT: <option type="number" min="0" max="115200"/>
*/
#ifndef BSPCFG_SCI4_BAUD_RATE
    #define BSPCFG_SCI4_BAUD_RATE             115200
#endif

/*
** TTYE and ITTYE buffer size
** MGCT: <option type="number" min="0" max="256"/>
*/
#ifndef BSPCFG_SCI4_QUEUE_SIZE
    #define BSPCFG_SCI4_QUEUE_SIZE             64
#endif

/** MGCT: </category> */

/** MGCT: <category name="UART5 Settings"> */

/*
** Polled TTY device (UART5)
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_TTYF
    #define BSPCFG_ENABLE_TTYF              0
#endif

/*
** Interrupt-driven TTY device (UART5)
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_ITTYF
    #define BSPCFG_ENABLE_ITTYF             0
#endif

/*
** TTYF and ITTYF baud rate
** MGCT: <option type="number" min="0" max="115200"/>
*/
#ifndef BSPCFG_SCI5_BAUD_RATE
    #define BSPCFG_SCI5_BAUD_RATE             115200
#endif

/*
** TTYF and ITTYF buffer size
** MGCT: <option type="number" min="0" max="256"/>
*/
#ifndef BSPCFG_SCI5_QUEUE_SIZE
    #define BSPCFG_SCI5_QUEUE_SIZE             64
#endif

/** MGCT: </category> */

/*
** Polled I2C0 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_I2C0
    #define BSPCFG_ENABLE_I2C0              1
#endif

/*
** Interrupt-driven I2C0 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_II2C0
    #define BSPCFG_ENABLE_II2C0             0
#endif

/*
** Polled I2C1 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_I2C1
    #define BSPCFG_ENABLE_I2C1              0
#endif

/*
** Interrupt-driven I2C1 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_II2C1
    #define BSPCFG_ENABLE_II2C1             0
#endif

/*
** Polled I2C2 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_I2C2
    #define BSPCFG_ENABLE_I2C2              0
#endif

/*
** Interrupt-driven I2C2 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_II2C2
    #define BSPCFG_ENABLE_II2C2             0
#endif

/*
** Polled I2C3 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_I2C3
    #define BSPCFG_ENABLE_I2C3              0
#endif

/*
** Interrupt-driven I2C3 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_II2C3
    #define BSPCFG_ENABLE_II2C3             0
#endif

/*
** Enable SPI0 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_SPI0
    #define BSPCFG_ENABLE_SPI0              0
#endif

/*
** Use DMA transfers on SPI0
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_DSPI0_USE_DMA
    #define BSPCFG_DSPI0_USE_DMA            1
#endif

/*
** Enable SPI1 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_SPI1
    #define BSPCFG_ENABLE_SPI1              0
#endif

/*
** Use DMA transfers on SPI1
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_DSPI1_USE_DMA
    #define BSPCFG_DSPI1_USE_DMA            1
#endif

/*
** Enable SPI2 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_SPI2
    #define BSPCFG_ENABLE_SPI2              0
#endif

/*
** Use DMA transfers on SPI2
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_DSPI2_USE_DMA
    #define BSPCFG_DSPI2_USE_DMA            1
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_QUADSPI0
    #define BSPCFG_ENABLE_QUADSPI0          0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_QUADSPI1
    #define BSPCFG_ENABLE_QUADSPI1          0
#endif

/*
** GPIO device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_GPIODEV
    #define BSPCFG_ENABLE_GPIODEV           0
#endif

/*
** RTC device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_RTCDEV
    #define BSPCFG_ENABLE_RTCDEV            0
#endif

/*
** PCFLASH device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_PCFLASH
    #define BSPCFG_ENABLE_PCFLASH           0
#endif

/*
** ADC
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_ADC
    #define BSPCFG_ENABLE_ADC               0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_FLASHX
    #define BSPCFG_ENABLE_FLASHX            0
#endif

/*
** DCU4 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_DCU4
    #define BSPCFG_ENABLE_DCU4              1
#endif

/*
** FBDEV device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_FBDEV
    #define BSPCFG_ENABLE_FBDEV             1
#endif

/*
** ESDHC device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_ESDHC
    #define BSPCFG_ENABLE_ESDHC             1
#endif

/*
** IODEBUG device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_IODEBUG
    #define BSPCFG_ENABLE_IODEBUG           0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_SAI
    #define BSPCFG_ENABLE_SAI               1
#endif

/*
** USB device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_USB
    #define BSPCFG_ENABLE_USB               1
#endif

/** MGCT: </category> */

/*
** The SDCARD settings.
*/
#define BSP_SDCARD_ESDHC_CHANNEL            "esdhc1:"

/* Card detect present in schematic of rev.D of the board, in previous revision routed to Bluetooth extension connector */
//#define BSP_SDCARD_GPIO_DETECT              (LWGPIO_PIN_PTE11)
//#define BSP_SDCARD_DETECT_MUX_GPIO          (LWGPIO_MUX_PTE11_GPIO)

/*----------------------------------------------------------------------
**                  USB Configuration
*/
#define USBCFG_CONNECTOR_UTMI               (0)
#define USBCFG_USBOTG_CONNECTOR             USBCFG_CONNECTOR_UTMI

#define USBCFG_DEFAULT_HOST_CONTROLLER      (&_bsp_usb_host_ehci0_if)
#define USBCFG_DEFAULT_DEVICE_CONTROLLER    (&_bsp_usb_dev_ehci0_if)

#define BSP_USB_INT_LEVEL                   (3)

#define USBCFG_EHCI                         (1)
#define USBCFG_MAX_DRIVERS                  (4)

/* This will be removed in the future and runtime option in the initialization struct will be used */
#define USBCFG_REGISTER_ENDIANNESS          MQX_LITTLE_ENDIAN
#define USBCFG_MEMORY_ENDIANNESS            MQX_LITTLE_ENDIAN

/*----------------------------------------------------------------------
**            MULTICORE SUPPORT
*/

/* IPC for Shared Memory */
// Set to let Shared memory driver that there is core specific support required
#define BSPCFG_IO_PCB_SHM_SUPPORT           1
#define BSPCFG_IO_PCB_SHM_RX_PRIO           2
#define BSPCFG_IO_PCB_SHM_TX_PRIO           2

#define BSP_EDMA_INT_LEVEL                 (3)
#define BSP_ESAI_INT_LEVEL                 (3)


/*FTM IRQ level*/
#define BSP_FTM_INT_LEVEL                   (3)

/*----------------------------------------------------------------------
**                  DEFAULT MQX INITIALIZATION DEFINITIONS
** MGCT: <category name="Default MQX initialization parameters">
*/

#define __INTERNAL_SRAM_BASE                (0x3f000000)
#define __INTERNAL_SRAM_SIZE                (0x00080000)

#define __EXTERNAL_DDRAM_BASE               (0x80000000)
#define __EXTERNAL_DDRAM_SIZE               (0x08000000)

#define __EXTERNAL_QSPI0_FLASH_BASE         (0x20000000)
#define __EXTERNAL_QSPI0_FLASH_SIZE         (0x04000000)

#define __EXTERNAL_QSPI1_FLASH_BASE         (0x50000000)
#define __EXTERNAL_QSPI1_FLASH_SIZE         (0x02000000)

extern unsigned char __KERNEL_DATA_START[];
extern unsigned char __KERNEL_DATA_END[];

extern unsigned char __SHARED_RAM_START[];
extern unsigned char __SHARED_RAM_END[];

extern unsigned char __UNCACHED_DATA_START[];
extern unsigned char __UNCACHED_DATA_END[];

extern unsigned char __SRAM_POOL_START[];
extern unsigned char __SRAM_POOL_END[];

#if __IAR_SYSTEMS_ICC__

#pragma section = "KERNEL_DATA"
#define BSP_DEFAULT_START_OF_KERNEL_MEMORY      ((void *)__sfb("KERNEL_DATA"))

#elif defined(__CC_ARM)

#define BSP_DEFAULT_START_OF_KERNEL_MEMORY      ((void *)__KERNEL_DATA_START)

#elif defined(__GNUC__)

#define BSP_DEFAULT_START_OF_KERNEL_MEMORY      ((void *)__KERNEL_DATA_START)

#else
#error UNKNOWN COMPILER - IMPLEMENT ME!!!
#endif

#define BSP_DEFAULT_END_OF_KERNEL_MEMORY                      ((void *)__KERNEL_DATA_END)

#define VF65GS10_SRAM_BASE                  (0x3f000000)
#define VF65GS10_SRAM_CORE_0_BASE           (0x3f07f000)
#define VF65GS10_SRAM_CORE_0_SIZE           (2 * 1024)
#define VF65GS10_SRAM_CORE_0_END            (VF65GS10_SRAM_CORE_0_BASE + VF65GS10_SRAM_CORE_0_SIZE)

#define VF65GS10_SRAM_CORE_1_BASE           (0x3f07f800)
#define VF65GS10_SRAM_CORE_1_SIZE           (2 * 1024)
#define VF65GS10_SRAM_CORE_1_END            (VF65GS10_SRAM_CORE_1_BASE + VF65GS10_SRAM_CORE_1_SIZE)

#if defined(__ARMCC_VERSION)
extern unsigned char Image$$SHARED_RAM_START$$Base[];
extern unsigned char Image$$SHARED_RAM_END$$Base[];
#define BSP_SHARED_RAM_START                ((void *)Image$$SHARED_RAM_START$$Base)
#define BSP_SHARED_RAM_SIZE                 (Image$$SHARED_RAM_END$$Base - Image$$SHARED_RAM_START$$Base)
#else
#define BSP_SHARED_RAM_START                ((void *)__SHARED_RAM_START)
#define BSP_SHARED_RAM_SIZE                 (__SHARED_RAM_SIZE)
#endif
#define BSP_REMOTE_SHARED_RAM_START         (VF65GS10_SRAM_CORE_1_BASE)

/*
** MGCT: <option type="string" quoted="false" allowempty="false"/>
*/
#ifndef BSP_DEFAULT_INTERRUPT_STACK_SIZE
    #define BSP_DEFAULT_INTERRUPT_STACK_SIZE              1024
#endif

/* MGCT: <option type="list">
** <item name="1 (all levels disabled)" value="(1L)"/>
** <item name="2" value="(2L)"/>
** <item name="3" value="(3L)"/>
** <item name="4" value="(4L)"/>
** <item name="5" value="(5L)"/>
** <item name="6" value="(6L)"/>
** <item name="7" value="(7L)"/>
** </option>
*/
#ifndef BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX
    #define BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX  (2L)
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef BSP_DEFAULT_MAX_MSGPOOLS
    #define BSP_DEFAULT_MAX_MSGPOOLS                      (2L)
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef BSP_DEFAULT_MAX_MSGQS
    #define BSP_DEFAULT_MAX_MSGQS                         (16L)
#endif

/*
** Other Serial console options:(do not forget to enable BSPCFG_ENABLE_TTY define if changed)
**      "ttya:"      TWR-SER     polled mode
** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/>
 */
#ifndef BSP_DEFAULT_IO_CHANNEL
    #if BSPCFG_ENABLE_TTYA
        #define BSP_DEFAULT_IO_CHANNEL                      "ttya:"
        #define BSP_DEFAULT_IO_CHANNEL_DEFINED
    #else
        #define BSP_DEFAULT_IO_CHANNEL                      NULL
    #endif
#else
    /* undef is for backward compatibility with user_configh.h files which have already had it defined */
    #undef  BSP_DEFAULT_IO_CHANNEL_DEFINED
    #define BSP_DEFAULT_IO_CHANNEL_DEFINED
#endif

/*
** MGCT: <option type="string" maxsize="1024" quoted="false" allowempty="false"/>
*/
#ifndef BSP_DEFAULT_IO_OPEN_MODE
    #define BSP_DEFAULT_IO_OPEN_MODE                      (void *) (/*IO_SERIAL_XON_XOFF |*/ IO_SERIAL_TRANSLATION | IO_SERIAL_ECHO)
#endif

/** MGCT: </category> */

#ifndef BSP_CORE_0_PROCESSOR_NUMBER
    #define BSP_CORE_0_PROCESSOR_NUMBER                 (1)
#endif

#ifndef BSP_CORE_1_PROCESSOR_NUMBER
    #define BSP_CORE_1_PROCESSOR_NUMBER                 (2)
#endif

#define BSP_DEFAULT_PROCESSOR_NUMBER                    ((uint32_t)BSP_CORE_0_PROCESSOR_NUMBER)

#endif // _vybrid_autoevb_a5_
