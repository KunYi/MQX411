ENTRY(__boot)


/* 
_cfm - to keep vectors.c variables
__init_hardware - must be used from bsp.a, not from librt.a
*/
EXTERN(_cfm __init_hardware)


MEMORY
{
    vectorrom   (RX): ORIGIN = 0x00000000, LENGTH = 0x00000400
    cfmprotrom  (R): ORIGIN = 0x00000400, LENGTH = 0x00000020
    rom         (RX): ORIGIN = 0x00000420, LENGTH = 0x000FFBE0  /* Code + Const data */
    ram         (RW): ORIGIN = 0x1FFF0000, LENGTH = 0x00020000  /* SRAM - RW data */

    /* kernel space starts after RAM variables (Location of MQX Kernel data + MQX heap) */
    end_of_kd   (RW): ORIGIN = 0x2000FFF0, LENGTH = 0x00000000

    /* Boot stack reused by MQX Kernel data */
    bstack      (RW): ORIGIN = 0x2000FA00, LENGTH = 0x00000200  /* Boot stack */
    end_bstack  (RW): ORIGIN = 0x2000FC00, LENGTH = 0x00000000
}


SECTIONS
{
    __INTERNAL_SRAM_BASE  = 0x1FFF0000;
    __INTERNAL_SRAM_SIZE  = 0x00020000;
    /* This symbol is unused in SRAM target (runtime check ensure that  SRAM pool = Kernel pool), 
     it is defined here just to keep linker quiete */
    __SRAM_POOL = 0x2000FFF0;

    __INTERNAL_FLASH_BASE = 0x00000000;
    __INTERNAL_FLASH_SIZE = 0x00100000;

    __INTERNAL_FLEXNVM_BASE = 0;
    __INTERNAL_FLEXNVM_SIZE = 0;

    __UNCACHED_DATA_END      = 0x2000FFF0;

    __EXTERNAL_MRAM_BASE  = 0xA0000000;
    __EXTERNAL_MRAM_SIZE  = 0x00080000;
    __EXTERNAL_MRAM_ROM_BASE = 0xA0000000;
    __EXTERNAL_MRAM_ROM_SIZE = 0x00000000;
    __EXTERNAL_MRAM_RAM_BASE = 0xA0000000;
    __EXTERNAL_MRAM_RAM_SIZE = 0x00080000;

    __EXTERNAL_DDR2_RAM_BASE = 0x70000000;
    __EXTERNAL_DDR2_RAM_SIZE = 0x08000000;

    __EXTERNAL_LCD_BASE = 0xA0000000;
    __EXTERNAL_LCD_SIZE = 0x1FFFF;
    __EXTERNAL_LCD_DC_BASE = 0xA0010000;

    /* MQX link time configurations */
    __DEFAULT_PROCESSOR_NUMBER = 1;
    __DEFAULT_INTERRUPT_STACK_SIZE = 1024;
    __KERNEL_DATA_VERIFY_ENABLE = 0;    /* Test SDRAM read/write */

    /* Flashx configurations */
    __FLASHX_SECT_SIZE = 0x1000;

    .vectors :
    {
        __vector_table = .;
        __VECTOR_TABLE_ROM_START = __vector_table;
        KEEP(*(.vectors_rom))
        . = ALIGN (0x4); 
    } > vectorrom

    /* Flash protection bits */
    .cfmprotect :
    {
        KEEP(*(.cfmconfig))
        . = ALIGN (0x4);
    } > cfmprotrom
    
    .text :
    {
        *(KERNEL)
        *(.text*)
        *(.rodata*)

        /* unwind exception frame */
        KEEP(*(.eh_frame))
        KEEP(*(.eh_frame_hdr))

        /* constructors */
        . = ALIGN(0x8);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))

        /* destructors */
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))

        /* preinit_array */
        __preinit_array_start = .;
        KEEP (*(.preinit_array*))
        __preinit_array_end = .;

        /* init_array */
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        __init_array_end = .;

        /* fini_array */
        __fini_array_start = .;
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        __fini_array_end = .;

        /* gnu arm zero table */
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (_bss_start)
        LONG (_bss_end - _bss_start)
        __zero_table_end__ = .;

        /* gnu arm copy table */
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (LOADADDR(.data))
        LONG (_data_start)
        LONG (_data_end - _data_start)
        __copy_table_end__ = .;
    } > rom

    /* unwind exception frame */
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > rom
    .ARM :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > rom

    .data :
    {
        . = ALIGN(128);
        _data_start = .;
        __VECTOR_TABLE_RAM_START = .;
        KEEP(*(.vectors_ram))

        . = ALIGN(512);
        __BDT_BASE = .;
        *(.usb_bdt)
        __BDT_END = .;

        *(.data*)
        . = ALIGN(4);
        __UNCACHED_DATA_START = .;
        _data_end = .;
    } > ram AT> rom

    .rom_end_data :
    {
        _rom_data_end = .;
    } > rom

    .bss :
    {
        . = ALIGN(4);
        _bss_start = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;
    } > ram

   .kernel_data : 
    {
        . = ALIGN(0x10);
        __KERNEL_DATA_START = .;
    } > ram

    .end_of_kernel_data :
    {
        __KERNEL_DATA_END = .;
        __KERNEL_AREA_END = .;
    } > end_of_kd

    .boot_stack :
    {
        _stack_end = .;
    } > bstack

    .end_of_boot_stack :
    {
        __BOOT_STACK_ADDRESS = .;
    } > end_bstack

    /* flashx working area spans across the whole rest of Flash memory */
    _flashx_start = _rom_data_end;
    __FLASHX_START_ADDR = ((_flashx_start + __FLASHX_SECT_SIZE - 1) / __FLASHX_SECT_SIZE) * __FLASHX_SECT_SIZE;
    __FLASHX_END_ADDR = __INTERNAL_FLASH_BASE + __INTERNAL_FLASH_SIZE;
}


