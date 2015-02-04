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

#ifndef _vybrid_autoevb_m4_h_
#define _vybrid_autoevb_m4_h_  1

/*----------------------------------------------------------------------
**                  HARDWARE INITIALIZATION DEFINITIONS
*/

/*
** Define the board type
*/
#define BSP_VYBRID_AUTOEVB_M4               1
#define BSP_NAME                            "AUTOEVB_M4"

/*
** PROCESSOR MEMORY BOUNDS
*/
#define BSP_PERIPH_BASE                     (CORTEX_PERIPH_BASE)

#ifdef __CC_ARM

#define __DEFAULT_PROCESSOR_NUMBER 1
#define __DEFAULT_INTERRUPT_STACK_SIZE 1024

extern const vector_entry __vector_table[256] __attribute__((used));
#define __VECTOR_TABLE_ROM_START    ((_mqx_max_type)__vector_table)
#define __VECTOR_TABLE_RAM_START    0x3f800000 /* where .vectors_ram is placed. Check linker command file. */

#else /* __CC_ARM */

extern unsigned char __EXTERNAL_MRAM_ROM_BASE[],    __EXTERNAL_MRAM_ROM_SIZE[];
extern unsigned char __EXTERNAL_MRAM_RAM_BASE[],    __EXTERNAL_MRAM_RAM_SIZE[];
extern unsigned char __EXTERNAL_LCD_BASE[],         __EXTERNAL_LCD_SIZE[];
extern unsigned char __EXTERNAL_LCD_DC_BASE[];
extern const unsigned char __FLASHX_START_ADDR[];
extern const unsigned char __FLASHX_END_ADDR[];
extern const unsigned char __FLASHX_SECT_SIZE[];
extern unsigned char __INTERNAL_FLEXNVM_BASE[];
extern unsigned char __INTERNAL_FLEXNVM_SIZE[];

extern vector_entry __VECTOR_TABLE_RAM_START[]; // defined in linker command file
extern vector_entry __VECTOR_TABLE_ROM_START[]; // defined in linker command file

extern unsigned char __DEFAULT_PROCESSOR_NUMBER[];
extern unsigned char __DEFAULT_INTERRUPT_STACK_SIZE[];

#endif /* __CC_ARM */

/** MGCT: <category name="BSP Hardware Options"> */

/*
** Enable modification of flash configuration bytes during loading for flash targets.
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_CFMPROTECT
    #define BSPCFG_ENABLE_CFMPROTECT        1
#endif
#if !BSPCFG_ENABLE_CFMPROTECT && defined(__ICCARM__)
    #error Cannot disable CFMPROTECT field on IAR compiler. Please define BSPCFG_ENABLE_CFMPROTECT to 1.
#endif

/*
 * The clock configuration settings
 * Remove old definitions of "BSP_CLOCKS" in drivers and replace
 * by runtime clock checking. Its assumed that BSP_CLOCK_CONFIGURATION_1
 * sets PLL to full speed 96MHz to be compatible with old drivers.
 */
#ifndef BSP_CLOCK_CONFIGURATION_STARTUP
    #define BSP_CLOCK_CONFIGURATION_STARTUP (BSP_CLOCK_CONFIGURATION_96MHZ)
#endif /* BSP_CLOCK_CONFIGURATION_STARTUP */

/* Init startup clock configuration is CPU_CLOCK_CONFIG_0 */
#define BSP_CLOCK_SRC                   (24000000)
#define BSP_CORE_CLOCK                  (132000000)
#define BSP_BUS_CLOCK                   (132000000)
#define BSP_SYSTEM_CLOCK                (66000000)
#define BSP_IPG_CLOCK                   (66000000)

#define BSP_CORE_CLOCK_FIRC             (8000000)
#define BSP_BUS_CLOCK_FIRC              (8000000)
#define BSP_SYSTEM_CLOCK_FIRC           (4000000)
#define BSP_IPG_CLOCK_FIRC              (4000000)

/*
** The clock tick rate in miliseconds - be cautious to keep this value such
** that it divides 1000 well.
**
** MGCT: <option type="number" min="1" max="1000"/>
*/
#ifndef BSP_ALARM_FREQUENCY
    #define BSP_ALARM_FREQUENCY             (200)
#endif

/*
** System timer definitions
*/
#define BSP_SYSTIMER_DEV          systick_devif
#define BSP_SYSTIMER_ID           0
#define BSP_SYSTIMER_SRC_CLK      CM_CLOCK_SOURCE_CORE
#define BSP_SYSTIMER_ISR_PRIOR    2
/* We need to keep BSP_TIMER_INTERRUPT_VECTOR macro for tests and watchdog.
 * Will be removed after hwtimer expand to all platforms */
#define BSP_TIMER_INTERRUPT_VECTOR NVIC_SysTick

/** MGCT: </category> */

/*
** Old clock rate definition in MS per tick, kept for compatibility
*/
#define BSP_ALARM_RESOLUTION                (1000 / BSP_ALARM_FREQUENCY)

/*
** Define the location of the hardware interrupt vector table
*/
#if MQX_ROM_VECTORS
    #define BSP_INTERRUPT_VECTOR_TABLE              ((uint32_t)__VECTOR_TABLE_ROM_START)
#else
    #define BSP_INTERRUPT_VECTOR_TABLE              ((uint32_t)__VECTOR_TABLE_RAM_START)
#endif

#ifndef BSP_FIRST_INTERRUPT_VECTOR_USED
    #define BSP_FIRST_INTERRUPT_VECTOR_USED     (0)
#endif

#ifndef BSP_LAST_INTERRUPT_VECTOR_USED
    #define BSP_LAST_INTERRUPT_VECTOR_USED      (250)
#endif

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

/*
** EDMA interrupt level
*/
#define BSP_EDMA_INT_LEVEL                     (3)

/*
** RTC interrupt level
*/
#define BSP_RTC_INT_LEVEL                      (5)

/*
** LPM related
*/
#define BSP_LPM_DEPENDENCY_LEVEL_SERIAL_POLLED (30)
#define BSP_LPM_DEPENDENCY_LEVEL_SERIAL_INT    (31)

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

/*
** HWTIMER specifications
*/
#define BSP_HWTIMER1_DEV        pit_devif
#define BSP_HWTIMER1_SOURCE_CLK (CM_CLOCK_SOURCE_IPG)
#define BSP_HWTIMER1_ID         (0)

#define BSP_HWTIMER2_DEV        pit_devif
#define BSP_HWTIMER2_SOURCE_CLK (CM_CLOCK_SOURCE_IPG)
#define BSP_HWTIMER2_ID         (1)

/* Port IRQ levels */
#define BSP_PORTA_INT_LEVEL         (3)
#define BSP_PORTB_INT_LEVEL         (3)
#define BSP_PORTC_INT_LEVEL         (3)
#define BSP_PORTD_INT_LEVEL         (3)
#define BSP_PORTE_INT_LEVEL         (3)

/* LCD board settings */
/* GPIO settings for navigation buttons on LCD board */
#define BSP_LCD_NAVSW_N             (GPIO_PORT_C | GPIO_PIN18)
#define BSP_LCD_NAVSW_W             (GPIO_PORT_E | GPIO_PIN28)
#define BSP_LCD_NAVSW_E             (GPIO_PORT_E | GPIO_PIN5)
#define BSP_LCD_NAVSW_S             (GPIO_PORT_C | GPIO_PIN19)
//#define BSP_LCD_NAVSW_CTR           (GPIO_PORT_E | GPIO_PIN27) /* Conflict with LCD RESET signal */

#define BSP_LCD_DC                  (GPIO_PORT_B | GPIO_PIN17) /* open J3 13-14 */
#define BSP_LCD_DC_FN               1                          /* Functionality for LCD DC pin */

/* GPIO settings for resistive touch screen on LCD board */
#define BSP_LCD_TCHRES_X_PLUS       (GPIO_PORT_B | GPIO_PIN4)
#define BSP_LCD_TCHRES_X_MINUS      (GPIO_PORT_B | GPIO_PIN6)
#define BSP_LCD_TCHRES_Y_PLUS       (GPIO_PORT_B | GPIO_PIN7)
#define BSP_LCD_TCHRES_Y_MINUS      (GPIO_PORT_B | GPIO_PIN5)

#define BSP_LCD_SPI_CHANNEL         "spi2:"

#define BSP_LCD_X_PLUS_ADC_CHANNEL  (ADC1_SOURCE_AD10)
#define BSP_LCD_Y_PLUS_ADC_CHANNEL  (ADC1_SOURCE_AD13)

/* ADC tchres device init struct */
#define BSP_TCHRES_ADC_DEVICE "adc1:"
#define BSP_TCHRES_X_TRIGGER ADC_PDB_TRIGGER
#define BSP_TCHRES_Y_TRIGGER ADC_PDB_TRIGGER

/*-----------------------------------------------------------------------------
**                      Ethernet Info
*/
#define BSP_ENET_DEVICE_COUNT               (1)

/*
** MACNET interrupt levels and vectors
*/
#define BSP_MACNET0_INT_TX_LEVEL           (4)
#define BSP_MACNET0_INT_RX_LEVEL           (4)
#define BSP_MACNET1_INT_TX_LEVEL           (4)
#define BSP_MACNET1_INT_RX_LEVEL           (4)

#define BSP_DEFAULT_ENET_DEVICE             0
#define BSP_DEFAULT_ENET_OUI                { 0x00, 0x00, 0x5E, 0, 0, 0 }

/*
** The Ethernet PHY device number 0..31
*/
#ifndef BSP_ENET0_PHY_ADDR
    #define BSP_ENET0_PHY_ADDR                 0
#endif

#ifndef BSP_ENET1_PHY_ADDR
    #define BSP_ENET1_PHY_ADDR                 1
#endif
/*
** PHY MII Speed (MDC - Management Data Clock)
*/
#define BSP_ENET0_PHY_MII_SPEED             (2500000L)
#define BSP_ENET1_PHY_MII_SPEED             (2500000L)

/** MGCT: <category name="BSP Ethernet Options"> */

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

/** MGCT: </category> */

/*-----------------------------------------------------------------------------
**                      DSPI
*/

#define BSP_DSPI_INT_LEVEL               (4)
#define BSP_SPI_MEMORY_CHANNEL           0

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
**                      ESDHC
*/

#define BSP_SDCARD_ESDHC_CHANNEL            "esdhc1:"

/* Card detect present in schematic of rev.D of the board, in previous revision routed to Bluetooth extension connector */
//#define BSP_SDCARD_GPIO_DETECT              (LWGPIO_PIN_PTE11)
//#define BSP_SDCARD_DETECT_MUX_GPIO          (LWGPIO_MUX_PTE11_GPIO)

/*-----------------------------------------------------------------------------
**                      I2C
*/
#define BSP_I2C_CLOCK                       (BSP_BUS_CLOCK)
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

#define BSP_I2C0_INT_LEVEL                  (5)
#define BSP_I2C1_INT_LEVEL                  (5)
#define BSP_I2C2_INT_LEVEL                  (5)
#define BSP_I2C3_INT_LEVEL                  (5)

#define BSP_I2C0_RX_BUFFER_SIZE             (64)
#define BSP_I2C0_TX_BUFFER_SIZE             (64)
#define BSP_I2C1_RX_BUFFER_SIZE             (64)
#define BSP_I2C1_TX_BUFFER_SIZE             (64)
#define BSP_I2C2_RX_BUFFER_SIZE             (64)
#define BSP_I2C2_TX_BUFFER_SIZE             (64)
#define BSP_I2C3_RX_BUFFER_SIZE             (64)
#define BSP_I2C3_TX_BUFFER_SIZE             (64)



/*-----------------------------------------------------------------------------
**                      I2S
*/

#define BSP_I2S0_MODE                   (I2S_MODE_MASTER)
#define BSP_I2S0_DATA_BITS              (16)
#define BSP_I2S0_CLOCK_SOURCE           (I2S_CLK_INT)
#define BSP_I2S0_INT_LEVEL              (5)
#define BSP_I2S0_BUFFER_SIZE            (128)

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
** ESDHC device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_ESDHC
    #define BSPCFG_ENABLE_ESDHC             1
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_II2S0
    #define BSPCFG_ENABLE_II2S0                 0
#endif

/*
** IODEBUG device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_IODEBUG
    #define BSPCFG_ENABLE_IODEBUG           0
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
// no hw switch to detect card


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

/*----------------------------------------------------------------------
**                  DEFAULT MQX INITIALIZATION DEFINITIONS
** MGCT: <category name="Default MQX initialization parameters">
*/

/* Defined in link.xxx */
extern unsigned char __KERNEL_DATA_START[];
extern unsigned char __KERNEL_DATA_END[];

extern unsigned char __KERNEL_AREA_START[];
extern unsigned char __KERNEL_AREA_END[];

extern unsigned char __USER_HEAP_START[];
extern unsigned char __USER_HEAP_END[];

extern unsigned char __SHARED_RAM_START[];
extern unsigned char __SHARED_RAM_END[];

extern unsigned char __UNCACHED_DATA_START[];
extern unsigned char __UNCACHED_DATA_END[];

extern unsigned char __SRAM_POOL_START[];
extern unsigned char __SRAM_POOL_END[];

/* Tightly coupled memory - lower */
#define BSP_CORE_M4_TCML_START                                ((void *) 0x1F800000)
#define BSP_CORE_M4_TCML_SIZE                                 (0x8000)

#if MQX_ENABLE_USER_MODE

#if __IAR_SYSTEMS_ICC__
#pragma section = "USER_RW_MEMORY"
#pragma section = "USER_RO_MEMORY"
#pragma section = "USER_NO_MEMORY"
#pragma section = "USER_HEAP"
#pragma section = "USER_DEFAULT_MEMORY"

#define BSP_DEFAULT_START_OF_USER_DEFAULT_MEMORY            ((void *)__sfb("USER_DEFAULT_MEMORY"))
#define BSP_DEFAULT_END_OF_USER_DEFAULT_MEMORY              ((void *)__sfe("USER_DEFAULT_MEMORY"))

//#define BSP_DEFAULT_START_OF_USER_HEAP                      ((void *)__sfb("USER_HEAP"))
//#define BSP_DEFAULT_END_OF_USER_HEAP                        ((void *)__sfe("USER_HEAP"))

#define BSP_DEFAULT_START_OF_USER_RW_MEMORY                 ((void *)__sfb("USER_RW_MEMORY"))
#define BSP_DEFAULT_END_OF_USER_RW_MEMORY                   ((void *)__sfe("USER_RW_MEMORY"))

#define BSP_DEFAULT_START_OF_USER_RO_MEMORY                 ((void *)__sfb("USER_RO_MEMORY"))
#define BSP_DEFAULT_END_OF_USER_RO_MEMORY                   ((void *)__sfe("USER_RO_MEMORY"))

#define BSP_DEFAULT_START_OF_USER_NO_MEMORY                 ((void *)__sfb("USER_NO_MEMORY"))
#define BSP_DEFAULT_END_OF_USER_NO_MEMORY                   ((void *)__sfe("USER_NO_MEMORY"))

#elif defined(__CC_ARM)

extern unsigned int Image$$RWUSER$$Base;
extern unsigned int Image$$RWUSER$$Limit;
extern unsigned int Image$$ROUSER$$Base;
extern unsigned int Image$$ROUSER$$Limit;
extern unsigned int Image$$NOUSER$$Base;
extern unsigned int Image$$NOUSER$$Limit;
extern unsigned int Image$$DATA$$Base;
extern unsigned int Image$$DATA$$Limit;

#define BSP_DEFAULT_START_OF_USER_RW_MEMORY                 ((void *)&Image$$RWUSER$$Base)
#define BSP_DEFAULT_END_OF_USER_RW_MEMORY                   ((void *)&Image$$RWUSER$$Limit)
#define BSP_DEFAULT_START_OF_USER_RO_MEMORY                 ((void *)&Image$$ROUSER$$Base)
#define BSP_DEFAULT_END_OF_USER_RO_MEMORY                   ((void *)&Image$$ROUSER$$Limit)
#define BSP_DEFAULT_START_OF_USER_NO_MEMORY                 ((void *)&Image$$NOUSER$$Base)
#define BSP_DEFAULT_END_OF_USER_NO_MEMORY                   ((void *)&Image$$NOUSER$$Limit)

#define BSP_DEFAULT_START_OF_USER_DEFAULT_MEMORY            ((void *)&Image$$DATA$$Base)
#define BSP_DEFAULT_END_OF_USER_DEFAULT_MEMORY              ((void *)&Image$$DATA$$Limit)

#else // CW
extern unsigned char __USER_DEFAULT_MEMORY_START[];
extern unsigned char __USER_DEFAULT_MEMORY_END[];
extern unsigned char __USER_RW_MEMORY_START[];
extern unsigned char __USER_RW_MEMORY_END[];
extern unsigned char __USER_RO_MEMORY_START[];
extern unsigned char __USER_RO_MEMORY_END[];
extern unsigned char __USER_NO_MEMORY_START[];
extern unsigned char __USER_NO_MEMORY_END[];

#define BSP_DEFAULT_START_OF_USER_DEFAULT_MEMORY            ((void *)__USER_DEFAULT_MEMORY_START)
#define BSP_DEFAULT_END_OF_USER_DEFAULT_MEMORY              ((void *)__USER_DEFAULT_MEMORY_END)

//#define BSP_DEFAULT_START_OF_USER_HEAP                      ((void *)__USER_HEAP_START)
//#define BSP_DEFAULT_END_OF_USER_HEAP                        ((void *)__USER_AREA_END)

#define BSP_DEFAULT_START_OF_USER_RW_MEMORY                 ((void *)__USER_RW_MEMORY_START)
#define BSP_DEFAULT_END_OF_USER_RW_MEMORY                   ((void *)__USER_RW_MEMORY_END)

#define BSP_DEFAULT_START_OF_USER_RO_MEMORY                 ((void *)__USER_RO_MEMORY_START)
#define BSP_DEFAULT_END_OF_USER_RO_MEMORY                   ((void *)__USER_RO_MEMORY_END)

#define BSP_DEFAULT_START_OF_USER_NO_MEMORY                 ((void *)__USER_NO_MEMORY_START)
#define BSP_DEFAULT_END_OF_USER_NO_MEMORY                   ((void *)__USER_NO_MEMORY_END)

#endif

#define BSP_DEFAULT_START_OF_USER_HEAP                      ((void *)(16 * 1024))
#define BSP_DEFAULT_END_OF_USER_HEAP                        ((void *)0)

#define BSP_DEFAULT_START_OF_KERNEL_AREA                    ((void *)__KERNEL_DATA_START)  //((void *)__KERNEL_AREA_START)
#define BSP_DEFAULT_END_OF_KERNEL_AREA                      ((void *)__KERNEL_DATA_END)    //((void *)__KERNEL_AREA_END)

#define BSP_DEFAULT_MAX_USER_TASK_PRIORITY                  (0)
#define BSP_DEFAULT_MAX_USER_TASK_COUNT                     (0)

#endif // MQX_ENABLE_USER_MODE

#define BSP_DEFAULT_START_OF_KERNEL_MEMORY                    ((void *)__KERNEL_DATA_START)
#define BSP_DEFAULT_END_OF_KERNEL_MEMORY                      ((void *)__KERNEL_DATA_END)

#ifndef BSP_CORE_0_PROCESSOR_NUMBER
    #define BSP_CORE_0_PROCESSOR_NUMBER                 (1)
#endif

#ifndef BSP_CORE_1_PROCESSOR_NUMBER
    #define BSP_CORE_1_PROCESSOR_NUMBER                 (2)
#endif

#define BSP_DEFAULT_PROCESSOR_NUMBER                    ((uint32_t)BSP_CORE_1_PROCESSOR_NUMBER)

#ifndef MQX_AUX_CORE
#define MQX_AUX_CORE                            1
#endif

#define VF65GS10_SRAM_BASE                     (0x3f000000)
#define VF65GS10_SRAM_CORE_0_BASE              (0x3f07f000)
#define VF65GS10_SRAM_CORE_0_SIZE              (2 * 1024)
#define VF65GS10_SRAM_CORE_0_END               (VF65GS10_SRAM_CORE_0_BASE + VF65GS10_SRAM_CORE_0_SIZE)

#define VF65GS10_SRAM_CORE_1_BASE              (0x3f07f800)
#define VF65GS10_SRAM_CORE_1_SIZE              (2 * 1024)
#define VF65GS10_SRAM_CORE_1_END               (VF65GS10_SRAM_CORE_1_BASE + VF65GS10_SRAM_CORE_1_SIZE)

#if defined(__ARMCC_VERSION)
extern unsigned char Image$$SHARED_RAM_START$$Base[];
extern unsigned char Image$$SHARED_RAM_END$$Base[];
#define BSP_SHARED_RAM_START                ((void *)Image$$SHARED_RAM_START$$Base)
#define BSP_SHARED_RAM_SIZE                 (Image$$SHARED_RAM_END$$Base - Image$$SHARED_RAM_START$$Base)
#else
#define BSP_SHARED_RAM_START                ((void *)__SHARED_RAM_START)
#define BSP_SHARED_RAM_SIZE                 (__SHARED_RAM_SIZE)
#endif
#define BSP_REMOTE_SHARED_RAM_START         (VF65GS10_SRAM_CORE_0_BASE)

/*
** MGCT: <option type="string" quoted="false" allowempty="false"/>
*/
#ifndef BSP_DEFAULT_INTERRUPT_STACK_SIZE
    #define BSP_DEFAULT_INTERRUPT_STACK_SIZE              (3000) //TODO: use ((uint32_t)__DEFAULT_INTERRUPT_STACK_SIZE)
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
    #define BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX      (2L)
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef BSP_DEFAULT_MAX_MSGPOOLS
    #define BSP_DEFAULT_MAX_MSGPOOLS                          (2L)
#endif

/*
** MGCT: <option type="number"/>
*/
#ifndef BSP_DEFAULT_MAX_MSGQS
    #define BSP_DEFAULT_MAX_MSGQS                             (16L)
#endif

/*
** Other Serial console options:(do not forget to enable BSPCFG_ENABLE_TTY define if changed)
**      "itty:"       interrupt mode
**      "ttya:"       uart0, polled mode
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
    #define BSP_DEFAULT_IO_OPEN_MODE                      (void *) (IO_SERIAL_XON_XOFF | IO_SERIAL_TRANSLATION | IO_SERIAL_ECHO)
#endif

/*
** FLASHX flash memory pool minimum size
** MGCT: <option type="string" maxsize="1024" quoted="false" allowempty="false"/>
*/
#if BSPCFG_ENABLE_FLASHX
    #ifndef BSPCFG_FLASHX_SIZE
        #define BSPCFG_FLASHX_SIZE      (FLASHX_END_ADDR - FLASHX_START_ADDR) /* defines maximum possible space */
    #endif
#else
    #undef  BSPCFG_FLASHX_SIZE
    #define BSPCFG_FLASHX_SIZE      0x0
#endif

/** MGCT: </category> */

#endif /* _vybrid_autoevb_m4_h_  */
