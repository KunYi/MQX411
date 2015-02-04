ENTRY(__boot)
EXTERN(main)

MEMORY
{
    text        (RX): ORIGIN = 0x3f000000, LENGTH = 0x0003fff0
    data        (RW): ORIGIN = 0x3f050000, LENGTH = 0x0002fbf0
}

CODE_BASE_ADDR_START            = ORIGIN(text);
CODE_SIZE                       = LENGTH(text);
CODE_BASE_ADDR_END              = CODE_BASE_ADDR_START + CODE_SIZE;

DATA_BASE_ADDR_START            = ORIGIN(data);
DATA_SIZE                       = LENGTH(data);
DATA_BASE_ADDR_END              = DATA_BASE_ADDR_START + DATA_SIZE;

/* exported symbols */
__DEFAULT_PROCESSOR_NUMBER      = 1;

/* 64kB of  shared memory */
__SHARED_RAM_START              = 0x3f040000;
__SHARED_RAM_END                = 0x3f04fff0;
__SRAM_POOL_START               = DATA_BASE_ADDR_START;
__SRAM_POOL_END                 = DATA_BASE_ADDR_END;
__UNCACHED_DATA_START           = DATA_BASE_ADDR_START;
__UNCACHED_DATA_END             = DATA_BASE_ADDR_END;

/* mem_init writes a storeblock_struct at the end of kernel data, max size 32 bytes, so use 0x100 offset */
__BOOT_STACK_SIZE               = 0x100;
__BOOT_STACK_ADDRESS            = DATA_BASE_ADDR_END - __BOOT_STACK_SIZE;
__KERNEL_DATA_END               = DATA_BASE_ADDR_END;

__FIQ_STACK_SIZE = 0;
__ABT_STACK_SIZE = 0;
__UND_STACK_SIZE = 0;
__SYS_STACK_SIZE = 0;


SECTIONS
{
    /* place text */
    .text :
    {
        *("STARTUP")
        *(KERNEL)
        *(.text*)
        *(.rodata*)
        
        /* glue arm/thumb and thumb/arm code */
        *(.glue_7)
        *(.glue_7t)
        
        /* unwind exception frame */
        KEEP(*(.eh_frame))
        KEEP(*(.eh_frame_hdr))
        
        . = ALIGN(0x8);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))
        
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
        /* NOTE: bootloader requires rom to ram data copy */
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (LOADADDR(.data))
        LONG (_data_start)
        LONG (_data_end - _data_start)
        __copy_table_end__ = .;
        
    } > text
    
    /* unwind exception frame */
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > text
    .ARM :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > text
    
    /* place data, region address must be 8B aligned */
    .data :
    {
        _data_start = .;
        *(.data*)
        . = ALIGN(8);
        _data_end = .;
    } > data AT> text
    
    /* place bss, section size must be 8 aligned because of startup */
    .bss BLOCK(8) :
    {
      _bss_start = .;
      *(.bss*)
      *(COMMON)
      . = ALIGN(8);
      _bss_end = .;
    } > data
    
    /* stacks */
    FIQ_STACK BLOCK(8) : 
    {
        _FIQ_STACK_START = .;
        . = . + __FIQ_STACK_SIZE;
        _FIQ_STACK_END = .;
    } > data
    ABT_STACK BLOCK(8) : 
    {
        _ABT_STACK_START = .;
        . = . + __ABT_STACK_SIZE;
        _ABT_STACK_END = .;
    } > data
    UND_STACK BLOCK(8) : 
    {
        _UND_STACK_START = .;
        . = . + __UND_STACK_SIZE;
        _UND_STACK_END = .;
    } > data
    SYS_STACK BLOCK(8) : 
    {
        _SYS_STACK_START = .;
        . = . + __SYS_STACK_SIZE;
        _SYS_STACK_END = .;
    } > data
    
    .kernel_data BLOCK(0x10) : 
    {
        __KERNEL_DATA_START = ALIGN(0x10);
    } > data
}

