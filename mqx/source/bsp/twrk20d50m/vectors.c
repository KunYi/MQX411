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
*    This file contains the exception vector table and flash configuration.
*
*
*END************************************************************************/

#include "mqx_inc.h"
#include "bsp.h" 

extern unsigned long __BOOT_STACK_ADDRESS[];

/* Configuration values for flash controller */
#define CONFIG_1        (uint32_t)0xffffffff
#define CONFIG_2        (uint32_t)0xffffffff
#define CONFIG_3        (uint32_t)0xffffffff
#define CONFIG_4        (uint32_t)0xfffffffe

#define BOOT_START __boot
    
extern void __boot(void);

#ifndef __CC_ARM
extern vector_entry __VECTOR_TABLE_ROM_START[]; /* defined in linker command file */
extern vector_entry __VECTOR_TABLE_RAM_START[]; /* defined in linker command file */
#endif

void _svc_handler(void);
void _pend_svc(void);

#ifdef __ICCARM__
    #pragma language = extended
    #pragma segment  = "CSTACK"
    
    #pragma location = ".flashx"
    #pragma segment  = ".flashx"
    /* Workaround to define __FLASHX_START_ADDR at the beginning of .flashx */
    const unsigned char __FLASHX_START_ADDR[1];
    /* Here you can put anything to your flashx area */
    
    #if BSPCFG_ENABLE_CFMPROTECT
        #pragma location = ".cfmconfig"
        #pragma segment  = ".cfmconfig"
        const uint32_t _cfm[4] = {
            CONFIG_1,
            CONFIG_2,
            CONFIG_3,
            CONFIG_4
        };
    #endif /* BSPCFG_ENABLE_CFMPROTECT */
#elif defined(__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".cfmconfig"))) const uint32_t _cfm[4] __attribute__((used)) = {
            CONFIG_1,
            CONFIG_2,
            CONFIG_3,
            CONFIG_4
    };
#else /* CodeWarrior compiler assumed */
    #pragma define_section flashx    ".flashx" ".flashx" ".flashx"  far_abs R
    /* Here you can put anything to your flashx area */
        
    #if BSPCFG_ENABLE_CFMPROTECT
        /* Pragma to place the flash configuration field on correct location defined in linker file. */
        #pragma define_section cfmconfig ".cfmconfig" ".cfmconfig" ".cfmconfig" far_abs R
        #pragma explicit_zero_data on
        __declspec(cfmconfig)  uint32_t _cfm[4] = {
            CONFIG_1,
            CONFIG_2,
            CONFIG_3,
            CONFIG_4
        };
    #endif /* BSPCFG_ENABLE_CFMPROTECT */
#endif /* CodeWarrior compiler assumed */

typedef union { vector_entry __fun; void * __ptr; } intvec_elem;

#if MQX_ROM_VECTORS
    #define DEFAULT_VECTOR  _int_kernel_isr
#else
    static void __boot_exception(void) {
        while(1);
    }
    
    #define DEFAULT_VECTOR  __boot_exception
    
    #if   defined(__ICCARM__)
        #pragma language = extended
        #pragma location = ".vectors_ram"
        #pragma segment  = ".vectors_ram"
        intvec_elem ram_vector[256] @ ".vectors_ram" =
    #elif defined(__CC_ARM) || defined(__GNUC__)
        __attribute__((section(".vectors_ram"))) vector_entry ram_vector[256] __attribute__((used)) =
    #else /* CodeWarrior compiler assumed */
        #pragma  define_section vectors_ram ".vectors_ram" far_abs RW
        /* 
         * Array for exception vectors in ram + space (6 words)
        // array for exception vectors in ram + space (6 words) for CW fun (when CW debugger handle exceptions, using(rewrite) VBR+0x408 address
         * VBR+0x408 address
         */
        __declspec(vectors_ram) vector_entry ram_vector[256 + 6] = 
    #endif /* CodeWarrior compiler assumed */
    {
        (vector_entry)__BOOT_STACK_ADDRESS,
        BOOT_START,         /* 0x01  0x00000004   -   ivINT_Initial_Program_Counter     */
        _int_kernel_isr,    /* 0x02  0x00000008   -   ivINT_NMI                         */
        _int_kernel_isr,    /* 0x03  0x0000000C   -   ivINT_Hard_Fault                  */
        _int_kernel_isr,    /* 0x04  0x00000010   -   ivINT_Mem_Manage_Fault            */
        _int_kernel_isr,    /* 0x05  0x00000014   -   ivINT_Bus_Fault                   */
        _int_kernel_isr,    /* 0x06  0x00000018   -   ivINT_Usage_Fault                 */
        0,                  /* 0x07  0x0000001C   -   ivINT_Reserved7                   */
        0,                  /* 0x08  0x00000020   -   ivINT_Reserved8                   */
        0,                  /* 0x09  0x00000024   -   ivINT_Reserved9                   */
        0,                  /* 0x0A  0x00000028   -   ivINT_Reserved10                  */
        _svc_handler,       /* 0x0B  0x0000002C   -   ivINT_SVCall                      */
        _int_kernel_isr,    /* 0x0C  0x00000030   -   ivINT_DebugMonitor                */
        0,                  /* 0x0D  0x00000034   -   ivINT_Reserved13                  */
        _pend_svc,          /* 0x0E  0x00000038   -   ivINT_PendableSrvReq              */
        _int_kernel_isr     /* 0x0F  0x0000003C   -   ivINT_SysTick                     */
    };
#endif
/*
** exception vector table - this table is really used
*/
#ifdef __ICCARM__
    #pragma language = extended
    #pragma segment  = "CSTACK"

    #pragma location = ".intvec"
    #pragma segment  = ".intvec"
    const intvec_elem __vector_table[] =
#elif defined(__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".vectors_rom"))) const vector_entry __vector_table[256] __attribute__((used)) =
#else /* CodeWarrior compiler assumed */
    #pragma  define_section vectors_rom ".vectors_rom" ".vectors_rom" ".vectors_rom" far_abs R
    __declspec(vectors_rom) vector_entry rom_vector[] = 
#endif /* CodeWarrior compiler assumed */
{
    (vector_entry)__BOOT_STACK_ADDRESS,
    BOOT_START,             /* 0x01  0x00000004   -   ivINT_Initial_Program_Counter     */
    DEFAULT_VECTOR,         /* 0x02  0x00000008   -   ivINT_NMI                         */
    DEFAULT_VECTOR,         /* 0x03  0x0000000C   -   ivINT_Hard_Fault                  */
    DEFAULT_VECTOR,         /* 0x04  0x00000010   -   ivINT_Mem_Manage_Fault            */
    DEFAULT_VECTOR,         /* 0x05  0x00000014   -   ivINT_Bus_Fault                   */
    DEFAULT_VECTOR,         /* 0x06  0x00000018   -   ivINT_Usage_Fault                 */
    0,                      /* 0x07  0x0000001C   -   ivINT_Reserved7                   */
    0,                      /* 0x08  0x00000020   -   ivINT_Reserved8                   */
    0,                      /* 0x09  0x00000024   -   ivINT_Reserved9                   */
    0,                      /* 0x0A  0x00000028   -   ivINT_Reserved10                  */
    _svc_handler,           /* 0x0B  0x0000002C   -   ivINT_SVCall                      */
    DEFAULT_VECTOR,         /* 0x0C  0x00000030   -   ivINT_DebugMonitor                */
    0,                      /* 0x0D  0x00000034   -   ivINT_Reserved13                  */
    _pend_svc,              /* 0x0E  0x00000038   -   ivINT_PendableSrvReq              */
    DEFAULT_VECTOR,         /* 0x0F  0x0000003C   -   ivINT_SysTick                     */
    /* Cortex external interrupt vectors                                                */
    DEFAULT_VECTOR,         /* 0x10  0x00000040   -   ivINT_DMA0                        */
    DEFAULT_VECTOR,         /* 0x11  0x00000044   -   ivINT_DMA1                        */
    DEFAULT_VECTOR,         /* 0x12  0x00000048   -   ivINT_DMA2                        */
    DEFAULT_VECTOR,         /* 0x13  0x0000004C   -   ivINT_DMA3                        */
    DEFAULT_VECTOR,         /* 0x14  0x00000050   -   ivINT_DMA_Error                   */
    DEFAULT_VECTOR,         /* 0x15  0x00000054   -   ivINT_DMA                         */
    DEFAULT_VECTOR,         /* 0x16  0x00000058   -   ivINT_FTFL                        */
    DEFAULT_VECTOR,         /* 0x17  0x0000005C   -   ivINT_Read_Collision              */
    DEFAULT_VECTOR,         /* 0x18  0x00000060   -   ivINT_LVD_LVW                     */
    DEFAULT_VECTOR,         /* 0x19  0x00000064   -   ivINT_LLW                         */
    DEFAULT_VECTOR,         /* 0x1A  0x00000068   -   ivINT_Watchdog                    */
    DEFAULT_VECTOR,         /* 0x1B  0x0000006C   -   ivINT_I2C0                        */
    DEFAULT_VECTOR,         /* 0x1C  0x00000070   -   ivINT_SPI0                        */
    DEFAULT_VECTOR,         /* 0x1D  0x00000074   -   ivINT_I2S0_TX                     */
    DEFAULT_VECTOR,         /* 0x1E  0x00000078   -   ivINT_I2S1_RX                     */
    DEFAULT_VECTOR,         /* 0x1F  0x0000007C   -   ivINT_UART0_LON                   */
    DEFAULT_VECTOR,         /* 0x20  0x00000080   -   ivINT_UART0_RX_TX                 */
    DEFAULT_VECTOR,         /* 0x21  0x00000084   -   ivINT_UART0_ERR                   */
    DEFAULT_VECTOR,         /* 0x22  0x00000088   -   ivINT_UART1_RX_TX                 */
    DEFAULT_VECTOR,         /* 0x23  0x0000008C   -   ivINT_UART1_ERR                   */
    DEFAULT_VECTOR,         /* 0x24  0x00000090   -   ivINT_UART2_RX_TX                 */
    DEFAULT_VECTOR,         /* 0x25  0x00000094   -   ivINT_UART2_ERR                   */
    DEFAULT_VECTOR,         /* 0x26  0x00000098   -   ivINT_ADC0                        */
    DEFAULT_VECTOR,         /* 0x27  0x0000009C   -   ivINT_CMP0                        */
    DEFAULT_VECTOR,         /* 0x28  0x000000A0   -   ivINT_CMP1                        */
    DEFAULT_VECTOR,         /* 0x29  0x000000A4   -   ivINT_FTM0                        */
    DEFAULT_VECTOR,         /* 0x2A  0x000000A8   -   ivINT_FTM1                        */
    DEFAULT_VECTOR,         /* 0x2B  0x000000AC   -   ivINT_CMT                         */
    DEFAULT_VECTOR,         /* 0x2C  0x000000B0   -   ivINT_RTC_ALARM                   */
    DEFAULT_VECTOR,         /* 0x2D  0x000000B4   -   ivINT_RTC_SECONDS                 */
    DEFAULT_VECTOR,         /* 0x2E  0x000000B8   -   ivINT_PIT0                        */
    DEFAULT_VECTOR,         /* 0x2F  0x000000BC   -   ivINT_PIT1                        */
    DEFAULT_VECTOR,         /* 0x30  0x000000C0   -   ivINT_PIT2                        */
    DEFAULT_VECTOR,         /* 0x31  0x000000C4   -   ivINT_PIT3                        */
    DEFAULT_VECTOR,         /* 0x32  0x000000C8   -   ivINT_PDB0                        */
    DEFAULT_VECTOR,         /* 0x33  0x000000CC   -   ivINT_USB0                        */
    DEFAULT_VECTOR,         /* 0x34  0x000000D0   -   ivINT_USBDCD                      */
    DEFAULT_VECTOR,         /* 0x35  0x000000D4   -   ivINT_TSI0                        */
    DEFAULT_VECTOR,         /* 0x36  0x000000D8   -   ivINT_MCG                         */
    DEFAULT_VECTOR,         /* 0x37  0x000000DC   -   ivINT_LPTimer                     */
    DEFAULT_VECTOR,         /* 0x38  0x000000E0   -   ivINT_PORTA                       */
    DEFAULT_VECTOR,         /* 0x39  0x000000E4   -   ivINT_PORTB                       */
    DEFAULT_VECTOR,         /* 0x3A  0x000000E8   -   ivINT_PORTC                       */
    DEFAULT_VECTOR,         /* 0x3B  0x000000EC   -   ivINT_PORTD                       */
    DEFAULT_VECTOR,         /* 0x3C  0x000000F0   -   ivINT_PORTE                       */
    DEFAULT_VECTOR,         /* 0x3D  0x000000F4   -   ivINT_SW                          */
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR
};

