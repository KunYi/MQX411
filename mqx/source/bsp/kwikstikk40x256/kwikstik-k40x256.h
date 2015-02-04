/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
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
*   Freescale Kwikstik Evaluation board. 
*
*
*END************************************************************************/

#ifndef _kwikstik_k40x256_h_
    #define _kwikstik_k40x256_h_  1


/*----------------------------------------------------------------------
**                  HARDWARE INITIALIZATION DEFINITIONS
*/

/*
** Define the board type
*/
#define BSP_KWIKSTIK_K40X256 1
#define BSP_NAME                            "KWIKSTIKK40X256"

/*
** PROCESSOR MEMORY BOUNDS
*/
#define BSP_PERIPH_BASE                     (CORTEX_PERIPH_BASE)

typedef void (*vector_entry)(void);

#define BSP_INTERNAL_FLASH_BASE    0x00000000
#define BSP_INTERNAL_FLASH_SIZE    0x00040000
#define BSP_INTERNAL_FLASH_SECTOR_SIZE  0x800

#ifdef __CC_ARM
#define __INTERNAL_FLASH_BASE    BSP_INTERNAL_FLASH_BASE
#define __INTERNAL_FLASH_SIZE    BSP_INTERNAL_FLASH_SIZE
#define __INTERNAL_FLEXNVM_BASE  0x10000000
#define __INTERNAL_FLEXNVM_SIZE  0x00040000
#define __INTERNAL_FLEXRAM_BASE  0x14000000
#define __INTERNAL_FLEXRAM_SIZE  0x00004000

extern unsigned char Image$$USEDFLASH_END$$Base[];
#define __FLASHX_START_ADDR     ((void *)Image$$USEDFLASH_END$$Base)
#define __FLASHX_END_ADDR       ((BSP_INTERNAL_FLASH_BASE) + (BSP_INTERNAL_FLASH_SIZE))
#define __FLASHX_SECT_SIZE      BSP_INTERNAL_FLASH_SECTOR_SIZE

#define __DEFAULT_PROCESSOR_NUMBER 1
#define __DEFAULT_INTERRUPT_STACK_SIZE 1024

extern unsigned char Image$$VECTORS$$Base[];
#define __VECTOR_TABLE_ROM_START ((void *)Image$$VECTORS$$Base)

extern unsigned char Image$$RAM_VECTORS$$Base[];
#define __VECTOR_TABLE_RAM_START ((void *)Image$$RAM_VECTORS$$Base)

#else 
/* IAR/CW compiler */
extern const unsigned char __FLASHX_START_ADDR[];
extern const unsigned char __FLASHX_END_ADDR[];
extern const unsigned char __FLASHX_SECT_SIZE[]; 
extern unsigned char __INTERNAL_FLEXNVM_BASE[];
extern unsigned char __INTERNAL_FLEXNVM_SIZE[];
extern unsigned char __INTERNAL_FLEXRAM_BASE[];
extern unsigned char __INTERNAL_FLEXRAM_SIZE[];
extern vector_entry __VECTOR_TABLE_RAM_START[]; // defined in linker command file
extern vector_entry __VECTOR_TABLE_ROM_START[]; // defined in linker command file

extern unsigned char __DEFAULT_PROCESSOR_NUMBER[];
extern unsigned char __DEFAULT_INTERRUPT_STACK_SIZE[];

#endif
#define BSP_EXTERNAL_LCD_BASE       ((void *)__EXTERNAL_LCD_BASE)
#define BSP_EXTERNAL_LCD_SIZE       ((uint32_t)__EXTERNAL_LCD_SIZE)
#define BSP_EXTERNAL_LCD_DC         ((void *)__EXTERNAL_LCD_DC_BASE)

#define BSP_INTERNAL_FLEXRAM_BASE           0x14000000
#define BSP_INTERNAL_FLEXRAM_SIZE           0x00004000

#define BSP_INTERNAL_PA_RAM_BASE            BSP_INTERNAL_FLEXRAM_BASE
#define BSP_INTERNAL_PA_RAM_SIZE            BSP_INTERNAL_FLEXRAM_SIZE
#define FTFL_PROGRAM_SECTION_ALIGNMENT      FTFL_PHRASE_SIZE

/*
** Flash details
*/
/*#define BSP_FLASH_BASE                BSP_BURST_FLASH_BASE
#define BSP_FLASH_SIZE                  BSP_BURST_FLASH_SIZE
#define BSP_FLASH_WIDTH                 (16)
#define BSP_FLASH_DEVICES               (1)
*/

/* Enable modification of flash configuration bytes during loading for flash targets */
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
#define BSP_CLOCK_SRC                   (CPU_XTAL_CLK_HZ)
#define BSP_CORE_CLOCK                  (CPU_CORE_CLK_HZ_CONFIG_0)
#define BSP_SYSTEM_CLOCK                (CPU_CORE_CLK_HZ_CONFIG_0)
#define BSP_CLOCK                       (CPU_BUS_CLK_HZ_CONFIG_0)
#define BSP_BUS_CLOCK                   (CPU_BUS_CLK_HZ_CONFIG_0)
#define BSP_FLEXBUS_CLOCK               (CPU_FLEXBUS_CLK_HZ_CONFIG_0)
#define BSP_FLASH_CLOCK                 (CPU_FLASH_CLK_HZ_CONFIG_0)

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
#define BSP_SYSTIMER_DEV          systick_devif
#define BSP_SYSTIMER_ID           0
#define BSP_SYSTIMER_SRC_CLK      CM_CLOCK_SOURCE_CORE
#define BSP_SYSTIMER_ISR_PRIOR    2
/* We need to keep BSP_TIMER_INTERRUPT_VECTOR macro for tests and watchdog. 
 * Will be removed after hwtimer expand to all platforms */
#define BSP_TIMER_INTERRUPT_VECTOR INT_SysTick

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
** EDMA interrupt level
*/
#define BSP_EDMA_INT_LEVEL                     (3)

/*
** RTC interrupt level
*/
#define BSP_RTC_INT_LEVEL                      (4)

/*
** LPM related
*/
#define BSP_LLWU_INTERRUPT_VECTOR              INT_LLW
#define BSP_LLWU_INTERRUPT_PRIORITY            3

#define BSP_LPM_DEPENDENCY_LEVEL_SERIAL_POLLED (30)
#define BSP_LPM_DEPENDENCY_LEVEL_SERIAL_INT    (31)
    
/*
** GPIO board specifications
*/
#define BSP_LED1        (GPIO_PORT_A | GPIO_PIN12)
#define BSP_ACCEL_IRQ   (GPIO_PORT_C | GPIO_PIN12) /* onboard accelerometer IRQ pin */
#define BSP_LED1_MUX_GPIO (LWGPIO_MUX_C7_GPIO)
/* definitions for user applications */

/*
** SAI/audio specifications
*/
#define BSP_HAS_AUDIO_CODEC                   (1)

/* HWTIMER definitions for user applications */
#define BSP_HWTIMER1_DEV        pit_devif
#define BSP_HWTIMER1_SOURCE_CLK (CM_CLOCK_SOURCE_BUS)
#define BSP_HWTIMER1_ID         (0)

#define BSP_HWTIMER2_DEV        lpt_devif
#define BSP_HWTIMER2_SOURCE_CLK (CM_CLOCK_SOURCE_LPO)
#define BSP_HWTIMER2_ID         (0)

/* HMI Touch TWRPI daughter cards */
#define BSP_TWRPI_VOID   0

#define BSP_TSI_INT_LEVEL   (4)

/* Port IRQ levels */
#define BSP_PORTA_INT_LEVEL                    (3)
#define BSP_PORTB_INT_LEVEL                    (3)
#define BSP_PORTC_INT_LEVEL                    (3)
#define BSP_PORTD_INT_LEVEL                    (3)
#define BSP_PORTE_INT_LEVEL                    (3)

/*-----------------------------------------------------------------------------
**                      DSPI
*/

#define BSP_DSPI_INT_LEVEL              (4)
#define BSP_SPI_MEMORY_CHANNEL          (0)

#define BSP_DSPI0_DMA_RX_CHANNEL        (0)
#define BSP_DSPI0_DMA_TX_CHANNEL        (1)
#define BSP_DSPI0_DMA_RX_SOURCE         (16)
#define BSP_DSPI0_DMA_TX_SOURCE         (17)
                                        
#define BSP_DSPI1_DMA_RX_CHANNEL        (2)
#define BSP_DSPI1_DMA_TX_CHANNEL        (3)
#define BSP_DSPI1_DMA_RX_SOURCE         (18)
#define BSP_DSPI1_DMA_TX_SOURCE         (19)
                                        
#define BSP_DSPI2_DMA_RX_CHANNEL        (4)
#define BSP_DSPI2_DMA_TX_CHANNEL        (5)
#define BSP_DSPI2_DMA_RX_SOURCE         (20)
#define BSP_DSPI2_DMA_TX_SOURCE         (21)

/*-----------------------------------------------------------------------------
**                      ESDHC
*/

#define BSP_SDCARD_ESDHC_CHANNEL         "esdhc:"
#define BSP_SDCARD_GPIO_DETECT           (GPIO_PORT_E | GPIO_PIN27)
#define BSP_SDCARD_ON                    (GPIO_PORT_E | GPIO_PIN6)
#define BSP_SDCARD_DETECT_MUX_GPIO       (LWGPIO_MUX_E27_GPIO)

/*-----------------------------------------------------------------------------
**                      I2C
*/
#define BSP_I2C_CLOCK                       (BSP_BUS_CLOCK)
#define BSP_I2C0_ADDRESS                    (0x6E)
#define BSP_I2C1_ADDRESS                    (0x6F)
#define BSP_I2C0_BAUD_RATE                  (100000)
#define BSP_I2C1_BAUD_RATE                  (100000)
#define BSP_I2C0_MODE                       (I2C_MODE_MASTER)
#define BSP_I2C1_MODE                       (I2C_MODE_MASTER)
#define BSP_I2C0_INT_LEVEL                  (5)
#define BSP_I2C1_INT_LEVEL                  (5)
#define BSP_I2C0_RX_BUFFER_SIZE             (64)
#define BSP_I2C0_TX_BUFFER_SIZE             (64)
#define BSP_I2C1_RX_BUFFER_SIZE             (64)
#define BSP_I2C1_TX_BUFFER_SIZE             (64)
#define BSP_I2C0_PORT_BASE                  (PORTB_BASE_PTR)
#define BSP_I2C0_PORT_VECTOR                (INT_PORTB)
#define BSP_I2C0_SDA_PIN_NUM                (3)
#define BSP_I2C1_PORT_BASE                  (PORTE_BASE_PTR)
#define BSP_I2C1_PORT_VECTOR                (INT_PORTE)
#define BSP_I2C1_SDA_PIN_NUM                (0)


/*-----------------------------------------------------------------------------
**                      I2S
*/

#define BSP_I2S0_MODE                   (I2S_MODE_MASTER)
#define BSP_I2S0_DATA_BITS              (16)
#define BSP_I2S0_CLOCK_SOURCE           (I2S_CLK_INT)
#define BSP_I2S0_INT_LEVEL              (5)
#define BSP_I2S0_BUFFER_SIZE            (512)

/*-----------------------------------------------------------------------------
**                      FlexNVM
*/
#define BSP_EE_DATA_SIZE_CODE               (FLEXNVM_EE_SPLIT_1_1 | FLEXNVM_EE_SIZE_4096)
#define BSP_FLEXNVM_PART_CODE               (FLEXNVM_PART_CODE_DATA128_EE128)

/*-----------------------------------------------------------------------------
**                      ADC
*/
#define ADC_MAX_MODULES                 (2)
#define BSP_ADC_POTENTIOMETER           (ADC0_SOURCE_AD0)
#define BSP_ADC_CH_POT                  (ADC0_SOURCE_AD0)

#define BSP_ADC0_VECTOR_PRIORITY        (3)
#define BSP_ADC1_VECTOR_PRIORITY        (3)

#define BSP_PDB_VECTOR_PRIORITY         (3)
#define BSP_ADC_VREF_DEFAULT            (3300)
#define BSP_ADC_POTENTIOMETER_MAX       (3300)

/*-----------------------------------------------------------------------------
**                      LWADC
*/
#define BSP_DEFAULT_LWADC_MODULE        lwadc0_init

/*-----------------------------------------------------------------------------
**                      USB
*/
#define USBCFG_MAX_DRIVERS               (2)

#define BSP_USB_INT_LEVEL                (4)
#define BSP_USB_TWR_SER2                 (0) /*set to 1 if TWR-SER2 (2 eth) board used (only host)*/

/* USB host controler for K40x processors */
#define USBCFG_KHCI                       1

#define USBCFG_DEFAULT_HOST_CONTROLLER    (&_bsp_usb_host_khci0_if)
#define USBCFG_DEFAULT_DEVICE_CONTROLLER  (&_bsp_usb_dev_khci0_if)

/*----------------------------------------------------------------------
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
    #define BSPCFG_ENABLE_TTYA              0
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
    #define BSPCFG_ENABLE_I2C0                  1
#endif

/*
** Polled I2C1 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_I2C1
    #define BSPCFG_ENABLE_I2C1                  0
#endif

/*
** Interrupt-driven I2C0 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_II2C0
    #define BSPCFG_ENABLE_II2C0                 0
#endif

/*
** Interrupt-driven I2C1 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_II2C1
    #define BSPCFG_ENABLE_II2C1                 0
#endif

/*
** Enable legacy II2C slave
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_LEGACY_II2C_SLAVE
    #define BSPCFG_ENABLE_LEGACY_II2C_SLAVE     1
#endif

/* GPIO Device - legacy, use LWGPIO instead */
#ifndef BSPCFG_ENABLE_GPIODEV
    #define BSPCFG_ENABLE_GPIODEV               0
#endif

/*
** RTC device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_RTCDEV
    #define BSPCFG_ENABLE_RTCDEV                0
#endif

/*
** PCFLASH device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_PCFLASH
    #define BSPCFG_ENABLE_PCFLASH               0
#endif

/*
** Enable SPI0 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_SPI0
    #define BSPCFG_ENABLE_SPI0                  0
#endif

/*
** Use DMA transfers on SPI0
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_DSPI0_USE_DMA
    #define BSPCFG_DSPI0_USE_DMA                1
#endif

/*
** Enable SPI1 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_SPI1
    #define BSPCFG_ENABLE_SPI1                  0
#endif

/*
** Use DMA transfers on SPI1
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_DSPI1_USE_DMA
    #define BSPCFG_DSPI1_USE_DMA                1
#endif

/*
** Enable SPI2 device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_SPI2
    #define BSPCFG_ENABLE_SPI2                  0
#endif

/*
** Use DMA transfers on SPI2
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_DSPI2_USE_DMA
    #define BSPCFG_DSPI2_USE_DMA                1
#endif

/*
** ADC0
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_ADC0
    #define BSPCFG_ENABLE_ADC0                  0
#endif

/*
** ADC1
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_ADC1
    #define BSPCFG_ENABLE_ADC1                  0
#endif

/*
** FTFL
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_FLASHX
    #define BSPCFG_ENABLE_FLASHX                0
#endif

/*
** ESDHC device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_ESDHC
    #define BSPCFG_ENABLE_ESDHC                 0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_USBDCD
    #define BSPCFG_ENABLE_USBDCD                0
#endif

/*
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_IUSBDCD
    #define BSPCFG_ENABLE_IUSBDCD               0
#endif

/*
** IODEBUG device
** MGCT: <option type="bool"/>
*/
#ifndef BSPCFG_ENABLE_IODEBUG
    #define BSPCFG_ENABLE_IODEBUG               1
#endif

/** MGCT: </category> */ 

/*----------------------------------------------------------------------
**                      Ethernet Info
*/
#define BSP_ENET_DEVICE_COUNT                   0

/*----------------------------------------------------------------------
**                  DEFAULT MQX INITIALIZATION DEFINITIONS
** MGCT: <category name="Default MQX initialization parameters"> 
*/

/* Defined in link.xxx */
extern unsigned char __KERNEL_DATA_START[];
extern unsigned char __KERNEL_DATA_END[];

#define BSP_DEFAULT_START_OF_KERNEL_MEMORY                    ((void *)__KERNEL_DATA_START)
#define BSP_DEFAULT_END_OF_KERNEL_MEMORY                      ((void *)__KERNEL_DATA_END)
#define BSP_DEFAULT_PROCESSOR_NUMBER                          ((uint32_t)__DEFAULT_PROCESSOR_NUMBER)

/* MGCT: <option type="string" quoted="false" allowempty="false"/> */
#ifndef BSP_DEFAULT_INTERRUPT_STACK_SIZE
    #define BSP_DEFAULT_INTERRUPT_STACK_SIZE                  ((uint32_t)__DEFAULT_INTERRUPT_STACK_SIZE)
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
 * Other Serial console options:(do not forget to enable BSPCFG_ENABLE_TTY define if changed)
 *      "ittya:"     OSJTAG-COM  interrupt mode
 *      "ttyf:"      TWR-SER     polled mode
 *      "ittyf:"     TWR-SER     interrupt mode
 *      "iodebug:"   IDE debug console
 ** MGCT: <option type="string" maxsize="256" quoted="false" allowempty="false"/> 
 */
#ifndef BSP_DEFAULT_IO_CHANNEL
    #if BSPCFG_ENABLE_IODEBUG
        #define BSP_DEFAULT_IO_CHANNEL                      "iodebug:"  /* IDE debug console */
        #define BSP_DEFAULT_IO_CHANNEL_DEFINED
    #else
        #define BSP_DEFAULT_IO_CHANNEL                      NULL
    #endif
#else
    /* undef is for backward compatibility with user_configh.h files which may have already had it defined */
    #undef  BSP_DEFAULT_IO_CHANNEL_DEFINED
    #define BSP_DEFAULT_IO_CHANNEL_DEFINED
#endif

/*
** MGCT: <option type="string" maxsize="1024" quoted="false" allowempty="false"/>  
*/
#ifndef BSP_DEFAULT_IO_OPEN_MODE
    #define BSP_DEFAULT_IO_OPEN_MODE                      (void *) (IO_SERIAL_XON_XOFF | IO_SERIAL_TRANSLATION | IO_SERIAL_ECHO)
#endif

/* USB host controller for K40x processors */
#define USBCFG_KHCI             1
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

/*
** FLASHX flash use Programming Acceleration RAM
** MGCT: <option type="bool"/>
*/
#if BSPCFG_ENABLE_FLASHX
    #ifndef BSPCFG_FLASHX_USE_PA_RAM
        #define BSPCFG_FLASHX_USE_PA_RAM 0
    #endif
#else
    #undef  BSPCFG_FLASHX_USE_PA_RAM
    #define BSPCFG_FLASHX_USE_PA_RAM 0
#endif

/** MGCT: </category> */
#endif /* _kwikstik-k40x256_h_  */
